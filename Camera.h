#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>

using namespace std;

class Camera {
    protected :
        GLfloat _position[3];
        GLfloat _direction[3];
        GLfloat _up[3];
        GLfloat _theta;
        GLfloat _phi;
        GLfloat _rho;

    public:
        Camera();
        ~Camera();

        inline const GLfloat* getDirection() const {return _direction;}
        inline const GLfloat* getPosition() const {return _position;}

        inline void setTheta(const GLfloat theta) {_theta = theta;}
        inline void setPhi(const GLfloat phi) {_phi = phi;}
        
        void rotate(const GLfloat dTheta, const GLfloat dPhi);
        void zoom(const GLfloat delta);
        void computePosition();

        void apply();
};
