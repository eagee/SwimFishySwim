// observerpatterntest.h
// Defines a concrete implementation for the ITest interface
// used to test the ISubect / IObserver implementations ...
// Author(s): Eagan Rackley

#ifndef TESTOBSERVERPATTERN_H
#define TESTOBSERVERPATTERN_H

#include "IObserver.h"
class Subject;

/// <summary>
/// This is the test subject object used to test the observer pattern.
/// </summary>
class TestSubject: public ISubject
{
public:
	/// <summary>
	/// This is the test subject object used to test the observer pattern
	/// </summary>
	TestSubject();

	/// <summary>
	/// This is the test subject object used to test the observer pattern
	/// </summary>	
	~TestSubject();

	/// <summary>
	/// This is the test subject object used to test the observer pattern
	/// </summary>	
	void registerObserver (IObserver *observer);
	
	/// <summary>
	/// This is the test subject object used to test the observer pattern
	/// </summary>	
	void removeObserver (IObserver *observer);	

	/// <summary>
	/// This is the test subject object used to test the observer pattern
	/// </summary>	
	void notifyObservers ();

private:
	Subject *mySubject;
};

///<summary>
/// Provides implementation of ITest interface to test functionality of the observer pattern objects
///</summary>
class TestObserverPattern: public ITest, public IObserver
{
	public:
	  ///<summary>
	  /// Initializes an instance of the ObserverPatternTest object
	  ///</summary>
	  TestObserverPattern();
      
	  ///<summary>
	  /// Disposes of an instance of the ObserverPatternTest object
	  ///</summary> 
	  ~TestObserverPattern();
	  
	  ///<summary>
	  /// Executes the ObserverPattern Test Case
	  ///</summary>
      bool executeTest();   
	  
	  ///<summary>
	  /// Implements the observer interface and receives a notification from T subject...
	  ///</summary>      
	  void update(ISubject *subject);
	
	private:
	  bool myTestSubjectSentUpdate;
	  TestSubject *myTestSubject;
};

#endif
