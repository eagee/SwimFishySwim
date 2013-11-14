// ProjectileWeaponInfo.h
// Provides structure for delegate implementation code for IProjectileWeapon interface
// Author(s): Eagan Rackley
#ifndef PROJECTILE_WEAPON_INFO_H
#define PROJECTILE_WEAPON_INFO_H

// Some forward declarations...
class ISprite;
class IAudio;
class SpriteBullet;

///<summary>
/// Defines trigger states used by a projectile weapon...
///</summary>
typedef enum
{
	PROJECTILE_TRIGGER_PRESSED,
	PROJECTILE_TRIGGER_RELEASED
} PROJECTILE_TRIGGER_STATE;

///<summary>
/// Defines state changes in the level of a projectile weapon.
///</summary>
typedef enum
{
	PROJECTILE_LEVEL_LOST,
	PROJECTILE_LEVEL_SAME,
	PROJECTILE_LEVEL_GAINED	
} PROJECTILE_LEVEL_STATE;

///<summary>
/// Defines information that is universal to all IGameMenu implementations and their delegate classes...
///</summary>
typedef struct tagProjectileWeaponInfo
{
	float RotationValue;
	float WeaponExperiencePoints;
	float MaxWeaponExperiencePoints;
	float PercentageComplete;
	float MaxFiringDelay;
	float FiringDelayCounter;
	int WeaponLevelNumber;	
	int CurrentProjectileIndex;
	int CurrentMaxProjectiles;
	IAudio *UpgradeSound;
	IAudio *DowngradeSound;
	IAudio *ShootingSound;
	PROJECTILE_TRIGGER_STATE WeaponState;	
	IModel *ProjectileModel;
	SPRITE_TYPE triggeredByType;
	std::vector<SpriteBullet> Projectiles;	
} ProjectileWeaponInfo;

#endif
