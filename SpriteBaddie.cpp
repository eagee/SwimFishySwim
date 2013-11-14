// SpriteBaddie.cpp
// Provides an implementation of ISprite interface for non playable sprite characters
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "ISprite.h"
#include "SpriteBaddie.h"
#include "SpriteMethods.h"

///<summary>
/// Constructor for Player Sprite object.
///</summary>
SpriteBaddie::SpriteBaddie()
{
	spirteMethodsInitSpriteInfo(&mySpriteInfo);	
	mySpriteInfo.Type = BADDIE_SPRITE;
	myOffsetValue.x = 0.0f;
	myOffsetValue.y = 0.0f;
	myOffsetValue.z = 0.0f;
	mySpriteInfo.Dead = true;		
	myCurrentWaypoint.x = 0.0f;
	myCurrentWaypoint.y = 0.0f;
	myCurrentWaypoint.z = 0.0f;
	setAngle(180.0f);
	mySpriteInfo.AngleDegrees = mySpriteInfo.TargetDegrees;	
}
 
///<summary>
/// Destructor for SpriteBaddie objects - should handle all the cleanup ...
///</summary>
SpriteBaddie::~SpriteBaddie() 
{	
	
}

//<summary>
/// [the sprite is dead] returns true, else false
///</summary>	
bool SpriteBaddie::isDead()
{
	return mySpriteInfo.Dead;
}

///<summary>
/// Assigns an IModel instances to a sprite
///</summary>	
void SpriteBaddie::assignModel(IModel *model)
{
	spriteMethodsAssignModel(&mySpriteInfo, model);
}	

///<summary>
/// Checks the direction of a sprite and returns true/false if the sprite is facing that direction...
///</summary>			
bool SpriteBaddie::checkDirection(DIRECTION direction)
{
	return spriteMethodsCheckDirection(&mySpriteInfo, direction);
}

///<summary>
/// Sets the direction of a sprite, this can be in addition to non-opposing direction
///</summary>
void SpriteBaddie::setDirection(DIRECTION direction)
{
	spriteMethodsSetDirection(&mySpriteInfo, direction);
}

///<summary>
/// Increases the speed vector length by the acceleration velocity specified
///</summary>
void SpriteBaddie::increaseAcceleration()
{
	spriteMethodsIncreaseAcceleration(&mySpriteInfo);
}

///<summary>
/// Unsets the direction of a sprite
///</summary>
void SpriteBaddie::unsetDirection(DIRECTION direction)
{
	spriteMethodsUnsetDirection(&mySpriteInfo, direction);
}


///<summary>
/// Translates the sprite to a new location and resets it's velocity vector to 0.0f for the x, y, and z values.
///</summary>
void SpriteBaddie::translateSprite(Vector spriteLocation)
{
	spriteMethodsTranslateSprite(&mySpriteInfo, spriteLocation);
}

///<summary>
/// Modifies velocity based upon direction to handle a wall style collision..
///</summary>
void SpriteBaddie::handleCollision(COLLISION_TYPE type, float collisionForce)
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
void SpriteBaddie::handleJump(float jumpForce)
{
	// This will move the sprite up from down below...
	mySpriteInfo.Velocity.z = -12.0f;	
	mySpriteInfo.Velocity.x = 0;
	mySpriteInfo.Velocity.y = 0;
	mySpriteInfo.VectorLength = 0;
	mySpriteInfo.JumpState = JUMP_STATE_JUMPING;
}

///<summary>
/// Handles Velocity
///</summary>											
void SpriteBaddie::processVelocity()
{			
	spriteMethodsProcessVelocity(&mySpriteInfo);
}

///<summary>
/// Update the current location of the sprite by velocity. If the Player sprite has reached
/// it's maximum x/y limit on the screen then it will return said velcoity so it can be used
/// to update the surrounding environment.
///</summary>							
Vector SpriteBaddie::updateLocationByVelocity()
{	
	return spriteMethodsUpdateLocationByVelocity(&mySpriteInfo);
}

