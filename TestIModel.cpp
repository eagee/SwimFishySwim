// TestIControllers.cpp
// Provides contrement implementation of ITest interface for testing the IControllers interface ...
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "ModelMd2.h"
#include "ITest.h"
#include "TestIModel.h"

///<summary>
/// Initializes an instance of the ObserverPatternTest object
///</summary>
TestIModel::TestIModel()
{
	myModel = new ModelMd2();
}
      
///<summary>
/// Disposes of an instance of the ObserverPatternTest object
///</summary>
TestIModel::~TestIModel()
{
	delete myModel;
}


///<summary>
/// Executes the IModel Unit Test
///</summary>
bool TestIModel::executeTest()
{	
	bool returnValue = false;

	
	// Clear the screen ...
	printf(" \033[2J");	
	printf ("\x1b[%d;%dH", 2, 0);
	// Display our first message
	printf("Executing IModel test:\n");
	if(myModel->loadFromFile("//fishy.md2", ANIMATION_TRADITIONAL_MD2) == true)
	{
		myModel->assignTexture(1);
		myModel->setAnimationType(STAND);
		for(int i = 0; i < 500; i++)
		{
			printf("Rendering...\n");
			myModel->renderModel(NULL, true);										
		}
		
		myModel->setAnimationType(RUN);
		for(int i = 0; i < 500; i++)
		{
			printf("Rendering...\n");
			myModel->renderModel(NULL, true);										
		}
		printf("PASSED!\n");
		printf("       PASSED!\n");
		
		printf("PASSED!\n");
		printf("       PASSED!\n");
		
	}
	else
	{
		//for(int i = 0; i < 100; i++)
		{
			printf("FAILED!\n");
			printf("       FAILED!\n");
		}
	}
	
	
	return returnValue;
}
