// LevelInfo.h
// Defines struct for processing level information. Used with ILevel interface.
// Author(s): Eagan Rackley
#ifndef LEVEL_INFO_H
#define LEVEL_INFO_H

#include "SFSHeaders.h"
#include "IControllers.h"
#include "IGraphics.h"
#include "SpritePowerUp.h"
#include "SpriteBullet.h"
#include "SpriteBaddie.h"
#include "SpriteTower.h"
#include "SpriteScenery.h"
#include "IParticleEffect.h"
#include "ParticleEffect.h"
#include "TiledMaps.h"


class IProjectileWeapon;
class IGameMenu;

// Defines Player movement velocities...
#define ALL_SPRITE_VELOCITY 21.0f
#define ALL_SPRITE_DRAG 	0.2f
#define ALL_SPRITE_ACCEL 	0.6f

/// <summary>
/// Defines update types that can arrive from our controller interface
/// </summary>

enum MODEL_TYPE
{
	PRIMARY_FISH_MODEL			= 0x000,
	SMALL_BADDIE_FISH_MODEL		= 0x001,
	MEDIUM_BADDIE_FISH_MODEL	= 0x002,
	FAST_BADDIE_FISH_MODEL		= 0x003,
	ATTACKING_BADDIE_FISH_MODEL = 0x004,
	PICKLE_MODEL				= 0x005,	
	SHOT_MODEL					= 0x006,
	BOMB_MODEL					= 0x007,
	MINE_MODEL					= 0x008,
	POWERUP_MODEL				= 0x009,
	ROCK_MODEL					= 0x00A,
	BOSS_FISH_MODEL				= 0x00B,
	TOSS_MODEL					= 0x00C,
	SPIT_MODEL					= 0x00D,
	WIND_MODEL					= 0x00E,
	TREE_MODEL					= 0x00F,
	FLOWER_A_MODEL				= 0x010,
	FLOWER_B_MODEL				= 0x011,
	FLOWER_C_MODEL				= 0x012,
	SPITTING_FLOWER_TOWER		= 0x013,
	TOSSING_FLOWER_TOWER		= 0x014,
	WINDY_FLOWER_TOWER			= 0x015,
	CHOMPY_FLOWER_TOWER			= 0x016
};
#define TOTAL_MODELS (CHOMPY_FLOWER_TOWER + 1)

enum SONG_NAME
{
	MP3_AUDIO					= 0x00
};
#define TOTAL_SONGS (MP3_AUDIO + 1)

enum SOUND_NAME
{
	BWOOP						= 0x00,
	BOING						= 0x01,
	LEVEL_UP					= 0x02,
	POWER_UP					= 0x03,
	SPIN_ATTACK					= 0x04,
	WAVE_FINISHED				= 0x05,
	BADDIE_DEATH				= 0x06,
	MENU_YES					= 0x07,
	MENU_NO						= 0x08,
	PICKLE_DAMAGE				= 0x09
};
#define TOTAL_SOUNDS (PICKLE_DAMAGE + 1)

enum PROJECTILE_TYPE
{
	PROJECTILE_TYPE_SHOT		= 0x00,
	PROJECTILE_TYPE_BOMB		= 0x01,
	PROJECTILE_TYPE_MINE		= 0x02,
	PROJECTILE_TYPE_SPIT		= 0x03,
	PROJECTILE_TYPE_THROW		= 0x04
};
#define TOTAL_PROJECTILE_TYPES (PROJECTILE_TYPE_THROW + 1)

enum TOWER_TYPES
{
	SPITTING_TOWER		= 0x00,
	THROWING_TOWER		= 0x01,
	WIND_TOWER			= 0x02,
	CHOMP_TOWER			= 0x03
};
#define TOTAL_TOWER_TYPES (CHOMP_TOWER + 1)

// Represents buttons used to display the UI for building towers...
enum TOWER_BUTTON_TYPES
{
	SPITTING_TOWER_BUTTON		= 0x00,
	THROWING_TOWER_BUTTON		= 0x01,
	WIND_TOWER_BUTTON			= 0x02,
	CHOMP_TOWER_BUTTON			= 0x03,
	DIRECTION_PAD_BUTTON		= 0x04,
	NO_TOWER_BUTTON_SELECTED    = 0x05
};
#define TOTAL_TOWER_BUTTONS (DIRECTION_PAD_BUTTON + 1)


// Defines how many power up sprites will be allowed...
#define MAXIMUM_POWERUPS 12

/// <summary>
/// Defines information used to control powerup behavior
/// </summary>
typedef struct tagLevelPowerupInfo
{
	int	CurrentPowerupIndex;
	std::vector<SpritePowerup> PowerUpSprites;
} LevelPowerupInfo;

/// <summary>
/// Defines scrolling informational messages displayed in the 3d world...
/// </summary>
typedef struct tagFloatingText
{
	Vector location;
	Vector velocity;
	float  life;
	std::string text;
} FloatingText;

/// <summary>
/// Values used to manage release of baddies during each wave in a level
/// </summary>
typedef struct tagBaddieDetailInfo
{
	bool  BaddiesThisWaveAreDead;
	int   WaveIndex;
	float ReleaseDelay;
	float ReleaseTime;
	int   ReleaseIndex;
	int   MaximumThisWave;
	int	  CurrentType;
	int   CurrentlyAlive;
	std::vector<SpriteBaddie>	Sprites;								
} BaddieDetailInfo;


/// <summary>
/// Defines the information that will be used to define the behavior in a level.
/// </summary>
typedef struct tagLevelInfo
{
	// Attributes
	bool					FinishedLooping;
	Vector					SpriteOffset;
	Vector					BaddieDestination;
	int						CurrentUpgradePoints;
	int						DisplayUpgradePoints;	
	int						CurrentProjectileWeapon;
	float					CurrentPlayerRepelDamage;
	float					CurrentPlayerBulletDamage;
	float					CurrentEnergyLevel;
	SONG_NAME				CurrentSong;	
	LevelPowerupInfo		PowerUps;	
	std::vector<Point>		RadarPoints;		
	std::vector<FloatingText> FloatingTextData;	
	BaddieDetailInfo		BaddieDetails;
	TextureHandle			RadarTexture;
	TextureHandle			MenuTexture;
	TextureHandle			ButtonTexture;
	TextureHandle			FocusedButtonTexture;
	TextureHandle			ModelTextures[TOTAL_MODELS];	
	TextureHandle			ModelBumpTextures[TOTAL_MODELS];	
	TextureHandle			LightMap;
	TextureHandle			TowerButtonTextures[TOTAL_TOWER_BUTTONS];

	// Associations
	IModel					*Models[TOTAL_MODELS];
	std::vector<SpriteTower> TowerSprites;
	std::vector<SpriteScenery> ScenerySprites;
	ISprite					*PlayerOneSprite;	
	ISprite					*GreatPickle;
	IGraphics				*Graphics;	
	IControllers			*Controllers;
	IProjectileWeapon		*ProjectileWeapons[TOTAL_PROJECTILE_TYPES];
	IProjectileWeapon		*ProjectileWeapon;
	IGameMenu				*InGameMenu;
	TiledMaps				*TileMap;
	IText					*TextOutput;				
	IAudio					*Songs[TOTAL_SONGS];
	IAudio					*Sounds[TOTAL_SOUNDS];
	ParticleEffect			*RippleParticles;
	ParticleEffect			*RepelParticles;
	ParticleEffect			*SparkParticles;
	ParticleEffect			*SplashParticles;
	ParticleEffect          *WarpParticles;
} LevelInfo;

#endif
