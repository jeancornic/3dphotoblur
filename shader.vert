attribute vec3 tangentA;
attribute vec2 texcoords;

varying vec3 normal;
varying vec3 position;
varying vec2 uvs;
varying vec3 lightT;
varying vec3 eyeT;
varying mat3 rotation;

void main(void)
{
    //Vector vertex view
    gl_Position     = ftransform();
    position        = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    vec3 light      = gl_LightSource[0].position.xyz - position;
    light           = normalize(light);

    position        = normalize(position);
    
    //Tangent plan
    normal          = normalize(gl_NormalMatrix * gl_Normal);
    vec3 tangent    = normalize(gl_NormalMatrix * tangentA);
    vec3 binormal   = cross(normal, tangent);
    rotation        = transpose(mat3(tangent, binormal, normal));
    
    //Convert vectors to tangent plan
    lightT          = rotation * light;
    eyeT            = -rotation * position;

    uvs             = texcoords.xy;
}
