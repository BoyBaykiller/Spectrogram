#pragma once
#include <array>
#include <string_view>

#include <BBG/BBG.h>
#include <BBG/private/NonCopyable.h>

namespace BBG
{
	class Shader
	{
	public:
		explicit Shader(ShaderType shaderType, std::string_view source) noexcept;
		~Shader() noexcept;
		BBG_NON_COPYABLE(Shader);

		bool HasCompiledSuccessfully() const;

		ShaderType GetType() const
		{
			return shaderType_;
		}

		uint32_t GetHandle() const
		{
			return handle_;
		}

	private:
		uint32_t handle_ = 0;
		ShaderType shaderType_;
	};

	template<ShaderType TShaderType>
	class TypedShader : public Shader
	{
	public:
		explicit TypedShader(std::string_view source) noexcept
			: Shader(TShaderType, source)
		{
		}
	};
}
