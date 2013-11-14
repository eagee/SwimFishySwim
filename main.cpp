// main.cpp - This is the main file, dummy!
// Author(s): Eagan Rackley

#include "globals.h"
#include "SFSHeaders.h"
#include "Testing.h"
#include "IControllers.h"
#include "ISprite.h"
#include "ILevel.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "IProjectileWeapon.h"
#include "LevelMethods.h"
#include "LevelOne.h"
#include "SpriteTarget.h"
#include "SpritePlayer.h"
#include "SpriteBullet.h"
#include "ProjectileShot.h"
#include "ProjectileSpit.h"
#include "ProjectileBomb.h"
#include "ProjectileMine.h"
#include "ProjectileToss.h"

#ifdef PLATFORM_IS_WII
#include "ControllerWii.h"
#include "GxGraphics.h"
#endif

#ifdef PLATFORM_IS_WIN
#include "ControllerWin.h"
#include "GlGraphics.h"
#endif


Text *gTextDisplay;

#define PLAYER_VELOCITY 20.0f
#define PLAYER_DRAG 	0.1f
#define PLAYER_ACCEL 	0.5f

#define PLAYER	0
#define TREE	1
#define ROCK	2
#define BADDIE	3

///<summary>
/// Provides an interface for executing all of the unit testing associated with swim fishy swim.
///</summary>
void runUnitTests()
{
	testing test;	
	printf("Start of automated unit tests:\n");
	test.performAutomatedTesting();		
}

// Sets up the text renderer...
void setupTextRendering(IText &text, IGraphics &graphics)
{
	// set up the text renderer
	TextureHandle fontTextureID = graphics.addTextureData("//assets//texOther//gameFontHalf.png", IMAGE_TEXTURE, false);
	ASSERT(fontTextureID != UNDEFINED_TEXTURE_HANDLE);
	if(fontTextureID != UNDEFINED_TEXTURE_HANDLE)
	{		
		text.assignTexture(fontTextureID);
	}
}

