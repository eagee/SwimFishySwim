// Text.h
// Implements the IText interface allowing textured text images to be displayed on screen similarly to printf...
// Author(s): Eagan Rackley
#ifndef TEXTOUT_H
#define TEXTOUT_H


#include "SFSHeaders.h"
#include "IGraphics.h"
#include "IText.h"

#define MAXIMUM_TEXT_CHARACTERS 256

///<summary>
/// Defines an interface into any type of model
///</summary>
class Text : public IText
   {
   public:
	///<summary>
	/// Constructor for the Text IText implementation
	/// Handles all the setup stuff, private for singleton...
	///</summary>		
	Text(IGraphics *graphics);

	///<summary>
	/// Changes the menuing options so that we can adjust the width, height, and spacing used by the menu...
	///</summary>	
	virtual void setTextSize(float width, float height, float spacing);

	///<summary>
	/// Returns the current text quad width
	///</summary>	
	virtual float getTextWidth();
	
	///<summary>
	/// Returns the current text quad height
	///</summary>	
	virtual float getTextHeight();
	
	///<summary>
	/// Returns the current spacing used between each text quad
	///</summary>	
	virtual float getTextSpacing();
			
	///<summary>
	/// Destructor for the Text IText implementation
	/// Handles all the shutdown stuff, private for singleton
	///</summary>
	virtual ~Text();
	
	///<summary>
	/// Implements text as a singleton object so that all objects can render text
	/// without having to be aware of the texture ID it is using, but allow objects
	/// to change the texture ID of the instantiated object...
	///</summary>
	static IText* getSingleInstance(IGraphics *graphics);
	
	///<summary>
	/// Assings a texture to be used with the text object...
	///</summary>	
	virtual void assignTexture(TextureHandle textureID);
	
	///<summary>
	/// Renders the formatted text specified at the location specified...
	///</summary>	
	virtual void renderText(Vector location, const char *fmt, ...);
	
   private:
	// Defines the texture handle the graphics subsystem has indicated for us to use ...
	TextureHandle myTextureHandle;
	
	// Reference to a GxGraphics object..
	IGraphics *myGraphics;
	
	// Defines the string buffer in which we keep our formatted string
	char myFormattedString[MAXIMUM_TEXT_CHARACTERS];

	// Defines width, height, and spacing for our fonts
	float myQuadWidth;
	float myQuadHeight;
	float myQuadSpacing;
   };

#endif

