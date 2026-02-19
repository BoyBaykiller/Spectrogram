#include <BBG/Buffer.h>

namespace BBG
{
    Buffer::Buffer(MemLocation memLocation, MemAccess memAccess, size_t size, const void* data) noexcept
        : memLocation_(memLocation), memAccess_(memAccess), bufferSize_(size)
    {
        glCreateBuffers(1, &handle_);
        glNamedBufferStorage(handle_, size, data, (GLbitfield)memLocation | (GLbitfield)memAccess);
    }
    Buffer::~Buffer() noexcept
    {
        glDeleteBuffers(1, &handle_);
    }

    Buffer::Buffer(Buffer&& old) noexcept
        : handle_(std::exchange(old.handle_, 0)),
        bufferSize_(std::exchange(old.bufferSize_, 0)),
        memLocation_(std::exchange(old.memLocation_, (BBG::MemLocation)0)),
        memAccess_(std::exchange(old.memAccess_, (BBG::MemAccess)0))
    {
    }

    Buffer& Buffer::operator=(Buffer&& old) noexcept
    {
        if (&old == this)
        {
            return *this;
        }
        this->~Buffer();
        return *(new (this) Buffer(std::move(old)));
    }

    void Buffer::UploadData(size_t offset, size_t size, const void* data) const
    {
        glNamedBufferSubData(handle_, offset, size, data);
    }

    void Buffer::DownloadData(size_t offset, size_t size, void* data) const
    {
        glGetNamedBufferSubData(handle_, offset, size, data);
    }

    void Buffer::SimpleClear(const void* data) const
    {
        Clear(
            ClearInfo{ 
                .internalFormat = BBG::InternalPixelFormat::R32_Float,
                .format = BBG::UploadPixelFormat::R,
                .type = BBG::UploadPixelType::Float,
            },
            data
        );
    }

    void Buffer::Clear(ClearInfo info, const void* data) const
    {
        auto derivedSize = info.size;
        if (derivedSize == WHOLE_BUFFER)
        {
            derivedSize = bufferSize_;
        }

        glClearNamedBufferSubData(handle_, (GLenum)info.internalFormat,	info.offset, derivedSize, (GLenum)info.format, (GLenum)info.type, data);
    }
}