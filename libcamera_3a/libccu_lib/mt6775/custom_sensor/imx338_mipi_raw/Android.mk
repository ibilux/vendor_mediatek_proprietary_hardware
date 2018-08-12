#binary precheck in makefile
ifneq ($(MTK_EMULATOR_SUPPORT),yes)

LOCAL_PATH := $(call my-dir)

ifeq ($(wildcard $(MTK_PATH_SOURCE)/hardware/libcamera_3a/libccu),)
CCU_USE_PREBUILD := false
endif

ifeq ($(CCU_SENSOR_FORCE_PREBUILT), true)
CCU_USE_PREBUILD := true
endif

ifeq ($(CCU_USE_PREBUILD), true)


CCU_CUSTOM_SENSOR_NAME := libccu_imx338_mipi_raw

include $(CLEAR_VARS)
LOCAL_MODULE := $(CCU_CUSTOM_SENSOR_NAME).dm
LOCAL_SRC_FILES_32 := $(CCU_CUSTOM_SENSOR_NAME).dm
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
LOCAL_MODULE_CLASS := ETC
LOCAL_MULTILIB := 32
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := $(CCU_CUSTOM_SENSOR_NAME).pm
LOCAL_SRC_FILES_32 := $(CCU_CUSTOM_SENSOR_NAME).pm
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
LOCAL_MODULE_CLASS := ETC
LOCAL_MULTILIB := 32
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
include $(BUILD_PREBUILT)

endif
endif
