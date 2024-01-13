#version 460 core
#include "../include/Complex.glsl"

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, std430) restrict readonly buffer FFTInputSSBO
{
    Complex samples[];
} fftInputSSBO;

layout(binding = 1, std430) restrict writeonly buffer FFTOutputSSBO
{
    Complex samples[];
} fftOutputSSBO;

layout(binding = 0, std140) uniform UniformsUBO
{
    uint log2FftSize;
    uint halfDftSize;
    uint numDfts;
} uniformsUBO;

uint BitsReverse(uint x, uint bits);

void main()
{
    const uint invocationID = gl_GlobalInvocationID.x;
    if (invocationID > fftOutputSSBO.samples.length() / 2)
    {
        return;
    }

    // A convolution tells us how much of a given signal is contained in an other.
    // A DFT peforms two convolutions [ sin(testedFreq) & cos(testedFreq) ] over the entire input signal just to get a single output. This makes it O(n^2).
    // Luckily there is a pattern when evaluating the two sinusoids. The two sinusoids evaluted are may be called rootOfUnity.
    //
    // See https://www.desmos.com/calculator/d4gnmx1zhc?lang=de.
	// For every sinusoid-function k pick the other sinusoid-function (k + size / 2)
	// The even index samples (2, 4, 6...) share the same value and the odd index samples are simply negated.
    // This is what the FFT exploits and what makes it O(nlog(n))

    // Reorder so we have pairs with the same rootOfUnity. See https://www.dspguide.com/ch12/2.htm
    const uint firstIndex = BitsReverse(invocationID * 2 + 0, uniformsUBO.log2FftSize);
    const uint secondIndex = BitsReverse(invocationID * 2 + 1, uniformsUBO.log2FftSize);

    // Perform size-2 DFTs as our base. The juicy stuff cancels out at this stage, the explicit version is:
    //  pairFirst = input[0] * GetNThRootsOfUnity(0 * 0, 2) + input[1] * GetNThRootsOfUnity(0 * 1, 2);
    //  pairOther = input[0] * GetNThRootsOfUnity(0 * 0, 2) - input[1] * GetNThRootsOfUnity(0 * 1, 2);
    const Complex cosAndSinConvolution = ComplexAdd(fftInputSSBO.samples[firstIndex], fftInputSSBO.samples[secondIndex]);
    const Complex otherCosAndSinConvolution = ComplexSubtract(fftInputSSBO.samples[firstIndex], fftInputSSBO.samples[secondIndex]);

    fftOutputSSBO.samples[invocationID * 2 + 0] = cosAndSinConvolution;
    fftOutputSSBO.samples[invocationID * 2 + 1] = otherCosAndSinConvolution;
}

uint BitsReverse(uint x, uint bits)
{
    return bitfieldReverse(x) >> (32 - bits);
}