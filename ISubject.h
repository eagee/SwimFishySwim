// ISubject.h
// Defines Subject Interface via a Pure Virtual Class and template methods
// Author(s): Eagan Rackley

#ifndef ISUBJECT_H
#define ISUBJECT_H

// forward declaration of IObserver interface
class IObserver;

///<summary>
/// Defines interface for implementing the Observer pattern in C++
///</summary>       
class ISubject
   {
   public:
      ISubject(){}
      virtual ~ISubject(){}
	  
	  ///<summary>
	  /// Registers an observer that implements the IObserver interface
	  ///</summary> 
      virtual void registerObserver (IObserver *observer) = 0;         
	  
	  ///<summary>
	  /// Removes an observer that implements the IObserver interface
	  ///</summary>       
	  virtual void removeObserver (IObserver *observer) = 0; 
	  
      ///<summary>
	  /// Notifies registered observers of a change in state 
	  ///</summary>       
	  virtual void notifyObservers () = 0;            		  
   };

#endif
