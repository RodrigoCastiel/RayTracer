//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
//  scene.h

#pragma once

#include "gl_header.h"

#include <string>
#include <vector>

#include <imageIO.h>

#include <glm/glm.hpp>

// ============================================================================================= //
//  
//
//
// ============================================================================================= //
// Triangles ---

struct VertexAttrib
{
  glm::dvec3 Kd;  // Material diffuse.
  glm::dvec3 Ks;  // Material specular.
  glm::dvec3 n;   // Vertex normal.
  double shininess;  // Alpha component.
};

struct Triangle  // Triangle intersection data.
{
  glm::dvec3 v[3];  // Three vertices;
};

struct TriangleAttrib
{
  VertexAttrib v[3];
};

// ============================================================================================= //
// Quadrics ---

struct Sphere  // Sphere intersection data.
{
  glm::dvec3 pos;
  double radius;
};

struct SphereAttrib  // Sphere material attributes.
{
  glm::dvec3 Kd;  // Material diffuse.
  glm::dvec3 Ks;  // Material specular.
  double shininess;  // Alpha component.
};

// ============================================================================================= //
// Light sources.

struct Light
{
  glm::dvec3 pos;
  glm::dvec3 col;
};

// ============================================================================================= //

class Scene
{
public:
  // Constructor.
  Scene() {  }

  // Loads scene from a file.
  bool Load(const std::string & filePath);

  void Log(std::ostream & stream);

  // RayCast - returns a RGB dvec3 containing the color of intersection.
  glm::dvec3 RayCast(const glm::dvec3 & ray, const glm::dvec3 & C) const;

private:
  // List of all triangles and list of all corresponding attributes.
  std::vector<Triangle> mTriangles;
  std::vector<TriangleAttrib> mTriangleAttribList;

  // List of all spheres and list of all corresponding attributes.
  std::vector<Sphere> mSpheres;
  std::vector<SphereAttrib> mSphereAttribList;

  // List of all light sources.
  std::vector<Light> mLights;
  glm::dvec3 mAmbLight;

};

