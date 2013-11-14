// SpriteMethods.cpp
// Provides delegate implementation details for SpriteMethods header -> Incorporated by all ISprite implementations
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "ISprite.h"
#include "SpriteInfo.h"
#include "SpriteMethods.h"

#define ROTATION_SPEED 360.0f

// Minumum and Maximum X/Y Values for moving the player sprite around the screen...
#define MAXIMUM_NORTH -2.5f
#define MAXIMUM_EAST 2.5f
#define MAXIMUM_SOUTH 2.5f
#define MAXIMUM_WEST -2.5f

#define MAXIMUM_TARGET_DISTANCE 64.0f

// This will be used to display health on any sprite that needs us to do it...
static TextureHandle gHealthTexture = UNDEFINED_TEXTURE_HANDLE;

// This will be used to display experience on any sprite that needs us to do it... (e.g. towers)
static TextureHandle gExperienceTexture = UNDEFINED_TEXTURE_HANDLE;


///<summary>
/// Initializes some of our default values ...
///</summary>	
void spirteMethodsInitSpriteInfo(SpriteInfo *spriteInfo)
{
	// Directions Details
	spriteInfo->Direction = DIRECTION_SOUTH;
	spriteInfo->LastDirection = DIRECTION_SOUTH;	

	// Attack options
	spriteInfo->AttackType = ATTACK_TYPE_NONE;

	// Location / Speed / Control Details
	Vector zeroVector = {0.0f, 0.0f, 0.0f};
	spriteInfo->Location = zeroVector;
	spriteInfo->CurrentTargetDistance = MAXIMUM_TARGET_DISTANCE;
	spriteInfo->Velocity = zeroVector;
	spriteInfo->MaxVelocity = 10.0f;
	spriteInfo->UnmodifiedMaxVelocity = 10.0f;
	spriteInfo->DragVelocity = 0.1f;	
	spriteInfo->AccelerationVelocity = 0.2f;
	spriteInfo->ControlMagnitude = 1.0f;
	spriteInfo->ControlAngle = 0.0f;
	
	// Rotation Details		
	Rotate baseRotation = {{0.0f, 1.0f, 0.0f}, -180.0f};
	Rotate currentRotation = {{0.0f, 0.0f, 1.0f}, -180.0f};
	spriteInfo->BaseRotation = baseRotation;	
	spriteInfo->CurrentRotation = currentRotation;
	spriteInfo->AngleDegrees = 0;
	spriteInfo->AngleRads = 0;
	spriteInfo->TargetDegrees = 0.0f;
		
	// Health Details
	spriteInfo->Dead = false;
	spriteInfo->Health = 1.0f;
	spriteInfo->MaxHealth = 1.0f;
	
	// Size / Collision Details
	spriteInfo->Width = 1.0f;
	spriteInfo->Height = 1.0f;
	spriteInfo->BoundingTriangleSize = 1.0f;
		
	// Behavior Details
	spriteInfo->PowerUpType = POWER_UP_NONE;
	spriteInfo->SpriteLevel = 0;	
	spriteInfo->WaterLevel = 0.0f;
	spriteInfo->JumpState = JUMP_STATE_NOT_READY;
	spriteInfo->spriteWeapon = NULL;
	
	// Animation Details
	spriteInfo->CurrentAnimation = STAND;
	spriteInfo->LastAnimation = STAND;
	spriteInfo->animationState.lastTime = 0.0f;
	spriteInfo->animationState.currentTime = 1.0f;
	spriteInfo->animationState.currFrame = 0;
	spriteInfo->animationState.endFrame = 1;
	spriteInfo->animationState.nextFrame = 1;
	spriteInfo->animationState.frameIncrement = 0.0f;
	spriteInfo->animationState.frameLoopCount = 0;
	spriteInfo->animationState.frameSkip = 0;

	Rotate spriteRotation = {{0.0f, 0.0f, 1.0f}, 0.0f};
	spriteInfo->spriteRotationValue = spriteRotation;

	// Set up the periodic timeout counter used for external animation activites (e.g. ripple effects in water)
	spriteInfo->periodicCounter = 0;
}

///<summary>
/// Returns true if the timer has expired, else false
///</summary>
bool spriteMethodsHasTimerExpired(SpriteInfo *spriteInfo, float maxValue)
{
	spriteInfo->periodicCounter += (gTimeScale * (spriteInfo->VectorLength/8)) + gTimeScale;
	if(spriteInfo->periodicCounter >= maxValue)
	{
		spriteInfo->periodicCounter = 0.0f;
		return true;
	}
	else
	{
		return false;
	}
}

///<summary>f
/// Assigns an IModel instances to a sprite
///</summary>	
void spriteMethodsAssignModel(SpriteInfo *spriteInfo, IModel *model)
{	
	ASSERT(model != NULL);
	spriteInfo->sprite3dModel = model;
	spriteInfo->sprite3dModel->setAnimationType(spriteInfo->CurrentAnimation, spriteInfo->animationState);				
}

///<summary>
/// Checks the direction of a sprite and returns true/false if the sprite is facing that direction...
///</summary>			
bool spriteMethodsCheckDirection(SpriteInfo *spriteInfo, DIRECTION direction)
{
	if((spriteInfo->Direction & direction) == direction)
	{
		return true;
	}
	else
	{
		return false;
	}
}


