#include <cmath>

#include <BBG/Rendering.h>

#include "../LoadFile.h"

#include "FFT.h"

namespace Rendering
{
	FFT::FFT() noexcept
	{
		uniformBuffer = BBG::TypedBuffer<GpuUniforms>(BBG::BufferStorageFlag::DynamicStorage, 1);

		reorderStageProgram = BBG::ShaderProgram();
		reorderStageProgram.value().Link(
			BBG::TypedShader<BBG::ShaderType::Compute>(LoadFileWithIncludes("res/shaders/FFT/ReorderStage.compute.glsl"))
		);

		generalStageProgram = BBG::ShaderProgram();
		generalStageProgram.value().Link(
			BBG::TypedShader<BBG::ShaderType::Compute>(LoadFileWithIncludes("res/shaders/FFT/GeneralStage.compute.glsl"))
		);
	}

	void FFT::ComputeFFT(const BBG::TypedBuffer<ComplexType>& complexInput, uint64_t inputElementsOffset, const BBG::TypedBuffer<ComplexType>& complexOutput, uint64_t outputElementsOffset, uint32_t fftSize) const
	{
		bool isPowerOfTwo = (fftSize != 0) && ((fftSize & (fftSize - 1)) == 0);
		assert(isPowerOfTwo && "FFT size must be a power of 2");

		BBG::Rendering::SetUniformBlock(uniformBuffer.value(), 0);
		BBG::Rendering::SetShaderStorageBlock(complexInput, 0, inputElementsOffset * sizeof(FFT::ComplexType), fftSize * sizeof(FFT::ComplexType));
		BBG::Rendering::SetShaderStorageBlock(complexOutput, 1, outputElementsOffset * sizeof(FFT::ComplexType), fftSize * sizeof(FFT::ComplexType));

		GpuUniforms uniforms = {
			.log2FftSize = (uint32_t)std::log2(fftSize),
			.halfDftSize = fftSize / 2,
			.numDfts = fftSize / 2,
		};
		uniformBuffer.value().UploadElements(uniforms);

		BBG::Rendering::BindShaderProgram(reorderStageProgram.value());
		BBG::Rendering::DispatchThreads({ (fftSize / 2), 1, 1 }, { 64, 1, 1 });

		BBG::Rendering::BindShaderProgram(generalStageProgram.value());
		for (int i = 2; i <= uniforms.log2FftSize; i++)
		{
			uint32_t dftSize = 1 << i;
			uint32_t numDFTs = fftSize / dftSize;

			uniforms.halfDftSize = dftSize / 2;
			uniforms.numDfts = numDFTs;
			uniformBuffer.value().UploadElements(uniforms);

			BBG::Rendering::MemoryBarrier(BBG::MemoryBarrierFlags::ShaderStorage);
			BBG::Rendering::DispatchThreads({ (fftSize / 2) , 1, 1 }, { 64, 1, 1 });
		}
	}
}