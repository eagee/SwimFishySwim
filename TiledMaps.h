// TiledMaps.h
// Implementation of IMap interface that uses tilemaps from the TiledMap (tmx) xml format
// The specifications for the tilemap format are included at the bottom of this file!
// Author(s): Eagan Rackley
#ifndef TILED_MAP_H
#define TILED_MAP_H

#include <map>
#include <vector>
#include <deque>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "IGraphics.h"
#include "INodeWaypoints.h"

#define TILEMAP_ANIMATIONS_PER_SECOND 	  15
#define TILEMAP_TILE_SIZE 				  32
#define TILE_QUAD_WIDTH 1.0f
#define TILE_QUAD_HEIGHT 1.0f

// Defines a type of collision that can be modified every frame
typedef std::map<int, int> TempCollisionTable;
typedef TempCollisionTable::iterator TempCollisionIterator;

/// <summary>
/// Defines the various layers in our tilemap
/// </summary>
enum TILEMAP_LAYER
{
	TILEMAP_LAYER_A		= 0,
	TILEMAP_LAYER_B		= 1,
	TILEMAP_LAYER_C		= 2,
	TILEMAP_LAYER_D	    = 3,
	TILEMAP_COLLISION   = 4
};

///<summary>
/// Used to track water motion in a tilemap...
///</summary>
typedef struct tagLayerMotion
{
	float x;
	float y;
	float z;
	float zVertsZeroAndThree;
	float zVertsOneAndTwo;
	float xVel;
	float yVel;
	float zVel;
	float xStep;	
	float yStep;
	float zStep;
	float xMin;
	float yMin;
	float zMin;
	float xMax;
	float yMax;
	float zMax;	
} LayerMotion;


///<summary>
/// Defines the width and height of the tilemap in tiles
///</summary>
typedef struct tagTilemapSize
{
	short tilemapWidth;
	short tilemapHeight;	
	short graphicWidth;
	short graphicHeight;	
} TilemapSize;

///<summary>
/// Defines the texture coordinate size of each tile on the graphic associated with the tilemap
///</summary>
typedef struct tagTextureCoordinateSize
{
	float width;
	float height;
} TextureCoordinateSize;

///<summary>
/// Defines how many units into a texture at X and Y 
///</summary>
typedef struct tagTextureIndex
{
	int xIndex;
	int yIndex;
} TextureIndex;

typedef struct tagQuadZValues
{
	unsigned char topLeft;
	unsigned char topRight;
	unsigned char bottomRight;
	unsigned char bottomLeft;
	unsigned char tlNormalX;
	unsigned char tlNormalY;
	unsigned char tlNormalZ;
	unsigned char trNormalX;
	unsigned char trNormalY;
	unsigned char trNormalZ;
	unsigned char brNormalX;
	unsigned char brNormalY;
	unsigned char brNormalZ;
	unsigned char blNormalX;
	unsigned char blNormalY;
	unsigned char blNormalZ;
} QuadZValues;

///<summary>
///Defines a vector of float data used to specify the x or y coordinate of graphic in the tilemap
///</summary>
typedef std::vector<TextureIndex> TilemapData;
typedef TilemapData::iterator TilemapDataIterator;

///<summary>
///Defines a vector of bool data used to specify whether or not a particular coordinate is marked for collision
///</summary>   
typedef std::vector<bool> CollisionData;
typedef CollisionData::iterator CollisionDataIterator;

///<summary>
/// Defines a vector that records the number of times a tile is used for pathfinding to keep sprites moving forward
/// and add some randomness to their behavior (used with pathfinding code)
///</summary>   
typedef int TileAccessCount;
typedef std::map<int, TileAccessCount> TileAccessCounts;
typedef TileAccessCounts::iterator TileAccessCountsIterator;

///<summary>
///Defines a map of quad objects that define a tilemap
///</summary>   
typedef std::map<int, Quad> TilemapQuads;
typedef TilemapQuads::iterator TilemapQuadsIterator;

///<summary>
///Defines a map of quad z values that define the vertical height of each corner of each tile in the tilemap
///</summary>   
typedef std::map<int, QuadZValues> TilemapZValues;
typedef TilemapZValues::iterator TilemapZValuesIterator;

///<summary>
/// Defines a single layer of x and y coordinate information used to texture our tilemap
///</summary>
typedef struct tagTilemapLayer
{
	TilemapData 		tiles;
	TilemapQuads		quads;
	float 				animationTimeScale;
	float 				currentAnimationFrame;	
	int 				mapAnimationIndex;
	bool 				mapWrapEdges;
	Vector				mapCoordinates;
	Vector				mapVelocityPerSec;	
	LayerMotion 		layerMotion;
	QuadVertexArray		quadVertexArray;
} TilemapLayer;

