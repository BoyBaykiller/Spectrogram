#pragma once
#include <span>

#include <BBG/BBG.h>

namespace BBG
{
    class Buffer
    {
    public:

        static constexpr size_t WHOLE_BUFFER = SIZE_MAX;

        struct ClearInfo
        {
            InternalPixelFormat internalFormat = {};
            UploadPixelFormat format = {};
            UploadPixelType type = {};
            size_t size = WHOLE_BUFFER;
            uint32_t offset = 0;
        };

    public:
        explicit Buffer(BufferStorageFlag flags, size_t size, const void* data = nullptr) noexcept;
        ~Buffer() noexcept;
        Buffer(Buffer&& old) noexcept;
        Buffer& operator=(Buffer&& old) noexcept;
        BBG_NON_COPYABLE(Buffer);

        void UploadData(size_t offset, size_t size, const void* data) const;
        void DownloadData(size_t offset, size_t size, void* data) const;
        void SimpleClear(const void* data) const;
        void Clear(ClearInfo clearInfo, const void* data) const;

        void* GetMappedMemory() const
        {
            return mappedMemory_;
        }

        BufferStorageFlag GetBufferStorageFlags() const
        {
            return bufferStorageFlags_;
        }

        size_t GetSize() const
        {
            return bufferSize_;
        }

        uint32_t GetHandle() const
        {
            return handle_;
        }


    private:
        uint32_t handle_ = 0;
        size_t bufferSize_;
        BufferStorageFlag bufferStorageFlags_;
        void* mappedMemory_;
    };

    template<typename T>
    class TypedBuffer : public Buffer
    {
    public:
        explicit TypedBuffer(BufferStorageFlag flags, const T& data) noexcept
            : TypedBuffer(flags, 1, &data)
        {
        }

        explicit TypedBuffer(BufferStorageFlag flags, std::span<const T> data) noexcept
            : TypedBuffer(flags, data.size(), data.data())
        {
        }

        explicit TypedBuffer(BufferStorageFlag flags, size_t count, const T* data = nullptr) noexcept
            : Buffer(flags, sizeof(T) * count, data)
        {
        }

        void UploadElements(const T& element, size_t startIndex = 0) const
        {
            UploadElements(startIndex, 1, &element);
        }

        void UploadElements(size_t startIndex, std::span<const T> data) const
        {
            UploadElements(startIndex, data.size(), data.data());
        }

        void UploadElements(size_t startIndex, size_t count, const T* data) const
        {
            Buffer::UploadData(sizeof(T) * startIndex, sizeof(T) * count, data);
        }

        void DownloadElements(size_t startIndex, std::span<T> data) const
        {
            DownloadElements(startIndex, data.size(), data.data());
        }

        void DownloadElements(size_t startIndex, size_t count, T* data) const
        {
            Buffer::DownloadData(sizeof(T) * startIndex, sizeof(T) * count, data);
        }


        size_t GetNumElements() const
        {
            return GetSize() / sizeof(T);
        }

        T* GetMappedMemory() const
        {
            return (T*)Buffer::GetMappedMemory();
        }
    };
}