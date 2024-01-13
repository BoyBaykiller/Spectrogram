#version 460 core
#include "../include/Complex.glsl"

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(binding = 0) restrict writeonly uniform image2D ImgResult;

layout(binding = 0, std430) restrict readonly buffer SpectrogramSSBOSSBO
{
    // flattened version of samples[][shortTimeFFTSize];
    Complex samples[];
} spectrogramSSBO;

layout(binding = 0, std140) uniform UniformsUBO
{
    uint spectrogramHeadXAxis;
    uint shortTimeFFTSize;
    float inputScale;
} uniformsUBO;

vec3 Heatmap(float a);

void main()
{
    const ivec2 imgResultSize = imageSize(ImgResult);
    const ivec2 imgCoord = ivec2(gl_GlobalInvocationID.xy); 
    if (any(greaterThanEqual(imgCoord, imgResultSize)))
    {
        return;
    }

    if (gl_GlobalInvocationID.y > uniformsUBO.shortTimeFFTSize)
    {
        return;
    }

    const uint countShortTimeFFTs = imgResultSize.x;
    const uint startIndex = ((gl_GlobalInvocationID.x + uniformsUBO.spectrogramHeadXAxis) % countShortTimeFFTs);

    const Complex spectrogramSample = spectrogramSSBO.samples[(uniformsUBO.shortTimeFFTSize * startIndex) + gl_GlobalInvocationID.y + 1]; // skip the 0th output of the FFT as its special

    const float magnitude = ComplexGetMagnitude(spectrogramSample);
    const float normalizedMagnitude = magnitude / uniformsUBO.shortTimeFFTSize;
    const float scaledMagnitude = pow(normalizedMagnitude, 1.0 - uniformsUBO.inputScale); // arbitrary scale

    const vec3 heatmapColor = Heatmap(scaledMagnitude);

    imageStore(ImgResult, imgCoord, vec4(heatmapColor, 1.0));
}

vec3 Heatmap(float a)
{
    // https://www.desmos.com/calculator/vybplrgwtv
    const float pointOfRapidRedGoing = 0.55;
    const float howRadpidRedShouldGo = 3.5;

    const float red = pow(a * (1.0 / pointOfRapidRedGoing), howRadpidRedShouldGo);
    const float green = 0.0;
    const float blue = max(pow(a / 0.3, 3.0) - pow(a / 0.34, 4.0), 0.0);
    
    const vec3 result = vec3(red, green, blue);
    return result;
}

