// NewTowerMenu.h
// Implements IGameMenu interface to provide a menu option at the end of a wave ...
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "IProjectileWeapon.h"
#include "GameMenuInfo.h"
#include "IGameMenu.h"
#include "NewTowerMenu.h"
#include "GameMenuMethods.h"
#include "LevelMethods.h"
#include "ISprite.h"
#include "SpriteTower.h"

enum
{	
	NEW_SPITTING_TOWER_BUTTON,
	NEW_THROWING_TOWER_BUTTON,
	NEW_WIND_TOWER_BUTTON,
	ENERGIZE_TOWERS_BUTTON,
	FINISHED_BUTTON
};

// Static valused used whenever this menu is displayed
static const int gBaseSpitTowerCost = 150;
static const int gEnergizeTowersCost = 10000;
static const int gBaseStunTowerCost = 250;
static const int gBaseSlowTowerCost = 600;

///<summary>
/// Sets up the upgrade weapon button based on the data specified ...
///</summary>	
static void setupSpittingTowerButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{	
	std::stringstream spittingCost;
	spittingCost << gBaseSpitTowerCost;
	GameButton spitTowerButton;
	spitTowerButton.ID = NEW_SPITTING_TOWER_BUTTON;	
	Vector btnVector = {1.1f, -0.9f, 4.9f};
	GameMenuMethodsSetupButton(btnVector, levelInfo, &spitTowerButton, 1.2f, DEFAULT_TEXT_HEIGHT, spittingCost.str());
	gameMenu.ButtonList.erase(NEW_SPITTING_TOWER_BUTTON);
	gameMenu.ButtonList[NEW_SPITTING_TOWER_BUTTON] = spitTowerButton;
}

///<summary>
/// Sets up the upgrade attack button based on the data specified ...
///</summary>
static void setupStunTowerButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{
	std::stringstream stunCost;
	stunCost << gBaseStunTowerCost;
	GameButton stunTowerButton;
	stunTowerButton.ID = NEW_THROWING_TOWER_BUTTON;
	Vector btnVector = {1.1f, -0.4f, 4.9f};
	GameMenuMethodsSetupButton(btnVector, levelInfo, &stunTowerButton, 1.2f, DEFAULT_TEXT_HEIGHT, stunCost.str());
	gameMenu.ButtonList.erase(NEW_THROWING_TOWER_BUTTON);
	gameMenu.ButtonList[NEW_THROWING_TOWER_BUTTON] = stunTowerButton;
}

///<summary>
/// Sets up the upgrade weapon button based on the data specified ...
///</summary>
static void setupEnergizeTowerButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{
	std::stringstream cost;
	cost << gEnergizeTowersCost;
	GameButton button;
	button.ID = ENERGIZE_TOWERS_BUTTON;
	Vector btnVector = {1.1f, 0.1f, 4.9f};
	GameMenuMethodsSetupButton(btnVector, levelInfo, &button, 1.2f, DEFAULT_TEXT_HEIGHT, cost.str());
	gameMenu.ButtonList.erase(ENERGIZE_TOWERS_BUTTON);
	gameMenu.ButtonList[ENERGIZE_TOWERS_BUTTON] = button;
}

