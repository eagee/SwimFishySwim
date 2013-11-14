// LevelOne.cpp
// Provides a concrete implementation of ILevel interface for the first map and wave of enemies
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "LevelOne.h"
#include "LevelMethods.h"
#include "IProjectileWeapon.h"
#include "EndOfWaveMenu.h"
#include "NewTowerMenu.h"
#include "GameMenuInfo.h"
#include "Profiler.h"

// Defines the total number of sprites we'll need to use (and re-use) for this level...
#define NUM_L1_BADDIE_SPRITES 73

#define MAX_BADDIES_AT_A_TIME 6
#define NUMBER_OF_WAVES   5
#define BADDIES_IN_WAVE_1 24
#define BADDIES_IN_WAVE_2 72
#define BADDIES_IN_WAVE_3 36
#define BADDIES_IN_WAVE_4 2
#define BADDIES_IN_WAVE_5 24
#define BADDIES_IN_WAVE_6 24

// These values increased by the wave to increase baddie dificulty?
#define LEVEL_ONE_BADDIE_VALUE		5
#define LEVEL_TWO_BADDIE_VALUE		10
#define LEVEL_THREE_BADDIE_VALUE	20
#define LEVEL_FOUR_BADDIE_VALUE		40
#define LEVEL_FIVE_BADDIE_VALUE		60
#define LEVEL_TEN_BADDIE_VALUE		250

typedef enum
{
	L1_CENTER 		= 0,
	L1_LEFT 		= 1,
	L1_RIGHT		= 2,
	L1_ZIG_ZAG_L 	= 3,
	L1_ZIG_ZAG_R 	= 4,
	L1_RANDOM 		= 5
} L1_APPROACH;

#define WAVE_READY_Z 0.036f
#define WAVE_DONE_Z  0.042f

//static Vector gTextVelocity = {0.0f, 0.0f, -10.0f};
//static Vector gTextLocation = {0.0f, -0.4f, 10.0f};

///<summary>
/// Constructor for level one which assigns key components to the LevelInfo struct for later use in runUntilLevelIsFinished...
///</summary>
LevelOne::LevelOne(LevelInfo &levelInfo)
{
	myLevelInfo = &levelInfo;
	myLevelInfo->CurrentSong = MP3_AUDIO;

	//LevelMethodsDeAllocateLevelMusic(MP3_AUDIO, myLevelInfo);
	//LevelMethodsPreAllocateLevelMusic(MP3_AUDIO, myLevelInfo, "//assets//music//fineBrine.mp3");
					
	myLevelInfo->CurrentUpgradePoints = 1000;
	// Setup the fog we're going to use in this level...			
	Color fogColor = {0.6875f, 0.78f, 0.75f};
	myLevelInfo->Graphics->setFogMode( 0.001f, 116.0f, 0.001f, 80.0f, fogColor);

	// Set up our baddie release and wave counters (baddie behavior will change by wave)
	myLevelInfo->BaddieDetails.ReleaseDelay = 1.0f;
	myLevelInfo->BaddieDetails.ReleaseIndex = -1;		
	myLevelInfo->BaddieDetails.WaveIndex = 1;
	myLevelInfo->BaddieDetails.BaddiesThisWaveAreDead = false;
	myLevelInfo->BaddieDetails.MaximumThisWave = BADDIES_IN_WAVE_1;
	myLevelInfo->BaddieDetails.CurrentType = 0;
	myLevelOneSpawnPoint.x =  116.0f;
	myLevelOneSpawnPoint.y =  118.0f;
	myLevelOneSpawnPoint.z =  SPRITE_WATER_LEVEL;

	// Set up the damage used by our player ...
	myLevelInfo->CurrentPlayerRepelDamage = 0.35f;
	myLevelInfo->CurrentPlayerBulletDamage = 0.25f;

	// Set up the goal for all of our baddies ...
	Vector baddieDestination = {37.0f, 31.0f, 0.0f}; 
	myLevelInfo->BaddieDestination = baddieDestination;

	myLevelInfo->BaddieDetails.CurrentlyAlive = 0;

	// Load our level texture so the user has a map and a radar...	
	myLevelInfo->RadarTexture = myLevelInfo->Graphics->addTextureData("//assets//texOther//levelOneMap.png", IMAGE_TEXTURE, false);

	myLevelInfo->GreatPickle->setAnimation(FLIP);

	myLevelInfo->CurrentEnergyLevel = 0.0f;

	// Allocate all of the sprite objects we'll need for this level...
	preAllocateBaddieSprites();

	// TODO: Remove this code, it's only for testing!!!
	// Set the baddies up on a grid, so we can get an idea how our frustum culling is actually working...

	// Setup our node history size for the baddie sprites to use ...
	//myNodeHistorySize.UnitsHigh = myLevelInfo->TileMap->getTilemapSize().tilemapHeight;
	//myNodeHistorySize.UnitsWide = myLevelInfo->TileMap->getTilemapSize().tilemapWidth;	
}


///<summary>
/// Destructor for level one - here we do some cleanup routines ...
///</summary>
LevelOne::~LevelOne()
{
	// The Tile Map is deleted after main procedure is run ...
	// but we do need to deallocate all of our baddie sprites
	// since we'll be using others on different levels
	deAllocateBaddieSprites();
	//myLevelInfo->TileMap->freeLayerVertexArrays(myLevelInfo->Graphics, TILEMAP_LAYER_A);	
	//myLevelInfo->TileMap->freeLayerVertexArrays(myLevelInfo->Graphics, TILEMAP_LAYER_D);	
}


/// <summary>
/// Instantiates as many baddie objects as are needed for this particular level
/// </summary>
void LevelOne::preAllocateBaddieSprites()
{
	for(int idx = 0; idx < NUM_L1_BADDIE_SPRITES; idx++)
	{
		SpriteBaddie newBaddie;
		myLevelInfo->BaddieDetails.Sprites.push_back(newBaddie);
		LevelMethodsSetupSprite(myLevelInfo->Models[SMALL_BADDIE_FISH_MODEL], &myLevelInfo->BaddieDetails.Sprites.at(idx), ATTACK_TYPE_NONE, myLevelOneSpawnPoint.x, myLevelOneSpawnPoint.y, myLevelOneSpawnPoint.z, 1.0f);
	}
}

