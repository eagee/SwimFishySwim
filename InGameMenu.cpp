// NewTowerMenu.h
// Implements IGameMenu interface to provide a menu option at the end of a wave ...
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "IProjectileWeapon.h"
#include "GameMenuInfo.h"
#include "IGameMenu.h"
#include "InGameMenu.h"
#include "GameMenuMethods.h"
#include "LevelMethods.h"
#include "ISprite.h"
#include "SpriteTower.h"

// Static valused used whenever this menu is displayed
static const int gBaseChompTowerCost = 50;
static const int gBaseStunTowerCost  = 70;
static const int gBaseSpitTowerCost  = 85;
static const int gBaseWindTowerCost  = 95;


///<summary>
/// Sets up a button used to create a tower that will slow enemies down whenever they are near it
///</summary>
static void setupChompTowerButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{
	GameButton towerButton;
	towerButton.ID = CHOMP_TOWER_BUTTON;
	GameMenuMethodsSetupSquareButton(VEC_INIT(0.0370f, -0.0370f, 0.11f), levelInfo, &towerButton, 0.009f, " ");
	gameMenu.ButtonList.erase(CHOMP_TOWER_BUTTON);
	towerButton.enabled = ENABLED_STATE_DISABLED;
	towerButton.disabledCounter = 0.0f;
	gameMenu.ButtonList[CHOMP_TOWER_BUTTON] = towerButton;
}

///<summary>
/// Sets up the upgrade attack button based on the data specified ...
///</summary>
static void setupStunTowerButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{
	GameButton towerButton;
	towerButton.ID = THROWING_TOWER_BUTTON;
	GameMenuMethodsSetupSquareButton(VEC_INIT(0.0465f, -0.0275f, 0.11f), levelInfo, &towerButton, 0.009f, " ");
	gameMenu.ButtonList.erase(THROWING_TOWER_BUTTON);
	towerButton.enabled = ENABLED_STATE_DISABLED;
	towerButton.disabledCounter = 0.0f;
	gameMenu.ButtonList[THROWING_TOWER_BUTTON] = towerButton;

}

///<summary>
/// Sets up the upgrade weapon button based on the data specified ...
///</summary>
static void setupSpittingTowerButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{
	GameButton towerButton;
	towerButton.ID = SPITTING_TOWER_BUTTON;

	GameMenuMethodsSetupSquareButton(VEC_INIT(0.0370f, -0.0180f, 0.11f), levelInfo, &towerButton, 0.009f, " ");
	gameMenu.ButtonList.erase(SPITTING_TOWER_BUTTON);
	towerButton.enabled = ENABLED_STATE_DISABLED;
	towerButton.disabledCounter = 0.0f;
	gameMenu.ButtonList[SPITTING_TOWER_BUTTON] = towerButton;

}

