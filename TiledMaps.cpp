// TiledMaps.cpp
// Provides an object that can read a custome format tilemap, load texture data, and render those tilemaps to the screen.
// Author(s): Eagan Rackley

#include <limits.h>
#include "SFSHeaders.h"
#include "TiledMaps.h"

// Defines how many tiles will be used to draw a water column...
#define NUMBER_OF_WATER_TILES 1
#define TILE_MOTION_Z_MIN 0.0f
#define TILE_MOTION_Z_MAX 0.5f


// Used to check collision boundaries
#define IS_WITHIN_MAP_BOUNDARIES(collisionPoint) (collisionPoint.x >= 0.0f) \
	&&(collisionPoint.y >= 0.0f) \
	&&(collisionPoint.x <= (myTilemapSize.tilemapWidth - 1)) \
	&&(collisionPoint.y <= (myTilemapSize.tilemapHeight - 1))

// Defines how much we change the z value specified with height in the tilemap...
#define REDUCE_HEIGHT_BY 0.062f

// Defines a static array of angles that are used in our pathfinding code to determine the angle a sprite should travel
// based on the tile that's closest to its' target destination.
#define NUMBER_OF_PATHFINDING_NODES 8
static const float gAnglesByPathfindingNode[NUMBER_OF_PATHFINDING_NODES] = {315.0f, 0.0f, 45.0f, 270.0f, 90.0f, 225.0f, 180.0f, 135.0f};

// Defines which squares of the pathfinding alg. scores are increased if collision is found
// with corresponding nodes we're evaluating.
static const int gCollisonScore[NUMBER_OF_PATHFINDING_NODES][4] = 	{{0, 1, 2, 3}, //0
																	{0, 1, 2, 3},  //1
																	{0, 1, 2, 4},  //2
																	{0, 1, 5, 6},  //3
																	{1, 2, 6, 7},  //4
																	{3, 5, 6, 7},  //5
																	{3, 5, 6, 7},  //6	
																	{5, 6, 7, 4}}; //7


///<summary>
/// Defines a set of heuristic data we use for pathfinding in the tilemap
///</summary>
typedef struct tagTilemapHeuristic
{
	int indexIntoTilemap;
	int score;	
	int x;
	int y;	
	bool considered;
		
	// Initializes the tilemap index based on the parameters specified...
	void initialize(int tilemapWidth, int tileX, int tileY)
	{
		x = tileX;
		y = tileY;		
		indexIntoTilemap = (tilemapWidth * y) + (x);
		score = 0;		
		considered = true;
	}	
} TilemapHeuristic;

// OPERATIONS:

///<summary>
/// Constructor for md2 map object, 'will accept a list of md2 objects to load ...
///</summary>
TiledMaps::TiledMaps()
{
	myTileTexture = UNDEFINED_TEXTURE_HANDLE;	
	myWaterTexture = UNDEFINED_TEXTURE_HANDLE;
	myWaterLightMap = UNDEFINED_TEXTURE_HANDLE;
	myAnimationTimer = 0.0f;
	myWaterAnimationCounter = 0;
	myCurrentWaterNormalMap = 0;
	myAnimationsPerSecond = 16.0f;
	myShakeTimer = 0.0;
	myShakeMagnitude = 0.0f;
}
 
///<summary>
/// Destructor for md2 objects - should handle all the cleanup ...
///</summary>
TiledMaps::~TiledMaps() 
{		
}

// Initializes the LayerMotion struct

