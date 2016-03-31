//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// math_functions.cpp

#include "math_functions.h"
#include "scene.h"

#include <cmath>

bool IntersectRay(const Triangle & triangle, const glm::vec3 & ray, const glm::vec3 & O,
                  glm::vec3 & intersection, glm::vec3 & baryCoord, float & t)
{
  // The algorithm used for intersection is based on computing a linear combination
  // of the vectors outgoing from the first triangle vertex and the ray vertex, which
  // outputs directly the parameter t of intersection and two of the barycentric 
  // coordinates.
  //   Triangle vertices: A, B, C. Line origin: O. Line ray: r.
  //   We want to find t, u and v such that:
  //   (O + t*r) = u*(B - A) + v*(C - A);
  //
  //   In order to solve this linear system, we can compute:
  //
  //   [t u v]' = [ -ray'; (B-A)'; (C-A)'] \ (O - A)
  //   (MATLAB notation).
  // Computing this inverse is expensive. Therefore, some well-known algorithms
  // show how to precompute couple of common values to improve performance. 

  glm::mat3 M(-ray, triangle.v[1] - triangle.v[0], triangle.v[2] - triangle.v[0]);
  baryCoord = glm::inverse(M) * (O - triangle.v[0]);
  t = baryCoord[0];
  intersection = O + t*ray;
  baryCoord[0] = 1 - baryCoord[1] - baryCoord[2];

  // Check if u + v <= 1 and also if t > 0.
  return (baryCoord[1] > 0) && (baryCoord[2] > 0) 
      && (baryCoord[1] + baryCoord[2] <= 1.0f) && (t > kEpsilon);
}


bool IntersectRay(const Sphere & sphere, const glm::vec3 & ray, const glm::vec3 & C,
                  glm::vec3 & intersection, float & t)
{
  return false;
}
