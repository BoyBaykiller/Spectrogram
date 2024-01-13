#pragma once
#include <array>

#include <BBG/BBG.h>

namespace BBG
{
    class TimerQuery
    {
    public:
        explicit TimerQuery() noexcept;
        ~TimerQuery() noexcept;

        float GetElapsedTimeInMicroSec() const;

        BBG_NON_COPYABLE(TimerQuery)

    private:
        std::array<uint32_t, 2> handles_{};
    };
}
