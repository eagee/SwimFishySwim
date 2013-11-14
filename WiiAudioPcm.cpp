// AudioMp3.cpp
// Provide a concrete implementation of IAudio supporting mp3 files ...
// Author(s): Eagan Rackley

#include "SFSHeaders.h"

#ifdef PLATFORM_IS_WII

#include "asndlib.h"
#include "SimpleWiiHeap.h"
#include "WiiAudioPcm.h"

///<summary>
/// Handles initialization
///</summary>			
WiiAudioPcm::WiiAudioPcm()
{
	myFilePointer = NULL;
	myAudioBuffer = NULL;	
	myVolumeLevel = 50;
	myVoiceNumber = -1;
}		

///<summary>
/// Handles shutdown ...
///</summary>			
WiiAudioPcm::~WiiAudioPcm()
{
	delete myAudioBuffer;
}

///<summary>
/// Loads data into memory from a file ...
///</summary>			
void WiiAudioPcm::loadFromFile(const char *fileName)
{		
	if(myFilePointer == NULL)
	{
		ASSERT(myAudioBuffer == NULL);
		myFilePointer = fopen(fileName, "rb");
		ASSERT(myFilePointer != NULL);
		fseek (myFilePointer, 0, SEEK_END);		
		myBufferSize = ftell(myFilePointer);
		//myAudioBuffer = simpleWiiHeapAllocatePersistentMemory(myBufferSize);			
		//myAudioBuffer = memalign(32, myBufferSize);		
		myAudioBuffer = new char[myBufferSize];
		rewind(myFilePointer);
		fread(myAudioBuffer, 1, myBufferSize, myFilePointer);
		fclose(myFilePointer);
	}
}

///<summary>
/// Plays the audio once then stops.
///</summary>			
void WiiAudioPcm::playOnce()
{
	ASSERT(myAudioBuffer != NULL);
	if(myAudioBuffer != NULL)
	{	
		int volume = 255*(myVolumeLevel/100.0);
		myVoiceNumber = ASND_GetFirstUnusedVoice();
		if(myVoiceNumber >= 0)
		{
			ASND_SetVoice(myVoiceNumber, VOICE_STEREO_16BIT, 11025, 0, (u8 *)myAudioBuffer, myBufferSize, volume, volume, NULL);
		}

	}
}

///<summary>
/// Plays the audio over and over again ad infinitium.
///</summary>	
void WiiAudioPcm::playLooped()
{
	ASSERT(myAudioBuffer != NULL);
	if(myAudioBuffer != NULL)
	{
		if(ASND_StatusVoice(myVoiceNumber) != SND_WORKING || ASND_StatusVoice(myVoiceNumber) != SND_WAITING)
		{
			if(myVoiceNumber >= 0)
			{
				int volume = 255*(myVolumeLevel/100.0);
				ASND_SetVoice(myVoiceNumber, VOICE_STEREO_16BIT, 11025, 0, (u8 *)myAudioBuffer, myBufferSize, volume, volume, NULL);
			}
		}
		else
		{
			ASND_PauseVoice(myVoiceNumber, 1);
		}
	}
}

///<summary>
/// Stops playing the audio and resets all counters.
///</summary>	
void WiiAudioPcm::stop()
{
	ASSERT(myAudioBuffer != NULL);	
	if(myVoiceNumber > 0)
	{
		ASND_StopVoice(myVoiceNumber);
	}
}

///<summary>
/// Pauses the audio where it is so that playing can resume from the same point.
///</summary>	
void WiiAudioPcm::pause()
{
	ASSERT(myAudioBuffer != NULL);
	if(myVoiceNumber > 0)
	{
		// Forget pause for now... dunno if we need it ...
		//ASND_PauseVoice(myVoiceNumber, 1);
	}

}

///<summary>
/// Returns true if the audio is currently being played, otherwise false...
///</summary>	
bool WiiAudioPcm::isPlaying()
{
	if(ASND_StatusVoice(myVoiceNumber) != SND_WORKING || ASND_StatusVoice(myVoiceNumber) != SND_WAITING)
	{
		return false;
	}
	else
	{
		return true;
	}
}

#endif