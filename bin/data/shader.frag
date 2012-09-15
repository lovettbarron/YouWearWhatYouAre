//uniform sampler2D tex;
//
//void main(void) {
//    gl_FragColor = texture2D(tex, TexCoord[0]);
//}



/*uniform sampler2D tex;
uniform vec2 uv;
const float sampleDist = 1.0;
const float sampleStrength = 2.2; 

void main(void)
{
    float samples[10];
    samples[0] = -0.08;
    samples[1] = -0.05;
    samples[2] = -0.03;
    samples[3] = -0.02;
    samples[4] = -0.01;
    samples[5] =  0.01;
    samples[6] =  0.02;
    samples[7] =  0.03;
    samples[8] =  0.05;
    samples[9] =  0.08;
    
    vec2 dir = 0.5 - uv; 
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y); 
    dir = dir/dist; 
    
    vec4 color = texture2D(tex,uv); 
    vec4 sum = color;
    
    for (int i = 0; i < 10; i++)
    sum += texture2D( tex, uv + dir * samples[i] * sampleDist );
    
    sum *= 1.0/11.0;
    float t = dist * sampleStrength;
    t = clamp( t ,0.0,1.0);
    
    gl_FragColor = mix( color, sum, t );
} */


uniform float sigma; 

uniform float blurSize;

uniform sampler2D tex; 

const float pi = 3.14159265;

const float numBlurPixelsPerSide = 0.0;
const vec2  blurMultiplyVec      = vec2(0.0, 0.0);

void main() {
    
    // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
    vec3 incrementalGaussian;
    incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);
    incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
    incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;
    
    vec4 avgValue = vec4(0.0, 0.0, 0.0, 0.0);
    float coefficientSum = 0.0;
    
    // Take the central sample first...
    avgValue += texture2D(tex, gl_TexCoord[0].xy) * incrementalGaussian.x;
    coefficientSum += incrementalGaussian.x;
    incrementalGaussian.xy *= incrementalGaussian.yz;
    
    // Go through the remaining 8 vertical samples (4 on each side of the center)
    for (float i = 1.0; i <= numBlurPixelsPerSide; i++) { 
        avgValue += texture2D(tex, gl_TexCoord[0].xy - i * blurSize * 
        blurMultiplyVec) * incrementalGaussian.x;         
        avgValue += texture2D(tex, gl_TexCoord[0].xy + i * blurSize * 
        blurMultiplyVec) * incrementalGaussian.x;         
        coefficientSum += (2.0 * incrementalGaussian.x);
        incrementalGaussian.xy *= incrementalGaussian.yz;
    }
    
    gl_FragColor = texture2D(tex, TexCoord[0]) * (avgValue / coefficientSum);
}