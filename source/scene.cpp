//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
//  scene.cpp

#include "scene.h"

#include <cmath>
#include <fstream>
#include <cstring>
#include <iostream>

#include <limits>

#include "math_functions.h"

// ============================================================================================= //
// RAY TRACING METHODS

glm::vec3 Scene::TraceRay(const glm::vec3 & r, const glm::vec3 & O, int depth) const
{  
  if (depth > 0)
  {
    float t;
    glm::vec3 intersection;
    glm::vec3 barycentric;

    int nearestTriangleIndex = Scene::NearestTriangle(r, O, intersection, barycentric, t);

    if (nearestTriangleIndex != -1)
    {
      const Triangle& triangle = mTriangles[nearestTriangleIndex];
      const TriangleAttrib& attrib = mTriangleAttribList[nearestTriangleIndex];

      glm::vec3 f_pos = triangle.v[0]*barycentric[0]
                      + triangle.v[1]*barycentric[1]
                      + triangle.v[2]*barycentric[2];
      glm::vec3 n  = glm::normalize(attrib.n * barycentric);
      glm::vec3 Kd = attrib.Kd * barycentric;
      glm::vec3 Ks = attrib.Ks * barycentric;
      float alpha = glm::dot(attrib.shininess, barycentric);

      glm::vec3 abs_color = Scene::ComputePhongIllumination(f_pos, n, Kd, Ks, alpha);

      if (depth == 1)
      {
        return abs_color * (glm::vec3(1) - Ks);
      }
      else
      {
        return  Ks * Scene::TraceRay(glm::normalize(glm::reflect(r, n)), f_pos, depth-1)
              + (glm::vec3(1) - Ks) * abs_color;
      }
    }
    else
    {
      return mBackgroundColor;
    }
  }
  else
  {
    return mBackgroundColor;
  }
}

glm::vec3 Scene::ComputePhongIllumination(const glm::vec3 & f_pos, const glm::vec3 & n, 
  const glm::vec3 & Kd, const glm::vec3 & Ks, float alpha) const
{
  glm::vec3 Id(0.0f);
  glm::vec3 Is(0.0f);

  glm::vec3 r = glm::normalize(glm::reflect(f_pos, n));

  for (auto& light : mLights)
  { 
    glm::vec3 l = glm::normalize(light.pos - f_pos);  // Vector from frag to light.

    glm::vec3 intersection, barycentric;
    float t;

    int nearestTriangleIndex = Scene::NearestTriangle(l, f_pos, intersection, barycentric, t);

    if ((nearestTriangleIndex == -1) || (t > glm::length(light.pos - f_pos) + kEpsilon))
    {
      float dot_l_n = glm::dot(l, n);
      float dot_l_r = glm::dot(l, r);

      // Basic lighting - Phong model.
      Id += light.col * Kd * std::max(dot_l_n, 0.0f);
      Is += light.col * Ks * std::pow(std::max(dot_l_r, 0.0f), alpha);
    }
  }

  return (mAmbLight + (Id + Is));
}

int Scene::NearestTriangle(const glm::vec3 & r, const glm::vec3 & O,
                           glm::vec3 & intersection, glm::vec3 & baryCoord, float & t) const
{
  float nearest_t = std::numeric_limits<float>::max();
  int nearestTriangleIndex = -1;

  glm::vec3 I;  // Intersection point.
  glm::vec3 B;  // Barycentric coordinates.
  float t_test;

  // Force brute - search over all triangles.
  for (int i = 0; i < mTriangles.size(); i++)
  {
    // Test if triangle i has an intersection with input ray.
    if (IntersectRay(mTriangles[i], r, O, I, B, t_test))
    {
      if (t_test < nearest_t)  // Found a better triangle!
      {
        nearestTriangleIndex = i;
        nearest_t = t_test;

        intersection = I;
        baryCoord = B;
        t = t_test;
      }
    }
  }

  return nearestTriangleIndex;
}

// ============================================================================================= //
// FILE LOADING METHODS

// Parse 3-tuple attributes, such as position, color and so on.
bool ParseAttribute(std::ifstream & file, const std::string & expectedName, glm::vec3 & v)
{
  std::string name;
  file >> name;

  if (file.fail() || strcasecmp(name.c_str(), expectedName.c_str()) != 0)
  {
    std::cerr << "ERROR Scene file parsing error - expected attribute " << expectedName << ".\n";
    return false;
  }

  file >> v[0] >> v[1] >> v[2];

  if (file.fail())
  {
    std::cerr << "ERROR Scene file parsing error - incorrect attribute " << expectedName << ".\n";
    return false;
  }

  return true;
}

