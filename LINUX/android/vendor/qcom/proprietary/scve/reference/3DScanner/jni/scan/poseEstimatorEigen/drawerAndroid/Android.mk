LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := drawerAndroid

# C Flags
LOCAL_ARM_NEON   := true
LOCAL_CFLAGS     += ${MY_GNU_C_FLAGS}

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../.. \
                    $(LOCAL_PATH)/../..

#wildcard uses current path
LOCAL_SRC_FILES         := SurfaceSplattingDriver.cpp \
					       pil/File.cpp \
					       pil/Timer.cpp \
					       core/Log.cpp \
					       core/Matrix.cpp \
					       core/Point.cpp \
					       core/Quaternion.cpp \
					       core/Vector.cpp \
					       resource/Assets.cpp \
					       resource/Image.cpp \
					       resource/Model.cpp \
					       resource/Shader.cpp \
					       graphics/Blending.cpp \
					       graphics/Camera.cpp \
					       graphics/Device.cpp \
					       graphics/Frame.cpp \
					       graphics/Mesh.cpp \
					       graphics/Program.cpp \
					       graphics/Projection.cpp \
					       graphics/Sampler.cpp \
					       graphics/Texture.cpp \
					       graphics/UniformBuffer.cpp \
					       graphics/Viewport.cpp \

include vendor/qcom/proprietary/scve/reference/common.mk

LOCAL_STATIC_LIBRARIES  := viewController

LOCAL_SHARED_LIBRARIES  += libfastcvopt \
									libjpeg
		
include $(BUILD_STATIC_LIBRARY)

