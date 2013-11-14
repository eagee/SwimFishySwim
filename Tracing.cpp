// Tracing.h
// Defines an interface for tracing debug information to a file
// Author(s): Eagan Rackley

#include "SFSHeaders.h"


// Standard C Library
#include <malloc.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// C++ Library
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <cctype>
#include <cstddef>
#include <cstdarg>
#include <string>
#include <iostream>
#include <sstream>

#include "assert.h"
#include "DebugStuffs.h"

// A file handle for us to use ...
FILE *gTraceFileHandle;

// Defines the string buffer in which we keep our formatted string
char gFormattedString[256];


///<summary>
/// Sets up the trace log
///</summary>
void setupTrace(const char* traceFile)
{
	ASSERT(gTraceFileHandle == NULL);	
	gTraceFileHandle = fopen(traceFile, "w+");	
	ASSERT(gTraceFileHandle != NULL);
}

///<summary>
/// Traces the information specified in message out 
///</summary>
void trace(const char* file, int line, const char *fmt, ...)
{
	
	ASSERT(gTraceFileHandle != NULL);
	if(gTraceFileHandle != NULL)
	{
		va_list args;
	
		// Clear out our formatted string buffer will null terminators
		memset(&gFormattedString, 0, sizeof(gFormattedString));	
		va_start(args, fmt);

		// Assign our arguments to a string ...
		vsprintf(gFormattedString, fmt, args);

		// We're done with the formatted text now ...
		va_end(args);

		printf("%s,%d,%s", file, line, gFormattedString); //usleep(1000000);
		fprintf(gTraceFileHandle, "%s,%d,%s", file, line, gFormattedString);
	}
}

///<summary>
/// Traces information without the file and line data...
///</summary>
void trace(const char *fmt, ...)
{	
	//debugPrint("%s\n", gFormattedString); usleep(100000);
	
	/*
	va_list args;
	
	ASSERT(gTraceFileHandle != NULL);
	if(gTraceFileHandle != NULL)
	{
		// Clear out our formatted string buffer will null terminators
		memset(&gFormattedString, 0, sizeof(gFormattedString));	
		va_start(args, fmt);

		// Assign our arguments to a string ...
		vsprintf(gFormattedString, fmt, args);

		// We're done with the formatted text now ...
		va_end(args);

		//fprintf(gTraceFileHandle, "%s\n\r", gFormattedString);
		debugPrint("%s\n", gFormattedString); usleep(100000);	
	}*/
}

///<summary>
/// Closes the trace log
///</summary>
void shutDownTrace()
{
	ASSERT(gTraceFileHandle != NULL);
	if(gTraceFileHandle != NULL)
	{
		fclose(gTraceFileHandle);
	}
}
