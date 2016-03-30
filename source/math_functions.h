//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// math_functions.h

#pragma once

#include <glm/glm.hpp>

const double kEpsilon = 1e-5;

struct Triangle;
struct Sphere;

// ============================================================================================= //
// Intersection functions.

bool IntersectRay(const Triangle & triangle, const glm::vec3 & ray, const glm::vec3 & O,
                  glm::vec3 & intersection, glm::vec3 & baryCoord, float & t);

bool IntersectRay(const Sphere & sphere, const glm::vec3 & ray, const glm::vec3 & O,
                  glm::vec3 & intersection, float & t);

// ============================================================================================= //