/// <summary>
/// Removes all of the instantiated baddie objects from the heap so we don't use it up for lords sake :D
/// </summary>
void LevelOne::deAllocateBaddieSprites()
{	
	myLevelInfo->BaddieDetails.Sprites.clear();
}

///<summary>
/// Used to complete the first half of a rendering process
///</summary>
static void beginMessageOutputScene(LevelInfo *levelInfo)
{
	CONTROLLER_STATUS status;	

	levelInfo->FinishedLooping = LevelMethodsHandleInput(0, status, levelInfo);

	//levelInfo->Songs[levelInfo->CurrentSong]->playLooped();
	
	// Set our Sprite offset ...
	levelInfo->SpriteOffset = levelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	levelInfo->SpriteOffset.z = 0.0f;

	// Update our map velocity based on the fishes movements ...
	LevelMethodsHandleTilemapCollision(*levelInfo->PlayerOneSprite, levelInfo);
	
	levelInfo->GreatPickle->setOffset(levelInfo->SpriteOffset);							

	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SHOT]->updateWeaponAndProjectiles(levelInfo);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_BOMB]->updateWeaponAndProjectiles(levelInfo);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_MINE]->updateWeaponAndProjectiles(levelInfo);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT]->updateWeaponAndProjectiles(levelInfo);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]->updateWeaponAndProjectiles(levelInfo);
		
	LevelMethodsUpdateActivePowerups(levelInfo);

	LevelMethodsUpdateTowers(levelInfo);
	LevelMethodsUpdateScenery(levelInfo);

	LevelMethodsBeginRendering(levelInfo);										
	LevelMethodsBegin2dCameraMode(levelInfo);
}

///<summary>
/// Used to complete the second half of a rendering process
///</summary>
static void finishMessageOutputScene(LevelInfo *levelInfo)
{
	LevelMethodsEndRendering(levelInfo);
	LevelMethodsEnd2DCameraMode(levelInfo);
}

///<summary>
/// Renders a WAVE-COMPLETE message at the end of a wave
///</summary>
static void renderWaveCompleteMessage(LevelInfo *levelInfo, int waveNumber)
{
	float line1 = -0.003f;
	float interpolationValue;

	levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, 0.24f);

	for(interpolationValue = 0; interpolationValue <= 1.0f; interpolationValue += 0.020f)
	{
		beginMessageOutputScene(levelInfo);
		float xCoordinate = (0.02f - (0.042f * interpolationValue));
		Vector textLocation = {xCoordinate, line1, WAVE_DONE_Z};
		levelInfo->TextOutput->renderText(textLocation, "Wave %d: Finished!", waveNumber);				
		finishMessageOutputScene(levelInfo);
	}

	for(interpolationValue = 0; interpolationValue <= 1.0f; interpolationValue += 0.015f)
	{
		beginMessageOutputScene(levelInfo);		
		Vector textLocation2 = {-0.022f, line1, WAVE_DONE_Z};
		levelInfo->TextOutput->renderText(textLocation2, "Wave %d: Finished!!!", waveNumber);
		finishMessageOutputScene(levelInfo);
	}

	levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, DEFAULT_TEXT_SPACING);
}

#define TX_X 0.0026f
#define TX_Y 0.0026f
#define TX_Z 0.0026f

///<summary>
/// Renders the wave number and title when a wave begins...
///</summary>
static void renderWaveReadyMessage(LevelInfo *levelInfo, int waveNumber, const char *message)
{
	float line1 = -0.003f;
	float line2 =  0.001f;
	float interpolationValue;

	for(interpolationValue = 0; interpolationValue <= 1.0f; interpolationValue += 0.025f)
	{
		beginMessageOutputScene(levelInfo);
		float xCoordinate = (0.020f - (0.030f * interpolationValue));
		//levelInfo->TextOutput->renderText((Vector){xCoordinate, -0.025f, 1.5f}, "Wave %d: Ready!", waveNumber);
		levelInfo->TextOutput->setTextSize(TX_X, TX_Y, TX_Z);
		levelInfo->TextOutput->renderText(VEC_INIT(xCoordinate, line1, WAVE_READY_Z), "Wave");				
		finishMessageOutputScene(levelInfo);
	}
	for(interpolationValue = 0; interpolationValue <= 1.0f; interpolationValue += 0.025f)
	{
		beginMessageOutputScene(levelInfo);
		float xCoordinate = (0.020f - (0.030f * interpolationValue));
		//levelInfo->TextOutput->renderText((Vector){xCoordinate, -0.025f, 1.5f}, "Wave %d Ready!", waveNumber);				
		levelInfo->TextOutput->setTextSize(TX_X, TX_Y, TX_Z);		
		levelInfo->TextOutput->renderText(VEC_INIT(-0.01f, line1, WAVE_READY_Z), "Wave");
		levelInfo->TextOutput->renderText(VEC_INIT(xCoordinate, line1, WAVE_READY_Z), "     %d:", waveNumber);						
		finishMessageOutputScene(levelInfo);
	}
	for(interpolationValue = 0; interpolationValue <= 1.0f; interpolationValue += 0.025f)
	{
		beginMessageOutputScene(levelInfo);
		float xCoordinate = (0.020f - (0.040f * interpolationValue));
		//levelInfo->TextOutput->renderText((Vector){xCoordinate, -0.025f, 1.5f}, "Wave %d Finished!", waveNumber);
		levelInfo->TextOutput->setTextSize(TX_X, TX_Y, TX_Z);
		levelInfo->TextOutput->renderText(VEC_INIT(-0.010f, line1, WAVE_READY_Z), "Wave");
		levelInfo->TextOutput->renderText(VEC_INIT(-0.010f, line1, WAVE_READY_Z), "     %d:", waveNumber);
		levelInfo->TextOutput->renderText(VEC_INIT(xCoordinate, line2, WAVE_READY_Z),"%s", message);				
		finishMessageOutputScene(levelInfo);
	}
	for(interpolationValue = 0; interpolationValue <= 1.0f; interpolationValue += 0.025f)
	{
		beginMessageOutputScene(levelInfo);		
		//levelInfo->TextOutput->renderText((Vector){xCoordinate, -0.025f, 1.5f}, "Wave %d Finished!", waveNumber);
		levelInfo->TextOutput->setTextSize(TX_X, TX_Y, TX_Z);
		levelInfo->TextOutput->renderText(VEC_INIT(-0.010f, line1, WAVE_READY_Z), "Wave");
		levelInfo->TextOutput->renderText(VEC_INIT(-0.010f, line1, WAVE_READY_Z), "     %d:", waveNumber);
		levelInfo->TextOutput->renderText(VEC_INIT(-0.020f, line2, WAVE_READY_Z), "%s", message);
		finishMessageOutputScene(levelInfo);
	}
	for(interpolationValue = 0; interpolationValue <= 1.0f; interpolationValue += 0.020f)
	{
		beginMessageOutputScene(levelInfo);
		float xCoordinate = (-0.010f - (0.080f * interpolationValue));
		//levelInfo->TextOutput->renderText((Vector){xCoordinate, -0.025f, 1.5f}, "Wave %d Finished!", waveNumber);
		levelInfo->TextOutput->setTextSize(TX_X, TX_Y, TX_Z);
		levelInfo->TextOutput->renderText(VEC_INIT(xCoordinate, line1, WAVE_READY_Z), "Wave");				
		levelInfo->TextOutput->renderText(VEC_INIT(xCoordinate, line1, WAVE_READY_Z), "     %d:", waveNumber);						
		levelInfo->TextOutput->renderText(VEC_INIT(xCoordinate-0.010f, line2, WAVE_READY_Z), "%s", message);
		finishMessageOutputScene(levelInfo);
	}

	levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, DEFAULT_TEXT_SPACING);
}

