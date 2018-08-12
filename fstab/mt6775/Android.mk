LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
FSTAB_OUT := $(PRODUCT_OUT)/obj/FAKE
LOCAL_PREBUILT_MODULE_FILE := $(FSTAB_OUT)/fstab.mt6775

LOCAL_MODULE := fstab.mt6775
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_SRC_FILES := fstab.mt6775
LOCAL_MODULE_CLASS := FAKE
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_VENDOR)/etc

ifeq ($(strip $(MTK_PERSIST_PARTITION_SUPPORT)),yes)
__CFLAGS += -D__PERSIST_PARTITION_SUPPORT
endif
ifeq ($(strip $(MTK_CIP_SUPPORT)),yes)
__CFLAGS += -D__CIP_SUPPORT
endif
ifeq ($(strip $(MTK_ICUSB_SUPPORT)),yes)
__CFLAGS += -D__ICUSB_SUPPORT
endif
ifeq ($(strip $(MTK_MULTI_PARTITION_MOUNT_ONLY_SUPPORT)),yes)
__CFLAGS += -D__MULTI_PARTITION_MOUNT_ONLY_SUPPORT
endif
ifeq ($(strip $(MTK_FACTORY_RESET_PROTECTION_SUPPORT)),yes)
__CFLAGS += -D__MTK_FACTORY_RESET_PROTECTION_SUPPORT
endif
ifeq ($(strip $(MTK_ATF_SUPPORT)),yes)
__CFLAGS += -D__MTK_ATF_SUPPORT
endif
ifeq ($(strip $(MTK_TEE_SUPPORT)),yes)
__CFLAGS += -D__MTK_TEE_SUPPORT
endif
ifeq ($(strip $(MTK_EMMC_SUPPORT)),yes)
__CFLAGS += -D__MTK_EMMC_SUPPORT
endif
ifeq ($(strip $(MTK_UFS_BOOTING)),yes)
__CFLAGS += -D__MTK_UFS_BOOTING
endif
ifeq ($(strip $(MTK_EMMC_SUPPORT_OTP)),yes)
__CFLAGS += -D__MTK_OTP_SUPPORT
endif
ifeq ($(strip $(MTK_UFS_OTP)),yes)
__CFLAGS += -D__MTK_OTP_SUPPORT
endif
ifeq ($(strip $(MTK_TINYSYS_SCP_SUPPORT)),yes)
__CFLAGS += -D__MTK_TINYSYS_SCP_SUPPORT
endif
ifeq ($(strip $(MTK_TINYSYS_SSPM_SUPPORT)),yes)
__CFLAGS += -D__MTK_TINYSYS_SSPM_SUPPORT
endif
ifeq ($(strip $(MTK_EFUSE_WRITER_SUPPORT)),yes)
__CFLAGS += -D__MTK_EFUSE_WRITER_SUPPORT
endif
ifeq (true,$(PRODUCTS.$(INTERNAL_PRODUCT).PRODUCT_SUPPORTS_VERITY))
__CFLAGS += -D__MTK_SEC_VERITY
endif
ifneq ($(strip $(BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE)),)
__CFLAGS += -D__MTK_SYSIMG_FSTYPE=$(BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE)
endif

ifeq ($(strip $(MTK_ENCRYPTION_DEFAULT_OFF)),yes)
  __CFLAGS += -D__MTK_FDE_NO_FORCE
else ifeq ($(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)),0x20000000)
  __CFLAGS += -D__MTK_FDE_NO_FORCE
endif
ifeq ($(strip $(MTK_TEE_SUPPORT)),yes)
ifeq ($(strip $(MTK_ENCRYPTION_TYPE_FILE)),yes)
  __CFLAGS += -D__MTK_FDE_TYPE_FILE
ifeq ($(strip $(MTK_ENCRYPTION_FDE_TO_FBE)),yes)
  __CFLAGS += -D__MTK_FDE_TO_FBE
endif
endif
endif
ifeq ($(strip $(MTK_VPU_SUPPORT)),yes)
  __CFLAGS += -D__MTK_VPU_SUPPORT
endif
ifeq ($(strip $(SPM_FW_USE_PARTITION)),yes)
  __CFLAGS += -D__SPM_FW_USE_PARTITION
