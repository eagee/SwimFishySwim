	// LevelMethods.cpp
// Provides delegate implementation details for LevelMethods header -> Incorporated by all ILevel implementations
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IControllers.h"
#include "IGraphics.h"
#include "IText.h"
#include "IModel.h"
#include "IAudio.h"
#include "ILevel.h"
#include "LevelInfo.h"
#include "IGameMenu.h"
#include "ModelMd2.h"
#include "ModelImageSprite.h"
#include "SpritePlayer.h"
#include "SpriteBaddie.h"
#include "SpriteTarget.h"
#include "SpriteTower.h"
#include "SpriteBullet.h"
#include "SpritePowerup.h"
#include "IProjectileWeapon.h"
#include "ProjectileShot.h"
#include "TiledMaps.h"
#include "SpriteInfo.h"
#include "LevelMethods.h"
#include "Profiler.h"
#include "InGameMenu.h"
#include "GameMenuMethods.h"

#ifdef PLATFORM_IS_WII
	#include "WiiAudioMp3.h"
	#include "WiiAudioPcm.h"
#endif
#ifdef PLATFORM_IS_WIN
#endif

// This will be used to display the tilemap for any level 
#define NUMBER_OF_WATER_TEXTURES 15
static TextureHandle gTilemapTexture = UNDEFINED_TEXTURE_HANDLE;
static TextureHandle gWaterTexture = UNDEFINED_TEXTURE_HANDLE;
static TextureHandle gTilemapBumpTexture = UNDEFINED_TEXTURE_HANDLE;
static TextureHandle gLightmapTexture = UNDEFINED_TEXTURE_HANDLE;
static TextureHandle gGunExpTexture = UNDEFINED_TEXTURE_HANDLE;
static TextureHandle gWaterNormalTextures[NUMBER_OF_WATER_TEXTURES];
static TextureHandle gTitleScreenTexture = UNDEFINED_TEXTURE_HANDLE;

// Used for displaying a loading dialog
static float gLoadProgress = 0.0f;
static float gAddProgress = 0.1f;

typedef enum tagCHANGE_WEAPON_BUTTON
{
	CHANGE_WEAPON_NONE,
	CHANGE_WEAPON_PRESSED	
} CHANGE_WEAPON_BUTTON;

static CHANGE_WEAPON_BUTTON gChangeButtonState = CHANGE_WEAPON_NONE;

// This will be used to display health of our pickle on any level ...
static TextureHandle gHealthTexture = UNDEFINED_TEXTURE_HANDLE;
// This will be used to display radar imagaes
static TextureHandle gRadarPointTexture = UNDEFINED_TEXTURE_HANDLE;
// This will be used to display a skybox for all of the levels (we can overwrite this memory location if we need to update)
static TextureHandle gSkyboxTexture = UNDEFINED_TEXTURE_HANDLE;
static TextureHandle gSceneboxTexture = UNDEFINED_TEXTURE_HANDLE;
// This defines an incremental ID for each sprite in our level system
static int gSpriteIDCounter = 1;

// Some values we can use to draw the health bar ...
#define HEALTH_TOP		 0.0f
#define HEALTH_BOTTOM	 0.2f
#define HEALTH_LEFT		 0.0f
#define HEALTH_RIGHT	 2.0f

// Some values we can use for drawing the radar
#define RADAR_LOCATION			VEC_INIT(-0.065f, 0.010f, 0.15f)
#define RADAR_TOP				 0.0f
#define RADAR_BOTTOM			 0.0275f
#define RADAR_LEFT				 0.0f
#define	RADAR_RIGHT				 0.0275f
#define RADAR_DOT_PIXEL_SIZE	 42
// These multipliers define what 1.0 is based on the size of the radar values above
#define RADAR_X_MULTIPLIER (0.0001953125f)
#define RADAR_Y_MULTIPLIER (0.0001953125f)


// Some values we can use for drawing the radar
#define SPLASH_LOCATION			VEC_INIT(0.0f, 0.0f, 3.0f)
#define SPLASH_TOP				 -1.25f
#define SPLASH_BOTTOM			 1.25f
#define SPLASH_LEFT				 -1.8f
#define	SPLASH_RIGHT			 1.8f



