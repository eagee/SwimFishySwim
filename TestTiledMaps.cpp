// TestTiledMaps.cpp
// Provides implementation of testObserverPattern.h definition
// Author(s): Eagan Rackley
#include "globals.h"
#include "SFSHeaders.h"
#include "ITest.h"
#include "TiledMaps.h"
#include "TestTiledMaps.h"

 //<summary>
/// Initializes an instance of the ObserverPatternTest object
///</summary>
TestTiledMaps::TestTiledMaps()
{
	
}
      
///<summary>
/// Disposes of an instance of the ObserverPatternTest object
///</summary>
TestTiledMaps::~TestTiledMaps()
{
	
}

// This is used for doing a simple animation test...
void setupAnimation(TiledMaps *map)
{

}


///<summary>
/// Executes the ObserverPattern Test Case
///</summary>
bool TestTiledMaps::executeTest()
{	
	bool returnValue = true;
	
	printf("Executing Tiledmaps test 1:\n");
	
	printf("LevelOne.sfs:\n");SLEEP(1000000);
	if(myTileMap.loadTileMap(ASSET_PATH("/assets/map/LevelOne.sfs")) == true)
	{		
	
		// First we'll test loading ...
		// Then we'll test building a vertex array ...
		// Then we'll test rendering the vertex array ...
		/*
		// Set the minimum boundary for our map. This would correspond to 0,0 in a 2d pixel based engine...
		// The Z value is currently ignored, but could be modified in the future for scaling...
		myTileMap.setMinimumBoundary((Vector){-11.0f, -11.0f, 100.0f});
		
		// Translate our tilemap
		myTileMap.translateMap(TILEMAP_LAYER_A, 	(Vector){-11.0f, -11.0f, 20.0f});
		myTileMap.translateMap(TILEMAP_LAYER_B, 	(Vector){-11.0f, -11.0f, 19.99f});
		myTileMap.translateMap(TILEMAP_LAYER_C, 	(Vector){-11.0f, -11.0f, 19.98f});		
		myTileMap.translateMap(TILEMAP_LAYER_D, 	(Vector){-11.0f, -11.0f, 19.97f});		
		
		printf("Performing Animation Tasks:\n");SLEEP(1000000);
		for(int index = 0; index < 200; index++)
		{
			myTileMap.velocityTask();
			myTileMap.animationTask(TILEMAP_LAYER_A);
		}
		printf("Checking for problems with tilemap collision for map:\n");SLEEP(1000000);
		
		// Test collision where we know a collision should exist
		bool collisionTestOne = false;		
		Vector mapCollisionOne = {(19.0f * TILE_QUAD_WIDTH), (20.0f * TILE_QUAD_HEIGHT), 0.0f};		
		collisionTestOne = myTileMap.checkForCollisionAt(mapCollisionOne);		
		if(collisionTestOne == true) 
		{
			returnValue = false;
			printf("\nCollision Test One Failed!!!\n"); SLEEP(1000000);
		}
		
		// Test collision where we know a collision should not exist
		bool collisionTestTwo = false;		
		Vector mapCollisionTwo = {(29.0f * TILE_QUAD_WIDTH), (21.0f * TILE_QUAD_HEIGHT), 0.0f};		
		collisionTestTwo = myTileMap.checkForCollisionAt(mapCollisionTwo);
		if(collisionTestTwo == false)
		{
			returnValue = false;		
			printf("\nCollision Test Two Failed!!!\n"); SLEEP(1000000);
		}
		
		printf("Testing Rendering Engine:\n");
		myTileMap.assignTextures(1, 1, 1, 1, 1);
		
		printf("Testing pathfinding deprecated... needs updating!:\n");SLEEP(1000000);
		//Vector current = {-13.0f, -13.0f, 0.0f};
		//Vector dest = {37.0f, 22.0f, 0.0f};
		//printf("Angle to travel from 13,13 to 30,30: %f", myTileMap.findAngleToDestination(current, 0.0f, dest));SLEEP(1000000);
		*/
		printf("	TEST COMPLETE\n");SLEEP(1000000);
	}
	else
	{
		returnValue = false;
		printf("	FILE LOADING FAILED\n");SLEEP(1000000);
		ASSERT(false);
	}
	
	return returnValue;
}