endif
ifeq ($(strip $(MCUPM_FW_USE_PARTITION)),yes)
  __CFLAGS += -D__MCUPM_FW_USE_PARTITION
endif

ifeq ($(strip $(TARGET_COPY_OUT_VENDOR)),vendor)
  __CFLAGS += -D__VENDOR_PARTITION_SUPPORT
endif

ifeq ($(strip $(MTK_DTBO_FEATURE)),yes)
  __CFLAGS += -D__MTK_DTBO_FEATURE
endif
ifeq ($(strip $(MTK_AB_OTA_UPDATER)),yes)
  __CFLAGS += -D__MTK_AB_OTA_UPDATER
endif

ifeq ($(strip $(TARGET_COPY_OUT_ODM)),yes)
  __CFLAGS += -D__ODM_PARTITION_SUPPORT
endif
ifeq ($(TARGET_BUILD_VARIANT),user)
  __CFLAGS += -D__TARGET_ENABLE_QUOTA
endif

$(LOCAL_PREBUILT_MODULE_FILE): $(LOCAL_PATH)/fstab.in $(MTK_TARGET_PROJECT_FOLDER)/ProjectConfig.mk
	@echo "Generate fstab"
	@echo "MTK_PERSIST_PARTITION_SUPPORT $(MTK_PERSIST_PARTITION_SUPPORT)"
	@echo "MTK_CIP_SUPPORT $(MTK_CIP_SUPPORT)"
	@echo "MTK_ICUSB_SUPPORT $(MTK_ICUSB_SUPPORT)"
	@echo "MTK_MULTI_PARTITION_MOUNT_ONLY_SUPPORT $(MTK_MULTI_PARTITION_MOUNT_ONLY_SUPPORT)"
	@echo "MTK_FACTORY_RESET_PROTECTION_SUPPORT $(MTK_FACTORY_RESET_PROTECTION_SUPPORT)"
	@echo "MTK_ATF_SUPPORT $(MTK_ATF_SUPPORT)"
	@echo "MTK_TEE_SUPPORT $(MTK_TEE_SUPPORT)"
	@echo "MTK_EMMC_SUPPORT $(MTK_EMMC_SUPPORT)"
	@echo "MTK_UFS_BOOTING $(MTK_UFS_BOOTING)"
	@echo "MTK_TINYSYS_SCP_SUPPORT $(MTK_TINYSYS_SCP_SUPPORT)"
	@echo "MTK_EFUSE_WRITER_SUPPORT $(MTK_EFUSE_WRITER_SUPPORT)"
	@echo "PRODUCT_SUPPORTS_VERITY $(PRODUCTS.$(INTERNAL_PRODUCT).PRODUCT_SUPPORTS_VERITY)"
	@echo "BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE $(BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE)"
	@echo "MTK_FDE_NO_FORCE $(MTK_FDE_NO_FORCE)"
	@echo "MTK_FDE_TYPE_FILE $(MTK_FDE_TYPE_FILE)"
	@echo "CUSTOM_CONFIG_MAX_DRAM_SIZE $(CUSTOM_CONFIG_MAX_DRAM_SIZE)"
	@echo "MTK_TINYSYS_SSPM_SUPPORT $(MTK_TINYSYS_SSPM_SUPPORT)"
	@echo "MTK_VPU_SUPPORT $(MTK_VPU_SUPPORT)"
	@echo "SPM_FW_USE_PARTITION $(SPM_FW_USE_PARTITION)"
	@echo "MCUPM_FW_USE_PARTITION $(MCUPM_FW_USE_PARTITION)"
	@echo "TARGET_COPY_OUT_VENDOR $(TARGET_COPY_OUT_VENDOR)"
	@echo "MTK_DTBO_FEATURE $(MTK_DTBO_FEATURE)"
	@echo "MTK_AB_OTA_UPDATER $(MTK_AB_OTA_UPDATER)"
	@echo "TARGET_COPY_OUT_ODM $(TARGET_COPY_OUT_ODM)"
	@mkdir -p $(FSTAB_OUT)
	@gcc $(__CFLAGS) -x c -E $< -o $@

include $(BUILD_PREBUILT)


