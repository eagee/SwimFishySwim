// WiiAudioMp3.cpp
// Provide a concrete implementation of IAudio supporting mp3 files in Wii homebrew
// Author(s): Eagan Rackley

#include "SFSHeaders.h"

#ifdef PLATFORM_IS_WII

#include "asndlib.h"
#include "mp3player.h"
#include "SimpleWiiHeap.h"
#include "WiiAudioMp3.h"

///<summary>
/// Handles initialization
///</summary>			
WiiAudioMp3::WiiAudioMp3()
{
	myFilePointer = NULL;
	myAudioBuffer = NULL;
	MP3Player_Init();
	MP3Player_Volume(127);		
}		

///<summary>
/// Handles shutdown ...
///</summary>			
WiiAudioMp3::~WiiAudioMp3()
{
	if(myAudioBuffer != NULL)
	{
		MP3Player_Stop();
		if(myAudioBuffer != NULL)
		{
			delete myAudioBuffer;
			//free(myAudioBuffer);
			myAudioBuffer = NULL;
		}
		//delete myAudioBuffer;		
	}
}

///<summary>
/// Loads data into memory from a file ...
///</summary>			
void WiiAudioMp3::loadFromFile(const char *fileName)
{		
	if(myFilePointer == NULL)
	{
		ASSERT(myAudioBuffer == NULL);
		myFilePointer = fopen(fileName, "rb");
		ASSERT(myFilePointer != NULL);
		fseek (myFilePointer, 0, SEEK_END);		
		myBufferSize = ftell(myFilePointer);
		//myAudioBuffer = simpleWiiHeapAllocatePersistentMemory(myBufferSize);			
		myAudioBuffer = new char[myBufferSize];
		ASSERT(myAudioBuffer != NULL);
		//myAudioBuffer = memalign(32, myBufferSize);		
		rewind(myFilePointer);
		fread(myAudioBuffer, 1, myBufferSize, myFilePointer);
		fclose(myFilePointer);
	}
}

///<summary>
/// Plays the audio once then stops.
///</summary>			
void WiiAudioMp3::playOnce()
{
	ASSERT(myAudioBuffer != NULL);
	if(myAudioBuffer != NULL)
	{	
		if(MP3Player_IsPlaying() == false)
		{
			MP3Player_PlayBuffer(myAudioBuffer, myBufferSize, NULL);
		}
	}
}

///<summary>
/// Plays the audio over and over again ad infinitium.
///</summary>	
void WiiAudioMp3::playLooped()
{
	ASSERT(myAudioBuffer != NULL);
	if(myAudioBuffer != NULL)
	{
		if(MP3Player_IsPlaying() == false)
		{			
			MP3Player_PlayBuffer(myAudioBuffer, myBufferSize, NULL);
		}
	}
}

///<summary>
/// Stops playing the audio and resets all counters.
///</summary>	
void WiiAudioMp3::stop()
{
	ASSERT(myAudioBuffer != NULL);
	MP3Player_Stop();
}

///<summary>
/// Pauses the audio where it is so that playing can resume from the same point.
///</summary>	
void WiiAudioMp3::pause()
{
	ASSERT(myAudioBuffer != NULL);
}

///<summary>
/// Returns true if the audio is currently being played, otherwise false...
///</summary>	
bool WiiAudioMp3::isPlaying()
{
	return MP3Player_IsPlaying();
}

#endif