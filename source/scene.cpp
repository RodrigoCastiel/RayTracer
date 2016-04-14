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
    float t_triangle, t_sphere;
    glm::vec3 intersection, barycentric;
    glm::vec3 intersectionSphere, normalSphere;

    int nearestTriangleIndex = Scene::NearestTriangle(r, O, intersection, barycentric, t_triangle);
    int nearestSphereIndex = Scene::NearestSphere(r, O, intersectionSphere, normalSphere, t_sphere);

    glm::vec3 f_pos, n, Kd, Ks;
    float alpha, n_refr = 1.0f;

    if ( ((nearestTriangleIndex != -1) && (nearestSphereIndex == -1))                            // Triangle only or
      || ((nearestTriangleIndex != -1) && (nearestSphereIndex != -1) && t_triangle <= t_sphere)) // Triangle closer than sphere
    {
      const Triangle& triangle = mTriangles[nearestTriangleIndex];
      const TriangleAttrib& attrib = mTriangleAttribList[nearestTriangleIndex];

      f_pos = triangle.v[0]*barycentric[0]
            + triangle.v[1]*barycentric[1]
            + triangle.v[2]*barycentric[2];
      n  = glm::normalize(attrib.n * barycentric);
      Kd = attrib.Kd * barycentric;
      Ks = attrib.Ks * barycentric;
      alpha = glm::dot(attrib.shininess, barycentric);
      n_refr = attrib.n_refr;
    }
    else if ( ((nearestTriangleIndex == -1) && (nearestSphereIndex != -1))                           // Sphere only or
           || ((nearestTriangleIndex != -1) && (nearestSphereIndex != -1) && t_sphere < t_triangle)) // Sphere closer than triangle
    {
      const SphereAttrib& attrib = mSphereAttribList[nearestSphereIndex];

      f_pos = intersectionSphere;
      n = normalSphere;
      Kd = attrib.Kd;
      Ks = attrib.Ks;
      alpha = attrib.shininess;
      n_refr = attrib.n_refr;
    }
    else // No intersection.
    {
      return mBackgroundColor;
    }

    glm::vec3 abs_color = Scene::ComputePhongIllumination(f_pos, n, Kd, Ks, alpha);

    if (depth == 1)
    {
      return abs_color * (glm::vec3(1) - Ks);
    }
    else
    {
      /* Calculating Reflection and Refraction */
      // Reference: 
      // http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
      
      glm::vec3 refracted_color(0.0f);
      glm::vec3 reflected_color(0.0f);

      // Reflect if specular coeffiecient is non-zero.
      if (Ks[0] > kEpsilon && Ks[1] > kEpsilon && Ks[2] > kEpsilon) 
      {
          reflected_color = Scene::TraceRay(glm::normalize(glm::reflect(r, n)), f_pos, depth-1);
      }

      float n_1 = 1.00f;   // Air.
      float n_2 = n_refr;   //n_refr;  // Index of refraction of the sphere.
      // Reflectance and transmittance.
      float R = 1.0f, T = 0.0f;
  
      if (std::abs(n_2 - n_1) > kEpsilon)  // Different indices of refraction.
      {
        float cos_ti = - glm::dot(r, n);

        if (cos_ti < 0.0f)  // From out to in.
        {
          std::swap(n_1, n_2);
        }
        float ratio_n = n_1/n_2;
        float sin2_tt = ratio_n*ratio_n*(1.0f - cos_ti*cos_ti);
        float cos_tt = (float)sqrt(1.0f - sin2_tt);

        float R0 = pow((n_1 - n_2)/(n_1 + n_2), 2.0);

        // Reflectance.
        float one_minus_cos_ti = (1 - cos_ti);
        //float power = pow(one_minus_cos_ti, 3.0);
        R = R0 + (1.0f - R0)*one_minus_cos_ti;

        // Transmittance.
        T = 1.0f - R;

        glm::vec3 refr_r = glm::normalize(ratio_n*r + (ratio_n*cos_ti - cos_tt) * n);
        refracted_color = Scene::TraceRay(refr_r, f_pos, depth-1);
      }
      

      return  Ks * (R*reflected_color + T*refracted_color) + (glm::vec3(1.0f) - Ks) * abs_color;
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

    glm::vec3 intersection, temp;
    float t_triangle, t_sphere;

    int nearestTriangleIndex = Scene::NearestTriangle(l, f_pos, intersection, temp, t_triangle);
    int nearestSphereIndex   = Scene::NearestSphere(l, f_pos, intersection, temp, t_sphere);

    if ( ((nearestTriangleIndex == -1) || (t_triangle > glm::length(light.pos - f_pos) + kEpsilon))
      && ((nearestSphereIndex   == -1) || (t_sphere   > glm::length(light.pos - f_pos) + kEpsilon)))
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

int Scene::NearestSphere(const glm::vec3 & r, const glm::vec3 & O,
                         glm::vec3 & intersection, glm::vec3 & n, float & t) const
{
  float nearest_t = std::numeric_limits<float>::max();
  int nearestSphereIndex = -1;

  glm::vec3 I;  // Intersection point.
  glm::vec3 normal;
  float t_test;

  // Force brute - search over all triangles.
  for (int i = 0; i < mSpheres.size(); i++)
  {
    // Test if sphere i has an intersection with input ray.
    if (IntersectRay(mSpheres[i], r, O, I, normal, t_test))
    {
      if (t_test < nearest_t)  // Found a better triangle!
      {
        nearestSphereIndex = i;
        nearest_t = t_test;

        intersection = I;
        n = normal;
        
        t = t_test;
      }
    }
  }

  return nearestSphereIndex;
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

  if (!file.is_open())  // Failure - not good.
  {
    std::cerr << "ERROR Scene file at " << filePath << " couldn't be opened." << std::endl;
    return false;
  }

  float n_refr = 1.00f;

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
        attrib.n_refr = n_refr;
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
        attrib.n_refr = n_refr;
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
    else if (strcasecmp(type.c_str(), "obj") == 0)  // 3D Obj.
    {
      std::string path;
      if(file >> path)
      {
        Scene::LoadObj(path);
      }
    }
    else if (strcasecmp(type.c_str(), "n_refr") == 0)  // Specify new index of refraction.
    {
      k--;
      if(!(file >> n_refr))
      {
        std::cerr << "ERROR Scene file parsing error - expected n_refr value.\n";
      }
    }
    else if (strcasecmp(type.c_str(), "camera") == 0)
    {
      glm::vec3 pos, rot;

      k--;
      if (ParseAttribute(file, "pos:", pos) &&
          ParseAttribute(file, "rot:", rot)
          )  // Success - new camera configuration.
      {
        mCamera->SetPosition(pos);
        mCamera->SetRotation(rot);
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

// ============================================================================================= //

bool Scene::LoadObj(const std::string & objFilepath)
{
  FILE* inFile = fopen(objFilepath.c_str(), "r");

  // Successfully opened the file.
  if (inFile != nullptr)
  {
    char line[256];  // Line buffer.
    char token[32];  // Token buffer.
    char name[100];  // Group name buffer.
    const int kNumIndices = 100;
    std::vector<GLfloat> positions;  // List of all positions (x, y, z).
    std::vector<GLfloat> texCoords;  // List of all texture (u, v).
    std::vector<GLfloat> normals;    // List of all normals (nx, ny, nz).
    bool buildingGroup = false;

    // Read until get EOF.
    while (fgets(line, sizeof(line), inFile) != nullptr)
    {
      // Exclude repeated spaces.
      int i = 0;
      while (line[i] != '\0')
      {
        if (line[i] == ' ' && line[i+1] == ' ')
        {
          for (int j = i; line[j] != '\0'; j++)
          {
            line[j] = line[j+1];
          }
        }       
        i++; 
      }

      // Read first token.
      sscanf(line, "%s", token);

      if (strcmp(token, "g") == 0)  // New group.
      {

      }
      else if (strcmp(token, "v") == 0)   // New vertex.
      {
        float x, y, z;
        sscanf(line, "%s %f %f %f", token, &x, &y, &z);
        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
      }
      else if (strcmp(token, "vt") == 0)  // New texture coordinate.
      {
        float u, v;
        sscanf(line, "%s %f %f", token, &u, &v);
        // texCoords.push_back(u);
        // texCoords.push_back(v);
      }
      else if (strcmp(token, "vn") == 0)  // New normal.
      {
        float nx, ny, nz;
        sscanf(line, "%s %f %f %f", token, &nx, &ny, &nz);
        normals.push_back(nx);
        normals.push_back(ny);
        normals.push_back(nz);
      }
      else if (strcmp(token, "f") == 0)  // New face (triangle).
      {
        int a_v, a_vt, a_vn;
        int b_v, b_vt, b_vn;
        int c_v, c_vt, c_vn;

        if (sscanf(line, "%s %d//%d %d//%d %d//%d", token, &a_v, &a_vn, 
                                                           &b_v, &b_vn, 
                                                           &c_v, &c_vn) == 7)
        {
            // No texture coordinates
        }
        else if (sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d", token, &a_v, &a_vt, &a_vn, 
                                                                      &b_v, &b_vt, &b_vn, 
                                                                      &c_v, &c_vt, &c_vn) == 9)
        {
          // // Has texture coordinates.
          // a_vt--;
          // b_vt--;
          // c_vt--;
        }

        a_v--; a_vn--;  
        b_v--; b_vn--;  
        c_v--; c_vn--;  

        Triangle triangle;
        TriangleAttrib attrib;

        attrib.Ks = glm::mat3(glm::vec3(.7), glm::vec3(.7), glm::vec3(.7));
        attrib.Kd = glm::mat3(glm::vec3(1.0, 0.0, 0.0), 
                              glm::vec3(1.0, 0.0, 0.0), 
                              glm::vec3(1.0, 0.0, 0.0));
        attrib.shininess = glm::vec3(200);
        attrib.n_refr = 1.0f;

        // Vertex A.
        triangle.v[0][0] = positions[3*a_v + 0];
        triangle.v[0][1] = positions[3*a_v + 1];
        triangle.v[0][2] = positions[3*a_v + 2];
        attrib.n[0][0] = normals[3*a_vn + 0];
        attrib.n[0][1] = normals[3*a_vn + 1];
        attrib.n[0][2] = normals[3*a_vn + 2];

        // Vertex B.
        triangle.v[1][0] = positions[3*b_v + 0];
        triangle.v[1][1] = positions[3*b_v + 1];
        triangle.v[1][2] = positions[3*b_v + 2];
        attrib.n[1][0] = normals[3*b_vn + 0];
        attrib.n[1][1] = normals[3*b_vn + 1];
        attrib.n[1][2] = normals[3*b_vn + 2];

        // Vertex C.
        triangle.v[2][0] = positions[3*c_v + 0];
        triangle.v[2][1] = positions[3*c_v + 1];
        triangle.v[2][2] = positions[3*c_v + 2];
        attrib.n[2][0] = normals[3*c_vn + 0];
        attrib.n[2][1] = normals[3*c_vn + 1];
        attrib.n[2][2] = normals[3*c_vn + 2];

        mTriangles.push_back(triangle);
        mTriangleAttribList.push_back(attrib);
      }
      else
      {
        // Ignore anything else
      }
    }

    fclose(inFile);
    return true;
  }
  else  // Failed.
  {
    printf("ERROR Couldn't load file at %s\n", objFilepath.c_str());
    return false;
  }
}
