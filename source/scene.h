//  +--------------------------------------+
//  | CSCI420 - Computer Graphics          |
//  | Assignment 3 - Ray Tracer            |
//  | Author: Rodrigo Castiel              |
//  +--------------------------------------+
//  scene.h

#ifdef WIN32
  #include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
  #include <GL/gl.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
  #define strcasecmp _stricmp
#endif

#include <imageIO.h>

// ============================================================================================= //
//  
//
//
// ============================================================================================= //

struct VertexAttrib
{
  glm::vec3 Kd;  // Material diffuse.
  glm::vec3 Ks;  // Material specular.
  glm::vec3 n;   // Vertex normal.
  double shininess;  // Alpha component.
};

struct Triangle  // Triangle intersection data.
{
  glm::vec3 a, b, c;  // Three vertices;
};

struct TriangleAttrib
{
  VertexAttrib a, b, c;
};

// ============================================================================================= //

struct Sphere  // Sphere intersection data.
{
  glm::vec3 Xc;
  double radius;
};

typedef VertexAttrib SphereAttrib;  // Sphere material properties.

// ============================================================================================= //

class Scene
{
public:
  // Constructor.
  Scene() {  }

  // Loads scene from a file.
  bool Load(const std::string & file);

  // RayCast - returns a RGB vec3 containing the color of intersection.
  glm::vec3 RayCast(const glm::vec3 & ray, const glm::vec3 & C) const;

private:
  // List of all triangles and list of all corresponding attributes.
  std::vector<Triangle> mTriangles;
  std::vecotr<TriangleAttrib> mTriangleAttribList;

  // List of all spheres and list of all corresponding attributes.
  std::vector<Sphere> mListSpheres;
  std::vector<SphereAttrib> mSphereAttribList;

};

