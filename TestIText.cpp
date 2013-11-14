// TestIText.cpp
// Provides concrement implementation of ITest for testing the IText interface
// Author(s): Eagan Rackley

#include "globals.h"
#include "SFSHeaders.h"
#include "assert.h"
#include "IGraphics.h"
#include "ITest.h"
#include "IText.h"
#include "Text.h"
#include "TestIText.h"


///<summary>
/// Initializes an instance of the ObserverPatternTest object
///</summary>
TestIText::TestIText()
{
	myText = Text::getSingleInstance(NULL);
}

///<summary>
/// Disposes of an instance of the ObserverPatternTest object
///</summary>
TestIText::~TestIText()
{
	delete myText;
}


///<summary>
/// Executes the IModel Unit Test
///</summary>
bool TestIText::executeTest()
{	
	bool returnValue = false;
	TextureHandle texHandle = 1;

	// Clear the screen ...
	printf(" \033[2J");	
	printf ("\x1b[%d;%dH", 2, 0);

	// Display our first message
	printf("Executing IText test:\n");
	printf("1/2: Assigning Texture Value:\n");	
	myText->assignTexture(texHandle);	

	printf("1/2: Testing Text Rendering Routine:\n");	
	Vector textLocation = {1.0f, 1.0f, 1.0f};
	myText->renderText(textLocation, "Hello Word %d ", 1);

	printf("PASSED!\n");
	printf("       PASSED!\n");


	return returnValue;
}
