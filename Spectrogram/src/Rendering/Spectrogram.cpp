#include <numbers>
#include <numeric>
#include <algorithm>

#include <imgui.h>
#include <glm/glm.hpp>

#include <BBG/Rendering.h>
#include <BBG/TimerQuery.h>

#include "../Logger.h"
#include "../Timer.h"
#include "../LoadFile.h"
#include "../FFT/FFT.h"
#include "Spectrogram.h"

namespace Rendering
{
    Spectrogram::Spectrogram(uint32_t timeAxisSizeX, uint32_t frequencyAxisSizeY, uint32_t shortTimeFFTSize) noexcept
    {
        uniformBuffer_ = BBG::TypedBuffer<GpuUniforms>(BBG::BufferStorageFlag::DynamicStorage, 1);
        renderSpectrogramProgram_ = BBG::ShaderProgram();
        renderSpectrogramProgram_.value().Link(
            BBG::TypedShader<BBG::ShaderType::Compute>(LoadFileWithIncludes("res/shaders/Spectrogram/compute.glsl"))
        );

        fft_ = FFT();

        gpuUniforms_.inputScale = 0.81f; // arbitrary default scale that works for me

        SetResolution(timeAxisSizeX, frequencyAxisSizeY, shortTimeFFTSize);
    }

    void Spectrogram::AddSamples(std::span<const ComplexType> newSamples)
    {
        bufferedSamples_.insert(bufferedSamples_.end(), newSamples.begin(), newSamples.end());
        if (bufferedSamples_.size() < gpuUniforms_.shortTimeFFTSize)
        {
            return;
        }

        auto inputData = BBG::TypedBuffer<FFT::ComplexType>(BBG::BufferStorageFlag::None, gpuUniforms_.shortTimeFFTSize, bufferedSamples_.data());
        const auto& outputData = spectrogramBuffer_.value();
        fft_.value().ComputeFFT(
            inputData, 0,
            outputData, gpuUniforms_.spectrogramHeadXAxis * gpuUniforms_.shortTimeFFTSize,
            gpuUniforms_.shortTimeFFTSize
        );
        BBG::Rendering::MemoryBarrier(BBG::MemoryBarrierFlags::ShaderStorage);

        gpuUniforms_.spectrogramHeadXAxis = (gpuUniforms_.spectrogramHeadXAxis + 1) % GetSize().x;
        bufferedSamples_.erase(bufferedSamples_.begin(), bufferedSamples_.begin() + gpuUniforms_.shortTimeFFTSize);
    }

    void Spectrogram::RenderSpectrogram() const
    {
        uniformBuffer_.value().UploadElements(gpuUniforms_);

        BBG::Rendering::SetUniformBlock(uniformBuffer_.value(), 0);
        BBG::Rendering::SetShaderStorageBlock(spectrogramBuffer_.value(), 0);
        BBG::Rendering::BindImage(spectrogramTexture_.value(), BBG::ImageFormat::R8G8B8A8_Unorm, 0);
        BBG::Rendering::BindShaderProgram(renderSpectrogramProgram_.value());
        BBG::Rendering::DispatchThreads({ spectrogramTexture_.value().GetCreateInfo().size, 1 }, { 8, 8, 1 });
    }

    void Spectrogram::InlineRenderGui() const
    {
        // Spectrogram Output
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        static bool isOpenWaveform = true;
        ImGui::Begin("Spectrogram", &isOpenWaveform, ImGuiWindowFlags_NoDecoration);
        {
            auto contentRegionAvail = ImGui::GetContentRegionAvail();
            ImGui::Image((void*)spectrogramTexture_.value().GetHandle(), contentRegionAvail, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        }
        ImGui::End();
        ImGui::PopStyleVar();

        // Spectrogram Settings
        if (ImGui::Begin("Spectrogram Settings"))
        {
            ImGui::SliderFloat("Input Scale", (float*)&gpuUniforms_.inputScale, 0.2f, 1.0);
        }
        ImGui::End();
    }

    void Spectrogram::SetResolution(uint32_t timeAxisSizeX, uint32_t frequencyAxisSizeY)
    {
        SetResolution(timeAxisSizeX, frequencyAxisSizeY, gpuUniforms_.shortTimeFFTSize);
    }

    void Spectrogram::SetResolution(uint32_t timeAxisSizeX, uint32_t frequencyAxisSizeY, uint32_t shortTimeFFTSize)
    {
        gpuUniforms_.shortTimeFFTSize = shortTimeFFTSize;
        spectrogram_ = std::vector<ComplexType>(timeAxisSizeX * shortTimeFFTSize);
        spectrogramBuffer_ = BBG::TypedBuffer<ComplexType>(BBG::BufferStorageFlag::DynamicStorage, spectrogram_.size());

        spectrogramTexture_ = BBG::Texture2D({ .size = { timeAxisSizeX, frequencyAxisSizeY }, .format = BBG::InternalPixelFormat::R8G8B8A8_Unorm });
        

        // Zero ressources
        const float clearValue = 0.0f;
        spectrogramBuffer_.value().SimpleClear(&clearValue);
        spectrogramTexture_.value().ClearPixels(
            BBG::Texture2D::ClearInfo{
                .format = BBG::UploadPixelFormat::Rgba,
                .type = BBG::UploadPixelType::Float,
            },
            &clearValue
        );
    }
}