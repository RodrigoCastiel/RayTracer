//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// ray_tracer.cpp

#include "ray_tracer.h"
#include "camera.h"
#include "scene.h"

// ============================================================================================= //

void RayTracer::RenderFrame(const Scene & scene, const Camera & camera)
{
  for(unsigned int y = 0; y < mBuffer.height; y++)
  {
    for(unsigned int x = 0; x < mBuffer.width; x++)
    {
      unsigned char* pixel = mBuffer.GetPixel(x, y);
      glm::vec3 O = camera.GetCenterCoordinates();
      glm::vec3 r = camera.CastRay(x, y, mBuffer.width, mBuffer.height);
      glm::vec3 rgb = scene.TraceRay(r, O);

      pixel[0] = static_cast<unsigned char>(rgb[0] * 255);
      pixel[1] = static_cast<unsigned char>(rgb[1] * 255);
      pixel[2] = static_cast<unsigned char>(rgb[2] * 255);
    }
  }

  glEnd();
}

void RayTracer::SaveFrame(const std::string & filePath)
{
  mBuffer.Save(filePath);
}

void RayTracer::DrawFrame()
{
  mBuffer.Draw();
}






// ============================================================================================= //
