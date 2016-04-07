//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// camera.h

#pragma once

#include <glm/glm.hpp>
#include <functional>

const unsigned kDefaultWidth  = 6400;
const unsigned kDefaultHeight = 4800;

class Camera
{
public:
  Camera() { } 

  // Computes the line which goes from camera origin to mouse coords at projection plane.
  glm::vec3 CastRay(float x_v, float y_v, float w, float h) const;
  glm::vec3 GetCenterCoordinates() const { return mPos; }
  
  // Setters and getters.
  void SetPosition(float x, float y, float z);
  void SetRotation(float rx, float ry, float rz);
  inline void SetFOVy(float fovy)     { mFovy = fovy;     }
  inline void SetAspect(float aspect) { mAspect = aspect; }

  inline float GetFOVy()   { return mFovy;   }
  inline float GetAspect() { return mAspect; }

private:
  // View parameters.
  glm::vec3 mPos   { 0.0, 20.0, 17};  // Center coordinates.
  glm::vec3 mRot   { -0.49, 0.0, 0.0};  // Rotation angles.
  glm::vec3 mScale { 1.0, 1.0, 1.0};  // Scales.

  // Projection parameters
  float mFovy   {M_PI/3.0};
  float mAspect {static_cast<float>(kDefaultWidth)/kDefaultHeight};
  float mFarZ   {100.0};
  float mNearZ  { 0.01}; 

  // Animation function (either lambda or functor).

};

inline 
void Camera::SetPosition(float x, float y, float z)
{
  mPos[0] = x;
  mPos[1] = y;
  mPos[2] = z;
}
inline 
void Camera::SetRotation(float rx, float ry, float rz)
{
  mRot[0] = rx;
  mRot[1] = ry;
  mRot[2] = rz;
}