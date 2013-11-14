// ProjectileBomb.h
// Defines implementation of IProjectileWeapon to shoot a star with varying behaviors
// Author(s): Eagan Rackley
#ifndef BULLET_PROJECTILE_H
#define BULLET_PROJECTILE_H

#include "IProjectileWeapon.h"
#include "TiledMaps.h"

class ProjectileBomb: public IProjectileWeapon
{
public:
		///<summary>
		/// Constructor...duh
		///</summary>			
		ProjectileBomb(IModel *model, IAudio *upgradeSound, IAudio *downGradeSound, IAudio *shootingSound);
		
		///<summary>
		/// Destru... oh come now, you must know this...
		///</summary>			
		~ProjectileBomb();

		///<summary>
		/// Get Projectile Model
		///</summary>			
		virtual IModel* getProjectileModel();

		///<summary>
		/// Gets a string describing the name of the weapon...
		///</summary>			
		virtual std::string getProjectileName();
		
		///<summary>
		/// Sets the experience level of the weapon
		///</summary>			
		virtual void setWeaponExperience(int experiencePoints);

		///<summary>
		/// Gets the experience points for the weapon
		///</summary>			
		virtual int getWeaponExperience();

		///<summary>
		/// Gets the maximum experience points for the weapon
		///</summary>
		virtual float getMaxWeaponExperience();

		///<summary>
		/// Sets the experience level of the weapon
		///</summary>			
		virtual void modifyWeaponExperienceBy(int experiencePoints);

		///<summary>
		/// Gets the level number of the weapon (based on exp pts)
		///</summary>			
		virtual int getWeaponLevelNumber();

		///<summary>
		/// Gets the total percentage of weapon experience (from 0% to 100%)
		///</summary>			
		virtual float getPercentWeaponExperience();

		///<summary>
		/// Handles projection collisions with other sprites...
		///</summary>			
		virtual void handleSpriteCollisions(LevelInfo *levelInfo, ISprite &sprite, IAudio *soundToPlay, float projectileDamage);

		///<summary>
		/// Updates the weapon and projectile states
		/// Returns state of the weapon level in case the update involved a weapon upgrade...
		///</summary>			
		virtual PROJECTILE_LEVEL_STATE updateWeaponAndProjectiles(LevelInfo *levelInfo);

		///<summary>
		/// Renders the weapon projectiles on the screen ...
		///</summary>			
		virtual void renderProjectiles(IGraphics *graphics);

		///<summary>
		/// Handles a change in the trigger state of the weapon...
		/// state == pressed/released
		/// triggeredBySprite -> The sprite who triggers the projectile to be fired
		/// spriteOffset -> The offset created by the tilemap location for the projectile sprite
		///</summary>			
		virtual bool handleTriggerState(PROJECTILE_TRIGGER_STATE state, ISprite *triggeredBySprite, Vector &spriteOffset);
	

private:
	ProjectileWeaponInfo myWeaponInfo;
	int myCurrentEffectIndex;
	int myCurrentMaxEffects;
};

#endif