///<summary>
/// Sets up the direction pad image between buttons
///</summary>
static void setupDirectionPad(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{
	GameButton button;
	button.ID = DIRECTION_PAD_BUTTON;
	GameMenuMethodsSetupSquareButton(VEC_INIT(0.03325f, -0.0255f, 0.1f), levelInfo, &button, 0.009f, " ");
	gameMenu.ButtonList.erase(DIRECTION_PAD_BUTTON);
	button.enabled = ENABLED_STATE_DISABLED;
	button.disabledCounter = 0.0f;
	gameMenu.ButtonList[DIRECTION_PAD_BUTTON] = button;

}

///<summary>
/// Sets up a button used to create a tower that will slow enemies down whenever they are near it
///</summary>
static void setupWindTowerButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{
	GameButton towerButton;
	towerButton.ID = WIND_TOWER_BUTTON;
	GameMenuMethodsSetupSquareButton(VEC_INIT(0.0275f, -0.0275f, 0.11f), levelInfo, &towerButton, 0.009f, " ");
	gameMenu.ButtonList.erase(WIND_TOWER_BUTTON);
	towerButton.enabled = ENABLED_STATE_DISABLED;
	towerButton.disabledCounter = 0.0f;
	gameMenu.ButtonList[WIND_TOWER_BUTTON] = towerButton;
}

///<summary>
/// Constructor that requires user to specify a texture handle for both the button and background images
///</summary>	
InGameMenu::InGameMenu(LevelInfo *levelInfo)
{	
	std::stringstream spittingTowerCost;
	spittingTowerCost << gBaseSpitTowerCost;

	GameMenuMethodsSetupMenuInfo(&myGameMenu, levelInfo->MenuTexture, levelInfo->ButtonTexture, levelInfo->FocusedButtonTexture);

	// Define the buttons that will be used in our menu and add it to our vector
	setupSpittingTowerButton(myGameMenu, levelInfo);
	setupStunTowerButton(myGameMenu, levelInfo);
	setupWindTowerButton(myGameMenu, levelInfo);
	setupChompTowerButton(myGameMenu, levelInfo);
	setupDirectionPad(myGameMenu, levelInfo);

	myButtonIsPressed = false;
}

///<summary>
/// Destructor, here we clean up our buttons :)
///</summary>	
InGameMenu::~InGameMenu()
{
}

///<summary>
/// When a button is selected, all buttons are temporarily disabled before
/// becoming available again...
///</summary>
static void temporarilyDisableAllButtons(GameMenuInfo &gameMenu)
{
	std::map<int, GameButton>::iterator buttonIterator;
	buttonIterator = gameMenu.ButtonList.begin();

	for (u32 idx = 0; idx < gameMenu.ButtonList.size(); idx++)
	{
		if(buttonIterator->second.enabled == ENABLED_STATE_ENABLED)
		{
			buttonIterator->second.disabledCounter = 3.0f;
		}
		buttonIterator++;
	}

}

///<summary>
/// If a button should be available if the player can afford the energy cost
/// returns true if a player can afford the cost, else false
///</summary>
bool isButtonAvailable(LevelInfo *levelInfo, GameMenuInfo &gameMenu, int ID, int cost)
{
	bool returnValue = false;

	if (gameMenu.ButtonList[ID].disabledCounter > 0.0f)
	{
			gameMenu.ButtonList[ID].enabled = ENABLED_STATE_JUST_USED;
			gameMenu.ButtonList[ID].disabledCounter -= (5.0f * gTimeScale);
			if(gameMenu.ButtonList[ID].disabledCounter < 0.0f)
			{
				gameMenu.ButtonList[ID].disabledCounter = 0.0f;
			}
	}
	else if(levelInfo->CurrentEnergyLevel >= cost)
	{
		gameMenu.ButtonList[ID].enabled = ENABLED_STATE_ENABLED;
		returnValue = true;
	}
	else if(levelInfo->CurrentEnergyLevel > (cost - 5))
	{
		gameMenu.ButtonList[ID].enabled = ENABLED_STATE_ALMOST;
	}
	else
	{
		gameMenu.ButtonList[ID].enabled = ENABLED_STATE_DISABLED;
	}

	return returnValue;
}

///<summary>
/// Runs code responsible for building a new spitting tower...
///</summary>
static void clickSpittingTowerButton(LevelInfo *levelInfo)
{
	SpriteTower *newTower = new SpriteTower ();
	Vector location;
	levelInfo->Sounds[MENU_YES]->playOnce();
	location = levelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	location.x = -location.x;
	location.y = -location.y;
	LevelMethodsSetupSprite(levelInfo->Models[SPITTING_FLOWER_TOWER], newTower, ATTACK_TYPE_GUN, location.x, location.y, SPRITE_WATER_LEVEL + 2.1f, 1.0f);
	newTower->setHealth(100.0f);
	newTower->setVelocities(0.0f, 0.0f, 0.0f);
	newTower->assignWeapon(levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT]);
	levelInfo->TowerSprites.push_back(*newTower);
	levelInfo->CurrentEnergyLevel -= gBaseSpitTowerCost;
	levelInfo->PlayerOneSprite->setAnimation(SALUTE);
	LevelMethodsHandleSplashEffects(levelInfo, levelInfo->PlayerOneSprite, 100);
	delete newTower;
}

