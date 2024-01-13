#pragma once

#include <optional>
#include <complex>

#include <BBG/BBG.h>
#include <BBG/Buffer.h>
#include <BBG/ShaderProgram.h>

namespace Rendering
{
    class FFT
    {
    public:
        using ComplexType = std::complex<float>;

    public:
        explicit FFT() noexcept;

        void ComputeFFT(const BBG::TypedBuffer<ComplexType>& complexInput, uint64_t inputElementsOffset, const BBG::TypedBuffer<ComplexType>& complexOutput, uint64_t outputElementsOffset, uint32_t fftSize) const;

    private:
        struct GpuUniforms
        {
            uint32_t log2FftSize;
            uint32_t halfDftSize;
            uint32_t numDfts;
        };

        std::optional<BBG::TypedBuffer<GpuUniforms>> uniformBuffer;
        std::optional<BBG::ShaderProgram> reorderStageProgram;
        std::optional<BBG::ShaderProgram> generalStageProgram;
    };
}