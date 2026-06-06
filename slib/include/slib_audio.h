#pragma once

namespace sl
{
	/*
	* a structure that repesents audio data.
	* 
	* \sa Load Sound
	* \sa PlaySound
	* \sa StopSound
	* \sa RepeatSound
	* \sa DestroySound
	*/
	struct Sound
	{
		void* Data = nullptr; /**< treated internally as MIX_Track */
	
		~Sound();
	};
	/*
	* Load an audio file into a Sound.
	* 
	* \param Path the path to the file.
	* 
	* \returns a Sound  structure with audio data
	* 
	* \sa PlaySound
	* \sa StopSound
	* \sa RepeatSound
	* \sa DestroySound
	*/
	Sound LoadSound(const char* Path);
	/*
	* Play a Sound.
	*
	* \param sound the sound to play.
	*
	* \sa LoadSound
	* \sa StopSound
	* \sa RepeatSound
	* \sa DestroySound
	*/
	void PlaySound(Sound& sound);
	/*
	* Play a Sound multiple times in a row.
	*
	* \param sound the sound to play.
	* \param times the number of times to play.
	*
	* \sa LoadSound
	* \sa StopSound
	* \sa RepeatSound
	* \sa DestroySound
	*/
	void PlaySound(Sound& sound, int times);
	/*
	* Stop a Sound that's currently playing.
	*
	* \param sound the sound to stop.
	*
	* \sa PlaySound
	* \sa LoadSound
	* \sa RepeatSound
	* \sa DestroySound
	*/
	void StopSound(Sound& sound);
	/*
	* Keep Playing a sound indefinitely.
	*
	* \param sound the sound to play.
	*
	* \sa PlaySound
	* \sa LoadSound
	* \sa StopSound
	* \sa DestroySound
	*/
	void RepeatSound(Sound& sound);
	/*
	* Free a Sound.
	*
	* \param sound the sound to free.
	*
	* \sa LoadSound
	* \sa PlaySound
	* \sa StopSound
	* \sa RepeatSound
	*/
	void DestroySound(Sound& sound);
}
