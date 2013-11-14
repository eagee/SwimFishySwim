// IControllers.h
// Defines a common interface for controller support. I'm just not smart enough
// to define an interface that would work with any controller, but this should still
// help reduce the dificulties associated with porting.
// Author(s): Eagan Rackley
#ifndef ICONTROLLERS_H
#define ICONTROLLERS_H

#include "globals.h"
#include "ISubject.h"
#include "IDisposeable.h"
#include "controller_codes.h"

///<summary>
/// A handy dandy struct to hold any old kind of accelerometer data
///</summary>
typedef struct tagAccelData
{
	ushort x;
	ushort y;
	ushort z;
} AccelerometerData;

///<summary>
/// A handy dandy interface to any old kind of controller (even if it doesn't have a toopid accelerometer)
///</summary>
class IControllers: public ISubject, IDisposeable
   {
public:
				
		///<summary>
		/// A call to this will update all of the most recent controller states ...
		///</summary>
		virtual void updateControllerStates() = 0;
		
		///<summary>
		/// Returns true if button code for controller ID is active, else false
		///</summary>		
		virtual bool checkButtonPress(int controllerID, CONTROLLER_BUTTONS code) = 0;

		///<summary>
		/// Returns true if button code for controller ID has been released, else false
		///</summary>		
		virtual bool checkButtonRelease(int controllerID, CONTROLLER_BUTTONS code) = 0;
		
		///<summary>
		/// Returns an unsigned int with or'd values of all active buttons
		///</summary>
		virtual unsigned int getStatusOfAllButtons(int controllerID) = 0;
		
		///<summary>
		/// Returns true if device is available, else false
		///</summary>
		virtual CONTROLLER_STATUS checkControllerStatus(int controllerID) = 0;
		
		///<summary>
		/// Returns an integer value specifying the number of devices properly connected and identified
		///</summary>
		virtual int numberOfConnectedDevices() = 0;
		
		///<summary>
		/// Can be used to check X,Y pointer information from controller
		///</summary>
		virtual float checkPointer(int controllerID, CONTROLLER_POINTER pointerData) = 0;
		
		///<summary>
		/// Can be used to check X,Y,Z Accelerometer motion from controller
		///</summary>
		virtual AccelerometerData checkMotion(int controllerID) = 0;		
		
		/// <summary>
		/// Can be used to check Pitch, Roll, and Yaw orientation data from controller
		/// </summary>
		virtual float checkOrientation(int controllerID, CONTROLLER_ORIENTATION orientData) = 0;
		
		/// <summary>
		/// Tells us the angle of the joystick if applicable (0 to 360 degrees, 0 being up)
		/// </summary>
		virtual float checkJoystickAngle(int controllerID) = 0;
		
		/// <summary>
		/// Returns a float between 0.0 and 1.0 defining how much pressure is being placed on the joystick...
		/// </summary>
		virtual float checkJoystickMagnitude(int controllerID) = 0;
		
		///<summary>
		/// [Controller is rumbling] stops the rumbling bit ...
		///</summary>
		virtual void stopRumblePack(int controllerID) = 0;
		
		///<summary>
		/// [Controller is not rumbling] starts the rumbling bit ...
		///</summary>
		virtual void startRumblePack(int controllerID) = 0;
		
		///<summary>
	    /// Disposes of whatever disposable object there is ...
	    ///</summary> 
		virtual void dispose() = 0;
				
   };

#endif