/// <summary>
/// Renders a camera sweep
/// </summary>
void LevelOne::renderCameraSweep(Vector &initialMapLocation, Vector &interpolatedALocation, Vector &interpolatedDLocation)
{
	Vector mapVelocity = {0.0f, 0.0f, 0.0f}; 

	myLevelInfo->TileMap->translateMap(TILEMAP_LAYER_A, interpolatedALocation);
	myLevelInfo->TileMap->translateMap(TILEMAP_LAYER_D, interpolatedDLocation);				

	// Set our Sprite offset ...
	myLevelInfo->SpriteOffset = myLevelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	myLevelInfo->SpriteOffset.z = 0.0f;		

	LevelMethodsUpdateMapVelocity(*myLevelInfo->TileMap, mapVelocity);
	myLevelInfo->TileMap->animationTask(TILEMAP_LAYER_D);
	
	myLevelInfo->Graphics->beginScene();
	LevelMethodsRenderSkybox(PROJECTION_NORMAL, myLevelInfo->Graphics);
	myLevelInfo->TileMap->renderLayerVertexArrays(myLevelInfo->Graphics, TILEMAP_LAYER_A);

	Vector playerOffset;
	playerOffset.x = myLevelInfo->SpriteOffset.x - initialMapLocation.x;
	playerOffset.y = myLevelInfo->SpriteOffset.y - initialMapLocation.y;
	myLevelInfo->PlayerOneSprite->setOffset(playerOffset);

    LevelMethodsUpdateTowers(myLevelInfo);
	LevelMethodsRenderTowers(myLevelInfo);

	LevelMethodsUpdateScenery(myLevelInfo);
	LevelMethodsRenderScenery(myLevelInfo);

	myLevelInfo->PlayerOneSprite->renderSprite(myLevelInfo->Graphics);
	// Render the boss model we're sweeping to...
	myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).renderSprite(myLevelInfo->Graphics);								

	myLevelInfo->TileMap->renderLayerVertexArrays(myLevelInfo->Graphics, TILEMAP_LAYER_D);

	// Update the particles we're using...
	myLevelInfo->RippleParticles->updateParticles();
	myLevelInfo->RepelParticles->updateParticles();
	myLevelInfo->SparkParticles->updateParticles();
	myLevelInfo->SplashParticles->updateParticles();
	myLevelInfo->WarpParticles->updateParticles();

	myLevelInfo->RippleParticles->renderParticles(myLevelInfo->SpriteOffset, myLevelInfo->Graphics);
	myLevelInfo->RepelParticles->renderParticles(myLevelInfo->SpriteOffset, myLevelInfo->Graphics);
	myLevelInfo->SparkParticles->renderParticles(myLevelInfo->SpriteOffset, myLevelInfo->Graphics);
	myLevelInfo->SplashParticles->renderParticles(myLevelInfo->SpriteOffset, myLevelInfo->Graphics);
	myLevelInfo->WarpParticles->renderParticles(myLevelInfo->SpriteOffset, myLevelInfo->Graphics);


	
	myLevelInfo->Graphics->endScene();
}

