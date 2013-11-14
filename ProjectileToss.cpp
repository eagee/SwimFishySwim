// ProjectileToss.cpp
// Implements IProjectileWeapon interface 
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "IProjectileWeapon.h"
#include "LevelMethods.h"
#include "ProjectileToss.h"
#include "ProjectileMethods.h"

// Defines how many possible projectiles will be allowed in this class
#define MAXIMUM_PROJECTILES 48
#define TOTAL_LEVELS 4
#define MAX_EXPERIENCE 400
#define FIRING_DELAY 5.0f

///<summary>
/// Constructor...duh
///</summary>			
ProjectileToss::ProjectileToss(IModel *model, IAudio *upgradeSound, IAudio *downGradeSound, IAudio *shootingSound)
{
	ProjectileMethodsInit(model, upgradeSound, downGradeSound, shootingSound, myWeaponInfo, TOTAL_LEVELS, MAX_EXPERIENCE, MAXIMUM_PROJECTILES);
	myWeaponInfo.MaxFiringDelay = FIRING_DELAY;
}

///<summary>
/// Destru... oh come now, you must know this...
///</summary>			
ProjectileToss::~ProjectileToss()
{
	// Deallocate our bullet sprites
	myWeaponInfo.Projectiles.clear();
}

///<summary>
/// Get Projectile Model
///</summary>			
IModel* ProjectileToss::getProjectileModel()
{
	ASSERT(myWeaponInfo.ProjectileModel != NULL);

	return myWeaponInfo.ProjectileModel;
}

///<summary>
/// Gets a string describing the name of the weapon...
///</summary>			
std::string ProjectileToss::getProjectileName()
{
	return "Bomba";
}

///<summary>
/// Sets the experience level of the weapon
///</summary>			
void ProjectileToss::setWeaponExperience(int experiencePoints)
{
	myWeaponInfo.WeaponExperiencePoints = (float)experiencePoints;
}

///<summary>
/// Gets the experience points for the weapon
///</summary>			
int ProjectileToss::getWeaponExperience()
{
	return (int)myWeaponInfo.WeaponExperiencePoints;
}

///<summary>
/// Gets the maximum experience points for the weapon
///</summary>
float ProjectileToss::getMaxWeaponExperience()
{
	return myWeaponInfo.MaxWeaponExperiencePoints;
}

///<summary>
/// Sets the experience level of the weapon
///</summary>			
void ProjectileToss::modifyWeaponExperienceBy(int experiencePoints)
{
	ProjectileMethodsModifyExperienceBy(myWeaponInfo, experiencePoints, MAX_EXPERIENCE);
}

///<summary>
/// Gets the level number of the weapon (based on exp pts)
///</summary>			
int ProjectileToss::getWeaponLevelNumber()
{
	return myWeaponInfo.WeaponLevelNumber;
}

///<summary>
/// Gets the percentage of experience gained toward the next level
///</summary>			
float ProjectileToss::getPercentWeaponExperience()
{	
	return myWeaponInfo.PercentageComplete;	 
}

/// <summary>
/// Activates an available bullet sprite so that it can shoot off into the wild blue yonder :D
/// </summary>
static void activateProjectile(ProjectileWeaponInfo &weaponInfo, ISprite *projectile,
		ISprite *triggeredBySprite, Vector &spriteOffset, IAudio *soundToPlay)
{
	float modifyAngleBy = ((45.0f - -45.0f)*((float)rand()/RAND_MAX)) + -45.0f;

	// Determine the level of the bullet we're going to activate then activate and start moving our bullet sprite		
	if(weaponInfo.WeaponLevelNumber == 1)
	{
		projectile->setLevel(0);
		projectile->setHealth(2.5f);
		weaponInfo.MaxFiringDelay = FIRING_DELAY;
	}
	else if(weaponInfo.WeaponLevelNumber == 2)
	{
		projectile->setLevel(0);
		projectile->setHealth(2.5f);
		weaponInfo.MaxFiringDelay = FIRING_DELAY;
	}
	else if(weaponInfo.WeaponLevelNumber == 3)
	{
		projectile->setLevel(0);
		projectile->setHealth(2.5f);
		weaponInfo.MaxFiringDelay = FIRING_DELAY;
	}
	else
	{
		projectile->setLevel(0);
		projectile->setHealth(2.5f);
		weaponInfo.MaxFiringDelay = FIRING_DELAY;
	}

	ProjectileMethodsActivate(weaponInfo.WeaponLevelNumber, 1.0f, modifyAngleBy, projectile, triggeredBySprite, spriteOffset, soundToPlay);

	projectile->setVelocities(2.0f, 0.0f, 1.0f);
	projectile->setControlMagnitude(1.0f);	
	projectile->handleJump(-12.0f);
}

