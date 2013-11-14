// TestIGraphics.h
// Defines a concrete implementation of the ITest interface for testing the graphics display ...
// Author(s): Eagan Rackley

#ifndef TESTIGraphics_H
#define TESTIGraphics_H

#include "IGraphics.h"

///<summary>
/// Defines a concrete implementation of the ITest interface for testing the graphics display ...
///</summary>
class TestIGraphics: public ITest
{
	public:
	  ///<summary>
	  /// Initializes an instance of the ObserverPatternTest object
	  ///</summary>
	  TestIGraphics();
      
	  ///<summary>
	  /// Disposes of an instance of the ObserverPatternTest object
	  ///</summary> 
	  ~TestIGraphics();
	  
	  ///<summary>
	  /// Executes the ObserverPattern Test Case
	  ///</summary>
      bool executeTest();   
	  	  
	
	private:
	  IGraphics *myGraphics;		  	  	  	  	  
};

#endif
