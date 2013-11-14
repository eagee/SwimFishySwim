// GameMenuMethods.h
// Provides delegate functionality for all IGameMenu implementations
// Author(s): Eagan Rackley
#ifndef GAME_MENU_METHODS_H
#define GAME_MENU_METHODS_H

#define NO_BUTTON_CLICKED 1024

///<summary>
/// Setup Game Menu Info Object
///<summary>
void GameMenuMethodsSetupMenuInfo(GameMenuInfo *menuInfo, TextureHandle background, TextureHandle button, TextureHandle focusedButton);

///<summary>
/// Setup a game label object
///<summary>
void GameMenuMethodsSetupLabel(Vector location, float width, float height, float spacing, GameLabel *label, std::string text);

///<summary>
/// Setup a game button object
///<summary>
void GameMenuMethodsSetupButton(Vector location, LevelInfo *levelInfo, GameButton *button, float w, float h, std::string text);

///<summary>
/// Only renders the buttons for a menu
///<summary>
void GameMethodsRenderButtonsOnly(int controllerID, GameMenuInfo *menuInfo, LevelInfo *levelInfo);

///<summary>
/// Render our menu information
///<summary>
void GameMenuMethodsRenderMenu(int controllerID, GameMenuInfo *menuInfo, LevelInfo *levelInfo);


///<summary>
/// Gets the button index ID from the button list that matches the current pointer location.
/// Returns NO_BUTTON_CLICKED if no buttons are active...
///<summary>
void GameMenuMethodsUpdateClickedButtonID(int controllerID, GameMenuInfo *menuInfo, LevelInfo *levelInfo);

///<summary>
/// Assigns a global variable used to render the pointer/cursor used in menu operations
///<summary>
void GameMenuMethodsSetupPointer(TextureHandle pointerTexture);

///<summary>
/// Setup a game button object as a square - ignorning the text settings...
///<summary>
void GameMenuMethodsSetupSquareButton(Vector location, LevelInfo *levelInfo, GameButton *button, float size, std::string text);

#endif
