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
// DATA STORAGE ARCHITECTURE
//
//  Instead of the usual storage architecture -- list of all struct Triangle containing all tri-
//  angle attributes -- class Scene implements a Data-Oriented Designed (against OO design).
//
//  It makes the computation of intersection faster, because the data used for this is compacted
//  in a single vector, which is cache-friendly. Then, by walking through all triangles to com-
//  pute an intersection the requested data from CPU will probably be in cache, therefore avoid-.
//  cache misses. This is applied for all data in Scene.
//
// ============================================================================================= //
// Triangles ---

// struct VertexAttrib
// {
//   glm::vec3 Kd;  // Material diffuse.
//   glm::vec3 Ks;  // Material specular.
//   glm::vec3 n;   // Vertex normal.
//   float shininess;  // Alpha component.
// };

struct Triangle  // Triangle intersection data.
{
  glm::vec3 v[3];  // Three vertices;
};

// struct TriangleAttrib
// {
//   VertexAttrib v[3];
// };

struct TriangleAttrib
{
  // Data is stored as a matrix so it is faster to interpolate using barycentric coordinates.
  glm::mat3 Kd;  // Each column corresponds to a vertex diffuse  material.
  glm::mat3 Ks;  // Each column corresponds to a vertex specular material.
  glm::mat3 n;   // Each column corresponds to a vertex normal.
  glm::vec3 shininess;  // Each element corresponds to a vertex shininess.
};

// ============================================================================================= //
// Quadrics ---

struct Sphere  // Sphere intersection data.
{
  glm::vec3 pos;
  float radius;
};

struct SphereAttrib  // Sphere material attributes.
{
  glm::vec3 Kd;  // Material diffuse.
  glm::vec3 Ks;  // Material specular.
  float shininess;  // Alpha component.
  float n_refr;  // Index of refraction.
};

// ============================================================================================= //
// Light sources ---

struct Light
{
  glm::vec3 pos;
  glm::vec3 col;
};

// ============================================================================================= //

class Scene
{
public:
  // Constructor.
  Scene() {  }

  // Loads scene from a file.
  bool Load(const std::string & filePath);

  // TraceRay - returns a RGB vec3 containing the color of intersection.
  glm::vec3 TraceRay(const glm::vec3 & r, const glm::vec3 & O, int depth = 20) const;

  // Returns the index of the nearest triangle reachable from input ray.
  // If there is a valid triangle, it also outputs barycentric coordinates,
  // depth and intersectino point.
  // If there is no triangle which intersects with input ray, return value
  // will be 0.
  int NearestTriangle(const glm::vec3 & r, const glm::vec3 & O,
                      glm::vec3 & intersection, glm::vec3 & baryCoord, float & t) const;

  int NearestSphere(const glm::vec3 & r, const glm::vec3 & O,
                    glm::vec3 & intersection, glm::vec3 & n, float & t) const;

  //
  glm::vec3 ComputePhongIllumination(const glm::vec3 & f_pos, const glm::vec3 & n, 
    const glm::vec3 & Kd, const glm::vec3 & Ks, float alpha) const;

  // Prints a log containing general Scene data.
  void Log(std::ostream & stream);

private:
  // List of all triangles 
  // and list of all corresponding attributes.
  std::vector<Triangle> mTriangles;
  std::vector<TriangleAttrib> mTriangleAttribList;

  // List of all spheres and list of all corresponding attributes.
  std::vector<Sphere> mSpheres;
  std::vector<SphereAttrib> mSphereAttribList;

  // List of all light sources.
  std::vector<Light> mLights;
  glm::vec3 mAmbLight;
  glm::vec3 mBackgroundColor { 0.0f, 0.0f, 0.0f };
};







