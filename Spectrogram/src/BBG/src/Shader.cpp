#include <BBG/Shader.h>

namespace BBG
{
    Shader::Shader(ShaderType shaderType, std::string_view source) noexcept
        : shaderType_(shaderType)
    {
        handle_ = glCreateShader((GLenum)shaderType);

        const GLchar* strings = source.data();

        glShaderSource(handle_, 1, &strings, nullptr);
        glCompileShader(handle_);

        auto infoLogBuffer = std::array<char, 4096>({ '\0' });
        glGetShaderInfoLog(handle_, infoLogBuffer.size(), nullptr, infoLogBuffer.data());
        printf(infoLogBuffer.data());
    }

    Shader::~Shader()
    {
        glDeleteShader(handle_);
    }

    bool Shader::HasCompiledSuccessfully() const
    {
        GLint compileStatus;
        glGetShaderiv(handle_, GL_COMPILE_STATUS, &compileStatus);

        return compileStatus;
    }
}