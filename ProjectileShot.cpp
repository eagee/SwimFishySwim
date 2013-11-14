// ProjectileShot.cpp
// Implements IProjectileWeapon interface 
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "IProjectileWeapon.h"
#include "LevelMethods.h"
#include "ProjectileShot.h"
#include "ProjectileMethods.h"

// Defines how many possible projectiles will be allowed in this class
#define MAXIMUM_PROJECTILES 16
#define MAX_EXPERIENCE 400
#define TOTAL_LEVELS 4

///<summary>
/// Constructor...duh
///</summary>			
ProjectileShot::ProjectileShot(IModel *model, IAudio *upgradeSound, IAudio *downGradeSound, IAudio *shootingSound)
{
	ProjectileMethodsInit(model, upgradeSound, downGradeSound, shootingSound, myWeaponInfo, TOTAL_LEVELS, MAX_EXPERIENCE, MAXIMUM_PROJECTILES);
}

///<summary>
/// Destru... oh come now, you must know this...
///</summary>			
ProjectileShot::~ProjectileShot()
{
	// Deallocate our bullet sprites
	myWeaponInfo.Projectiles.clear();
}

///<summary>
/// Get Projectile Model
///</summary>			
IModel* ProjectileShot::getProjectileModel()
{
	ASSERT(myWeaponInfo.ProjectileModel != NULL);

	return myWeaponInfo.ProjectileModel;
}

///<summary>
/// Gets a string describing the name of the weapon...
///</summary>			
std::string ProjectileShot::getProjectileName()
{
	return "Bubbly";
}

///<summary>
/// Sets the experience level of the weapon
///</summary>			
void ProjectileShot::setWeaponExperience(int experiencePoints)
{
	myWeaponInfo.WeaponExperiencePoints = (float)experiencePoints;
}

///<summary>
/// Gets the experience points for the weapon
///</summary>			
int ProjectileShot::getWeaponExperience()
{
	return (int)myWeaponInfo.WeaponExperiencePoints;
}

///<summary>
/// Gets the maximum experience points for the weapon
///</summary>
float ProjectileShot::getMaxWeaponExperience()
{
	return myWeaponInfo.MaxWeaponExperiencePoints;
}

///<summary>
/// Sets the experience level of the weapon
///</summary>			
void ProjectileShot::modifyWeaponExperienceBy(int experiencePoints)
{
	ProjectileMethodsModifyExperienceBy(myWeaponInfo, experiencePoints, MAX_EXPERIENCE);
}

///<summary>
/// Gets the level number of the weapon (based on exp pts)
///</summary>			
int ProjectileShot::getWeaponLevelNumber()
{
	return myWeaponInfo.WeaponLevelNumber;
}

///<summary>
/// Gets the percentage of experience gained toward the next level
///</summary>			
float ProjectileShot::getPercentWeaponExperience()
{	
	return myWeaponInfo.PercentageComplete;	 
}

///<summary>
/// Handles projection collisions with other sprites...
///</summary>			
void ProjectileShot::handleSpriteCollisions(LevelInfo *levelInfo, ISprite &sprite, IAudio *soundToPlay, float projectileDamage)
{
	LevelMethodCollisionSet collisionSet;
	std::vector<SpriteBullet>::iterator spriteIterator = myWeaponInfo.Projectiles.begin();
	for(int idx = 0; idx < myWeaponInfo.CurrentMaxProjectiles; idx++)
	{		
		// Handle all the controls and inputs for our baddie sprites!
		if(spriteIterator->isDead() == false)		
		{		
			if(spriteIterator->getLocation().z >= (spriteIterator->getWaterLevel() - 1.0f))
			{
				collisionSet = LevelMethodsCheckCollisionSet(myWeaponInfo.Projectiles.at(idx), sprite);
				if((collisionSet.collisionABIntersects == true) || (collisionSet.collisionACIntersects == true) || (collisionSet.collisionBCIntersects == true))
				{																
					if ((sprite.getCurrentAnimation() != DEATH_FALLBACKSLOW) && (sprite.getCurrentAnimation() != DEATH_FALLBACK)
							&& (sprite.getCurrentAnimation() != DEATH_FALLFORWARD))
					{
						LevelMethodsHandleSparkEffects(levelInfo, &myWeaponInfo.Projectiles.at(idx), 1);
						// If the sprite isn't dying, collide with the sucka...
						// If it's not in pain, play the hurty sound, and modify their health...
						// If we hit both A and B handle a front collision (change movement by 180 degrees)
						if(collisionSet.collisionABIntersects == true)
						{			
							spriteIterator->handleCollision(FRONT_ATTACK, 7.0f);
							spriteIterator->setJump(JUMP_STATE_READY);
						}
						// If we hit both A and C handle a rear left collision (change movement by 45 degrees)
						if(collisionSet.collisionACIntersects == true)
						{
							spriteIterator->handleCollision(LEFT_SIDE_ATTACK, 7.0f);
							spriteIterator->setJump(JUMP_STATE_READY);
						}
						// If we hit both B and C handle a rear right collision (change movement by 315 degrees)
						if(collisionSet.collisionBCIntersects == true)
						{
							spriteIterator->handleCollision(RIGHT_SIDE_ATTACK, 7.0f);
							spriteIterator->setJump(JUMP_STATE_READY);
						}					

						if((sprite.getCurrentAnimation() != PAIN_A)	&& (sprite.getCurrentAnimation() != PAIN_B)
							&& (sprite.getCurrentAnimation() != PAIN_C))
						{						
							soundToPlay->playOnce();																						
						}
						sprite.modifyHealthBy(-projectileDamage);

					}

					ProjectileMethodsHandleIfSpriteDead(sprite, myWeaponInfo);

				}			
			}
		}
		spriteIterator++;
	}
}