///<summary>
/// Runs code responsible for building a new throwing tower...
///</summary>
static void clickThrowingTowerButton(LevelInfo *levelInfo)
{
	SpriteTower *newTower = new SpriteTower ();
	Vector location;
	levelInfo->Sounds[MENU_YES]->playOnce();
	location = levelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	location.x = -location.x;
	location.y = -location.y;
	LevelMethodsSetupSprite(levelInfo->Models[TOSSING_FLOWER_TOWER], newTower, ATTACK_TYPE_GUN, location.x, location.y, SPRITE_WATER_LEVEL + 2.0f, 1.0f);
	newTower->setHealth(100.0f);
	newTower->setVelocities(0.0f, 0.0f, 0.0f);
	newTower->assignWeapon(levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]);
	levelInfo->TowerSprites.push_back(*newTower);
	levelInfo->CurrentEnergyLevel -= gBaseStunTowerCost;
	levelInfo->PlayerOneSprite->setAnimation(SALUTE);
	LevelMethodsHandleSplashEffects(levelInfo, levelInfo->PlayerOneSprite, 100);
	delete newTower;
}

///<summary>
/// Runs code responsible for building a new throwing tower...
///</summary>
static void clickFanTowerButton(LevelInfo *levelInfo)
{
	SpriteTower *newTower = new SpriteTower ();
	Vector location;
	levelInfo->Sounds[MENU_YES]->playOnce();
	location = levelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	location.x = -location.x;
	location.y = -location.y;
	LevelMethodsSetupSprite(levelInfo->Models[WINDY_FLOWER_TOWER], newTower, ATTACK_TYPE_GUN, location.x, location.y, SPRITE_WATER_LEVEL + 2.0f, 1.0f);
	newTower->setHealth(100.0f);
	newTower->setVelocities(0.0f, 0.0f, 0.0f);
	newTower->assignWeapon(levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]);
	levelInfo->TowerSprites.push_back(*newTower);
	levelInfo->CurrentEnergyLevel -= gBaseStunTowerCost;
	levelInfo->PlayerOneSprite->setAnimation(SALUTE);
	LevelMethodsHandleSplashEffects(levelInfo, levelInfo->PlayerOneSprite, 100);
	delete newTower;
}

///<summary>
/// Runs code responsible for building a new throwing tower...
///</summary>
static void clickChompTowerButton(LevelInfo *levelInfo)
{
	SpriteTower *newTower = new SpriteTower ();
	Vector location;
	levelInfo->Sounds[MENU_YES]->playOnce();
	location = levelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
	location.x = -location.x;
	location.y = -location.y;
	LevelMethodsSetupSprite(levelInfo->Models[CHOMPY_FLOWER_TOWER], newTower, ATTACK_TYPE_GUN, location.x, location.y, SPRITE_WATER_LEVEL + 2.0f, 1.0f);
	newTower->setHealth(100.0f);
	newTower->setVelocities(0.0f, 0.0f, 0.0f);
	newTower->assignWeapon(levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]);
	levelInfo->TowerSprites.push_back(*newTower);
	levelInfo->CurrentEnergyLevel -= gBaseStunTowerCost;
	levelInfo->PlayerOneSprite->setAnimation(SALUTE);
	LevelMethodsHandleSplashEffects(levelInfo, levelInfo->PlayerOneSprite, 100);
	delete newTower;
}