// Used to determine if a vector is inside of a bounding box. Returns true if it is, returns false if not...
static bool vectorIsInsideBoundingBox(Vector testVector, BoundingBox boundingBox)
{
	if((testVector.x >= boundingBox.left) && (testVector.x <= boundingBox.right)
		&& (testVector.y >= boundingBox.top) && (testVector.y <= boundingBox.bottom))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Used to determine if two vectors intersect a bounding box. Returns true if they do, returns false if not...
static bool vectorIntersectsBoundingBox(Vector testVectorA, Vector testVectorB, BoundingBox boundingBox)
{
	// Check to see if an intersection occurs by always making sure that one of the two points
	// is within one of the hight/width combinations of the bounding box...
	if( ((testVectorA.x >= boundingBox.left) && (testVectorA.x <= boundingBox.right) && (testVectorB.y >= boundingBox.top) && (testVectorB.y <= boundingBox.bottom))
		|| ((testVectorB.x >= boundingBox.left) && (testVectorB.x <= boundingBox.right) && (testVectorA.y >= boundingBox.top) && (testVectorA.y <= boundingBox.bottom)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/// <summary>
/// Indicates one of six directions an angle is facing ...
/// <summary>
DIRECTION LevelMethodsGetAngleDirection(float angle, float magnitude)
{
	DIRECTION returnValue = DIRECTION_NONE;
		
	// If our joystick is pressed hard enough and in the right angle return true, otherwise false
	if(magnitude >= 0.25f)
	{
		if((angle >= 315.0f) || (angle < 45.0f))
		{
			returnValue = DIRECTION_NORTH;
		}
		else if((angle >= 45.0f) && (angle < 135.0f))
		{
			returnValue = DIRECTION_EAST;
		}
		else if((angle >= 135.0f) && (angle <=225.0f))
		{
			returnValue = DIRECTION_SOUTH;
		}
		else if((angle >= 225.0f) && (angle < 315.0f))
		{
			returnValue = DIRECTION_WEST;
		}
	}
	else
	{
		returnValue = DIRECTION_NONE;
	}
	return returnValue;
}

/// <summary>
/// Allocates the graphic used with tilemaps
/// </summary>
void LevelMethodsPreAllocateTilemapGraphic(IGraphics *graphics, const char *textureMap)
{
	// Load the gTilemapTexture value here if it hasn't been loaded yet ...
	if(gTilemapTexture == UNDEFINED_TEXTURE_HANDLE)
	{
		//printf("Preallocating tile and water graphics: %s \n", textureMap);
		gWaterNormalTextures[ 0] = graphics->addTextureData("//assets//tex128//waterBump001.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 1] = graphics->addTextureData("//assets//tex128//waterBump002.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 2] = graphics->addTextureData("//assets//tex128//waterBump003.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 3] = graphics->addTextureData("//assets//tex128//waterBump004.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 4] = graphics->addTextureData("//assets//tex128//waterBump005.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 5] = graphics->addTextureData("//assets//tex128//waterBump006.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 6] = graphics->addTextureData("//assets//tex128//waterBump007.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 7] = graphics->addTextureData("//assets//tex128//waterBump008.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 8] = graphics->addTextureData("//assets//tex128//waterBump009.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[ 9] = graphics->addTextureData("//assets//tex128//waterBump010.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[10] = graphics->addTextureData("//assets//tex128//waterBump011.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[11] = graphics->addTextureData("//assets//tex128//waterBump012.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[12] = graphics->addTextureData("//assets//tex128//waterBump013.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[13] = graphics->addTextureData("//assets//tex128//waterBump014.png", NORMAL_MAP_TEXTURE, true);
		gWaterNormalTextures[14] = graphics->addTextureData("//assets//tex128//waterBump015.png", NORMAL_MAP_TEXTURE, true);
		gWaterTexture = graphics->addTextureData("//assets//tex128//waterA.png", IMAGE_TEXTURE, true);
		gTilemapBumpTexture = graphics->addTextureData("//assets//texOther//mapTypeABump.png", NORMAL_MAP_TEXTURE, false);
		gTilemapTexture = graphics->addTextureData(textureMap, IMAGE_TEXTURE, false);								
	}	
		//gWaterLayerHeightTexture = graphics->addTextureData("//water1Height.bin", HEIGHT_TEXTURE, true);
		//gWaterLayerTwoTexture = graphics->addTextureData("//waterHighlight.png", IMAGE_TEXTURE, true);

}

/// <summary>
/// Allocates the graphic used for drawing the level skybox...
/// </summary>
void LevelMethodsPreAllocateSkyboxTexture(IGraphics *graphics, const char *skyboxFile, const char *sceneboxFile)
{	

	gLightmapTexture = graphics->addTextureData("//assets//tex64//lightProbeA.png", LIGHT_MAP_TEXTURE, false);

	// Load the gTilemapTexture value here if it hasn't been loaded yet ...
	if(gSkyboxTexture == UNDEFINED_TEXTURE_HANDLE)
	{
		//printf("Preallocating skybox: %s \n", fileName);
		gSkyboxTexture = graphics->addTextureData(skyboxFile, IMAGE_TEXTURE, false);
	}

	// Load the gTilemapTexture value here if it hasn't been loaded yet ...
	if(gSceneboxTexture == UNDEFINED_TEXTURE_HANDLE)
	{
		//printf("Preallocating skybox: %s \n", fileName);
		gSceneboxTexture = graphics->addTextureData(sceneboxFile, IMAGE_TEXTURE, false);
	}
}

/// <summary>
/// Renders a skybox to the scene using the preallocated skybox texture.
/// Will assert if the texture has not been loaded ...
/// </summary>
void LevelMethodsRenderSkybox(PROJECTION_TYPE projectionType, IGraphics *graphics)
{	
	ASSERT(gSkyboxTexture != UNDEFINED_TEXTURE_HANDLE);
	// Load the gSkyboxTexture value here if it hasn't been loaded yet ...
	if(gSkyboxTexture != UNDEFINED_TEXTURE_HANDLE)
	{
		graphics->addSkyboxToScene(projectionType, gSkyboxTexture, gSceneboxTexture);
	}
}

/// <summary>
/// Takes a null tilemap reference and turns it into something *beautiful*
/// </summary>
bool LevelMethodsSetupTileMap(LevelInfo *levelInfo, const char *fileName, float translateX, float translateY, float translateZ)
{	
	//printf("Loading tilemap Level: %s\n", fileName);

	bool mapLoaded = levelInfo->TileMap->loadTileMap(fileName);
	
	if(mapLoaded == true) 
	{
		ASSERT(gTilemapTexture != UNDEFINED_TEXTURE_HANDLE);		
		ASSERT(gWaterTexture != UNDEFINED_TEXTURE_HANDLE);
		ASSERT(gLightmapTexture != UNDEFINED_TEXTURE_HANDLE);
		ASSERT(gTilemapBumpTexture != UNDEFINED_TEXTURE_HANDLE);		
		levelInfo->TileMap->assignTextures(gTilemapTexture, gWaterTexture, gTilemapBumpTexture, gLightmapTexture, gWaterNormalTextures[0]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[1]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[2]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[3]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[4]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[5]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[6]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[7]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[8]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[9]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[10]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[11]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[12]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[13]);
		levelInfo->TileMap->addWaterNormalMap(gWaterNormalTextures[14]);		
		
		//TODO: This is where we would go rite on ahead and load all the textures associated with the normal mapping.

		// Set the minimum boundary for our map. This would correspond to 0,0 in a 2d pixel based engine...
		// The Z value is currently ignored, but could be modified in the future for scaling...
		levelInfo->TileMap->setMinimumBoundary(VEC_INIT(-12.0f, -12.0f, 50.0f));

		// Translate our tilemap
		levelInfo->TileMap->translateMap(TILEMAP_LAYER_A, 	VEC_INIT(translateX, translateY, translateZ + 2.798f));
		levelInfo->TileMap->translateMap(TILEMAP_LAYER_B, 	VEC_INIT(translateX, translateY, translateZ + 2.797f));
		levelInfo->TileMap->translateMap(TILEMAP_LAYER_C, 	VEC_INIT(translateX, translateY, translateZ + 0.30f));		
		levelInfo->TileMap->translateMap(TILEMAP_LAYER_D, 	VEC_INIT(translateX, translateY, translateZ + 0.05f));		

		levelInfo->TileMap->generateLayerVertexArrays(levelInfo->Graphics, TILEMAP_LAYER_A);
		levelInfo->TileMap->generateLayerVertexArrays(levelInfo->Graphics, TILEMAP_LAYER_C);
		levelInfo->TileMap->generateLayerVertexArrays(levelInfo->Graphics, TILEMAP_LAYER_D);
	}
	
	return mapLoaded;
}

///<summary>
/// Handles the generation of ripple effects for the specified sprite
///<summary>
void LevelMethodsHandleRippleEffects(LevelInfo *levelInfo, ISprite *sprite)
{
	Vector rippleLocation = sprite->getLocation();

	// See if the baddie needs a ripple to appear under its location
	if (sprite->hasPeriodicTimerExpired() == true)
	{
		levelInfo->RippleParticles->addParticles(sprite, levelInfo->SpriteOffset, 1);
	}
}

///<summary>
/// Handles the generation of ripple effects for the specified sprite
///<summary>
void LevelMethodsHandleRippleSplashEffects(LevelInfo *levelInfo, ISprite *sprite)
{
	Vector rippleLocation = sprite->getLocation();

	// See if the baddie needs a ripple to appear under its location
	if (sprite->hasPeriodicTimerExpired() == true)
	{
		levelInfo->RippleParticles->addParticles(sprite, levelInfo->SpriteOffset, 1);
		levelInfo->SplashParticles->addParticles(sprite, levelInfo->SpriteOffset, 3);
	}
}

///<summary>
/// Handles the generation of repel effects for the specified sprite
///<summary>
void LevelMethodsHandleRepelEffects(LevelInfo *levelInfo, ISprite *sprite)
{
	levelInfo->RepelParticles->addParticles(sprite, levelInfo->SpriteOffset, 1);
	// Add a ripple as well b/c we're spinning :)
	levelInfo->RippleParticles->addParticles(sprite, levelInfo->SpriteOffset, 1);
}

///<summary>
/// Handles the generation of repel effects for the specified sprite
///<summary>
void LevelMethodsHandleWarpEffects(LevelInfo *levelInfo, ISprite *sprite)
{
	levelInfo->WarpParticles->addParticles(sprite, levelInfo->SpriteOffset, 1);
	// Add a ripple as well b/c we're spinning :)
	levelInfo->WarpParticles->addParticles(sprite, levelInfo->SpriteOffset, 1);
}

///<summary>
/// Handles the generation of spark effects for the specified sprite
///<summary>
void LevelMethodsHandleSparkEffects(LevelInfo *levelInfo, ISprite *sprite, int number)
{
	levelInfo->SparkParticles->addParticles(sprite, levelInfo->SpriteOffset, number);
}

///<summary>
/// Handles the generation of splash effects for the specified sprite
///<summary>
void LevelMethodsHandleSplashEffects(LevelInfo *levelInfo, ISprite *sprite, int number)
{
	levelInfo->SplashParticles->addParticles(sprite, levelInfo->SpriteOffset, number);
}


/// <summary>
/// Activates an available powerup sprite so that the player can pick up a little extra power :)
/// </summary>
void LevelMethodsActivatePowerup(ISprite *baddieSprite, LevelInfo *levelInfo, SPRITE_POWER_UP powerUpLevel)
{
	if(levelInfo->PowerUps.PowerUpSprites.at(levelInfo->PowerUps.CurrentPowerupIndex).isDead() == true)
	{			
		ASSERT(powerUpLevel != POWER_UP_NONE);
		if(powerUpLevel != POWER_UP_NONE)
		{
			Vector newLocation = baddieSprite->getLocation();		
			newLocation.z = baddieSprite->getWaterLevel();
			newLocation.x -= levelInfo->TileMap->getLocation(TILEMAP_LAYER_C).x;
			newLocation.y -= levelInfo->TileMap->getLocation(TILEMAP_LAYER_C).y;				

			DIRECTION newDirection = LevelMethodsGetAngleDirection(baddieSprite->getControlAngle(), 1.0f);
			ISprite *powerup = &levelInfo->PowerUps.PowerUpSprites.at(levelInfo->PowerUps.CurrentPowerupIndex);		
			powerup->setAnimation(STAND);
			powerup->translateSprite(newLocation);
			powerup->setOffset(levelInfo->SpriteOffset);
			powerup->setAngle(baddieSprite->getControlAngle());
			powerup->setDirection(newDirection);										
			powerup->setControlMagnitude(0.0f);	
			powerup->setLevel((int)powerUpLevel);
			powerup->setHealth(10.5f);	
			powerup->setPointValue((int)powerUpLevel * 5);
			powerup->setLevel(powerUpLevel);
			powerup->handleJump(-18.0f);

			levelInfo->PowerUps.CurrentPowerupIndex++;
			if(levelInfo->PowerUps.CurrentPowerupIndex >= MAXIMUM_POWERUPS)
			{
				levelInfo->PowerUps.CurrentPowerupIndex = 0;
			}
		}
	}	



}

/// <summary>
/// Switches to another projectile weapon by the index, ensuring the index doesn't go out of range ...
/// </summary>
static void switchProjectileWeapon(LevelInfo *levelInfo, int changeIndexBy)
{
	levelInfo->CurrentProjectileWeapon += changeIndexBy;
	if(levelInfo->CurrentProjectileWeapon >= TOTAL_PROJECTILE_TYPES)
	{
		levelInfo->CurrentProjectileWeapon = 0;
	}
	else if(levelInfo->CurrentProjectileWeapon < 0)
	{
		levelInfo->CurrentProjectileWeapon = (TOTAL_PROJECTILE_TYPES - 1);
	}
	levelInfo->ProjectileWeapon = levelInfo->ProjectileWeapons[levelInfo->CurrentProjectileWeapon];
}

/// <summary>
/// // Updates the controller state and modifies actions based on controller state, returns true if home is pressed.
/// </summary>
bool LevelMethodsHandleInput(int controllerID, CONTROLLER_STATUS &status, LevelInfo *levelInfo)
{		
	bool doneLooping = false;
	
	levelInfo->Controllers->updateControllerStates();
	
	status = levelInfo->Controllers->checkControllerStatus(controllerID);

	if(status == CONTROLLER_READY)
	{
		// See if the user wishes to end this level ...
		if(levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_HOME) == true)
		{
			doneLooping = true;
		}

		//TODO: Remove/Modify these so that cam dist does not change
		if(levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_UP) == true)
		{
			levelInfo->InGameMenu->clickButton(levelInfo, CHOMP_TOWER_BUTTON);
			//levelInfo->Graphics->adjustCameraDistance(-1.0f);
		}
		else if(levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_DOWN) == true)
		{
			levelInfo->InGameMenu->clickButton(levelInfo, SPITTING_TOWER_BUTTON);
			//levelInfo->Graphics->adjustCameraDistance(1.0f);
		}
		else if(levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_LEFT) == true)
		{
			levelInfo->InGameMenu->clickButton(levelInfo, WIND_TOWER_BUTTON);
			//levelInfo->Graphics->adjustCameraRadius(1.0f);
		}
		else if(levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_RIGHT) == true)
		{
			levelInfo->InGameMenu->clickButton(levelInfo, THROWING_TOWER_BUTTON);
			//levelInfo->Graphics->adjustCameraRadius(-1.0f);
		}
		else
		{
			// Sending -1 will trigger all buttons to be set as released...
			levelInfo->InGameMenu->clickButton(levelInfo, NO_TOWER_BUTTON_SELECTED);
		}

		// Trigger a break point if we hit plus...
		if(levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_PLUS) == true)
		{
#ifdef PLATFORM_IS_WII
			_break();			
#endif
		}		

		// Handle the trigger state of our current weapon...
		if (levelInfo->CurrentEnergyLevel >= 1.0f)
		{
			if (levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_B) == true)
			{
				levelInfo->ProjectileWeapon->handleTriggerState(PROJECTILE_TRIGGER_PRESSED, levelInfo->PlayerOneSprite, levelInfo->SpriteOffset);

			}
			else
			{
				if (levelInfo->ProjectileWeapon->handleTriggerState(PROJECTILE_TRIGGER_RELEASED, levelInfo->PlayerOneSprite, levelInfo->SpriteOffset) == true)
				{
					LevelMethodsModifyEnergy(levelInfo, -100.0f);
				}
			}
		}

		// If player presses C on the nunchuck, then cycle through the available weapons...
		if(levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_C) == true)
		{			
			gChangeButtonState = CHANGE_WEAPON_PRESSED;
		}	
		else if (gChangeButtonState == CHANGE_WEAPON_PRESSED)
		{
			switchProjectileWeapon(levelInfo, 1);
			gChangeButtonState = CHANGE_WEAPON_NONE;
		}
		
		if(levelInfo->Controllers->checkButtonPress(controllerID, WII_BUTTON_Z) == true)
		{
			levelInfo->PlayerOneSprite->handleJump(-20.0f);
		}

		// Check for a swing to trigger a physical attack...
		if(levelInfo->Controllers->checkButtonPress(controllerID, WII_HORIZONTAL_SWING_RIGHT) == true)
		{						
			if((levelInfo->PlayerOneSprite->getCurrentAnimation() != ATTACK_A)
				&& (levelInfo->PlayerOneSprite->getCurrentAnimation() != REVERSE_ATTACK))

			{
				levelInfo->Sounds[SPIN_ATTACK]->playOnce();
				levelInfo->PlayerOneSprite->setAnimation(ATTACK_A);
				LevelMethodsHandleSplashEffects(levelInfo, levelInfo->PlayerOneSprite, 32);
				LevelMethodsHandleRepelEffects(levelInfo, levelInfo->PlayerOneSprite);
			}
			
		}
		// Check for a swing to trigger a physical attack...
		else if(levelInfo->Controllers->checkButtonPress(controllerID, WII_HORIZONTAL_SWING_LEFT) == true)
		{
			if((levelInfo->PlayerOneSprite->getCurrentAnimation() != ATTACK_A)
				&& (levelInfo->PlayerOneSprite->getCurrentAnimation() != REVERSE_ATTACK))
			{
				levelInfo->Sounds[SPIN_ATTACK]->playOnce();
				levelInfo->PlayerOneSprite->setAnimation(REVERSE_ATTACK);
				LevelMethodsHandleSplashEffects(levelInfo, levelInfo->PlayerOneSprite, 32);
				LevelMethodsHandleRepelEffects(levelInfo, levelInfo->PlayerOneSprite);
			}

		}
		

		if((levelInfo->PlayerOneSprite->getJump() != JUMP_STATE_LANDING))
		{
		// Obtain the joystick angle, then modify it to adjust for the camera angle
		float joystickAngle = levelInfo->Controllers->checkJoystickAngle(controllerID);		
		//printf("\x1b[2;0H"); printf("JoyAngle: %f, CameraAngle: %f\n", joystickAngle, levelInfo->Graphics->getCameraAngle());	
		joystickAngle = adjustInputAngleByCameraAngle(joystickAngle, levelInfo->Graphics->getCameraAngle());
		//printf("NewAngle: %f", joystickAngle);			
		float joystickMagnitude = levelInfo->Controllers->checkJoystickMagnitude(controllerID);		

		if((joystickMagnitude >= 0.250f) && (joystickMagnitude != NAN))
		{
			// Adjust our maximum velocity by the joystick magnitude, so that the player can go slower if they want			
			DIRECTION currentDirection = LevelMethodsGetAngleDirection(joystickAngle, joystickMagnitude);
			levelInfo->PlayerOneSprite->setDirection(currentDirection);		
			if(currentDirection != DIRECTION_NONE)
			{
				levelInfo->PlayerOneSprite->increaseAcceleration();
			}
			levelInfo->PlayerOneSprite->setAngle(joystickAngle);
			levelInfo->PlayerOneSprite->setControlMagnitude(joystickMagnitude);
		}
		}
	}
	//TODO: We need a dialog that informs the user that the controller is not responding...
	return doneLooping;
}

