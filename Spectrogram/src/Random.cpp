#include "Random.h"

namespace RNG
{
    static thread_local uint32_t rngSeed = 0u;
    static uint32_t AdvanceHash(uint32_t& seed);

    void ThreadInitializeRandomSeed(uint32_t value)
    {
        rngSeed = value;
    }

    uint32_t NextUint()
    {
        return AdvanceHash(rngSeed);
    }

    float NextFloat01()
    {
        static constexpr uint32_t MAX_UINT_PLUS_ONE = UINT32_MAX + 1.0f;
        return float(NextUint()) / MAX_UINT_PLUS_ONE;
    }

    static uint32_t AdvanceHash(uint32_t& seed)
    {
        seed = seed * 747796405u + 2891336453u;
        uint32_t word = ((seed >> ((seed >> 28u) + 4u)) ^ seed) * 277803737u;
        return (word >> 22u) ^ word;
    }
}