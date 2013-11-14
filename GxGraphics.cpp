// GxGraphics.cpp
// Implements IGraphics interface as it applieds to the GX GPU on the Wii
// Author(s): Eagan Rackley

#include "SFSHeaders.h"
#include "SimpleWiiHeap.h"
#include "GxGraphics.h"

// GX Specific headers - not portable so doesn't go in SFSHeaders
#include <ogcsys.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/video.h>
#include <png.h>
#include <pngconf.h>
#include "pngu.h"


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

// FIFO Data
//(512*1024)
#define DEFAULT_FIFO_SIZE GX_FIFO_MINSIZE
#define FIFO_DRAWDONE_TOKEN  0xBEEF
#define FIFO_DRAWING_TOKEN   0xB00B
#define NUMBER_OF_GFX_BUFFERS 2

// Used to scale any movement in the game
float gTimeScale = 0.0f;
float gLastFrameTime;
float gThisFrameTime;

typedef struct tagFrameBuffer
{
	// Points to our framebuffer ...
	void *GxFrameBuffer[1];
	u32 SequenceNumber;
}FrameBuffer;

FrameBuffer gFrameBuffers[NUMBER_OF_GFX_BUFFERS];
u32 gFrameSequenceNumber = 0;

// References the current framebuffer ...
u32	gCurrentDrawBuffer;

// Texture coordinates used with a skybox
#define SKYBOX_WIDTH	0.25f
#define SKYBOX_HEIGHT	1.0f
#define SKYBOX_FRONT	0.0f
#define SKYBOX_RIGHT	0.75f
#define SKYBOX_BACK		0.50f
#define SKYBOX_LEFT		0.25f

#define SKYBOX_SIZE 164.0f
#define SCENEBOX_SIZE 116.0f
#define SKYBOX_BOTTOM 20.0f

///<summary>
///changes the GP breakpoint so CPU can calculate next frame
///while GP processes last frame. should be called before any
///commands are sent to GP (at the top of the main loop)
///since this function assumes that both the CPU and GP are
///not processing.
///</summary>
void GxGraphics::HaveGPProcessFIFO()
{
	GXFifoObj fifo;
    void *readPtr, *writePtr;

    // Make sure the current write gather data from the fifo is flushed to the GP
    GX_Flush();

    // Set a breakpoint at the current point in the CPU
    // and disable the previous one to let the GP start processing
    GX_GetCPUFifo( &fifo);
    GX_GetFifoPtrs( &fifo, &readPtr, &writePtr );
    //GX_EnableBreakPt( writePtr );
    GX_SetDrawSync( FIFO_DRAWING_TOKEN );
}

void GxGraphics::setupSkyboxValues()
{
	mySkyboxCoords = new Vector[16];
	mySceneboxCoords = new Vector[16];
	mySkyboxTextures = new float[32];
	mySkyboxNormals = new Vector[4];			

	// This box holds our far away view of clouds / hills...

	// Front                    //z             x               y
	mySkyboxCoords[0] = (Vector){-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE/6};
	mySkyboxCoords[1] = (Vector){-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE/6};
	mySkyboxCoords[2] = (Vector){-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE/6};
	mySkyboxCoords[3] = (Vector){-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE/6};


	// Right
	mySkyboxCoords[4] = (Vector){-SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE/6};
	mySkyboxCoords[5] = (Vector){ SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE/6};
	mySkyboxCoords[6] = (Vector){ SKYBOX_SIZE,  SKYBOX_SIZE,   SKYBOX_SIZE/6};
	mySkyboxCoords[7] = (Vector){-SKYBOX_SIZE, SKYBOX_SIZE,   SKYBOX_SIZE/6};

	// Back
	mySkyboxCoords[8] =  (Vector){ SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE/6};
	mySkyboxCoords[9] =  (Vector){ SKYBOX_SIZE, -SKYBOX_SIZE,  -SKYBOX_SIZE/6};
	mySkyboxCoords[10] = (Vector){ SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE/6};
	mySkyboxCoords[11] = (Vector){ SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE/6};

	// Left
	mySkyboxCoords[12] = (Vector){-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE/6};
	mySkyboxCoords[13] = (Vector){ SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE/6};
	mySkyboxCoords[14] = (Vector){ SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE/6};
	mySkyboxCoords[15] = (Vector){-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE/6};

	// This box holds the scene midway between the skybox and the tilemap...

	// Front                       //z               x              y
	mySceneboxCoords[0] = (Vector){-SCENEBOX_SIZE, -SCENEBOX_SIZE, -SCENEBOX_SIZE/2};
	mySceneboxCoords[1] = (Vector){-SCENEBOX_SIZE,  SCENEBOX_SIZE, -SCENEBOX_SIZE/2};
	mySceneboxCoords[2] = (Vector){-SCENEBOX_SIZE,  SCENEBOX_SIZE,  SCENEBOX_SIZE/2};
	mySceneboxCoords[3] = (Vector){-SCENEBOX_SIZE, -SCENEBOX_SIZE,  SCENEBOX_SIZE/2};


	// Right
	mySceneboxCoords[4] = (Vector){-SCENEBOX_SIZE, SCENEBOX_SIZE,  -SCENEBOX_SIZE/2};
	mySceneboxCoords[5] = (Vector){ SCENEBOX_SIZE,  SCENEBOX_SIZE,  -SCENEBOX_SIZE/2};
	mySceneboxCoords[6] = (Vector){ SCENEBOX_SIZE,  SCENEBOX_SIZE,   SCENEBOX_SIZE/2};
	mySceneboxCoords[7] = (Vector){-SCENEBOX_SIZE, SCENEBOX_SIZE,   SCENEBOX_SIZE/2};

	// Back
	mySceneboxCoords[8] =  (Vector){ SCENEBOX_SIZE,  SCENEBOX_SIZE,  -SCENEBOX_SIZE/2};
	mySceneboxCoords[9] =  (Vector){ SCENEBOX_SIZE, -SCENEBOX_SIZE,  -SCENEBOX_SIZE/2};
	mySceneboxCoords[10] = (Vector){ SCENEBOX_SIZE, -SCENEBOX_SIZE,  SCENEBOX_SIZE/2};
	mySceneboxCoords[11] = (Vector){ SCENEBOX_SIZE,  SCENEBOX_SIZE,  SCENEBOX_SIZE/2};

	// Left
	mySceneboxCoords[12] = (Vector){-SCENEBOX_SIZE, -SCENEBOX_SIZE, -SCENEBOX_SIZE/2};
	mySceneboxCoords[13] = (Vector){ SCENEBOX_SIZE, -SCENEBOX_SIZE, -SCENEBOX_SIZE/2};
	mySceneboxCoords[14] = (Vector){ SCENEBOX_SIZE, -SCENEBOX_SIZE,  SCENEBOX_SIZE/2};
	mySceneboxCoords[15] = (Vector){-SCENEBOX_SIZE, -SCENEBOX_SIZE,  SCENEBOX_SIZE/2};


	/*Use the following values for top and bottom of box if needed
	// Render the top quad
	mySkyboxCoords[] = (Vector){ -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE };
	mySkyboxCoords[] = (Vector){ -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE };
	mySkyboxCoords[] = (Vector){ SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE };
	mySkyboxCoords[] = (Vector){ SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE };

	// Render the bottom quad
	mySkyboxCoords[] = (Vector){ -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE };
	mySkyboxCoords[] = (Vector){ -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE };
	mySkyboxCoords[] = (Vector){ SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE };
	mySkyboxCoords[] = (Vector){ SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE };
	*/

	// Set up our textures

	// Front
	mySkyboxTextures[0] = (SKYBOX_FRONT + SKYBOX_WIDTH);
	mySkyboxTextures[1] = 0;
	mySkyboxTextures[2] = SKYBOX_FRONT;
	mySkyboxTextures[3] = 0;
	mySkyboxTextures[4] = SKYBOX_FRONT;
	mySkyboxTextures[5] = SKYBOX_HEIGHT;
	mySkyboxTextures[6] = (SKYBOX_FRONT + SKYBOX_WIDTH);
	mySkyboxTextures[7] = SKYBOX_HEIGHT;

	// Right
	mySkyboxTextures[8] = (SKYBOX_RIGHT + SKYBOX_WIDTH);
	mySkyboxTextures[9] = 0;
	mySkyboxTextures[10] = SKYBOX_RIGHT;
	mySkyboxTextures[11] = 0;
	mySkyboxTextures[12] = SKYBOX_RIGHT;
	mySkyboxTextures[13] = SKYBOX_HEIGHT;
	mySkyboxTextures[14] = (SKYBOX_RIGHT + SKYBOX_WIDTH);
	mySkyboxTextures[15] = SKYBOX_HEIGHT;

	// Back
	mySkyboxTextures[16] = (SKYBOX_BACK + SKYBOX_WIDTH);
	mySkyboxTextures[17] = 0;
	mySkyboxTextures[18] = SKYBOX_BACK;
	mySkyboxTextures[19] = 0;
	mySkyboxTextures[20] = SKYBOX_BACK;
	mySkyboxTextures[21] = SKYBOX_HEIGHT;
	mySkyboxTextures[22] = (SKYBOX_BACK + SKYBOX_WIDTH);
	mySkyboxTextures[23] = SKYBOX_HEIGHT;

	// Left
	mySkyboxTextures[24] = SKYBOX_LEFT;
	mySkyboxTextures[25] = 0;
	mySkyboxTextures[26] = (SKYBOX_LEFT + SKYBOX_WIDTH);
	mySkyboxTextures[27] = 0;
	mySkyboxTextures[28] = (SKYBOX_LEFT + SKYBOX_WIDTH);
	mySkyboxTextures[29] = SKYBOX_HEIGHT;
	mySkyboxTextures[30] = SKYBOX_LEFT;
	mySkyboxTextures[31] = SKYBOX_HEIGHT;

	// Set up our normals
	mySkyboxNormals[0] = (Vector){0.0f, 0.0f, -1.0f};
	mySkyboxNormals[1] = (Vector){0.0f, 0.0f, -1.0f};
	mySkyboxNormals[2] = (Vector){0.0f, 0.0f, -1.0f};
	mySkyboxNormals[3] = (Vector){0.0f, 0.0f, -1.0f};

	mySkyboxRotation = (Rotate){{0.0f, 0.0f, 1.0f}, 1.0f};
}

// Setup viewport, scissor, copy, etc. based on w/h.
void changeViewSize(u16 w, u16 h)
{
    GX_SetViewport(0, 0, w, h, 0, 1);
    GX_SetScissor(0, 0, w, h);
    GX_SetDispCopySrc(0, 0, w, h);
    GX_SetDispCopyDst(w, h);
    GX_SetDispCopyYScale(1);
}


///<summary>
/// Sets up the GX for direct writing
///</summary>		
static void setupNRMDirectVertexDescriptors()
{
	GX_ClearVtxDesc();	
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_NRM,  GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

///<summary>
/// Sets up the GX for direct writing
///</summary>		
static void setupNRMIndex8VertexDescriptors()
{
	GX_ClearVtxDesc();	
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_NRM,  GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);
}

///<summary>
/// Sets up the GX for indexed writing
///</summary>		
static void setupNRMIndex16VertexDescriptors()
{
	GX_ClearVtxDesc();	
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_NRM,  GX_INDEX16);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
}

///<summary>
/// Sets up the GX for direct writing
///</summary>		
static void setupNBTIndex16VertexDescriptors()
{
	GX_ClearVtxDesc();	
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_NBT,  GX_INDEX16);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
}

///<summary>
/// Returns whatever buffer was last successfully drawn to (to avoid tearing?)
///</summary>
static u32 getBestDisplayBuffer()
{
	u32 bestBufferNumber = 0;
	u32 highestValue = 0;

	for(int idx = 0; idx < NUMBER_OF_GFX_BUFFERS; idx++)
	{
		if(highestValue < gFrameBuffers[idx].SequenceNumber)
		{
			highestValue = gFrameBuffers[idx].SequenceNumber;
			bestBufferNumber = idx;
		}

	}

	return bestBufferNumber;
}

///<summary>
/// Triggered by the Vsync interrupt, this will allow us to only blit the new display buffer
/// from the EFB to XFB once vsync has occurred...
///</summary>
void handleRetraceCallback(u32 retraceCount)
{
	VIDEO_SetNextFramebuffer(gFrameBuffers[getBestDisplayBuffer()].GxFrameBuffer[0]);
	VIDEO_Flush();
}

///<summary>
/// Constructor for the Subject ISubject implementation
///</summary>
GxGraphics::GxGraphics()
{
	// Make sure everything gets initialized properly...
	myNextRotateValues.clear();	
	myGxTextures.clear();
	myYScale = 0.0f;	
	
	// rad = 17, dist = 12 (ideal for no skybox view...)
	myCamera.radius = 15.59f;
	myCamera.distance = 5.59f;
	myCamera.angleFromLook = 0.0f;
	myCamera.rotation = 0.0f;	
	myCamera.pos.x = 0.0f;
	myCamera.pos.y = 0.0f;
	myCamera.pos.z = 0.0f;
	myCamera.up.x = -17.0f;
	myCamera.up.y = -12.0f;
	myCamera.up.z = 0.0f;
	myCamera.rotation = 0.0f;	
	myCamera.look.x = 1.0f;
	myCamera.look.y = 1.0f;
	myCamera.look.z = 20.0f;
	myCamera.target.x = 0.0f;
	myCamera.target.y = 0.0f;
	myCamera.target.z = 0.0f;	

	myFrameBufferHeight = 0;
	gCurrentDrawBuffer = 0;
	myCurrentTextureID = 0;
	myRegisterMode = NULL;

	myGpFifoBuffer = NULL;
	myFirstGxFrame = 1;

	mySpecularTheta = 94.0f;
	mySpecularPhi = -45.0f;

	myFPS = 0;
	myFPSCounter = 0;
	myFPSTimeSpan = 0;

	// Initialise the video system
	VIDEO_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	myRegisterMode = VIDEO_GetPreferredMode(NULL);

	if(myRegisterMode == NULL)
	{
		ASSERT(false);
		exit(0);
		return;
	}

	// Set up the video registers with the chosen mode
	VIDEO_Configure(myRegisterMode);

	// Allocate the number of framebuffers we're using
	for(int idx = 0; idx < NUMBER_OF_GFX_BUFFERS; idx++)
	{
		gFrameBuffers[idx].GxFrameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(myRegisterMode));
		gFrameBuffers[idx].SequenceNumber = 0;
		VIDEO_ClearFrameBuffer(myRegisterMode, gFrameBuffers[idx].GxFrameBuffer[0], COLOR_BLACK);
	}

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(gFrameBuffers[0].GxFrameBuffer[0]);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(myRegisterMode->viTVMode&VI_NON_INTERLACE)
	{
		VIDEO_WaitVSync();
	}
	else
	{
		while (VIDEO_GetNextField())
		{
			VIDEO_WaitVSync();
		}
	}

	// Set up our fifo...
	setupFIFO();

	// Set up the console... is this correct?
	console_init(gFrameBuffers[0].GxFrameBuffer[0], 20, 20, myRegisterMode->fbWidth, myRegisterMode->xfbHeight, myRegisterMode->fbWidth * 2);

	// Set up the details of our rendering environment
	setRenderingParameters();

	// Set up our projection matrix
	setupProjectionMatrix();

	VIDEO_WaitVSync();

	//simpleWiiHeapInit();	

	// Our simple heap will be used to allocate texture data onto the MEM2 heap of the Wii
	myAmbientColor0	= (GXColor){ 0xC0, 0xC0, 0xC0, 0x20 }; // Ambient  for channel 0
	myMaterialColor0 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF }; // Material for channel 0
	myAmbientColor1	= (GXColor){ 0x00, 0x00, 0x00, 0x74 }; // Ambient  for channel 0
	myMaterialColor1 = (GXColor){ 0x7F, 0x7F, 0x7F, 0xFF }; // Material for channel 0

	// Uses up 640x480 worth of texture area :)
	setupReflectionTexture();

	// Sets up 64x64 bytes worth of 8bit intensity data for use as a light map on TEXMAP4
	setupLightMapTexture();

	setupSkyboxValues();	

	// Setup our lighting matrix
	Mtx tempMtx;
	guMtxTrans(myLightMatrix, 0, 0, -1000); // Make light "infinite"
	Vector yAxis = (Vector){0.0, -1.0, 0.0};
	guMtxRotAxisDeg(tempMtx, &yAxis, -(LIT_Y_ANGLE));
	guMtxConcat(myLightMatrix, tempMtx, myLightMatrix);
	Vector xAxis = (Vector){1.0, 0.0, 0.0};
	guMtxRotAxisDeg(tempMtx, &xAxis, -(LIT_X_ANGLE));
	guMtxConcat(myLightMatrix, tempMtx, myLightMatrix);   	

	// Setup default bump mapping values
	myPostTransMtxScale = 132;
	myPostTransMtxOffset = 176;
	myIndTexMtxFactor = 0;

	enableLighting();

	//VIDEO_SetPreRetraceCallback(&handleRetraceCallback);
}

