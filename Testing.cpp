// testing.cpp
// Provides a class that can implement and perform an automated test for *ALL* of the objects
// in this project. (And should be used religiously, since we don't have a testing team)
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "LevelOne.h"
#include "LevelMethods.h"
#include "IProjectileWeapon.h"
#include "EndOfWaveMenu.h"
#include "NewTowerMenu.h"
#include "GameMenuInfo.h"
#include "Profiler.h"
#include "angles.h"
#include "IObserver.h"
#include "ISubject.h"
#include "Subject.h"
#include "ITest.h"
#include "IControllers.h"
#include "testing.h"


/// <summary> 
/// Constructor for testing object 
/// </summary>
testing::testing()
{
	myObserverPatternTest = new TestObserverPattern ();
	//myIControllersTest = new TestIControllers();
	myIModelTest = new TestIModel();
	myTilemapTest = new TestTiledMaps();
	myITextTest = new TestIText();
}

/// <summary> 
/// Destructor for testing object <see>test.cpp</see>
/// </summary>		
testing::~testing()
{
	delete myObserverPatternTest;
	//delete myIControllersTest;
	delete myIModelTest;
	delete myTilemapTest;
	delete myITextTest;
}

// Tests our adjust rotation routine... which is apparently cracked out...
bool testAdjustRotation()
{
	float angle = 0;
	float target = 271;
	
	for(int i = 0; i < 89; i++)
	{		
		adjustAngleRotation(angle, target, 1.0f);
	}
	
	return true;
}

/// <summary> 
/// Performs all automated testing and displays results on using printf... 
/// </summary>
void testing::performAutomatedTesting()
{
	bool returnValue;
	returnValue = myObserverPatternTest->executeTest();
	//if(returnValue) returnValue = testAdjustRotation();
	//if(returnValue) myITextTest->executeTest();
	//if(returnValue) myIModelTest->executeTest();
	if(returnValue) returnValue = myTilemapTest->executeTest();
	//if(returnValue) returnValue = myIControllersTest->executeTest();
	//ASSERT(returnValue == true);
}

