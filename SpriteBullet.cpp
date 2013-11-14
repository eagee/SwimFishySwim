// SpriteBullet.cpp
// Defines implementation for a common interface for dealing with multiple sprite behaviors...
// Author(s): Eagan Rackley
#include "SFSHeaders.h"
#include "ISprite.h"
#include "SpriteBullet.h"
#include "SpriteMethods.h"

///<summary>
/// Constructor for Player Sprite object.
///</summary>
SpriteBullet::SpriteBullet()
{	
	spirteMethodsInitSpriteInfo(&mySpriteInfo);
	mySpriteInfo.Type = BULLET_SPRITE;
	myOffsetValue.x = 0.0f;
	myOffsetValue.y = 0.0f;
	myOffsetValue.z = 0.0f;
	mySpriteInfo.Dead = true;	
	myBulletSize = 0.0f;
}
 
///<summary>
/// Destructor for SpriteBaddie objects - should handle all the cleanup ...
///</summary>
SpriteBullet::~SpriteBullet() 
{	

}

//<summary>
/// [the sprite is dead] returns true, else false
///</summary>	
bool SpriteBullet::isDead()
{	
	return mySpriteInfo.Dead;
}

///<summary>
/// Assigns an IModel instances to a sprite
///</summary>	
void SpriteBullet::assignModel(IModel *model)
{
	spriteMethodsAssignModel(&mySpriteInfo, model);
}	

///<summary>
/// Checks the direction of a sprite and returns true/false if the sprite is facing that direction...
///</summary>			
bool SpriteBullet::checkDirection(DIRECTION direction)
{
	return spriteMethodsCheckDirection(&mySpriteInfo, direction);
}

///<summary>
/// Sets the direction of a sprite, this can be in addition to non-opposing direction
///</summary>
void SpriteBullet::setDirection(DIRECTION direction)
{
	spriteMethodsSetDirection(&mySpriteInfo, direction);
}

///<summary>
/// Increases the speed vector length by the acceleration velocity specified
///</summary>
void SpriteBullet::increaseAcceleration()
{
	spriteMethodsIncreaseAcceleration(&mySpriteInfo);
}

///<summary>
/// Unsets the direction of a sprite
///</summary>
void SpriteBullet::unsetDirection(DIRECTION direction)
{
	spriteMethodsUnsetDirection(&mySpriteInfo, direction);
}


///<summary>
/// Translates the sprite to a new location and resets it's velocity vector to 0.0f for the x, y, and z values.
///</summary>
void SpriteBullet::translateSprite(Vector spriteLocation)
{
	spriteMethodsTranslateSprite(&mySpriteInfo, spriteLocation);
}

///<summary>
/// Modifies velocity based upon direction to handle a wall style collision..
///</summary>
void SpriteBullet::handleCollision(COLLISION_TYPE type, float collisionForce)
{
	// If we're in the water, we want to use the standard collision handler
	if (type < FRONT_LEFT_SLIDE)
	{
		spriteMethodsHandleCollision(&mySpriteInfo, type, collisionForce);
	}
	// If we're stuck up on land then we want to use the sliding collision handler...
	else
	{
		spriteMethodsHandleSlideCollision(&mySpriteInfo, type, collisionForce);
	}
	
	handleJump(-12.0f);

	if(mySpriteInfo.SpriteLevel <= 0)
	{
		mySpriteInfo.Health -= (mySpriteInfo.Health/2);
	}
	else if(mySpriteInfo.SpriteLevel == 1)
	{
		mySpriteInfo.Health -= (mySpriteInfo.Health/2);
	}
	else if(mySpriteInfo.SpriteLevel == 2)
	{
		mySpriteInfo.Health -= (mySpriteInfo.Health/2);
	}
	else if(mySpriteInfo.SpriteLevel == 3)
	{
		mySpriteInfo.Health -= (mySpriteInfo.Health/8);
	}
}

///<summary>
/// Modifies Z Velocity to create a jumping animation
///</summary>
void SpriteBullet::handleJump(float jumpForce)
{	
	//mySpriteInfo.VectorLength = mySpriteInfo.MaxVelocity;
	mySpriteInfo.Velocity.z = jumpForce;	
	mySpriteInfo.JumpState = JUMP_STATE_JUMPING;
}

