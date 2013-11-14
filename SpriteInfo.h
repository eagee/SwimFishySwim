// SpriteInfo.h
// Defines implementation for a common interface for dealing with multiple sprite behaviors...
// Author(s): Eagan Rackley

#ifndef SPRITE_INFO_H
#define SPRITE_INFO_H

// Forward declarations
class IProjectileWeapon;

// Preprocessor Definitions
#define RENDER_X_MIN	-43.0f
#define RENDER_X_MAX	 43.0f
#define RENDER_Y_MIN	-40.0f
#define RENDER_Y_MAX	 40.0f

typedef int DIRECTION;
#define DIRECTION_NONE		0
#define DIRECTION_NORTH   	1
#define DIRECTION_SOUTH   	2
#define DIRECTION_EAST		4
#define DIRECTION_WEST 		8

#define DEFAULT_PERIODIC_TIMER 0.65f

///<summary>
/// Defines the possible sprite behaviors available 
///</summary>
enum SPRITE_TYPE
{	
	PLAYER_SPRITE,
	ENEMY_TARGET_SPRITE,
	BADDIE_SPRITE,
	EFFECT_SPRITE,
	BULLET_SPRITE,
	POWER_UP_SPRITE,
	TOWER_SPRITE
};

///<summary>
/// Defines the possible type of power up a sprite might have
///</summary>
enum SPRITE_POWER_UP
{
	POWER_UP_NONE,
	POWER_UP_GUN_TINY,
	POWER_UP_GUN_SMALL,
	POWER_UP_GUN_MEDIUM,
	POWER_UP_GUN_LARGE	
};

///<summary>
/// Defines the possible sprite collision behaviors
///</summary>
enum COLLISION_TYPE
{	
	FRONT_COLLISION,
	FRONT_LEFT_COLLISION,
	FRONT_RIGHT_COLLISION,
	RIGHT_SIDE_COLLISION,
	LEFT_SIDE_COLLISION,
	RIGHT_SIDE_ATTACK,
	LEFT_SIDE_ATTACK,
	REAR_COLLISION,
	FRONT_ATTACK,
	FRONT_LEFT_SLIDE,
	FRONT_RIGHT_SLIDE,
	REAR_SLIDE,
	FRONT_SLIDE,
	///<summary>Works differently than and may replace other options. This turns the sprite to the angle specified in the foce param...</summary>
	SLIDE_ANGLE_BY_FORCE
};

///<summary>
/// Defines a list of animations types that we can make use of in our game, they stand for generic state used in other games, too
///</summary>
typedef enum 
{
	ATTACK_TYPE_NONE,
	ATTACK_TYPE_PHYSICAL,
	ATTACK_TYPE_GUN,
	ATTACK_TYPE_BOTH
} ATTACK_TYPE;


///<summary>
/// Defines the possible states when determining whether or not a sprite will jump ...
///</summary>
enum JUMP_STATE
{
	JUMP_STATE_NOT_READY,
	JUMP_STATE_READY,
	JUMP_STATE_JUMPING,
	JUMP_STATE_LANDING,
	JUMP_STATE_RECOVERING
};

///<summary>
/// Defines a Bounding Triangle type that contains three space coordinates.
/// A and B make the bottom of the triangle and C is the point.
/// A and B will be near the head of the sprite, and C will be near the rear
/// We're using the triangle because it's appropriate for the sprite type
/// and quite a deal more simple than implementing a bounding sphere...
///</summary>
typedef struct tagSpriteBoundingTriangle
{
  Vector	collisionA;
  Vector	collisionB;
  Vector collisionC;
} BoundingTriangle;

typedef struct tagBoundingPoint
{
	float angle;
	Vector vertex;
} BoundingVertex;

///<summary>
/// The points of the bounding circle are set from the center of the model by the bounding size of a sprite.
/// The circle will be offset from the angle of the sprite and contain eight points [0] - [7].
/// Element [0] will represent degree 0 based on the current angle of the fish. Elements [1] to [7] will represent 45 degree increments
/// from element [0] (e.g. 0, 45, 90, 135, 180, 225, 270, 315). The value stored in each element will be vertex coordinates at the
/// specified bounding size / angle away from the center of the sprite...
///</summary>
typedef struct tagBoundingCircle
{
	std::vector<BoundingVertex> verticies;
	std::vector<BoundingVertex>::iterator vertIterator;
} BoundingCircle;

