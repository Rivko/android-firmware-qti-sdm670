ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../..)
else
LOCAL_PATH := $(CAMX_CDK_PATH)/generated
endif

include $(CLEAR_VARS)

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_INC_FILES :=      \
    g_facedetection/camxfdconfig.h              \
    g_facedetection/fdsetmanager.h              \
    g_parser/parameterfilesymboltable.h         \
    g_parser/parameterfilesymboltableentry.h    \
    g_parser/parametermodule.h                  \
    g_parser/parametersetmanager.h              \
    g_parser/parametertuningtypes.h             \
    g_parser/parametertypes.h                   \
    g_parser/parameterutils.h                   \
    g_sensor/camxactuatordriver.h               \
    g_sensor/camxeepromdriver.h                 \
    g_sensor/camxflashdriver.h                  \
    g_sensor/camxmoduleconfig.h                 \
    g_sensor/camxoisdriver.h                    \
    g_sensor/camxpdafconfig.h                   \
    g_sensor/camxsensorcommon.h                 \
    g_sensor/camxsensordriver.h                 \
    g_sensor/imagesensormodulesetmanager.h

ifneq ($(IQSETTING),OEM1)
LOCAL_INC_FILES +=                              \
    g_chromatix/abf_3_4_0.h                     \
    g_chromatix/abf_4_0_0.h                     \
    g_chromatix/aecarbitration.h                \
    g_chromatix/aecconvergence.h                \
    g_chromatix/aecextension.h                  \
    g_chromatix/aecface.h                       \
    g_chromatix/aecflash.h                      \
    g_chromatix/aecglobalelements.h             \
    g_chromatix/aecmetering.h                   \
    g_chromatix/aecshdr.h                       \
    g_chromatix/aecir.h                         \
    g_chromatix/aecsnapshotctrl.h               \
    g_chromatix/aecstartup.h                    \
    g_chromatix/aecstatsconfig.h                \
    g_chromatix/aectouch.h                      \
    g_chromatix/afalgotuningelements.h          \
    g_chromatix/afbafelements.h                 \
    g_chromatix/afcontrastafelements.h          \
    g_chromatix/afdciafelements.h               \
    g_chromatix/afdtuning.h                     \
    g_chromatix/afglobalelements.h              \
    g_chromatix/afhafelements.h                 \
    g_chromatix/afmonitorelements.h             \
    g_chromatix/afpdafelements.h                \
    g_chromatix/afpdlibelements.h               \
    g_chromatix/afsoftwarestatelements.h        \
    g_chromatix/aftofelements.h                 \
    g_chromatix/anr_1_0_0.h                     \
    g_chromatix/asdtuning.h                     \
    g_chromatix/asf_3_0_0.h                     \
    g_chromatix/awbglobalelements.h             \
    g_chromatix/bls_1_2_0.h                     \
    g_chromatix/bpcbcc_5_0_0.h                  \
    g_chromatix/cac_2_2_0.h                     \
    g_chromatix/cc_1_2_0.h                      \
    g_chromatix/cc_1_3_0.h                      \
    g_chromatix/cst_1_2_0.h                     \
    g_chromatix/cs_2_0_0.h                      \
    g_chromatix/cv_1_2_0.h                      \
    g_chromatix/demosaic_3_6_0.h                \
    g_chromatix/demux_1_3_0.h                   \
    g_chromatix/ds4to1_1_0_0.h                  \
    g_chromatix/eis_1_0_0.h                     \
    g_chromatix/gamma_1_5_0.h                   \
    g_chromatix/gamma_1_6_0.h                   \
    g_chromatix/gic_3_0_0.h                     \
    g_chromatix/globalelements.h                \
    g_chromatix/gra_1_0_0.h                     \
    g_chromatix/gtm_1_0_0.h                     \
    g_chromatix/hdrvideo_1_0_0.h                \
    g_chromatix/hdr_2_0_0.h                     \
    g_chromatix/hdr_2_2_0.h                     \
    g_chromatix/hnr_1_0_0.h                     \
    g_chromatix/ica_1_0_0.h                     \
    g_chromatix/iotglobalelements.h             \
    g_chromatix/ispglobalelements.h             \
    g_chromatix/linearization_3_3_0.h           \
    g_chromatix/linearization_3_4_0.h           \
    g_chromatix/lrme_1_0_0.h                    \
    g_chromatix/lsc_3_4_0.h                     \
    g_chromatix/ltm_1_3_0.h                     \
    g_chromatix/mf_1_0_0.h                      \
    g_chromatix/modawbbestshotv1.h              \
    g_chromatix/modawbfaceassistv1.h            \
    g_chromatix/modawbinitv1.h                  \
    g_chromatix/modbgstatsconfigv1.h            \
    g_chromatix/modcustrefptv1.h                \
    g_chromatix/moddav1.h                       \
    g_chromatix/moddistwvv1.h                   \
    g_chromatix/moddsfv1.h                      \
    g_chromatix/moddualcamsyncv1.h              \
    g_chromatix/modflashdecv1.h                 \
    g_chromatix/modflashrefptv1.h               \
    g_chromatix/modflashwv1.h                   \
    g_chromatix/modhistctrlv1.h                 \
    g_chromatix/modilluwvv1.h                   \
    g_chromatix/modmlczv1.h                     \
    g_chromatix/modmwbv1.h                      \
    g_chromatix/modrefptv1.h                    \
    g_chromatix/modsaagwv1.h                    \
    g_chromatix/modsagenv1.h                    \
    g_chromatix/modsensorcalv1.h                \
    g_chromatix/modsfstatscrv1.h                \
    g_chromatix/modsfsubsmpv1.h                 \
    g_chromatix/modspecilluv1.h                 \
    g_chromatix/modtmpconvv1.h                  \
    g_chromatix/modtrigladjv1.h                 \
    g_chromatix/pdpc_1_1_0.h                    \
    g_chromatix/pdpc_2_0_0.h                    \
    g_chromatix/pedestal_1_3_0.h                \
    g_chromatix/rertuning.h                     \
    g_chromatix/sce_1_1_0.h                     \
    g_chromatix/shdr_1_0_0.h                    \
    g_chromatix/iwarp_1_0_0.h                   \
    g_chromatix/ioteis_1_0_0.h                  \
    g_chromatix/yuvcac_1_0_0.h                  \
    g_chromatix/shdr_3_0_0.h                    \
    g_chromatix/tdl_1_0_0.h                     \
    g_chromatix/tf_1_0_0.h                      \
    g_chromatix/tintless_2_0_0.h                \
    g_chromatix/tmc_1_0_0.h                     \
    g_chromatix/tuningsetmanager.h              \
    g_chromatix/upscale_2_0_0.h