///<summary>
/// Pre allocate model and texture information from a file ...
///</summary>
bool LevelMethodsPreAllocateModelMd2(LevelInfo *levelInfo, float scale, const char *modelFile, const char *graphicFile, MODEL_TYPE modelType, AnimationFormat animationFormat, bool cullBackfaces)
{
	bool modelLoaded = false;
	//printf("Preallocating model: %s\n", modelFile);		
	// Load the texture associated with this model and get a handle to it ...
	levelInfo->ModelTextures[modelType] = levelInfo->Graphics->addTextureData(graphicFile, IMAGE_TEXTURE, false);
	
	// Instantiate our new model ...
	levelInfo->Models[modelType] = new ModelMd2();
	modelLoaded = levelInfo->Models[modelType]->loadFromFile(modelFile, animationFormat);
		
	if(modelLoaded)
	{
		levelInfo->Models[modelType]->assignTexture(levelInfo->ModelTextures[modelType]);
		levelInfo->Models[modelType]->setModelScale(scale);
		levelInfo->Models[modelType]->setAnimationType(STAND);			
		levelInfo->Models[modelType]->buildVertexArrays(levelInfo->Graphics, cullBackfaces);
	}

	return modelLoaded;
}

///<summary>
/// Overloaded version that allows user to specify bump to apply to the model texture ...
///</summary>
bool LevelMethodsPreAllocateModelMd2(LevelInfo *levelInfo, float scale, const char *modelFile, const char *graphicFile, const char *bumpFile, MODEL_TYPE modelType, AnimationFormat animationFormat, bool cullBackfaces)
{
	bool modelLoaded = false;

	// We've got to make sure our light map has been loaded already by this point!
	ASSERT(gLightmapTexture != UNDEFINED_TEXTURE_HANDLE);

	// Load the bump map / normal map associated with this model and get a handle to it ...	
	levelInfo->ModelBumpTextures[modelType] = levelInfo->Graphics->addTextureData(bumpFile, NORMAL_MAP_TEXTURE, false);
	ASSERT(levelInfo->ModelBumpTextures[modelType] != UNDEFINED_TEXTURE_HANDLE);

	modelLoaded = LevelMethodsPreAllocateModelMd2(levelInfo, scale, modelFile, graphicFile, modelType, animationFormat, cullBackfaces);
	
	ASSERT(modelLoaded == true);
	
	if(modelLoaded == true)
	{
		levelInfo->Models[modelType]->assignBumpTexture(levelInfo->ModelBumpTextures[modelType], gLightmapTexture);
	}

	return modelLoaded;
}

// TODO: create another overloaded version of pre allocate md2 model that allows you to assign a texture ID as opposed to a new tex ...


///<summary>
/// Pre allocates music for a level
///</summary>
void LevelMethodsPreAllocateLevelMusic(SONG_NAME songName, LevelInfo *levelInfo, const char *fileName)
{
	//printf("Preallocating music data: %s\n", fileName);
#ifdef PLATFORM_IS_WII
	levelInfo->Songs[songName] = new WiiAudioMp3();
	levelInfo->Songs[songName]->loadFromFile(fileName);
	levelInfo->Songs[songName]->playLooped();
#endif
#ifdef PLATFORM_IS_WII
	//TODO: Implement windows sound code here (most likely midi)!
#endif
}

///<summary>
/// De allocates music object for level
///</summary>
void LevelMethodsDeAllocateLevelMusic(SONG_NAME songName, LevelInfo *levelInfo)
{
	delete levelInfo->Songs[songName];
}

///<summary>
/// Pre allocates sound for a level
///</summary>
void LevelMethodsPreAllocateLevelSound(SOUND_NAME soundName, LevelInfo *levelInfo, const char *fileName)
{
	//printf("Preallocating sound data: %s\n", fileName);
#ifdef PLATFORM_IS_WII
	levelInfo->Sounds[soundName] = new WiiAudioPcm();
	levelInfo->Sounds[soundName]->loadFromFile(fileName);
#endif
#ifdef PLATFORM_IS_WIN
	// TODO: Implement Windows PCM sound effect code...
#endif
}

///<summary>
/// De allocates sound object for level WARNING: DOES NOT DEALLOCATE SOUND DATA!!!!!
///</summary>
void LevelMethodsDeAllocateLevelSound(SOUND_NAME soundName, LevelInfo *levelInfo)
{
	delete levelInfo->Sounds[soundName];
}

///<summary>
/// Pre allocate model and texture information from a file ...
///</summary>
bool LevelMethodsPreAllocateSpriteModel(LevelInfo *levelInfo, float scale, const char *graphicFile, MODEL_TYPE modelType)
{
	//printf("Preallocating sprite model: %s\n", graphicFile);
	
	// Load the texture associated with this model and get a handle to it ...
	levelInfo->ModelTextures[modelType] = levelInfo->Graphics->addTextureData(graphicFile, IMAGE_TEXTURE, false);
		// Instantiate our new model ...
		levelInfo->Models[modelType] = new ModelImageSprite();
		levelInfo->Models[modelType]->setModelScale(scale);
		levelInfo->Models[modelType]->assignTexture(levelInfo->ModelTextures[modelType]);	
		levelInfo->Models[modelType]->setAnimationType(STAND);			
	
	return true;
}

///<summary>
/// De allocate model information before we shut down ...
///</summary>
void LevelMethodsDeAllocateModel(LevelInfo *levelInfo, MODEL_TYPE modelType)
{
	if(levelInfo->Models[modelType] != NULL)
	{
		levelInfo->Models[modelType]->freeVertexArrays(levelInfo->Graphics);
		delete levelInfo->Models[modelType];
	}
}

///<summary>
/// Assigns a model to one of our sprite characters and instantiates the sprite to it's new location...
///<summary>
void LevelMethodsSetupSprite(IModel *model, ISprite *sprite, ATTACK_TYPE attackType, float x, float y, float z, float boundingTriangleSize)
{
	//printf("Loading sprite: \n");
	ASSERT(sprite != NULL);
	// Set up our fish model ..		
	sprite->setID(gSpriteIDCounter+=10);
	sprite->assignModel(model);				
	sprite->setOffset(VEC_INIT(0.0f, 0.0f, 0.0f));			
	sprite->translateSprite(VEC_INIT(x, y, z));			
	sprite->setVelocities(ALL_SPRITE_VELOCITY, ALL_SPRITE_DRAG, ALL_SPRITE_ACCEL);
	sprite->setAttackType(attackType);
	sprite->setBoundingTriangleSize(boundingTriangleSize);
}

///<summary>
/// Returns a single Z value based on the location of a vector in the tilemap
///<summary>
static float getInterpolatedZValue(Vector spriteLocation, Vector spriteOffset, TiledMaps &map)
{
	float returnZValue;
	Vector interpolatedVector = {0.0f, 0.0f, 0.0f};
	// Interpolate our z values so it doesn't look like we're trying to swim into a wall...
	
	Vector mapLocation = map.getLocation(TILEMAP_LAYER_C);	
	interpolatedVector.x = (mapLocation.x - (spriteLocation.x + spriteOffset.x));
	interpolatedVector.y = (mapLocation.y - (spriteLocation.y + spriteOffset.y));
	returnZValue = map.getZValueAt(interpolatedVector);							
	return returnZValue; 
}

///<summary>
/// Checks collision points and handles updates accordingly with map
///<summary>
void LevelMethodsHandleTilemapCollision(ISprite &sprite, LevelInfo *levelInfo)
{
	float pointAZValue;
	float pointBZValue;
	float pointCZValue;
	float mapZLocation;

	// Get our bounding triangle, sprite location, and map location
	BoundingTriangle spriteCollision = sprite.getBoundingTriangle();		
	Vector spriteLocation = sprite.getLocation();	

	// Get an interpolated Z value for each collision point
	pointAZValue = getInterpolatedZValue(spriteLocation, spriteCollision.collisionA, *levelInfo->TileMap);
	pointBZValue = getInterpolatedZValue(spriteLocation, spriteCollision.collisionB, *levelInfo->TileMap);
	pointCZValue = getInterpolatedZValue(spriteLocation, spriteCollision.collisionC, *levelInfo->TileMap);
	mapZLocation = getInterpolatedZValue(spriteLocation, VEC_INIT(0.0f, 0.0f, 0.0f), *levelInfo->TileMap);

	if(spriteLocation.z >= sprite.getWaterLevel())
	{
		float collisionDistance = -0.25f;
	
		// Now modify those z values to represent their distance from the sprites water level ...
		float pointADistance = (pointAZValue - sprite.getWaterLevel());
		float pointBDistance = (pointBZValue - sprite.getWaterLevel());
		float pointCDistance = (pointCZValue - sprite.getWaterLevel());

		// If we hit both A and B handle a front collision (change movement by 180 degrees)
		if((pointADistance < collisionDistance) && (pointBDistance < collisionDistance) && (pointCDistance < collisionDistance))
		{
			//LevelMethodsHandleSparkEffects(levelInfo, &sprite, 1);
			sprite.handleCollision(REAR_COLLISION, 0.0f);
		}
		// If we hit both A and B handle a front collision (change movement by 180 degrees)
		if((pointADistance < collisionDistance) && (pointBDistance < collisionDistance))
		{
			//LevelMethodsHandleSparkEffects(levelInfo, &sprite, 1);
			sprite.handleCollision(FRONT_COLLISION, 0.0f);
			//sprite.setJump(JUMP_STATE_READY);
		}
		// If we hit both A and C handle a rear left collision (change movement by 45 degrees)
		if((pointADistance < collisionDistance) && (pointCDistance < collisionDistance))
		{
			//LevelMethodsHandleSparkEffects(levelInfo, &sprite, 1);
			sprite.handleCollision(LEFT_SIDE_COLLISION, 0.0f);
			//sprite.setJump(JUMP_STATE_READY);
		}
		// If we hit both B and C handle a rear right collision (change movement by 315 degrees)
		if((pointBDistance < collisionDistance) && (pointCDistance < collisionDistance))
		{
			//LevelMethodsHandleSparkEffects(levelInfo, &sprite, 1);
			sprite.handleCollision(RIGHT_SIDE_COLLISION, 0.0f);
			//sprite.setJump(JUMP_STATE_READY);
		}
		// If we hit just A handle a front left  collision (change movement by 135 degrees)
		else if(pointADistance < collisionDistance)
		{
			//LevelMethodsHandleSparkEffects(levelInfo, &sprite, 1);
			sprite.handleCollision(FRONT_LEFT_COLLISION, 0.0f);
			//sprite.setJump(JUMP_STATE_READY);
		}
		// If we hit just B handle a front right  collision (change movement by 225 degrees)
		else if(pointBDistance < collisionDistance)
		{
			//LevelMethodsHandleSparkEffects(levelInfo, &sprite, 1);
			sprite.handleCollision(FRONT_RIGHT_COLLISION, 0.0f);
			//sprite.setJump(JUMP_STATE_READY);
		}
		// If we hit just C handle a rear collision (increase forward vector but don't change movement
		else if(pointCDistance < collisionDistance)
		{
			//LevelMethodsHandleSparkEffects(levelInfo, &sprite, 1);
			sprite.handleCollision(REAR_COLLISION, 0.0f);
		}
	}
	else if((spriteLocation.z < sprite.getWaterLevel()) && (spriteLocation.z >= mapZLocation - 1.0f) )
	{
		float furthestAway = mapZLocation;
		if(pointAZValue > furthestAway) furthestAway = pointAZValue;
		if(pointBZValue > furthestAway) furthestAway = pointBZValue;
		if(pointCZValue > furthestAway) furthestAway = pointCZValue;

		if(furthestAway == pointAZValue)
		{
			sprite.handleCollision(FRONT_SLIDE, 0.0f);
			if(mapZLocation >= sprite.getWaterLevel()) sprite.handleJump(-0.1f);
		}
		else if(furthestAway == pointBZValue)
		{
			sprite.handleCollision(FRONT_SLIDE, 0.0f);
			if(mapZLocation >= sprite.getWaterLevel()) sprite.handleJump(-0.1f);
		}
		else if(furthestAway == pointCZValue)
		{
			sprite.handleCollision(REAR_SLIDE, 0.0f);
			if(mapZLocation >= sprite.getWaterLevel()) sprite.handleJump(-0.1f);
		}
		else
		{
			levelInfo->PlayerOneSprite->handleJump(-14.0f);
		}
	}
	spriteLocation.z = mapZLocation;
	sprite.setLocation(spriteLocation);

}

