// TestIControllers.h
// Definest a concrete implementation of the ITest interface for testing the model loaders ...
// Author(s): Eagan Rackley

#ifndef TESTITEXT_H
#define TESTITEXT_H

#include "IGraphics.h"
#include "ITest.h"
#include "IText.h"

///<summary>
/// Definest a concrete implementation of the ITest interface for testing the controllers interface ...
///</summary>
class TestIText: public ITest
{
	public:
	  ///<summary>
	  /// Initializes an instance of the ObserverPatternTest object
	  ///</summary>
	  TestIText();
      
	  ///<summary>
	  /// Disposes of an instance of the ObserverPatternTest object
	  ///</summary> 
	  ~TestIText();
	  
	  ///<summary>
	  /// Executes the ObserverPattern Test Case
	  ///</summary>
      bool executeTest();   
	  	  
	
	private:
	  IText *myText;	
};

#endif