///<summary>
/// Handles Velocity
///</summary>											
void SpriteBullet::processVelocity()
{			
	spriteMethodsProcessVelocity(&mySpriteInfo);
}

///<summary>
/// Update the current location of the sprite by velocity. If the Player sprite has reached
/// it's maximum x/y limit on the screen then it will return said velcoity so it can be used
/// to update the surrounding environment.
///</summary>							
Vector SpriteBullet::updateLocationByVelocity()
{	
	// Reduce our health as we move ...
	mySpriteInfo.Health -= 0.01f;
	
	if(mySpriteInfo.Health <= 0.5f)
	{		

		//myBulletSize = 0.0f;			
		if(mySpriteInfo.Health <= -0.1f)
		{
			mySpriteInfo.Dead = true;
		}	
		mySpriteInfo.Velocity.x /= 1.1f;
		mySpriteInfo.Velocity.y /= 1.1f;
		mySpriteInfo.VectorLength /= 1.1f;					
	}

	return spriteMethodsUpdateLocationByVelocity(&mySpriteInfo);
}

///<summary>
/// Defines the current x/y/z location of the top left corner of the sprite
///</summary>													
Vector SpriteBullet::getLocation()
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
BoundingTriangle SpriteBullet::getBoundingTriangle()
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
BoundingCircle SpriteBullet::getBoundingCircle()
{
	return spriteMethodsGetBoundingCircle(&mySpriteInfo, 8);
}

///<summary>
/// Sets the distance of all three corners of the bounding triangle (e.g. how far away from the center of the sprite each point is)
///</summary>
void SpriteBullet::setBoundingTriangleSize(float size)
{
	mySpriteInfo.BoundingTriangleSize = size;
}

///<summary>
/// Returns four floats top, bottom, left, and right, which can be used for simple sprite collision calculations...
///</summary>													
BoundingBox SpriteBullet::getBoundingBox()
{
	BoundingBox returnValue;
	returnValue.initialize(spriteMethodsGetBoundingTriangle(&mySpriteInfo));
	/*returnValue.left += myOffsetValue.x;
	returnValue.right += myOffsetValue.x;
	returnValue.top += myOffsetValue.y;
	returnValue.bottom += myOffsetValue.y;
	*/
	return returnValue;
}

///<summary>
/// Defines the current x/y/z location of the top left corner of the sprite but doesn't change velocity like translate does ...
///</summary>													
void SpriteBullet::setLocation(Vector location)
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
void SpriteBullet::setOffset(Vector offset)
{
	myOffsetValue = offset;
}

///<summary>
/// Returns the offset location being applied to the sprite ...
///</summary>													
Vector SpriteBullet::getOffset()
{
	return myOffsetValue;
}

	
///<summary>
/// Defines the width of the sprite including scaling if it applies
///</summary>													
float SpriteBullet::spriteWidth()
{
	return mySpriteInfo.Width;
}
	
///<summary>
/// Defines the width of the sprite including scaling if it applies
///</summary>													
float SpriteBullet::spriteHeight()
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
void SpriteBullet::setVelocities(float maxNormalVelocity, float dragVelocity, float accelerationVelocity)
{
	// Bullet won't change velocity based on control magnitude. The Max is the max...
	mySpriteInfo.UnmodifiedMaxVelocity = maxNormalVelocity;
	mySpriteInfo.MaxVelocity = (mySpriteInfo.UnmodifiedMaxVelocity);
	mySpriteInfo.DragVelocity = dragVelocity;
	mySpriteInfo.AccelerationVelocity = accelerationVelocity;
}

///<summary>
/// Control magnitude can affect the maximum speed of the sprite. This can be used with analog joysticks
/// or maybe adjusted if a sprite is damaged in some way (e.g. can only move at .75 of it's maximum velocity)
/// another example might be traveling through a stick substance can reduce it's control magnitude
///</summary>													
void SpriteBullet::setControlMagnitude(float controlMagnitude)
{
	spriteMethodsSetControlMagnitude(&mySpriteInfo, controlMagnitude);	
}


///<summary>
/// Sets the angle at which our velocity will modify sprite movement...
///</summary>													
void SpriteBullet::setAngle(float angle)
{
	spriteMethodsSetAngle(&mySpriteInfo, angle);
	// This can be changed for other bullets so that they remember the last direction they were fired from...
	mySpriteInfo.AngleDegrees = mySpriteInfo.TargetDegrees;
}