///<summary>
/// Runs a menu, and makes changes to the LevelInfo struct based on the menu selection...
///</summary>	
void  InGameMenu::executeMenu(LevelInfo *levelInfo)
{
	bool menuButtonPressed = levelInfo->Controllers->checkButtonPress(0, WII_BUTTON_A);

	// We have to check whether or not the user has enough energy,
	// and only display the buttons the user has the energy for...

	isButtonAvailable(levelInfo, myGameMenu, CHOMP_TOWER_BUTTON, gBaseChompTowerCost);
	isButtonAvailable(levelInfo, myGameMenu, DIRECTION_PAD_BUTTON, gBaseChompTowerCost);
	isButtonAvailable(levelInfo, myGameMenu, THROWING_TOWER_BUTTON, gBaseStunTowerCost);
	isButtonAvailable(levelInfo, myGameMenu, SPITTING_TOWER_BUTTON, gBaseSpitTowerCost);
	isButtonAvailable(levelInfo, myGameMenu, WIND_TOWER_BUTTON, gBaseWindTowerCost);

	// Check for the A button to be pressed
	if (myGameMenu.CurrentButtonState == BUTTON_RELEASED)
	{
		myGameMenu.CurrentButtonState = BUTTON_FREE;
	}
	else if (myGameMenu.CurrentButtonState == BUTTON_FREE)
	{
		myGameMenu.ClickedButtonID = NO_BUTTON_CLICKED;
		if (menuButtonPressed == true)
		{
			myGameMenu.CurrentButtonState = BUTTON_PRESSED;
			GameMenuMethodsUpdateClickedButtonID(0, &myGameMenu, levelInfo);
		}
	}
	// Once A is pressed, check for it to be released, then once it's released check to see if we were inside
	// a set of button coordinates...
	else if ((myGameMenu.CurrentButtonState == BUTTON_PRESSED) && (menuButtonPressed == false))
	{
		GameMenuMethodsUpdateClickedButtonID(0, &myGameMenu, levelInfo);
		switch (myGameMenu.ClickedButtonID)
		{
		case SPITTING_TOWER_BUTTON:
			// Add a new throwing tower to our level (we'll have to find out how many towers we can actually have)
			if(isButtonAvailable(levelInfo, myGameMenu, myGameMenu.ClickedButtonID, gBaseSpitTowerCost) == true)
			{
				clickSpittingTowerButton(levelInfo);
				//temporarilyDisableAllButtons(myGameMenu);
			}
			else
			{
				levelInfo->Sounds[MENU_NO]->playOnce();
			}
			break;
		case THROWING_TOWER_BUTTON:
			// Add a new throwing tower to our level (we'll have to find out how many towers we can actually have)
			if(isButtonAvailable(levelInfo, myGameMenu, myGameMenu.ClickedButtonID, gBaseStunTowerCost) == true)
			{
				clickThrowingTowerButton(levelInfo);
				//temporarilyDisableAllButtons(myGameMenu);
			}
			else
			{
				levelInfo->Sounds[MENU_NO]->playOnce();
			}
			break;
		default:
			break;
		}

		myGameMenu.CurrentButtonState = BUTTON_RELEASED;
	}

	GameMethodsRenderButtonsOnly(0, &myGameMenu, levelInfo);
}

///<summary>
/// Allows a particular button in a menu to be executed
///</summary>
void InGameMenu::clickButton(LevelInfo *levelInfo, int buttonID)
{
	if (buttonID == NO_TOWER_BUTTON_SELECTED)
	{
		myButtonIsPressed = false;
	}

	// Once A is pressed, check for it to be released, then once it's released check to see if we were inside
	// a set of button coordinates...
	else if (myButtonIsPressed == false)
	{
		myButtonIsPressed = true;
		switch (buttonID)
		{
		case CHOMP_TOWER_BUTTON:
			// Add a new throwing tower to our level (we'll have to find out how many towers we can actually have)
			if (isButtonAvailable(levelInfo, myGameMenu, buttonID, gBaseChompTowerCost) == true)
			{
				clickChompTowerButton(levelInfo);
				temporarilyDisableAllButtons(myGameMenu);
			}
			else
			{
				levelInfo->Sounds[MENU_NO]->playOnce();
			}
			break;
		case THROWING_TOWER_BUTTON:
			// Add a new throwing tower to our level (we'll have to find out how many towers we can actually have)
			if (isButtonAvailable(levelInfo, myGameMenu, buttonID, gBaseStunTowerCost) == true)
			{
				clickThrowingTowerButton(levelInfo);
				temporarilyDisableAllButtons(myGameMenu);
			}
			else
			{
				levelInfo->Sounds[MENU_NO]->playOnce();
			}
			break;
		case SPITTING_TOWER_BUTTON:
			// Add a new throwing tower to our level (we'll have to find out how many towers we can actually have)
			if (isButtonAvailable(levelInfo, myGameMenu, buttonID, gBaseSpitTowerCost) == true)
			{
				clickSpittingTowerButton(levelInfo);
				temporarilyDisableAllButtons(myGameMenu);
			}
			else
			{
				levelInfo->Sounds[MENU_NO]->playOnce();
			}
			break;
		case WIND_TOWER_BUTTON:
			// Add a new throwing tower to our level (we'll have to find out how many towers we can actually have)
			if (isButtonAvailable(levelInfo, myGameMenu, buttonID, gBaseWindTowerCost) == true)
			{
				clickFanTowerButton(levelInfo);
				temporarilyDisableAllButtons(myGameMenu);
			}
			else
			{
				levelInfo->Sounds[MENU_NO]->playOnce();
			}
			break;
		default:
			myButtonIsPressed = false;
			break;
		}
	}
}
