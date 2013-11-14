// EndOfWaveMenu.h
// Implements IGameMenu interface to provide a menu option at the end of a wave ...
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "GameMenuInfo.h"
#include "IGameMenu.h"
#include "EndOfWaveMenu.h"
#include "GameMenuMethods.h"

// When a button is selected this changes the price
#define PROJECTILE_COST_MODIFIER 250
#define ATTACK_COST_MODIFIER 400

enum
{
	SAVE_BUTTON,
	FINISHED_BUTTON,
	TOWER_BUTTON,
	ATTACK_BUTTON,
	PROJECTILE_BUTTON,
	HEALTH_BUTTON
};

// Static valused used whenever this menu is displayed
static int gProjectileCost = 250;
static int gAttackCost = 400;
static int gTowerCost = 500;
static int gPickleCost = 1000;

///<summary>
/// Sets up the upgrade weapon button based on the data specified ...
///</summary>	
static void setupUpgradeWeaponButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{	
	std::stringstream projectileCost;
	projectileCost << gProjectileCost;
	GameButton upWeaponButton;
	upWeaponButton.ID = PROJECTILE_BUTTON;
	GameMenuMethodsSetupButton(VEC_INIT(1.1f, -0.9f, 4.9f), levelInfo, &upWeaponButton, 1.2f, DEFAULT_TEXT_HEIGHT, projectileCost.str());
	gameMenu.ButtonList.erase(PROJECTILE_BUTTON);
	gameMenu.ButtonList[PROJECTILE_BUTTON] = upWeaponButton;
}

///<summary>
/// Sets up the upgrade attack button based on the data specified ...
///</summary>	
static void setupUpgradeAttackButton(GameMenuInfo &gameMenu, LevelInfo *levelInfo)
{	
	std::stringstream attackCost;
	attackCost << gAttackCost;
	GameButton upAttackButton;	
	upAttackButton.ID = ATTACK_BUTTON;
	GameMenuMethodsSetupButton(VEC_INIT(1.1f, -0.4f, 4.9f), levelInfo, &upAttackButton, 1.2f, DEFAULT_TEXT_HEIGHT, attackCost.str());
	gameMenu.ButtonList.erase(ATTACK_BUTTON);
	gameMenu.ButtonList[ATTACK_BUTTON] = upAttackButton;
}

