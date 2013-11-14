// ProjectileMethods.h
// Defines delegate implementation for common Projectile oriented procedures
// Author(s): Eagan Rackley
#ifndef PROJECTILE_METHODS_H
#define PROJECTILE_METHODS_H

#include "ProjectileWeaponInfo.h"


/// <summary>
/// Performs projectile initialization
/// </summary>
void ProjectileMethodsInit(IModel *model, IAudio *upgradeSound, IAudio *downGradeSound, IAudio *shootingSound, ProjectileWeaponInfo &weaponInfo,
		int totalLevels, int maxExperience, int maxProjectiles);

/// <summary>
/// Calculates the maximum projectiles available when a level changes...
/// </summary>
void ProjectileMethodsCalculateMaxProjectiles(ProjectileWeaponInfo &weaponInfo, int totalLevels, int maximumProjectiles);

/// <summary>
/// Delegate method to activate an available bullet sprite so that it can shoot off into the wild blue yonder :D
/// </summary>
void ProjectileMethodsActivate(int weaponLevel, float xyModifier, float angleOffset, ISprite *projectile, ISprite *triggeredBySprite, Vector &spriteOffset, IAudio *soundToPlay);

///<summary>
/// Renders the weapon projectiles on the screen ...
///</summary>
void ProjectileMethodsRender(IGraphics *graphics, ProjectileWeaponInfo &weaponInfo);

///<summary>
/// Determines if the triggering sprite is in a state to use this weapon...
///</summary>
bool ProjectileMethodsIsWeaponReady(PROJECTILE_TRIGGER_STATE state, ISprite *triggeredBySprite, ProjectileWeaponInfo &weaponInfo);

///<summary>
/// Increments to the next available projectile
///</summary>
void ProjectileMethodsIncrementProjectile(ProjectileWeaponInfo &weaponInfo, int maxProjectiles);

///<summary>
/// Updates the projectiles for a weapon
///</summary>
void ProjectileMethodsUpdateProjectiles(LevelInfo* levelInfo, ProjectileWeaponInfo &weaponInfo);

///<summary>
/// Updates the current weapon percentage, and level...
///</summary>
PROJECTILE_LEVEL_STATE ProjectileMethodsUpdateExperience(ProjectileWeaponInfo &weaponInfo, int totalLevels, int maximumProjectiles);

///<summary>
/// Sets the experience level of the weapon
///</summary>
void ProjectileMethodsModifyExperienceBy(ProjectileWeaponInfo &weaponInfo, int experiencePoints, int maxExperience);

///<summary>
/// Handles adjusting the experience used by a projectile when it is fired...
///</summary>
void ProjectileMethodsReduceExperience(ProjectileWeaponInfo &weaponInfo, float reduceBy);

///<summary>
/// Handles the situation if a sprite dies due to collision with a projectile
///</summary>
void ProjectileMethodsHandleIfSpriteDead(ISprite &sprite, ProjectileWeaponInfo &weaponInfo);



#endif
