// SpriteTarget.cpp
// Provides an implementation of ISprite interface for non playable sprite characters
// Author(s): Eagan Rackley
#include "SFSHeaders.h"
#include "ISprite.h"
#include "SpriteTarget.h"
#include "SpriteMethods.h"

// Some Const values used for player sprite rotations...

///<summary>
/// Constructor for Player Sprite object.
///</summary>
SpriteTarget::SpriteTarget()
{
	spirteMethodsInitSpriteInfo(&mySpriteInfo);
	mySpriteInfo.Type = ENEMY_TARGET_SPRITE;
	Vector zeroVector = {0.0f, 0.0f, 0.0f};
	myOffsetValue = zeroVector;
	myRotationValue = 0;
}
 
///<summary>
/// Destructor for SpriteTarget objects - should handle all the cleanup ...
///</summary>
SpriteTarget::~SpriteTarget() 
{	

}

//<summary>
/// [the sprite is dead] returns true, else false
///</summary>	
bool SpriteTarget::isDead()
{
	return mySpriteInfo.Dead;
}

///<summary>
/// Assigns an IModel instances to a sprite
///</summary>	
void SpriteTarget::assignModel(IModel *model)
{
	spriteMethodsAssignModel(&mySpriteInfo, model);
}	

///<summary>
/// Checks the direction of a sprite and returns true/false if the sprite is facing that direction...
///</summary>			
bool SpriteTarget::checkDirection(DIRECTION direction)
{
	return spriteMethodsCheckDirection(&mySpriteInfo, direction);
}

///<summary>
/// Sets the direction of a sprite, this can be in addition to non-opposing direction
///</summary>
void SpriteTarget::setDirection(DIRECTION direction)
{
	spriteMethodsSetDirection(&mySpriteInfo, direction);
}

///<summary>
/// Increases the speed vector length by the acceleration velocity specified
///</summary>
void SpriteTarget::increaseAcceleration()
{
	spriteMethodsIncreaseAcceleration(&mySpriteInfo);
}

///<summary>
/// Unsets the direction of a sprite
///</summary>
void SpriteTarget::unsetDirection(DIRECTION direction)
{
	spriteMethodsUnsetDirection(&mySpriteInfo, direction);
}


///<summary>
/// Translates the sprite to a new location and resets it's velocity vector to 0.0f for the x, y, and z values.
///</summary>
void SpriteTarget::translateSprite(Vector spriteLocation)
{
	spriteMethodsTranslateSprite(&mySpriteInfo, spriteLocation);
}

///<summary>
/// Modifies velocity based upon direction to handle a wall style collision..
///</summary>
void SpriteTarget::handleCollision(COLLISION_TYPE type, float collisionForce)
{
	// If we're in the water, we want to use the standard collision handler
		if(type < FRONT_LEFT_SLIDE)
		{
			spriteMethodsHandleCollision(&mySpriteInfo, type, collisionForce);
		}
		// If we're stuck up on land then we want to use the sliding collision handler...
		else
		{
			spriteMethodsHandleSlideCollision(&mySpriteInfo, type, collisionForce);
		}
}

///<summary>
/// Modifies Z Velocity to create a jumping animation
///</summary>
void SpriteTarget::handleJump(float jumpForce)
{
	
}

///<summary>
/// Handles Velocity
///</summary>											
void SpriteTarget::processVelocity()
{			
	spriteMethodsProcessVelocity(&mySpriteInfo);
}

///<summary>
/// Update the current location of the sprite by velocity. If the Player sprite has reached
/// it's maximum x/y limit on the screen then it will return said velcoity so it can be used
/// to update the surrounding environment.
///</summary>							
Vector SpriteTarget::updateLocationByVelocity()
{	
	return spriteMethodsUpdateLocationByVelocity(&mySpriteInfo);
}

