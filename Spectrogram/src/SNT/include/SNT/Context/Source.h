#pragma once
#include <SNT/SNT.h>
#include <SNT/Context/Buffer.h>
#include <SNT/private/NonCopyable.h>

namespace SNT::Context
{
	class Source
	{
	public:
		struct AudioInfo
		{
			std::array<float, 3> position = {};
			std::array<float, 3> velocity = {};
			float gain = 1.0f;
			float pitch = 1.0f;
			bool looping = false;
		};

	public: 
		explicit Source(AudioInfo audioSettings = {}) noexcept;
		~Source() noexcept;
		SNT_NON_COPYABLE(Source);

		void SetBuffer(const Buffer& buffer) const;
		
		void SetPosition(std::array<float, 3> position);
		void SetVelocity(std::array<float, 3> velocity);
		void SetGain(float gain);
		void SetPitch(float pitch);
		void SetLooping(bool looping);
		void Play() const;

		AudioInfo GetAudioInfo() const
		{
			return audioInfo_;
		}

		uint32_t GetHandle() const
		{
			return handle_;
		}

	private:
		uint32_t handle_ = 0;
		AudioInfo audioInfo_;
	};
}