///<summary>
/// Defines four corners of a rectangle which is always sorted from top to bottom,
/// left to right. However one key thing to keep in mind is that our top and bottom
/// zero out in the middle of the screen. So going up is a negative value, and going down is positive...
/// This kind of bounding box is specifically desinged to simplify collision with other sprite objects
/// and it's values can be generated by obtaining the bounding triangle object used in the tilemap
/// collision routines...
///</summary>
typedef struct tagSpriteBoundingBox
{
  float top;
  float bottom;
  float left;
  float right;
  
  ///<summary>
  /// Takes coordinates used in a bounding triangle and sorts them into a bounding box ...
  ///</summary>
  void initialize(BoundingTriangle boundingTriangle)
  {
	  // top = the lowest number (indicates up)
	  // bottom = the highest number (indicates down)	  
	  // left = the lowest number (indicates left)
	  // right = the highest number (indicates right)
	  top = 0.0f;
	  bottom = 0.0f;
	  left = 0.0f;
	  right = 0.0f;

	  // Find the top y coordinate
	  top = boundingTriangle.collisionA.y;
	  if(boundingTriangle.collisionB.y < top) top = boundingTriangle.collisionB.y;
	  if(boundingTriangle.collisionC.y < top) top = boundingTriangle.collisionC.y;

	  // Find the bottom y coordinate
	  bottom = boundingTriangle.collisionA.y;
	  if(boundingTriangle.collisionB.y > bottom) bottom = boundingTriangle.collisionB.y;
	  if(boundingTriangle.collisionC.y > bottom) bottom = boundingTriangle.collisionC.y;

	  // Find the left x coordinate
	  left = boundingTriangle.collisionA.x;
	  if(boundingTriangle.collisionB.x < left) left = boundingTriangle.collisionB.x;
	  if(boundingTriangle.collisionC.x < left) left = boundingTriangle.collisionC.x;

	  // Find the right x coordinate
	  right = boundingTriangle.collisionA.x;
	  if(boundingTriangle.collisionB.x > right) right = boundingTriangle.collisionB.x;
	  if(boundingTriangle.collisionC.x > right) right = boundingTriangle.collisionC.x;
	  
	  // Now we have enough data for our bounding box!
	  // The code can compare x and y coordinates against top, bottom, left, and right
  }
} BoundingBox;

///<summary>
/// Defines a common set of data that can be shared by all ISprite implementations
///</summary>
typedef struct tagSpriteInfo
{	
		// Attributes
		AnimationType CurrentAnimation;
		AnimationType LastAnimation;
		DIRECTION Direction;
		DIRECTION LastDirection;
		SPRITE_TYPE Type;
		Vector Location;				
		Rotate BaseRotation;		
		Rotate CurrentRotation;
		ModelAnimState animationState;		
		SPRITE_POWER_UP PowerUpType;
		bool	Dead;
		float	Health;
		float	MaxHealth;
		float	Width;
		float	Height;
		Vector	Velocity;		
		float	VectorLength;
		float	UnmodifiedMaxVelocity;
		float	MaxVelocity;				
		float	DragVelocity;		
		float	AccelerationVelocity;
		float	AngleRads;
		float	AngleDegrees;
		float	TargetDegrees;
		float   ControlAngle;
		float	ControlMagnitude;
		float	BoundingTriangleSize;	
		float   WaterLevel;
		Rotate  spriteRotationValue;
		int		SpriteLevel;
		int		SpritePointValue;
		int		SpriteID;
		float  CurrentTargetDistance;
		JUMP_STATE	JumpState;
		ATTACK_TYPE AttackType;
		IModel	*sprite3dModel;		
		float periodicCounter;
		IProjectileWeapon *spriteWeapon;
}SpriteInfo;

#endif