///<summary>
/// Gets the angle at which our velocity will modify sprite movement...
///</summary>															
float SpriteBullet::getAngle()
{
	return mySpriteInfo.AngleDegrees;
}		

///<summary>
/// Gets the angle assigned with setAngle (which is modified by 90 degrees b4 returning with getAngle)
///</summary>															
float SpriteBullet::getControlAngle()
{
	return mySpriteInfo.ControlAngle;
}

		

///<summary>
/// Renders the sprite using an IGraphics implementation...
///</summary>											
void SpriteBullet::renderSprite(IGraphics *graphics)
{	
	if(myBulletSize < 1.0f)
	{
		myBulletSize += 1.0f * gTimeScale;
		mySpriteInfo.BoundingTriangleSize = myBulletSize;
	}
	
	if(mySpriteInfo.spriteRotationValue.value != 0.0f)
	{
		graphics->rotateNextObjectInScene(&mySpriteInfo.spriteRotationValue);	
	}	

	spriteMethodsProcessBulletAnimation(&mySpriteInfo);
	spriteMethodsRenderBulletSprite(&mySpriteInfo, graphics, myOffsetValue);
}

///<summary>
/// Renders the sprite health bar using an IGraphics implementation
///</summary>											
void SpriteBullet::renderHealthBar(IGraphics *graphics)
{
	spriteMethodsRenderHealthBar(&mySpriteInfo, graphics, myOffsetValue);
}

///<summary>
/// IDentifies the behavior associated with an instantiated sprite implementation
///</summary>											
SPRITE_TYPE SpriteBullet::spriteType()
{
	return mySpriteInfo.Type;
}		

///<summary>
/// Sets the animation property of the 3d model ...
///</summary>											
void SpriteBullet::setAnimation(AnimationType animation)
{
	
}

///<summary>
/// Gets the state/animation property of the model ...
///</summary>											
AnimationType SpriteBullet::getCurrentAnimation()
{
	return mySpriteInfo.CurrentAnimation;
}

///<summary>
/// Sets teh value recording the amount of health a sprite has...
///</summary>											
void SpriteBullet::setHealth(float health)
{	
	mySpriteInfo.Health = health;	
	mySpriteInfo.MaxHealth = health;
	mySpriteInfo.Dead = false;
}

///<summary>
/// adjusts the health of the sprite by the specified value
///</summary>											
void SpriteBullet::modifyHealthBy(float healthModifier)
{
	mySpriteInfo.Health += healthModifier;	
	if(mySpriteInfo.Health > mySpriteInfo.MaxHealth) mySpriteInfo.Health = mySpriteInfo.MaxHealth;
}

///<summary>
/// Gets the number of health points left in the sprite...
///</summary>											
float SpriteBullet::getHealth()
{
	return mySpriteInfo.Health;
}

///<summary>
/// Gets the total health that the sprite could contain
///</summary>											
float SpriteBullet::getMaxHealth()
{
	return mySpriteInfo.MaxHealth;
}

///<summary>
/// Sets teh value recording the amount of health a sprite has...
///</summary>											
int SpriteBullet::getPointValue()
{
	return mySpriteInfo.SpritePointValue;
}

///<summary>
/// Sets the value of the sprite in points if it is destroyed
///</summary>											
void SpriteBullet::setPointValue(int value)
{
	mySpriteInfo.SpritePointValue = value;
}

///<summary>
/// Sets a level number for the sprite which can be used to determine sprite behavior
///</summary>											
void SpriteBullet::setLevel(int level)
{
	ASSERT(mySpriteInfo.sprite3dModel != NULL);

	mySpriteInfo.SpriteLevel = level;
		
	
	// Set up the method by which we will display our bullet...
	if(level == 0)
	{		
		mySpriteInfo.CurrentAnimation = PAIN_A;				
		mySpriteInfo.sprite3dModel->setAnimationType(PAIN_A, mySpriteInfo.animationState);
	}
	else if (level == 1)
	{
		mySpriteInfo.CurrentAnimation = PAIN_B;				
		mySpriteInfo.sprite3dModel->setAnimationType(PAIN_B, mySpriteInfo.animationState);
	}
	else if (level == 2)
	{
		mySpriteInfo.CurrentAnimation = PAIN_C;				
		mySpriteInfo.sprite3dModel->setAnimationType(PAIN_C, mySpriteInfo.animationState);
	}
	else if (level == 3)
	{
		mySpriteInfo.CurrentAnimation = CROUCH_PAIN;				
		mySpriteInfo.sprite3dModel->setAnimationType(CROUCH_PAIN, mySpriteInfo.animationState);
	}
	else if (level == 4)
	{
		mySpriteInfo.CurrentAnimation = DEATH_FALLBACK;				
		mySpriteInfo.sprite3dModel->setAnimationType(DEATH_FALLBACK, mySpriteInfo.animationState);
	}
}

