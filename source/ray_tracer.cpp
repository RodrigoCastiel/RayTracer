//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// ray_tracer.cpp

#include "ray_tracer.h"

#include "camera.h"
#include "scene.h"

#include <fstream>
#include <iostream>

// ============================================================================================= //

void RayTracer::RenderFrame(const Scene & scene, const std::string & outputFilePath)
{
    RayTracer::ParallelRender(scene);

    if (mSettings.mAntiAliasing)
    {
       RayTracer::AdaptiveAntiAliasing(scene);
    }

    if (mSettings.mSaveFile)
    {
      RayTracer::SaveFrame(outputFilePath);
    }
}

void RayTracer::ParallelRender(const Scene & scene, int numThreads)
{
  std::vector<std::thread> threads(numThreads);
  Camera& camera = *(scene.GetCamera());
  auto& buffer = mBuffer;

  mProgressMessage = "Ray Tracing First Stage - Initial computation";

  RayTracer::ClearProgressBar();
  auto Renderer = [this, &buffer, &scene, &camera, numThreads] (int id)
  {
    for(unsigned int y = id; y < buffer.height; y += numThreads)
    {
      for(unsigned int x = 0; x < buffer.width; x++)
      {
        unsigned char* pixel = buffer.GetPixel(x, y);
        glm::vec3 O = camera.GetCenterCoordinates();
        glm::vec3 r = camera.CastRay(x, y, buffer.width, buffer.height);
        glm::vec3 rgb = scene.TraceRay(r, O, mSettings.mRecursionDepth);     

        pixel[0] = static_cast<unsigned char>(std::min(rgb[0], 1.0f) * 255);
        pixel[1] = static_cast<unsigned char>(std::min(rgb[1], 1.0f) * 255);
        pixel[2] = static_cast<unsigned char>(std::min(rgb[2], 1.0f) * 255);
      }

      RayTracer::IncrementProgressBar();
      RayTracer::PrintProgressBar();
    }
  };

  std::chrono::high_resolution_clock::time_point start;
  start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < numThreads; i++)
  {
    threads[i] = std::thread(Renderer, i);
  }

  for (int i = 0; i < numThreads; i++)
  {
    threads[i].join();
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  double delta_t =  duration / 1000000000.0;

  std::cout << "Frame rendered. Elapsed time: " << delta_t << " seconds.\n";
}

void RayTracer::AdaptiveAntiAliasing(const Scene & scene, int numThreads,
  int numSuperSample)
{
  std::vector<std::thread> threads(numThreads);
  Camera& camera = *(scene.GetCamera());
  auto& buffer = mBuffer;

  mProgressMessage = "Ray Tracing Anti-aliasing - Adaptive Supersampling";

  RayTracer::ClearProgressBar();
  auto Renderer = [this, &buffer, &scene, &camera, numThreads, numSuperSample] (int id)
  {
    for(unsigned int y = id+1; y < buffer.height-1; y += numThreads)
    {
      for(unsigned int x = 1; x < buffer.width-1; x++)
      {
        unsigned char* pixel = buffer.GetPixel(x, y);
        unsigned char* top = buffer.GetPixel(x, y-1);
        unsigned char* bot = buffer.GetPixel(x, y+1);
        unsigned char* lft = buffer.GetPixel(x-1, y);
        unsigned char* rgt = buffer.GetPixel(x+1, y);

        float dx = (((rgt[0] + rgt[1] + rgt[2])/3.0f) - ((lft[0] + lft[1] + lft[2])/3.0f))/255.0f;
        float dy = (((bot[0] + bot[1] + bot[2])/3.0f) - ((top[0] + top[1] + top[2])/3.0f))/255.0f;
        float gradient = sqrt(dx*dx + dy*dy);

        if (gradient > kGradientThreshold)
        {
          glm::vec3 O = camera.GetCenterCoordinates();
          glm::vec3 rgb(0.0f);

          for (int i = 0; i < numSuperSample; i++)
          {
            float theta = 2.0 * M_PI * static_cast<float>(i)/(numSuperSample);
            glm::vec3 r = camera.CastRay(x + 0.5*cos(theta), y + 0.5*sin(theta), buffer.width, buffer.height);
            rgb += scene.TraceRay(r, O, mSettings.mRecursionDepth);
          }
          rgb *= 1.0/(numSuperSample);

          unsigned char r = static_cast<unsigned char>(std::min(rgb[0], 1.0f) * 255);
          unsigned char g = static_cast<unsigned char>(std::min(rgb[1], 1.0f) * 255);
          unsigned char b = static_cast<unsigned char>(std::min(rgb[2], 1.0f) * 255);

          pixel[0] = r;
          pixel[1] = g;
          pixel[2] = b;
        }
      }

      RayTracer::IncrementProgressBar();
      RayTracer::PrintProgressBar();
    }
  };

  std::chrono::high_resolution_clock::time_point start;
  start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < numThreads; i++)
  {
    threads[i] = std::thread(Renderer, i);
  }

  for (int i = 0; i < numThreads; i++)
  {
    threads[i].join();
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  double delta_t =  duration / 1000000000.0;

  std::cout << "Adaptive Anti-aliasing finished. Elapsed time: " << delta_t << " seconds.\n";
}

void RayTracer::IncrementProgressBar()
{
    std::lock_guard<std::mutex> lock(mProgressBarMutex);
    mProgressBar++;
}

void RayTracer::PrintProgressBar()
{
  std::lock_guard<std::mutex> lock(mProgressBarMutex);
  int value = static_cast<int>((mProgressBar * 100.0)/(mBuffer.height-1));

  //system("clear");
  std::cout << mProgressMessage << "\nProgress: |";

  for (int i = 0; i < value; i++)
  {
    std::cout << "=";
  }

  for (int i = 0; i < 100-value; i++)
  {
    std::cout << " ";
  }  

  std::cout << "| " << (mProgressBar * 100.0)/(mBuffer.height-1) << "%\n";
}

void RayTracer::ClearProgressBar()
{
  std::lock_guard<std::mutex> lock(mProgressBarMutex);
  mProgressBar = 0;
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

void RayTracerSettings::Load(const std::string & configFilePath)
{
  std::ifstream file(configFilePath, std::ifstream::in);

  if (file.is_open())
  {
    std::string property;
    int value;

    while (file >> property)
    {
      if (!(file >> value))
      {
        continue;
      }

      if (property == "w")
      {
        mWidth = value;
      }
      else if (property == "h")
      {
        mHeight = value;
      }
      else if (property == "depth")
      {
        mRecursionDepth = (value > 0) ? (value%25) : 1;
      }
      else if (property == "anti-aliasing") 
      {
        mAntiAliasing = (value != 0);
      }
      else if (property == "save_file")
      {
        mSaveFile = (value != 0);
      }
    }
  }
  else
  {
    std::cerr << "ERROR Couldn't open the Ray Tracer configuration file at " << configFilePath << std::endl;
  }

}