///<summary>
/// Updates temporary collision points for a sprite (assigns to ID + 4)
///<summary>
void LevelMethodsSetupTempCollision(ISprite &sprite, TiledMaps &map)
{
	Vector mapLocation = map.getLocation(TILEMAP_LAYER_C);	
	Vector fishLocation = sprite.getLocation();
	BoundingTriangle spriteCollision = sprite.getBoundingTriangle();

	// Add / Update this sprite in our temp collision table ..
	Vector collisionCheck;	
	Vector interpolatedCheck;

	collisionCheck.x = (mapLocation.x - fishLocation.x);
	collisionCheck.y = (mapLocation.y - fishLocation.y);
	collisionCheck.z = 0.0f;
	map.setTempCollisionAt(sprite.getID(), collisionCheck);
	
	collisionCheck.x = mapLocation.x - (fishLocation.x + spriteCollision.collisionA.x);
	collisionCheck.y = mapLocation.y - (fishLocation.y + spriteCollision.collisionA.y);
	map.setTempCollisionAt(sprite.getID()+1, collisionCheck);

	// add a collision interpolated between A and B	
	interpolatedCheck.x = mapLocation.x - (fishLocation.x +  spriteCollision.collisionA.x + ((spriteCollision.collisionA.x - spriteCollision.collisionC.x) /2));
	interpolatedCheck.y = mapLocation.y - (fishLocation.y +  spriteCollision.collisionA.y + ((spriteCollision.collisionA.y - spriteCollision.collisionC.y) /2));
	map.setTempCollisionAt(sprite.getID()+2, interpolatedCheck);
	
	collisionCheck.x = mapLocation.x - (fishLocation.x + spriteCollision.collisionB.x);
	collisionCheck.y = mapLocation.y - (fishLocation.y + spriteCollision.collisionB.y);
	map.setTempCollisionAt(sprite.getID()+3, collisionCheck);

	// add a collision interpolated between B and C
	interpolatedCheck.x = mapLocation.x - (fishLocation.x +  spriteCollision.collisionB.x + ((spriteCollision.collisionB.x - spriteCollision.collisionC.x) /2));
	interpolatedCheck.y = mapLocation.y - (fishLocation.y +  spriteCollision.collisionB.y + ((spriteCollision.collisionB.y - spriteCollision.collisionC.y) /2));
	map.setTempCollisionAt(sprite.getID()+4, interpolatedCheck);
	
	collisionCheck.x = mapLocation.x - (fishLocation.x + spriteCollision.collisionC.x);
	collisionCheck.y = mapLocation.y - (fishLocation.y + spriteCollision.collisionC.y);
	map.setTempCollisionAt(sprite.getID()+5, collisionCheck);
}


///<summary>
/// Perform pathfinding operations for a sprite - creating angle and movement information
///<summary>
void LevelMethodsFindSpritePath(ISprite &sprite, Vector fromLocation, Vector toLocation, TiledMaps &map)
{	
	Vector mapLocation = map.getLocation(TILEMAP_LAYER_C);
	
	// Get a positive from location offset by the map x and y - This *ONLY* works with the player sprite...
	fromLocation.x = abs(mapLocation.x - fromLocation.x);
	fromLocation.y = abs(mapLocation.y - fromLocation.y);
			
	// Otherwise let's let our fishy friend try to find his way on his own...
	if(fromLocation.x >= 1.0f && fromLocation.y >= 1.0f)
	{			
		Vector newDestination = map.findBestVectorOnPath(fromLocation, toLocation, sprite.getAngle(), 6, sprite.getID());
		float currentAngle = map.findAngleToDestination(fromLocation, sprite.getAngle(), newDestination, sprite.getID()); 			
		DIRECTION currentDirection = LevelMethodsGetAngleDirection(currentAngle, 1.0f);
		sprite.setDirection(currentDirection);
		if(currentDirection != DIRECTION_NONE)
		{
			sprite.increaseAcceleration();
		}
		sprite.setAngle(currentAngle);			
	}		
}

///<summary>
/// Called by several separate procedures to render a precentage status bar to the display
///<summary>
static void renderStatusBar(LevelInfo *levelInfo, Vector location, float width, float height, float percentageComplete, TextureHandle healthBarTexture)
{
	Quad quad, quadBorder;
	ASSERT(healthBarTexture != UNDEFINED_TEXTURE_HANDLE);	

	float statusPosition = (width * percentageComplete * 0.01f);
	float texturePosition = (0.5f * percentageComplete * 0.01f);

	// Set up the verticies for quad to render our status bar (e.g. we might use this when loading, after the title screen)		

	quad.verticies[0].x = 0.0f;
	quad.verticies[0].y = 0.0f;
	quad.verticies[0].z = 0.0f;

	quad.verticies[1].x = statusPosition;
	quad.verticies[1].y = 0.0f;
	quad.verticies[1].z = 0.0f;

	quad.verticies[2].x = statusPosition;
	quad.verticies[2].y = height;
	quad.verticies[2].z = 0.0f;

	quad.verticies[3].x = 0.0f;
	quad.verticies[3].y = height;
	quad.verticies[3].z = 0.0f;

	// TopLeft, TopRight, BottomRight, BottomLeft tex coords for the health meter
	quad.texture_S[0] = 0.0f;
	quad.texture_T[0] = 0.0f;

	quad.texture_S[1] = texturePosition;
	quad.texture_T[1] = 0.0f;

	quad.texture_S[2] = texturePosition;
	quad.texture_T[2] = 1.0f;

	quad.texture_S[3] = 0.0f;
	quad.texture_T[3] = 1.0f;

	// Set up the border around the quad drawing our health display
	quadBorder = quad;
	quadBorder.verticies[0].z -= 0.001f;
	quadBorder.verticies[1].z -= 0.001f;
	quadBorder.verticies[2].z -= 0.001f;
	quadBorder.verticies[0].x -= 0.001f;	
	quadBorder.verticies[1].x = width + 0.001f;
	quadBorder.verticies[2].x = width + 0.001f;
	quadBorder.verticies[3].x -= 0.001f;

	// TopLeft, TopRight, BottomRight, BottomLeft tex coords for the health meter border
	quadBorder.texture_S[0] = 0.5f;
	quadBorder.texture_T[0] = 0.0f;

	quadBorder.texture_S[1] = 1.0f;
	quadBorder.texture_T[1] = 0.0f;

	quadBorder.texture_S[2] = 1.0f;
	quadBorder.texture_T[2] = 1.0f;

	quadBorder.texture_S[3] = 0.5f;
	quadBorder.texture_T[3] = 1.0f;

	std::map<int, Quad> quads;
	quads[0] = quad;	
	quads[1] = quadBorder;	
	levelInfo->Graphics->addQuadsToScene(location, quads, quads.size(), healthBarTexture);		

	// offset our text slightly...
	location.x += width * 0.25f;
	location.y -= height * 0.2f;
	location.z -= 0.001f;
	//levelInfo->TextOutput->renderText(location, "%3.0f%%", percentageComplete);				
	quads.clear();
}

///<summary>
/// Renders text denoting the players current experience points
///<summary>
static void renderExperiencePoints(LevelInfo *levelInfo)
{
	// Render experience points, and attack power
	levelInfo->TextOutput->setTextSize(.0049f, .0049f, .0025f);
	levelInfo->TextOutput->renderText(VEC_INIT(0.030f, 0.045f, 0.14f), "Exp: %d",  levelInfo->DisplayUpgradePoints);
	levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, DEFAULT_TEXT_SPACING);

	// Adjust for display of upgrade points. Change by 1 if < 100, change by 100 otherwise...

	int updatedModifier = 1;
	if(abs(levelInfo->DisplayUpgradePoints - levelInfo->CurrentUpgradePoints) > 10)
	{
		updatedModifier = 10;
	}
	if(levelInfo->DisplayUpgradePoints > levelInfo->CurrentUpgradePoints) levelInfo->DisplayUpgradePoints -= updatedModifier;
	else if(levelInfo->DisplayUpgradePoints < levelInfo->CurrentUpgradePoints) levelInfo->DisplayUpgradePoints += updatedModifier;
}

///<summary>
/// Increments the counter used to display the current energy level
///<summary>
void LevelMethodsModifyEnergy(LevelInfo *levelInfo, float value)
{
		if(levelInfo->CurrentEnergyLevel < 98.0f) levelInfo->CurrentEnergyLevel += (value * gTimeScale);
}

///<summary>
/// Renders the bar displaying current attack level and experience points
///<summary>
static void renderPicklePowerbar(LevelInfo *levelInfo)
{
	//levelInfo->TextOutput->setTextSize(0.0049f, 0.0049f, 0.0026f);
	//levelInfo->TextOutput->renderText((Vector){-0.068f, -0.0415f, 0.13f}, " Pickle");
	//levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, DEFAULT_TEXT_SPACING);


	levelInfo->TextOutput->setTextSize(.0049f, .0049f, .0025f);
	renderStatusBar(levelInfo, VEC_INIT(-0.0665f, -0.036f, 0.14f), 0.025f, 0.0040f, levelInfo->CurrentEnergyLevel, gGunExpTexture);
	levelInfo->TextOutput->renderText(VEC_INIT(-0.060f, -0.0345f, 0.13f), " Magic");

	LevelMethodsModifyEnergy(levelInfo, 2.5f);
}

