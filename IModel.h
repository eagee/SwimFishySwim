// Model.h
// Defines Interface for loading and displaying 3d model information...
// Author(s): Eagan Rackley
#ifndef IMODEL_H
#define IMODEL_H

#include "IGraphics.h"

///<summary>
/// Defines a list of animations types that we can make use of in our game, they stand for generic state used in other games, too
///</summary>
typedef enum 
{
	STAND,
	RUN,
	ATTACK_A,
	ATTACK_B,
	ATTACK_C,
	PAIN_A,
	PAIN_B,
	PAIN_C,
	JUMP,
	FLIP,
	SALUTE,
	FALLBACK,
	WAVE,
	POINTY,
	CROUCH_STAND,
	CROUCH_WALK,
	CROUCH_ATTACK,
	CROUCH_PAIN,
	CROUCH_DEATH, 
	DEATH_FALLBACK,
	DEATH_FALLFORWARD,
	DEATH_FALLBACKSLOW,
	BOOM,
	REVERSE_ATTACK,
	MAX_ANIMATIONS
} AnimationType;

///<summary>
/// Defines a list of animations types that we can make use of in our game, they stand for generic state used in other games, too
///</summary>
typedef enum
{
	ANIMATION_TRADITIONAL_MD2,
	ANIMATION_PRE_RENDERED_MD2,
	ANIMATION_TRADITIONAL_FAST_MD2,
	ANIMATION_STATIC_MD2,
	ANIMATION_BADDIE_MD2,
	ANIMATION_TEXTURE
} AnimationFormat;

///<summary>
/// Defines animation state data as stored in the md2 file (float data compressed into 16 bits)
///</summary>
typedef struct
{
	u32		startFrame;
	u32		endFrame;
	u32		fps;
	char    frameSkip;

	float	currentTime;
	float	lastTime;

	float	interpolateBy;
	
	u32		type;

	u32		currFrame;
	u32		nextFrame;
	float   frameIncrement;

	// Is incremented each time a frame is completely drawn...
	u32 frameLoopCount;

} ModelAnimState;

///<summary>
/// Defines an interface into any type of model
///</summary>
class IModel
   {
   public:
      IModel() {}
      virtual ~IModel() {}
	  virtual bool loadFromFile(const char *fileName, AnimationFormat animationFormat) = 0;
	  virtual void assignTexture(TextureHandle textureID) = 0;
	  virtual void assignBumpTexture(TextureHandle bumpTextureID, TextureHandle lightTextureID) = 0;
	  virtual void translateModel(Vector translation) = 0;
	  virtual void setAnimationType(AnimationType type) = 0;
	  virtual void setAnimationType(AnimationType type, ModelAnimState &modelAnim) = 0;
	  virtual void setModelScale(float scale) = 0;	  
	  virtual void buildVertexArrays(IGraphics *graphics, bool cullBacks) = 0;
	  virtual void freeVertexArrays(IGraphics *graphics) = 0;
	  virtual void renderModel(IGraphics *graphics, ModelAnimState &animation, bool interpolate) = 0;
	  virtual void renderModel(IGraphics *graphics, bool interpolate) = 0;
	  virtual void handleAnimation(float time, ModelAnimState &modelAnim) = 0;	  	
   };

#endif

