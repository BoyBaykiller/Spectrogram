#include <SNT/PlayDevice.h>

namespace SNT
{
	PlayDevice::PlayDevice() noexcept
	{
		handle_ = alcOpenDevice(nullptr);
		assert(handle_ && "Play device could not be opened.");
	}

	PlayDevice::~PlayDevice() noexcept
	{
		bool success = alcCloseDevice(handle_);
		assert(success && "Failed to close play device.");
	}
}
