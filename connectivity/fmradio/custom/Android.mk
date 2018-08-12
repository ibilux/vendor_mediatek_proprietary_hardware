ifeq ($(MTK_FM_SUPPORT), yes)
LOCAL_PATH := $(call my-dir)
###############################################################################
# Define MTK FM Radio Chip solution
###############################################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	custom.cpp
	
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)

#LOCAL_CFLAGS:= \
#    -D$(MTK_FM_CHIP)

ifeq ($(findstring MT6627_FM,$(MTK_FM_CHIP)),MT6627_FM)
LOCAL_CFLAGS+= \
    -DMT6627_FM
endif
ifeq ($(findstring MT6625_FM,$(MTK_FM_CHIP)),MT6625_FM)
LOCAL_CFLAGS+= \
    -DMT6627_FM
endif
ifeq ($(findstring MT6580_FM,$(MTK_FM_CHIP)),MT6580_FM)
LOCAL_CFLAGS+= \
    -DMT6580_FM
endif
ifeq ($(findstring MT0633_FM,$(MTK_FM_CHIP)),MT0633_FM)
LOCAL_CFLAGS+= \
    -DMT0633_FM
endif
ifeq ($(findstring MT6630_FM,$(MTK_FM_CHIP)),MT6630_FM)
LOCAL_CFLAGS+= \
    -DMT6630_FM
endif
ifeq ($(findstring MT6631_FM,$(MTK_FM_CHIP)),MT6631_FM)
LOCAL_CFLAGS+= \
    -DMT6631_FM
endif
ifeq ($(findstring MT6632_FM,$(MTK_FM_CHIP)),MT6632_FM)
LOCAL_CFLAGS+= \
    -DMT6632_FM
endif
LOCAL_SHARED_LIBRARIES := libcutils liblog
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmcust
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
include $(MTK_SHARED_LIBRARY)

endif