///<summary>
/// Updates projectiles and handles interaction with elements of level...
/// Returns level state of weapon (e.g. if an upgrade or downgrade occurred)
///</summary>			
PROJECTILE_LEVEL_STATE ProjectileShot::updateWeaponAndProjectiles(LevelInfo *levelInfo)
{
	ProjectileMethodsUpdateProjectiles(levelInfo, myWeaponInfo);
	return ProjectileMethodsUpdateExperience(myWeaponInfo, TOTAL_LEVELS, MAXIMUM_PROJECTILES);
}

///<summary>
/// Renders the weapon projectiles on the screen ...
///</summary>			
void ProjectileShot::renderProjectiles(IGraphics *graphics)
{
	ProjectileMethodsRender(graphics, myWeaponInfo);
}

/// <summary>
/// Activates an available bullet sprite so that it can shoot off into the wild blue yonder :D
/// </summary>
static void activateProjectile(int weaponLevel, float angleOffset, ISprite *projectile, ISprite *triggeredBySprite, Vector &spriteOffset, IAudio *soundToPlay)
{
	// Determine the level of the bullet we're going to activate then activate and start moving our bullet sprite
	if(weaponLevel == 1)
	{
		projectile->setLevel(0);
		projectile->setHealth(0.7f);		
	}
	else if(weaponLevel == 2)
	{
		projectile->setLevel(1);
		projectile->setHealth(0.6f);		
	}
	else if(weaponLevel == 3)
	{
		projectile->setLevel(2);
		projectile->setHealth(0.5f);
	}
	else
	{
		projectile->setLevel(3);
		projectile->setHealth(0.4f);				
	}

	ProjectileMethodsActivate( weaponLevel, 1.5, angleOffset, projectile, triggeredBySprite, spriteOffset, soundToPlay);

	projectile->setVelocities(14.0f, 0.0f, 2.0f);
	projectile->handleJump(-10.0f);
	projectile->setControlMagnitude(2.0f);

	projectile->increaseAcceleration();
	projectile->increaseAcceleration();
	projectile->increaseAcceleration();
	projectile->increaseAcceleration();
	projectile->increaseAcceleration();
}

///<summary>
/// Handles a change in the trigger state of the weapon...
/// state == pressed/released
/// triggeredBySprite -> The sprite who triggers the projectile to be fired
/// spriteOffset -> The offset created by the tilemap location for the projectile sprite
///</summary>			
bool ProjectileShot::handleTriggerState(PROJECTILE_TRIGGER_STATE state, ISprite *triggeredBySprite, Vector &spriteOffset)
{

	bool returnValue = ProjectileMethodsIsWeaponReady(state, triggeredBySprite, myWeaponInfo);
	if(returnValue == true)
	{
		// adjust our angle so that we spread our shots evenly out over the specified range...
		float angleRange = 45.0;
		float angleIncrement = (angleRange / (myWeaponInfo.WeaponLevelNumber + 1));
		float angleOffset = -((myWeaponInfo.WeaponLevelNumber * angleIncrement)/2);

		// For this projectile, we want to shoot one for every level...
		for(int idx = 0; idx < myWeaponInfo.WeaponLevelNumber; idx++)
		{
			if(myWeaponInfo.Projectiles.at(myWeaponInfo.CurrentProjectileIndex).isDead() == true)
			{					
				triggeredBySprite->setAnimation(CROUCH_ATTACK);
				activateProjectile(myWeaponInfo.WeaponLevelNumber, angleOffset, 
					&myWeaponInfo.Projectiles.at(myWeaponInfo.CurrentProjectileIndex), 
					triggeredBySprite, spriteOffset, myWeaponInfo.ShootingSound);

				angleOffset += angleIncrement;

				ProjectileMethodsIncrementProjectile(myWeaponInfo, MAXIMUM_PROJECTILES);
			}			
		}
	}
	myWeaponInfo.WeaponState = state;
	return returnValue;
}
