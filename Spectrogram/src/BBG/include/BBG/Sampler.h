#pragma once
#include <BBG/BBG.h>

namespace BBG
{
	class Sampler
	{
	public:
		struct State
		{
			FilterMode minFilter = FilterMode::Nearest;
			FilterMode magFilter = FilterMode::Nearest;
			AddressMode addressModeU = AddressMode::ClampToEdge;
			AddressMode addressModeV = AddressMode::ClampToEdge;
			AddressMode addressModeW = AddressMode::ClampToEdge;
		};

	public:
		explicit Sampler(const State& state) noexcept;
		~Sampler() noexcept;
		BBG_NON_COPYABLE(Sampler);

		void SetFilter(FilterMode minFilter, FilterMode magFilter);
		void SetWrapMode(AddressMode addressModeU, AddressMode addressModeV, AddressMode addressModeW);
		
		const State& GetState() const
		{
			return state_;
		}

		uint32_t GetHandle() const
		{
			return handle_;
		}

	private:
		uint32_t handle_ = 0;
		State state_;
	};
}