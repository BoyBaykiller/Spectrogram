#pragma once
#include <optional>

#include <SNT/SNT.h>

#include <BBG/Buffer.h>
#include <BBG/ShaderProgram.h>
#include <BBG/Texture2D.h>

namespace Rendering
{
	class WaveformPlot
	{
	public:
		using AudioSample = SNT::AudioSample<SNT::AudioFormat::MonoFloat32>;

	public:
		explicit WaveformPlot(uint32_t width, uint32_t height, uint32_t downsampleCount) noexcept;

		void AddSamples(std::span<const AudioSample> newSamples);
		void RenderWaveform() const;
		void InlineRenderGui() const;
		void SetResolution(uint32_t width, uint32_t height);

		uint32_t GetTimeAxisSize() const
		{
			return waveformTexture_.value().GetCreateInfo().size.x;
		}

		uint32_t GetAmplitudeAxisSize() const
		{
			return waveformTexture_.value().GetCreateInfo().size.y;
		}

		glm::uvec2 GetSize() const
		{
			auto size = glm::uvec2(GetTimeAxisSize(), GetAmplitudeAxisSize());
			return size;
		}

		const BBG::Texture2D& GetWaveformGraph() const
		{
			return waveformTexture_.value();
		}

	private:
		struct GpuUniforms
		{
			glm::mat4 orthoProjection;
			uint32_t ringBufferHead;
		};

		GpuUniforms gpuUniforms_ = {};

		std::optional<BBG::Texture2D> waveformTexture_;
		std::optional<BBG::TypedBuffer<GpuUniforms>> uniformBuffer_;
		std::optional<BBG::TypedBuffer<AudioSample>> ringBufferAudioSamples_;
		std::optional<BBG::ShaderProgram> renderWaveformProgram_;

		std::vector<AudioSample> bufferedSamples_;
		std::vector<float> waveformDownsampleWeights_;
	};
}