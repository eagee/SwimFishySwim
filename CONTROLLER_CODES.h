// CONTROLLER_CODES.h
// Defines a set of enumerated values used to access controller information
// <see>IControllers.h</see>
// Author(s): Eagan Rackley
#ifndef CONTROLLER_CODES_H
#define CONTROLLER_CODES_H

#include "globals.h"

/// <summary>
/// Defines update types that can arrive from our controller interface
/// </summary>
enum CONTROLLER_UPDATE_TYPE
{
	BUTTON_UPDATE		= 0x001,
	POINTER_UPDATE		= 0x002,
	MOTION_UPDATE		= 0x003
};

/// <summary>
/// Defines values to access acceleromter data for our controller interface
/// </summary>
enum CONTROLLER_MOTION
{
	MOTION_X = 0x0001,
	MOTION_Y = 0x0002,
	MOTION_Z = 0x0004	
};

/// <summary>
/// Defines values to access orientation data for our controller interface
/// </summary>
enum CONTROLLER_ORIENTATION
{
	ORIENT_PITCH = 0x0001,
	ORIENT_YAW = 0x0002,
	ORIENT_ROLL = 0x0004
};

/// <summary>
/// Defines values to access acceleromter data for our controller interface
/// </summary>
enum CONTROLLER_POINTER
{
	POINTER_X = 0x0001,
	POINTER_Y = 0x0002	
};

#ifdef PLATFORM_IS_WII
	#include "wiiuse/wpad.h"

	/// <summary>
	/// Defines our controller state
	/// </summary>
	enum CONTROLLER_STATUS
	{
		CONTROLLER_READY			= WPAD_ERR_NONE,
		CONTROLLER_NOT_PRESENT		= WPAD_ERR_NO_CONTROLLER,
		CONTROLLER_NOT_READY		= WPAD_ERR_NOT_READY,
		CONTROLLER_STATUS_UNKNOWN   = WPAD_ERR_UNKNOWN
	};

	/// <summary>
	/// Defines values to access buttons for our controller interface
	/// </summary>
	enum CONTROLLER_BUTTONS
	{
		WII_BUTTON_2 			= WPAD_BUTTON_2,
	    WII_BUTTON_1 			= WPAD_BUTTON_1,
		WII_BUTTON_B 			= WPAD_BUTTON_B,
		WII_BUTTON_A			= WPAD_BUTTON_A,
		WII_BUTTON_MINUS		= WPAD_BUTTON_MINUS,
		WII_BUTTON_HOME			= WPAD_BUTTON_HOME,
		WII_BUTTON_LEFT			= WPAD_BUTTON_LEFT,
		WII_BUTTON_RIGHT		= WPAD_BUTTON_RIGHT,
		WII_BUTTON_DOWN			= WPAD_BUTTON_DOWN,
		WII_BUTTON_UP			= WPAD_BUTTON_UP,
		WII_BUTTON_PLUS			= WPAD_BUTTON_PLUS,
		WII_BUTTON_C			= WPAD_NUNCHUK_BUTTON_C,
		WII_BUTTON_Z			= WPAD_NUNCHUK_BUTTON_Z,
		WII_HORIZONTAL_SWING_LEFT	= 0x40000,
		WII_HORIZONTAL_SWING_RIGHT	= 0x80000,
		WII_VERTICAL_SWING_UP		= 0x100000,
		WII_VERTICAL_SWING_DOWN		= 0x200000,
		WII_BUTTON_UNKNOWN		= 0x00
	};
#endif

#ifdef PLATFORM_IS_WIN
		/// <summary>
		/// Defines our controller state
		/// </summary>
		enum CONTROLLER_STATUS
		{
			CONTROLLER_READY			= 0,
			CONTROLLER_NOT_PRESENT		= 1,
			CONTROLLER_NOT_READY		= 2,
			CONTROLLER_STATUS_UNKNOWN   = 3
		};

		/// <summary>
		/// Defines values to access buttons for our controller interface
		/// </summary>
		enum CONTROLLER_BUTTONS
		{
			WII_BUTTON_2 				= 0x00001,
			WII_BUTTON_1 				= 0x00002,
			WII_BUTTON_B 				= 0x00004,
			WII_BUTTON_A				= 0x00008,
			WII_BUTTON_MINUS			= 0x00010,
			WII_BUTTON_HOME				= 0x00020,
			WII_BUTTON_LEFT				= 0x00040,
			WII_BUTTON_RIGHT			= 0x00080,
			WII_BUTTON_DOWN				= 0x00100,
			WII_BUTTON_UP				= 0x00200,
			WII_BUTTON_PLUS				= 0x00400,
			WII_BUTTON_C				= 0x00800,
			WII_BUTTON_Z				= 0x01000,
			WII_HORIZONTAL_SWING_LEFT	= 0x40000,
			WII_HORIZONTAL_SWING_RIGHT	= 0x80000,
			WII_VERTICAL_SWING_UP		= 0x100000,
			WII_VERTICAL_SWING_DOWN		= 0x200000,
			WII_BUTTON_UNKNOWN			= 0x000000
		};
#endif

#endif
