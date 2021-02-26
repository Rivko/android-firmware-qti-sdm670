# This file lists all qcom products and defines the QC_PROP flag which
# is used to enable projects inside $(QC_PROP_ROOT) directory.

# Also, This file intended for use by device/product makefiles
# to pick and choose the optional proprietary modules

# Root of Qualcomm Proprietary component tree
QC_PROP_ROOT := vendor/qcom/proprietary
SDCLANG_LTO_DEFS := device/qcom/common/sdllvm-lto-defs.mk

PRODUCT_LIST := msm7625_surf
PRODUCT_LIST += msm7625_ffa
PRODUCT_LIST += msm7627_6x
PRODUCT_LIST += msm7627_ffa
PRODUCT_LIST += msm7627_surf
PRODUCT_LIST += msm7627a
PRODUCT_LIST += msm8625
PRODUCT_LIST += msm7630_surf
PRODUCT_LIST += msm7630_1x
PRODUCT_LIST += msm7630_fusion
PRODUCT_LIST += msm8660_surf
PRODUCT_LIST += qsd8250_surf
PRODUCT_LIST += qsd8250_ffa
PRODUCT_LIST += qsd8650a_st1x
PRODUCT_LIST += msm8660_csfb
PRODUCT_LIST += msm8960
PRODUCT_LIST += msm8974
PRODUCT_LIST += mpq8064
PRODUCT_LIST += msm8610
PRODUCT_LIST += msm8226
PRODUCT_LIST += apq8084
PRODUCT_LIST += mpq8092
PRODUCT_LIST += msm8916_32
PRODUCT_LIST += msm8916_64
PRODUCT_LIST += msm8916_32_512
PRODUCT_LIST += msm8916_32_k64
PRODUCT_LIST += msm8916_64
PRODUCT_LIST += msm8994
PRODUCT_LIST += msm8996
PRODUCT_LIST += msm8996_gvmq
PRODUCT_LIST += msm8909
PRODUCT_LIST += msm8909go
PRODUCT_LIST += msm8909_512
PRODUCT_LIST += msm8909_512go
PRODUCT_LIST += msm8992
PRODUCT_LIST += msm8952_64
PRODUCT_LIST += msm8952_32
PRODUCT_LIST += msm8937_32
PRODUCT_LIST += msm8937_32go
PRODUCT_LIST += msm8937_64
PRODUCT_LIST += msm8953_32
PRODUCT_LIST += msm8953_64
PRODUCT_LIST += msm8998
PRODUCT_LIST += msm8998_32
PRODUCT_LIST += sdm660_64
PRODUCT_LIST += sdm660_32
PRODUCT_LIST += sdm845
PRODUCT_LIST += apq8098_latv
PRODUCT_LIST += sdm710
PRODUCT_LIST += msmnile
PRODUCT_LIST += qcs605
PRODUCT_LIST += qcs605h

MSM7K_PRODUCT_LIST := msm7625_surf
MSM7K_PRODUCT_LIST += msm7625_ffa
MSM7K_PRODUCT_LIST += msm7627_6x
MSM7K_PRODUCT_LIST += msm7627_ffa
MSM7K_PRODUCT_LIST += msm7627_surf
MSM7K_PRODUCT_LIST += msm7627a
MSM7K_PRODUCT_LIST += msm7630_surf
MSM7K_PRODUCT_LIST += msm7630_1x
MSM7K_PRODUCT_LIST += msm7630_fusion

FOTA_PRODUCT_LIST := msm7627a

ifneq ($(strip $(TARGET_VENDOR)),)
  PRODUCT_LIST += $(TARGET_PRODUCT)
endif

ifneq (, $(filter $(PRODUCT_LIST), $(TARGET_PRODUCT)))

  ifneq ($(strip $(TARGET_VENDOR)),)
    include device/$(TARGET_VENDOR)/$(TARGET_PRODUCT)/BoardConfig.mk
  else
    include device/qcom/$(TARGET_PRODUCT)/BoardConfig.mk
  endif

  ifeq ($(call is-board-platform,msm8660),true)
    PREBUILT_BOARD_PLATFORM_DIR := msm8660_surf
  else ifeq ($(TARGET_PRODUCT),msm8625)
    PREBUILT_BOARD_PLATFORM_DIR := msm8625
  else ifeq ($(TARGET_PRODUCT),mpq8064)
    PREBUILT_BOARD_PLATFORM_DIR := mpq8064
  else ifneq ($(strip $(TARGET_BOARD_SUFFIX)),)
    PREBUILT_BOARD_PLATFORM_DIR := $(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX)
  else
    PREBUILT_BOARD_PLATFORM_DIR := $(TARGET_BOARD_PLATFORM)
  endif

  $(call inherit-product-if-exists, $(QC_PROP_ROOT)/common-noship/etc/device-vendor-noship.mk)
  $(call inherit-product-if-exists, $(QC_PROP_ROOT)/prebuilt_grease/target/product/$(PREBUILT_BOARD_PLATFORM_DIR)/prebuilt.mk)
  $(call inherit-product-if-exists, $(QC_PROP_ROOT)/prebuilt_HY11/target/product/$(PREBUILT_BOARD_PLATFORM_DIR)/prebuilt.mk)
  $(call inherit-product-if-exists, $(QC_PROP_ROOT)/prebuilt_HY22/target/product/$(PREBUILT_BOARD_PLATFORM_DIR)/prebuilt.mk)
  ifneq ($(TARGET_BOARD_AUTO),true)
  XML_CONF_PATH := $(QCPATH)/telephony-apps/etc
  endif

  #Add HY22 support
  HY22_XML_CONF_PATH := $(QC_PROP_ROOT)/prebuilt_HY22/target/product/$(PREBUILT_BOARD_PLATFORM_DIR)/system/etc
  ifneq ($(wildcard $(HY22_XML_CONF_PATH)),)
    ifneq ($(TARGET_BOARD_AUTO),true)
    PRODUCT_COPY_FILES += $(HY22_XML_CONF_PATH)/apns-conf.xml:system/etc/apns-conf.xml
    PRODUCT_COPY_FILES += $(HY22_XML_CONF_PATH)/cdma_call_conf.xml:system/etc/cdma_call_conf.xml
    PRODUCT_COPY_FILES += $(HY22_XML_CONF_PATH)/spn-conf.xml:system/etc/spn-conf.xml
    endif
    ifneq ($(wildcard $(HY22_XML_CONF_PATH)/permissions/qti_libpermissions.xml),)
      PRODUCT_COPY_FILES += $(HY22_XML_CONF_PATH)/permissions/qti_libpermissions.xml:system/etc/permissions/qti_libpermissions.xml
    endif
  endif

  ifneq ($(wildcard $(XML_CONF_PATH)),)
    PRODUCT_COPY_FILES += $(XML_CONF_PATH)/apns-conf.xml:system/etc/apns-conf.xml
    PRODUCT_COPY_FILES += $(XML_CONF_PATH)/cdma_call_conf.xml:system/etc/cdma_call_conf.xml
    PRODUCT_COPY_FILES += $(XML_CONF_PATH)/spn-conf.xml:system/etc/spn-conf.xml
    PRODUCT_COPY_FILES += $(XML_CONF_PATH)/qti_libpermissions.xml:system/etc/permissions/qti_libpermissions.xml
  endif
  ifeq ($(BUILD_TINY_ANDROID),true)
    VENDOR_TINY_ANDROID_PACKAGES := $(QC_PROP_ROOT)/diag
    VENDOR_TINY_ANDROID_PACKAGES += $(QC_PROP_ROOT)/kernel-tests
    VENDOR_TINY_ANDROID_PACKAGES += $(QC_PROP_ROOT)/modem-apis
    VENDOR_TINY_ANDROID_PACKAGES += $(QC_PROP_ROOT)/oncrpc
    VENDOR_TINY_ANDROID_PACKAGES += $(QC_PROP_ROOT)/common/build/remote_api_makefiles
    VENDOR_TINY_ANDROID_PACKAGES += $(QC_PROP_ROOT)/common/build/fusion_api_makefiles
    VENDOR_TINY_ANDROID_PACKAGES += $(QC_PROP_ROOT)/common/config

    ifeq ($(call is-board-platform-in-list,$(MSM7K_PRODUCT_LIST)),true)
      VENDOR_TINY_ANDROID_PACKAGES += $(QC_PROP_ROOT)/gps
      VENDOR_TINY_ANDROID_PACKAGES += hardware
      VENDOR_TINY_ANDROID_PACKAGES += external/wpa_supplicant
    endif

  endif # BUILD_TINY_ANDROID
endif

ifeq ($(call is-board-platform-in-list,$(FOTA_PRODUCT_LIST)),true)
  TARGET_FOTA_UPDATE_LIB := libipth libipthlzmadummy
  TARGET_HAS_FOTA        := true
endif

