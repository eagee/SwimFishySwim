// IAudio.h
// Defines code that will play mp3 music as a placeholder until midi is done (Wii support only)
// Author(s): Eagan Rackley
#ifndef WII_MP3_AUDIO_H
#define WII_MP3_AUDIO_H

#include "IAudio.h"

///<summary>
/// Defines code that will play mp3 music as a placeholder until midi is done
///</summary>
class WiiAudioMp3: public IAudio
{ 
	public:
		WiiAudioMp3();
		
		virtual ~WiiAudioMp3();
		
		///<summary>
		/// Loads data into memory from a file ...
		///</summary>			
		virtual void loadFromFile(const char *fileName);

		///<summary>
		/// Plays the audio once then stops.
		///</summary>			
		virtual void playOnce();

		///<summary>
		/// Plays the audio over and over again ad infinitium.
		///</summary>	
		virtual void playLooped();						

		///<summary>
		/// Stops playing the audio and resets all counters.
		///</summary>	
		virtual void stop();

		///<summary>
		/// Pauses the audio where it is so that playing can resume from the same point.
		///</summary>	
		virtual void pause();

		///<summary>
		/// Returns true if the audio is currently being played, otherwise false...
		///</summary>	
		virtual bool isPlaying();

private:

		FILE *myFilePointer;
		char *myAudioBuffer;
		unsigned long myBufferSize;
};

#endif