///<summary>
/// Gets the level number for the sprite which can be used to determine sprite behavior
///</summary>											
int SpriteBullet::getLevel()
{
	return mySpriteInfo.SpriteLevel;
}

///<summary>
/// Sets the type of power up contained in a dying sprite
///</summary>											
void SpriteBullet::setPowerUpType(SPRITE_POWER_UP powerUpType)
{
	mySpriteInfo.PowerUpType = powerUpType;
}

///<summary>
/// Gets the type of power up contained in a dying sprite
///</summary>											
SPRITE_POWER_UP SpriteBullet::getPowerUpType()
{
	return mySpriteInfo.PowerUpType;
}

///<summary>
/// Sets an ID value used to reference the sprite
///</summary>	
void SpriteBullet::setID(int ID)
{
	mySpriteInfo.SpriteID = ID;
}

///<summary>
/// Gets an ID value used to reference the sprite
///</summary>	
int SpriteBullet::getID()
{
	return mySpriteInfo.SpriteID;
}

///<summary>
/// Returns the Z point that specifies the target z for normal behavior
/// (e.g. after a jump, we want to land at the water level)
///</summary>											
float SpriteBullet::getWaterLevel()
{
	return mySpriteInfo.WaterLevel;
}

///<summary>
/// Sets the z of a water level as necessary...
///</summary>
void SpriteBullet::setWaterLevel(float waterLevel)
{
	mySpriteInfo.WaterLevel = waterLevel;
}

///<summary>
/// Indicates to the sprite object if it is ready for a jump :)
///</summary>											
void SpriteBullet::setJump(JUMP_STATE jumpState)
{
	mySpriteInfo.JumpState = jumpState;
}

///<summary>
/// Returns the JUMP_STATE of the sprite object
///</summary>											
JUMP_STATE SpriteBullet::getJump()
{
	return mySpriteInfo.JumpState;
}

///<summary>
/// Returns the length of the speed vector to indicate the speed of the sprite
///</summary>											
float SpriteBullet::getSpeed()
{
	return (mySpriteInfo.VectorLength / mySpriteInfo.UnmodifiedMaxVelocity);
}

///<summary>
/// Sets what types of attack the sprite is capable of
///</summary>											
void SpriteBullet::setAttackType(ATTACK_TYPE type)
{
	mySpriteInfo.AttackType = type;
}

///<summary>
/// Gets what types of attack the sprite is capable of
///</summary>											
ATTACK_TYPE SpriteBullet::getAttackType()
{
	return mySpriteInfo.AttackType;
}

///<summary>
/// Updates the rotation value a sprite uses (may not be implemented for all objects)
///</summary>											
void SpriteBullet::modifyRotationBy(float value)
{
	mySpriteInfo.spriteRotationValue.value += value;
}

///<summary>
/// Assigns a projectile weapon to the sprite that can be triggered by the sprite...
///</summary>			
void SpriteBullet::assignWeapon(IProjectileWeapon *weapon)
{
}

///<summary>
/// Causes the sprite to change it's current angle to face the specified target...
///</summary>			
void SpriteBullet::lockOnTarget(ISprite &target)
{
}

///<summary>
/// Returns true when a periodic timer or condition has expired. Condition will be reset once true is returned once.
///</summary>
bool SpriteBullet::hasPeriodicTimerExpired()
{
	return spriteMethodsHasTimerExpired(&mySpriteInfo, (DEFAULT_PERIODIC_TIMER * 2));
}

///<summary>
/// Returns the current velocity of the sprite (e.g. The X,Y,Z Momentum of the object)
///</summary>
Vector SpriteBullet::getVelocity()
{
	return mySpriteInfo.Velocity;
}
