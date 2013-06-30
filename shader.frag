#define NUM_LIGHTS 1
uniform float near;
uniform float far;

varying vec3 normal;
varying vec3 position;



void main(void)
{
    //depth
    float depth = length(position) / (far - near);
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
}