///<summary>
/// Sets up our projection matrix
/// this creates a perspective matrix with a view angle of 90,
/// and aspect ratio based on the display resolution
///</summary>
void GxGraphics::setupProjectionMatrix()
{
	f32 w = myRegisterMode->viWidth;
	f32 h = myRegisterMode->viHeight;
	guPerspective(myPerspective, 45, (f32)w/h, 0.001f, 1000.0F);
	//guFrustum(myPerspective, 1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 100.0f);
	GX_LoadProjectionMtx(myPerspective, GX_PERSPECTIVE);	
}

///<summary>
/// Sets up basic scene lighting...
///</summary>
void GxGraphics::enableLighting()
{	
	Vector  lDir;
	f32  theta;
	f32 phi;

	// Load our specular lighting object!
	// Light position	
	theta = (f32)mySpecularTheta * PI / 180.0F;
	phi   = (f32)mySpecularPhi * PI / 180.0F;
	lDir.x = - 1.0F * cosf(phi) * sinf(theta);
	lDir.y = - 1.0F * sinf(phi);
	lDir.z = - 1.0F * cosf(phi) * cosf(theta);
	// Normalize light position before rendering...
	fastVectorNormalize(&lDir);		
	GX_InitSpecularDirv(&mySpecLight, &lDir);		
	//GX_InitLightShininess(&mySpecLight, 1.0f);    
	GX_InitLightAttn(&mySpecLight, -3.0f, 0.0f, 4.0f, 1.0f, 0.0f, 0.0f);	
	GX_InitLightColor(&mySpecLight, myMaterialColor0);	
	GX_LoadLightObj(&mySpecLight, GX_LIGHT0);

	GX_SetNumChans(1);

	// Set material and ambient color.
	GX_SetChanAmbColor(GX_COLOR0, myAmbientColor0);
	GX_SetChanMatColor(GX_COLOR0, myMaterialColor0);
	GX_SetChanAmbColor(GX_ALPHA0, myAmbientColor1);
    GX_SetChanMatColor(GX_ALPHA0, myMaterialColor1);
	
	// Enable diffuse and specular lights.
    // COLOR0 = diffuse
    // ALPHA0 = specular
	GX_SetChanCtrl(GX_COLOR0, TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0,
		GX_DF_CLAMP, GX_AF_NONE);
	GX_SetChanCtrl(GX_ALPHA0, TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0,
		GX_DF_NONE, GX_AF_SPEC);

	// Load our projection lighting object!

	GXColor color = {255, 255, 255, 255};
	

	GX_InitLightPos(&mySpotLight, 0.0F, 0.0F, 0.0F);
	GX_InitLightColor(&mySpotLight, color);
	GX_LoadLightObj(&mySpotLight, GX_LIGHT1);
	color.g = color.b = 0;
	GX_SetChanMatColor(GX_COLOR1, color);
	GX_SetChanCtrl(GX_COLOR1, TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT1, GX_DF_CLAMP, GX_AF_NONE);

}

///<summary>
/// Sets up basic scene lighting...
///</summary>
void GxGraphics::disableLighting()
{
	GX_SetNumChans(1);
	// These are for the specular and diffuse lighting ...
	GX_SetChanCtrl(
		GX_COLOR0,
		GX_DISABLE,    // enable channel
		GX_SRC_REG,  // amb source
		GX_SRC_REG,  // mat source
		GX_LIGHT0,   // light mask
		GX_DF_CLAMP, // diffuse function
		GX_AF_NONE);
	GX_SetChanCtrl(
		GX_ALPHA0,
		GX_DISABLE,       // enable channel
		GX_SRC_REG,  // amb source
		GX_SRC_REG,  // mat source
		GX_LIGHT0,   // light mask
		GX_DF_NONE,  // diffuse function
		GX_AF_SPEC);
}

///<summary>
/// Destructor for the Subject ISubject implementation
///</summary>
GxGraphics::~GxGraphics() 
{
	free(myLightMapTexture.textureData);
	free(myReflectionTexture.textureData);
	dispose();
}

///<summary>
/// Destructor for the Subject ISubject implementation
///</summary>
void GxGraphics::dispose() 
{
	GX_AbortFrame();
	GX_Flush();

	VIDEO_SetBlack(TRUE);
	VIDEO_Flush();

	clearTextureData();

	// These values are used to build our skybox arrays
	delete mySkyboxCoords;
	delete mySceneboxCoords;
	delete mySkyboxTextures;
	delete mySkyboxNormals;

	// TODO: When we get a real wii devkit, this should be fixed! 
	//free(gGxFrameBuffer[0]);
	//free(gGxFrameBuffer[1]);
	//free(myGpFifo);
	//free(myRegisterMode);
}

///<summary>
/// I sure wish there was some clear documentation on what all this is doing...
/// Some of it is common sense, some if it... no idea ...
/// but suffice it to say, this helps set up our rendering environment...
///</summary>
void GxGraphics::setRenderingParameters()
{
	// Set up our background color
	GXColor	backgroundColor	= {0, 0, 0,	0xFF};
	GX_SetCopyClear(backgroundColor, 0x00ffffff);

	changeViewSize(myRegisterMode->fbWidth, myRegisterMode->efbHeight);

	// Set up our viewport and clipping options ...
	GX_SetFieldMode(myRegisterMode->field_rendering, ((myRegisterMode->viHeight==2 * myRegisterMode->xfbHeight) ? GX_ENABLE:GX_DISABLE));
	GX_SetCopyFilter(myRegisterMode->aa, myRegisterMode->sample_pattern, GX_TRUE, myRegisterMode->vfilter);

	if(myRegisterMode->aa)
	{
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	}
	else
	{
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}

	GX_SetZMode(GX_ENABLE, GX_LEQUAL, GX_TRUE);
	GX_SetZCompLoc(GX_FALSE);

	GX_SetColorUpdate(GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);			

	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(gFrameBuffers[gCurrentDrawBuffer].GxFrameBuffer[0], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	// Vertexd format 0 will be our normal rendering attribute set
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);	
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	// Vertexd format 1 will be our normal/binormal/tangent rendering set
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_NBT, GX_NRM_NBT3, GX_S8, 0);			
	GX_SetVtxAttrFmt (GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	// Vertexd format 2 will be when our normal rendering uses fractional fixed point data...
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);	
	GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	
	// Vertexd format 4 will be used for our light map hemisphere...
	GX_SetVtxAttrFmt(GX_VTXFMT4, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT4, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);


	GX_InvVtxCache();
	GX_InvalidateTexAll();
	
	guLookAt(myView, &myCamera.pos, &myCamera.up, &myCamera.look);
}

///<summary>
/// Sets up the GX for a rendering pass
///</summary>		
void GxGraphics::setupRenderingPass()
{
	// Sets up the lighting we'll use this rendering pass 
	//TODO: base this on our texture properties, and pass in a texture object when determining the rendering pass
	enableLighting();

	// This one works and actually looks quite nice
	GX_SetNumTevStages(1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	//GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR1);
	//GX_SetTevOp(GX_TEVSTAGE1, GX_MODULATE);

	//GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
}

///<summary>
/// Sets up the GX for a rendering pass
///</summary>		
void GxGraphics::setupHeightRenderingPass()
{
	// Sets up the lighting we'll use this rendering pass 
	//TODO: base this on our texture properties, and pass in a texture object when determining the rendering pass
	enableLighting();

	GX_SetNumChans(0);

	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);	
	// this uses settings from ind-bump-st
	// TEV setting
	//GX_SetAlphaCompare(GX_GREATER, 1, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetNumTevStages(2);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
	GX_SetTevOp(GX_TEVSTAGE1, GX_PASSCLR);
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP1, GX_COLORNULL);	
}

///<summary>
/// Sets up the GX for a rendering pass
///</summary>		
void GxGraphics::setupEmbossedRenderingPass()
{
	// Sets up the lighting we'll use this rendering pass 
	enableLighting();	

	// load normal matrix - this is used to adjust the bump depth
	Mtx nrmMtx = {
		{ (float)250.0f/255.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, (float)250.0f/255.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, (float)250.0f/255.0f, 0.0f }
	};

	GX_LoadNrmMtxImm(nrmMtx, GX_PNMTX0);

	GX_SetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);

	GX_SetNumTexGens(2);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_BUMP0, GX_TG_TEXCOORD0, GX_IDENTITY);	

	GX_SetNumTevStages(4);		
	/*GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
	GX_SetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KCSEL_1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GX_SetTevDirect(GX_TEVSTAGE0);
	*/
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetTevDirect(GX_TEVSTAGE0);

	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
	GX_SetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KCSEL_1);
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR0A0);
	GX_SetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GX_SetTevDirect(GX_TEVSTAGE1);

	GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_RASC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
	GX_SetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KCSEL_1);
	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR0A0);
	GX_SetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GX_SetTevDirect(GX_TEVSTAGE2);

	GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE3, GX_MODULATE);	

	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
}

///<summary>
/// Sets up the GX for a rendering pass
///</summary>		
void GxGraphics::setupNormalMapRenderingPass()
{	
	// Sets up the lighting we'll use this rendering pass 
	enableLighting();	

	// Setup a texcoordinate lookup matrix for the bump mapping
	Mtx nullMtx = {{ 0.0f,  0.0f,  0.0f,  0.0f},
	{ 0.0f,  0.0f,  0.0f,  0.0f},
	{ 0.0f,  0.0f,  0.0f,  0.0f}};

	GX_SetNumTexGens(3);
	GX_LoadTexMtxImm(nullMtx, GX_TEXMTX0, GX_MTX3x4);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY); // normal map
	GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0); // (0,0) 	
	GX_SetTexCoordGen(GX_TEXCOORD2, GX_TG_BUMP0, GX_TG_TEXCOORD1, GX_IDENTITY);

	// setup indirect unit for tev stage 0 (for some reason I get a ABGR triplet, so transforms are done on normal map's B and G components)	
	f32 indMtx[2][3] = {
		{  0.5f, -0.125f,    0.0f}, // B -> horizontal coordinate (s)
		{  0.0f,    0.0f, -0.125f}  // G -> vertical coordinate (t)
	};		

	// the indirect texture unit multiplies the tangent space with the bump normal
	GX_SetNumIndStages(1);
	GX_SetIndTexMatrix(GX_ITM_0, indMtx, 0);
	GX_SetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP2);		// bump
	GX_SetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);	

	GX_SetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
	GX_SetTevIndBumpST(GX_TEVSTAGE1, GX_INDTEXSTAGE0, GX_ITM_0);
	//GX_SetTevIndBumpXYZ(GX_TEVSTAGE1, GX_INDTEXSTAGE0, GX_ITM_0);
	GX_SetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP2);	

	GX_SetNumTevStages(4);
		
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

	GX_SetTevIndirect(
		GX_TEVSTAGE1, // tevstage 0 is used to combine normal map and lightmap
		GX_INDTEXSTAGE0, // ind stage is used to read normal map
		GX_ITF_8, // texture format 8 bits per channel
		GX_ITB_STU, // use bias on s and t channels (red and green and blue)
		GX_ITM_0, // use 3x2 matrix 0
		GX_ITW_0, // do not use texture wrapping
		GX_ITW_0, // do not use texture wrapping
		GX_FALSE, // don't add in previous coordinates
		GX_FALSE, // don't use mipmap stuff
		GX_ITBA_OFF // don't use bump alpha
		); 

	GX_SetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_1_8); // ambient intensity
	GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, GX_CC_KONST);
	GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP1, GX_COLOR0A0);
	
	GX_SetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR1A1);
	GX_SetTevOp(GX_TEVSTAGE3, GX_MODULATE);

	//GX_SetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR0A0);
	//GX_SetTevOp(GX_TEVSTAGE4, GX_MODULATE);

	//GX_SetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	//GX_SetTevOp(GX_TEVSTAGE5, GX_BLEND);


	// For specular to work here we need ad iff. lighting channel...
	//GX_SetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD2, GX_TEXMAP1, GX_COLORNULL);		
	//GX_SetTevColorOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV );  // output
	//GX_SetTevColorIn(GX_TEVSTAGE4, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO, GX_CC_TEXA);				
}

