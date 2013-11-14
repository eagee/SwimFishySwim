// GameMenuMethods.cpp
// Provides delegate functionality for all IGameMenu implementation objects
// Author(s): Eagan Rackley
#include "SFSHeaders.h"
#include "GameMenuInfo.h"
#include "IAudio.h"
#include "LevelInfo.h"
#include "IProjectileWeapon.h"
#include "LevelMethods.h"
#include "GameMenuMethods.h"

static TextureHandle gPointerTexture = UNDEFINED_TEXTURE_HANDLE;
static Quad gPointerQuad;
static std::map<int, Quad> gPointerQuads;

#define BUTTON_WIDTH  0.10f
#define BUTTON_HEIGHT 0.10f

///<summary>
/// Sets up a quad by reference, excepting the location of the verticies...
///<summary>
static void setupBasicQuadDetails(Quad &quad)
{
	quad.normals[0] = VEC_INIT(0.0f, 0.0f, 1.0f);
	quad.normals[1] = VEC_INIT(0.0f, 0.0f, 1.0f);
	quad.normals[2] = VEC_INIT(0.0f, 0.0f, 1.0f);
	quad.normals[3] = VEC_INIT(0.0f, 0.0f, 1.0f);

	quad.texture_S[0] = 0.0f;
	quad.texture_T[0] = 0.0f;

	quad.texture_S[1] = 1.0f;
	quad.texture_T[1] = 0.0f;

	quad.texture_S[2] = 1.0f;
	quad.texture_T[2] = 1.0f;

	quad.texture_S[3] = 0.0f;
	quad.texture_T[3] = 1.0f;
}

///<summary>
/// Sets the texture coordinates for an enabled button...
///<summary>
static void setupEnabledButtonTexture(Quad &quad)
{
	quad.texture_S[0] = 0.0f;
	quad.texture_T[0] = 0.0f;

	quad.texture_S[1] = 0.5f;
	quad.texture_T[1] = 0.0f;

	quad.texture_S[2] = 0.5f;
	quad.texture_T[2] = 1.0f;

	quad.texture_S[3] = 0.0f;
	quad.texture_T[3] = 1.0f;
}

///<summary>
/// Sets the texture coordinates for an enabled button...
///<summary>
static void setupDisabledButtonTexture(Quad &quad)
{
	quad.texture_S[0] = 0.5f;
	quad.texture_T[0] = 0.0f;

	quad.texture_S[1] = 1.0f;
	quad.texture_T[1] = 0.0f;

	quad.texture_S[2] = 1.0f;
	quad.texture_T[2] = 1.0f;

	quad.texture_S[3] = 0.5f;
	quad.texture_T[3] = 1.0f;
}

///<summary>
/// Setup Game Menu Info Object
///<summary>
void GameMenuMethodsSetupMenuInfo(GameMenuInfo *menuInfo, TextureHandle background, TextureHandle button, TextureHandle focusedButton)
{
	// Assign our textures and assert their values...
	menuInfo->BackgroundTexture = background;
	menuInfo->ButtonTexture = button;
	menuInfo->FocusedButtonTexture = focusedButton;

	ASSERT(menuInfo->BackgroundTexture != UNDEFINED_TEXTURE_HANDLE);
	ASSERT(menuInfo->ButtonTexture != UNDEFINED_TEXTURE_HANDLE);
	ASSERT(menuInfo->FocusedButtonTexture != UNDEFINED_TEXTURE_HANDLE);

	setupBasicQuadDetails(menuInfo->backgroundQuad);

	menuInfo->CurrentButtonState = BUTTON_FREE;
}

