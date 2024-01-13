#include <BBG/Sampler.h>

namespace BBG
{
    Sampler::Sampler(const State& state) noexcept
    {
        glCreateSamplers(1, &handle_);
        SetFilter(state.minFilter, state.magFilter);
        SetWrapMode(state.addressModeU, state.addressModeV, state.addressModeW);
    }

    Sampler::~Sampler() noexcept
    {
        glDeleteSamplers(1, &handle_);
    }

    void Sampler::SetFilter(FilterMode minFilter, FilterMode magFilter)
    {
        glSamplerParameteri(handle_, GL_TEXTURE_MIN_FILTER, (GLint)minFilter);
        glSamplerParameteri(handle_, GL_TEXTURE_MAG_FILTER, (GLint)magFilter);
        state_.minFilter = minFilter;
        state_.magFilter = magFilter;
    }

    void Sampler::SetWrapMode(AddressMode addressModeU, AddressMode addressModeV, AddressMode addressModeW)
    {
        glSamplerParameteri(handle_, GL_TEXTURE_WRAP_S, (GLint)addressModeU);
        glSamplerParameteri(handle_, GL_TEXTURE_WRAP_T, (GLint)addressModeV);
        glSamplerParameteri(handle_, GL_TEXTURE_WRAP_R, (GLint)addressModeW);
        state_.addressModeU = addressModeU;
        state_.addressModeV = addressModeV;
        state_.addressModeW = addressModeW;
    }
}