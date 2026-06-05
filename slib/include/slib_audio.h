#pragma once

namespace sl
{
	// interface not complete so docs aren't complete
	struct Sound
	{
		void* Data = nullptr; //treated internally as MIX_Track*
	};

	Sound LoadSound(const char* Path);
	void PlaySound(Sound& sound);
	void PlaySound(Sound& sound, int times);
	void StopSound(Sound& sound);
	void RepeatSound(Sound& sound);;
}