///<summary>
/// Assigns a global variable used to render the pointer/cursor used in menu operations
///<summary>
void GameMenuMethodsSetupPointer(TextureHandle pointerTexture)
{
	// Assign our textures and assert their values...
	gPointerTexture = pointerTexture;

	// Setup the shape of the quad
	setupBasicQuadDetails(gPointerQuad);
	gPointerQuad.verticies[0] = VEC_INIT(-0.05f,  -0.05f, 0.0f);
	gPointerQuad.verticies[1] = VEC_INIT(0.3f, -0.05f, 0.0f);
	gPointerQuad.verticies[2] = VEC_INIT(0.3f, 0.3f, 0.0f);
	gPointerQuad.verticies[3] = VEC_INIT(-0.05f, 0.3f, 0.0f);

	// Add the quad to a map (maps are handly in case we want pointer trails in the future, plus as of this writing it's the best way to do it)
	gPointerQuads[0] = gPointerQuad;
}

///<summary>
/// Handles rendering of the quad pointer to the screen...
///<summary>
void GameMenuMethodsRenderPointer(LevelInfo *levelInfo, Vector location)
{
	ASSERT(gPointerTexture != UNDEFINED_TEXTURE_HANDLE);
	levelInfo->Graphics->addQuadsToScene(location, gPointerQuads, gPointerQuads.size(), gPointerTexture);
}

///<summary>
/// Setup a game label object
///<summary>
void GameMenuMethodsSetupLabel(Vector location, float width, float height, float spacing, GameLabel *label, std::string text)
{
	label->width = width;
	label->height = height;
	label->spacing = spacing;	
	
	label->location = location;
	location.z -= 0.001f;	
	label->text = text;
}

///<summary>
/// Setup a game button object
///<summary>
void GameMenuMethodsSetupButton(Vector location, LevelInfo *levelInfo, GameButton *button, float w, float h, std::string text)
{
	button->label.width = DEFAULT_TEXT_WIDTH;
	button->label.height = DEFAULT_TEXT_HEIGHT;
	button->label.spacing = DEFAULT_TEXT_SPACING;	
	
	setupBasicQuadDetails(button->quad);
	
	float adjustedW;
	float adjustedH;	
	float adjustedOffset = 0.0f;

	adjustedW = levelInfo->TextOutput->getTextSpacing() * text.length();
	adjustedH = levelInfo->TextOutput->getTextHeight();
	
	// If the width or height of the button is greater than the specified adjustment, then make sure we use the specified height
	if(h > adjustedH) 
	{
		adjustedH = h;
	}
	if(w > adjustedW) 
	{
		adjustedOffset = (w - adjustedW) / 2.0f;
		adjustedW = w;
	}
		
	button->quad.verticies[0] = VEC_INIT(location.x, location.y, location.z);
	button->quad.verticies[1] = VEC_INIT(location.x + adjustedW, location.y, location.z);
	button->quad.verticies[2] = VEC_INIT(location.x + adjustedW, location.y + adjustedH, location.z);
	button->quad.verticies[3] = VEC_INIT(location.x, location.y + adjustedH, location.z);	

	location.z -= 0.001f;
	location.x += adjustedOffset;
	button->label.location = location;
	button->label.text = text;
}


///<summary>
/// Setup a game button object as a square - ignorning the text settings...
///<summary>
void GameMenuMethodsSetupSquareButton(Vector location, LevelInfo *levelInfo, GameButton *button, float size, std::string text)
{
	button->label.width = DEFAULT_TEXT_WIDTH;
	button->label.height = DEFAULT_TEXT_HEIGHT;
	button->label.spacing = DEFAULT_TEXT_SPACING;

	setupBasicQuadDetails(button->quad);

	float adjustedW;
	float adjustedH;
	float adjustedOffset = 0.0f;

	// If the width or height of the button is greater than the specified adjustment, then make sure we use the specified height
	adjustedH = size;
	adjustedW = size;
	adjustedOffset = (size) / 2.0f;


	button->quad.verticies[0] = VEC_INIT(location.x, location.y, location.z);
	button->quad.verticies[1] = VEC_INIT(location.x + adjustedW, location.y, location.z);
	button->quad.verticies[2] = VEC_INIT(location.x + adjustedW, location.y + adjustedH, location.z);
	button->quad.verticies[3] = VEC_INIT(location.x, location.y + adjustedH, location.z);

	location.z -= 0.001f;
	location.x += adjustedOffset;
	button->label.location = location;
	button->label.text = text;
}


