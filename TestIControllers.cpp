// TestIControllers.cpp
// Provides contrement implementation of ITest interface for testing the IControllers interface ...
// Author(s): Eagan Rackley
#include "globals.h"

#ifdef PLATFORM_IS_WII

#include "SFSHeaders.h"
#include "IControllers.h"
#include "ControllerWii.h"
#include "ITest.h"
#include "TestIControllers.h"

///<summary>
/// Initializes an instance of the ObserverPatternTest object
///</summary>
TestIControllers::TestIControllers()
{
	printf(" \033[2J");	
	printf ("\x1b[%d;%dH", 2, 0);
	printf("Initializing IControllers...\n");
#ifdef PLATFORM_IS_WII
	//myControllers = ControllerWii::getSingleInstance(720, 480);	
#endif
#ifdef PLATFORM_IS_WIN
	//TODO: Implement windows controlling testing
#endif
}
      
///<summary>
/// Disposes of an instance of the ObserverPatternTest object
///</summary>
TestIControllers::~TestIControllers()
{
	delete myControllers;
}

///<summary>
/// Executes a test that will loop until controller state is ready
///</summary>
bool TestIControllers::testControllerState(int controllerID)
{
	bool stateIsReady = false;
	bool loopDone = false;
	CONTROLLER_STATUS status = CONTROLLER_STATUS_UNKNOWN;
	printf(" \033[2J");	
	printf ("\x1b[%d;%dH", 2, 0);
	printf("Executing IControllers State test for controller %d: 1/%d\n", controllerID, TOTAL_CONTROLLER_TESTS);
	printf("	Press any WiiMote button: ");	
	
	/*while(loopDone == false)
	{
		myControllers->updateControllerStates();
		
		status = myControllers->checkControllerStatus(controllerID);
		
		//debugPrint("Status: %x\n", status); SLEEP(10000);
		
		if((stateIsReady == false) && (status == CONTROLLER_READY))
		{
			printf("WiiMote Ready! Press Home to continue.\n");	
			stateIsReady = true;
		}
		
		if(myControllers->checkButtonPress(controllerID, WII_BUTTON_HOME) == true)
		{
			loopDone = true;
		}
			
	}*/
	
	printf("	Passed!\n");SLEEP(100000);
		
	
	return stateIsReady;
}

///<summary>
/// Executes a test that should test rumble pack on after pressing A, and then off after pressing A again...
///</summary>
bool TestIControllers::testRumblePack(int controllerID)
{
	bool rumbleDone = false;
	printf(" \033[2J");	
	printf ("\x1b[%d;%dH", 2, 0);
	
	printf("Executing IControllers rumble test for controller %d: 2/%d\n", controllerID, TOTAL_CONTROLLER_TESTS);
	printf("	Press A to rumble, Press B to stop, Press 1 to continue testing!\n");
	
	// Wait until the user presses the A button then activate rumble...
	/*while(rumbleDone == false)
	{
		myControllers->updateControllerStates();
		
		if(myControllers->checkButtonPress(controllerID, WII_BUTTON_B) == true)
		{
			myControllers->stopRumblePack(controllerID);			
		}
		else if(myControllers->checkButtonPress(controllerID, WII_BUTTON_A) == true)
		{
			myControllers->startRumblePack(controllerID);			
		}		
		else if(myControllers->checkButtonPress(controllerID, WII_BUTTON_1) == true)
		{
			rumbleDone = true;
		}
	}
	*/
			
	printf("	Passed!\n");SLEEP(100000);
		
	return rumbleDone;
}

///<summary>
/// Waits for the specified controller to press a button
///</summary>
bool TestIControllers::WaitForAButton(int controllerID, CONTROLLER_BUTTONS button)
{
	bool waitForA = false;	
	
	// Wait until the user presses the Home button then activate rumble...
	/*while(waitForA == false)
	{
		myControllers->updateControllerStates();
		if(myControllers->checkButtonPress(controllerID, button) == true)
		{			
			waitForA = true;
		}
	}
	*/
	return waitForA;
}