///<summary>
/// Defines the current x/y/z location of the top left corner of the sprite
///</summary>													
Vector SpriteTarget::getLocation()
{
	Vector location = mySpriteInfo.Location;
	location.x += myOffsetValue.x;
	location.y += myOffsetValue.y;
	location.z += myOffsetValue.z;
	return location;
}

///<summary>
/// Returns three sets of x/y/z coordinates indicating a bounding triangle for a sprite implementation...
///</summary>													
BoundingTriangle SpriteTarget::getBoundingTriangle()
{
	return spriteMethodsGetBoundingTriangle(&mySpriteInfo);
}

///<summary>
/// The points of the bounding circle are set from the center of the model by the bounding triangle size. There are
/// The circle will be offset from the angle of the sprite and contain eight points [0] - [7].
/// Element [0] will represent degree 0 based on the current angle of the fish. Elements [1] to [7] will represent 45 degree increments
/// from element [0] (e.g. 0, 45, 90, 135, 180, 225, 270, 315). The value stored in each element will be vertex coordinates at the
/// specified bounding size / angle away from the center of the sprite...
///</summary>
BoundingCircle SpriteTarget::getBoundingCircle()
{
	return spriteMethodsGetBoundingCircle(&mySpriteInfo, 8);
}

///<summary>
/// Sets the distance of all three corners of the bounding triangle (e.g. how far away from the center of the sprite each point is)
///</summary>
void SpriteTarget::setBoundingTriangleSize(float size)
{
	mySpriteInfo.BoundingTriangleSize = size;
}

///<summary>
/// Returns four floats top, bottom, left, and right, which can be used for simple sprite collision calculations...
///</summary>													
BoundingBox SpriteTarget::getBoundingBox()
{
	BoundingBox returnValue;
	returnValue.initialize(spriteMethodsGetBoundingTriangle(&mySpriteInfo));
	/*returnValue.left += myOffsetValue.x;
	returnValue.right += myOffsetValue.x;
	returnValue.top += myOffsetValue.y;
	returnValue.bottom += myOffsetValue.y;*/
	return returnValue;
}

///<summary>
/// Defines the current x/y/z location of the top left corner of the sprite
///</summary>													
void SpriteTarget::setLocation(Vector location)
{
	// If we got the location of this sprite, then we need to adjust the location by our offset...
	location.x -= myOffsetValue.x;
	location.y -= myOffsetValue.y;
	location.z -= myOffsetValue.z;
	
	spriteMethodsSetLocation(location, &mySpriteInfo);
}

///<summary>
/// Only used in non player sprites, this sets an offset location for the sprite...
///</summary>													
void SpriteTarget::setOffset(Vector offset)
{
	myOffsetValue = offset;
}

///<summary>
/// Returns the offset location being applied to the sprite ...
///</summary>													
Vector SpriteTarget::getOffset()
{
	return myOffsetValue;
}

	
///<summary>
/// Defines the width of the sprite including scaling if it applies
///</summary>													
float SpriteTarget::spriteWidth()
{
	return mySpriteInfo.Width;
}
	
///<summary>
/// Defines the width of the sprite including scaling if it applies
///</summary>													
float SpriteTarget::spriteHeight()
{
	return mySpriteInfo.Height;
}
	
///<summary>
/// Sets the speeds at which the sprite travels based on direction...
/// maxNormalVelocity is the maximum normal speed the sprite can go
/// maxBurstVelocity is the maximum burst speed the sprite can go (burst is a special speed increase)
/// dragVelocity is how much speed is reduced when the sprite isn't moving in a direction (until speed is 0)
/// modifyVelocityBy is how much speed is increased when the sprite is moving in a direction (until speed is maximumvelocity)
///</summary>									
void SpriteTarget::setVelocities(float maxNormalVelocity, float dragVelocity, float accelerationVelocity)
{
	spriteMethodsSetVelocities(&mySpriteInfo, maxNormalVelocity, dragVelocity, accelerationVelocity);
}

