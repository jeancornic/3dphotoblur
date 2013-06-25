//Camera.cpp
#include "Camera.h"

Camera::Camera()
{
    _theta      = M_PI / 2;
    _phi        = 5 * M_PI / 12;
    _rho        = 15;

    _up[0]      = 0.0;
    _up[1]      = 0.0;
    _up[2]      = 1;

    computePosition();
}

Camera::~Camera()
{
}

void Camera::computePosition()
{
    _position[0]    = _rho * cos(_theta) * sin(_phi);
    _position[1]    = _rho * sin(_theta) * sin(_phi); 
    _position[2]    = _rho * cos(_phi);

    _direction[0]   = -_position[0];
    _direction[1]   = -_position[1];
    _direction[2]   = -_position[2];
}

void Camera::rotate(const GLfloat dTheta, const GLfloat dPhi)
{
    _theta  += dTheta;
    _phi    += dPhi;

    if (_phi > M_PI) _phi = M_PI - 0.01;
    if (_phi < 0) _phi = 0.01;
}

void Camera::zoom(const GLfloat delta)
{
    _rho += delta;

    if (_rho < 0) _rho = 0;
}

void Camera::apply()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        _position[0], _position[1], _position[2],
        _direction[0], _direction[1], _direction[2],
        _up[0], _up[1], _up[2]
    );
}
