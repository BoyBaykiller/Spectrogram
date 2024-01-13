#include <BBG/Buffer.h>

namespace BBG
{
	Buffer::Buffer(BufferStorageFlag flags, size_t size, const void* data) noexcept
		: bufferSize_(size), bufferStorageFlags_(flags)
	{
		glCreateBuffers(1, &handle_);
		glNamedBufferStorage(handle_, size, data, (GLbitfield)flags);
		
		mappedMemory_ = nullptr;
		if (flags == BufferStorageFlag::MappedStorage)
		{
			mappedMemory_ = glMapNamedBufferRange(handle_, 0, size, (GLbitfield)flags);
		}
	}
	Buffer::~Buffer() noexcept
	{
		if (mappedMemory_ != nullptr)
		{
			glUnmapNamedBuffer(handle_);
		}
		glDeleteBuffers(1, &handle_);
	}

	Buffer::Buffer(Buffer&& old) noexcept
		: handle_(std::exchange(old.handle_, 0)),
		bufferSize_(std::exchange(old.bufferSize_, 0)),
		bufferStorageFlags_(std::exchange(old.bufferStorageFlags_, BufferStorageFlag::None)),
		mappedMemory_(std::exchange(old.mappedMemory_, nullptr))
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