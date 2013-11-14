// Subject.h
// Provides an implementation for the ISubject pure virtual class (interface)
// Author(s): Eagan Rackley

#ifndef SUBJECT_H
#define SUBJECT_H

class Subject: public ISubject
   {
    public:
       
		///<summary>
		/// Constructor for the Subject ISubject implementation
		///</summary>		
		Subject(ISubject *owner);
	   
		///<summary>
		/// Destructor for the Subject ISubject implementation
		///</summary>
		~Subject();

		///<summary>
		///Registers an observer of this subject object
		///</summary> 
		virtual void registerObserver (IObserver *observer);         
		
		///<summary>
		/// Removes an observer of this subject object
		///</summary>
		virtual void removeObserver (IObserver *observer); 
	    
		///<summary>
		/// Notifies observers of this object of a change in state ...
		///</summary>
		virtual void notifyObservers ();
		
	private:
	
		// Attributes				
		typedef std::list<IObserver*> ObserverList;
		typedef ObserverList::iterator ObserverIterator;
		ObserverList myObservers;

		
		// Associations
		ISubject *myOwner;
		
		// Operations
		
		///<summary>
		///Checks to see whether ISubject implementation already contains observer ...
		///</summary>
		///<returns> [list contains observer] true [else] false </returns>
		bool containsObserver (IObserver *observer);
      	    
   };

#endif
