#ifndef __BWC_CONFIGURATION_H__
#define __BWC_CONFIGURATION_H__

#include  "bandwidth_control.h"
#include "BWCProfileAdapter.h"
#include "mt_smi.h"

#define BWCPT_PROFILE(x) x, #x

#ifdef  BWC_J
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_D1)||defined(BWC_D3)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE,(char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_VSS"),
#ifdef BWC_D3
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
#else
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VSS,(char*)"CON_SCE_VSS"),
#endif
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL)
};
#elif defined(BWC_D2) || defined(BWC_D2_P) || defined(BWC_D2_M)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE,(char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_ICFP"),
#if defined(BWC_D2)
	BWCProfile(BWCPT_CAMERA_ICFP,"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,"CON_SCE_ICFP"),
#elif defined(BWC_D2_P)
	BWCProfile(BWCPT_CAMERA_ICFP,"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,"CON_SCE_ICFS_35P"),
#else
	BWCProfile(BWCPT_CAMERA_ICFP,"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,"CON_SCE_ICFS_35M"),
#endif
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_VR,(char*)"CON_SCE_ICFP"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_ICFP"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL)
};
#elif defined(BWC_EV)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VP4K"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_VR,(char*)"CON_SCE_ICFP"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_ICFP"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_VP4K"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"CON_SCE_VP4K"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"CON_SCE_VP4K"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_SMVR120"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_ICFP"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_RO)
BWCProfile profileSetting[] = {
	BWCProfile(BWCPT_PROFILE(BWCPT_NONE), SMI_BWC_SCEN_NORMAL, "CON_SCE_NORMAL"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_NORMAL), SMI_BWC_SCEN_VR, "CON_SCE_NORMAL"),
	BWCProfile(BWCPT_PROFILE(BWCPT_CAMERA_PREVIEW), SMI_BWC_SCEN_VR, "CON_SCE_VR_H264"),
	BWCProfile(BWCPT_PROFILE(BWCPT_CAMERA_ZSD), SMI_BWC_SCEN_VR, "CON_SCE_ZSD"),
	BWCProfile(BWCPT_PROFILE(BWCPT_CAMERA_CAPTURE), SMI_BWC_SCEN_VR, "CON_SCE_IC"),
	BWCProfile(BWCPT_PROFILE(BWCPT_CAMERA_ICFP), SMI_BWC_SCEN_ICFP, "CON_SCE_IC"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_SWDEC_PLAYBACK), SMI_BWC_SCEN_SWDEC_VP, "CON_SCE_VP"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_PLAYBACK), SMI_BWC_SCEN_VP, "CON_SCE_VP"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_TELEPHONY), SMI_BWC_SCEN_VR, "CON_SCE_VC"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_RECORD), SMI_BWC_SCEN_VENC, "CON_SCE_VR_H264"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_RECORD_CAMERA), SMI_BWC_SCEN_VR, "CON_SCE_VR_H264"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_RECORD_SLOWMOTION), SMI_BWC_SCEN_VR_SLOW, "CON_SCE_VR_H264"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_SNAPSHOT), SMI_BWC_SCEN_VR, "CON_SCE_VR_H264"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_LIVE_PHOTO), SMI_BWC_SCEN_MM_GPU, "CON_SCE_NORMAL"),
	BWCProfile(BWCPT_PROFILE(BWCPT_VIDEO_WIFI_DISPLAY), SMI_BWC_SCEN_WFD, "CON_SCE_NORMAL"),
	BWCProfile(BWCPT_PROFILE(BWCPT_FORCE_MMDVFS), SMI_BWC_SCEN_FORCE_MMDVFS, NULL),
};
#elif defined(BWC_R)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_UI_6L"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_UI_6L"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_UI_6L"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VSS"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_UI_6L"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_UI_6L"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_OLY)
/* Olympus has 3 scenarios: VSS/VPWFD/UI */
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_WHI)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_CAM_PV,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_CAM_CP,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VS"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_VP"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_ALA)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_CAM_PV,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_CAM_CP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_BIA)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_CAM_PV,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_CAM_CP,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_ICFP"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_VIN)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_CAM_PV,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_CAM_CP,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_ICFP"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_VR4K"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_VPWFD"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_ZIO)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_SW_PDE"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_SW_PDE"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"CON_SCE_SW_PDE"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_SW_PDE"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"CON_SCE_SW_PDE"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_ICFP,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"CON_SCE_UI"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_CAN)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"BWL_SCN_UI"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"BWL_SCN_VRAPP"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_CAM_PV,(char*)"BWL_SCN_ICFP"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_CAM_PV,(char*)"BWL_SCN_ICFP"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_CAM_CP,(char*)"BWL_SCN_ICFP"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"BWL_SCN_ICFP"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"BWL_SCN_VRAPP"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"BWL_SCN_VRAPP"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"BWL_SCN_VRAPP"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"BWL_SCN_VRAPP"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VSS,(char*)"BWL_SCN_VRAPP"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"BWL_SCN_UI"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#elif defined(BWC_SYL)
BWCProfile profileSetting[]={
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"BWL_SCN_UI"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"BWL_SCN_VR4K"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_CAM_PV,(char*)"BWL_SCN_ICFP"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_CAM_PV,(char*)"BWL_SCN_ICFP"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_CAM_CP,(char*)"BWL_SCN_ICFP"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_ICFP,(char*)"BWL_SCN_ICFP"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_SWDEC_VP,(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP_HIGH_FPS,
			(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_RESOLUTION",
			SMI_BWC_SCEN_VP_HIGH_RESOLUTION,(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"BWL_SCN_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VENC,(char*)"BWL_SCN_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"BWL_SCN_VR4K"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR_SLOW,(char*)"BWL_SCN_VR4K"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VSS,(char*)"BWL_SCN_VR4K"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_MM_GPU,(char*)"BWL_SCN_UI"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_WFD,(char*)"BWL_SCN_VPWFD"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_FORCE_MMDVFS,NULL),
};
#else
BWCProfile profileSetting[] = {
	BWCProfile(BWCPT_NONE, (char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL")
};
#endif

BWCProfile defaultProfileSetting[]={
	BWCProfile(BWCPT_NONE,(char*)"BWCPT_NONE",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_NORMAL,(char*)"BWCPT_VIDEO_NORMAL",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_PREVIEW,(char*)"BWCPT_CAMERA_PREVIEW",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_ZSD,(char*)"BWCPT_CAMERA_ZSD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_CAPTURE,(char*)"BWCPT_CAMERA_CAPTURE",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_CAMERA_ICFP,(char*)"BWCPT_CAMERA_ICFP",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_SWDEC_PLAYBACK,(char*)"BWCPT_VIDEO_SWDEC_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK_HIGH_FPS,(char*)"BWCPT_VIDEO_PLAYBACK_HIGH_FPS",SMI_BWC_SCEN_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_PLAYBACK,(char*)"BWCPT_VIDEO_PLAYBACK",SMI_BWC_SCEN_VP,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_TELEPHONY,(char*)"BWCPT_VIDEO_TELEPHONY",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_RECORD,(char*)"BWCPT_VIDEO_RECORD",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_RECORD_CAMERA,(char*)"BWCPT_VIDEO_RECORD_CAMERA",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_RECORD_SLOWMOTION,(char*)"BWCPT_VIDEO_RECORD_SLOWMOTION",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_SNAPSHOT,(char*)"BWCPT_VIDEO_SNAPSHOT",SMI_BWC_SCEN_VR,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_LIVE_PHOTO,(char*)"BWCPT_VIDEO_LIVE_PHOTO",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_VIDEO_WIFI_DISPLAY,(char*)"BWCPT_VIDEO_WIFI_DISPLAY",SMI_BWC_SCEN_NORMAL,(char*)"CON_SCE_NORMAL"),
	BWCProfile(BWCPT_FORCE_MMDVFS,(char*)"BWCPT_FORCE_MMDVFS",SMI_BWC_SCEN_NORMAL,NULL)
};
const int total_default_bwc_profiles = sizeof(defaultProfileSetting) / sizeof(BWCProfile);
const int total_bwc_profiles = sizeof(profileSetting) / sizeof(BWCProfile);
#endif
