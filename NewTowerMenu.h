// NewTowerMenu.h
// Implements IGameMenu interface to provide a menu option for building a new tower...
// Author(s): Eagan Rackley
#ifndef NEW_TOWER_H
#define NEW_TOWER_H

#include "IGameMenu.h"
#include "GameMenuInfo.h"

///<summary>
/// Defines the simple interface for all audio protocols (midi, mp3, pcm, controller)
///</summary>
class NewTowerMenu: public IGameMenu
{ 
public:

	///<summary>
	/// Constructor that requires user to specify a texture handle for both the button and background images
	///</summary>	
	NewTowerMenu(LevelInfo *levelInfo);

	~NewTowerMenu();

	///<summary>
	/// Runs a menu, and makes changes to the LevelInfo struct based on the menu selection...
	///</summary>	
	virtual void executeMenu(LevelInfo *levelInfo);
private:
	GameMenuInfo myGameMenu;		
};

#endif
