#include <SNT/Context/Buffer.h>

namespace SNT::Context
{
    Buffer::Buffer() noexcept
    {
        alGenBuffers(1, &handle_);
    }

    Buffer::~Buffer() noexcept
    {
        alDeleteBuffers(1, &handle_);
    }

    void Buffer::AllocateData(AudioFormat format, void* data, int32_t size, uint32_t frequency)
    {
        alBufferData(handle_, (ALCenum)format, data, size, frequency);
    }
}