/// <summary>
/// Randomly selects from a set of three different routes the baddies can take...
/// </summary>static void
void setupLevelOneWaypoints(SpriteBaddie *sprite, L1_APPROACH approach)
{
	sprite->clearWaypoints();

	// If it's a random approach randomly reassign the approach value
	if(approach == L1_RANDOM)
	{
		approach = (L1_APPROACH)(rand() % (int)L1_RANDOM);
	}

	ASSERT(approach <= L1_ZIG_ZAG_R);

	if(approach == L1_CENTER)
	{
		sprite->addWaypoint(VEC_INIT(101.0f, 105.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(74.0f, 72.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(44.0f, 50.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(36.0f, 30.0f, 21.1f));
	}
	else if(approach == L1_RIGHT)
	{
		sprite->addWaypoint(VEC_INIT(101.0f, 105.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(118.0f, 82.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(102.0f, 52.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(63.0f, 32.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(44.0f, 50.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(36.0f, 30.0f, 21.1f));
	}
	else if(approach == L1_LEFT)
	{
		sprite->addWaypoint(VEC_INIT(101.0f, 105.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(80.0f, 114.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(60.0f, 106.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(32.0f, 76.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(40.0f, 50.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(36.0f, 30.0f, 21.1f));
	}
	else if(approach == L1_ZIG_ZAG_L)
	{
		sprite->addWaypoint(VEC_INIT(101.0f, 105.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(80.0f, 114.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(60.0f, 106.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(51.0f, 91.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(74.0f, 72.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(102.0f, 52.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(88.0f, 32.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(36.0f, 30.0f, 21.1f));
	}
	else if(approach == L1_ZIG_ZAG_R)
	{
		sprite->addWaypoint(VEC_INIT(101.0f, 105.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(118.0f, 82.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(102.0f, 52.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(74.0f, 72.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(51.0f, 91.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(32.0f, 76.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(44.0f, 50.0f, 21.1f));
		sprite->addWaypoint(VEC_INIT(36.0f, 30.0f, 21.1f));
	}


}

/// <summary>
/// Handles any visual indications associated with the end of the level...
/// </summary>
static void handleWaveSetup(LevelInfo *levelInfo, int maxBaddies, const char *waveName)
{
	levelInfo->BaddieDetails.MaximumThisWave = maxBaddies;
	levelInfo->BaddieDetails.ReleaseIndex = 0;
	renderWaveReadyMessage(levelInfo, levelInfo->BaddieDetails.WaveIndex, waveName);
}


/// <summary>
/// Handles any visual indications associated with the end of the level...
/// </summary>
static void handleEndOfLevel(LevelInfo *levelInfo)
{
	if (levelInfo->BaddieDetails.WaveIndex <= NUMBER_OF_WAVES)
	{
		levelInfo->BaddieDetails.WaveIndex++;

		levelInfo->GreatPickle->setAnimation(FLIP);

		levelInfo->PlayerOneSprite->setAnimation(FLIP);

		levelInfo->Sounds[WAVE_FINISHED]->playOnce();

		renderWaveCompleteMessage(levelInfo, (levelInfo->BaddieDetails.WaveIndex - 1));

		if (levelInfo->BaddieDetails.WaveIndex > NUMBER_OF_WAVES)
			levelInfo->BaddieDetails.WaveIndex = 1;

		EndOfWaveMenu endOfWaveMenu(levelInfo);
		endOfWaveMenu.executeMenu(levelInfo);

		levelInfo->BaddieDetails.BaddiesThisWaveAreDead = false;

		// Wave one setup done at start of level...
		//if (levelInfo->BaddieDetails.WaveIndex == 1)
		//{
		//	handleWaveSetup(levelInfo, BADDIES_IN_WAVE_1, "Slimer Scouts!!!!!");
		//}
		if (levelInfo->BaddieDetails.WaveIndex == 2)
		{
			handleWaveSetup(levelInfo, BADDIES_IN_WAVE_2, "Smarmy Swarmers!!!!!");
		}
		else if (levelInfo->BaddieDetails.WaveIndex == 3)
		{
			handleWaveSetup(levelInfo, BADDIES_IN_WAVE_3, "Ack! Icky Eels!!!!!");
		}
		else if (levelInfo->BaddieDetails.WaveIndex == 4)
		{
			handleWaveSetup(levelInfo, BADDIES_IN_WAVE_4, "Ohhhh noooz!!!!!");
		}
		else if (levelInfo->BaddieDetails.WaveIndex == 5)
		{
			handleWaveSetup(levelInfo, BADDIES_IN_WAVE_5, "Slime Bullies!!!!");
		}
	}
}

/// <summary>
/// Handles any code associated with the release of the next baddie in a wave
/// </summary>
static void handleReleaseDelay(LevelInfo *levelInfo, float delayLength)
{
	if(levelInfo->BaddieDetails.WaveIndex == 2)
	{
		levelInfo->BaddieDetails.ReleaseDelay = 0.95f;
		levelInfo->BaddieDetails.ReleaseTime = 0.009f;
	}
	else
	{
		levelInfo->BaddieDetails.ReleaseDelay = 0.0f;
		levelInfo->BaddieDetails.ReleaseTime = (fmod((float)rand(), delayLength) + 0.003f) * 0.001f;
		if(levelInfo->BaddieDetails.ReleaseTime <= 0.009f) levelInfo->BaddieDetails.ReleaseTime = 0.009f;
	}
}

/// <summary>
/// Handles any code associated with the release of the next baddie in a wave
/// </summary>
static void handleBaddieRelease(LevelInfo *levelInfo, MODEL_TYPE modelType, float controlMagnitude, int value, ATTACK_TYPE attackType, Vector spawnPoint, L1_APPROACH approach)
{
	int releaseIndex = levelInfo->BaddieDetails.ReleaseIndex;
	LevelMethodsSetupSprite(levelInfo->Models[modelType], &levelInfo->BaddieDetails.Sprites.at(releaseIndex), attackType, spawnPoint.x, spawnPoint.y, spawnPoint.z, 1.5f);
	levelInfo->BaddieDetails.Sprites.at(releaseIndex).setControlMagnitude(controlMagnitude);
	levelInfo->BaddieDetails.Sprites.at(releaseIndex).setPointValue(levelInfo->BaddieDetails.WaveIndex * LEVEL_ONE_BADDIE_VALUE);
	levelInfo->BaddieDetails.Sprites.at(releaseIndex).setHealth((float)(levelInfo->BaddieDetails.WaveIndex * LEVEL_ONE_BADDIE_VALUE));
	// Setup the waypoints we want the baddies to meet on this map
	setupLevelOneWaypoints(&levelInfo->BaddieDetails.Sprites.at(releaseIndex), approach);

	levelInfo->BaddieDetails.Sprites.at(releaseIndex).setOffset(levelInfo->SpriteOffset);
	LevelMethodsHandleWarpEffects(levelInfo, &levelInfo->BaddieDetails.Sprites.at(releaseIndex));
}

/// <summary>
/// Releases our baddies on a time scale
/// </summary>
void LevelOne::releaseBaddiesOnASchedule(float counterIncrement)
{	
	//printf("\x1b[2;0H"); printf("Wave: %d, Idx: %d Max: %d", myLevelInfo->BaddieDetails.WaveIndex, myLevelInfo->BaddieDetails.ReleaseIndex, myLevelInfo->BaddieDetails.MaximumThisWave);
	myLevelInfo->BaddieDetails.ReleaseDelay += counterIncrement;

	// Go through our baddies. They're all the same on this level...
	int maxBaddies = MAX_BADDIES_AT_A_TIME;
	if(myLevelInfo->BaddieDetails.WaveIndex == 2) maxBaddies = 12;
	if((myLevelInfo->BaddieDetails.ReleaseDelay >= 1.0f) && (myLevelInfo->BaddieDetails.CurrentlyAlive < maxBaddies))
	{
		handleReleaseDelay(myLevelInfo, 9.0f);

		// If we've already released all our baddies, check to see if they're all dead,
		// and increment to the next available wave if they are :D :D
		if(myLevelInfo->BaddieDetails.ReleaseIndex >= myLevelInfo->BaddieDetails.MaximumThisWave)
		{			
			if(myLevelInfo->BaddieDetails.BaddiesThisWaveAreDead == true)
			{
				handleEndOfLevel(myLevelInfo);
			}
		}

		if(myLevelInfo->BaddieDetails.ReleaseIndex < myLevelInfo->BaddieDetails.MaximumThisWave)
		{				
			myLevelInfo->BaddieDetails.ReleaseIndex++;
			if(myLevelInfo->BaddieDetails.WaveIndex == 1)
			{
				if(myLevelInfo->BaddieDetails.ReleaseIndex <= 6)
					handleBaddieRelease(myLevelInfo, SMALL_BADDIE_FISH_MODEL, 0.20f, LEVEL_ONE_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_RIGHT);
				else if(myLevelInfo->BaddieDetails.ReleaseIndex <= 12)
					handleBaddieRelease(myLevelInfo, SMALL_BADDIE_FISH_MODEL, 0.20f, LEVEL_ONE_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_LEFT);
				else if(myLevelInfo->BaddieDetails.ReleaseIndex <= 18)
					handleBaddieRelease(myLevelInfo, SMALL_BADDIE_FISH_MODEL, 0.20f, LEVEL_ONE_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_CENTER);
				else if(myLevelInfo->BaddieDetails.ReleaseIndex <= 24)
					handleBaddieRelease(myLevelInfo, SMALL_BADDIE_FISH_MODEL, 0.20f, LEVEL_ONE_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_RANDOM);
			}
			else if(myLevelInfo->BaddieDetails.WaveIndex == 2)
			{
				if (myLevelInfo->BaddieDetails.ReleaseIndex <= 12)
					handleBaddieRelease(myLevelInfo, FAST_BADDIE_FISH_MODEL, 0.6f, LEVEL_ONE_BADDIE_VALUE / 8, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_CENTER);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 24)
					handleBaddieRelease(myLevelInfo, FAST_BADDIE_FISH_MODEL, 0.6f, LEVEL_ONE_BADDIE_VALUE / 8, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_LEFT);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 36)
					handleBaddieRelease(myLevelInfo, FAST_BADDIE_FISH_MODEL, 0.6f, LEVEL_ONE_BADDIE_VALUE / 8, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_RIGHT);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 48)
					handleBaddieRelease(myLevelInfo, FAST_BADDIE_FISH_MODEL, 0.6f, LEVEL_ONE_BADDIE_VALUE / 8, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_ZIG_ZAG_L);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 60)
					handleBaddieRelease(myLevelInfo, FAST_BADDIE_FISH_MODEL, 0.6f, LEVEL_ONE_BADDIE_VALUE / 8, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_ZIG_ZAG_R);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 72)
					handleBaddieRelease(myLevelInfo, FAST_BADDIE_FISH_MODEL, 0.6f, LEVEL_ONE_BADDIE_VALUE / 8, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_RANDOM);
			}
			else if (myLevelInfo->BaddieDetails.WaveIndex == 3)
			{
				if (myLevelInfo->BaddieDetails.ReleaseIndex <= 6)
					handleBaddieRelease(myLevelInfo, MEDIUM_BADDIE_FISH_MODEL, 0.25f, LEVEL_TWO_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_ZIG_ZAG_L);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 12)
					handleBaddieRelease(myLevelInfo, MEDIUM_BADDIE_FISH_MODEL, 0.25f, LEVEL_TWO_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_LEFT);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 18)
					handleBaddieRelease(myLevelInfo, MEDIUM_BADDIE_FISH_MODEL, 0.25f, LEVEL_TWO_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_RIGHT);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 24)
					handleBaddieRelease(myLevelInfo, MEDIUM_BADDIE_FISH_MODEL, 0.25f, LEVEL_TWO_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_ZIG_ZAG_R);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 30)
					handleBaddieRelease(myLevelInfo, MEDIUM_BADDIE_FISH_MODEL, 0.25f, LEVEL_TWO_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_CENTER);
				else if (myLevelInfo->BaddieDetails.ReleaseIndex <= 36)
					handleBaddieRelease(myLevelInfo, MEDIUM_BADDIE_FISH_MODEL, 0.25f, LEVEL_TWO_BADDIE_VALUE, ATTACK_TYPE_NONE, myLevelOneSpawnPoint, L1_RANDOM);
			}
			else if ((myLevelInfo->BaddieDetails.WaveIndex == 4) && (myLevelInfo->BaddieDetails.ReleaseIndex == (myLevelInfo->BaddieDetails.MaximumThisWave - 1)))
			{				
				LevelMethodsSetupSprite(myLevelInfo->Models[BOSS_FISH_MODEL], &myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex), ATTACK_TYPE_PHYSICAL, myLevelOneSpawnPoint.x, myLevelOneSpawnPoint.y, myLevelOneSpawnPoint.z, 1.0f);
				myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setControlMagnitude(0.20f);
				myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setPointValue(myLevelInfo->BaddieDetails.WaveIndex * LEVEL_FIVE_BADDIE_VALUE * LEVEL_TEN_BADDIE_VALUE);
				myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setHealth((float)(myLevelInfo->BaddieDetails.WaveIndex * LEVEL_TEN_BADDIE_VALUE));
				myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setJump(JUMP_STATE_RECOVERING);																							
				// Setup the waypoints we want the baddies to meet on this map
				setupLevelOneWaypoints(&myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex), L1_ZIG_ZAG_L);
				myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setAnimation(STAND);
				renderBossIntro();				
			}
			else if(myLevelInfo->BaddieDetails.WaveIndex == 5)
			{				
				LevelMethodsSetupSprite(myLevelInfo->Models[ATTACKING_BADDIE_FISH_MODEL], &myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex), ATTACK_TYPE_PHYSICAL, myLevelOneSpawnPoint.x, myLevelOneSpawnPoint.y, myLevelOneSpawnPoint.z, 1.0f);
				myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setControlMagnitude(0.40f);
				myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setPointValue(myLevelInfo->BaddieDetails.WaveIndex * LEVEL_ONE_BADDIE_VALUE);
				myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setHealth((float)(myLevelInfo->BaddieDetails.WaveIndex * LEVEL_ONE_BADDIE_VALUE));
				setupLevelOneWaypoints(&myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex), L1_RANDOM);
			}
			else
			{
				myLevelInfo->BaddieDetails.ReleaseTime = 1.0f;
			}

			myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setAnimation(STAND);
		}// end if we have baddies to release. ..

	} // end if counter > 1.0f	
}

///<summary>
/// Updates the state of the available projectile weapons in this level..
///</summary>
static void updateProjectileWeapons(LevelInfo *levelInfo)
{
	// Update player projectile weapons...
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SHOT]->updateWeaponAndProjectiles(levelInfo);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_BOMB]->updateWeaponAndProjectiles(levelInfo);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_MINE]->updateWeaponAndProjectiles(levelInfo);
	
	// Update our tower projectile weapons
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT]->updateWeaponAndProjectiles(levelInfo);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]->updateWeaponAndProjectiles(levelInfo);
}

///<summary>
/// Handles all the AI update code for the level ...
///</summary>
void LevelOne::updateLevel()
{
	//myLevelInfo->Songs[myLevelInfo->CurrentSong]->playLooped();

	// Set our Sprite offset ...
	myLevelInfo->SpriteOffset = myLevelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	myLevelInfo->SpriteOffset.z = 0.0f;

	LevelMethodsSetupTempCollision(*myLevelInfo->PlayerOneSprite, *myLevelInfo->TileMap);

	// Update the particles we're using...
	myLevelInfo->RippleParticles->updateParticles();
	myLevelInfo->RepelParticles->updateParticles();
	myLevelInfo->SparkParticles->updateParticles();
	myLevelInfo->SplashParticles->updateParticles();
	myLevelInfo->WarpParticles->updateParticles();

	// Update our map velocity based on the fishes movements ...
	LevelMethodsHandleTilemapCollision(*myLevelInfo->PlayerOneSprite, myLevelInfo);

	// Set up ripple effects on the player sprite...
	LevelMethodsHandleRippleSplashEffects(myLevelInfo, myLevelInfo->PlayerOneSprite);

	// Updates our projectile weapon states
	updateProjectileWeapons(myLevelInfo);

	LevelMethodsUpdateActivePowerups(myLevelInfo);

	LevelMethodsUpdateBaddies(myLevelInfo);

	LevelMethodsUpdateTowers(myLevelInfo);

	LevelMethodsUpdateScenery(myLevelInfo);

	LevelMethodsUpdateFloatingTextData(myLevelInfo);

	myLevelInfo->GreatPickle->setOffset(myLevelInfo->SpriteOffset);						
}


///<summary>
/// Handles all the rendering code for the level ...
///</summary>
void LevelOne::renderLevel()
{	
	profileStart("LevelMethodsBeginRendering");
	LevelMethodsBeginRendering(myLevelInfo);
	profileStop("LevelMethodsBeginRendering");

	profileStart("LevelMethodsBegin2dCameraMode");
	LevelMethodsBegin2dCameraMode(myLevelInfo);
	profileStop("LevelMethodsBegin2dCameraMode");

	profileStart("LevelMethodsEnd2DCameraMode");
	LevelMethodsEnd2DCameraMode(myLevelInfo);
	profileStop("LevelMethodsEnd2DCameraMode");

	profileStart("LevelMethodsEndRendering");
	LevelMethodsEndRendering(myLevelInfo);
	profileStop("LevelMethodsEndRendering");
}


/// <summary>
/// Performs a nice little intro for the upcoming boss...
/// </summary>
void LevelOne::renderBossIntro()
{	
	Vector lastAMapLocation = myLevelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	Vector lastDMapLocation = myLevelInfo->TileMap->getLocation(TILEMAP_LAYER_D);
	Vector initialMapLocation = lastAMapLocation;


	Vector targetMapLocation = {-116.0f, -118.0f, 0.0f};
	Vector interpolatedALocation = {0.0f, 0.0f, 0.0f};
	Vector interpolatedDLocation = {0.0f, 0.0f, 0.0f};
	
	// We need to trigger a single update to make sure our baddie renders...
	LevelMethodsUpdateTowers(myLevelInfo);
	LevelMethodsUpdateBaddies(myLevelInfo);

	// Setup our baddie for rendering during an earthquake
	Vector baddieLocation = myLevelOneSpawnPoint;
	baddieLocation.z += 8.0f;
	myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).translateSprite(baddieLocation);	
	
	myLevelInfo->Sounds[PICKLE_DAMAGE]->playOnce();
	myLevelInfo->TileMap->shakeMapFor(3, 0.1f);	

	// Swing camera over to where the bad guy is ...

	for(float interpolation = 0.0f; interpolation <= 1.0f; interpolation += 0.0025f)
	{				
		targetMapLocation.z = lastAMapLocation.z;
		fastVectorSubtract(&targetMapLocation, &lastAMapLocation, &interpolatedALocation);				
		fastVectorScale(&interpolatedALocation, &interpolatedALocation, interpolation);
		fastVectorAdd(&interpolatedALocation, &lastAMapLocation, &interpolatedALocation);				
		
		targetMapLocation.z = lastDMapLocation.z;
		fastVectorSubtract(&targetMapLocation, &lastDMapLocation, &interpolatedDLocation);				
		fastVectorScale(&interpolatedDLocation, &interpolatedDLocation, interpolation);
		fastVectorAdd(&interpolatedDLocation, &lastDMapLocation, &interpolatedDLocation);
		
		myLevelInfo->SpriteOffset = myLevelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
		myLevelInfo->SpriteOffset.z = 0.0f;
		myLevelInfo->Graphics->setCameraTarget(VEC_INIT(0.0f, 0.0f, 20.0f), 270.0f, 30.0f);
		
		renderCameraSweep(initialMapLocation, interpolatedALocation, interpolatedDLocation);

		if((interpolation >= 0.5f) && (interpolation <= 0.51f))
		{
			myLevelInfo->TileMap->shakeMapFor(3, 0.20f);	
			myLevelInfo->Sounds[PICKLE_DAMAGE]->playOnce();
			LevelMethodsHandleWarpEffects(myLevelInfo, &myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex));
			LevelMethodsHandleSplashEffects(myLevelInfo, &myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex), 10);
		}
	}

	
	myLevelInfo->Sounds[PICKLE_DAMAGE]->playOnce();
	LevelMethodsHandleSplashEffects(myLevelInfo, &myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex), 10);
	LevelMethodsHandleWarpEffects(myLevelInfo, &myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex));
	myLevelInfo->TileMap->shakeMapFor(12, 0.25);	
	
	// Render the baddie rising from the depths after the earthquake...
	for(float interpolation = 0.0f; interpolation <= 1.0f; interpolation += 0.0025f)
	{						
		if(baddieLocation.z > myLevelOneSpawnPoint.z)
		{
			baddieLocation.z -= (0.04f);
			myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setOffset(myLevelInfo->SpriteOffset);			
			myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).translateSprite(baddieLocation);			
		}				

		if( ((interpolation >= 0.25f) && (interpolation <= 0.2550f))
			|| ((interpolation >= 0.5f) && (interpolation <= 0.5050f))
			|| ((interpolation >= 0.75f) && (interpolation <= 0.7550f)) )
		{
			LevelMethodsHandleWarpEffects(myLevelInfo, &myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex));
			myLevelInfo->Sounds[PICKLE_DAMAGE]->playOnce();
		}

		myLevelInfo->Graphics->setCameraTarget(VEC_INIT(0.0f, 0.0f, 20.0f), 270.0f, 30.0f);
		LevelMethodsHandleSplashEffects(myLevelInfo, &myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex), 10);
		renderCameraSweep(initialMapLocation, interpolatedALocation, interpolatedDLocation);
	}

	// Swing the camera back over to our good guy ...
	for(float interpolation = 1.0f; interpolation >= 0.0f; interpolation -= 0.008f)
	{		
		targetMapLocation.z = lastAMapLocation.z;
		fastVectorSubtract(&targetMapLocation, &lastAMapLocation, &interpolatedALocation);				
		fastVectorScale(&interpolatedALocation, &interpolatedALocation, interpolation);
		fastVectorAdd(&interpolatedALocation, &lastAMapLocation, &interpolatedALocation);				
		
		targetMapLocation.z = lastDMapLocation.z;
		fastVectorSubtract(&targetMapLocation, &lastDMapLocation, &interpolatedDLocation);				
		fastVectorScale(&interpolatedDLocation, &interpolatedDLocation, interpolation);
		fastVectorAdd(&interpolatedDLocation, &lastDMapLocation, &interpolatedDLocation);

		myLevelInfo->SpriteOffset = myLevelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
		myLevelInfo->SpriteOffset.z = 0.0f;
		myLevelInfo->BaddieDetails.Sprites.at(myLevelInfo->BaddieDetails.ReleaseIndex).setOffset(myLevelInfo->SpriteOffset);			
		myLevelInfo->Graphics->setCameraTarget(myLevelInfo->PlayerOneSprite->getLocation(), myLevelInfo->PlayerOneSprite->getAngle(), 30.0f);

		renderCameraSweep(initialMapLocation, interpolatedALocation, interpolatedDLocation);
	}

	myLevelInfo->PlayerOneSprite->setOffset(VEC_INIT(0.0f, 0.0f, 0.0f));
	
	// Reset our map to it's original location
	myLevelInfo->TileMap->translateMap(TILEMAP_LAYER_A, lastAMapLocation);
	myLevelInfo->TileMap->translateMap(TILEMAP_LAYER_D, lastDMapLocation);
	myLevelInfo->SpriteOffset = myLevelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	myLevelInfo->SpriteOffset.z = 0.0f;		
}

