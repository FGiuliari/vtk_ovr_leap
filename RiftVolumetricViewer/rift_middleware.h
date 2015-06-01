#pragma once
#include <OVR.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <CAPI/CAPI_HMDState.h>
#include <Kernel\OVR_Math.h>
#include <OVR_Profile.h>
#include <Service\Service_NetClient.h>
//! Class that is used to initialize the oculus and get information about the head position and texture size
/*!
   this class needs a link to the libovr.lib, ws2_32.lib(for the sockets used on the oculus lib) and winmm.lib (for the timers used on the oculus lib)
*/



using namespace OVR; 
using namespace CAPI;
struct Headpose {
	float yaw;
	float pitch;
	float roll;
};

class rift_middleware
{
public:
	rift_middleware();
	~rift_middleware();
	//!return the resolution of the oculus
	ovrSizei* getResolution();
	ovrSizei* getFovTextureSize();
	Headpose getHeadpose();
	float* getLensCenter();
	HMDState* getHMDState();
	

private:
	ovrTrackingState state;
	ovrHmd g_Hmd;
	ovrGLConfig g_Cfg;
	ovrEyeRenderDesc g_EyeRenderDesc[2];
	ovrVector3f g_EyeOffsets[2];
	ovrPosef g_EyePoses[2];
	ovrTexture g_EyeTextures[2];
	ovrSizei l_EyeTextureSizes[2];
	Headpose head;
	HMDState* HMDstate;

};