///<summary>
/// Displays the update points the player currently has and adjusts them as they increase or decrease...
///<summary>
void LevelMethodsRenderHeadsUpDisplay(LevelInfo *levelInfo)
{		
	ASSERT(gGunExpTexture != UNDEFINED_TEXTURE_HANDLE);	
	
	// Renders experience points, and attack power
	renderExperiencePoints(levelInfo);
	
	renderPicklePowerbar(levelInfo);

	LevelMethodsDisplayPickleHeath(levelInfo);

	// Draw our in game menu & buttons :)
	levelInfo->InGameMenu->executeMenu(levelInfo);

	// Draw our radar image and all our points on the screen....
	ASSERT(levelInfo->RadarTexture != UNDEFINED_TEXTURE_HANDLE);
	LevelMethodsDisplayRadarImage(levelInfo, levelInfo->RadarPoints, levelInfo->RadarTexture);

	// Display our current fps...
	levelInfo->TextOutput->renderText(VEC_INIT(-3.5f, 0.0f, 6.5f), "FPS: %d",  levelInfo->Graphics->getFPS());
}


///<summary>
/// Adds floating text data to the game world ...
///<summary>
void LevelMethodsAddFloatingText(LevelInfo *levelInfo, Vector location, Vector velocity, float life, const char* text)
{
	// Adjust our location so the text is centered
	location.x -= (strlen(text) * DEFAULT_TEXT_SPACING)/2;
	
	// Create the new text object...
	FloatingText newText;
	newText.life = life;
	newText.location = location;
	newText.velocity = velocity;
	newText.text = text;	
	levelInfo->FloatingTextData.push_back(newText);
}

///<summary>
/// Updates the position of any floating text data alive in the game world...
///<summary>
void LevelMethodsUpdateFloatingTextData(LevelInfo *levelInfo)
{
	std::vector<FloatingText>::iterator floatingTextIterator = levelInfo->FloatingTextData.begin();
	for(u32 idx = 0; idx < levelInfo->FloatingTextData.size(); idx++)
	{
		floatingTextIterator->life -= gTimeScale;
		floatingTextIterator->location.x += floatingTextIterator->velocity.x * gTimeScale;
		floatingTextIterator->location.y += floatingTextIterator->velocity.y * gTimeScale;
		floatingTextIterator->location.z += floatingTextIterator->velocity.z * gTimeScale;
		floatingTextIterator++;
	}
}

///<summary>
/// Renders any floating text info that is alive in the game world ...
///<summary>
void LevelMethodsRenderFloatingTextData(LevelInfo *levelInfo)
{
	std::vector<FloatingText>::iterator floatingTextIterator = levelInfo->FloatingTextData.begin();
	for(u32 idx = 0; idx < levelInfo->FloatingTextData.size(); idx++)
	{
		levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, DEFAULT_TEXT_SPACING);
		Vector renderingLocation = floatingTextIterator->location;		
		levelInfo->TextOutput->renderText(renderingLocation, "%s", floatingTextIterator->text.c_str());
		floatingTextIterator++;
	}
}

///<summary>
/// Displays the update points the player currently has and adjusts them as they increase or decrease...
///<summary>
void LevelMethodsDisplayPickleHeath(LevelInfo *levelInfo)
{	

	ASSERT(gHealthTexture != UNDEFINED_TEXTURE_HANDLE);	

	if(levelInfo->GreatPickle->getHealth() > 0)
	{		
		float healthyPercentage = ((levelInfo->GreatPickle->getHealth() / levelInfo->GreatPickle->getMaxHealth()) * 100.0f);
		renderStatusBar(levelInfo, VEC_INIT(-0.073f, -0.045f, 0.141f), 0.04f, 0.0054f, healthyPercentage, gHealthTexture);

		levelInfo->TextOutput->setTextSize(0.0049f, 0.0049f, 0.0026f);
		levelInfo->TextOutput->renderText(VEC_INIT(-0.068f, -0.0415f, 0.13f), " Pickle");
		levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, DEFAULT_TEXT_SPACING);

	}
}

///<summary>
/// Displays a specified radar image in the radar location on the screen ...
///<summary>
void LevelMethodsDisplayRadarImage(LevelInfo *levelInfo, std::vector<Point> radarPoints, TextureHandle radarTexture)
{
	Quad quad;

	ASSERT(radarTexture != UNDEFINED_TEXTURE_HANDLE);

	// If our texture handle is still undefined we should load our health meter texture here ...
	if(gRadarPointTexture == UNDEFINED_TEXTURE_HANDLE)
	{		
		gRadarPointTexture = levelInfo->Graphics->addTextureData("//assets//texOther//radarColors.png", IMAGE_TEXTURE, false);
	}

	ASSERT(gRadarPointTexture != UNDEFINED_TEXTURE_HANDLE);

	// Set up the verticies for quad so that we're drawing a health bar on top of the sprite
	// and in the center...
	quad.verticies[0].x = RADAR_LEFT;
	quad.verticies[0].y = RADAR_TOP;
	quad.verticies[0].z = 0.0f;

	quad.verticies[1].x = RADAR_RIGHT;
	quad.verticies[1].y = RADAR_TOP;
	quad.verticies[1].z = 0.0f;

	quad.verticies[2].x = RADAR_RIGHT;
	quad.verticies[2].y = RADAR_BOTTOM;
	quad.verticies[2].z = 0.0f;

	quad.verticies[3].x = RADAR_LEFT;
	quad.verticies[3].y = RADAR_BOTTOM;
	quad.verticies[3].z = 0.0f;

	// TopLeft, TopRight, BottomRight, BottomLeft tex coords for the health meter
	quad.texture_S[0] = 0.0f;
	quad.texture_T[0] = 0.0f;

	quad.texture_S[1] = 1.0f;
	quad.texture_T[1] = 0.0f;

	quad.texture_S[2] = 1.0f;
	quad.texture_T[2] = 1.0f;

	quad.texture_S[3] = 0.0f;
	quad.texture_T[3] = 1.0f;

	// TODO: Generate a point for the player sprite 

	std::map<int, Quad> quads;
	quads[0] = quad;	
	levelInfo->Graphics->addQuadsToScene(RADAR_LOCATION, quads, quads.size(), radarTexture);
	levelInfo->Graphics->addPointsToScene(RADAR_LOCATION, radarPoints, RADAR_DOT_PIXEL_SIZE, USE_HALF_TEXTURE, gRadarPointTexture);
	quads.clear();
}

///<summary>
/// Local method that generates a pseudo random number representing the type of power up, and assigns that value to a sprite
///<summary>
static void assignPowerupValue(ISprite *sprite)
{
	SPRITE_POWER_UP powerUpType = (SPRITE_POWER_UP)(rand() % POWER_UP_GUN_LARGE);
	sprite->setPowerUpType(powerUpType);	
}

///<summary>
/// Used in LevelMethodsHandleSpriteCollision to determine what sort of collision is required
///<summary>
static void handleSpriteCollision(bool intersectsAB, bool intersectsAC, bool intersectsBC, ISprite &sprite)
{
	// If we hit both A and B handle a front collision (change movement by 180 degrees)
	if(intersectsAB == true)
	{			
		sprite.handleCollision(FRONT_ATTACK, 14.0f);
		sprite.setJump(JUMP_STATE_READY);
	}
	// If we hit both A and C handle a rear left collision (change movement by 45 degrees)
	if(intersectsAC == true)
	{
		sprite.handleCollision(LEFT_SIDE_ATTACK, 14.0f);
		sprite.setJump(JUMP_STATE_READY);
	}
	// If we hit both B and C handle a rear right collision (change movement by 315 degrees)
	if(intersectsBC == true)
	{
		sprite.handleCollision(RIGHT_SIDE_ATTACK, 14.0f);
		sprite.setJump(JUMP_STATE_READY);
	}
}

///<summary>
/// Checks collision points for both sprites and returns a collision set;
///<summary>
LevelMethodCollisionSet LevelMethodsCheckCollisionSet(ISprite &mainSprite, ISprite &otherSprite)
{
	LevelMethodCollisionSet returnValue;
	BoundingBox mainBoundingBox = mainSprite.getBoundingBox();
	Vector otherLocation = otherSprite.getLocation();
	BoundingTriangle otherBoundingTriangle = otherSprite.getBoundingTriangle();
	Vector mainLocation = mainSprite.getLocation();	

	mainBoundingBox.left += mainLocation.x;
	mainBoundingBox.right += mainLocation.x;
	mainBoundingBox.top += mainLocation.y;
	mainBoundingBox.bottom += mainLocation.y;

	// Check point A and point B on the Triangle. If either is inside
	// of the bounding box then handle it with a FRONT ATTACK
	
	// Check point A
	Vector collisionCheckA;	
	collisionCheckA.x = (otherLocation.x + otherBoundingTriangle.collisionA.x);
	collisionCheckA.y = (otherLocation.y + otherBoundingTriangle.collisionA.y);
	returnValue.collisionA = vectorIsInsideBoundingBox(collisionCheckA, mainBoundingBox);

	// Check point B
	Vector collisionCheckB;	
	collisionCheckB.x = (otherLocation.x + otherBoundingTriangle.collisionB.x);
	collisionCheckB.y = (otherLocation.y + otherBoundingTriangle.collisionB.y);
	returnValue.collisionB = vectorIsInsideBoundingBox(collisionCheckB, mainBoundingBox);	

	// Check point C
	Vector collisionCheckC;	
	collisionCheckC.x = (otherLocation.x + otherBoundingTriangle.collisionC.x);
	collisionCheckC.y = (otherLocation.y + otherBoundingTriangle.collisionC.y);
	returnValue.collisionC = vectorIsInsideBoundingBox(collisionCheckC, mainBoundingBox);

	// Check to see if a line between A and B intersects the bounding box ...
	returnValue.collisionABIntersects = vectorIntersectsBoundingBox(collisionCheckA, collisionCheckB, mainBoundingBox);
	returnValue.collisionACIntersects = vectorIntersectsBoundingBox(collisionCheckA, collisionCheckC, mainBoundingBox);
	returnValue.collisionBCIntersects = vectorIntersectsBoundingBox(collisionCheckB, collisionCheckC, mainBoundingBox);

	return returnValue;
}

