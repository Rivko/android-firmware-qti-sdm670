REFERENCE_ROOT := $(call my-dir)

#include $(REFERENCE_ROOT)/CleverCapture/Android.mk
#include $(REFERENCE_ROOT)/ImageStudio/Android.mk
##### Enable only social Camera apk for the qcs605
ifneq ($(call is-board-platform-in-list,qcs605),true)
   include $(REFERENCE_ROOT)/ObjectTracker/Android.mk
   include $(REFERENCE_ROOT)/Panorama/Android.mk
endif
include $(REFERENCE_ROOT)/SocialCamera/Android.mk
#include $(REFERENCE_ROOT)/TextReco/Android.mk

#SCVE_VIDEOSUMMARY_APK_TARGET := msm8998 sdm660 sdm845
#ifeq ($(call is-board-platform-in-list,$(SCVE_VIDEOSUMMARY_APK_TARGET)),true)
#   include $(REFERENCE_ROOT)/VideoSummary/Android.mk
#endif

SCVE_MAKE_SCAN3D_TARGETS := sdm845
ifeq ($(call is-board-platform-in-list,$(SCVE_MAKE_SCAN3D_TARGETS)),true)
# Uncomment this line below after downloading depth driver libraries.
#   include $(REFERENCE_ROOT)/3DScanner/Android.mk
endif
