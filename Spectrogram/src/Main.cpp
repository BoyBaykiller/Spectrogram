#include <iostream>
#include <vector>
#include <memory>
#include <numbers>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <SNT/CaptureDevice.h>
#include <SNT/PlayDevice.h>

#include <BBG/BBG.h>
#include <BBG/Buffer.h>
#include <BBG/Texture2D.h>
#include <BBG/ShaderProgram.h>
#include <BBG/Rendering.h>
#include <BBG/TimerQuery.h>

#include "LoadFile.h"
#include "Timer.h"
#include "Random.h"
#include "Logger.h"
#include "Rendering/WaveformPlot.h"
#include "Rendering/Spectrogram.h"


static constexpr uint32_t SHORT_TIME_FFT_SIZE = 4096; // Rayleigh Frequency: (1 / SHORT_TIME_FFT_SIZE) * CaptureFrequency
static uint32_t CaptureFrequency = 88200; // Nyquist Frequency: CaptureFrequency / 2
static uint32_t TimeHistoryLengthInSec = 10;
static uint32_t MaxFrequency = 4000; // Frequencies >= CaptureFrequency will result in blank areas on the spectrogramPlot as they can not be captured

static constexpr auto AudioFormat = SNT::AudioFormat::MonoFloat32;
using AudioSample = SNT::AudioSample<AudioFormat>;

static glm::uvec2 CalcSpectrogramSize(uint32_t historyLength, uint32_t maxFrequency, uint32_t captureFrequency, uint32_t fftSize)
{
	uint32_t spectrogramTimeAxisSize = std::ceilf(historyLength / ((float)fftSize / captureFrequency));
	uint32_t spectrogramFrequencyAxisSize = std::ceilf(maxFrequency / (float)captureFrequency * fftSize);

	return glm::uvec2(spectrogramTimeAxisSize, spectrogramFrequencyAxisSize);
}

static uint32_t renderWidth = 1600;
static uint32_t renderHeight = 900;
static bool windowFocused = true;
static std::optional<Rendering::WaveformPlot> waveformPlot;
static std::optional<Rendering::Spectrogram> spectrogramPlot;
static void ResizeRessources(uint32_t width, uint32_t height)
{
	if (!windowFocused)
	{
		return;
	}
	if (width == 0 || height == 0)
	{
		return;
	}

	renderWidth = width;
	renderHeight = height;

	if (!waveformPlot.has_value())
	{
		waveformPlot = Rendering::WaveformPlot(renderWidth / 2, renderHeight, 31);
	}
	else
	{
		waveformPlot.value().SetResolution(renderWidth / 2, renderHeight);
	}

	auto size = CalcSpectrogramSize(TimeHistoryLengthInSec, MaxFrequency, CaptureFrequency, SHORT_TIME_FFT_SIZE);
	if (!spectrogramPlot.has_value())
	{
		spectrogramPlot = Rendering::Spectrogram(size.x, size.y, SHORT_TIME_FFT_SIZE);
	}
	else
	{
		spectrogramPlot.value().SetResolution(size.x, size.y);
	}
}

static void GLAPIENTRY GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (id == 131185) return; // NVIDIA, Buffer detailed info

	Logger::Log(message);
}

int main()
{
	GLFWwindow* window;
	{
		constexpr uint32_t OPENGL_MAJOR_VERSION = 4;
		constexpr uint32_t OPENGL_MINOR_VERSION = 6;

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_COMPAT_PROFILE
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		window = glfwCreateWindow(renderWidth, renderHeight, "Spectrogram", nullptr, nullptr);

		if (window == nullptr)
		{
			Logger::Log("Window creation failed. Make sure you have OpenGL {}.{} support. Press Enter to exit", OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION);
			std::cin.get();
			return 0;
		}

		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				renderWidth = width;
				renderHeight = height;
				ResizeRessources(renderWidth, renderHeight);
			}
		);

		glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused)
			{
				windowFocused = focused;
			}
		);

		glfwMakeContextCurrent(window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		glfwSwapInterval(1);

		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 460 core");
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigDockingWithShift = true;
		}
	}

	// TODO: This should be part of BBG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLDebugCallback, 0);
	BBG::Initialize();

	ResizeRessources(renderWidth, renderHeight);

	auto captureDevice = SNT::TypedCaptureDevice<AudioFormat>(SNT::GetDefaultCaptureDeviceName(), CaptureFrequency);
	captureDevice.StartCapture();
	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, 1);
		}

		{
			auto newSamples = std::vector<AudioSample>(captureDevice.AvailableSamples());
			captureDevice.CaptureSamples(newSamples);

			auto complexInput = std::vector<Rendering::Spectrogram::ComplexType>(newSamples.size());
			for (int i = 0; i < complexInput.size(); i++)
			{
				complexInput[i] = Rendering::Spectrogram::ComplexType(newSamples[i].channels[0], 0.0f);
			}

			waveformPlot.value().AddSamples(newSamples);
			spectrogramPlot.value().AddSamples(complexInput);
		}

		{
			waveformPlot.value().RenderWaveform();
			spectrogramPlot.value().RenderSpectrogram();

			BBG::Rendering::ScreenColorAttachment colorAttachment {
				.clearColor = std::to_array({ 0.0f, 0.0f, 0.0f, 0.0f }),
			};
			BBG::Rendering::SetViewport({ renderWidth, renderHeight });
			BBG::Rendering::SetColorAttachments(colorAttachment);
			
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				ImGui::DockSpaceOverViewport();

				waveformPlot.value().InlineRenderGui();
				spectrogramPlot.value().InlineRenderGui();

				if (ImGui::Begin("Settings"))
				{
					ImGui::PushItemWidth(ImGui::CalcTextSize(captureDevice.GetName()).x);
					if (ImGui::BeginCombo("###0", captureDevice.GetName()))
					{
						static auto captureDeviceNames = SNT::GetAllCaptureDeviceNames();
						for (const auto& it : captureDeviceNames)
						{
							bool isSelected = captureDevice.GetName() == it.c_str();
							if (ImGui::Selectable(it.c_str(), isSelected))
							{
								captureDevice = SNT::TypedCaptureDevice<AudioFormat>(it, CaptureFrequency);
								captureDevice.StartCapture();
							}

							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();

				
					uint32_t nyquistFreq = CaptureFrequency / 2;
					ImGui::SliderInt("History in Sec", (int*)&TimeHistoryLengthInSec, 1, 20);
					ImGui::SliderInt("Max Frequency", (int*)&MaxFrequency, 1, nyquistFreq);

					ImGui::BeginDisabled();
					// TODO: Implement
					ImGui::SliderInt("FFT Size", (int*)&SHORT_TIME_FFT_SIZE, 4, 16384);

					ImGui::EndDisabled();

					auto curSize = spectrogramPlot.value().GetSpectrogramTexture().GetCreateInfo().size;
					auto size = CalcSpectrogramSize(TimeHistoryLengthInSec, MaxFrequency, CaptureFrequency, SHORT_TIME_FFT_SIZE);
					if (size.x != curSize.x || size.y != curSize.y)
					{
						spectrogramPlot.value().SetResolution(size.x, size.y, SHORT_TIME_FFT_SIZE);
					}

					if (ImGui::InputInt("Frequency", (int*)&CaptureFrequency, 1000))
					{
						captureDevice = SNT::TypedCaptureDevice<AudioFormat>(captureDevice.GetName(), CaptureFrequency);
						captureDevice.StartCapture();
					}

				}
				ImGui::End();

				//ImGui::ShowDemoWindow();

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	captureDevice.EndCapture();
}
