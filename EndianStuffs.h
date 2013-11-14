// EndianStuffs.h
// Defines a set of Macros that can be used to swap the endianness of anything in the known universe ...
// Author(s): Eagan Rackley

#ifndef ENDIAN_STUFFS_H
#define ENDIAN_STUFFS_H

// Change this to 0 if we're going to compile for one of those retard-little-endian-chips... oh wait...
#define THIS_IS_A_BIG_ENDIAN_SYSTEM 1

// This is used for converting a float from a little endian system to a float on a big endian system ...
typedef union
{
      unsigned char i[ sizeof(float) ];	  
      float f;
}convert;

float convertFloat(float toConvert);

#define SWAP_ENDIAN_SHORT(x) x = (((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8));
#define SWAP_ENDIAN_LONG(x) x = (((x & 0xFF000000) >> 24) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24));
#define SWAP_ENDIAN_FLOAT(theFloat) theFloat = convertFloat(theFloat);
 
#endif