///<summary>
/// Allocate scenery sprites for this level (they should be deallocated at the end of the level)
///</summary>
void allocateScenerySprites(LevelInfo &levelInfo)
{

	int numberOfSprites = 30;

	while (numberOfSprites > 0)
	{
		Vector randomValues;
		randomValues.x = (float) (rand() % 128) + 24;
		randomValues.y = (float) (rand() % 128) + 24;
		randomValues.z = levelInfo.TileMap->getZValueAt(randomValues);

		if ((randomValues.z <= 19.9f) && (randomValues.z >= 15.0f))
		{
			SpriteScenery newSprite;

			int type = rand() % 4;

			if (type == 1)
			{
				LevelMethodsSetupSprite(levelInfo.Models[FLOWER_C_MODEL], &newSprite, ATTACK_TYPE_NONE, randomValues.x, randomValues.y, randomValues.z - 0.42f, 1.0f);
			}
			else if (type == 2)
			{
				LevelMethodsSetupSprite(levelInfo.Models[FLOWER_A_MODEL], &newSprite, ATTACK_TYPE_NONE, randomValues.x, randomValues.y, randomValues.z - 0.42f, 1.0f);
			}
			else if (type == 3)
			{
				LevelMethodsSetupSprite(levelInfo.Models[FLOWER_B_MODEL], &newSprite, ATTACK_TYPE_NONE, randomValues.x, randomValues.y, randomValues.z - 0.42f, 1.0f);
			}
			else
			{
				LevelMethodsSetupSprite(levelInfo.Models[TREE_MODEL], &newSprite, ATTACK_TYPE_NONE, randomValues.x, randomValues.y, randomValues.z + 1.5f, 1.0f);
			}

			levelInfo.ScenerySprites.push_back(newSprite);
			numberOfSprites--;
		}
	}
}

