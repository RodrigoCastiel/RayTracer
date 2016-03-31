HW3_CXX_SRC=main.cpp scene.cpp ray_tracer.cpp camera.cpp math_functions.cpp
HW3_HEADER=scene.h ray_tracer.h camera.h math_functions.h
HW3_OBJ=$(notdir $(patsubst %.cpp,%.o,$(HW3_CXX_SRC)))

IMAGE_LIB_SRC=$(wildcard ../external/imageIO/*.cpp)
IMAGE_LIB_HEADER=$(wildcard ../external/imageIO/*.h)
IMAGE_LIB_OBJ=$(notdir $(patsubst %.cpp,%.o,$(IMAGE_LIB_SRC)))

HEADER=$(HW3_HEADER) $(IMAGE_LIB_HEADER)
CXX_OBJ=$(HW3_OBJ) $(IMAGE_LIB_OBJ)

CXX=g++
TARGET=hw3
CXXFLAGS=-DGLM_FORCE_RADIANS -Wno-unused-result -std=c++11 -pthread
OPT=-O3

UNAME_S=$(shell uname -s)

ifeq ($(UNAME_S),Linux)
  PLATFORM=Linux
  INCLUDE=-I../external/glm/ -I../external/imageIO -I../external/
  LIB=-lGLEW -lGL -lglut -ljpeg
  LDFLAGS=
else
  PLATFORM=Mac OS
  INCLUDE=-I../external/glm/ -I../external/imageIO -I../external/jpeg-9a-mac/include -I../external/
  LIB=-framework OpenGL -framework GLUT ../external/jpeg-9a-mac/lib/libjpeg.a
  CXXFLAGS+= -Wno-deprecated-declarations
  LDFLAGS=-Wl,-w
endif

all: $(TARGET)

$(TARGET): $(CXX_OBJ)
	$(CXX) $(LDFLAGS) $^ $(OPT) $(LIB) -o $@

$(HW3_OBJ):%.o: %.cpp $(HEADER)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

$(IMAGE_LIB_OBJ):%.o: ../external/imageIO/%.cpp $(IMAGE_LIB_HEADER)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

clean:
	rm -rf *.o $(TARGET)
