//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
//  scene.cpp

#include "scene.h"

#include <fstream>
#include <iostream>
#include <cstring>

// Parse 3-tuple attributes, such as position, color and so on.
bool ParseAttribute(std::ifstream & file, const std::string & expectedName, glm::dvec3 & v)
{
  std::string name;
  file >> name;

  if (file.fail() || strcasecmp(name.c_str(), expectedName.c_str()) != 0)
  {
    std::cerr << "ERROR Scene file parsing error - expected attribute " << expectedName << ".\n";
    return false;
  }

  file >> v[0] >> v[1] >> v[0];

  if (file.fail())
  {
    std::cerr << "ERROR Scene file parsing error - incorrect attribute " << expectedName << ".\n";
    return false;
  }

  return true;
}

// Parse a single attribute, such as shininess.
bool ParseAttribute(std::ifstream & file, const std::string & expectedName, double & a)
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
        if (!(ParseAttribute(file, "pos:", triangle.v[i])  &&
              ParseAttribute(file, "nor:", attrib.v[i].n)  &&
              ParseAttribute(file, "dif:", attrib.v[i].Kd) &&
              ParseAttribute(file, "spe:", attrib.v[i].Ks) &&
              ParseAttribute(file, "shi:", attrib.v[i].shininess)
            ))   // Failure, no new triangle.
        { 
          std::cout << "\tRefer to triangle number " << mTriangles.size() << ".\n";
          return false;
        }
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
    // INSERT HERE NEW TYPE OF OBJECT.
    
    else  // Invalid object.
    {
      std::cerr << "ERROR Scene file parsing error - invalid object type: '" << type << "'." << std::endl;
      return false;
    }
  }

  Scene::Log(std::cout);

  return true;
}
