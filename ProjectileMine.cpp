// ProjectileMine.cpp
// Implements IProjectileWeapon interface 
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "IProjectileWeapon.h"
#include "LevelMethods.h"
#include "ProjectileMine.h"
#include "ProjectileMethods.h"


// Defines how many possible projectiles will be allowed in this class
#define MAXIMUM_PROJECTILES 4
#define MAX_EXPERIENCE 400
#define TOTAL_LEVELS 4

///<summary>
/// Constructor...duh
///</summary>			
ProjectileMine::ProjectileMine(IModel *model, IAudio *upgradeSound, IAudio *downGradeSound, IAudio *shootingSound)
{
	ProjectileMethodsInit(model, upgradeSound, downGradeSound, shootingSound, myWeaponInfo, TOTAL_LEVELS, MAX_EXPERIENCE, MAXIMUM_PROJECTILES);
}

///<summary>
/// Destru... oh come now, you must know this...
///</summary>			
ProjectileMine::~ProjectileMine()
{
	// Deallocate our bullet sprites
	myWeaponInfo.Projectiles.clear();
}

///<summary>
/// Get Projectile Model
///</summary>			
IModel* ProjectileMine::getProjectileModel()
{
	ASSERT(myWeaponInfo.ProjectileModel != NULL);

	return myWeaponInfo.ProjectileModel;
}

///<summary>
/// Gets a string describing the name of the weapon...
///</summary>			
std::string ProjectileMine::getProjectileName()
{
	return "Mine";
}

///<summary>
/// Sets the experience level of the weapon
///</summary>			
void ProjectileMine::setWeaponExperience(int experiencePoints)
{
	myWeaponInfo.WeaponExperiencePoints = (float)experiencePoints;
}

///<summary>
/// Gets the experience points for the weapon
///</summary>			
int ProjectileMine::getWeaponExperience()
{
	return (int)myWeaponInfo.WeaponExperiencePoints;
}

///<summary>
/// Gets the maximum experience points for the weapon
///</summary>
float ProjectileMine::getMaxWeaponExperience()
{
	return myWeaponInfo.MaxWeaponExperiencePoints;
}

///<summary>
/// Sets the experience level of the weapon
///</summary>			
void ProjectileMine::modifyWeaponExperienceBy(int experiencePoints)
{
	ProjectileMethodsModifyExperienceBy(myWeaponInfo, experiencePoints, MAX_EXPERIENCE);
}

///<summary>
/// Gets the level number of the weapon (based on exp pts)
///</summary>			
int ProjectileMine::getWeaponLevelNumber()
{
	return myWeaponInfo.WeaponLevelNumber;
}

///<summary>
/// Gets the percentage of experience gained toward the next level
///</summary>			
float ProjectileMine::getPercentWeaponExperience()
{	
	return myWeaponInfo.PercentageComplete;	 
}