///<summary>
/// Checks collision points and handles updates accordingly with sprites
/// Returns true if a collision occurred!
///<summary>
bool LevelMethodsHandleSpriteCollision(ISprite &mainSprite, ISprite &otherSprite, LevelInfo *levelInfo)
{
	bool returnValue = false;
	// Get our collision set...
	LevelMethodCollisionSet collisionSet = LevelMethodsCheckCollisionSet(mainSprite, otherSprite);

	// Act on that collision set...
	if((collisionSet.collisionA == true) && (collisionSet.collisionB == true))
	{
		//mainSprite.handleCollision(SLOWDOWN_ATTACK);
	}	
	if((collisionSet.collisionABIntersects == true) || (collisionSet.collisionACIntersects == true) || (collisionSet.collisionBCIntersects == true))
	{		
		if(mainSprite.spriteType() == ENEMY_TARGET_SPRITE)
		{
			// When a baddie sprite runs into our target it kills the baddie sprite
			// but whatever health that baddie sprite has is taken from the target..
			mainSprite.modifyHealthBy(-(otherSprite.getPointValue()/2.0f));

			if((otherSprite.getCurrentAnimation() == RUN) &&
					(mainSprite.getCurrentAnimation() == STAND))
			{
				mainSprite.setAnimation(JUMP);
				levelInfo->Sounds[PICKLE_DAMAGE]->playOnce();
			}
						
			otherSprite.modifyHealthBy(-1000.0f);
			levelInfo->TileMap->shakeMapFor(0.5f, 0.15f);
		}		
		else if(mainSprite.spriteType() == POWER_UP_SPRITE)
		{			
			
			if(mainSprite.getHealth() >= 0.0f)
			{
				levelInfo->Sounds[POWER_UP]->playOnce();
			}
			mainSprite.modifyHealthBy(-100.0f);
			LevelMethodsModifyEnergy(levelInfo, 100.0f);
		}
		else if (mainSprite.spriteType() == PLAYER_SPRITE)
		{
			// If we're not attacking then have the baddie sprite attack
			if(otherSprite.spriteType() == BADDIE_SPRITE)
			{								
				if( (mainSprite.getCurrentAnimation() == ATTACK_A)
					|| (mainSprite.getCurrentAnimation() == REVERSE_ATTACK))
				{			
					returnValue = true;
					if((otherSprite.getCurrentAnimation() != PAIN_A)
						&& (otherSprite.getCurrentAnimation() != PAIN_B)
					&& (otherSprite.getCurrentAnimation() != PAIN_C)
					&& (otherSprite.getCurrentAnimation() != DEATH_FALLBACKSLOW)
					&& (otherSprite.getCurrentAnimation() != DEATH_FALLBACK)
					&& (otherSprite.getCurrentAnimation() != DEATH_FALLFORWARD))
					{
						levelInfo->Sounds[BOING]->playOnce();
					}
					handleSpriteCollision(collisionSet.collisionABIntersects, collisionSet.collisionACIntersects, collisionSet.collisionBCIntersects, otherSprite);
					otherSprite.modifyHealthBy(-levelInfo->CurrentPlayerRepelDamage);
					LevelMethodsHandleSparkEffects(levelInfo, &otherSprite, 3);

					if((otherSprite.getHealth() <= 0.0f) && (otherSprite.getHealth() >= -100.0f))
					{
						assignPowerupValue(&otherSprite);
						otherSprite.setHealth(-200);					
					}
				}
				else if(otherSprite.getCurrentAnimation() == ATTACK_A)
				{
					if((mainSprite.getJump() != JUMP_STATE_JUMPING) && (mainSprite.getJump() != JUMP_STATE_LANDING))
					{
						mainSprite.handleJump(-16.0f);
						handleSpriteCollision(collisionSet.collisionABIntersects, collisionSet.collisionACIntersects, collisionSet.collisionBCIntersects, mainSprite);
						int painAnim = rand() % 2;
						LevelMethodsHandleSparkEffects(levelInfo, &mainSprite, 3);
						if (painAnim == 0)
						{
							mainSprite.setAnimation(PAIN_A);
						}
						else if (painAnim == 1)
						{
							mainSprite.setAnimation(PAIN_B);
						}
						else if (painAnim == 2)
						{
							mainSprite.setAnimation(PAIN_C);
						}
					}
				}
				else if((mainSprite.getCurrentAnimation() != ATTACK_A)  && (mainSprite.getCurrentAnimation() != REVERSE_ATTACK)
					&& ((otherSprite.getAttackType() == ATTACK_TYPE_BOTH) || (otherSprite.getAttackType() == ATTACK_TYPE_PHYSICAL)) )
				{					
					if((otherSprite.getCurrentAnimation() != DEATH_FALLBACKSLOW)
						&& (otherSprite.getCurrentAnimation() != DEATH_FALLBACK)
						&& (otherSprite.getCurrentAnimation() != DEATH_FALLFORWARD)
						&& (mainSprite.getCurrentAnimation() != PAIN_A)
						&& (mainSprite.getCurrentAnimation() != PAIN_B)
						&& (mainSprite.getCurrentAnimation() != PAIN_C))
					{
						otherSprite.setAnimation(ATTACK_A);
					}
				}
			}							
		}
	}	
	return returnValue;
}

///<summary>
/// Iterate through and update our tower sprites...
///<summary>
void LevelMethodsUpdateTowers(LevelInfo *levelInfo)
{
	std::vector<SpriteTower>::iterator towerIterator = levelInfo->TowerSprites.begin();
	for(u32 idx = 0; idx < levelInfo->TowerSprites.size(); idx++)
	{
		towerIterator->setOffset(levelInfo->SpriteOffset);
		towerIterator->setControlMagnitude(0.75f);
		LevelMethodsHandleRippleEffects(levelInfo, &*towerIterator);
		towerIterator++;
	}
}

///<summary>
/// Iterate through and update our scenery sprites...
///<summary>
void LevelMethodsUpdateScenery(LevelInfo *levelInfo)
{
	std::vector<SpriteScenery>::iterator spriteIterator = levelInfo->ScenerySprites.begin();
	for(u32 idx = 0; idx < levelInfo->ScenerySprites.size(); idx++)
	{
		spriteIterator->setOffset(levelInfo->SpriteOffset);
		spriteIterator->setControlMagnitude(0.75f);
		spriteIterator++;
	}
}

///<summary>
/// Handles common steps for setting up a radar point...
///<summary>
static Point getDefaultRadarPoint(Vector mapLocation, Vector spriteLocation)
{
	Point returnValue;
	returnValue.point.x = (int)abs((mapLocation.x - spriteLocation.x));
	returnValue.point.y = (int)abs((mapLocation.y - spriteLocation.y));
	returnValue.point.z = -0.0000001f;
	returnValue.point.x *= RADAR_X_MULTIPLIER;
	returnValue.point.y *= RADAR_Y_MULTIPLIER;
	return returnValue;
}

///<summary>
/// Returns a point for displaying a radar image of the player
///<summary>
Point LevelMethodsGetPlayerRadarPoint(LevelInfo *levelInfo)
{
	Point newPoint = getDefaultRadarPoint(levelInfo->TileMap->getLocation(TILEMAP_LAYER_C),
			levelInfo->PlayerOneSprite->getLocation());
	newPoint.texture_S = 0.0f;
	newPoint.texture_T = 0.5f;
	return newPoint;
}

///<summary>
/// Returns a point for displaying a radar image of the pickle
///<summary>
Point LevelMethodsGetPickleRadarPoint(LevelInfo *levelInfo)
{
	Point newPoint = getDefaultRadarPoint(levelInfo->TileMap->getLocation(TILEMAP_LAYER_C),
			levelInfo->GreatPickle->getLocation());
	newPoint.texture_S = 0.00f;
	newPoint.texture_T = 0.00f;
	return newPoint;
}

///<summary>
/// Returns a point for displaying a radar image a baddie
///<summary>
Point LevelMethodsGetBaddieRadarPoint(Vector mapLocation, Vector baddieLocation)
{
	Point newPoint = getDefaultRadarPoint(mapLocation, baddieLocation);
	newPoint.texture_S = 0.5f;
	newPoint.texture_T = 0.0f;
	return newPoint;
}

///<summary>
/// Returns a point for displaying a radar image a baddie
///<summary>
Point LevelMethodsGetFlowerRadarPoint(Vector mapLocation, Vector flowerLocation)
{
	Point newPoint = getDefaultRadarPoint(mapLocation, flowerLocation);
	newPoint.texture_S = 0.5f;
	newPoint.texture_T = 0.5f;
	return newPoint;
}

///<summary>
/// Draws the bounding triangle used by a sprite
///<summary>
void renderCollisionFrame(IGraphics &gxGraphics, ISprite &sprite, TextureHandle textureHandle)
{
	std::vector<Triangle> renderTriangles;
	BoundingTriangle collision = sprite.getBoundingTriangle();
	Triangle collisionView;
	collisionView.verticies[0] = collision.collisionA;
	collisionView.verticies[1] = collision.collisionB;
	collisionView.verticies[2] = collision.collisionC;
	collisionView.texture_S[0] = 0.0f;
	collisionView.texture_S[1] = 0.5f;
	collisionView.texture_S[2] = 1.0f;
	collisionView.texture_T[0] = 0.0f;
	collisionView.texture_T[1] = 0.5f;
	collisionView.texture_T[2] = 1.0f;

	//text.renderText(&gxGraphics, (Vector){-4.f, -2.5f, 9.0f}, "C: %f,%f,%f", collision.collisionC.x, collision.collisionC.y, collision.collisionC.z);

	renderTriangles.push_back(collisionView);

	gxGraphics.addTrianglesToScene(sprite.getLocation(), renderTriangles, renderTriangles.size(), textureHandle);
}

///<summary>
/// Draws the bounding circle used by a sprite
///<summary>
void renderBoundingCircle(LevelInfo *levelInfo, ISprite &sprite, TextureHandle textureHandle)
{
	BoundingCircle circle = sprite.getBoundingCircle();

	//handleTileCollisionChecks(circle, sprite.getLocation(), sprite.getWaterLevel(), levelInfo);

	std::vector<Triangle> renderTriangles;

	ASSERT((circle.verticies.size() % 2) == 0)

	// Iterate through all the verticies in the circle and create triangles out of them
	// in order to render a "circle" (actually, a fan) around the center of the sprite...
	circle.vertIterator = circle.verticies.begin();
	while (circle.vertIterator != circle.verticies.end())
	{
		Triangle boundingView;

		boundingView.verticies[0].x = sprite.getLocation().x;
		boundingView.verticies[0].y = sprite.getLocation().y;
		boundingView.verticies[0].z = circle.vertIterator->vertex.z;

		boundingView.verticies[1].x = circle.vertIterator->vertex.x;
		boundingView.verticies[1].y = circle.vertIterator->vertex.y;
		boundingView.verticies[1].z = circle.vertIterator->vertex.z;

		circle.vertIterator++;

		boundingView.verticies[2].x = circle.vertIterator->vertex.x;
		boundingView.verticies[2].y = circle.vertIterator->vertex.y;
		boundingView.verticies[2].z = circle.vertIterator->vertex.z;

		boundingView.texture_S[0] = 0.0f;
		boundingView.texture_S[1] = 0.5f;
		boundingView.texture_S[2] = 1.0f;
		boundingView.texture_T[0] = 0.0f;
		boundingView.texture_T[1] = 0.5f;
		boundingView.texture_T[2] = 1.0f;

		renderTriangles.push_back(boundingView);
	}

	levelInfo->Graphics->addTrianglesToScene(sprite.getLocation(), renderTriangles, renderTriangles.size(), textureHandle);
}


///<summary>
/// Iterate through and update our tower sprites...
///<summary>
void LevelMethodsRenderTowers(LevelInfo *levelInfo)
{
	Vector mapLocation = levelInfo->TileMap->getLocation(TILEMAP_LAYER_C);
	std::vector<SpriteTower>::iterator towerIterator = levelInfo->TowerSprites.begin();
	for (u32 idx = 0; idx < levelInfo->TowerSprites.size(); idx++)
	{
		if (towerIterator->isDead() == false)
		{
			towerIterator->renderSprite(levelInfo->Graphics);
			renderCollisionFrame(*levelInfo->Graphics, *towerIterator, 0);
			// Add a radar point for any active flower/tower sprite
			Point newPoint = LevelMethodsGetFlowerRadarPoint(mapLocation, towerIterator->getLocation());
			levelInfo->RadarPoints.push_back(newPoint);
		}
		towerIterator++;
	}
}

///<summary>
/// Draw all our wild ass scenery :)
///<summary>
void LevelMethodsRenderScenery(LevelInfo *levelInfo)
{
	std::vector<SpriteScenery>::iterator spriteIterator = levelInfo->ScenerySprites.begin();
	for (u32 idx = 0; idx < levelInfo->ScenerySprites.size(); idx++)
	{
		spriteIterator->renderSprite(levelInfo->Graphics);
		spriteIterator++;
	}
}

///<summary>
/// Iterate through our towers and have them attempt to lock onto a particular target...
///<summary>
static void towersLockOnTarget(LevelInfo *levelInfo, ISprite &sprite)
{
	std::vector<SpriteTower>::iterator towerIterator = levelInfo->TowerSprites.begin();
	for (u32 idx = 0; idx < levelInfo->TowerSprites.size(); idx++)
	{
		if (towerIterator->isDead() == false)
		{
			towerIterator->lockOnTarget(sprite);
		}
		towerIterator++;
	}

	// Sneak the great pickle locking on target in here, too
	levelInfo->GreatPickle->lockOnTarget(sprite);
}