///<summary>
/// Returns a Vector with our current IR pointer location
///<summary>
static Vector getPointerLocation(int controllerID, LevelInfo *levelInfo)
{
	Vector returnValue;
	float pointerX = (levelInfo->Controllers->checkPointer(controllerID, POINTER_X) - 320);
	float pointerY = (levelInfo->Controllers->checkPointer(controllerID, POINTER_Y) - 240);
	returnValue.x = (pointerX * 0.01f);
	returnValue.y = (pointerY * 0.01f);	
	returnValue.z = 4.79f;
	return returnValue;
}

///<summary>
/// Only renders the buttons for a menu
///<summary>
void GameMethodsRenderButtonsOnly(int controllerID, GameMenuInfo *menuInfo, LevelInfo *levelInfo)
{
	std::map<int, Quad> buttonMap;

	// Get our mouse pointer information...
	Vector pointerLocation = getPointerLocation(controllerID, levelInfo);

	// Add the buttons from our gamemenuinfo object to our quad map
	std::map<int, GameButton>::iterator buttonIterator = menuInfo->ButtonList.begin();
	for (u32 idx = 0; idx < menuInfo->ButtonList.size(); idx++)
	{
		// If our pointer is within the quad, then create a new quad and make it bigger, otherwise
		// just assign it. Also, don't make it bigger if it's being clicked (the small will look like a click, then)
		ASSERT(buttonIterator != menuInfo->ButtonList.end());

		Quad buttonQuad = buttonIterator->second.quad;

		if (buttonIterator->second.enabled == ENABLED_STATE_ENABLED)
		{
			// Increase the size of our button if it has focus...
			if ((pointerLocation.x > buttonQuad.verticies[0].x) && (pointerLocation.x < buttonQuad.verticies[1].x) && (pointerLocation.y
					> buttonQuad.verticies[0].y) && (pointerLocation.y < buttonQuad.verticies[3].y) && (menuInfo->ClickedButtonID != buttonIterator->second.ID))
			{
				buttonQuad.verticies[0].x -= 0.05f;
				buttonQuad.verticies[1].x += 0.05f;
				buttonQuad.verticies[2].x += 0.05f;
				buttonQuad.verticies[3].x -= 0.05f;

				buttonQuad.verticies[0].y -= 0.05f;
				buttonQuad.verticies[1].y -= 0.05f;
				buttonQuad.verticies[2].y += 0.05f;
				buttonQuad.verticies[3].y += 0.05f;
			}
			setupEnabledButtonTexture(buttonQuad);
		}
		else if (buttonIterator->second.enabled == ENABLED_STATE_ALMOST)
		{
			int randEnabled = (rand() % 5);
			if(randEnabled <= 3)
			{
				setupDisabledButtonTexture(buttonQuad);
			}
			else
			{
				setupEnabledButtonTexture(buttonQuad);
			}
		}
		else if (buttonIterator->second.enabled == ENABLED_STATE_JUST_USED)
		{
			int randEnabled = (rand() % 3);
			if (randEnabled <= 1)
			{
				setupEnabledButtonTexture(buttonQuad);
			}
			else
			{
				setupDisabledButtonTexture(buttonQuad);
			}
		}
		else
		{
			setupDisabledButtonTexture(buttonQuad);
		}

		// Handle rendering the buttons one at a time
		buttonMap[idx] = buttonQuad;
		levelInfo->Graphics->addQuadsToScene(VEC_INIT(0.0f,0.0f,0.0f),
					buttonMap, buttonMap.size(),
					levelInfo->TowerButtonTextures[buttonIterator->second.ID]);
		buttonMap.clear();

		buttonIterator++;

	}

	// Print all of the labels for our buttons...
	std::map<int, GameButton>::iterator buttonListIterator = menuInfo->ButtonList.begin();
	for (u32 idx = 0; idx < menuInfo->ButtonList.size(); idx++)
	{
		levelInfo->TextOutput->setTextSize(buttonListIterator->second.label.width, buttonListIterator->second.label.height,
				buttonListIterator->second.label.spacing);
		levelInfo->TextOutput->renderText(buttonListIterator->second.label.location, "%s", buttonListIterator->second.label.text.c_str());
		buttonListIterator++;
	}

	// Draw our radar/mouse pointer here ...
	//levelInfo->TextOutput->renderText(pointerLocation, "^ %f,%f", pointerLocation.x, pointerLocation.y);
	levelInfo->TextOutput->renderText(pointerLocation, "^");
	GameMenuMethodsRenderPointer(levelInfo, pointerLocation);

	// Put our text back to the default size
	levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, DEFAULT_TEXT_SPACING);

	buttonMap.clear();
}