///<summary>
/// Defines a single layer of boolean values accessible as an x/y index ...
///</summary>
typedef struct tagTilemapCollisionLayer
{
  CollisionData  		tilemapCollision;
  TileAccessCounts		tilemapAccessCounts;
  Vector				mapCoordinates;
  Vector				mapVelocityPerSec;  
} CollisionLayer;

///<summary>
/// Defines an interface into any type of model
///</summary>
class TiledMaps
{
   public:
	
	///<summary>
	/// The destructor implementation for the Md2Map object
	///</summary>	
	TiledMaps();
	
	///<summary>
	/// The destructor implementation for the Md2Map object
	///</summary>	
	~TiledMaps();
		
	///<summary>
	/// Allows a user to specify a texture handle returned from the graphics subsystem
	///</summary>	
	virtual void assignTextures(TextureHandle tileTexture, TextureHandle waterTexture, TextureHandle cloudTexture, TextureHandle waterLightMap, TextureHandle waterNormalMap);
	
	///<summary>
	/// Adds a texture into the list of normal mapped textures used by the system
	///</summary>	
	virtual void addWaterNormalMap(TextureHandle waterNormalMap);	

	///<summary>
	/// Clears all existing references to water normal maps
	///</summary>	
	virtual void clearWaterNormalMaps();

	///<summary>
	/// Performs some very basic heuristic pathfinding analysis to obtain the next tile
	/// on the path that will best lead a sprite to its destination. This can be used with
	/// findAngleToDestination to assist a sprite in using a highly modify A* alogrithm
	/// to find it's way without knowing the full path every iteration...
	///</summary>		
	Vector findBestVectorOnPath(Vector currentPosition, Vector destination, float currentAngle, int numberOfTilesAway, int skipTempCollisionID);
	
	///<summary>
	/// Performs some very basic heuristic pathfinding analysis to obtain the angle at which a sprite
	/// would move to find the shortest path to destinationX and destinationY from x and y on the tilemap.
	/// Will currently assert if x or y are an edge tile (Either o, or the respective width / height of the tilemap)
	/// Since Swim Fishy Swim doesn't let sprites navigate to those points. It must always start from 1 tile into the
	/// map in any direction.
	///</summary>		
	virtual float findAngleToDestination(Vector currentPosition, float currentAngle, Vector destination, int skipCollisionID);
	
	///<summary>
	/// Returns the size details of the tilemap
	///</summary>		
	virtual TilemapSize getTilemapSize();

	///<summary>
	/// Adds a file/texture combination into the IMap object
	///</summary>	
	virtual bool loadTileMap(const char* fileName);
	
	///<summary>
	/// Sets a starting point vector that the map cannot move beyond. The ending point will be based upon the starting point + width + height of map
	///</summary>
	virtual void setMinimumBoundary(Vector minimumBoundary);
	
	///<summary>
	/// Translates all objects loaded in the map based on an xyz coordinate set
	///</summary>
	virtual void translateMap(TILEMAP_LAYER layer, Vector translation);
	
	///<summary>
	/// Gets the xyz vector of the map layer specified defining the upper left hand corner of the layer
	///</summary>
	virtual Vector getLocation(TILEMAP_LAYER layer);
	
	///<summary>
	/// Sets the velocity at which the map will move
	///</summary>
	virtual void setMapVelocity(TILEMAP_LAYER layer, Vector unitsPerSecond);
		
	///<summary>
	/// Returns true if the specified x,y coordinate is marked on the collision map
	/// if Moveable is true, it'll check for the moveable collision point too.
	///</summary>
	virtual bool checkForCollisionAt(Vector collisionOffset, int tempCollisionID);

	/// Returns true if the specified x,y coordinate is marked on the collision map
	/// Overloaded to send -1 for tempCollisionID...
	///</summary>
	virtual bool checkForCollisionAt(Vector collisionOffset);

	///<summary>
	/// Adds/updates a temporary collision element - which lasts until clearTempCollisionTable(), or removeTempCollision is called
	///</summary>
	virtual void setTempCollisionAt(int collisionID, Vector collisionOffset);	

	///<summary>
	/// Returns the z value at the specified location in the tilemap
	/// using bilinear interpolation between the tilemap verticies...
	///</summary>
	float getZValueAt(Vector &location);

	///<summary>
	/// Removes a temporary collision element by the ID it was created under ...
	///</summary>
	virtual void removeTempCollision(int collisionID);	

	///<summary>
	/// Clears out any temporary collisions that have been added to the tilemap
	///</summary>
	virtual void clearTempCollisionTable();
	
	///<summary>
	/// Generates a display list to use for the specified map layer...
	///</summary>	
	virtual void generateLayerVertexArrays(IGraphics *graphics, TILEMAP_LAYER layer);

	///<summary>
	/// Frees memory associated with the vertex arrays
	///</summary>	
	virtual void freeLayerVertexArrays(IGraphics *graphics, TILEMAP_LAYER layer);

	///<summary>
	/// Renders the display list for the specified map layer
	///</summary>	
    virtual void renderLayerVertexArrays(IGraphics *graphics, TILEMAP_LAYER layer);
	