///<summary>
/// Updates baddie locations and handles collision details
///<summary>
void LevelMethodsUpdateBaddies(LevelInfo *levelInfo)
{
	levelInfo->BaddieDetails.BaddiesThisWaveAreDead = true;
	levelInfo->BaddieDetails.CurrentlyAlive = 0;
	// Handle all the controls and inputs for our baddie sprites!
	if(levelInfo->BaddieDetails.ReleaseIndex >= 0)
	{			
		for(int idx = 0; idx <= levelInfo->BaddieDetails.ReleaseIndex; idx++)
		{
			ASSERT((u32)idx < levelInfo->BaddieDetails.Sprites.size());
			if(levelInfo->BaddieDetails.Sprites.at(idx).isDead() == false)
			{				
				levelInfo->BaddieDetails.CurrentlyAlive++;

				// At least one baddie in our wave isn't dead :)
				levelInfo->BaddieDetails.BaddiesThisWaveAreDead = false;

				// Handle all our updates including pathfinding :)
				Vector fromLocation = levelInfo->BaddieDetails.Sprites.at(idx).getLocation();
				LevelMethodsFindSpritePath(levelInfo->BaddieDetails.Sprites.at(idx), fromLocation, levelInfo->BaddieDetails.Sprites.at(idx).getCurrentWaypoint(), *levelInfo->TileMap);

				LevelMethodsHandleRippleSplashEffects(levelInfo, &levelInfo->BaddieDetails.Sprites.at(idx));

				levelInfo->BaddieDetails.Sprites.at(idx).setOffset(levelInfo->SpriteOffset);
				levelInfo->BaddieDetails.Sprites.at(idx).updateLocationByVelocity();							
				LevelMethodsHandleSpriteCollision(*levelInfo->PlayerOneSprite, levelInfo->BaddieDetails.Sprites.at(idx), levelInfo);				
				LevelMethodsHandleSpriteCollision(*levelInfo->GreatPickle, levelInfo->BaddieDetails.Sprites.at(idx), levelInfo);			
													
				levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SHOT]->handleSpriteCollisions(levelInfo, levelInfo->BaddieDetails.Sprites.at(idx), levelInfo->Sounds[BOING], levelInfo->CurrentPlayerBulletDamage);
				levelInfo->ProjectileWeapons[PROJECTILE_TYPE_BOMB]->handleSpriteCollisions(levelInfo, levelInfo->BaddieDetails.Sprites.at(idx), levelInfo->Sounds[BOING], levelInfo->CurrentPlayerBulletDamage);
				levelInfo->ProjectileWeapons[PROJECTILE_TYPE_MINE]->handleSpriteCollisions(levelInfo, levelInfo->BaddieDetails.Sprites.at(idx), levelInfo->Sounds[BOING], levelInfo->CurrentPlayerBulletDamage);
				levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT]->handleSpriteCollisions(levelInfo, levelInfo->BaddieDetails.Sprites.at(idx), levelInfo->Sounds[BOING], levelInfo->CurrentPlayerBulletDamage);
				levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]->handleSpriteCollisions(levelInfo, levelInfo->BaddieDetails.Sprites.at(idx), levelInfo->Sounds[BOING], levelInfo->CurrentPlayerBulletDamage);
	
				// Have our tower sprite lock onto the nearest active sprite...				
				towersLockOnTarget(levelInfo, levelInfo->BaddieDetails.Sprites.at(idx));
				
				LevelMethodsHandleTilemapCollision(levelInfo->BaddieDetails.Sprites.at(idx), levelInfo);
			}
			if(levelInfo->BaddieDetails.Sprites.at(idx).getPowerUpType() != POWER_UP_NONE)
			{				
				LevelMethodsActivatePowerup(&levelInfo->BaddieDetails.Sprites.at(idx), levelInfo, levelInfo->BaddieDetails.Sprites.at(idx).getPowerUpType());
				levelInfo->BaddieDetails.Sprites.at(idx).setPowerUpType(POWER_UP_NONE);
				LevelMethodsHandleSparkEffects(levelInfo, &levelInfo->BaddieDetails.Sprites.at(idx), 30);
				//levelInfo->Sounds[BADDIE_DEATH]->playOnce();
			}
		}
		// If a collision occurred as part of this loop we'll enable rumble, otherwise disable...
		if((levelInfo->PlayerOneSprite->getCurrentAnimation() == ATTACK_A)
			|| (levelInfo->PlayerOneSprite->getCurrentAnimation() == REVERSE_ATTACK))
		{
			levelInfo->Controllers->startRumblePack(0);
		}
		else
		{
			levelInfo->Controllers->stopRumblePack(0);
		}
	}
}

/// <summary>
/// Updates the power up sprites that have been activated
/// </summary>
void LevelMethodsUpdateActivePowerups(LevelInfo *levelInfo)
{
	for(int idx = 0; idx < MAXIMUM_POWERUPS; idx++)
	{
		// Handle all the controls and inputs for our baddie sprites!
		if(levelInfo->PowerUps.PowerUpSprites.at(idx).isDead() == false)
		{				
			levelInfo->PowerUps.PowerUpSprites.at(idx).setOffset(levelInfo->SpriteOffset);						
			levelInfo->PowerUps.PowerUpSprites.at(idx).setControlMagnitude(0.0f);			
			levelInfo->PowerUps.PowerUpSprites.at(idx).updateLocationByVelocity();			
			LevelMethodsHandleSpriteCollision(levelInfo->PowerUps.PowerUpSprites.at(idx), *levelInfo->PlayerOneSprite, levelInfo);
			LevelMethodsHandleRippleEffects(levelInfo, &levelInfo->PowerUps.PowerUpSprites.at(idx));
			//LevelMethodsHandleTilemapCollision(levelInfo->PowerUps.PowerUpSprites.at(idx), levelInfo);
		}
	}
}


/// <summary>
/// Animates and renders the power ups to the screen
/// </summary>
void LevelMethodsRenderActivePowerups(LevelInfo *levelInfo)
{
	for(int powerupIndex = 0; powerupIndex < MAXIMUM_POWERUPS; powerupIndex++)
	{
		if(levelInfo->PowerUps.PowerUpSprites.at(powerupIndex).isDead() == false)
		{
			levelInfo->PowerUps.PowerUpSprites.at(powerupIndex).renderSprite(levelInfo->Graphics);
			renderCollisionFrame(*levelInfo->Graphics, levelInfo->PowerUps.PowerUpSprites.at(powerupIndex), 0);
		}
	}
}


/// <summary>
/// Updates the baddies that have been activated
/// </summary>
void LevelMethodsRenderActiveBaddies(LevelInfo *levelInfo)
{	
	Vector mapLocation = levelInfo->TileMap->getLocation(TILEMAP_LAYER_C);
	if(levelInfo->BaddieDetails.ReleaseIndex >= 0)
	{				
		for(int idx = 0; idx <= levelInfo->BaddieDetails.ReleaseIndex; idx++)
		{
			if(levelInfo->BaddieDetails.Sprites.at(idx).isDead() == false)
			{
				// Render the model our sprite is using...
				levelInfo->BaddieDetails.Sprites.at(idx).renderSprite(levelInfo->Graphics);
				renderCollisionFrame(*levelInfo->Graphics, levelInfo->BaddieDetails.Sprites.at(idx), 0);

				// Add a Point for the radar display...
				Vector baddieLocation = levelInfo->BaddieDetails.Sprites.at(idx).getLocation();

				// TODO: Remove this frustum cull for the radar points!!
				//SEEN_BY_CAMERA cameraSight = levelInfo->Graphics->isLocationWithinViewingRange(baddieLocation);

				//if((cameraSight == SEEN_BY_CAMERA_NEAR) || (cameraSight == SEEN_BY_CAMERA_MEDIUM))
				{
					Point newPoint = LevelMethodsGetBaddieRadarPoint(mapLocation, baddieLocation);
					levelInfo->RadarPoints.push_back(newPoint);
				}

				// If a baddie dies during the render process that means it was completeing the "dying" animation. Player gets points... yay... :(
				if(levelInfo->BaddieDetails.Sprites.at(idx).isDead() == true)
				{				
					levelInfo->CurrentUpgradePoints += levelInfo->BaddieDetails.Sprites.at(idx).getPointValue();					
				}
			}			
		}		
	}	
}

/// <summary>
/// Updates the baddies that have been activated
/// </summary>
void LevelMethodsRenderBaddieStatus(LevelInfo *levelInfo)
{
	if(levelInfo->BaddieDetails.ReleaseIndex >= 0)
	{				
		for(int idx = 0; idx <= levelInfo->BaddieDetails.ReleaseIndex; idx++)
		{
			if(levelInfo->BaddieDetails.Sprites.at(idx).isDead() == false)
			{
				// Render the model our sprite is using...
				levelInfo->BaddieDetails.Sprites.at(idx).renderHealthBar(levelInfo->Graphics);	
			}
		}
	}
}

/// <summary>
/// Updates the towers that have been activated
/// </summary>
void LevelMethodsRenderTowerStatus(LevelInfo *levelInfo)
{
	std::vector<SpriteTower>::iterator towerIterator = levelInfo->TowerSprites.begin();
	for (u32 idx = 0; idx < levelInfo->TowerSprites.size(); idx++)
	{
		if (towerIterator->isDead() == false)
		{
			towerIterator->renderHealthBar(levelInfo->Graphics);
		}
		towerIterator++;
	}
}

///<summary>
/// Static method that renders a triangle around a sprite to help understand the collision a sprite uses...
///</summary>


