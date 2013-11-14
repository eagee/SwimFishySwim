// IProjectileWeapon.h
// Defines interface for all projectile weapons in the game
// Author(s): Eagan Rackley



#ifndef IPROJECTILE_WEAPON_H
#define IPROJECTILE_WEAPON_H

#include "ProjectileWeaponInfo.h"

// Some forward declarations...
class IGraphics;
class IAudio;
class ISprite;
class SpriteBullet;
class TiledMaps;

#define IPROJECTILE_EXP_MODIFIER 20.0f

///<summary>
/// Defines interface for all projectile weapons in the game
///</summary>
class IProjectileWeapon
{ 
	public:
		IProjectileWeapon(){}
		
		virtual ~IProjectileWeapon(){}

		///<summary>
		/// Get Projectile Model
		///</summary>			
		virtual IModel* getProjectileModel() = 0;

		///<summary>
		/// Gets a string describing the name of the weapon...
		///</summary>			
		virtual std::string getProjectileName() = 0;
		
		///<summary>
		/// Sets the experience level of the weapon
		///</summary>			
		virtual void setWeaponExperience(int experiencePoints) = 0;

		///<summary>
		/// Gets the maximum experience points for the weapon
		///</summary>
		virtual float getMaxWeaponExperience() = 0;


		///<summary>
		/// Gets the current experience points for the weapon
		///</summary>			
		virtual int getWeaponExperience() = 0;

		///<summary>
		/// Gets the level number of the weapon (based on exp pts)
		///</summary>			
		virtual int getWeaponLevelNumber() = 0;

		///<summary>
		/// Gets the total percentage of weapon experience (from 0% to 100%)
		///</summary>			
		virtual float getPercentWeaponExperience() = 0;

		///<summary>
		/// Sets the experience level of the weapon
		///</summary>			
		virtual void modifyWeaponExperienceBy(int experiencePoints) = 0;

		///<summary>
		/// Handles projection collisions with other sprites...
		///</summary>			
		virtual void handleSpriteCollisions(LevelInfo *levelInfo, ISprite &sprite, IAudio *soundToPlay, float projectileDamage) = 0;

		///<summary>
		/// Updates projectiles and handles interaction with elements of level...
		///</summary>			
		virtual PROJECTILE_LEVEL_STATE updateWeaponAndProjectiles(LevelInfo *levelInfo) = 0;

		///<summary>
		/// Renders the weapon projectiles on the screen ...
		///</summary>			
		virtual void renderProjectiles(IGraphics *graphics) = 0;

		///<summary>
		/// Handles a change in the trigger state of the weapon...
		// state == pressed/released
		// triggeredBySprite -> The sprite who triggers the projectile to be fired
		// spriteOffset -> The offset created by the tilemap location for the projectile sprite
		///</summary>			
		/// <returns>Returns true if triggered, otherwise false</returns>
		virtual bool handleTriggerState(PROJECTILE_TRIGGER_STATE state, ISprite *triggeredBySprite, Vector &spriteOffset) = 0;
};

#endif
