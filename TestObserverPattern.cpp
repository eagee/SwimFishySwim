// testObserverPattern.cpp
// Provides implementation of testObserverPattern.h definition
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "ITest.h"
#include "IObserver.h"
#include "ISubject.h"
#include "Subject.h"
#include "TestObserverPattern.h"

 /// <summary>
 /// This is the test subject object used to test the observer pattern
 /// </summary>
 TestSubject::TestSubject() 
 {
 	mySubject = new Subject(this);
 }

 /// <summary>
 /// This is the test subject object used to test the observer pattern
 /// </summary>	
 TestSubject::~TestSubject() 
 {
 	delete mySubject;
 }

 /// <summary>
 /// This is the test subject object used to test the observer pattern
 /// </summary>	
 void TestSubject::registerObserver (IObserver *observer)         
 {
 	mySubject->registerObserver(observer);
 }
	
 /// <summary>
 /// This is the test subject object used to test the observer pattern
 /// </summary>	
 void TestSubject::removeObserver (IObserver *observer)
 {
	mySubject->removeObserver(observer);
 } 

 /// <summary>
 /// This is the test subject object used to test the observer pattern
 /// </summary>	
 void TestSubject::notifyObservers ()
 {
	mySubject->notifyObservers(); 
 }


///<summary>
/// Initializes an instance of the ObserverPatternTest object
///</summary>
TestObserverPattern::TestObserverPattern()
{
	myTestSubject = new TestSubject();
	myTestSubjectSentUpdate = false;
}
      
///<summary>
/// Disposes of an instance of the ObserverPatternTest object
///</summary>
TestObserverPattern::~TestObserverPattern()
{
	delete myTestSubject;
}

///<summary>
/// Executes the ObserverPattern Test Case
///</summary>
bool TestObserverPattern::executeTest()
{	
	bool returnValue = false;
	printf("Executing ObserverPattern test 1: Register and Notify\n");
	myTestSubject->registerObserver(this);
	myTestSubject->notifyObservers();
	if(myTestSubjectSentUpdate == true)
	{
		myTestSubjectSentUpdate = false;
		printf("	Passed!\n");
		printf("Executing ObserverPattern test 2: Remove and Notify\n");
		myTestSubject->removeObserver(this);
		myTestSubject->notifyObservers();
		if(myTestSubjectSentUpdate == false)
		{
			printf("	Passed!\n");
			returnValue = true;
		}
		else
		{
			printf("	Failed!\n");
		}
	}
	else
	{
		printf("	Failed!\n");
	}
	
	return returnValue;
}

///<summary>
/// Implements the observer interface and receives a notification from T subject...
///</summary>      
void TestObserverPattern::update(ISubject *subject)
{
	
	if(subject == myTestSubject)
	{
		printf("	Update Rxed!\n");
		myTestSubjectSentUpdate = true;
	}
	else
	{
		printf("	Update Rxed, but invalid type!\n");
	}
}
