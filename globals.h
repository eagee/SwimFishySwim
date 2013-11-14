// globals.h
// Defines variables for the application that every old body will use...
// Author(s): Eagan Rackley

#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <string>


///<summary> Defines a global variable for tracking time (e.g. Time passed to define how far an object should move)</summary>
extern float gTimeScale;

///<summary> How long it took the most recent frame to render to the display</summary>
extern float gThisFrameTime;

///<summary> How long it took the last frame to render to the display </summary>
extern float gLastFrameTime;

/// <summary> 
/// Defines the possible platforms that we might be porting for. Comment out all but the target platform.
/// </summary>
#define PLATFORM_IS_WII 1
//#define PLATFORM_IS_WIN 1
//#define PLATFORM_IS_LIN 1
//#define PLATFORM_IS_IOS 1
//#define PLATFORM_IS_AND 1

/// <summary>
/// Defines a set of common commands, structs, and values used on either platform.
/// </summary>
#ifdef PLATFORM_IS_WII
    // Functions/Macros
	#define SLEEP(value) usleep(value)	
#endif
#ifdef PLATFORM_IS_WIN
	// Functions/Macros
	#define SLEEP(value)
	static const union { unsigned long __i[1]; float __d; } __Nanf = {0x7FC00000};
    #define NAN (__Nanf.__d)
    // Structs, typedefs
    typedef float f32 ;
	typedef unsigned char u8;
	typedef unsigned int uint;
	typedef unsigned short ushort;
    typedef struct tagVecf {f32 x,y,z;} Vector;
#endif

///<summary>
/// decleared and defined in fastVectors.h and fastVectors.cpp
/// used with functions below.
///</summary>
extern Vector fastVectorInit(float x, float y, float z);

///<summary>
/// Returns a default path appended to another asset path in order to ensure 
/// assets can be loaded regardless of the path on the intended platform.
///</summary>
std::string getAssetPath(std::string root, std::string path);

///<summary>
/// We use the following macros to get around porting problems with compilers that support ISO C99
/// initializations (e.g. (Vector){0.0f, 0.0f, 0.0f} and those that don't.
///</summary>
#ifdef PLATFORM_IS_WII
	#define VEC_INIT(x, y, z) (Vector){x, y, z}
#endif
#ifdef PLATFORM_IS_WIN
	#define VEC_INIT(x, y, z) fastVectorInit(x, y, z)
#endif

///<summary>
/// Macros below used to determine where game data is pulled from.
///</summary>
#ifdef PLATFORM_IS_WII
	#define ASSET_PATH(f) getAssetPath("/", f)
#endif
#ifdef PLATFORM_IS_WIN
	#define ASSET_PATH(f) getAssetPath("C:/SFSData", f).c_str()
#endif


#endif
