#pragma once
#include <optional>
#include <vector>
#include <complex>

#include <SNT/SNT.h>

#include <BBG/Buffer.h>
#include <BBG/ShaderProgram.h>
#include <BBG/Texture2D.h>

#include "../FFT/FFT.h"

namespace Rendering
{
	class Spectrogram
	{
	public:
		using ComplexType = std::complex<float>;

	public:
		explicit Spectrogram(uint32_t timeAxisSizeX, uint32_t frequencyAxisSizeY, uint32_t shortTimeFFTSize) noexcept;

		void AddSamples(std::span<const ComplexType> newSamples);
		void RenderSpectrogram() const;
		void InlineRenderGui() const;
		void SetResolution(uint32_t timeAxisSizeX, uint32_t frequencyAxisSizeY);
		void SetResolution(uint32_t timeAxisSizeX, uint32_t frequencyAxisSizeY, uint32_t shortTimeFFTSize);

		uint32_t GetTimeAxisSizeX() const
		{
			return GetSize().x;
		}

		uint32_t GetFrequencyAxisSizeY() const
		{
			return GetSize().y;
		}

		glm::uvec2 GetSize() const
		{
			return GetSpectrogramTexture().GetCreateInfo().size;
		}

		const BBG::Texture2D& GetSpectrogramTexture() const
		{
			return spectrogramTexture_.value();
		}

	private:
		struct GpuUniforms
		{
			uint32_t spectrogramHeadXAxis;
			uint32_t shortTimeFFTSize;
			float inputScale;
		};

		GpuUniforms gpuUniforms_ = {};

		std::optional<BBG::Texture2D> spectrogramTexture_;
		std::optional<BBG::TypedBuffer<GpuUniforms>> uniformBuffer_;
		std::optional<BBG::TypedBuffer<ComplexType>> spectrogramBuffer_;
		std::optional<BBG::ShaderProgram> renderSpectrogramProgram_;
		std::optional<Rendering::FFT> fft_;

		std::vector<ComplexType> spectrogram_;
		std::vector<ComplexType> bufferedSamples_;
	};
}