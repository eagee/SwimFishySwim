// Tracing.h
// Defines an interface for tracing debug information to a file
// Author(s): Eagan Rackley

#ifndef TRACING_H
#define TRACING_H

///<summary>
/// Sets up the trace log
///</summary>
void setupTrace(const char* traceFile);

///<summary>
/// Traces the information specified in message out 
///</summary>
void trace(const char* file, int line, const char *fmt, ...);

///<summary>
/// Traces information without the file and line data...
///</summary>
void trace(const char *fmt, ...);

///<summary>
/// Closes the trace log
///</summary>
void shutDownTrace();


#endif
