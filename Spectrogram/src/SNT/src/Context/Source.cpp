#include <SNT/Context/Source.h>

namespace SNT::Context
{
	Source::Source(AudioInfo audioInfo) noexcept
	{
		alGenSources(1, &handle_);

		SetPosition(audioInfo.position);
		SetVelocity(audioInfo.velocity);
		SetGain(audioInfo.gain);
		SetPitch(audioInfo.pitch);
		SetLooping(audioInfo.looping);
	}

	Source::~Source() noexcept
	{
		alDeleteSources(1, &handle_);
	}

	void Source::SetBuffer(const Buffer& buffer) const
	{
		alSourcei(handle_, AL_BUFFER, buffer.GetHandle());
	}

	void Source::SetPosition(std::array<float, 3> position)
	{
		alSource3f(handle_, AL_POSITION, position[0], position[1], position[2]);
		audioInfo_.position = position;
	}

	void Source::SetVelocity(std::array<float, 3> velocity)
	{
		alSource3f(handle_, AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
		audioInfo_.velocity = velocity;
	}

	void Source::SetGain(float gain)
	{
		alSourcei(handle_, AL_GAIN, gain);
		audioInfo_.gain = gain;
	}

	void Source::SetPitch(float pitch)
	{
		alSourcei(handle_, AL_PITCH, pitch);
		audioInfo_.pitch = pitch;
	}

	void Source::SetLooping(bool looping)
	{
		alSourcei(handle_, AL_LOOPING, looping);
		audioInfo_.looping = looping;
	}

	void Source::Play() const
	{
		alSourcePlay(handle_);
	}
}
