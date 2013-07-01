#define NUM_LIGHTS 1

//Depth of field parameters
uniform float near;
uniform float far;
uniform float focus;
uniform float blurCoeff;
uniform float PPM;

uniform sampler2D imageTex;

varying vec3 normal;
varying vec3 position;
varying vec2 uv;

float getBlurDiameter(float depth)
{ 
    float d = depth * (far - near);
    float diff = abs(d - focus);
    float xdd = (d < focus) ? (focus - diff) : (focus + diff); 
    
    float b = blurCoeff * (diff / xdd); 
    
    return b * PPM; 
}

void main(void)
{
    //depth
    float depth = length(position) / (far - near);

    /*
    vec4 finalC = vec4(0,0,0,1);

    vec4 baseC  = gl_Color; //Or textured
    
    for (int i = 0; i < NUM_LIGHTS; i++) {
        vec3 light      = normalize(gl_LightSource[i].position.xyz - position);
        vec3 R          = normalize(-reflect(light, normal));

        //ambient
        vec4 ambientI   = gl_FrontMaterial.ambient * gl_LightSource[i].ambient;
        vec4 diffuseI   = vec4(0,0,0,1);
        vec4 specularI  = vec4(0,0,0,1);

        //diffuse + specular
        float lProduct  = dot(light, normal);

        if (lProduct > 0.0) {
            diffuseI    = gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse
                * lProduct;
            
            float shininess = gl_FrontMaterial.shininess;

            specularI   = pow( max(0.0, dot(R, -position)), shininess ) *
                gl_FrontMaterial.specular * gl_LightSource[i].specular;
        }

        finalC  += ambientI * baseC + diffuseI * baseC + specularI;
    }
   
    gl_FragColor    = finalC;
    */

    gl_FragColor    = vec4(texture2D(imageTex, uv.xy).xyz, 1);
}
