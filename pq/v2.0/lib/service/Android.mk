ifneq ($(MTK_EMULATOR_SUPPORT), yes)
ifeq (,$(filter $(strip $(MTK_PQ_SUPPORT)), no PQ_OFF))

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    PictureQuality.cpp \
    PQAshmemProxy.cpp \
    PQLightSensor.cpp

LOCAL_C_INCLUDES := \
    $(TOP)/$(MTK_PATH_SOURCE)/hardware/pq/v2.0/include \
    $(TOP)/$(MTK_PATH_PLATFORM)/kernel/drivers/dispsys \
    $(TOP)/$(MTK_PATH_SOURCE)/platform/$(MTK_PLATFORM_DIR)/kernel/drivers/dispsys \
    $(TOP)/system/core/base/include

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libhidltransport \
    libhwbinder \
    liblog \
    libutils \
    libcutils \
    libpq_prot \
    libhidlmemory \
    vendor.mediatek.hardware.pq@2.0_vendor \
    android.hidl.allocator@1.0 \
    android.hidl.memory@1.0

ifeq ($(MTK_BASIC_PACKAGE), yes)
    LOCAL_CFLAGS += -DBASIC_PACKAGE
    LOCAL_CFLAGS += -DDISP_COLOR_OFF
else
    ifeq ($(strip $(MTK_PQ_COLOR_MODE)),MDP)
        LOCAL_CFLAGS += -DDISP_COLOR_OFF
    else ifeq ($(strip $(MTK_PQ_COLOR_MODE)),OFF)
        LOCAL_CFLAGS += -DDISP_COLOR_OFF
    endif

    ifeq ($(strip $(MTK_PQ_COLOR_MODE)),MDP)
        LOCAL_CFLAGS += -DMDP_COLOR_ENABLE
    else ifeq ($(strip $(MTK_PQ_COLOR_MODE)),DISP_MDP)
        LOCAL_CFLAGS += -DMDP_COLOR_ENABLE
    endif

    ifeq ($(MTK_BLULIGHT_DEFENDER_SUPPORT),yes)
        LOCAL_CFLAGS += -DBLULIGHT_DEFENDER_SUPPORT
    endif

    ifeq ($(MTK_CHAMELEON_DISPLAY_SUPPORT),yes)
        LOCAL_CFLAGS += -DCHAMELEON_DISPLAY_SUPPORT
    endif

    ifneq ($(PRODUCT_CHARACTERISTICS),tablet)
        ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6735 mt6735m mt6737m mt6737t mt6753 \
                                                           mt6750 mt6755 mt6757 mt6797 mt6799 \
                                                           mt6763 mt6758 mt6739 mt6771 mt6775))
            ifneq ($(strip $(MTK_PQ_COLOR_MODE)), OFF MDP)
                LOCAL_CFLAGS += -DTRANSITION_SUPPORT
            endif
        endif
    endif

    ifeq ($(MTK_FACTORY_GAMMA_SUPPORT),yes)
        LOCAL_CFLAGS += -DFACTORY_GAMMA_SUPPORT
        LOCAL_SHARED_LIBRARIES += \
            libnvram \
            libcustom_nvram \
            libfile_op
    endif

    #Debug cmd
    ifeq ($(TARGET_BUILD_VARIANT), eng)
        LOCAL_CFLAGS += -DPQ_DEBUG
    else ifeq ($(TARGET_BUILD_VARIANT), userdebug)
        LOCAL_CFLAGS += -DPQ_DEBUG
    endif
endif

ifeq ($(strip $(MTK_GLOBAL_PQ_SUPPORT)),yes)
    LOCAL_CFLAGS += -DMTK_GLOBAL_PQ_SUPPORT
endif

ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6580 mt6570))
    LOCAL_CFLAGS += -DCCORR_OFF
endif

ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6757 mt6763 mt6758 mt6771 mt6775))
    LOCAL_CFLAGS += -DCOLOR_3_0
endif

ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6799))
    LOCAL_CFLAGS += -DSUPPORT_HDR
endif

ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6797 mt6799 mt6763 mt6758))
    LOCAL_CFLAGS += -DSUPPORT_UR
endif

ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6797))
    LOCAL_CFLAGS += -DDYN_SHARP_VERSION=1
    LOCAL_CFLAGS += -DDYN_CONTRAST_VERSION=1
else ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6799 mt6763 mt6758))
    LOCAL_CFLAGS += -DDYN_SHARP_VERSION=1
    LOCAL_CFLAGS += -DDYN_CONTRAST_VERSION=2
else ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6771 mt6775))
    LOCAL_CFLAGS += -DDYN_SHARP_VERSION=2
    LOCAL_CFLAGS += -DDYN_CONTRAST_VERSION=2
else
    LOCAL_CFLAGS += -DDYN_SHARP_VERSION=0
    LOCAL_CFLAGS += -DDYN_CONTRAST_VERSION=0
endif

ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6763 mt6739 mt6771))
    LOCAL_CFLAGS += -DGAMMA_LIGHT
endif

LOCAL_MODULE := vendor.mediatek.hardware.pq@2.0-impl
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_RELATIVE_PATH := hw

include $(MTK_SHARED_LIBRARY)

endif
endif
