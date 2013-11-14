// Defines a common interface for graphical controller implementations.
// Author(s): Eagan Rackley
#ifndef IGRAPHICS_H
#define IGRAPHICS_H

#include <map>
#include <list>
#include <vector>
#include "SFSHeaders.h"

#define UNDEFINED_TEXTURE_HANDLE 0xFFFFFFFF

/// <summary>
/// Defines a vector for fractional 8bit fixed numbers
/// </summary>
typedef struct tagFixedVector
{
	unsigned char x;
	unsigned char y;
	unsigned char z;
} fixedVector;

/// <summary>
/// Defines four points of a frustum plane
/// </summary>
typedef struct tagFrustumPlane
{
	f32 A;
	f32 B;
	f32 C;
	f32 D;
}FrustumPlane;

/// <summary>
/// Defines an rgb color that can be associated with a Vector
/// </summary>
typedef struct tagColor
{
	float r;
	float b;
	float g;
} Color;

typedef unsigned int u32;

/// <summary>
/// Defines a handle for reference at texture in the graphics interface
/// </summary>
typedef u32 TextureHandle;

/// <summary>
/// Defines a handle for reference to a display list graphics interface.
/// Each display list is associated with a model name so that
/// Multiple models can reuse the exact same frames as needed
/// as they are interpolated
/// </summary>
typedef u32 DisplayListHandle;

/// <summary>
/// Defines how textures are displayed on a point ...
/// </summary>
#define NUMBER_OF_TEXTURE_STYLES 2

/// <summary>
/// Defines how point textures are displayed using the graphics engine
/// </summary>
typedef enum 
{
	USE_WHOLE_TEXTURE		= 0x001,
	USE_HALF_TEXTURE		= 0x002,
	USE_QUARTER_TEXTURE		= 0x003,
	USE_EIGHTH_TEXTURE		= 0x004
} POINT_TEXTURE_STYLE;

///<summary>
/// Returned by functions evaluating whether or not a bounding volume is in the view frustrum or not...
///</summary>
typedef enum
{
	SEEN_BY_CAMERA_NEAR,
	SEEN_BY_CAMERA_MEDIUM,
	SEEN_BY_CAMERA_FAR,
	SEEN_BY_CAMERA_NO
} SEEN_BY_CAMERA;


/// <summary>
/// Defines the types of bump mapping supported by the system ...
/// </summary>
typedef enum
{
	BUMP_TYPE_EMBOSS_MAP,
	BUMP_TYPE_NORMAL_MAP,
	BUMP_TYPE_XYZ_MAP
} BUMP_TYPE;

/// <summary>
/// Defines the type of projection matrix we want to use for a scene...
/// </summary>
typedef enum
{
	PROJECTION_NORMAL,
	PROJECTION_REFLECTION
} PROJECTION_TYPE;

/// <summary>
/// Defines how a texture will be used by an IGraphics implementation
/// </summary>
typedef enum 
{
	IMAGE_TEXTURE				= 0x001,
	HEIGHT_TEXTURE				= 0x002,	
	LIGHT_MAP_TEXTURE			= 0x003,
	NORMAL_MAP_TEXTURE			= 0x004,
	REFLECTION_TEXTURE			= 0x005,
	MULTIPASS_IMAGE_TEXTURE		= 0x006,
	LIGHT_MAP_TEXTURE_TEMP		= 0x007
} TEXTURE_FORMAT;


/// <summary>
/// Defines what constitutes the definition of a point (this is a square that can be textured but always faces forward)
/// </summary>
typedef struct tagPoint
{
	Vector point;
	float texture_S;
	float texture_T;	
} Point;

/// <summary>
/// Defines what constitutes the definition of a triangle
/// </summary>
typedef struct tagTriangle
{
	Vector verticies[3];	
	Vector normals[3];
	float texture_S[3];
	float texture_T[3];
} Triangle;

/// <summary>
/// Defines what constitutes the definition of a Quad
/// </summary>
typedef struct tagQuad
{
	///<summary>Represents the x coordinate of a tile to be pulled from the tilemap graphic</summary>
	int texIndexX;
	///<summary>Represents the y coordinate of a tile to be pulled from the tilemap graphic</summary>
	int texIndexY;
	///<summary>The four x,y,z locations of this particular quad...</summary>
	Vector verticies[4];			
	///<summary>Represents the actual S texture coordinates when we're not using a vertex array...</summary>
	float texture_S[4];
	///<summary>Represents the actual T texture coordinates when we're not using a vertex array...</summary>
	float texture_T[4];
	///<summary>Represents a normal lighting value for each corner of the quad...</summary>
	Vector normals[4];	
} Quad;