///<summary>
/// Sets up the GX for a rendering pass
///</summary>		
void GxGraphics::setupXYZMapRenderingPass()
{
	// Sets up the lighting we'll use this rendering pass 
	enableLighting();	
	Mtx tempMtx;
	Mtx normalMtx;
	Mtx normalTexMtx;
	Mtx bumpnormalMtx;

	// this is used to rotate each normal into light space
	f32 texMtx[2][3] = {
		{ 0.5f, 0.0f, 0.0f }, // S Horizontal Coordinate
		{ 0.0f, 0.5f, 0.0f}  // T Vertical Coordinate
	};

	// The normal mtx starts out as the inverse transpose of the vertex mtx (which is myModelView)	
	if (!guMtxInverse(myModelView, tempMtx)) {ASSERT(FALSE);}
    guMtxTranspose(tempMtx, normalMtx);
	
	// matrix for transforming normals from object space to eye space,
    // scale by NRM_SCALE/2, offset by 0.5,0.5.
    // the negation in Y is needed since the T axis is opposite of the Y axis
    guMtxScale(tempMtx, 0.4f, -0.4f, 0.4f);
    guMtxConcat(tempMtx, normalMtx, normalMtx);
    guMtxTrans(tempMtx, 0.5f, 0.5f, 0.0f);
    guMtxConcat(tempMtx, normalMtx, normalTexMtx);
    
	// matrix for transforming bump offsets
    // take normal mtx, then scale by 0.4f
    // the negation in [1][1] is needed since the T axis is opposite of the Y axis
    guMtxScale(tempMtx, 0.4f, 0.4f, 0.4f);
    guMtxConcat(tempMtx, normalMtx, bumpnormalMtx);
    texMtx[0][0] =  bumpnormalMtx[0][0];
    texMtx[0][1] =  bumpnormalMtx[0][1];
    texMtx[0][2] =  bumpnormalMtx[0][2];
    texMtx[1][0] =  bumpnormalMtx[1][0];
    texMtx[1][1] = -bumpnormalMtx[1][1];
    texMtx[1][2] =  bumpnormalMtx[1][2];
	    	
	Mtx dttMtx = {
		{ myPostTransMtxScale / 256.0f, 0.0, 0.0, myPostTransMtxOffset / 256.0f },
		{ 0.0, myPostTransMtxScale / 256.0f, 0.0, myPostTransMtxOffset / 256.0f },
		{ 0.0, 0.0, myPostTransMtxScale / 256.0f, myPostTransMtxOffset / 256.0f }
	};
	
	GX_SetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
	
	GX_LoadNrmMtxImm(normalMtx, GX_PNMTX0);    
    GX_LoadTexMtxImm(normalTexMtx, GX_TEXMTX0, GX_MTX2x4);
	GX_SetIndTexMatrix(GX_ITM_0, texMtx, myIndTexMtxFactor);

	//GX_LoadTexMtxImm(nrmMtx, GX_TEXMTX0, GX_MTX3x4);
	GX_LoadTexMtxImm(dttMtx, GX_DTTMTX0, GX_MTX3x4);
	GX_SetIndTexMatrix(GX_ITM_0, texMtx, myIndTexMtxFactor);
	
	GX_SetNumTexGens(2);
	GX_SetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, FALSE, GX_DTTIDENTITY);
	GX_SetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_NRM, GX_TEXMTX0, TRUE, GX_DTTMTX0);
	
	GX_SetNumTevStages(2);
	
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
	GX_SetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KCSEL_1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);	// environment map
	GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);

	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);	
	GX_SetTevOp(GX_TEVSTAGE1, GX_MODULATE); 
	
	GX_SetNumIndStages(1);
	GX_SetTevIndBumpXYZ(GX_TEVSTAGE0, GX_INDTEXSTAGE0, GX_ITM_0);
	GX_SetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP2);		// bump map
	GX_SetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);		// this scales the incoming texcoords for the bump map from the vertex by a constant		

	/*
	// Two tev stages, one Bump stage.
    GX_SetNumTevStages(2);

    GX_SetNumIndStages(1);

    // Two texture coordinates, no colors.
    GX_SetNumTexGens(2);
    GX_SetNumChans(0);

    // Indirect Stage 0 -- Sample normal perturbation map
    GX_SetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP2);
    GX_SetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);

    // Stage 0 -- Save material texture
    //
    // TEVPREV = TEXC/TEXA
    //
    GX_SetTevDirect(GX_TEVSTAGE0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
    GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0); // RGBA
    GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

    // Set up the indirect bump calculation for Stage 1
    //
    GX_SetTevIndBumpXYZ(GX_TEVSTAGE1, GX_INDTEXSTAGE0, GX_ITM_0);

    // Stage 1 -- Add source normal in Bump. Index lightmap with result of
    //            perturbation. Apply diffuse and specular components.
    //
    // TEVPREVC = PREVC * TEXC + TEXA
    // TEVPREVA = PREVA
    //
    GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLORNULL);
    GX_SetTevColorIn(GX_TEVSTAGE1,
                    GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA);
    GX_SetTevAlphaIn(GX_TEVSTAGE1,
                    GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GX_SetTevColorOp(GX_TEVSTAGE1,
                    GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
    GX_SetTevAlphaOp(GX_TEVSTAGE1,
                    GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
*/
}

///<summary>
/// completes the rendering pass
///</summary>		
static void endRenderingPass()
{
	GX_SetNumIndStages(0);
	GX_SetTevDirect(GX_TEVSTAGE0);
	GX_SetTevDirect(GX_TEVSTAGE1);
	GX_SetTevDirect(GX_TEVSTAGE2);
	GX_SetTevDirect(GX_TEVSTAGE3);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);	
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);				

	//GX_SetCullMode(GX_CULL_BACK);

	GX_ClearVtxDesc();	
}


///<summary>
/// Set up our fifo and flipper for the GX unit ...
///</summary>
void GxGraphics::setupFIFO()
{

	myGpFifoBuffer = memalign(32, DEFAULT_FIFO_SIZE);
	memset(myGpFifoBuffer, 0, DEFAULT_FIFO_SIZE);
	GX_Init(myGpFifoBuffer, DEFAULT_FIFO_SIZE);

	// This will cause the fifo to start processing right away...
	//GX_SetDrawSync(FIFO_DRAWDONE_TOKEN);
}

///<summary>
/// Normalizes a frustum plane...
///</summary>
static void normalizePlane(FrustumPlane* frustumPlane)
{
	float t;
	// Normalize the right plane...
	t = sqrt((frustumPlane->A * frustumPlane->A)
			+ (frustumPlane->B * frustumPlane->B)
			+ (frustumPlane->C * frustumPlane->C));
	frustumPlane->A /= t;
	frustumPlane->B /= t;
	frustumPlane->C /= t;
	frustumPlane->D /= t;
}

///<summary>
/// Builds our frustum planes for check to see if a point is in our camera view...
///</summary>
void GxGraphics::updateFrustumPlanes()
{
	Mtx44 frustumMtx;
	//guMtxConcat(myPerspective, myView, frustumMtx);
	//guMtxCopy(myView, frustumMtx);
	//guMtxConcat(myView, myPerspective, frustumMtx);

	//guMtxIdentity(myModelView);
	//Rotate rotate = (Rotate){{0.0f, 1.0f, 0.0f}, myCamera.rotation};
	//guMtxRotAxisDeg(myTempRotate, &rotate.axis, rotate.value);
	//	guMtxConcat(myModel, myTempRotate, myModel);
	//guMtxIdentity(myModel);
	//Rotate rotate = (Rotate){{0.0f, 1.0f, 0.0f},myCamera.rotation};
	//guMtxRotAxisDeg(myTempRotate, &rotate.axis, rotate.value);
	//guMtxConcat(myModel, myTempRotate, myModel);
	//guMtxTransApply(myModel, myModel, 0.0f, 0.0f, 0.0f);

	// load the modelview matrix into matrix memory
	//guMtxInverse(myModel, myModelView);
	//guMtxTranspose(myModelView, myModel);
	guMtxTransApply(myModelView, myModelView, -myCamera.target.x, -myCamera.target.y, myCamera.target.z);
	guMtxConcat(myPerspective, myModelView, frustumMtx);


	// Calculate and Normalize all 6 of our possible frustum planes...

	myFrustumPlanes[RIGHT_PLANE].A = guMtxRowCol(frustumMtx,0,3) - guMtxRowCol(frustumMtx,0,0);
	myFrustumPlanes[RIGHT_PLANE].B = guMtxRowCol(frustumMtx,1,3) - guMtxRowCol(frustumMtx,1,0);
	myFrustumPlanes[RIGHT_PLANE].C = guMtxRowCol(frustumMtx,2,3) - guMtxRowCol(frustumMtx,2,0);
	myFrustumPlanes[RIGHT_PLANE].D = guMtxRowCol(frustumMtx,3,3) - guMtxRowCol(frustumMtx,3,0);
	normalizePlane(&myFrustumPlanes[RIGHT_PLANE]);

	myFrustumPlanes[LEFT_PLANE].A = guMtxRowCol(frustumMtx,0,3) + guMtxRowCol(frustumMtx,0,0);
	myFrustumPlanes[LEFT_PLANE].B = guMtxRowCol(frustumMtx,1,3) + guMtxRowCol(frustumMtx,1,0);
	myFrustumPlanes[LEFT_PLANE].C = guMtxRowCol(frustumMtx,2,3) + guMtxRowCol(frustumMtx,2,0);
	myFrustumPlanes[LEFT_PLANE].D = guMtxRowCol(frustumMtx,3,3) + guMtxRowCol(frustumMtx,3,0);
	normalizePlane(&myFrustumPlanes[LEFT_PLANE]);

	myFrustumPlanes[BOTTOM_PLANE].A = guMtxRowCol(frustumMtx,0,3) + guMtxRowCol(frustumMtx,0,1);
	myFrustumPlanes[BOTTOM_PLANE].B = guMtxRowCol(frustumMtx,1,3) + guMtxRowCol(frustumMtx,1,1);
	myFrustumPlanes[BOTTOM_PLANE].C = guMtxRowCol(frustumMtx,2,3) + guMtxRowCol(frustumMtx,2,1);
	myFrustumPlanes[BOTTOM_PLANE].D = guMtxRowCol(frustumMtx,3,3) + guMtxRowCol(frustumMtx,3,1);
	normalizePlane(&myFrustumPlanes[BOTTOM_PLANE]);

	myFrustumPlanes[TOP_PLANE].A = guMtxRowCol(frustumMtx,0,3) - guMtxRowCol(frustumMtx,0,1);
	myFrustumPlanes[TOP_PLANE].B = guMtxRowCol(frustumMtx,1,3) - guMtxRowCol(frustumMtx,1,1);
	myFrustumPlanes[TOP_PLANE].C = guMtxRowCol(frustumMtx,2,3) - guMtxRowCol(frustumMtx,2,1);
	myFrustumPlanes[TOP_PLANE].D = guMtxRowCol(frustumMtx,3,3) - guMtxRowCol(frustumMtx,3,1);
	normalizePlane(&myFrustumPlanes[TOP_PLANE]);

	myFrustumPlanes[FAR_PLANE].A = guMtxRowCol(frustumMtx,0,3) - guMtxRowCol(frustumMtx,0,2);
	myFrustumPlanes[FAR_PLANE].B = guMtxRowCol(frustumMtx,1,3) - guMtxRowCol(frustumMtx,1,2);
	myFrustumPlanes[FAR_PLANE].C = guMtxRowCol(frustumMtx,2,3) - guMtxRowCol(frustumMtx,2,2);
	myFrustumPlanes[FAR_PLANE].D = guMtxRowCol(frustumMtx,3,3) - guMtxRowCol(frustumMtx,3,2);
	normalizePlane(&myFrustumPlanes[FAR_PLANE]);

	myFrustumPlanes[NEAR_PLANE].A = guMtxRowCol(frustumMtx,0,3) + guMtxRowCol(frustumMtx,0,2);
	myFrustumPlanes[NEAR_PLANE].B = guMtxRowCol(frustumMtx,1,3) + guMtxRowCol(frustumMtx,1,2);
	myFrustumPlanes[NEAR_PLANE].C = guMtxRowCol(frustumMtx,2,3) + guMtxRowCol(frustumMtx,2,2);
	myFrustumPlanes[NEAR_PLANE].D = guMtxRowCol(frustumMtx,3,3) + guMtxRowCol(frustumMtx,3,2);
	normalizePlane(&myFrustumPlanes[NEAR_PLANE]);
}

///<summary>
/// Prepares a scene for rendering in the specified environment
///</summary>				
void GxGraphics::beginScene()
{

	// This will have the GP start processing the last frame (if we don't kick it in the gonads!)
	//HaveGPProcessFIFO();

	// While we begin rendering our new frame here..
	uint ticks = gettime();

	guLookAt(myView, &myCamera.pos, &myCamera.up, &myCamera.look);

	updateFrustumPlanes();

	guMtxIdentity(myModel);

	GX_SetViewport(0, 0, myRegisterMode->fbWidth, myRegisterMode->efbHeight, 0, 1);

	// Update gTimeScale based on when we begin the scene (so we can't keep movement consistent on all platforms)
	gLastFrameTime = gThisFrameTime;
	gThisFrameTime = (ticks / TB_TIMER_CLOCK); // gets a whole # of ms
	float difference = ((gThisFrameTime - gLastFrameTime) * 0.001f);
	if((difference <= 2.0f) && (difference >= -2.0f))
	{
		gTimeScale = difference;
		myFPSTimeSpan += gTimeScale;
	}	
	// Handle tracking our FPS...
	myFPSCounter++;		
	if(myFPSTimeSpan >= 1.0f)
	{
		myFPS = myFPSCounter;
		myFPSCounter = 0;			
		myFPSTimeSpan = 0.0f;
	}
	//printf("\x1b[2;0H"); printf("FPS: %d", myFPS);		
	//printf("\x1b[2;0H"); printf("\n\nLM ID: %d", myLightMapTexHandle);
}

///<summary>
/// Whatever is drawn next (be it a single primitive, or be it a complex object)
/// is rotated on the axis specified and by the amount in the rotate object
///</summary>		
void GxGraphics::rotateNextObjectInScene(Rotate *rotate)
{		
	myNextRotateValues.push_back(*rotate);
}		

///<summary>
/// Handles rotation if one is pending ...
///</summary>			
void GxGraphics::checkObjectRotation()
{
	if(myNextRotateValues.size() > 0)
	{					
		for(u32 idx = 0; idx <= myNextRotateValues.size(); idx++)
		{
			Rotate *rotateValue = &myNextRotateValues.back();
			myNextRotateValues.pop_back();
			guMtxRotAxisDeg(myTempRotate, &rotateValue->axis, rotateValue->value);						
			guMtxConcat(myModel, myTempRotate, myModel);
		}		
	}
	myNextRotateValues.clear();
}

///<summary>
/// Adds a triangle based on the specified triangle definition to the scene
///</summary>		
void GxGraphics::addTriangleToScene(Vector location, Triangle triangle)
{	
	setupRenderingPass();
	setupNRMDirectVertexDescriptors();
	handleMatrixCalculations(location);

	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);		
	GX_Position3f32( triangle.verticies[0].x, triangle.verticies[0].y, triangle.verticies[0].z);
	GX_Normal3f32(0.0f, 0.0f, 1.0f);
	GX_TexCoord2f32(0.0, 0.0);		

	GX_Position3f32(triangle.verticies[1].x, triangle.verticies[1].y, triangle.verticies[1].z);
	GX_Normal3f32(0.0f, 0.0f, 1.0f);
	GX_TexCoord2f32(1.0, 0.0);		

	GX_Position3f32( triangle.verticies[2].x, triangle.verticies[2].y, triangle.verticies[2].z);
	GX_Normal3f32(0.0f, 0.0f, 1.0f);
	GX_TexCoord2f32(1.0, 1.0);		

	GX_End();	

	endRenderingPass();
}

///<summary>
/// Sets up a temporary texture object for display on screen ...
///</summary>		
void GxGraphics::setupTextureObject(int textureID)
{	
	ASSERT((myGxTextures.empty() == false));
	GxTexMapIterator mapIterator = myGxTextures.find(textureID);
	
	if(mapIterator != myGxTextures.end())
	{
		GxTextureData *currentTexture = &mapIterator->second;						
				
		GX_LoadTexObj(&currentTexture->textureObject, currentTexture->textureMap);			
	}
}

