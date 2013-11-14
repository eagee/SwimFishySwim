// ControllerWii.h
// Provides an implementation for the IControllers interface
// Author(s): Eagan Rackley

#ifndef WIIMOTES_H
#define WIIMOTES_H

#ifdef PLATFORM_IS_WII

#include <wiiuse/wpad.h>
#include "IObserver.h"
#include "ISubject.h"
#include "Subject.h"

#define MAXIMUM_WIIMOTES 4

#define MOTION_SAMPLES 3

/// <summary>
/// Used so we can determine some directional comparisons with wiimote activity...
/// </summary>
typedef struct tagSignedAccelData
{
	short x;
	short y;
	short z;
} SignedAccelData;

/// <summary>
/// This class implements the IControllers and ISubject interfaces ...
/// </summary>
class ControllerWii: public IControllers
{
public:

	// Private Operations ...	
	/// <summary>
	/// This is a singleton instance, so the constructor will be private ...
	/// </summary>
	ControllerWii(int screenWidth, int screenHeight);
	
	/// <summary>
	/// This is the test subject object used to test the observer pattern
	/// </summary>	
	virtual ~ControllerWii();
	
	///<summary>
	/// This should be implemented as a singleton object so that anything using an IController
	/// can use a single initialized instance!
	///</summary>
	static IControllers* getSingleInstance(int screenWidth, int screenHeight);
	
	///<summary>
	/// Gets rid of the single instance object used by this class ...
	///</summary>
	void dispose();	
	
	///<summary>
	/// A call to this will update all of the most recent controller states ...
	///</summary>
	void updateControllerStates();
	
	///<summary>
	/// Returns true if button code for controller ID is active, else false
	///</summary>		
	bool checkButtonPress(int controllerID, CONTROLLER_BUTTONS code);

	///<summary>
	/// Returns true if button code for controller ID has been released, else false
	///</summary>		
	bool checkButtonRelease(int controllerID, CONTROLLER_BUTTONS code);
	
	///<summary>
	/// Returns an unsigned int with or'd values of all active buttons
	///</summary>
	unsigned int getStatusOfAllButtons(int controllerID);
	
	///<summary>
	/// Returns status of a device by ID ...
	///</summary>
	CONTROLLER_STATUS checkControllerStatus(int controllerID);
	
	///<summary>
	/// Returns an integer value specifying the number of devices properly connected and identified
	///</summary>
	int numberOfConnectedDevices();
	
	///<summary>
	/// Enables Observer pattern updates of type: BUTTON_UPDATE, POINTER_UPDATE, and MOTION_UPDATE
	///</summary>
	void enableUpdatesFrom(int controllerID, CONTROLLER_UPDATE_TYPE updateType);
	
	///<summary>
	/// Disables Observer pattern updates of type: BUTTON_UPDATE, POINTER_UPDATE, and MOTION_UPDATE
	///</summary>
	void disableUpdatesFrom(int controllerID, CONTROLLER_UPDATE_TYPE updateType);
	
	///<summary>
	/// Can be used to check X,Y pointer information from controller
	///</summary>
	float checkPointer(int controllerID, CONTROLLER_POINTER pointerData);
	
	///<summary>
	/// Can be used to check X,Y,Z Accelerometer motion from controller
	///</summary>
	AccelerometerData checkMotion(int controllerID);
	
	/// <summary>
	/// Can be used to check Pitch, Roll, and Yaw orientation data from controller
	/// </summary>
	float checkOrientation(int controllerID, CONTROLLER_ORIENTATION orientData);
	
	/// <summary>
	/// Tells us the angle of the joystick if applicable (0 to 360 degrees, 0 being up)
	/// </summary>
	float checkJoystickAngle(int controllerID);
		
	/// <summary>
	/// Returns a float between 0.0 and 1.0 defining how much pressure is being placed on the joystick...
	/// </summary>
	float checkJoystickMagnitude(int controllerID);
	
	///<summary>
	/// [Controller is rumbling] stops the rumbling bit ...
	///</summary>
	void stopRumblePack(int controllerID);
		
	///<summary>
	/// [Controller is not rumbling] starts the rumbling bit ...
	///</summary>
	void startRumblePack(int controllerID);

	/// <summary>
	/// This is used to register an observer
	/// </summary>	
	void registerObserver (IObserver *observer);
	
	/// <summary>
	/// This is used to remove a registered observer
	/// </summary>	
	void removeObserver (IObserver *observer);	

	/// <summary>
	/// This is used to notify the observer objects of a change 
	/// </summary>	
	void notifyObservers ();

private:
	
	// Attributes	
	int myMotionIndex;
	AccelerometerData myMotionValues[MOTION_SAMPLES];

	// Used to determine what the averaged motion is between two frames ...
	AccelerometerData myCurrentMotion;
	AccelerometerData myLastMotion;
	SignedAccelData myAccelDifference;
	
	// It's my subject, dummy...
	Subject *mySubject;
	
	// Points to our four possible wiimotes
	WPADData *wiiMotes[MAXIMUM_WIIMOTES];
	
	// Describes the current status of each wiimote
	CONTROLLER_STATUS wiiMoteStatus[MAXIMUM_WIIMOTES];		
};

#endif
#endif