///<summary>
/// Defines the current x/y/z location of the top left corner of the sprite
///</summary>													
Vector SpriteBaddie::getLocation()
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
BoundingTriangle SpriteBaddie::getBoundingTriangle()
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
BoundingCircle SpriteBaddie::getBoundingCircle()
{
	return spriteMethodsGetBoundingCircle(&mySpriteInfo, 8);
}

///<summary>
/// Sets the distance of all three corners of the bounding triangle (e.g. how far away from the center of the sprite each point is)
///</summary>
void SpriteBaddie::setBoundingTriangleSize(float size)
{
	mySpriteInfo.BoundingTriangleSize = size;
}


///<summary>
/// Returns four floats top, bottom, left, and right, which can be used for simple sprite collision calculations...
///</summary>													
BoundingBox SpriteBaddie::getBoundingBox()
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
/// Defines the current x/y/z location of the top left corner of the sprite
///</summary>													
void SpriteBaddie::setLocation(Vector location)
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
void SpriteBaddie::setOffset(Vector offset)
{
	myOffsetValue = offset;
}

///<summary>
/// Returns the offset location being applied to the sprite ...
///</summary>													
Vector SpriteBaddie::getOffset()
{
	return myOffsetValue;
}
	
///<summary>
/// Defines the width of the sprite including scaling if it applies
///</summary>													
float SpriteBaddie::spriteWidth()
{
	return mySpriteInfo.Width;
}
	
///<summary>
/// Defines the width of the sprite including scaling if it applies
///</summary>													
float SpriteBaddie::spriteHeight()
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
void SpriteBaddie::setVelocities(float maxNormalVelocity, float dragVelocity, float accelerationVelocity)
{
	spriteMethodsSetVelocities(&mySpriteInfo, maxNormalVelocity, dragVelocity, accelerationVelocity);
}

///<summary>
/// Control magnitude can affect the maximum speed of the sprite. This can be used with analog joysticks
/// or maybe adjusted if a sprite is damaged in some way (e.g. can only move at .75 of it's maximum velocity)
/// another example might be traveling through a stick substance can reduce it's control magnitude
///</summary>													
void SpriteBaddie::setControlMagnitude(float controlMagnitude)
{
	spriteMethodsSetControlMagnitude(&mySpriteInfo, controlMagnitude);	
}


///<summary>
/// Sets the angle at which our velocity will modify sprite movement...
///</summary>													
void SpriteBaddie::setAngle(float angle)
{
	spriteMethodsSetAngle(&mySpriteInfo, angle);
}

///<summary>
/// Gets the angle at which our velocity will modify sprite movement...
///</summary>															
float SpriteBaddie::getAngle()
{
	return mySpriteInfo.AngleDegrees;
}		

///<summary>
/// Gets the angle assigned with setAngle (which is modified by 90 degrees b4 returning with getAngle)
///</summary>															
float SpriteBaddie::getControlAngle()
{
	return mySpriteInfo.ControlAngle;
}

///<summary>
/// Renders the sprite using an IGraphics implementation...
///</summary>											
void SpriteBaddie::renderSprite(IGraphics *graphics)
{
	spriteMethodsProcessAnimation(&mySpriteInfo);
	spriteMethodsRenderSprite(&mySpriteInfo, graphics, myOffsetValue, true);

	/// Track our node history here by evaluating the current x/y value. If it changes
	/// assign the old x/y value to our node history

	Vector currentLocation;
	currentLocation.x = fabs(mySpriteInfo.Location.x);
	currentLocation.y = fabs(mySpriteInfo.Location.y);
	// Perform a comparison to see if we've reached our intended waypoint. 
	if( ((myCurrentWaypoint.x == 0.0f) && (myCurrentWaypoint.y == 0.0f))
		|| ((fabs(currentLocation.x - myCurrentWaypoint.x) <= 2.0f) && (fabs(currentLocation.y - myCurrentWaypoint.y) <= 2.0f)))
	{
		// If we have then we need to dequeue our next waypoint (if there is one). Otherwise this target stays the same...
		if(myNodeWaypoints.size() > 0)
		{
			myCurrentWaypoint = myNodeWaypoints.front();
			myNodeWaypoints.pop_front();
		}
	}
}

