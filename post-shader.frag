#define NUM_LIGHTS 1
#define HORIZONTAL 1
#define VERTICAL 2

//Depth of field parameters
uniform vec2 screenWH;

uniform float near;
uniform float far;
uniform float inFocus;
uniform float blurCoeff;
uniform float PPM;

uniform sampler2D imageTex;
uniform sampler2D depthTex;

varying vec3 normal;
varying vec3 position;
varying vec2 uv;

float linearizeDepth(vec2 uv)
{
    float z = texture2D(depthTex, uv).x;
    return (2.0 * near) / (far + near - z * (far - near));   
}

float getBlurDiameter(float depth)
{ 
    float d = depth * (far - near);
    float diff = abs(d - inFocus);
    float xdd = (d < inFocus) ? (inFocus - diff) : (inFocus + diff); 
    
    float b = blurCoeff * (diff / xdd); 
    return b * PPM; 
}

vec4 blur(float blurD, int dir)
{
    vec4 color = vec4(0,0,0,1);
    
    float pixelSize;

    // Compute size of the pixel
    if (dir = HORIZONTAL)
        pixelSize = 1 / screenWH.x;
    else
        pixelSize = 1 / screenWH.y;

    for (int i = 0; i < blurD; i++) {
        float offset    = i - blurD / 2.0;
        vec2 uvPixel    = uv + offset * pixelSize;

        color   += vec4(texture2D(imageTex, uvPixel).xyz, 1);
    }

    return color;
}

void main(void)
{
    vec4 color          = vec4(texture2D(imageTex, uv.xy).xyz, 1);
    float MAX_BLUR      = 3.0;

    float depth = linearizeDepth(uv.xy);
    depth       = texture2D(depthTex, uv).x;

    float blurD = getBlurDiameter(depth);
    blurD   = min(MAX_BLUR, floor(blurD));
 
    //Bluring
    vec4 blurColor   = vec4(0);
    if (blurD > 1.0) {
        blurColor = blur(blurD, HORIZONTAL); 
    } else {
        blurColor = vec4(texture2D(imageTex, uv.xy).xyz, 1);
    }
   
    float c = min(blurD / MAX_BLUR, 1.0);

    gl_FragColor    = (1-c)*color + c * blurColor;

    //gl_FragColor    = vec4(texture2D(imageTex, uv.xy).xyz, 1);
    //gl_FragColor    = vec4(depth, depth, depth, 1);
    //gl_FragColor    = vec4(blurD, blurD, blurD, 1);
}