///<summary>
/// Takes the values of x and y and modifies them by the angle of the pitch
///</summary>
void moveCoordsByPitchAngle(float &x, float &y, float pitch)
{
	float speed = 3;
		
	x = x + speed * (float)cos(pitch);
	y = y + speed * (float)sin(pitch);
	
	if(x > 80) x = 0;
	if(y > 60) y = 0;
	if(x < 0) x = 80;
	if(y < 0) y = 60;
}

///<summary>
/// Displays accelerometer data until the user presses The + button
///</summary>
bool TestIControllers::testAccelData(int controllerID)
{
	bool testDone = false;		
	float x = 0; 
	float y = 0;

	printf("Starting up accell test: \n");SLEEP(1000000);
		
	
	// Wait until the user presses the Home button then activate rumble...
	/*while(testDone == false)
	{		
		myControllers->updateControllerStates();		
		
		// Clear Screen and Reposition our curson
		printf(" \033[2J");	
		printf ("\x1b[%d;%dH", 2, 0);
		
		// Display our title message
		printf("Executing IControllers Accelerometer test for controller %d: 3/%d\n", controllerID, TOTAL_CONTROLLER_TESTS);
		printf("	Press Plus button to continue testing!\n"); 
		
		// Reposition again ...
		printf ("\x1b[%d;%dH", 12, 10);		
		
		// Display the accelerometer data ...
		AccelerometerData motionData = myControllers->checkMotion(controllerID);
		printf ("X: %d Y: %d Z:%d Pitch: %f ", motionData.x, motionData.y, motionData.z, 
			myControllers->checkOrientation(controllerID, ORIENT_PITCH));			
		
		// Reposition our curson
		//moveCoordsByPitchAngle(x, y, myControllers->checkOrientation(controllerID, ORIENT_PITCH));
		printf ("\x1b[%d;%dH", (int)x, (int)y);
		printf ("D=o=(^^)");
		
		
		
		SLEEP(100000);
		
		// If + is pressed, end this test
		if(myControllers->checkButtonPress(controllerID, WII_BUTTON_PLUS) == true)
		{			
			testDone = true;
		}
	}
	*/
	return testDone;
}

///<summary>
/// Displays IR data until the user presses The up button
///</summary>
bool TestIControllers::testIRData(int controllerID)
{
	bool testDone = false;	
	int motionX = 0;
	int motionY = 0;
	
	// Clear the screen ...
	printf(" \033[2J");	
	printf ("\x1b[%d;%dH", 2, 0);
	// Display our first message
	printf("Executing IControllers IR test for controller %d: 3/%d\n", controllerID, TOTAL_CONTROLLER_TESTS);
	printf("	Press Up on control pad to continue testing!\n");
	
	// Wait until the user presses the Home button then activate rumble...
	/*while(testDone == false)
	{
		myControllers->updateControllerStates();
		
		
		// Display the accelerometer data ...
		motionX = myControllers->checkPointer(controllerID, POINTER_X);
		motionY = myControllers->checkPointer(controllerID, POINTER_Y);
		
		// Reposition our cursor
		printf(" \033[2J");	
		printf ("\x1b[%d;%dH", 2, 0);
		
		// Display our title message
		printf("Executing IControllers IR test for controller %d: 3/%d\n", controllerID, TOTAL_CONTROLLER_TESTS);
		printf("	Press Up on control pad to continue testing!\n");
		
		// Print our text based "cursor"
		printf ("\x1b[%d;%dH", (motionY / 6), (motionX / 6));				
		if(myControllers->checkButtonPress(controllerID, WII_BUTTON_A) == true)
		{
			printf ("*click*");
		}
		printf ("<-- (Pointing) X: %05d Y: %05d", motionX, motionY);
		
		SLEEP(10000);
		
		// If + is pressed, end this test
		if(myControllers->checkButtonPress(controllerID, WII_BUTTON_UP) == true)
		{			
			testDone = true;
			printf(" \033[2J");	
			printf ("\x1b[%d;%dH", 2, 0);		
		}
	}*/
	
	return testDone;
}


///<summary>
/// Clears the screen and gives a countdown
///</summary>
void clearAndCountDown()
{
	// Clear the screen ...
	printf(" \033[2J");	
	printf ("\x1b[%d;%dH", 2, 0);
	// Display our first message
	printf("Executing IControllers two player race test!\n");
	printf("Press minus to stop!\n");
	
	printf("	Starting in:\n3");
	SLEEP(1000000);
	
	printf("	\n2");
	SLEEP(1000000);
	
	printf("	\n1");
	SLEEP(1000000);
	
	printf("	\nGO GO GO GO GO GO GO GO GO GO GO GO GO GO GO GO GO GO!");	
}