///<summary>
/// Sets ups a texture object for use as a Z Height Texture 
///</summary>		
void GxGraphics::setupHeightTextureObject(int textureID)
{
	// TODO: Implement this thingy...
}

///<summary>
/// Adds a triangle based on the specified triangle definition to the scene
///</summary>		
void GxGraphics::addPointsToScene(Vector location, std::vector<Point> &points, int pixelSize, POINT_TEXTURE_STYLE textureStyle, TextureHandle textureID)
{
	ASSERT(myGxTextures.empty() == false);

	GxTexMapIterator mapIterator = myGxTextures.find(textureID);
		
	if(mapIterator != myGxTextures.end())
	{			
		setupRenderingPass();
		disableLighting();
		setupNRMDirectVertexDescriptors();

		GX_SetNumTevStages(1);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
		GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
		//GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
		//GX_SetTevOp(GX_TEVSTAGE1, GX_MODULATE);

		GX_SetChanCtrl(
			GX_COLOR0,
			GX_DISABLE,    // enable channel
			GX_SRC_REG,  // amb source
			GX_SRC_REG,  // mat source
			GX_LIGHT0,   // light mask
			GX_DF_CLAMP, // diffuse function
			GX_AF_NONE);

		//GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_LEQUAL, 255 );

		guMtxIdentity(myModel);		
		guMtxTransApply(myModel, myModel, location.x, location.y, location.z);
		guMtxConcat(myView, myModel, myModelView);

		// load the modelview matrix into matrix memory
		GX_LoadPosMtxImm(myModelView, GX_PNMTX0);

		// Set up our texture for drawing ...
		setupTextureObject(textureID);

		// Set the point size and texture style		
		GX_EnableTexOffsets(GX_TEXCOORD0, GX_FALSE, GX_TRUE);
		switch(textureStyle)
		{
		case USE_EIGHTH_TEXTURE:
			GX_SetPointSize(pixelSize, GX_TO_EIGHTH );
			break;
		case USE_QUARTER_TEXTURE: 
			GX_SetPointSize(pixelSize, GX_TO_FOURTH );
			break;
		case USE_HALF_TEXTURE:
			GX_SetPointSize(pixelSize, GX_TO_HALF);
			break;
		case USE_WHOLE_TEXTURE: 
			GX_SetPointSize(pixelSize, GX_TO_ONE);
			break;
		default:
			GX_SetPointSize(pixelSize, GX_TO_ZERO);
			break;
		}

		GX_Begin(GX_POINTS, GX_VTXFMT0, points.size());
		std::vector<Point>::iterator pointIterator = points.begin();			
		for(u32 idx = 0; idx < points.size(); idx++)
		{					
			//GX_Color4u8(15, 15, 255, 255);			
			GX_Position3f32(pointIterator->point.x, pointIterator->point.y, pointIterator->point.z);			
			GX_Normal3f32(0.0, 0.0, 1.0);
			GX_TexCoord2f32(pointIterator->texture_S, pointIterator->texture_T);
			pointIterator++;
		}		
		GX_End();		
		
		endRenderingPass();
	}// end if((myGxTextures...
}

///<summary>
/// Adds a triangle based on the specified triangle definition to the scene
///</summary>		
void GxGraphics::addTrianglesToScene(Vector location, std::vector<Triangle> &triangles, int count, u32 textureID)
{
	// Now is a good time to load the texture object
	ASSERT(myGxTextures.empty() == false);

	setupRenderingPass();
	setupNRMDirectVertexDescriptors();
	handleMatrixCalculations(location);
	setupTextureObject(textureID);

	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, (3 * triangles.size()));		

	std::vector<Triangle>::iterator triangleIterator = triangles.begin();	
	for(u32 idx = 0; idx < triangles.size(); idx++)
	{	
		GX_Position3f32( triangleIterator->verticies[0].x, triangleIterator->verticies[0].y, triangleIterator->verticies[0].z);
		GX_Normal3f32(triangleIterator->normals[2].x, triangleIterator->normals[2].y, triangleIterator->normals[2].z);		
		GX_TexCoord2f32(triangleIterator->texture_S[0], triangleIterator->texture_T[0]);		

		GX_Position3f32(triangleIterator->verticies[1].x, triangleIterator->verticies[1].y, triangleIterator->verticies[1].z);
		GX_Normal3f32(triangleIterator->normals[1].x, triangleIterator->normals[1].y, triangleIterator->normals[1].z);		
		GX_TexCoord2f32(triangleIterator->texture_S[1], triangleIterator->texture_T[1]);		

		GX_Position3f32( triangleIterator->verticies[2].x, triangleIterator->verticies[2].y, triangleIterator->verticies[2].z);
		GX_Normal3f32(triangleIterator->normals[0].x, triangleIterator->normals[0].y, triangleIterator->normals[0].z);		
		GX_TexCoord2f32(triangleIterator->texture_S[2], triangleIterator->texture_T[2]);

		triangleIterator++;		
	}
	GX_End();	
	endRenderingPass();
}

///<summary>
/// Adds quads based on the specified quad definitions to the scene
///</summary>		
void GxGraphics::addQuadsToScene(Vector location, std::map<int, Quad> &quads, int count, TextureHandle textureID)
{
	// Now is a good time to load the texture object
	ASSERT(myGxTextures.empty() == false);

	setupRenderingPass();
	disableLighting();
	setupNRMDirectVertexDescriptors();	
	handleMatrixCalculations(location);
	setupTextureObject(textureID);

	// Render our quads with additive blending :)
	GX_Begin(GX_QUADS, GX_VTXFMT0, (4 * quads.size()));		

	//GX_TRIANGLESTRIP   GL_TRIANGLE_STRIP
	//0 1 2                      0 1 2
	//1 3 2                      2 1 3
	//2 3 4                      2 3 4

	GX_SetCullMode(GX_CULL_NONE);

	// Loop through our map using an iterator and draw the quad objects...
	std::map<int, Quad>::iterator quadItr;
	quadItr = quads.begin();	
	for(u32 idx = 0; idx < quads.size(); idx++)
	{		
		GX_Position3f32(quadItr->second.verticies[0].x, quadItr->second.verticies[0].y, quadItr->second.verticies[0].z);
		GX_Normal3f32(quadItr->second.normals[0].x, quadItr->second.normals[0].y, quadItr->second.normals[0].z);		
		GX_TexCoord2f32(quadItr->second.texture_S[0], quadItr->second.texture_T[0]);		

		GX_Position3f32(quadItr->second.verticies[1].x, quadItr->second.verticies[1].y, quadItr->second.verticies[1].z);
		GX_Normal3f32(quadItr->second.normals[1].x, quadItr->second.normals[1].y, quadItr->second.normals[1].z);
		GX_TexCoord2f32(quadItr->second.texture_S[1], quadItr->second.texture_T[1]);


		GX_Position3f32(quadItr->second.verticies[2].x, quadItr->second.verticies[2].y, quadItr->second.verticies[2].z);
		GX_Normal3f32(quadItr->second.normals[2].x, quadItr->second.normals[2].y, quadItr->second.normals[0].z);
		GX_TexCoord2f32(quadItr->second.texture_S[2], quadItr->second.texture_T[2]);


		GX_Position3f32(quadItr->second.verticies[3].x, quadItr->second.verticies[3].y, quadItr->second.verticies[3].z);
		GX_Normal3f32(quadItr->second.normals[3].x, quadItr->second.normals[3].y, quadItr->second.normals[3].z);
		GX_TexCoord2f32(quadItr->second.texture_S[3], quadItr->second.texture_T[3]);		

		quadItr++;
	}

	GX_End();

	endRenderingPass();
}

///<summary>
/// Handles our box rendering code :)
///</summary>
static void renderSceneOrSkybox()
{
	// Render the front quad
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position1x8(0);
		GX_Normal1x8(0);
		GX_TexCoord1x8(0);

		GX_Position1x8(1);
		GX_Normal1x8(0);
		GX_TexCoord1x8(1);

		GX_Position1x8(2);
		GX_Normal1x8(0);
		GX_TexCoord1x8(2);

		GX_Position1x8(3);
		GX_Normal1x8(0);
		GX_TexCoord1x8(3);
		GX_End();

		// Render the left quad
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position1x8(4);
		GX_Normal1x8(0);
		GX_TexCoord1x8(4);

		GX_Position1x8(5);
		GX_Normal1x8(0);
		GX_TexCoord1x8(5);

		GX_Position1x8(6);
		GX_Normal1x8(0);
		GX_TexCoord1x8(6);

		GX_Position1x8(7);
		GX_Normal1x8(0);
		GX_TexCoord1x8(7);
		GX_End();

		// Render the right quad
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position1x8(8);
		GX_Normal1x8(0);
		GX_TexCoord1x8(8);

		GX_Position1x8(9);
		GX_Normal1x8(0);
		GX_TexCoord1x8(9);

		GX_Position1x8(10);
		GX_Normal1x8(0);
		GX_TexCoord1x8(10);

		GX_Position1x8(11);
		GX_Normal1x8(0);
		GX_TexCoord1x8(11);
		GX_End();

		// Render the back quad
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position1x8(12);
		GX_Normal1x8(0);
		GX_TexCoord1x8(12);

		GX_Position1x8(13);
		GX_Normal1x8(0);
		GX_TexCoord1x8(13);

		GX_Position1x8(14);
		GX_Normal1x8(0);
		GX_TexCoord1x8(14);

		GX_Position1x8(15);
		GX_Normal1x8(0);
		GX_TexCoord1x8(15);
		GX_End();
}

///<summary>
/// Handles all the rendering of a skybox, all it needs is a texture ...
///</summary>					
void GxGraphics::addSkyboxToScene(PROJECTION_TYPE projectionType, TextureHandle skyboxID, TextureHandle sceneboxID)
{
	// Now is a good time to load the texture object
	ASSERT(myGxTextures.empty() == false);

	GX_SetCullMode(GX_CULL_NONE);

	GX_SetNumTevStages(1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_DECAL);

	disableLighting();

	setupNRMIndex8VertexDescriptors();

	// Rotate our skybox then handle the matrix calcs...
	mySkyboxRotation.value += 1.0f * gTimeScale;
	rotateNextObjectInScene(&mySkyboxRotation);

	handleMatrixCalculations((Vector){0.0f, 0.0f, -8.0f});

	// Let the GPU know about our vertex arrays...	
	GX_SetArray(GX_VA_POS, mySkyboxCoords, sizeof(Vector));	
	GX_SetArray(GX_VA_TEX0, mySkyboxTextures, sizeof(float) * 2);
	GX_SetArray(GX_VA_NRM, mySkyboxNormals, sizeof(Vector));

	// Render the skybox
	setupTextureObject(skyboxID);
	renderSceneOrSkybox();

	// Switch vertex arrays and render the scenebox
	GX_SetNumTevStages(1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	handleMatrixCalculations((Vector){0.0f, 0.0f, -6.0f});

	GX_SetArray(GX_VA_POS, mySceneboxCoords, sizeof(Vector));
	setupTextureObject(sceneboxID);
	renderSceneOrSkybox();

	endRenderingPass();
}

///<summary>
/// Returns whether or not it's within our frustum, and then how far away it is...
///</summary>							
SEEN_BY_CAMERA GxGraphics::isLocationWithinViewingRange(Vector location)
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

/*
// Render our quads with additive blending :)
GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, (2 * (quads.size() - 1)) + 4);		

// Loop through our map using an iterator and draw the quad objects...
std::map<int, Quad>	::iterator quadItr;
quadItr = quads.begin();	

// Draw the first triangle of the quad

GX_Position3f32(quadItr->second.verticies[3].x, quadItr->second.verticies[3].y, quadItr->second.verticies[3].z);
GX_Normal3f32(quadItr->second.normal.x, quadItr->second.normal.y, quadItr->second.normal.z);
GX_TexCoord2f32(quadItr->second.texture_S[3], quadItr->second.texture_T[3]);		

GX_Position3f32(quadItr->second.verticies[0].x, quadItr->second.verticies[0].y, quadItr->second.verticies[0].z);
GX_Normal3f32(quadItr->second.normal.x, quadItr->second.normal.y, quadItr->second.normal.z);		
GX_TexCoord2f32(quadItr->second.texture_S[0], quadItr->second.texture_T[0]);		

GX_Position3f32(quadItr->second.verticies[2].x, quadItr->second.verticies[2].y, quadItr->second.verticies[2].z);
GX_Normal3f32(quadItr->second.normal.x, quadItr->second.normal.y, quadItr->second.normal.z);
GX_TexCoord2f32(quadItr->second.texture_S[2], quadItr->second.texture_T[2]);


GX_Position3f32(quadItr->second.verticies[1].x, quadItr->second.verticies[1].y, quadItr->second.verticies[1].z);
GX_Normal3f32(quadItr->second.normal.x, quadItr->second.normal.y, quadItr->second.normal.z);
GX_TexCoord2f32(quadItr->second.texture_S[1], quadItr->second.texture_T[1]);		


// Now to get the rest of the quads we indciate the top left and bottom right corner of each subsequent triangle
for(u32 idx = 1; idx < quads.size(); idx++)
{
// Go to the next quad
quadItr++;

// Indicate the bottom right vertex to draw the first triangle of the next quad...
GX_Position3f32(quadItr->second.verticies[2].x, quadItr->second.verticies[2].y, quadItr->second.verticies[2].z);
GX_Normal3f32(quadItr->second.normal.x, quadItr->second.normal.y, quadItr->second.normal.z);
GX_TexCoord2f32(quadItr->second.texture_S[2], quadItr->second.texture_T[2]);

// Indicate the top left vertex to complete the previous triangle		
GX_Position3f32(quadItr->second.verticies[1].x, quadItr->second.verticies[1].y, quadItr->second.verticies[1].z);
GX_Normal3f32(quadItr->second.normal.x, quadItr->second.normal.y, quadItr->second.normal.z);
GX_TexCoord2f32(quadItr->second.texture_S[1], quadItr->second.texture_T[1]);
}

GX_End();	
*/

///<summary>
/// Adds quads from a map based on the rectangle created by xMin, xMax, yMin, and yMax
///</summary>			
void GxGraphics::addQuadMapToScene(Vector location, std::map<int, Quad> &quads, int xMin, int xMax, int yMin, int yMax, int rowSize, TextureHandle textureID)
{
	int rowIndex = 0;
	int quadIndex = 0;
	std::map<int, Quad>::iterator mapIterator;

	// Now is a good time to load the texture object
	ASSERT(myGxTextures.empty() == false);
	//ASSERT(textureID <= myGxTextures.size() + 1);	

	//GX_SetCullMode(GX_CULL_NONE);

	setupRenderingPass();
	setupNRMDirectVertexDescriptors();	
	handleMatrixCalculations(location);
	setupTextureObject(textureID);

	// Loop through all of the X values in our tilemap layer and assign them to the tilemap layer
	for(int indexY = yMin; indexY <= yMax; indexY++)
	{
		// Get our index into the tilemap x/y coordinates
		rowIndex = (rowSize * indexY);
		for(int indexX = xMin; indexX <= xMax; indexX++)
		{	
			// If we find a matching index in the map, then draw that index!
			quadIndex = (rowIndex + indexX);
			mapIterator = quads.find(quadIndex);
			if(mapIterator->first == quadIndex)
			{
				GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, 4);		

				GX_Position3f32(quads[quadIndex].verticies[3].x, quads[quadIndex].verticies[3].y, quads[quadIndex].verticies[3].z);
				GX_Normal3f32(quads[quadIndex].normals[3].x, quads[quadIndex].normals[3].y, quads[quadIndex].normals[0].z);
				GX_TexCoord2f32(quads[quadIndex].texture_S[3], quads[quadIndex].texture_T[3]);		

				GX_Position3f32(quads[quadIndex].verticies[0].x, quads[quadIndex].verticies[0].y, quads[quadIndex].verticies[0].z);
				GX_Normal3f32(quads[quadIndex].normals[0].x, quads[quadIndex].normals[0].y, quads[quadIndex].normals[0].z);		
				GX_TexCoord2f32(quads[quadIndex].texture_S[0], quads[quadIndex].texture_T[0]);		

				GX_Position3f32(quads[quadIndex].verticies[2].x, quads[quadIndex].verticies[2].y, quads[quadIndex].verticies[2].z);
				GX_Normal3f32(quads[quadIndex].normals[2].x, quads[quadIndex].normals[2].y, quads[quadIndex].normals[2].z);
				GX_TexCoord2f32(quads[quadIndex].texture_S[2], quads[quadIndex].texture_T[2]);

				GX_Position3f32(quads[quadIndex].verticies[1].x, quads[quadIndex].verticies[1].y, quads[quadIndex].verticies[1].z);
				GX_Normal3f32(quads[quadIndex].normals[1].x, quads[quadIndex].normals[1].y, quads[quadIndex].normals[1].z);
				GX_TexCoord2f32(quads[quadIndex].texture_S[1], quads[quadIndex].texture_T[1]);

				GX_End();
			}			
		}
	}	
	endRenderingPass();
}

