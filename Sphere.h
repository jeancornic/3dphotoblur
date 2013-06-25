#include <GL/glew.h>
#include <math.h>
#include <stdlib.h>

using namespace std;

class Sphere
{
    protected:
        GLfloat * _vertexs;
        GLfloat * _normals;
        GLfloat * _tangents;
        GLfloat * _texcoords; 

        GLint _hDivs;
        GLint _vDivs;
        GLfloat _radius;

        void drawVertex(int, int, GLuint = 0, GLuint = 0) const;

    public:
        Sphere(const GLfloat radius, const GLint hDivs, const GLint vDivs);
        ~Sphere();

        void draw(GLuint = 0, GLuint = 0) const;
        void drawHalfSphere(GLuint = 0, GLuint = 0) const;
};

