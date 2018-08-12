#binary precheck in makefile
ifneq ($(MTK_EMULATOR_SUPPORT),yes)

LOCAL_PATH := $(call my-dir)

ifeq ($(wildcard $(MTK_PATH_SOURCE)/hardware/libcamera_3a/libccu),)

include $(CLEAR_VARS)
LOCAL_MODULE := libccu_bin.dm
LOCAL_SRC_FILES_32 := libccu_bin.dm
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
LOCAL_MODULE_CLASS := ETC
LOCAL_MULTILIB := 32
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libccu_bin.pm
LOCAL_SRC_FILES_32 := libccu_bin.pm
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
LOCAL_MODULE_CLASS := ETC
LOCAL_MULTILIB := 32
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
include $(BUILD_PREBUILT)

endif
endif
