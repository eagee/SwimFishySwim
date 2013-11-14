// ModelImageSprite.h
// Implements the IModel interface as it applies to a flat graphical point as opposed to an actual 3d model...
// Author(s): Eagan Rackley
#ifndef SPRITE_MODEL_H
#define SPRITE_MODEL_H

#include "IModel.h"


///<summary>
/// Defines an interface into any type of model
///</summary>
class ModelImageSprite: public IModel
{
public:
	/// <summary>
	/// Constructor for the Sprite Model
	/// </summary>
	ModelImageSprite();

	/// <summary>
	/// Destructor for the Sprite Model
	/// </summary>
	~ModelImageSprite();

	/// <summary>
	/// THE SPRITE MODELS CURRENTLY DO NOT LOAD FILE INFO
	/// </summary>
	virtual bool loadFromFile(const char *fileName, AnimationFormat animationFormat);

	/// <summary>
	/// Defines which texture in the graphics subsystem this model will use
	/// </summary>		
	virtual void assignTexture(TextureHandle textureID);

	/// <summary>
	/// Defines which texture in the graphics subsystem this model will use
	/// </summary>		
	virtual void assignBumpTexture(TextureHandle bumpTextureID, TextureHandle lightTextureID);

	/// <summary>
	/// Will translate model to vector coordinates
	/// </summary>
	virtual void translateModel(Vector translation);

	/// <summary>
	/// Decides what type of animation to render ...
	/// </summary>		
	virtual void setAnimationType(AnimationType type);

	/// <summary>
	/// Decides what type of animation to render ...
	/// </summary>		
	virtual void setAnimationType(AnimationType type, ModelAnimState &modelAnim);

	/// <summary>
	/// Some models r big, some models r small, this helps determine which one that is ...
	/// </summary>				
	virtual void setModelScale(float scale);	  

	/// <summary>
	/// Will build vertex arrays from our Scaled frames for faster rendering ...
	/// </summary>				
	virtual void buildVertexArrays(IGraphics *graphics, bool cullBacks);

	/// <summary>
	/// Cleans up our vertex arrays...
	/// </summary>				
	virtual void freeVertexArrays(IGraphics *graphics);

	/// <summary>
	/// Can be called after begin scene has been called, it will render the model in the scene, using it's own inernal animation state...
	/// </summary>
	virtual void renderModel(IGraphics *graphics, ModelAnimState &animation, bool interpolate);

	/// <summary>
	/// Can be called after begin scene has been called, it will render the model in the scene...
	/// </summary>
	virtual void renderModel(IGraphics *graphics, bool interpolate);

	/// <summary>
	/// Handles all the logic necessary for animating this model ...
	/// </summary>
	virtual void handleAnimation(float time, ModelAnimState &modelAnim);
		

private:

	// This identifies the current x,y,z position of the model
	Vector myLocation;

	// Indicates our current animation state ...
	ModelAnimState	myAnim;

	// Indicates the current scale we're drawing our model at ...
	float	myScale;

	// Defines the texture handle the graphics subsystem has indicated for us to use ...
	TextureHandle myTextureHandle;

	// Defines the Point used with the graphics lib to display this sprite ...
	Point myPoint;
	std::vector<Point> myPoints;
};

#endif

