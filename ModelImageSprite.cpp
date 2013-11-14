// ModelImageSprite.cpp
// Provides a class that can implement a basic sprite model with animation states for quick and dirty
// character renderings that don't require model details (like bullets and particle effects)
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "ModelImageSprite.h"

///<summary>
/// This static const represents the two quads used to represent a bullet object...
///</summary>
#define SPRITE_SIZE 1.0f
#define SPRITE_HALF 0.5f

/// <summary>
/// Constructor for the Sprite Model
/// </summary>
ModelImageSprite::ModelImageSprite() 
{
	// Setup our starting animation parameters
	myAnim.lastTime = 0.0f;
	myAnim.currentTime = 1.0f;
	setAnimationType(STAND);
	setModelScale(1.0);
	// Set our model to render at the default location
	myLocation.x = 0.0f; 
	myLocation.y = 0.0f; 
	myLocation.z = 15.1f;	
	myTextureHandle = UNDEFINED_TEXTURE_HANDLE;
	// Set up the point structure we use to render the Sprite Model
	myPoint.point.x = 0.0f;
	myPoint.point.y = 0.0f; 
	myPoint.point.z = 0.0f;
	myPoint.texture_S = 0.0f;
	myPoint.texture_T = 0.5f;
	myPoints.push_back(myPoint);
}

/// <summary>
/// Destructor for the Sprite Model
/// </summary>
ModelImageSprite::~ModelImageSprite() 
{
	//TODO: We might need to do some cleanup here... like with the particle effect engines...
}

/// <summary>
/// THE SPRITE MODELS CURRENTLY DO NOT LOAD FILE INFO
/// </summary>
bool ModelImageSprite::loadFromFile(const char *fileName, AnimationFormat animationFormat)
{
	ASSERT(false);
	return true;
}

/// <summary>
/// Defines which texture in the graphics subsystem this model will use
/// </summary>		
void ModelImageSprite::assignTexture(TextureHandle textureID) 
{
	myTextureHandle = textureID;
}

/// <summary>
/// Defines which texture in the graphics subsystem this model will use
/// </summary>		
void ModelImageSprite::assignBumpTexture(TextureHandle bumpTextureID, TextureHandle lightTextureID)
{
	// Sprite model doesn't support bump mapping right now. Maybe some other time :D
	ASSERT(false);
}

/// <summary>
/// Will translate model to vector coordinates
/// </summary>
void ModelImageSprite::translateModel(Vector translation) 
{
	myLocation = translation;
}

/// <summary>
/// Decides what type of animation to render ...
/// </summary>		
void ModelImageSprite::setAnimationType(AnimationType type)
{	
	// Level 1
	if(type == STAND)
	{
		myPoint.texture_S = 0.0f;
		myPoint.texture_T = 0.0f;		
	}
	// Level 2
	else if (type == RUN)
	{
		myPoint.texture_S = 0.5f;
		myPoint.texture_T = 0.0f;		
	}
	// Level 3
	else if (type == ATTACK_A)
	{
		myPoint.texture_S = 0.0f;
		myPoint.texture_T = 0.5f;		
	}
	// Level 4
	else if (type == PAIN_A)
	{
		myPoint.texture_S = 0.5f;
		myPoint.texture_T = 0.5f;		
	}
	else
	{
		myPoint.texture_S = 0.0f;
		myPoint.texture_T = 0.0f;		
	}
	myPoints.clear();
	myPoints.push_back(myPoint);
}

/// <summary>
/// Decides what type of animation to render ...
/// </summary>		
void ModelImageSprite::setAnimationType(AnimationType type, ModelAnimState &modelAnim)
{
	setAnimationType(type);	
}

/// <summary>
/// Some models r big, some models r small, this helps determine which one that is ...
/// </summary>				
void ModelImageSprite::setModelScale( float scale ) 
{
	myScale = scale;
}	  

/// <summary>
/// Will build vertex arrays from our Scaled frames for faster rendering ...
/// </summary>				
void ModelImageSprite::buildVertexArrays(IGraphics *graphics, bool cullBacks)
{
	
}

/// <summary>
/// Cleans up our vertex arrays...
/// </summary>				
void ModelImageSprite::freeVertexArrays(IGraphics *graphics)
{
	
}



/// <summary>
/// Handles all the logic necessary for animating this model ...
/// </summary>
void ModelImageSprite::handleAnimation(float time, ModelAnimState &modelAnim)
{
	// Does nothing ...
}

/// <summary>
/// Can be called after begin scene has been called, it will render the model in the scene, using it's own inernal animation state...
/// </summary>
void ModelImageSprite::renderModel(IGraphics *graphics, bool interpolate)
{
	// TODO: We're going to need a handleAnimation option when
	//handleAnimation(gTimeScale, animation);

	if(graphics != NULL)
	{		
	    ASSERT(myTextureHandle != UNDEFINED_TEXTURE_HANDLE);
				
		if(myTextureHandle != UNDEFINED_TEXTURE_HANDLE)
		{							
			// TODO: Here we want to draw a point for our model, and maybe the points of our particle effect
			//       depending on the current animation state!!! 
			//graphics->addTrianglesToScene(myLocation, newFrame, myMd2HeaderData.numTriangles, myTextureHandle);	
			float pointSize = float(255 * myScale);
			if(pointSize > 255) pointSize = 255;
			graphics->addPointsToScene(myLocation, myPoints, (unsigned char)pointSize, USE_QUARTER_TEXTURE, myTextureHandle);			
			//graphics->addVertexArrayToScene(myLocation, myTextureHandle, gSpriteVertexArray[0]);
		}
	}
}

/// <summary>
/// Can be called after begin scene has been called, it will render the model in the scene...
/// </summary>
void ModelImageSprite::renderModel(IGraphics *graphics, ModelAnimState &animation, bool interpolate)
{		
	renderModel(graphics, interpolate);
}