endif

LOCAL_SRC_FILES :=      \
    g_facedetection/camxfdconfig.cpp            \
    g_facedetection/fdsetmanager.cpp            \
    g_parser/parameterfilesymboltable.cpp       \
    g_parser/parameterfilesymboltableentry.cpp  \
    g_parser/parametermodule.cpp                \
    g_parser/parametersetmanager.cpp            \
    g_parser/parametertypes.cpp                 \
    g_parser/parameterutils.cpp                 \
    g_sensor/camxactuatordriver.cpp             \
    g_sensor/camxeepromdriver.cpp               \
    g_sensor/camxflashdriver.cpp                \
    g_sensor/camxmoduleconfig.cpp               \
    g_sensor/camxoisdriver.cpp                  \
    g_sensor/camxpdafconfig.cpp                 \
    g_sensor/camxsensorcommon.cpp               \
    g_sensor/camxsensordriver.cpp               \
    g_sensor/imagesensormodulesetmanager.cpp

ifneq ($(IQSETTING),OEM1)
LOCAL_SRC_FILES +=                              \
    g_chromatix/abf_3_4_0.cpp                   \
    g_chromatix/abf_4_0_0.cpp                   \
    g_chromatix/aecarbitration.cpp              \
    g_chromatix/aecconvergence.cpp              \
    g_chromatix/aecextension.cpp                \
    g_chromatix/aecface.cpp                     \
    g_chromatix/aecflash.cpp                    \
    g_chromatix/aecglobalelements.cpp           \
    g_chromatix/aecmetering.cpp                 \
    g_chromatix/aecshdr.cpp                     \
    g_chromatix/aecir.cpp                       \
    g_chromatix/aecsnapshotctrl.cpp             \
    g_chromatix/aecstartup.cpp                  \
    g_chromatix/aecstatsconfig.cpp              \
    g_chromatix/aectouch.cpp                    \
    g_chromatix/afalgotuningelements.cpp        \
    g_chromatix/afbafelements.cpp               \
    g_chromatix/afcontrastafelements.cpp        \
    g_chromatix/afdciafelements.cpp             \
    g_chromatix/afdtuning.cpp                   \
    g_chromatix/afglobalelements.cpp            \
    g_chromatix/afhafelements.cpp               \
    g_chromatix/afmonitorelements.cpp           \
    g_chromatix/afpdafelements.cpp              \
    g_chromatix/afpdlibelements.cpp             \
    g_chromatix/afsoftwarestatelements.cpp      \
    g_chromatix/aftofelements.cpp               \
    g_chromatix/anr_1_0_0.cpp                   \
    g_chromatix/asdtuning.cpp                   \
    g_chromatix/asf_3_0_0.cpp                   \
    g_chromatix/awbglobalelements.cpp           \
    g_chromatix/bls_1_2_0.cpp                   \
    g_chromatix/bpcbcc_5_0_0.cpp                \
    g_chromatix/cac_2_2_0.cpp                   \
    g_chromatix/cc_1_2_0.cpp                    \
    g_chromatix/cc_1_3_0.cpp                    \
    g_chromatix/cst_1_2_0.cpp                   \
    g_chromatix/cs_2_0_0.cpp                    \
    g_chromatix/cv_1_2_0.cpp                    \
    g_chromatix/demosaic_3_6_0.cpp              \
    g_chromatix/demux_1_3_0.cpp                 \
    g_chromatix/ds4to1_1_0_0.cpp                \
    g_chromatix/eis_1_0_0.cpp                   \
    g_chromatix/gamma_1_5_0.cpp                 \
    g_chromatix/gamma_1_6_0.cpp                 \
    g_chromatix/gic_3_0_0.cpp                   \
    g_chromatix/globalelements.cpp              \
    g_chromatix/gra_1_0_0.cpp                   \
    g_chromatix/gtm_1_0_0.cpp                   \
    g_chromatix/hdrvideo_1_0_0.cpp              \
    g_chromatix/hdr_2_0_0.cpp                   \
    g_chromatix/hdr_2_2_0.cpp                   \
    g_chromatix/hnr_1_0_0.cpp                   \
    g_chromatix/ica_1_0_0.cpp                   \
    g_chromatix/iotglobalelements.cpp           \
    g_chromatix/ispglobalelements.cpp           \
    g_chromatix/linearization_3_3_0.cpp         \
    g_chromatix/linearization_3_4_0.cpp         \
    g_chromatix/lrme_1_0_0.cpp                  \
    g_chromatix/lsc_3_4_0.cpp                   \
    g_chromatix/ltm_1_3_0.cpp                   \
    g_chromatix/mf_1_0_0.cpp                    \
    g_chromatix/modawbbestshotv1.cpp            \
    g_chromatix/modawbfaceassistv1.cpp          \
    g_chromatix/modawbinitv1.cpp                \
    g_chromatix/modbgstatsconfigv1.cpp          \
    g_chromatix/modcustrefptv1.cpp              \
    g_chromatix/moddav1.cpp                     \
    g_chromatix/moddistwvv1.cpp                 \
    g_chromatix/moddsfv1.cpp                    \
    g_chromatix/moddualcamsyncv1.cpp            \
    g_chromatix/modflashdecv1.cpp               \
    g_chromatix/modflashrefptv1.cpp             \
    g_chromatix/modflashwv1.cpp                 \
    g_chromatix/modhistctrlv1.cpp               \
    g_chromatix/modilluwvv1.cpp                 \
    g_chromatix/modmlczv1.cpp                   \
    g_chromatix/modmwbv1.cpp                    \
    g_chromatix/modrefptv1.cpp                  \
    g_chromatix/modsaagwv1.cpp                  \
    g_chromatix/modsagenv1.cpp                  \
    g_chromatix/modsensorcalv1.cpp              \
    g_chromatix/modsfstatscrv1.cpp              \
    g_chromatix/modsfsubsmpv1.cpp               \
    g_chromatix/modspecilluv1.cpp               \
    g_chromatix/modtmpconvv1.cpp                \
    g_chromatix/modtrigladjv1.cpp               \
    g_chromatix/pdpc_1_1_0.cpp                  \
    g_chromatix/pdpc_2_0_0.cpp                  \
    g_chromatix/pedestal_1_3_0.cpp              \
    g_chromatix/rertuning.cpp                   \
    g_chromatix/sce_1_1_0.cpp                   \
    g_chromatix/shdr_1_0_0.cpp                  \
    g_chromatix/iwarp_1_0_0.cpp                 \
    g_chromatix/ioteis_1_0_0.cpp                \
    g_chromatix/yuvcac_1_0_0.cpp                \
    g_chromatix/shdr_3_0_0.cpp                  \
    g_chromatix/tdl_1_0_0.cpp                   \
    g_chromatix/tf_1_0_0.cpp                    \
    g_chromatix/tintless_2_0_0.cpp              \
    g_chromatix/tmc_1_0_0.cpp                   \
    g_chromatix/tuningsetmanager.cpp            \
    g_chromatix/upscale_2_0_0.cpp
