#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include <SOIL/SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

#include "ShaderProgram.h"
#include "Sphere.h"
#include "Camera.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

static int screenWidth;
static int screenHeight;
static int WindowName;
static int FPS;
static float t;

/*
 * texture ids container
 */
static GLuint texIdDepth;
static GLuint texIdFBO;
static GLuint idDepthBuffer;
static GLuint idFBO;

static GLuint texIdBump[2];

static ShaderProgram * shaderProgram;
static ShaderProgram * postShaderProgram;

static Camera * camera;

void initLights();
void initShaders();
void initTextures();
void initMaterials();
void initScene();
void initFBO();

//display
void display();
void reshape(int, int);
void idle();

//keyboard
void handleKeyStates();
void keyboardUpFunc(unsigned char, int, int);
void keyboardFunc(unsigned char, int, int);
void specialFunc(int, int, int);
void specialUpFunc(int, int, int);
void updateBlur();

void loadTexture(const char*, GLuint *);
void printHelp();

static bool keyStates[256] = {};
static int polygonMode  = 0;
static int shaderMode   = 3; 
static int movingMode   = 0;
static float focal      = 10.0;
static float inFocus    = 20.0;
static float fStop      = 2.8;
static float blurCoeff;

/**
 * Main function
 * start display loop
 */
int main(int argc, char* argv[], char* envp[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //Double Buffer + Z-Buffer

    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT); 

    WindowName = glutCreateWindow("Bump mapping");

    //init glut functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutSpecialFunc(specialFunc);
    glutSpecialUpFunc(specialUpFunc);
    glutKeyboardFunc(keyboardFunc);
    glutKeyboardUpFunc(keyboardUpFunc);
    
    GLenum glewErr = glewInit();

    initFBO();
    initShaders();
    initTextures();
    initMaterials();
    initScene();
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    camera  = new Camera();

    updateBlur();
    printHelp();

    glutMainLoop();

    return 0;
}

/**
 * display loop
 */
void display()
{
    handleKeyStates();
    
    //z-buffer
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    /*
     * First Pass : Render Scene to FrameBuffer
     *                     Depth to FrameBuffer
     */
    
    //FrameBuffer Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texIdFBO);
    glEnable(GL_TEXTURE_2D);

    //DepthBuffer Texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texIdDepth);
    glEnable(GL_TEXTURE_2D);

    /*
    //Color texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texIdBump[1]);
    
    //Bump mapping texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texIdBump[2]);
    */

    glBindFramebuffer(GL_FRAMEBUFFER, idFBO);
    
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    camera->apply();
    
    //Position lights
    initLights();
    
    if (shaderMode) shaderProgram->bind();

    GLuint programId        = shaderProgram->getShaderProgramId(); 
    //GLuint bumpMappingLoc   = glGetAttribLocation(programId, "bumpMapping");
    GLuint texCoordsLoc     = glGetAttribLocation(programId, "texCoords");
    //GLuint texMapLoc        = glGetUniformLocation(programId, "texMap");
    //GLuint texColorLoc      = glGetUniformLocation(programId, "texColor");
    //glUniform1i(texMapLoc, 2);
    //glUniform1i(texColorLoc, 3);
    
    //Bindings
    GLuint postProgramId    = postShaderProgram->getShaderProgramId(); 

    /**
     * Drawing floor
     */

    //Floor vertexs
    //glVertexAttrib1f(bumpMappingLoc, 1.0);
    glNormal3f(0,0,1.0f);
    glColor3f(0, 1.0f, 0);
    glBegin(GL_QUADS);
        glVertexAttrib2f(texCoordsLoc,0,0); glVertex3f(-10,-10,0);
        glVertexAttrib2f(texCoordsLoc,1,0); glVertex3f(-10,10,0);
        glVertexAttrib2f(texCoordsLoc,1,1); glVertex3f(10,10,0);
        glVertexAttrib2f(texCoordsLoc,0,1); glVertex3f(10,-10,0);
    glEnd();

    //glVertexAttrib1f(bumpMappingLoc, 0.0);
    glNormal3f(0,1.0f,0);
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_QUADS);
        glVertex3f(10,-10,0);
        glVertex3f(-10,-10,0);
        glVertex3f(-10,-10,15);
        glVertex3f(10,-10,15);
    glEnd();
    
    glColor3f(1.0, 0, 0);
    glTranslatef(5,2,2);
    glutSolidSphere(2,20,20);

    glTranslatef(-10,-4,-2);
    glutSolidCone(3, 5, 20, 20);
    
    shaderProgram->unbind();
    
    /*
     * Second PASS
     */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClearColor(0,0,0,1.0f);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0,20, 0, 0, 0, 0, 1, 0);
   
    if (shaderMode) postShaderProgram->bind();

    GLuint screenWHLoc      = glGetUniformLocation(postProgramId, "screenWH");
    GLuint nearLoc          = glGetUniformLocation(postProgramId, "near");
    GLuint farLoc           = glGetUniformLocation(postProgramId, "far");
    GLuint inFocusLoc       = glGetUniformLocation(postProgramId, "inFocus");
    GLuint blurCoeffLoc     = glGetUniformLocation(postProgramId, "blurCoeff");
    GLuint PPMLoc           = glGetUniformLocation(postProgramId, "PPM");
    GLuint imageTexLoc      = glGetUniformLocation(postProgramId, "imageTex");
    GLuint depthTexLoc      = glGetUniformLocation(postProgramId, "depthTex");
    
    glUniform2f(screenWHLoc, (float)screenWidth, (float)screenHeight);
    glUniform1i(imageTexLoc, 0);
    glUniform1i(depthTexLoc, 1);
    glUniform1f(nearLoc, 1.0);
    glUniform1f(farLoc, 100000.0);
    glUniform1f(PPMLoc, 20.0);
    glUniform1f(inFocusLoc, inFocus);
    glUniform1f(blurCoeffLoc, blurCoeff);
    
    GLuint uvALoc       = glGetAttribLocation(postProgramId, "uvA");

    glNormal3f(0,0,1);
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_QUADS);
        glVertexAttrib2f(uvALoc, 0,0); glVertex3f(-20,-15,0);
        glVertexAttrib2f(uvALoc, 1,0); glVertex3f(20,-15,0);
        glVertexAttrib2f(uvALoc, 1,1); glVertex3f(20,15,0);
        glVertexAttrib2f(uvALoc, 0,1); glVertex3f(-20,15,0);
    glEnd();

    glutSwapBuffers();
    glutPostRedisplay();
}

