//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
//  main.cpp

#include <cstdio>

#include "gl_header.h"

#include "scene.h"
#include "camera.h"
#include "ray_tracer.h"
#include "math_functions.h"

#define MODE_DISPLAY 1
#define MODE_JPEG    2

int mode = MODE_DISPLAY;

void display()
{
  //hack to make it only draw once
  static int once = 0;
  if(!once)
  {
    ImgBuffer<kDefaultWidth, kDefaultHeight> img;
    img.Draw();
    glFlush();
    if(mode == MODE_JPEG)
    {
      img.Save("blah.jpg");
    } 
    printf("saved\n");
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
  Camera camera;

  glm::vec3 O = camera.GetCenterCoordinates();
  glm::vec3 r = camera.CastRay(0, 400, 400, 400);
  Triangle triangle = { glm::vec3(-5,  0, -1),
                        glm::vec3(+5,  0, -1),
                        glm::vec3( 0, 10, -1)};

  printf("origin = (%.2lf, %.2lf, %.2lf)'\n", O[0], O[1], O[2]);
  printf("ray    = (%.2lf, %.2lf, %.2lf)'\n", r[0], r[1], r[2]);

  glm::vec3 intersection;
  glm::vec3 barycentric;
  float t;

  if (IntersectRay(triangle, glm::normalize(r), O, intersection, barycentric, t))
  {
    printf("intersection = (%.2lf, %.2lf, %.2lf)'\n", intersection[0], intersection[1], intersection[2]);
    printf("barycentric  = (%.2lf, %.2lf, %.2lf)'\n", barycentric[0],  barycentric[1],  barycentric[2]);
    printf("t = %lf\n\n", t);
  }

  // Scene scene;
  // scene.Load("../scenes/SIGGRAPH.scene");

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

  // glutInit(&argc, argv);
  // glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  // glutInitWindowPosition(0, 0);
  // glutInitWindowSize(kDefaultWidth, kDefaultHeight);
  // glutCreateWindow("Ray Tracer");
  // glutDisplayFunc(display);
  // glutIdleFunc(idle);
  // init();
  // glutMainLoop();

  return 0;
}

