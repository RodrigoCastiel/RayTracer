//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
//  main.cpp

#include <cstdio>
#include <chrono>

#include "gl_header.h"

#include "scene.h"
#include "camera.h"
#include "ray_tracer.h"
#include "math_functions.h"

#define MODE_DISPLAY 1
#define MODE_JPEG    2

int mode = MODE_DISPLAY;

Scene scene;
Camera camera;
RayTracer rayTracer;

void display()
{
  //hack to make it only draw once
  static int once = 0;
  if(!once)
  {
    rayTracer.ParallelRender(scene, camera);
    rayTracer.SaveFrame("without_anti-aliasing.jpg");

    rayTracer.AdaptativeAntiAliasing(scene, camera);
    rayTracer.SaveFrame("with_anti-aliasing.jpg");

    rayTracer.DrawFrame();
    glFlush();

    if(mode == MODE_JPEG)
    {
      rayTracer.SaveFrame("blah.jpg");
      printf("Frame successfully saved.\n");
    } 
  }
  once = 1;
}

void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(0, kDefaultWidth, 0, kDefaultHeight, 1, -1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  glutPostRedisplay();
}

int main(int argc, char ** argv)
{
  scene.Load("../scenes/spheres.scene");

  // if ((argc < 2) || (argc > 3))
  // {  
  //   printf ("ERROR Usage: %s <input scenefile> [output jpegname]\n", argv[0]);
  //   exit(0);
  // }
  // if(argc == 3)
  // {
  //   mode = MODE_JPEG;
  //   filename = argv[2];
  // }
  // else if(argc == 2)
  // {
  //   mode = MODE_DISPLAY;
  // }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(kDefaultWidth, kDefaultHeight);
  glutCreateWindow("Ray Tracer");
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  init();
  glutMainLoop();

  return 0;
}

