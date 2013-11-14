// IGameMenu.h
// Defines a very simple interface for running menu code in the game
// Author(s): Eagan Rackley
#ifndef IGAMEMENU_H
#define IGAMEMENU_H

///<summary>
/// Defines the simple interface for all audio protocols (midi, mp3, pcm, controller)
///</summary>
class IGameMenu
{ 
	public:
		IGameMenu(){}
		
		virtual ~IGameMenu(){}		
		
		///<summary>
		/// Runs a menu, and makes changes to the LevelInfo struct based on the menu selection...
		///</summary>	
		virtual void executeMenu(LevelInfo *levelInfo) = 0;

		///<summary>
		/// Allows a particular button in a menu to be executed
		///</summary>
		virtual void clickButton(LevelInfo *levelInfo, int buttonID) = 0;
};

#endif