///<summary>
/// Render our menu information
///<summary>
void GameMenuMethodsRenderMenu(int controllerID, GameMenuInfo *menuInfo, LevelInfo *levelInfo)
{
	std::map<int, Quad> bgMap;
	std::map<int, Quad> buttonMap;
	bgMap[0] = menuInfo->backgroundQuad;	
	
	levelInfo->TileMap->animationTask(TILEMAP_LAYER_D);
	levelInfo->Graphics->beginScene();
	levelInfo->PlayerOneSprite->renderSprite(levelInfo->Graphics);
	if(levelInfo->GreatPickle->isDead() == false)
	{			
		levelInfo->GreatPickle->renderSprite(levelInfo->Graphics);
	}
	LevelMethodsRenderActiveBaddies(levelInfo);		
	levelInfo->ProjectileWeapon->renderProjectiles(levelInfo->Graphics);
	LevelMethodsRenderSkybox(PROJECTION_NORMAL, levelInfo->Graphics);
	levelInfo->TileMap->renderLayerVertexArrays(levelInfo->Graphics, TILEMAP_LAYER_A);				
	levelInfo->TileMap->renderLayerVertexArrays(levelInfo->Graphics, TILEMAP_LAYER_D);				
	LevelMethodsRenderActivePowerups(levelInfo);							
	levelInfo->Graphics->switchToOverheadView();
	//levelInfo->RadarPoints.push_back(LevelMethodsGetPlayerRadarPoint(levelInfo));
	
	//levelInfo->RadarPoints.push_back(LevelMethodsGetPickleRadarPoint(levelInfo));

	// Draw our current points, and the health of our pickle ;;;)
	//LevelMethodsRenderHeadsUpDisplay(levelInfo);
	//LevelMethodsDisplayPickleHeath(levelInfo);						

	// Get our mouse pointer information...
	Vector pointerLocation = getPointerLocation(controllerID, levelInfo);
	
	// Add the buttons from our gamemenuinfo object to our quad map
	std::map<int, GameButton>::iterator buttonIterator = menuInfo->ButtonList.begin();
	for(u32 idx = 0; idx < menuInfo->ButtonList.size(); idx++)
	{
		// If our pointer is within the quad, then create a new quad and make it bigger, otherwise
		// just assign it. Also, don't make it bigger if it's being clicked (the small will look like a click, then)
		ASSERT(buttonIterator != menuInfo->ButtonList.end());
		Quad buttonQuad = buttonIterator->second.quad;
		if((pointerLocation.x > buttonQuad.verticies[0].x) && (pointerLocation.x < buttonQuad.verticies[1].x) 
			&& (pointerLocation.y > buttonQuad.verticies[0].y) && (pointerLocation.y < buttonQuad.verticies[3].y)
			&& (menuInfo->ClickedButtonID != buttonIterator->second.ID))
		{
			buttonQuad.verticies[0].x -= 0.05f;
			buttonQuad.verticies[1].x += 0.05f;
			buttonQuad.verticies[2].x += 0.05f;
			buttonQuad.verticies[3].x -= 0.05f;
			
			buttonQuad.verticies[0].y -= 0.05f;
			buttonQuad.verticies[1].y -= 0.05f;
			buttonQuad.verticies[2].y += 0.05f;
			buttonQuad.verticies[3].y += 0.05f;						
		}
		buttonMap[idx] = buttonQuad;		
		buttonIterator++;
	}
	
	// Draw our radar image and all our points on the screen....
	//ASSERT(levelInfo->RadarTexture != UNDEFINED_TEXTURE_HANDLE);
	//LevelMethodsDisplayRadarImage(levelInfo, levelInfo->RadarPoints, levelInfo->RadarTexture);

	levelInfo->Graphics->addQuadsToScene(VEC_INIT(0.0f,0.0f,0.0f), bgMap, bgMap.size(), levelInfo->MenuTexture);
	levelInfo->Graphics->addQuadsToScene(VEC_INIT(0.0f,0.0f,0.0f), buttonMap, buttonMap.size(), levelInfo->ButtonTexture);

	// Print all of the labels for our buttons...
	std::map<int, GameButton>::iterator buttonListIterator = menuInfo->ButtonList.begin();		
	for(u32 idx = 0; idx < menuInfo->ButtonList.size(); idx++)
	{
		levelInfo->TextOutput->setTextSize(buttonListIterator->second.label.width, buttonListIterator->second.label.height, buttonListIterator->second.label.spacing);
		levelInfo->TextOutput->renderText(buttonListIterator->second.label.location, "%s", buttonListIterator->second.label.text.c_str());		
		buttonListIterator++;
	}

	// Print all of the labels that aren't associated with buttons now ...
	for(u32 idx = 0; idx < menuInfo->LabelList.size(); idx++)
	{
		levelInfo->TextOutput->setTextSize(menuInfo->LabelList.at(idx).width, menuInfo->LabelList.at(idx).height, menuInfo->LabelList.at(idx).spacing);
		levelInfo->TextOutput->renderText(menuInfo->LabelList.at(idx).location, "%s", menuInfo->LabelList.at(idx).text.c_str());		
	}

	// Draw our radar/mouse pointer here ...
	levelInfo->TextOutput->renderText(pointerLocation, "^");
	GameMenuMethodsRenderPointer(levelInfo, pointerLocation);

	
	//levelInfo->RadarPoints.clear();

	levelInfo->Graphics->switchBackToCameraView();

	levelInfo->Graphics->endScene();
	
	// Put our text back to the default size
	levelInfo->TextOutput->setTextSize(DEFAULT_TEXT_WIDTH, DEFAULT_TEXT_HEIGHT, DEFAULT_TEXT_SPACING);
	
	bgMap.clear();
	buttonMap.clear();
}

