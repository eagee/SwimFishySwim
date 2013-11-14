// Model.h
// Defines Interface for loading and displaying 3d model information...
// Author(s): Eagan Rackley
#ifndef ModelMd2_H
#define ModelMd2_H

#include <time.h>
#include <vector>
#include "IGraphics.h"
#include "IModel.h"


#define TOTAL_MD2_ANIMATIONS 25
#define NUMBER_OF_NORMALS		162
#define NUMBER_OF_DOT_PRODUCTS	16


typedef struct
{
	// magic number. must be equal to "IDP2"
	u32     magicNumber;              
	// md2 version. must be equal to 8
	u32     md2Version;            
	// width of the texture
	u32     skinwidth;          
	// height of the texture
	u32     skinheight;         
	// size of one frame in bytes
	u32     framesize;          
	// number of textures
	u32     numSkins;          
	// number of vertices in a frame
	u32     numVertices;            
	// number of texture coordinates
	u32     numTexCoords;             
	// number of triangles in the file
	u32     numTriangles;           
	// number of opengl commands
	u32     numGlCommands;         
	// total number of frames in the file
	u32     numFrames;         
	// offset to skin names (64 bytes each)
	u32     skinOffset;          
	// offset to s-t texture coordinates
	u32     texCoordsDataOffset;             
	// offset to triangles
	u32     triangleDataOffset;           
	// offset to frame data
	u32     frameDataOffset;         
	// offset to opengl commands
	u32     glCommandDataOffset;         
	// offset to end of file
	u32     EOFoffset;
} md2HeaderData;

///<summary>
/// Defines a vector as stored in the md2 file (float data compressed into 8 bits)
///</summary>
typedef struct
{
    unsigned char   v[3];                // compressed vertex (x, y, z) coordinates
    unsigned char   lightnormalindex;    // index to a normal vector for the lighting

} md2Vertex;

/* Triangle info */
typedef struct
{
  unsigned short vertex[3];
  unsigned short st[3];
} md2Triangle;


///<summary>
/// Defines texture coordinates as stored in the md2 file (float data compressed into 16 bits)
///</summary>
typedef struct
{
    short    s;
    short    t;
} md2TexCoord;

#define NUMBER_OF_SCALES_IN_FRAME 3
#define NUMBER_OF_TRANSLATES_IN_FRAME 3
#define NUMBER_OF_CHARS_IN_FRAME 16

///<summary>
/// Defines frame data as stored in the md2 file (float data compressed into 16 bits)
///</summary>
typedef struct
{
	// scale values
	float		scale[NUMBER_OF_SCALES_IN_FRAME];		
	// translation vector
	float		translate[NUMBER_OF_TRANSLATES_IN_FRAME];	
	// frame name
	char		name[NUMBER_OF_CHARS_IN_FRAME];		
	// List of verticies in this frame
	std::vector<md2Vertex>	verts;					
} md2Frame;

///<summary>
/// Defines animation data as stored in the md2 file (float data compressed into 16 bits)
///</summary>
typedef struct
{
	int		firstFrame;			// first frame of the animation
	int		lastFrame;				// number of frames
	int		fps;					// number of frames per second

} md2Animation;

///<summary>
/// Defines a list of triangles used to represent a single frame
///</summary>
typedef std::vector<Triangle> ScaledFrame;
typedef std::vector<ScaledFrame> ScaledMd2Frames;
typedef std::vector<TriangleVertexArray> VertexFrame;



