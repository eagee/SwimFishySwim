// SimpleWiiHeap.h
// Defines a very simple heap that we can preallocate persistant data to
// Author(s): Eagan Rackley

#ifndef SIMPLE_HEAP_H
#define SIMPLE_HEAP_H

///<summary>
/// Sets up the Wii heap to use MEM2...
///</summary>			
void simpleWiiHeapInit();

///<summary>
/// Mark the requested memory as used, align the *new* current start address
/// to 32 bytes, and then return the old starting address. That memory will now 
/// no longer be availe. ASSERTS failure and throws std::bad_alloc if we're out
// of room.
///</summary>			
void *simpleWiiHeapAllocatePersistentMemory(size_t memorySize);

#endif