endif

# Put here any libraries that should be linked by CAMX projects
LOCAL_C_LIBS := $(CAMX_C_LIBS)

# Paths to included headers
LOCAL_C_INCLUDES :=                             \
    $(CAMX_C_INCLUDES)                          \
    $(CAMX_CDK_PATH)/generated/g_parser

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)                  \
    -Wno-uninitialized                          \
    -Wno-unused-parameter                       \
    -Wno-unused-variable

LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

# Binary name
LOCAL_MODULE := libcamxgenerated

LOCAL_LDFLAGS :=
LOCAL_LDLIBS :=
include $(BUILD_STATIC_LIBRARY)

# Only build the bins if this is a non-VGDB build
ifeq ($(CAMX_EXT_VBUILD),)

####################################################################################################
# Camera-Specific XML to BIN file generation.
#
# This must go after BUILD_STATIC_LIBRARY because we need the value of LOCAL_BUILT_MODULE.
# The script when it run in the $(shell command only figures out if there's work to be done. If
# so, it outputs:
#  $(LOCAL_BUILT_MODULE) : CamxBuildBins
#
# If not, it prints nothing, which will keep the .lib from having to be built every time.
####################################################################################################

CAMX_GEN_HEADERS := $(addprefix $(LOCAL_PATH)/,$(LOCAL_INC_FILES))
CAMX_GEN_SOURCES := $(addprefix $(LOCAL_PATH)/,$(LOCAL_SRC_FILES))
include $(CLEAR_VARS)

