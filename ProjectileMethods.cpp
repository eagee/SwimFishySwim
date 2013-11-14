// ProjectileMethods.cpp
// Provides delegate implementation details for IProjectileWeapon implementations
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "IProjectileWeapon.h"
#include "LevelMethods.h"
#include "ProjectileWeaponInfo.h"
#include "ProjectileMethods.h"



/// <summary>
/// Performs projectile initialization
/// </summary>
void ProjectileMethodsInit(IModel *model, IAudio *upgradeSound, IAudio *downGradeSound, IAudio *shootingSound, ProjectileWeaponInfo &weaponInfo,
		int totalLevels, int maxExperience, int maxProjectiles)
{
	// Preallocate our bullet sprites
	for (int index = 0; index < maxProjectiles; index++)
	{
		SpriteBullet newBullet;
		weaponInfo.Projectiles.push_back(newBullet);
		LevelMethodsSetupSprite(model, &weaponInfo.Projectiles.at(index), ATTACK_TYPE_NONE, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	weaponInfo.UpgradeSound = upgradeSound;
	weaponInfo.DowngradeSound = downGradeSound;
	weaponInfo.ShootingSound = shootingSound;
	weaponInfo.WeaponLevelNumber = 1;
	weaponInfo.CurrentProjectileIndex = 0;
	ProjectileMethodsCalculateMaxProjectiles(weaponInfo, totalLevels, maxProjectiles);
	weaponInfo.WeaponExperiencePoints = 0;
	weaponInfo.PercentageComplete = 0;
	weaponInfo.MaxWeaponExperiencePoints = (float)maxExperience;
	weaponInfo.WeaponState = PROJECTILE_TRIGGER_RELEASED;
	weaponInfo.ProjectileModel = model;
	weaponInfo.FiringDelayCounter = 0.0f;

}


/// <summary>
/// Delegate method to activate an available bullet sprite so that it can shoot off into the wild blue yonder :D
/// </summary>
void ProjectileMethodsActivate(int weaponLevel, float xyModifier, float angleOffset, ISprite *projectile, ISprite *triggeredBySprite, Vector &spriteOffset, IAudio *soundToPlay)
{
	Vector newLocation = triggeredBySprite->getLocation();
	DIRECTION newDirection = LevelMethodsGetAngleDirection(triggeredBySprite->getControlAngle(), 1.0f);

	// Increase our location to xymodifier in front of the direction the sprite is facing...
	// as well as change our offset depending on our weapon level number...
	float angleDegrees = modifyAngleByDegrees(triggeredBySprite->getAngle(), angleOffset);
	float angleRads = getRadiansForAngle(angleDegrees);
	float moveXBy = -(xyModifier * cos(angleRads));
	float moveYBy = -(xyModifier * sin(angleRads));

	newLocation.x -= spriteOffset.x;
	newLocation.y -= spriteOffset.y;


	// bomb: newLocation.z -= 0.7f;
	// shot: newLocation.z -= 0.6f;

	newLocation.x += moveXBy;
	newLocation.y += moveYBy;
	float waterLevel = triggeredBySprite->getWaterLevel();
	newLocation.z = waterLevel + spriteOffset.z;

	angleDegrees = modifyAngleByDegrees(triggeredBySprite->getControlAngle(), angleOffset);
	projectile->setDirection(newDirection);
	projectile->translateSprite(newLocation);
	projectile->setWaterLevel(20.8f);
	projectile->setOffset(spriteOffset);
	projectile->setAngle(angleDegrees);
	projectile->setJump(JUMP_STATE_NOT_READY);
	projectile->setJump(JUMP_STATE_READY);
	soundToPlay->playOnce();
}

///<summary>
/// Renders the weapon projectiles on the screen ...
///</summary>
void ProjectileMethodsRender(IGraphics *graphics, ProjectileWeaponInfo &weaponInfo)
{
	std::vector<SpriteBullet>::iterator spriteIterator = weaponInfo.Projectiles.begin();
	for(int idx = 0; idx < weaponInfo.CurrentMaxProjectiles; idx++)
	{
		if(spriteIterator->isDead() == false)
		{
			spriteIterator->renderSprite(graphics);
		}
		spriteIterator++;
	}
}

///<summary>
/// Determines if the triggering sprite is in a state to use this weapon...
///</summary>
bool ProjectileMethodsIsWeaponReady(PROJECTILE_TRIGGER_STATE state, ISprite *triggeredBySprite, ProjectileWeaponInfo &weaponInfo)
{
	bool returnValue = false;
	// If the sprite animation is right for this type of gun, and the player just released their weapon...
	if ((weaponInfo.WeaponState == PROJECTILE_TRIGGER_PRESSED) && (state == PROJECTILE_TRIGGER_RELEASED)
			//&& (triggeredBySprite->getCurrentAnimation() != CROUCH_ATTACK)
			&& (triggeredBySprite->getCurrentAnimation() != JUMP)
			&& (triggeredBySprite->getCurrentAnimation() != PAIN_A)
			&& (triggeredBySprite->getCurrentAnimation() != PAIN_B)
			&& (triggeredBySprite->getCurrentAnimation() != PAIN_C)
			&& (triggeredBySprite->getCurrentAnimation() != ATTACK_A))
	{
		weaponInfo.triggeredByType = triggeredBySprite->spriteType();
		returnValue = true;
	}

	return returnValue;
}

///<summary>
/// Increments to the next available projectile
///</summary>
void ProjectileMethodsIncrementProjectile(ProjectileWeaponInfo &weaponInfo, int maxProjectiles)
{
	weaponInfo.CurrentProjectileIndex++;
	ASSERT(weaponInfo.CurrentMaxProjectiles <= maxProjectiles);
	if (weaponInfo.CurrentProjectileIndex >= weaponInfo.CurrentMaxProjectiles)
	{
		weaponInfo.CurrentProjectileIndex = 0;
	}
}

///<summary>
/// Handles adjusting the experience used by a projectile when it is fired...
///</summary>
void ProjectileMethodsReduceExperience(ProjectileWeaponInfo &weaponInfo, float reduceBy)
{
	if(weaponInfo.WeaponExperiencePoints >= 0.0f)
	{
		weaponInfo.WeaponExperiencePoints -= reduceBy;
	}
}


///<summary>
/// Updates the projectiles for a weapon
///</summary>
void ProjectileMethodsUpdateProjectiles(LevelInfo* levelInfo, ProjectileWeaponInfo &weaponInfo)
{
	std::vector<SpriteBullet>::iterator spriteIterator = weaponInfo.Projectiles.begin();
		for(int idx = 0; idx < weaponInfo.CurrentMaxProjectiles; idx++)
		{
			// Handle all the controls and inputs for our baddie sprites!
			if(spriteIterator->isDead() == false)
			{
				JUMP_STATE lastJumpState = spriteIterator->getJump();
				spriteIterator->setOffset(levelInfo->SpriteOffset);
				spriteIterator->increaseAcceleration();
				spriteIterator->setControlMagnitude(2.0f);
				LevelMethodsHandleTilemapCollision(*spriteIterator, levelInfo);
				LevelMethodsHandleRippleEffects(levelInfo, &*spriteIterator);
				spriteIterator->updateLocationByVelocity();
				if ((lastJumpState == JUMP_STATE_LANDING) && (spriteIterator->getJump() != JUMP_STATE_LANDING))
				{
					LevelMethodsHandleSplashEffects(levelInfo, &*spriteIterator, 3);
				}
			}
			spriteIterator++;
		}
}

/// <summary>
/// Calculates the maximum projectiles available when a level changes...
/// </summary>
void ProjectileMethodsCalculateMaxProjectiles(ProjectileWeaponInfo &weaponInfo, int totalLevels, int maximumProjectiles)
{
	weaponInfo.CurrentMaxProjectiles = (maximumProjectiles / ((totalLevels - weaponInfo.WeaponLevelNumber) + 1));
}



///<summary>
/// Updates the current weapon percentage, and level...
///</summary>
PROJECTILE_LEVEL_STATE ProjectileMethodsUpdateExperience(ProjectileWeaponInfo &weaponInfo, int totalLevels, int maximumProjectiles)
{
	PROJECTILE_LEVEL_STATE returnValue = PROJECTILE_LEVEL_SAME;
	float targetPercent = ((weaponInfo.WeaponExperiencePoints / weaponInfo.MaxWeaponExperiencePoints) * 100.0f);
	//float lastPercentageComplete = weaponInfo.PercentageComplete;
	//int lastWeaponLevel = weaponInfo.WeaponLevelNumber;

	// Adjust our weapon percentage incrementally each update (for display purposes)
	if(weaponInfo.PercentageComplete < targetPercent - 1.0f)
	{
		weaponInfo.PercentageComplete += 10.0f * gTimeScale;
	}
	else if(weaponInfo.PercentageComplete > targetPercent + 1.0f)
	{
		weaponInfo.PercentageComplete -= 10.0f * gTimeScale;
	}
	else
	{
		weaponInfo.PercentageComplete = targetPercent;
	}

	if(weaponInfo.PercentageComplete <= 25.0f)
	{
		weaponInfo.WeaponLevelNumber = 1;
		ProjectileMethodsCalculateMaxProjectiles(weaponInfo, totalLevels, maximumProjectiles);
	}
	else if(weaponInfo.PercentageComplete <= 50.0f)
	{
		weaponInfo.WeaponLevelNumber = 2;
		ProjectileMethodsCalculateMaxProjectiles(weaponInfo, totalLevels, maximumProjectiles);
	}
	else if(weaponInfo.PercentageComplete <= 75.0f)
	{
		weaponInfo.WeaponLevelNumber = 3;
		ProjectileMethodsCalculateMaxProjectiles(weaponInfo, totalLevels, maximumProjectiles);
	}
	else if(weaponInfo.PercentageComplete > 75.0f)
	{
		weaponInfo.WeaponLevelNumber = 4;
		ProjectileMethodsCalculateMaxProjectiles(weaponInfo, totalLevels, maximumProjectiles);
	}

	return returnValue;
}

///<summary>
/// Sets the experience level of the weapon...
///</summary>
void ProjectileMethodsModifyExperienceBy(ProjectileWeaponInfo &weaponInfo, int experiencePoints, int maxExperience)
{
	weaponInfo.WeaponExperiencePoints += experiencePoints;
	if(weaponInfo.WeaponExperiencePoints > maxExperience)
	{
		weaponInfo.WeaponExperiencePoints = (float)maxExperience;
	}
	else if (weaponInfo.WeaponExperiencePoints < 0)
	{
		weaponInfo.WeaponExperiencePoints = 0;
	}
}

///<summary>
/// Handles the situation if a sprite dies due to collision with a projectile
///</summary>
void ProjectileMethodsHandleIfSpriteDead(ISprite &sprite, ProjectileWeaponInfo &weaponInfo)
{
	// If the player killed this sprite, drop a power up
	// If a tower killed this sprite, then add experience to that tower
	if ((sprite.getHealth() <= 0.0f) && (sprite.getHealth() >= -100.0f))
	{

		if (weaponInfo.triggeredByType == TOWER_SPRITE)
		{
			// Only increase experience randomly, so that it has a chance to go down
			// and provides a random element :D
			if(rand() % 3 != 0)
			{
				if(weaponInfo.WeaponExperiencePoints < weaponInfo.MaxWeaponExperiencePoints)
				{
					weaponInfo.WeaponExperiencePoints += 7.0f;
				}
			}
		}
		else
		{
			// This is the kind of power up created by our gun...
			SPRITE_POWER_UP powerUpType = (SPRITE_POWER_UP) (rand() % POWER_UP_GUN_LARGE);
			sprite.setPowerUpType(powerUpType);
		}
		sprite.setHealth(-200);

	}
}
