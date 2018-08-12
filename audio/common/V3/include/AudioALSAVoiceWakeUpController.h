#ifndef ANDROID_ALSA_AUDIO_VOICE_WAKE_UP_CONTROLLER_H
#define ANDROID_ALSA_AUDIO_VOICE_WAKE_UP_CONTROLLER_H

#include <tinyalsa/asoundlib.h>

#include "AudioType.h"
#include <AudioLock.h>

#include "AudioALSACaptureHandlerBase.h"
#include "AudioALSACaptureHandlerVOW.h"

#include <CFG_AUDIO_File.h>

namespace android {
//class AudioALSACaptureDataProviderBase;

class AudioALSACaptureDataProviderVOW;

class AudioALSAVoiceWakeUpController {
public:
    virtual ~AudioALSAVoiceWakeUpController();

    static AudioALSAVoiceWakeUpController *getInstance();

    virtual status_t setVoiceWakeUpEnable(const bool enable);
    virtual bool     getVoiceWakeUpEnable();

    virtual status_t updateDeviceInfoForVoiceWakeUp();

    virtual int      SetVOWCustParam(int index, int value);
    virtual bool    getVoiceWakeUpStateFromKernel();



protected:
    AudioALSAVoiceWakeUpController();

    virtual status_t updateParamToKernel();



private:
    /**
     * singleton pattern
     */
    static AudioALSAVoiceWakeUpController *mAudioALSAVoiceWakeUpController;

    AudioALSACaptureDataProviderVOW *mVOWCaptureDataProvider;
    AudioALSACaptureHandlerBase *mCaptureHandler;
    stream_attribute_t *stream_attribute_target;
    status_t setVoiceWakeUpDebugDumpEnable(const bool enable);
    bool mDebug_Enable;

    struct mixer *mMixer; // TODO(Harvey): move it to AudioALSAHardwareResourceManager later

    AudioLock mLock;

    bool mEnable;
    bool mIsUseHeadsetMic;
    AudioALSAHardwareResourceManager *mHardwareResourceManager;
    bool mIsNeedToUpdateParamToKernel;

    VOICE_RECOGNITION_PARAM_STRUCT mVRParam;

    uint32_t mHandsetMicMode;
    uint32_t mHeadsetMicMode;

};

} // end namespace android

#endif // end of ANDROID_ALSA_AUDIO_VOICE_WAKE_UP_CONTROLLER_H
