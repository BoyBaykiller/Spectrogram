#include <SNT/CaptureDevice.h>
#include <SNT/private/NonCopyable.h>

namespace SNT
{
	CaptureDevice::CaptureDevice(std::string_view name, AudioFormat format, uint32_t samplingRate, uint32_t minInternalBufferSampleCount) noexcept
		: format_(format), samplingRate_(samplingRate)
	{
		handle_ = alcCaptureOpenDevice(name.data(), samplingRate, (ALCenum)format, minInternalBufferSampleCount);
		assert(handle_ && "Capture device could not be opened.");
	}

	CaptureDevice::~CaptureDevice() noexcept
	{
		if (handle_ == nullptr)
		{
			return;
		}
		bool success = alcCaptureCloseDevice(handle_);
		assert(success && "Failed to close capture device.");
	}

	CaptureDevice::CaptureDevice(CaptureDevice&& old) noexcept
		: handle_(std::exchange(old.handle_, nullptr)),
		format_(std::exchange(old.format_, (AudioFormat)0)),
		samplingRate_(std::exchange(old.samplingRate_, 0))
	{

	}

	CaptureDevice& CaptureDevice::operator=(CaptureDevice&& old) noexcept
	{
		if (&old == this)
		{
			return *this;
		}
		this->~CaptureDevice();
		return *(new (this) CaptureDevice(std::move(old)));
	}

	void CaptureDevice::StartCapture() const
	{
		alcCaptureStart(handle_);
	}

	void CaptureDevice::EndCapture() const
	{
		alcCaptureStop(handle_);
	}

	void CaptureDevice::CaptureSamples(void* samples, uint32_t sampleCount) const
	{
		alcCaptureSamples(handle_, samples, sampleCount);
	}

	uint32_t CaptureDevice::AvailableSamples() const
	{
		ALCint samplesAvailable;
		alcGetIntegerv(handle_, ALC_CAPTURE_SAMPLES, 1, &samplesAvailable);
		return samplesAvailable;
	}

	const char* CaptureDevice::GetName()
	{
		const char* name = alcGetString(handle_, ALC_CAPTURE_DEVICE_SPECIFIER);
		return name;
	}
}