///<summary>
/// Preallocates models that are used throughout the game
///</summary>
void preAllocateGameModels(LevelInfo *levelInfo)
{	
	LevelMethodsPreAllocateTilemapGraphic(levelInfo->Graphics, "//assets//texOther//mapTypeA.png");
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateSkyboxTexture(levelInfo->Graphics, "//assets//texOther//skyboxB.png", "//assets//texOther//sceneboxA.png");
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.18f, "//assets//models//traditional//crab.md2",
				"//assets/tex128//bumble.png", "//assets/tex128//bumbleBump.png",
				BOSS_FISH_MODEL, ANIMATION_TRADITIONAL_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.09f, "//assets//models//traditional//hero.md2",
			"//assets/tex128//hero.png", "//assets/tex128//hero.png",
			PRIMARY_FISH_MODEL, ANIMATION_TRADITIONAL_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.08f, "//assets//models//interpol_baddie//swarmer.md2",
			"//assets/tex128//bumble.png", "//assets/tex128//bumbleBump.png",
			SMALL_BADDIE_FISH_MODEL, ANIMATION_TRADITIONAL_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.21f, "//assets//models//interpol_baddie//swarmer.md2",
			"//assets/tex128//eel.png", "//assets/tex128//eelBump.png",
			MEDIUM_BADDIE_FISH_MODEL, ANIMATION_BADDIE_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.13f, "//assets//models//interpol_baddie//swarmer.md2",
			"//assets/tex64//swarmerC.png", "//assets/tex64//swarmerBump.png",
			FAST_BADDIE_FISH_MODEL, ANIMATION_TRADITIONAL_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.10f, "//assets//models//interpol_baddie//swarmer.md2",
			"//assets/tex128//bumble.png", "//assets/tex128//bumbleBump.png",
			ATTACKING_BADDIE_FISH_MODEL, ANIMATION_TRADITIONAL_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 3.0f, "//assets//models//traditional//pickle.md2",
			"//assets/tex128//pickle.png", "//assets/tex128//pickleBump.png",
			PICKLE_MODEL, ANIMATION_TRADITIONAL_MD2, true);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.016f, "//assets//models//traditional//bubble.md2",
			"//assets/tex64//bubbleYellow.png", "//assets/tex64//bubbleBump.png",
			SHOT_MODEL, ANIMATION_TRADITIONAL_MD2, true);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.024f, "//assets//models//traditional//bubble.md2",
			"//assets/tex64//bubbleRainbow.png", "//assets/tex64//bubbleBump.png",
			BOMB_MODEL, ANIMATION_TRADITIONAL_MD2, true);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.028f, "//assets//models//traditional//bubble.md2",
			"//assets/tex64//bubbleRainbow.png", "//assets/tex64//bubbleBump.png",
			TOSS_MODEL, ANIMATION_TRADITIONAL_MD2, true);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.40f, "//assets//models//traditional//mine.md2",
			"//assets/texOther//mine.png", "//assets/tex64//bubbleBump.png",
			MINE_MODEL, ANIMATION_TRADITIONAL_MD2, true);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.18f, "//assets//models//static//powerup.md2",
				"//assets/tex64//powerup.png", "//assets/tex64//powerupBump.png",
				POWERUP_MODEL, ANIMATION_STATIC_MD2, false);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.08f, "//assets//models//static//tree.md2",
				"//assets/tex128//tree.png", "//assets/tex128//treeBump.png",
				TREE_MODEL, ANIMATION_STATIC_MD2, true);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.36f, "//assets//models//static//flower.md2",
					"//assets/tex128//flowerA.png", "//assets/tex128//flowerBump.png",
					FLOWER_A_MODEL, ANIMATION_STATIC_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.28f, "//assets//models//static//flower.md2",
						"//assets/tex128//flowerB.png", "//assets/tex128//flowerBump.png",
						FLOWER_B_MODEL, ANIMATION_STATIC_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.20f, "//assets//models//static//flower.md2",
							"//assets/tex128//flowerC.png", "//assets/tex128//flowerBump.png",
							FLOWER_C_MODEL, ANIMATION_STATIC_MD2, false);
	LevelMethodsDisplaySplashScreen(levelInfo);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.08f, "//assets//models//traditional//spitFlower.md2",
								"//assets/tex128//spitFlower.png", "//assets/tex128//spitFlowerBump.png",
								SPITTING_FLOWER_TOWER, ANIMATION_TRADITIONAL_MD2, false);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.08f, "//assets//models//traditional//lobFlower.md2",
								"//assets/tex128//lobFlower.png", "//assets/tex128//lobFlowerBump.png",
								TOSSING_FLOWER_TOWER, ANIMATION_TRADITIONAL_MD2, false);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.08f, "//assets//models//traditional//fanFlower.md2",
									"//assets/tex128//fanFlower.png", "//assets/tex128//fanFlowerBump.png",
									WINDY_FLOWER_TOWER, ANIMATION_TRADITIONAL_MD2, false);

	LevelMethodsPreAllocateModelMd2(levelInfo, 0.08f, "//assets//models//traditional//stabFlower.md2",
										"//assets/tex128//stabFlower.png", "//assets/tex128//stabFlowerBump.png",
										CHOMPY_FLOWER_TOWER, ANIMATION_TRADITIONAL_MD2, false);
}