/// <summary>
/// Defines a set of properties used to allocate a vertex array for accessing quad data...
/// </summary>
typedef struct tagQuadVertexArrayProperties
{
	///<summary>The number of quads in a row of the quad map</summary>
	short quadsWide;
	///<summary>The number of quads in a column of the quad map</summary>
	short quadsHigh;
	///<summary>The standard width used to generate texture coordinates for a quad (all tiles on a quadmap are the same width)</summary>
	float quadTexWidth;
	///<summary>The standard height used to generate texture coordinates for a quad (all tiles on a quadmap are the same height)</summary>
	float quadTexHeight;
	///<summary>The number of tiles wide the texture for the quad map is (e.g. 512 pixels would be 16 wide if the tiles are 32 pixels wide)</summary>
	int   texTilesWide;
	///<summary>The number of tiles high the texture for the quad map is (e.g. 640 pixels would be 20 high if the tiles are 32 pixels wide)</summary>
	int   texTilesHigh;
	///<summary>Specifies for a quad map whether or not to cull backfaces when rendering...</summary>
	bool cullBacks;
} QuadVertexArrayProperties;

/// <summary>
/// Defines a vertex array we can use for rendering our quad ...
/// </summary>
typedef struct tagVertexArray
{
	QuadVertexArrayProperties properties;
	size_t VertexArraySize;
	size_t VertexStride;
	size_t TextureStride;
	size_t NormalStride;
	Vector *Verticies;
	float *TextureCoordsST;
	short *TexCoordLookup;
	fixedVector *Normals;
} QuadVertexArray;

/// <summary>
/// Defines a vertex array we can use for rendering our quad ...
/// </summary>
typedef struct tagTriangleVertexArray
{
	bool CullTriangleBacks;
	size_t VertexArraySize;	
	size_t VertexStride;
	size_t TextureStride;
	size_t NormalStride;
	Vector *Verticies;
	float *TextureCoordsST;	
	fixedVector *Normals;
} TriangleVertexArray;

/// <summary>
/// Defines the offsets required when rendering a vertex array ...
/// </summary>
typedef struct tagVertexArrayOffsets
{
	u32 Base;
	u32 Binormal;
	u32 Tangent;
	u32 Texture;
} VertexArrayOffsets;


///<summary>
/// Defines vectors for the camera position, view, and which axis is up...
///</summary>						
typedef struct tagCamera 
{
	Vector pos;
	Vector up;
	Vector look;
	Vector target;
	float rotation;
	float radius;
	float distance;
	float angleFromLook;
} Camera;

///<summary>
/// Defines the axis used and the value used to perform a rotate transform on a rendered object..
///</summary>						
typedef struct tagRotate
{
	Vector axis;
	float value;
} Rotate;


class IGraphics
{ 
public:

	///<summary>
	/// Prepares a scene for rendering in the specified environment
	///</summary>				
	virtual void beginScene() = 0;

	///<summary>
	/// Renders all of the primitives previously defined in the scene
	///</summary>				
	virtual void endScene() = 0;		

	///<summary>
	/// Whatever is drawn next (be it a single primitive, or be it a complex object)
	/// is rotated on the axis specified and by the amount in the rotate object
	///</summary>		
	virtual void rotateNextObjectInScene(Rotate *rotate) = 0;

	///<summary>
	/// Adds a triangle based on the specified triangle definition to the scene
	///</summary>		
	virtual void addPointsToScene(Vector location, std::vector<Point> &points, int pixelSize, POINT_TEXTURE_STYLE textureStyle, TextureHandle textureID) = 0;

	///<summary>
	/// Adds a triangle based on the specified triangle definition to the scene
	///</summary>		
	virtual void addTriangleToScene(Vector location, Triangle triangle) = 0;		

	///<summary>
	/// Adds triangles based on the specified triangle definitions to the scene
	///</summary>		
	virtual void addTrianglesToScene(Vector location, std::vector<Triangle> &triangles, int count, TextureHandle textureID) = 0;

	///<summary>
	/// Adds quads based on the specified quad definitions to the scene
	///</summary>		
	virtual void addQuadsToScene(Vector location, std::map<int, Quad> &quads, int count, TextureHandle textureID) = 0;

	///<summary>
	/// Adds quads from a map based on a row size and then an x/y rectange
	///</summary>					
	virtual void addQuadMapToScene(Vector location, std::map<int, Quad> &quads, int xMin, int xMax, int yMin, int yMax, int rowSize, TextureHandle textureID) = 0;

	///<summary>
	/// Handles all the rendering of a skybox, all it needs is a texture ...
	///</summary>					
	virtual void addSkyboxToScene(PROJECTION_TYPE projectionType, TextureHandle skyboxID, TextureHandle sceneboxID) = 0;

	///<summary>
	/// Allocates and configures a vertex array for use with the GPU ... 
	///</summary>							
	virtual void allocateAndPopulateVertexArray(std::map<int, Quad> &quads, QuadVertexArray &vertexArray, QuadVertexArrayProperties arrayProperties) = 0;