///<summary>
/// Modifies velocity based upon direction to handle a wall style collision..
///</summary>
void spriteMethodsHandleCollision(SpriteInfo *spriteInfo, COLLISION_TYPE type, float collisionForce)
{
	float angleModification = 0.0f;
	float forceOfCollision = (spriteInfo->VectorLength * 0.7f);

	switch(type)
	{
	case FRONT_COLLISION:
		angleModification = 180.0f;		
		break;
	case FRONT_ATTACK:
		angleModification = 180.0f;
		forceOfCollision = collisionForce;
		break;
	case FRONT_LEFT_COLLISION:
		angleModification = 225.0f;
		break;
	case FRONT_RIGHT_COLLISION:
		angleModification = 135.0f;
		break;
	case LEFT_SIDE_COLLISION:
		angleModification = 270.0f;
		break;
	case RIGHT_SIDE_COLLISION:
		angleModification = 90.0f;
		break;
	case LEFT_SIDE_ATTACK:
		angleModification = 270.0f;
		forceOfCollision = collisionForce;
		break;
	case RIGHT_SIDE_ATTACK:
		angleModification = 90.0f;
		forceOfCollision = collisionForce;
		break;
	case REAR_COLLISION:		
		if(spriteInfo->Type == BULLET_SPRITE)
		{
			return;
		}
		else
		{
			forceOfCollision = (spriteInfo->VectorLength * 0.7f);
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	// Whatever our current angle is, we modify it by the type of collision that occurred...	
	float modifiedAngle = modifyAngleByDegrees(spriteInfo->AngleDegrees, angleModification);
	float angleInRads = getRadiansForAngle(modifiedAngle);			
	spriteInfo->Velocity.x = (forceOfCollision * cos(angleInRads));
	spriteInfo->Velocity.y = (forceOfCollision * sin(angleInRads));	
	// If this is a bullet sprite, then we'll modify the control angle, too, so that it goes the other way
	if(spriteInfo->Type == BULLET_SPRITE)
	{
		spriteInfo->AngleDegrees = modifiedAngle;
		spriteInfo->ControlAngle = modifiedAngle - 90.0f;
		spriteInfo->TargetDegrees = modifiedAngle;
		if(spriteInfo->ControlAngle < 0.0f) spriteInfo->ControlAngle += 360.0f;
		spriteInfo->VectorLength *= 2.0f;
		if(spriteInfo->VectorLength > spriteInfo->MaxVelocity) spriteInfo->VectorLength = spriteInfo->MaxVelocity;
	}
	spriteMethodsUpdateLocationByVelocity(spriteInfo);	
}

///<summary>
/// Modifies velocity as if the player is sliding in a particular direction
///</summary>
void spriteMethodsHandleSlideCollision(SpriteInfo *spriteInfo, COLLISION_TYPE type, float collisionForce)
{
	float angleModification = 0.0f;

	//ASSERT(spriteInfo->Type != BULLET_SPRITE);

	// Determine the angle for this type of slide collision..
	switch (type)
	{
	case FRONT_LEFT_SLIDE:
		angleModification = 90.0f;
		break;
	case FRONT_RIGHT_SLIDE:
		angleModification = 270.0f;
		break;
	case REAR_SLIDE:
		angleModification = 180.0f;
		break;
	case FRONT_SLIDE:
		angleModification = 0.0f;
		break;
	default:
		ASSERT(false);
		break;
	}

	// Increase our vector length...
	if(spriteInfo->VectorLength < (spriteInfo->MaxVelocity)) spriteInfo->VectorLength += spriteInfo->AccelerationVelocity;
	else spriteInfo->VectorLength = spriteInfo->MaxVelocity;

	// Whatever our current angle is, we modify it by the type of collision that occurred and apply the increased vector to the new direction
	float modifiedAngle = modifyAngleByDegrees(spriteInfo->AngleDegrees, angleModification);
	float angleInRads = getRadiansForAngle(modifiedAngle);

	//spriteInfo->Velocity.x = (spriteInfo->VectorLength * cos(angleInRads));

	// Apply these values to our velocity
	spriteInfo->Velocity.x = (spriteInfo->VectorLength * cos(angleInRads));
	spriteInfo->Velocity.y = (spriteInfo->VectorLength * sin(angleInRads));

	// And update our position accordingly...
	spriteMethodsUpdateLocationByVelocity(spriteInfo);
}

///<summary>
/// Control magnitude can affect the maximum speed of the sprite. This can be used with analog joysticks
/// or maybe adjusted if a sprite is damaged in some way (e.g. can only move at .75 of it's maximum velocity)
/// another example might be traveling through a stick substance can reduce it's control magnitude
///</summary>													
void spriteMethodsSetControlMagnitude(SpriteInfo *spriteInfo, float controlMagnitude)
{
	spriteInfo->ControlMagnitude = controlMagnitude;
	spriteInfo->MaxVelocity = (spriteInfo->UnmodifiedMaxVelocity * spriteInfo->ControlMagnitude);
}

///<summary>
/// Increases the speed vector length by the acceleration velocity specified
///</summary>
void spriteMethodsIncreaseAcceleration(SpriteInfo *spriteInfo)
{
	if(spriteInfo->VectorLength < spriteInfo->MaxVelocity) spriteInfo->VectorLength += spriteInfo->AccelerationVelocity;
}

///<summary>
/// Sets the direction of a sprite ...
///</summary>					
void spriteMethodsSetDirection(SpriteInfo *spriteInfo, DIRECTION direction)
{
	spriteInfo->Direction = spriteInfo->Direction | direction;	
	
	// Set our rotation value based on the new direction...
	switch(direction)
	{		
		case DIRECTION_EAST:
			spriteMethodsUnsetDirection(spriteInfo, DIRECTION_WEST);						
		break;
		case DIRECTION_WEST:
			spriteMethodsUnsetDirection(spriteInfo, DIRECTION_EAST);			
		break;
		case DIRECTION_NORTH:
			spriteMethodsUnsetDirection(spriteInfo, DIRECTION_SOUTH);
		break;
		case DIRECTION_SOUTH:
			spriteMethodsUnsetDirection(spriteInfo, DIRECTION_NORTH);			
		break;
		default:
		break;
	}
	
	spriteInfo->LastDirection = spriteInfo->Direction;
}

///<summary>
/// Unsets the direction of a sprite
///</summary>
void spriteMethodsUnsetDirection(SpriteInfo *spriteInfo, DIRECTION direction)
{
	spriteInfo->Direction &= !(direction);			
}

///<summary>
/// Translates the location of a sprite ...
///</summary>							
void spriteMethodsTranslateSprite(SpriteInfo *spriteInfo, Vector spriteLocation)
{
	spriteInfo->Location = spriteLocation;
	Vector zeroVector = {0.0f, 0.0f, 0.0f};	
	spriteInfo->Velocity = zeroVector;
	spriteInfo->VectorLength = 0.0f;	
	spriteInfo->WaterLevel = spriteLocation.z;
}

///<summary>
/// Returns three sets of x/y/z coordinates indicating a bounding triangle for a sprite implementation...
///</summary>													
BoundingTriangle spriteMethodsGetBoundingTriangle(SpriteInfo *spriteInfo)
{
	BoundingTriangle returnValue;
	
	float collisionAngleA = modifyAngleByDegrees(spriteInfo->AngleDegrees, 200);
	float collisionAngleB = modifyAngleByDegrees(spriteInfo->AngleDegrees, 160);
	float collisionAngleC = modifyAngleByDegrees(spriteInfo->AngleDegrees, 0);
	
	// CollisionAngles A to C define a trianged based on our current angle modified by a certain number of degrees.
	// We will then use the value in spriteInfo->BoundingTriangleVector to convert these angels into cartesian coordinates
	// that we can then use for collision detection.
	Vector zeroVector = {0.0f, 0.0f, 0.0f};
	returnValue.collisionA = modifyVectorByAngle(zeroVector, spriteInfo->BoundingTriangleSize * 1.5f, collisionAngleA);
	returnValue.collisionB = modifyVectorByAngle(zeroVector, spriteInfo->BoundingTriangleSize * 1.5f, collisionAngleB);
	returnValue.collisionC = modifyVectorByAngle(zeroVector, (spriteInfo->BoundingTriangleSize * 1.5f), collisionAngleC);
			
	return returnValue;
}

///<summary>
/// Returns a bounding circle of points (specified by numberOfPoints) in a circle based on the current angle of
/// the sprite info struct specified...
///</summary>
BoundingCircle spriteMethodsGetBoundingCircle(SpriteInfo *spriteInfo, int numberOfPoints)
{
	BoundingCircle returnValue;

	float boundingAngle = spriteInfo->AngleDegrees;
	float angleIncrement = (360.0f / numberOfPoints);

	for(int idx = 0; idx < numberOfPoints; idx++)
	{
		BoundingVertex newBoundingVert;

		newBoundingVert.angle = modifyAngleByDegrees(spriteInfo->AngleDegrees, boundingAngle);

		// Front and back of circle should be larger than sides (since all our sprites are generally fish) (yes I know it's not really a circle now)
		Vector zeroVector = {0.0f, 0.0f, 0.0f};
		newBoundingVert.vertex = modifyVectorByAngle(zeroVector, spriteInfo->BoundingTriangleSize, newBoundingVert.angle);

		returnValue.verticies.push_back(newBoundingVert);

		// Increment angle by increment degrees (8 points would be 45.0f between each) and adjust for rollover...
		boundingAngle += angleIncrement;
		if(boundingAngle > 360.0f)
		{
			boundingAngle -= 360;
		}
	}


	return returnValue;
}

///<summary>
/// Sets the speeds at which the sprite travels based on direction...
/// maxNormalVelocity is the maximum normal speed the sprite can go
/// maxBurstVelocity is the maximum burst speed the sprite can go (burst is a special speed increase)
/// dragVelocity is how much speed is reduced when the sprite isn't moving in a direction (until speed is 0)		
///</summary>									
void spriteMethodsSetVelocities(SpriteInfo *spriteInfo, float maxNormalVelocity, float dragVelocity, float accelerationVelocity)
{
	spriteInfo->UnmodifiedMaxVelocity = maxNormalVelocity;
	spriteInfo->MaxVelocity = (spriteInfo->UnmodifiedMaxVelocity * spriteInfo->ControlMagnitude);	
	spriteInfo->DragVelocity = dragVelocity;
	spriteInfo->AccelerationVelocity = accelerationVelocity;
}

///<summary>
/// Sets the angle at which our velocity will modify sprite movement...
///</summary>													
void spriteMethodsSetAngle(SpriteInfo *spriteInfo, float angle)
{
	if(angle != NAN)
	{
		spriteInfo->ControlAngle = angle;
		spriteInfo->TargetDegrees = angle + 90.0f;
		if(spriteInfo->TargetDegrees > 360.0f) spriteInfo->TargetDegrees -= 360.0f;
	}
}

///<summary>
/// A static local method use the render the health bar of the sprite...
///</summary>									
void renderSpriteHealth(SpriteInfo *spriteInfo, Vector renderLocation, IGraphics *graphics, TextureHandle textureHandle, float percentage)
{
	ASSERT(gHealthTexture != UNDEFINED_TEXTURE_HANDLE);	
	Quad quad, quadBorder;
	

	float healthBarSize = (percentage);

	// Set up the verticies for quad so that we're drawing a health bar on top of the sprite
	// and in the center...
	quad.verticies[0].x = -0.75f;
	quad.verticies[0].y =  0.0f;
	quad.verticies[0].z = -2.0f;
	
	quad.verticies[1].x = -0.75f + healthBarSize;
	quad.verticies[1].y =  0.0f;
	quad.verticies[1].z = -2.0f;
	
	quad.verticies[2].x = -0.75f + healthBarSize;
	quad.verticies[2].y =  0.0f;
	quad.verticies[2].z = -1.6f;
	
	quad.verticies[3].x = -0.75f;
	quad.verticies[3].y =  0.0f;
	quad.verticies[3].z = -1.6f;
	
	// TopLeft, TopRight, BottomRight, BottomLeft tex coords for the health meter
	quad.texture_S[0] = 0.0f;
	quad.texture_T[0] = 0.0f;

	quad.texture_S[1] = 0.5f;
	quad.texture_T[1] = 0.0f;
	
	quad.texture_S[2] = 0.5f;
	quad.texture_T[2] = 1.0f;

	quad.texture_S[3] = 0.0f;
	quad.texture_T[3] = 1.0f;
	
	// Set up the border around the quad drawing our health display
	quadBorder = quad;
	quadBorder.verticies[0].y -= 0.001f;
	quadBorder.verticies[1].y -= 0.001f;
	quadBorder.verticies[2].y -= 0.001f;
	quadBorder.verticies[1].x = 0.75f;
	quadBorder.verticies[2].x = 0.75f;

	// TopLeft, TopRight, BottomRight, BottomLeft tex coords for the health meter border
	quadBorder.texture_S[0] = 0.5f;
	quadBorder.texture_T[0] = 0.0f;

	quadBorder.texture_S[1] = 1.0f;
	quadBorder.texture_T[1] = 0.0f;
	
	quadBorder.texture_S[2] = 1.0f;
	quadBorder.texture_T[2] = 1.0f;

	quadBorder.texture_S[3] = 0.5f;
	quadBorder.texture_T[3] = 1.0f;
		
	std::map<int, Quad> quads;
	quads[0] = quad;	
	quads[1] = quadBorder;	
	
	Rotate rotation = {{0.0f, 0.0f, 1.0f}, graphics->getCameraAngle() };	
	graphics->rotateNextObjectInScene(&rotation);		

	graphics->addQuadsToScene(renderLocation, quads, quads.size(), textureHandle);
	quads.clear();	
}

///<summary>
/// Processes animation frames (usually before rendering)
///</summary>											
void spriteMethodsProcessAnimation(SpriteInfo *spriteInfo)
{
	// Change our animation if needed
	if(spriteInfo->LastAnimation != spriteInfo->CurrentAnimation)
	{
		spriteInfo->LastAnimation = spriteInfo->CurrentAnimation;
		spriteInfo->sprite3dModel->setAnimationType(spriteInfo->CurrentAnimation, spriteInfo->animationState);				
	}

	if(spriteInfo->animationState.currFrame == spriteInfo->animationState.endFrame)
	{
		if(((spriteInfo->CurrentAnimation == JUMP) || (spriteInfo->CurrentAnimation == CROUCH_STAND) 
			|| (spriteInfo->CurrentAnimation == FLIP) || (spriteInfo->CurrentAnimation == CROUCH_ATTACK) 
			|| (spriteInfo->CurrentAnimation == ATTACK_A) || (spriteInfo->CurrentAnimation == REVERSE_ATTACK)
			|| (spriteInfo->CurrentAnimation == PAIN_A)	|| (spriteInfo->CurrentAnimation == PAIN_B)
			|| (spriteInfo->CurrentAnimation == PAIN_C)	|| (spriteInfo->CurrentAnimation == JUMP) || (spriteInfo->CurrentAnimation == WAVE)
			|| (spriteInfo->CurrentAnimation == SALUTE)))
		{
			if(spriteInfo->Health <= 0.0f)
			{
				int deathAnim = (rand() % 3);
				if(deathAnim == 0) spriteInfo->CurrentAnimation = DEATH_FALLBACK;
				else if(deathAnim == 1) spriteInfo->CurrentAnimation = DEATH_FALLFORWARD;
				else if(deathAnim == 2) spriteInfo->CurrentAnimation = DEATH_FALLBACKSLOW;
			}
			else
			{
				spriteInfo->CurrentAnimation = STAND;
			}
		}
	}
	else if(spriteInfo->animationState.nextFrame == spriteInfo->animationState.endFrame)
	{
		if((spriteInfo->CurrentAnimation == DEATH_FALLBACKSLOW) 
			|| (spriteInfo->CurrentAnimation == DEATH_FALLFORWARD) 
			|| (spriteInfo->CurrentAnimation == DEATH_FALLBACK)
			|| (spriteInfo->CurrentAnimation == CROUCH_DEATH))
		{
			spriteInfo->Dead = true;		
		}
	}

	// Handle animation here ..
	spriteInfo->sprite3dModel->handleAnimation(gTimeScale, spriteInfo->animationState);
}

///<summary>
/// Processes animation frames (usually before rendering) for bullet models...
///</summary>											
void spriteMethodsProcessBulletAnimation(SpriteInfo *spriteInfo)
{
	// Change our animation if needed
	if(spriteInfo->LastAnimation != spriteInfo->CurrentAnimation)
	{
		spriteInfo->LastAnimation = spriteInfo->CurrentAnimation;
		spriteInfo->sprite3dModel->setAnimationType(spriteInfo->CurrentAnimation, spriteInfo->animationState);				
	}

	// Handle the bullet startup animation...

	// Handle the bullet ending animation
	if(spriteInfo->SpriteLevel == 4)
	{
		// Handle how the bullet dies, and when it is dead make sure that we set our sprite to the first frame...
		if(spriteInfo->animationState.currFrame == spriteInfo->animationState.endFrame)
			{
				if((spriteInfo->CurrentAnimation == DEATH_FALLBACKSLOW) 
					|| (spriteInfo->CurrentAnimation == DEATH_FALLFORWARD) 
					|| (spriteInfo->CurrentAnimation == DEATH_FALLBACK)
					|| (spriteInfo->CurrentAnimation == CROUCH_DEATH))
				{
					spriteInfo->Health = 0.0f;
					spriteInfo->Dead = true;
					spriteInfo->CurrentAnimation = PAIN_A;									
					spriteInfo->sprite3dModel->setAnimationType(PAIN_A, spriteInfo->animationState);					
					spriteInfo->animationState.currFrame = spriteInfo->animationState.startFrame;
				}
			}		
	}
	else if(spriteInfo->Health <= 0.0f)
	{		
		spriteInfo->Dead = true;		
	}

	// Handle animation here ..
	spriteInfo->sprite3dModel->handleAnimation(gTimeScale, spriteInfo->animationState);
}

///<summary>
/// Renders the sprite using an IGraphics implementation...
///</summary>											
void spriteMethodsRenderSprite(SpriteInfo *spriteInfo, IGraphics *graphics, Vector renderOffset, bool interpolate)
{
	if(spriteInfo->Dead == false)
	{						
		// If we're not dying then handle turns
		if((spriteInfo->CurrentAnimation != DEATH_FALLBACKSLOW)
				&& (spriteInfo->CurrentAnimation != DEATH_FALLFORWARD)
				&& (spriteInfo->CurrentAnimation != DEATH_FALLBACK))
		{
			// Adjust our rotation so that currentAngle moves appropriately when targetAngle moves...
			adjustAngleRotation(spriteInfo->AngleDegrees, spriteInfo->TargetDegrees, (ROTATION_SPEED * spriteInfo->ControlMagnitude));	

			// Assign this to our current rotation value...
			spriteInfo->CurrentRotation.value = spriteInfo->AngleDegrees;
		}
		
		// Reset our target distance value on render so that we can accept a new target if a close one dies ...
		spriteInfo->CurrentTargetDistance = MAXIMUM_TARGET_DISTANCE;

		// Translate and render our sprites model
		Vector renderLocation = spriteInfo->Location;
		renderLocation.x += renderOffset.x;
		renderLocation.y += renderOffset.y;
		renderLocation.z += renderOffset.z;

		SEEN_BY_CAMERA cameraSight = graphics->isLocationWithinViewingRange(renderLocation);
		if(cameraSight == SEEN_BY_CAMERA_NEAR)
		{
			// and render our model
			spriteInfo->sprite3dModel->translateModel(renderLocation);
			graphics->rotateNextObjectInScene(&spriteInfo->BaseRotation);
			graphics->rotateNextObjectInScene(&spriteInfo->CurrentRotation);
			spriteInfo->sprite3dModel->renderModel(graphics, spriteInfo->animationState, interpolate);
		}
	}
}

///<summary>
/// Renders the health bar for the specified spriteinfo object
///</summary>											
void spriteMethodsRenderHealthBar(SpriteInfo *spriteInfo, IGraphics *graphics, Vector renderOffset)
{
	if(spriteInfo->Dead == false)
	{		
		// If our texture handle is still undefined we should load our health meter texture here ...
		if(gHealthTexture == UNDEFINED_TEXTURE_HANDLE)
		{
			//TODO: Load our texture handle up in this joint!!!
			gHealthTexture = graphics->addTextureData("//assets//texOther//baddieHealth.png", IMAGE_TEXTURE, false);
		}
		// Translate and render our fish model
		Vector renderLocation = spriteInfo->Location;
		renderLocation.x += renderOffset.x;
		renderLocation.y += renderOffset.y;
		renderLocation.z += renderOffset.z;
		if(graphics->isLocationWithinViewingRange(renderLocation) >= SEEN_BY_CAMERA_NEAR)
		{
			if((spriteInfo->Health < spriteInfo->MaxHealth) && (spriteInfo->Health > 0.0f))
			{
				float percentage = (spriteInfo->Health / spriteInfo->MaxHealth) * 1.5f;
				renderSpriteHealth(spriteInfo, renderLocation, graphics, gHealthTexture, percentage);
			}

		}
	}
}

///<summary>
/// Renders the experience bar used to track to current experience level of our towers...
///</summary>
void spriteMethodsRenderExperienceBar(SpriteInfo *spriteInfo, IGraphics *graphics, Vector renderOffset, float percentage)
{
	if(spriteInfo->Dead == false)
	{
		// If our texture handle is still undefined we should load our health meter texture here ...
		if(gExperienceTexture == UNDEFINED_TEXTURE_HANDLE)
		{
			gExperienceTexture = graphics->addTextureData("//assets//texOther//gunExperience.png", IMAGE_TEXTURE, false);
		}
		// Translate and render our fish model
		Vector renderLocation = spriteInfo->Location;
		renderLocation.x += renderOffset.x;
		renderLocation.y += renderOffset.y;
		renderLocation.z += renderOffset.z;
		if(graphics->isLocationWithinViewingRange(renderLocation) >= SEEN_BY_CAMERA_NEAR)
		{
			renderSpriteHealth(spriteInfo, renderLocation, graphics, gExperienceTexture, percentage);
		}
	}
}


///<summary>
/// Renders the sprite using an IGraphics implementation...
///</summary>											
void spriteMethodsRenderBulletSprite(SpriteInfo *spriteInfo, IGraphics *graphics, Vector renderOffset)
{
	if(spriteInfo->Dead == false)
	{				
		// Adjust our rotation so that currentAngle moves appropriately when targetAngle moves...
		adjustAngleRotation(spriteInfo->AngleDegrees, spriteInfo->TargetDegrees, ROTATION_SPEED);	

		// Assign this to our current rotation value...
		spriteInfo->CurrentRotation.value = spriteInfo->AngleDegrees;
		
		// Translate and render our bullet model...
		Vector renderLocation = spriteInfo->Location;
		renderLocation.x += renderOffset.x;
		renderLocation.y += renderOffset.y;
		renderLocation.z += renderOffset.z;
		if(graphics->isLocationWithinViewingRange(renderLocation) >= SEEN_BY_CAMERA_NEAR)
		{
			spriteInfo->sprite3dModel->translateModel(renderLocation);
			//graphics->rotateNextObjectInScene(&spriteInfo->BaseRotation);				
			//graphics->rotateNextObjectInScene(&spriteInfo->CurrentRotation);				
			// and render our model
			if(spriteInfo->SpriteLevel <= 3)
			{
				spriteInfo->sprite3dModel->renderModel(graphics, spriteInfo->animationState, false);	
			}
			else
			{
				spriteInfo->sprite3dModel->renderModel(graphics, spriteInfo->animationState, true);
			}
		}
	}
}

///<summary>
/// Handles Velocity... TOTALLY GROSS - PLEASE EAGAN, REFACTOR ME!
///</summary>											
void spriteMethodsProcessVelocity(SpriteInfo *spriteInfo)
{
	if((spriteInfo->CurrentAnimation != ATTACK_A)
		&& (spriteInfo->CurrentAnimation != REVERSE_ATTACK)
		&& (spriteInfo->CurrentAnimation != PAIN_A) 
		&& (spriteInfo->CurrentAnimation != PAIN_B) 
		&& (spriteInfo->CurrentAnimation != PAIN_C) 
		&& (spriteInfo->CurrentAnimation != CROUCH_ATTACK)
		&& (spriteInfo->CurrentAnimation != DEATH_FALLBACKSLOW)
		&& (spriteInfo->CurrentAnimation != DEATH_FALLBACK)
		&& (spriteInfo->CurrentAnimation != DEATH_FALLFORWARD)
		&& (spriteInfo->CurrentAnimation != FLIP)
		&& (spriteInfo->CurrentAnimation != WAVE)
		&& (spriteInfo->CurrentAnimation != SALUTE)
		&& (spriteInfo->Dead == false))
	{
		// Always subtract our drag velocity so that if we're 
		// not pressing a key, the fish is slowing down...
		if(spriteInfo->VectorLength > 0.0f)
		{
			if (spriteInfo->CurrentAnimation != JUMP)
			{
				if(spriteInfo->VectorLength <= spriteInfo->AccelerationVelocity)
				{
					spriteInfo->CurrentAnimation = STAND;
				}
				else 
				{
					spriteInfo->CurrentAnimation = RUN;	
				}
			}
				spriteInfo->VectorLength -= spriteInfo->DragVelocity;			
		}
		else
		{
			spriteInfo->VectorLength = 0.0f;
			spriteInfo->CurrentAnimation = STAND;		
		}
	}
	else if((spriteInfo->CurrentAnimation == ATTACK_A) || (spriteInfo->CurrentAnimation == REVERSE_ATTACK) ||(spriteInfo->CurrentAnimation == DEATH_FALLBACKSLOW)
		|| (spriteInfo->CurrentAnimation == DEATH_FALLBACK)|| (spriteInfo->CurrentAnimation == DEATH_FALLFORWARD)
		|| (spriteInfo->CurrentAnimation == CROUCH_ATTACK) || (spriteInfo->Dead == true))
	{
		// Quickly slow down during an attack animation
		spriteInfo->VectorLength = (spriteInfo->VectorLength / 1.5f);
	}
	else if (spriteInfo->CurrentAnimation == PAIN_A) 
	{
		// TODO: We should only implement this in the case of a repel attack!
		// An attack will cause an enemy to pause slightly...
		//spriteInfo->VectorLength = (spriteInfo->VectorLength / 1.2);
	}
		
	
	// If the control magnitude (usually of a joystick) is significant, then we should adjust 
	/// our velocity by our vector length. Otherwise we'll process the drag value down below...
	if((spriteInfo->ControlMagnitude >= 0.15f) && (spriteInfo->CurrentAnimation != DEATH_FALLBACKSLOW)
			&& (spriteInfo->CurrentAnimation != DEATH_FALLBACK) && (spriteInfo->CurrentAnimation != DEATH_FALLFORWARD))
	{
		// adjust our angle in radians so that we can change our movement...
		spriteInfo->AngleRads = getRadiansForAngle(spriteInfo->AngleDegrees);
		float targetXVelocity = (spriteInfo->VectorLength * cos(spriteInfo->AngleRads));
		float targetYVelocity = (spriteInfo->VectorLength * sin(spriteInfo->AngleRads));
		
		// If we're going e/w but not n/s modify x by accel, and y by drag
		if( (((spriteInfo->LastDirection & DIRECTION_EAST) == DIRECTION_EAST) 
		 || ((spriteInfo->LastDirection & DIRECTION_WEST) == DIRECTION_WEST))
		 && (((spriteInfo->LastDirection & DIRECTION_NORTH) != DIRECTION_NORTH)
		 && ((spriteInfo->LastDirection & DIRECTION_SOUTH) != DIRECTION_SOUTH)))
		{
			// Change our y velocity based on the target y for our angle
			if(spriteInfo->Velocity.y < targetYVelocity)
			{
				spriteInfo->Velocity.y += spriteInfo->DragVelocity;
			}
			else if(spriteInfo->Velocity.y > targetYVelocity)
			{
				spriteInfo->Velocity.y -= spriteInfo->DragVelocity;
			}
	
			// Change our x velocity based on the target x for our angle
			if(spriteInfo->Velocity.x < targetXVelocity)
			{
				spriteInfo->Velocity.x += spriteInfo->AccelerationVelocity;
			}
			else if(spriteInfo->Velocity.x > targetXVelocity)
			{
				spriteInfo->Velocity.x -= spriteInfo->AccelerationVelocity;
			}
		}
		// If we're going n/S but not e/w modify y by accel, and x by drag		
		else if( (((spriteInfo->LastDirection & DIRECTION_NORTH) == DIRECTION_NORTH) 
		 || ((spriteInfo->LastDirection & DIRECTION_SOUTH) == DIRECTION_SOUTH))
		 && (((spriteInfo->LastDirection & DIRECTION_EAST) != DIRECTION_EAST)
		 && ((spriteInfo->LastDirection & DIRECTION_WEST) != DIRECTION_WEST)))
		{
			// Change our y velocity based on the target y for our angle
			if(spriteInfo->Velocity.y < targetYVelocity)
			{
				spriteInfo->Velocity.y += spriteInfo->AccelerationVelocity;
			}
			else if(spriteInfo->Velocity.y > targetYVelocity)
			{
				spriteInfo->Velocity.y -= spriteInfo->AccelerationVelocity;
			}
	
			// Change our x velocity based on the target x for our angle
			if(spriteInfo->Velocity.x < targetXVelocity)
			{
				spriteInfo->Velocity.x += spriteInfo->DragVelocity;
			}
			else if(spriteInfo->Velocity.x > targetXVelocity)
			{
				spriteInfo->Velocity.x -= spriteInfo->DragVelocity;
			}
		}
		// If we're going in a combination of n/s or e/w then modify both x and y by accel
		else
		{
			// Change our y velocity based on the target y for our angle
			if(spriteInfo->Velocity.y < targetYVelocity)
			{
				spriteInfo->Velocity.y += spriteInfo->AccelerationVelocity;
			}
			else if(spriteInfo->Velocity.y > targetYVelocity)
			{
				spriteInfo->Velocity.y -= spriteInfo->AccelerationVelocity;
			}			
	
			// Change our x velocity based on the target x for our angle
			if(spriteInfo->Velocity.x < targetXVelocity)
			{
				spriteInfo->Velocity.x += spriteInfo->AccelerationVelocity;
			}
			else if(spriteInfo->Velocity.x > targetXVelocity)
			{
				spriteInfo->Velocity.x -= spriteInfo->AccelerationVelocity;
			}			
		}		
	}
	else
	{
		spriteMethodsUnsetDirection(spriteInfo, DIRECTION_NORTH);
		spriteMethodsUnsetDirection(spriteInfo, DIRECTION_SOUTH);
		spriteMethodsUnsetDirection(spriteInfo, DIRECTION_EAST);
		spriteMethodsUnsetDirection(spriteInfo, DIRECTION_WEST);						
		spriteInfo->Velocity.x = 0.0f;
		spriteInfo->Velocity.y = 0.0f;
	}
	
	
	// Deal with our E/W movement
	if((spriteMethodsCheckDirection(spriteInfo, DIRECTION_EAST) == false) && (spriteMethodsCheckDirection(spriteInfo, DIRECTION_WEST) == false))
	{
		if(spriteInfo->Velocity.x > spriteInfo->DragVelocity)
		{
			spriteInfo->Velocity.x -= spriteInfo->DragVelocity;
		}
		else if(spriteInfo->Velocity.x < -spriteInfo->DragVelocity)
		{
			spriteInfo->Velocity.x += spriteInfo->DragVelocity;
		}
		else
		{
			spriteInfo->Velocity.x = 0.0f;
		}
				
	}
	
	// Now deal with our N/S movement
	if((spriteMethodsCheckDirection(spriteInfo, DIRECTION_NORTH) == false) && (spriteMethodsCheckDirection(spriteInfo, DIRECTION_SOUTH) == false))
	{		
		if(spriteInfo->Velocity.y > spriteInfo->DragVelocity)
		{
			spriteInfo->Velocity.y -= spriteInfo->DragVelocity;
		}
		else if(spriteInfo->Velocity.y < -spriteInfo->DragVelocity)
		{
			spriteInfo->Velocity.y += spriteInfo->DragVelocity;
		}
		else
		{
			spriteInfo->Velocity.y = 0.0f;			
		}				
	}	
}

///<summary>
/// Handles Velocity
///</summary>											
void spriteMethodsProcessBulletVelocity(SpriteInfo *spriteInfo)
{
	if(spriteInfo->Dead == false)
	{
		// Always subtract our drag velocity so that if we're 
		// not pressing a key, the fish is slowing down...
		if(spriteInfo->VectorLength > 0.0f)
		{						
			spriteInfo->VectorLength -= spriteInfo->DragVelocity;
		}
		else
		{
			spriteInfo->VectorLength = 0.0f;
			spriteInfo->CurrentAnimation = STAND;		
		}
	}		
	
	// If the control magnitude (usually of a joystick) is significant, then we should adjust 
	/// our velocity by our vector length. Otherwise we'll process the drag value down below...
	if(spriteInfo->ControlMagnitude >= 0.15f)
	{
		// adjust our angle in radians so that we can change our movement...
		spriteInfo->AngleRads = getRadiansForAngle(spriteInfo->AngleDegrees);
		float targetXVelocity = (spriteInfo->VectorLength * cos(spriteInfo->AngleRads));
		float targetYVelocity = (spriteInfo->VectorLength * sin(spriteInfo->AngleRads));
		
		// Change our x velocity based on the target x for our angle
		if(spriteInfo->Velocity.x < targetXVelocity)
		{
			spriteInfo->Velocity.x += spriteInfo->AccelerationVelocity;
		}
		else if(spriteInfo->Velocity.x > targetXVelocity)
		{
			spriteInfo->Velocity.x -= spriteInfo->AccelerationVelocity;
		}

		// Change our y velocity based on the target y for our angle
		if(spriteInfo->Velocity.y < targetYVelocity)
		{
			spriteInfo->Velocity.y += spriteInfo->AccelerationVelocity;
		}
		else if(spriteInfo->Velocity.y > targetYVelocity)
		{
			spriteInfo->Velocity.y -= spriteInfo->AccelerationVelocity;
		}			
	}
	else
	{
		spriteInfo->Velocity.x = 0.0f;
		spriteInfo->Velocity.y = 0.0f;
	}	
}

///<summary>
/// Update the current location of the sprite by velocity and returns any affect that may have on the map...
///</summary>							
Vector spriteMethodsUpdateLocationByVelocity(SpriteInfo *spriteInfo)
{
	Vector returnVelocity = {0.0f, 0.0f, 0.0f};
	float xVelocity = (spriteInfo->Velocity.x * gTimeScale);
	float yVelocity = (spriteInfo->Velocity.y * gTimeScale);

	//text.renderText((Vector){-4.0f,  0.0f, 9.0f}, "Velocity Z: %f", mapVelocity.z);	
	//text.renderText((Vector){-4.0f,  0.0f, 9.0f}, "Velocity Z: %f", mapVelocity.z);	
	
	//ASSERT(gTimeScale <= 1.0f);
	//ASSERT(abs(xVelocity) <= 30.0f);
	//ASSERT(abs(yVelocity) <= 30.0f);
		
	if(spriteInfo->Type == BADDIE_SPRITE)
	{
		spriteMethodsProcessVelocity(spriteInfo);
		spriteInfo->Location.x -= xVelocity;	
		spriteInfo->Location.y -= yVelocity;		
	}
	else if ((spriteInfo->Type == BULLET_SPRITE) || (spriteInfo->Type == POWER_UP_SPRITE))
	{
		spriteMethodsProcessBulletVelocity(spriteInfo);
		spriteInfo->Location.x -= xVelocity;	
		spriteInfo->Location.y -= yVelocity;
	}
	else if (spriteInfo->Type == PLAYER_SPRITE)
	{
		spriteMethodsProcessVelocity(spriteInfo);
		// Figure out if we're within x boundaries. If we are, then move the sprite. Otherwise return our velocity...
		if((spriteInfo->Velocity.x < spriteInfo->DragVelocity) && (spriteInfo->Location.x <= MAXIMUM_EAST))
		{
			spriteInfo->Location.x -= xVelocity;	
		}
		else if((spriteInfo->Velocity.x > spriteInfo->DragVelocity) && (spriteInfo->Location.x >= MAXIMUM_WEST))
		{
			spriteInfo->Location.x -= xVelocity;
		}
		else
		{
			returnVelocity.x = spriteInfo->Velocity.x;
		}

		// Figure out if we're within x boundaries. If we are, then move the sprite. Otherwise return our velocity...
		if((spriteInfo->Velocity.y < spriteInfo->DragVelocity) && (spriteInfo->Location.y <= MAXIMUM_SOUTH))
		{
			spriteInfo->Location.y -= yVelocity;	
		}
		else if((spriteInfo->Velocity.y > spriteInfo->DragVelocity) && (spriteInfo->Location.y >= MAXIMUM_NORTH))
		{
			spriteInfo->Location.y -= yVelocity;
		}
		else
		{
			returnVelocity.y = spriteInfo->Velocity.y;
		}	
	}
	else
	{
		ASSERT(false);
	}

	//printf("\x1b[2;0H"); printf("Jump State: %d", spriteInfo->JumpState);					
	// If the sprite is jump state ready, then make el jumpo happeno
	if(spriteInfo->JumpState == JUMP_STATE_READY)
	{
		spriteInfo->JumpState = JUMP_STATE_JUMPING;
		//spriteInfo->CurrentAnimation = JUMP;						
		spriteInfo->Velocity.z = (fabs(spriteInfo->VectorLength) * -1.0f)/3.0f;		
	}
	else if (spriteInfo->JumpState == JUMP_STATE_JUMPING)
	{
		spriteInfo->Velocity.z += 40.8f * gTimeScale;
		spriteInfo->Location.z += (spriteInfo->Velocity.z * gTimeScale);
		if(spriteInfo->Velocity.z > 0.0f)
		{			
			spriteInfo->JumpState = JUMP_STATE_LANDING;
		}
	}
	// if Jump state is jumping, then handle gravity ...
	else if (spriteInfo->JumpState == JUMP_STATE_LANDING)
	{
		spriteInfo->Velocity.z += 40.8f* gTimeScale;
		spriteInfo->Location.z += (spriteInfo->Velocity.z * gTimeScale);
		if(spriteInfo->Location.z > spriteInfo->WaterLevel)
		{
			spriteInfo->Velocity.z /= 2;
			spriteInfo->JumpState = JUMP_STATE_RECOVERING;
		}
	}
	else if (spriteInfo->JumpState == JUMP_STATE_RECOVERING)
	{
		spriteInfo->Velocity.z -= 31.2f* gTimeScale;
		spriteInfo->Location.z += (spriteInfo->Velocity.z * gTimeScale);
		
		if(spriteInfo->Location.z <= spriteInfo->WaterLevel)
		{
			if(fabs(spriteInfo->Velocity.z) <= 0.65f)
			{
				spriteInfo->Velocity.z = 0;
				spriteInfo->Location.z = spriteInfo->WaterLevel;
				spriteInfo->JumpState = JUMP_STATE_NOT_READY;
			}
			else
			{
				spriteInfo->JumpState = JUMP_STATE_LANDING;
			}
			
		}
	}
		
	returnVelocity.z = 0.0f;
	
	return returnVelocity;
}

static void spriteMethodsSetJumpAnim(SpriteInfo *spriteInfo)
{
	int anim = (rand() % 3);
	if (anim == 0)
	{
		spriteInfo->CurrentAnimation = PAIN_A;
	}
	else if (anim == 1)
	{
		spriteInfo->CurrentAnimation = PAIN_B;
	}
	else if (anim == 2)
	{
		spriteInfo->CurrentAnimation = PAIN_C;
	}
}

///<summary>
/// Handles a jump animation for a sprite as required...
///</summary>							
void spriteMethodsHandleJump(SpriteInfo *spriteInfo, float jumpForce)
{
	if((spriteInfo->JumpState == JUMP_STATE_NOT_READY) || (spriteInfo->JumpState == JUMP_STATE_RECOVERING))
	{
			// This will move the sprite up from down below...
		spriteInfo->Velocity.z = jumpForce;
		spriteInfo->JumpState = JUMP_STATE_JUMPING;
		if(jumpForce < -0.25f)
		{
			spriteMethodsSetJumpAnim(spriteInfo);
		}
	}
}

///<summary>
/// Updates the current location of the sprite, and accounts for any jumps needed...
///</summary>
void spriteMethodsSetLocation(Vector &location, SpriteInfo *spriteInfo)
{
	//printf("\x1b[2;0H"); printf("PZ: %4.4f, LZ: %4.4f, WL: %4.4f, State: %d", mySpriteInfo.Location.z, 
	//	location.z, mySpriteInfo.WaterLevel, mySpriteInfo.JumpState);

	if((location.z < spriteInfo->WaterLevel) && spriteInfo->JumpState == JUMP_STATE_NOT_READY)
	{
		spriteInfo->Location = location;
	}
	else if ((spriteInfo->Location.z  >= location.z) && ((spriteInfo->JumpState == JUMP_STATE_LANDING) || (spriteInfo->JumpState == JUMP_STATE_RECOVERING)))
	{
		spriteInfo->VectorLength = 0.0f;
		spriteInfo->Velocity.x = 0;
		spriteInfo->Velocity.y = 0;
		spriteInfo->Location.z  = location.z;
		spriteInfo->Velocity.z = 0.0f;
		//spriteInfo->Velocity.z = -(spriteInfo->Velocity.z/1.9);
		spriteInfo->JumpState = JUMP_STATE_RECOVERING;

	}
	else
	{
		spriteInfo->Location.x = location.x;
		spriteInfo->Location.y = location.y;		
	}
}

///<summary>
/// Locks onto the closest target by changing the angle of the sprite...
/// returns true if target is in range, otherwise false...
///</summary>							
bool spriteMethodsLockOnTarget(ISprite &target, SpriteInfo *spriteInfo)
{
	bool baddieInRange = false;
	// Add up the number of units away a sprite is...
	Vector targetLocation = target.getLocation();
	targetLocation.x -= target.getOffset().x;
	targetLocation.y -= target.getOffset().y;	

	float targetDistance = fabs(spriteInfo->Location.x - targetLocation.x) + fabs(spriteInfo->Location.y - targetLocation.y);
	//printf("\x1b[2;0H"); printf("dist: %4.4f", targetDistance); 	
	if(targetDistance <= MAXIMUM_TARGET_DISTANCE)
	{
		// If the total distance is less than the current target distance then we should adjust our angle
		// so that it's pointing at the new target...
		if ((targetDistance < spriteInfo->CurrentTargetDistance)
				&& (target.getCurrentAnimation() != DEATH_FALLBACK)
				&& (target.getCurrentAnimation() != DEATH_FALLFORWARD)
				&& (target.getCurrentAnimation() != DEATH_FALLBACKSLOW))
		{		
			// Only return true if we're ready to fire (that should be closer than max distance...)
			if(targetDistance < MAXIMUM_TARGET_DISTANCE/2)
			{
				baddieInRange = true;
			}

			// Find the angle between the new target
			spriteInfo->CurrentTargetDistance = targetDistance;			
			
			// Find the angle between the sprite and the target, and change the angle of this sprite to point toward it...
			float newAngle = getAngleBetweenTwoPoints(targetLocation.x, targetLocation.y, spriteInfo->Location.x, spriteInfo->Location.y);		
			
			if(newAngle != NAN)
			{
				spriteInfo->ControlAngle = newAngle - 90.0f;
				if(spriteInfo->ControlAngle < 0.0f) spriteInfo->ControlAngle += 360.0f;				
				spriteInfo->TargetDegrees = newAngle;				
			}						
		}	
	}
	return baddieInRange;
}
