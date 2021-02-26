TARGET_LIST := msm8998 sdm845 sdm710


ifeq ($(call is-board-platform-in-list,$(TARGET_LIST)),true)

# Firmware hardware addresses
# If these differ across devices, define them accordingly.
DPS_ADDR := 0x01E5E000
HPS_ADDR := 0x01E5E080
GSI_ADDR := 0x01E08000
GSI_MEM_SIZE := 0x4000

# msm8998 IPA FWs configs (IPAv3.1)
ifeq ($(call is-board-platform-in-list,msm8998),true)
$(info IPA FW creation for msm8998)
GSI_FW_FILE := fw_mhi_ipa_v3.bin

SOC_HW_VERSION := 0x30020000
# For 8997 HW version
SOC_VERS := 0x3005
endif #msm8998 check

# sdm845 IPA FWs configs (IPAv3.5.1)
ifeq ($(call is-board-platform-in-list,sdm845),true)
$(info IPA FW creation for sdm845)
GSI_FW_FILE := fw_mhi_ipa_v3.5.bin

SOC_HW_VERSION := 0x60000000
SOC_VERS := 0x6001
endif #sdm845 check

# sdm710 IPA FWs configs (IPAv3.5.1)
ifeq ($(call is-board-platform-in-list,sdm710),true)
$(info IPA FW creation for sdm710)
GSI_FW_FILE := fw_mhi_ipa_v3.5.bin

SOC_HW_VERSION := 0x60040000
SOC_VERS := "0x6005 0x6009 0x600A"
endif #sdm710 check

LOCAL_PATH := $(call my-dir)
SECIMAGE_BASE := vendor/qcom/proprietary/common/scripts/SecImage
SECIMAGE_CONFIG_FILE := config/integration/secimagev2.xml
PILSPLITTER_BASE := vendor/qcom/proprietary/common/scripts

# Check GSI FW file existence
ifeq ("$(wildcard $(LOCAL_PATH)/$(GSI_FW_FILE))","")
$(info $(LOCAL_PATH)/$(GSI_FW_FILE) does not exist!)
else

BINS := $(LOCAL_PATH)/$(GSI_FW_FILE) $(LOCAL_PATH)/hps.bin $(LOCAL_PATH)/dps.bin
TOOLS := $(LOCAL_PATH)/elf_creator.py $(SECIMAGE_BASE)/sectools_builder.py $(PILSPLITTER_BASE)/pil-splitter.py
$(LOCAL_PATH)/ipa_fws.elf: PRIVATE_PATH := $(LOCAL_PATH)
$(LOCAL_PATH)/ipa_fws.elf: $(BINS) $(TOOLS)
	# Assemble binaries/firmwares to a single ELF file
	python $(PRIVATE_PATH)/elf_creator.py \
		--gsi_fw $(PRIVATE_PATH)/$(GSI_FW_FILE) \
		--gsi_fw_address $(GSI_ADDR) \
		--gsi_fw_mem_size $(GSI_MEM_SIZE) \
		--hps $(PRIVATE_PATH)/hps.bin \
		--hps_addr $(HPS_ADDR) \
		--dps $(PRIVATE_PATH)/dps.bin \
		--dps_addr $(DPS_ADDR) \
		--outfile $(PRIVATE_PATH)/ipa_fws.elf
	echo Creating ipa ELF image OUT folders
	mkdir -p $(PRODUCT_OUT)/ipa
	mkdir -p $(PRODUCT_OUT)/ipa/signed
	mkdir -p $(PRODUCT_OUT)/ipa/unsigned
	echo install unsigned ipa_fws.elf at $(PRODUCT_OUT)/ipa/unsigned
	cp -f $(PRIVATE_PATH)/ipa_fws.elf $(PRODUCT_OUT)/ipa/unsigned
	# Sign the ELF file using SecImage tool
	SECIMAGE_LOCAL_DIR=$(SECIMAGE_BASE) USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN=$(USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN) \
	python $(SECIMAGE_BASE)/sectools_builder.py \
		-i $(PRIVATE_PATH)/ipa_fws.elf \
		-g ipa_fw \
		-t $(PRIVATE_PATH) \
		--install_base_dir=$(PRIVATE_PATH) \
		--soc_hw_version=$(SOC_HW_VERSION) \
		--soc_vers=$(SOC_VERS) \
		--config $(SECIMAGE_BASE)/$(SECIMAGE_CONFIG_FILE) \
		> $(PRIVATE_PATH)/secimage.log 2>&1
	echo install signed ipa_fws.elf at $(PRODUCT_OUT)/ipa/signed
	cp -f $(PRIVATE_PATH)/ipa_fws.elf $(PRODUCT_OUT)/ipa/signed
	# PIL split the output of the SecImage tool
	python $(PILSPLITTER_BASE)/pil-splitter.py \
		$(PRIVATE_PATH)/ipa_fws.elf \
		$(PRIVATE_PATH)/ipa_fws
	# Remove the SecImage tool intermediate output
	rm -rf $(PRIVATE_PATH)/signed $(PRIVATE_PATH)/sign \
		$(PRIVATE_PATH)/signed_encrypted $(PRIVATE_PATH)/sign_and_encrypt

include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.elf
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

$(LOCAL_PATH)/ipa_fws.b00: ipa_fws.elf
include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b00
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

$(LOCAL_PATH)/ipa_fws.b01: ipa_fws.elf
include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b01
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

$(LOCAL_PATH)/ipa_fws.b02: ipa_fws.elf
include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b02
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# Only if the policy is defined, these files will be created
ifeq ($(USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN),1)
$(LOCAL_PATH)/ipa_fws.b03: ipa_fws.elf
include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b03
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

$(LOCAL_PATH)/ipa_fws.b04: ipa_fws.elf
include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b04
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif # Signing policy check

$(LOCAL_PATH)/ipa_fws.mdt: ipa_fws.elf
include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.mdt
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

endif # GSI FW file existence check
endif # Target check
