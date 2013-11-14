// LevelMethods.h
// Defines delegate implementation for common level oriented procedures
// Author(s): Eagan Rackley
#ifndef LEVEL_METHODS_H
#define LEVEL_METHODS_H

typedef struct tagCollisionSets
{
	bool collisionA;
	bool collisionB;
	bool collisionC;
	bool collisionABIntersects;
	bool collisionACIntersects;
	bool collisionBCIntersects;

} LevelMethodCollisionSet;

///<summary>
/// Allocates the graphic used with tilemaps
///</summary>
void LevelMethodsPreAllocateTilemapGraphic(IGraphics *graphics, const char *textureMap);

/// <summary>
/// Allocates the graphic used for drawing a level skybox...
/// </summary>
void LevelMethodsPreAllocateSkyboxTexture(IGraphics *graphics, const char *skyboxFile, const char *sceneboxFile);

/// <summary>
/// Renders a skybox to the scene using the preallocated skybox texture.
/// Will assert if the texture has not been loaded ...
/// </summary>
void LevelMethodsRenderSkybox(PROJECTION_TYPE projectionType, IGraphics *graphics);

///<summary>
/// Delegate proc. that will load a tilemap specified by file name and use texture data specified by textureName
///</summary>
bool LevelMethodsSetupTileMap(LevelInfo *levelInfo, const char *fileName, float translateX, float translateY, float translateZ);

/// <summary>
/// // Updates the controller state and modifies actions based on controller state, returns true if home is pressed.
/// </summary>
bool LevelMethodsHandleInput(int controllerID, CONTROLLER_STATUS &status, LevelInfo *levelInfo);

///<summary>
/// Pre allocate model and texture information from a file ...
///</summary>
bool LevelMethodsPreAllocateModelMd2(LevelInfo *levelInfo, float scale, const char *modelFile, const char *graphicFile, MODEL_TYPE modelType, AnimationFormat animationFormat, bool cullBackfaces);

///<summary>
/// Overloaded version that allows user to specify bump to apply to the model texture ...
///</summary>
bool LevelMethodsPreAllocateModelMd2(LevelInfo *levelInfo, float scale, const char *modelFile, const char *graphicFile, const char *bumpFile, MODEL_TYPE modelType, AnimationFormat animationFormat, bool cullBackfaces);

///<summary>
/// Pre allocate model and texture information from a file ...
///</summary>
bool LevelMethodsPreAllocateSpriteModel(LevelInfo *levelInfo, float scale, const char *graphicFile, MODEL_TYPE modelType);

///<summary>
/// De allocate model information before we shut down ...
///</summary>
void LevelMethodsDeAllocateModel(LevelInfo *levelInfo, MODEL_TYPE modelType);

///<summary>
/// Assigns a model to one of our sprite characters and instantiates the sprite to it's new location...
///<summary>
void LevelMethodsSetupSprite(IModel *model, ISprite *sprite, ATTACK_TYPE attackType, float x, float y, float z, float boundingTriangleSize);

///<summary>
/// Checks collision points and handles updates accordingly with map
///<summary>
void LevelMethodsHandleTilemapCollision(ISprite &sprite, LevelInfo *levelInfo);

///<summary>
/// Perform pathfinding operations for a sprite - creating angle and movement information
///<summary>
void LevelMethodsFindSpritePath(ISprite &sprite, Vector fromLocation, Vector toLocation, TiledMaps &map);

///<summary>
/// Checks collision points for both sprites and returns a collision set;
///<summary>
LevelMethodCollisionSet LevelMethodsCheckCollisionSet(ISprite &mainSprite, ISprite &otherSprite);

///<summary>
/// Checks collision points and handles updates accordingly with sprites
///<summary>
bool LevelMethodsHandleSpriteCollision(ISprite &mainSprite, ISprite &otherSprite, LevelInfo *levelInfo);

///<summary>
/// Displays the update points the player currently has and adjusts them as they increase or decrease...
///<summary>
void LevelMethodsRenderHeadsUpDisplay(LevelInfo *levelInfo);

///<summary>
/// Displays the update points the player currently has and adjusts them as they increase or decrease...
///<summary>
void LevelMethodsDisplayPickleHeath(LevelInfo *levelInfo);

///<summary>
/// Displays a specified radar image in the radar location on the screen ...
///<summary>
void LevelMethodsDisplayRadarImage(LevelInfo *levelInfo, std::vector<Point> radarPoints, TextureHandle radarTexture);

/// <summary>
/// Activates an available powerup sprite so that the player can pick up a little extra power :)
/// </summary>
void LevelMethodsActivatePowerup(ISprite *baddieSprite, LevelInfo *levelInfo, SPRITE_POWER_UP powerUpLevel);

/// <summary>
/// Indicates one of six directions an angle is facing ...
/// <summary>
DIRECTION LevelMethodsGetAngleDirection(float angle, float magnitude);

///<summary>
/// Pre allocates music for a level
///</summary>
void LevelMethodsPreAllocateLevelMusic(SONG_NAME songName, LevelInfo *levelInfo, const char *fileName);

///<summary>
/// De allocates music for a level
///</summary>
void LevelMethodsDeAllocateLevelMusic(SONG_NAME songName, LevelInfo *levelInfo);

///<summary>
/// Pre allocates sound for a level
///</summary>
void LevelMethodsPreAllocateLevelSound(SOUND_NAME soundName, LevelInfo *levelInfo, const char *fileName);

///<summary>
/// De allocates sound object for level WARNING: DOES NOT DEALLOCATE SOUND DATA!!!!!
///</summary>
void LevelMethodsDeAllocateLevelSound(SOUND_NAME soundName, LevelInfo *levelInfo);