///<summary>
/// Handles the first universal steps of the rendering process
///</summary>
void LevelMethodsBeginRendering(LevelInfo *levelInfo)
{
	profileStart("levelInfo->Graphics->beginScene");

	// Get the interpolated z value of the camera location
	float mapZValue = getInterpolatedZValue(levelInfo->Graphics->getCameraLocation(), VEC_INIT(0.0f, 0.0f, 0.0f), *levelInfo->TileMap);
	// Have our camera point at our player, and adjust movement if it's not running into scenery...
	levelInfo->Graphics->setCameraTarget(levelInfo->PlayerOneSprite->getLocation(), levelInfo->PlayerOneSprite->getAngle(), mapZValue);

	Vector mapVelocity = levelInfo->PlayerOneSprite->updateLocationByVelocity();									
	
	LevelMethodsUpdateMapVelocity(*levelInfo->TileMap, mapVelocity);
		
	levelInfo->TileMap->animationTask(TILEMAP_LAYER_D);

	// Render our reflected viewpoint...
	//levelInfo->Graphics->switchToOverheadView();
	//levelInfo->TileMap->renderLayerVertexArrays(levelInfo->Graphics, TILEMAP_LAYER_A);
	//levelInfo->Graphics->updateMirrorReflection((Vector){0.0f, 0.0f, 0.0f}, (Vector){0.0f, 0.0f, 0.0f});
	//levelInfo->Graphics->switchBackToCameraView();

	// TODO: DRAW A REFLECTION SCENE HERE, THEN CLEAR THE FRAME BUFFER

	// Now we can draw everything else :)
	levelInfo->Graphics->beginScene();
	profileStop("levelInfo->Graphics->beginScene");

	profileStart("LevelMethodsRenderSkybox");
			LevelMethodsRenderSkybox(PROJECTION_NORMAL, levelInfo->Graphics);
	profileStop("LevelMethodsRenderSkybox");

	// Now Render out normal viewpoint...
	levelInfo->PlayerOneSprite->renderSprite(levelInfo->Graphics);
	renderCollisionFrame(*levelInfo->Graphics, *levelInfo->PlayerOneSprite, 0);
	
	if(levelInfo->GreatPickle->isDead() == false)
	{
		levelInfo->GreatPickle->renderSprite(levelInfo->Graphics);
		renderCollisionFrame(*levelInfo->Graphics, *levelInfo->GreatPickle, 0);
	}
	
	profileStart("LevelMethodsRenderTowers");
		LevelMethodsRenderTowers(levelInfo);
	profileStop("LevelMethodsRenderTowers");

	profileStart("LevelMethodsRenderScenery");
	LevelMethodsRenderScenery(levelInfo);
	profileStop("LevelMethodsRenderScenery");

	profileStart("LevelMethodsRenderActiveBaddies");
		LevelMethodsRenderActiveBaddies(levelInfo);
	profileStop("LevelMethodsRenderActiveBaddies");

	profileStart("renderLayerVertexArrays");
		levelInfo->TileMap->renderLayerVertexArrays(levelInfo->Graphics, TILEMAP_LAYER_A);
	profileStop("renderLayerVertexArrays");

	profileStart("renderProjectiles");
		levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SHOT]->renderProjectiles(levelInfo->Graphics);
		levelInfo->ProjectileWeapons[PROJECTILE_TYPE_BOMB]->renderProjectiles(levelInfo->Graphics);
		levelInfo->ProjectileWeapons[PROJECTILE_TYPE_MINE]->renderProjectiles(levelInfo->Graphics);
		levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT]->renderProjectiles(levelInfo->Graphics);
		levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]->renderProjectiles(levelInfo->Graphics);
	profileStop("renderProjectiles");

	profileStart("LevelMethodsRenderActivePowerups");
			LevelMethodsRenderActivePowerups(levelInfo);
	profileStop("LevelMethodsRenderActivePowerups");

	levelInfo->Graphics->updateMirrorReflection(VEC_INIT(0.0f, 0.0f, 0.0f), VEC_INIT(0.0f, 0.0f, 0.0f));

	profileStart("renderLayerVertexArrays");
		levelInfo->TileMap->renderLayerVertexArrays(levelInfo->Graphics, TILEMAP_LAYER_D);
	profileStop("renderLayerVertexArrays");

	profileStart("renderParticlesB");
	// Render particles that will appear above the water level
	levelInfo->RippleParticles->renderParticles(levelInfo->SpriteOffset, levelInfo->Graphics);
	levelInfo->RepelParticles->renderParticles(levelInfo->SpriteOffset, levelInfo->Graphics);
	levelInfo->SparkParticles->renderParticles(levelInfo->SpriteOffset, levelInfo->Graphics);
	levelInfo->SplashParticles->renderParticles(levelInfo->SpriteOffset, levelInfo->Graphics);
	levelInfo->WarpParticles->renderParticles(levelInfo->SpriteOffset, levelInfo->Graphics);
	profileStop("renderParticlesB");


	profileStart("LevelMethodsRenderBaddieStatus");
		LevelMethodsRenderBaddieStatus(levelInfo);
		LevelMethodsRenderTowerStatus(levelInfo);
	profileStop("LevelMethodsRenderBaddieStatus");
}

///<summary>
/// Handles universal display of 2d information
///</summary>
void LevelMethodsBegin2dCameraMode(LevelInfo *levelInfo)
{
	levelInfo->Graphics->switchToOverheadView();

	levelInfo->RadarPoints.push_back(LevelMethodsGetPlayerRadarPoint(levelInfo));
	
	levelInfo->RadarPoints.push_back(LevelMethodsGetPickleRadarPoint(levelInfo));	

	// Draw our current points, radar, buttons, and the health of our pickle ;;;)
	LevelMethodsRenderHeadsUpDisplay(levelInfo);

	LevelMethodsRenderFloatingTextData(levelInfo);	
}

///<summary>
/// Ends display of 2d information
///</summary>
void LevelMethodsEnd2DCameraMode(LevelInfo *levelInfo)
{
	levelInfo->RadarPoints.clear();

	levelInfo->Graphics->switchBackToCameraView();
}

///<summary>
/// Ends the rendering process
///</summary>
void LevelMethodsEndRendering(LevelInfo *levelInfo)
{
	levelInfo->TileMap->clearTempCollisionTable();
	levelInfo->Graphics->endScene();
}

///<summary>
/// Updates the velocity settings of the tilemap
///<summary>
void LevelMethodsUpdateMapVelocity(TiledMaps &map, Vector &mapVelocity)
{	
	map.setMapVelocity(TILEMAP_LAYER_A, mapVelocity);
	map.setMapVelocity(TILEMAP_LAYER_B, mapVelocity);
	map.setMapVelocity(TILEMAP_LAYER_C, mapVelocity);			
	map.setMapVelocity(TILEMAP_LAYER_D, mapVelocity);

	// Handle any velocity modifications that need to be done 
	map.velocityTask();
}

///<summary>
/// PreAllocates texture data required for menus
///<summary>
void LevelMethodsPreAllocateMenuData(LevelInfo *levelInfo, const char *buttonTexture, const char *focusedButtonTexture, const char *backgroundTexture)
{
	//printf("Preallocating button texture: %s \n", buttonTexture);
	levelInfo->ButtonTexture = levelInfo->Graphics->addTextureData(buttonTexture, IMAGE_TEXTURE, false);
	//printf("Preallocating button texture: %s \n", focusedButtonTexture);
	levelInfo->FocusedButtonTexture = levelInfo->Graphics->addTextureData(focusedButtonTexture, IMAGE_TEXTURE, false);
	//printf("Preallocating menu texture: %s \n", backgroundTexture);
	levelInfo->MenuTexture = levelInfo->Graphics->addTextureData(backgroundTexture, IMAGE_TEXTURE, false);

	levelInfo->TowerButtonTextures[SPITTING_TOWER]  	 = levelInfo->Graphics->addTextureData("//assets//texOther//spitFlower.png", IMAGE_TEXTURE, false);
	levelInfo->TowerButtonTextures[THROWING_TOWER]  	 = levelInfo->Graphics->addTextureData("//assets//texOther//tossFlower.png", IMAGE_TEXTURE, false);
	levelInfo->TowerButtonTextures[WIND_TOWER] 			 = levelInfo->Graphics->addTextureData("//assets//texOther//windyFlower.png", IMAGE_TEXTURE, false);
	levelInfo->TowerButtonTextures[CHOMP_TOWER] 		 = levelInfo->Graphics->addTextureData("//assets//texOther//thornFlower.png", IMAGE_TEXTURE, false);
	levelInfo->TowerButtonTextures[DIRECTION_PAD_BUTTON] = levelInfo->Graphics->addTextureData("//assets//texOther//dpad.png", IMAGE_TEXTURE, false);

	//printf("Preallocating in game menu object: THROWING_TOWER%s \n", backgroundTexture);
	levelInfo->InGameMenu = new InGameMenu(levelInfo);
	//printf("Preallocating poinrter/cursor for menus");
	GameMenuMethodsSetupPointer(levelInfo->Graphics->addTextureData("//assets/tex128//repelC.png", IMAGE_TEXTURE, false));
}

///<summary>
/// Deallocates texture data required for menus
///<summary>
void LevelMethodsDeAllocateMenuData(LevelInfo *levelInfo)
{
	// TODO: Deallocate anything that needs to be deallocated for our menus...
	// Note: As of this writing we ware not freeing menu graphics b/c they are global in operation
	delete levelInfo->InGameMenu;
}

///<summary>
/// Allocates the data required to display the title screen
///<summary>
void LevelMethodsPrepareSplashScreen(LevelInfo *levelInfo, float progressIncrement)
{
	if(gTitleScreenTexture == UNDEFINED_TEXTURE_HANDLE)
	{
		gTitleScreenTexture = levelInfo->Graphics->addTextureData("//assets//texOther//titleBg.png", IMAGE_TEXTURE, false);
	}
	// If our texture handle is still undefined we should load our health meter texture here ...
	if(gHealthTexture == UNDEFINED_TEXTURE_HANDLE)
	{		
		gHealthTexture = levelInfo->Graphics->addTextureData("//assets//texOther//health.png", IMAGE_TEXTURE, false);
	}
	// If we need to, go ahead and allocate the exp texture. We should use it throughout the game ...
	if(gGunExpTexture == UNDEFINED_TEXTURE_HANDLE)
	{		
		gGunExpTexture = levelInfo->Graphics->addTextureData("//assets//texOther//gunExperience.png", IMAGE_TEXTURE, false);
	}	
	gLoadProgress = 0.0f;
	gAddProgress = progressIncrement;
}

///<summary>
/// Displays the title screen with a loading progress bar
///<summary>
void LevelMethodsDisplaySplashScreen(LevelInfo *levelInfo)
{
	Quad quad;

	ASSERT(gTitleScreenTexture != UNDEFINED_TEXTURE_HANDLE);	
	ASSERT(gGunExpTexture != UNDEFINED_TEXTURE_HANDLE);	
	ASSERT(gHealthTexture != UNDEFINED_TEXTURE_HANDLE);	
	
	quad.verticies[0].x = SPLASH_LEFT;
	quad.verticies[0].y = SPLASH_TOP;
	quad.verticies[0].z = 0.0f;

	quad.verticies[1].x = SPLASH_RIGHT;
	quad.verticies[1].y = SPLASH_TOP;
	quad.verticies[1].z = 0.0f;

	quad.verticies[2].x = SPLASH_RIGHT;
	quad.verticies[2].y = SPLASH_BOTTOM;
	quad.verticies[2].z = 0.0f;

	quad.verticies[3].x = SPLASH_LEFT;
	quad.verticies[3].y = SPLASH_BOTTOM;
	quad.verticies[3].z = 0.0f;

	// TopLeft, TopRight, BottomRight, BottomLeft tex coords for the health meter
	quad.texture_S[0] = 0.0f;
	quad.texture_T[0] = 0.0f;

	quad.texture_S[1] = 1.0f;
	quad.texture_T[1] = 0.0f;

	quad.texture_S[2] = 1.0f;
	quad.texture_T[2] = 1.0f;

	quad.texture_S[3] = 0.0f;
	quad.texture_T[3] = 1.0f;

	std::map<int, Quad> quads;
	quads[0] = quad;	
		
	ASSERT(gHealthTexture != UNDEFINED_TEXTURE_HANDLE);

	levelInfo->Graphics->beginScene();
	levelInfo->Graphics->setCameraTarget(VEC_INIT(0.0f, 0.0f, 2.0f), 0.0f, 30.0f);
	levelInfo->Graphics->switchToOverheadView();
	
	// Move our status bar along...
	gLoadProgress += gAddProgress;
	
	levelInfo->Graphics->addQuadsToScene(SPLASH_LOCATION, quads, quads.size(), gTitleScreenTexture);	

	if(gLoadProgress <= 100.0f)
	{
		renderStatusBar(levelInfo, VEC_INIT(-1.5f, 0.85f, 2.9f), 3.0f, 0.12f, gLoadProgress, gHealthTexture);	
	}
	
	levelInfo->Graphics->switchBackToCameraView();
	levelInfo->Graphics->endScene();	
	quads.clear();
	
}

///<summary>
/// Frees the resources used for the title screen
///<summary>
void LevelMethodsDeAllocateSplashScreen(LevelInfo *levelInfo)
{
	if(gTitleScreenTexture != UNDEFINED_TEXTURE_HANDLE)
	{
		levelInfo->Graphics->freeTextureData(gTitleScreenTexture);		
		gTitleScreenTexture = UNDEFINED_TEXTURE_HANDLE;
	}
}