///<summary>
/// Returns the current number of frames being rendered per second...
///</summary>							
int GxGraphics::getFPS()
{
	return myFPS;
}

///<summary>
/// Similar to guLookAt, this will allow us to move our camera around as the game continues...
///</summary>								
void GxGraphics::setCameraPosition(Camera camera)
{
	myCamera = camera;
}	

///<summary>
/// Similar to setCameraPosition, however this focuses only on the the x,y,z vector we want our camera to point at
/// The heightOfScene variable indicates if the camera is intersecting with something taller than it is (that
/// would obstruct our view). If it is, then it won't adjust position until that changes...
///</summary>								
void GxGraphics::setCameraTarget(Vector target, float targetAngle, float heightOfScenery)
{		

	// Set where our camera is looking
	myCamera.look = target;

	// As long as the camera is not going to be obstructed by scenery, adjust the angle to be behind the target...
	//if ((heightOfScenery - 5.0f) >= myCamera.pos.z)
	//{
		float myCameraXMovement = 0;
		float myCameraYMovement = 0;

		// Adjust our camera angle to match up with the target angle
		//printf("\x1b[2;0H");
		//printf("Rad,Dis: %f, %f", myCamera.radius, myCamera.distance);
		adjustAngleRotation(myCamera.rotation, targetAngle, 96.0f);
		float angleInRads = getRadiansForAngle(myCamera.rotation);
		float xValue = (myCamera.radius * cos(angleInRads));
		float yValue = (myCamera.radius * sin(angleInRads));

		// Move our camera target location to match up with the opposite of the target interpolating by the number of fps
		myCameraXMovement = fabs(myCamera.target.x - xValue);
		myCameraXMovement = ((gTimeScale * 2.0f) * myCameraXMovement);
		if (myCamera.target.x < xValue)
		{
			myCamera.target.x += myCameraXMovement;
		}
		else if (myCamera.target.x > xValue)
		{
			myCamera.target.x -= myCameraXMovement;
		}

		myCameraYMovement = fabs(myCamera.target.y - yValue);
		myCameraYMovement = ((gTimeScale * 2.0f) * myCameraYMovement);
		if (myCamera.target.y < yValue)
		{
			myCamera.target.y += myCameraYMovement;
		}
		else if (myCamera.target.y > yValue)
		{
			myCamera.target.y -= myCameraYMovement;
		}

	//}

	// Adjust the position, up and current angle of the camera from the target based on above or previous calculations
	myCamera.pos.x = myCamera.target.x;
	myCamera.pos.y = myCamera.target.y;
	myCamera.pos.z = target.z - (myCamera.distance);

	myCamera.up.x = -(myCamera.pos.x - myCamera.look.x);
	myCamera.up.y = -(myCamera.pos.y - myCamera.look.y);

	myCamera.angleFromLook = getAngleBetweenTwoPoints(myCamera.pos.x, myCamera.pos.y, 0.0f, 0.0f);
	myCamera.angleFromLook -= 90.0f;
	if (myCamera.angleFromLook < 0)
	{
		myCamera.angleFromLook += 360.0f;
	}
}

///<summary>
/// Defines the angle of the camera as it's viewing the player ...
///</summary>								
float GxGraphics::getCameraAngle()
{
	return myCamera.angleFromLook;
}



///<summary>
/// Sets the mode used for rendering fog
///</summary>								
void GxGraphics::setFogMode(f32 startZ, f32 endZ, f32 nearZ, f32 farZ, Color color)
{
	/*myLightDirection.z += startZ *0.1;
	myLightDirection.x += endZ   * 0.1;
	myLightDirection.y += nearZ  * 0.1;
	//myLightDirection = myCamera.look;
	//
	//printf("\x1b[2;0H"); printf("\n\n%f, %f, %f", myLightDirection.x, myLightDirection.y, myLightDirection.z);usleep(10000);
	*/
	GXColor gxColor;
	gxColor.r = (u8)(color.r * 255);
	gxColor.g = (u8)(color.g * 255);
	gxColor.b = (u8)(color.b * 255);
	gxColor.a = 64;
	GX_SetFog(GX_FOG_PERSP_LIN, startZ, endZ, nearZ, farZ, gxColor);
}

///<summary>
/// Renders all of the primitives previously defined in the scene
///</summary>				
void GxGraphics::endScene()
{
	GX_SetZMode(GX_ENABLE, GX_LEQUAL, GX_TRUE);
	GX_SetZCompLoc( GX_FALSE);
	GX_SetColorUpdate( GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);
	gFrameSequenceNumber++;
	gFrameBuffers[gCurrentDrawBuffer].SequenceNumber = gFrameSequenceNumber;
	GX_CopyDisp(gFrameBuffers[gCurrentDrawBuffer].GxFrameBuffer[0], GX_TRUE);
	GX_DrawDone();

	VIDEO_SetNextFramebuffer(gFrameBuffers[gCurrentDrawBuffer].GxFrameBuffer[0]);
	if (myFirstGxFrame)
	{
		VIDEO_SetBlack( FALSE);
		myFirstGxFrame = 0;
	}

	VIDEO_Flush();

	//VIDEO_WaitVSync();

	// Increment our draw buffer, and make sure we're working within the # of buffers specified
	gCurrentDrawBuffer++;
	if (gCurrentDrawBuffer >= NUMBER_OF_GFX_BUFFERS)
	{
		gCurrentDrawBuffer = 0;
	}

	// This code was supposed to help us process the fifo buffer on GP and CPU, but it
	// never really seemed to work, so we're going back with vsync and trying to find
	// another way to speed the system up ...

	/*// Since we've set up our GP to be a frame behind, we're going to copy the buffer that we're not writing to
	if (gCurrentDrawBuffer == 1)
	{
		GX_CopyDisp(gFrameBuffers[0].GxFrameBuffer[0], GX_TRUE);
	}
	else
	{
		GX_CopyDisp(gFrameBuffers[1].GxFrameBuffer[0], GX_TRUE);
	}

	// Ok, if we change the drawdone token here, ReadDrawSync will return it once drawing is done...
	GX_SetDrawSync(FIFO_DRAWDONE_TOKEN);

	// wait until GP is finished by polling for the drawdone token in
	// current GP Fifo (previously processed by CPU)
	while ((GX_GetDrawSync()) != FIFO_DRAWDONE_TOKEN)
	{
		GX_DisableBreakPt();
	}

	// Now we're going to Swap our buffers and display what we just copied out...
	VIDEO_SetNextFramebuffer(gFrameBuffers[gCurrentDrawBuffer].GxFrameBuffer[0]);

	if (myFirstGxFrame)
	{
		VIDEO_SetBlack( FALSE);
		myFirstGxFrame = 0;
	}


	VIDEO_Flush();

	VIDEO_WaitVSync();

	// Increment our draw buffer, and make sure we're working within the # of buffers specified
	gCurrentDrawBuffer++;
	if (gCurrentDrawBuffer >= NUMBER_OF_GFX_BUFFERS)
	{
		gCurrentDrawBuffer = 0;
	}*/


	/*
	//regular way of doing it
	GX_SetZMode(GX_ENABLE, GX_LEQUAL, GX_TRUE);
	GX_SetZCompLoc(GX_FALSE);
	GX_SetColorUpdate(GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);
	gFrameSequenceNumber++;
	gFrameBuffers[gCurrentDrawBuffer].SequenceNumber = gFrameSequenceNumber;
	GX_CopyDisp(gFrameBuffers[gCurrentDrawBuffer].GxFrameBuffer[0], GX_TRUE);
	GX_DrawDone();
	VIDEO_SetNextFramebuffer(gFrameBuffers[gCurrentDrawBuffer].GxFrameBuffer[0]);
	 */


}


///<summary>
/// clears the current texture data in video memory
///</summary>
void GxGraphics::clearTextureData()
{
	// Delete any GxTexture data that's still stored in our map
	GxTexMapIterator mapIterator;
	mapIterator = myGxTextures.begin();	
	
	// Delete all of our GX Texture data from memory, then clear our map out...
	for(uint index = 0; index < myGxTextures.size(); index++)
	{
		GxTextureData *data = &mapIterator->second;						
		free(data->textureData);		
		mapIterator++;
	}
	myGxTextures.clear();

	myCurrentTextureID = 0;
}

///<summary>
/// private member that will initialize a texture object
///</summary>
void GxGraphics::initTextureObject(GxTextureData &textureData)
{
	u32 wrapS = GX_CLAMP;
	u32 wrapT = GX_CLAMP;
	u32 mipMap = GX_FALSE;
	u32 filter = GX_LIN_MIP_NEAR;
	u32 format = GX_TF_RGBA8;
	//float minLOD = 0.0f;
	//float maxLOD = 0.0f;
	//u32 clampAndBias = GX_ENABLE;

	switch(textureData.format)
	{
	case IMAGE_TEXTURE:
		textureData.textureMap = GX_TEXMAP0;
		break;
	case LIGHT_MAP_TEXTURE_TEMP:		
		//format = GX_TF_IA8;
		textureData.textureMap = GX_TEXMAP0;			
		//textureData.wrap = true;
		break;
	case LIGHT_MAP_TEXTURE:
		//format = GX_TF_IA8;
		textureData.textureMap = GX_TEXMAP1;
		break;
	case NORMAL_MAP_TEXTURE:
		textureData.textureMap = GX_TEXMAP2;
		break;
	case REFLECTION_TEXTURE:
	case MULTIPASS_IMAGE_TEXTURE:
		textureData.textureMap = GX_TEXMAP3;
		break;	
	case HEIGHT_TEXTURE: 
		format = GX_TF_Z24X8;		
		textureData.wrap = true;
		filter = GX_NEAR;	
		textureData.textureMap = GX_TEXMAP1;	
		break;
	default:			
		ASSERT(false); 
		textureData.textureMap = GX_TEXMAP0;				
		break;
	}

	if(textureData.wrap == true)
	{
		wrapS = GX_REPEAT;
		wrapT = GX_REPEAT;
		mipMap = GX_FALSE;
	}	

	GX_InitTexObj (&textureData.textureObject, textureData.textureData, textureData.width, textureData.height, format, wrapS, wrapT, mipMap);
	//GX_InitTexObjLOD(&textureData.textureObject, filter, GX_LINEAR, minLOD, maxLOD, 0.3f, clampAndBias, clampAndBias, GX_ANISO_1);
}

///<summary>
/// Loads texture information from a .png file
///</summary>
///<returns>A GxTextureData object populated with the specified information</returns>
GxTextureData allocatePNGTexture(const char* fileName)
{
	IMGCTX ctx;
	PNGUPROP imgProp;
	GxTextureData gxTexture;

	gxTexture.textureData = NULL;

	// Get our image context and properties using the PNGU procedures
	ctx = PNGU_SelectImageFromDevice(fileName);
	ASSERT(ctx != NULL);
	PNGU_GetImageProperties (ctx, &imgProp);

	gxTexture.width = imgProp.imgWidth;
	gxTexture.height = imgProp.imgHeight;

	// Align our texture information so that it properly displays	
	gxTexture.bytesPerTexel = 4;
	gxTexture.size = imgProp.imgWidth * imgProp.imgHeight * 4;

	//gxTexture.textureData = new char[gxTexture.size];
	gxTexture.textureData = memalign (32, gxTexture.size);		
	//gxTexture.textureData = simpleWiiHeapAllocatePersistentMemory(gxTexture.size);
	
	ASSERT(gxTexture.textureData != NULL);

	if(gxTexture.textureData != NULL)
	{
		// Make sure our texture png is converted to the proper format ...
		PNGU_DecodeTo4x4RGBA8(ctx, imgProp.imgWidth, imgProp.imgHeight, gxTexture.textureData, 0);
		// And call flush for the texture memory DC
		DCFlushRange (gxTexture.textureData, imgProp.imgWidth * imgProp.imgHeight * 4);		
		// Make sure we close ye olde file handle ...
		PNGU_ReleaseImageContext (ctx);

	}
	else
	{
		printf("\nCOULD NOT ALLOCATE: %s.\nTRIGGERING BREAK POINT!!!", fileName);
		_break();
	}

	return gxTexture;
}

///<summary>
/// Loads texture information from a .tpl file
///</summary>
///<returns>A GxTextureData object populated with the specified information</returns>
GxTextureData allocateHeightTexture(const char *fileName)
{
	GxTextureData gxTexture;
	FILE *fileHandle = NULL;

	// TODO: Figure out how to get width and height from a bmp image ...
	gxTexture.width = 512;
	gxTexture.height = 512;
	gxTexture.bytesPerTexel = 3;
	gxTexture.size = (gxTexture.width * gxTexture.height * gxTexture.bytesPerTexel);

	fileHandle = fopen(fileName, "rb");
	if (fileHandle != NULL)
	{	
		//gxTexture.textureData = new char[gxTexture.size];
		gxTexture.textureData = gxTexture.textureData = memalign(32, gxTexture.size); 
		//simpleWiiHeapAllocatePersistentMemory(gxTexture.size);
		ASSERT(gxTexture.textureData != NULL);
		if(gxTexture.textureData != NULL)
		{
			fread(gxTexture.textureData, 1, gxTexture.size, fileHandle);						
		}		
	}// end if(myFileHandle	

	// close the file and return
	fclose(fileHandle);	

	return gxTexture;
}