///<summary>
/// Renders the sprite health bar using an IGraphics implementation
///</summary>											
void SpriteBaddie::renderHealthBar(IGraphics *graphics)
{
	spriteMethodsRenderHealthBar(&mySpriteInfo, graphics, myOffsetValue);
}

///<summary>
/// IDentifies the behavior associated with an instantiated sprite implementation
///</summary>											
SPRITE_TYPE SpriteBaddie::spriteType()
{
	return mySpriteInfo.Type;
}		

///<summary>
/// Sets the animation property of the 3d model ...
///</summary>											
void SpriteBaddie::setAnimation(AnimationType animation)
{
	mySpriteInfo.CurrentAnimation = animation;
}

///<summary>
/// Gets the state/animation property of the model ...
///</summary>											
AnimationType SpriteBaddie::getCurrentAnimation()
{
	return mySpriteInfo.CurrentAnimation;
}

///<summary>
/// Sets teh value recording the amount of health a sprite has...
///</summary>											
void SpriteBaddie::setHealth(float health)
{
	mySpriteInfo.Health = health;	
	mySpriteInfo.MaxHealth = health;
	if(mySpriteInfo.Health > 0.0f) 
	{
		mySpriteInfo.Dead = false;
	}
}

///<summary>
/// adjusts the health of the sprite by the specified value
///</summary>											
void SpriteBaddie::modifyHealthBy(float healthModifier)
{
	mySpriteInfo.Health += healthModifier;	
	if(mySpriteInfo.Health > mySpriteInfo.MaxHealth) mySpriteInfo.Health = mySpriteInfo.MaxHealth;
	// Pain A moves into death in the health <= 0 inside of the spriteMethods animation codeb
	if((mySpriteInfo.CurrentAnimation != DEATH_FALLBACKSLOW) 
		&& (mySpriteInfo.CurrentAnimation != DEATH_FALLBACK)
		&& (mySpriteInfo.CurrentAnimation != DEATH_FALLFORWARD)
		&& (mySpriteInfo.CurrentAnimation != PAIN_A) 
		&& (mySpriteInfo.CurrentAnimation != PAIN_B) 
		&& (mySpriteInfo.CurrentAnimation != PAIN_C))
	{		
		int painAnim = rand() % 2;
		if(painAnim == 0)
		{
			mySpriteInfo.CurrentAnimation = PAIN_A;	
		}
		else if(painAnim == 1)
		{
			mySpriteInfo.CurrentAnimation = PAIN_B;	
		}
		else if(painAnim == 2)
		{
			mySpriteInfo.CurrentAnimation = PAIN_C;	
		}
	}
}

///<summary>
/// Gets the number of health points left in the sprite...
///</summary>											
float SpriteBaddie::getHealth()
{
	return mySpriteInfo.Health;
}

///<summary>
/// Gets the total health that the sprite could contain
///</summary>											
float SpriteBaddie::getMaxHealth()
{
	return mySpriteInfo.MaxHealth;
}

///<summary>
/// Sets teh value recording the amount of health a sprite has...
///</summary>											
int SpriteBaddie::getPointValue()
{
	int returnValue = mySpriteInfo.SpritePointValue;	
	mySpriteInfo.SpritePointValue = 0;

	return returnValue;;
}

///<summary>
/// Sets the value of the sprite in points if it is destroyed
///</summary>											
void SpriteBaddie::setPointValue(int value)
{
	mySpriteInfo.SpritePointValue = value;
}

///<summary>
/// Sets a level number for the sprite which can be used to determine sprite behavior
///</summary>											
void SpriteBaddie::setLevel(int level)
{
	mySpriteInfo.SpriteLevel = level;
}

///<summary>
/// Gets the level number for the sprite which can be used to determine sprite behavior
///</summary>											
int SpriteBaddie::getLevel()
{
	return mySpriteInfo.SpriteLevel;
}

///<summary>
/// Sets the type of power up contained in a dying sprite
///</summary>											
void SpriteBaddie::setPowerUpType(SPRITE_POWER_UP powerUpType)
{
	mySpriteInfo.PowerUpType = powerUpType;
}

