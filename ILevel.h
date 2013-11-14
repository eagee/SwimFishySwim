// ILevel.h
// Defines a common interface for dealing with level behaviors...
// Author(s): Eagan Rackley
#ifndef ILEVEL_H
#define ILEVEL_H

///<summary>
/// Provides an interface for level behavior, whether that is a player sprite, or an NPC of some sort ...
///</summary>
class ILevel
{ 
	public:
		ILevel(){}
		
		virtual ~ILevel(){}	

		//TODO: We need to be able to specify an IMenu interface that will allow the player
		//      to pause, update options, and exit the level...

		///<summary>
		/// Indicates that the level will loop internally handling 
		/// all input, and output (audio, visual) until the user
		/// either completes the level (e.g. defeats all the waves for that level)
		/// or chooses to exit
		///<summary>
		virtual void runUntilLevelIsFinished() = 0;
};

#endif
