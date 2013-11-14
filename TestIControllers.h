// TestIControllers.h
// Definest a concrete implementation of the ITest interface for testing the controllers ...
// Author(s): Eagan Rackley

#ifndef TESTICONTROLLERS_H
#define TESTICONTROLLERS_H

#include "IControllers.h"

#define TOTAL_CONTROLLER_TESTS 3

///<summary>
/// Definest a concrete implementation of the ITest interface for testing the controllers interface ...
///</summary>
class TestIControllers: public ITest
{
	public:
	  ///<summary>
	  /// Initializes an instance of the ObserverPatternTest object
	  ///</summary>
	  TestIControllers();
      
	  ///<summary>
	  /// Disposes of an instance of the ObserverPatternTest object
	  ///</summary> 
	  ~TestIControllers();
	  
	  ///<summary>
	  /// Executes the ObserverPattern Test Case
	  ///</summary>
      bool executeTest();   
	  	  
	
	private:
	  IControllers *myControllers;	
	  
	  
	  ///<summary>
	  /// Executes all of the sub tests for each controller
	  ///</summary>
	  bool runAllTestsForController(int controllerID);	  

	  ///<summary>
	  /// Executes a test that will loop until controller state is ready
	  ///</summary>
	  bool testControllerState(int controllerID);
	  
	  ///<summary>
	  /// Executes a test that should test rumble pack on after pressing A, and then off after pressing A again...
	  ///</summary>
	  bool testRumblePack(int controllerID);
	  
	  ///<summary>
	  /// Displays accelerometer data until the user presses The + button
	  ///</summary>
	  bool testAccelData(int controllerID);
	  
	  ///<summary>
	  /// Displays IR data until the user presses The up button
	  ///</summary>
	  bool testIRData(int controllerID);

	  
	  ///<summary>
	  /// Simulates a race between controller one and controller two
	  ///</summary>
  	  bool twoPlayerRacingTest();
	  
	  ///<summary>
	  /// Executes a test that will wait for the A key to be pressed before continuning
	  ///</summary>
	  bool WaitForAButton(int controllerID, CONTROLLER_BUTTONS button);
};

#endif