///<summary>
/// Handles projection collisions with other sprites...
///</summary>			
void ProjectileMine::handleSpriteCollisions(LevelInfo *levelInfo, ISprite &sprite, IAudio *soundToPlay, float projectileDamage)
{
	LevelMethodCollisionSet collisionSet;
	std::vector<SpriteBullet>::iterator spriteIterator = myWeaponInfo.Projectiles.begin();
	for(int idx = 0; idx < myWeaponInfo.CurrentMaxProjectiles; idx++)
	{		
		// Handle all the controls and inputs for our baddie sprites!
		if(spriteIterator->isDead() == false)		
		{		
			if(spriteIterator->getLocation().z >= (spriteIterator->getWaterLevel()))
			{
				collisionSet = LevelMethodsCheckCollisionSet(myWeaponInfo.Projectiles.at(idx), sprite);
				if((collisionSet.collisionABIntersects == true) || (collisionSet.collisionACIntersects == true) || (collisionSet.collisionBCIntersects == true))
				{																
					if ((sprite.getCurrentAnimation() != DEATH_FALLBACKSLOW) && (sprite.getCurrentAnimation() != DEATH_FALLBACK)
							&& (sprite.getCurrentAnimation() != DEATH_FALLFORWARD))
					{
						LevelMethodsHandleSparkEffects(levelInfo, &myWeaponInfo.Projectiles.at(idx), 30);
						// If the sprite isn't dying, collide with the sucka...
						// If it's not in pain, play the hurty sound, and modify their health...
						// If we hit both A and B handle a front collision (change movement by 180 degrees)
						if(collisionSet.collisionABIntersects == true)
						{			
							sprite.handleCollision(FRONT_ATTACK, (5.0f * myWeaponInfo.WeaponLevelNumber));							
							sprite.handleJump(-12.0f);
						}
						// If we hit both A and C handle a rear left collision (change movement by 45 degrees)
						if(collisionSet.collisionACIntersects == true)
						{
							sprite.handleCollision(LEFT_SIDE_ATTACK, (5.0f * myWeaponInfo.WeaponLevelNumber));							
							sprite.handleJump(-12.0f);
						}
						// If we hit both B and C handle a rear right collision (change movement by 315 degrees)
						if(collisionSet.collisionBCIntersects == true)
						{
							sprite.handleCollision(RIGHT_SIDE_ATTACK, (5.0f * myWeaponInfo.WeaponLevelNumber));							
							sprite.handleJump(-12.0f);
						}					

						if((sprite.getCurrentAnimation() != PAIN_A)	&& (sprite.getCurrentAnimation() != PAIN_B)
							&& (sprite.getCurrentAnimation() != PAIN_C))
						{						
							soundToPlay->playOnce();																						
						}
												
						sprite.modifyHealthBy(-(projectileDamage * myWeaponInfo.WeaponLevelNumber));
						
						// Setup a death animation on the mine that will kill the sprite as well...
						if(spriteIterator->getLevel() < 4)
						{
							spriteIterator->setLevel(4);						
						}
					
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
PROJECTILE_LEVEL_STATE ProjectileMine::updateWeaponAndProjectiles(LevelInfo *levelInfo)
{
	ProjectileMethodsUpdateProjectiles(levelInfo, myWeaponInfo);
	return ProjectileMethodsUpdateExperience(myWeaponInfo, TOTAL_LEVELS, MAXIMUM_PROJECTILES);
}

///<summary>
/// Renders the weapon projectiles on the screen ...
///</summary>			
void ProjectileMine::renderProjectiles(IGraphics *graphics)
{
	ProjectileMethodsRender(graphics, myWeaponInfo);
}

/// <summary>
/// Activates an available bullet sprite so that it can shoot off into the wild blue yonder :D
/// </summary>
static void activateProjectile(int weaponLevel, float angleOffset, ISprite *projectile, ISprite *triggeredBySprite, Vector &spriteOffset, IAudio *soundToPlay)
{

	// Determine the level of the bullet we're going to activate then activate and Minet moving our bullet sprite		
	if(weaponLevel == 1)
	{
		projectile->setLevel(1);
		projectile->setHealth(2.7f);		
	}
	else if(weaponLevel == 2)
	{
		projectile->setLevel(1);
		projectile->setHealth(2.6f);		
	}
	else if(weaponLevel == 3)
	{
		projectile->setLevel(2);
		projectile->setHealth(2.5f);
	}
	else
	{
		projectile->setLevel(3);
		projectile->setHealth(2.4f);				
	}		

	ProjectileMethodsActivate( weaponLevel, 1.5, angleOffset, projectile, triggeredBySprite, spriteOffset, soundToPlay);

	projectile->setVelocities(1.0f, 0.0f, 0.001f);
	projectile->handleJump(-10.0f);
	projectile->setControlMagnitude(2.0f);
	projectile->increaseAcceleration();			

}

///<summary>
/// Handles a change in the trigger state of the weapon...
/// state == pressed/released
/// triggeredBySprite -> The sprite who triggers the projectile to be fired
/// spriteOffset -> The offset created by the tilemap location for the projectile sprite
///</summary>			
bool ProjectileMine::handleTriggerState(PROJECTILE_TRIGGER_STATE state, ISprite *triggeredBySprite, Vector &spriteOffset)
{

	bool returnValue = ProjectileMethodsIsWeaponReady(state, triggeredBySprite, myWeaponInfo);
	if(returnValue == true)
	{
			// For this projectile, we want to shoot one for every level...
		for (int idx = 0; idx < myWeaponInfo.WeaponLevelNumber; idx++)
		{
			if (myWeaponInfo.Projectiles.at(myWeaponInfo.CurrentProjectileIndex).isDead() == true)
			{
				triggeredBySprite->setAnimation(CROUCH_ATTACK);
				activateProjectile(myWeaponInfo.WeaponLevelNumber, 180, &myWeaponInfo.Projectiles.at(myWeaponInfo.CurrentProjectileIndex), triggeredBySprite,
						spriteOffset, myWeaponInfo.ShootingSound);

				ProjectileMethodsIncrementProjectile(myWeaponInfo, MAXIMUM_PROJECTILES);

			}
		}
	}
	myWeaponInfo.WeaponState = state;
	return returnValue;
}
