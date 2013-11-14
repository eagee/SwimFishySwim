// itest.h
// Defines an interface used by all test cases in order to run a test and define success or fail ...
// Author(s): Eagan Rackley

#ifndef ITEST_H
#define ITEST_H

class ITest
   {
   public:
      ///<summary>
	  /// Interface constructor... doesn't do a damn thing ...
	  ///</summary>
	  ITest() {}
      
	  ///<summary>
	  /// Virtual destructor ... doesn't do a damn thing ...
	  ///</summary>
	  virtual ~ITest() {}
	  
	  ///<summary>
	  /// Executes a test case, returns true if successful, false on failure ...
	  ///</summary>
      virtual bool executeTest() = 0;   
   };
   
#endif
