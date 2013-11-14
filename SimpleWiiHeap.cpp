// SimpleWiiHeap.h
// Defines a very simple heap for the MEM2 area
// This will be for preallocated data, so *nothing* will ever get freed, you can just *add* to it until
// the toolchain supports the extra 64MB correctly...
// Author(s): Eagan Rackley

#include "globals.h"
#ifdef PLATFORM_IS_WII

#include "SFSHeaders.h"

// Everything will need to be aligned by 32 since this is where we're keeping our images ...
#define MEMORY_ALIGNMENT_SIZE 32

typedef struct tagMemoryBlock
{
	unsigned long startAddress;
	unsigned long endAddress;
} MemoryBlock;

MemoryBlock gMemoryAvailable;

///<summary>
/// Sets up the Wii heap to use MEM2...
///</summary>			
void simpleWiiHeapInit()
{
	gMemoryAvailable.startAddress = (unsigned long)SYS_GetArena2Lo();
	gMemoryAvailable.endAddress = (unsigned long)SYS_GetArena2Hi();

	// Let's (if possible) only use 1/2 of the available Arena2 memory...
	gMemoryAvailable.startAddress += abs((gMemoryAvailable.endAddress - gMemoryAvailable.startAddress)/2);

	// Make sure our starting address stays alligned...
	if(gMemoryAvailable.startAddress % MEMORY_ALIGNMENT_SIZE != 0)
	{
		gMemoryAvailable.startAddress += (gMemoryAvailable.startAddress % MEMORY_ALIGNMENT_SIZE);
	}
}

///<summary>
/// Mark the requested memory as used, align the *new* current start address
/// to 32 bytes, and then return the old starting address. That memory will now 
/// no longer be availe. ASSERTS failure and throws std::bad_alloc if we're out
// of room.
///</summary>			
void *simpleWiiHeapAllocatePersistentMemory(size_t memorySize)
{		
	unsigned long returnValue = gMemoryAvailable.startAddress;
	
	if((returnValue + memorySize) >= gMemoryAvailable.endAddress)
	{
		ASSERT(false);
		throw std::bad_alloc();
	}
	
	gMemoryAvailable.startAddress += memorySize;

	// Figure out how many bytes we need to pad the memory by to keep it aligned...
	unsigned long paddingBytes = (MEMORY_ALIGNMENT_SIZE - (gMemoryAvailable.startAddress % MEMORY_ALIGNMENT_SIZE)) % MEMORY_ALIGNMENT_SIZE;
	
	gMemoryAvailable.startAddress += paddingBytes;
		
	return (void*)returnValue;
}

#endif