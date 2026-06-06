#include <SDL3_mixer/SDL_mixer.h>

#include "../include/slib_audio.h"

namespace sl
{
	//forward declarations
	void* GetMixer();
	void PrintError(const char* Header);
	//*forward declarations

	Sound LoadSound(const char* Path)
	{
		MIX_Audio* audio = MIX_LoadAudio((MIX_Mixer*)GetMixer(), Path, true);
		if (!audio)
		{
			PrintError("Failed to load audio: ");
			return { nullptr };
		}

		MIX_Track* track = MIX_CreateTrack((MIX_Mixer*)GetMixer());
		if (!track)
		{
			PrintError("Failed to create track: ");
			MIX_DestroyAudio(audio);
			return { nullptr };
		}

		if (!MIX_SetTrackAudio(track, audio))
		{
			PrintError("Failed to set track audio: ");
			MIX_DestroyAudio(audio);
			return { nullptr };
		}
		MIX_DestroyAudio(audio);

		return { track };
	}
	void PlaySound(Sound& sound)
	{
		if (!MIX_PlayTrack((MIX_Track*)sound.Data, 0))
		//if (!MIX_PlayAudio((MIX_Mixer*)GetMixer(), (MIX_Audio*)sound.Data))
		{
			PrintError("Failed to play track: ");
		}
	}
	void PlaySound(Sound& sound, int times)
	{
		static SDL_PropertiesID prop = SDL_CreateProperties();
		if (!prop)
		{
			PrintError("Failed to create propertires: ");
		}
		if (times < 1)
		{
			return;
		}

		SDL_SetNumberProperty(prop, MIX_PROP_PLAY_LOOPS_NUMBER, times - 1);
		MIX_PlayTrack((MIX_Track*)sound.Data, prop);
	}
	void RepeatSound(Sound& sound)
	{
		static SDL_PropertiesID prop = SDL_CreateProperties();
		if (!prop)
		{
			PrintError("Failed to create propertires: ");
		}

		SDL_SetNumberProperty(prop, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
		MIX_PlayTrack((MIX_Track*)sound.Data, prop);
	}
	void StopSound(Sound& sound)
	{
		MIX_PauseTrack((MIX_Track*)sound.Data);
	}
	void DestroySound(Sound& sound)
	{
		MIX_DestroyTrack((MIX_Track*)sound.Data);
		sound.Data = nullptr;
	}
	Sound::~Sound()
	{
		if (Data)
		{
			DestroySound(*this);
		}
	}
}




