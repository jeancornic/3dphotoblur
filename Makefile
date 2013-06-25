TARGET = main
SOURCES = main.cpp
LIBS = -lGL -lGLU -lglut -lSOIL -lGLEW
SRCS = ShaderProgram.cpp ShaderProgram.h Sphere.h Sphere.cpp Camera.h Camera.cpp
LDLIBS = $(LIBS)

CC = g++

CCFLAGS = -Wall 

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)

clean:
	rm -rf $(TARGET) *~

dep: gcc -MM $(SOURCES)
