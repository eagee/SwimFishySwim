// SpritePlayer.h
// Defines implementation for a common interface for dealing with multiple sprite behaviors...
// Author(s): Eagan Rackley
#ifndef PLAYER_SPRITE_H
#define PLAYER_SPRITE_H

#include "ISprite.h"

///<summary>
/// Provides an interface for sprite behavior, whether that is a player sprite, or an NPC of some sort ...
///</summary>
class SpritePlayer: public ISprite
{ 
	public:
		
		SpritePlayer();
		
		virtual ~SpritePlayer();

		///<summary>
		/// Sets an ID value used to reference the sprite
		///</summary>	
		virtual void setID(int ID);
		
		///<summary>
		/// Gets an ID value used to reference the sprite
		///</summary>	
		virtual int getID();

		///<summary>
		/// [the sprite is dead] returns true, else false
		///</summary>	
		virtual bool isDead();		
		
		///<summary>
		/// Assigns an IModel instances to a sprite
		///</summary>	
		virtual void assignModel(IModel *model);

		///<summary>
		/// Assigns a projectile weapon to the sprite that can be triggered by the sprite...
		///</summary>			
		virtual void assignWeapon(IProjectileWeapon *weapon);

		///<summary>
		/// Causes the sprite to change it's current angle to face the specified target...
		///</summary>			
		virtual void lockOnTarget(ISprite &target);
		
		///<summary>
		/// Checks the direction of a sprite and returns true/false if the sprite is facing that direction...
		///</summary>			
		virtual bool checkDirection(DIRECTION direction);
		
		///<summary>
		/// Modifies velocity based upon direction to handle a wall style collision..
		///</summary>
		virtual void handleCollision(COLLISION_TYPE type, float collisionForce);
		
		///<summary>
		/// Modifies Z Velocity to create a jumping animation
		///</summary>
		virtual void handleJump(float jumpForce);
		
		///<summary>
		/// Sets the direction of a sprite ...
		///</summary>					
		virtual void setDirection(DIRECTION direction);		
		
		///<summary>
		/// UnSets the direction of a sprite ...
		///</summary>							
		virtual void unsetDirection(DIRECTION direction);				
		
		///<summary>
		/// Translates the location of a sprite ...
		///</summary>							
		virtual void translateSprite(Vector spriteLocation);
		
		///<summary>
		/// Update the current location of the sprite by velocity and returns any affect that may have on the map...
		///</summary>							
		virtual Vector updateLocationByVelocity();
		
		///<summary>
		/// Gets the current x/y/z location of the top left corner of the sprite
		///</summary>													
		virtual Vector getLocation();
		
		///<summary>
		/// Returns three sets of x/y/z coordinates indicating a bounding triangle for a sprite implementation...
		///</summary>													
		BoundingTriangle getBoundingTriangle();
		
		///<summary>
		/// The points of the bounding circle are set from the center of the model by the bounding triangle size. There are
		/// The circle will be offset from the angle of the sprite and contain eight points [0] - [7].
		/// Element [0] will represent degree 0 based on the current angle of the fish. Elements [1] to [7] will represent 45 degree increments
		/// from element [0] (e.g. 0, 45, 90, 135, 180, 225, 270, 315). The value stored in each element will be vertex coordinates at the
		/// specified bounding size / angle away from the center of the sprite...
		///</summary>
		virtual BoundingCircle getBoundingCircle();

		///<summary>
		/// Sets the distance of all three corners of the bounding triangle (e.g. how far away from the center of the sprite each point is)
		///</summary>
		virtual void setBoundingTriangleSize(float size);

		///<summary>
		/// Returns four floats top, bottom, left, and right, which can be used for simple sprite collision calculations...
		///</summary>													
		BoundingBox getBoundingBox();		
		
		///<summary>
		/// Defines the current x/y/z location of the top left corner of the sprite
		///</summary>													
		virtual void setLocation(Vector location);
		
		///<summary>
		/// Only used in non player sprites, this sets an offset location for the sprite...
		///</summary>													
		virtual void setOffset(Vector offset);

		///<summary>
		/// Returns the offset location being applied to the sprite ...
		///</summary>													
		Vector getOffset();
		
		///<summary>
		/// Defines the width of the sprite including scaling if it applies
		///</summary>													
		virtual float spriteWidth();
		
		///<summary>
		/// Defines the width of the sprite including scaling if it applies
		///</summary>													
		virtual float spriteHeight();
		
		///<summary>
		/// Sets the speeds at which the sprite travels based on direction...
		/// maxNormalVelocity is the maximum normal speed the sprite can go
		/// maxBurstVelocity is the maximum burst speed the sprite can go (burst is a special speed increase)
		/// dragVelocity is how much speed is reduced when the sprite isn't moving in a direction (until speed is 0)		
		///</summary>									
		virtual void setVelocities(float maxNormalVelocity, float dragVelocity, float accelerationVelocity);
		
