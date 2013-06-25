uniform sampler2D bumpMap;
uniform sampler2D colorMap;

varying vec3 normal;
varying vec3 position;
varying vec2 uvs;
varying vec3 lightT;
varying vec3 eyeT;
varying mat3 rotation;

void main(void)
{
    vec4 color          = vec4(0.0f, 0.0f, 0.0f, 1.0f); 

    // Bump value : between 0 and 1
    vec2 uv;
    uv.x                = fract(uvs.x);
    uv.y                = fract(uvs.y);

    //Load color from texture
    vec4 textureColor   = vec4(texture2D(colorMap, uv.xy).xyz, 1);

    //Load height from height map
    vec3 bump           = texture2D(bumpMap, uv.xy).xyz; 

    /*****************************
     * Compute normal deplacement
     */
    //First calculate texture coordinates of 2 other points nearby
    vec2 uvh            = vec2(min(uv.x + 1.0/512.0, 1.0), uv.y);
    vec2 uvv            = vec2(uv.x, min(uv.y + 1.0/512.0, 1.0));

    //Calculate difference on x axis and y axis
    vec3 normalDeplH    = vec3(1.0f, 0.0f, texture2D(bumpMap, uvh.xy).x - bump.x);
    vec3 normalDeplV    = vec3(0.0f, 1.0f, texture2D(bumpMap, uvv.xy).x - bump.x);

    //Compute 3D normal deplacement
    vec3 normalDepl     = cross(normalDeplH, normalDeplV);

    /******************************
     * Compute new normal in tangent space
     */
    vec3 bumpNormalT    = normalize(vec3(normal.x + 12*normalDepl.x, normal.y + 12*normalDepl.y,1));

    /*******************************
     * Lighting
     */

    vec3 reflectedT     = reflect(-lightT, bumpNormalT);
        
     //Ambient component
    vec4 ambient        = (gl_FrontMaterial.ambient * gl_LightSource[0].ambient) * textureColor; 
    
    color              += ambient;

    float lambertProduct = dot(lightT, bumpNormalT);

    if (lambertProduct > 0.0) {

        //Diffuse component
        float diffuse   = max(0.0, lambertProduct);

        //Specular component
        float shininess = gl_FrontMaterial.shininess;
        float specular  = max(0.0,pow(dot(reflectedT, eyeT), shininess));

        vec4 fromLightColor = textureColor * gl_FrontMaterial.diffuse * diffuse * gl_LightSource[0].diffuse
            + gl_FrontMaterial.specular * specular * gl_LightSource[0].specular;
        color          += fromLightColor;
    }

    gl_FragColor        = color;
  
}