///<summary>
/// Sets up the light map texture that we'll use for lighting / bump mapping...
///</summary>
void GxGraphics::setupLightMapTexture()
{
	myLightMapTexHandle = UNDEFINED_TEXTURE_HANDLE;

	// Here we allocate the reflection texture based on the screen w and h 
	myLightMapTexture.textureData = NULL;
	myLightMapTexture.format = LIGHT_MAP_TEXTURE;
	myLightMapTexture.width = LIGHT_MAP_WIDTH;
	myLightMapTexture.height = LIGHT_MAP_HEIGHT;

	// Align our texture information so that it properly displays	
	myLightMapTexture.bytesPerTexel = 4;
	myLightMapTexture.size = GX_GetTexBufferSize(LIGHT_MAP_WIDTH, LIGHT_MAP_HEIGHT, GX_TF_RGBA8, FALSE, 0);

	myLightMapTexture.textureData = memalign (32, myLightMapTexture.size);		
	
	ASSERT(myLightMapTexture.textureData != NULL);

	// And then append this to our list of available textures
	myCurrentTextureID++;		
	myLightMapTexture.wrap = false;				
	initTextureObject(myLightMapTexture);
	myGxTextures[myCurrentTextureID] = myLightMapTexture;	
	myLightMapTexHandle = myCurrentTextureID;
}


///<summary>
/// Sets up the reflection texture that we'll use with normal mapping water tiles...
///</summary>
void GxGraphics::setupReflectionTexture()
{			
	myReflectionTexHandle = UNDEFINED_TEXTURE_HANDLE;

	// Here we allocate the reflection texture based on the screen w and h 
	myReflectionTexture.textureData = NULL;

	myReflectionTexture.format = REFLECTION_TEXTURE;

	myReflectionTexture.width = myRegisterMode->fbWidth;
	myReflectionTexture.height = myRegisterMode->efbHeight;

	// Align our texture information so that it properly displays	
	myReflectionTexture.bytesPerTexel = 4;
	myReflectionTexture.size = myReflectionTexture.width * myReflectionTexture.height * 4;

	myReflectionTexture.textureData = memalign (32, myReflectionTexture.size);		
	//myReflectionTexture.textureData = simpleWiiHeapAllocatePersistentMemory(myReflectionTexture.size);
	
	ASSERT(myReflectionTexture.textureData != NULL);

	// And then append this to our list of available textures
	myCurrentTextureID++;		
	myReflectionTexture.wrap = false;
	initTextureObject(myReflectionTexture);
	myGxTextures[myCurrentTextureID] = myReflectionTexture;	
	myReflectionTexHandle = myCurrentTextureID;
}

///<summary>
/// Loads texture information into video memory ...
///</summary>
///<returns>A handle indicating the texture ID</returns>
TextureHandle GxGraphics::addTextureData(const char* fileName, TEXTURE_FORMAT format, bool wrap)
{	
	GxTextureData gxTexture;

	// Height textures are in an I8 format
	if(format == HEIGHT_TEXTURE)
	{
		gxTexture = allocateHeightTexture(fileName);		
	}
	else
	{
		gxTexture = allocatePNGTexture(fileName);
	}

	if(gxTexture.textureData != NULL)
	{		
		// And then append this to our list of available textures
		myCurrentTextureID++;		
		gxTexture.format = format;
		gxTexture.wrap = wrap;				
		initTextureObject(gxTexture);
		myGxTextures[myCurrentTextureID] = gxTexture;		
	}

	return myCurrentTextureID;
}

///<summary>
/// Removes allocated texture data from memory
///</summary>
void GxGraphics::freeTextureData(TextureHandle handle)
{
	GxTexMapIterator textureIterator = myGxTextures.find(handle);
	if(textureIterator != myGxTextures.end())
	{
		free(textureIterator->second.textureData);
		myGxTextures.erase(textureIterator);
	}
}


///<summary>
/// In theory pushes the current matrix onto a stack so we can revert after changes are made
/// but we only need it once right now, so we're going to do it that way ...
///</summary>
void GxGraphics::switchToOverheadView()
{
	Camera camera;
	camera.pos.x = 0.0F;
	camera.pos.y = 0.0F;
	camera.pos.z = 0.0F;
	camera.up.x = 0.0F;
	camera.up.y = -1.0F;
	camera.up.z = 0.0F;
	camera.look.x = 0.0F;
	camera.look.y = 0.0F;
	camera.look.z = 20.0F;
	guLookAt(myView, &camera.pos, &camera.up, &camera.look);
}

///<summary>
/// Get the matrix we pushed and set it as our current matrix
///</summary>
void GxGraphics::switchBackToCameraView()
{
	guLookAt(myView, &myCamera.pos, &myCamera.up, &myCamera.look);
}

///<summary>
/// Used to calculate a tangent from a normal for use with bump mapping...
///</summary>							
static Vector calculateTangent(Vector normal)
{
	static Vector crossVectorA = {0.0f, 0.0f, 1.0f};
	static Vector crossVectorB = {0.0f, 1.0f, 0.0f};
	Vector crossProductA;	
	Vector crossProductB;	

	// Get the cross product for y and z perpindicular vectors, and return the one with the greatest "length" (sum of elements)
	fastVectorCrossProduct(&normal, &crossVectorA, &crossProductA);
	fastVectorCrossProduct(&normal, &crossVectorB, &crossProductB);

	if(fastVectorLength(&crossProductA) > fastVectorLength(&crossProductB))
	{
		fastVectorNormalize(&crossProductA);
		return crossProductA;
	}
	else
	{
		fastVectorNormalize(&crossProductB);
		return crossProductB;
	}
}

///<summary>
/// Used to calculate a binormal from a normal and tangent pair for use with bump mapping...
///</summary>							
static Vector calculateBinormal(Vector normal, Vector tangent)
{
	Vector biNormal;
	fastVectorCrossProduct(&normal, &tangent, &biNormal);
	fastVectorNormalize(&biNormal);
	return biNormal;
}

///<summary>
/// Calculates and populates NBT values for an offset and a quad vertex array
///</summary>							
static void calculateNormalValues(int normalOffset, int tangentOffset, int binormalOffset, Vector normal, QuadVertexArray &vertexArray)
{
	Vector tangent;
	Vector binormal;
	
	// normal
	vertexArray.Normals[normalOffset].x = FTOFRAC8(normal.x);
	vertexArray.Normals[normalOffset].y = FTOFRAC8(normal.y);
	vertexArray.Normals[normalOffset].z = FTOFRAC8(normal.z);
	// tangent
	tangent = calculateTangent(normal);
	vertexArray.Normals[tangentOffset].x = FTOFRAC8(tangent.x);
	vertexArray.Normals[tangentOffset].y = FTOFRAC8(tangent.y);
	vertexArray.Normals[tangentOffset].z = FTOFRAC8(tangent.z);
	// binormal	
	binormal = calculateBinormal(normal, tangent);
	vertexArray.Normals[binormalOffset].x = FTOFRAC8(binormal.x);
	vertexArray.Normals[binormalOffset].y = FTOFRAC8(binormal.y);
	vertexArray.Normals[binormalOffset].z = FTOFRAC8(binormal.z);
}

///<summary>
/// Calculates and populates NBT values for an offset and triangle vertex array
///</summary>							
static void calculateNormalValues(int normalOffset, int tangentOffset, int binormalOffset, Vector normal, TriangleVertexArray &vertexArray)
{
	Vector tangent;
	Vector binormal;
	
	// normal
	vertexArray.Normals[normalOffset].x = FTOFRAC8(normal.x);
	vertexArray.Normals[normalOffset].y = FTOFRAC8(normal.y);
	vertexArray.Normals[normalOffset].z = FTOFRAC8(normal.z);
	// tangent
	tangent = calculateTangent(normal);
	vertexArray.Normals[tangentOffset].x = FTOFRAC8(tangent.x);
	vertexArray.Normals[tangentOffset].y = FTOFRAC8(tangent.y);
	vertexArray.Normals[tangentOffset].z = FTOFRAC8(tangent.z);
	// binormal	
	binormal = calculateBinormal(normal, tangent);
	vertexArray.Normals[binormalOffset].x = FTOFRAC8(binormal.x);
	vertexArray.Normals[binormalOffset].y = FTOFRAC8(binormal.y);
	vertexArray.Normals[binormalOffset].z = FTOFRAC8(binormal.z);
}

///<summary>
/// Calculates the normal offset values
///</summary>
static void calculateNormalOffsets(u32 &normalOffset, u32 &tangentOffset, u32 &binormalOffset, int vertexNumber, QuadVertexArray &vertexArray)
{
	ASSERT(vertexArray.VertexArraySize >= 1);

	if(vertexArray.VertexArraySize > 1)
	{
		normalOffset = (vertexNumber);
		binormalOffset = (vertexNumber) + (vertexArray.VertexArraySize / 3);
		tangentOffset  = (vertexNumber) + ((vertexArray.VertexArraySize / 3) * 2);
	}
	else
	{
		// These are the offsets for the normals contained in a 1x1 quad map, which has a different format than a map of any other size...
		normalOffset   = 0;
		binormalOffset = 4;
		tangentOffset  = 8;
	}
}


///<summary>
/// Calculates the right hand set of normals of the last quad in a row in the function below...
/// Never implemented, but extracted to make code more readable...
///</summary>
/*static void calculateRightSideNormals()
{
	// This has never been implemented, but one day we might want those normals :D
	if(columnNumber >= (quadsWide - 1))
	{
		vertexNumber++;
		vertexArray.Verticies[vertexNumber] = quadItr->second.verticies[1];
		calculateNormalValues(normalOffset+1, binormalOffset+1, tangentOffset+1, quadItr->second.normals[1], vertexArray);
		columnNumber = -1;
		rowNumber++;
	}
}*/

///<summary>
/// Populates texture coordinates based on the width and height of a tilemap...
///</summary>
static void populateQuadTexCoords(QuadVertexArray &vertexArray, QuadVertexArrayProperties arrayProperties)
{
	ASSERT(vertexArray.TextureCoordsST != NULL);
	ASSERT(arrayProperties.quadTexWidth > 0.0f);
	ASSERT(arrayProperties.quadTexHeight > 0.0f);

	for(int yIndex = 0; yIndex < arrayProperties.texTilesHigh; yIndex++)
	{
		for(int xIndex = 0; xIndex < arrayProperties.texTilesWide; xIndex++)
		{
			int textureIndex = ((yIndex * arrayProperties.texTilesWide) + xIndex) * 8;
			float topLeftS = (xIndex * arrayProperties.quadTexWidth);
			float topLeftT = (yIndex * arrayProperties.quadTexHeight);

			// Top left
			vertexArray.TextureCoordsST[textureIndex    ] = topLeftS;
			vertexArray.TextureCoordsST[textureIndex + 1] = topLeftT;

			// Top right
			vertexArray.TextureCoordsST[textureIndex + 2] = topLeftS + arrayProperties.quadTexWidth;
			vertexArray.TextureCoordsST[textureIndex + 3] = topLeftT;

			// Bottom right
			vertexArray.TextureCoordsST[textureIndex + 4] = topLeftS + arrayProperties.quadTexWidth;
			vertexArray.TextureCoordsST[textureIndex + 5] = topLeftT + arrayProperties.quadTexHeight;

			// Bottom Left
			vertexArray.TextureCoordsST[textureIndex + 6] = topLeftS;
			vertexArray.TextureCoordsST[textureIndex + 7] = topLeftT + arrayProperties.quadTexHeight;
		}
	}
}

///<summary>
/// Handles allocation for a quad element...
///</summary>
static void allocateQuadVertexArrayElements(std::map<int, Quad> &quads, QuadVertexArray &vertexArray, QuadVertexArrayProperties arrayProperties)
{
	//  If it's a single 1x1 quad map, then our verts needed will always be 4 (as opposed to any other combination)...
	int numberOfVertsNeeded = 4;

	// Assign the total number of elements in the vertex array...
	vertexArray.VertexArraySize = quads.size();

	vertexArray.VertexStride = (sizeof(Vector));
	vertexArray.NormalStride = sizeof(fixedVector);
	vertexArray.TextureStride = (sizeof(float) * 2);

	//  The quads share many coordinates and provide an opportunity to reduce memory consumption.
	//  We will however be required to avoid using the far right and bottom row/column to get this feature
	if ((arrayProperties.quadsWide > 1) || (arrayProperties.quadsHigh > 1))
	{
		numberOfVertsNeeded = ((arrayProperties.quadsWide) * (arrayProperties.quadsHigh));
	}

	//TODO: In this scenario we can try switching to aligned memory for a small performance increase
	// We need to allocate 4 vectors for each element in the quad map
	vertexArray.Verticies = new Vector [numberOfVertsNeeded];
	//(Vector*)memalign (32, (sizeof(Vector) * quads.size() * 4));

	// We need to allocate a three normals for each vertex in the array (normal, binormal, and tangent)
	vertexArray.Normals = new fixedVector [numberOfVertsNeeded * 3];
	//(Vector*)memalign (32, (sizeof(Vector) * quads.size() * 4));

	// We'll need w*h of the graphic tiles /32 texcoords w/ two for each corner (one for S and one for T, a total of eight per quad)
	// e.g. a 512x640 image is 16x20 tiles is 320 possible texture combinations
	int numberOfTexCoordsNeedeed = ((arrayProperties.texTilesHigh * arrayProperties.texTilesWide) * 8);
	vertexArray.TextureCoordsST = new float [numberOfTexCoordsNeedeed];
	//(float* )memalign (32, (sizeof(float) * quads.size() * 8));

	// We need to allocate a single short for each quad in the map indicating a value ((y * row size) + x) of where the top left coordiante begins
	vertexArray.TexCoordLookup = new short [quads.size()];
}