///<summary>
/// Preallocates sprite objects that are used throughout the game
///</summary>
void preAllocateSpriteObjects(LevelInfo *levelInfo)
{
	// Preallocate Sprites that will remain consistent between levels...

	// Setup the projectile weapons that will persist throughout the game ...
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SHOT] = new ProjectileShot(levelInfo->Models[SHOT_MODEL], levelInfo->Sounds[LEVEL_UP], levelInfo->Sounds[MENU_NO], levelInfo->Sounds[BWOOP]);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_BOMB] = new ProjectileBomb(levelInfo->Models[BOMB_MODEL], levelInfo->Sounds[LEVEL_UP], levelInfo->Sounds[MENU_NO], levelInfo->Sounds[BWOOP]);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_MINE] = new ProjectileMine(levelInfo->Models[MINE_MODEL], levelInfo->Sounds[LEVEL_UP], levelInfo->Sounds[MENU_NO], levelInfo->Sounds[BWOOP]);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT] = new ProjectileSpit(levelInfo->Models[SHOT_MODEL], levelInfo->Sounds[LEVEL_UP], levelInfo->Sounds[MENU_NO], levelInfo->Sounds[BWOOP]);
	levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW] = new ProjectileToss(levelInfo->Models[TOSS_MODEL], levelInfo->Sounds[LEVEL_UP], levelInfo->Sounds[MENU_NO], levelInfo->Sounds[BWOOP]);

	// Setup our Player Sprite
	levelInfo->PlayerOneSprite = new SpritePlayer();
	LevelMethodsSetupSprite(levelInfo->Models[PRIMARY_FISH_MODEL], levelInfo->PlayerOneSprite, ATTACK_TYPE_BOTH, 0.0f, 0.0f, SPRITE_WATER_LEVEL, 1.6f);

	// Setup our Pickle Sprite
	levelInfo->GreatPickle = new SpriteTarget();
	LevelMethodsSetupSprite(levelInfo->Models[PICKLE_MODEL], levelInfo->GreatPickle, ATTACK_TYPE_NONE, 36.0f, 30.0f, 19.6f, 1.0f);
	levelInfo->GreatPickle->setHealth(100.0f);
	levelInfo->GreatPickle->setVelocities(0.0f, 0.0f, 0.0f);
	
	levelInfo->CurrentProjectileWeapon = PROJECTILE_TYPE_SHOT;
	levelInfo->ProjectileWeapon = levelInfo->ProjectileWeapons[levelInfo->CurrentProjectileWeapon];
	
	// Preallocate powerup sprites (also consistent between levels...)
	for(int powerupIndex = 0; powerupIndex < MAXIMUM_POWERUPS; powerupIndex++)
	{
		SpritePowerup newSprite;
		levelInfo->PowerUps.PowerUpSprites.push_back(newSprite);		
		LevelMethodsSetupSprite(levelInfo->Models[POWERUP_MODEL], &levelInfo->PowerUps.PowerUpSprites.at(powerupIndex), ATTACK_TYPE_NONE, 27.0f, 20.0f, SPRITE_WATER_LEVEL, 1.0f);
	}
}

///<summary>
/// Preallocates the menus we'll use in the game...
///</summary>
void preAllocateMenuData(LevelInfo *levelInfo)
{
	LevelMethodsPreAllocateMenuData(levelInfo, "//assets//texOther//menuButton.png", "//assets//texOther//menuSelect.png", "//assets//texOther//menuBg.png");
	LevelMethodsDisplaySplashScreen(levelInfo);
}

///<summary>
/// Preallocates the particle engines we'll use in the game...
///</summary>
void preAllocateParticleData(LevelInfo *levelInfo)
{
	levelInfo->RippleParticles = new ParticleEffect(PARTICLE_EFFECT_WATER_RIPPLE, 200);
	levelInfo->RippleParticles->assignTexture(levelInfo->Graphics->addTextureData("//assets/tex128//ripple.png", IMAGE_TEXTURE, false));

	levelInfo->RepelParticles = new ParticleEffect(PARTICLE_EFFECT_REPEL_ENEMIES, 2);
	levelInfo->RepelParticles->assignTexture(levelInfo->Graphics->addTextureData("//assets/tex128//repelC.png", IMAGE_TEXTURE, false));

	levelInfo->WarpParticles = new ParticleEffect(PARTICLE_EFFECT_WARP, 2);
	levelInfo->WarpParticles->assignTexture(levelInfo->Graphics->addTextureData("//assets/tex32//spark.png", IMAGE_TEXTURE, false));

	levelInfo->SparkParticles = new ParticleEffect(PARTICLE_EFFECT_SPARK, 200);
	levelInfo->SparkParticles->assignTexture(levelInfo->Graphics->addTextureData("//assets/tex32//spark.png", IMAGE_TEXTURE, false));

	levelInfo->SplashParticles = new ParticleEffect(PARTICLE_EFFECT_SPLASH, 200);
	levelInfo->SplashParticles->assignTexture(levelInfo->Graphics->addTextureData("//assets/tex32//splash.png", IMAGE_TEXTURE, false));
}

