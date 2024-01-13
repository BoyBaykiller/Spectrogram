#pragma once
#include <SNT/SNT.h>
#include <SNT/private/NonCopyable.h>

namespace SNT
{
    class PlayDevice
    {
    public:
        explicit PlayDevice() noexcept;
        ~PlayDevice() noexcept;

        SNT_NON_COPYABLE(PlayDevice)

        const ALCdevice* GetHandle() const
        {
            return handle_;
        }

    private:
        ALCdevice* handle_ = nullptr;
    };
}