/**
 * Reshape function
 */
void reshape(int w, int h)
{
    if (h==0) {
        h = 1;
    }

    float ratio  = 1.0 * w / h;
    screenWidth  = w;
    screenHeight = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h); //Entire window view port

    gluPerspective(70, ratio, 1, 100000);
}

/**
 * idle function : handle time
 */
void idle () {
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    static unsigned int counter = 0;
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [64];
        sprintf (winTitle, "3d - FPS: %d, Focal: %.2f, Aperture: %.1f", FPS, focal, fStop);
        glutSetWindowTitle (winTitle);
        lastTime = currentTime;
    }

    t = currentTime + 10000;

    glutPostRedisplay ();
}

void initFBO()
{
    /*
     * Frame Buffer Texture Creation
     */
    glGenTextures(1, &texIdFBO);
    glBindTexture(GL_TEXTURE_2D, texIdFBO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    /**
     * Depth texture
     */
    glGenTextures(1, &texIdDepth);
    glBindTexture(GL_TEXTURE_2D, texIdDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    /*
     * Frame Buffer generation and bindings
     */
    glGenFramebuffers(1, &idFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, idFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texIdFBO, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texIdDepth, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texIdDepth);

    // Draw buffer
    GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    /*
     * Depth render buffer generation and binding
     */
    //glGenRenderbuffers(1, &idDepthBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, idDepthBuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
   
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        //Error
        printf("Problem");
    }

    //Frame Buffer Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
 * Load textures to texIDs table
 */
void initTextures()
{
    glGenTextures(2, texIdBump);
    loadTexture("tileNormals.jpg", &texIdBump[0]);
    loadTexture("tileColor.jpg", &texIdBump[1]);
}

/**
 * Position lights
 */
void initLights()
{
    GLfloat light0Pos[4]    = {0.0, -15.0, 20.0, 0};
    GLfloat light0Dir[3]    = {0.0,15.0,-20.0};
    GLfloat light0Color[4]  = {1.0, 1.0, 1.0, 1};
    GLfloat light0Spec[4]   = {0.8,0.8,0.8,1};

    GLfloat ambient[4]      = {0.2, 0.2, 0.2, 1};

    glLightfv (GL_LIGHT0, GL_POSITION, light0Pos);
//    glLightfv (GL_LIGHT0, GL_SPOT_DIRECTION, light0Dir);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light0Color);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light0Spec);
    glLightfv (GL_LIGHT0, GL_AMBIENT, ambient);
}

/**
 * Define objects material values
 */
void initMaterials()
{
    GLfloat am[4] = {0.7f, 0.7f, 0.7f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, am);

    GLfloat dm[4] = {0.4f, 0.4f, 0.4f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dm);

    GLfloat sm[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sm);

    float shininess = 0.1f;
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * 128.0);
}

/**
 * Init vertex and fragment shaders
 */
void initShaders()
{
	shaderProgram = new ShaderProgram();
	shaderProgram->addShaderFromFile("shader.vert");
    shaderProgram->addShaderFromFile("shader.frag");
    
	shaderProgram->start();
	
    postShaderProgram = new ShaderProgram();
	postShaderProgram->addShaderFromFile("post-shader.vert");
    postShaderProgram->addShaderFromFile("post-shader.frag");
    
	postShaderProgram->start();
}

/**
 * Load objects of the scene, who will be drawn in display function
 */
void initScene()
{
}

/*
 ********************
 * KEYBOARS FUNCTIONS
 ********************
 */

void keyboardFunc(unsigned char key, int x, int y)
{
    keyStates[(int)key]  = true;

    // W
    if ((int)key == 119) {
        if (polygonMode == 0) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            polygonMode = 1;
        } else if (polygonMode == 1) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            polygonMode = 0;
        }
    }

    // S : toggle shaders
    if ((int)key == 115) {
        if (shaderMode == 3) {
            shaderMode = 0;
        } else if (shaderMode == 0) {
            shaderMode = 3;
        }
    }

    // M : moving mode
    if ((int)key == 109) {
        movingMode += 1;
        if (movingMode == 2) {
            movingMode = 0;
        }
    }

    // Q : quit
    if ((int)key == 113) {
        exit(0);
    }
}