void initWaterMovment(LayerMotion &waterMovement, Vector motionStep, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
{
	waterMovement.x = 0.000f;
	waterMovement.y = 0.000f;
	waterMovement.zVertsZeroAndThree = zMin;
	waterMovement.zVertsOneAndTwo = zMax;
	waterMovement.xVel = 0;
	waterMovement.yVel = 0;
	waterMovement.zVel = 0;
	
	waterMovement.xStep =  motionStep.x;	
	waterMovement.xMin =   -xMin;		
	waterMovement.xMax =   -xMax;

	waterMovement.yStep =  motionStep.y;	
	waterMovement.yMin =   -yMin;		
	waterMovement.yMax =   -yMax;
	
	waterMovement.zStep = motionStep.z;
	waterMovement.zMin = zMin;
	waterMovement.zMax = -zMax;				
}

	
// Peforms any animations we might need in the layer motion struct...
void animateWaterMovement(TilemapLayer &waterLayer)
{
	// Handle the x and y movement of the entire layer...
	waterLayer.layerMotion.xVel += waterLayer.layerMotion.xStep;
	waterLayer.layerMotion.yVel += waterLayer.layerMotion.yStep;	

	waterLayer.layerMotion.x += waterLayer.layerMotion.xVel;
	waterLayer.layerMotion.y += waterLayer.layerMotion.yVel;
	
	if(waterLayer.layerMotion.x >= waterLayer.layerMotion.xMax)
	{
		if(waterLayer.layerMotion.xStep > 0) waterLayer.layerMotion.xStep = -waterLayer.layerMotion.xStep;
	}
	else if(waterLayer.layerMotion.x <= waterLayer.layerMotion.xMin)
	{
		if(waterLayer.layerMotion.xStep < 0) waterLayer.layerMotion.xStep = -waterLayer.layerMotion.xStep;
	}

	if(waterLayer.layerMotion.y >= waterLayer.layerMotion.yMax)
	{
		if(waterLayer.layerMotion.yStep > 0) waterLayer.layerMotion.yStep = -waterLayer.layerMotion.yStep;
	}
	else if(waterLayer.layerMotion.y <= waterLayer.layerMotion.yMin)
	{
		if(waterLayer.layerMotion.yStep < 0) waterLayer.layerMotion.yStep = -waterLayer.layerMotion.yStep;
	}

	// Increase our current velocity by the current step and have move our two z values in opposite directions
	//waterLayer.layerMotion.zVel += waterLayer.layerMotion.zStep;
	//waterLayer.layerMotion.z += waterLayer.layerMotion.zVel;
	waterLayer.layerMotion.z = -0.1f;
	//waterLayer.layerMotion.zVertsZeroAndThree += waterLayer.layerMotion.zVel;
	//waterLayer.layerMotion.zVertsOneAndTwo -= waterLayer.layerMotion.zVel;

	// If one of our z values goes beyond the intended goal, change this value for everyone
	if (waterLayer.layerMotion.z >= waterLayer.layerMotion.zMax)
	{
		if (waterLayer.layerMotion.zStep > 0) waterLayer.layerMotion.zStep = -waterLayer.layerMotion.zStep;
	}
	else if (waterLayer.layerMotion.z <= waterLayer.layerMotion.zMin)
	{
		if (waterLayer.layerMotion.zStep < 0) waterLayer.layerMotion.zStep = -waterLayer.layerMotion.zStep;
	}

	//waterLayer.quadVertexArray.Verticies[0].z = waterLayer.layerMotion.zVertsZeroAndThree;
	//waterLayer.quadVertexArray.Verticies[1].z = waterLayer.layerMotion.zVertsOneAndTwo;
	//waterLayer.quadVertexArray.Verticies[2].z = waterLayer.layerMotion.zVertsOneAndTwo;
	//waterLayer.quadVertexArray.Verticies[3].z = waterLayer.layerMotion.zVertsZeroAndThree;


	/*
	u32 totalVerticies = (waterLayer.quadVertexArray.VertexArraySize * 4);

	// Save the bottom lefmost two verticies...
	f32 leftmostZValues[4];
	u32 index = 0;

	leftmostZValues[0] = waterLayer.quadVertexArray.Verticies[0].z;
	leftmostZValues[1] = waterLayer.quadVertexArray.Verticies[1].z;
	leftmostZValues[2] = waterLayer.quadVertexArray.Verticies[2].z;
	leftmostZValues[3] = waterLayer.quadVertexArray.Verticies[3].z;

	// Go through our vertex array and update the z values for each vertex!
	for(index = 0; index < (totalVerticies - 4); index += 4)
	{
		waterLayer.quadVertexArray.Verticies[index    ].z = waterLayer.quadVertexArray.Verticies[(index+4)    ].z ;
		waterLayer.quadVertexArray.Verticies[index + 1].z = waterLayer.quadVertexArray.Verticies[(index+4) + 1].z;

		waterLayer.quadVertexArray.Verticies[index + 2].z = waterLayer.quadVertexArray.Verticies[(index+4) + 2].z ;
		waterLayer.quadVertexArray.Verticies[index + 3].z = waterLayer.quadVertexArray.Verticies[(index+4) + 3].z;
	}
	//index += 4;
	waterLayer.quadVertexArray.Verticies[index    ].z = leftmostZValues[0];
	waterLayer.quadVertexArray.Verticies[index + 1].z = leftmostZValues[1];
	waterLayer.quadVertexArray.Verticies[index + 2].z = leftmostZValues[2];
	waterLayer.quadVertexArray.Verticies[index + 3].z = leftmostZValues[3];
	*/

	// Flush the last values from the cache since they've changed...
#ifdef PLATFORM_IS_WII
	DCFlushRange(waterLayer.quadVertexArray.Verticies, (waterLayer.quadVertexArray.VertexStride * (waterLayer.quadVertexArray.VertexArraySize * 4)));
#endif

}


///<summary>
/// Allows a user to specify a texture handle returned from the graphics subsystem
///</summary>	
void TiledMaps::assignTextures(TextureHandle tileTexture, TextureHandle waterTexture, TextureHandle cloudTexture, TextureHandle waterLightMap, TextureHandle waterNormalMap)
{
	ASSERT(tileTexture != UNDEFINED_TEXTURE_HANDLE);
	ASSERT(waterTexture != UNDEFINED_TEXTURE_HANDLE);
	ASSERT(cloudTexture != UNDEFINED_TEXTURE_HANDLE);
	ASSERT(waterLightMap != UNDEFINED_TEXTURE_HANDLE);
	ASSERT(waterNormalMap != UNDEFINED_TEXTURE_HANDLE);
	
	myTileTexture = tileTexture;
	myWaterTexture = waterTexture;
	myWaterLightMap = waterLightMap;
	myCloudTexture = cloudTexture;
	myWaterNormalMaps.push_back(waterNormalMap);
}	

///<summary>
/// Adds a texture into the list of normal mapped textures used by the system
///</summary>	
void TiledMaps::addWaterNormalMap(TextureHandle waterNormalMap)
{
	myWaterNormalMaps.push_back(waterNormalMap);
}

///<summary>
/// Clears all existing references to water normal maps
///</summary>	
void TiledMaps::clearWaterNormalMaps()
{
	myWaterNormalMaps.clear();
}

///<summary>
/// Local procedure to read out the header from a file handle pointing to a tilemap
///</summary>	
static bool readFileHeader(FILE *mapFileHandle, TilemapSize *mapSize)
{
	bool returnValue = false;
	
	ASSERT(mapFileHandle != NULL);
	ASSERT(mapSize != NULL);
	
	// Read out our tilemap size data, and handle any endian conversions we might need to make
	size_t sizeRead = fread(&mapSize->tilemapWidth, 1, sizeof(short), mapFileHandle);
	if(sizeRead == sizeof(short))
	{
		sizeRead = fread(&mapSize->tilemapHeight, 1, sizeof(short), mapFileHandle);
		if(sizeRead == sizeof(short))
		{
			size_t sizeRead = fread(&mapSize->graphicWidth, 1, sizeof(short), mapFileHandle);
			if(sizeRead == sizeof(short))
			{
				size_t sizeRead = fread(&mapSize->graphicHeight, 1, sizeof(short), mapFileHandle);
				if(sizeRead == sizeof(short))
				{
					returnValue = true;
			
					#ifdef THIS_IS_A_BIG_ENDIAN_SYSTEM
					mapSize->tilemapWidth = SWAP_ENDIAN_SHORT(mapSize->tilemapWidth);
					mapSize->tilemapHeight = SWAP_ENDIAN_SHORT(mapSize->tilemapHeight);	
					mapSize->graphicWidth = SWAP_ENDIAN_SHORT(mapSize->graphicWidth);
					mapSize->graphicHeight = SWAP_ENDIAN_SHORT(mapSize->graphicHeight);	
					#endif
					//debugPrint("Tilemap : W: %d\n H: %d\n GW: %d\n GH: %d\n", 
					//	mapSize->tilemapWidth, mapSize->tilemapHeight, mapSize->graphicWidth, mapSize->graphicHeight);								
					//usleep(1000000);
				}
			}
		}
	}
	
	return returnValue;
}

///<summary>
/// Local procedure to read out the layer data from a file handle pointing to a tilemap
///</summary>	
static bool readTilemapLayer(FILE *mapFileHandle, TilemapSize mapSize, TilemapLayer *mapLayer)
{
	bool returnValue = true;
	int indexX;
	int indexY;
	int tilemapIndex;	
	
	ASSERT(mapFileHandle != NULL);
	ASSERT(mapLayer != NULL);
	
	// Loop through all of the X values in our tilemap layer and assign them to the tilemap layer
	for(indexY = 0; indexY < mapSize.tilemapHeight; indexY++)
	{
		for(indexX = 0; indexX < mapSize.tilemapWidth; indexX++)
		{						
			// Create a new TextureIndex object for storing the x/y coordinates in our layer
			TextureIndex newIndex;
			
			// Add that TextureIndex to our list of tile indicies
			mapLayer->tiles.push_back(newIndex);
			
			// Get our index into the tilemap x/y coordinates
			tilemapIndex = (mapSize.tilemapWidth * indexY) + indexX;			
			
			size_t sizeRead = fread(&mapLayer->tiles.at(tilemapIndex).xIndex, 1, sizeof(int), mapFileHandle);
			if(sizeRead == sizeof(int))
			{				
				#ifdef THIS_IS_A_BIG_ENDIAN_SYSTEM
				SWAP_ENDIAN_LONG(mapLayer->tiles.at(tilemapIndex).xIndex);		
				#endif				
			}
			else
			{
				returnValue = false;
			}
		}
	}
	
	// Loop through all of the Y values in our tilemap layer and assign them to the tilemap layer
	for(indexY = 0; indexY < mapSize.tilemapHeight; indexY++)
	{
		for(indexX = 0; indexX < mapSize.tilemapWidth; indexX++)
		{
			// Get our index into the tilemap x/y coordinates
			tilemapIndex = (mapSize.tilemapWidth * indexY) + indexX;
			
			// Read out the Y value and index into our tilemap layer to assign it...
			size_t sizeRead = fread(&mapLayer->tiles.at(tilemapIndex).yIndex, 1, sizeof(int), mapFileHandle);
			if(sizeRead == sizeof(int))
			{				
				#ifdef THIS_IS_A_BIG_ENDIAN_SYSTEM
				SWAP_ENDIAN_LONG(mapLayer->tiles.at(tilemapIndex).yIndex);		
				#endif
			}
			else
			{
				returnValue = false;
			}
		}
		//debugPrint("Tile index: %d : (%d, %d) Y: %d\n", tilemapIndex, indexX, indexY, mapLayer->tiles.at(tilemapIndex).yIndex); usleep(1000);//usleep(1000000);
	}
	
	// Read out our wrap edges flag ...
	fread(&mapLayer->mapWrapEdges, 1, sizeof(bool), mapFileHandle);
		
	return returnValue;
}


///<summary>
/// Local procedure to read out the collision data from a file handle pointing to a tilemap
///</summary>	
static bool readTilemapCollision(FILE *mapFileHandle, TilemapSize mapSize, CollisionData *collision)
{
	bool returnValue = true;
	int indexX;
	int indexY;
	
	ASSERT(mapFileHandle != NULL);
	ASSERT(collision != NULL);
	
	// Loop through all of the X values in our tilemap layer and assign them to the tilemap layer
	for(indexY = 0; indexY < mapSize.tilemapHeight; indexY++)
	{
		for(indexX = 0; indexX < mapSize.tilemapWidth; indexX++)
		{						
			// Create a new TextureIndex object for storing the x/y coordinates in our layer
			bool newCollisionValue;

			size_t sizeRead = fread(&newCollisionValue, 1, sizeof(bool), mapFileHandle);
			
			// Add that TextureIndex to our list of tile indicies
			collision->push_back(newCollisionValue);
			
			
			if(sizeRead != sizeof(bool))
			{				
				returnValue = false;
			}			
		}
	}

	return returnValue;
}

///<summary>
/// Local procedure to read out the Z data from a file handle pointing to a tilemap
///</summary>	
static bool readTilemapZData(FILE *mapFileHandle, TilemapSize mapSize, TilemapZValues &tileZData)
{
	bool returnValue = true;
	int indexX;
	int indexY;
	int tilemapIndex;

	ASSERT(mapFileHandle != NULL);
	ASSERT(&tileZData != NULL);
	
	// Loop through all of the X values in our tilemap layer and assign them to the tilemap layer
	for(indexY = 0; indexY < mapSize.tilemapHeight; indexY++)
	{
		for(indexX = 0; indexX < mapSize.tilemapWidth; indexX++)
		{						
			tilemapIndex = (mapSize.tilemapWidth * indexY) + indexX;
			
			// Create a new TextureIndex object for storing the x/y coordinates in our layer
			QuadZValues newZData;
						
			size_t sizeRead = fread(&newZData, 1, sizeof(QuadZValues), mapFileHandle);
			
			if(sizeRead != sizeof(QuadZValues))
			{				
				ASSERT(false);
				returnValue = false;
			}				
			
			// Add that TextureIndex to our list of tile indicies
			tileZData[tilemapIndex] = newZData;						
		}
	}

	return returnValue;
}
///<summary>
/// Updates the texture coordinates of the quad to facilitate animation...
///</summary>	
static void updateQuad(Quad *quad, TextureCoordinateSize textureSize, TextureIndex textureIndex)
{
	// Texture our tiles based on the data specified ...
	float baseTextureX = (float)(textureIndex.xIndex * textureSize.width);
	float baseTextureY = (float)(textureIndex.yIndex * textureSize.height);
	
	quad->texture_S[0] = (float)(baseTextureX + 0.00075);
	quad->texture_S[1] = (float)(baseTextureX + textureSize.width - 0.00075);
	quad->texture_S[2] = (float)(baseTextureX + textureSize.width - 0.00075);
	quad->texture_S[3] = (float)(baseTextureX + 0.00075);
			
	quad->texture_T[0] = (float)(baseTextureY + 0.00075);
	quad->texture_T[1] = (float)(baseTextureY + 0.00075);
	quad->texture_T[2] = (float)(baseTextureY + textureSize.height - 0.00075);
	quad->texture_T[3] = (float)(baseTextureY + textureSize.height - 0.00075);
}

///<summary>
/// Normalizes a calculated or precalculated normal vector
///</summary>	
/*static Vector normalize(Vector normal)
{
// Now we need to normalize our normal so the results aren't all funkified
	float combinedSquares = (normal.x * normal.x) 
							+ (normal.y * normal.y) 
							+ (normal.z * normal.z);
	float normalizeValue = sqrt(combinedSquares);
	
	if(normalizeValue == 0.0f) normalizeValue = 1.0f;
	
	normal.x = normal.x / normalizeValue;
	normal.y = normal.y / normalizeValue;
	normal.z = normal.z / normalizeValue;
	
	return normal;
}

///<summary>
/// Calculates the normal for a quad based on it's existing vector coordinates...
/// base vertex is the corner of the quad you're starting with
/// offset 1 is the next corner in a CCW direction
/// offset 2 is the next corner after offset 1 in a CCW direction
///</summary>	
static Vector calculateNormal(Quad quad, int baseVertex1, int offsetVertex1, int baseVertex2, int offsetVertex2)
{	
	Vector v1; //= quad->verticies[2] - quad->verticies[3];
	Vector v2; //= quad->verticies[1] - quad->verticies[3];
	Vector normal;
	
	// Get the two vectors we need to calculate our normal
	v1.x = quad.verticies[baseVertex1].x - quad.verticies[offsetVertex1].x;
	v1.y = quad.verticies[baseVertex1].y - quad.verticies[offsetVertex1].y;
	v1.z = quad.verticies[baseVertex1].z - quad.verticies[offsetVertex1].z;

	v2.x = quad.verticies[baseVertex2].x - quad.verticies[offsetVertex2].x;
	v2.y = quad.verticies[baseVertex2].y - quad.verticies[offsetVertex2].y;
	v2.z = quad.verticies[baseVertex2].z - quad.verticies[offsetVertex2].z;
	
	// Get the cross product of those two vectors to get our base normal...
	normal.x = (v1.y * v2.z) - (v1.z * v2.y);
	normal.y = -((v2.z * v1.x) - (v2.x * v1.z));
	normal.z = (v1.x * v2.y) - (v1.y * v2.x);
	
	normal = normalize(normal);
	
	return normal;
}*/

///<summary>
/// sets up the verticies for a specifc quad based on x, y, and z values...
///</summary>	
static void setupQuad(Quad *quad, float x, float y, QuadZValues zValues, TextureCoordinateSize textureSize, TextureIndex textureIndex)
{
	quad->texIndexX = textureIndex.xIndex;
    quad->texIndexY = textureIndex.yIndex;

	quad->verticies[0].x = x;	
	quad->verticies[0].y = y;
	quad->verticies[0].z = (-((float)zValues.topLeft * REDUCE_HEIGHT_BY));
		
	quad->verticies[1].x = x + TILE_QUAD_WIDTH;	
	quad->verticies[1].y = y;
	quad->verticies[1].z = (-((float)zValues.topRight * REDUCE_HEIGHT_BY));;
	
	quad->verticies[2].x = x + TILE_QUAD_WIDTH;	
	quad->verticies[2].y = y + TILE_QUAD_HEIGHT;
	quad->verticies[2].z = (-((float)zValues.bottomRight * REDUCE_HEIGHT_BY));;
	
	quad->verticies[3].x = x ;	
	quad->verticies[3].y = y + TILE_QUAD_HEIGHT;
	quad->verticies[3].z = (-((float)zValues.bottomLeft * REDUCE_HEIGHT_BY));;
	
	//quad->normal = calculateNormal(*quad, 0, 1, 0, 2);	
	quad->normals[0].x = (((float)zValues.brNormalX));
	quad->normals[0].y = (((float)zValues.brNormalY));
	quad->normals[0].z = (((float)zValues.brNormalZ));
	quad->normals[1].x = (((float)zValues.blNormalX));
	quad->normals[1].y = (((float)zValues.blNormalY));
	quad->normals[1].z = (((float)zValues.blNormalZ));
	quad->normals[2].x = (((float)zValues.tlNormalX));
	quad->normals[2].y = (((float)zValues.tlNormalY));
	quad->normals[2].z = (((float)zValues.tlNormalZ));
	quad->normals[3].x = (((float)zValues.trNormalX));
	quad->normals[3].y = (((float)zValues.trNormalY));
	quad->normals[3].z = (((float)zValues.trNormalZ));		
	fastVectorNormalize(&quad->normals[0]);
	fastVectorNormalize(&quad->normals[1]);
	fastVectorNormalize(&quad->normals[2]);
	fastVectorNormalize(&quad->normals[3]);

	
	/* for testing, remove this...
	quad->normals[0].x = 0.0f;
	quad->normals[0].y = 0.0f;
	quad->normals[0].z = -1.0f;
	quad->normals[1].x = 0.0f;
	quad->normals[1].y = 0.0f;
	quad->normals[1].z = -1.0f;
	quad->normals[2].x = 0.0f;
	quad->normals[2].y = 0.0f;
	quad->normals[2].z = -1.0f;
	quad->normals[3].x = 0.0f;
	quad->normals[3].y = 0.0f;
	quad->normals[3].z = -1.0f;
	*/

	updateQuad(quad, textureSize, textureIndex);	
	
	/* This is only for testing, and will lay out a single texture over the map
	float baseTextureX = (x * textureSize.width);
	float baseTextureY = (y * textureSize.height);
	quad->texture_S[0] = baseTextureX;
	quad->texture_S[1] = baseTextureX + textureSize.width;
	quad->texture_S[2] = baseTextureX + textureSize.width;
	quad->texture_S[3] = baseTextureX;
			
	quad->texture_T[0] = baseTextureY;
	quad->texture_T[1] = baseTextureY;
	quad->texture_T[2] = baseTextureY + textureSize.height;
	quad->texture_T[3] = baseTextureY + textureSize.height;
	*/
}


///<summary>
/// sets up the verticies for a specifc quad based on x, y, and z values...
///</summary>	
static void setupWaterQuads(TilemapLayer *layer, TilemapSize mapSize, float wrapSize)
{	
	int totalQuads = NUMBER_OF_WATER_TILES; // size of 152 / 4

		//TODO: Increase size of this quad to stretch off into the distance :) (maybe)
	float tilemapXOffset = 0.0f;
	float tileWidth = ((TILE_QUAD_WIDTH / totalQuads) * mapSize.tilemapWidth);
	float lastZOffset = 0.0f;
	float currentZOffset = 0.0f;
	//float tilmapZMax = 0.25f;
	//float tilemapZVelocity = (tilmapZMax / NUMBER_OF_WATER_TILES) * 4;

	// Create vertial rows so the water can have a basic "wave" pattern
	// without requiring a row for each quad...
	// (question - can we calculate normals for the waves on the fly? - is that worth doing?)
	for (int index = 0; index < totalQuads; index++)
	{
		Quad quad;

		tilemapXOffset = (index * tileWidth);

		/*currentZOffset += tilemapZVelocity;
		if (currentZOffset <= 0.0f)
			tilemapZVelocity = -tilemapZVelocity;
		if (currentZOffset >= tilmapZMax)
			tilemapZVelocity = -tilemapZVelocity;
		 */
		quad.verticies[0].x = tilemapXOffset;
		quad.verticies[0].y = 0.0f;
		quad.verticies[0].z = lastZOffset;

		quad.verticies[1].x = tilemapXOffset + (tileWidth);
		quad.verticies[1].y = 0.0f;
		quad.verticies[1].z = currentZOffset;

		quad.verticies[2].x = tilemapXOffset + (tileWidth);
		quad.verticies[2].y = (tileWidth * totalQuads);
		quad.verticies[2].z = currentZOffset;

		quad.verticies[3].x = tilemapXOffset;
		quad.verticies[3].y = (tileWidth * totalQuads);
		quad.verticies[3].z = lastZOffset;

		lastZOffset = currentZOffset;

		quad.normals[0].x = 10.0f;
		quad.normals[0].y = 10.0f;
		quad.normals[0].z = -10.0f;
		quad.normals[1].x = 10.0f;
		quad.normals[1].y = 10.0f;
		quad.normals[1].z = -10.0f;
		quad.normals[2].x = 10.0f;
		quad.normals[2].y = 10.0f;
		quad.normals[2].z = -10.0;
		quad.normals[3].x = 10.0f;
		quad.normals[3].y = 10.0f;
		quad.normals[3].z = -10.0f;

		quad.texture_S[0] = 0.0f;
		quad.texture_S[1] = wrapSize;
		quad.texture_S[2] = wrapSize;
		quad.texture_S[3] = 0.0f;

		quad.texture_T[0] = 0.0f;
		quad.texture_T[1] = 0.0f;
		quad.texture_T[2] = wrapSize;
		quad.texture_T[3] = wrapSize;

		// Add this quad to our map
		layer->quads[(index * (int)tileWidth)] = quad;
	}
}


///<summary>
/// Sets the default values required for processing a set of surrounding tiles
///</summary>		
static void setupSurroundingTiles(TilemapHeuristic *surroundingTiles, TilemapSize mapSize, int startingX, int startingY, int tileOffset)
{
	// Initialization of Top Row heuristic data	
	surroundingTiles[0].initialize(mapSize.tilemapWidth, (startingX - tileOffset), (startingY - tileOffset));
	surroundingTiles[1].initialize(mapSize.tilemapWidth, (startingX    ), (startingY - tileOffset));
	surroundingTiles[2].initialize(mapSize.tilemapWidth, (startingX + tileOffset), (startingY - tileOffset));
	// Initialization of Middle Row heuristic data	
	surroundingTiles[3].initialize(mapSize.tilemapWidth, (startingX - tileOffset), (startingY));
	surroundingTiles[4].initialize(mapSize.tilemapWidth, (startingX + tileOffset), (startingY));
	// Initialization of Bottom Row heuristic data	
	surroundingTiles[5].initialize(mapSize.tilemapWidth, (startingX - tileOffset), (startingY + tileOffset));
	surroundingTiles[6].initialize(mapSize.tilemapWidth, (startingX    ), (startingY + tileOffset));
	surroundingTiles[7].initialize(mapSize.tilemapWidth, (startingX + tileOffset), (startingY + tileOffset));
}

///<summary>
/// Checks for collision in the surrounding tiles and updates scores accordingly
///</summary>		
static void updateCollisionForSurroundingTiles(TilemapHeuristic *surroundingTiles, CollisionLayer *collisionLayer, int currentIndex)
{
	// If this isn't a collision tile then we can see if it has the lowest score and is the best tile for the job...
	bool collision = false;
	
	if((uint)surroundingTiles[currentIndex].indexIntoTilemap < collisionLayer->tilemapCollision.size())
	{		
		collision = collisionLayer->tilemapCollision.at(surroundingTiles[currentIndex].indexIntoTilemap);								
	}
	if(collision == true)
	{					
		// If we did have a collision we need to add to the score of the 2 adjacent tiles using gCollisionScore
		// so that we turn away from the collision...
		surroundingTiles[currentIndex].considered = false;				
		surroundingTiles[gCollisonScore[currentIndex][0]].score += 125;
		surroundingTiles[gCollisonScore[currentIndex][1]].score += 125;
		surroundingTiles[gCollisonScore[currentIndex][2]].score += 125;
		surroundingTiles[gCollisonScore[currentIndex][3]].score += 125;
	}
}

///<summary>
/// Checks the temporary collision for the surrounding tiles and updates accordingly...
/// tempCollisions is the table of temporary collision indicies...
/// currentIndex is the references into surroundingTiles
/// skipCollisionID is an ID that will disable an entry in tempCollisions...
///</summary>		
static void updateTempCollisionForTiles(TilemapHeuristic *surroundingTiles, TempCollisionTable *tempCollisions, int skipCollisionID, int currentIndex)
{
	// If this isn't a collision tile then we can see if it has the lowest score and is the best tile for the job...
	bool collision = false;

	// Iterator through the temp collision table to see if there's a potential collision pending
	TempCollisionIterator tempIterator = tempCollisions->begin();
	for(u32 idx = 0; idx < tempCollisions->size(); idx++)
	{
		// If our key value is not equal to the tempCollisionID (which we don't want to use to avoid self-collisions)
		if(tempIterator->first != skipCollisionID)
		{			
			// Check for a range of tiles near an object...			
			//if(surroundingTiles[currentIndex].indexIntoTilemap == tempIterator->second)
			if(abs(surroundingTiles[currentIndex].indexIntoTilemap - tempIterator->second) <= 2)
			{
				collision = true;
			}
		}
		tempIterator++;
	}
	if(collision == true)
	{					
		// If we did have a collision we need to add to the score of the 2 adjacent tiles using gCollisionScore
		// so that we turn away from the collision...
		surroundingTiles[currentIndex].considered = false;
		surroundingTiles[currentIndex].score += 125;
		//surroundingTiles[gCollisonScore[currentIndex][0]].score += 125;
		//surroundingTiles[gCollisonScore[currentIndex][1]].score += 125;
		//surroundingTiles[gCollisonScore[currentIndex][2]].score += 125;
		//surroundingTiles[gCollisonScore[currentIndex][3]].score += 125;
	}
}

///<summary>
/// Performs heuristic calucations to generate a score for a particular tile
///</summary>		
static void calculateScoreForTileIndex(int currentIndex, TilemapHeuristic *surroundingTiles, CollisionLayer *collisionLayer, TempCollisionTable *tempCollisions, int skipTempCollisionID, float currentAngle, int destX, int destY, bool useTempCollisionTable)
{
	ASSERT(currentIndex < NUMBER_OF_PATHFINDING_NODES);

	// If it's an orthagonal tile we add 14 to the score (it's ~ 1.441 times further), otherwise 10
	if((currentIndex == 0) || (currentIndex == 2) || (currentIndex == 5) || (currentIndex == 7))
	{
		surroundingTiles[currentIndex].score += 14;
	}		
	else
	{
		surroundingTiles[currentIndex].score += 10;
	}

	// If the angle required for this tile is different than the current angle we add a turning score...
	if(currentAngle != gAnglesByPathfindingNode[currentIndex])
	{
		surroundingTiles[currentIndex].score += 10;
	}	

	// Add our H value to the score based on the x and y distance of destX and destY from this tile
	// TODO: Create a procedure here that will calculate the score based on 
	int distance = (abs(surroundingTiles[currentIndex].x - destX) + abs(surroundingTiles[currentIndex].y - destY));		
	surroundingTiles[currentIndex].score += ( 10 * distance);

	// Update surrounding tiles based on whether or not there is another temp value on that tile
	if(useTempCollisionTable == true)
	{
		updateTempCollisionForTiles(surroundingTiles, tempCollisions, skipTempCollisionID, currentIndex);
	}

	// Update surrounding tiles based on whether or not a collision occurs on that tile
	updateCollisionForSurroundingTiles(surroundingTiles, collisionLayer, currentIndex);	
}

///<summary>
/// Performs some very basic heuristic pathfinding analysis to obtain the angle at which a sprite
/// would move to find the shortest path to destinationX and destinationY from x and y on the tilemap.
/// Will currently ASSERT if x or y are an edge tile (Either o, or the respective width / height of the tilemap)
/// Since Swim Fishy Swim doesn't let sprites navigate to those points. It must always start from 1 tile into the
/// map in any direction.
/// EWWWWEEEE PLEASE REFACTOR THIS YHAR FUNCHEEEN
///</summary>		
float TiledMaps::findAngleToDestination(Vector currentPosition, float currentAngle, Vector destination, int skipCollisionID)
{	
	TilemapHeuristic surroundingTiles[NUMBER_OF_PATHFINDING_NODES];
	int startingX = (int)(abs(currentPosition.x) / TILE_QUAD_WIDTH);
	int startingY = (int)(abs(currentPosition.y) / TILE_QUAD_HEIGHT);
	int destX = (int)(abs(destination.x) / TILE_QUAD_WIDTH);
	int destY = (int)(abs(destination.y) / TILE_QUAD_HEIGHT);
	int lowestScore = INT_MAX;
	int lowestScoreIndex = 0;
	
	// Sets up our top, middle, and bottom heuristic information
	setupSurroundingTiles(surroundingTiles, myTilemapSize, startingX, startingY, 3);

	// Now to calcuate each score we get information about each tile in the tilemap and apply the following
	// forumla: Score = G + H Where G is the movement value (10 for for non orthagonal moves, 14 for orthagonal moves)
	// And H is the distance in x and y tiles to the destination times 10. The one with the lowest value is the one we return.
	for(int index = 0; index < NUMBER_OF_PATHFINDING_NODES; index++)
	{
		calculateScoreForTileIndex(index, surroundingTiles, &myTilemapCollisionLayer, 
				&myTempCollisionTable, skipCollisionID, currentAngle, destX, destY, true);
	}	
	
	// Find the lowest considerable score and set our angle to point toward that tile :)
	for(int index = 0; index < NUMBER_OF_PATHFINDING_NODES; index++)
	{
		if((surroundingTiles[index].considered == true) && (surroundingTiles[index].score < lowestScore))
		{
			lowestScore = surroundingTiles[index].score;
			lowestScoreIndex = index;
		}
	}

	return 	gAnglesByPathfindingNode[lowestScoreIndex];
}

///<summary>
/// Returns the size details of the tilemap
///</summary>		
TilemapSize TiledMaps::getTilemapSize()
{
	return myTilemapSize;
}

///<summary>
/// Performs some very basic heuristic pathfinding analysis to obtain the next tile
/// on the path that will best lead a sprite to its destination. This can be used with
/// findAngleToDestination to assist a sprite in using a highly modify A* alogrithm
/// to find it's way without knowing the full path every iteration...
///</summary>		
Vector TiledMaps::findBestVectorOnPath(Vector currentPosition, Vector destination, float currentAngle, int numberOfTilesAway, int skipTempCollisionID)
{	
	int startingX = (int)(abs(currentPosition.x) / TILE_QUAD_WIDTH);
	int startingY = (int)(abs(currentPosition.y) / TILE_QUAD_HEIGHT);
	int destX = (int)(abs(destination.x) / TILE_QUAD_WIDTH);
	int destY = (int)(abs(destination.y) / TILE_QUAD_HEIGHT);
	int lowestScore = INT_MAX;
	int lowestScoreIndex = 0;
	Vector returnPosition;

	if (numberOfTilesAway > 0)
	{
		TilemapHeuristic surroundingTiles[NUMBER_OF_PATHFINDING_NODES];

		// Loop through and find the most appropriate angle based on a certain number of samples
		for (int idx = 0; idx < numberOfTilesAway; idx++)
		{
			lowestScore = INT_MAX;

			// Sets up our top, middle, and bottom heuristic information
			setupSurroundingTiles(surroundingTiles, myTilemapSize, startingX, startingY, 1);

			// Now to calcuate each score we get information about each tile in the tilemap and apply the following
			// forumla: Score = G + H Where G is the movement value (10 for for non orthagonal moves, 14 for orthagonal moves)
			// And H is the distance in x and y tiles to the destination times 10. The one with the lowest value is the one we return.
			for (int index = 0; index < NUMBER_OF_PATHFINDING_NODES; index++)
			{
				calculateScoreForTileIndex(index, surroundingTiles, &myTilemapCollisionLayer, &myTempCollisionTable, skipTempCollisionID, currentAngle, destX,
						destY, false);
			}

			// Now we do another loop this time to find the lowest score in the bunch...
			for (int index = 0; index < NUMBER_OF_PATHFINDING_NODES; index++)
			{
				if ((surroundingTiles[index].score < lowestScore) && (surroundingTiles[index].considered == true))
				{
					lowestScore = surroundingTiles[index].score;
					lowestScoreIndex = index;
				}
			}

			// We modify startingx and startingy so that when we loop through again we'll
			// be checking those values...
			startingX = surroundingTiles[lowestScoreIndex].x;
			startingY = surroundingTiles[lowestScoreIndex].y;
		} // end for index...

		// Now we should have a tile the specified number of blocks away that is on the best path to our destination.
		// we can assign the x and y from that tile and return it as our return vector...
		returnPosition.x = (f32)surroundingTiles[lowestScoreIndex].x;
		returnPosition.y = (f32)surroundingTiles[lowestScoreIndex].y;
		returnPosition.z = 0.0f;
	}
	else
	{
		// If we're already where we needs to be - then dun worry about it!
		returnPosition.x = (f32)startingX;
		returnPosition.y = (f32)startingY;
		returnPosition.z = 0.0f;
	}

	return 	returnPosition;
}

///<summary>
/// builds a quad map based on the width and height of the tilemap
///</summary>	
void buildTilemapQuads(TilemapSize mapSize, TilemapLayer *layer, TextureCoordinateSize textureSize, TilemapZValues tilemapZData, bool applyHeightMap)
{
	float indexX;
	float indexY;
	int tilemapIndex;

	ASSERT(layer != NULL);	
	
	// Loop through all of the X,Y values in our tilemap layer and assign them to the tilemap layer
	for(indexY = 0.0f; indexY < (float)mapSize.tilemapHeight; indexY+=1.0f)
	{
		//debugPrint("\nRow: %f\n", indexY);
		for(indexX = 0.0f; indexX < (float)mapSize.tilemapWidth; indexX+=1.0f)
		{	
			// Create a new TextureIndex object for storing the x/y coordinates in our layer
			Quad newQuad;
			
			// Get our index into the tilemap x/y coordinates
			tilemapIndex = (int)((mapSize.tilemapWidth * indexY) + indexX);			
											
			if((layer->tiles.at(tilemapIndex).xIndex > 0) || (layer->tiles.at(tilemapIndex).yIndex > 0))
			{				
				if(applyHeightMap == true)
				{
					setupQuad(&newQuad, (indexX * TILE_QUAD_WIDTH), (indexY * TILE_QUAD_HEIGHT), tilemapZData[tilemapIndex], textureSize, layer->tiles.at(tilemapIndex));
				}
				else
				{
					//ASSERT(false);
					QuadZValues zVals = {0, 0, 0, 0};
					setupQuad(&newQuad, (indexX * TILE_QUAD_WIDTH), (indexY * TILE_QUAD_HEIGHT), zVals, textureSize, layer->tiles.at(tilemapIndex));
				}
				
				// Add this quad to our map
				layer->quads[tilemapIndex] = newQuad;
				//debugPrint("X(%d) Y(%d),", layer->tiles.at(tilemapIndex).xIndex, layer->tiles.at(tilemapIndex).yIndex);
				//trace("X(%d) Y(%d),", layer->tiles.at(tilemapIndex).xIndex, layer->tiles.at(tilemapIndex).yIndex);

			}
		}
		// Debug, please remove :D
		//if(indexY >= 125) usleep(1000000);
	}
}

///<summary>
/// builds a quad map based on the width and height of the tilemap
///</summary>	
void buildWaterQuads(TilemapSize mapSize, TilemapLayer *layer, float wrapSize)
{
	ASSERT(layer != NULL);	
	// Create a new TextureIndex object for storing the x/y coordinates in our layer
	
	setupWaterQuads(layer, mapSize, wrapSize);	
}
	
///<summary>
/// Adds a file/texture combination into the IMap object
///</summary>	
bool TiledMaps::loadTileMap(const char* fileName)
{
	bool returnValue = false;
   
	FILE *mapFileHandle;
	//Load our map file
	mapFileHandle = fopen(fileName, "r");	
	ASSERT(mapFileHandle != NULL);
	
	if(mapFileHandle != NULL)
	{
		// Read out our TilemapSize object ...
		if(readFileHeader(mapFileHandle, &myTilemapSize) == true)
		{
			// If we got our header, then let's figure out what our unit size is ...			
			myTextureCoordinateSize.width = (float)(1.0 / (myTilemapSize.graphicWidth / TILEMAP_TILE_SIZE));
			myTextureCoordinateSize.height = (float)(1.0 / (myTilemapSize.graphicHeight / TILEMAP_TILE_SIZE));
			
			// Read out the tile information for Layer A
			//debugPrint("Reading Layer A.\n");usleep(1000000);
			if(readTilemapLayer(mapFileHandle, myTilemapSize, &myTilemapLayerA) == true)
			{		
				//debugPrint("Reading Layer B.\n");usleep(1000000);
				// Read out the tile information for Layer B
				if(readTilemapLayer(mapFileHandle, myTilemapSize, &myTilemapLayerB) == true)
				{	
					//debugPrint("Reading Layer C.\n");usleep(1000000);
					// Read out the tile information for Layer C
					if(readTilemapLayer(mapFileHandle, myTilemapSize, &myTilemapLayerC) == true)
					{						
						//debugPrint("Reading Layer D.\n");usleep(1000000);
						// Read out the tile information for Layer D
						if(readTilemapLayer(mapFileHandle, myTilemapSize, &myTilemapLayerD) == true)
						{							
							//debugPrint("Reading Collision Layer.\n");usleep(1000000);
							// Read out the collision information as a set of boolean values
							if(readTilemapCollision(mapFileHandle, myTilemapSize, &myTilemapCollisionLayer.tilemapCollision) == true)
							{
								//debugPrint("Reading Z Layer.\n");usleep(1000000);
								// Read out the Z information as a set of 4 byte values							
								if(readTilemapZData(mapFileHandle, myTilemapSize, myTilemapZValues) == true)
								{
									// Set up the quad map for each of our tilemap layers...
									buildTilemapQuads(myTilemapSize, &myTilemapLayerA, myTextureCoordinateSize, myTilemapZValues, true);
									//buildTilemapQuads(myTilemapSize, &myTilemapLayerB, myTextureCoordinateSize, myTilemapZValues, false);
									//buildTilemapQuads(myTilemapSize, &myTilemapLayerC, myTextureCoordinateSize, myTilemapZValues, false);
									//buildTilemapQuads(myTilemapSize, &myTilemapLayerD, myTextureCoordinateSize, myTilemapZValues, false);
									//buildWaterQuads(myTilemapSize, &myTilemapLayerC, 8.0f);
									buildWaterQuads(myTilemapSize, &myTilemapLayerD, 4.0f);
									returnValue = true;
								}
							}// End Collision Data
						} // End Layer D
					} // End Layer C
				} // End Layer B
			}// End Layer A
		}// End Read Tilemap Size ...	
	}// End if handle != null
		
	// Close our file pointer!
   fclose(mapFileHandle);

   return returnValue;
}

///<summary>
/// Translates all objects loaded in the map based on an xyz coordinate set
///</summary>
void TiledMaps::translateMap(TILEMAP_LAYER layer, Vector translation)
{	
	Vector zeroVelocity = {0.0f, 0.0f, 0.0f};
	if(layer == TILEMAP_LAYER_A)
	{
		myTilemapLayerA.mapCoordinates = translation;
		myTilemapLayerA.mapVelocityPerSec = zeroVelocity;
	}
	else if(layer == TILEMAP_LAYER_B)
	{
		myTilemapLayerB.mapCoordinates = translation;
		myTilemapLayerB.mapVelocityPerSec = zeroVelocity;
	}
	else if(layer == TILEMAP_LAYER_C)
	{
		myTilemapLayerC.mapCoordinates = translation;
		myTilemapLayerC.mapVelocityPerSec = zeroVelocity;
		//initWaterMovment(myTilemapLayerC.layerMotion, (Vector){-0.0002f, 0.0001f, -0.0002f}, 0.016f, 0.384, 0.016f, 0.584f, 0.032f, 0.032f);
	}
	else if(layer == TILEMAP_LAYER_D)
	{
		myTilemapLayerD.mapCoordinates = translation;
		myTilemapLayerD.mapVelocityPerSec = zeroVelocity;
		Vector waterMovementVector = {-0.0003f, -0.0002f, -0.000005f};
		initWaterMovment(myTilemapLayerD.layerMotion, waterMovementVector, 0.016f, 1.256f, 0.016f, 1.384f, 0.00f, 0.0001f);
	}
	
}

///<summary>
/// Sets a starting point vector that the map cannot move beyond. The ending point will be based upon the starting point + width + height of map
///</summary>
void TiledMaps::setMinimumBoundary(Vector minimumBoundary)
{
	myMinimumBoundary = minimumBoundary;
	// Our coordinate space works in negative numbers, so we'll always subtract the maximum
	// We have to adjust the maximum bounary by 2x the minumum boundary
	myMaximumBoundary.x = minimumBoundary.x - ((myTilemapSize.tilemapWidth * TILE_QUAD_WIDTH) + (minimumBoundary.x * 2));
	myMaximumBoundary.y = minimumBoundary.y - ((myTilemapSize.tilemapWidth * TILE_QUAD_HEIGHT) + (minimumBoundary.y * 2));
	myMaximumBoundary.z = minimumBoundary.z - 100.0f;
}
	
///<summary>
/// Gets the xyz vector of the map layer specified defining the upper left hand corner of the layer
///</summary>
Vector TiledMaps::getLocation(TILEMAP_LAYER layer)
{
	Vector myReturnValue;
	if(layer == TILEMAP_LAYER_A)
	{
		myReturnValue = myTilemapLayerA.mapCoordinates;		
	}
	else if(layer == TILEMAP_LAYER_B)
	{
		myReturnValue = myTilemapLayerB.mapCoordinates;
	}
	else if(layer == TILEMAP_LAYER_C)
	{
		myReturnValue = myTilemapLayerC.mapCoordinates;
	}
	else if(layer == TILEMAP_LAYER_D)
	{
		myReturnValue = myTilemapLayerD.mapCoordinates;
	}
	
	return myReturnValue;
}

///<summary>
/// Sets the velocity at which the map will move
///</summary>
void TiledMaps::setMapVelocity(TILEMAP_LAYER layer, Vector unitsPerSecond)
{		
	if(layer == TILEMAP_LAYER_A)
	{
		myTilemapLayerA.mapVelocityPerSec = unitsPerSecond;
	}
	else if(layer == TILEMAP_LAYER_B)
	{
		myTilemapLayerB.mapVelocityPerSec = unitsPerSecond;
	}
	else if(layer == TILEMAP_LAYER_C)
	{
		myTilemapLayerC.mapVelocityPerSec = unitsPerSecond;
	}
	else if(layer == TILEMAP_LAYER_D)
	{
		myTilemapLayerD.mapVelocityPerSec = unitsPerSecond;
	}	
}

/// Returns true if the specified x,y coordinate is marked on the collision map
/// Overloaded to send -1 for tempCollisionID...
///</summary>
bool TiledMaps::checkForCollisionAt(Vector collisionOffset)
{
	return checkForCollisionAt(collisionOffset, -1);
}

///<summary>
/// Returns the z value at the specified location in the tilemap
/// using bilinear interpolation between the tilemap verticies...
///</summary>
float TiledMaps::getZValueAt(Vector &location)
{
	float returnValue = 0.0f;	
	Vector collisionPoint;
	float xOffset = 0;
	float yOffset = 0;
	
	collisionPoint.x = abs(location.x) / TILE_QUAD_WIDTH;
	collisionPoint.y = abs(location.y) / TILE_QUAD_HEIGHT;
	// Get only the partial X/Y values from our location	
	xOffset = fmod(location.x, 1.0f);
	yOffset = fmod(location.y, 1.0f);

	// To perform bilinear interpolation on a quad (to get a z value) you
	// perform linear interpolation on the top to corners, and bottom two corners
	// using the x value within the tile, and the perform linear interpolation
	// on those two values using the y value within the tile to get what your actual
	// z value should be. Isn't that easy! I love it when I get the maths :D.

	if(IS_WITHIN_MAP_BOUNDARIES(collisionPoint))
	{			
		// Now we should have an X and Y set of values for the tilemap. Let's convert that into an index and access our collision array
		int tilemapIndex = (int)(((myTilemapSize.tilemapWidth) * collisionPoint.y) + collisionPoint.x);
		//ASSERT((uint)tilemapIndex < myTilemapCollisionLayer.tilemapCollision.size());
		
		// Increase our tilemap index to account for 4 vectors in each tile of the vertex array...
		//tilemapIndex *= 4;
		
		// Now that we have our tilemap index we need to perform bilinear interpolation 
		// on the 4 z values of the tile to obtain our current z value based on our offset in the tile...
		u32 topIndex = tilemapIndex;
		u32 bottomIndex = tilemapIndex + (myTilemapSize.tilemapWidth);
		float topHorizontal	  = fastLinearInterpolation(myTilemapLayerA.quadVertexArray.Verticies[topIndex].z, myTilemapLayerA.quadVertexArray.Verticies[topIndex + 1].z, xOffset);
		float bottomHorizontal = fastLinearInterpolation(myTilemapLayerA.quadVertexArray.Verticies[bottomIndex].z, myTilemapLayerA.quadVertexArray.Verticies[bottomIndex + 1].z, xOffset);
		returnValue = fastLinearInterpolation(topHorizontal, bottomHorizontal, yOffset);				
	}

	return returnValue + myTilemapLayerA.mapCoordinates.z;
}

///<summary>
/// Returns true if the specified x,y coordinate is marked on the collision map
/// Set tempCollisionID is used to prevent use of a particular temp collision ID
/// (e.g. Use the PlayerID so the player doesn't collide with his own spot
///</summary>
bool TiledMaps::checkForCollisionAt(Vector collisionOffset, int tempCollisionID)
{
	bool returnValue = false;
	Vector collisionPoint;	
	
	// Our minumum and maximum boundaires should prevent any problems with collision.
	// So we should be able to essentially take the absolute values of the x/y coordinates
	// specifies and have them correspond directly to a tile when devided by the coordinate
	// widht and height.
	collisionPoint.x = abs(collisionOffset.x) / TILE_QUAD_WIDTH;
	collisionPoint.y = abs(collisionOffset.y) / TILE_QUAD_HEIGHT;
	
	//debugPrint("X: %f of %d, Y: %f of %d\n", collisionPoint.x, myTilemapSize.tilemapWidth, collisionPoint.y, myTilemapSize.tilemapHeight);usleep(1000000);										
	
	// Let's just make sure we're not returning any truly funky values ...
	//ASSERT(collisionPoint.x >= 0.0f);
	//ASSERT(collisionPoint.y >= 0.0f);
	//ASSERT(collisionPoint.x <= myTilemapSize.tilemapWidth);
	//ASSERT(collisionPoint.y <= myTilemapSize.tilemapHeight);
	if(IS_WITHIN_MAP_BOUNDARIES(collisionPoint))
	{	
		
		// Now we should have an X and Y set of values for the tilemap. Let's convert that into an index and access our collision array
		int tilemapIndex = (int)((myTilemapSize.tilemapWidth * collisionPoint.y) + collisionPoint.x);	
		ASSERT((uint)tilemapIndex < myTilemapCollisionLayer.tilemapCollision.size());

		// If a tempCollisionID was specified, then let's check and see if there's a spot for it to collide with...
		if(tempCollisionID >= 0)
		{
			// Iterator through the temp collision table to see if there's a potential collision pending
			TempCollisionIterator tempIterator = myTempCollisionTable.begin();
			for(u32 idx = 0; idx < myTempCollisionTable.size(); idx++)
			{
				// If our key value is not equal to the tempCollisionID (which we don't want to use to avoid self-collisions)
				if(tempIterator->first != tempCollisionID)
				{
					if(tilemapIndex == tempIterator->second)
					{
						returnValue = true;
					}
				}
				tempIterator++;
			}
		}
		
		// If not temp collision was found, then we can check for an actual collision tile ...
		if(returnValue == false)
		{
			returnValue = myTilemapCollisionLayer.tilemapCollision.at(tilemapIndex);
		}
	}
	
	return returnValue;
}

///<summary>
/// Generates Vertex Arrays so that we can speed up rendering ...
///</summary>	
void TiledMaps::generateLayerVertexArrays(IGraphics *graphics, TILEMAP_LAYER layer)
{
	if(layer == TILEMAP_LAYER_A)
	{		
		QuadVertexArrayProperties arrayProperties;
		arrayProperties.cullBacks = true;
		arrayProperties.quadsWide = myTilemapSize.tilemapWidth;
		arrayProperties.quadsHigh = myTilemapSize.tilemapHeight;
		arrayProperties.texTilesWide = (myTilemapSize.graphicWidth / TILEMAP_TILE_SIZE);
		arrayProperties.texTilesHigh = (myTilemapSize.graphicHeight / TILEMAP_TILE_SIZE);
		arrayProperties.quadTexWidth = myTextureCoordinateSize.width;
		arrayProperties.quadTexHeight = myTextureCoordinateSize.height;

		graphics->allocateAndPopulateVertexArray(myTilemapLayerA.quads, myTilemapLayerA.quadVertexArray, arrayProperties);
		myTilemapLayerA.quads.clear();
		myTilemapLayerA.tiles.clear();		
	}	
	/*else if(layer == TILEMAP_LAYER_C)
	{		
		graphics->allocateAndPopulateVertexArray(myTilemapLayerC.quads, myTilemapLayerC.quadVertexArray, true);
		myTilemapLayerC.quads.clear();
		myTilemapLayerC.tiles.clear();
	}*/
	else if(layer == TILEMAP_LAYER_D)
	{		
		QuadVertexArrayProperties arrayProperties;
		arrayProperties.cullBacks = true;
		arrayProperties.quadsHigh = 1;
		arrayProperties.quadsWide = 1;
		arrayProperties.texTilesWide = 8;
		arrayProperties.texTilesHigh = 8;
		arrayProperties.quadTexWidth = myTextureCoordinateSize.width;
		arrayProperties.quadTexHeight = myTextureCoordinateSize.height;
		graphics->allocateAndPopulateVertexArray(myTilemapLayerD.quads, myTilemapLayerD.quadVertexArray, arrayProperties);
		myTilemapLayerD.quads.clear();
		myTilemapLayerD.tiles.clear();
	}	
	//TODO: Support layers b,c, and d
}

///<summary>
/// Generates Vertex Arrays so that we can speed up rendering ...
///</summary>	
void TiledMaps::freeLayerVertexArrays(IGraphics *graphics, TILEMAP_LAYER layer)
{
	if(layer == TILEMAP_LAYER_A)
	{		
		graphics->freeVertexArray(myTilemapLayerA.quadVertexArray);		
	}	
	else if(layer == TILEMAP_LAYER_C)
	{		
		graphics->freeVertexArray(myTilemapLayerC.quadVertexArray);
	}
	else if(layer == TILEMAP_LAYER_D)
	{		
		graphics->freeVertexArray(myTilemapLayerD.quadVertexArray);
	}	
	//TODO: Support layers b,c, and d
}

///<summary>
/// Will cause the map coordinates to give the appears of shaking
///</summary>	
static void handleMapShaking(float &shakeTimer, float &shakeMagnitude, Vector &renderingCoordinates)
{
	if(shakeTimer > 0.0f)
	{
		shakeTimer -= (float)(1.0 * gTimeScale);		
		float shakeValueX = (float)(rand() % 2) - 1;
		float shakeValueY = (float)(rand() % 2) - 1;
		renderingCoordinates.x += shakeValueX * shakeMagnitude;
		renderingCoordinates.y += shakeValueY * shakeMagnitude;				
	}
}

///<summary>
/// Renders the vertex arrays for the specified map layer
///</summary>	
void TiledMaps::renderLayerVertexArrays(IGraphics *graphics, TILEMAP_LAYER layer)
{
	ASSERT(myTileTexture != UNDEFINED_TEXTURE_HANDLE);

	if(myTileTexture != UNDEFINED_TEXTURE_HANDLE)
	{
		if(graphics != NULL)
		{
			if(layer == TILEMAP_LAYER_A)
			{		
				Vector renderingCoordinates = myTilemapLayerA.mapCoordinates;
				handleMapShaking(myShakeTimer, myShakeMagnitude, renderingCoordinates);
				graphics->addNormalMappedVertexArrayToScene(BUMP_TYPE_XYZ_MAP, renderingCoordinates, myTileTexture, myWaterLightMap, myCloudTexture, myTilemapLayerA.quadVertexArray);						
				//graphics->addVertexArrayToScene(renderingCoordinates, myTileTexture, myTilemapLayerA.quadVertexArray);								
			}
			else if(layer == TILEMAP_LAYER_C)
			{				
				//Vector renderingCoordinates = myTilemapLayerC.mapCoordinates;
				//renderingCoordinates.x += myTilemapLayerC.layerMotion.x;
				//renderingCoordinates.y += myTilemapLayerC.layerMotion.y;
				//handleMapShaking(myShakeTimer, myShakeMagnitude, renderingCoordinates);
				//graphics->addNormalMappedVertexArrayToScene(BUMP_TYPE_NORMAL_MAP, renderingCoordinates, myCloudTexture, myWaterLightMap, myWaterNormalMaps.at(myCurrentWaterNormalMap), myTilemapLayerC.quadVertexArray);						
			}
			else if(layer == TILEMAP_LAYER_D)
			{				

				Vector renderingCoordinates = myTilemapLayerD.mapCoordinates;
				renderingCoordinates.x += myTilemapLayerD.layerMotion.x;
				renderingCoordinates.y += myTilemapLayerD.layerMotion.y;
				renderingCoordinates.z += myTilemapLayerD.layerMotion.z;
				handleMapShaking(myShakeTimer, myShakeMagnitude, renderingCoordinates);
				graphics->addNormalMappedVertexArrayToScene(BUMP_TYPE_XYZ_MAP, renderingCoordinates, myWaterTexture, myWaterLightMap, myWaterNormalMaps.at(myCurrentWaterNormalMap), myTilemapLayerD.quadVertexArray);
			}
		} // end if graphics != null
	}
}

///<summary>
/// Returns the current motion details for a layer so that they can be applied to objects outside of the tilemap...
///</summary>
LayerMotion TiledMaps::getLayerMotion(TILEMAP_LAYER layer)
{
	LayerMotion returnValue = myTilemapLayerD.layerMotion;

	if(layer == TILEMAP_LAYER_A)
	{
		returnValue = myTilemapLayerA.layerMotion;
	}
	else if(layer == TILEMAP_LAYER_B)
	{
		returnValue = myTilemapLayerB.layerMotion;
	} 
	else if(layer == TILEMAP_LAYER_C)
	{
		returnValue = myTilemapLayerC.layerMotion;
	}	

	return returnValue;
}

///<summary>
/// Is in charge of handling all changes associated with tile animation over a period of time
///</summary>
void TiledMaps::animationTask(TILEMAP_LAYER layer)
{	
	// Increment the animation timer
	myAnimationTimer += myAnimationsPerSecond * gTimeScale;	
	
	if(myAnimationTimer > 1.00f)
	{
		myCurrentWaterNormalMap++;
		if((uint)myCurrentWaterNormalMap >= myWaterNormalMaps.size())
		{
			myCurrentWaterNormalMap = 0;
		}
		myAnimationTimer = 0.0f;
	}

	// Animate our water motion every other frame...
	myWaterAnimationCounter++;
	if((myWaterAnimationCounter % 2) == 0)
	{
		animateWaterMovement(myTilemapLayerD);
	}
}

///<summary>
/// Updates tilemap velocity if we are within the boundaries of the tilemap. Otherwise, it won't allow it to change...
///</summary>
void updateMapVelcoity(Vector minBoundary, Vector maxBoundary, Vector velocityPerSec, Vector &coordinates)
{
	float xVelocity = (velocityPerSec.x * gTimeScale);
	float yVelocity = (velocityPerSec.y * gTimeScale);
	float zVelocity = (velocityPerSec.z * gTimeScale);
	
	// See if we can update our x location based on our boundaries
	if(((coordinates.x + xVelocity) < minBoundary.x) && ((coordinates.x + xVelocity) > maxBoundary.x))
	{
		coordinates.x += xVelocity;
	}
	
	// See if we can update our y location based on our boundaries
	if(((coordinates.y + yVelocity) < minBoundary.y) && ((coordinates.y + yVelocity) > maxBoundary.y))
	{
		coordinates.y += yVelocity;
	}
	
	// See if we can update our z location based on our boundaries
	if(((coordinates.z + zVelocity) < minBoundary.z) && ((coordinates.z + zVelocity) > maxBoundary.z))
	{
		coordinates.z += zVelocity;
	}	
}
  
///<summary>
/// Is in charge of handling all changes associated with map velocity over a period of time
///</summary>
void TiledMaps::velocityTask()
{	
	// Apply the velocity information for each of our map layers ...
	updateMapVelcoity(myMinimumBoundary, myMaximumBoundary, myTilemapLayerA.mapVelocityPerSec, myTilemapLayerA.mapCoordinates);
	updateMapVelcoity(myMinimumBoundary, myMaximumBoundary, myTilemapLayerB.mapVelocityPerSec, myTilemapLayerB.mapCoordinates);
	updateMapVelcoity(myMinimumBoundary, myMaximumBoundary, myTilemapLayerC.mapVelocityPerSec, myTilemapLayerC.mapCoordinates);
	updateMapVelcoity(myMinimumBoundary, myMaximumBoundary, myTilemapLayerD.mapVelocityPerSec, myTilemapLayerD.mapCoordinates);
}
	
///<summary>
/// Add/Update a temporary collision element - which lasts until clearTempCollisionTable() is called
///</summary>
void TiledMaps::setTempCollisionAt(int collisionID, Vector collisionOffset)
{
	Vector collisionPoint;
	
	collisionPoint.x = abs(collisionOffset.x) / TILE_QUAD_WIDTH;
	collisionPoint.y = abs(collisionOffset.y) / TILE_QUAD_HEIGHT;
	
	
	if(IS_WITHIN_MAP_BOUNDARIES(collisionPoint))
	{	
		// Now we should have an X and Y set of values for the tilemap. Let's convert that into an index and access our collision array
		// and add it to our table ...		
		int tempCollisionIndex = (int)((myTilemapSize.tilemapWidth * collisionPoint.y) + collisionPoint.x);				
		myTempCollisionTable[collisionID] = tempCollisionIndex;
	}
}

///<summary>
/// Removes a temporary collision element by the ID it was created under ...
///</summary>
void TiledMaps::removeTempCollision(int collisionID)
{
	myTempCollisionTable.erase(collisionID);
}

///<summary>
/// Clears out any temporary collisions that have been added to the tilemap
///</summary>
void TiledMaps::clearTempCollisionTable()
{
	myTempCollisionTable.clear();
}


///<summary>
/// Sets a time during which the map will randomly shake back and forth...
///</summary>
void TiledMaps::shakeMapFor(float timeUnits, float magnitude)
{
	myShakeTimer = timeUnits;
	myShakeMagnitude = magnitude;
}

///<summary>
/// Gets the TextureHandle for the current water normal map (for use with projection and other features)
///</summary>
TextureHandle TiledMaps::getCurrentWaterNormalMap()
{
	return myWaterNormalMaps.at(myCurrentWaterNormalMap);
}
