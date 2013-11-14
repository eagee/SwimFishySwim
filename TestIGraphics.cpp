// testObserverPattern.cpp
// Provides implementation of testObserverPattern.h definition
// Author(s): Eagan Rackley

#ifndef GLFW_DLL
	#define GLFW_DLL
#endif

#include "SFSHeaders.h"
#include "ITest.h"
#include "TestIGraphics.h"
#include "GlGraphics.h"
#include "TiledMaps.h"
#include "IModel.h"
#include "ModelMd2.h"


 /// <summary>
 /// This is the test subject object used to test the observer pattern
 /// </summary>
 TestIGraphics::TestIGraphics() 
 {
 }

 /// <summary>
 /// This is the test subject object used to test the observer pattern
 /// </summary>	
 TestIGraphics::~TestIGraphics() 
 {
 	
 }


///<summary>
/// Executes the ObserverPattern Test Case
///</summary>
bool TestIGraphics::executeTest()
{	
	bool returnValue = false;

	myGraphics = new GlGraphics();
	
	//printf("Preallocating skybox: %s \n", fileName);
	TextureHandle skyboxTexture = myGraphics->addTextureData(ASSET_PATH("//assets//texOther//skyboxA.tga"), IMAGE_TEXTURE, false);
	
	//printf("Preallocating skybox: %s \n", fileName);
	TextureHandle sceneboxTexture = myGraphics->addTextureData(ASSET_PATH("//assets//texOther//sceneboxA.tga"), IMAGE_TEXTURE, false);

	//printf("Preallocating tilemap texture: %s \n", fileName);
	TextureHandle tilemapTexture = myGraphics->addTextureData(ASSET_PATH("//assets//texOther//mapTypeA.tga"), IMAGE_TEXTURE, false);
	TextureHandle waterTexture = myGraphics->addTextureData(ASSET_PATH("//assets//tex128//waterA.tga"), IMAGE_TEXTURE, true);

	Vector mapCoords = VEC_INIT(-32.0, -40.0f, -20.5f);
	Vector waterCoords = VEC_INIT(-32.0f, -40.0f, -18.5f);

	Vector cameraTarget = VEC_INIT(0.0, 0.0f, 20.5f);

	//printf("Preallocating tilemap: %s \n", fileNme);
	TiledMaps tileMap;
	tileMap.loadTileMap(ASSET_PATH("/assets/map/LevelOne.sfs"));
	tileMap.generateLayerVertexArrays(myGraphics, TILEMAP_LAYER_A);
	tileMap.generateLayerVertexArrays(myGraphics, TILEMAP_LAYER_D);
	tileMap.assignTextures(tilemapTexture, waterTexture, tilemapTexture, tilemapTexture, tilemapTexture);
	tileMap.setMinimumBoundary(VEC_INIT(-12.0f, -12.0f, 50.0f));	

	//printf("Allocating test md2 model...\n");
	ModelMd2 model;
	ModelAnimState animState;
	model.loadFromFile(ASSET_PATH("/assets//models//traditional//hero.md2"), ANIMATION_TRADITIONAL_MD2);
	TextureHandle modelTexture = myGraphics->addTextureData(ASSET_PATH("/assets/tex128//hero.tga"), IMAGE_TEXTURE, false);
	TextureHandle modelBump = myGraphics->addTextureData(ASSET_PATH("/assets/tex128//heroBump.tga"), IMAGE_TEXTURE, false);
	model.assignTexture(modelTexture);
	model.assignBumpTexture(modelBump, modelBump);	
	model.handleAnimation(gTimeScale, animState);
	model.setAnimationType(ATTACK_A);	
	model.setModelScale(0.11f);
	model.translateModel(VEC_INIT(10.0f, 10.0f, -18.0f));
	model.buildVertexArrays(myGraphics, true);

	float zValue = 90.0f;
		
	//for(float zValue = 0.0f; zValue <= 360.0f; zValue += 0.075f)
	while(returnValue == false)
	{		
		
		// Get some input so that we can pan around our graphics...
		if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			cameraTarget.x += 0.25f;
		}
		if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			cameraTarget.x -= 0.25f;
		}
		if(glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS)
		{
			//myGraphics->adjustCameraDistance(1.0f);
			cameraTarget.y -= 0.25f;
		}
		if(glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			//myGraphics->adjustCameraDistance(-1.0f);
			cameraTarget.y += 0.25f;
		}
		if(glfwGetKey(GLFW_KEY_F1) == GLFW_PRESS)
		{
			//myGraphics->adjustCameraDistance(1.0f);
			cameraTarget.z -= 0.25f;
		}
		if(glfwGetKey(GLFW_KEY_F2) == GLFW_PRESS)
		{
			//myGraphics->adjustCameraDistance(-1.0f);
			cameraTarget.z += 0.25f;
		}
		if(glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
		{
			returnValue = true;
		}
		tileMap.translateMap(TILEMAP_LAYER_A, mapCoords);
		tileMap.translateMap(TILEMAP_LAYER_D, waterCoords);
		tileMap.animationTask(TILEMAP_LAYER_D);

		myGraphics->beginScene();				

		myGraphics->setCameraTarget(cameraTarget, zValue, 0.0f);		

		// This part of the test draws a rotating triangle
		Triangle test;
		test.texture_S[0] = 0.0f; test.texture_T[0] = 0.0f;
		test.verticies[0].x =  1.0f; test.verticies[0].y =  0.0f; test.verticies[0].z = 20.0f;
		
		test.texture_S[0] = 1.0f; test.texture_T[0] = 1.0f;
		test.verticies[1].x = -1.0f; test.verticies[1].y = -1.0f; test.verticies[1].z = 20.0f;
		
		test.texture_S[0] = 1.0f; test.texture_T[0] = 1.0f;		
		test.verticies[2].x =  1.0f; test.verticies[2].y = 1.0f; test.verticies[2].z = 20.0f;
		
		myGraphics->rotateNextObjectInScene(&ROT_INIT(1.0f, 0.0f, 0.0f, zValue));
		myGraphics->addTriangleToScene(VEC_INIT(5.0f, 5.0f, 20.0f), test, skyboxTexture);
		
		tileMap.renderLayerVertexArrays(myGraphics, TILEMAP_LAYER_A);
		
		model.handleAnimation(gTimeScale, animState);
		model.renderModel(myGraphics, animState, false);
		
		tileMap.renderLayerVertexArrays(myGraphics, TILEMAP_LAYER_D);		
		
		// This part of the test adds a triangle into the scene...
		myGraphics->addSkyboxToScene(PROJECTION_NORMAL, skyboxTexture, sceneboxTexture);
		myGraphics->endScene();	
	}

	tileMap.freeLayerVertexArrays(myGraphics, TILEMAP_LAYER_A);
	model.freeVertexArrays(myGraphics);

	myGraphics->dispose();

	delete myGraphics;
	
	return returnValue;
}