///<summary>
/// Allocates and configures a vertex array for use with the GPU ... 
///</summary>							
void GxGraphics::allocateAndPopulateVertexArray(std::map<int, Quad> &quads, QuadVertexArray &vertexArray,
		QuadVertexArrayProperties arrayProperties)
{
	int columnNumber = 0;
	int vertexNumber = 0;
	u32 normalOffset   = 0;
	u32 binormalOffset = 0;
	u32 tangentOffset  = 0;

	// Assign the backface culling property. Some of our models can use it, some can't...
	vertexArray.properties = arrayProperties;

	// Allocate stride and memory data for our quad vertex array ...
	allocateQuadVertexArrayElements(quads, vertexArray, vertexArray.properties);

	// Here we go ahead and populate the texture coordinates for the vertex array
	populateQuadTexCoords(vertexArray, vertexArray.properties);

	// Now that we've allocated the vertex array, we can loop through our quads and populate it!
	std::map<int, Quad>::iterator quadItr;
	quadItr = quads.begin();

	// If the map is more the 1x1 then we only need to record the top left corner...
	if((vertexArray.properties.quadsWide > 1) || (vertexArray.properties.quadsHigh > 1))
	{
		for(u32 idx = 0; idx < quads.size(); idx++)
		{
			// Copy and store the top left corner of the quad (this is mostly all we need)
			vertexArray.Verticies[vertexNumber] = quadItr->second.verticies[0];

			// Calculate and store the normal, binormal, and tangent for the top left corner
			calculateNormalOffsets(normalOffset, tangentOffset, binormalOffset, vertexNumber, vertexArray);
			calculateNormalValues(normalOffset, binormalOffset, tangentOffset, quadItr->second.normals[0], vertexArray);

			// If we're at the end of a row, copy out the right hand portion of the quad...
			//calculateRightSideNormals();

			columnNumber++;
			vertexNumber++;

			// We only need to assign one coordinate lookup per quad, which will point to the tex coords for the top left corner of our graphic...
			vertexArray.TexCoordLookup[idx] = ((quadItr->second.texIndexY * vertexArray.properties.texTilesWide) + quadItr->second.texIndexX)*4;

			quadItr++;
		}
	}
	// If this is a single quad, then we need to populate all four corners in order to draw it properly...
	// We'll have code to check for a single quad later in the addNormalMappedVertexArray routine for quads
	else if((vertexArray.properties.quadsWide == 1) && (vertexArray.properties.quadsHigh == 1))
	{
		ASSERT(vertexNumber == 0);

		// Copy and store the top left corner of the quad (this is mostly all we need)
		vertexArray.Verticies[vertexNumber  ] = quadItr->second.verticies[0];
		vertexArray.Verticies[vertexNumber+1] = quadItr->second.verticies[1];
		vertexArray.Verticies[vertexNumber+2] = quadItr->second.verticies[2];
		vertexArray.Verticies[vertexNumber+3] = quadItr->second.verticies[3];

		// Calculate and store the normal, binormal, and tangent for the top left corner
		calculateNormalOffsets(normalOffset, tangentOffset, binormalOffset, vertexNumber, vertexArray);

		calculateNormalValues(normalOffset,   binormalOffset,   tangentOffset,   quadItr->second.normals[0], vertexArray);
		calculateNormalValues(normalOffset+1, binormalOffset+1, tangentOffset+1, quadItr->second.normals[1], vertexArray);
		calculateNormalValues(normalOffset+2, binormalOffset+2, tangentOffset+2, quadItr->second.normals[2], vertexArray);
		calculateNormalValues(normalOffset+3, binormalOffset+3, tangentOffset+3, quadItr->second.normals[3], vertexArray);


		// Top left
		vertexArray.TextureCoordsST[0] = quadItr->second.texture_S[0];
		vertexArray.TextureCoordsST[1] = quadItr->second.texture_T[0];

		// Top right
		vertexArray.TextureCoordsST[2] = quadItr->second.texture_S[1];
		vertexArray.TextureCoordsST[3] = quadItr->second.texture_T[1];

		// Bottom right
		vertexArray.TextureCoordsST[4] = quadItr->second.texture_S[2];
		vertexArray.TextureCoordsST[5] = quadItr->second.texture_T[2];

		// Bottom Left
		vertexArray.TextureCoordsST[6] = quadItr->second.texture_S[3];
		vertexArray.TextureCoordsST[7] = quadItr->second.texture_T[3];


		// We should only have one entry for a 1x1 map...
		vertexArray.TexCoordLookup[0] = 0;
	}
	else
	{
		ASSERT(false);
	}
}

/*if(idx >= 19456)
{
trace("%d, %2.2f, %2.2f, %2.2f\n\r",
	idx,
	vertexArray.Verticies[vertexOffset].x,
	vertexArray.Verticies[vertexOffset].y,
	vertexArray.Verticies[vertexOffset].z,
	vertexArray.TextureCoordsST[texCoordOffset],
	vertexArray.TextureCoordsST[texCoordOffset+1]);
}*/

// Handles Matrix Calculations for Rotation and Translation
void GxGraphics::handleMatrixCalculations(Vector location)
{
	guMtxIdentity(myModel);

	// If we should rotate then apply the transform here ...
	checkObjectRotation();

	guMtxTransApply(myModel, myModel, location.x, location.y, location.z);
	guMtxConcat(myView, myModel, myModelView);

	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(myModelView, GX_PNMTX0);

	guMtxInverse(myModel, myModelView);
	guMtxTranspose(myModelView, myModel);
}





///<summary>
/// Allocates and configures a vertex array for use with the GPU ...
/// IMPORTANT: If vertexArray.Normals == NULL then it will allocate normal data, but if it's not
/// then this method WILL NOT ALLOCATE NORMAL DATA! (The idea is that the normal data will be pointing
/// to preallocated normal data to save space & reduce cache misses...
///</summary>							
void GxGraphics::allocateAndPopulateVertexArray(std::vector<Triangle> &triangles, TriangleVertexArray &vertexArray, bool cullBacks)
{
	bool populateNormalData = false;

	// Assign the backface culling property. Some of our models can use it, some can't...
	vertexArray.CullTriangleBacks = cullBacks;

	// Assign the total number of elements in the vertex array...
	vertexArray.VertexArraySize = triangles.size();

	vertexArray.VertexStride = (sizeof(Vector));
	vertexArray.NormalStride = sizeof(fixedVector);
	vertexArray.TextureStride = (sizeof(float) * 2);
	// TODO: Try aligning this data if we have problems ...

	// We need to allocate 3 vectors for each element in the triangle map
	vertexArray.Verticies = new Vector[(triangles.size() * 3)];
		//(Vector*)memalign (32, (sizeof(Vector) * triangles.size() * 3));
	
	// In order to the normals to be allocated, the pointer must be null.
	// Otherwise this means we're using a repeat set of normal data...
	if(vertexArray.Normals == NULL)
	{
		vertexArray.Normals	 =	new fixedVector[((triangles.size() * 3) * 3)];
		//(Vector*)memalign (32, (sizeof(Vector) * triangles.size() * 3));
		populateNormalData = true;
	}
		
	// We need to allocate 8 floats for each element in the quad map (4 for S and 4 for T)
	vertexArray.TextureCoordsST = new float[(triangles.size() * 6)];
		//(float* )memalign(32, (sizeof(float) * triangles.size() * 6));
	
	ASSERT(vertexArray.Verticies != NULL);
	ASSERT(vertexArray.Normals != NULL);
	ASSERT(vertexArray.TextureCoordsST != NULL);

	// Loop through our map using an iterator and draw the quad objects...
	std::vector<Triangle>::iterator triangleItr;
	triangleItr = triangles.begin();	
	for(u32 idx = 0; idx < triangles.size(); idx++)
	{	
		// Copy all four verticies into the array
		u32 vertexOffset = (idx * 3);
		vertexArray.Verticies[vertexOffset    ] = triangleItr->verticies[0];
		vertexArray.Verticies[vertexOffset + 1] = triangleItr->verticies[1];
		vertexArray.Verticies[vertexOffset + 2] = triangleItr->verticies[2];

		if(populateNormalData == true)
		{

			// Copy all of the normal elements into the array. Since we only have one normal for quad, we just use idx		
			u32 normalOffset   = (idx * 3);
			u32 binormalOffset = (idx * 3) + (vertexArray.VertexArraySize / 3);
			u32 tangentOffset  = (idx * 3) + ((vertexArray.VertexArraySize / 3) * 2);
			
			calculateNormalValues(normalOffset, binormalOffset, tangentOffset, triangleItr->normals[2], vertexArray);
			calculateNormalValues(normalOffset+1, binormalOffset+1, tangentOffset+1, triangleItr->normals[1], vertexArray);
			calculateNormalValues(normalOffset+2, binormalOffset+2, tangentOffset+2, triangleItr->normals[0], vertexArray);						
		}

		// Copy all eight texture coordinates into the array
		u32 texCoordOffset = (idx * 6);
		vertexArray.TextureCoordsST[texCoordOffset 	  ] = triangleItr->texture_S[0];
		vertexArray.TextureCoordsST[texCoordOffset + 1] = triangleItr->texture_T[0];
		vertexArray.TextureCoordsST[texCoordOffset + 2] = triangleItr->texture_S[1];
		vertexArray.TextureCoordsST[texCoordOffset + 3] = triangleItr->texture_T[1];
		vertexArray.TextureCoordsST[texCoordOffset + 4] = triangleItr->texture_S[2];
		vertexArray.TextureCoordsST[texCoordOffset + 5] = triangleItr->texture_T[2];		

		triangleItr++;
	}	
}

///<summary>
/// Deallocates vertex array ...
///</summary>							
void GxGraphics::freeVertexArray(TriangleVertexArray &vertexArray)
{
	//TODO: use the following 3 lines with memalign, otherwise use delete
	delete (vertexArray.Verticies);
	delete (vertexArray.TextureCoordsST);
	delete (vertexArray.Normals);
	//free(vertexArray.Verticies);
	//free(vertexArray.TextureCoordsST);	
	//free(vertexArray.Normals);	
}

///<summary>
/// Deallocates vertex array ...
///</summary>							
void GxGraphics::freeVertexArray(QuadVertexArray &vertexArray)
{
	// TODO: if we're using memalign use the 3 items below, otherwise use delete
	delete vertexArray.Verticies;
	delete vertexArray.TextureCoordsST;
	delete vertexArray.TexCoordLookup;
	delete vertexArray.Normals;
	//free(vertexArray.Verticies);
	//free(vertexArray.TextureCoordsST);	
	//free(vertexArray.TexCoordLookup);
	//free(vertexArray.Normals);

}

///<summary>
/// This routine will render a quad vertex array into a scene just like addQuadsToScene etc.
///</summary>						
void GxGraphics::addVertexArrayToScene(Vector location, TextureHandle textureID, TriangleVertexArray &vertexArray)
{
	// Now is a good time to load the texture object
	ASSERT(myGxTextures.empty() == false);	

	// Before we do anything, perform a frustrum check to make sure the location specified is within our frustrum...

	setupRenderingPass();
	
	setupNRMIndex16VertexDescriptors();
	handleMatrixCalculations(location);

	setupTextureObject(textureID);

	//GX_SetCullMode(GX_CULL_NONE);

	// Let the GPU know about our vertex arrays...	
	GX_SetArray(GX_VA_POS, vertexArray.Verticies, vertexArray.VertexStride);
	GX_SetArray(GX_VA_NRM, vertexArray.Normals, vertexArray.NormalStride);
	GX_SetArray(GX_VA_TEX0, vertexArray.TextureCoordsST, vertexArray.TextureStride);

	// Loop through our map using an iterator and draw the quad objects...
	for(u32 idx = 0; idx < (vertexArray.VertexArraySize); idx++)
	{				
		u32 vertexOffset = (idx * 3);		

		GX_Begin(GX_TRIANGLES, GX_VTXFMT2, 3);

		GX_Position1x16(vertexOffset);
		GX_Normal1x16(vertexOffset);
		GX_TexCoord1x16(vertexOffset);

		GX_Position1x16(vertexOffset + 1);
		GX_Normal1x16(vertexOffset + 1);
		GX_TexCoord1x16(vertexOffset + 1);

		GX_Position1x16(vertexOffset + 2);
		GX_Normal1x16(vertexOffset + 2);
		GX_TexCoord1x16(vertexOffset + 2);

		GX_End();
	}

	endRenderingPass();
}

///<summary>
/// Updates the Mirror texture map by placing a camera at mirrorPosition, and pointing it at mirrorTarget...
/// This should be done
///</summary>
void GxGraphics::updateMirrorReflection(Vector mirrorPosition, Vector mirrorTarget)
{
/*
	 STUFF FROM GX DEMO!
	// Render a scene, and load the image onto TEXMAP3
    f32 currentProjection[7];   // current projection matrix: (myPerspective)
    f32 currentViewport[6];     // current viewport: myView, or myModelView (try both)
    Vector sc[4];      			// screen coords of mirror
    s16 minx, maxx; 			// x bounds of mirror - make sure to set these up ...
    s16 miny, maxy; 			// y bounds of mirror
    u16 width;      			// mirror width
    u16 height;     			// mirror height
    f32 x1, x2, y1, y2; 		// used for texture projection matrix calculation
    Mtx mv;         			// modelview mtx
    Mtx mt;         			// texture proj mtx
    static u16 count = 0;   	// drawsync counter

	// Here we compute the bounding box for a mirrored render

	// Step 1 - read the relevant elements out of the projection matrix

	
	// Step 2 - setup the relevant viewport information

		
	// Set copy source location
    //GXSetTexCopySrc((u16) minx, (u16) miny, width, height);
    //GXSetTexCopyDst((u16) width, (u16) height, GX_TF_I8, GX_FALSE);

    // Wait for texture to be rendered before copying to memory
    // One could also have used GXPixModeSync(), which simply
    // flushes the rasterizer.  Using draw sync allows other commands
    // to be executed in place of the flush (although in this case
    // there are very few).
    //while( count != GXReadDrawSync() ) {};

    // Copy texture
    //GXCopyTex(MirrTexData, GX_TRUE);
    
	// We're basically going to just be copying the entire EFB out and reflecting
	// that before we draw the water right now. We'll figure out a better reflection
	// once we have one that is consistently capturing a single screen space ...
	*/
	
	GX_SetTexCopySrc(0, 0, myRegisterMode->fbWidth, myRegisterMode->efbHeight);
	GX_SetTexCopyDst(myReflectionTexture.width, myReflectionTexture.height, GX_TF_RGBA8, GX_FALSE);    

    // Wait for texture to be rendered before copying to memory
    // One could also have used GXPixModeSync(), which simply
    // flushes the rasterizer.  Using draw sync allows other commands
    // to be executed in place of the flush (although in this case
    // there are very few).
	GX_PixModeSync();

	ASSERT(myReflectionTexHandle != UNDEFINED_TEXTURE_HANDLE);

	// Copy texture
	GX_CopyTex(myReflectionTexture.textureData, GX_FALSE);

/* This stuff kind of matches up to tf-mirror, but doesn't work out right b/c i'm no good at maths :(
// BEFORE WE RENDER THE REFLECTION, WE'RE GOING TO ROCK A PROJECTION MATRIX
	// Here we're going to go ahead and compute the texture projection matrix
	// that we'll use to render the reflection on screen ...
    // First, return min/max's to unit projection space
    float x1 = (0.0 / 5.0f ) - 1.0f;
    float x2 = (10.0f / 5.0f ) - 1.0f;
    float y1 = (0.0f / 5.0f) - 1.0f;
    float y2 = (10.0f / 5.0f) - 1.0f;
    // Adjust such that 0-1 range encompasses viewport used to make texture
	guLightPerspective(myTextureProjection, FOVY, ASPECT, 1.0f/(x2-x1), -1.0f/(y2-y1), -x1/(x2-x1), -y1/(y2-y1));
    // Concatenate the modelview matrix into the projection matrix
	guMtxConcat(myTextureProjection, myModelView, myTextureProjection);    
    GX_LoadTexMtxImm(myTextureProjection, GX_TEXMTX2, GX_MTX3x4);
	
	// NOW WE CAN ROCK THE BUMP MAPPING AND OTHER SETUP STUFFS..
	GX_SetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX3x4, GX_TG_POS, GX_TEXMTX2); // this is for projecting reflections
*/
}

///<summary>
/// Handles setup routines used in normal mapped vertex array rendering
///</summary>						
void GxGraphics::handleBumpMatrixAndRenderingSetup(BUMP_TYPE &bumpType, Vector &location)
{
	// Set up our rendering pass
	if (bumpType == BUMP_TYPE_EMBOSS_MAP)
	{
		handleMatrixCalculations(location);
		setupEmbossedRenderingPass();
	}
	else if (bumpType == BUMP_TYPE_NORMAL_MAP)
	{
		handleMatrixCalculations(location);
		setupNormalMapRenderingPass();
	}
	else if (bumpType == BUMP_TYPE_XYZ_MAP)
	{
		handleMatrixCalculations(location);
		setupXYZMapRenderingPass();
	}
	setupNBTIndex16VertexDescriptors();
}