///<summary>
/// Control magnitude can affect the maximum speed of the sprite. This can be used with analog joysticks
/// or maybe adjusted if a sprite is damaged in some way (e.g. can only move at .75 of it's maximum velocity)
/// another example might be traveling through a stick substance can reduce it's control magnitude
///</summary>													
void SpriteTarget::setControlMagnitude(float controlMagnitude)
{
	spriteMethodsSetControlMagnitude(&mySpriteInfo, controlMagnitude);	
}


///<summary>
/// Sets the angle at which our velocity will modify sprite movement...
///</summary>													
void SpriteTarget::setAngle(float angle)
{
	spriteMethodsSetAngle(&mySpriteInfo, angle);
}

///<summary>
/// Gets the angle at which our velocity will modify sprite movement...
///</summary>															
float SpriteTarget::getAngle()
{
	return mySpriteInfo.TargetDegrees;
}		

///<summary>
/// Gets the angle assigned with setAngle (which is modified by 90 degrees b4 returning with getAngle)
///</summary>															
float SpriteTarget::getControlAngle()
{
	return mySpriteInfo.ControlAngle;
}
		

///<summary>
/// Renders the sprite using an IGraphics implementation...
///</summary>											
void SpriteTarget::renderSprite(IGraphics *graphics)
{	
	spriteMethodsProcessAnimation(&mySpriteInfo);	
		
	//myRotationValue += 4.0f;
	//Rotate rotateBy = {{0.0, 0.0, 1.0}, myRotationValue};
	//graphics->rotateNextObjectInScene(&rotateBy);

	spriteMethodsRenderSprite(&mySpriteInfo, graphics, myOffsetValue, true);
	//spriteMethodsRenderBulletSprite(&mySpriteInfo, graphics, myOffsetValue);
	mySpriteInfo.ControlMagnitude = 1.0f;	
}

///<summary>
/// Renders the sprite health bar using an IGraphics implementation
///</summary>											
void SpriteTarget::renderHealthBar(IGraphics *graphics)
{
	spriteMethodsRenderHealthBar(&mySpriteInfo, graphics, myOffsetValue);
}

///<summary>
/// IDentifies the behavior associated with an instantiated sprite implementation
///</summary>											
SPRITE_TYPE SpriteTarget::spriteType()
{
	return mySpriteInfo.Type;
}		

///<summary>
/// Sets the animation property of the 3d model ...
///</summary>											
void SpriteTarget::setAnimation(AnimationType animation)
{
	mySpriteInfo.CurrentAnimation = animation;
}

///<summary>
/// Gets the state/animation property of the model ...
///</summary>											
AnimationType SpriteTarget::getCurrentAnimation()
{
	return mySpriteInfo.CurrentAnimation;
}

///<summary>
/// Sets teh value recording the amount of health a sprite has...
///</summary>											
void SpriteTarget::setHealth(float health)
{
	mySpriteInfo.Health = health;	
	mySpriteInfo.MaxHealth = health;
}

///<summary>
/// adjusts the health of the sprite by the specified value
///</summary>											
void SpriteTarget::modifyHealthBy(float healthModifier)
{
	mySpriteInfo.Health += healthModifier;	
	if(mySpriteInfo.Health > mySpriteInfo.MaxHealth) mySpriteInfo.Health = mySpriteInfo.MaxHealth;	
}

///<summary>
/// Gets the number of health points left in the sprite...
///</summary>											
float SpriteTarget::getHealth()
{
	return mySpriteInfo.Health;
}

///<summary>
/// Gets the total health that the sprite could contain
///</summary>											
float SpriteTarget::getMaxHealth()
{
	return mySpriteInfo.MaxHealth;
}


///<summary>
/// Sets teh value recording the amount of health a sprite has...
///</summary>											
int SpriteTarget::getPointValue()
{
	return mySpriteInfo.SpritePointValue;
}

///<summary>
/// Sets the value of the sprite in points if it is destroyed
///</summary>											
void SpriteTarget::setPointValue(int value)
{
	mySpriteInfo.SpritePointValue = value;
}

