LOCAL_PATH:= $(call my-dir)
EIGENROOT        := $(notdir ${LOCAL_PATH}/3rdparty/eigen)
APP_JNI_ROOT     := ${LOCAL_PATH}

COMMON_C_INCLUDE := ${EIGENROOT}/ \
					${APP_JNI_ROOT} \
					${APP_JNI_ROOT}/poseEstimatorEigen

COMMON_LDLIBS    := libm liblog libandroid libGLESv3 libEGL

MY_GNU_C_FLAGS   := -mfloat-abi=softfp -O3 -mfpu=neon -mtune=cortex-a57 -Wno-write-strings


include $(call all-subdir-makefiles)


LOCAL_PATH := $(APP_JNI_ROOT)

include $(CLEAR_VARS)

# C Flags
LOCAL_ARM_NEON   := true
LOCAL_ARM_MODE   := arm
LOCAL_CFLAGS     += ${MY_GNU_C_FLAGS} -DEIGEN_MPL2_ONLY

# include and src
LOCAL_C_INCLUDES := ${COMMON_C_INCLUDES} \
                    ${APP_JNI_ROOT}/poseEstimatorEigen \
                    $(LOCAL_PATH)/poseEstimatorEigen/drawerAndroid \
                    $(LOCAL_PATH)/.. \
						  $(LOCAL_PATH)/libjpeg

LOCAL_SRC_FILES  := 3DR_APIs.cpp \
                    jniUtils.cpp



# library
LOCAL_SHARED_LIBRARIES += libfastcvopt \
								  libglviewer

LOCAL_STATIC_LIBRARIES += drawerAndroid


LOCAL_SHARED_LIBRARIES += libjpeg \
        				  libscveScan3D \
                          libscveCommon \
                          libscveCommon_stub

LOCAL_SHARED_LIBRARIES += $(COMMON_LDLIBS)


include vendor/qcom/proprietary/scve/reference/common.mk

# module name
LOCAL_MODULE    := lib3DR

include $(BUILD_SHARED_LIBRARY)

#LOCAL_PATH := $(APP_JNI_ROOT)

include $(CLEAR_VARS)

LOCAL_SRC_FILES  := poseEstimatorJniWrapper.cpp 

					
LOCAL_MODULE    := lib3DRgl2jni
LOCAL_SHARED_LIBRARIES += lib3DR

LOCAL_C_INCLUDES := ${COMMON_C_INCLUDES} \
                    $(LOCAL_PATH)/..

LOCAL_SHARED_LIBRARIES += $(COMMON_LDLIBS)

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