///<summary>
/// Handles any setup associated with the texture maps used with bumped rendering
///</summary>
void GxGraphics::handleBumpTextureSetup(TextureHandle &textureID, TextureHandle &lightMapID, TextureHandle &normalMapID)
{
	// Load our texture objects which now which hardware texture to map to ...
	setupTextureObject(textureID);
	setupTextureObject(normalMapID);
	setupTextureObject(lightMapID);
	// Following items are currently disabled in engine...
	//setupTextureObject(myLightMapTexHandle);
	//TODO: PUT THIS BACK TO ENABLE REFLECTION MAPPING
	setupTextureObject(myReflectionTexHandle);
}

///<summary>
/// Sets up a bump vertex array for processing in the GPU
///</summary>
static VertexArrayOffsets handleVertexArraySetup(QuadVertexArray &vertexArray)
{
	VertexArrayOffsets returnValue;

	// Let the GPU know about our vertex arrays...
	GX_SetArray(GX_VA_POS, vertexArray.Verticies, vertexArray.VertexStride);
	GX_SetArray(GX_VA_NBT, vertexArray.Normals, vertexArray.NormalStride);
	GX_SetArray(GX_VA_TEX0, vertexArray.TextureCoordsST, vertexArray.TextureStride);

	// Setup our offsets into the vertex array for verticies/normals, binormals, and tangents
	// The normal/binormal/tangent vertex array is split into 3 sections, each the same size as
	// the vertex array. So normals will always match up to the vertex index, binormals 1/3 into
	// the normal array, and then tangents 2/3 into the normal array...
	returnValue.Base   	  = 0;
	returnValue.Texture   	  = 0;
	returnValue.Binormal = vertexArray.VertexArraySize / 3;
	returnValue.Tangent  = (vertexArray.VertexArraySize / 3) * 2;

	return returnValue;
}

///<summary>
/// Sets up a bump vertex array for processing in the GPU
///</summary>
static VertexArrayOffsets handleVertexArraySetup(TriangleVertexArray &vertexArray)
{
	VertexArrayOffsets returnValue;

	// Let the GPU know about our vertex arrays...
	GX_SetArray(GX_VA_POS, vertexArray.Verticies, vertexArray.VertexStride);
	GX_SetArray(GX_VA_NBT, vertexArray.Normals, vertexArray.NormalStride);
	GX_SetArray(GX_VA_TEX0, vertexArray.TextureCoordsST, vertexArray.TextureStride);

	// Setup our offsets into the vertex array for verticies/normals, binormals, and tangents
	// The normal/binormal/tangent vertex array is split into 3 sections, each the same size as
	// the vertex array. So normals will always match up to the vertex index, binormals 1/3 into
	// the normal array, and then tangents 2/3 into the normal array...
	returnValue.Base = 0;
	returnValue.Binormal = vertexArray.VertexArraySize / 3;
	returnValue.Tangent = (vertexArray.VertexArraySize / 3) * 2;

	return returnValue;
}

///<summary>
/// Increases the offset of the array by the specified value
///</summary>
static void incrementArrayOffsetsBy(VertexArrayOffsets &offsets, int numberOfElements)
{
	offsets.Base += numberOfElements;
	offsets.Binormal += numberOfElements;
	offsets.Tangent += numberOfElements;
}

///<summary>
/// Sets up the texture offset used in rendering quad elements...
///</summary>
static void setTextureLookupOffset(VertexArrayOffsets &offsets, int xIndex, int yIndex, QuadVertexArray &vertexArray)
{
	int textureOffset = ((yIndex * vertexArray.properties.quadsWide) + xIndex);
	offsets.Texture = vertexArray.TexCoordLookup[textureOffset];
}

///<summary>
/// This routine will render a triangle vertex array into a scene just like addTrianglessToScene etc.
///</summary>
void GxGraphics::addNormalMappedVertexArrayToScene(BUMP_TYPE bumpType, Vector location, TextureHandle textureID, TextureHandle lightMapID,
		TextureHandle normalMapID, TriangleVertexArray &vertexArray)
{
	ASSERT(myGxTextures.empty() == false);

	handleBumpMatrixAndRenderingSetup(bumpType, location);

	handleBumpTextureSetup(textureID, lightMapID, normalMapID);

	handleVertexArraySetup(vertexArray);

	VertexArrayOffsets offsets = handleVertexArraySetup(vertexArray);

	if(vertexArray.CullTriangleBacks == true)
	{
		GX_SetCullMode(GX_CULL_BACK);
	}

	// Loop through our map using an iterator and draw the quad objects...
	for(u32 idx = 0; idx < (vertexArray.VertexArraySize); idx++)
	{				

		GX_Begin(GX_QUADS, GX_VTXFMT1, 3);

		GX_Position1x16(offsets.Base      );
		GX_Normal1x16(offsets.Base        );
		GX_Normal1x16(offsets.Tangent     );
		GX_Normal1x16(offsets.Binormal    );
		GX_TexCoord1x16(offsets.Base      );


		GX_Position1x16(offsets.Base   + 1);
		GX_Normal1x16(offsets.Base     + 1);
		GX_Normal1x16(offsets.Tangent  + 1);
		GX_Normal1x16(offsets.Binormal + 1);
		GX_TexCoord1x16(offsets.Base   + 1);

		GX_Position1x16(offsets.Base   + 2);
		GX_Normal1x16(offsets.Base     + 2);
		GX_Normal1x16(offsets.Tangent  + 2);
		GX_Normal1x16(offsets.Binormal + 2);
		GX_TexCoord1x16(offsets.Base   + 2);

		GX_End();

		incrementArrayOffsetsBy(offsets, 3);
	}

	GX_SetCullMode(GX_CULL_NONE);

	endRenderingPass();
}


///<summary>
/// Handles the steps for rendering a quad vertex array...
///</summary>
static void renderNormalMappedQuad(VertexArrayOffsets &offsets, int rowOffset)
{
	// Render the quad

	GX_Begin(GX_QUADS, GX_VTXFMT1, 4);

	// The top elements are on the current row
	GX_Position1x16(offsets.Base);
	GX_Normal1x16(offsets.Base);
	GX_Normal1x16(offsets.Tangent);
	GX_Normal1x16(offsets.Binormal);
	GX_TexCoord1x16(offsets.Texture);

	GX_Position1x16(offsets.Base + 1);
	GX_Normal1x16(offsets.Base + 1);
	GX_Normal1x16(offsets.Tangent + 1);
	GX_Normal1x16(offsets.Binormal + 1);
	GX_TexCoord1x16(offsets.Texture + 1);

	// The bottom elements are offset by one rows width from the current elements
	GX_Position1x16(offsets.Base + rowOffset + 1);
	GX_Normal1x16(offsets.Base + rowOffset + 1);
	GX_Normal1x16(offsets.Tangent + rowOffset + 1);
	GX_Normal1x16(offsets.Binormal + rowOffset + 1);
	GX_TexCoord1x16(offsets.Texture + 2);

	GX_Position1x16(offsets.Base  + rowOffset);
	GX_Normal1x16(offsets.Base  + rowOffset);
	GX_Normal1x16(offsets.Tangent  + rowOffset);
	GX_Normal1x16(offsets.Binormal  + rowOffset);
	GX_TexCoord1x16(offsets.Texture + 3);

	GX_End();
}

///<summary>
/// Handles the steps for rendering a quad vertex array...
///</summary>
static void render1by1NormalMappedQuad(VertexArrayOffsets &offsets)
{
	// Render the quad

	GX_Begin(GX_QUADS, GX_VTXFMT1, 4);

	// The top elements are on the current row
	GX_Position1x16(offsets.Base);
	GX_Normal1x16(offsets.Base);
	GX_Normal1x16(offsets.Tangent);
	GX_Normal1x16(offsets.Binormal);
	GX_TexCoord1x16(offsets.Texture);

	GX_Position1x16(offsets.Base + 1);
	GX_Normal1x16(offsets.Base + 1);
	GX_Normal1x16(offsets.Tangent + 1);
	GX_Normal1x16(offsets.Binormal + 1);
	GX_TexCoord1x16(offsets.Texture + 1);

	// The bottom elements are offset by one rows width from the current elements
	GX_Position1x16(offsets.Base + 2);
	GX_Normal1x16(offsets.Base + 2);
	GX_Normal1x16(offsets.Tangent + 2);
	GX_Normal1x16(offsets.Binormal + 2);
	GX_TexCoord1x16(offsets.Texture + 2);

	GX_Position1x16(offsets.Base + 3);
	GX_Normal1x16(offsets.Base + 3);
	GX_Normal1x16(offsets.Tangent + 3);
	GX_Normal1x16(offsets.Binormal + 3);
	GX_TexCoord1x16(offsets.Texture + 3);

	GX_End();
}


///<summary>
/// This routine will render a quad vertex array into a scene just like addQuadsToScene etc.
///</summary>						
void GxGraphics::addNormalMappedVertexArrayToScene(BUMP_TYPE bumpType, Vector location, TextureHandle textureID, TextureHandle lightMapID,
		TextureHandle normalMapID, QuadVertexArray &vertexArray)
{
	ASSERT(myGxTextures.empty() == false);

	ASSERT(vertexArray.properties.quadsWide != 0);

	handleBumpMatrixAndRenderingSetup(bumpType, location);

	handleBumpTextureSetup(textureID, lightMapID, normalMapID);

	VertexArrayOffsets offsets = handleVertexArraySetup(vertexArray);

    if(vertexArray.properties.cullBacks == true)
    {
    	GX_SetCullMode(GX_CULL_BACK);
    }

    // If the vertex array is not 1x1, then only one corner is used per quad. Otherwise, all four corners are used for a quad...
    if ((vertexArray.properties.quadsHigh > 1) || (vertexArray.properties.quadsWide > 1))
	{
		// Loop through our rows and columns and render our quads...
		for (int yIndex = 0; yIndex < (vertexArray.properties.quadsHigh - 1); yIndex++)
		{
			for (int xIndex = 0; xIndex < (vertexArray.properties.quadsWide - 1); xIndex++)
			{
				setTextureLookupOffset(offsets, xIndex, yIndex, vertexArray);
				renderNormalMappedQuad(offsets, vertexArray.properties.quadsWide);
				incrementArrayOffsetsBy(offsets, 1);
			}
			// This increments past the one last quad that we won't use to render with...
			incrementArrayOffsetsBy(offsets, 1);
		}
	}
    // We need to render a 1x1 array, which is stored differently...
	else
	{
		setTextureLookupOffset(offsets, 0, 0, vertexArray);
		render1by1NormalMappedQuad(offsets);
	}

    /* THIS IS HOW WE USED TO RENDER AND CULL QUADS THAT DIDN'T NEED RENDER
       THIS HAS BEEN REMOVED FOR THE PRESENT DUE TO A LACK IN SIGNIFICANT SPEED GAINS...
	// Just go ahead and render any small quad arrays...
    if (vertexArray.VertexArraySize <= 16)
	{
		for (u32 chunkIndex = 0; chunkIndex < vertexArray.VertexArraySize; chunkIndex++)
		{
			renderNormalMappedQuad(offsets);
			incrementArrayOffsetsBy(offsets, 4);
		}
	}
	// Otherwise go through and render all of the quads in chunks (skipping any that are out of range...
	else
	{
		Vector checkLocation;

		// Loop through our map using an iterator and draw the quad objects...
		for (u32 idx = 0; idx < (vertexArray.VertexArraySize); idx += 8)
		{
			checkLocation = location;
			int checkOffset = (idx * 4);
			checkLocation.x += vertexArray.Verticies[checkOffset].x;
			checkLocation.y += vertexArray.Verticies[checkOffset].y;
			checkLocation.z += vertexArray.Verticies[checkOffset].z;

			// If it is in range, then render it...
			// Right now we're turning off frustum culling on the tilemap...
			//SEEN_BY_CAMERA cameraView = isLocationWithinViewingRange(checkLocation);
			//if (cameraView != SEEN_BY_CAMERA_NO)
			{
				for (u32 chunkIndex = 0; chunkIndex < 8; chunkIndex++)
				{
					renderNormalMappedQuad(offsets);
					incrementArrayOffsetsBy(offsets, 4);
				}
			}
			// If it's not in range, then skip it...
			//else
			//{
			//	incrementArrayOffsetsBy(offsets, (4 * 8));
			//}

			// This is a TOTAL-HACK for the GX to deal with overflowing the vertex cache.
			// If we have an opportunity we'll get smarted about how we use the cache, but until then
			// we'll just reset the pointer :)
			if (idx == 11552)
			{
				GX_SetArray(GX_VA_POS, &vertexArray.Verticies[((idx + 8) * 4)], vertexArray.VertexStride);
				GX_SetArray(GX_VA_NBT, &vertexArray.Normals[((idx + 8) * 4)], vertexArray.NormalStride);
				GX_SetArray(GX_VA_TEX0, &vertexArray.TextureCoordsST[((idx + 8) * 8)], vertexArray.TextureStride);
				offsets.Base = 0;
				offsets.Binormal = (vertexArray.VertexArraySize - (idx + 8)) / 3;
				offsets.Tangent  = ((vertexArray.VertexArraySize - (idx + 8)) / 3) * 2;
			}
		}
	}*/

	GX_SetCullMode(GX_CULL_NONE);

	endRenderingPass();
}

///<summary>
/// Adjusts the current radius of the camera by the specified value
///</summary>								
void GxGraphics::adjustCameraRadius(float adjustBy)
{
	//myPostTransMtxOffset += adjustBy;
	//printf("\x1b[2;0H"); printf("Offset: %4.4f", myPostTransMtxOffset);
	myCamera.radius += adjustBy * 0.1;
	//mySpecularPhi += adjustBy;
	//Clamp(mySpecularPhi, -90.0, 90.0);
	//usleep(100000);

}

///<summary>
/// returns the current radius of the camera
///</summary>								
float GxGraphics::getCameraRadius()
{
	return myCamera.radius;
}

///<summary>
/// Adjusts the current radius of the camera by the specified value
///</summary>								
void GxGraphics::adjustCameraDistance(float adjustBy)
{
	//myIndTexMtxFactor += adjustBy;
	//printf("\x1b[2;0H"); printf("FACTOR: %4.4f", myIndTexMtxFactor);
	//myPostTransMtxOffset = 128;
	//myIndTexMtxFactor = 0;
	
	myCamera.distance += adjustBy  * 0.1;

	//mySpecularTheta += adjustBy;	
	//usleep(100000);
	//mySpecularTheta = fmod(mySpecularTheta, 360.0f);	
}

///<summary>
/// returns the current radius of the camera
///</summary>								
float GxGraphics::getCameraDistance()
{

	return myCamera.distance;
}

///<summary>
/// returns a vector with the current camera location
///</summary>								
Vector GxGraphics::getCameraLocation()
{
	return myCamera.pos;
}
