// SFSHeaders.h
// Provides a simple method of distributing header files that will be compatible with multiple platforms...
// Author(s): Eagan Rackley
#ifndef SFS_HEADERS_H
#define SFS_HEADERS_H

#include "globals.h"

// Standard C Library
#include <malloc.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef PLATFORM_IS_WII
	#include "asndlib.h"
	#include <gccore.h>
	#include <unistd.h>
	#include <debug.h>
	#include <fat.h>
#endif

#ifdef PLATFORM_IS_WIN
	#include <windows.h>
#endif

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

// 3rd Party Libraries
#include "MemTrack.h"
#include "globals.h"
#include "assert.h"
#include "Tracing.h"
#include "DebugStuffs.h"
#include "EndianStuffs.h"
#include "fastVectors.h"
#include "angles.h"

#endif

