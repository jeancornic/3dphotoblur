#include <GL/glew.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

class ShaderProgram
{
    protected:
        GLuint shaderProgramId;
        GLuint vertexShaderId;
        GLuint fragmentShaderId;

        GLuint compileShaderSource(const string, GLenum);

    public:
        ShaderProgram();
        virtual ~ShaderProgram();

        inline GLuint getShaderProgramId() const {return shaderProgramId;}
        inline GLuint getVertexShaderId() const {return vertexShaderId;}
        inline GLuint getFragmentShaderId() const {return fragmentShaderId;}
        inline bool hasVertexShader() const {return vertexShaderId > 0;}
        inline bool hasFragmentShader() const {return fragmentShaderId > 0;}

        void addShaderFromFile(const string);
        static GLenum computeShaderType(const string);
        void start();
        
        inline void bind() {glUseProgram(shaderProgramId);}
        inline void unbind() {glUseProgram(0);}
};