#Include below QRD extensions,will real control compiling in Android.mk under qrdplus
ifeq ($(strip $(TARGET_USES_QTIC)),true)
# Include the QRD extensions.
$(call inherit-product-if-exists, vendor/qcom/proprietary/qrdplus/Extension/products.mk)
# Include the QRD customer extensions for ChinaMobile.
$(call inherit-product-if-exists, vendor/qcom/proprietary/qrdplus/China/ChinaMobile/products.mk)
# Include the QRD customer extensions for ChinaTelecom.
$(call inherit-product-if-exists, vendor/qcom/proprietary/qrdplus/China/ChinaTelecom/products.mk)
# Include the QRD customer extensions for ChinaUnicom.
$(call inherit-product-if-exists, vendor/qcom/proprietary/qrdplus/China/ChinaUnicom/products.mk)
# Include the QRD customer extensions for Regional/MetroPCS/TMobile.
$(call inherit-product-if-exists, vendor/qcom/proprietary/qrdplus/Global/*/products.mk)
# Include the QRD customer extensions for CTA
$(call inherit-product-if-exists, vendor/qcom/proprietary/qrdplus/China/CTA/products.mk)
endif

ifeq ($(strip $(TARGET_USES_QTIC_EXTENSION)),true)
#include the QRD extensions.
$(call inherit-product-if-exists, vendor/qcom/proprietary/qrdplus/Extension/products.mk)
endif

$(call inherit-product-if-exists, $(QC_PROP_ROOT)/android-perf/profiles.mk)
#Add MMI modules to build
$(call inherit-product-if-exists, $(QC_PROP_ROOT)/fastmmi/mmi.mk)

#Add logkit module to build
TARGET_USES_LOGKIT_LOGGING := true

#Include other rules if any
$(call inherit-product-if-exists, $(QC_PROP_ROOT)/common-noship/build/generate_extra_images_prop.mk)


#prebuilt javalib
ifneq ($(wildcard $(QC_PROP_ROOT)/common/build/prebuilt_javalib.mk),)
BUILD_PREBUILT_JAVALIB := $(QC_PROP_ROOT)/common/build/prebuilt_javalib.mk
else
BUILD_PREBUILT_JAVALIB := $(BUILD_PREBUILT)
endif

# Each line here corresponds to an optional LOCAL_MODULE built by
# Android.mk(s) in the proprietary projects. Where project
# corresponds to the vars here in CAPs.

# These modules are tagged with optional as their LOCAL_MODULE_TAGS
# wouldn't be present in your on target images, unless listed here
# explicitly.

#Connection Security
CONNECTION_SECURITY := libminksocket
CONNECTION_SECURITY += ConnectionSecurityService
CONNECTION_SECURITY += SSGTelemetryService
CONNECTION_SECURITY += SSGCredentialService
CONNECTION_SECURITY += ssgqmigd
CONNECTION_SECURITY += ssgtzd

#ADC
ADC := qcci_adc_test
ADC += libqcci_adc

#ADSPRPC
ADSPRPC := libadsprpc
ADSPRPC += libmdsprpc
ADSPRPC += libsdsprpc
ADSPRPC += libcdsprpc
ADSPRPC += libadsprpc_system
ADSPRPC += libmdsprpc_system
ADSPRPC += libsdsprpc_system
ADSPRPC += libcdsprpc_system
ADSPRPC += libadsp_default_listener
ADSPRPC += libcdsp_default_listener
ADSPRPC += libssc_default_listener
ADSPRPC += adsprpcd
ADSPRPC += cdsprpcd
ADSPRPC += libsysmon_cdsp_skel

#ALLJOYN
ALLJOYN := liballjoyn

#AIVPLAY
AIVPLAY := libaivdrmclient
AIVPLAY += libAivPlay

#ANT
ANT := com.qualcomm.qti.ant@1.0-impl
ANT += com.qualcomm.qti.ant@1.0
ANT += com.qualcomm.qti.ant@1.0_vendor

#AntiTheftDemo
ANTITHEFTDEMO := AntiTheftDemo

#Backup Agent
BACKUP_AGENT := QtiBackupAgent

#CameraHawk  App
CAMERAHAWK_APPS := CameraHawk
CAMERAHAWK_APPS += libamui
CAMERAHAWK_APPS += libarcimgfilters
CAMERAHAWK_APPS += libarcimgutils
CAMERAHAWK_APPS += libarcimgutilsbase
CAMERAHAWK_APPS += libarcutils
CAMERAHAWK_APPS += libhdr
CAMERAHAWK_APPS += libphotofix
CAMERAHAWK_APPS += libpluginadaptor
CAMERAHAWK_APPS += libquickview
CAMERAHAWK_APPS += libstory
CAMERAHAWK_APPS += libvideopmk

#Perfect365 App
PERFECT365_APPS := Perfect365
PERFECT365_APPS += libArcSoftFlawlessFace

#Whip Apps
WHIP_APPS := WhipForPhone
WHIP_APPS += libamuiwhip
WHIP_APPS += libarcwhipimgutils
WHIP_APPS += libattextengine
WHIP_APPS += libsns_album
WHIP_APPS += libarcwhipimgutilsbase
WHIP_APPS += libclipmap3

# camerahawk,Whip Common Lib
CAMERAHAWK_WHIP_COMMON_LIB := libarcplatform

#Atmel's mxt cfg file
MXT_CFG := mxt1386e_apq8064_liquid.cfg
MXT_CFG += mxt224_7x27a_ffa.cfg

#BATTERY_CHARGING
BATTERY_CHARGING := battery_charging

#BT
BT := btnvtool
BT += dun-server
BT += hci_qcomm_init
BT += liboi_sbc_decoder
BT += PS_ASIC.pst
BT += RamPatch.txt
BT += sapd
BT += rampatch_tlv.img
BT += nvm_tlv.bin
BT += nvm_tlv_usb.bin
BT += rampatch_tlv_1.3.tlv
BT += nvm_tlv_1.3.bin
BT += rampatch_tlv_2.1.tlv
BT += nvm_tlv_2.1.bin
BT += rampatch_tlv_3.0.tlv
BT += nvm_tlv_3.0.bin
BT += rampatch_tlv_3.2.tlv
BT += nvm_tlv_3.2.bin
BT += wcnss_filter
BT += android.hardware.bluetooth@1.0-service-qti
BT += android.hardware.bluetooth@1.0-impl-qti
BT += android.hardware.bluetooth@1.0-service-qti.rc
BT += com.qualcomm.qti.bluetooth_audio@1.0-impl
BT += com.qualcomm.qti.bluetooth_audio@1.0
BT += com.qualcomm.qti.bluetooth_audio@1.0_vendor

#CCID
CCID := ccid_daemon

#CHARGER_MONITOR
CHARGER_MONITOR := charger_monitor

STATSPOLLMGR := StatsPollManager

#CNE
CNE := andsfCne.xml
CNE += cnd
CNE += cneapiclient
CNE += cneapiclient.xml
CNE += CNEService
CNE += com.quicinc.cne
CNE += com.quicinc.cne.xml
CNE += com.quicinc.cneapiclient
CNE += libcne
CNE += libcneapiclient
CNE += libcneqmiutils
CNE += libcneoplookup
CNE += libmasc
CNE += libvendorconn
CNE += libwms
CNE += libwqe
CNE += libxml
CNE += ROW_profile1.xml
CNE += ROW_profile2.xml
CNE += ROW_profile3.xml
CNE += ROW_profile4.xml
CNE += ROW_profile5.xml
CNE += ROW_profile6.xml
CNE += ROW_profile7.xml
CNE += ROW_profile15.xml
CNE += ROW_profile16.xml
CNE += ROW_profile17.xml
CNE += ROW_profile18.xml
CNE += ROW_profile19.xml
CNE += ROW_profile20.xml
CNE += ROW_profile21.xml
CNE += ROW_profile32.xml
CNE += ATT_profile1.xml
CNE += ATT_profile2.xml
CNE += ATT_profile3.xml
CNE += ATT_profile4.xml
CNE += ATT_profile5.xml
CNE += ATT_profile6.xml
CNE += VZW_profile1.xml
CNE += VZW_profile2.xml
CNE += VZW_profile3.xml
CNE += VZW_profile4.xml
CNE += VZW_profile5.xml
CNE += VZW_profile6.xml
CNE += SwimConfig.xml
CNE += com.quicinc.cne.api@1.0
CNE += com.quicinc.cne.server@1.0
CNE += com.quicinc.cne.server@2.0
CNE += com.quicinc.cne.constants@1.0
CNE += com.quicinc.cne.constants@2.0
CNE += com.quicinc.cne.api-V1.0-java

#DPM
DPM := com.qti.dpmframework
DPM += com.qti.dpmframework.xml
DPM += dpm.conf
DPM += dpmapi
DPM += dpmapi.xml
DPM += dpmd
DPM += dpmd.rc
DPM += dpmQmiMgr
DPM += dpmserviceapp
DPM += libdpmctmgr
DPM += libdpmfdmgr
DPM += libdpmframework
DPM += libdpmnsrm
DPM += libdpmqmihal
DPM += libdpmtcm
DPM += NsrmConfiguration.xml
DPM += tcmclient
DPM += com.qualcomm.qti.dpm.api@1.0
DPM += com.qualcomm.qti.dpm.api@1.0_vendor

#CRASH_LOGGER
CRASH_LOGGER :=  libramdump

#DATA
ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
DATA := ATFWD-daemon
endif
DATA += CKPD-daemon
DATA += dsdnsutil
DATA += dsi_config.xml
DATA += libconfigdb
DATA += libdsnet
DATA += libdsnetutil
DATA += libdsprofile
DATA += libdss
DATA += libdssock
DATA += libdsutils
DATA += libnetmgr
DATA += libqcmaputils
DATA += netmgrd
DATA += netmgr_config.xml
DATA += port-bridge
DATA += qti

#DISPLAY_TESTS
DISPLAY_TESTS := MutexTest

#DISPLAY
DISPLAY := libsdmextension
DISPLAY += libscalar
DISPLAY += libqseed3
DISPLAY += libsdm-disp-apis
DISPLAY += libsdm-color
DISPLAY += libsdm-diag
DISPLAY += libhdr_tm
DISPLAY += libgtestext
DISPLAY += libxmlext
DISPLAY += libsdedrm
DISPLAY += libdisplayqos
DISPLAY += libtinyxml2_1
DISPLAY += libsdm-disp-vndapis

ifneq ($(TARGET_HAS_LOW_RAM),true)
DISPLAY += vendor.display.color@1.0
DISPLAY += vendor.display.color@1.0_vendor
DISPLAY += vendor.display.color@1.1
DISPLAY += vendor.display.color@1.1_vendor
DISPLAY += vendor.display.postproc@1.0
DISPLAY += vendor.display.postproc@1.0_vendor
DISPLAY += vendor.display.color@1.0-service.rc
endif

DISPLAY += vendor.display.config@1.0
DISPLAY += vendor.display.config@1.0_vendor
DISPLAY += libdisplayconfig
ifeq ($(ENABLE_HYP),true)
DISPLAY += lib_drm_fe
endif
DISPLAY += QdcmFF
DISPLAY += qdcmss


#FASTCV
FASTCV := libapps_mem_heap
FASTCV += libapps_mem_heap.so
FASTCV += libdspCV_skel
FASTCV += libdspCV_skel.so
FASTCV += libfastcvadsp
FASTCV += libfastcvadsp.so
FASTCV += libfastcvadsp_skel
FASTCV += libfastcvadsp_skel.so
FASTCV += libfastcvdsp_skel
FASTCV += libfastcvdsp_skel.so
FASTCV += libfastcvadsp_stub
FASTCV += libfastcvdsp_stub
FASTCV += libfastcvopt
FASTCV += libapps_mem_heap_system
FASTCV += libapps_mem_heap_system.so
FASTCV += libdspCV_skel_system
FASTCV += libdspCV_skel_system.so
FASTCV += libfastcvadsp_system
FASTCV += libfastcvadsp_system.so
FASTCV += libfastcvadsp_skel_system
FASTCV += libfastcvadsp_skel_system.so
FASTCV += libfastcvdsp_skel_system
FASTCV += libfastcvdsp_skel_system.so
FASTCV += libfastcvadsp_stub_system
FASTCV += libfastcvdsp_stub_system
FASTCV += libfastcvopt_system
#FASTCV_TEST_BINARIES
FASTCV += fastcv_test_oem
FASTCV += fastcv_test_oem_system
FASTCV += fastcv_test
FASTCV += fastcv_test_system
FASTCV += fastcv_test_noopencv
FASTCV += fastcv_test_noopencv_system

#FASTMMI
FASTMMI := mmi
FASTMMI += mmi.cfg
FASTMMI += MMI_PCBA.cfg
FASTMMI += fail.png
FASTMMI += fonts.ttf
FASTMMI += pass.png
FASTMMI += libmmi
FASTMMI += mmi_audio
FASTMMI += mmi_battery
FASTMMI += mmi_bt
FASTMMI += mmi_camera
FASTMMI += mmi_flashlight
FASTMMI += mmi_fm
FASTMMI += mmi_gps_garden
FASTMMI += mmi_gps
FASTMMI += mmi_gsensor
FASTMMI += mmi_gyroscope
FASTMMI += mmi_key
FASTMMI += mmi_keypadbacklight
FASTMMI += mmi_lcd
FASTMMI += mmi_led
FASTMMI += mmi_lsensor
FASTMMI += mmi_msensor
FASTMMI += mmi_nfc
FASTMMI += mmi_psensor
FASTMMI += mmi_sdcard
FASTMMI += mmi_sim
FASTMMI += mmi_sysinfo
FASTMMI += mmi_touchpanel
FASTMMI += mmi_touch
FASTMMI += mmi_vibrator
FASTMMI += mmi_volume
FASTMMI += mmi_wifi
FASTMMI += strings.xml
FASTMMI += strings-zh-rCN.xml

#FLASH
FLASH := install_flash_player.apk
FLASH += libflashplayer.so
FLASH += libstagefright_froyo.so
FLASH += libstagefright_honeycomb.so
FLASH += libysshared.so
FLASH += oem_install_flash_player.apk

#FM
FM := fmconfig
FM += fmfactorytest
FM += fmfactorytestserver
FM += fm_qsoc_patches
FM += vendor.qti.hardware.fm@1.0_vendor
FM += vendor.qti.hardware.fm@1.0-impl

#FOTA
FOTA := ipth_dua
FOTA += libcrc32.a
FOTA += libdua.a
FOTA += libdme_main
FOTA += libidev_dua.a
FOTA += libipth.a
FOTA += libshared_lib.a
FOTA += libzlib.1.2.1.a
FOTA += MobileUpdateClient.apk

#FTM
FTM := ftmdaemon
FTM += wdsdaemon

#DIAG
DIAG := diag_callback_sample
DIAG += diag_dci_sample
DIAG += diag_klog
DIAG += diag_mdlog
DIAG += diag_socket_log
DIAG += diag_qshrink4_daemon
DIAG += diag_uart_log
DIAG += PktRspTest
DIAG += test_diag
DIAG += libdiag_system
DIAG += libdiagjni

# ENTERPRISE_SECURITY
ENTERPRISE_SECURITY := libescommonmisc
ENTERPRISE_SECURITY += lib_nqs
ENTERPRISE_SECURITY += lib_pfe
ENTERPRISE_SECURITY += libpfecommon
ENTERPRISE_SECURITY += ml_daemon
ENTERPRISE_SECURITY += nqs
ENTERPRISE_SECURITY += pfm
ENTERPRISE_SECURITY += seald
ENTERPRISE_SECURITY += libsealcomm
ENTERPRISE_SECURITY += libsealdsvc
ENTERPRISE_SECURITY += libsealaccess
ENTERPRISE_SECURITY += libsealjni
ENTERPRISE_SECURITY += libprotobuf-cpp-2.3.0-qti-lite
ENTERPRISE_SECURITY += qsb-port
ENTERPRISE_SECURITY += security_boot_check
ENTERPRISE_SECURITY += security-bridge
#ENTERPRISE_SECURITY += tloc_daemon

# FINGERPRINT
FINGERPRINT := board2.ini
FINGERPRINT += libqfp-service
FINGERPRINT += qfp-daemon
FINGERPRINT += QFingerprintService
FINGERPRINT += QFPCalibration
FINGERPRINT += template_imaginary.bin
FINGERPRINT += template_real.bin
FINGERPRINT += vendor.qti.hardware.fingerprint@1.0
FINGERPRINT += vendor.qti.hardware.fingerprint-V1.0-java
FINGERPRINT += android.hardware.biometrics.fingerprint@2.1
FINGERPRINT += qti_fingerprint_interface.xml

#GPS
GPS := loc_api_app
GPS += test_loc_api_client
GPS += test_bit
GPS += test_agps_server
GPS += izat.conf
GPS += sap.conf
GPS += com.qualcomm.location.vzw_library.xml
GPS += libloc_ext
GPS += xtra_t_app
GPS += libgeofence
GPS += libflp
GPS += izat.xt.srv
GPS += izat.xt.srv.xml
GPS += com.qti.location.sdk
GPS += com.qti.location.sdk.xml
GPS += libxtadapter
GPS += slim_daemon
GPS += libslimclient
GPS += libulp2
GPS += loc_parameter.ini
GPS += pf_test_app
GPS += garden_app
GPS += libalarmservice_jni
GPS += liblocationservice_glue
GPS += liblocationservice_jni
GPS += liblocationservice
GPS += libdataitems
GPS += libizat_core
GPS += liblbs_core
GPS += com.qualcomm.location
GPS += com.qualcomm.location.xml
GPS += privapp-permissions-com.qualcomm.location.xml
GPS += libquipc_os_api
GPS += libxt_native
GPS += liblowi_client
GPS += liblowi_wifihal
GPS += liblowi_wifihal_nl
GPS += mlid
GPS += flp.conf
GPS += flp.default
GPS += loc_launcher
GPS += libgdtap
GPS += libizat_client_api
GPS += libloc_pla
GPS += libloc_stub
GPS += xtra-daemon
GPS += OptInAppOverlay

#GPS-XTWiFi
GPS += cacert_location.pem
GPS += libxtwifi_ulp_adaptor
GPS += libasn1crt
GPS += libasn1crtx
GPS += libasn1cper
GPS += xtra_root_cert.pem
GPS += xtwifi-client
GPS += xtwifi.conf
GPS += xtwifi-inet-agent
GPS += xtwifi-upload-test
GPS += test-fake-ap
GPS += test-pos-tx
GPS += test-version
GPS += lowi-server
GPS += lowi.conf
GPS += liblowi_client

#GPS-Automotive
GPS += libDRPlugin
GPS += libdrplugin_client
GPS += lib_drplugin_server
GPS += DR_AP_Service
GPS += libloc_externalDr
GPS += apdr.conf

ifneq ($(TARGET_HAS_LOW_RAM),true)
# GPS-HIDL
LOC_HIDL_1_0_TARGET_LIST := msm8960
LOC_HIDL_1_0_TARGET_LIST += msm8974
LOC_HIDL_1_0_TARGET_LIST += msm8226
LOC_HIDL_1_0_TARGET_LIST += msm8610
LOC_HIDL_1_0_TARGET_LIST += apq8084
LOC_HIDL_1_0_TARGET_LIST += msm8916
LOC_HIDL_1_0_TARGET_LIST += msm8994
LOC_HIDL_1_0_TARGET_LIST += msm8909
LOC_HIDL_1_0_TARGET_LIST += msm8952
LOC_HIDL_1_0_TARGET_LIST += msm8992
LOC_HIDL_1_0_TARGET_LIST += msm8996
LOC_HIDL_1_0_TARGET_LIST += msm8937
LOC_HIDL_1_0_TARGET_LIST += msm8953
LOC_HIDL_1_0_TARGET_LIST += msm8998
LOC_HIDL_1_0_TARGET_LIST += apq8098_latv
LOC_HIDL_1_0_TARGET_LIST += sdm660
LOC_HIDL_1_1_TARGET_LIST := sdm845
LOC_HIDL_1_2_TARGET_LIST := sdm710
LOC_HIDL_1_2_TARGET_LIST += qcs605
LOC_HIDL_1_2_TARGET_LIST += msmnile

ifneq (,$(filter $(LOC_HIDL_1_0_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
GPS += vendor.qti.gnss@1.0
GPS += vendor.qti.gnss@1.0_vendor
GPS += vendor.qti.gnss@1.0-impl
GPS += vendor.qti.gnss@1.0-service
LOC_HIDL_VERSION = 1.0
endif

ifneq (,$(filter $(LOC_HIDL_1_1_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
GPS += vendor.qti.gnss@1.0
GPS += vendor.qti.gnss@1.0_vendor
GPS += vendor.qti.gnss@1.1
GPS += vendor.qti.gnss@1.1_vendor
GPS += vendor.qti.gnss@1.1-impl
GPS += vendor.qti.gnss@1.1-service
LOC_HIDL_VERSION = 1.1
endif

ifneq (,$(filter $(LOC_HIDL_1_2_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
GPS += vendor.qti.gnss@1.0
GPS += vendor.qti.gnss@1.0_vendor
GPS += vendor.qti.gnss@1.1
GPS += vendor.qti.gnss@1.1_vendor
GPS += vendor.qti.gnss@1.2
GPS += vendor.qti.gnss@1.2_vendor
GPS += vendor.qti.gnss@1.2-impl
GPS += vendor.qti.gnss@1.2-service
LOC_HIDL_VERSION = 1.2
endif

endif # TARGET_HAS_LOW_RAM
#hvdcp 3.0 daemon
HVDCP_OPTI := hvdcp_opti

#GsmaNfcService
ifeq ($(strip $(TARGET_ENABLE_PROPRIETARY_SMARTCARD_SERVICE)),true)
ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
GSMA_NFC := GsmaNfcService
GSMA_NFC += com.gsma.services.nfc
GSMA_NFC += com.gsma.services.nfc.xml
GSMA_NFC += com.gsma.services.utils
GSMA_NFC += com.gsma.services.utils.xml
endif
endif

#HBTP
HBTP := hbtp_daemon
HBTP += hbtp_tool
HBTP += hbtpcfg.dat
HBTP += hbtpcfg_8917.dat
HBTP += qtc800s_dsp.bin
HBTP += qtc800s_dsp30.bin
HBTP += qtc800h_dsp30.bin
HBTP += libfastrpc_aue_stub
HBTP += libFastRPC_AUE_Forward_skel
HBTP += libhbtpdsp
HBTP += libhbtpclient
HBTP += libhbtpfrmwk
HBTP += libfastrpc_utf_stub
HBTP += libFastRPC_UTF_Forward_skel
HBTP += libhbtpjni
HBTP += improveTouchStudio
HBTP += libFastRPC_UTF_Forward_Rohm_skel
HBTP += qtc800s.bin
HBTP += qtc800t.bin
HBTP += qtc800h.bin
HBTP += libFastRPC_UTF_Forward_Qtc2_skel
HBTP += hbtpcfg2.dat
HBTP += hbtpcfg_sdm660_800s_qhd.dat
HBTP += hbtpcfg_sdm660_800s_qhd_dsp30.dat
HBTP += hbtpcfg_sdm660_800h_qhd_dsp30.dat
HBTP += libFastRPC_UTF_Forward_sdm660_skel
HBTP += hbtpcfg_sdm660_800h_qhd.dat
HBTP += libFastRPC_UTF_Forward_800h_skel
HBTP += hbtpcfg_sdm630_800s_fhd.dat
HBTP += hbtpcfg_adsp_800s_fhd.dat
HBTP += libFastRPC_UTF_Forward_800s_sdm845_skel
HBTP += libFastRPC_UTF_Forward_800s_sdm845_uimg_skel
HBTP += hbtpcfg_sdm845_800s_qhd.dat
HBTP += hbtpcfg_sdm845_800s_4k.dat
HBTP += hbtpcfg_sdm845_800h_qhd.dat
HBTP += vendor.qti.hardware.improvetouch.touchcompanion@1.0
HBTP += vendor.qti.hardware.improvetouch.touchcompanion@1.0_vendor
HBTP += vendor.qti.hardware.improvetouch.touchcompanion@1.0-service
HBTP += vendor.qti.hardware.improvetouch.gesturemanager@1.0
HBTP += vendor.qti.hardware.improvetouch.gesturemanager@1.0_vendor
HBTP += vendor.qti.hardware.improvetouch.gesturemanager@1.0-service
HBTP += vendor.qti.hardware.improvetouch.blobmanager@1.0
HBTP += vendor.qti.hardware.improvetouch.blobmanager@1.0_vendor
HBTP += vendor.qti.hardware.improvetouch.blobmanager@1.0-service
HBTP += hbtpcfg_msm8937_800s_fhd.dat
HBTP += hbtpcfg_msm8953_800s_fhd.dat
HBTP += loader.cfg
HBTP += improveTouch
HBTP += qtc801s.bin
HBTP += qtc801s_450.bin
HBTP += hbtpcfg_sdm710_801s_qhd.dat
HBTP += hbtpcfg_sdm660_801s_qhd.dat
HBTP += hbtpcfg_sdm710_800h_qhd.dat
HBTP += hbtpcfg_sdm710_800s_fhd.dat
HBTP += hbtpcfg_sdm710_801s_2k.dat
HBTP += hbtpcfg_hdk670_801s_2k.dat
HBTP += qtc800s_msm8937.bin
HBTP += hbtpcfg_sdm450_801s_fhdp.dat

#MLVM
MLVM += mlvm-system.img
#HY11_HY22 diff
HY11_HY22_diff += libacdb-fts
HY11_HY22_diff += libdatactrl
HY11_HY22_diff += libevent_observer
HY11_HY22_diff += libimsmedia_jni
HY11_HY22_diff += libril-qc-ltedirectdisc
HY11_HY22_diff += libsettings
HY11_HY22_diff += libwfdavenhancements
HY11_HY22_diff += libacdb-fts
HY11_HY22_diff += libacdbrtac
HY11_HY22_diff += libadiertac
HY11_HY22_diff += libdatactrl
HY11_HY22_diff += libdiag
HY11_HY22_diff += libevent_observer
HY11_HY22_diff += libimsmedia_jni
HY11_HY22_diff += libwbc_jni
HY11_HY22_diff += libqti-iop
HY11_HY22_diff += libmmcamera2_c2d_module
HY11_HY22_diff += libmmcamera2_cpp_module
HY11_HY22_diff += libmmcamera2_iface_modules
HY11_HY22_diff += libmmcamera2_imglib_modules
HY11_HY22_diff += libmmcamera2_isp_modules
HY11_HY22_diff += libmmcamera2_pp_buf_mgr
HY11_HY22_diff += libmmcamera2_pproc_modules
HY11_HY22_diff += libmmcamera2_sensor_modules
HY11_HY22_diff += libmmcamera2_stats_modules
HY11_HY22_diff += libmmcamera2_stats_lib
HY11_HY22_diff += libmmcamera_eztune_module
HY11_HY22_diff += libmmcamera_ppbase_module
HY11_HY22_diff += libmmcamera_sw2d_lib
HY11_HY22_diff += libmmcamera_thread_services
HY11_HY22_diff += libqdi
HY11_HY22_diff += libqdp
HY11_HY22_diff += libqmi_client_qmux
HY11_HY22_diff += libremosaic_daemon
HY11_HY22_diff += libril-qc-ltedirectdisc
HY11_HY22_diff += libsensor_reg
HY11_HY22_diff += libsettings
HY11_HY22_diff += libtime_genoff
HY11_HY22_diff += libwfdavenhancements
HY11_HY22_diff += libwfdmmsink
HY11_HY22_diff += libwfduibcsinkinterface
HY11_HY22_diff += libwfduibcsink
HY11_HY22_diff += libpdnotifier
HY11_HY22_diff += libperfgluelayer
HY11_HY22_diff += libqti-iopd-client
HY11_HY22_diff += libvpptestutils
HY11_HY22_diff += libqti-gt-prop
HY11_HY22_diff += vendor.qti.hardware.radio.atcmdfwd@1.0_vendor
HY11_HY22_diff += LteDirectDiscovery.xml
HY11_HY22_diff += qti_libpermissions.xml
HY11_HY22_diff += vendor-qti-hardware-alarm.xml
HY11_HY22_diff += cdma_call_conf.xml
HY11_HY22_diff += spn-conf.xml
HY11_HY22_diff += libsns_low_lat_stream_skel_system
HY11_HY22_diff += libqti-util_system
HY11_HY22_diff += CarrierCacheService
HY11_HY22_diff += CarrierConfigure
HY11_HY22_diff += CarrierLoadService
HY11_HY22_diff += com.emoji.keyboard.touchpal-6.2.5.1
HY11_HY22_diff += SnapdragonSVA
HY11_HY22_diff += PowerOffAlarmHandler
HY11_HY22_diff += Qmmi
HY11_HY22_diff += SecProtect
HY11_HY22_diff += vendor.qti.hardware.alarm@1.0-service
HY11_HY22_diff += vendor.qti.hardware.alarm@1.0-impl
HY11_HY22_diff += libexternal_dog_skel
HY11_HY22_diff += vendor.qti.hardware.alarm@1.0-impl
HY11_HY22_diff += qmi-framework-tests
HY11_HY22_diff += liblogwrap
HY11_HY22_diff += libminui
HY11_HY22_diff += libwificond_ipc
HY11_HY22_diff += libqcrilFramework
HY11_HY22_diff += vendor.qti.hardware.vpp@1.1_vendor
HY11_HY22_diff += libgarden
HY11_HY22_diff += qcril_database

#HY22 debug modules
HY11_HY22_diff_debug += com.qti.vzw.ims.internal.tests
HY11_HY22_diff_debug += uimremoteserver
HY11_HY22_diff_debug += uimremoteserver.xml
HY11_HY22_diff_debug += uimremoteclient
HY11_HY22_diff_debug += uimremoteclient
HY11_HY22_diff_debug += radioconfig
HY11_HY22_diff_debug += radioconfig.xml
HY11_HY22_diff_debug += radioconfiginterface.xml
HY11_HY22_diff_debug += PresenceApp
HY11_HY22_diff_debug += libmmi_jni
HY11_HY22_diff_debug += libomx-dts
#IMS
IMS := exe-ims-regmanagerprocessnative
#IMS += exe-ims-videoshareprocessnative
IMS += lib-imsdpl
IMS += lib-imsfiledemux
IMS += lib-imsfilemux
IMS += lib-imsqimf
IMS += lib-ims-regmanagerbindernative
IMS += lib-ims-regmanagerjninative
IMS += lib-ims-regmanager
#IMS += lib-ims-videosharebindernative
#IMS += lib-ims-videosharejninative
#IMS += lib-ims-videoshare

# IMS Telephony Libs
IMS_TEL := ims.xml
IMS_TEL += imslibrary
IMS_TEL += ims

# for low RAM projects, remove btmultisim
ifneq ($(strip $(TARGET_USES_IOTCC_HEADLESS)),true)
ifneq ($(TARGET_HAS_LOW_RAM),true)
# BT-Telephony Lib for DSDA
BT_TEL := btmultisim.xml
BT_TEL += btmultisimlibrary
BT_TEL += btmultisim
endif
endif

#IMS_VT
IMS_VT := lib-imsvt
IMS_VT += lib-imscamera
IMS_VT += lib-imsvtextutils
IMS_VT += lib-imsvtutils
IMS_VT += lib-imsvideocodec
IMS_VT += libloopbackvtjni
IMS_VT += LoopbackVT

ifeq ($(call is-board-platform-in-list,msm8974 msm8226 apq8084),true)
IMS_VT += libvcel
else
IMS_VT += librcc
endif

# Vzw Specific RCS APIs (Android Telephony)
IMS_TEL_RCS := qti-vzw-ims-internal
IMS_TEL_RCS += qti-vzw-ims-internal.xml
IMS_TEL_RCS += qti-vzw-ims-common
IMS_TEL_RCS += qti-vzw-ims-common.xml

#IMS_RCS
IMS_RCS := lib-imsxml
IMS_RCS += lib-uceservice
IMS_RCS += lib-imscmservice
IMS_RCS += imsrcsd
IMS_RCS += uceShimService
IMS_RCS += whitelist_com.qualcomm.qti.uceShimService
IMS_RCS += com.qualcomm.qti.imscmservice@1.0
IMS_RCS += com.qualcomm.qti.imscmservice@1.0_vendor
IMS_RCS += lib-imsrcs-v2
IMS_RCS += com.qualcomm.qti.imscmservice@1.0-java
IMS_RCS += com.qualcomm.qti.imscmservice@1.1
IMS_RCS += com.qualcomm.qti.imscmservice@1.1_vendor
IMS_RCS += com.qualcomm.qti.imscmservice@1.1-java
IMS_RCS += com.qualcomm.qti.imscmservice.xml
IMS_RCS += com.qualcomm.qti.imscmservice_1_1.xml

#IMS_NEWARCH
IMS_NEWARCH := imsdatadaemon
IMS_NEWARCH += imsqmidaemon
IMS_NEWARCH += ims_rtp_daemon
IMS_NEWARCH += lib-dplmedia
IMS_NEWARCH += lib-imsdpl
IMS_NEWARCH += lib-imsqimf
IMS_NEWARCH += lib-imsSDP
IMS_NEWARCH += lib-rtpcommon
IMS_NEWARCH += lib-rtpcore
IMS_NEWARCH += lib-rtpdaemoninterface
IMS_NEWARCH += lib-rtpsl
IMS_NEWARCH += lib-siputility
IMS_NEWARCH += vendor.qti.imsrtpservice@1.0-service-Impl
IMS_NEWARCH += vendor.qti.imsrtpservice@1.0
IMS_NEWARCH += vendor.qti.imsrtpservice@1.0_vendor

#IMS_REGMGR
IMS_REGMGR := RegmanagerApi

#INTERFACE_PERMISSIONS
INTERFACE_PERMISSIONS := InterfacePermissions
INTERFACE_PERMISSIONS += interface_permissions.xml

#IQAGENT
IQAGENT := libiq_service
IQAGENT += libiq_client
IQAGENT += IQAgent
IQAGENT += iqmsd
IQAGENT += iqclient

# MARE/QBLAS
MARE := libmare-1.1
MARE += libmare-cpu-1.1
MARE += libQBLAS-0.13.0
MARE += libmare_hexagon_skel.so
MARE += libAMF_hexagon_skel.so

#MDM_HELPER
MDM_HELPER := mdm_helper

#MDM_HELPER_PROXY
MDM_HELPER_PROXY := mdm_helper_proxy

#QCAT_UNBUFFERED
QCAT_UNBUFFERED := qcat_unbuffered

#MM_AUDIO
MM_AUDIO := acdbtest
MM_AUDIO += audiod
MM_AUDIO += AudioFilter
MM_AUDIO += libacdbloader
MM_AUDIO += libacdbmapper
MM_AUDIO += libalsautils
MM_AUDIO += libatu
MM_AUDIO += libaudcal
MM_AUDIO += MTP_Bluetooth_cal.acdb
MM_AUDIO += MTP_Codec_cal.acdb
MM_AUDIO += MTP_General_cal.acdb
MM_AUDIO += MTP_Global_cal.acdb
MM_AUDIO += MTP_Handset_cal.acdb
MM_AUDIO += MTP_Hdmi_cal.acdb
MM_AUDIO += MTP_Headset_cal.acdb
MM_AUDIO += MTP_Speaker_cal.acdb
MM_AUDIO += MTP_workspaceFile.qwsp
MM_AUDIO += MTP_360CAM_Bluetooth_cal.acdb
MM_AUDIO += MTP_360CAM_General_cal.acdb
MM_AUDIO += MTP_360CAM_Global_cal.acdb
MM_AUDIO += MTP_360CAM_Handset_cal.acdb
MM_AUDIO += MTP_360CAM_Hdmi_cal.acdb
MM_AUDIO += MTP_360CAM_Headset_cal.acdb
MM_AUDIO += MTP_360CAM_Speaker_cal.acdb
MM_AUDIO += MTP_360CAM_workspaceFile.qwsp
MM_AUDIO += MTP_WCD9330_Bluetooth_cal.acdb
MM_AUDIO += MTP_WCD9330_General_cal.acdb
MM_AUDIO += MTP_WCD9330_Global_cal.acdb
MM_AUDIO += MTP_WCD9330_Handset_cal.acdb
MM_AUDIO += MTP_WCD9330_Hdmi_cal.acdb
MM_AUDIO += MTP_WCD9330_Headset_cal.acdb
MM_AUDIO += MTP_WCD9330_Speaker_cal.acdb
MM_AUDIO += MTP_WCD9335_Bluetooth_cal.acdb
MM_AUDIO += MTP_WCD9335_General_cal.acdb
MM_AUDIO += MTP_WCD9335_Global_cal.acdb
MM_AUDIO += MTP_WCD9335_Handset_cal.acdb
MM_AUDIO += MTP_WCD9335_Hdmi_cal.acdb
MM_AUDIO += MTP_WCD9335_Headset_cal.acdb
MM_AUDIO += MTP_WCD9335_Speaker_cal.acdb
MM_AUDIO += MTP_WCD9335_workspaceFile.qwsp
MM_AUDIO += MTP_WCD9335_TDM_Bluetooth_cal.acdb
MM_AUDIO += MTP_WCD9335_TDM_General_cal.acdb
MM_AUDIO += MTP_WCD9335_TDM_Global_cal.acdb
MM_AUDIO += MTP_WCD9335_TDM_Handset_cal.acdb
MM_AUDIO += MTP_WCD9335_TDM_Hdmi_cal.acdb
MM_AUDIO += MTP_WCD9335_TDM_Headset_cal.acdb
MM_AUDIO += MTP_WCD9335_TDM_Speaker_cal.acdb
MM_AUDIO += MTP_WCD9335_TDM_workspaceFile.qwsp
MM_AUDIO += MTP_WCD9340_Bluetooth_cal.acdb
MM_AUDIO += MTP_WCD9340_Codec_cal.acdb
MM_AUDIO += MTP_WCD9340_General_cal.acdb
MM_AUDIO += MTP_WCD9340_Global_cal.acdb
MM_AUDIO += MTP_WCD9340_Handset_cal.acdb
MM_AUDIO += MTP_WCD9340_Hdmi_cal.acdb
MM_AUDIO += MTP_WCD9340_Headset_cal.acdb
MM_AUDIO += MTP_WCD9340_Speaker_cal.acdb
MM_AUDIO += MTP_WCD9340_workspaceFile.qwsp
MM_AUDIO += MTP_WCD9306_Bluetooth_cal.acdb
MM_AUDIO += MTP_WCD9306_General_cal.acdb
MM_AUDIO += MTP_WCD9306_Global_cal.acdb
MM_AUDIO += MTP_WCD9306_Handset_cal.acdb
MM_AUDIO += MTP_WCD9306_Hdmi_cal.acdb
MM_AUDIO += MTP_WCD9306_Headset_cal.acdb
MM_AUDIO += MTP_WCD9306_Speaker_cal.acdb
MM_AUDIO += MTP_QCS605_Bluetooth_cal.acdb
MM_AUDIO += MTP_QCS605_Codec_cal.acdb
MM_AUDIO += MTP_QCS605_General_cal.acdb
MM_AUDIO += MTP_QCS605_Global_cal.acdb
MM_AUDIO += MTP_QCS605_Handset_cal.acdb
MM_AUDIO += MTP_QCS605_Hdmi_cal.acdb
MM_AUDIO += MTP_QCS605_Headset_cal.acdb
MM_AUDIO += MTP_QCS605_Speaker_cal.acdb
MM_AUDIO += MTP_QCS605_workspaceFile.qwsp
MM_AUDIO += QRD_Bluetooth_cal.acdb
MM_AUDIO += QRD_Codec_cal.acdb
MM_AUDIO += QRD_General_cal.acdb
MM_AUDIO += QRD_Global_cal.acdb
MM_AUDIO += QRD_Handset_cal.acdb
MM_AUDIO += QRD_Hdmi_cal.acdb
MM_AUDIO += QRD_Headset_cal.acdb
MM_AUDIO += QRD_Speaker_cal.acdb
MM_AUDIO += QRD_workspaceFile.qwsp
MM_AUDIO += QRD_WCD9340_Bluetooth_cal.acdb
MM_AUDIO += QRD_WCD9340_Codec_cal.acdb
MM_AUDIO += QRD_WCD9340_General_cal.acdb
MM_AUDIO += QRD_WCD9340_Global_cal.acdb
MM_AUDIO += QRD_WCD9340_Handset_cal.acdb
MM_AUDIO += QRD_WCD9340_Hdmi_cal.acdb
MM_AUDIO += QRD_WCD9340_Headset_cal.acdb
MM_AUDIO += QRD_WCD9340_Speaker_cal.acdb
MM_AUDIO += QRD_WCD9340_workspaceFile.qwsp
MM_AUDIO += QRD_QVR_WCD9340_Bluetooth_cal.acdb
MM_AUDIO += QRD_QVR_WCD9340_Codec_cal.acdb
MM_AUDIO += QRD_QVR_WCD9340_General_cal.acdb
MM_AUDIO += QRD_QVR_WCD9340_Global_cal.acdb
MM_AUDIO += QRD_QVR_WCD9340_Handset_cal.acdb
MM_AUDIO += QRD_QVR_WCD9340_Hdmi_cal.acdb
MM_AUDIO += QRD_QVR_WCD9340_Headset_cal.acdb
MM_AUDIO += QRD_QVR_WCD9340_Speaker_cal.acdb
MM_AUDIO += QRD_QVR_WCD9340_workspaceFile.qwsp
MM_AUDIO += QRD_SKUK_WCD9340_Bluetooth_cal.acdb
MM_AUDIO += QRD_SKUK_WCD9340_Codec_cal.acdb
MM_AUDIO += QRD_SKUK_WCD9340_General_cal.acdb
MM_AUDIO += QRD_SKUK_WCD9340_Global_cal.acdb
MM_AUDIO += QRD_SKUK_WCD9340_Handset_cal.acdb
MM_AUDIO += QRD_SKUK_WCD9340_Hdmi_cal.acdb
MM_AUDIO += QRD_SKUK_WCD9340_Headset_cal.acdb
MM_AUDIO += QRD_SKUK_WCD9340_Speaker_cal.acdb
MM_AUDIO += QRD_SKUK_WCD9340_workspaceFile.qwsp
MM_AUDIO += QRD_SKUW_Bluetooth_cal.acdb
MM_AUDIO += QRD_SKUW_General_cal.acdb
MM_AUDIO += QRD_SKUW_Global_cal.acdb
MM_AUDIO += QRD_SKUW_Handset_cal.acdb
MM_AUDIO += QRD_SKUW_Hdmi_cal.acdb
MM_AUDIO += QRD_SKUW_Headset_cal.acdb
MM_AUDIO += QRD_SKUW_Speaker_cal.acdb
MM_AUDIO += QRD_SKUW_workspaceFile.qwsp
MM_AUDIO += QRD_360CAM_Bluetooth_cal.acdb
MM_AUDIO += QRD_360CAM_General_cal.acdb
MM_AUDIO += QRD_360CAM_Global_cal.acdb
MM_AUDIO += QRD_360CAM_Handset_cal.acdb
MM_AUDIO += QRD_360CAM_Hdmi_cal.acdb
MM_AUDIO += QRD_360CAM_Headset_cal.acdb
MM_AUDIO += QRD_360CAM_Speaker_cal.acdb
MM_AUDIO += QRD_360CAM_workspaceFile.qwsp
MM_AUDIO += QRD_SKUS_Bluetooth_cal.acdb
MM_AUDIO += QRD_SKUS_General_cal.acdb
MM_AUDIO += QRD_SKUS_Global_cal.acdb
MM_AUDIO += QRD_SKUS_Handset_cal.acdb
MM_AUDIO += QRD_SKUS_Hdmi_cal.acdb
MM_AUDIO += QRD_SKUS_Headset_cal.acdb
MM_AUDIO += QRD_SKUS_Speaker_cal.acdb
MM_AUDIO += QRD_SKUS_workspaceFile.qwsp
MM_AUDIO += QRD_SKUSH_Bluetooth_cal.acdb
MM_AUDIO += QRD_SKUSH_General_cal.acdb
MM_AUDIO += QRD_SKUSH_Global_cal.acdb
MM_AUDIO += QRD_SKUSH_Handset_cal.acdb
MM_AUDIO += QRD_SKUSH_Hdmi_cal.acdb
MM_AUDIO += QRD_SKUSH_Headset_cal.acdb
MM_AUDIO += QRD_SKUSH_Speaker_cal.acdb
MM_AUDIO += QRD_SKUSH_workspaceFile.qwsp
MM_AUDIO += QRD_SKU2_Bluetooth_cal.acdb
MM_AUDIO += QRD_SKU2_General_cal.acdb
MM_AUDIO += QRD_SKU2_Global_cal.acdb
MM_AUDIO += QRD_SKU2_Handset_cal.acdb
MM_AUDIO += QRD_SKU2_Hdmi_cal.acdb
MM_AUDIO += QRD_SKU2_Headset_cal.acdb
MM_AUDIO += QRD_SKU2_Speaker_cal.acdb
MM_AUDIO += QRD_SKU2_workspaceFile.acdb
MM_AUDIO += QRD_SKU6_Bluetooth_cal.acdb
MM_AUDIO += QRD_SKU6_General_cal.acdb
MM_AUDIO += QRD_SKU6_Global_cal.acdb
MM_AUDIO += QRD_SKU6_Handset_cal.acdb
MM_AUDIO += QRD_SKU6_Hdmi_cal.acdb
MM_AUDIO += QRD_SKU6_Headset_cal.acdb
MM_AUDIO += QRD_SKU6_Speaker_cal.acdb
MM_AUDIO += QRD_SKU6_workspaceFile.acdb
MM_AUDIO += QRD_TMO_Bluetooth_cal.acdb
MM_AUDIO += QRD_TMO_General_cal.acdb
MM_AUDIO += QRD_TMO_Global_cal.acdb
MM_AUDIO += QRD_TMO_Handset_cal.acdb
MM_AUDIO += QRD_TMO_Hdmi_cal.acdb
MM_AUDIO += QRD_TMO_Headset_cal.acdb
MM_AUDIO += QRD_TMO_Speaker_cal.acdb
MM_AUDIO += QRD_TMO_workspaceFile.acdb
MM_AUDIO += QRD_SKU4_Bluetooth_cal.acdb
MM_AUDIO += QRD_SKU4_General_cal.acdb
MM_AUDIO += QRD_SKU4_Global_cal.acdb
MM_AUDIO += QRD_SKU4_Handset_cal.acdb
MM_AUDIO += QRD_SKU4_Hdmi_cal.acdb
MM_AUDIO += QRD_SKU4_Headset_cal.acdb
MM_AUDIO += QRD_SKU4_Speaker_cal.acdb
MM_AUDIO += QRD_SKU4_workspaceFile.qwsp
MM_AUDIO += SDM439_SKU1_Bluetooth_cal.acdb
MM_AUDIO += SDM439_SKU1_General_cal.acdb
MM_AUDIO += SDM439_SKU1_Global_cal.acdb
MM_AUDIO += SDM439_SKU1_Handset_cal.acdb
MM_AUDIO += SDM439_SKU1_Hdmi_cal.acdb
MM_AUDIO += SDM439_SKU1_Headset_cal.acdb
MM_AUDIO += SDM439_SKU1_Speaker_cal.acdb
MM_AUDIO += SDM439_SKU1_workspaceFile.qwsp
MM_AUDIO += Liquid_Bluetooth_cal.acdb
MM_AUDIO += Liquid_General_cal.acdb
MM_AUDIO += Liquid_Global_cal.acdb
MM_AUDIO += Liquid_Handset_cal.acdb
MM_AUDIO += Liquid_Hdmi_cal.acdb
MM_AUDIO += Liquid_Headset_cal.acdb
MM_AUDIO += Liquid_Speaker_cal.acdb
MM_AUDIO += Liquid_workspaceFile.qwsp
MM_AUDIO += Liquid_WCD9340_Bluetooth_cal.acdb
MM_AUDIO += Liquid_WCD9340_Codec_cal.acdb
MM_AUDIO += Liquid_WCD9340_General_cal.acdb
MM_AUDIO += Liquid_WCD9340_Global_cal.acdb
MM_AUDIO += Liquid_WCD9340_Handset_cal.acdb
MM_AUDIO += Liquid_WCD9340_Hdmi_cal.acdb
MM_AUDIO += Liquid_WCD9340_Headset_cal.acdb
MM_AUDIO += Liquid_WCD9340_Speaker_cal.acdb
MM_AUDIO += Liquid_WCD9340_workspaceFile.qwsp
MM_AUDIO += Fluid_Bluetooth_cal.acdb
MM_AUDIO += Fluid_General_cal.acdb
MM_AUDIO += Fluid_Global_cal.acdb
MM_AUDIO += Fluid_Handset_cal.acdb
MM_AUDIO += Fluid_Hdmi_cal.acdb
MM_AUDIO += Fluid_Headset_cal.acdb
MM_AUDIO += Fluid_Speaker_cal.acdb
MM_AUDIO += Fluid_workspaceFile.qwsp
MM_AUDIO += Fluid_WCD9340_Bluetooth_cal.acdb
MM_AUDIO += Fluid_WCD9340_Codec_cal.acdb
MM_AUDIO += Fluid_WCD9340_General_cal.acdb
MM_AUDIO += Fluid_WCD9340_Global_cal.acdb
MM_AUDIO += Fluid_WCD9340_Handset_cal.acdb
MM_AUDIO += Fluid_WCD9340_Hdmi_cal.acdb
MM_AUDIO += Fluid_WCD9340_Headset_cal.acdb
MM_AUDIO += Fluid_WCD9340_Speaker_cal.acdb
MM_AUDIO += Fluid_WCD9340_workspaceFile.qwsp
MM_AUDIO += libaudioalsa
MM_AUDIO += libaudioeq
MM_AUDIO += libaudioparsers
MM_AUDIO += libcsd-client
MM_AUDIO += lib_iec_60958_61937
MM_AUDIO += libmm-audio-resampler
MM_AUDIO += libstagefright_soft_qtiflacdec
MM_AUDIO += libOmxAacDec
MM_AUDIO += libOmxAacEnc
MM_AUDIO += libOmxAdpcmDec
MM_AUDIO += libOmxAmrDec
MM_AUDIO += libOmxAmrEnc
MM_AUDIO += libOmxAmrRtpDec
MM_AUDIO += libOmxAmrwbDec
MM_AUDIO += libOmxAmrwbplusDec
MM_AUDIO += libOmxEvrcDec
MM_AUDIO += libOmxEvrcEnc
MM_AUDIO += libOmxEvrcHwDec
MM_AUDIO += libOmxMp3Dec
MM_AUDIO += libOmxQcelp13Dec
MM_AUDIO += libOmxQcelp13Enc
MM_AUDIO += libOmxQcelpHwDec
MM_AUDIO += libOmxVp8Dec
MM_AUDIO += libOmxWmaDec
MM_AUDIO += libOmxAlacDec
MM_AUDIO += libOmxApeDec
MM_AUDIO += libOmxG711Enc
MM_AUDIO += libOmxG711Dec
MM_AUDIO += libOmxAlacDecSw
MM_AUDIO += libOmxApeDecSw
MM_AUDIO += libOmxMpeghEncSw
MM_AUDIO += libOmxDsdDec
MM_AUDIO += libvoem-jni
MM_AUDIO += libvoem-test
MM_AUDIO += mm-audio-ctrl-test
MM_AUDIO += mm-audio-voem_if-test
MM_AUDIO += mm-omx-devmgr
MM_AUDIO += QCAudioManager
MM_AUDIO += liblistensoundmodel
MM_AUDIO += liblistensoundmodel2
MM_AUDIO += liblistenjni
MM_AUDIO += liblisten
MM_AUDIO += liblistenhardware
MM_AUDIO += lns_test
MM_AUDIO += STApp
MM_AUDIO += libqtigef
MM_AUDIO += libqcbassboost
MM_AUDIO += libqcvirt
MM_AUDIO += libqcreverb
MM_AUDIO += libasphere
MM_AUDIO += audiosphere
MM_AUDIO += audiosphere.xml
MM_AUDIO += audio_effects.conf
MM_AUDIO += ftm_test_config
MM_AUDIO += ftm_test_config_mtp
MM_AUDIO += ftm_test_config_wcd9335
MM_AUDIO += ftm_test_config_sdm439-sku1-snd-card
MM_AUDIO += ftm_test_config_msm8953-sku3-tasha-snd-card
MM_AUDIO += ftm_test_config_msm8953-sku4-snd-card
MM_AUDIO += ftm_test_config_wcd9306
MM_AUDIO += ftm_test_config_msm8996-dtp-tasha-snd-card
MM_AUDIO += ftm_test_config_msm8998-tasha-snd-card
MM_AUDIO += ftm_test_config_sdm710-tasha-snd-card
MM_AUDIO += ftm_test_config_sdm710-tavil-snd-card
MM_AUDIO += ftm_test_config_sdm670-skuw-snd-card
MM_AUDIO += libFlacSwDec
MM_AUDIO += libAlacSwDec
MM_AUDIO += libApeSwDec
MM_AUDIO += libMpeghSwEnc
MM_AUDIO += libdsd2pcm
MM_AUDIO += audioflacapp
MM_AUDIO += libqct_resampler
MM_AUDIO += libaudiodevarb
MM_AUDIO += audiod
MM_AUDIO += libsmwrapper
MM_AUDIO += libadpcmdec
MM_AUDIO += libmulawdec
MM_AUDIO += sound_trigger.primary.$(TARGET_BOARD_PLATFORM)
MM_AUDIO += libqtisoundtriggerhw
MM_AUDIO += qti_sound_trigger_test
ifeq ($(strip $(COMPILE_HAL_TEST_APPS_IN_VENDOR_IMAGE)),true)
MM_AUDIO += libqtisoundtriggerwrapper
MM_AUDIO += libqahw
MM_AUDIO += libqahwwrapper
MM_AUDIO += hal_play_test
MM_AUDIO += hal_rec_test
endif
MM_AUDIO += libffv
MM_AUDIO += libnative_audio_latency_jni
MM_AUDIO += AudioLatencyTest
MM_AUDIO += libhwdaphal
MM_AUDIO += libqcomvisualizer
MM_AUDIO += libqcomvoiceprocessing
MM_AUDIO += libqcompostprocbundle
MM_AUDIO += libqvop-service
MM_AUDIO += qvop-daemon
MM_AUDIO += vendor.qti.voiceprint@1.0
MM_AUDIO += VoicePrintSDK
MM_AUDIO += VoicePrintDemo
MM_AUDIO += libadm
MM_AUDIO += libsurround_3mic_proc
MM_AUDIO += surround_sound_rec_AZ.cfg
MM_AUDIO += surround_sound_rec_5.1.cfg
MM_AUDIO += libdrc
MM_AUDIO += drc_cfg_AZ.txt
MM_AUDIO += drc_cfg_5.1.txt
MM_AUDIO += libgcs-osal
MM_AUDIO += libgcs-calwrapper
MM_AUDIO += libgcs-ipc
MM_AUDIO += libgcs
MM_AUDIO += cmudict.bin
MM_AUDIO += poc_64_hmm.gmm
MM_AUDIO += noisesample.bin
MM_AUDIO += antispoofing.bin
MM_AUDIO += libshoebox
MM_AUDIO += libvr_amb_engine
MM_AUDIO += libvr_object_engine
MM_AUDIO += libvr_sam_wrapper
MM_AUDIO += libvraudio
MM_AUDIO += libvraudio_client
MM_AUDIO += libdolby_ms12_wrapper
MM_AUDIO += ms12_wrapper_test
MM_AUDIO += silence.ac3
MM_AUDIO += libaudio_ip_handler
ifeq ($(TARGET_BOARD_AUTO),true)
MM_AUDIO += adsp_avs_config.acdb
MM_AUDIO += Bluetooth_cal.acdb
MM_AUDIO += Codec_cal.acdb
MM_AUDIO += General_cal.acdb
MM_AUDIO += Global_cal.acdb
MM_AUDIO += Handset_cal.acdb
MM_AUDIO += Hdmi_cal.acdb
MM_AUDIO += Headset_cal.acdb
MM_AUDIO += Speaker_cal.acdb

MM_AUDIO += libaudiohalplugin
MM_AUDIO += libcdcdriver
MM_AUDIO += libvad
MM_AUDIO += capi_v2_bmt
MM_AUDIO += capi_v2_fnb
MM_AUDIO += capi_v2_loud
MM_AUDIO += capi_v2_peq
MM_AUDIO += capi_v2_sumx
MM_AUDIO += icc_module.so.1
MM_AUDIO += sec_module.so.1
MM_AUDIO += audio-nxp-auto

MM_AUDIO += vendor.qti.hardware.automotive.audiocontrol@1.0-service
MM_AUDIO += vendor.qti.hardware.automotive.audiocontrol@1.0-service.rc
endif

ifeq ($(ENABLE_HYP), true)
MM_AUDIO += amfsservice
endif

ifeq ($(TARGET_BOARD_AUTO),true)
MM_AVINPUT := libmmavinput
MM_AVINPUT += libsideband
endif

ifeq ($(ENABLE_HYP), true)
MM_HYP_VIDEO := libhyp_video_fe
MM_HYP_VIDEO += hyp_dvr_be
MM_HYP_VIDEO += vendor.qti.dvr@1.0
MM_HYP_VIDEO += vendor.qti.dvr@1.0-service
endif

ifeq ($(ENABLE_HYP), true)
MM_UHAB := libuhab
MM_BUFFERQ := libbufferq
MM_BUFFERQ += libfenceq
endif
#MM_CAMERA

MM_CAMERA := cpp_firmware_v1_1_1.fw
MM_CAMERA += cpp_firmware_v1_1_6.fw
MM_CAMERA += cpp_firmware_v1_2_0.fw
MM_CAMERA += cpp_firmware_v1_2_A.fw
MM_CAMERA += cpp_firmware_v1_6_0.fw
MM_CAMERA += cpp_firmware_v1_4_0.fw
MM_CAMERA += cpp_firmware_v1_5_0.fw
MM_CAMERA += cpp_firmware_v1_5_1.fw
MM_CAMERA += cpp_firmware_v1_5_2.fw
MM_CAMERA += cpp_firmware_v1_8_0.fw
MM_CAMERA += cpp_firmware_v1_10_0.fw
MM_CAMERA += cpp_firmware_v1_12_0.fw
MM_CAMERA += libflash_pmic
MM_CAMERA += libactuator_lc898217xc
MM_CAMERA += libactuator_ad5816g
MM_CAMERA += libactuator_a3907
MM_CAMERA += libactuator_a3907_camcorder
MM_CAMERA += libactuator_a3907_camera
MM_CAMERA += libactuator_dw9714
MM_CAMERA += libactuator_dw9714_camcorder
MM_CAMERA += libactuator_dw9714_camera
MM_CAMERA += libactuator_dw9714_q13v04b
MM_CAMERA += libactuator_dw9714_q13v04b_camcorder
MM_CAMERA += libactuator_dw9714_q13v04b_camera
MM_CAMERA += libactuator_dw9714_q13n04a
MM_CAMERA += libactuator_dw9714_q13n04a_camcorder
MM_CAMERA += libactuator_dw9714_q13n04a_camera
MM_CAMERA += libactuator_dw9714_q8v19w
MM_CAMERA += libactuator_dw9714_q8v19w_camcorder
MM_CAMERA += libactuator_dw9714_q8v19w_camera
MM_CAMERA += libactuator_dw9716
MM_CAMERA += libactuator_dw9716_camcorder
MM_CAMERA += libactuator_dw9716_camera
MM_CAMERA += libactuator_dw9790a
MM_CAMERA += libactuator_lc898122
MM_CAMERA += libactuator_dw9716
MM_CAMERA += libactuator_dw9716_camcorder
MM_CAMERA += libactuator_dw9716_camera
MM_CAMERA += libactuator_dw9790a
MM_CAMERA += libactuator_lc898122
MM_CAMERA += libactuator_lc898122_camcorder
MM_CAMERA += libactuator_lc898122_camera
MM_CAMERA += libactuator_lc898212xd
MM_CAMERA += libactuator_lc898212xd_camcorder
MM_CAMERA += libactuator_lc898212xd_camera
MM_CAMERA += libactuator_lc898212xd_qc2002
MM_CAMERA += libactuator_ak7371
MM_CAMERA += libactuator_ak7345
MM_CAMERA += libois_lc898122
MM_CAMERA += libactuator_iu074
MM_CAMERA += libactuator_iu074_camcorder
MM_CAMERA += libactuator_iu074_camera
MM_CAMERA += libactuator_ov12830
MM_CAMERA += libactuator_ov12830_camcorder
MM_CAMERA += libactuator_ov12830_camera
MM_CAMERA += libactuator_ov8825
MM_CAMERA += libactuator_ov8825_camcorder
MM_CAMERA += libactuator_ov8825_camera
MM_CAMERA += libactuator_rohm_bu64243gwz
MM_CAMERA += libactuator_rohm_bu64243gwz_camcorder
MM_CAMERA += libactuator_rohm_bu64243gwz_camera
MM_CAMERA += libactuator_ad5823
MM_CAMERA += libactuator_ad5823_camcorder
MM_CAMERA += libactuator_ad5823_camera
MM_CAMERA += libactuator_bu64297
MM_CAMERA += libactuator_bu64297_f13v10l
MM_CAMERA += libadsp_denoise_skel
MM_CAMERA += libadsp_hvx_add_constant
MM_CAMERA += libadsp_hvx_add_constant.so
MM_CAMERA += libadsp_hvx_stats
MM_CAMERA += libadsp_hvx_stats.so
MM_CAMERA += libadsp_hvx_callback_skel
MM_CAMERA += libadsp_hvx_stub
MM_CAMERA += libadsp_hvx_skel
MM_CAMERA += libadsp_hvx_skel.so
MM_CAMERA += libadsp_fd_skel
MM_CAMERA += libadsp_hvx_zzhdr_BGGR
MM_CAMERA += libadsp_hvx_zzhdr_BGGR.so
MM_CAMERA += libadsp_hvx_zzhdr_RGGB
MM_CAMERA += libadsp_hvx_zzhdr_RGGB.so
MM_CAMERA += libactuator_dw9714_camera
MM_CAMERA += libactuator_dw9714_camcorder
MM_CAMERA += libactuator_dw9716_camera
MM_CAMERA += libactuator_dw9716_camcorder
MM_CAMERA += libactuator_ov8825_camera
MM_CAMERA += libactuator_ov8825_camcorder
MM_CAMERA += libactuator_ROHM_BU64243GWZ_camera
MM_CAMERA += libactuator_ROHM_BU64243GWZ_camcorder
MM_CAMERA += libactuator_ov12830_camera
MM_CAMERA += libactuator_ov12830_camcorder
MM_CAMERA += libactuator_iu074_camera
MM_CAMERA += libactuator_iu074_camcorder
MM_CAMERA += libactuator_bu64244gwz
MM_CAMERA += libactuator_pseudo
MM_CAMERA += libchromatix_imx214_4k_preview_lc898122
MM_CAMERA += libchromatix_imx214_4k_video_lc898122
MM_CAMERA += libchromatix_imx214_common
MM_CAMERA += libchromatix_imx214_cpp_hfr_120
MM_CAMERA += libchromatix_imx214_cpp_hfr_60
MM_CAMERA += libchromatix_imx214_cpp_hfr_90
MM_CAMERA += libchromatix_imx214_cpp_liveshot
MM_CAMERA += libchromatix_imx214_cpp_preview
MM_CAMERA += libchromatix_imx214_cpp_snapshot
MM_CAMERA += libchromatix_imx214_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx214_cpp_video
MM_CAMERA += libchromatix_imx214_cpp_video_hdr
MM_CAMERA += libchromatix_imx214_default_preview_lc898122
MM_CAMERA += libchromatix_imx214_default_video
MM_CAMERA += libchromatix_imx214_default_video_lc898122
MM_CAMERA += libchromatix_imx214_hfr_120
MM_CAMERA += libchromatix_imx214_hfr_120_lc898122
MM_CAMERA += libchromatix_imx214_hfr_60
MM_CAMERA += libchromatix_imx214_hfr_60_lc898122
MM_CAMERA += libchromatix_imx214_hfr_90
MM_CAMERA += libchromatix_imx214_hfr_90_lc898122
MM_CAMERA += libchromatix_imx214_liveshot
MM_CAMERA += libchromatix_imx214_postproc
MM_CAMERA += libchromatix_imx214_preview
MM_CAMERA += libchromatix_imx214_snapshot
MM_CAMERA += libchromatix_imx214_snapshot_hdr
MM_CAMERA += libchromatix_imx214_snapshot_hdr_lc898122
MM_CAMERA += libchromatix_imx214_video_hdr
MM_CAMERA += libchromatix_imx214_video_hdr_lc898122
MM_CAMERA += libchromatix_imx214_zsl_preview_lc898122
MM_CAMERA += libchromatix_imx214_zsl_video_lc898122
MM_CAMERA += libchromatix_imx230_1080p_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_1080p_video_lc898212xd
MM_CAMERA += libchromatix_imx230_4k_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_4k_video_lc898212xd
MM_CAMERA += libchromatix_imx230_common
MM_CAMERA += libchromatix_imx230_cpp_hfr_120
MM_CAMERA += libchromatix_imx230_cpp_hfr_240
MM_CAMERA += libchromatix_imx230_cpp_hfr_60
MM_CAMERA += libchromatix_imx230_cpp_hfr_90
MM_CAMERA += libchromatix_imx230_cpp_liveshot
MM_CAMERA += libchromatix_imx230_cpp_preview
MM_CAMERA += libchromatix_imx230_cpp_snapshot
MM_CAMERA += libchromatix_imx230_cpp_snapshot_downscale
MM_CAMERA += libchromatix_imx230_cpp_snapshot_upscale
MM_CAMERA += libchromatix_imx230_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx230_cpp_video
MM_CAMERA += libchromatix_imx230_cpp_video_4k
MM_CAMERA += libchromatix_imx230_cpp_video_16M
MM_CAMERA += libchromatix_imx230_cpp_video_hdr
MM_CAMERA += libchromatix_imx230_default_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_default_video
MM_CAMERA += libchromatix_imx230_default_video_lc898212xd
MM_CAMERA += libchromatix_imx230_hdr_snapshot_lc898212xd
MM_CAMERA += libchromatix_imx230_hdr_video_lc898212xd
MM_CAMERA += libchromatix_imx230_hfr_120
MM_CAMERA += libchromatix_imx230_hfr_120_lc898212xd
MM_CAMERA += libchromatix_imx230_hfr_240
MM_CAMERA += libchromatix_imx230_hfr_240_lc898212xd
MM_CAMERA += libchromatix_imx230_hfr_60
MM_CAMERA += libchromatix_imx230_hfr_60_lc898212xd
MM_CAMERA += libchromatix_imx230_hfr_90
MM_CAMERA += libchromatix_imx230_hfr_90_lc898212xd
MM_CAMERA += libchromatix_imx230_liveshot
MM_CAMERA += libchromatix_imx230_postproc
MM_CAMERA += libchromatix_imx230_preview
MM_CAMERA += libchromatix_imx230_snapshot
MM_CAMERA += libchromatix_imx230_snapshot_hdr
MM_CAMERA += libchromatix_imx230_video_4k
MM_CAMERA += libchromatix_imx230_video_16M
MM_CAMERA += libchromatix_imx230_video_16M_lc898212xd
MM_CAMERA += libchromatix_imx230_video_hdr
MM_CAMERA += libchromatix_imx230_zsl_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_zsl_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_1080p_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_1080p_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_4k_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_4k_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_common
MM_CAMERA += libchromatix_imx230_qc2002_cpp_hfr_120
MM_CAMERA += libchromatix_imx230_qc2002_cpp_hfr_240
MM_CAMERA += libchromatix_imx230_qc2002_cpp_hfr_60
MM_CAMERA += libchromatix_imx230_qc2002_cpp_hfr_90
MM_CAMERA += libchromatix_imx230_qc2002_cpp_liveshot
MM_CAMERA += libchromatix_imx230_qc2002_cpp_preview
MM_CAMERA += libchromatix_imx230_qc2002_cpp_snapshot
MM_CAMERA += libchromatix_imx230_qc2002_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx230_qc2002_cpp_video
MM_CAMERA += libchromatix_imx230_qc2002_cpp_video_4k
MM_CAMERA += libchromatix_imx230_qc2002_cpp_video_16M
MM_CAMERA += libchromatix_imx230_qc2002_cpp_video_hdr
MM_CAMERA += libchromatix_imx230_qc2002_default_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_default_video
MM_CAMERA += libchromatix_imx230_qc2002_default_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_hdr_snapshot_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_hdr_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_hfr_120
MM_CAMERA += libchromatix_imx230_qc2002_hfr_120_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_hfr_240
MM_CAMERA += libchromatix_imx230_qc2002_hfr_240_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_hfr_60
MM_CAMERA += libchromatix_imx230_qc2002_hfr_60_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_hfr_90
MM_CAMERA += libchromatix_imx230_qc2002_hfr_90_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_liveshot
MM_CAMERA += libchromatix_imx230_qc2002_postproc
MM_CAMERA += libchromatix_imx230_qc2002_preview
MM_CAMERA += libchromatix_imx230_qc2002_snapshot
MM_CAMERA += libchromatix_imx230_qc2002_snapshot_hdr
MM_CAMERA += libchromatix_imx230_qc2002_video_4k
MM_CAMERA += libchromatix_imx230_qc2002_video_16M
MM_CAMERA += libchromatix_imx230_qc2002_video_16M_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_video_hdr
MM_CAMERA += libchromatix_imx230_qc2002_zsl_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_zsl_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_1080p_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_1080p_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_4k_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_4k_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_common
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_hfr_120
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_hfr_240
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_hfr_60
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_hfr_90
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_liveshot
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_preview
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_snapshot
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_video
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_video_4k
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_video_16M
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_cpp_video_hdr
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_default_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_default_video
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_default_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hdr_snapshot_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hdr_video_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hfr_120
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hfr_120_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hfr_240
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hfr_240_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hfr_60
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hfr_60_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hfr_90
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_hfr_90_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_liveshot
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_postproc
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_preview
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_snapshot
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_snapshot_hdr
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_video_4k
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_video_16M
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_video_16M_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_video_hdr
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_zsl_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_qc2002_with_gyro_zsl_video_lc898212xd
MM_CAMERA += libchromatix_imx230_cpp_raw_hdr
MM_CAMERA += libchromatix_imx230_raw_hdr
MM_CAMERA += libchromatix_imx230_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx298_4K_preview
MM_CAMERA += libchromatix_imx298_4K_video
MM_CAMERA += libchromatix_imx298_common
MM_CAMERA += libchromatix_imx298_cpp_hfr_120
MM_CAMERA += libchromatix_imx298_cpp_hfr_60
MM_CAMERA += libchromatix_imx298_cpp_hfr_90
MM_CAMERA += libchromatix_imx298_cpp_liveshot
MM_CAMERA += libchromatix_imx298_cpp_preview
MM_CAMERA += libchromatix_imx298_cpp_snapshot
MM_CAMERA += libchromatix_imx298_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx298_cpp_video
MM_CAMERA += libchromatix_imx298_cpp_video_hdr
MM_CAMERA += libchromatix_imx298_default_preview
MM_CAMERA += libchromatix_imx298_default_video
MM_CAMERA += libchromatix_imx298_hdr_snapshot_3a
MM_CAMERA += libchromatix_imx298_hdr_video_3a
MM_CAMERA += libchromatix_imx298_hfr_120
MM_CAMERA += libchromatix_imx298_hfr_120_3a
MM_CAMERA += libchromatix_imx298_hfr_60
MM_CAMERA += libchromatix_imx298_hfr_60_3a
MM_CAMERA += libchromatix_imx298_hfr_90
MM_CAMERA += libchromatix_imx298_hfr_90_3a
MM_CAMERA += libchromatix_imx298_liveshot
MM_CAMERA += libchromatix_imx298_postproc
MM_CAMERA += libchromatix_imx298_preview
MM_CAMERA += libchromatix_imx298_snapshot
MM_CAMERA += libchromatix_imx298_snapshot_hdr
MM_CAMERA += libchromatix_imx298_video
MM_CAMERA += libchromatix_imx298_video_hdr
MM_CAMERA += libchromatix_imx298_zsl_preview
MM_CAMERA += libchromatix_imx298_zsl_video
MM_CAMERA += libchromatix_imx298_gt24c64_4K_preview
MM_CAMERA += libchromatix_imx298_gt24c64_4K_video
MM_CAMERA += libchromatix_imx298_gt24c64_common
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_hfr_120
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_hfr_60
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_hfr_90
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_liveshot
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_preview
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_snapshot
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_video
MM_CAMERA += libchromatix_imx298_gt24c64_cpp_video_hdr
MM_CAMERA += libchromatix_imx298_gt24c64_default_preview
MM_CAMERA += libchromatix_imx298_gt24c64_default_video
MM_CAMERA += libchromatix_imx298_gt24c64_hdr_snapshot_3a
MM_CAMERA += libchromatix_imx298_gt24c64_hdr_video_3a
MM_CAMERA += libchromatix_imx298_gt24c64_hfr_120
MM_CAMERA += libchromatix_imx298_gt24c64_hfr_120_3a
MM_CAMERA += libchromatix_imx298_gt24c64_hfr_60
MM_CAMERA += libchromatix_imx298_gt24c64_hfr_60_3a
MM_CAMERA += libchromatix_imx298_gt24c64_hfr_90
MM_CAMERA += libchromatix_imx298_gt24c64_hfr_90_3a
MM_CAMERA += libchromatix_imx298_gt24c64_liveshot
MM_CAMERA += libchromatix_imx298_gt24c64_postproc
MM_CAMERA += libchromatix_imx298_gt24c64_preview
MM_CAMERA += libchromatix_imx298_gt24c64_snapshot
MM_CAMERA += libchromatix_imx298_gt24c64_snapshot_hdr
MM_CAMERA += libchromatix_imx298_gt24c64_video
MM_CAMERA += libchromatix_imx298_gt24c64_video_hdr
MM_CAMERA += libchromatix_imx298_gt24c64_zsl_preview
MM_CAMERA += libchromatix_imx298_gt24c64_zsl_video
MM_CAMERA += libchromatix_imx258_common
MM_CAMERA += libchromatix_imx258_cpp_hfr_60
MM_CAMERA += libchromatix_imx258_cpp_hfr_90
MM_CAMERA += libchromatix_imx258_cpp_hfr_120
MM_CAMERA += libchromatix_imx258_cpp_liveshot
MM_CAMERA += libchromatix_imx258_cpp_preview
MM_CAMERA += libchromatix_imx258_cpp_snapshot
MM_CAMERA += libchromatix_imx258_cpp_video
MM_CAMERA += libchromatix_imx258_cpp_video_4k
MM_CAMERA += libchromatix_imx258_default_video
MM_CAMERA += libchromatix_imx258_hfr_60
MM_CAMERA += libchromatix_imx258_hfr_90
MM_CAMERA += libchromatix_imx258_hfr_120
MM_CAMERA += libchromatix_imx258_liveshot
MM_CAMERA += libchromatix_imx258_postproc
MM_CAMERA += libchromatix_imx258_preview
MM_CAMERA += libchromatix_imx258_snapshot
MM_CAMERA += libchromatix_imx258_video_4k
MM_CAMERA += libchromatix_imx258_4k_preview_3a
MM_CAMERA += libchromatix_imx258_4k_video_3a
MM_CAMERA += libchromatix_imx258_default_preview_3a
MM_CAMERA += libchromatix_imx258_default_video_3a
MM_CAMERA += libchromatix_imx258_hfr_60_3a
MM_CAMERA += libchromatix_imx258_hfr_90_3a
MM_CAMERA += libchromatix_imx258_hfr_120_3a
MM_CAMERA += libchromatix_imx258_zsl_preview_3a
MM_CAMERA += libchromatix_imx258_zsl_video_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_common
MM_CAMERA += libchromatix_imx258_lc898217xc_cpp_hfr_60
MM_CAMERA += libchromatix_imx258_lc898217xc_cpp_hfr_90
MM_CAMERA += libchromatix_imx258_lc898217xc_cpp_hfr_120
MM_CAMERA += libchromatix_imx258_lc898217xc_cpp_liveshot
MM_CAMERA += libchromatix_imx258_lc898217xc_cpp_preview
MM_CAMERA += libchromatix_imx258_lc898217xc_cpp_snapshot
MM_CAMERA += libchromatix_imx258_lc898217xc_cpp_video
MM_CAMERA += libchromatix_imx258_lc898217xc_cpp_video_4k
MM_CAMERA += libchromatix_imx258_lc898217xc_default_video
MM_CAMERA += libchromatix_imx258_lc898217xc_hfr_60
MM_CAMERA += libchromatix_imx258_lc898217xc_hfr_90
MM_CAMERA += libchromatix_imx258_lc898217xc_hfr_120
MM_CAMERA += libchromatix_imx258_lc898217xc_liveshot
MM_CAMERA += libchromatix_imx258_lc898217xc_postproc
MM_CAMERA += libchromatix_imx258_lc898217xc_preview
MM_CAMERA += libchromatix_imx258_lc898217xc_snapshot
MM_CAMERA += libchromatix_imx258_lc898217xc_video_4k
MM_CAMERA += libchromatix_imx258_lc898217xc_4k_preview_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_4k_video_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_default_preview_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_default_video_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_hfr_60_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_hfr_90_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_hfr_120_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_zsl_preview_3a
MM_CAMERA += libchromatix_imx258_lc898217xc_zsl_video_3a
MM_CAMERA += libchromatix_imx258_bear_common
MM_CAMERA += libchromatix_imx258_bear_cpp_hfr_60
MM_CAMERA += libchromatix_imx258_bear_cpp_hfr_90
MM_CAMERA += libchromatix_imx258_bear_cpp_hfr_120
MM_CAMERA += libchromatix_imx258_bear_cpp_liveshot
MM_CAMERA += libchromatix_imx258_bear_cpp_preview
MM_CAMERA += libchromatix_imx258_bear_cpp_snapshot
MM_CAMERA += libchromatix_imx258_bear_cpp_video
MM_CAMERA += libchromatix_imx258_bear_cpp_video_4k
MM_CAMERA += libchromatix_imx258_bear_default_video
MM_CAMERA += libchromatix_imx258_bear_hfr_60
MM_CAMERA += libchromatix_imx258_bear_hfr_90
MM_CAMERA += libchromatix_imx258_bear_hfr_120
MM_CAMERA += libchromatix_imx258_bear_liveshot
MM_CAMERA += libchromatix_imx258_bear_postproc
MM_CAMERA += libchromatix_imx258_bear_preview
MM_CAMERA += libchromatix_imx258_bear_snapshot
MM_CAMERA += libchromatix_imx258_bear_video_4k
MM_CAMERA += libchromatix_imx258_bear_4k_preview_lc898217xc
MM_CAMERA += libchromatix_imx258_bear_4k_video_lc898217xc
MM_CAMERA += libchromatix_imx258_bear_default_preview_lc898217xc
MM_CAMERA += libchromatix_imx258_bear_default_video_lc898217xc
MM_CAMERA += libchromatix_imx258_bear_hfr_60_lc898217xc
MM_CAMERA += libchromatix_imx258_bear_hfr_90_lc898217xc
MM_CAMERA += libchromatix_imx258_bear_hfr_120_lc898217xc
MM_CAMERA += libchromatix_imx258_bear_zsl_preview_lc898217xc
MM_CAMERA += libchromatix_imx258_bear_zsl_video_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_common
MM_CAMERA += libchromatix_imx258_mono_bear_cpp_hfr_60
MM_CAMERA += libchromatix_imx258_mono_bear_cpp_hfr_90
MM_CAMERA += libchromatix_imx258_mono_bear_cpp_hfr_120
MM_CAMERA += libchromatix_imx258_mono_bear_cpp_liveshot
MM_CAMERA += libchromatix_imx258_mono_bear_cpp_preview
MM_CAMERA += libchromatix_imx258_mono_bear_cpp_snapshot
MM_CAMERA += libchromatix_imx258_mono_bear_cpp_video
MM_CAMERA += libchromatix_imx258_mono_bear_cpp_video_4k
MM_CAMERA += libchromatix_imx258_mono_bear_default_video
MM_CAMERA += libchromatix_imx258_mono_bear_hfr_60
MM_CAMERA += libchromatix_imx258_mono_bear_hfr_90
MM_CAMERA += libchromatix_imx258_mono_bear_hfr_120
MM_CAMERA += libchromatix_imx258_mono_bear_liveshot
MM_CAMERA += libchromatix_imx258_mono_bear_postproc
MM_CAMERA += libchromatix_imx258_mono_bear_preview
MM_CAMERA += libchromatix_imx258_mono_bear_snapshot
MM_CAMERA += libchromatix_imx258_mono_bear_video_4k
MM_CAMERA += libchromatix_imx258_mono_bear_4k_preview_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_4k_video_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_default_preview_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_default_video_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_hfr_60_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_hfr_90_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_hfr_120_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_zsl_preview_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_bear_zsl_video_lc898217xc
MM_CAMERA += libchromatix_imx258_mono_common
MM_CAMERA += libchromatix_imx258_mono_cpp_hfr_60
MM_CAMERA += libchromatix_imx258_mono_cpp_hfr_90
MM_CAMERA += libchromatix_imx258_mono_cpp_hfr_120
MM_CAMERA += libchromatix_imx258_mono_cpp_liveshot
MM_CAMERA += libchromatix_imx258_mono_cpp_preview
MM_CAMERA += libchromatix_imx258_mono_cpp_snapshot
MM_CAMERA += libchromatix_imx258_mono_cpp_video
MM_CAMERA += libchromatix_imx258_mono_cpp_video_4k
MM_CAMERA += libchromatix_imx258_mono_default_video
MM_CAMERA += libchromatix_imx258_mono_hfr_60
MM_CAMERA += libchromatix_imx258_mono_hfr_90
MM_CAMERA += libchromatix_imx258_mono_hfr_120
MM_CAMERA += libchromatix_imx258_mono_liveshot
MM_CAMERA += libchromatix_imx258_mono_postproc
MM_CAMERA += libchromatix_imx258_mono_preview
MM_CAMERA += libchromatix_imx258_mono_snapshot
MM_CAMERA += libchromatix_imx258_mono_video_4k
MM_CAMERA += libchromatix_imx258_mono_4k_preview_3a
MM_CAMERA += libchromatix_imx258_mono_4k_video_3a
MM_CAMERA += libchromatix_imx258_mono_default_preview_3a
MM_CAMERA += libchromatix_imx258_mono_default_video_3a
MM_CAMERA += libchromatix_imx258_mono_hfr_60_3a
MM_CAMERA += libchromatix_imx258_mono_hfr_90_3a
MM_CAMERA += libchromatix_imx258_mono_hfr_120_3a
MM_CAMERA += libchromatix_imx258_mono_zsl_preview_3a
MM_CAMERA += libchromatix_imx258_mono_zsl_video_3a
MM_CAMERA += libchromatix_imx318_1080p_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_1080p_video_lc898212xd
MM_CAMERA += libchromatix_imx318_4k_12M_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_4k_12M_video_lc898212xd
MM_CAMERA += libchromatix_imx318_4k_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_4k_video_lc898212xd
MM_CAMERA += libchromatix_imx318_common
MM_CAMERA += libchromatix_imx318_cpp_hfr_120
MM_CAMERA += libchromatix_imx318_cpp_hfr_240
MM_CAMERA += libchromatix_imx318_cpp_hfr_60
MM_CAMERA += libchromatix_imx318_cpp_liveshot
MM_CAMERA += libchromatix_imx318_cpp_preview
MM_CAMERA += libchromatix_imx318_cpp_snapshot
MM_CAMERA += libchromatix_imx318_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx318_cpp_snapshot_raw_hdr
MM_CAMERA += libchromatix_imx318_cpp_video
MM_CAMERA += libchromatix_imx318_cpp_video_4k
MM_CAMERA += libchromatix_imx318_cpp_video_hdr
MM_CAMERA += libchromatix_imx318_cpp_video_4k_raw_hdr
MM_CAMERA += libchromatix_imx318_default_video
MM_CAMERA += libchromatix_imx318_qtrsize_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_qtrsize_video_lc898212xd
MM_CAMERA += libchromatix_imx318_fullsize_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_fullsize_video_lc898212xd
MM_CAMERA += libchromatix_imx318_hdr_snapshot_lc898212xd
MM_CAMERA += libchromatix_imx318_hdr_video_lc898212xd
MM_CAMERA += libchromatix_imx318_snapshot_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx318_full_snapshot_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx318_video_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx318_video_12M_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx318_hfr_120
MM_CAMERA += libchromatix_imx318_hfr_120_lc898212xd
MM_CAMERA += libchromatix_imx318_hfr_240
MM_CAMERA += libchromatix_imx318_hfr_240_lc898212xd
MM_CAMERA += libchromatix_imx318_hfr_60
MM_CAMERA += libchromatix_imx318_hfr_60_lc898212xd
MM_CAMERA += libchromatix_imx318_liveshot
MM_CAMERA += libchromatix_imx318_postproc
MM_CAMERA += libchromatix_imx318_preview
MM_CAMERA += libchromatix_imx318_snapshot
MM_CAMERA += libchromatix_imx318_snapshot_hdr
MM_CAMERA += libchromatix_imx318_snapshot_raw_hdr
MM_CAMERA += libchromatix_imx318_video_4k
MM_CAMERA += libchromatix_imx318_video_4k_raw_hdr
MM_CAMERA += libchromatix_imx318_video_hdr
MM_CAMERA += libchromatix_imx074_default_video
MM_CAMERA += libchromatix_imx074_preview
MM_CAMERA += libchromatix_imx074_video_hd
MM_CAMERA += libchromatix_imx074_zsl
MM_CAMERA += libchromatix_imx091_default_video
MM_CAMERA += libchromatix_imx091_preview
MM_CAMERA += libchromatix_imx091_video_hd
MM_CAMERA += libchromatix_imx132_1080p_preview
MM_CAMERA += libchromatix_imx132_1080p_video
MM_CAMERA += libchromatix_imx132_common
MM_CAMERA += libchromatix_imx132_cpp_liveshot
MM_CAMERA += libchromatix_imx132_cpp_preview
MM_CAMERA += libchromatix_imx132_cpp_snapshot
MM_CAMERA += libchromatix_imx132_cpp_video
MM_CAMERA += libchromatix_imx132_default_video
MM_CAMERA += libchromatix_imx132_liveshot
MM_CAMERA += libchromatix_imx132_postproc
MM_CAMERA += libchromatix_imx132_preview
MM_CAMERA += libchromatix_imx132_snapshot
MM_CAMERA += libchromatix_imx132_zsl_preview
MM_CAMERA += libchromatix_imx132_zsl_video
MM_CAMERA += libchromatix_imx134_common
MM_CAMERA += libchromatix_imx134_cpp
MM_CAMERA += libchromatix_imx134_default_video
MM_CAMERA += libchromatix_imx134_hfr_120
MM_CAMERA += libchromatix_imx134_hfr_60
MM_CAMERA += libchromatix_imx134_preview
MM_CAMERA += libchromatix_imx134_snapshot
MM_CAMERA += libchromatix_imx135_1080p_preview_dw9714
MM_CAMERA += libchromatix_imx135_1080p_video_dw9714
MM_CAMERA += libchromatix_imx135_common
MM_CAMERA += libchromatix_imx135_cpp_hfr_120
MM_CAMERA += libchromatix_imx135_cpp_hfr_60
MM_CAMERA += libchromatix_imx135_cpp_hfr_90
MM_CAMERA += libchromatix_imx135_cpp_liveshot
MM_CAMERA += libchromatix_imx135_cpp_preview
MM_CAMERA += libchromatix_imx135_cpp_snapshot
MM_CAMERA += libchromatix_imx135_cpp_video
MM_CAMERA += libchromatix_imx135_cpp_video_hd
MM_CAMERA += libchromatix_imx135_default_preview_dw9714
MM_CAMERA += libchromatix_imx135_default_video
MM_CAMERA += libchromatix_imx135_default_video_dw9714
MM_CAMERA += libchromatix_imx135_hdr_video_dw9714
MM_CAMERA += libchromatix_imx135_hfr_60
MM_CAMERA += libchromatix_imx135_hfr_60_dw9714
MM_CAMERA += libchromatix_imx135_hfr_90
MM_CAMERA += libchromatix_imx135_hfr_90_dw9714
MM_CAMERA += libchromatix_imx135_hfr_120
MM_CAMERA += libchromatix_imx135_hfr_120_dw9714
MM_CAMERA += libchromatix_imx135_liveshot
MM_CAMERA += libchromatix_imx135_postproc
MM_CAMERA += libchromatix_imx135_preview
MM_CAMERA += libchromatix_imx135_snapshot
MM_CAMERA += libchromatix_imx135_video_hd
MM_CAMERA += libchromatix_imx135_zsl
MM_CAMERA += libchromatix_imx135_zsl_preview_dw9714
MM_CAMERA += libchromatix_imx135_zsl_video_dw9714
MM_CAMERA += libchromatix_imx362_1080p_preview_3a
MM_CAMERA += libchromatix_imx362_1080p_video_3a
MM_CAMERA += libchromatix_imx362_4k_preview_3a
MM_CAMERA += libchromatix_imx362_4k_video_3a
MM_CAMERA += libchromatix_imx362_common
MM_CAMERA += libchromatix_imx362_cpp_hfr_120
MM_CAMERA += libchromatix_imx362_cpp_hfr_240
MM_CAMERA += libchromatix_imx362_cpp_hfr_60
MM_CAMERA += libchromatix_imx362_cpp_liveshot
MM_CAMERA += libchromatix_imx362_cpp_preview
MM_CAMERA += libchromatix_imx362_cpp_snapshot
MM_CAMERA += libchromatix_imx362_cpp_snapshot_downscale
MM_CAMERA += libchromatix_imx362_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx362_cpp_snapshot_upscale
MM_CAMERA += libchromatix_imx362_cpp_video
MM_CAMERA += libchromatix_imx362_cpp_video_4k
MM_CAMERA += libchromatix_imx362_cpp_video_hdr
MM_CAMERA += libchromatix_imx362_default_video
MM_CAMERA += libchromatix_imx362_default_preview_3a
MM_CAMERA += libchromatix_imx362_default_video_3a
MM_CAMERA += libchromatix_imx362_fullsize_preview_3a
MM_CAMERA += libchromatix_imx362_fullsize_video_3a
MM_CAMERA += libchromatix_imx362_hdr_snapshot_3a
MM_CAMERA += libchromatix_imx362_hdr_video_3a
MM_CAMERA += libchromatix_imx362_hfr_120
MM_CAMERA += libchromatix_imx362_hfr_120_3a
MM_CAMERA += libchromatix_imx362_hfr_240
MM_CAMERA += libchromatix_imx362_hfr_240_3a
MM_CAMERA += libchromatix_imx362_hfr_60
MM_CAMERA += libchromatix_imx362_hfr_60_3a
MM_CAMERA += libchromatix_imx362_liveshot
MM_CAMERA += libchromatix_imx362_postproc
MM_CAMERA += libchromatix_imx362_preview
MM_CAMERA += libchromatix_imx362_snapshot
MM_CAMERA += libchromatix_imx362_snapshot_hdr
MM_CAMERA += libchromatix_imx362_video_4k
MM_CAMERA += libchromatix_imx362_video_hdr
MM_CAMERA += libchromatix_imx362_1080p_preview_3a_bear
MM_CAMERA += libchromatix_imx362_1080p_video_3a_bear
MM_CAMERA += libchromatix_imx362_4k_preview_3a_bear
MM_CAMERA += libchromatix_imx362_4k_video_3a_bear
MM_CAMERA += libchromatix_imx362_common_bear
MM_CAMERA += libchromatix_imx362_cpp_hfr_120_bear
MM_CAMERA += libchromatix_imx362_cpp_hfr_240_bear
MM_CAMERA += libchromatix_imx362_cpp_hfr_60_bear
MM_CAMERA += libchromatix_imx362_cpp_liveshot_bear
MM_CAMERA += libchromatix_imx362_cpp_preview_bear
MM_CAMERA += libchromatix_imx362_cpp_snapshot_bear
MM_CAMERA += libchromatix_imx362_cpp_video_bear
MM_CAMERA += libchromatix_imx362_cpp_video_4k_bear
MM_CAMERA += libchromatix_imx362_default_video_bear
MM_CAMERA += libchromatix_imx362_default_preview_3a_bear
MM_CAMERA += libchromatix_imx362_default_video_3a_bear
MM_CAMERA += libchromatix_imx362_fullsize_preview_3a_bear
MM_CAMERA += libchromatix_imx362_fullsize_video_3a_bear
MM_CAMERA += libchromatix_imx362_hfr_120_bear
MM_CAMERA += libchromatix_imx362_hfr_120_3a_bear
MM_CAMERA += libchromatix_imx362_hfr_240_bear
MM_CAMERA += libchromatix_imx362_hfr_240_3a_bear
MM_CAMERA += libchromatix_imx362_hfr_60_bear
MM_CAMERA += libchromatix_imx362_hfr_60_3a_bear
MM_CAMERA += libchromatix_imx362_postproc_bear
MM_CAMERA += libchromatix_imx362_preview_bear
MM_CAMERA += libchromatix_imx362_snapshot_bear
MM_CAMERA += libchromatix_imx362_video_4k_bear
MM_CAMERA += libchromatix_imx362_hdr_snapshot_3a_bear
MM_CAMERA += libchromatix_imx362_hdr_video_3a_bear
MM_CAMERA += libchromatix_imx362_cpp_snapshot_hdr_bear
MM_CAMERA += libchromatix_imx362_cpp_video_hdr_bear
MM_CAMERA += libchromatix_imx362_snapshot_hdr_bear
MM_CAMERA += libchromatix_imx362_video_hdr_bear
MM_CAMERA += libchromatix_imx362_cpp_snapshot_downscale_bear
MM_CAMERA += libchromatix_imx362_cpp_snapshot_upscale_bear
MM_CAMERA += libchromatix_imx362_gt24c64a_1080p_preview_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_1080p_video_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_4k_preview_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_4k_video_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_common
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_hfr_120
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_hfr_240
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_hfr_60
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_liveshot
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_preview
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_snapshot
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_snapshot_downscale
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_snapshot_upscale
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_video
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_video_4k
MM_CAMERA += libchromatix_imx362_gt24c64a_cpp_video_hdr
MM_CAMERA += libchromatix_imx362_gt24c64a_default_video
MM_CAMERA += libchromatix_imx362_gt24c64a_default_preview_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_default_video_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_fullsize_preview_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_fullsize_video_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_hdr_snapshot_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_hdr_video_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_hfr_120
MM_CAMERA += libchromatix_imx362_gt24c64a_hfr_120_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_hfr_240
MM_CAMERA += libchromatix_imx362_gt24c64a_hfr_240_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_hfr_60
MM_CAMERA += libchromatix_imx362_gt24c64a_hfr_60_3a
MM_CAMERA += libchromatix_imx362_gt24c64a_liveshot
MM_CAMERA += libchromatix_imx362_gt24c64a_postproc
MM_CAMERA += libchromatix_imx362_gt24c64a_preview
MM_CAMERA += libchromatix_imx362_gt24c64a_snapshot
MM_CAMERA += libchromatix_imx362_gt24c64a_snapshot_hdr
MM_CAMERA += libchromatix_imx362_gt24c64a_video_4k
MM_CAMERA += libchromatix_imx362_gt24c64a_video_hdr
MM_CAMERA += libchromatix_imx376_common
MM_CAMERA += libchromatix_imx376_cpp_liveshot
MM_CAMERA += libchromatix_imx376_cpp_preview
MM_CAMERA += libchromatix_imx376_cpp_snapshot_downscale
MM_CAMERA += libchromatix_imx376_cpp_snapshot
MM_CAMERA += libchromatix_imx376_cpp_snapshot_upscale
MM_CAMERA += libchromatix_imx376_cpp_video_default
MM_CAMERA += libchromatix_imx376_cpp_video
MM_CAMERA += libchromatix_imx376_default_preview_3a
MM_CAMERA += libchromatix_imx376_default_video_3a
MM_CAMERA += libchromatix_imx376_fullsize_preview_3a
MM_CAMERA += libchromatix_imx376_fullsize_video_3a
MM_CAMERA += libchromatix_imx376_postproc
MM_CAMERA += libchromatix_imx376_snapshot
MM_CAMERA += libchromatix_imx376_video_default
MM_CAMERA += libchromatix_imx378_4K_preview
MM_CAMERA += libchromatix_imx378_4K_video
MM_CAMERA += libchromatix_imx378_common
MM_CAMERA += libchromatix_imx378_cpp_hfr_60
MM_CAMERA += libchromatix_imx378_cpp_hfr_90
MM_CAMERA += libchromatix_imx378_cpp_hfr_120
MM_CAMERA += libchromatix_imx378_cpp_hfr_240
MM_CAMERA += libchromatix_imx378_cpp_liveshot
MM_CAMERA += libchromatix_imx378_cpp_preview
MM_CAMERA += libchromatix_imx378_cpp_snapshot
MM_CAMERA += libchromatix_imx378_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx378_cpp_video
MM_CAMERA += libchromatix_imx378_cpp_video_hdr
MM_CAMERA += libchromatix_imx378_default_preview
MM_CAMERA += libchromatix_imx378_default_video
MM_CAMERA += libchromatix_imx378_hdr_snapshot_3a
MM_CAMERA += libchromatix_imx378_hdr_video_3a
MM_CAMERA += libchromatix_imx378_hfr_60
MM_CAMERA += libchromatix_imx378_hfr_60_3a
MM_CAMERA += libchromatix_imx378_hfr_90
MM_CAMERA += libchromatix_imx378_hfr_90_3a
MM_CAMERA += libchromatix_imx378_hfr_120
MM_CAMERA += libchromatix_imx378_hfr_120_3a
MM_CAMERA += libchromatix_imx378_hfr_240
MM_CAMERA += libchromatix_imx378_hfr_240_3a
MM_CAMERA += libchromatix_imx378_liveshot
MM_CAMERA += libchromatix_imx378_postproc
MM_CAMERA += libchromatix_imx378_preview
MM_CAMERA += libchromatix_imx378_snapshot
MM_CAMERA += libchromatix_imx378_snapshot_hdr
MM_CAMERA += libchromatix_imx378_video
MM_CAMERA += libchromatix_imx378_video_hdr
MM_CAMERA += libchromatix_imx378_zsl_preview
MM_CAMERA += libchromatix_imx378_zsl_video
MM_CAMERA += libchromatix_ov4688_common
MM_CAMERA += libchromatix_ov4688_cpp_hfr_120
MM_CAMERA += libchromatix_ov4688_cpp_hfr_60
MM_CAMERA += libchromatix_ov4688_cpp_hfr_90
MM_CAMERA += libchromatix_ov4688_cpp_liveshot
MM_CAMERA += libchromatix_ov4688_cpp_preview
MM_CAMERA += libchromatix_ov4688_cpp_snapshot
MM_CAMERA += libchromatix_ov4688_cpp_video
MM_CAMERA += libchromatix_ov4688_default_video
MM_CAMERA += libchromatix_ov4688_hfr_60
MM_CAMERA += libchromatix_ov4688_hfr_60_ad5823
MM_CAMERA += libchromatix_ov4688_hfr_90
MM_CAMERA += libchromatix_ov4688_hfr_90_ad5823
MM_CAMERA += libchromatix_ov4688_hfr_120
MM_CAMERA += libchromatix_ov4688_hfr_120_ad5823
MM_CAMERA += libchromatix_ov4688_postproc
MM_CAMERA += libchromatix_ov4688_preview
MM_CAMERA += libchromatix_ov4688_snapshot
MM_CAMERA += libchromatix_ov4688_zsl_preview_ad5823
MM_CAMERA += libchromatix_ov4688_zsl_video_ad5823
MM_CAMERA += libchromatix_mt9e013_ar
MM_CAMERA += libchromatix_mt9e013_default_video
MM_CAMERA += libchromatix_mt9e013_preview
MM_CAMERA += libchromatix_mt9e013_video_hfr
MM_CAMERA += libchromatix_ov2680_common
MM_CAMERA += libchromatix_ov2680_default_video
MM_CAMERA += libchromatix_ov2680_preview
MM_CAMERA += libchromatix_ov2680_snapshot
MM_CAMERA += libchromatix_ov2680_5987fhq_common
MM_CAMERA += libchromatix_ov2680_5987fhq_default_video
MM_CAMERA += libchromatix_ov2680_5987fhq_preview
MM_CAMERA += libchromatix_ov2680_5987fhq_snapshot
MM_CAMERA += libchromatix_ov2680_5987fhq_common_v32
MM_CAMERA += libchromatix_ov2680_5987fhq_default_video_v32
MM_CAMERA += libchromatix_ov2680_5987fhq_preview_v32
MM_CAMERA += libchromatix_ov2680_5987fhq_snapshot_v32
MM_CAMERA += libchromatix_ov2680_a3_default_preview
MM_CAMERA += libchromatix_ov2680_a3_default_video
MM_CAMERA += libchromatix_ov2680_a3_hfr_60
MM_CAMERA += libchromatix_ov2680_common
MM_CAMERA += libchromatix_ov2680_cpp_hfr_60
MM_CAMERA += libchromatix_ov2680_cpp_liveshot
MM_CAMERA += libchromatix_ov2680_cpp_preview
MM_CAMERA += libchromatix_ov2680_cpp_snapshot
MM_CAMERA += libchromatix_ov2680_cpp_video
MM_CAMERA += libchromatix_ov2680_default_video
MM_CAMERA += libchromatix_ov2680_hfr_60
MM_CAMERA += libchromatix_ov2680_liveshot
MM_CAMERA += libchromatix_ov2680_postproc
MM_CAMERA += libchromatix_ov2680_preview
MM_CAMERA += libchromatix_ov2680_snapshot
MM_CAMERA += libchromatix_ov2680_zsl_preview
MM_CAMERA += libchromatix_ov13850_common
MM_CAMERA += libchromatix_ov13850_default_video
MM_CAMERA += libchromatix_ov13850_hfr_120fps
MM_CAMERA += libchromatix_ov13850_hfr_60fps
MM_CAMERA += libchromatix_ov13850_hfr_90fps
MM_CAMERA += libchromatix_ov13850_preview
MM_CAMERA += libchromatix_ov13850_snapshot
MM_CAMERA += libchromatix_ov13855_hfr_60_bu64297
MM_CAMERA += libchromatix_ov13855_default_preview_bu64297
MM_CAMERA += libchromatix_ov13855_zsl_video_bu64297
MM_CAMERA += libchromatix_ov13855_hfr_90_bu64297
MM_CAMERA += libchromatix_ov13855_default_video_bu64297
MM_CAMERA += libchromatix_ov13855_zsl_preview_bu64297
MM_CAMERA += libchromatix_ov13855_hfr_120_bu64297
MM_CAMERA += libchromatix_ov13855_cpp_liveshot
MM_CAMERA += libchromatix_ov13855_cpp_hfr_60
MM_CAMERA += libchromatix_ov13855_cpp_us_chromatix
MM_CAMERA += libchromatix_ov13855_cpp_hfr_120
MM_CAMERA += libchromatix_ov13855_cpp_video_full
MM_CAMERA += libchromatix_ov13855_cpp_snapshot
MM_CAMERA += libchromatix_ov13855_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov13855_cpp_video
MM_CAMERA += libchromatix_ov13855_cpp_preview
MM_CAMERA += libchromatix_ov13855_cpp_hfr_90
MM_CAMERA += libchromatix_ov13855_postproc
MM_CAMERA += libchromatix_ov13855_common
MM_CAMERA += libchromatix_ov13855_hfr_60
MM_CAMERA += libchromatix_ov13855_liveshot
MM_CAMERA += libchromatix_ov13855_video_full
MM_CAMERA += libchromatix_ov13855_hfr_90
MM_CAMERA += libchromatix_ov13855_snapshot
MM_CAMERA += libchromatix_ov13855_default_video
MM_CAMERA += libchromatix_ov13855_hfr_120
MM_CAMERA += libchromatix_ov13855_preview

MM_CAMERA += libchromatix_ov2281_a3_default_preview
MM_CAMERA += libchromatix_ov2281_a3_default_video
MM_CAMERA += libchromatix_ov2281_common
MM_CAMERA += libchromatix_ov2281_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov2281_cpp_liveshot
MM_CAMERA += libchromatix_ov2281_cpp_preview
MM_CAMERA += libchromatix_ov2281_cpp_snapshot
MM_CAMERA += libchromatix_ov2281_cpp_us_chromatix
MM_CAMERA += libchromatix_ov2281_cpp_video
MM_CAMERA += libchromatix_ov2281_default_video
MM_CAMERA += libchromatix_ov2281_liveshot
MM_CAMERA += libchromatix_ov2281_postproc
MM_CAMERA += libchromatix_ov2281_preview
MM_CAMERA += libchromatix_ov2281_snapshot
MM_CAMERA += libchromatix_ov2281_zsl_preview
MM_CAMERA += libchromatix_ov2281_zsl_video

MM_CAMERA += libchromatix_imx214_common
MM_CAMERA += libchromatix_imx214_default_video
MM_CAMERA += libchromatix_imx214_hfr_120fps
MM_CAMERA += libchromatix_imx214_hfr_60fps
MM_CAMERA += libchromatix_imx214_hfr_90fps
MM_CAMERA += libchromatix_imx214_preview
MM_CAMERA += libchromatix_imx214_snapshot
MM_CAMERA += libchromatix_imx214_liveshot
MM_CAMERA += libchromatix_imx214_snapshot_hdr
MM_CAMERA += libchromatix_imx214_video_hdr
MM_CAMERA += libchromatix_ov5670_common
MM_CAMERA += libchromatix_ov5670_postproc
MM_CAMERA += libchromatix_ov5670_cpp_preview
MM_CAMERA += libchromatix_ov5670_cpp_snapshot
MM_CAMERA += libchromatix_ov5670_cpp_liveshot
MM_CAMERA += libchromatix_ov5670_cpp_us_chromatix
MM_CAMERA += libchromatix_ov5670_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov5670_snapshot
MM_CAMERA += libchromatix_ov5670_video_full
MM_CAMERA += libchromatix_ov5670_cpp_video_full
MM_CAMERA += libchromatix_ov5670_zsl_preview
MM_CAMERA += libchromatix_ov5670_zsl_video
MM_CAMERA += libchromatix_ov5670_preview
MM_CAMERA += libchromatix_ov5670_default_video
MM_CAMERA += libchromatix_ov5670_cpp_video
MM_CAMERA += libchromatix_ov5670_a3_default_preview
MM_CAMERA += libchromatix_ov5670_a3_default_video
MM_CAMERA += libchromatix_ov5670_hfr_60
MM_CAMERA += libchromatix_ov5670_cpp_hfr_60
MM_CAMERA += libchromatix_ov5670_a3_hfr_60
MM_CAMERA += libchromatix_ov5670_hfr_90
MM_CAMERA += libchromatix_ov5670_cpp_hfr_90
MM_CAMERA += libchromatix_ov5670_a3_hfr_90
MM_CAMERA += libchromatix_ov5670_hfr_120
MM_CAMERA += libchromatix_ov5670_cpp_hfr_120
MM_CAMERA += libchromatix_ov5670_a3_hfr_120
MM_CAMERA += libchromatix_ov5670_f5670bq_common
MM_CAMERA += libchromatix_ov5670_f5670bq_postproc
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_preview
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_snapshot
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_liveshot
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_us_chromatix
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov5670_f5670bq_snapshot
MM_CAMERA += libchromatix_ov5670_f5670bq_video_full
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_video_full
MM_CAMERA += libchromatix_ov5670_f5670bq_zsl_preview
MM_CAMERA += libchromatix_ov5670_f5670bq_zsl_video
MM_CAMERA += libchromatix_ov5670_f5670bq_preview
MM_CAMERA += libchromatix_ov5670_f5670bq_default_video
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_video
MM_CAMERA += libchromatix_ov5670_f5670bq_a3_default_preview
MM_CAMERA += libchromatix_ov5670_f5670bq_a3_default_video
MM_CAMERA += libchromatix_ov5670_f5670bq_hfr_60
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_hfr_60
MM_CAMERA += libchromatix_ov5670_f5670bq_a3_hfr_60
MM_CAMERA += libchromatix_ov5670_f5670bq_hfr_90
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_hfr_90
MM_CAMERA += libchromatix_ov5670_f5670bq_a3_hfr_90
MM_CAMERA += libchromatix_ov5670_f5670bq_hfr_120
MM_CAMERA += libchromatix_ov5670_f5670bq_cpp_hfr_120
MM_CAMERA += libchromatix_ov5670_f5670bq_a3_hfr_120
MM_CAMERA += libchromatix_ov5647_ar
MM_CAMERA += libchromatix_ov5647_default_video
MM_CAMERA += libchromatix_ov5647_preview
MM_CAMERA += libchromatix_ov5648_oty5f03_common
MM_CAMERA += libchromatix_ov5648_oty5f03_cpp
MM_CAMERA += libchromatix_ov5648_oty5f03_default_video
MM_CAMERA += libchromatix_ov5648_oty5f03_preview
MM_CAMERA += libchromatix_ov5648_oty5f03_snapshot
MM_CAMERA += libchromatix_ov5648_oty5f03_video_hd
MM_CAMERA += libchromatix_ov5648_oty5f03_zsl
MM_CAMERA += libchromatix_ov5648_p5v18g_common
MM_CAMERA += libchromatix_ov5648_p5v18g_default_video_hd
MM_CAMERA += libchromatix_ov5648_p5v18g_default_video
MM_CAMERA += libchromatix_ov5648_p5v18g_preview
MM_CAMERA += libchromatix_ov5648_p5v18g_snapshot
MM_CAMERA += libchromatix_ov5648_p5v18g_zsl

MM_CAMERA += libchromatix_ov16825_common
MM_CAMERA += libchromatix_ov16825_default_video
MM_CAMERA += libchromatix_ov16825_hfr_120fps
MM_CAMERA += libchromatix_ov16825_hfr_60fps
MM_CAMERA += libchromatix_ov16825_hfr_90fps
MM_CAMERA += libchromatix_ov16825_preview
MM_CAMERA += libchromatix_ov16825_snapshot

MM_CAMERA += libchromatix_ov5648_q5v22e_common
MM_CAMERA += libchromatix_ov5648_q5v22e_default_video_hd
MM_CAMERA += libchromatix_ov5648_q5v22e_default_video
MM_CAMERA += libchromatix_ov5648_q5v22e_preview
MM_CAMERA += libchromatix_ov5648_q5v22e_snapshot
MM_CAMERA += libchromatix_ov5648_q5v22e_zsl

MM_CAMERA += libchromatix_ov7251_common
MM_CAMERA += libchromatix_ov7251_postproc
MM_CAMERA += libchromatix_ov7251_cpp_preview
MM_CAMERA += libchromatix_ov7251_cpp_snapshot
MM_CAMERA += libchromatix_ov7251_cpp_liveshot
MM_CAMERA += libchromatix_ov7251_preview
MM_CAMERA += libchromatix_ov7251_snapshot
MM_CAMERA += libchromatix_ov7251_default_video
MM_CAMERA += libchromatix_ov7251_cpp_video
MM_CAMERA += libchromatix_ov7251_zsl_preview
MM_CAMERA += libchromatix_ov7251_zsl_video

MM_CAMERA += libchromatix_ov8856_common
MM_CAMERA += libchromatix_ov8856_a3_default_preview
MM_CAMERA += libchromatix_ov8856_a3_default_video
MM_CAMERA += libchromatix_ov8856_a3_hfr_60
MM_CAMERA += libchromatix_ov8856_a3_hfr_90
MM_CAMERA += libchromatix_ov8856_a3_hfr_120
MM_CAMERA += libchromatix_ov8856_zsl_preview
MM_CAMERA += libchromatix_ov8856_zsl_video
MM_CAMERA += libchromatix_ov8856_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov8856_cpp_hfr_60
MM_CAMERA += libchromatix_ov8856_cpp_hfr_90
MM_CAMERA += libchromatix_ov8856_cpp_hfr_120
MM_CAMERA += libchromatix_ov8856_cpp_liveshot
MM_CAMERA += libchromatix_ov8856_cpp_preview
MM_CAMERA += libchromatix_ov8856_cpp_snapshot
MM_CAMERA += libchromatix_ov8856_cpp_us_chromatix
MM_CAMERA += libchromatix_ov8856_cpp_video
MM_CAMERA += libchromatix_ov8856_hfr_60
MM_CAMERA += libchromatix_ov8856_hfr_90
MM_CAMERA += libchromatix_ov8856_hfr_120
MM_CAMERA += libchromatix_ov8856_liveshot
MM_CAMERA += libchromatix_ov8856_preview
MM_CAMERA += libchromatix_ov8856_snapshot
MM_CAMERA += libchromatix_ov8856_default_video
MM_CAMERA += libchromatix_ov8856_postproc

MM_CAMERA += libchromatix_ov8858_common
MM_CAMERA += libchromatix_ov8858_preview
MM_CAMERA += libchromatix_ov8858_default_video
MM_CAMERA += libchromatix_ov8858_snapshot
MM_CAMERA += libchromatix_ov8858_hfr_120fps
MM_CAMERA += libchromatix_ov8858_video_hd
MM_CAMERA += libchromatix_ov8858_hfr_60fps
MM_CAMERA += libchromatix_ov8858_zsl
MM_CAMERA += libchromatix_ov8858_hfr_90fps
MM_CAMERA += libchromatix_ov8858_liveshot

MM_CAMERA += libchromatix_ov8858_q8v19w_common
MM_CAMERA += libchromatix_ov8858_q8v19w_preview
MM_CAMERA += libchromatix_ov8858_q8v19w_default_video
MM_CAMERA += libchromatix_ov8858_q8v19w_snapshot
MM_CAMERA += libchromatix_ov8858_q8v19w_hfr_120fps
MM_CAMERA += libchromatix_ov8858_q8v19w_video_hd
MM_CAMERA += libchromatix_ov8858_q8v19w_hfr_60fps
MM_CAMERA += libchromatix_ov8858_q8v19w_zsl
MM_CAMERA += libchromatix_ov8858_q8v19w_zsl_30
MM_CAMERA += libchromatix_ov8858_q8v19w_hfr_90fps
MM_CAMERA += libchromatix_ov8858_q8v19w_liveshot
MM_CAMERA += libchromatix_ov8858_q8v19w_video_nonHD

MM_CAMERA += libchromatix_s5k3p8sp_common
MM_CAMERA += libchromatix_s5k3p8sp_postproc
MM_CAMERA += libchromatix_s5k3p8sp_preview
MM_CAMERA += libchromatix_s5k3p8sp_snapshot
MM_CAMERA += libchromatix_s5k3p8sp_cpp_preview
MM_CAMERA += libchromatix_s5k3p8sp_cpp_snapshot
MM_CAMERA += libchromatix_s5k3p8sp_cpp_video
MM_CAMERA += libchromatix_s5k3p8sp_cpp_liveshot
MM_CAMERA += libchromatix_s5k3p8sp_cpp_snapshot_4m
MM_CAMERA += libchromatix_s5k3p8sp_a3_zsl_preview
MM_CAMERA += libchromatix_s5k3p8sp_a3_default_preview
MM_CAMERA += libchromatix_s5k3p8sp_a3_zsl_video
MM_CAMERA += libchromatix_s5k3p8sp_a3_default_video

MM_CAMERA += libchromatix_ov7692_ar
MM_CAMERA += libchromatix_ov7692_default_video
MM_CAMERA += libchromatix_ov7692_preview
MM_CAMERA += libchromatix_ov8865_q8v18a_common
MM_CAMERA += libchromatix_ov8865_q8v18a_cpp
MM_CAMERA += libchromatix_ov8865_q8v18a_default_video
MM_CAMERA += libchromatix_ov8865_q8v18a_hfr_120fps
MM_CAMERA += libchromatix_ov8865_q8v18a_hfr_60fps
MM_CAMERA += libchromatix_ov8865_q8v18a_hfr_90fps
MM_CAMERA += libchromatix_ov8865_q8v18a_liveshot
MM_CAMERA += libchromatix_ov8865_q8v18a_preview
MM_CAMERA += libchromatix_ov8865_q8v18a_snapshot
MM_CAMERA += libchromatix_ov8865_q8v18a_video_hd
MM_CAMERA += libchromatix_ov8865_q8v18a_zsl
MM_CAMERA += libchromatix_imx230_4k_preview_lc898212xd
MM_CAMERA += libchromatix_imx230_4k_video_lc898212xd
MM_CAMERA += libchromatix_s5k3m2xx_common
MM_CAMERA += libchromatix_s5k3m2xx_cpp_hfr_120
MM_CAMERA += libchromatix_s5k3m2xx_cpp_hfr_60
MM_CAMERA += libchromatix_s5k3m2xx_cpp_hfr_90
MM_CAMERA += libchromatix_s5k3m2xx_cpp_liveshot
MM_CAMERA += libchromatix_s5k3m2xx_cpp_preview
MM_CAMERA += libchromatix_s5k3m2xx_cpp_snapshot
MM_CAMERA += libchromatix_s5k3m2xx_cpp_snapshot_downscale
MM_CAMERA += libchromatix_s5k3m2xx_cpp_snapshot_upscale
MM_CAMERA += libchromatix_s5k3m2xx_cpp_video
MM_CAMERA += libchromatix_s5k3m2xx_cpp_video_4k
MM_CAMERA += libchromatix_s5k3m2xx_default_preview_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_default_video
MM_CAMERA += libchromatix_s5k3m2xx_default_video_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_hfr_120
MM_CAMERA += libchromatix_s5k3m2xx_hfr_120_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_hfr_60
MM_CAMERA += libchromatix_s5k3m2xx_hfr_60_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_hfr_90
MM_CAMERA += libchromatix_s5k3m2xx_hfr_90_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_liveshot
MM_CAMERA += libchromatix_s5k3m2xx_postproc
MM_CAMERA += libchromatix_s5k3m2xx_preview
MM_CAMERA += libchromatix_s5k3m2xx_snapshot
MM_CAMERA += libchromatix_s5k3m2xx_video_4k
MM_CAMERA += libchromatix_s5k3m2xx_zsl_preview_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_zsl_video_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_1080p_preview_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_1080p_video_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_4k_preview_ad5816g
MM_CAMERA += libchromatix_s5k3m2xx_4k_video_ad5816g
MM_CAMERA += libchromatix_s5k3m2xm_common
MM_CAMERA += libchromatix_s5k3m2xm_cpp_hfr_120
MM_CAMERA += libchromatix_s5k3m2xm_cpp_hfr_60
MM_CAMERA += libchromatix_s5k3m2xm_cpp_hfr_90
MM_CAMERA += libchromatix_s5k3m2xm_cpp_liveshot
MM_CAMERA += libchromatix_s5k3m2xm_cpp_preview
MM_CAMERA += libchromatix_s5k3m2xm_cpp_snapshot
MM_CAMERA += libchromatix_s5k3m2xm_cpp_video
MM_CAMERA += libchromatix_s5k3m2xm_default_preview_dw9761b
MM_CAMERA += libchromatix_s5k3m2xm_default_video
MM_CAMERA += libchromatix_s5k3m2xm_default_video_dw9761b
MM_CAMERA += libchromatix_s5k3m2xm_hfr_120
MM_CAMERA += libchromatix_s5k3m2xm_hfr_120_dw9761b
MM_CAMERA += libchromatix_s5k3m2xm_hfr_60
MM_CAMERA += libchromatix_s5k3m2xm_hfr_60_dw9761b
MM_CAMERA += libchromatix_s5k3m2xm_hfr_90
MM_CAMERA += libchromatix_s5k3m2xm_hfr_90_dw9761b
MM_CAMERA += libchromatix_s5k3m2xm_liveshot
MM_CAMERA += libchromatix_s5k3m2xm_postproc
MM_CAMERA += libchromatix_s5k3m2xm_preview
MM_CAMERA += libchromatix_s5k3m2xm_snapshot
MM_CAMERA += libchromatix_s5k3m2xm_zsl_preview_dw9761b
MM_CAMERA += libchromatix_s5k3m2xm_zsl_video_dw9761b
MM_CAMERA += libchromatix_s5k3m2xm_common_bear
MM_CAMERA += libchromatix_s5k3m2xm_cpp_hfr_120_bear
MM_CAMERA += libchromatix_s5k3m2xm_cpp_hfr_60_bear
MM_CAMERA += libchromatix_s5k3m2xm_cpp_hfr_90_bear
MM_CAMERA += libchromatix_s5k3m2xm_cpp_liveshot_bear
MM_CAMERA += libchromatix_s5k3m2xm_cpp_preview_bear
MM_CAMERA += libchromatix_s5k3m2xm_cpp_snapshot_bear
MM_CAMERA += libchromatix_s5k3m2xm_cpp_video_bear
MM_CAMERA += libchromatix_s5k3m2xm_default_preview_dw9761b_bear
MM_CAMERA += libchromatix_s5k3m2xm_default_video_bear
MM_CAMERA += libchromatix_s5k3m2xm_default_video_dw9761b_bear
MM_CAMERA += libchromatix_s5k3m2xm_hfr_120_bear
MM_CAMERA += libchromatix_s5k3m2xm_hfr_120_dw9761b_bear
MM_CAMERA += libchromatix_s5k3m2xm_hfr_60_bear
MM_CAMERA += libchromatix_s5k3m2xm_hfr_60_dw9761b_bear
MM_CAMERA += libchromatix_s5k3m2xm_hfr_90_bear
MM_CAMERA += libchromatix_s5k3m2xm_hfr_90_dw9761b_bear
MM_CAMERA += libchromatix_s5k3m2xm_liveshot_bear
MM_CAMERA += libchromatix_s5k3m2xm_postproc_bear
MM_CAMERA += libchromatix_s5k3m2xm_preview_bear
MM_CAMERA += libchromatix_s5k3m2xm_snapshot_bear
MM_CAMERA += libchromatix_s5k3m2xm_zsl_preview_dw9761b_bear
MM_CAMERA += libchromatix_s5k3m2xm_zsl_video_dw9761b_bear
MM_CAMERA += libchromatix_s5k3p3sm_common
MM_CAMERA += libchromatix_s5k3p3sm_cpp_liveshot
MM_CAMERA += libchromatix_s5k3p3sm_cpp_preview
MM_CAMERA += libchromatix_s5k3p3sm_cpp_snapshot
MM_CAMERA += libchromatix_s5k3p3sm_cpp_snapshot_hdr
MM_CAMERA += libchromatix_s5k3p3sm_cpp_video
MM_CAMERA += libchromatix_s5k3p3sm_default_video
MM_CAMERA += libchromatix_s5k3p3sm_default_preview_3a
MM_CAMERA += libchromatix_s5k3p3sm_default_video_3a
MM_CAMERA += libchromatix_s5k3p3sm_fullsize_preview_3a
MM_CAMERA += libchromatix_s5k3p3sm_fullsize_video_3a
MM_CAMERA += libchromatix_s5k3p3sm_liveshot
MM_CAMERA += libchromatix_s5k3p3sm_postproc
MM_CAMERA += libchromatix_s5k3p3sm_preview
MM_CAMERA += libchromatix_s5k3p3sm_snapshot
MM_CAMERA += libchromatix_s5k4e1_ar
MM_CAMERA += libchromatix_s5k4e1_default_video
MM_CAMERA += libchromatix_s5k4e1_preview
MM_CAMERA += libchromatix_SKUAA_ST_gc0339_common
MM_CAMERA += libchromatix_SKUAA_ST_gc0339_cpp
MM_CAMERA += libchromatix_SKUAA_ST_gc0339_default_video
MM_CAMERA += libchromatix_SKUAA_ST_gc0339_preview
MM_CAMERA += libchromatix_skuab_shinetech_gc0339_common
MM_CAMERA += libchromatix_skuab_shinetech_gc0339_cpp
MM_CAMERA += libchromatix_skuab_shinetech_gc0339_default_video
MM_CAMERA += libchromatix_skuab_shinetech_gc0339_liveshot
MM_CAMERA += libchromatix_skuab_shinetech_gc0339_preview
MM_CAMERA += libchromatix_skuab_shinetech_gc0339_snapshot
MM_CAMERA += libchromatix_skuab_shinetech_gc0339_zsl
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_common
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_cpp
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_default_video
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_hfr_120fps
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_hfr_60fps
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_hfr_90fps
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_liveshot
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_preview
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_snapshot
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_video_hd
MM_CAMERA += libchromatix_SKUAB_ST_s5k4e1_zsl
MM_CAMERA += libchromatix_S5K4E1_13P1BA_zsl
MM_CAMERA += libchromatix_S5K4E1_13P1BA_default_video
MM_CAMERA += libchromatix_S5K4E1_13P1BA_video_hd
MM_CAMERA += libchromatix_S5K4E1_13P1BA_snapshot
MM_CAMERA += libchromatix_S5K4E1_13P1BA_preview
MM_CAMERA += libchromatix_S5K4E1_13P1BA_hfr_120fps
MM_CAMERA += libchromatix_S5K4E1_13P1BA_hfr_90fps
MM_CAMERA += libchromatix_S5K4E1_13P1BA_hfr_60fps
MM_CAMERA += libchromatix_S5K4E1_13P1BA_common
MM_CAMERA += libchromatix_S5K4E1_13P1BA_liveshot
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_common
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_cpp
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_default_video
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_hfr_120fps
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_hfr_60fps
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_hfr_90fps
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_preview
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_snapshot
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_video_hd
MM_CAMERA += libchromatix_skuf_ov12830_p12v01c_zsl
MM_CAMERA += libchromatix_skuf_ov5648_p5v23c_common
MM_CAMERA += libchromatix_skuf_ov5648_p5v23c_cpp
MM_CAMERA += libchromatix_skuf_ov5648_p5v23c_default_video
MM_CAMERA += libchromatix_skuf_ov5648_p5v23c_preview
MM_CAMERA += libchromatix_skuf_ov5648_p5v23c_snapshot
MM_CAMERA += libchromatix_s5k3m3sm_4k_preview_3a
MM_CAMERA += libchromatix_s5k3m3sm_4k_video_3a
MM_CAMERA += libchromatix_s5k3m3sm_default_preview_3a
MM_CAMERA += libchromatix_s5k3m3sm_default_video_3a
MM_CAMERA += libchromatix_s5k3m3sm_hfr60_3a
MM_CAMERA += libchromatix_s5k3m3sm_zsl_preview_3a
MM_CAMERA += libchromatix_s5k3m3sm_zsl_video_3a
MM_CAMERA += libchromatix_s5k3m3sm_common
MM_CAMERA += libchromatix_s5k3m3sm_cpp_hfr_60
MM_CAMERA += libchromatix_s5k3m3sm_cpp_liveshot
MM_CAMERA += libchromatix_s5k3m3sm_cpp_preview
MM_CAMERA += libchromatix_s5k3m3sm_cpp_snapshot
MM_CAMERA += libchromatix_s5k3m3sm_cpp_video
MM_CAMERA += libchromatix_s5k3m3sm_cpp_video_4k
MM_CAMERA += libchromatix_s5k3m3sm_hfr_60
MM_CAMERA += libchromatix_s5k3m3sm_liveshot
MM_CAMERA += libchromatix_s5k3m3sm_preview
MM_CAMERA += libchromatix_s5k3m3sm_snapshot
MM_CAMERA += libchromatix_s5k3m3sm_video
MM_CAMERA += libchromatix_s5k3m3sm_video_4k
MM_CAMERA += libchromatix_s5k3m3sm_postproc
MM_CAMERA += libchromatix_s5k2l7sx_4k_preview_3a
MM_CAMERA += libchromatix_s5k2l7sx_4k_video_3a
MM_CAMERA += libchromatix_s5k2l7sx_default_preview_3a
MM_CAMERA += libchromatix_s5k2l7sx_default_video_3a
MM_CAMERA += libchromatix_s5k2l7sx_hfr_60_3a
MM_CAMERA += libchromatix_s5k2l7sx_zsl_preview_3a
MM_CAMERA += libchromatix_s5k2l7sx_zsl_video_3a
MM_CAMERA += libchromatix_s5k2l7sx_common
MM_CAMERA += libchromatix_s5k2l7sx_cpp_hfr_60
MM_CAMERA += libchromatix_s5k2l7sx_cpp_liveshot
MM_CAMERA += libchromatix_s5k2l7sx_cpp_preview
MM_CAMERA += libchromatix_s5k2l7sx_cpp_snapshot
MM_CAMERA += libchromatix_s5k2l7sx_cpp_video
MM_CAMERA += libchromatix_s5k2l7sx_cpp_video_4k
MM_CAMERA += libchromatix_s5k2l7sx_hfr_60
MM_CAMERA += libchromatix_s5k2l7sx_liveshot
MM_CAMERA += libchromatix_s5k2l7sx_preview
MM_CAMERA += libchromatix_s5k2l7sx_snapshot
MM_CAMERA += libchromatix_s5k2l7sx_video
MM_CAMERA += libchromatix_s5k2l7sx_video_4k
MM_CAMERA += libchromatix_s5k2l7sx_postproc
ifeq ($(SD624_SOM_SUPPORT), true)
MM_CAMERA += libmmcamera_ov5693
MM_CAMERA += libchromatix_ov5693_common
MM_CAMERA += libchromatix_ov5693_default_video_3a
MM_CAMERA += libchromatix_ov5693_default_preview
MM_CAMERA += libchromatix_ov5693_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov5693_cpp_hfr_60
MM_CAMERA += libchromatix_ov5693_cpp_liveshot
MM_CAMERA += libchromatix_ov5693_cpp_preview
MM_CAMERA += libchromatix_ov5693_cpp_snapshot
MM_CAMERA += libchromatix_ov5693_cpp_us_chromatix
MM_CAMERA += libchromatix_ov5693_cpp_video
MM_CAMERA += libchromatix_ov5693_cpp_video_full
MM_CAMERA += libchromatix_ov5693_default_video
MM_CAMERA += libchromatix_ov5693_hfr_60
MM_CAMERA += libchromatix_ov5693_liveshot
MM_CAMERA += libchromatix_ov5693_postproc
MM_CAMERA += libchromatix_ov5693_preview
MM_CAMERA += libchromatix_ov5693_snapshot
MM_CAMERA += libchromatix_ov5693_video_full
MM_CAMERA += libchromatix_ov5693_zsl_preview
MM_CAMERA += libchromatix_ov5693_zsl_video
MM_CAMERA += libchromatix_ov5693_a3_hfr_60
MM_CAMERA += libmmcamera_qtech_ov5693_eeprom
MM_CAMERA += ov5693_chromatix.xml
endif
MM_CAMERA += libedgesmooth
MM_CAMERA += libedge_smooth_hvx_stub
MM_CAMERA += libedge_smooth_skel
MM_CAMERA += libmmcamera_atmel_at24c32e_eeprom
MM_CAMERA += libmmcamera_rohm_brcg064gwz_3_eeprom
MM_CAMERA += libmmcamera_hvx_add_constant
MM_CAMERA += libmmcamera_hvx_grid_sum
MM_CAMERA += libmmcamera_hvx_zzHDR
MM_CAMERA += libmmcamera
MM_CAMERA += libmmcamera_dbg
MM_CAMERA += libmmcamera_cac_lib
MM_CAMERA += libmmcamera_cac2_lib
MM_CAMERA += libmmcamera_cac3_lib
MM_CAMERA += libmmcamera_chromaflash_lib
MM_CAMERA += libmmcamera_edgesmooth_lib
MM_CAMERA += libmmcamera_quadracfa
MM_CAMERA += libremosaiclib
MM_CAMERA += libmmcamera_eeprom_util
MM_CAMERA += libmmcamera_facedetection_lib
MM_CAMERA += libmmcamera_faceproc
MM_CAMERA += libmmcamera_faceproc2
MM_CAMERA += libswregistrationalgo
MM_CAMERA += libmmcamera_faceproc_system
MM_CAMERA += libmmcamera_faceproc2_system
MM_CAMERA += libswregistrationalgo_system
MM_CAMERA += libmmcamera_frameproc
MM_CAMERA += libmmcamera_hdr_gb_lib
MM_CAMERA += libmmcamera_hdr_lib
MM_CAMERA += libmmcamera_hi256
MM_CAMERA += libmmcamera_imglib
MM_CAMERA += libmmcamera_imx091
MM_CAMERA += libmmcamera_imx132
MM_CAMERA += libmmcamera_imx134
MM_CAMERA += libmmcamera_imx135
MM_CAMERA += libmmcamera_paaf_lib
MM_CAMERA += libmmcamera_ppeiscore
MM_CAMERA += libmmcamera_depth_map
MM_CAMERA += libdepthmapwrapper
MM_CAMERA += libmmcamera_ov2685
MM_CAMERA += libmmcamera_ov4688
MM_CAMERA += libmmcamera_ov4688_eeprom
MM_CAMERA += libmmcamera_ov4188
MM_CAMERA += libmmcamera_ov7251
MM_CAMERA += libmmcamera_imx214
MM_CAMERA += libmmcamera_sony_imx214_eeprom
MM_CAMERA += libmmcamera_irs1645
MM_CAMERA += libmmcamera_s5k3m2xx
MM_CAMERA += libmmcamera_onsemi_cat24c32_eeprom
MM_CAMERA += libmmcamera_onsemi_cat24c32_imx362_eeprom
MM_CAMERA += libmmcamera_onsemi_cat24c16_eeprom
MM_CAMERA += libmmcamera_sunny_gt24c64a_imx376_eeprom
MM_CAMERA += libmmcamera_imx230
MM_CAMERA += libmmcamera_imx258
MM_CAMERA += libmmcamera_imx258_gt24c16_eeprom
MM_CAMERA += libmmcamera_imx258_gt24c32_eeprom
MM_CAMERA += libmmcamera_imx258_mono
MM_CAMERA += libmmcamera_imx258_mono_gt24c16_eeprom
MM_CAMERA += libmmcamera_imx258_mono_gt24c32_eeprom
MM_CAMERA += libmmcamera_imx318
MM_CAMERA += libmmcamera_imx362
MM_CAMERA += libmmcamera_imx362_gt24c64a
MM_CAMERA += libmmcamera_imx376
MM_CAMERA += libmmcamera_dw9761b_eeprom
MM_CAMERA += libmmcamera_dcrf_lib
MM_CAMERA += libmmcamera_le2464c_eeprom
MM_CAMERA += libmmcamera_llvd
MM_CAMERA += libmmcamera_bokeh
MM_CAMERA += libmmcamera_sw_tnr
MM_CAMERA += libmmcamera_s5k3m2xm
MM_CAMERA += libmmcamera_isp_abf44
MM_CAMERA += libmmcamera_isp_abf46
MM_CAMERA += libmmcamera_isp_abf47
MM_CAMERA += libmmcamera_isp_abf48
MM_CAMERA += libmmcamera_isp_aec_bg_stats47
MM_CAMERA += libmmcamera_isp_abcc44
MM_CAMERA += libmmcamera_isp_bcc44
MM_CAMERA += libmmcamera_isp_black_level47
MM_CAMERA += libmmcamera_isp_bpc40
MM_CAMERA += libmmcamera_isp_bcc40
MM_CAMERA += libmmcamera_isp_bpc44
MM_CAMERA += libmmcamera_isp_bpc47
MM_CAMERA += libmmcamera_isp_bpc48
MM_CAMERA += libmmcamera_isp_be_stats44
MM_CAMERA += libmmcamera_isp_bf_scale_stats44
MM_CAMERA += libmmcamera_isp_bf_scale_stats46
MM_CAMERA += libmmcamera_isp_bf_stats44
MM_CAMERA += libmmcamera_isp_bf_stats47
MM_CAMERA += libmmcamera_isp_bhist_stats44
MM_CAMERA += libmmcamera_isp_bg_stats44
MM_CAMERA += libmmcamera_isp_bg_stats46
MM_CAMERA += libmmcamera_isp_black_level48
MM_CAMERA += libmmcamera_isp_cac47
MM_CAMERA += libmmcamera_isp_cs_stats44
MM_CAMERA += libmmcamera_isp_cs_stats46
MM_CAMERA += libmmcamera_isp_chroma_enhan40
MM_CAMERA += libmmcamera_isp_chroma_suppress40
MM_CAMERA += libmmcamera_isp_clamp_encoder40
MM_CAMERA += libmmcamera_isp_clamp_viewfinder40
MM_CAMERA += libmmcamera_isp_clamp_video40
MM_CAMERA += libmmcamera_isp_clf44
MM_CAMERA += libmmcamera_isp_clf46
MM_CAMERA += libmmcamera_isp_color_correct40
MM_CAMERA += libmmcamera_isp_color_correct46
MM_CAMERA += libmmcamera_isp_color_xform_encoder40
MM_CAMERA += libmmcamera_isp_color_xform_viewfinder40
MM_CAMERA += libmmcamera_isp_color_xform_encoder46
MM_CAMERA += libmmcamera_isp_color_xform_viewfinder46
MM_CAMERA += libmmcamera_isp_color_xform_video46
MM_CAMERA += libmmcamera_isp_demosaic40
MM_CAMERA += libmmcamera_isp_demosaic44
MM_CAMERA += libmmcamera_isp_demosaic47
MM_CAMERA += libmmcamera_isp_demosaic48
MM_CAMERA += libmmcamera_isp_demux40
MM_CAMERA += libmmcamera_isp_demux48
MM_CAMERA += libmmcamera_isp_fovcrop_encoder40
MM_CAMERA += libmmcamera_isp_fovcrop_viewfinder40
MM_CAMERA += libmmcamera_isp_fovcrop_encoder46
MM_CAMERA += libmmcamera_isp_fovcrop_viewfinder46
MM_CAMERA += libmmcamera_isp_fovcrop_video46
MM_CAMERA += libmmcamera_isp_gamma40
MM_CAMERA += libmmcamera_isp_gamma44
MM_CAMERA += libmmcamera_isp_gamma46
MM_CAMERA += libmmcamera_isp_gic46
MM_CAMERA += libmmcamera_isp_gic48
MM_CAMERA += libmmcamera_isp_gtm46
MM_CAMERA += libmmcamera_isp_hdr_be_stats46
MM_CAMERA += libmmcamera_isp_hdr46
MM_CAMERA += libmmcamera_isp_hdr48
MM_CAMERA += libmmcamera_isp_hdr_bhist_stats44
MM_CAMERA += libmmcamera_isp_ihist_stats44
MM_CAMERA += libmmcamera_isp_ihist_stats46
MM_CAMERA += libmmcamera_isp_linearization40
MM_CAMERA += libmmcamera_isp_luma_adaptation40
MM_CAMERA += libmmcamera_isp_ltm44
MM_CAMERA += libmmcamera_isp_ltm47
MM_CAMERA += libmmcamera_isp_mce40
MM_CAMERA += libmmcamera_isp_mesh_rolloff40
MM_CAMERA += libmmcamera_isp_mesh_rolloff44
MM_CAMERA += libmmcamera_isp_pedestal_correct46
MM_CAMERA += libmmcamera_isp_pdaf48
MM_CAMERA += libmmcamera_isp_rs_stats44
MM_CAMERA += libmmcamera_isp_scaler_encoder40
MM_CAMERA += libmmcamera_isp_scaler_viewfinder40
MM_CAMERA += libmmcamera_isp_snr47
MM_CAMERA += libmmcamera_isp_rs_stats46
MM_CAMERA += libmmcamera_isp_scaler_encoder44
MM_CAMERA += libmmcamera_isp_scaler_viewfinder44
MM_CAMERA += libmmcamera_isp_scaler_encoder46
MM_CAMERA += libmmcamera_isp_scaler_viewfinder46
MM_CAMERA += libmmcamera_isp_scaler_video46
MM_CAMERA += libmmcamera_isp_sce40
MM_CAMERA += libmmcamera_isp_sub_module
MM_CAMERA += libmmcamera_isp_wb40
MM_CAMERA += libmmcamera_isp_wb46
MM_CAMERA += libmmcamera_mt9m114
MM_CAMERA += libmmcamera_optizoom_lib
MM_CAMERA += libmmcamera_ov5645
MM_CAMERA += libmmcamera_ov2680
MM_CAMERA += libmmcamera_ov2680_5987fhq
MM_CAMERA += libmmcamera_ov5648_oty5f03
MM_CAMERA += libmmcamera_ov5648_p5v18g
MM_CAMERA += libmmcamera_ov5695
MM_CAMERA += libmmcamera_ov16825
MM_CAMERA += libmmcamera_ov5648_q5v22e
MM_CAMERA += libmmcamera_ov8858
MM_CAMERA += libmmcamera_ov8858_q8v19w
MM_CAMERA += libmmcamera_ov8858_q8v19w_30
MM_CAMERA += libmmcamera_ov8865_q8v18a
MM_CAMERA += libmmcamera_ov13850
MM_CAMERA += libmmcamera_ov12a10
MM_CAMERA += libmmcamera_ov13880
MM_CAMERA += libmmcamera_ov13855
MM_CAMERA += libmmcamera_ov2281
MM_CAMERA += libmmcamera_s5k3l8
MM_CAMERA += libmmcamera_s5k3l8_mono
MM_CAMERA += libmmcamera_s5k2l7
MM_CAMERA += libmmcamera_s5k2l7sx
MM_CAMERA += libmmcamera_s5k3p8sp
MM_CAMERA += libmmcamera_ov5670
MM_CAMERA += libmmcamera_imx298
MM_CAMERA += libmmcamera_imx378
MM_CAMERA += libois_bu63165
MM_CAMERA += libmmcamera_pdaf
MM_CAMERA += libmmcamera_pdafcamif
MM_CAMERA += libmmcamera_plugin
MM_CAMERA += libmmcamera_SKUAA_ST_gc0339
MM_CAMERA += libmmcamera_skuab_shinetech_gc0339
MM_CAMERA += libmmcamera_SKUAB_ST_s5k4e1
MM_CAMERA += libmmcamera_s5k3m2xx
MM_CAMERA += libmmcamera_S5K4E1_13P1BA
MM_CAMERA += libmmcamera_skuf_ov12830_p12v01c
MM_CAMERA += libmmcamera_skuf_ov5648_p5v23c
MM_CAMERA += libmmcamera_sp1628
MM_CAMERA += libmmcamera_statsproc31
MM_CAMERA += libmmcamera_stillmore_lib
MM_CAMERA += libmmcamera_ofilm_oty5f03_eeprom
MM_CAMERA += libmmcamera_sunny_q13v04b_eeprom
MM_CAMERA += libmmcamera_ofilm_ohs0443_ov12a10_eeprom
MM_CAMERA += libmmcamera_ofilm_ohs0443_ov13880_eeprom
MM_CAMERA += libmmcamera_sonyimx135_eeprom
MM_CAMERA += libmmcamera_sunny_ov13855_eeprom
MM_CAMERA += libmmcamera_sunny_p12v01m_eeprom
MM_CAMERA += libmmcamera_sunny_p5v23c_eeprom
MM_CAMERA += libmmcamera_sunny_q8v18a_eeprom
MM_CAMERA += libmmcamera_sunny_ov8858_q8v19w_eeprom
MM_CAMERA += libmmcamera_sunny_q5v41b_eeprom
MM_CAMERA += libmmcamera_sunny_q5v22e_eeprom
MM_CAMERA += libmmcamera_sunny_gt24c64_s5k2l7_eeprom
MM_CAMERA += libmmcamera_sunny_gt24c64_s5k2l7sx_eeprom
MM_CAMERA += libmmcamera_sunny_gt24c64_imx298_eeprom
MM_CAMERA += libmmcamera_target
MM_CAMERA += libmmcamera_trueportrait_lib
MM_CAMERA += libmmcamera_truly_cm7700_eeprom
MM_CAMERA += libmmcamera_ubifocus_lib
MM_CAMERA += libmmcamera_wavelet_lib
MM_CAMERA += libmmcamera2_dcrf
MM_CAMERA += libmmcamera2_frame_algorithm
MM_CAMERA += libmmcamera2_q3a_core
MM_CAMERA += libmmcamera2_q3a_release
MM_CAMERA += libmmcamera2_is
MM_CAMERA += libmmcamera2_sensor_debug
MM_CAMERA += libmmcamera2_stats_algorithm
MM_CAMERA += libmmcamera2_stats_release
MM_CAMERA += libmmcamera-core
MM_CAMERA += libmm-qcamera
MM_CAMERA += liboemcamera
MM_CAMERA += libmmcamera2_mct
MM_CAMERA += libmmcamera2_mct_shimlayer
MM_CAMERA += libqcamera
MM_CAMERA += libseemore_hexagon_skel
MM_CAMERA += mm-qcamera-app
MM_CAMERA += mm-qcamera-daemon
MM_CAMERA += mm-qcamera-test
MM_CAMERA += mm-qcamera-testsuite-client
MM_CAMERA += test_bet_8996
MM_CAMERA += test_bet_8998
MM_CAMERA += test_module_pproc
MM_CAMERA += mct-unit-test-app
MM_CAMERA += libtm_interface
MM_CAMERA += test_suite_all_modules
MM_CAMERA += test_suite_no_sensor
MM_CAMERA += test_suite_pproc
MM_CAMERA += test_suite_vfe
MM_CAMERA += v4l2-qcamera-app
MM_CAMERA += libmmcamera_tintless_algo
MM_CAMERA += libmmcamera_tintless_bg_pca_algo
MM_CAMERA += secure_camera_sample_client
MM_CAMERA += SnapdragonCamera
MM_CAMERA += PIPCamera
MM_CAMERA += AWBTestApp

MM_CAMERA += libmmcamera_ov8856_4lane
MM_CAMERA += libmmcamera_ov8856
MM_CAMERA += libmmcamera_ov8856_2lane
MM_CAMERA += libmmcamera_ov8865
MM_CAMERA += libmmcamera_ov13850_q13v06k
MM_CAMERA += libmmcamera_ov2685_scv3b4035
MM_CAMERA += libmmcamera_ov5670
MM_CAMERA += libmmcamera_s5k3m2xm
MM_CAMERA += libmmcamera_s5k3p3sm
MM_CAMERA += libmmcamera_s5k3m3sm
MM_CAMERA += libmmcamera_m24c64s_eeprom
MM_CAMERA += libmmcamera_sunny_8865_eeprom
MM_CAMERA += libmmcamera_truly_cma481_eeprom
MM_CAMERA += libmmcamera_truly_cmb433_eeprom
MM_CAMERA += libmmcamera_qtech_f5670bq_eeprom
MM_CAMERA += libmmcamera_sunny_q13v06k_eeprom
MM_CAMERA += libmmcamera_qtech_f3l8yam_eeprom
MM_CAMERA += libmmcamera_sunny_gt24c64a_imx362_eeprom
MM_CAMERA += libmmcamera_csidtg
MM_CAMERA += libmmcamera_dw9761b_eeprom
MM_CAMERA += libmmcamera_eebinparse
MM_CAMERA += libmmcamera_imglib_faceproc_adspstub
MM_CAMERA += libmmcamera_isp_abf40
MM_CAMERA += libmmcamera_isp_template
MM_CAMERA += libmmcamera_le2464c_master_eeprom
MM_CAMERA += libmmcamera_dw9761b_2d_eeprom
MM_CAMERA += libmmcamera_sony_imx298_eeprom
MM_CAMERA += libmmcamera_sony_imx378_eeprom
MM_CAMERA += libchromatix_ov5695_a3_default_preview
MM_CAMERA += libchromatix_ov5695_a3_default_video
MM_CAMERA += libchromatix_ov5695_a3_hfr_120
MM_CAMERA += libchromatix_ov5695_a3_hfr_60
MM_CAMERA += libchromatix_ov5695_a3_hfr_90
MM_CAMERA += libchromatix_ov5695_common
MM_CAMERA += libchromatix_ov5695_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov5695_cpp_hfr_120
MM_CAMERA += libchromatix_ov5695_cpp_hfr_60
MM_CAMERA += libchromatix_ov5695_cpp_hfr_90
MM_CAMERA += libchromatix_ov5695_cpp_liveshot
MM_CAMERA += libchromatix_ov5695_cpp_preview
MM_CAMERA += libchromatix_ov5695_cpp_snapshot
MM_CAMERA += libchromatix_ov5695_cpp_us_chromatix
MM_CAMERA += libchromatix_ov5695_cpp_video_full
MM_CAMERA += libchromatix_ov5695_cpp_video
MM_CAMERA += libchromatix_ov5695_default_video
MM_CAMERA += libchromatix_ov5695_hfr_120
MM_CAMERA += libchromatix_ov5695_hfr_60
MM_CAMERA += libchromatix_ov5695_hfr_90
MM_CAMERA += libchromatix_ov5695_liveshot
MM_CAMERA += libchromatix_ov5695_postproc
MM_CAMERA += libchromatix_ov5695_preview
MM_CAMERA += libchromatix_ov5695_snapshot
MM_CAMERA += libchromatix_ov5695_video_full
MM_CAMERA += libchromatix_ov5695_zsl_preview
MM_CAMERA += libchromatix_ov5695_zsl_video
MM_CAMERA += libmmcamera_dummyalgo
MM_CAMERA += libchromatix_ov8865_default_video
MM_CAMERA += libchromatix_ov8865_postproc
MM_CAMERA += libchromatix_ov8865_preview
MM_CAMERA += libchromatix_ov8865_snapshot
MM_CAMERA += libchromatix_ov8865_video_full
MM_CAMERA += libchromatix_ov8865_cpp_hfr_60
MM_CAMERA += libchromatix_ov8865_cpp_hfr_90
MM_CAMERA += libchromatix_ov8865_cpp_liveshot
MM_CAMERA += libchromatix_ov8865_cpp_preview
MM_CAMERA += libchromatix_ov8865_cpp_snapshot
MM_CAMERA += libchromatix_ov8865_cpp_us_chromatix
MM_CAMERA += libchromatix_ov8865_cpp_video
MM_CAMERA += libchromatix_ov8865_cpp_video_full
MM_CAMERA += libchromatix_ov8865_hfr_60
MM_CAMERA += libchromatix_ov8865_hfr_90
MM_CAMERA += libchromatix_ov8865_hfr_120
MM_CAMERA += libchromatix_ov8865_liveshot
MM_CAMERA += libchromatix_ov8858_hfr_90
MM_CAMERA += libchromatix_ov8858_postproc
MM_CAMERA += libchromatix_ov8865_common
MM_CAMERA += libchromatix_ov8865_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov8865_cpp_hfr_120
MM_CAMERA += libchromatix_ov8865_default_preview_dw9714
MM_CAMERA += libchromatix_ov8865_default_video_dw9714
MM_CAMERA += libchromatix_ov8865_hfr_60_dw9714
MM_CAMERA += libchromatix_ov8865_hfr_90_dw9714
MM_CAMERA += libchromatix_ov8865_hfr_120_dw9714
MM_CAMERA += libchromatix_ov8865_zsl_preview
MM_CAMERA += libchromatix_ov8865_zsl_video
MM_CAMERA += libchromatix_ov8858_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov8858_cpp_hfr_60
MM_CAMERA += libchromatix_ov8858_cpp_hfr_90
MM_CAMERA += libchromatix_ov8858_cpp_hfr_120
MM_CAMERA += libchromatix_ov8858_cpp_liveshot
MM_CAMERA += libchromatix_ov8858_cpp_preview
MM_CAMERA += libchromatix_ov8858_cpp_snapshot
MM_CAMERA += libchromatix_ov8858_cpp_us_chromatix
MM_CAMERA += libchromatix_ov8858_cpp_video
MM_CAMERA += libchromatix_ov8858_hfr_60
MM_CAMERA += libchromatix_ov8858_hfr_120
MM_CAMERA += libchromatix_ov8858_zsl_preview
MM_CAMERA += libchromatix_ov8858_zsl_video
MM_CAMERA += libchromatix_ov4688_liveshot
MM_CAMERA += libchromatix_ov5670_f5670bq_liveshot
MM_CAMERA += libchromatix_ov8858_a3_default_preview
MM_CAMERA += libchromatix_ov8858_a3_default_video
MM_CAMERA += libchromatix_ov8858_a3_hfr_60
MM_CAMERA += libchromatix_ov8858_a3_hfr_90
MM_CAMERA += libchromatix_ov8858_a3_hfr_120
MM_CAMERA += libchromatix_ov12a10_zsl_preview_3a
MM_CAMERA += libchromatix_ov12a10_hfr_120_3a
MM_CAMERA += libchromatix_ov12a10_zsl_video_3a
MM_CAMERA += libchromatix_ov12a10_default_preview_3a
MM_CAMERA += libchromatix_ov12a10_postproc
MM_CAMERA += libchromatix_ov12a10_cpp_video
MM_CAMERA += libchromatix_ov12a10_common
MM_CAMERA += libchromatix_ov12a10_cpp_hfr_90
MM_CAMERA += libchromatix_ov12a10_cpp_hfr_60
MM_CAMERA += libchromatix_ov12a10_default_video_3a
MM_CAMERA += libchromatix_ov12a10_cpp_snapshot
MM_CAMERA += libchromatix_ov12a10_hfr_120
MM_CAMERA += libchromatix_ov12a10_preview
MM_CAMERA += libchromatix_ov12a10_default_video
MM_CAMERA += libchromatix_ov12a10_hfr_90_3a
MM_CAMERA += libchromatix_ov12a10_cpp_liveshot
MM_CAMERA += libchromatix_ov12a10_hfr_60_3a
MM_CAMERA += libchromatix_ov12a10_cpp_hfr_120
MM_CAMERA += libchromatix_ov12a10_hfr_90
MM_CAMERA += libchromatix_ov12a10_cpp_preview
MM_CAMERA += libchromatix_ov12a10_hfr_60
MM_CAMERA += libchromatix_ov12a10_snapshot
MM_CAMERA += libchromatix_ov12a10_default_video_4k
MM_CAMERA += libchromatix_ov12a10_4k_video_3a
MM_CAMERA += libchromatix_ov12a10_cpp_video_4k
MM_CAMERA += libchromatix_ov12a10_4k_preview_3a

MM_CAMERA += libchromatix_ov12a10_bear_zsl_preview_3a
MM_CAMERA += libchromatix_ov12a10_bear_hfr_120_3a
MM_CAMERA += libchromatix_ov12a10_bear_zsl_video_3a
MM_CAMERA += libchromatix_ov12a10_bear_default_preview_3a
MM_CAMERA += libchromatix_ov12a10_bear_postproc
MM_CAMERA += libchromatix_ov12a10_bear_cpp_video
MM_CAMERA += libchromatix_ov12a10_bear_common
MM_CAMERA += libchromatix_ov12a10_bear_cpp_hfr_90
MM_CAMERA += libchromatix_ov12a10_bear_cpp_hfr_60
MM_CAMERA += libchromatix_ov12a10_bear_default_video_3a
MM_CAMERA += libchromatix_ov12a10_bear_cpp_snapshot
MM_CAMERA += libchromatix_ov12a10_bear_hfr_120
MM_CAMERA += libchromatix_ov12a10_bear_preview
MM_CAMERA += libchromatix_ov12a10_bear_default_video
MM_CAMERA += libchromatix_ov12a10_bear_hfr_90_3a
MM_CAMERA += libchromatix_ov12a10_bear_cpp_liveshot
MM_CAMERA += libchromatix_ov12a10_bear_hfr_60_3a
MM_CAMERA += libchromatix_ov12a10_bear_cpp_hfr_120
MM_CAMERA += libchromatix_ov12a10_bear_hfr_90
MM_CAMERA += libchromatix_ov12a10_bear_cpp_preview
MM_CAMERA += libchromatix_ov12a10_bear_hfr_60
MM_CAMERA += libchromatix_ov12a10_bear_snapshot
MM_CAMERA += libchromatix_ov12a10_bear_default_video_4k
MM_CAMERA += libchromatix_ov12a10_bear_4k_video_3a
MM_CAMERA += libchromatix_ov12a10_bear_cpp_video_4k
MM_CAMERA += libchromatix_ov12a10_bear_4k_preview_3a
MM_CAMERA += ov12a10_bear_chromatix.xml

MM_CAMERA += libchromatix_ov8856_bear_3a_default_preview
MM_CAMERA += libchromatix_ov8856_bear_3a_default_video
MM_CAMERA += libchromatix_ov8856_bear_3a_hfr_60
MM_CAMERA += libchromatix_ov8856_bear_3a_hfr_90
MM_CAMERA += libchromatix_ov8856_bear_3a_hfr_120
MM_CAMERA += libchromatix_ov8856_bear_zsl_preview
MM_CAMERA += libchromatix_ov8856_bear_zsl_video
MM_CAMERA += libchromatix_ov8856_bear_common
MM_CAMERA += libchromatix_ov8856_bear_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov8856_bear_cpp_hfr_60
MM_CAMERA += libchromatix_ov8856_bear_cpp_hfr_90
MM_CAMERA += libchromatix_ov8856_bear_cpp_hfr_120
MM_CAMERA += libchromatix_ov8856_bear_cpp_liveshot
MM_CAMERA += libchromatix_ov8856_bear_cpp_preview
MM_CAMERA += libchromatix_ov8856_bear_cpp_snapshot
MM_CAMERA += libchromatix_ov8856_bear_cpp_us_chromatix
MM_CAMERA += libchromatix_ov8856_bear_cpp_video
MM_CAMERA += libchromatix_ov8856_bear_hfr_60
MM_CAMERA += libchromatix_ov8856_bear_hfr_90
MM_CAMERA += libchromatix_ov8856_bear_hfr_120
MM_CAMERA += libchromatix_ov8856_bear_liveshot
MM_CAMERA += libchromatix_ov8856_bear_preview
MM_CAMERA += libchromatix_ov8856_bear_snapshot
MM_CAMERA += libchromatix_ov8856_bear_default_video
MM_CAMERA += libchromatix_ov8856_bear_postproc
MM_CAMERA += ov8856_bear_chromatix.xml

MM_CAMERA += libchromatix_imx318_bear_1080p_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_1080p_video_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_4k_12M_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_4k_12M_video_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_4k_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_4k_video_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_common
MM_CAMERA += libchromatix_imx318_bear_cpp_hfr_120
MM_CAMERA += libchromatix_imx318_bear_cpp_hfr_240
MM_CAMERA += libchromatix_imx318_bear_cpp_hfr_60
MM_CAMERA += libchromatix_imx318_bear_cpp_liveshot
MM_CAMERA += libchromatix_imx318_bear_cpp_preview
MM_CAMERA += libchromatix_imx318_bear_cpp_snapshot
MM_CAMERA += libchromatix_imx318_bear_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx318_bear_cpp_snapshot_raw_hdr
MM_CAMERA += libchromatix_imx318_bear_cpp_video
MM_CAMERA += libchromatix_imx318_bear_cpp_video_4k
MM_CAMERA += libchromatix_imx318_bear_cpp_video_hdr
MM_CAMERA += libchromatix_imx318_bear_cpp_video_4k_raw_hdr
MM_CAMERA += libchromatix_imx318_bear_default_video
MM_CAMERA += libchromatix_imx318_bear_qtrsize_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_qtrsize_video_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_fullsize_preview_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_fullsize_video_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_hdr_snapshot_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_hdr_video_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_snapshot_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_full_snapshot_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_video_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_video_12M_raw_hdr_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_hfr_120
MM_CAMERA += libchromatix_imx318_bear_hfr_120_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_hfr_240
MM_CAMERA += libchromatix_imx318_bear_hfr_240_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_hfr_60
MM_CAMERA += libchromatix_imx318_bear_hfr_60_lc898212xd
MM_CAMERA += libchromatix_imx318_bear_liveshot
MM_CAMERA += libchromatix_imx318_bear_postproc
MM_CAMERA += libchromatix_imx318_bear_preview
MM_CAMERA += libchromatix_imx318_bear_snapshot
MM_CAMERA += libchromatix_imx318_bear_snapshot_hdr
MM_CAMERA += libchromatix_imx318_bear_snapshot_raw_hdr
MM_CAMERA += libchromatix_imx318_bear_video_4k
MM_CAMERA += libchromatix_imx318_bear_video_4k_raw_hdr
MM_CAMERA += libchromatix_imx318_bear_video_hdr
MM_CAMERA += imx318_bear_chromatix.xml

MM_CAMERA += libchromatix_ov8858_bear_common
MM_CAMERA += libchromatix_ov8858_bear_preview
MM_CAMERA += libchromatix_ov8858_bear_default_video
MM_CAMERA += libchromatix_ov8858_bear_snapshot
MM_CAMERA += libchromatix_ov8858_bear_hfr_120fps
MM_CAMERA += libchromatix_ov8858_bear_video_hd
MM_CAMERA += libchromatix_ov8858_bear_hfr_60fps
MM_CAMERA += libchromatix_ov8858_bear_zsl
MM_CAMERA += libchromatix_ov8858_bear_hfr_90fps
MM_CAMERA += libchromatix_ov8858_bear_liveshot
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_common
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_preview
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_default_video
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_snapshot
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_hfr_120fps
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_video_hd
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_hfr_60fps
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_zsl
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_zsl_30
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_hfr_90fps
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_liveshot
MM_CAMERA += libchromatix_ov8858_bear_q8v19w_video_nonHD
MM_CAMERA += libchromatix_ov8858_bear_hfr_90
MM_CAMERA += libchromatix_ov8858_bear_postproc
MM_CAMERA += libchromatix_ov8858_bear_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov8858_bear_cpp_hfr_60
MM_CAMERA += libchromatix_ov8858_bear_cpp_hfr_90
MM_CAMERA += libchromatix_ov8858_bear_cpp_hfr_120
MM_CAMERA += libchromatix_ov8858_bear_cpp_liveshot
MM_CAMERA += libchromatix_ov8858_bear_cpp_preview
MM_CAMERA += libchromatix_ov8858_bear_cpp_snapshot
MM_CAMERA += libchromatix_ov8858_bear_cpp_us_chromatix
MM_CAMERA += libchromatix_ov8858_bear_cpp_video
MM_CAMERA += libchromatix_ov8858_bear_hfr_60
MM_CAMERA += libchromatix_ov8858_bear_hfr_120
MM_CAMERA += libchromatix_ov8858_bear_zsl_preview
MM_CAMERA += libchromatix_ov8858_bear_zsl_video
MM_CAMERA += libchromatix_ov8858_bear_a3_default_preview
MM_CAMERA += libchromatix_ov8858_bear_a3_default_video
MM_CAMERA += libchromatix_ov8858_bear_a3_hfr_60
MM_CAMERA += libchromatix_ov8858_bear_a3_hfr_90
MM_CAMERA += libchromatix_ov8858_bear_a3_hfr_120
MM_CAMERA += ov8858_bear_chromatix.xml

MM_CAMERA += libchromatix_imx298_bear_4K_preview
MM_CAMERA += libchromatix_imx298_bear_4K_video
MM_CAMERA += libchromatix_imx298_bear_common
MM_CAMERA += libchromatix_imx298_bear_cpp_hfr_120
MM_CAMERA += libchromatix_imx298_bear_cpp_hfr_60
MM_CAMERA += libchromatix_imx298_bear_cpp_hfr_90
MM_CAMERA += libchromatix_imx298_bear_cpp_liveshot
MM_CAMERA += libchromatix_imx298_bear_cpp_preview
MM_CAMERA += libchromatix_imx298_bear_cpp_snapshot
MM_CAMERA += libchromatix_imx298_bear_cpp_snapshot_hdr
MM_CAMERA += libchromatix_imx298_bear_cpp_video
MM_CAMERA += libchromatix_imx298_bear_cpp_video_hdr
MM_CAMERA += libchromatix_imx298_bear_default_preview
MM_CAMERA += libchromatix_imx298_bear_default_video
MM_CAMERA += libchromatix_imx298_bear_hdr_snapshot_3a
MM_CAMERA += libchromatix_imx298_bear_hdr_video_3a
MM_CAMERA += libchromatix_imx298_bear_hfr_120
MM_CAMERA += libchromatix_imx298_bear_hfr_120_3a
MM_CAMERA += libchromatix_imx298_bear_hfr_60
MM_CAMERA += libchromatix_imx298_bear_hfr_60_3a
MM_CAMERA += libchromatix_imx298_bear_hfr_90
MM_CAMERA += libchromatix_imx298_bear_hfr_90_3a
MM_CAMERA += libchromatix_imx298_bear_liveshot
MM_CAMERA += libchromatix_imx298_bear_postproc
MM_CAMERA += libchromatix_imx298_bear_preview
MM_CAMERA += libchromatix_imx298_bear_snapshot
MM_CAMERA += libchromatix_imx298_bear_snapshot_hdr
MM_CAMERA += libchromatix_imx298_bear_video
MM_CAMERA += libchromatix_imx298_bear_video_hdr
MM_CAMERA += libchromatix_imx298_bear_zsl_preview
MM_CAMERA += libchromatix_imx298_bear_zsl_video
MM_CAMERA += imx298_bear_chromatix.xml

MM_CAMERA += libchromatix_s5k3l8_f3l8yam_common
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_cpp_hfr_120
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_cpp_hfr_60
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_cpp_hfr_90
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_cpp_liveshot
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_cpp_preview
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_cpp_snapshot
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_cpp_video
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_default_preview_dw9763
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_default_video_dw9763
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_default_video
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_hfr_120_dw9763
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_hfr_120
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_hfr_60_dw9763
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_hfr_60
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_hfr_90_dw9763
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_hfr_90
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_liveshot
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_postproc
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_preview
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_snapshot
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_zsl_preview_dw9763
MM_CAMERA += libchromatix_s5k3l8_f3l8yam_zsl_video_dw9763
MM_CAMERA += libchromatix_s5k3l8_common
MM_CAMERA += libchromatix_s5k3l8_cpp_hfr_120
MM_CAMERA += libchromatix_s5k3l8_cpp_hfr_60
MM_CAMERA += libchromatix_s5k3l8_cpp_hfr_90
MM_CAMERA += libchromatix_s5k3l8_cpp_liveshot
MM_CAMERA += libchromatix_s5k3l8_cpp_preview
MM_CAMERA += libchromatix_s5k3l8_cpp_snapshot
MM_CAMERA += libchromatix_s5k3l8_cpp_video
MM_CAMERA += libchromatix_s5k3l8_default_preview_ak7345
MM_CAMERA += libchromatix_s5k3l8_default_video_ak7345
MM_CAMERA += libchromatix_s5k3l8_default_video
MM_CAMERA += libchromatix_s5k3l8_hfr_120_ak7345
MM_CAMERA += libchromatix_s5k3l8_hfr_120
MM_CAMERA += libchromatix_s5k3l8_hfr_60_ak7345
MM_CAMERA += libchromatix_s5k3l8_hfr_60
MM_CAMERA += libchromatix_s5k3l8_hfr_90_ak7345
MM_CAMERA += libchromatix_s5k3l8_hfr_90
MM_CAMERA += libchromatix_s5k3l8_liveshot
MM_CAMERA += libchromatix_s5k3l8_postproc
MM_CAMERA += libchromatix_s5k3l8_preview
MM_CAMERA += libchromatix_s5k3l8_snapshot
MM_CAMERA += libchromatix_s5k3l8_zsl_preview_ak7345
MM_CAMERA += libchromatix_s5k3l8_zsl_video_ak7345
MM_CAMERA += libchromatix_s5k3l8_mono_common
MM_CAMERA += libchromatix_s5k3l8_mono_cpp_hfr_120
MM_CAMERA += libchromatix_s5k3l8_mono_cpp_hfr_60
MM_CAMERA += libchromatix_s5k3l8_mono_cpp_hfr_90
MM_CAMERA += libchromatix_s5k3l8_mono_cpp_liveshot
MM_CAMERA += libchromatix_s5k3l8_mono_cpp_preview
MM_CAMERA += libchromatix_s5k3l8_mono_cpp_snapshot
MM_CAMERA += libchromatix_s5k3l8_mono_cpp_video
MM_CAMERA += libchromatix_s5k3l8_mono_default_preview_ak7345
MM_CAMERA += libchromatix_s5k3l8_mono_default_video_ak7345
MM_CAMERA += libchromatix_s5k3l8_mono_default_video
MM_CAMERA += libchromatix_s5k3l8_mono_hfr_120_ak7345
MM_CAMERA += libchromatix_s5k3l8_mono_hfr_120
MM_CAMERA += libchromatix_s5k3l8_mono_hfr_60_ak7345
MM_CAMERA += libchromatix_s5k3l8_mono_hfr_60
MM_CAMERA += libchromatix_s5k3l8_mono_hfr_90_ak7345
MM_CAMERA += libchromatix_s5k3l8_mono_hfr_90
MM_CAMERA += libchromatix_s5k3l8_mono_liveshot
MM_CAMERA += libchromatix_s5k3l8_mono_postproc
MM_CAMERA += libchromatix_s5k3l8_mono_preview
MM_CAMERA += libchromatix_s5k3l8_mono_snapshot
MM_CAMERA += libchromatix_s5k3l8_mono_zsl_preview_ak7345
MM_CAMERA += libchromatix_s5k3l8_mono_zsl_video_ak7345
MM_CAMERA += libchromatix_s5k2l7_snapshot
MM_CAMERA += libchromatix_s5k2l7_video_4k
MM_CAMERA += libchromatix_s5k2l7_zsl_preview_ak7371
MM_CAMERA += libchromatix_s5k2l7_zsl_video_ak7371
MM_CAMERA += libchromatix_s5k2l7_4k_preview_ak7371
MM_CAMERA += libchromatix_s5k2l7_4k_video_ak7371
MM_CAMERA += libchromatix_s5k2l7_common
MM_CAMERA += libchromatix_s5k2l7_cpp_hfr_60
MM_CAMERA += libchromatix_s5k2l7_cpp_hfr_90
MM_CAMERA += libchromatix_s5k2l7_cpp_hfr_120
MM_CAMERA += libchromatix_s5k2l7_cpp_liveshot
MM_CAMERA += libchromatix_s5k2l7_cpp_preview
MM_CAMERA += libchromatix_s5k2l7_cpp_snapshot
MM_CAMERA += libchromatix_s5k2l7_cpp_video
MM_CAMERA += libchromatix_s5k2l7_cpp_video_4k
MM_CAMERA += libchromatix_s5k2l7_default_preview_ak7371
MM_CAMERA += libchromatix_s5k2l7_default_video_ak7371
MM_CAMERA += libchromatix_s5k2l7_hfr_60
MM_CAMERA += libchromatix_s5k2l7_hfr_60_ak7371
MM_CAMERA += libchromatix_s5k2l7_hfr_90
MM_CAMERA += libchromatix_s5k2l7_hfr_90_ak7371
MM_CAMERA += libchromatix_s5k2l7_hfr_120
MM_CAMERA += libchromatix_s5k2l7_hfr_120_ak7371
MM_CAMERA += libchromatix_s5k2l7_postproc
MM_CAMERA += libchromatix_s5k2l7_preview
MM_CAMERA += libchromatix_s5k2l7_8953_4k_preview_ak7371
MM_CAMERA += libchromatix_s5k2l7_8953_4k_video_ak7371
MM_CAMERA += libchromatix_s5k2l7_8953_common
MM_CAMERA += libchromatix_s5k2l7_8953_cpp_hfr_60
MM_CAMERA += libchromatix_s5k2l7_8953_cpp_hfr_90
MM_CAMERA += libchromatix_s5k2l7_8953_cpp_hfr_120
MM_CAMERA += libchromatix_s5k2l7_8953_cpp_liveshot
MM_CAMERA += libchromatix_s5k2l7_8953_cpp_preview
MM_CAMERA += libchromatix_s5k2l7_8953_cpp_snapshot
MM_CAMERA += libchromatix_s5k2l7_8953_cpp_video
MM_CAMERA += libchromatix_s5k2l7_8953_cpp_video_4k
MM_CAMERA += libchromatix_s5k2l7_8953_default_preview_ak7371
MM_CAMERA += libchromatix_s5k2l7_8953_default_video_ak7371
MM_CAMERA += libchromatix_s5k2l7_8953_hfr_60
MM_CAMERA += libchromatix_s5k2l7_8953_hfr_60_ak7371
MM_CAMERA += libchromatix_s5k2l7_8953_hfr_90
MM_CAMERA += libchromatix_s5k2l7_8953_hfr_90_ak7371
MM_CAMERA += libchromatix_s5k2l7_8953_hfr_120
MM_CAMERA += libchromatix_s5k2l7_8953_hfr_120_ak7371
MM_CAMERA += libchromatix_s5k2l7_8953_postproc
MM_CAMERA += libchromatix_s5k2l7_8953_preview
MM_CAMERA += libchromatix_s5k2l7_8953_snapshot
MM_CAMERA += libchromatix_s5k2l7_8953_video_4k
MM_CAMERA += libchromatix_s5k2l7_8953_zsl_preview_ak7371
MM_CAMERA += libchromatix_s5k2l7_8953_zsl_video_ak7371
MM_CAMERA += libchromatix_ov13880_default_video
MM_CAMERA += libchromatix_ov13880_hfr_120_3a
MM_CAMERA += libchromatix_ov13880_hfr_90_3a
MM_CAMERA += libchromatix_ov13880_hfr_90
MM_CAMERA += libchromatix_ov13880_cpp_liveshot
MM_CAMERA += libchromatix_ov13880_zsl_video_3a
MM_CAMERA += libchromatix_ov13880_preview
MM_CAMERA += libchromatix_ov13880_default_video_3a
MM_CAMERA += libchromatix_ov13880_cpp_hfr_120
MM_CAMERA += libchromatix_ov13880_default_preview_3a
MM_CAMERA += libchromatix_ov13880_cpp_hfr_60
MM_CAMERA += libchromatix_ov13880_snapshot
MM_CAMERA += libchromatix_ov13880_cpp_snapshot
MM_CAMERA += libchromatix_ov13880_hfr_60
MM_CAMERA += libchromatix_ov13880_cpp_preview
MM_CAMERA += libchromatix_ov13880_common
MM_CAMERA += libchromatix_ov13880_postproc
MM_CAMERA += libchromatix_ov13880_cpp_hfr_90
MM_CAMERA += libchromatix_ov13880_zsl_preview_3a
MM_CAMERA += libchromatix_ov13880_hfr_60_3a
MM_CAMERA += libchromatix_ov13880_cpp_video
MM_CAMERA += libchromatix_ov13880_hfr_120
MM_CAMERA += libchromatix_ov13880_default_video_4k
MM_CAMERA += libchromatix_ov13880_4k_video_3a
MM_CAMERA += libchromatix_ov13880_cpp_video_4k
MM_CAMERA += libchromatix_ov13880_4k_preview_3a
MM_CAMERA += libchromatix_ov13850_q13v06k_default_video
MM_CAMERA += libchromatix_ov13850_q13v06k_postproc
MM_CAMERA += libchromatix_ov13850_q13v06k_preview
MM_CAMERA += libchromatix_ov13850_q13v06k_snapshot
MM_CAMERA += libchromatix_ov13850_q13v06k_video_full
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_hfr_60
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_hfr_90
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_liveshot
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_preview
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_snapshot
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_us_chromatix
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_video
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_video_full
MM_CAMERA += libchromatix_ov13850_q13v06k_hfr_60
MM_CAMERA += libchromatix_ov13850_q13v06k_hfr_90
MM_CAMERA += libchromatix_ov13850_q13v06k_hfr_120
MM_CAMERA += libchromatix_ov13850_q13v06k_liveshot
MM_CAMERA += libchromatix_ov13850_postproc
MM_CAMERA += libchromatix_ov13850_q13v06k_common
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov13850_q13v06k_cpp_hfr_120
MM_CAMERA += libchromatix_ov13850_q13v06k_default_preview_bu64297
MM_CAMERA += libchromatix_ov13850_q13v06k_default_video_bu64297
MM_CAMERA += libchromatix_ov13850_q13v06k_hfr_60_bu64297
MM_CAMERA += libchromatix_ov13850_q13v06k_hfr_90_bu64297
MM_CAMERA += libchromatix_ov13850_q13v06k_hfr_120_bu64297
MM_CAMERA += libchromatix_ov13850_q13v06k_zsl_preview_bu64297
MM_CAMERA += libchromatix_ov13850_q13v06k_zsl_video_bu64297
MM_CAMERA += libchromatix_ov13850_video_full
MM_CAMERA += libchromatix_ov13850_cpp_hfr_60
MM_CAMERA += libchromatix_ov13850_cpp_hfr_90
MM_CAMERA += libchromatix_ov13850_cpp_hfr_120
MM_CAMERA += libchromatix_ov13850_cpp_liveshot
MM_CAMERA += libchromatix_ov13850_cpp_preview
MM_CAMERA += libchromatix_ov13850_cpp_snapshot
MM_CAMERA += libchromatix_ov13850_cpp_us_chromatix
MM_CAMERA += libchromatix_ov13850_cpp_video
MM_CAMERA += libchromatix_ov13850_cpp_video_full
MM_CAMERA += libchromatix_ov13850_hfr_60
MM_CAMERA += libchromatix_ov13850_hfr_90
MM_CAMERA += libchromatix_ov13850_hfr_120
MM_CAMERA += libchromatix_ov13850_liveshot
MM_CAMERA += libchromatix_imx214_hdr_snapshot_lc898122
MM_CAMERA += libchromatix_imx214_hdr_video_lc898122
MM_CAMERA += libchromatix_ov13850_cpp_ds_chromatix
MM_CAMERA += libchromatix_ov13850_default_preview_lc898212xd
MM_CAMERA += libchromatix_ov13850_default_video_lc898212xd
MM_CAMERA += libchromatix_ov13850_hfr_60_lc898212xd
MM_CAMERA += libchromatix_ov13850_hfr_90_lc898212xd
MM_CAMERA += libchromatix_ov13850_hfr_120_lc898212xd
MM_CAMERA += libchromatix_ov13850_zsl_preview_lc898212xd
MM_CAMERA += libchromatix_ov13850_zsl_video_lc898212xd
MM_CAMERA += libactuator_bu64297
MM_CAMERA += libactuator_dw9761b
MM_CAMERA += libactuator_dw9763
MM_CAMERA += libactuator_dw9763b
MM_CAMERA += libactuator_dw9800
MM_CAMERA += libchromatix_csidtg_common
MM_CAMERA += libchromatix_csidtg_cpp_preview
MM_CAMERA += libchromatix_csidtg_postproc
MM_CAMERA += libchromatix_csidtg_preview
MM_CAMERA += libchromatix_csidtg_zsl_preview

MM_CAMERA += camera_config.xml
MM_CAMERA += msm8996_camera.xml
MM_CAMERA += msm8952_camera.xml
MM_CAMERA += msm8937_camera.xml
MM_CAMERA += msm8953_camera.xml
MM_CAMERA += csidtg_camera.xml
MM_CAMERA += csidtg_chromatix.xml
MM_CAMERA += imx214_chromatix.xml
MM_CAMERA += imx230_chromatix.xml
MM_CAMERA += imx230_qc2002_chromatix.xml
MM_CAMERA += imx230_qc2002_with_gyro_chromatix.xml
MM_CAMERA += imx298_chromatix.xml
MM_CAMERA += imx298_gt24c64_chromatix.xml
MM_CAMERA += imx378_chromatix.xml
MM_CAMERA += imx258_chromatix.xml
MM_CAMERA += imx258_lc898217xc_chromatix.xml
MM_CAMERA += imx258_bear_chromatix.xml
MM_CAMERA += imx258_mono_chromatix.xml
MM_CAMERA += imx258_mono_bear_chromatix.xml
MM_CAMERA += imx318_chromatix.xml
MM_CAMERA += imx362_chromatix.xml
MM_CAMERA += imx362_chromatix_bear.xml
MM_CAMERA += imx362_gt24c64a_chromatix.xml
MM_CAMERA += imx376_chromatix.xml
MM_CAMERA += ov2680_chromatix.xml
MM_CAMERA += ov4688_chromatix.xml
MM_CAMERA += ov5695_chromatix.xml
MM_CAMERA += ov5670_chromatix.xml
MM_CAMERA += ov5670_f5670bq_chromatix.xml
MM_CAMERA += ov7251_chromatix.xml
MM_CAMERA += s5k3p8sp_chromatix.xml
MM_CAMERA += ov8858_chromatix.xml
MM_CAMERA += ov8865_chromatix.xml
MM_CAMERA += ov8856_chromatix.xml
MM_CAMERA += ov13850_chromatix.xml
MM_CAMERA += ov13880_chromatix.xml
MM_CAMERA += ov12a10_chromatix.xml
MM_CAMERA += ov13855_chromatix.xml
MM_CAMERA += ov2281_chromatix.xml
MM_CAMERA += s5k3l8_f3l8yam_chromatix.xml
MM_CAMERA += s5k3l8_chromatix.xml
MM_CAMERA += s5k3l8_mono_chromatix.xml
MM_CAMERA += s5k2l7_chromatix.xml
MM_CAMERA += s5k2l7sx_chromatix.xml
MM_CAMERA += s5k3m3sm_chromatix.xml
MM_CAMERA += s5k2l7_8953_chromatix.xml
MM_CAMERA += ov13850_q13v06k_chromatix.xml
MM_CAMERA += s5k3m2xm_chromatix.xml
MM_CAMERA += s5k3m2xm_chromatix_bear.xml
MM_CAMERA += s5k3m2xx_chromatix.xml
MM_CAMERA += s5k3p3sm_chromatix.xml
MM_CAMERA += sensors.hal.tof

MM_CAMERA += libchromaflash
MM_CAMERA += liboptizoom
MM_CAMERA += libseemore
MM_CAMERA += libblurbuster
MM_CAMERA += libclearsight
MM_CAMERA += libts_face_beautify_hal_system
MM_CAMERA += libubifocus_system
MM_CAMERA += libdualcameraddm_system
MM_CAMERA += libchromaflash_system
MM_CAMERA += liboptizoom_system
MM_CAMERA += libseemore_system
ifeq ($(TARGET_BOARD_AUTO),true)
MM_CAMERA_AIS := libais
MM_CAMERA_AIS += libais_client
MM_CAMERA_AIS += libais_config
MM_CAMERA_AIS += libais_ti960
MM_CAMERA_AIS += ais_server
MM_CAMERA_AIS += ais_v4l2_proxy
MM_CAMERA_AIS += libais_ba
MM_CAMERA_AIS += libais_test_util
MM_CAMERA_AIS += libais_log
MM_CAMERA_AIS += qcarcam_test
MM_CAMERA_AIS += qcarcam_config_single.xml
MM_CAMERA_AIS += ais_v4l2loopback_config.xml
endif

ifneq ($(TARGET_SCVE_DISABLED),true)
MM_CAMERA += libjni_trackingfocus
MM_CAMERA += libjni_panorama
endif
MM_CAMERA += libjni_makeupV2
MM_CAMERA += libjni_sharpshooter
MM_CAMERA += libjni_stillmore
MM_CAMERA += libjni_optizoom
MM_CAMERA += libjni_chromaflash
MM_CAMERA += libjni_ubifocus
MM_CAMERA += libjni_blurbuster
MM_CAMERA += libllvd_smore
MM_CAMERA += libllvd_sw_tnr
MM_CAMERA += libdualcameraddm
MM_CAMERA += libvideobokeh
MM_CAMERA += libubifocus
MM_CAMERA += libjni_dualcamera
MM_CAMERA += libts_detected_face_hal
MM_CAMERA += libts_detected_face_jni
MM_CAMERA += libts_face_beautify_hal
MM_CAMERA += libts_face_beautify_jni
MM_CAMERA += libjni_clearsight

#Deepportrait
MM_CAMERA += libjni_deepportrait
MM_CAMERA += libdeepportrait_system
MM_CAMERA += libSNPE_system
MM_CAMERA += libbcvsnpe_system
MM_CAMERA += libsnpe_adsp_system
MM_CAMERA += libsnpe_cdsp_system
MM_CAMERA += libsymphony-cpu_system
MM_CAMERA += libsymphonypower_system
MM_CAMERA += libc++_shared_system

#DeepDoom
MM_CAMERA += libjni_deepzoom
MM_CAMERA += libdeepzoom_system

#Gallery
MM_CAMERA += libfiltergenerator
MM_CAMERA += libhazebuster
MM_CAMERA += libtrueportrait
MM_CAMERA += libseestraight
MM_CAMERA += libtruescanner

MM_CAMERA += libjni_filtergenerator
MM_CAMERA += libjni_hazebuster
MM_CAMERA += libjni_trueportrait
MM_CAMERA += libjni_seestraight
MM_CAMERA += libjni_truescanner_v2

#CamX

# Camx Hardware Interface - CDK
$(call inherit-product-if-exists, $(QC_PROP_ROOT)/chi-cdk/product.mk)

MM_CAMERA += CAMERA_ICP.elf

MM_CAMERA += camera.qcom
MM_CAMERA += com.qti.eisv2
MM_CAMERA += com.qti.eisv3
MM_CAMERA += com.qti.chi.override
MM_CAMERA += com.qti.node.eisv2
MM_CAMERA += com.qti.node.eisv3
MM_CAMERA += com.qti.node.memcpy
MM_CAMERA += com.qti.node.dummyrtb
MM_CAMERA += com.qti.node.gpu
MM_CAMERA += com.qti.node.swregistration
MM_CAMERA += com.qti.sensormodule.altek_imx577.bin
MM_CAMERA += com.qti.sensormodule.liteon_imx318.bin
MM_CAMERA += com.qti.sensormodule.liteon_imx362.bin
MM_CAMERA += com.qti.sensormodule.semco_imx258.bin
MM_CAMERA += com.qti.sensormodule.ofilm_ov12a10.bin
MM_CAMERA += com.qti.sensormodule.ofilm_ov13880.bin
MM_CAMERA += com.qti.sensormodule.liteon_max7366_ov9282.bin
MM_CAMERA += com.qti.sensormodule.liteon_max7366_ov6211.bin
MM_CAMERA += com.qti.sensor.max7366_ov6211
MM_CAMERA += com.qti.sensor.max7366_ov9282
MM_CAMERA += com.qti.sensor.imx318
MM_CAMERA += com.qti.sensor.imx362
MM_CAMERA += com.qti.sensor.ov12a10
MM_CAMERA += com.qti.sensor.ov13880
MM_CAMERA += fdconfigpreview.bin
MM_CAMERA += fdconfigvideo.bin
MM_CAMERA += imx318tuned.bin
MM_CAMERA += imx258tuned.bin
MM_CAMERA += libcamxfdalgov7
MM_CAMERA += libcamxfdalgov7_system
MM_CAMERA += libcamxfdengine
MM_CAMERA += libcamxncs
MM_CAMERA += libcamxosutils

#to be removed after source code merges
MM_CAMERA += libcamxstaticaecalgo
MM_CAMERA += libcamxstaticafalgo
MM_CAMERA += libcamxstaticafdalgo
MM_CAMERA += libcamxstaticasdalgo
MM_CAMERA += libcamxstaticawbalgo
MM_CAMERA += libcom.qti.stats.aec
MM_CAMERA += libcom.qti.stats.af
MM_CAMERA += libcom.qti.stats.awb
MM_CAMERA += libcom.qti.stats.asd
MM_CAMERA += libcom.qtistatic.stats.asd
#end of can be removed after source code merges

MM_CAMERA += com.qtistatic.stats.aec
MM_CAMERA += com.qtistatic.stats.af
MM_CAMERA += com.qtistatic.stats.awb
MM_CAMERA += com.qti.stats.aec
MM_CAMERA += com.qti.stats.af
MM_CAMERA += com.qti.stats.afd
MM_CAMERA += com.qti.stats.asd
MM_CAMERA += com.qti.stats.awb
MM_CAMERA += com.qti.stats.pdlib
MM_CAMERA += com.qtistatic.stats.pdlib

#start HY11 temp fix
MM_CAMERA += com.qtistatic.stats.aec.so
MM_CAMERA += com.qtistatic.stats.af.so
MM_CAMERA += com.qtistatic.stats.awb.so
MM_CAMERA += com.qti.stats.aec.so
MM_CAMERA += com.qti.stats.af.so
MM_CAMERA += com.qti.stats.afd.so
MM_CAMERA += com.qti.stats.awb.so
MM_CAMERA += com.qti.stats.pdlib.so
MM_CAMERA += com.qti.stats.asd.so
MM_CAMERA += libcom.qti.stats.asd.so
#end HY11 temp fix

MM_CAMERA += libcamxstatscore
MM_CAMERA += libcamxstatsparser
MM_CAMERA += libcamxtintlessalgo
MM_CAMERA += libcamxutils
MM_CAMERA += libcom.qti.chinodeutils
MM_CAMERA += libsync
MM_CAMERA += stripinglib
MM_CAMERA += systemdefault.bin
MM_CAMERA += com.qti.hvx.addconstant
MM_CAMERA += com.qti.hvx.addconstant.so
MM_CAMERA += libdsp_streamer_skel
MM_CAMERA += libdsp_streamer_skel.so
MM_CAMERA += libdsp_streamer_add_constant
MM_CAMERA += libdsp_streamer_add_constant.so

MM_CAMERA += libmmcamera_ov5670_30010a3
MM_CAMERA += libchromatix_ov5670_30010a3_common
MM_CAMERA += libchromatix_ov5670_30010a3_preview
MM_CAMERA += libchromatix_ov5670_30010a3_default_video
MM_CAMERA += libchromatix_ov5670_30010a3_snapshot
MM_CAMERA += libchromatix_ov5670_30010a3_hfr_120fps
MM_CAMERA += libchromatix_ov5670_30010a3_video_hd
MM_CAMERA += libchromatix_ov5670_30010a3_hfr_60fps
MM_CAMERA += libchromatix_ov5670_30010a3_zsl
MM_CAMERA += libchromatix_ov5670_30010a3_hfr_90fps
MM_CAMERA += libchromatix_ov5670_30010a3_livesho

#MM_CORE
MM_CORE := libscale
MM_CORE += libmm-hdcpmgr
MM_CORE += libvpu
MM_CORE += libvfmclientutils
ifneq ($(TARGET_HAS_LOW_RAM),true)
MM_CORE += CABLService
MM_CORE += libdisp-aba
MM_CORE += libmm-abl
MM_CORE += libmm-abl-oem
MM_CORE += mm-pp-daemon
MM_CORE += SVIService
MM_CORE += libmm-qdcm
MM_CORE += libmm-disp-apis
MM_CORE += libmm-als
endif

ifneq ($(TARGET_HAS_LOW_RAM),true)
#DISPLAY_DPPS
DISPLAY_DPPS := mm-pp-dpps
DISPLAY_DPPS += ppd

#DISPLAY_SERVICE
DISPLAY_SERVICE := vendor.display.color@1.0-service
endif

#MM_COLOR_CONVERSION
MM_COLOR_CONVERSION := libtile2linear

#MM_COLOR_CONVERTOR
MM_COLOR_CONVERTOR := libmm-color-convertor
MM_COLOR_CONVERTOR += libI420colorconvert

#MM_GESTURES
MM_GESTURES := gesture_mouse.idc
MM_GESTURES += GestureOverlayService
MM_GESTURES += GestureTouchInjectionConfig
MM_GESTURES += GestureTouchInjectionService
MM_GESTURES += libgesture-core
MM_GESTURES += libmmgesture-activity-trigger
MM_GESTURES += libmmgesture-bus
MM_GESTURES += libmmgesture-camera
MM_GESTURES += libmmgesture-camera-factory
MM_GESTURES += libmmgesture-client
MM_GESTURES += libmmgesture-jni
MM_GESTURES += libmmgesture-linux
MM_GESTURES += libmmgesture-service
MM_GESTURES += mm-gesture-daemon

#MM_GRAPHICS
MM_GRAPHICS := a225_pfp.fw
MM_GRAPHICS += a225_pm4.fw
MM_GRAPHICS += a225p5_pm4.fw
MM_GRAPHICS += a300_pfp.fw
MM_GRAPHICS += a300_pm4.fw
MM_GRAPHICS += a330_pfp.fw
MM_GRAPHICS += a330_pm4.fw
MM_GRAPHICS += a420_pfp.fw
MM_GRAPHICS += a420_pm4.fw
MM_GRAPHICS += a530_pfp.fw
MM_GRAPHICS += a530_pm4.fw
MM_GRAPHICS += a530v1_pfp.fw
MM_GRAPHICS += a530v1_pm4.fw
MM_GRAPHICS += a530_gpmu.fw2
MM_GRAPHICS += a530v2_seq.fw2
MM_GRAPHICS += a530v3_gpmu.fw2
MM_GRAPHICS += a530v3_seq.fw2
MM_GRAPHICS += a530_zap.b00
MM_GRAPHICS += a530_zap.b01
MM_GRAPHICS += a530_zap.b02
MM_GRAPHICS += a530_zap.mdt
MM_GRAPHICS += a530_zap.elf
MM_GRAPHICS += a540_zap.b00
MM_GRAPHICS += a540_zap.b01
MM_GRAPHICS += a540_zap.b02
MM_GRAPHICS += a540_zap.mdt
MM_GRAPHICS += a540_zap.elf
MM_GRAPHICS += a512_zap.b00
MM_GRAPHICS += a512_zap.b01
MM_GRAPHICS += a512_zap.b02
MM_GRAPHICS += a512_zap.mdt
MM_GRAPHICS += a512_zap.elf
MM_GRAPHICS += a508_zap.b00
MM_GRAPHICS += a508_zap.b01
MM_GRAPHICS += a508_zap.b02
MM_GRAPHICS += a508_zap.mdt
MM_GRAPHICS += a508_zap.elf
MM_GRAPHICS += a506_zap.b00
MM_GRAPHICS += a506_zap.b01
MM_GRAPHICS += a506_zap.b02
MM_GRAPHICS += a506_zap.mdt
MM_GRAPHICS += a506_zap.elf
MM_GRAPHICS += a540_gpmu.fw2
MM_GRAPHICS += a630_sqe.fw
MM_GRAPHICS += a630_gmu.bin
MM_GRAPHICS += a630_zap.b00
MM_GRAPHICS += a630_zap.b01
MM_GRAPHICS += a630_zap.b02
MM_GRAPHICS += a630_zap.elf
MM_GRAPHICS += a630_zap.mdt
MM_GRAPHICS += a615_zap.b00
MM_GRAPHICS += a615_zap.b01
MM_GRAPHICS += a615_zap.b02
MM_GRAPHICS += a615_zap.elf
MM_GRAPHICS += a615_zap.mdt
MM_GRAPHICS += eglsubAndroid
MM_GRAPHICS += eglSubDriverAndroid
MM_GRAPHICS += gpu_dcvsd
MM_GRAPHICS += leia_pfp_470.fw
MM_GRAPHICS += leia_pm4_470.fw
MM_GRAPHICS += libadreno_utils
MM_GRAPHICS += libC2D2
MM_GRAPHICS += libCB
MM_GRAPHICS += libc2d2_z180
MM_GRAPHICS += libc2d2_a3xx
MM_GRAPHICS += libEGL_adreno
MM_GRAPHICS += libc2d30-a3xx
MM_GRAPHICS += libc2d30-a4xx
MM_GRAPHICS += libc2d30-a5xx
MM_GRAPHICS += libc2d30_bltlib
MM_GRAPHICS += libc2d30
MM_GRAPHICS += libgsl
MM_GRAPHICS += libGLESv2_adreno
MM_GRAPHICS += libGLESv2S3D_adreno
MM_GRAPHICS += libGLESv1_CM_adreno
MM_GRAPHICS += libllvm-a3xx
MM_GRAPHICS += libllvm-arm
MM_GRAPHICS += libllvm-glnext
MM_GRAPHICS += libllvm-qcom
MM_GRAPHICS += libOpenVG
MM_GRAPHICS += libOpenCL
MM_GRAPHICS += libOpenCL_system
MM_GRAPHICS += libplayback_adreno
MM_GRAPHICS += libq3dtools_adreno
MM_GRAPHICS += libq3dtools_esx
MM_GRAPHICS += libQTapGLES
MM_GRAPHICS += libsc-a2xx
MM_GRAPHICS += libsc-a3xx
MM_GRAPHICS += libsc-adreno.a
MM_GRAPHICS += ProfilerPlaybackTools
MM_GRAPHICS += yamato_pfp.fw
MM_GRAPHICS += yamato_pm4.fw
MM_GRAPHICS += librs_adreno
MM_GRAPHICS += libRSDriver_adreno
MM_GRAPHICS += libbccQTI
MM_GRAPHICS += android.hardware.renderscript@1.0-impl
MM_GRAPHICS += libintrinsics_skel
MM_GRAPHICS += librs_adreno_sha1
MM_GRAPHICS += libESXEGL_adreno
MM_GRAPHICS += libESXGLESv1_CM_adreno
MM_GRAPHICS += libESXGLESv2_adreno
MM_GRAPHICS += libRBEGL_adreno
MM_GRAPHICS += libRBGLESv1_CM_adreno
MM_GRAPHICS += libRBGLESv2_adreno
ifneq ($(TARGET_NOT_SUPPORT_VULKAN), true)
MM_GRAPHICS += vulkan.$(TARGET_BOARD_PLATFORM)
MM_GRAPHICS += vulkan.adreno
MM_GRAPHICS += libllvm-qgl
PRODUCT_PROPERTY_OVERRIDES += ro.hardware.vulkan=adreno
PRODUCT_PROPERTY_OVERRIDES += ro.hardware.egl=adreno
endif

#MM_HTTP
MM_HTTP := libmmipstreamaal
MM_HTTP += libmmipstreamnetwork
MM_HTTP += libmmipstreamutils
MM_HTTP += libmmiipstreammmihttp
MM_HTTP += libmmhttpstack
MM_HTTP += libmmipstreamsourcehttp
MM_HTTP += libmmqcmediaplayer
MM_HTTP += libmmipstreamdeal

#MM_DRMPLAY
MM_DRMPLAY := drmclientlib
MM_DRMPLAY += libDrmPlay

#MM_MUX
MM_MUX := libFileMux

#MM_OSAL
MM_OSAL := libmmosal
MM_OSAL += libmmosal_proprietary

#MM_PARSER
MM_PARSER := libASFParserLib
MM_PARSER += libExtendedExtractor
MM_PARSER += libmmparser
MM_PARSER += libmmparser_lite

#MM_QSM
MM_QSM := libmmQSM

#MM_RTP
MM_RTP := libmmrtpdecoder
MM_RTP += libmmrtpencoder

#MM_STEREOLIB
MM_STEREOLIB := libmmstereo

#MM_STILL
MM_STILL := libadsp_jpege_skel
MM_STILL += libgemini
MM_STILL += libimage-jpeg-dec-omx-comp
MM_STILL += libimage-jpeg-enc-omx-comp
MM_STILL += libimage-omx-common
MM_STILL += libjpegdhw
MM_STILL += libjpegehw
MM_STILL += libmmipl
MM_STILL += libmmjpeg
MM_STILL += libmmjps
MM_STILL += libmmmpo
MM_STILL += libmmmpod
MM_STILL += libmmqjpeg_codec
MM_STILL += libmmstillomx
MM_STILL += libqomx_jpegenc
MM_STILL += libqomx_jpegdec
MM_STILL += mm-jpeg-dec-test
MM_STILL += mm-jpeg-dec-test-client
MM_STILL += mm-jpeg-enc-test
MM_STILL += mm-jpeg-enc-test-client
MM_STILL += mm-jps-enc-test
MM_STILL += mm-mpo-enc-test
MM_STILL += mm-mpo-dec-test
MM_STILL += mm-qjpeg-dec-test
MM_STILL += mm-qjpeg-enc-test
MM_STILL += mm-qomx-ienc-test
MM_STILL += mm-qomx-idec-test
MM_STILL += test_gemini
MM_STILL += libjpegdmahw
MM_STILL += libmmqjpegdma
MM_STILL += qjpeg-dma-test
MM_STILL += libqomx_jpegenc_pipe

#MM_VIDEO
ifneq ($(call is-board-platform,sdm845),true)
MM_VIDEO := ast-mm-vdec-omx-test7k
MM_VIDEO += iv_h264_dec_lib
MM_VIDEO += iv_mpeg4_dec_lib
MM_VIDEO += libh264decoder
MM_VIDEO += libHevcSwDecoder
MM_VIDEO += liblasic
MM_VIDEO += libmm-adspsvc
MM_VIDEO += libmp4decoder
MM_VIDEO += libmp4decodervlddsp
MM_VIDEO += libOmxH264Dec
MM_VIDEO += libOmxIttiamVdec
MM_VIDEO += libOmxIttiamVenc
MM_VIDEO += libOmxMpeg4Dec
MM_VIDEO += libOmxOn2Dec
MM_VIDEO += libOmxrv9Dec
MM_VIDEO += libon2decoder
MM_VIDEO += librv9decoder
MM_VIDEO += libVenusMbiConv
MM_VIDEO += mm-vdec-omx-test
MM_VIDEO += mm-venc-omx-test
MM_VIDEO += msm-vidc-test
MM_VIDEO += venc-device-android
MM_VIDEO += venus-v1.b00
MM_VIDEO += venus-v1.b01
MM_VIDEO += venus-v1.b02
MM_VIDEO += venus-v1.b03
MM_VIDEO += venus-v1.b04
MM_VIDEO += venus-v1.mdt
MM_VIDEO += venus-v1.mbn
MM_VIDEO += venus.b00
MM_VIDEO += venus.b01
MM_VIDEO += venus.b02
MM_VIDEO += venus.b03
MM_VIDEO += venus.b04
MM_VIDEO += venus.mbn
MM_VIDEO += venus.mdt
MM_VIDEO += vidc_1080p.fw
MM_VIDEO += vidc.b00
MM_VIDEO += vidc.b01
MM_VIDEO += vidc.b02
MM_VIDEO += vidc.b03
MM_VIDEO += vidcfw.elf
MM_VIDEO += vidc.mdt
MM_VIDEO += vidc_720p_command_control.fw
MM_VIDEO += vidc_720p_h263_dec_mc.fw
MM_VIDEO += vidc_720p_h264_dec_mc.fw
MM_VIDEO += vidc_720p_h264_enc_mc.fw
MM_VIDEO += vidc_720p_mp4_dec_mc.fw
MM_VIDEO += vidc_720p_mp4_enc_mc.fw
MM_VIDEO += vidc_720p_vc1_dec_mc.fw
MM_VIDEO += vt-sim-test
MM_VIDEO += libgpustats
MM_VIDEO += libvqzip
ifeq ($(TARGET_BOARD_AUTO),true)
MM_VIDEO += libadspstats
MM_VIDEO += libdspVideoUtils_skel
MM_VIDEO += libvideoppPQ_skel
endif
endif

MM_VIDEO += libOmxVidEnc
MM_VIDEO += libOmxWmvDec
MM_VIDEO += libMpeg4SwEncoder
MM_VIDEO += libswvdec
MM_VIDEO += libVC1DecDsp_skel
MM_VIDEO += libVC1DecDsp_skel.so
MM_VIDEO += libVC1Dec
MM_VIDEO += libVC1Dec.so
MM_VIDEO += mm-vidc-omx-test
MM_VIDEO += mm-swvenc-test
MM_VIDEO += mm-swvdec-test
MM_VIDEO += libavenhancements
MM_VIDEO += libfastcrc
MM_VIDEO += libstreamparser
MM_VIDEO += libvideoutils
MM_VIDEO += libUBWC

#MM_VPU
MM_VPU := vpu.b00
MM_VPU += vpu.b01
MM_VPU += vpu.b02
MM_VPU += vpu.b03
MM_VPU += vpu.b04
MM_VPU += vpu.b05
MM_VPU += vpu.b06
MM_VPU += vpu.b07
MM_VPU += vpu.b08
MM_VPU += vpu.b09
MM_VPU += vpu.b10
MM_VPU += vpu.b11
MM_VPU += vpu.b12
MM_VPU += vpu.mbn
MM_VPU += vpu.mdt


#MODEM_APIS
MODEM_APIS := libadc
MODEM_APIS += libauth
MODEM_APIS += libcm
MODEM_APIS += libdsucsd
MODEM_APIS += libfm_wan_api
MODEM_APIS += libgsdi_exp
MODEM_APIS += libgstk_exp
MODEM_APIS += libisense
MODEM_APIS += libloc_api
MODEM_APIS += libmmgsdilib
MODEM_APIS += libmmgsdisessionlib
MODEM_APIS += libmvs
MODEM_APIS += libnv
MODEM_APIS += liboem_rapi
MODEM_APIS += libpbmlib
MODEM_APIS += libpdapi
MODEM_APIS += libpdsm_atl
MODEM_APIS += libping_mdm
MODEM_APIS += libplayready
MODEM_APIS += librfm_sar
MODEM_APIS += libsnd
MODEM_APIS += libtime_remote_atom
MODEM_APIS += libuim
MODEM_APIS += libvoem_if
MODEM_APIS += libwidevine
MODEM_APIS += libwms
MODEM_APIS += libcommondefs
MODEM_APIS += libcm_fusion
MODEM_APIS += libcm_mm_fusion
MODEM_APIS += libdsucsdappif_apis_fusion
MODEM_APIS += liboem_rapi_fusion
MODEM_APIS += libpbmlib_fusion
MODEM_APIS += libping_lte_rpc
MODEM_APIS += libwms_fusion

#MODEM_API_TEST
MODEM_API_TEST := librstf

#MPDCVS
MPDCVS := dcvsd
MPDCVS += mpdecision

#ENERGY-AWARENESS
ENERGY_AWARENESS := energy-awareness

#MPQ_COMMAND_IF
MPQ_COMMAND_IF := libmpq_ci
MPQ_COMMAND_IF += libmpqci_cimax_spi
MPQ_COMMAND_IF += libmpqci_tsc_ci_driver

#MPQ_PLATFORM
MPQ_PLATFORM := AvSyncTest
MPQ_PLATFORM += libavinput
MPQ_PLATFORM += libavinputhaladi
MPQ_PLATFORM += libfrc
MPQ_PLATFORM += libmpqaudiocomponent
MPQ_PLATFORM += libmpqaudiosettings
MPQ_PLATFORM += libmpqavs
MPQ_PLATFORM += libmpqavstreammanager
MPQ_PLATFORM += libmpqcore
MPQ_PLATFORM += libmpqplatform
MPQ_PLATFORM += libmpqplayer
MPQ_PLATFORM += libmpqplayerclient
MPQ_PLATFORM += libmpqsource
MPQ_PLATFORM += libmpqstobinder
MPQ_PLATFORM += libmpqstreambuffersource
MPQ_PLATFORM += libmpqtopology_cimax_mux_driver
MPQ_PLATFORM += libmpqtopology_ts_out_bridge_mux_driver
MPQ_PLATFORM += libmpqtopology_tsc_mux_driver
MPQ_PLATFORM += libmpqtsdm
MPQ_PLATFORM += libmpqutils
MPQ_PLATFORM += libmpqvcapsource
MPQ_PLATFORM += libmpqvcxo
MPQ_PLATFORM += libmpqvideodecoder
MPQ_PLATFORM += libmpqvideorenderer
MPQ_PLATFORM += libmpqvideosettings
MPQ_PLATFORM += librffrontend
MPQ_PLATFORM += libmpqtestpvr
MPQ_PLATFORM += MPQDvbCTestApp
MPQ_PLATFORM += MPQPlayerApp
MPQ_PLATFORM += MPQStrMgrTest
MPQ_PLATFORM += MPQUnitTest
MPQ_PLATFORM += MPQVideoRendererTestApp
MPQ_PLATFORM += mpqavinputtest
MPQ_PLATFORM += mpqi2ctest
MPQ_PLATFORM += mpqvcaptest
MPQ_PLATFORM += mpqdvbtestapps
MPQ_PLATFORM += mpqdvbservice

#MSM_IRQBALANCE
MSM_IRQBALANCE := msm_irqbalance

#N_SMUX
N_SMUX := n_smux

#NQ NFC Config files + firmware images
ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
NQ_NFC_PROP := libnfc-mtp_default.conf
NQ_NFC_PROP += libnfc-mtp_rf1.conf
NQ_NFC_PROP += libnfc-mtp_rf2.conf
NQ_NFC_PROP += libnfc-mtp-NQ3XX.conf
NQ_NFC_PROP += libnfc-mtp-NQ4XX.conf
NQ_NFC_PROP += libnfc-qrd_default.conf
NQ_NFC_PROP += libnfc-qrd_rf1.conf
NQ_NFC_PROP += libnfc-qrd_rf2.conf
NQ_NFC_PROP += libnfc-qrd-NQ3XX.conf
NQ_NFC_PROP += libnfc-qrd-NQ4XX.conf
NQ_NFC_PROP += libpn547_fw.so
NQ_NFC_PROP += libpn548ad_fw.so
NQ_NFC_PROP += libpn551_fw.so
NQ_NFC_PROP += libpn553_fw.so
NQ_NFC_PROP += libpn557_fw.so
endif

#OEM_SERVICES - system monitor shutdown modules
OEM_SERVICES := libSubSystemShutdown
OEM_SERVICES += libsubsystem_control
OEM_SERVICES += oem-services

#ONCRPC
ONCRPC := libdsm
ONCRPC += liboncrpc
ONCRPC += libping_apps
ONCRPC += libqueue
ONCRPC += ping_apps_client_test_0000

#PD_LOCATER - Service locater binary/libs
PD_LOCATER := pd-mapper
PD_LOCATER += libpdmapper
PD_LOCATER += libjson  # 3rd party support library

#PERF
PERF := QPerformance
PERF += libqti_performance
PERF += libqti-perfd-client
PERF += libqti-at
PERF += gamed
PERF += libqti-iop-client

PERF += libqti-iopd
PERF += libqti-iopd-client_system
PERF += vendor.qti.hardware.iop@1.0
PERF += vendor.qti.hardware.iop@1.0_vendor
PERF += vendor.qti.hardware.iop@1.0-service.rc
PERF += vendor.qti.hardware.iop@1.0-impl
PERF += vendor.qti.hardware.iop@1.0-service
PERF += vendor.qti.hardware.iop@2.0
PERF += vendor.qti.hardware.iop@2.0_vendor
PERF += vendor.qti.hardware.iop@2.0-service.rc
PERF += vendor.qti.hardware.iop@2.0-impl
PERF += vendor.qti.hardware.iop@2.0-service

PERF += libmemperf
PERF += libqti-perfd
PERF += libqti-perfd-client_system
PERF += libqti-util
PERF += libfeedbackhandler
PERF += vendor.qti.hardware.perf@1.0
PERF += vendor.qti.hardware.perf@1.0_vendor
PERF += vendor.qti.hardware.perf@1.0-service.rc
PERF += vendor.qti.hardware.perf@1.0-impl
PERF += vendor.qti.hardware.perf@1.0-service
PERF += android.hardware.tests.libhwbinder@1.0-impl
PERF += libperfgluelayer
PERF += liblearningmodule
PERF += libmeters
PERF += libreffeature
PERF += ReferenceFeature.xml
PERF += libprekill
PERF += libmeters-ns
PERF += PreKillFeature.xml
PERF += libmemperfd
PERF += AdaptLaunchFeature.xml
PERF += libadaptlaunch
PERF += libactionhelpers

PERF_DBG := perflock_native_test
PERF_DBG += perflock_native_stress_test
PERF_DBG += perfunittests
PERF_DBG += boostConfigParser
PERF_DBG += libqti-tests-mod1
PERF_DBG += libqti-tests-mod2
PERF_DBG += libqti-tests-mod3
PERF_DBG += libqti-tests-mod4
PERF_DBG += libqti-tests-mod5
PERF_DBG += libqti-tests-mod6

#LATENCY
LATENCY := vendor.qti.hardware.data.latency@1.0
LATENCY += vendor.qti.hardware.data.latency@1.0_vendor
LATENCY += vendor.qti.hardware.data.latency-V1.0-java
LATENCY += android.hardware.tests.libhwbinder@1.0-impl
LATENCY += vendor.qti.hardware.data.latency.xml

#PLAYREADY
PLAYREADY := drmtest
PLAYREADY += libdrmfs
PLAYREADY += libdrmMinimalfs
PLAYREADY += libdrmtime
PLAYREADY += libtzplayready
PLAYREADY += libbase64
PLAYREADY += libprpk3
PLAYREADY += libprdrmengine

#PLAYREADY3.0 PRODUCT LIST
PLAYREADY_3_PRODUCT_LIST:= apq8098_latv
#PERIPHERAL MANAGER:
PERMGR := pm-service
PERMGR += libperipheral_client
PERMGR += pm-proxy

#PROFILER
PROFILER := profiler_tester
PROFILER += profiler_daemon
PROFILER += libprofiler_msmadc

#TREPN
TREPN := Trepn

#QCHAT
QCHAT := QComQMIPermissions

#QCNVITEM
QCNVITEM := qcnvitems
QCNVITEM += qcnvitems.xml

ifneq ($(strip $(TARGET_USES_IOTCC_HEADLESS)),true)
#QCRIL
QCRIL := libril-qc-1
QCRIL += libril-qc-qmi-1
QCRIL += libril-qc-hal-qmi
QCRIL += libril-qcril-hook-oem
QCRIL += qcrilhook
QCRIL += qcrilmsgtunnel
QCRIL += qcrilhook.xml
QCRIL += libwmsts
QCRIL += libril-qc-radioconfig
QCRIL += libril-qc-ltedirectdisc
QCRIL += librilqmiservices
QCRIL += libsettings
QCRIL += liblqe
QCRIL += lib_remote_simlock
QCRIL += vendor.qti.hardware.radio.am@1.0_vendor
QCRIL += vendor.qti.hardware.radio.ims@1.0_vendor
QCRIL += vendor.qti.hardware.radio.lpa@1.0_vendor
QCRIL += vendor.qti.hardware.radio.qcrilhook@1.0_vendor
QCRIL += vendor.qti.hardware.radio.qtiradio@1.0_vendor
QCRIL += vendor.qti.hardware.radio.uim@1.0_vendor
QCRIL += vendor.qti.hardware.radio.uim@1.1_vendor
QCRIL += vendor.qti.hardware.radio.uim_remote_client@1.0_vendor
QCRIL += vendor.qti.hardware.radio.uim_remote_server@1.0_vendor
endif

#QMI
QMI := check_system_health
QMI := irsc_util
QMI += libidl
QMI += libqcci_legacy
QMI += libqmi
QMI += libqmi_cci
QMI += libqmi_client_helper
QMI += libqmi_common_so
QMI += libqmi_csi
QMI += libqmi_encdec
QMI += libsmemlog
QMI += libmdmdetect
QMI += libqmiservices
QMI += qmiproxy
QMI += qmi_config.xml
QMI += qmi_test_mt_client_init_instance
QMI += qmuxd
QMI += libqmi_cci_system
QMI += libqrtr
QMI += qrtr-cfg
QMI += qrtr-ns
QMI += qrtr-lookup

#QOSMGR
QOSMGR := qosmgr
QOSMGR += qosmgr_rules.xml

QRD_CALENDAR_APPS += LunarInfoProvider

QRD_APPS += PowerOffMusic
#QUICKCHARGE
QUICKCHARGE := hvdcp

#QVR
QVR := 6dof_config.xml
QVR += qvrservice_config.txt
QVR += libqvrservice_ov7251_hvx_tuning
QVR += libqvrservice_ov9282_hvx_tuning
QVR += qvrservice
ifeq ($(call is-board-platform-in-list,sdm845),true)
QVR += lldservice
endif
QVR += qvrservicetest
QVR += qvrservicetest64
QVR += qvrcameratest
QVR += qvrcameratest64
QVR += qvrcameratseq
QVR += qvrcameratseq64
QVR += libqvr_cdsp_driver_stub
QVR += libqvr_dsp_driver_skel
QVR += libqvr_mapper_stub
QVR += libqvr_mapper_skel
QVR += libtracker_6dof_skel
QVR += libVIOMapping_6dof_skel
QVR += libqvrservice
QVR += libqvrservice_client
ifeq ($(call is-board-platform-in-list,sdm845),true)
QVR += liblldservice_client.qti
endif
QVR += libqvrcamera_client
QVR += libqvrcamera_client_system
QVR += libqvrservice_hvxcameraclient
QVR += libdsp_streamer_qvrcam_receiver
QVR += libqvr_cam_cdsp_driver_stub
QVR += libqvr_cam_dsp_driver_skel
QVR += libqvr_eyetracking_plugin
QVR += libeye_tracking_dsp_sample_stub
QVR += libeye_tracking_dsp_sample_skel
QVR += qvrdatalogger

#REMOTEFS
REMOTEFS := rmt_storage

#RFS_ACCESS
RFS_ACCESS := rfs_access

# RIDL/LogKit II
RIDL_BINS := RIDL_KIT
RIDL_BINS += libsregex
RIDL_BINS += libtar
RIDL_BINS += RIDLClient.exe
RIDL_BINS += RIDLClient
RIDL_BINS += RIDL.db
RIDL_BINS += qdss.cfg
RIDL_BINS += GoldenLogmask.dmc
RIDL_BINS += OTA-Logs.dmc
RIDL_BINS += liblz4

#RNGD
RNGD := rngd

#SCVE
SCVE := _conf_eng_num_sym_font40_4transd_zscore_morph_.trn2876.trn
SCVE += _conf_eng_num_sym_font40_conc2_meshrn__de__1_1__zscore_morph.trn10158.trn
SCVE += _conf_eng_num_sym_font40_rbp_data5100_patch500_5x5_24x24_dim727.trn31585.trn
SCVE += _eng_font40_4transmeshrnorm6x6_leaflda85_ligature_ext14_c70_sp1lI_newxml3.trn31299.trn
SCVE += _numpunc_font40_4transmeshrnorm_leafnum1.trn9614.trn
SCVE += _numpunc_font40_conc2_DEFn__BGTouchy6x6n__1_1__morph.trn32025.trn
SCVE += _numpunc_parteng_font40_4transmeshr_morph.trn400.trn
SCVE += character.cost
SCVE += CharType.dat
SCVE += chinese.lm
SCVE += chinese_address_20150304.bin
SCVE += ChinesePunctuation.rst
SCVE += cnn_small5_synthrev_sw_sampled2_bin
SCVE += dcnConfigForEngCNN.txt
SCVE += dcnConfigForMultiCNN.txt
SCVE += english_address_20150213.bin
SCVE += english_dictionary_20150213.bin
SCVE += forestData.bin
SCVE += glvq_kor_2197classes_576_100dim_i42_centroidNorm.dat
SCVE += glvq_kor_consonant_19classes_64_16dim_i0_linearNorm.dat
SCVE += GLVQDecoder_fixed.ohie
SCVE += gModel.dat
SCVE += gModel_system.dat
SCVE += gModel.dat_system
SCVE += hGLVQ_kor_RLF80_float.hie
SCVE += korean.lm
SCVE += korean_address_20150129.bin
SCVE += korean_dictionary_20150414.bin
SCVE += LDA_kor_2197classes_576dim_centroidNorm.dat
SCVE += LDA_kor_consonant_19classes_64dim_linearNorm.dat
SCVE += libhvxMathVIO
SCVE += libhvxMathVIO.so
SCVE += libobjectMattingApp_skel
SCVE += libobjectMattingApp_skel.so
SCVE += libscveBlobDescriptor
SCVE += libscveBlobDescriptor_skel
SCVE += libscveBlobDescriptor_skel.so
SCVE += libscveBlobDescriptor_stub
SCVE += libscveCleverCapture
SCVE += libscveCleverCapture_skel
SCVE += libscveCleverCapture_skel.so
SCVE += libscveCleverCapture_stub
SCVE += libscveCommon
SCVE += libscveCommon_stub
SCVE += libscveFaceLandmark_skel
SCVE += libscveFaceLandmark_skel.so
SCVE += libscveFaceLandmarks
SCVE += libscveFaceLandmarks_stub
SCVE += libscveFaceRecognition
SCVE += libscveFaceRecognition_skel
SCVE += libscveFaceRecognition_skel.so
SCVE += libscveFaceRecognition_stub
SCVE += libscveImageCloning
SCVE += libscveImageCorrection
SCVE += libscveImageRemoval
SCVE += libscveMotionVector
SCVE += libscveObjectMatting
SCVE += libscveObjectMatting_stub
SCVE += libscveObjectSegmentation
SCVE += libscveObjectSegmentation_skel
SCVE += libscveObjectSegmentation_skel.so
SCVE += libscveObjectSegmentation_stub
SCVE += libscveObjectTracker
SCVE += libscveObjectTracker_stub
SCVE += libscvePanorama
SCVE += libscvePanorama_lite
SCVE += libscveScan3D
SCVE += libscveT2T_skel
SCVE += libscveT2T_skel.so
SCVE += libscveTextReco
SCVE += libscveTextReco_skel
SCVE += libscveTextReco_skel.so
SCVE += libscveTextReco_stub
SCVE += libscveTextRecoPostProcessing
SCVE += libscveVideoSummary
SCVE += libscveVideoSummary_skel
SCVE += libscveVideoSummary_skel.so
SCVE += libscveVideoSummary_stub
SCVE += libobjectMattingApp_skel_system
SCVE += libobjectMattingApp_skel_system.so
SCVE += libscveBlobDescriptor_system
SCVE += libscveBlobDescriptor_skel_system
SCVE += libscveBlobDescriptor_skel_system.so
SCVE += libscveBlobDescriptor_stub_system
SCVE += libscveCleverCapture_system
SCVE += libscveCleverCapture_skel_system
SCVE += libscveCleverCapture_skel_system.so
SCVE += libscveCleverCapture_stub_system
SCVE += libscveCommon_system
SCVE += libscveCommon_stub_system
SCVE += libscveFaceLandmark_skel_system
SCVE += libscveFaceLandmark_skel_system.so
SCVE += libscveFaceLandmarks_system
SCVE += libscveFaceLandmarks_stub_system
SCVE += libscveFaceRecognition_system
SCVE += libscveFaceRecognition_skel_system
SCVE += libscveFaceRecognition_skel_system.so
SCVE += libscveFaceRecognition_stub_system
SCVE += libscveImageCloning_system
SCVE += libscveImageCorrection_system
SCVE += libscveImageRemoval_system
SCVE += libscveMotionVector_system
SCVE += libscveObjectMatting_system
SCVE += libscveObjectMatting_stub_system
SCVE += libscveObjectSegmentation_system
SCVE += libscveObjectSegmentation_skel_system
SCVE += libscveObjectSegmentation_skel_system.so
SCVE += libscveObjectSegmentation_stub_system
SCVE += libscveObjectTracker_system
SCVE += libscveObjectTracker_stub_system
SCVE += libscvePanorama_system
SCVE += libscvePanorama_lite_system
SCVE += libscveScan3D_system
SCVE += libhvxMathVIO_system
SCVE += libhvxMathVIO_system.so
SCVE += libscveT2T_skel_system
SCVE += libscveT2T_skel_system.so
SCVE += libscveTextReco_system
SCVE += libscveTextReco_skel_system
SCVE += libscveTextReco_skel_system.so
SCVE += libscveTextReco_stub_system
SCVE += libscveTextRecoPostProcessing_system
SCVE += libscveVideoSummary_system
SCVE += libscveVideoSummary_skel_system
SCVE += libscveVideoSummary_skel_system.so
SCVE += libscveVideoSummary_stub_system
SCVE += nontextremoval_eng.model
SCVE += nontextremoval_multilang.model
SCVE += punRangeData.rst
SCVE += libscveMotionTracker
SCVE += libscveMotionTracker_system
SCVE += libscveMotionTracker_stub
SCVE += libscveMotionTracker_stub_system
SCVE += libscveMotionTracker_skel_system
SCVE += libscveMotionTracker_skel_system.so
SCVE += libscveMotionTracker_skel
SCVE += libscveMotionTracker_skel.so
#SCVE TEST BINARIES
SCVE += scveTestFaceLandmarks_system
SCVE += scveTestFaceLandmarks
SCVE += scveTestPanorama_system
SCVE += scveTestPanorama
SCVE += scveTestObjectTracker_system
SCVE += scveTestObjectTracker
SCVE += scveTestFaceRecognition_system
SCVE += scveTestFaceRecognition
SCVE += scveTestImageCorrection_system
SCVE += scveTestImageCorrection
SCVE += scveTestObjectSegmentation_system
SCVE += scveTestObjectSegmentation
SCVE += scveTestScan3D_system
SCVE += scveTestScan3D
SCVE += scveTestMotionTracker_system
SCVE += scveTestMotionTracker


#SECUREMSM
SECUREMSM :=

ifeq ($(KMGK_USE_QTI_SERVICE),true)
SECUREMSM += android.hardware.gatekeeper@1.0-impl-qti
SECUREMSM += android.hardware.gatekeeper@1.0-service-qti
SECUREMSM += android.hardware.gatekeeper@1.0-service-qti.rc
SECUREMSM += android.hardware.keymaster@3.0-impl-qti
SECUREMSM += android.hardware.keymaster@3.0-service-qti
SECUREMSM += android.hardware.keymaster@3.0-service-qti.rc
else
SECUREMSM += android.hardware.gatekeeper@1.0-impl
SECUREMSM += android.hardware.gatekeeper@1.0-service
SECUREMSM += android.hardware.keymaster@3.0-impl
SECUREMSM += android.hardware.keymaster@3.0-service
endif # KMGK_USE_QTI_SERVICE

ifeq ($(ENABLE_HYP),true)

SECUREMSM += gatekeeper.$(TARGET_BOARD_PLATFORM)
SECUREMSM += libQSEEComAPI
SECUREMSM += libQSEEComAPIStatic
SECUREMSM += qseecom_sample_client
SECUREMSM += qseecomd
SECUREMSM += libkeymasterprovision
SECUREMSM += KmInstallKeybox
SECUREMSM += libkeymasterutils
SECUREMSM += libkeymasterdeviceutils
SECUREMSM += qti_keymaster_tests


else

SECUREMSM += vendor.qti.hardware.tui_comm@1.0
SECUREMSM += vendor.qti.hardware.tui_comm@1.0_vendor
SECUREMSM += vendor.qti.hardware.tui_comm@1.0-impl-qti
SECUREMSM += vendor.qti.hardware.tui_comm@1.0-service-qti
SECUREMSM += vendor.qti.hardware.tui_comm@1.0-service-qti.rc
SECUREMSM += vendor.qti.hardware.qdutils_disp@1.0
SECUREMSM += vendor.qti.hardware.qdutils_disp@1.0_vendor
SECUREMSM += vendor.qti.hardware.qdutils_disp@1.0-impl-qti
SECUREMSM += vendor.qti.hardware.qdutils_disp@1.0-service-qti
SECUREMSM += vendor.qti.hardware.qdutils_disp@1.0-service-qti.rc
SECUREMSM += vendor.qti.hardware.seccam@1.0
SECUREMSM += vendor.qti.hardware.seccam@1.0_vendor
SECUREMSM += vendor.qti.hardware.seccam@1.0-service-qti
SECUREMSM += vendor.qti.hardware.seccam@1.0-service-qti.rc
SECUREMSM += aostlmd
SECUREMSM += com.qualcomm.qti.services.secureui
SECUREMSM += chamomile_provision
SECUREMSM += dbAccess
SECUREMSM += default_qti_regular_37.bin
SECUREMSM += default_qti_regular_43.bin
SECUREMSM += default_qti_regular_90.bin
SECUREMSM += drm_generic_prov_test
SECUREMSM += e2image_blocks
SECUREMSM += FidoCryptoService
SECUREMSM += filefrag_blocks
SECUREMSM += FidoSuiService
SECUREMSM += gatekeeper.$(TARGET_BOARD_PLATFORM)
SECUREMSM += gpfspath_oem_config.xml
SECUREMSM += isdbtmmtest
SECUREMSM += InstallKeybox
SECUREMSM += keystore.$(TARGET_BOARD_PLATFORM)
SECUREMSM += KmInstallKeybox
SECUREMSM += libChamomilePA
SECUREMSM += libcppf
SECUREMSM += libdrmprplugin
SECUREMSM += libdrmprplugin_customer
SECUREMSM += libFidoCrypto_system
SECUREMSM += libFidoCryptov1_system
SECUREMSM += libFidoCrypto_vendor
SECUREMSM += libFidoCryptov1_vendor
SECUREMSM += libFidoCryptoJNI
SECUREMSM += libFIDOKeyProvisioning
SECUREMSM += libFidoSuiJNI
SECUREMSM += libprdrmdecrypt
SECUREMSM += libprdrmdecrypt_customer
SECUREMSM += libprmediadrmdecrypt
SECUREMSM += libprmediadrmdecrypt_customer
SECUREMSM += libprmediadrmplugin
SECUREMSM += libprmediadrmplugin_customer
SECUREMSM += libdrmfs
SECUREMSM += libdrmMinimalfs
SECUREMSM += libdrmMinimalfsHelper
SECUREMSM += libdrmtime
SECUREMSM += libgoogletest
SECUREMSM += libGPreqcancel
SECUREMSM += libGPreqcancel_svc
SECUREMSM += libGPTEE_system
SECUREMSM += libGPTEE_vendor
SECUREMSM += libhdcpsrm
SECUREMSM += libkeymasterdeviceutils
SECUREMSM += libkeymasterprovision
SECUREMSM += libkeymasterutils
SECUREMSM += libtzplayready
SECUREMSM += libbase64
SECUREMSM += libtzplayready_customer
SECUREMSM += libprpk3
SECUREMSM += libprdrmengine
SECUREMSM += libtzdrmgenprov
SECUREMSM += liboemcrypto
SECUREMSM += liboemcrypto.a
SECUREMSM += libcpion
SECUREMSM += libQSEEComAPI
SECUREMSM += libQSEEComAPIStatic
SECUREMSM += libMcClient
SECUREMSM += libMcCommon
SECUREMSM += libMcDriverDevice
SECUREMSM += libMcRegistry
SECUREMSM += libmdtp
SECUREMSM += libmdtp_crypto
SECUREMSM += libmdtpdemojni
SECUREMSM += libPaApi
SECUREMSM += libqmp_sphinx_jni
SECUREMSM += libqmp_sphinxlog
SECUREMSM += libqmpart
SECUREMSM += librmp
SECUREMSM += libpvr
SECUREMSM += librpmb
SECUREMSM += librpmbStatic
SECUREMSM += librpmbStaticHelper
SECUREMSM += libSampleAuthJNI
SECUREMSM += libSampleExtAuthJNI
SECUREMSM += libsecotacommon
SECUREMSM += libsecotanservice
SECUREMSM += libSecureSampleAuthJNI
SECUREMSM += libSecureExtAuthJNI
SECUREMSM += lib-sec-disp
SECUREMSM += libsecureui
SECUREMSM += libsecureui_svcsock
SECUREMSM += libsecureui_svcsock_system
SECUREMSM += libSecureUILib
SECUREMSM += libsecureuisvc_jni
SECUREMSM += libseemp_binder
SECUREMSM += libseempnative
SECUREMSM += libSeemplog
SECUREMSM += libSeempMsgService
SECUREMSM += libsi
SECUREMSM += libsmcinvokecred
SECUREMSM += libspcom
SECUREMSM += libspiris
SECUREMSM += libspl
SECUREMSM += spdaemon
SECUREMSM += sec_nvm
SECUREMSM += libssd
SECUREMSM += libssdStatic
SECUREMSM += libssdStaticHelper
SECUREMSM += libqsappsver
SECUREMSM += libStDrvInt
SECUREMSM += libTLV
SECUREMSM += libtzcom
SECUREMSM += libqcbor
SECUREMSM += libqisl
SECUREMSM += libseccam
SECUREMSM += seccamservice
SECUREMSM += mcDriverDaemon
SECUREMSM += mdtp_fota
SECUREMSM += mdtp_ut
SECUREMSM += mdtp.img
SECUREMSM += mdtp
SECUREMSM += mdtpd
SECUREMSM += MdtpService
SECUREMSM += MdtpDemo
SECUREMSM += oemwvtest
SECUREMSM += qfipsverify
SECUREMSM += qfipsverify.hmac
SECUREMSM += bootimg.hmac
SECUREMSM += libDevHealth
SECUREMSM += seemp_healthd
SECUREMSM += libHealthAuthClient
SECUREMSM += libHealthAuthJNI
SECUREMSM += liblmclient
SECUREMSM += libippclient
SECUREMSM += HealthAuthService
SECUREMSM += pvclicense_sample
SECUREMSM += qseecom_sample_client
SECUREMSM += qseecom_security_test
SECUREMSM += qseecomd
SECUREMSM += qti_gatekeeper_tests
SECUREMSM += qti_keymaster_tests
SECUREMSM += SampleAuthenticatorService
SECUREMSM += SampleExtAuthService
SECUREMSM += SecotaAPI
SECUREMSM += secotad
SECUREMSM += SecotaService
SECUREMSM += SecureExtAuthService
SECUREMSM += SecureSampleAuthService
SECUREMSM += secure_ui_sample_client
SECUREMSM += seemp
SECUREMSM += SeempAPI
SECUREMSM += seemp_cli
SECUREMSM += SeempCommon
SECUREMSM += seempd
SECUREMSM += SeempService
SECUREMSM += smcinvoked
SECUREMSM += smcinvokepkgmgr
SECUREMSM += SoterProvisioningTool
SECUREMSM += soter_client
SECUREMSM += StoreKeybox
SECUREMSM += sphinxproxy
SECUREMSM += TelemetryService
SECUREMSM += tbaseLoader
SECUREMSM += widevinetest
SECUREMSM += widevinetest_rpc
SECUREMSM += hdcp1prov
SECUREMSM += libhdcp1prov
SECUREMSM += hdcp2p2prov
SECUREMSM += hdcp_srm
SECUREMSM += libhdcp2p2prov
SECUREMSM += tloc_daemon
SECUREMSM += libQTEEConnector_system
SECUREMSM += libQTEEConnector_vendor
SECUREMSM += VtsHalDataLatencyV1_0TargetTest
SECUREMSM += vendor.qti.hardware.qteeconnector@1.0
SECUREMSM += vendor.qti.hardware.qteeconnector@1.0_vendor
SECUREMSM += vendor.qti.hardware.qteeconnector@1.0-impl
SECUREMSM += vendor.qti.hardware.qteeconnector@1.0-service
SECUREMSM += vendor.qti.hardware.qteeconnector@1.0-service.rc
SECUREMSM += vendor.qti.esepowermanager@1.0-service
SECUREMSM += vendor.qti.esepowermanager@1.0-impl
SECUREMSM += vendor.qti.esepowermanager@1.0-service.rc
SECUREMSM += vendor.qti.esepowermanager@1.0
SECUREMSM += vendor.qti.esepowermanager@1.0_vendor

endif

#SENSORS
ifeq ($(TARGET_BOARD_AUTO),true)
SENSORS += sensors.msm8996_auto
SENSORS += bst_hal_cfg.txt
endif
SENSORS := activity_recognition.apq8084
SENSORS += activity_recognition.msm8226
SENSORS += activity_recognition.msm8974
SENSORS += activity_recognition.msm8992
SENSORS += activity_recognition.msm8994
SENSORS += activity_recognition.msm8996
SENSORS += activity_recognition.msm8952
SENSORS += activity_recognition.msm8937
SENSORS += activity_recognition.msm8998
SENSORS += activity_recognition.sdm660
SENSORS += activity_recognition.sdc660
SENSORS += activity_recognition.apq8098_latv
SENSORS += activity_recognition.msm8953
SENSORS += sensor_def_qcomdev.conf
SENSORS += sensors_settings
SENSORS += sensors_registry
SENSORS += sensors.apq8084
SENSORS += sensors.msm8226
SENSORS += sensors.msm8610
SENSORS += sensors.msm8930
SENSORS += sensors.msm8960
SENSORS += sensors.msm8974
SENSORS += sensors.msm8992
SENSORS += sensors.msm8994
SENSORS += sensors.msm8996
SENSORS += sensors.msm8916
SENSORS += sensors.msm8909
SENSORS += sensors.msm8909go
SENSORS += sensors.msm8909_512go
SENSORS += sensors.msm8952_64
SENSORS += sensors.msm8952_32
SENSORS += sensors.msm8937_64
SENSORS += sensors.msm8937_32
SENSORS += sensors.msm8937_32go
SENSORS += sensors.msm8998
SENSORS += sensors.sdm660_64
SENSORS += sensors.sdm660_32
SENSORS += sensors.sdm845
SENSORS += sensors.sdm710
SENSORS += sensors.sdc660
SENSORS += sensors.msm8953_64
SENSORS += sensors.msm8953_32
SENSORS += sensors.apq8098_latv
SENSORS += libsnsdiaglog
SENSORS += sensors.native
SENSORS += sensors.qcom
SENSORS += sensors.qti
SENSORS += sscrpcd
SENSORS += sensors.ssc
SENSORS += libssc
SENSORS += libsnsapi
SENSORS += libsensorslog
SENSORS += libsensor1
SENSORS += libcalmodule_common
SENSORS += calmodule.cfg
SENSORS += sensor_calibrate
ifneq ($(TARGET_HAS_LOW_RAM), true)
SENSORS += android.hardware.sensors@1.0-service
endif
SENSORS += android.hardware.sensors@1.0-impl
SENSORS += libsns_low_lat_stream_stub
SENSORS += libsns_low_lat_stream_stub_system
SENSORS += libsns_low_lat_stream_skel
SENSORS += libsns_low_lat_stream_skel_system
SENSORS += libsns_device_mode_stub
SENSORS += libsns_device_mode_stub_system
SENSORS += libsns_device_mode_skel
SENSORS += libsns_device_mode_skel_system
SENSORS += libsensor1_system
SENSORS += libsensor_reg_system
SENSORS += libssc_system
SENSORS += libsnsapi_system
SENSORS += libsensorslog_system
SENSORS += libsnsdiaglog_system
SENSORS += sensors_config_module

#SmartcardService
ifeq ($(strip $(TARGET_ENABLE_PROPRIETARY_SMARTCARD_SERVICE)),true)
SCS_PROP := SmartcardService
SCS_PROP += org.simalliance.openmobileapi
SCS_PROP += org.simalliance.openmobileapi.xml
#GPQESE
ifeq ($(strip $(TARGET_USES_GPQESE)),true)
GPQESE := com.qualcomm.qti.libesewrapper
GPQESE += com.qualcomm.qti.libesewrapper.xml
GPQESE += libese
GPQESE += libese_system
GPQESE += libgpqese_system
GPQESE += libgpqese
PRODUCT_PROPERTY_OVERRIDES += persist.nfc.smartcard.config=SIM1,eSE1
else
PRODUCT_PROPERTY_OVERRIDES += persist.nfc.smartcard.config=SIM1
endif
endif

#SS_RESTART
SS_RESTART := ssr_diag
SS_RESTART += subsystem_ramdump

#SVGT
SVGT := libsvgecmascriptBindings
SVGT += libsvgutils
SVGT += libsvgabstraction
SVGT += libsvgscriptEngBindings
SVGT += libsvgnativeandroid
SVGT += libsvgt
SVGT += libsvgcore

#SWDEC2DTO3D
SW2DTO3D := libswdec_2dto3d

#SYSTEM HEALTH MONITOR
SYSTEM_HEALTH_MONITOR := libsystem_health_mon

ifneq ($(TARGET_NO_TELEPHONY), true)
#TELEPHONY_APPS
TELEPHONY_APPS := shutdownlistener
TELEPHONY_APPS += fastdormancy
TELEPHONY_APPS += GsmTuneAway
TELEPHONY_APPS += NetworkSetting
TELEPHONY_APPS += xdivert
TELEPHONY_APPS += CallFeaturesSetting
TELEPHONY_APPS += datastatusnotification
TELEPHONY_APPS += QtiTelephonyService
TELEPHONY_APPS += telephonyservice.xml
TELEPHONY_APPS += SimContacts
TELEPHONY_APPS += embms
TELEPHONY_APPS += embms.xml
TELEPHONY_APPS += embmstuneaway
TELEPHONY_APPS += PrimaryCardController
TELEPHONY_APPS += TelephonyExtVodafonePack
TELEPHONY_APPS += SimSettings
TELEPHONY_APPS += ConferenceDialer
TELEPHONY_APPS += uimlpalibrary
TELEPHONY_APPS += lpa.xml
TELEPHONY_APPS += uimlpaservice
TELEPHONY_APPS += remotesimlockservice
TELEPHONY_APPS += uimservicelibrary
TELEPHONY_APPS += UimService.xml
TELEPHONY_APPS += uimremotesimlocklibrary
TELEPHONY_APPS += RemoteSimlock.xml
TELEPHONY_APPS += remoteSimLockAuthentication
TELEPHONY_APPS += remotesimlockmanagerlibrary
TELEPHONY_APPS += RemoteSimlockManager.xml
TELEPHONY_APPS += libimscamera_jni
TELEPHONY_APPS += libimsmedia_jni
TELEPHONY_APPS += CallEnhancement
TELEPHONY_APPS += DeviceInfo
TELEPHONY_APPS += QtiSystemService
TELEPHONY_APPS += AutoRegistration
TELEPHONY_APPS += CallSettings
TELEPHONY_APPS += atfwd
#Qc extended functionality of android telephony
ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
QTI_TELEPHONY_FWK := qti-telephony-common
endif
QTI_TELEPHONY_FWK += QtiTelephonyServicelibrary

#Qc extended telephony framework resource APK
QTI_TELEPHONY_RES := telresources
endif  # TARGET_NO_TELEPHONY

#TFTP
TFTP := tftp_server

#THERMAL
THERMAL := thermald
THERMAL += thermald.conf
THERMAL += thermald-8960.conf
THERMAL += thermald-8064.conf
THERMAL += thermald-8930.conf
THERMAL += thermald-8960ab.conf
THERMAL += thermald-8064ab.conf
THERMAL += thermald-8930ab.conf
THERMAL += thermald-8974.conf
THERMAL += thermald-8x25-msm1-pmic_therm.conf
THERMAL += thermald-8x25-msm2-pmic_therm.conf
THERMAL += thermald-8x25-msm2-msm_therm.conf

#THERMAL-ENGINE
THERMAL-ENGINE := thermal-engine
THERMAL-ENGINE += libthermalclient
THERMAL-ENGINE += thermal-engine.conf
THERMAL-ENGINE += thermal-engine-8960.conf
THERMAL-ENGINE += thermal-engine-8064.conf
THERMAL-ENGINE += thermal-engine-8064ab.conf
THERMAL-ENGINE += thermal-engine-8930.conf
THERMAL-ENGINE += thermal-engine-8974.conf
THERMAL-ENGINE += thermal-engine-8226.conf
THERMAL-ENGINE += thermal-engine-8610.conf

#LIMITS
LIMITS:= vendor.qti.hardware.limits@1.0
LIMITS += vendor.qti.hardware.limits@1.0_vendor
LIMITS += vendor.qti.hardware.limits@1.0-service.rc
LIMITS += vendor.qti.hardware.limits@1.0-impl
LIMITS += vendor.qti.hardware.limits@1.0-service
#TIME_SERVICES
TIME_SERVICES := time_daemon TimeService libTimeService

#TINY xml
TINYXML := libtinyxml

#TINYXML2
TINYXML2 := libtinyxml2

#TOUCH FUSION
TOUCH_FUSION := touch_fusion
TOUCH_FUSION += qtc800s.cfg
TOUCH_FUSION += qtc800s.bin

#TS_TOOLS
TS_TOOLS := evt-sniff.cfg

#TTSP firmware
TTSP_FW := cyttsp_7630_fluid.hex
TTSP_FW += cyttsp_8064_mtp.hex
TTSP_FW += cyttsp_8660_fluid_p2.hex
TTSP_FW += cyttsp_8660_fluid_p3.hex
TTSP_FW += cyttsp_8660_ffa.hex
TTSP_FW += cyttsp_8960_cdp.hex

#TV INPUT
ifeq ($(TARGET_BOARD_AUTO),true)
TVINPUT := tv_input.msm8996
TVINPUT += android.hardware.tv.input@1.0-service
TVINPUT += android.hardware.tv.input@1.0-impl
TVINPUT += libTvInputHalEnhancements
TVINPUT_APP := QTvInput
TVINPUT_SERVICE := QHwTvInput
endif
#TV_TUNER
TV_TUNER := atv_fe_test
TV_TUNER += dtv_fe_test
TV_TUNER += lib_atv_rf_fe
TV_TUNER += lib_dtv_rf_fe
TV_TUNER += lib_MPQ_RFFE
TV_TUNER += libmpq_bsp8092_cdp_h1
TV_TUNER += libmpq_bsp8092_cdp_h5
TV_TUNER += libmpq_bsp8092_rd_h1
TV_TUNER += lib_tdsn_c231d
TV_TUNER += lib_tdsq_g631d
TV_TUNER += lib_tdss_g682d
TV_TUNER += libmpq_rf_utils
TV_TUNER += lib_sif_demod_stub
TV_TUNER += lib_tv_bsp_mpq8064_dvb
TV_TUNER += lib_tv_receiver_stub
TV_TUNER += libtv_tuners_io
TV_TUNER += tv_driver_test
TV_TUNER += tv_fe_test
TV_TUNER += libUCCP330
TV_TUNER += libForza


#ULTRASOUND_COMMON
ULTRASOUND_COMMON := UltrasoundSettings
ULTRASOUND_COMMON += form_factor_fluid.cfg
ULTRASOUND_COMMON += form_factor_liquid.cfg
ULTRASOUND_COMMON += form_factor_mtp.cfg
ULTRASOUND_COMMON += libual
ULTRASOUND_COMMON += libualutil
ULTRASOUND_COMMON += libusndroute
ULTRASOUND_COMMON += libusutils
ULTRASOUND_COMMON += mixer_paths_dragon.xml
ULTRASOUND_COMMON += mixer_paths_fluid.xml
ULTRASOUND_COMMON += mixer_paths_liquid.xml
ULTRASOUND_COMMON += mixer_paths_mtp.xml
ULTRASOUND_COMMON += readme.txt
ULTRASOUND_COMMON += usf_post_boot.sh
ULTRASOUND_COMMON += usf_settings.sh
ULTRASOUND_COMMON += usf_tester
ULTRASOUND_COMMON += usf_tester_echo_fluid.cfg
ULTRASOUND_COMMON += usf_tester_echo_mtp.cfg
ULTRASOUND_COMMON += usf_tester_epos_fluid.cfg
ULTRASOUND_COMMON += usf_tester_epos_liquid.cfg
ULTRASOUND_COMMON += usf_tester_epos_mtp.cfg
ULTRASOUND_COMMON += usf_tsc.idc
ULTRASOUND_COMMON += usf_tsc_ext.idc
ULTRASOUND_COMMON += usf_tsc_ptr.idc
ULTRASOUND_COMMON += version.txt

#ULTRASOUND_GESTURE
ULTRASOUND_GESTURE := libgessyncsockadapter
ULTRASOUND_GESTURE += libqcsyncgesture
ULTRASOUND_GESTURE += libsyncgesadapter
ULTRASOUND_GESTURE += usf_sync_gesture
ULTRASOUND_GESTURE += usf_sync_gesture_apps_fluid.cfg
ULTRASOUND_GESTURE += usf_sync_gesture_apps_mtp.cfg
ULTRASOUND_GESTURE += usf_sync_gesture_fluid.cfg
ULTRASOUND_GESTURE += usf_sync_gesture_fluid_tx_transparent_data.bin
ULTRASOUND_GESTURE += usf_sync_gesture_fw_apps_mtp.cfg
ULTRASOUND_GESTURE += usf_sync_gesture_fw_mtp.cfg
ULTRASOUND_GESTURE += usf_sync_gesture_liquid.cfg
ULTRASOUND_GESTURE += usf_sync_gesture_liquid_algo_transparent_data.bin
ULTRASOUND_GESTURE += usf_sync_gesture_liquid_tx_transparent_data.bin
ULTRASOUND_GESTURE += usf_sync_gesture_lpass_rec_mtp.cfg
ULTRASOUND_GESTURE += usf_sync_gesture_mtp.cfg
ULTRASOUND_GESTURE += usf_sync_gesture_mtp_algo_transparent_data.bin
ULTRASOUND_GESTURE += usf_sync_gesture_mtp_tx_transparent_data.bin

#ULTRASOUND_PEN
ULTRASOUND_PEN := ASDConf.sdc
ULTRASOUND_PEN += DigitalPenService
ULTRASOUND_PEN += DigitalPenSettings
ULTRASOUND_PEN += PenPairingApp
ULTRASOUND_PEN += ScopeDebuggerRecordingTool
ULTRASOUND_PEN += calibver
ULTRASOUND_PEN += digitalpenservice.xml
ULTRASOUND_PEN += libdpencalib
ULTRASOUND_PEN += libdpencalib_asm
ULTRASOUND_PEN += libepdsp
ULTRASOUND_PEN += libepdsp_SDserver
ULTRASOUND_PEN += libppl
ULTRASOUND_PEN += product_calib_dragon_ref1.dat
ULTRASOUND_PEN += product_calib_dragon_ref2.dat
ULTRASOUND_PEN += product_calib_dragon_ref3.dat
ULTRASOUND_PEN += product_calib_fluid_ref1.dat
ULTRASOUND_PEN += product_calib_fluid_ref2.dat
ULTRASOUND_PEN += product_calib_fluid_ref3.dat
ULTRASOUND_PEN += product_calib_liquid_ref1.dat
ULTRASOUND_PEN += product_calib_liquid_ref2.dat
ULTRASOUND_PEN += product_calib_liquid_ref3.dat
ULTRASOUND_PEN += product_calib_mtp_ref1.dat
ULTRASOUND_PEN += product_calib_mtp_ref2.dat
ULTRASOUND_PEN += product_calib_mtp_ref3.dat
ULTRASOUND_PEN += ps_tuning1_fluid.bin
ULTRASOUND_PEN += ps_tuning1_idle_liquid.bin
ULTRASOUND_PEN += ps_tuning1_idle_mtp.bin
ULTRASOUND_PEN += ps_tuning1_mtp.bin
ULTRASOUND_PEN += ps_tuning1_standby_liquid.bin
ULTRASOUND_PEN += ps_tuning1_standby_mtp.bin
ULTRASOUND_PEN += service_settings_dragon.xml
ULTRASOUND_PEN += service_settings_fluid.xml
ULTRASOUND_PEN += service_settings_liquid.xml
ULTRASOUND_PEN += service_settings_liquid_1_to_1.xml
ULTRASOUND_PEN += service_settings_liquid_folio.xml
ULTRASOUND_PEN += service_settings_mtp.xml
ULTRASOUND_PEN += service_settings_mtp_1_to_1.xml
ULTRASOUND_PEN += service_settings_mtp_folio.xml
ULTRASOUND_PEN += sw_calib_liquid.dat
ULTRASOUND_PEN += sw_calib_mtp.dat
ULTRASOUND_PEN += unit_calib_dragon_ref1.dat
ULTRASOUND_PEN += unit_calib_dragon_ref2.dat
ULTRASOUND_PEN += unit_calib_dragon_ref3.dat
ULTRASOUND_PEN += unit_calib_fluid_ref1.dat
ULTRASOUND_PEN += unit_calib_fluid_ref2.dat
ULTRASOUND_PEN += unit_calib_fluid_ref3.dat
ULTRASOUND_PEN += unit_calib_liquid_ref1.dat
ULTRASOUND_PEN += unit_calib_liquid_ref2.dat
ULTRASOUND_PEN += unit_calib_liquid_ref3.dat
ULTRASOUND_PEN += unit_calib_mtp_ref1.dat
ULTRASOUND_PEN += unit_calib_mtp_ref2.dat
ULTRASOUND_PEN += unit_calib_mtp_ref3.dat
ULTRASOUND_PEN += usf_epos
ULTRASOUND_PEN += usf_epos_fluid.cfg
ULTRASOUND_PEN += usf_epos_liquid.cfg
ULTRASOUND_PEN += usf_epos_liquid_6_channels.cfg
ULTRASOUND_PEN += usf_epos_liquid_ps_disabled.cfg
ULTRASOUND_PEN += usf_epos_mtp.cfg
ULTRASOUND_PEN += usf_epos_mtp_ps_disabled.cfg
ULTRASOUND_PEN += usf_pairing
ULTRASOUND_PEN += usf_pairing_fluid.cfg
ULTRASOUND_PEN += usf_pairing_liquid.cfg
ULTRASOUND_PEN += usf_pairing_mtp.cfg
ULTRASOUND_PEN += usf_sw_calib
ULTRASOUND_PEN += usf_sw_calib_dragon.cfg
ULTRASOUND_PEN += usf_sw_calib_fluid.cfg
ULTRASOUND_PEN += usf_sw_calib_liquid.cfg
ULTRASOUND_PEN += usf_sw_calib_mtp.cfg
ULTRASOUND_PEN += usf_sw_calib_tester_dragon.cfg
ULTRASOUND_PEN += usf_sw_calib_tester_fluid.cfg
ULTRASOUND_PEN += usf_sw_calib_tester_liquid.cfg
ULTRASOUND_PEN += usf_sw_calib_tester_mtp.cfg
REF1_SERIES_FILES_NUM = 1 2 3 4 5 6 7 8 9 10
REF2_SERIES_FILES_NUM = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36
REF3_SERIES_FILES_NUM = 1
define add_series_packets
  ULTRASOUND_PEN += $1
endef
$(foreach item,$(REF1_SERIES_FILES_NUM),$(eval $(call add_series_packets,series_calib$(item)_ref1.dat)))
$(foreach item,$(REF2_SERIES_FILES_NUM),$(eval $(call add_series_packets,series_calib$(item)_ref2.dat)))
$(foreach item,$(REF3_SERIES_FILES_NUM),$(eval $(call add_series_packets,series_calib$(item)_ref3.dat)))

#ULTRASOUND_PROXIMITY
ULTRASOUND_PROXIMITY := libproxadapter
ULTRASOUND_PROXIMITY += libproxsockadapter
ULTRASOUND_PROXIMITY += libqcproximity
ULTRASOUND_PROXIMITY += Proximity_calib
ULTRASOUND_PROXIMITY += Proximity_tester
ULTRASOUND_PROXIMITY += usf_proximity_calib.sh
ULTRASOUND_PROXIMITY += sensors.oem
ULTRASOUND_PROXIMITY += us-syncproximity
ULTRASOUND_PROXIMITY += us-syncproximity.so
ULTRASOUND_PROXIMITY += usf_pocket_apps_mtp.cfg
ULTRASOUND_PROXIMITY += usf_pocket_mtp.cfg
ULTRASOUND_PROXIMITY += usf_pocket_mtp_algo_transparent_data.bin
ULTRASOUND_PROXIMITY += usf_proximity
ULTRASOUND_PROXIMITY += usf_proximity_apps_mtp.cfg
ULTRASOUND_PROXIMITY += usf_proximity_mtp.cfg
ULTRASOUND_PROXIMITY += usf_proximity_mtp_algo_transparent_data.bin
ULTRASOUND_PROXIMITY += usf_proximity_mtp_debug.cfg
ULTRASOUND_PROXIMITY += usf_proximity_mtp_rx_transparent_data.bin
ULTRASOUND_PROXIMITY += usf_proximity_mtp_tx_transparent_data.bin
ULTRASOUND_PROXIMITY += usf_ranging_apps_mtp.cfg
ULTRASOUND_PROXIMITY += usf_ranging_mtp_algo_transparent_data.bin

#USB
USB := ice40.bin

#USB_UICC_CLIENT
USB_UICC_CLIENT := usb_uicc_client

#VEHICLE_NETWORKS
ifeq ($(TARGET_BOARD_AUTO),true)
VEHICLE_NETWORKS := libcanwrapper
VEHICLE_NETWORKS += libcantranslator
VEHICLE_NETWORKS += rh850_fw.out
VEHICLE_NETWORKS += rh850_fw.hex
VEHICLE_NETWORKS += rh850_boot_fw.out
VEHICLE_NETWORKS += rh850_boot_fw.hex
VEHICLE_NETWORKS += rh850_upgrade_fw.out
VEHICLE_NETWORKS += rh850_upgrade_fw.hex
VEHICLE_NETWORKS += canflasher
VEHICLE_NETWORKS += vendor.qti.hardware.automotive.vehicle@1.0-service
VEHICLE_NETWORKS += vendor.qti.hardware.automotive.vehicle@1.0-service.rc
VEHICLE_NETWORKS += android.hardware.automotive.vehicle@2.0-manager-lib-shared
VEHICLE_NETWORKS += vendor.qti.hardware.automotive.vehicle@1.0_vendor
VEHICLE_NETWORKS += vendor.qti.hardware.automotive.vehicle@1.0
VEHICLE_NETWORKS += init.qti.can.sh
endif

#VM_BMS
VM_BMS := vm_bms

#VPP
VPP := DE.o.msm8937
VPP += DE.o.msm8953
VPP += DE.o.sdm660
VPP += libhcp_rpc_skel
VPP += libhcp_rpc_skel.so
VPP += libmmsw_detail_enhancement
VPP += libmmsw_math
VPP += libmmsw_opencl
VPP += libmmsw_platform
VPP += libOmxVpp
VPP += libvpplibrary
VPP += libvpphcp
VPP += libvpphvx
VPP += libvpp_frc
VPP += libvpp_frc.so
VPP += libvpp_svc_skel
VPP += libvpp_svc_skel.so
VPP += libvppclient
VPP += vendor.qti.hardware.vpp@1.1
ifneq ($(strip $(TARGET_USES_IOTCC_HEADLESS)),true)
VPP += vppservice
endif


#WEBKIT
WEBKIT := browsermanagement
WEBKIT += libwebkitaccel
WEBKIT += PrivInit
WEBKIT += libdnshostprio
WEBKIT += libnetmonitor
WEBKIT += qnet-plugin
WEBKIT += pp_proc_plugin
WEBKIT += tcp-connections
WEBKIT += libtcpfinaggr
WEBKIT += modemwarmup
WEBKIT += libgetzip
WEBKIT += spl_proc_plugin
WEBKIT += libsocketpoolextend
WEBKIT += libvideo_cor

#WFD
WFD := capability.xml
WFD += libwfdmminterface
WFD += libmmwfdsinkinterface
WFD += libmmwfdsrcinterface
WFD += libwfdmmservice
WFD += libwfduibcinterface
WFD += libwfduibcsrcinterface
WFD += libwfduibcsrc
WFD += libOmxMux
WFD += libwfdcommonutils
WFD += libwfdhdcpcp
WFD += libwfdlinkstub
WFD += libwfdmmsrc
WFD += libwfdmmutils
WFD += libwfdnative
WFD += libwfdsm
WFD += libwfdservice
WFD += libwfdrtsp
WFD += libextendedremotedisplay
WFD += libOmxVideoDSMode
WFD += WfdCommon
WFD += WfdService
WFD += wfdconfig.xml
WFD += wfdconfigsink.xml
WFD += ArmHDCP_QTI_Android.cfg
WFD += WfdP2pCommon
WFD += WfdP2pService
WFD += com.qualcomm.wfd.permissions.xml
WFD += wfdservice
WFD += libqti-wl
WFD += com.qualcomm.qti.wifidisplayhal@1.0-halimpl
WFD += com.qualcomm.qti.wifidisplayhal@1.0
WFD += com.qualcomm.qti.wifidisplayhal@1.0_vendor
WFD += com.qualcomm.qti.wifidisplayhal@1.0-impl
WFD += wifidisplayhalservice
WFD += libwfdhaldsmanager
WFD += libwfdmodulehdcpsession
WFD += libwfdavenhancements
WFD += libwfdcodecv4l2
WFD += libwfdcommonutils_proprietary
WFD += wfdservice.rc
WFD += com.qualcomm.qti.wifidisplayhal@1.0-service.rc
WFD += libwfdconfigutils
WFD += libmiracast
#WIGIG
ifeq ($(BOARD_HAS_QCOM_WIGIG), true)
WIGIG := wigig-service
WIGIG += WigigSettings
WIGIG += libaoa
WIGIG += ftmtest
WIGIG += aoa_cldb_falcon.bin
WIGIG += aoa_cldb_swl14.bin
WIGIG += wigig_supplicant.conf
WIGIG += wigig_p2p_supplicant.conf
WIGIG += wigighalsvc
WIGIG += vendor.qti.hardware.wigig.supptunnel@1.0
WIGIG += wigignpt
WIGIG += vendor.qti.hardware.wigig.netperftuner@1.0
endif

#WIPOWER, wbc
WIPOWER := wbc_hal.default
WIPOWER += com.quicinc.wbc
WIPOWER += com.quicinc.wbc.xml
WIPOWER += com.quicinc.wbcservice
WIPOWER += com.quicinc.wbcservice.xml
WIPOWER += libwbc_jni
WIPOWER += com.quicinc.wipoweragent
WIPOWER += com.quicinc.wbcserviceapp
WIPOWER += vendor.qti.hardware.wipower@1.0_vendor
WIPOWER += vendor.qti.hardware.wipower@1.0-impl
#WIPOWER, a4wp
WIPOWER += libwipower_jni
WIPOWER += android.wipower
WIPOWER += android.wipower.xml

#WLAN
WLAN := libAniAsf
WLAN += cfg.dat
WLAN += msm7627a_qcom_wlan_nv.bin
WLAN += msm7630_qcom_wlan_nv.bin
WLAN += msm8660_qcom_wlan_nv.bin

ifneq ($(BOARD_HAS_ATH_WLAN_AR6320), true)
WLAN += ptt_socket_app
BUILD_PTT_SOCKET_APP:=1
endif
WLAN += WifiLogger_app
WLAN += qcom_cfg_default.ini
WLAN += qcom_cfg.ini
WLAN += qcom_fw.bin
WLAN += qcom_wapi_fw.bin
WLAN += qcom_wlan_nv.bin
WLAN += WCN1314_cfg.dat
WLAN += WCN1314_qcom_cfg.ini
WLAN += WCN1314_qcom_fw.bin
WLAN += WCN1314_qcom_wlan_nv.bin
WLAN += WCNSS_cfg.dat
WLAN += WCNSS_qcom_cfg.ini
WLAN += WCNSS_qcom_wlan_nv.bin
WLAN += pktlogconf
WLAN += athdiag
WLAN += cld-fwlog-record
WLAN += cld-fwlog-netlink
WLAN += cld-fwlog-parser
WLAN += cnss_diag
ifneq ($(TARGET_HAS_LOW_RAM), true)
WLAN += cnss-daemon
endif
WLAN += libwifi-hal-qcom
WLAN += libwpa_client
WLAN += libqsap_sdk
WLAN += qdc
WLAN += hal_proxy_daemon
WLAN += sigma_dut
WLAN += e_loop
WLAN += myftm
WLAN += vendor_cmd_tool
WLAN += icm
WLAN += libwpa_drv_oem
WLAN += android.hardware.wifi@1.0-service
WLAN += android.hardware.wifi.offload@1.0-service
WLAN += spectraltool

ifneq ($(TARGET_HAS_LOW_RAM),true)
#SNAPDRAGON_SDK_DISPLAY
SNAPDRAGON_SDK_DISPLAY := com.qti.snapdragon.sdk.display
SNAPDRAGON_SDK_DISPLAY += com.qti.snapdragon.sdk.display.xml
SNAPDRAGON_SDK_DISPLAY += colorservice
SNAPDRAGON_SDK_DISPLAY += libsd_sdk_display
endif

#LOG_SYSTEM
LOG_SYSTEM := Logkit
LOG_SYSTEM += SystemAgent
LOG_SYSTEM += qlogd
LOG_SYSTEM += qlog-conf.xml
LOG_SYSTEM += qdss.cfg
LOG_SYSTEM += default_diag_mask.cfg
LOG_SYSTEM += rootagent
LOG_SYSTEM += init.qcom.rootagent.sh
LOG_SYSTEM += dynamic_debug_mask.cfg

# LogKit 3
QTI_LOGKIT_BINS := LKCore
QTI_LOGKIT_BINS += qti-logkit
QTI_LOGKIT_BINS += libUserAgent
QTI_LOGKIT_BINS += access-qcom-logkit
QTI_LOGKIT_BINS += qcom_logkit.xml
QTI_LOGKIT_BINS += qti_logkit_config.xml
QTI_LOGKIT_BINS += qti_logkit_command.xml
QTI_LOGKIT_BINS += goldenlogmask.dmc
QTI_LOGKIT_BINS += gnsslocationlogging.dmc
QTI_LOGKIT_BINS += audiobasic.dmc
QTI_LOGKIT_BINS += IMS.dmc
QTI_LOGKIT_BINS += default.png
QTI_LOGKIT_BINS += bluetooth.png
QTI_LOGKIT_BINS += IMS.png
QTI_LOGKIT_BINS += liblz4

#PERFDUMP_APPS
PERFDUMP_APPS := Perfdump

#IPA_FWS
IPA_FWS := ipa_fws.b00
IPA_FWS += ipa_fws.b01
IPA_FWS += ipa_fws.b02
IPA_FWS += ipa_fws.b03
IPA_FWS += ipa_fws.b04
IPA_FWS += ipa_fws.mdt

#CSK
CSK := Csk
CSK += CskServer

#CSM
CSM := Csm
CSM += libcsm_data

#CHRE daemon
CHRE += chre

ifneq ($(strip $(TARGET_USES_IOTCC_HEADLESS)),true)
#QDMA
QDMA += QDMA
QDMA += QDMA-UI
QDMA += qdmastatsd
QDMA += libqdma_file_agent
endif

#scr modules
SCR_MODULES := bg_daemon
SCR_MODULES += vendor.google_clockwork.sidekickgraphics@1.0-impl
SCR_MODULES += vendor.qti.hardware.sidekickgraphics@1.0-impl
SCR_MODULES += vendor.google_clockwork.sidekickgraphics@1.0-service
SCR_MODULES += vendor.google_clockwork.sidekickgraphics@1.0-service.rc
SCR_MODULES += skgtest

#POWER_OFF_ALARM
POWER_OFF_ALARM = vendor.qti.hardware.alarm@1.0
POWER_OFF_ALARM += vendor.qti.hardware.alarm-V1.0-java

#neutrino modules
ifeq ($(TARGET_BOARD_AUTO),true)
NTN := ntn_flashloader
NTN += ntn_firmware.bin
endif

#SECURE QTI_UTILS
QTIUTILS := libqti-utils

#SENSORSCAL
SENSORSCAL = vendor.qti.hardware.sensorscalibrate@1.0
SENSORSCAL += vendor.qti.hardware.sensorscalibrate-V1.0-java
SENSORSCAL += vendor.qti.hardware.sensorscalibrate@1.0-service
SENSORSCAL += vendor.qti.hardware.sensorscalibrate@1.0-impl
SENSORSCAL += vendor-qti-hardware-sensorscalibrate.xml
SENSORSCAL += libsensorcal

# HVXNNHAL Stack Components
HVXNNHAL = vendor.qti.neuralnetworks@1.0-service-hvx
HVXNNHAL += vendor.qti.neuralnetworks@1.0-service-hvx.rc
HVXNNHAL += libhexagon_nn_skel
HVXNNHAL += libhexagon_nn_skel.so
HVXNNHAL += libhexagon_nn_stub

#QMMF_SDK
ifeq ($(TARGET_BOARD_PLATFORM),qcs605)
QMMF_SDK := qmmf-server
QMMF_SDK += libqmmf_av_codec
QMMF_SDK += libqmmf_recorder_client
QMMF_SDK += libqmmf_recorder_service
QMMF_SDK += libqmmf_demuxer
QMMF_SDK += libqmmf_player_client
QMMF_SDK += libqmmf_player_service
QMMF_SDK += libqmmf_display_client
QMMF_SDK += libqmmf_display_service
QMMF_SDK += libqmmf_overlay
QMMF_SDK += libqmmf_codec_adaptor
QMMF_SDK += libqmmf_camera_adaptor
QMMF_SDK += libqmmf_av_queue
QMMF_SDK += libqmmf_audio_client
QMMF_SDK += libqmmf_audio_service
QMMF_SDK += libqmmf_system_client
QMMF_SDK += libqmmf_system_service
QMMF_SDK += libqmmf_postproc_test
QMMF_SDK += libqmmf_postproc_frame_skip
QMMF_SDK += libqmmf_camera_hal_reproc
QMMF_SDK += libqmmf_postproc_algo
QMMF_SDK += libqmmf_jpeg
QMMF_SDK += libqmmf_utils
QMMF_SDK += libqmmf_test_algo_outplace
QMMF_SDK += libqmmf_test_algo_inplace
QMMF_SDK += libqmmf_test_algo_outplace_history
QMMF_SDK += libqmmf_test_algo_inplace_history
QMMF_SDK += libqmmf_test_algo_resizer
QMMF_SDK += com.qti.node.stitchalgo
QMMF_SDK += libqmmf_alg_side_by_side
QMMF_SDK += overlay_test.rgba
QMMF_SDK += imx577_sensor_mode_config.json

#QMMF_WEBSERVER
QMMF_WEBSERVER = libqmmf_vam_interface
QMMF_WEBSERVER += libqmmf_mux_interface
QMMF_WEBSERVER += libhttp_interface
QMMF_WEBSERVER += libqmmf_live555rtsp_in

#IPC_WEBSERVER
IPC_WEBSERVER := 360_cam.conf
IPC_WEBSERVER += audio_config
IPC_WEBSERVER += ip_cam.conf
IPC_WEBSERVER += net_config
IPC_WEBSERVER += res_config

#VAM
VAM := libEngine_CustomEngineTemplate
VAM += VASim
VAM += libVAManager
VAM += VAMReport
VAM += libjson_apis
VAM += libEngine_GTRenderer
VAM += libEngine_Test1
VAM += libEngine_Test2

#VAM_ENGINES
VAM_ENGINES := libEngine_FaceRecognition
VAM_ENGINES += libEngine_MotionTracker
VAM_ENGINES += libEngine_RuleInterpreter
VAM_ENGINES += gFRConfig.json
VAM_ENGINES += gMTConfig.json
VAM_ENGINES += fr_2.db

#QMMF_ALG
QMMF_ALG := libqmmf_alg_es
QMMF_ALG += libqmmf_alg_yuv_cac
QMMF_ALG += bit_blit.cl
QMMF_ALG += libqmmf_alg_bit_blit
QMMF_ALG += shdr-gpu-pass-fusion-and-tm-fast.cl
QMMF_ALG += shdr-gpu-pass-fusion-and-tm.cl
QMMF_ALG += libqmmf_alg_svhdr
QMMF_ALG += fill_yuv_rectangle.cl
QMMF_ALG += fill_yuv_triangle.cl
QMMF_ALG += mask_resizer_luma.cl
QMMF_ALG += mask_resizer_chroma.cl
QMMF_ALG += libqmmf_alg_privacy_mask_resizer
QMMF_ALG += libqmmf_alg_binning_correction
QMMF_ALG += libqmmf_alg_sidebyside
QMMF_ALG += libqmmf_alg_lcac
QMMF_ALG += libqmmf_alg_common_open_cl_base

#live555
LIVE555 := liblive555Media
LIVE555 += liblive555groupsock
LIVE555 += liblive555BasicUsageEnvironment
endif

ifeq ($(TARGET_BOARD_AUTO),true)
PRODUCT_PACKAGES += $(NTN)
PRODUCT_PACKAGES += $(MM_AVINPUT)
PRODUCT_PACKAGES += $(TVINPUT)
PRODUCT_PACKAGES += $(VEHICLE_NETWORKS)
endif
ifeq ($(ENABLE_HYP),true)
PRODUCT_PACKAGES += $(MM_HYP_VIDEO)
endif
ifeq ($(ENABLE_HYP),true)
PRODUCT_PACKAGES += $(MM_UHAB)
PRODUCT_PACKAGES += $(MM_BUFFERQ)
endif

PRODUCT_PACKAGES += $(ADC)
PRODUCT_PACKAGES += $(ADSPRPC)
PRODUCT_PACKAGES += $(ALLJOYN)
PRODUCT_PACKAGES += $(AIVPLAY)
PRODUCT_PACKAGES += $(ANT)
PRODUCT_PACKAGES += $(ANTITHEFTDEMO)
PRODUCT_PACKAGES += $(BACKUP_AGENT)
PRODUCT_PACKAGES += $(BATTERY_CHARGING)
PRODUCT_PACKAGES += $(BT)
PRODUCT_PACKAGES += $(CAMERAHAWK_APPS)
PRODUCT_PACKAGES += $(CAMERAHAWK_WHIP_COMMON_LIB)
PRODUCT_PACKAGES += $(CCID)
PRODUCT_PACKAGES += $(CHARGER_MONITOR)
PRODUCT_PACKAGES += $(CHRE)
PRODUCT_PACKAGES += $(CONNECTION_SECURITY)
PRODUCT_PACKAGES += $(STATSPOLLMGR)
PRODUCT_PACKAGES += $(CNE)
PRODUCT_PACKAGES += $(DPM)
PRODUCT_PACKAGES += $(CRASH_LOGGER)
PRODUCT_PACKAGES += $(DATA)
PRODUCT_PACKAGES += $(DIAG)
PRODUCT_PACKAGES += $(DISPLAY_TESTS)
PRODUCT_PACKAGES += $(DISPLAY)
PRODUCT_PACKAGES += $(ENERGY_AWARENESS)
#PRODUCT_PACKAGES += $(ENTERPRISE_SECURITY)
PRODUCT_PACKAGES += $(FASTCV)
PRODUCT_PACKAGES += $(FASTMMI)
PRODUCT_PACKAGES += $(FINGERPRINT)
PRODUCT_PACKAGES += $(FLASH)
PRODUCT_PACKAGES += $(FM)
PRODUCT_PACKAGES += $(FOTA)
PRODUCT_PACKAGES += $(FTM)
PRODUCT_PACKAGES += $(GPS)
PRODUCT_PACKAGES += $(GPQESE)
PRODUCT_PACKAGES += $(GSMA_NFC)
PRODUCT_PACKAGES += $(HBTP)
PRODUCT_PACKAGES += $(HVDCP_OPTI)
PRODUCT_PACKAGES += $(HVXNNHAL)
PRODUCT_PACKAGES += $(HY11_HY22_diff)
PRODUCT_PACKAGES += $(MLVM)


#Ims modules are intentionally set to optional
#so that they are not packaged as part of system image.
#Hence removing them from PRODUCT_PACKAGES list
#PRODUCT_PACKAGES += $(IMS)
PRODUCT_PACKAGES += $(IMS_VT)
PRODUCT_PACKAGES += $(IMS_TEL)
PRODUCT_PACKAGES += $(IMS_TEL_RCS)
PRODUCT_PACKAGES += $(IMS_RCS)
PRODUCT_PACKAGES += $(IMS_NEWARCH)
PRODUCT_PACKAGES += $(IMS_REGMGR)
PRODUCT_PACKAGES += $(INTERFACE_PERMISSIONS)
PRODUCT_PACKAGES += $(IPA_FWS)
PRODUCT_PACKAGES += $(IQAGENT)
PRODUCT_PACKAGES += $(LATENCY)
PRODUCT_PACKAGES += $(MARE)
PRODUCT_PACKAGES += $(MDM_HELPER)
PRODUCT_PACKAGES += $(MDM_HELPER_PROXY)
PRODUCT_PACKAGES += $(MM_AUDIO)
PRODUCT_PACKAGES += $(MM_CAMERA)
ifeq ($(TARGET_BOARD_AUTO),true)
PRODUCT_PACKAGES += $(MM_CAMERA_AIS)
endif
PRODUCT_PACKAGES += $(MM_CORE)
PRODUCT_PACKAGES += $(MM_COLOR_CONVERSION)
PRODUCT_PACKAGES += $(MM_COLOR_CONVERTOR)
PRODUCT_PACKAGES += $(MM_DRMPLAY)
PRODUCT_PACKAGES += $(MM_GESTURES)
PRODUCT_PACKAGES += $(MM_GRAPHICS)
PRODUCT_PACKAGES += $(MM_HTTP)
PRODUCT_PACKAGES += $(MM_STA)
PRODUCT_PACKAGES += $(MM_MUX)
PRODUCT_PACKAGES += $(MM_OSAL)
PRODUCT_PACKAGES += $(MM_PARSER)
PRODUCT_PACKAGES += $(MM_QSM)
PRODUCT_PACKAGES += $(MM_RTP)
PRODUCT_PACKAGES += $(MM_STEREOLIB)
PRODUCT_PACKAGES += $(MM_STILL)
PRODUCT_PACKAGES += $(MM_VIDEO)
PRODUCT_PACKAGES += $(MM_VPU)
PRODUCT_PACKAGES += $(MODEM_APIS)
PRODUCT_PACKAGES += $(MODEM_API_TEST)
PRODUCT_PACKAGES += $(MPDCVS)
PRODUCT_PACKAGES += $(MPQ_COMMAND_IF)
PRODUCT_PACKAGES += $(MPQ_PLATFORM)
PRODUCT_PACKAGES += $(MSM_IRQBALANCE)
PRODUCT_PACKAGES += $(MXT_CFG)
PRODUCT_PACKAGES += $(N_SMUX)
PRODUCT_PACKAGES += $(NQ_NFC_PROP)
PRODUCT_PACKAGES += $(OEM_SERVICES)
PRODUCT_PACKAGES += $(ONCRPC)
PRODUCT_PACKAGES += $(PD_LOCATER)
PRODUCT_PACKAGES += $(PERF)
PRODUCT_PACKAGES += $(PERMGR)
PRODUCT_PACKAGES += $(PERFECT365_APPS)
PRODUCT_PACKAGES += $(PLAYREADY)
PRODUCT_PACKAGES += $(PROFILER)
PRODUCT_PACKAGES += $(QCHAT)
PRODUCT_PACKAGES += $(QCRIL)
PRODUCT_PACKAGES += $(QCNVITEM)
PRODUCT_PACKAGES += $(QMI)
PRODUCT_PACKAGES += $(QOSMGR)
PRODUCT_PACKAGES += $(QUICKCHARGE)
PRODUCT_PACKAGES += $(QVR)
PRODUCT_PACKAGES += $(QDMA)
PRODUCT_PACKAGES += $(REMOTEFS)
PRODUCT_PACKAGES += $(RIDL_BINS)
#PRODUCT_PACKAGES += $(RFS_ACCESS)
PRODUCT_PACKAGES += $(RNGD)
PRODUCT_PACKAGES += $(SCVE)
PRODUCT_PACKAGES += $(SENSORS)
PRODUCT_PACKAGES += $(SECUREMSM)
PRODUCT_PACKAGES += $(SCS_PROP)
PRODUCT_PACKAGES += $(SNAPDRAGON_SDK_DISPLAY)
PRODUCT_PACKAGES += $(SS_RESTART)
PRODUCT_PACKAGES += $(SVGT)
PRODUCT_PACKAGES += $(SW2DTO3D)
PRODUCT_PACKAGES += $(SYSTEM_HEALTH_MONITOR)
PRODUCT_PACKAGES += $(TELEPHONY_APPS)
PRODUCT_PACKAGES += $(QTI_LOGKIT_BINS)
PRODUCT_PACKAGES += $(PERFDUMP_APPS)
PRODUCT_PACKAGES += $(QTI_TELEPHONY_FWK)
PRODUCT_PACKAGES += $(QTI_TELEPHONY_RES)
PRODUCT_PACKAGES += $(TFTP)
PRODUCT_PACKAGES += $(THERMAL)
PRODUCT_PACKAGES += $(THERMAL-ENGINE)
PRODUCT_PACKAGES += $(LIMITS)
PRODUCT_PACKAGES += $(TIME_SERVICES)
PRODUCT_PACKAGES += $(TINYXML)
PRODUCT_PACKAGES += $(TINYXML2)
PRODUCT_PACKAGES += $(TREPN)
PRODUCT_PACKAGES += $(TOUCH_FUSION)
PRODUCT_PACKAGES += $(TS_TOOLS)
PRODUCT_PACKAGES += $(TTSP_FW)
PRODUCT_PACKAGES += $(TV_TUNER)
PRODUCT_PACKAGES += $(ULTRASOUND_COMMON)
PRODUCT_PACKAGES += $(ULTRASOUND_PROXIMITY)
PRODUCT_PACKAGES += $(USB)
PRODUCT_PACKAGES += $(USB_UICC_CLIENT)
PRODUCT_PACKAGES += $(VM_BMS)
PRODUCT_PACKAGES += $(VPP)
PRODUCT_PACKAGES += $(WEBKIT)
PRODUCT_PACKAGES += $(WFD)
PRODUCT_PACKAGES += $(WIGIG)
PRODUCT_PACKAGES += $(WHIP_APPS)
PRODUCT_PACKAGES += $(WIPOWER)
PRODUCT_PACKAGES += $(WLAN)
PRODUCT_PACKAGES += $(BT_TEL)
PRODUCT_PACKAGES += $(LOG_SYSTEM)
PRODUCT_PACKAGES += $(DISPLAY_DPPS)
PRODUCT_PACKAGES += $(DISPLAY_SERVICE)
PRODUCT_PACKAGES += $(QRD_CALENDAR_APPS)
PRODUCT_PACKAGES += $(CSM)
PRODUCT_PACKAGES += $(POWER_OFF_ALARM)
PRODUCT_PACKAGES += $(SENSORSCAL)

#Added to include QuickSettingsService
ifeq ($(TARGET_USES_QSSERVICE), true)
PRODUCT_PACKAGES += QSService
endif

ifeq ($(TARGET_SUPPORTS_WEARABLES), true)
PRODUCT_PACKAGES += $(SCR_MODULES)
endif

PRODUCT_PACKAGES += $(QTIUTILS)

ifeq ($(TARGET_BOARD_PLATFORM),qcs605)
PRODUCT_PACKAGES += $(QMMF_SDK)
PRODUCT_PACKAGES += $(QMMF_WEBSERVER)
PRODUCT_PACKAGES += $(IPC_WEBSERVER)
PRODUCT_PACKAGES += $(VAM)
PRODUCT_PACKAGES += $(VAM_ENGINES)
PRODUCT_PACKAGES += $(QMMF_ALG)
PRODUCT_PACKAGES += $(LIVE555)
endif

# Each line here corresponds to a debug LOCAL_MODULE built by
# Android.mk(s) in the proprietary projects. Where project
# corresponds to the vars here in CAPs.

# These modules are tagged with LOCAL_MODULE_TAGS := optional.
# They would not be installed into the image unless
# they are listed here explicitly.

#BT_DBG
BT_DBG := AR3002_PS_ASIC
BT_DBG += AR3002_RamPatch

#CNE_DBG
CNE_DBG := AndsfParser
CNE_DBG += cnelogger
CNE_DBG += swimcudpclient
CNE_DBG += swimstcpclient
CNE_DBG += swimtcpclient
CNE_DBG += swimudpclient
CNE_DBG += test2client

CNESETTINGS_DBG := CNESettings

#COMMON_DBG
COMMON_DBG := remote_apis_verify

#DATA_DBG
DATA_DBG := libCommandSvc
DATA_DBG += libdsnetutils
DATA_DBG += libqdi
DATA_DBG += libqdp

#DIAG_DBG
DIAG_DBG := libdiag

#FM_DBG
FM_DBG := hal_ss_test_manual

#GPS_DBG
GPS_DBG := lowi_test
GPS_DBG += test-fdal
GPS_DBG += gps-test.sh
GPS_DBG += libloc_xtra
GPS_DBG += libposlog
GPS_DBG += IZatSample
GPS_DBG += QCLocSvcTests
GPS_DBG += com.qualcomm.qti.qlogcat
GPS_DBG += libloc2jnibridge
GPS_DBG += libdiagbridge
GPS_DBG += com.qualcomm.qmapbridge.xml
GPS_DBG += com.qualcomm.qti.izattools.xml
GPS_DBG += sftc
GPS_DBG += slim_client
GPS_DBG += qmislim_client
GPS_DBG += qmislim_service
GPS_DBG += location_hal_test

#KERNEL_DBG
KERNEL_TEST_DBG := cpuhotplug_test.sh
KERNEL_TEST_DBG += cputest.sh
KERNEL_TEST_DBG += msm_uart_test
KERNEL_TEST_DBG += probe_test.sh
KERNEL_TEST_DBG += msm_uart_test.sh
KERNEL_TEST_DBG += uarttest.sh
KERNEL_TEST_DBG += clocksourcetest.sh
KERNEL_TEST_DBG += socinfotest.sh
KERNEL_TEST_DBG += timertest.sh
KERNEL_TEST_DBG += vfp.sh
KERNEL_TEST_DBG += vfptest
KERNEL_TEST_DBG += pctest
KERNEL_TEST_DBG += modem_test
KERNEL_TEST_DBG += pc-compound-test.sh
KERNEL_TEST_DBG += msm_sps_test
KERNEL_TEST_DBG += cacheflush
KERNEL_TEST_DBG += cacheflush.sh
KERNEL_TEST_DBG += _cacheflush.sh
KERNEL_TEST_DBG += loop.sh
KERNEL_TEST_DBG += clk_test.sh
KERNEL_TEST_DBG += cpufreq_test.sh
KERNEL_TEST_DBG += fbtest
KERNEL_TEST_DBG += fbtest.sh
KERNEL_TEST_DBG += geoinfo_flash
KERNEL_TEST_DBG += gpio_lib.conf
KERNEL_TEST_DBG += gpio_lib.sh
KERNEL_TEST_DBG += gpio_tlmm.sh
KERNEL_TEST_DBG += gpio_tlmm.conf
KERNEL_TEST_DBG += i2c-msm-test
KERNEL_TEST_DBG += i2c-msm-test.sh
KERNEL_TEST_DBG += irq_test.sh
KERNEL_TEST_DBG += kgsl_test
KERNEL_TEST_DBG += mpp_test.sh
KERNEL_TEST_DBG += msm_adc_test
KERNEL_TEST_DBG += msm_dma
KERNEL_TEST_DBG += msm_dma.sh
KERNEL_TEST_DBG += mtd_driver_test.sh
KERNEL_TEST_DBG += mtd_test.sh
KERNEL_TEST_DBG += mtd_yaffs2_test.sh
KERNEL_TEST_DBG += AR_LUT_1_0_B0
KERNEL_TEST_DBG += AR_LUT_1_0_B
KERNEL_TEST_DBG += AR_LUT_1_0_G0
KERNEL_TEST_DBG += AR_LUT_1_0_G
KERNEL_TEST_DBG += AR_LUT_1_0_R0
KERNEL_TEST_DBG += r_only_igc
KERNEL_TEST_DBG += SanityCfg.cfg
KERNEL_TEST_DBG += qcedev_test
KERNEL_TEST_DBG += qcedev_test.sh
KERNEL_TEST_DBG += yv12_qcif.yuv
KERNEL_TEST_DBG += rotator
KERNEL_TEST_DBG += rtc_test
KERNEL_TEST_DBG += sd_test.sh
KERNEL_TEST_DBG += smd_pkt_loopback_test
KERNEL_TEST_DBG += smem_log_test
KERNEL_TEST_DBG += smd_tty_loopback_test
KERNEL_TEST_DBG += spidevtest
KERNEL_TEST_DBG += spidevtest_slave
KERNEL_TEST_DBG += spidevtest.sh
KERNEL_TEST_DBG += spitest
KERNEL_TEST_DBG += spitest.sh
KERNEL_TEST_DBG += spiethernettest.sh
KERNEL_TEST_DBG += test_env_setup.sh
KERNEL_TEST_DBG += vreg_test.sh

#KS_DBG
KS_DBG := efsks
KS_DBG += ks
KS_DBG += qcks

#MM_AUDIO_DBG
MM_AUDIO_DBG := libds_native
MM_AUDIO_DBG += libds_jni
MM_AUDIO_DBG += libstagefright_soft_ddpdec
MM_AUDIO_DBG += libsurround_proc
MM_AUDIO_DBG += libsurround_proc_32
MM_AUDIO_DBG += surround_sound_headers
MM_AUDIO_DBG += filter1i.pcm
MM_AUDIO_DBG += filter1r.pcm
MM_AUDIO_DBG += filter2i.pcm
MM_AUDIO_DBG += filter2r.pcm
MM_AUDIO_DBG += filter3i.pcm
MM_AUDIO_DBG += filter3r.pcm
MM_AUDIO_DBG += filter4i.pcm
MM_AUDIO_DBG += filter4r.pcm
MM_AUDIO_DBG += mm-audio-ftm
MM_AUDIO_DBG += libOmxAc3HwDec
MM_AUDIO_DBG += mm-adec-omxac3-hw-test
MM_AUDIO_DBG += mm-adec-omxQcelpHw-test
MM_AUDIO_DBG += mm-adec-omxvam-test
MM_AUDIO_DBG += mm-adec-omxevrc-hw-test
MM_AUDIO_DBG += mm-adec-omxamrwb-test
MM_AUDIO_DBG += mm-adec-omxamrwbplus-test
MM_AUDIO_DBG += mm-adec-omxamr-test
MM_AUDIO_DBG += mm-adec-omxadpcm-test
MM_AUDIO_DBG += mm-adec-omxwma-test
MM_AUDIO_DBG += mm-audio-alsa-test
MM_AUDIO_DBG += mm-adec-omxaac-test
MM_AUDIO_DBG += mm-adec-omxdsd-test
MM_AUDIO_DBG += mm-adec-omxg711-test
MM_AUDIO_DBG += avs_test_ker.ko
MM_AUDIO_DBG += mm-adec-omxQcelp13-test
MM_AUDIO_DBG += mm-adec-omxevrc-test
MM_AUDIO_DBG += libsrsprocessing_libs
MM_AUDIO_DBG += libsrsprocessing
MM_AUDIO_DBG += libacdbrtac
MM_AUDIO_DBG += libadiertac
MM_AUDIO_DBG += libomx-dts.so
MM_AUDIO_DBG += mm-aenc-omxamr-test
MM_AUDIO_DBG += mm-aenc-omxaac-test
MM_AUDIO_DBG += mm-aenc-omxevrc-test
MM_AUDIO_DBG += mm-aenc-omxqcelp13-test
MM_AUDIO_DBG += mm-aenc-omxg711-test
MM_AUDIO_DBG += mm-aenc-omxmpegh-sw-test

#MM_CAMERA_DBG
MM_CAMERA_DBG := libmmcamera_wavelet_debug
MM_CAMERA_DBG += libmmcamera2_c2d_module
MM_CAMERA_DBG += libmmcamera2_cpp_module
MM_CAMERA_DBG += libmmcamera2_iface_modules
MM_CAMERA_DBG += libmmcamera2_imglib_modules
MM_CAMERA_DBG += libmmcamera2_isp_modules
MM_CAMERA_DBG += libmmcamera2_pp_buf_mgr
MM_CAMERA_DBG += libmmcamera2_pproc_modules
MM_CAMERA_DBG += libmmcamera2_sensor_modules
MM_CAMERA_DBG += libmmcamera2_stats_modules
MM_CAMERA_DBG += libmmcamera2_vpe_module
MM_CAMERA_DBG += libmmcamera2_wnr_module
MM_CAMERA_DBG += libmmcamera_vpu_module
MM_CAMERA_DBG += camxhal3test
MM_CAMERA_DBG += camxhal3unittest
MM_CAMERA_DBG += camxunittest
MM_CAMERA_DBG += libmmcamera_tuning
ifeq ($(TARGET_BOARD_AUTO),true)
MM_CAMERA_DBG += libais_32
MM_CAMERA_DBG += libais_client_32
MM_CAMERA_DBG += libais_config_32
MM_CAMERA_DBG += libais_ti960_32
MM_CAMERA_DBG += ais_server_32
MM_CAMERA_DBG += qcarcam_test_32
MM_CAMERA_DBG += qcarcam_config_single.xml
endif

#MM_CORE_DBG
MM_CORE_DBG := libmm-abl
MM_CORE_DBG += PPPreference
MM_CORE_DBG += ADService

#MM_VIDEO_DBG
MM_VIDEO_DBG := mm-adspsvc-test

#QCOM_SETTINGS
QCOM_SETTINGS_DBG := QualcommSettings
QCOM_SETTINGS_DBG += libDiagService
QCOM_SETTINGS_DBG += QTIDiagServices

#QCOMSYSDAEMON
QCOMSYSDAEMON_DBG := qcom-system-daemon

#QMI
QMI_DBG := qmi_fw.conf
QMI_DBG += qmi_simple_ril_test
QMI_DBG += libqmi_client_qmux
QMI_DBG += libqmi_csvt_srvc
QMI_DBG += qmi_ping_clnt_test_0000
QMI_DBG += qmi_ping_clnt_test_0001
QMI_DBG += qmi_ping_clnt_test_1000
QMI_DBG += qmi_ping_clnt_test_1001
QMI_DBG += qmi_ping_clnt_test_2000
QMI_DBG += qmi_ping_svc
QMI_DBG += qmi_ping_test
QMI_DBG += qmi_test_service_clnt_test_0000
QMI_DBG += qmi_test_service_clnt_test_0001
QMI_DBG += qmi_test_service_clnt_test_1000
QMI_DBG += qmi_test_service_clnt_test_1001
QMI_DBG += qmi_test_service_clnt_test_2000
QMI_DBG += qmi_test_service_test

#SECUREMSM_DBG
SECUREMSM_DBG := invoke_test_client
SECUREMSM_DBG += libcontentcopy
SECUREMSM_DBG += prdrmkeyprov
SECUREMSM_DBG += oemprtest
SECUREMSM_DBG += prdrm_gtest
SECUREMSM_DBG += seemp_health_client_test
SECUREMSM_DBG += SeempHealthTestApp
SECUREMSM_DBG += libSecureUIApt

#SENSORS_DEBUG
ifneq ($(TARGET_HAS_LOW_RAM), true)
SENSORS_DBG := QSensorTest
endif
SENSORS_DBG += libAR_jni
SENSORS_DBG += libAR_jni_32
SENSORS_DBG += libsensor_low_lat
SENSORS_DBG += libsensor_low_lat_system
SENSORS_DBG += libsensor_reg
SENSORS_DBG += libsensor_test
SENSORS_DBG += libsensor_test_32
SENSORS_DBG += libsensor_thresh
SENSORS_DBG += libsensor_thresh_32
SENSORS_DBG += libsensor_user_cal
SENSORS_DBG += libsensor_user_cal_32
SENSORS_DBG += see_resampler
SENSORS_DBG += see_selftest
SENSORS_DBG += see_stability
SENSORS_DBG += see_workhorse
SENSORS_DBG += sns_ar_testapp
SENSORS_DBG += sns_cm_conc_test
SENSORS_DBG += sns_cm_test
SENSORS_DBG += sns_dsps_tc0001
SENSORS_DBG += sns_file_test
SENSORS_DBG += sns_hal_batch
SENSORS_DBG += ssc_drva_test
SENSORS_DBG += ssc_sensor_info
SENSORS_DBG += ssc_threshold
SENSORS_DBG += sns_ext_svc_test
SENSORS_DBG += sns_oem_test
SENSORS_DBG += sns_regedit_ssi
SENSORS_DBG += sns_smr_loopback_test
SENSORS_DBG += UnifiedSensorTestApp
SENSORS_DBG += libSEESalt
SENSORS_DBG += libUSTANative
SENSORS_DBG += libUSTANative_system
SENSORS_DBG += libSensorCalLibNative
SENSORS_DBG += libSensorCalLibNative_system

#TELEPHONY_APPS_DBG
TELEPHONY_APPS_DBG := Presence
TELEPHONY_APPS_DBG += NetworkSetting
TELEPHONY_APPS_DBG += QosTest
TELEPHONY_APPS_DBG += LDSTestApp.xml
TELEPHONY_APPS_DBG += QosTestConfig.xml
TELEPHONY_APPS_DBG += FTMMode
TELEPHONY_APPS_DBG += RoamingSettings
TELEPHONY_APPS_DBG += UniversalDownload
TELEPHONY_APPS_DBG += atfwd
TELEPHONY_APPS_DBG += atuner
TELEPHONY_APPS_DBG += qcom_qmi.xml
TELEPHONY_APPS_DBG += ModemTestMode

#THERMAL_ENGINE_DBG
THERMAL_ENGINE_DBG := libthermalioctl

#TIME_SERVICES_DBG
TIME_SERVICES_DBG := libtime_genoff

#VPP_DBG
VPP_DBG := vpplibraryunittest
VPP_DBG += vpplibraryfunctionaltest
VPP_DBG += libvpptestutils
VPP_DBG += vppipcunittest

#WEBKIT_DBG
WEBKIT_DBG := pageload_proc_plugin
WEBKIT_DBG += modemwarmup.xml

#WFD_DBG
WFD_DBG := rtspclient
WFD_DBG += rtspserver
WFD_DBG += libwfdmmsink
WFD_DBG += libwfduibcsink
WFD_DBG += libwfduibcsinkinterface

#WLAN_DBG
WLAN_DBG := abtfilt
WLAN_DBG += artagent
WLAN_DBG += ar6004_wlan.conf
WLAN_DBG += ar6004_fw_12
WLAN_DBG += ar6004_bdata_12
WLAN_DBG += ar6004_usb_fw_13
WLAN_DBG += ar6004_usb_bdata_13
WLAN_DBG += ar6004_sdio_fw_13
WLAN_DBG += ar6004_sdio_bdata_13
WLAN_DBG += ar6004_usb_fw_ext_13
WLAN_DBG += ar6004_fw_30
WLAN_DBG += ar6004_usb_bdata_30
WLAN_DBG += ar6004_sdio_bdata_30
WLAN_DBG += proprietary_pronto_wlan.ko
WLAN_DBG += bdata.bin
WLAN_DBG += fw.ram.bin
WLAN_DBG += bdata.bin_sdio
WLAN_DBG += bdata.bin_usb
WLAN_DBG += fw_ext.ram.bin
WLAN_DBG += fw.ram.bin_sdio
WLAN_DBG += fw.ram.bin_usb

#QUICKBOOT
QUICKBOOT := QuickBoot

#SNAPDRAGON_SDK

SNAPDRAGON_SDK := com.qualcomm.snapdragon.sdk.deviceinfo.DeviceInfo
SNAPDRAGON_SDK += com.qualcomm.snapdragon.sdk.deviceinfo.DeviceInfoHelper
SNAPDRAGON_SDK += com.qualcomm.snapdragon.sdk.face.FacialProcessing
SNAPDRAGON_SDK += com.qualcomm.snapdragon.sdk.face.FacialProcessingHelper
SNAPDRAGON_SDK += libfacialproc_jni

#QMMF_SDK_DEBUG
ifeq ($(TARGET_BOARD_PLATFORM),qcs605)
QMMF_SDK_DEBUG := qmmf_av_codec_test
QMMF_SDK_DEBUG += qmmf_av_codec_gtest
QMMF_SDK_DEBUG += qmmf_av_codec_audio_decode_test
QMMF_SDK_DEBUG += qmmf_recorder_gtest
QMMF_SDK_DEBUG += qmmf_recorder_test
QMMF_SDK_DEBUG += qmmf_recorder_timelapse
QMMF_SDK_DEBUG += qmmf_player_gtest
QMMF_SDK_DEBUG += qmmf_player_test
QMMF_SDK_DEBUG += qmmf_player_parser_test
QMMF_SDK_DEBUG += qmmf_display_gtest
QMMF_SDK_DEBUG += qmmf_display_test
QMMF_SDK_DEBUG += qmmf_overlay_test
QMMF_SDK_DEBUG += qmmf_camera_adaptor_gtest
QMMF_SDK_DEBUG += qmmf_camera_dual_adaptor_gtest
QMMF_SDK_DEBUG += qmmf_audio_test
QMMF_SDK_DEBUG += qmmf_system_test
QMMF_SDK_DEBUG += qmmf_recorder_360cam_gtest
QMMF_SDK_DEBUG += qmmf_recorder_video_timelapse
QMMF_SDK_DEBUG += qmmf_transcode_app
QMMF_SDK_DEBUG += qmmf_algo_interface_gtest
QMMF_SDK_DEBUG += qmmf_utils_gtest

#QMMF_WEBSERVER_DEBUG
QMMF_WEBSERVER_DEBUG += qmmf_http_interface_gtest
endif

PRODUCT_PACKAGES_DEBUG += $(SNAPDRAGON_SDK)

PRODUCT_PACKAGES_DEBUG += $(BT_DBG)
PRODUCT_PACKAGES_DEBUG += $(CNE_DBG)
PRODUCT_PACKAGES_DEBUG += $(CNESETTINGS_DBG)
PRODUCT_PACKAGES_DEBUG += $(COMMON_DBG)
PRODUCT_PACKAGES_DEBUG += $(DATA_DBG)
PRODUCT_PACKAGES_DEBUG += $(DIAG_DBG)
PRODUCT_PACKAGES_DEBUG += $(FM_DBG)
#Disable GPS on automotive hypervisor builds.
ifeq ($(ENABLE_HYP),)
PRODUCT_PACKAGES_DEBUG += $(GPS_DBG)
endif
PRODUCT_PACKAGES_DEBUG += $(HY11_HY22_diff_debug)
PRODUCT_PACKAGES_DEBUG += $(KERNEL_TEST_DBG)
PRODUCT_PACKAGES_DEBUG += $(KS_DBG)
PRODUCT_PACKAGES_DEBUG += $(MM_AUDIO_DBG)
PRODUCT_PACKAGES_DEBUG += $(MM_CAMERA_DBG)
PRODUCT_PACKAGES_DEBUG += $(MM_CORE_DBG)
PRODUCT_PACKAGES_DEBUG += $(MM_VIDEO_DBG)
PRODUCT_PACKAGES_DEBUG += $(QCAT_UNBUFFERED)
PRODUCT_PACKAGES_DEBUG += $(QCOM_SETTINGS_DBG)
PRODUCT_PACKAGES_DEBUG += $(QMI_DBG)
PRODUCT_PACKAGES_DEBUG += $(QCOMSYSDAEMON_DBG)
PRODUCT_PACKAGES_DEBUG += $(SECUREMSM_DBG)
PRODUCT_PACKAGES_DEBUG += $(SENSORS_DBG)
PRODUCT_PACKAGES_DEBUG += $(TELEPHONY_APPS_DBG)
PRODUCT_PACKAGES_DEBUG += $(THERMAL_ENGINE_DBG)
PRODUCT_PACKAGES_DEBUG += $(TIME_SERVICES_DBG)
PRODUCT_PACKAGES_DEBUG += $(VPP_DBG)
PRODUCT_PACKAGES_DEBUG += $(WEBKIT_DBG)
PRODUCT_PACKAGES_DEBUG += $(WFD_DBG)
PRODUCT_PACKAGES_DEBUG += $(WLAN_DBG)
PRODUCT_PACKAGES_DEBUG += $(QUICKBOOT)
PRODUCT_PACKAGES_DEBUG += $(CSK)
PRODUCT_PACKAGES_DEBUG += $(PERF_DBG)
ifeq ($(TARGET_BOARD_AUTO),true)
PRODUCT_PACKAGES_DEBUG += $(TVINPUT_APP)
PRODUCT_PACKAGES_DEBUG += $(TVINPUT_SERVICE)
endif
ifeq ($(TARGET_BOARD_PLATFORM),qcs605)
PRODUCT_PACKAGES_DEBUG += $(QMMF_SDK_DEBUG)
PRODUCT_PACKAGES_DEBUG += $(QMMF_WEBSERVER_DEBUG)
endif
