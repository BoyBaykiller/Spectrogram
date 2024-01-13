#pragma once
#include <span>

#include <BBG/BBG.h>
#include <BBG/Shader.h>

namespace BBG
{
	class ShaderProgram
	{
	public:
		explicit ShaderProgram() noexcept;
		~ShaderProgram() noexcept;
		ShaderProgram(ShaderProgram&& old) noexcept;
		ShaderProgram& operator=(ShaderProgram&& old) noexcept;
		BBG_NON_COPYABLE(ShaderProgram)

		void Link(const TypedShader<ShaderType::Vertex>& vertexShader, const TypedShader<ShaderType::Fragment>& fragmentShader) const;
		void Link(const TypedShader<ShaderType::Compute>& computeShader) const;
		bool HasLinkedSuccessfully() const;

		uint32_t GetHandle() const
		{
			return handle_;
		}

	private:
		void Link(std::span<const std::reference_wrapper<Shader>> shaders) const;

		uint32_t handle_ = 0;
	};
}