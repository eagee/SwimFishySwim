// IMap.h
// Defines an interface to be used with map objects. These keep track of map x, map y, and tracks collision with obstacle characters associated with the map.
// Author(s): Eagan Rackley
#ifndef IMAP_H
#define IMAP_H

#include <gccore.h>
#include "IGraphics.h"


///<summary>
/// Defines an interface into any type of model
///</summary>
class IMap
   {
   public:
	
	///<summary>
	/// Defines the constructor for an IMap implementation
	///</summary>
	IMap() {}
	
	///<summary>
	/// Defines the destructor for an IMap implementation
	///</summary>	
	virtual ~IMap() {}
	
	///<summary>
	/// Adds a file/texture combination into the IMap object
	///</summary>	
	virtual bool addFileToMap(const char* fileName, TextureHandle textureID) = 0;
	
	///<summary>
	/// Translates all objects loaded in the map based on an xyz coordinate set
	///</summary>	
	virtual void translateMap(Vector translation) = 0;	  	  
	
	///<summary>
	/// Renders the map object onto the IGraphics implementation
	///</summary>	
	virtual void renderMap(IGraphics *graphics) = 0;
   };

#endif

