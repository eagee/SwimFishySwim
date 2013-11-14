// LevelOne.h
// Defines an implementation of ILevel interface which is awesome...
// Author(s): Eagan Rackley
#ifndef LEVEL_ONE_H
#define LEVEL_ONE_H

#include "ILevel.h"
#include "LevelInfo.h"

///<summary>
/// Provides a concrete implementation for the first level and the myriad waves of baddies that you'll have to face off with...
///</summary>
class LevelOne: public ILevel
{ 
	public:
		
		LevelOne(LevelInfo &levelInfo);
		
		virtual ~LevelOne();

		///<summary>
		/// Indicates that the level will loop internally handling 
		/// all input, and output (audio, visual) until the user
		/// either completes the level (e.g. defeats all the waves for that level)
		/// or chooses to exit
		///<summary>
		virtual void runUntilLevelIsFinished();
	
	private:
		/// <summary>
		/// Releases our baddies on a time scale
		/// </summary>
		void releaseBaddiesOnASchedule(float counterIncrement);

		/// <summary>
		/// Performs a nice little intro for the upcoming boss...
		/// </summary>
		void renderBossIntro();

		/// <summary>
		/// Sweeps the tilemap to a certain location
		/// </summary>		
		void renderCameraSweep(Vector &initialMapLocation, Vector &interpolatedALocation, Vector &interpolatedDLocation);

		/// <summary>
		/// Instantiates as many baddie objects as are needed for this particular level
		/// </summary>
		void preAllocateBaddieSprites();

		/// <summary>
		/// Removes all of the instantiated baddie objects from the heap so we don't use it up for lords sake :D
		/// </summary>
		void deAllocateBaddieSprites();

		///<summary>
		/// Handles all the rendering code for the level ...
		///</summary>
		void renderLevel();

		///<summary>
		/// Handles all the AI update code for the level ...
		///</summary>
		void updateLevel();


		// Attributes
		bool  myTilemapLoaded;				
		Vector myLevelOneSpawnPoint;

		// Assocations
		LevelInfo					*myLevelInfo;				
};

#endif
