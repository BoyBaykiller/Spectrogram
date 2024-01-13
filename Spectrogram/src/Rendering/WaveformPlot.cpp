#include <numbers>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <BBG/Rendering.h>

#include "../LoadFile.h"
#include "../Logger.h"

#include "WaveformPlot.h"

namespace Rendering
{
	WaveformPlot::WaveformPlot(uint32_t width, uint32_t height, uint32_t downsampleCount) noexcept
	{
		gpuUniforms_.orthoProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
		uniformBuffer_ = BBG::TypedBuffer<GpuUniforms>(BBG::BufferStorageFlag::DynamicStorage, gpuUniforms_);

		renderWaveformProgram_ = BBG::ShaderProgram();
		renderWaveformProgram_.value().Link(
			BBG::TypedShader<BBG::ShaderType::Vertex>(LoadFile("res/shaders/Waveform/vertex.glsl")),
			BBG::TypedShader<BBG::ShaderType::Fragment>(LoadFile("res/shaders/Waveform/fragment.glsl"))
		);

		SetResolution(width, height);

		waveformDownsampleWeights_ = std::vector<float>(downsampleCount); // prefer odd number so the distribution is perfectly symmetrical
		{
			// Gaussian Distribution

			float stdDev = std::sqrtf((waveformDownsampleWeights_.size() - 1.0f) / 4.0f / std::logf(2.0f));
			float constTerm = 1.0f / (stdDev * std::sqrtf(2.0f * std::numbers::pi));
			float sum = 0.0f;
			for (int i = 0; i < waveformDownsampleWeights_.size(); i++)
			{
				int32_t x = -((int32_t)waveformDownsampleWeights_.size() / 2) + i;
				waveformDownsampleWeights_[i] = constTerm * (1.0f / (std::expf((x * x) / (2.0f * stdDev * stdDev))));
				sum += waveformDownsampleWeights_[i];
			}

			for (int i = 0; i < waveformDownsampleWeights_.size(); i++)
			{
				waveformDownsampleWeights_[i] /= sum;
			}

			if (waveformDownsampleWeights_.size() == 1)
			{
				waveformDownsampleWeights_[0] = 1.0f;
			}
		}
	}

	void WaveformPlot::AddSamples(std::span<const AudioSample> newSamples)
	{
		bufferedSamples_.insert(bufferedSamples_.end(), newSamples.begin(), newSamples.end());
		uint32_t numNewDownsampledSamples = bufferedSamples_.size() / waveformDownsampleWeights_.size();
		if (numNewDownsampledSamples == 0)
		{
			return;
		}
		
		for (int i = 0; i < numNewDownsampledSamples; i++)
		{
			float downsampledAudioSample = 0.0;
			for (int j = 0; j < waveformDownsampleWeights_.size(); j++)
			{
				float weight = waveformDownsampleWeights_[j];
				downsampledAudioSample += bufferedSamples_[i * waveformDownsampleWeights_.size() + j].channels[0] * weight;
			}

			AudioSample sample = { downsampledAudioSample };
			ringBufferAudioSamples_.value().UploadElements((gpuUniforms_.ringBufferHead + i) % (uint32_t)ringBufferAudioSamples_.value().GetNumElements(), 1, &sample);
		}
		gpuUniforms_.ringBufferHead = (gpuUniforms_.ringBufferHead + numNewDownsampledSamples) % (uint32_t)ringBufferAudioSamples_.value().GetNumElements();

		bufferedSamples_.erase(bufferedSamples_.begin(), bufferedSamples_.begin() + numNewDownsampledSamples * waveformDownsampleWeights_.size());
	}

	void WaveformPlot::RenderWaveform() const
	{
		uniformBuffer_.value().UploadElements(gpuUniforms_);

		BBG::Rendering::SetViewport({ GetTimeAxisSize(), GetAmplitudeAxisSize() });
		BBG::Rendering::ColorAttachment attachments[] = {
			BBG::Rendering::ColorAttachment{
				.texture = waveformTexture_.value(),
				.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f },
				.loadOp = BBG::Rendering::AttachmentLoadOp::Clear
			}
		};
		BBG::Rendering::SetColorAttachments(attachments);
		BBG::Rendering::SetUniformBlock(uniformBuffer_.value(), 0);
		BBG::Rendering::SetShaderStorageBlock(ringBufferAudioSamples_.value(), 0);
		BBG::Rendering::BindImage(waveformTexture_.value(), BBG::ImageFormat::R8G8B8A8_Unorm, 0);
		BBG::Rendering::BindShaderProgram(renderWaveformProgram_.value());

		BBG::Rendering::DrawNonIndexed(BBG::Topology::LineStrip, 0, waveformTexture_.value().GetCreateInfo().size.x, 1, 0);
	}

	void WaveformPlot::InlineRenderGui() const
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

		static bool isOpenFrequencySpecturm = true;
		ImGui::Begin("Waveform", &isOpenFrequencySpecturm, ImGuiWindowFlags_NoDecoration);
		{
			auto contentRegionAvail = ImGui::GetContentRegionAvail();
			ImGui::Image((void*)waveformTexture_.value().GetHandle(), contentRegionAvail, { 0.0f, 1.0f }, { 1.0f, 0.0f });
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void WaveformPlot::SetResolution(uint32_t width, uint32_t height)
	{
		waveformTexture_ = BBG::Texture2D({ .size = { width, height }, .format = BBG::InternalPixelFormat::R8G8B8A8_Unorm });

		ringBufferAudioSamples_ = BBG::TypedBuffer<AudioSample>(BBG::BufferStorageFlag::DynamicStorage, width);
	}
}
