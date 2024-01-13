#version 460 core
#include "../include/Complex.glsl"

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(binding = 1, std430) restrict buffer FFTOutputSSBO
{
    Complex samples[];
} fftOutputSSBO;

layout(binding = 0, std140) uniform UniformsUBO
{
    uint log2FftSize;
    uint halfDftSize;
    uint numDfts;
} uniformsUBO;

void main()
{
    const uint invocationID = gl_GlobalInvocationID.x;
    if (invocationID > fftOutputSSBO.samples.length() / 2)
    {
        return;
    }
    
    const uint halfDftSize = uniformsUBO.halfDftSize;

    // these always multiply together to input.size()
    const uint dftSize = halfDftSize * 2; // doubles every pass until size of input
    const uint numDfts = uniformsUBO.numDfts; // halves every pass until 1

    const uint dftIndex = invocationID / halfDftSize;
    const uint globalOffset = dftIndex * dftSize;
    const uint localDftOffset = invocationID % halfDftSize;
    const uint index = globalOffset + localDftOffset;

    // Working from bottom to top combining DFTs of ever larger size
    const Complex rootOfUnity = GetNThRootsOfUnity(localDftOffset, dftSize);
    const Complex x = ComplexMultiply(rootOfUnity, fftOutputSSBO.samples[index + halfDftSize]);
    const Complex first = ComplexAdd(fftOutputSSBO.samples[index], x);
    const Complex other = ComplexSubtract(fftOutputSSBO.samples[index], x);

    fftOutputSSBO.samples[index] = first;
    fftOutputSSBO.samples[index + halfDftSize] = other;
}