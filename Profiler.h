// Profiler.h
// Provides methods to perform simple profiling operations on a method by method basis and output that information to the trace log...
// Author(s): Eagan Rackley

#ifndef PROFILER_H
#define PROFILER_H

///<summary>
/// Starts profiling a routine, calls the routine, then stops profiling the routine...
///</summary>
void profileStart(std::string methodName);

///<summary>
/// Records the time the profiler was stopped, and adds the total time elapsed to a total for that function
///</summary>
void profileStop(std::string methodName);

///<summary>
/// Writes output from the profiler to the display
///</summary>
void outputProfilingInformation();

#endif
