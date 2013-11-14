// GlGraphics.h
// Implements IGraphics interface as it applieds to the OpenGL Library
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "GlGraphics.h"

// Function for claming a value :)
#define Clamp(val,min,max) \
	((val) = (((val) < (min)) ? (min) : ((val) > (max)) ? (max) : (val)))

#define LIT_X_ANGLE -55.0f        // Rotation around X for light.
#define LIT_Y_ANGLE 45.0f         // Rotation around Y for light.
#define LIGHT_MAP_RENDER_W 64
#define LIGHT_MAP_RENDER_H 64
#define LIGHT_MAP_WIDTH 64
#define LIGHT_MAP_HEIGHT 64

// Default format information for texture data 
#define COLUMN_FORMAT 6
#define TRANSPARENCY_COLOR 0xFFFFFFFF
#define ASPECT  (10.0f/7.0f)
#define FOVY    (45.0f)

// Texture coordinates used with a skybox
#define SKYBOX_WIDTH	0.25f
#define SKYBOX_HEIGHT	1.0f
#define SKYBOX_FRONT	0.0f
#define SKYBOX_RIGHT	0.75f
#define SKYBOX_BACK		0.50f
#define SKYBOX_LEFT		0.25f
#define SKYBOX_SIZE		164.0f
#define SCENEBOX_SIZE	116.0f
#define SKYBOX_BOTTOM	20.0f

///<summary>
/// Constructor for the GLGraphics IGraphics implementation
/// Handles all the setup stuff...
///</summary>		
GlGraphics::GlGraphics()
{
}

///<summary>
/// Destructor for the GLGraphics IGraphics implementation
/// Handles all the shutdown stuff
///</summary>
GlGraphics::~GlGraphics()
{
}

///<summary>
/// This is used to dispose of the singleton object since the constructor and destructor aren't available ..
///</summary>
void GlGraphics::dispose()
{
}

///<summary>
/// Prepares a scene for rendering in the specified environment
///</summary>				
void GlGraphics::beginScene()
{
}

///<summary>
/// Whatever is drawn next (be it a single primitive, or be it a complex object)
/// is rotated on the axis specified and by the amount in the rotate object
///</summary>		
void GlGraphics::rotateNextObjectInScene(Rotate *rotate)
{
}

///<summary>
/// Adds a triangle based on the specified triangle definition to the scene
///</summary>		
void GlGraphics::addPointsToScene(Vector location, std::vector<Point> &points, int pixelSize, POINT_TEXTURE_STYLE textureStyle, TextureHandle textureID)
{
}

///<summary>
/// Adds a triangle based on the specified triangle definition to the scene
///</summary>		
void GlGraphics::addTriangleToScene(Vector location, Triangle triangle)
{
}

///<summary>
/// Adds multiple triangles to the scene, including their texture ID
///</summary>		
void GlGraphics::addTrianglesToScene(Vector location, std::vector<Triangle> &triangles, int count, TextureHandle textureID)
{
}

///<summary>
/// Adds quads based on the specified quad definitions to the scene
///</summary>		
void GlGraphics::addQuadsToScene(Vector location, std::map<int, Quad> &quads, int count, TextureHandle textureID)
{
}

///<summary>
/// Handles all the rendering of a skybox, all it needs is a texture ...
///</summary>					
void GlGraphics::addSkyboxToScene(PROJECTION_TYPE projectionType, TextureHandle skyboxID, TextureHandle sceneboxID)
{
}

///<summary>
/// Returns SEEN_BY_CAMERA value if the vector is in the camera frustum
///</summary>							
SEEN_BY_CAMERA GlGraphics::isLocationWithinViewingRange(Vector location)
{
	// Make sure the image is within our left and right frustum planes
	// and if it's not, describe how far away it is to the rest of the system...

	// Just use the left and right planes for right now...
	float distanceFromLeftPlane = ((myFrustumPlanes[LEFT_PLANE].A * location.x)
			+ (myFrustumPlanes[LEFT_PLANE].B * location.y)
			+ (myFrustumPlanes[LEFT_PLANE].C * location.z)
			+ (myFrustumPlanes[LEFT_PLANE].D));

	float distanceFromRightPlane = ((myFrustumPlanes[RIGHT_PLANE].A * location.x)
				+ (myFrustumPlanes[RIGHT_PLANE].B * location.y)
				+ (myFrustumPlanes[RIGHT_PLANE].C * location.z)
				+ (myFrustumPlanes[RIGHT_PLANE].D));

	// Check to see if an object is outside of our left or right planes. If so, then we don't draw it...
	if ((distanceFromLeftPlane < -22) || (distanceFromRightPlane < -22))
	{
		return SEEN_BY_CAMERA_NO;
	}

	// Check the distance of an object so we can determine what level of detail should be used ...
	float distanceFromNearPlane = ((myFrustumPlanes[NEAR_PLANE].A * location.x)
					+ (myFrustumPlanes[NEAR_PLANE].B * location.y)
					+ (myFrustumPlanes[NEAR_PLANE].C * location.z)
					+ (myFrustumPlanes[NEAR_PLANE].D));

	// Check to see how far away an object is. If it's too far, we don't draw it...
	if (distanceFromNearPlane < -22)
	{
		return SEEN_BY_CAMERA_NO;
	}

	// Ok, we know it's within our viewing frustrum, but just how far away is the sucka?
	if (distanceFromNearPlane <= 152)
	{
		return SEEN_BY_CAMERA_NEAR;
	}
	else if ((distanceFromNearPlane > 152) && (distanceFromNearPlane <= 256))
	{
		return SEEN_BY_CAMERA_MEDIUM;
	}
	else if ((distanceFromNearPlane > 256) && (distanceFromNearPlane <= 384))
	{
		return SEEN_BY_CAMERA_FAR;
	}

	return SEEN_BY_CAMERA_NO;
}