		///<summary>
		/// Increases the speed vector length by the acceleration velocity specified
		///</summary>
		virtual void increaseAcceleration();

		///<summary>
		/// Control magnitude can affect the maximum speed of the sprite. This can be used with analog joysticks
		/// or maybe adjusted if a sprite is damaged in some way (e.g. can only move at .75 of it's maximum velocity)
		/// another example might be traveling through a stick substance can reduce it's control magnitude
		///</summary>													
		void setControlMagnitude(float controlMagnitude);

		///<summary>
		/// Sets the angle at which our velocity will modify sprite movement...
		///</summary>													
		void setAngle(float angle);
		
		///<summary>
		/// Gets the angle at which our velocity will modify sprite movement...
		///</summary>															
		virtual float getAngle();

		///<summary>
		/// Gets the angle assigned with setAngle (which is modified by 90 degrees b4 returning with getAngle)
		///</summary>															
		virtual float getControlAngle();
				
		///<summary>
		/// Renders the sprite using an IGraphics implementation...
		///</summary>											
		virtual void renderSprite(IGraphics *graphics);	

		///<summary>
		/// Renders the sprite health bar using an IGraphics implementation
		///</summary>											
		virtual void renderHealthBar(IGraphics *graphics);		
		
		///<summary>
		/// IDentifies the behavior associated with an instantiated sprite implementation
		///</summary>											
		virtual SPRITE_TYPE spriteType();
		
		///<summary>
		/// Sets the animation property of the 3d model ...
		///</summary>											
		virtual void setAnimation(AnimationType animation);		
		
		///<summary>
		/// Gets the state/animation property of the model ...
		///</summary>											
		virtual AnimationType getCurrentAnimation();

		///<summary>
		/// Sets teh value recording the amount of health a sprite has...
		///</summary>											
		virtual void setHealth(float health);

		///<summary>
		/// adjusts the health of the sprite by the specified value
		///</summary>											
		virtual void modifyHealthBy(float healthModifier);

		///<summary>
		/// Gets the number of health points left in the sprite...
		///</summary>											
		virtual float getHealth();

		///<summary>
		/// Gets the total health that the sprite could contain
		///</summary>											
		virtual float getMaxHealth();

		///<summary>
		/// Sets teh value recording the amount of health a sprite has...
		///</summary>											
		virtual int getPointValue();

		///<summary>
		/// Sets the value of the sprite in points if it is destroyed
		///</summary>											
		virtual void setPointValue(int value);

		///<summary>
		/// Sets a level number for the sprite which can be used to determine sprite behavior
		///</summary>											
		virtual void setLevel(int level);

		///<summary>
		/// Gets the level number for the sprite which can be used to determine sprite behavior
		///</summary>											
		virtual int getLevel();

		///<summary>
		/// Sets the type of power up contained in a dying sprite
		///</summary>											
		virtual void setPowerUpType(SPRITE_POWER_UP powerUpType);

		///<summary>
		/// Gets the type of power up contained in a dying sprite
		///</summary>											
		virtual SPRITE_POWER_UP getPowerUpType();

		///<summary>
		/// Returns the Z point that specifies the target z for normal behavior
		/// (e.g. after a jump, we want to land at the water level)
		///</summary>											
		virtual float getWaterLevel();

		///<summary>
		/// Sets the z of a water level as necessary...
		///</summary>
		virtual void setWaterLevel(float waterLevel);

		///<summary>
		/// Indicates to the sprite object if it is ready for a jump :)
		///</summary>											
		virtual void setJump(JUMP_STATE jumpState);

		///<summary>
		/// Returns the JUMP_STATE of the sprite object
		///</summary>											
		virtual JUMP_STATE getJump();

		///<summary>
		/// Returns the length of the speed vector to indicate the speed of the sprite
		///</summary>											
		virtual float getSpeed();

		///<summary>
		/// Sets what types of attack the sprite is capable of
		///</summary>											
		virtual void setAttackType(ATTACK_TYPE type);
		
		///<summary>
		/// Gets what types of attack the sprite is capable of
		///</summary>											
		virtual ATTACK_TYPE getAttackType();

		///<summary>
		/// Updates the rotation value a sprite uses (may not be implemented for all objects)
		///</summary>											
		virtual void modifyRotationBy(float value);
		
		///<summary>
		/// Returns true when a periodic timer or condition has expired. Condition will be reset once true is returned once.
		///</summary>
		virtual bool hasPeriodicTimerExpired();

		///<summary>
		/// Returns the current velocity of the sprite (e.g. The X,Y,Z Momentum of the object)
		///</summary>
		virtual Vector getVelocity();


	private:
	
		///<summary>
		/// Handles Velocity changes in the sprite
		///</summary>											
		void processVelocity();		
		
		// Common sprite attributes
		SpriteInfo mySpriteInfo;	
		Vector myOffsetValue;
};

#endif