///<summary>
/// Updates temporary collision points for a sprite (assigns to ID + 4)
///<summary>
void LevelMethodsSetupTempCollision(ISprite &sprite, TiledMaps &map);

///<summary>
/// Updates baddie locations and handles collision details
///<summary>
void LevelMethodsUpdateBaddies(LevelInfo *levelInfo);

/// <summary>
/// Updates the baddies that have been activated
/// </summary>
void LevelMethodsRenderActiveBaddies(LevelInfo *levelInfo);

/// <summary>
/// Updates the baddies that have been activated
/// </summary>
void LevelMethodsRenderBaddieStatus(LevelInfo *levelInfo);

/// <summary>
/// Updates the towers that have been activated
/// </summary>
void LevelMethodsRenderTowerStatus(LevelInfo *levelInfo);

/// <summary>
/// Animates and renders the power ups to the screen
/// </summary>
void LevelMethodsRenderActivePowerups(LevelInfo *levelInfo);

/// <summary>
/// Updates the power up sprites that have been activated
/// </summary>
void LevelMethodsUpdateActivePowerups(LevelInfo *levelInfo);

///<summary>
/// Handles the first universal steps of the rendering process
///</summary>
void LevelMethodsBeginRendering(LevelInfo *levelInfo);

///<summary>
/// Handles universal display of 2d information
///</summary>
void LevelMethodsBegin2dCameraMode(LevelInfo *levelInfo);

///<summary>
/// Ends display of 2d information
///</summary>
void LevelMethodsEnd2DCameraMode(LevelInfo *levelInfo);

///<summary>
/// Ends the rendering process
///</summary>
void LevelMethodsEndRendering(LevelInfo *levelInfo);

///<summary>
/// Updates the velocity settings of the tilemap
///<summary>
void LevelMethodsUpdateMapVelocity(TiledMaps &map, Vector &mapVelocity);

///<summary>
/// Returns a point for displaying a radar image of the player
///<summary>
Point LevelMethodsGetPlayerRadarPoint(LevelInfo *levelInfo);

///<summary>
/// Returns a point for displaying a radar image of the pickle
///<summary>
Point LevelMethodsGetPickleRadarPoint(LevelInfo *levelInfo);

///<summary>
/// PreAllocates texture data required for menus
///<summary>
void LevelMethodsPreAllocateMenuData(LevelInfo *levelInfo, const char *buttonTexture, const char *focusedButtonTexture, const char *backgroundTexture);

///<summary>
/// Deallocates texture data required for menus
///<summary>
void LevelMethodsDeAllocateMenuData(LevelInfo *levelInfo);

///<summary>
/// Adds floating text data to the game world ...
///<summary>
void LevelMethodsAddFloatingText(LevelInfo *levelInfo, Vector location, Vector velocity, float life, const char *text);

///<summary>
/// Updates the position of any floating text data alive in the game world...
///<summary>
void LevelMethodsUpdateFloatingTextData(LevelInfo *levelInfo);

///<summary>
/// Renders any floating text info that is alive in the game world ...
///<summary>
void LevelMethodsRenderFloatingTextData(LevelInfo *levelInfo);

///<summary>
/// Iterate through and update our tower sprites...
///<summary>
void LevelMethodsUpdateTowers(LevelInfo *levelInfo);

///<summary>
/// Iterate through and update our scenery sprites...
///<summary>
void LevelMethodsUpdateScenery(LevelInfo *levelInfo);

///<summary>
/// Iterate through and update our tower sprites...
///<summary>
void LevelMethodsRenderTowers(LevelInfo *levelInfo);

///<summary>
/// Draw all our wild ass scenery :)
///<summary>
void LevelMethodsRenderScenery(LevelInfo *levelInfo);

///<summary>
/// Allocates the data required to display the title screen
///<summary>
void LevelMethodsPrepareSplashScreen(LevelInfo *levelInfo, float progressIncrement);

///<summary>
/// Displays the title screen with a loading progress bar
///<summary>
void LevelMethodsDisplaySplashScreen(LevelInfo *levelInfo);

///<summary>
/// Frees the resources used for the title screen
///<summary>
void LevelMethodsDeAllocateSplashScreen(LevelInfo *levelInfo);

///<summary>
/// Handles the generation of ripple effects for the specified sprite
///<summary>
void LevelMethodsHandleRippleEffects(LevelInfo *levelInfo, ISprite *sprite);

///<summary>
/// Handles the generation of repel effects for the specified sprite
///<summary>
void LevelMethodsHandleRepelEffects(LevelInfo *levelInfo, ISprite *sprite);

///<summary>
/// Handles the generation of repel effects for the specified sprite
///<summary>
void LevelMethodsHandleWarpEffects(LevelInfo *levelInfo, ISprite *sprite);

///<summary>
/// Handles the generation of spark effects for the specified sprite
///<summary>
void LevelMethodsHandleSparkEffects(LevelInfo *levelInfo, ISprite *sprite, int number);

///<summary>
/// Handles the generation of splash effects for the specified sprite
///<summary>
void LevelMethodsHandleSplashEffects(LevelInfo *levelInfo, ISprite *sprite, int number);


///<summary>
/// Handles the generation of ripple and splash effects for the specified sprite on a timer
///<summary>
void LevelMethodsHandleRippleSplashEffects(LevelInfo *levelInfo, ISprite *sprite);

///<summary>
/// Increments the counter used to display the current energy level
///<summary>
void LevelMethodsModifyEnergy(LevelInfo *levelInfo, float value);


#endif
