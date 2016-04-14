+------ Computer Graphics Homework 3 -----+
| Professor: Jernej Barbic                |
| TA: Bohan Wang                          |
| Student: Rodrigo Castiel                |
+-----------------------------------------+

===================================================================================================

This project implements a basic Ray Tracer algorithm in C++.

Usage: the width, height, recursion depth, anti-aliasing and output file saving can be set up
in the file "scenes/config.txt".

===================================================================================================

Main Features:

1. Loads a scene file (in 'scene' folder) containing lights, triangles and spheres;
  [EXTRA] Obj file loading
  a. Loads a triangulated .obj file with normals. Example:
    obj
    ../objs/chinese_77k.obj

  [EXTRA] Camera position settable
  b. Camera can be speciied in .scene file. Example:
    camera
    pos: 0 0 3
    rot: .1 0 0

  [EXTRA] Index of refraction settable
  c. Index of refraction of triangles/spheres can also be specified in .scene file:
    n_refr 1.33
    // objects
    n_refr 1.0

2. Renders the provided objects using Phong lighting model and hard shadows;
  [REQUIRED]
  a. Triangle Intersection;
  b. Sphere Intersection;
  c. Triangle Phong shading;
  d. Sphere Phong shading;
  e. Shadows rays;

  [EXTRA]
  f. Recursive reflection;
  g. Recursive refraction using Snell's Law;
  h. Reflectance and Trasmittance following Fresnel's Equations;

[EXTRA] Configutation file
3. Reads configuration file config.txt containing frame properties, such as width,
height, anti-aliasing and save_file.

[EXTRA] Anti-aliasing
4. If anti-aliasing is specified as 1 in scenes/config.txt, then the program performs
an gradient-based adaptive anti-aliasing by supersampling.

[EXTRA] Multi-threading
5. Every RayTracing computation is parallel (by using std::threads) and a progress bar is
showed on the console.

6. Still images: the program generates images if the flag save_file is set up in config.txt.
Check them out in the folder 'pictures'.

=================================================================================================== 