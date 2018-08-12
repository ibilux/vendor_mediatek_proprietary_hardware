LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := meta_keypadbk.c
LOCAL_C_INCLUDES := $(MTK_PATH_SOURCE)/hardware/meta/common/inc
LOCAL_STATIC_LIBRARIES := libft
LOCAL_MODULE := libmeta_keypadbk
LOCAL_PRELINK_MODULE := false
include $(MTK_STATIC_LIBRARY)

############
# Self Test
############
BUILD_SELF_TEST := false

ifeq ($(BUILD_SELF_TEST), true)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := meta_keypadbk_test.c
LOCAL_C_INCLUDES := $(MTK_PATH_SOURCE)/hardware/meta/common/inc
LOCAL_STATIC_LIBRARIES := libft libmeta_keypadbk
LOCAL_MODULE := meta_keypadbk_test
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)
include $(MTK_EXECUTABLE)
endif
