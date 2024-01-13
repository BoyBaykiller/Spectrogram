#version 460 core

struct AudioSample
{
    float channel0;
};

layout(binding = 0, std430) restrict readonly buffer WaveformSSBO
{
    AudioSample samples[];
} waveformSSBO;

layout(binding = 0, std140) uniform UniformsUBO
{
    mat4 orthoProjection;
    uint waveformRingBufferHead;
} uniformsUBO;

float MapYCoord(float v);

void main()
{
    uint sampleIndex = ((gl_VertexID + uniformsUBO.waveformRingBufferHead)) % waveformSSBO.samples.length();
    AudioSample audioSample = waveformSSBO.samples[sampleIndex];
    
    float x = gl_VertexID / float(waveformSSBO.samples.length());
    float y = MapYCoord(audioSample.channel0);
    vec4 clipPos = uniformsUBO.orthoProjection * vec4(x, y, 0.0, 1.0);
    gl_Position = clipPos;
}

float MapYCoord(float v)
{
    // v is expected to be in range [-1.0, 1.0]
    // v output is expected to be in range [0.0, 1.0]

    float y = mix(0.0, 1.0, v * 0.5 + 0.5);
    //float y = mix(0.0, 1.0, sqrt(abs(v)) * sign(v) * 0.5 + 0.5);
    return y;
}
