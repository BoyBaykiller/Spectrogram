#pragma once
#include <cstdint>
#include <numeric>

namespace RNG
{
    void ThreadInitializeRandomSeed(uint32_t value);
    
    uint32_t NextUint();
    float NextFloat01();
}

