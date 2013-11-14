// SpriteMethods.h
// Defines implementation for a common interface for dealing with multiple sprite behaviors...
// Author(s): Eagan Rackley
#ifndef SPRITE_METHODS_H
#define SPRITE_METHODS_H

#include "IModel.h"
#include "ISprite.h"
#include "SpriteInfo.h"

///<summary>
/// Initializes some of our default values ...
///</summary>	
void spirteMethodsInitSpriteInfo(SpriteInfo *spriteInfo);

///<summary>
/// Returns true if the timer has expired, else false
///</summary>
bool spriteMethodsHasTimerExpired(SpriteInfo *spriteInfo, float maxValue);

///<summary>
/// Assigns an IModel instances to a sprite
///</summary>	
void spriteMethodsAssignModel(SpriteInfo *spriteInfo, IModel *model);

///<summary>
/// Checks the direction of a sprite and returns true/false if the sprite is facing that direction...
///</summary>			
bool spriteMethodsCheckDirection(SpriteInfo *spriteInfo, DIRECTION direction);
		
///<summary>
/// Modifies velocity based upon direction to handle a wall style collision..
///</summary>
void spriteMethodsHandleCollision(SpriteInfo *spriteInfo, COLLISION_TYPE type, float collisionForce);

///<summary>
/// Modifies velocity as if the player is sliding in a particular direction
///</summary>
void spriteMethodsHandleSlideCollision(SpriteInfo *spriteInfo, COLLISION_TYPE type, float collisionForce);

///<summary>
/// Control magnitude can affect the maximum speed of the sprite. This can be used with analog joysticks
/// or maybe adjusted if a sprite is damaged in some way (e.g. can only move at .75 of it's maximum velocity)
/// another example might be traveling through a stick substance can reduce it's control magnitude
///</summary>													
void spriteMethodsSetControlMagnitude(SpriteInfo *spriteInfo, float controlMagnitude);

///<summary>
/// Increases the speed vector length by the acceleration velocity specified
///</summary>
void spriteMethodsIncreaseAcceleration(SpriteInfo *spriteInfo);

///<summary>
/// Sets the direction of a sprite ...
///</summary>					
void spriteMethodsSetDirection(SpriteInfo *spriteInfo, DIRECTION direction);

///<summary>
/// Unsets the direction of a sprite
///</summary>
void spriteMethodsUnsetDirection(SpriteInfo *spriteInfo, DIRECTION direction);

///<summary>
/// Translates the location of a sprite ...
///</summary>							
void spriteMethodsTranslateSprite(SpriteInfo *spriteInfo, Vector spriteLocation);

///<summary>
/// Returns three sets of x/y/z coordinates indicating a bounding triangle for a sprite implementation...
///</summary>													
BoundingTriangle spriteMethodsGetBoundingTriangle(SpriteInfo *spriteInfo);

///<summary>
/// Returns a bounding circle of points (specified by numberOfPoints) in a circle based on the current angle of
/// the sprite info struct specified...
///</summary>
BoundingCircle spriteMethodsGetBoundingCircle(SpriteInfo *spriteInfo, int numberOfPoints);

///<summary>
/// Sets the speeds at which the sprite travels based on direction...
/// maxNormalVelocity is the maximum normal speed the sprite can go
/// maxBurstVelocity is the maximum burst speed the sprite can go (burst is a special speed increase)
/// dragVelocity is how much speed is reduced when the sprite isn't moving in a direction (until speed is 0)		
///</summary>									
void spriteMethodsSetVelocities(SpriteInfo *spriteInfo, float maxNormalVelocity, float dragVelocity, float accelerationVelocity);


///<summary>
/// Sets the angle at which our velocity will modify sprite movement...
///</summary>													
void spriteMethodsSetAngle(SpriteInfo *spriteInfo, float angle);

///<summary>
/// Processes animation frames (usually before rendering)
///</summary>											
void spriteMethodsProcessAnimation(SpriteInfo *spriteInfo);

///<summary>
/// Processes animation frames (usually before rendering) for bullet models...
///</summary>											
void spriteMethodsProcessBulletAnimation(SpriteInfo *spriteInfo);

///<summary>
/// Renders the sprite using an IGraphics implementation...
///</summary>											
void spriteMethodsRenderSprite(SpriteInfo *spriteInfo, IGraphics *graphics, Vector renderOffset, bool interpolate);

///<summary>
/// Renders the health bar for the specified spriteinfo object
///</summary>											
void spriteMethodsRenderHealthBar(SpriteInfo *spriteInfo, IGraphics *graphics, Vector renderOffset);

///<summary>
/// Renders the experience bar used to track to current experience level of our towers...
///</summary>
void spriteMethodsRenderExperienceBar(SpriteInfo *spriteInfo, IGraphics *graphics, Vector renderOffset, float percentage);
	
///<summary>
/// Renders the sprite using an IGraphics implementation...
///</summary>											
void spriteMethodsRenderBulletSprite(SpriteInfo *spriteInfo, IGraphics *graphics, Vector renderOffset);

///<summary>
/// Handles Velocity
///</summary>											
void spriteMethodsProcessVelocity(SpriteInfo *spriteInfo);

///<summary>
/// Update the current location of the sprite by velocity and returns any affect that may have on the map...
///</summary>							
Vector spriteMethodsUpdateLocationByVelocity(SpriteInfo *spriteInfo);

///<summary>
/// Handles a collision event in the case of a fish ramming us from the front...
///</summary>			
void spriteMethodsHandleRearCollision(SpriteInfo *spriteInfo, float multiplyReactionSpeedBy, float multiplyVectorBy);

///<summary>
/// Updates the current location of the sprite, and accounts for any jumps needed...
///</summary>							
void spriteMethodsSetLocation(Vector &location, SpriteInfo *spriteInfo);

///<summary>
/// Handles a jump animation for a sprite as required...
///</summary>
void spriteMethodsHandleJump(SpriteInfo *spriteInfo, float jumpForce);

///<summary>
/// Locks onto the closest target by changing the angle of the sprite...
/// returns true if target is in range, otherwise false...
///</summary>							
bool spriteMethodsLockOnTarget(ISprite &target, SpriteInfo *spriteInfo);

		
#endif