// Parse a single attribute, such as shininess.
bool ParseAttribute(std::ifstream & file, const std::string & expectedName, float & a)
{
  std::string name;
  file >> name;

  if (file.fail() || strcasecmp(name.c_str(), expectedName.c_str()) != 0)
  {
    std::cerr << "ERROR Scene file parsing error - expected attribute " << expectedName << ".\n";
    return false;
  }

  file >> a;

  if (file.fail())
  {
    std::cerr << "ERROR Scene file parsing error - incorrect attribute " << expectedName << ".\n";
    return false;
  }

  return true;
}

void Scene::Log(std::ostream & stream)
{
  stream << "SCENE LOG" << std::endl;
  stream << "Number of light sources ... " << mLights.size()    << ";" << std::endl;
  stream << "Number of triangles ....... " << mTriangles.size() << ";" << std::endl;
  stream << "Number of spheres ......... " << mSpheres.size()   << ";" << std::endl;
  stream << "TOTAL ..................... " << mTriangles.size() + mSpheres.size()  + mLights.size() << ";" << std::endl;
}

bool Scene::Load(const std::string & filePath)
{
  // Open input file.
  std::ifstream file(filePath, std::ifstream::in);

  if (!file.good())  // Failure - not good.
  {
    std::cerr << "ERROR Scene file at " << filePath << " couldn't be opened." << std::endl;
    return false;
  }

  // Read number of objects.
  int numObjects = 0;
  file >> numObjects;

  // Validate number of objects.
  if (numObjects < 0)
  {
    std::cerr << "ERROR Scene file parsing error - number of objects must be non-negative." << std::endl;
    return false;
  }

  // Read ambient light.
  if (!ParseAttribute(file, "amb:", mAmbLight))
  {
    return false;
  }

  // Read each object data.
  for (int k = 0; k < numObjects; k++)
  {
    // Read type of object.
    std::string type;
    file >> type;

    if (file.fail())
    {
      std::cerr << "ERROR Scene file parsing error - expected an object (number " << (k+1) << ")." << std::endl;
      return false;
    }

    if (strcasecmp(type.c_str(), "triangle") == 0)  // Triangle.
    {
      Triangle triangle;
      TriangleAttrib attrib;

      for (int i = 0; i < 3; i++) // Read all vertices.
      {
        glm::vec3 Kd, Ks, n;
        float shininess;

        if (!(ParseAttribute(file, "pos:", triangle.v[i])  &&
              ParseAttribute(file, "nor:", n)  &&
              ParseAttribute(file, "dif:", Kd) &&
              ParseAttribute(file, "spe:", Ks) &&
              ParseAttribute(file, "shi:", shininess)
            ))   // Failure, no new triangle.
        { 
          std::cout << "\tRefer to triangle number " << mTriangles.size() << ".\n";
          return false;
        }

        attrib.Kd[i] = Kd;
        attrib.Ks[i] = Ks;
        attrib.n[i]  = n;
        attrib.shininess[i] = shininess;
      }

      mTriangles.push_back(triangle);
      mTriangleAttribList.push_back(attrib);
    }
    else if (strcasecmp(type.c_str(), "sphere") == 0)  // Sphere.
    {
      Sphere sphere;
      SphereAttrib attrib;

      if (ParseAttribute(file, "pos:", sphere.pos)    &&
          ParseAttribute(file, "rad:", sphere.radius) &&
          ParseAttribute(file, "dif:", attrib.Kd)     && 
          ParseAttribute(file, "spe:", attrib.Ks)     &&
          ParseAttribute(file, "shi:", attrib.shininess)
        )   // Success - new sphere.
      { 
        mSpheres.push_back(sphere);
        mSphereAttribList.push_back(attrib);
      }
      else  // Failure, no new sphere.
      {
        std::cout << "\tRefer to sphere number " << mSpheres.size() << ".\n";
        return false;
      }
    }
    else if (strcasecmp(type.c_str(), "light") == 0)  // Light Source.
    {
      Light light;

      if (ParseAttribute(file, "pos:", light.pos) &&
          ParseAttribute(file, "col:", light.col)
          )  // Success - new light source.
      {
        mLights.push_back(light);
      }
      else
      {
        std::cout << "\tRefer to light number " << mLights.size() << ".\n";
        return false;
      }
    }
    // INSERT HERE NEW TYPES OF OBJECT.
    
    else  // Invalid object.
    {
      std::cerr << "ERROR Scene file parsing error - invalid object type: '" << type << "'." << std::endl;
      return false;
    }
  }

  return true;
}
