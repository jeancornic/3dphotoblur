#define NUM_LIGHTS 1
attribute vec2 uvA;

varying vec3 normal;
varying vec3 position;
varying vec2 uv;

void main(void)
{
    position        = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    normal          = normalize(gl_NormalMatrix * gl_Normal);
    position        = normalize(position);

    uv              = uvA;
    
    gl_Position     = ftransform();
    gl_FrontColor   = gl_Color;
}
