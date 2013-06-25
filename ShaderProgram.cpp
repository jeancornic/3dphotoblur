#include "ShaderProgram.h"
//ShaderProgram.cpp

ShaderProgram::ShaderProgram()
{
    shaderProgramId = glCreateProgram();
    vertexShaderId = 0;
    fragmentShaderId = 0;
}

ShaderProgram::~ShaderProgram()
{
    if (hasVertexShader()) {
        glDeleteShader(vertexShaderId);
    }
    if (hasFragmentShader()) {
        glDeleteShader(fragmentShaderId);
    }

    glDeleteProgram(shaderProgramId);
}

void ShaderProgram::addShaderFromFile(const string fileName)
{
	GLenum type = 0;
	type 		= ShaderProgram::computeShaderType(fileName);

	GLuint shaderId = compileShaderSource(fileName, type);

	if (!shaderId) {
		exit(0);
	}

	if (type == GL_VERTEX_SHADER) {
		this->vertexShaderId = shaderId;
	} else if (type == GL_FRAGMENT_SHADER) {
		this->fragmentShaderId = shaderId;
	}

	glAttachShader(shaderProgramId, shaderId);
}

GLuint ShaderProgram::compileShaderSource(const string fileName, GLenum type)
{
	//File exists ?
	FILE* file = fopen(fileName.c_str(), "r");

	if (file == NULL) {
		return 0;
	}

	//File reading
	GLchar * data;
	fseek (file , 0 , SEEK_END);
	GLint lSize = ftell (file);
	rewind (file);
	data = (char*) malloc (sizeof(char)*(lSize + 1));
	if (data == NULL) {
		fputs ("Memory error", stderr);
		exit(2);
	}
	size_t result = fread (data,1,lSize,file);
	data[lSize] = '\0';
	fclose(file);

	GLuint shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, (const GLchar**)&data, NULL);
	free(data);

	glCompileShader(shaderId);
    glAttachShader(shaderProgramId, shaderId);

	return shaderId;
}

void ShaderProgram::start()
{
	glLinkProgram(shaderProgramId);
    
    GLint status; 
    glGetProgramiv(shaderProgramId, GL_VALIDATE_STATUS, &status);

    if (status == GL_FALSE) {
        printf("Error compiling program\n");
   
        glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {

            char log[1000];
            int lenLog;
       
            glGetShaderInfoLog(vertexShaderId, 1000, &lenLog, log);
            printf("%d %s\n", lenLog, log);
        }
        
        glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {

            char log[1000];
            int lenLog;
       
            glGetShaderInfoLog(fragmentShaderId, 1000, &lenLog, log);
            printf("%d %s\n", lenLog, log);
        }
    }
}

GLenum ShaderProgram::computeShaderType(const string fileName)
{
    //Get extension
    string extension;
    size_t pointer  = fileName.find_last_of(".");

    if (!pointer) {
        return 0;
    }
    
    extension = fileName.substr(pointer);

    if (extension == ".vert" || extension == ".vs") {
        return GL_VERTEX_SHADER;
    } else if (extension == ".frag" || extension == ".fs") {
        return GL_FRAGMENT_SHADER;
    } else {
        return 0;
    }
}
