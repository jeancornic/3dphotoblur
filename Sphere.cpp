#include "Sphere.h"
#include <stdio.h>
//Sphere.cpp

/**
 * Draw 1 sphere by drawing 2 half spheres
 */
Sphere::Sphere(const GLfloat radius, const GLint hDivs, const GLint vDivs)
{
    //Number of rings
    _hDivs   = hDivs;

    //Number of sectors of the half sphere
    _vDivs   = (vDivs + 1) / 2;
    _radius  = radius;
    
    // Note : number of points = (number of sectors + 1) * (number of rings +
    // 1)
    _vertexs = (GLfloat*) malloc (3 * (_hDivs+1) * (_vDivs+1) * sizeof(GLfloat));
    _normals = (GLfloat*) malloc (3 * (_hDivs+1) * (_vDivs+1) * sizeof(GLfloat));
    _tangents = (GLfloat*) malloc (3 * (_hDivs+1) * (_vDivs+1) * sizeof(GLfloat));
    _texcoords = (GLfloat*) malloc (2 * (_hDivs+1) * (_vDivs+1) * sizeof(GLfloat)); 

    //s = 0 to PI
    GLfloat s     = 0;
    //t = 0 to PI
    GLfloat t     = 0;

    GLfloat ds    = M_PI / _vDivs;
    GLfloat dt    = M_PI / _hDivs;
  
    GLfloat x, y, z, u, v; 

    for (int j = 0; j <= _hDivs; j++) {
        t = j * dt;
        v = 1.0 * j / _hDivs;
        
        for (int i = 0; i <= _vDivs; i++) {
            s = i*ds;

            int indice = j * (_vDivs + 1) + i;

            x       = _radius * cos(s) * sin(t);
            y       = _radius * sin(s) * sin(t);
            z       = _radius * cos(t);

            _vertexs[3*indice] = x;
            _vertexs[3*indice + 1] = y;
            _vertexs[3*indice + 2] = z;

            //Normal
            _normals[3*indice] = x / _radius;
            _normals[3*indice + 1] = y / _radius;
            _normals[3*indice + 2] = z / _radius;

            //Tex coords
            u = 1.0 * i / _vDivs;

            _texcoords[2*indice] = u;
            _texcoords[2*indice + 1] = v;

            _tangents[3*indice] = -y / _radius; 
            _tangents[3*indice + 1] = x / _radius;
            _tangents[3*indice + 2] = 0;
        }
    }
}

Sphere::~Sphere()
{
    free(_vertexs);
    free(_normals);
    free(_tangents);
    free(_texcoords);
}

void Sphere::draw(GLuint tgLoc, GLuint tcoordsLoc) const
{
    drawHalfSphere(tgLoc, tcoordsLoc);

    glPushMatrix();
        glRotatef(180, 0, 0, 1);
        drawHalfSphere(tgLoc, tcoordsLoc);
    glPopMatrix();
}

/**
 * Draw half a sphere, with one texture
 */
void Sphere::drawHalfSphere(GLuint tgLoc, GLuint tcoordsLoc) const
{
    glBegin(GL_QUADS);

    for (int j = 0; j < _hDivs; j++) {
        for (int i = 0; i < _vDivs; i++) {
            drawVertex(i, j, tgLoc, tcoordsLoc);
            drawVertex(i+1, j, tgLoc, tcoordsLoc);
            drawVertex(i+1, j+1, tgLoc, tcoordsLoc);
            drawVertex(i, j+1, tgLoc, tcoordsLoc);
        }
    }
    glEnd();
}

/*
 * Specify a vertex to opengl with texCoords, tangent, normal and position
 */
void Sphere::drawVertex(int i, int j, GLuint tgLoc, GLuint tcoordsLoc) const
{
    if (tgLoc != 0) {
        glVertexAttrib3fv(tgLoc, &_tangents[ 3*(j*(_vDivs+1) + i) ]);
    }

    if (tcoordsLoc != 0) {
        glVertexAttrib2fv(tcoordsLoc, &_texcoords[2*(j*(_vDivs+1) + i) ]);
    }

    glNormal3fv(&_normals[ 3*(j*(_vDivs+1) + i) ]);
    glVertex3fv(&_vertexs[ 3*(j*(_vDivs+1) + i) ]);
}
