// TestTiledMaps.h
// Defines a concrete implementation for unit testing the TiledMaps object
// Author(s): Eagan Rackley

#ifndef TESTTILEDMAPS_H
#define TESTTILEDMAPS_H

#include "TiledMaps.h"

///<summary>
/// Provides implementation of ITest interface to test functionality of the observer pattern objects
///</summary>
class TestTiledMaps: public ITest
{
	public:
	  ///<summary>
	  /// Initializes an instance of the TestTiledMaps object
	  ///</summary>
	  TestTiledMaps();
      
	  ///<summary>
	  /// Disposes of an instance of the TestTiledMaps object
	  ///</summary> 
	  ~TestTiledMaps();
	  
	  ///<summary>
	  /// Executes the ObserverPattern Test Case
	  ///</summary>
      bool executeTest();   
	  
	
	private:
	
	TiledMaps myTileMap;
};

#endif
