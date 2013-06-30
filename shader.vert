#define NUM_LIGHTS 1



varying vec3 normal;
varying vec3 position;

void main(void)
{
    position        = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    normal          = normalize(gl_NormalMatrix * gl_Normal);
    position        = normalize(position);

    gl_Position     = ftransform();
    gl_FrontColor   = gl_Color;
}
