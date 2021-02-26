## BoardConfigVendor.mk
## Qualcomm Technologies proprietary product specific compile-time definitions.
#
possible_cvv_dirs = $(QCPATH)/prebuilt_HY11 $(QCPATH)/prebuilt_HY22 $(QCPATH)/prebuilt_grease
prebuilt_cvv_dirs =  $(wildcard $(possible_cvv_dirs))
ifneq ($(prebuilt_cvv_dirs),)
ifneq ($(TARGET_BOARD_SUFFIX),)
$(shell for i in $(prebuilt_cvv_dirs);do echo -e "optional_subdirs = [\n    \"target/product/$(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX)\",\n]" > $$i/Android.bp;done)
else
$(shell for i in $(prebuilt_cvv_dirs);do echo -e "optional_subdirs = [\n    \"target/product/$(TARGET_BOARD_PLATFORM)\",\n]" > $$i/Android.bp;done;)
endif
endif

USE_CAMERA_STUB := false
DEVICE_PACKAGE_OVERLAYS += vendor/qcom/proprietary/common/msm8996/overlay

ifeq ($(TARGET_BOARD_AUTO),true)
# Still is not used for automotive
DISABLE_CAMERA_STILL := true
# AIS is used for automotive
USE_IMAGING_AIS := true
# Since in AutoPLs, AIS build is true
USE_CAMERA_STUB := true
endif

#
BOARD_USES_QTI_HARDWARE := true
BOARD_USES_ADRENO := true
HAVE_ADRENO_SOURCE := true
HAVE_ADRENO_SC_SOURCE := true
HAVE_ADRENO_FIRMWARE := true
TARGET_USES_ION := true
USE_OPENGL_RENDERER := true
TARGET_USES_C2D_COMPOSITION := true
BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := default

ifeq ($(TARGET_USES_AOSP),true)
  BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET := false
  TARGET_FASTCV_DISABLED := true
  TARGET_ENABLE_PROPRIETARY_SMARTCARD_SERVICE := false
  TARGET_USES_QPAY := false
else
  BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET := true
  TARGET_USES_QPAY := true
endif

#Enable IMS
TARGET_USES_IMS := true
BOARD_USES_QCNE := true
#Enable DPM
BOARD_USES_DPM := true
BOARD_USES_LIBC_WRAPPER := true
#Enable/Disable SCVE
TARGET_SCVE_DISABLED := true
USESECIMAGETOOL := true
## Gensecimage generation of signed apps bootloader
QTI_GENSECIMAGE_MSM_IDS := msm8996
## Use signed image as default
QTI_GENSECIMAGE_SIGNED_DEFAULT := msm8996
#
##SECIMAGE tool feature flags
USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN := 1
USES_SEC_POLICY_INTEGRITY_CHECK := 1

##ENERGY_AWARENESS := true

#Enable uibeautification
TARGET_TS_MAKEUP := true

# Enables Adreno RS driver
OVERRIDE_RS_DRIVER := libRSDriver_adreno.so

BOARD_HAS_QCOM_WLAN := true
#CONFIG_EAP_PROXY_DUAL_SIM := true  disabled during bring-up
#CONFIG_EAP_PROXY_AKA_PRIME := true  disabled during bring-up
BOARD_HAS_QCOM_WIGIG := true
ifneq ($(wildcard vendor/qcom/proprietary/mdm-helper/libmdmdetect),)
  CONFIG_EAP_PROXY_MDM_DETECT := true
endif
CONFIG_EAP_PROXY := qmi
CONFIG_EAP_PROXY_DUAL_SIM := true
CONFIG_EAP_PROXY_AKA_PRIME := true
#BOARD_HAS_ATH_WLAN_AR6004 := true
BOARD_HAS_ATH_WLAN_AR6320 := true
BOARD_HAVE_BLUETOOTH := true
#BOARD_HAS_QCA_BT_AR3002 := true
BOARD_HAVE_QCOM_FM := true
BOARD_ANT_WIRELESS_DEVICE := "qualcomm-hidl"
BOARD_HAS_QCA_BT_ROME := true
QCOM_BT_USE_SIBS := false
WCNSS_FILTER_USES_SIBS := true

ifeq ($(BOARD_HAVE_BLUETOOTH), true)
  BOARD_HAVE_BLUETOOTH_BLUEZ := false
  ifneq ($(BOARD_HAVE_BLUETOOTH_BLUEZ), true)
    BOARD_HAVE_BLUETOOTH_QCOM := true
    QCOM_BT_USE_SMD_TTY := true
    BOARD_USES_WIPOWER := false
  endif # BOARD_HAVE_BLUETOOTH_BLUEZ
endif # BOARD_HAVE_BLUETOOTH

ifeq ($(findstring true,$(BOARD_HAS_ATH_WLAN_AR6004) $(BOARD_HAS_QCOM_WLAN)),true)
  BOARD_WLAN_DEVICE := qcwcn
  BOARD_WPA_SUPPLICANT_DRIVER := NL80211
  BOARD_HOSTAPD_DRIVER := NL80211
  ifneq ($(TARGET_BOARD_AUTO),true)
    ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
      WIFI_DRIVER_MODULE_PATH := "/system/vendor/lib/modules/wlan.ko"
    else
      WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/wlan.ko"
    endif
    WIFI_DRIVER_MODULE_NAME := "wlan"
    WIFI_DRIVER_MODULE_ARG := ""
  endif
  WIFI_DRIVER_BUILT := qca_cld3
  WIFI_DRIVER_DEFAULT := qca_cld3
  WIFI_DRIVER_INSTALL_TO_KERNEL_OUT := true
  WIFI_FST_DRIVER_MODULE_PATH := "/system/lib/modules/wil6210.ko"
  WIFI_FST_DRIVER_MODULE_NAME := "wil6210"
  WIFI_FST_DRIVER_MODULE_ARG := ""
  WPA_SUPPLICANT_VERSION := VER_0_8_X
  HOSTAPD_VERSION := VER_0_8_X
  CONFIG_ACS := true
  CONFIG_IEEE80211AC := true
  # JB MR1 or later
  BOARD_HAS_CFG80211_KERNEL3_4 := true
  BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
  BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
endif

FEATURE_QCRIL_UIM_SAP_SERVER_MODE := true


# TODO: Bring-up exception - all modules must be fixed by corresponding teams
INTERNAL_LOCAL_CLANG_EXCEPTION_PROJECTS += \
  $(abspath $(TOPDIR)vendor/qcom/proprietary/gles/adreno200) \
  vendor/qcom/proprietary/gles/adreno200 \
  vendor/qcom/proprietary/fastcv-noship \
  vendor/qcom/proprietary/gps-noship \
  vendor/qcom/proprietary/kernel-tests \
  vendor/qcom/proprietary/mare-noship \
  vendor/qcom/proprietary/mm-camera-lib \
  vendor/qcom/proprietary/mm-camera/mm-camera2/media-controller/modules/imglib \
  vendor/qcom/proprietary/mm-camera/mm-camera2/tests \
  vendor/qcom/proprietary/mm-mux

# TODO: Bring-up exception - all modules must be fixed by corresponding teams
TARGET_CINCLUDES_EXCEPTION_PROJECTS := \
  $(abspath $(TOPDIR)vendor/qcom/proprietary/gles/adreno200) \
  vendor/qcom/proprietary/gles/adreno200 \
  vendor/qcom/proprietary/gps-noship-external \
  vendor/qcom/proprietary/gps-noship \
  disregard/filesystems/mtd-utils
