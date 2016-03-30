//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// math_functions.h

#pragma once

#include <glm/glm.hpp>

struct Triangle;
struct Sphere;

// ============================================================================================= //
// Intersection functions.

bool IntersectRay(const Triangle & triangle, const glm::vec3 & ray, const glm::vec3 & O,
                  glm::vec3 & intersection, glm::vec3 & baryCoord, float & t);

bool IntersectRay(const Sphere & sphere, const glm::vec3 & ray, const glm::vec3 & O,
                  glm::vec3 & intersection, float & t);

// ============================================================================================= //
// Triangle Barycentric coordinates.

void BarycentricCoord(const Triangle & triangle, const glm::vec3 & P, glm::vec3 & barycentric);

inline float TriangleArea(const glm::vec3 & A, const glm::vec3 & B, const glm::vec3 & C)
{
  return 0.5;
}