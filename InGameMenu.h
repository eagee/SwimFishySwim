// NewTowerMenu.h
// Implements IGameMenu interface to provide a menu option for building a new tower...
// Author(s): Eagan Rackley
#ifndef IN_GAME_MENU_H
#define IN_GAME_MENU_H

#include "IGameMenu.h"
#include "GameMenuInfo.h"

///<summary>
/// Defines the simple interface for all audio protocols (midi, mp3, pcm, controller)
///</summary>
class InGameMenu: public IGameMenu
{ 
public:

	///<summary>
	/// Constructor that requires user to specify a texture handle for both the button and background images
	///</summary>	
	InGameMenu(LevelInfo *levelInfo);

	~InGameMenu();

	///<summary>
	/// Runs a menu, and makes changes to the LevelInfo struct based on the menu selection...
	///</summary>	
	virtual void executeMenu(LevelInfo *levelInfo);

	///<summary>
	/// Allows a particular button in a menu to be executed
	///</summary>
	virtual void clickButton(LevelInfo *levelInfo, int buttonID);
private:
	GameMenuInfo myGameMenu;
	bool myButtonIsPressed;
};

#endif
