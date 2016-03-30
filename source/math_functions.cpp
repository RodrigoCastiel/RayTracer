//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// math_functions.cpp

#include "math_functions.h"
#include "scene.h"

#include <cmath>

// bool IntersectRay(const Triangle & triangle, const glm::vec3 & ray, const glm::vec3 & O,
//                   glm::vec3 & intersection, glm::vec3 & baryCoord, float & t)
// {
//   // Normal computation.
//   glm::vec3 n = glm::normalize(glm::cross(triangle.v[1] - triangle.v[0],    // First vector.
//                                            triangle.v[2] - triangle.v[0]));  // Second vector.
//   float dot_ray_normal = glm::dot(n, ray);

//   if (std::abs(dot_ray_normal) > 1e-4)  // There is an intersection.
//   {
//     float d = -glm::dot(triangle.v[0], n);

//     t = - (glm::dot(O, n) + d) / dot_ray_normal;  // t.
//     intersection = O + t*ray;

//     //

//     return true;
//   }
//   else // No intersection, line is parellel to the plane.
//   {
//     return false;
//   }
// }

bool IntersectRay(const Triangle & triangle, const glm::vec3 & ray, const glm::vec3 & O,
                  glm::vec3 & intersection, glm::vec3 & baryCoord, float & t)
{
  glm::mat3 M(-ray, triangle.v[1] - triangle.v[0], triangle.v[2] - triangle.v[0]);
  baryCoord = glm::inverse(M) * (O - triangle.v[0]);
  t = baryCoord[0];
  intersection = O + t*ray;
  baryCoord[0] = 1 - baryCoord[1] - baryCoord[2];

  return (baryCoord[0] + baryCoord[1] + baryCoord[2] - 1 < 1e-4);
}


bool IntersectRay(const Sphere & sphere, const glm::vec3 & ray, const glm::vec3 & C,
                  glm::vec3 & intersection, float & t)
{
  return false;
}

void BarycentricCoord(const Triangle & triangle, const glm::vec3 & P, glm::vec3 & barycentric)
{
  barycentric = glm::inverse(glm::mat3(triangle.v[0], triangle.v[1], triangle.v[2])) * P;
}