///<summary>
/// Constructor that requires user to specify a texture handle for both the button and background images
///</summary>	
EndOfWaveMenu::EndOfWaveMenu(LevelInfo *levelInfo)
{	
	std::stringstream towerCost;
	std::stringstream pickleCost;
	towerCost << gTowerCost;
	pickleCost << gPickleCost;
	
	GameMenuMethodsSetupMenuInfo(&myGameMenu, levelInfo->MenuTexture, levelInfo->ButtonTexture, levelInfo->FocusedButtonTexture);
	
	// Define the labels that will be used in our menu and add it to our vector

	GameLabel titleLabel;
	GameMenuMethodsSetupLabel(VEC_INIT(-2.45f, -1.65f, 4.9f), 0.28f, 0.28f, 0.15f, &titleLabel, "End of Wave Upgrades:");
	myGameMenu.LabelList.push_back(titleLabel);

	GameLabel underscoreLabel;
	GameMenuMethodsSetupLabel(VEC_INIT(-2.50f, -1.6f, 4.9f), 0.36f, 0.36f, 0.15f, &underscoreLabel, "___________________________");
	myGameMenu.LabelList.push_back(underscoreLabel);

	// Define the buttons that will be used in our menu and add it to our vector
	
	GameLabel weaponLabel;
	GameLabel weaponDetails;	
	GameMenuMethodsSetupLabel(VEC_INIT(-2.4f, -1.0f, 4.9f), 0.26f, 0.26f, 0.15f, &weaponLabel,   "Projectile Damage:");
	GameMenuMethodsSetupLabel(VEC_INIT(-2.4f, -0.8f, 4.9f), 0.18f, 0.18f, 0.09f, &weaponDetails, "Increases players' projectile damage");
	myGameMenu.LabelList.push_back(weaponLabel);
	myGameMenu.LabelList.push_back(weaponDetails);
	setupUpgradeWeaponButton(myGameMenu, levelInfo);
	

	GameLabel attackLabel;
	GameLabel attackDetails;
	GameMenuMethodsSetupLabel(VEC_INIT(-2.4f, -0.5f, 4.9f), 0.26f, 0.26f, 0.15f, &attackLabel,   "Attack Damage:");
	GameMenuMethodsSetupLabel(VEC_INIT(-2.4f, -0.3f, 4.9f), 0.18f, 0.18f, 0.09f, &attackDetails, "Increases players' physical damage");
	myGameMenu.LabelList.push_back(attackLabel);
	myGameMenu.LabelList.push_back(attackDetails);
	setupUpgradeAttackButton(myGameMenu, levelInfo);
	
	GameLabel towerLabel;
	GameLabel towerDetails;
	GameButton upTowerButton;	
	upTowerButton.ID = TOWER_BUTTON;
	GameMenuMethodsSetupLabel(VEC_INIT(-2.4f, 0.0f, 4.9f), 0.26f, 0.26f, 0.15f, &towerLabel,   "Tower Damage:");
	GameMenuMethodsSetupLabel(VEC_INIT(-2.4f, 0.2f, 4.9f), 0.18f, 0.18f, 0.09f, &towerDetails, "Increases all Tower damage");
	GameMenuMethodsSetupButton(VEC_INIT(1.1f, 0.1f, 4.9f), levelInfo, &upTowerButton, 1.2f, DEFAULT_TEXT_HEIGHT, towerCost.str());
	myGameMenu.ButtonList[TOWER_BUTTON] = upTowerButton;
	myGameMenu.LabelList.push_back(towerLabel);
	myGameMenu.LabelList.push_back(towerDetails);
	
	GameLabel healthLabel;
	GameLabel healthDetails;
	GameButton upHealthButton;	
	upHealthButton.ID = HEALTH_BUTTON;
	GameMenuMethodsSetupLabel(VEC_INIT(-2.4f, 0.5f, 4.9f), 0.26f, 0.26f, 0.15f, &healthLabel,   "Pickle Health:");
	GameMenuMethodsSetupLabel(VEC_INIT(-2.4f, 0.7f, 4.9f), 0.18f, 0.18f, 0.09f, &healthDetails, "Recovers Pickle Health Points");
	GameMenuMethodsSetupButton(VEC_INIT(1.1f, 0.6f, 4.9f), levelInfo, &upHealthButton, 1.2f, DEFAULT_TEXT_HEIGHT, pickleCost.str());
	myGameMenu.ButtonList[HEALTH_BUTTON] = upHealthButton;
	myGameMenu.LabelList.push_back(healthLabel);
	myGameMenu.LabelList.push_back(healthDetails);
	
	GameButton saveButton;	
	saveButton.ID = SAVE_BUTTON;
	GameMenuMethodsSetupButton(VEC_INIT(-2.2f, 1.3f, 4.9f), levelInfo, &saveButton, DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, " Save Game ");
	myGameMenu.ButtonList[SAVE_BUTTON] = saveButton;

	GameButton finishedButton;	
	finishedButton.ID = FINISHED_BUTTON;
	GameMenuMethodsSetupButton(VEC_INIT(0.2f, 1.3f, 4.9f), levelInfo, &finishedButton, DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, " Finished ");
	myGameMenu.ButtonList[FINISHED_BUTTON] = finishedButton;

	// Define the verticies our background quad object that we'll use to display the menu (the rest of the setup was done in SetupMenuInfo above)
	myGameMenu.backgroundQuad.verticies[0] = VEC_INIT(-2.5f, -1.7f, 5.0f);
	myGameMenu.backgroundQuad.verticies[1] = VEC_INIT( 2.5f, -1.7f, 5.0f);
	myGameMenu.backgroundQuad.verticies[2] = VEC_INIT( 2.5f,  1.7f, 5.0f);
	myGameMenu.backgroundQuad.verticies[3] = VEC_INIT(-2.5f,  1.7f, 5.0f);	
}

///<summary>
/// Destructor, here we clean up our buttons :)
///</summary>	
EndOfWaveMenu::~EndOfWaveMenu()
{
}

///<summary>
/// Runs a menu, and makes changes to the LevelInfo struct based on the menu selection...
///</summary>	
void  EndOfWaveMenu::executeMenu(LevelInfo *levelInfo)
{
	bool doneLooping = false;
	CONTROLLER_STATUS status;

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
				GameMenuMethodsUpdateClickedButtonID(0, &myGameMenu, levelInfo);
				switch(myGameMenu.ClickedButtonID)
				{
				case FINISHED_BUTTON:
					levelInfo->Sounds[MENU_YES]->playOnce();
					doneLooping = true;
					break;
				case PROJECTILE_BUTTON:
					if(levelInfo->CurrentUpgradePoints >= gProjectileCost)
					{
						levelInfo->Sounds[MENU_YES]->playOnce();
						levelInfo->CurrentUpgradePoints -= gProjectileCost;
						levelInfo->CurrentPlayerBulletDamage += 1.0f;
						gProjectileCost += PROJECTILE_COST_MODIFIER;						
						setupUpgradeWeaponButton(myGameMenu, levelInfo);
					}
					else
					{
						levelInfo->Sounds[MENU_NO]->playOnce();
					}
					break;
					case ATTACK_BUTTON:
					if(levelInfo->CurrentUpgradePoints >= gAttackCost)
					{
						levelInfo->Sounds[MENU_YES]->playOnce();
						levelInfo->CurrentUpgradePoints -= gAttackCost;
						levelInfo->CurrentPlayerRepelDamage += 1.0f;
						gAttackCost += ATTACK_COST_MODIFIER;						
						setupUpgradeAttackButton(myGameMenu, levelInfo);
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

			GameMenuMethodsRenderMenu(0, &myGameMenu, levelInfo);
		}
		//TODO: We need a dialog that informs the user that the controller is not responding...
	}
}

///<summary>
/// Allows a particular button in a menu to be executed
///</summary>
void EndOfWaveMenu::clickButton(LevelInfo *levelInfo, int buttonID)
{
}
