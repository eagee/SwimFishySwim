// Profiler.cpp
// Provides methods to perform simple profiling operations on a method by method basis and output that information to the trace log...
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include <time.h>

#ifdef PLATFORM_IS_WII
#include <ogcsys.h>
#include <ogc/lwp_watchdog.h>
#include <ogcsys.h>
#include <ogc/lwp_watchdog.h>
#endif

#ifdef PLATFORM_IS_WIN
#include <Windows.h>
#endif

#include "assert.h"
#include "Profiler.h"

// Used to store the profiling information...
typedef struct tagMethodProfile
{	
	std::string MethodName;
	float StartedAtMs;	
	float TotalMsElapsed;
} MethodProfile;

typedef std::map<std::string, MethodProfile> MethodData;
typedef MethodData::iterator MethodDataIterator;

MethodData gMethodData;
MethodDataIterator gMethodIterator;

///<summary>
/// Records the time the profiler was started
///</summary>
void profileStart(std::string methodName)
{
	gMethodIterator = gMethodData.find(methodName);
	
	// If we found the method already, then update the starting time...
	// otherwise add a new element into the map based on the method name...
	if(gMethodIterator != gMethodData.end())
	{
		ASSERT(gMethodData[methodName].StartedAtMs == 0);
#ifdef PLATFORM_IS_WII
		gMethodData[methodName].StartedAtMs = (gettime() / TB_TIMER_CLOCK);
#endif
#ifdef PLATFORM_IS_WIN		
		gMethodData[methodName].StartedAtMs = (float)GetTickCount();
#endif
	}
	else
	{
		MethodProfile newMethod;
		newMethod.MethodName = methodName;

#ifdef PLATFORM_IS_WII
		newMethod.StartedAtMs = (gettime() / TB_TIMER_CLOCK);
#endif
#ifdef PLATFORM_IS_WIN		
		gMethodData[methodName].StartedAtMs = (float)GetTickCount();
#endif
		newMethod.TotalMsElapsed = 0;		
		gMethodData[methodName] = newMethod;
	}
}

///<summary>
/// Records the time the profiler was stopped, and adds the total time elapsed to a total for that function
///</summary>
void profileStop(std::string methodName)
{
	gMethodIterator = gMethodData.find(methodName);

	// If we found the method already, then update the starting time...
	// otherwise add a new element into the map based on the method name...
	if (gMethodIterator != gMethodData.end())
	{
#ifdef PLATFORM_IS_WII
		float elapsedThisRun = ((gettime() / TB_TIMER_CLOCK) - gMethodData[methodName].StartedAtMs);
#endif
#ifdef PLATFORM_IS_WIN		
		float elapsedThisRun = GetTickCount() - gMethodData[methodName].StartedAtMs;
#endif
		gMethodData[methodName].TotalMsElapsed += elapsedThisRun;
		gMethodData[methodName].StartedAtMs = 0;
	}
}

///<summary>
/// Writes output from the profiler to the display
///</summary>
void outputProfilingInformation()
{
	/*
	// Printout all of our profiling details!
	MethodDataIterator methodIterator = gMethodData.begin();
	printf("\x1b[2;0H");
	printf("Profiling Results (Press A to Exit):\n");
	printf("MethodName:				TotalTime:\n\n");
	for(u32 idx = 0; idx < gMethodData.size(); idx++)
	{
		printf("%s				%f\n", methodIterator->second.MethodName.c_str(), methodIterator->second.TotalMsElapsed);
		methodIterator++;
	}
	*/
	// Print out memory usage info :D
	MemTrack::TrackListMemoryUsage();
}