///<summary>
/// Constructor that requires user to specify a texture handle for both the button and background images
///</summary>	
NewTowerMenu::NewTowerMenu(LevelInfo *levelInfo)
{	
	std::stringstream spittingTowerCost;
	spittingTowerCost << gBaseSpitTowerCost;
	
	GameMenuMethodsSetupMenuInfo(&myGameMenu, levelInfo->MenuTexture, levelInfo->ButtonTexture, levelInfo->FocusedButtonTexture);
	
	// Define the labels that will be used in our menu and add it to our vector

	GameLabel titleLabel;
	Vector btnVector = {-2.45f, -1.65f, 4.9f};
	GameMenuMethodsSetupLabel(btnVector, 0.28f, 0.28f, 0.15f, &titleLabel, "New Tower Menu:");
	myGameMenu.LabelList.push_back(titleLabel);

	GameLabel underscoreLabel;
	Vector btnVector2 = {-2.50f, -1.6f, 4.9f};
	GameMenuMethodsSetupLabel(btnVector2, 0.36f, 0.36f, 0.15f, &underscoreLabel, "___________________________");
	myGameMenu.LabelList.push_back(underscoreLabel);

	// Define the buttons that will be used in our menu and add it to our vector
	
	GameLabel towerLabel;
	GameLabel towerDetails;	
	Vector lblVector = {-2.4f, -1.0f, 4.9f};
	GameMenuMethodsSetupLabel(lblVector, 0.26f, 0.26f, 0.15f, &towerLabel,   "Spitting Flower:");
	Vector descVector = {-2.4f, -0.8f, 4.9f};
	GameMenuMethodsSetupLabel(descVector, 0.18f, 0.18f, 0.09f, &towerDetails, "Spits bubbles at the baddies");
	myGameMenu.LabelList.push_back(towerLabel);
	myGameMenu.LabelList.push_back(towerDetails);
	setupSpittingTowerButton(myGameMenu, levelInfo);

	GameLabel stunLabel;
	GameLabel stunDetails;
	Vector lblVector2 = {-2.4f, -0.5f, 4.9f};
	GameMenuMethodsSetupLabel(lblVector2, 0.26f, 0.26f, 0.15f, &stunLabel,   "Stun Flower:");
	Vector descVector2 = {-2.4f, -0.3f, 4.9f};
	GameMenuMethodsSetupLabel(descVector2, 0.18f, 0.18f, 0.09f, &stunDetails, "Temorarily stuns baddies");
	myGameMenu.LabelList.push_back(stunLabel);
	myGameMenu.LabelList.push_back(stunDetails);
	setupStunTowerButton(myGameMenu, levelInfo);

	GameLabel nrgizeLabel;
	GameLabel nrgizeDetails;
	Vector lblVector3 = {-2.4f,  0.0f, 4.9f};
	GameMenuMethodsSetupLabel(lblVector, 0.26f, 0.26f, 0.15f, &nrgizeLabel,   "Energize Flowers:");
	Vector descVector3 = {-2.4f,  0.2f, 4.9f};
	GameMenuMethodsSetupLabel(descVector, 0.18f, 0.18f, 0.09f, &nrgizeDetails, "Infuse flowers with pickle energy");
	myGameMenu.LabelList.push_back(nrgizeLabel);
	myGameMenu.LabelList.push_back(nrgizeDetails);
	setupEnergizeTowerButton(myGameMenu, levelInfo);

	GameButton finishedButton;	
	finishedButton.ID = FINISHED_BUTTON;
	Vector btnFinVector = {0.2f, 1.3f, 4.9f};
	GameMenuMethodsSetupButton(btnFinVector, levelInfo, &finishedButton, DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, " Finished ");
	myGameMenu.ButtonList[FINISHED_BUTTON] = finishedButton;

	// Define the verticies our background quad object that we'll use to display the menu (the rest of the setup was done in SetupMenuInfo above)
	myGameMenu.backgroundQuad.verticies[0].x = -2.5f;
	myGameMenu.backgroundQuad.verticies[0].y = -1.7f;
	myGameMenu.backgroundQuad.verticies[0].z =  5.0f;
	myGameMenu.backgroundQuad.verticies[1].x =  2.5f;
	myGameMenu.backgroundQuad.verticies[1].y = -1.7f;
	myGameMenu.backgroundQuad.verticies[1].z =  5.0f;
	myGameMenu.backgroundQuad.verticies[2].x =  2.5f;
	myGameMenu.backgroundQuad.verticies[2].y =  1.7f;
	myGameMenu.backgroundQuad.verticies[2].z =  5.0f;
	myGameMenu.backgroundQuad.verticies[3].x = -2.5f;
	myGameMenu.backgroundQuad.verticies[3].y =  1.7f;
	myGameMenu.backgroundQuad.verticies[3].z =  5.0f;
	
/*
	GameLabel attackLabel;
	GameLabel attackDetails;
	GameMenuMethodsSetupLabel((Vector){-2.4f, -0.5f, 4.9f}, 0.26f, 0.26f, 0.15f, &attackLabel,   "Attack Damage:");
	GameMenuMethodsSetupLabel((Vector){-2.4f, -0.3f, 4.9f}, 0.18f, 0.18f, 0.09f, &attackDetails, "Increases players' physical damage");
	myGameMenu.LabelList.push_back(attackLabel);
	myGameMenu.LabelList.push_back(attackDetails);
	setupUpgradeAttackButton(myGameMenu, levelInfo);
	
	GameLabel towerLabel;
	GameLabel towerDetails;
	GameButton upTowerButton;	
	upTowerButton.ID = TOWER_BUTTON;
	GameMenuMethodsSetupLabel((Vector){-2.4f, 0.0f, 4.9f}, 0.26f, 0.26f, 0.15f, &towerLabel,   "Tower Damage:");
	GameMenuMethodsSetupLabel((Vector){-2.4f, 0.2f, 4.9f}, 0.18f, 0.18f, 0.09f, &towerDetails, "Increases all Tower damage");
	GameMenuMethodsSetupButton((Vector){1.1f, 0.1f, 4.9f}, levelInfo, &upTowerButton, 1.2, DEFAULT_TEXT_HEIGHT, towerCost.str());
	myGameMenu.ButtonList[TOWER_BUTTON] = upTowerButton;
	myGameMenu.LabelList.push_back(towerLabel);
	myGameMenu.LabelList.push_back(towerDetails);

	*/
}

///<summary>
/// Destructor, here we clean up our buttons :)
///</summary>	
NewTowerMenu::~NewTowerMenu()
{
}