	///<summary>
	/// Is in charge of handling all changes associated with tile animation over a period of time
	///</summary>
	virtual void animationTask(TILEMAP_LAYER layer);

	///<summary>
	/// Is in charge of handling all changes associated with map velocity over a period of time
	///</summary>
	virtual void velocityTask();

	///<summary>
	/// Returns the current motion details for a layer so that they can be applied to objects outside of the tilemap...
	///</summary>
	virtual LayerMotion getLayerMotion(TILEMAP_LAYER layer);

	///<summary>
	/// Sets a time during which the map will randomly shake back and forth...
	///</summary>
	virtual void shakeMapFor(float timeUnits, float magnitude);

	///<summary>
	/// Gets the TextureID for the current water normal map (for use with projection and other features)
	///</summary>
	virtual TextureHandle getCurrentWaterNormalMap();

   private:   	
	
	// Specifies the width and height of our texture coordinates...
	TextureCoordinateSize myTextureCoordinateSize;
	
	// Defines the texture handle the graphics subsystem has indicated for us to use ...
	TextureHandle myTileTexture;	
	TextureHandle myWaterTexture;
	TextureHandle myCloudTexture;
	TextureHandle myWaterLightMap;	
	std::vector<TextureHandle> myWaterNormalMaps;
	int myCurrentWaterNormalMap;
   
	// Specifies the width and height of our tilemap
	TilemapSize myTilemapSize;
	
	// Specifies the minimum and maximum display coordinates of our tilemap
	Vector myMinimumBoundary;
	Vector myMaximumBoundary;	
	
	// specifies a timer used to delay animations
	float myAnimationTimer;
	
	// Used to count how many frames to skip before rendering water motion
	ushort myWaterAnimationCounter;

	// Some data we use in our tilemap code
	float myAnimationsPerSecond;	

	// If this value is > 0 then we'll shake the tilemap
	float myShakeTimer;
	float myShakeMagnitude;
	
	// This data severs as a container for each graphic and collision layer in the tilemap
	TilemapLayer myTilemapLayerA;
	TilemapLayer myTilemapLayerB;
	TilemapLayer myTilemapLayerC;
	TilemapLayer myTilemapLayerD;
	CollisionLayer myTilemapCollisionLayer;
	TilemapZValues myTilemapZValues;

	// stores a tilemap index for the moveable collision point...	
	TempCollisionTable myTempCollisionTable;	
};

#endif

/* SWIM FISHY SWIM TILEMAP FORMAT
	MAP DATA IS STORED AS BIG ENDIAN AUTOMATICALLY -> Although endian support will be incorporated

1. TilemapSize Data:
	short tilemapWidth;  (2 bytes)   - Describes the width of the actual tilemap in tiles
	short tilemapHeight; (2 bytes)	 - Describes the height of the actual tilemap in tiles
	short graphicWidth;  (2 bytes)	 - Descirbes the width of the graphic file in pixels
	short graphicHeight; (2 bytes)	 - Describes the height of the graphic file in pixels
	
2.  TilemapData Layer A:
	int mapIndexDataX[tilemapWidth * tilemapHeight]; - Describes the X graphic index for each tile in the tilemap
	int mapIndexDataY[tilemapWidth * tilemapHeight]; - Describes the Y graphic index for each tile in the tilemap
	(The above two arrays should be loaded into an vector of TextureIndex structs)
	
3.  TilemapData Layer B:
	int mapIndexDataX[tilemapWidth * tilemapHeight]; - Describes the X graphic index for each tile in the tilemap
	int mapIndexDataY[tilemapWidth * tilemapHeight]; - Describes the Y graphic index for each tile in the tilemap
	(The above two arrays should be loaded into an vector of TextureIndex structs)

4.  TilemapData Layer C:
	int mapIndexDataX[tilemapWidth * tilemapHeight]; - Describes the X graphic index for each tile in the tilemap
	int mapIndexDataY[tilemapWidth * tilemapHeight]; - Describes the Y graphic index for each tile in the tilemap
	(The above two arrays should be loaded into an vector of TextureIndex structs)

5.  TilemapData Layer D:
	int mapIndexDataX[tilemapWidth * tilemapHeight]; - Describes the X graphic index for each tile in the tilemap
	int mapIndexDataY[tilemapWidth * tilemapHeight]; - Describes the Y graphic index for each tile in the tilemap
	(The above two arrays should be loaded into an vector of TextureIndex structs)

5.  Collision Layer:
	bool mapCollisionData[tilemapWidth * tilemapHeight]; - Describes collisions by (x * y) indexing. true if collision on tile, false if not.

6.  Z Layer:
	QuadZValues mapZData[tilemapWidth * tilemapHeight]; - Describes z values as a set of 4 bytes for each tile... I know it should technically be for each corner... but this let's us get away with some normal looking tilemap combinations...

7.  That's the entire file!
	
*/
