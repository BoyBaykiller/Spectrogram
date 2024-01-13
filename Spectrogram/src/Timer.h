#pragma once
#include <chrono>

class Timer
{
public:
	explicit Timer() noexcept
	{
		begin_ = std::chrono::steady_clock::now();
	}

	float GetElapsedTimeInMilliSec() const
	{
		return GetElapsedTimeInMicroSec() / 1000.0f;
	}

	uint32_t GetElapsedTimeInMicroSec() const
	{
		return GetElapsedTime<std::chrono::microseconds>();
	}

	template<typename T>
	uint32_t GetElapsedTime() const
	{
		auto end = std::chrono::steady_clock::now();
		auto duration = end - begin_;
		return std::chrono::duration_cast<T>(duration).count();
	}

private:
	std::chrono::steady_clock::time_point begin_;
};