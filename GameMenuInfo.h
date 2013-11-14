// GameMenuInfo.h
// Details information that is universal to all IGameMenu implementations...
// Author(s): Eagan Rackley
#ifndef GAME_MENU_INFO_H
#define GAME_MENU_INFO_H

typedef enum
{
	BUTTON_FREE,
	BUTTON_PRESSED,
	BUTTON_RELEASED
} MENU_BUTTON_STATE;

typedef enum
{
	ENABLED_STATE_DISABLED,
	ENABLED_STATE_ALMOST,
	ENABLED_STATE_ENABLED,
	ENABLED_STATE_JUST_USED
} ENABLED_STATE;


///<summary>
/// Defines what constitutes a button in a menu...
///</summary>
typedef struct tagGameLabel
{
	Vector location;
	std::string text;	
	float width;
	float height;
	float spacing;
} GameLabel;

///<summary>
/// Defines what constitutes a button in a menu...
///</summary>
typedef struct tagGameButton
{
	int ID;
	Quad quad;	
	GameLabel label;
	ENABLED_STATE enabled;
	float disabledCounter;
} GameButton;

///<summary>
/// Defines information that is universal to all IGameMenu implementations and their delegate classes...
///</summary>
typedef struct tagGameMenuInfo
{
	std::map<int, GameButton> ButtonList;
	std::vector<GameLabel>  LabelList;
	TextureHandle BackgroundTexture;
	TextureHandle ButtonTexture;
	TextureHandle FocusedButtonTexture;
	Quad backgroundQuad;
	MENU_BUTTON_STATE CurrentButtonState;
	int ClickedButtonID;
} GameMenuInfo;


#endif