///<summary>
/// Handles projection collisions with other sprites...
///</summary>			
void ProjectileToss::handleSpriteCollisions(LevelInfo *levelInfo, ISprite &sprite, IAudio *soundToPlay, float projectileDamage)
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
				collisionSet = LevelMethodsCheckCollisionSet(*spriteIterator, sprite);
				if((collisionSet.collisionABIntersects == true) || (collisionSet.collisionACIntersects == true) || (collisionSet.collisionBCIntersects == true))
				{																
					if ((sprite.getCurrentAnimation() != DEATH_FALLBACKSLOW) && (sprite.getCurrentAnimation() != DEATH_FALLBACK)
						&& (sprite.getCurrentAnimation() != DEATH_FALLFORWARD))
					{
						LevelMethodsHandleSparkEffects(levelInfo, &myWeaponInfo.Projectiles.at(idx), 6);
						// If the sprite isn't dying, collide with the sucka...
						//spriteIterator->handleCollision(FRONT_COLLISION, 0.1f);

						// We'll have our sprite bounce off of the other sprite if it collides...
						spriteIterator->handleJump(-21.0f);
						
						// If it's not in pain, play the hurty sound, and modify their health...
						if((sprite.getCurrentAnimation() != PAIN_A)	&& (sprite.getCurrentAnimation() != PAIN_B)
							&& (sprite.getCurrentAnimation() != PAIN_C))
						{							
							// If we hit both A and B handle a front collision (change movement by 180 degrees)
							if(collisionSet.collisionABIntersects == true)
							{			
								//sprite.handleCollision(FRONT_ATTACK, (5.0f * myWeaponInfo.WeaponLevelNumber));
								sprite.handleJump(-12.0f);
								sprite.handleCollision(FRONT_ATTACK, 16.0f);
							}
							// If we hit both A and C handle a rear left collision (change movement by 45 degrees)
							if(collisionSet.collisionACIntersects == true)
							{
								//sprite.handleCollision(LEFT_SIDE_ATTACK, (5.0f * myWeaponInfo.WeaponLevelNumber));
								sprite.handleJump(-12.0f);
								sprite.handleCollision(FRONT_ATTACK, 16.0f);

							}
							// If we hit both B and C handle a rear right collision (change movement by 315 degrees)
							if(collisionSet.collisionBCIntersects == true)
							{
								//sprite.handleCollision(RIGHT_SIDE_ATTACK, (5.0f * myWeaponInfo.WeaponLevelNumber));
								sprite.handleJump(-12.0f);
								sprite.handleCollision(FRONT_ATTACK, 16.0f);
							}
														
							// Play a sound and handle damge
							soundToPlay->playOnce();
							sprite.modifyHealthBy(-(projectileDamage * (myWeaponInfo.WeaponLevelNumber * 10)));									
						}
					}

					// If the player killed this sprite, drop a power up
					// If a tower killed this sprite, then add experience to that tower
					ProjectileMethodsHandleIfSpriteDead(sprite, myWeaponInfo);

				}
			}// end if >= water level ish
		}
		spriteIterator++;
	}
}

///<summary>
/// Updates projectiles and handles interaction with elements of level...
/// Returns level state of weapon (e.g. if an upgrade or downgrade occurred)
///</summary>			
PROJECTILE_LEVEL_STATE ProjectileToss::updateWeaponAndProjectiles(LevelInfo *levelInfo)
{
	myWeaponInfo.FiringDelayCounter += (gTimeScale * myWeaponInfo.WeaponLevelNumber);
	ProjectileMethodsUpdateProjectiles(levelInfo, myWeaponInfo);
	return ProjectileMethodsUpdateExperience(myWeaponInfo, TOTAL_LEVELS, MAXIMUM_PROJECTILES);
}

///<summary>
/// Renders the weapon projectiles on the screen ...
///</summary>			
void ProjectileToss::renderProjectiles(IGraphics *graphics)
{
	ProjectileMethodsRender(graphics, myWeaponInfo);
}

///<summary>
/// Handles a change in the trigger state of the weapon...
/// state == pressed/released
/// triggeredBySprite -> The sprite who triggers the projectile to be fired
/// spriteOffset -> The offset created by the tilemap location for the projectile sprite
///</summary>			
bool ProjectileToss::handleTriggerState(PROJECTILE_TRIGGER_STATE state, ISprite *triggeredBySprite, Vector &spriteOffset)
{
	bool returnValue = false;
	// For our tower weapons, we'll use a timer delay to denote their level :)
	myWeaponInfo.FiringDelayCounter += gTimeScale;
	if (myWeaponInfo.FiringDelayCounter > myWeaponInfo.MaxFiringDelay)
	{
		// If the sprite animation is right for this type of gun, and the player just released their weapon...
		returnValue = ProjectileMethodsIsWeaponReady(state, triggeredBySprite, myWeaponInfo);
		if(returnValue == true)
		{
			if (myWeaponInfo.Projectiles.at(myWeaponInfo.CurrentProjectileIndex).isDead() == true)
			{
				triggeredBySprite->setAnimation(CROUCH_ATTACK);

				activateProjectile(myWeaponInfo, &myWeaponInfo.Projectiles.at(myWeaponInfo.CurrentProjectileIndex), triggeredBySprite,
						spriteOffset, myWeaponInfo.ShootingSound);

				ProjectileMethodsIncrementProjectile(myWeaponInfo, MAXIMUM_PROJECTILES);

				ProjectileMethodsReduceExperience(myWeaponInfo, IPROJECTILE_EXP_MODIFIER);

				myWeaponInfo.FiringDelayCounter = 0;
			}
		}
	}
	myWeaponInfo.WeaponState = state;
	return returnValue;
}