///<summary>
/// Sets a level number for the sprite which can be used to determine sprite behavior
///</summary>											
void SpriteTarget::setLevel(int level)
{
	mySpriteInfo.SpriteLevel = level;
}

///<summary>
/// Gets the level number for the sprite which can be used to determine sprite behavior
///</summary>											
int SpriteTarget::getLevel()
{
	return mySpriteInfo.SpriteLevel;
}

///<summary>
/// Sets the type of power up contained in a dying sprite
///</summary>											
void SpriteTarget::setPowerUpType(SPRITE_POWER_UP powerUpType)
{
	mySpriteInfo.PowerUpType = powerUpType;
}

///<summary>
/// Gets the type of power up contained in a dying sprite
///</summary>											
SPRITE_POWER_UP SpriteTarget::getPowerUpType()
{
	return mySpriteInfo.PowerUpType;
}

///<summary>
/// Sets an ID value used to reference the sprite
///</summary>	
void SpriteTarget::setID(int ID)
{
	mySpriteInfo.SpriteID = ID;
}

///<summary>
/// Gets an ID value used to reference the sprite
///</summary>	
int SpriteTarget::getID()
{
	return mySpriteInfo.SpriteID;
}

///<summary>
/// Returns the Z point that specifies the target z for normal behavior
/// (e.g. after a jump, we want to land at the water level)
///</summary>											
float SpriteTarget::getWaterLevel()
{
	return mySpriteInfo.WaterLevel;
}

///<summary>
/// Sets the z of a water level as necessary...
///</summary>
void SpriteTarget::setWaterLevel(float waterLevel)
{
	mySpriteInfo.WaterLevel = waterLevel;
}

///<summary>
/// Indicates to the sprite object if it is ready for a jump :)
///</summary>											
void SpriteTarget::setJump(JUMP_STATE jumpState)
{
	mySpriteInfo.JumpState = jumpState;
}

///<summary>
/// Returns the JUMP_STATE of the sprite object
///</summary>											
JUMP_STATE SpriteTarget::getJump()
{
	return mySpriteInfo.JumpState;
}

///<summary>
/// Returns the length of the speed vector to indicate the speed of the sprite
///</summary>											
float SpriteTarget::getSpeed()
{
	return (mySpriteInfo.VectorLength / mySpriteInfo.UnmodifiedMaxVelocity);
}

///<summary>
/// Sets what types of attack the sprite is capable of
///</summary>											
void SpriteTarget::setAttackType(ATTACK_TYPE type)
{
	mySpriteInfo.AttackType = type;
}

///<summary>
/// Gets what types of attack the sprite is capable of
///</summary>											
ATTACK_TYPE SpriteTarget::getAttackType()
{
	return mySpriteInfo.AttackType;
}

///<summary>
/// Updates the rotation value a sprite uses (may not be implemented for all objects)
///</summary>											
void SpriteTarget::modifyRotationBy(float value)
{
}

///<summary>
/// Assigns a projectile weapon to the sprite that can be triggered by the sprite...
///</summary>			
void SpriteTarget::assignWeapon(IProjectileWeapon *weapon)
{
}

///<summary>
/// Causes the sprite to change it's current angle to face the specified target...
///</summary>			
void SpriteTarget::lockOnTarget(ISprite &target)
{
	spriteMethodsLockOnTarget(target, &mySpriteInfo);
}

///<summary>
/// Returns true when a periodic timer or condition has expired. Condition will be reset once true is returned once.
///</summary>
bool SpriteTarget::hasPeriodicTimerExpired()
{
	return spriteMethodsHasTimerExpired(&mySpriteInfo, DEFAULT_PERIODIC_TIMER);
}

///<summary>
/// Returns the current velocity of the sprite (e.g. The X,Y,Z Momentum of the object)
///</summary>
Vector SpriteTarget::getVelocity()
{
	return mySpriteInfo.Velocity;
}