///<summary>
/// Adds quads from a map based on a row size and then an x/y rectange
/// TODO: Once vertex arrays are implemented this procedure should become deprecated ...
///</summary>					
void GlGraphics::addQuadMapToScene(Vector location, std::map<int, Quad> &quads, int xMin, int xMax, int yMin, int yMax, int rowSize, TextureHandle textureID)
{
}

///<summary>
/// Allocates and configures a vertex array for use with the GPU ... 
/// TODO: Overload these procedures to support triangles etc.
///</summary>							
void GlGraphics::allocateAndPopulateVertexArray(std::map<int, Quad> &quads, QuadVertexArray &vertexArray, QuadVertexArrayProperties arrayProperties)
{
}

///<summary>
/// Deallocates vertex array ...
///</summary>							
void GlGraphics::freeVertexArray(QuadVertexArray &vertexArray)
{
}

///<summary>
/// This routine will render a triangle vertex array into a scene just like addTrianglessToScene etc.
///</summary>
void GlGraphics::addNormalMappedVertexArrayToScene(BUMP_TYPE bumpType, Vector location, TextureHandle textureID, TextureHandle lightMapID, TextureHandle normalMapID,
									   TriangleVertexArray &vertexArray)
{
}

///<summary>
/// This routine will render a quad vertex array into a scene just like addQuadsToScene, and apply a light and normal map for EMBM effects...
///</summary>						
void GlGraphics::addNormalMappedVertexArrayToScene(BUMP_TYPE bumpType, Vector location, TextureHandle textureID, TextureHandle lightMapID, TextureHandle normalMapID,
									   QuadVertexArray &vertexArray)
{
}

///<summary>
/// Updates the Mirror texture map by placing a camera at mirrorPosition, and pointing it at mirrorTarget...
///</summary>
void GlGraphics::updateMirrorReflection(Vector mirrorPosition, Vector mirrorTarget)
{
}

///<summary>
/// Allocates and configures a vertex array for use with the GPU ... 
/// IMPORTANT: If vertexArray.Normals == NULL then it will allocate normal data, but if it's not
/// then this method WILL NOT ALLOCATE NORMAL DATA! (The idea is that the normal data will be pointing
/// to preallocated normal data to save space & reduce cache misses...
///</summary>							
void GlGraphics::allocateAndPopulateVertexArray(std::vector<Triangle> &triangles, TriangleVertexArray &vertexArray, bool cullBacks)
{
}

///<summary>
/// Deallocates triangle vertex array ...
///</summary>							
void GlGraphics::freeVertexArray(TriangleVertexArray &vertexArray)
{
}

///<summary>
/// This routine will render a quad vertex array into a scene just like addTrianglesToScene etc.
///</summary>						
void GlGraphics::addVertexArrayToScene(Vector location, TextureHandle textureID, TriangleVertexArray &vertexArray)
{
}


///<summary>
/// Returns the current number of frames being rendered per second...
///</summary>							
int GlGraphics::getFPS()
{
	return 0;
}

///<summary>
/// Similar to guLookAt, this will allow us to move our i around as the game continues...
///</summary>								
void GlGraphics::setCameraPosition(Camera camera)
{
}

///<summary>
/// Similar to setCameraPosition, however this focuses only on the the x,y,z vector we want our camera to point at
///</summary>								
void GlGraphics::setCameraTarget(Vector target, float targetAngle, float heightOfScenery)
{
}

///<summary>
/// Defines the angle of the camera as it's viewing the player ...
///</summary>								
float GlGraphics::getCameraAngle()
{
	return myCamera.angleFromLook;
}

///<summary>
/// Sets the mode used for rendering fog
///</summary>								
void GlGraphics::setFogMode(f32 startZ, f32 endZ, f32 nearZ, f32 farZ, Color color)
{
}

///<summary>
/// Renders all of the primitives previously defined in the scene
///</summary>				
void GlGraphics::endScene()
{
}

///<summary>
/// Switches to overhead view for drawing the status display
///</summary>
void GlGraphics::switchToOverheadView()
{
}

///<summary>
/// Switches back to camera view if we were in overhead view
///</summary>
void GlGraphics::switchBackToCameraView()
{
}

///<summary>
/// Adjusts the current radius of the camera by the specified value
///</summary>								
void GlGraphics::adjustCameraRadius(float adjustBy)
{
	myCamera.radius += adjustBy * 0.1f;
}

///<summary>
/// returns the current radius of the camera
///</summary>								
float GlGraphics::getCameraRadius()
{
	return myCamera.radius;
}

///<summary>
/// Adjusts the current radius of the camera by the specified value
///</summary>								
void GlGraphics::adjustCameraDistance(float adjustBy)
{
	myCamera.distance += adjustBy  * 0.1f;
}

///<summary>
/// returns the current radius of the camera
///</summary>								
float GlGraphics::getCameraDistance()
{
	return myCamera.distance;
}

///<summary>
/// returns a vector with the current camera location
///</summary>								
Vector GlGraphics::getCameraLocation()
{
	return myCamera.pos;
}