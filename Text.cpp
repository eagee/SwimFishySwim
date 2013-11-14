// Text.cpp
// Provides a class implementation of the IText interface allowing textured text images to be displayed 
// on screen similarly to how printf statements are displayed...
// Author(s): Eagan Rackley

#include "SFSHeaders.h"

// Defines the range of ascii values supported by our texture...
#define MIN_ASCII_RANGE 0
#define MAX_ASCII_RANGE 255

#define TEXT_SPACING 0.20f

// Defines values specifically associated with pulling characters from our textures...
#define NUMBER_OF_CHARACTERS_IN_ROW 	16
#define NUMBER_OF_CHARACTERS_IN_COLUMN 	16
#define WIDTH_OF_EACH_CHARACTER 		0.0625f
#define HEIGHT_OF_EACH_CHARACTER		0.0625f

// Variables to track our single instance and whether or not it's been created ...	
static IText *globalSingleInstance;

///<summary>
///Defines a static type of vector objects using the quad type that are used to define a line of text...
///</summary>   
typedef std::map<int, Quad> TextQuads;

///<summary> 
/// Handles the singleton implementation of the Text object so that it can be used globally...
///</summary>
IText* Text::getSingleInstance(IGraphics *graphics)
{
	if(globalSingleInstance == NULL)
	{		
		globalSingleInstance = new Text(graphics);	
	}
	
	return globalSingleInstance;
}

///<summary>
/// Constructor for the Text IText implementation
///</summary>
Text::Text(IGraphics *graphics)
{	
	myGraphics = graphics;
	myQuadWidth = DEFAULT_TEXT_WIDTH;
	myQuadHeight = DEFAULT_TEXT_HEIGHT;
	myQuadSpacing = DEFAULT_TEXT_SPACING;	
}

///<summary>
/// Changes the menuing options so that we can adjust the width, height, and spacing used by the menu...
///</summary>	
void Text::setTextSize(float width, float height, float spacing)
{
	myQuadWidth = width;
	myQuadHeight = height;
	myQuadSpacing = spacing;
}

///<summary>
/// Returns the current text quad width
///</summary>	
float Text::getTextWidth()
{
	return myQuadWidth;
}

///<summary>
/// Returns the current text quad height
///</summary>	
float Text::getTextHeight()
{
	return myQuadHeight;
}

///<summary>
/// Returns the current spacing used between each text quad
///</summary>	
float Text::getTextSpacing()
{
	return myQuadSpacing;
}

///<summary>
/// Destructor for the Text IText implementation
///</summary>
Text::~Text()
{
}

///<summary>
/// Assings a texture to be used with the text object...
///</summary>	
void Text::assignTexture(TextureHandle textureID)
{
	myTextureHandle = textureID;
}

///<summary>
/// Updates the texture coordinates of the quad to facilitate animation...
///</summary>	
static void updateTextureCoordinates(Quad *quad, int textureIndex)
{
	// Texture our tiles based on the data specified ...
	ASSERT(textureIndex >= MIN_ASCII_RANGE);
	ASSERT(textureIndex < MAX_ASCII_RANGE);
	
 	// Figure out our X and Y texture coordinates based on the ASCII value specified in textureIndex
	float baseTextureX = (textureIndex % NUMBER_OF_CHARACTERS_IN_COLUMN) * WIDTH_OF_EACH_CHARACTER;
	float baseTextureY = (textureIndex / NUMBER_OF_CHARACTERS_IN_ROW) * HEIGHT_OF_EACH_CHARACTER;

	quad->texture_S[0] = baseTextureX;
	quad->texture_S[1] = baseTextureX + WIDTH_OF_EACH_CHARACTER;
	quad->texture_S[2] = baseTextureX + WIDTH_OF_EACH_CHARACTER;
	quad->texture_S[3] = baseTextureX;
			
	quad->texture_T[0] = baseTextureY;
	quad->texture_T[1] = baseTextureY;
	quad->texture_T[2] = baseTextureY + HEIGHT_OF_EACH_CHARACTER;
	quad->texture_T[3] = baseTextureY + HEIGHT_OF_EACH_CHARACTER;
}

///<summary>
/// Here we have a small bit of static duplicate code. If delegate classes weren't such a pain in the ass in C++ I'd move this out...
///</summary>	
static void setupQuad(Quad *quad, float x, float y, float z, float w, float h, int textureIndex)
{
	quad->verticies[0].x = x;	
	quad->verticies[0].y = y;
	quad->verticies[0].z = z;
	
	quad->verticies[1].x = x + w;	
	quad->verticies[1].y = y;
	quad->verticies[1].z = z;
	
	quad->verticies[2].x = x + w;	
	quad->verticies[2].y = y + h;
	quad->verticies[2].z = z;
	
	quad->verticies[3].x = x ;	
	quad->verticies[3].y = y + h;
	quad->verticies[3].z = z;
	
	updateTextureCoordinates(quad, textureIndex);
}
	
///<summary>
/// Renders the formatted text specified at the location specified...
///</summary>	
void Text::renderText(Vector location, const char *fmt, ...)
{	
	TextQuads lineOfQuads;
	Vector startingLocation = location;
	va_list args;
	int asciiValue = 0;
	int newQuadIndex = 0;
	int outputIndex = 0;
	
	// Clear out our formatted string buffer will null terminators
	memset(&myFormattedString, 0, sizeof(myFormattedString));	
	
	va_start(args, fmt);
	
	// Assign our arguments to a string ...
	vsprintf(myFormattedString, fmt, args);
	
	// Loop through our formatted string until we hit a null value...
	while((myFormattedString[outputIndex] != 0) && (outputIndex < MAXIMUM_TEXT_CHARACTERS))
	{
		asciiValue = myFormattedString[outputIndex];
		
		//debugPrint("\n\nASCII VALUE: %d\n\n", asciiValue);usleep(1000000);
		
		// Create a new quad for drawing this value, texture map it, and add it to the list ...
		Quad newQuad;	
		
		setupQuad(&newQuad, location.x, location.y, location.z, myQuadWidth, myQuadHeight, asciiValue);
		
		// Add this quad to our map
		lineOfQuads[newQuadIndex] = newQuad;						
		newQuadIndex++;
		
		location.x += myQuadSpacing;
		outputIndex++;
	}
	
	// We're done with the formatted text now ...
	va_end(args);
	
	if(myGraphics != NULL)
	{
		// So we can render using the IGraphics implementation specified in the parameters above
		Vector zeroVector = {0.0f, 0.0f, 0.0f};
		myGraphics->addQuadsToScene(zeroVector, lineOfQuads, lineOfQuads.size(), myTextureHandle);			
	}
	
}