///<summary>
/// Gets the type of power up contained in a dying sprite
///</summary>											
SPRITE_POWER_UP SpriteBaddie::getPowerUpType()
{
	return mySpriteInfo.PowerUpType;
}

///<summary>
/// Adds a waypoint to our list of waypoints to access
///</summary>			
void SpriteBaddie::addWaypoint(Vector waypoint)
{
	myNodeWaypoints.push_back(waypoint);
}

///<summary>
/// Gets the current waypoint vector that the INodeWaypoints implementation is targeting
///</summary>			
Vector SpriteBaddie::getCurrentWaypoint()
{
	return myCurrentWaypoint;
}

///<summary>
/// clears the history of waypoint data ...
///</summary>	
void SpriteBaddie::clearWaypoints()
{
	Vector noWaypont = NO_WAYPOINT_SPECIFIED;
	myCurrentWaypoint = noWaypont;
	myNodeWaypoints.clear();	
}

///<summary>
/// Sets an ID value used to reference the sprite
///</summary>	
void SpriteBaddie::setID(int ID)
{
	mySpriteInfo.SpriteID = ID;
}

///<summary>
/// Gets an ID value used to reference the sprite
///</summary>	
int SpriteBaddie::getID()
{
	return mySpriteInfo.SpriteID;
}

///<summary>
/// Returns the Z point that specifies the target z for normal behavior
/// (e.g. after a jump, we want to land at the water level)
///</summary>											
float SpriteBaddie::getWaterLevel()
{
	return mySpriteInfo.WaterLevel;
}

///<summary>
/// Sets the z of a water level as necessary...
///</summary>
void SpriteBaddie::setWaterLevel(float waterLevel)
{
	mySpriteInfo.WaterLevel = waterLevel;
}

///<summary>
/// Indicates to the sprite object if it is ready for a jump :)
///</summary>											
void SpriteBaddie::setJump(JUMP_STATE jumpState)
{
	mySpriteInfo.JumpState = jumpState;
}

///<summary>
/// Returns the JUMP_STATE of the sprite object
///</summary>											
JUMP_STATE SpriteBaddie::getJump()
{
	return mySpriteInfo.JumpState;
}

///<summary>
/// Returns the length of the speed vector to indicate the speed of the sprite
///</summary>											
float SpriteBaddie::getSpeed()
{
	return (mySpriteInfo.VectorLength / mySpriteInfo.UnmodifiedMaxVelocity);
}

///<summary>
/// Sets what types of attack the sprite is capable of
///</summary>											
void SpriteBaddie::setAttackType(ATTACK_TYPE type)
{
	mySpriteInfo.AttackType = type;
}

///<summary>
/// Gets what types of attack the sprite is capable of
///</summary>											
ATTACK_TYPE SpriteBaddie::getAttackType()
{
	return mySpriteInfo.AttackType;
}

///<summary>
/// Updates the rotation value a sprite uses (may not be implemented for all objects)
///</summary>											
void SpriteBaddie::modifyRotationBy(float value)
{
}

///<summary>
/// Assigns a projectile weapon to the sprite that can be triggered by the sprite...
///</summary>			
void SpriteBaddie::assignWeapon(IProjectileWeapon *weapon)
{
	mySpriteInfo.spriteWeapon = weapon;
}

///<summary>
/// Causes the sprite to change it's current angle to face the specified target...
///</summary>			
void SpriteBaddie::lockOnTarget(ISprite &target)
{
}

///<summary>
/// Returns true when a periodic timer or condition has expired. Condition will be reset once true is returned once.
///</summary>
bool SpriteBaddie::hasPeriodicTimerExpired()
{
	return spriteMethodsHasTimerExpired(&mySpriteInfo, DEFAULT_PERIODIC_TIMER);
}

///<summary>
/// Returns the current velocity of the sprite (e.g. The X,Y,Z Momentum of the object)
///</summary>
Vector SpriteBaddie::getVelocity()
{
	return mySpriteInfo.Velocity;
}

