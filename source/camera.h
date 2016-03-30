//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// camera.h

#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
  Camera() { } 

  // Computes the line which goes from camera origin to mouse coords at projection plane.
  glm::vec3 CastRay(float x_v, float y_v, float w, float h);
  glm::vec3 GetCenterCoordinates() { return mPos; }
  
  // Setters and getters.
  void SetPosition(float x, float y, float z);
  void SetRotation(float rx, float ry, float rz);
  inline void SetFOVy(float fovy)     { mFovy = fovy;     }
  inline void SetAspect(float aspect) { mAspect = aspect; }

  inline float GetFOVy()   { return mFovy;   }
  inline float GetAspect() { return mAspect; }

private:
  // View parameters.
  glm::vec3 mPos   { 0.0, 0.0, 0.0};  // Center coordinates.
  glm::vec3 mRot   { 0.0, 0.0, 0.0};  // Rotation angles.
  glm::vec3 mScale { 1.0, 1.0, 1.0};  // Scales.

  // Projection parameters
  float mFovy   {M_PI/3.0};
  float mAspect {4096.0/2160.0};
  float mFarZ   {500.0};
  float mNearZ  { 0.1}; 
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