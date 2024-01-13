#pragma once
#include <span>

#include <SNT/SNT.h>
#include <SNT/private/NonCopyable.h>

namespace SNT::Context
{
	class Buffer
	{
	public:
		explicit Buffer() noexcept;
		~Buffer() noexcept;
		SNT_NON_COPYABLE(Buffer);
	
		void AllocateData(AudioFormat format, void* data, int32_t size, uint32_t frequency);

		uint32_t GetHandle() const
		{
			return handle_;
		}

	private:
		uint32_t handle_ = 0;
	};

	template<AudioFormat TFormat>
	class TypedBuffer : public Buffer
	{
	public:
		explicit TypedBuffer()
			: Buffer()
		{
		}

		void AllocateData(std::span<AudioSample<TFormat>> data, uint32_t frequency)
		{
			Buffer::AllocateData(TFormat, data.data(), data.size_bytes(), frequency);
		}
	};
}