///<summary>
/// Definies an implementation of the md2 model format - perfect for flat fish, right?
///</summary>
class ModelMd2: public IModel
   {
   public:
		/// <summary>
		/// Constructor for the Md2 Model
		/// </summary>
		ModelMd2();
      
		/// <summary>
		/// Destructor for the Md2 Model
		/// </summary>
		~ModelMd2();
	  
		/// <summary>
		/// Will load the necessary md2 data from a file ...
		/// </summary>
		bool loadFromFile(const char *fileName, AnimationFormat animationFormat);
		
		/// <summary>
		/// Will translate model to vector coordinates
		/// </summary>
		void translateModel(Vector translation);
		
		/// <summary>
		/// Decides what type of animation to render ...
		/// </summary>		
		void setAnimationType(AnimationType type);

		/// <summary>
		/// Decides what type of animation to render, allowing an external animation state to be specified
		/// </summary>		
		void setAnimationType(AnimationType type, ModelAnimState &modelAnim);
		
		/// <summary>
		/// Some models r big, some models r small, this hels determine which one that is ...
		/// </summary>				
		void setModelScale(float scale);

		/// <summary>
		/// Will build vertex arrays from our Scaled frames for faster rendering, and indicate whether we want to cull backfaces in our model or not...
		/// </summary>				
		void buildVertexArrays(IGraphics *graphics, bool cullBacks);

		/// <summary>
		/// Cleans up our vertex arrays...
		/// </summary>				
		void freeVertexArrays(IGraphics *graphics);

		/// <summary>
		/// Can be called after begin scene has been called, it will render the model in the scene allowing you to specify an animation state
		/// </summary>
		void renderModel(IGraphics *graphics, ModelAnimState &animation, bool interpolate);

		/// <summary>
		/// Can be called after begin scene has been called, it will render the model in the scene, using it's own inernal animation state...
		/// </summary>
		void renderModel(IGraphics *graphics, bool interpolate);

		/// <summary>
		/// Handles all the logic necessary for animating this model ...
		/// </summary>
		void handleAnimation(float time, ModelAnimState &modelAnim);
		
	private:
	
		/// <summary>
		/// Private procedure for opening the file handle specified ...
		/// </summary>
		/// <returns>true if successful, else false </returns>
		bool openMyFileHandle(const char *fileName);
		
		/// <summary>
		/// Private procedure for opening the texture file handle specified ...
		/// </summary>
		/// <returns>true if successful, else false </returns>
		bool openMyTextureFileHandle(const char *fileName);
		
		/// <summary>
		/// Private procedure to load md2 header data from a file and swap the endian-ness (if need be)
		/// </summary>
		/// <returns> <c>true</c> [success] else, <c>false</c></returns>
		bool loadMd2HeaderDataFromFile();
		
		/// <summary>
		/// Private procedure to load md2 frame data from a file 
		/// </summary>		
		/// <returns>[Successfully loaded] true, else false</returns>
		bool loadMd2FrameDataFromFile();		
		
		/// <summary>
		/// Private procedure to load md2 triangle data from a file 
		/// </summary>		
		/// <returns>[Successfully loaded] true, else false</returns>
		bool loadMd2TriangleDataFromFile();		
		
		/// <summary>
		/// Private procedure to load md2 texture coordinate data from a file 
		/// </summary>		
		/// <returns>[Successfully loaded] true, else false</returns>
		bool loadMd2TextureCoordinatesFromFile();		

		/// <summary>
		/// Get Current Triangle used in getInterpolatedTriangle
		/// </summary>
		Triangle getCurrentTriangle(int triangleIndex);
		
		/// <summary>
		/// Gets the Current Triangle scaled for rendering...
		/// </summary>
		Triangle getCurrentScaledTriangle(int triangleIndex);
		
		/// <summary>
		/// Get the Next Triangle used in getInterpolatedTraingle
		/// </summary>
		Triangle getNextTriangle(int triangleIndex);
		
		/// <summary>
		/// Get an interpolated Triangle using getCurrentTriangle and getNextTriangle
		/// </summary>		
		Triangle getInterpolatedTriangle(int triangleIndex);
		
		/// <summary>
		/// Defines which texture in the graphics subsystem this model will use
		/// </summary>		
		void assignTexture(TextureHandle textureID);

		/// <summary>
		/// Defines which texture in the graphics subsystem this model will use
		/// </summary>		
		void assignBumpTexture(TextureHandle bumpTextureID, TextureHandle lightTextureID);
		
		/// <summary>
		/// Populates a ScaledFrame object with data
		/// </summary>		
		void buildScaledFrame(ScaledFrame &triangles, bool interpolate);
		
		/// <summary>
		/// Builds a new frame that is interpolated between two other frames based on time
		/// </summary>
		void buildInterpolatedFrame(TriangleVertexArray &currentFrame, TriangleVertexArray &nextFrame, TriangleVertexArray &interpolatedFrame);

		/// <summary>
		/// Used to load md2 gl commands from a file to help accelerate the rendering process...
		/// </summary>
		bool loadMd2CommandsFromFile();


		// animation list
		static md2Animation	animlist[21];		
		
		// keeps track of the current time every time a frame is rendered ...
		time_t myCurrentTime;
		
		// Temporarily holds frame data ...		
		typedef std::vector<md2Frame>  FrameList;
		typedef FrameList::iterator FrameIterator;
		FrameList myFrames;				
		
		// This is a list of all the triangles in the durned thingy
		typedef std::vector<md2Triangle> TriangleList;
		typedef TriangleList::iterator TriangleIterator;
		TriangleList myTriangles;
		
		// This is a list of all the Texture Coordinates in the md2 file
		typedef std::vector<md2TexCoord> TexCoordList;
		typedef TexCoordList::iterator TexCoordListIterator;
		TexCoordList myTexCoords;		
		
		/// The file handle we'll use for the md2 model information
		FILE* myFileHandle;
			
		// This identifies the current x,y,z position of the model
		Vector myLocation;
		
		// Indicates our current animation state ...
		ModelAnimState	myAnim;
		
		// Indicates the current scale we're drawing our model at ...
		float	myScale;
		 
		// Defines the data that contains our md2 header ...
		md2HeaderData myMd2HeaderData;
		
		// Defines a list of animations that we define as the default animations for each model ...
		static md2Animation	myTraditionalAnimations[TOTAL_MD2_ANIMATIONS];
		static md2Animation	myTraditionalFastAnimations[TOTAL_MD2_ANIMATIONS];
		static md2Animation	myFullAnimations[TOTAL_MD2_ANIMATIONS];
		static md2Animation	myStaticAnimations[TOTAL_MD2_ANIMATIONS];
		static md2Animation	myFullBaddieAnims[TOTAL_MD2_ANIMATIONS];
		static md2Animation	myInterpolatedBaddieAnims[TOTAL_MD2_ANIMATIONS];

		
		// Defines the texture handle the graphics subsystem has indicated for us to use ...
		TextureHandle myTextureHandle;				

		// Defines the texture handle to apply a bump map to the model texture...
		TextureHandle myBumpTextureHandle;
		TextureHandle myLightmapTextureHandle;

		// A vector that contains a list of ScaledFrame objects which can be used to either build an interpolated
		// ScaledFrame faster than using the original md2 data, or simply pass to the graphics engine as a list 
		// of triangle data. A third option would be to create vertex arrays, but that would make interpolation
		// impossible, and memory is a commodity on the Wii.
		ScaledMd2Frames myScaledMd2Frames;
		ScaledFrame myInterpolatedFrame;
		VertexFrame myMd2VertexArrays;
		TriangleVertexArray myInterpolatedVertexArray;
		bool myVertexArraysConstructed;
		std::vector<int> myGLCommands;

		// member variables
		static Vector	myPreCalculatedNormals[NUMBER_OF_NORMALS];
		static float	preCalculatedDotProdcuts[NUMBER_OF_DOT_PRODUCTS][256];

		// Defines what type of animation format we're using to render our frames.
		// In this case it would be traditional md2, or pre rendered md2
		AnimationFormat myAnimationFormat;
   };

#endif

