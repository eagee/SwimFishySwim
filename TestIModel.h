// TestIControllers.h
// Definest a concrete implementation of the ITest interface for testing the model loaders ...
// Author(s): Eagan Rackley

#ifndef TESTIMODEL_H
#define TESTIMODEL_H

#include "IModel.h"

///<summary>
/// Definest a concrete implementation of the ITest interface for testing the controllers interface ...
///</summary>
class TestIModel: public ITest
{
	public:
	  ///<summary>
	  /// Initializes an instance of the ObserverPatternTest object
	  ///</summary>
	  TestIModel();
      
	  ///<summary>
	  /// Disposes of an instance of the ObserverPatternTest object
	  ///</summary> 
	  ~TestIModel();
	  
	  ///<summary>
	  /// Executes the ObserverPattern Test Case
	  ///</summary>
      bool executeTest();   
	  	  
	
	private:
	  IModel *myModel;	
};

#endif
