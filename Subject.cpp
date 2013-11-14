// testing.cpp
// Provides a class that can implement and perform an automated test for *ALL* of the objects
// in this project. (And should be used religiously, since we don't have a testing team)
// Author(s): Eagan Rackley
#include "SFSHeaders.h"
#include "IObserver.h"
#include "ISubject.h"
#include "Subject.h"

 ///<summary>
 /// Constructor for the Subject ISubject implementation
 ///</summary>
 Subject::Subject(ISubject *owner)
 {
	myOwner = owner;
 }
 
 ///<summary>
 /// Destructor for the Subject ISubject implementation
 ///</summary>
 Subject::~Subject() 
 {	
 }
 
 ///<summary>
 // Registers an observer of this subject object
 ///</summary>
 void Subject::registerObserver (IObserver *observer)
 {
    if(containsObserver(observer) == false)
	{
		myObservers.push_back(observer);
	}
 }
 
 
 ///<summary>
 /// Removes an observer of this subject object
 ///</summary>
 void Subject::removeObserver (IObserver *observer)
 {
    if(containsObserver(observer) == true)
	{
		myObservers.remove(observer);
	}
 }
 
 ///<summary>
 /// Notifies observers of this object of a change in state ...
 ///</summary>
 void Subject::notifyObservers ()
 {
	ObserverIterator i;

	// Send out an update using the myOwner object since this is a delegate class implemetation ...
	for (i = myObservers.begin(); i != myObservers.end(); ++i)
	{
		IObserver* observer = *i;
		observer->update(myOwner);
	}
	
 }
 
 ///<summary>
 ///Checks to see whether ISubject implementation already contains observer ...
 ///</summary>
 ///<returns> [list contains observer] true [else] false </returns?bool Subject::containsObserver (IObserver *observer)
 bool Subject::containsObserver (IObserver *observer)
 {
    bool returnValue = false;
	ObserverIterator i;

	// Send out an update using the myOwner object since this is a delegate class implemetation ...
	for (i = myObservers.begin(); i != myObservers.end(); ++i)
	{
		IObserver* listObserver = *i;
		if(listObserver == observer)
		{
			returnValue = true;
		}
	}
	
	return returnValue;
 }
