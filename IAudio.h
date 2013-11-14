// IAudio.h
// Defines the simple interface for all audio protocols (midi, mp3, pcm, controller)
// Author(s): Eagan Rackley
#ifndef IAUDIO_H
#define IAUDIO_H

///<summary>
/// // Defines the simple interface for all audio protocols (midi, mp3, pcm, controller)
///</summary>
class IAudio
{ 
	public:
		IAudio(){}
		
		virtual ~IAudio(){}
		
		///<summary>
		/// Loads data into memory from a file ...
		///</summary>			
		virtual void loadFromFile(const char *fileName) = 0;

		///<summary>
		/// Plays the audio once then stops.
		///</summary>			
		virtual void playOnce() = 0;

		///<summary>
		/// Plays the audio over and over again ad infinitium.
		///</summary>	
		virtual void playLooped() = 0;						

		///<summary>
		/// Stops playing the audio and resets all counters.
		///</summary>	
		virtual void stop() = 0;

		///<summary>
		/// Pauses the audio where it is so that playing can resume from the same point.
		///</summary>	
		virtual void pause() = 0;

		///<summary>
		/// Returns true if the audio is currently being played, otherwise false...
		///</summary>	
		virtual bool isPlaying() = 0;
};

#endif
