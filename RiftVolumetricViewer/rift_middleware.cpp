#include "rift_middleware.h"


rift_middleware::rift_middleware()
{
	ovr_Initialize();
	g_Hmd = ovrHmd_Create(0);
	if (!g_Hmd)
	{
		printf("No Oculus Rift device attached, using virtual version...\n");
		g_Hmd = ovrHmd_CreateDebug(ovrHmd_DK1);
	}	
	HMDstate=HMDState::CreateHMDState(g_Hmd->Type);

	ovrHmd_ConfigureTracking(g_Hmd, ovrTrackingCap_Orientation, ovrTrackingCap_Orientation);
	l_EyeTextureSizes[ovrEye_Left] = ovrHmd_GetFovTextureSize(g_Hmd, ovrEye_Left, g_Hmd->MaxEyeFov[ovrEye_Left], 1.0f);
	l_EyeTextureSizes[ovrEye_Right] = ovrHmd_GetFovTextureSize(g_Hmd, ovrEye_Right, g_Hmd->MaxEyeFov[ovrEye_Right], 1.0f);
	ovrHmd_ResetFrameTiming(g_Hmd, 0);
}


rift_middleware::~rift_middleware()
{
	ovrHmd_Destroy(g_Hmd);
	ovr_Shutdown();
	
}

ovrSizei* rift_middleware::getResolution(){
	ovrSizei *res = (ovrSizei*)malloc(sizeof(ovrSizei));
	res->h= g_Hmd->Resolution.h;
	res->w= g_Hmd->Resolution.w;
	return res;
}

ovrSizei* rift_middleware::getFovTextureSize(){
	ovrSizei* fov = (ovrSizei*)malloc(sizeof ovrSizei * 2);
	fov[0] = l_EyeTextureSizes[ovrEye_Left];
	fov[1] = l_EyeTextureSizes[ovrEye_Right];
	return fov;
	
}

float* rift_middleware::getLensCenter(){
	// <--------------left eye------------------><-ScreenGapSizeInMeters-><--------------right eye----------------->
	// <------------------------------------------ScreenSizeInMeters.Width----------------------------------------->
	//                            <----------------LensSeparationInMeters--------------->
	// <--centerFromLeftInMeters->
	//                            ^
	//                      Center of lens

	// Find the lens centers in scale of [-1,+1] (NDC) in left eye.

	float* lenses = (float*)malloc(sizeof (float) * 2);
	float visibleWidthOfOneEye = 0.5f * (HMDstate->RenderState.RenderInfo.ScreenSizeInMeters.w - HMDstate->RenderState.RenderInfo.ScreenGapSizeInMeters);
	float centerFromLeftInMeters = (HMDstate->RenderState.RenderInfo.ScreenSizeInMeters.w - HMDstate->RenderState.RenderInfo.LensSeparationInMeters) * 0.5f;
	lenses[0] = centerFromLeftInMeters / visibleWidthOfOneEye; // we consider the single lens 
	lenses[1] = 1 - lenses[0];
	return lenses;
}

HMDState* rift_middleware::getHMDState(){
	return HMDstate;
}

Headpose rift_middleware::getHeadpose(){
	ovrFrameTiming frameTiming = ovrHmd_BeginFrameTiming(g_Hmd, 0);
	ovrTrackingState ss =
		ovrHmd_GetTrackingState(g_Hmd, frameTiming.ScanoutMidpointSeconds);
	if (ss.StatusFlags & (ovrStatus_OrientationTracked)){
		ovrPosef pose = ss.HeadPose.ThePose;
		Quatf orientation = pose.Orientation;
		orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&head.yaw,&head.pitch,&head.roll);
		head.yaw = OVR::RadToDegree(head.yaw);
		head.pitch = OVR::RadToDegree(head.pitch);
		head.roll = OVR::RadToDegree(head.roll);
		ovrHmd_EndFrameTiming(g_Hmd);
		return head;
	}
	ovrHmd_EndFrameTiming(g_Hmd);
	return head;
}