LOCAL_MODULE := camx_parsergen_headers

ifeq ($(CAMX_PYTHON),)
  CAMX_PYTHON           := prebuilts/python/linux-x86/2.7.5/bin/python
endif
CAMX_BUILDBINS_SCRIPT := $(CAMX_CDK_PATH)/tools/buildbins.py
CAMX_BIN_OUTPUT_LOC   := $(ANDROID_BUILD_TOP)/$(TARGET_OUT_VENDOR)/lib/camera,$(ANDROID_BUILD_TOP)/$(TARGET_OUT_VENDOR)/lib64/camera

.PHONY: CamxBuildBins

# Invoke the buildbins.py script every time someone runs make. That seems excessive, but in this mode
# its just doing the same checks that make would -- namely comparing the input dates to the output
# dates. The output is then evaluated as makefile syntax.
#
# If there's nothing to be done, the script returns just a commented line. If there is work to be
# done, then the script returns "CAMX_DO_BUILD_GEN := true", which is then read in as makefile
# information, and used to force regeneration of anything that uses generated headers.
CAMX_BIN_MAKEFILE_CODE := $(shell $(CAMX_PYTHON) $(CAMX_BUILDBINS_SCRIPT) --check-deps-only --bin-path=$(CAMX_BIN_OUTPUT_LOC))

$(info (CamX Bin Generation Dep Check) $(CAMX_BIN_MAKEFILE_CODE))
$(eval $(CAMX_BIN_MAKEFILE_CODE))

GEN := CamxBuildBins
$(GEN): PRIVATE_CUSTOM_TOOL := $(CAMX_PYTHON) $(CAMX_BUILDBINS_SCRIPT) --bin-path=$(CAMX_BIN_OUTPUT_LOC)
$(GEN): PRIVATE_OUTPUT_DIR  := $(LOCAL_PATH)
$(GEN):
	@echo Building Camera Binaries, please be patient, this can take up to 20 minutes.
	$(transform-generated-source)

ifeq ($(CAMX_DO_GEN_BUILD),true)
$(CAMX_GEN_HEADERS) : CamxBuildBins
$(CAMX_GEN_SOURCES) : CamxBuildBins
LOCAL_GENERATED_SOURCES += $(CAMX_GEN_HEADERS)
endif

endif

LOCAL_EXPORT_C_INCLUDE_DIRS =          \
    $(LOCAL_PATH)/g_parser             \
    $(LOCAL_PATH)/g_chromatix          \
    $(LOCAL_PATH)/g_facedetection      \
    $(LOCAL_PATH)/g_sensors

include $(BUILD_HEADER_LIBRARY)
