// IText.h
// Defines an interface for displaying text in the application
// Author(s): Eagan Rackley
#ifndef ITEXTOUT_H
#define ITEXTOUT_H

// Defines the width of the tiles we'll use to represent text
#define DEFAULT_TEXT_WIDTH 0.26f
#define DEFAULT_TEXT_HEIGHT 0.26f
#define DEFAULT_TEXT_SPACING 0.20f

///<summary>
/// Defines an interface into any type of model
///</summary>
class IText
   {
   public:
	
	///<summary>
	/// Defines the constructor for an IText implementation
	///</summary>
	IText() {}
	
	///<summary>
	/// Defines the destructor for an IText implementation
	///</summary>	
	virtual ~IText() {}	
	
	///<summary>
	/// Changes the menuing options so that we can adjust the width, height, and spacing used by the menu...
	///</summary>	
	virtual void setTextSize(float width, float height, float spacing) = 0;

	///<summary>
	/// Returns the current text quad width
	///</summary>	
	virtual float getTextWidth() = 0;
	
	///<summary>
	/// Returns the current text quad height
	///</summary>	
	virtual float getTextHeight() = 0;
	
	///<summary>
	/// Returns the current spacing used between each text quad
	///</summary>	
	virtual float getTextSpacing() = 0;

	///<summary>
	/// Assings a texture to be used with the text object...
	///</summary>	
	virtual void assignTexture(TextureHandle textureID) = 0;
	
	///<summary>
	/// Renders the formatted text specified at the location specified...
	///</summary>	
	virtual void renderText(Vector location, const char *fmt, ...) = 0;
   };

#endif

