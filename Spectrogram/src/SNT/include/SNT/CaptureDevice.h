#pragma once
#include <span>

#include <SNT/SNT.h>
#include <SNT/private/NonCopyable.h>

namespace SNT
{
    class CaptureDevice
    {
    public:
        explicit CaptureDevice(std::string_view name, AudioFormat format, uint32_t samplingRate, uint32_t minInternalBufferSampleCount = 10000) noexcept;
        ~CaptureDevice() noexcept;
        CaptureDevice(CaptureDevice&& old) noexcept;
        CaptureDevice& operator=(CaptureDevice&& old) noexcept;
        SNT_NON_COPYABLE(CaptureDevice)

        void StartCapture() const;
        void EndCapture() const;
        void CaptureSamples(void* samples, uint32_t sampleCount) const;
        uint32_t AvailableSamples() const;

        const char* GetName();

        uint32_t GetSamplingRate() const
        {
            return samplingRate_;
        }

        AudioFormat GetFormat() const
        {
            return format_;
        }


        const ALCdevice* GetHandle() const
        {
            return handle_;
        }

    private:
        ALCdevice* handle_ = nullptr;
        AudioFormat format_;
        uint32_t samplingRate_;
    };

    template<AudioFormat TFormat>
    class TypedCaptureDevice : public CaptureDevice
    {
    public:
        using AudioSample = AudioSample<TFormat>;

    public:
        explicit TypedCaptureDevice(std::string_view name, uint32_t frequency) noexcept
            : CaptureDevice(name, TFormat, frequency)
        {
        }

        void CaptureSamples(AudioSample* samples, uint32_t sampleCount) const
        {
            CaptureDevice::CaptureSamples(samples, sampleCount);
        }

        void CaptureSamples(std::span<AudioSample> samples) const
        {
            CaptureDevice::CaptureSamples(samples.data(), samples.size());
        }
    };
}