///<summary>
/// Deallocate scenery sprites for this level
///</summary>
void deallocateScenerySprites(LevelInfo &levelInfo)
{
	levelInfo.ScenerySprites.clear();
}


///<summary>
/// Indicates that the level will loop internally handling 
/// all input, and output (audio, visual) until the user
/// either completes the level (e.g. defeats all the waves for that level)
/// or chooses to exit
///<summary>
void LevelOne::runUntilLevelIsFinished()
{		
	CONTROLLER_STATUS status = CONTROLLER_STATUS_UNKNOWN;	

	bool waveOneSetupComplete = false;

	myLevelInfo->TileMap = new TiledMaps();
	myTilemapLoaded = LevelMethodsSetupTileMap(myLevelInfo, "//assets//map//LevelOne152.sfs", -40.0, -34.0f, 20.5f);
	ASSERT(myTilemapLoaded == true);
	allocateScenerySprites(*myLevelInfo);
///////////////////////////////////////
//	THIS IS ONLY GOOD FOR TESTING THE FRUSTRUM CULLING!
/*	float yLocation = 0.0f;
		float xLocation = 0.0f;
		int spriteIndex = 0;
		while(spriteIndex < (NUM_L1_BADDIE_SPRITES - 1))
		{
				LevelMethodsSetupSprite(myLevelInfo->Models[SMALL_BADDIE_FISH_MODEL], &myLevelInfo->BaddieDetails.Sprites.at(spriteIndex), ATTACK_TYPE_NONE, xLocation, yLocation, 20.1f, 1.0f);
				myLevelInfo->BaddieDetails.Sprites.at(spriteIndex).setControlMagnitude(0.02f);
				myLevelInfo->BaddieDetails.Sprites.at(spriteIndex).setPointValue(LEVEL_ONE_BADDIE_VALUE);
				myLevelInfo->BaddieDetails.Sprites.at(spriteIndex).setHealth(LEVEL_ONE_BADDIE_VALUE);
				myLevelInfo->BaddieDetails.Sprites.at(spriteIndex).clearWaypoints();
				myLevelInfo->BaddieDetails.Sprites.at(spriteIndex).addWaypoint((Vector){xLocation, yLocation, 20.1f});
				xLocation += 16.0f;
				if(xLocation > 128.0f)
				{
					xLocation = 0.0f;
					yLocation += 16.0f;
				}
				spriteIndex++;
		}
		myLevelInfo->BaddieDetails.ReleaseIndex = NUM_L1_BADDIE_SPRITES - 1;
*/
//////////////////////////////////


	while(myLevelInfo->FinishedLooping == false)	
	{
		// This is our input code, and should always be run
		myLevelInfo->FinishedLooping = LevelMethodsHandleInput(0, status, myLevelInfo);

		// If the user wishes to see the profiler data they press minus :D
		if(myLevelInfo->Controllers->checkButtonPress(0, WII_BUTTON_MINUS) == true)
		{
			outputProfilingInformation();
			myLevelInfo->Graphics->beginScene();
			while(myLevelInfo->Controllers->checkButtonPress(0, WII_BUTTON_A) == false)
			{
				outputProfilingInformation();
				LevelMethodsHandleInput(0, status, myLevelInfo);
			}
		}

		if(myLevelInfo->Controllers->checkButtonPress(0, WII_BUTTON_1) == true)
		{
			renderWaveReadyMessage(myLevelInfo, myLevelInfo->BaddieDetails.WaveIndex, "Green Slimey Slimers");
			//renderWaveCompleteMessage(myLevelInfo, myLevelInfo->BaddieDetails.WaveIndex);
			//NewTowerMenu newTowerMenu(myLevelInfo);
			//newTowerMenu.executeMenu(myLevelInfo);
			int animation = rand() % 3;
			if(animation == 0) myLevelInfo->PlayerOneSprite->setAnimation(FLIP);
			else if(animation == 1) myLevelInfo->PlayerOneSprite->setAnimation(SALUTE);
			else if(animation == 2) myLevelInfo->PlayerOneSprite->setAnimation(WAVE);
			//LevelMethodsAddFloatingText(myLevelInfo, gTextLocation, gTextVelocity, 0.5f, "Power Up!");						
		}

		if(myLevelInfo->Controllers->checkButtonPress(0, WII_BUTTON_2) == true)
		{
			renderWaveReadyMessage(myLevelInfo, myLevelInfo->BaddieDetails.WaveIndex, "Green Slimers Go!!!");
			renderWaveCompleteMessage(myLevelInfo, myLevelInfo->BaddieDetails.WaveIndex);
		}

		//printf("\x1b[2;0H"); printf("gTimeScale: %f", gTimeScale);

		profileStart("updateLevel");
		updateLevel();
		profileStop("updateLevel");

		profileStart("renderLevel");
		renderLevel();
		profileStop("renderLevel");

		if (waveOneSetupComplete == false)
		{
			waveOneSetupComplete = true;
			handleWaveSetup(myLevelInfo, BADDIES_IN_WAVE_1, "Slimey Scouts!");
		}
		else
		{
			releaseBaddiesOnASchedule(myLevelInfo->BaddieDetails.ReleaseTime);
		}

	}// end while	

	// Print out some memory usage information...
	
	/*MemTrack::TrackListMemoryUsage();	
	myLevelInfo->FinishedLooping = false;
	while(myLevelInfo->FinishedLooping == false)
	{
		myLevelInfo->Graphics->beginScene();
		myLevelInfo->FinishedLooping = LevelMethodsHandleInput(0, status, myLevelInfo);		
		myLevelInfo->Graphics->endScene();
	}*/
	deallocateScenerySprites(*myLevelInfo);
	delete myLevelInfo->TileMap;
}


