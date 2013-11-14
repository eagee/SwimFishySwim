// testing.h
// Provides a class that can implement and perform an automated test for *ALL* of the objects
// in this project. (And should be used religiously, since we don't have a testing team)
// Author(s): Eagan Rackley

#ifndef TESTING_H
#define TESTING_H

#include "IGraphics.h"
#include "IText.h"
#include "Text.h"
#include "ITest.h"
#include "TestIControllers.h"
#include "TestObserverPattern.h"
#include "TestIModel.h"
#include "TestTiledMaps.h"
#include "TestIText.h"

/// <summary>
/// Provides object that performs and displays automated test-cases on screen for *ALL* objects
/// in this project. 
/// </summary>
class testing
{
	
	public: 

		/// <summary> 
		/// Constructor for testing object 
		/// </summary>
		testing();

		/// <summary> 
		/// Destructor for testing object <see>test.cpp</see>
		/// </summary>
		~testing();
	
		/// <summary> 
		/// Performs all automated testing and displays results on using printf... 
		/// </summary>
		void performAutomatedTesting();		
		
	private:
			TestObserverPattern *myObserverPatternTest;
			TestIControllers *myIControllersTest;
			TestIModel *myIModelTest;
			TestTiledMaps *myTilemapTest;
			TestIText *myITextTest;
};

#endif
