#include <BBG/ShaderProgram.h>
#include <vector>

namespace BBG
{
	ShaderProgram::ShaderProgram() noexcept
	{
		handle_ = glCreateProgram();
	}

	ShaderProgram::~ShaderProgram() noexcept
	{
		glDeleteProgram(handle_);
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& old) noexcept
		: handle_(std::exchange(old.handle_, 0))
	{
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& old) noexcept
	{
		if (&old == this)
		{
			return *this;
		}
		this->~ShaderProgram();
		return *(new (this) ShaderProgram(std::move(old)));
	}

	void ShaderProgram::Link(const TypedShader<ShaderType::Vertex>& vertexShader, const TypedShader<ShaderType::Fragment>& fragmentShader) const
	{
		auto shaders = std::to_array({ std::ref((Shader&)vertexShader), std::ref((Shader&)fragmentShader) });
		Link(shaders);
	}
	void ShaderProgram::Link(const TypedShader<ShaderType::Compute>& computeShader) const
	{
		auto shaders = std::to_array({ std::ref((Shader&)computeShader) });
		Link(shaders);
	}

	bool ShaderProgram::HasLinkedSuccessfully() const
	{
		GLint linkStatus;
		glGetProgramiv(handle_, GL_LINK_STATUS, &linkStatus);

		return linkStatus;
	}

	void ShaderProgram::Link(std::span<const std::reference_wrapper<Shader>> shaders) const
	{
		for (const Shader& id : shaders)
		{
			glAttachShader(handle_, id.GetHandle());
		}
		glLinkProgram(handle_);

		auto infoLogBuffer = std::array<char, 4096>({'\0'});
		glGetProgramInfoLog(handle_, infoLogBuffer.size(), nullptr, infoLogBuffer.data());
		printf(infoLogBuffer.data());
		
		for (const Shader& id : shaders)
		{
			glDetachShader(handle_, id.GetHandle());
		}
	}
}