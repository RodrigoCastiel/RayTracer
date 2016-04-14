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

RayTracer rayTracer;

void display()
{
  //hack to make it only draw once
  static int once = 0;
  if(!once)
  {
    rayTracer.DrawFrame();

    glFlush();
  }
  once = 1;
}

void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(0, rayTracer.GetFrame().width, 0, rayTracer.GetFrame().height, 1, -1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  glutPostRedisplay();
}

int main(int argc, char ** argv)
{
  Scene scene;
  std::string outputFilePath;

  rayTracer.LoadConfigFile("../scenes/config.txt");

  if ((argc < 2) || (argc > 3))
  {  
    printf ("ERROR Usage: %s <input scenefile> [output jpegname]\n", argv[0]);
    exit(0);
  }

  scene.Load(argv[1]);

  if(argc == 3)
  {
    outputFilePath = argv[2];
    
  }
  else if(argc == 2)
  {
    outputFilePath = "output/out.jpg";
  }

  rayTracer.RenderFrame(scene, outputFilePath);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(rayTracer.GetFrame().width, rayTracer.GetFrame().height);
  glutCreateWindow("Ray Tracer");
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  init();
  glutMainLoop();

  return 0;
}

