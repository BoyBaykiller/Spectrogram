#pragma once
#include <cstdint>
#include <cassert>
#include <array>
#include <string>
#include <vector>

#define AL_LIBTYPE_STATIC
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h> // AL_EXT_float32

namespace SNT
{
    enum class ErrorCode
    {
        NoError = AL_NO_ERROR,
        InvalidName = AL_INVALID_NAME,
        InvalidEnum = AL_INVALID_ENUM,
        InvalidValue = AL_INVALID_VALUE,
        InvalidOperation = AL_INVALID_OPERATION,
        OutOfMemory = AL_OUT_OF_MEMORY,
    };

    enum class AudioFormat
    {
        MonoSigned16 = AL_FORMAT_MONO16,
        StereoSigned16 = AL_FORMAT_STEREO16,

        // Requires support for AL_EXT_float32
        MonoFloat32 = AL_FORMAT_MONO_FLOAT32,

        // Requires support for AL_EXT_float32
        StereoFloat32 = AL_FORMAT_STEREO_FLOAT32,
    };

    constexpr uint32_t FormatNumChannels(AudioFormat format)
    {
        switch (format)
        {
        case AudioFormat::StereoSigned16:
        case AudioFormat::StereoFloat32:
            return 2;

        case AudioFormat::MonoSigned16:
        case AudioFormat::MonoFloat32:
            return 1;

        default:
            assert(false && "Unrecognized format");
        }
    }
    
    template<AudioFormat TFormat> struct FormatToPrimitiveType { /* Error. Unsupported format */ };
    template<> struct FormatToPrimitiveType<AudioFormat::MonoFloat32> { using type = float; };
    template<> struct FormatToPrimitiveType<AudioFormat::StereoFloat32> { using type = float; };
    template<> struct FormatToPrimitiveType<AudioFormat::MonoSigned16> { using type = int16_t; };
    template<> struct FormatToPrimitiveType<AudioFormat::StereoSigned16> { using type = int16_t; };

    template<AudioFormat TFormat>
    struct AudioSample
    {
        using subsample_t = FormatToPrimitiveType<TFormat>::type;
        
        std::array<subsample_t, FormatNumChannels(TFormat)> channels;
    };

    std::vector<std::string> GetAllCaptureDeviceNames();
    std::string GetDefaultCaptureDeviceName();
}