///<summary>
/// Runs a menu, and makes changes to the LevelInfo struct based on the menu selection...
///</summary>	
void  NewTowerMenu::executeMenu(LevelInfo *levelInfo)
{
	bool doneLooping = false;
	CONTROLLER_STATUS status;	

	Vector location;

	while(doneLooping == false)
	{
		// This is our input code, and should always be run
		levelInfo->Controllers->updateControllerStates();
		status = levelInfo->Controllers->checkControllerStatus(0);
		if(status == CONTROLLER_READY)
		{

			if(levelInfo->Controllers->checkButtonPress(0, WII_BUTTON_PLUS) == true)
			{
#ifdef PLATFORM_IS_WII
				_break();
#endif
			}
			
			bool menuButtonPressed = levelInfo->Controllers->checkButtonPress(0, WII_BUTTON_A);

			if(levelInfo->Controllers->checkButtonPress(0, WII_BUTTON_B) == true)
			{
				// if A and B are pressed at the same time, they want to exit cest menu...
				levelInfo->Sounds[MENU_YES]->playOnce();
				doneLooping = true;
			}
			
			// Check for the A button to be pressed
			if(myGameMenu.CurrentButtonState == BUTTON_RELEASED)
			{
				myGameMenu.CurrentButtonState = BUTTON_FREE;
			}
			else if(myGameMenu.CurrentButtonState == BUTTON_FREE)
			{
				myGameMenu.ClickedButtonID = NO_BUTTON_CLICKED;
				if(menuButtonPressed == true)
			    {
					myGameMenu.CurrentButtonState = BUTTON_PRESSED;
					GameMenuMethodsUpdateClickedButtonID(0, &myGameMenu, levelInfo);
				}
			}			
			// Once A is pressed, check for it to be released, then once it's released check to see if we were inside
			// a set of button coordinates...
			else if((myGameMenu.CurrentButtonState == BUTTON_PRESSED) && (menuButtonPressed == false))
			{
				SpriteTower *newTower = new SpriteTower();
				GameMenuMethodsUpdateClickedButtonID(0, &myGameMenu, levelInfo);
				switch(myGameMenu.ClickedButtonID)
				{
				case FINISHED_BUTTON:
					levelInfo->Sounds[MENU_YES]->playOnce();
					doneLooping = true;
					break;
				case NEW_SPITTING_TOWER_BUTTON:
					// Add a new throwing tower to our level (we'll have to find out how many towers we can actually have)
					levelInfo->Sounds[MENU_YES]->playOnce();
					doneLooping = true;
					levelInfo->CurrentUpgradePoints -= gBaseSpitTowerCost;
					location = levelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
					location.x =-location.x; location.y =-location.y;
					LevelMethodsSetupSprite(levelInfo->Models[PRIMARY_FISH_MODEL], newTower, ATTACK_TYPE_GUN, location.x, location.y, SPRITE_WATER_LEVEL, 1.0f);
					newTower->setHealth(100.0f);
					newTower->setVelocities(0.0f, 0.0f, 0.0f);
					newTower->assignWeapon(levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT]);
					levelInfo->TowerSprites.push_back(*newTower);
					break;
				case NEW_THROWING_TOWER_BUTTON:
					// Add a new throwing tower to our level (we'll have to find out how many towers we can actually have)
					levelInfo->Sounds[MENU_YES]->playOnce();
					doneLooping = true;
					levelInfo->CurrentUpgradePoints -= gBaseStunTowerCost;
					location = levelInfo->TileMap->getLocation(TILEMAP_LAYER_A);
					location.x = -location.x;
					location.y = -location.y;
					LevelMethodsSetupSprite(levelInfo->Models[ATTACKING_BADDIE_FISH_MODEL], newTower, ATTACK_TYPE_GUN, location.x, location.y, SPRITE_WATER_LEVEL, 1.0f);
					newTower->setHealth(100.0f);
					newTower->setVelocities(0.0f, 0.0f, 0.0f);
					newTower->assignWeapon(levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]);
					levelInfo->TowerSprites.push_back(*newTower);
					break;
				case ENERGIZE_TOWERS_BUTTON:
					// Energize all of the existing towers
					levelInfo->Sounds[MENU_YES]->playOnce();
					doneLooping = true;
					levelInfo->ProjectileWeapons[PROJECTILE_TYPE_SPIT]->modifyWeaponExperienceBy(500);
					levelInfo->ProjectileWeapons[PROJECTILE_TYPE_THROW]->modifyWeaponExperienceBy(500);
					levelInfo->CurrentUpgradePoints -= gEnergizeTowersCost;
					break;

				default:
					break;
				}
				delete newTower;
								
				myGameMenu.CurrentButtonState = BUTTON_RELEASED;
			}

			GameMenuMethodsRenderMenu(0, &myGameMenu, levelInfo);
		}
		//TODO: We need a dialog that informs the user that the controller is not responding...
	}
}