void keyboardUpFunc(unsigned char key, int x, int y)
{
    keyStates[(int)key]  = false;

    printf("%d\n", (int)key);
}

void specialFunc(int key, int x, int y)
{
    keyStates[key]  = true;
}

void specialUpFunc(int key, int x, int y)
{
    keyStates[key]  = false;
}

void handleKeyStates()
{
    GLfloat dRho = 0.2, 
            dTheta = M_PI / 180, 
            dPhi = M_PI / 180;

    if (keyStates[(int)GLUT_KEY_UP]) {
        camera->rotate(0, -dPhi);
    }
    
    if (keyStates[(int)GLUT_KEY_DOWN]) {
        camera->rotate(0, dPhi);
    }
    
    if (keyStates[(int)GLUT_KEY_LEFT]) {
        camera->rotate(-dTheta, 0);
    }
    
    if (keyStates[(int)GLUT_KEY_RIGHT]) {
        camera->rotate(dTheta, 0);
    }

    //Key : A
    if (keyStates[97]) {
        camera->zoom(-dRho);
    }

    if(keyStates[122]) {
        camera->zoom(dRho);
    }
    
    // O : focal + / P : -
    if (keyStates[111]) {
        inFocus += 1;
        updateBlur();
    }
    if (keyStates[112]) {
        inFocus -= 1;
        if (inFocus < 0) inFocus = 0;
        updateBlur();
    }

    // L/M : fStop +-
    if (keyStates[108]) {
        fStop += 0.1;
        if (fStop > 20) fStop = 20;
        updateBlur();
    }
    if (keyStates[109]) {
        fStop -= 0.1;
        if (fStop < 1.5) fStop = 1.5;
        updateBlur();
    }

    camera->computePosition();
}

/**
 * load a texture from a file
 */
void loadTexture(const char* file, GLuint * texid)
{
    *texid = SOIL_load_OGL_texture(
		file,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_COMPRESS_TO_DXT
	);

    if (!*texid) {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }
	
	glBindTexture(GL_TEXTURE_2D, *texid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

void updateBlur()
{
    float ms = focal / (inFocus - focal);

    blurCoeff = focal * ms / fStop;

    printf("%.2f %.2f %.2f\n", inFocus, ms, blurCoeff);
}

void printHelp()
{
    printf("Action keys :\n");
    printf("- A : Zoom in\n- Z : Zoom out\n- W : Toggle wireframe mode\n");
    printf("- Up, Down, Left, Right : Rotate camera\n");
    printf("- Q : Quit\n");
}