	///<summary>
	/// Returns SEEN_BY_CAMERA value if the vector is in the camera frustum
	///</summary>							
	virtual SEEN_BY_CAMERA isLocationWithinViewingRange(Vector location) = 0;

	///<summary>
	/// Deallocates vertex array ...
	///</summary>							
	virtual void freeVertexArray(QuadVertexArray &vertexArray) = 0;

	///<summary>
	/// This routine will render a triangle vertex array into a scene just like addTrianglessToScene etc.
	///</summary>						
	virtual void addNormalMappedVertexArrayToScene(BUMP_TYPE bumpType, Vector location, TextureHandle textureID, TextureHandle lightMapID, TextureHandle normalMapID, TriangleVertexArray &vertexArray) = 0;

	///<summary>
	/// This routine will render a quad vertex array into a scene just like addQuadsToScene, and apply a light and normal map for EMBM effects...
	///</summary>						
	virtual void addNormalMappedVertexArrayToScene(BUMP_TYPE bumpType, Vector location, TextureHandle textureID, TextureHandle lightMapID, TextureHandle normalMapID, QuadVertexArray &vertexArray) = 0;

	///<summary>
	/// Updates the Mirror texture map by placing a camera at mirrorPosition, and pointing it at mirrorTarget...
	///</summary>
	virtual void updateMirrorReflection(Vector mirrorPosition, Vector mirrorTarget) = 0;

	///<summary>
	/// Allocates and configures a vertex array for use with the GPU ... 
	/// IMPORTANT: If vertexArray.Normals == NULL then it will allocate normal data, but if it's not
	/// then this method WILL NOT ALLOCATE NORMAL DATA! (The idea is that the normal data will be pointing
	/// to preallocated normal data to save space & reduce cache misses...)
	///</summary>							
	virtual void allocateAndPopulateVertexArray(std::vector<Triangle> &triangles, TriangleVertexArray &vertexArray, bool cullBacks) = 0;

	///<summary>
	/// Deallocates triangle vertex array ...
	///</summary>							
	virtual void freeVertexArray(TriangleVertexArray &vertexArray) = 0;

	///<summary>
	/// This routine will render a quad vertex array into a scene just like addTrianglesToScene etc.
	///</summary>						
	virtual void addVertexArrayToScene(Vector location, TextureHandle textureID, TriangleVertexArray &vertexArray) = 0;

	///<summary>
	/// Returns the current number of frames being rendered per second...
	///</summary>							
	virtual int getFPS() = 0;

	///<summary>
	/// Similar to guLookAt, this will allow us to move our camera around as the game continues...
	///</summary>								
	virtual void setCameraPosition(Camera camera) = 0;

	///<summary>
	/// Similar to setCameraPosition, however this focuses only on the the x,y,z vector we want our camera to point at
	///</summary>								
	virtual void setCameraTarget(Vector target, float targetAngle, float heightOfScenery) = 0;

	///<summary>
	/// Defines the angle of the camera as it's viewing the player ...
	///</summary>								
	virtual float getCameraAngle() = 0;

	///<summary>
	/// Adjusts the current radius of the camera by the specified value
	///</summary>								
	virtual void adjustCameraRadius(float adjustBy) = 0;

	///<summary>
	/// returns the current radius of the camera
	///</summary>								
	virtual float getCameraRadius() = 0;

	///<summary>
	/// Adjusts the current radius of the camera by the specified value
	///</summary>								
	virtual void adjustCameraDistance(float adjustBy) = 0;

	///<summary>
	/// returns the current radius of the camera
	///</summary>								
	virtual float getCameraDistance() = 0;

	///<summary>
	/// returns a vector with the current camera location
	///</summary>								
	virtual Vector getCameraLocation() = 0;

	///<summary>
	/// Sets the mode used for rendering fog
	///</summary>								
	virtual void setFogMode(float startZ, float endZ, float nearZ, float farZ, Color color) = 0;

	///<summary>
	/// clears the current texture data in video memory
	///</summary>
	virtual void clearTextureData() = 0;

	///<summary>
	/// Loads a texture file into video memory, formatting it appropriately
	///</summary>
	///<returns>A handle indicating the texture ID</returns>
	virtual TextureHandle addTextureData(const char* fileName, TEXTURE_FORMAT format, bool wrap) = 0;

	///<summary>
	/// Removes allocated texture data from memory
	///</summary>
	virtual void freeTextureData(TextureHandle handle) = 0;

	///<summary>
	/// Switches to overhead view for drawing the status display
	///</summary>
	virtual void switchToOverheadView() = 0;

	///<summary>
	/// Switches back to camera view if we were in overhead view
	///</summary>
	virtual void switchBackToCameraView() = 0;
};

#endif