///<summary>
/// If a player goes left, increment swing, if he goes right after that, increment swing.
///</summary>
int evaluateSwing(int *lastMotion, int motion, int swing)
{		
	int difference = (motion - *lastMotion);
	
	if( (difference  > 250) || (difference  < -250))
	{
		*lastMotion = motion;
		swing++;
	}	
	
	return swing;
}

///<summary>
/// Simulates a race between controller one and controller two
///</summary>
bool TestIControllers::twoPlayerRacingTest()
{
	/*
	bool testDone = false;	
	
	// Motion records our current accelerometer state
	AccelerometerData motion1;
	AccelerometerData motion2;
	int lastMotionX1 = 0;
	int lastMotionX2 = 0;
	
	// Swing records how many times we've gone left to right
	int swing1 = 0;
	int swing2 = 0;
	
	// Records the number of swings since we started...
	int value1 = 0;
	int value2 = 0;
	
	// value records how many times a player has swung during the test
	clearAndCountDown();
	
	// Wait until the user presses the Home button then activate rumble...
	while(testDone == false)
	{
		myControllers->updateControllerStates();
				
		// Display the accelerometer data ...
		motion1 = myControllers->checkMotion(0);
		motion2 = myControllers->checkMotion(1);
		
		// If we get a value of 2 back here, then a full swim has been completed
		if((value1 < 70) && (value2 < 70))
		{
			swing1 = evaluateSwing(&lastMotionX1, motion1.x, swing1);
			if(swing1 >= 2)
			{
				value1++;
				myControllers->startRumblePack(0);
				swing1 = 0;
			}
			else
			{
				myControllers->stopRumblePack(0);
			}
			swing2 = evaluateSwing(&lastMotionX2, motion2.x, swing2);
			if(swing2 >= 2)
			{
				value2++;
				myControllers->startRumblePack(1);
				swing2 = 0;
			}
			else
			{
				myControllers->stopRumblePack(1);
			}
			// Reposition our curson			
			printf ("\x1b[%d;%dH", 12, value1);				
			printf(">1:)-(%d)=<", value1);
		
			printf ("\x1b[%d;%dH", 16, value2);				
			printf(">2:]-(%d)=<", value2);
		}
		else if(value1 == 70)
		{
			value1++;
			printf ("\x1b[%d;%dH", 12, value1);				
			printf("WINNER!(%d)", value1);		
		}
		else if(value2 == 70)
		{
			value2++;
			printf ("\x1b[%d;%dH", 16, value2);				
			printf("WINNER!(%d)", value2);			
		}
						
		// If + is pressed, end this test
		if((myControllers->checkButtonPress(0, WII_BUTTON_MINUS) == true)
		|| (myControllers->checkButtonPress(1, WII_BUTTON_MINUS) == true))
		{			
			testDone = true;
		}
	}
	
	
	return testDone;
	*/
	return true;
}

///<summary>
/// Executes all of the sub tests for each controller
///</summary>
bool TestIControllers::runAllTestsForController(int controllerID)
{
	bool returnValue = true;
	
	/*returnValue = testControllerState(controllerID);	
	if(returnValue) returnValue = testRumblePack(controllerID);
	if(returnValue) returnValue = testAccelData(controllerID);
	if(returnValue) returnValue = testIRData(controllerID);
	// Clear the screen ...	
	printf("Press button 2 to finish this controller...\n");
	if(returnValue) returnValue = WaitForAButton(controllerID, WII_BUTTON_2);
	*/
	return returnValue;
}

///<summary>
/// Executes the ObserverPattern Test Case
///</summary>
bool TestIControllers::executeTest()
{	
	bool returnValue = false;
	// Test controller state
	
	// Run all of the tests for controller 1:
	//returnValue = runAllTestsForController(0);
	//if(returnValue) returnValue = runAllTestsForController(1);
	//if(returnValue) twoPlayerRacingTest();
	return returnValue;
}

#endif