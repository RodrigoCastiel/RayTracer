//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
// ray_tracer.h

#pragma once

#include <string>
#include <iostream>
#include "gl_header.h"

const int kDefaultWidth  = 640;
const int kDefaultHeight = 480;
const int kFormatSize = 3;

const int kMaxWidth  = 4096;
const int kMaxHeight = 2160;


// ============================================================================================= //

template <const int W = kMaxWidth, const int H = kMaxHeight>
struct ImgBuffer
{
  // The actual buffer (static).
  unsigned char data[ W * H * kFormatSize ];

  // Dimensions of output images.
  int width   { kDefaultWidth  };
  int height  { kDefaultHeight };

  // Does not check boundaries - returns pixel regarding sub buffer defined by width and height.
  inline unsigned char * GetPixel(int x, int y) { return &data[kFormatSize*(width * y + x)]; }

  // Outputs image as JPEG to 'filePath'.
  void Save(const std::string & filePath)
  {
    ImageIO img(width, height, kFormatSize, data);
  
    if (img.save(filePath.c_str(), ImageIO::FORMAT_JPEG) != ImageIO::OK)
    {
      std::cerr << "ERROR Saving image buffer (JPEG) at " << filePath << ".\n";
    }
    else
    { 
      std::cout << "Image buffer (JPEG) saved at " << filePath << ".\n";
    }
  }
};

// ============================================================================================= //

class RayTracer
{
public:
  RayTracer() {  }

  

private:
  ImgBuffer mBuffer;

  

};

// ============================================================================================= //