///<summary>
/// Gets the button index ID from the button list that matches the current pointer location.
/// Returns NO_BUTTON_HIGHLIGHTED if no buttons are active...
///<summary>
void GameMenuMethodsUpdateClickedButtonID(int controllerID, GameMenuInfo *menuInfo, LevelInfo *levelInfo)
{
	menuInfo->ClickedButtonID = NO_BUTTON_CLICKED;

	// Get our mouse pointer information...
	Vector pointerLocation = getPointerLocation(controllerID, levelInfo);
	
	// Return the first ID that matches the coordinates from theIR pointer...
	std::map<int, GameButton>::iterator buttonListIterator = menuInfo->ButtonList.begin();		
	for(u32 idx = 0; idx < menuInfo->ButtonList.size(); idx++)
	{
		// If our pointer is within the quad, then create a new quad and make it bigger, otherwise
		// just assign it.
		Quad buttonQuad = buttonListIterator->second.quad;
		if((pointerLocation.x > buttonQuad.verticies[0].x) && (pointerLocation.x < buttonQuad.verticies[1].x) 
			&& (pointerLocation.y > buttonQuad.verticies[0].y) && (pointerLocation.y < buttonQuad.verticies[3].y))
		{			
				menuInfo->ClickedButtonID = buttonListIterator->second.ID;			
		}
		buttonListIterator++;
	}	
}
