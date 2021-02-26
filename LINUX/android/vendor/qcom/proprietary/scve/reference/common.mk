## ------------------------------------------------------------------------------
## Common make configurations for SCVE Reference Apps
## ------------------------------------------------------------------------------

# Make stand-alone apk
SCVE_MAKE_STANDALONE_APK := true

#To compile only in 32bit mode.
LOCAL_MULTILIB := 32

LOCAL_MODULE_TAGS           := tests

LOCAL_CFLAGS += -Wno-unused-parameter

ifeq ($(call is-board-platform-in-list,sdm660),true)
   CDSP_ENABLE := true
endif

MULTIDOMAIN_ENABLED_TARGETS := sdm845 sdm710 qcs605
ifeq ($(call is-board-platform-in-list,$(MULTIDOMAIN_ENABLED_TARGETS)),true)
   CDSP_ENABLE := true
   LOCAL_JNI_SHARED_LIBRARIES += libfastcvdsp_stub \
											libcdsprpc \
											libadsprpc
#temporarily add libadsprpc, until fastcv gets rid of it
endif

LOCAL_JNI_SHARED_LIBRARIES += libc++    \
                              libcutils
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS) \
                    $(TARGET_OUT_HEADERS)/scve \
                    $(TARGET_OUT_HEADERS)/fastcv \
                    vendor/qcom/proprietary/prebuilt_HY11/target/product/$(TARGET_PRODUCT)/obj/include \
                    vendor/qcom/proprietary/prebuilt_HY11/target/product/$(TARGET_PRODUCT)/obj/include/scve \
                    vendor/qcom/proprietary/prebuilt_HY11/target/product/$(TARGET_PRODUCT)/obj/include/fastcv \
                    vendor/qcom/proprietary/prebuilt_grease/target/product/$(TARGET_PRODUCT)/obj/include \
                    vendor/qcom/proprietary/prebuilt_grease/target/product/$(TARGET_PRODUCT)/obj/include/scve \
                    vendor/qcom/proprietary/prebuilt_grease/target/product/$(TARGET_PRODUCT)/obj/include/fastcv

# Make the hash-style compatible with more devices
LOCAL_LDFLAGS += -Wl,--hash-style=both

ifeq ($(SCVE_MAKE_ANDROID_L), true)
   LOCAL_C_INCLUDES_32 += prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/4.7/include \
           prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/4.7/libs/armeabi-v7a/include
else
   LOCAL_C_INCLUDES    += prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/4.7/include \
                          prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/4.7/libs/armeabi-v7a/include
endif