///<summary>
/// deallocates the particle engines we'll use in the game...
///</summary>
void deAllocateParticleData(LevelInfo *levelInfo)
{
	delete levelInfo->RippleParticles;
	delete levelInfo->RepelParticles;
	delete levelInfo->WarpParticles;
	delete levelInfo->SparkParticles;
	delete levelInfo->SplashParticles;
}


///<summary>
/// Deallocates sprite objects that were used throughtout the game
///</summary>
void deAllocateSpriteObjects(LevelInfo *levelInfo)
{
	delete levelInfo->PlayerOneSprite;
	delete levelInfo->GreatPickle;
	delete levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SHOT];
	delete levelInfo->ProjectileWeapons[PROJECTILE_TYPE_BOMB];
	delete levelInfo->ProjectileWeapons[PROJECTILE_TYPE_MINE];
	delete levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT];
	delete levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW];

	levelInfo->PowerUps.PowerUpSprites.clear();
}

///<summary>
/// Deallocates models that were used throughout the game
///</summary>
void deAllocateGameModels(LevelInfo *levelInfo)
{
	LevelMethodsDeAllocateModel(levelInfo, PRIMARY_FISH_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, SMALL_BADDIE_FISH_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, MEDIUM_BADDIE_FISH_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, FAST_BADDIE_FISH_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, PICKLE_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, SHOT_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, BOMB_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, MINE_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, POWERUP_MODEL);
	LevelMethodsDeAllocateModel(levelInfo, TOSS_MODEL);
	//LevelMethodsDeAllocateModel(levelInfo, ROCK_MODEL);
}


