#define NUM_LIGHTS 1
#define HORIZONTAL 1
#define VERTICAL 2
#define PI 3.141592

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
    vec4 color = vec4(0,0,0,0);
    
    float pixelSize;
    vec2  pixelOffset;

    // Compute size of the pixel
    if (dir = HORIZONTAL) {
        pixelSize   = 1 / screenWH.x;
        pixelOffset = vec2(pixelSize, 0.0);
    } else {
        pixelSize = 1 / screenWH.y;
        pixelOffset = vec2(0.0, pixelSize);
    }

    int count = 0;

/*
    float sigma = 1;  // Gaussian sigma  
    float norm  = 1.0/(sqrt(2*PI)*sigma);  

    vec4 acc;                      // accumulator  
    acc = texture2D(imageTex, uv); // accumulate center pixel  

    for (int i = 1; i <= blurD; i++) {  
        float coeff = exp(-0.5 * float(i) * float(i) / (sigma * sigma));  
        acc += (texture2D(imageTex, uv - float(i) * pixelOffset)) * coeff; // L  
        acc += (texture2D(imageTex, uv + float(i) * pixelOffset)) * coeff; // R  
    }  
    acc *= norm;            // normalize for unity gain  
    return acc;
*/

    for (int i = 0; i < blurD; i++) {
        float offset    = i - blurD / 2.0;
        vec2 uvPixel    = uv.xy + offset * pixelOffset;

        color   += vec4(texture2D(imageTex, uvPixel).xyz, 1);
        count++;
    }

    return color / (count);
}

void main(void)
{
    vec4 color          = vec4(texture2D(imageTex, uv.xy).xyz, 1);
    float MAX_BLUR      = 10.0;

    float depth = linearizeDepth(uv.xy);
    //depth       = texture2D(depthTex, uv).x;
    
    float blurD = getBlurDiameter(depth);
    blurD   = min(MAX_BLUR, floor(blurD));
   
    //Bluring
    vec4 blurColorH     = vec4(0);
    vec4 blurColorV     = vec4(0);
    vec4 blurColor      = vec4(0);

    if (blurD >= 1.0) {
        blurColorH = blur(blurD, HORIZONTAL); 
        blurColorV = blur(blurD, VERTICAL);
        blurColor  = (blurColorH + blurColorV) / 2;
    } else {
        blurColor = vec4(texture2D(imageTex, uv.xy).xyz, 1);
    }
   
    float c = min(blurD / MAX_BLUR, 1.0);

    gl_FragColor    = (1-c)*color + c * vec4(blurColor.xyz, 1);
    
    //gl_FragColor    = blurColor;
    //gl_FragColor    = vec4(texture2D(imageTex, uv.xy).xyz, 1);
    //gl_FragColor    = vec4(depth, depth, depth, 1);
    //gl_FragColor    = vec4(blurD / MAX_BLUR, blurD/MAX_BLUR, blurD/MAX_BLUR, 1);
}
