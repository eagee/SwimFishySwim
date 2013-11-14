// GxGraphics.h
// Implements IGraphics interface as it applieds to the GX GPU on the Wii
// Author(s): Eagan Rackley

#ifndef GX_GRAPHICS_H
#define GX_GRAPHICS_H

#include "IGraphics.h"

// Defines for calculating our frustum planes
#define TOTAL_FRUSTUM_PLANES 6
#define RIGHT_PLANE 	0
#define LEFT_PLANE 		1
#define BOTTOM_PLANE 	2
#define TOP_PLANE 		3
#define FAR_PLANE 		4
#define NEAR_PLANE 		5

	///<summary>
	/// Defines a texture object and texture data for rendering textures
	///</summary>
	typedef struct tagGxTextureData
	{		
		GXTexObj textureObject;
		void *textureData;
		int width;
		int height;
		int bytesPerTexel;
		int size;
		bool wrap;
		TEXTURE_FORMAT format;
		u32 textureMap;
	} GxTextureData;

	///<summary>
	/// IGraphics implementation for Wii GX
	///</summary>	
	class GxGraphics: public IGraphics
	{ 
		public:	
			
			///<summary>
			/// Constructor for the GXGraphics IGraphics implementation
			/// Handles all the setup stuff...
			///</summary>		
			GxGraphics();
			
			///<summary>
			/// Destructor for the GXGraphics IGraphics implementation
			/// Handles all the shutdown stuff
			///</summary>
			~GxGraphics();
			
			///<summary>
			/// This is used to dispose of the singleton object since the constructor and destructor aren't available ..
			///</summary>
			void dispose();
							
			///<summary>
			/// Prepares a scene for rendering in the specified environment
			///</summary>				
			void beginScene();
			
			///<summary>
			/// Whatever is drawn next (be it a single primitive, or be it a complex object)
			/// is rotated on the axis specified and by the amount in the rotate object
			///</summary>		
			void rotateNextObjectInScene(Rotate *rotate);

			///<summary>
			/// Adds a triangle based on the specified triangle definition to the scene
			///</summary>		
			void addPointsToScene(Vector location, std::vector<Point> &points, int pixelSize, POINT_TEXTURE_STYLE textureStyle, TextureHandle textureID);

			///<summary>
			/// Adds a triangle based on the specified triangle definition to the scene
			///</summary>		
			void addTriangleToScene(Vector location, Triangle triangle);			
			
			///<summary>
			/// Adds multiple triangles to the scene, including their texture ID
			///</summary>		
			void addTrianglesToScene(Vector location, std::vector<Triangle> &triangles, int count, TextureHandle textureID);
			
			///<summary>
			/// Adds quads based on the specified quad definitions to the scene
			///</summary>		
			void addQuadsToScene(Vector location, std::map<int, Quad> &quads, int count, TextureHandle textureID);
			
			///<summary>
			/// Handles all the rendering of a skybox, all it needs is a texture ...
			///</summary>					
			void addSkyboxToScene(PROJECTION_TYPE projectionType, TextureHandle skyboxID, TextureHandle sceneboxID);

			///<summary>
			/// Returns SEEN_BY_CAMERA value if the vector is in the camera frustum
			///</summary>							
			virtual SEEN_BY_CAMERA isLocationWithinViewingRange(Vector location);

			///<summary>
			/// Adds quads from a map based on a row size and then an x/y rectange
			/// TODO: Once vertex arrays are implemented this procedure should become deprecated ...
			///</summary>					
			void addQuadMapToScene(Vector location, std::map<int, Quad> &quads, int xMin, int xMax, int yMin, int yMax, int rowSize, TextureHandle textureID);
			
			///<summary>
			/// Allocates and configures a vertex array for use with the GPU ... 
			/// TODO: Overload these procedures to support triangles etc.
			///</summary>							
			void allocateAndPopulateVertexArray(std::map<int, Quad> &quads, QuadVertexArray &vertexArray, QuadVertexArrayProperties arrayProperties);

			///<summary>
			/// Deallocates vertex array ...
			///</summary>							
			void freeVertexArray(QuadVertexArray &vertexArray);

			///<summary>
			/// This routine will render a triangle vertex array into a scene just like addTrianglessToScene etc.
			///</summary>
			void addNormalMappedVertexArrayToScene(BUMP_TYPE bumpType, Vector location, TextureHandle textureID, TextureHandle lightMapID, TextureHandle normalMapID,
					TriangleVertexArray &vertexArray);

			///<summary>
			/// This routine will render a quad vertex array into a scene just like addQuadsToScene, and apply a light and normal map for EMBM effects...
			///</summary>						
			void addNormalMappedVertexArrayToScene(BUMP_TYPE bumpType, Vector location, TextureHandle textureID, TextureHandle lightMapID, TextureHandle normalMapID,
					QuadVertexArray &vertexArray);

			///<summary>
			/// Updates the Mirror texture map by placing a camera at mirrorPosition, and pointing it at mirrorTarget...
			///</summary>
			void updateMirrorReflection(Vector mirrorPosition, Vector mirrorTarget);

			///<summary>
			/// Allocates and configures a vertex array for use with the GPU ... 
			/// IMPORTANT: If vertexArray.Normals == NULL then it will allocate normal data, but if it's not
			/// then this method WILL NOT ALLOCATE NORMAL DATA! (The idea is that the normal data will be pointing
			/// to preallocated normal data to save space & reduce cache misses...
			///</summary>							
			void allocateAndPopulateVertexArray(std::vector<Triangle> &triangles, TriangleVertexArray &vertexArray, bool cullBacks);

			///<summary>
			/// Deallocates triangle vertex array ...
			///</summary>							
			void freeVertexArray(TriangleVertexArray &vertexArray);

			///<summary>
			/// This routine will render a quad vertex array into a scene just like addTrianglesToScene etc.
			///</summary>						
			void addVertexArrayToScene(Vector location, TextureHandle textureID, TriangleVertexArray &vertexArray);


			///<summary>
			/// Returns the current number of frames being rendered per second...
			///</summary>							
			int getFPS();
		
			///<summary>
			/// Similar to guLookAt, this will allow us to move our i around as the game continues...
			///</summary>								
			void setCameraPosition(Camera camera);

			///<summary>
			/// Similar to setCameraPosition, however this focuses only on the the x,y,z vector we want our camera to point at
			///</summary>								
			void setCameraTarget(Vector target, float targetAngle, float heightOfScenery);

			///<summary>
			/// Defines the angle of the camera as it's viewing the player ...
			///</summary>								
			float getCameraAngle();
			
			///<summary>
			/// Sets the mode used for rendering fog
			///</summary>								
			void setFogMode(f32 startZ, f32 endZ, f32 nearZ, f32 farZ, Color color);
	
			///<summary>
			/// Renders all of the primitives previously defined in the scene
			///</summary>				
			void endScene();

			///<summary>
			/// Switches to overhead view for drawing the status display
			///</summary>
			void switchToOverheadView();

			///<summary>
			/// Switches back to camera view if we were in overhead view
			///</summary>
			void switchBackToCameraView();

			///<summary>
			/// Uses a preallocation method to allocate persistent memory to be used with a display list.
			/// On systems using an OS this memory will need to be disposed of.
			///</summary>							
			void *allocatePersistentMemoryForDisplayList(size_t memorySize);

			///<summary>
			/// Adjusts the current radius of the camera by the specified value
			///</summary>								
			void adjustCameraRadius(float adjustBy);

			///<summary>
			/// returns the current radius of the camera
			///</summary>								
			float getCameraRadius();

			///<summary>
			/// Adjusts the current radius of the camera by the specified value
			///</summary>								
			void adjustCameraDistance(float adjustBy);

			///<summary>
			/// returns the current radius of the camera
			///</summary>								
			float getCameraDistance();

			///<summary>
			/// returns a vector with the current camera location
			///</summary>								
			Vector getCameraLocation();

	private:							
			// Initializes a texture object
			void initTextureObject(GxTextureData &textureData);
	
			// Assigns a GXTexObj to a texture map for rendering ...			
			void setupTextureObject(int textureID);
			
			// Assigns a GXTexObj to a texture map for tesselating pixel z values ...						
			void setupHeightTextureObject(int textureID);

			// Handles Matrix Calculations for Rotation and Translation
			void handleMatrixCalculations(Vector location);

			///<summary>
			/// Set up our fifo and flipper for the GX unit ...
			///</summary>
			void setupSkyboxValues();


			///<summary>
			/// Set up our fifo and flipper for the GX unit ...
			///</summary>
			void setupFIFO();
			
			///<summary>
			/// I sure wish there was some clear documentation on what all this is doing...
			/// Some of it is common sense, some if it... no idea ...
			/// but suffice it to say, this helps set up our rendering environment...
			///</summary>
			void setRenderingParameters();

			///<summary>
			/// Sets up the the light matrix for use in the scene. Only needs to be done once.
			///</summary>			
			void setupLightingMatrix(float xAngle, float yAngle);
			

			///<summary>
			/// Sets up the scene rendering			
			///</summary>			
			void setupRenderingPass();
			void setupHeightRenderingPass();
			void setupEmbossedRenderingPass();
			void setupNormalMapRenderingPass();
			void setupXYZMapRenderingPass();

			///<summary>
			/// Sets up our projection matrix ...
			///</summary>
			void setupProjectionMatrix();
			
			///<summary>
			/// Handles rotation if one is pending ...
			///</summary>
			void checkObjectRotation();			
			
			///<summary>
			/// clears the current texture data in video memory
			///</summary>
			void clearTextureData();
		
			///<summary>
			/// Loads a texture file into video memory, formatting it appropriately and handling 
			/// width and height parameters from the file header
			///</summary>
			///<returns>A handle indicating the texture ID</returns>			
			TextureHandle addTextureData(const char* fileName, TEXTURE_FORMAT format, bool wrap);	

			///<summary>
			/// Removes allocated texture data from memory
			///</summary>
			void freeTextureData(TextureHandle handle);

			///<summary>
			/// Sets up basic scene lighting...
			///</summary>
			void enableLighting();			


			///<summary>
			/// Sets up basic scene lighting...
			///</summary>
			void disableLighting();			

			///<summary>
			/// Sets up the reflection texture that we'll use with normal mapping water tiles...
			///</summary>
			void setupReflectionTexture();

			///<summary>
			/// Sets up the light map texture that we'll use for lighting / bump mapping...
			///</summary>
			void setupLightMapTexture();

			///<summary>
			/// Builds our frustum planes for check to see if a point is in our camera view...
			///</summary>
			void updateFrustumPlanes();

			///<summary>
			///changes the GP breakpoint so CPU can calculate next frame
			///while GP processes last frame. should be called before any
			///commands are sent to GP (at the top of the main loop)
			///since this function assumes that both the CPU and GP are
			///not processing.
			///</summary>
			void HaveGPProcessFIFO();

			///<summary>
			/// Handles setup routines used in normal mapped vertex array rendering
			///</summary>
			void handleBumpMatrixAndRenderingSetup(BUMP_TYPE &bumpType, Vector &location);

			///<summary>
			/// Handles any setup associated with the texture maps used with bumped rendering
			///</summary>
			void handleBumpTextureSetup(TextureHandle &textureID, TextureHandle &lightMapID, TextureHandle &normalMapID);

			// This indicates the amount at which we should rotate if myNextObjectShouldRotate is set ....
			// and can include several routates for one set of triangles
			std::vector<Rotate> myNextRotateValues;
			
			// This is the camera associated with our graphics object ...			
			Camera myCamera;
			
			// Defines the y scale for our rendering environment
			f32 myYScale;
			
			// Defines our framebuffer height ...
			u32 myFrameBufferHeight;

			// References the current framebuffer ...
			u32	myCurrentFrameBuffer;
			
			// Points to our mode object... TODO: Define this a little better...
			GXRModeObj *myRegisterMode;
			
			// Here we define our matricies for view, perspective, model, and modelview
			// View Matrix - what the camera is pointing at
			Mtx44 myView;
			// Projection Matrix - how things are projected into screen coords
			Mtx44 myPerspective;  
			// model matrix
			Mtx44 myModel;
			// model view matrix
			Mtx44 myModelView;
			// temporary rotation matrix
			Mtx44 myTempRotate;
			// Used for texture projection (which we aint doint rite now)
			Mtx myTextureProjection;
			// Used to keep track of what our frustum is for culling on the model level...
			//Mtx44 myFrustumMatrix;
			// Used for working out those fine lighting effects :)
			Mtx myLightMatrix;
			
			// This variable actually stores the 6 planes of the frustum which helps us identify
			// whether or not a location/radius exists within the frustum...
			FrustumPlane myFrustumPlanes[TOTAL_FRUSTUM_PLANES];

			// Points to the fifo in the rendering pipeline.
			void *myGpFifoBuffer;
			GXFifoObj myFifo;
			
			// This is a list of all the Texture Coordinates in the md2 file
			TextureHandle myCurrentTextureID;
			typedef std::map<int, GxTextureData> GxTexMap;
			typedef GxTexMap::iterator GxTexMapIterator;
			GxTexMap myGxTextures;
			
			// Used to track our current FPS...
			int myFPS;
			int myFPSCounter;
			float myFPSTimeSpan;

			// Variables used to manage bump mapping
			float myPostTransMtxScale;
			float myPostTransMtxOffset;
			float myIndTexMtxFactor;			

			///<summary>
			/// Defines whether this is the first time the GX is drawing...
			///</summary>
			int myFirstGxFrame;

			///<summary>
			/// Defines textures specifically associated with and generated by the graphics code
			/// for reflections and lightmaps...
			///</summary>
			GxTextureData myReflectionTexture;
			TextureHandle myReflectionTexHandle;
			GxTextureData myLightMapTexture;
			TextureHandle myLightMapTexHandle;
			
			
			///<summary>
			/// Defines data used to manage light in the engine
			///</summary>
			GXLightObj mySpecLight;
			GXLightObj mySpotLight;
			float mySpecularTheta;
			float mySpecularPhi;
			Vector myLightPosition;
			Vector myLightDirection;
			GXColor myAmbientColor0;
			GXColor myMaterialColor0;
			GXColor myAmbientColor1;
			GXColor myMaterialColor1;

			// These values are used to build our skybox arrays
			Vector *mySkyboxCoords;
			Vector *mySceneboxCoords;
			float *mySkyboxTextures;
			Vector *mySkyboxNormals;
			Rotate mySkyboxRotation;
	};

#endif