///<summary>
/// Preallocates data used for music ...
///</summary>
void preAllocateAudioData(LevelInfo *levelInfo)
{
	LevelMethodsPreAllocateLevelSound(BWOOP, levelInfo, "//assets//sound//bwoop.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(LEVEL_UP, levelInfo, "//assets//sound//levelUp.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(POWER_UP, levelInfo, "//assets//sound//powerUp.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(SPIN_ATTACK, levelInfo, "//assets//sound//spinAttack.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(BADDIE_DEATH, levelInfo, "//assets//sound//baddieDeath.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(BOING, levelInfo, "//assets//sound//boing.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(WAVE_FINISHED, levelInfo, "//assets//sound//finishedWave.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(MENU_YES, levelInfo, "//assets//sound//yes.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(MENU_NO, levelInfo, "//assets//sound//no.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
	LevelMethodsPreAllocateLevelSound(PICKLE_DAMAGE, levelInfo, "//assets//sound//pickleDamage.snd");
	LevelMethodsDisplaySplashScreen(levelInfo);
}

///<summary>
/// Deallocates data used for music ...
///</summary>
void deAllocateAudioData(LevelInfo *levelInfo)
{
	LevelMethodsDeAllocateLevelMusic(MP3_AUDIO, levelInfo);
	LevelMethodsDeAllocateLevelSound(BWOOP, levelInfo);
	LevelMethodsDeAllocateLevelSound(LEVEL_UP, levelInfo);
	LevelMethodsDeAllocateLevelSound(POWER_UP, levelInfo);
	LevelMethodsDeAllocateLevelSound(SPIN_ATTACK, levelInfo);
	LevelMethodsDeAllocateLevelSound(BADDIE_DEATH, levelInfo);
	
}

///<summary>
/// Deallocates data used for menus ...
///</summary>
void deAllocateMenuData(LevelInfo *levelInfo)
{
	LevelMethodsDeAllocateMenuData(levelInfo);
}

///<summary>
/// RunGameMain preallocates data, runs levels, and deallocates data before exiting
///</summary>
void RunGameMain()
{	
	// Set up our essential system objects that will be used by all of our ILevel implementations...
#ifdef PLATFORM_IS_WII
	IGraphics *graphics = new GxGraphics();
	ControllerWii controllers(720,480);	
#endif
#ifdef PLATFORM_IS_WIN
	IGraphics *graphics;// = new GxGraphics();
	ControllerWin controllers(720,480);	
#endif
	
	gTextDisplay = new Text(graphics);
	setupTextRendering(*gTextDisplay, *graphics);

	// Set up a base level info object that is passed to each ILevel implementation...
	LevelInfo *levelInfo = new LevelInfo();
	levelInfo->Graphics = graphics;
	levelInfo->TextOutput = gTextDisplay;
	levelInfo->Controllers = &controllers;		

	printf("\x1b[2;0H");printf("\n\nWaiting for GDB Connection...");
	//_break();

	// At this point we can load and display our title screen / start the title music...
	//LevelMethodsPreAllocateLevelMusic(MP3_AUDIO, levelInfo, "//assets//music//fishyPop.mp3");
	LevelMethodsPrepareSplashScreen(levelInfo, 100.0f);	

	// Now that all our data is loaded we can just wait for a keypress...
	//TODO: WAIT HERE FOR A KEY TO BE PRESSED
	//while(levelInfo->Controllers->checkButtonPress(0, WII_BUTTON_A) != true)
	//{
	//	levelInfo->Controllers->updateControllerStates();
	//	LevelMethodsDisplaySplashScreen(levelInfo);
	//}

	LevelMethodsPrepareSplashScreen(levelInfo, 4.0f);

	// Preallocate all of the sprites, models, textures, music, and sound that will persist throughout the game ...	
	preAllocateAudioData(levelInfo);
	preAllocateGameModels(levelInfo);
	preAllocateSpriteObjects(levelInfo);	
	preAllocateMenuData(levelInfo);
	preAllocateParticleData(levelInfo);
	
	// Free up the resources our title screen was using, and load the in game mp3 data..
	LevelMethodsDeAllocateSplashScreen(levelInfo);

	// Load our tilemap and then play level one!
	LevelOne levelOne(*levelInfo);
	levelOne.runUntilLevelIsFinished();		
	
	// TODO: Deallocate our tilemap, load another one, and play level two!	

	// Deallocate all of the sprites, models, textures, music, and sound that persisted throughout the game ...
	deAllocateSpriteObjects(levelInfo);
	deAllocateGameModels(levelInfo);	
	deAllocateAudioData(levelInfo);
	deAllocateMenuData(levelInfo);
	deAllocateParticleData(levelInfo);
	
	delete gTextDisplay;		
}

// This runs our unit testing main
void setupUnitTesting()
{
	#ifdef PLATFORM_IS_WII
	#endif

	// Do all of our unit testing!
	runUnitTests();
}

//---------------------------------------------------------------------------------
// main!
//---------------------------------------------------------------------------------
int main(int argc, char **argv) 
{
#ifdef PLATFORM_IS_WII
	try
	{
		// Setup the remote debugger if possible...
		DEBUG_Init(GDBSTUB_DEVICE_USB, 1);

		// Initialize filesystem to load from SD Card - This should change to NAND for official releases ...
		fatInitDefault();

		for(int i = 0; i < 100; i++)
		{
			usleep(100);
		}	

		//setupTrace("//Map.csv");
		
		srand ( time(NULL) );

		AUDIO_Init(NULL);
        ASND_Init();
        ASND_Pause(0);

		//setupUnitTesting();
		
		RunGameMain();
		
		ASND_Pause(1);
        ASND_End();

		//MemTrack::TrackListMemoryUsage();

		//shutDownTrace();

	}
	catch(int ex)
	{
		printf("\x1b[2;0H");printf("Exception Triggered, Executing Breakpoint!\n");
		ASSERT(false);

		_break();
	}
#endif

#ifdef PLATFORM_IS_WIN
	runUnitTests();
#endif

	return 0;
}
