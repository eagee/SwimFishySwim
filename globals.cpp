// globals.cpp
// Defines variables for the application that every old body will use...
// Author(s): Eagan Rackley
#include <iostream>
#include <string>
#include "globals.h"

float gTimeScale;

///<summary>
/// Returns a default path appended to another asset path in order to ensure 
/// assets can be loaded regardless of the path on the intended platform.
///</summary>
std::string getAssetPath(std::string root, std::string path)
{
	std::string returnValue = root + path;
	return returnValue.c_str();
}
