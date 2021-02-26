# User Variables
set (SUPPORT_FUNCTION_CALL_TRACE 1)

# Common Include
include (${CMAKE_CURRENT_LIST_DIR}/../../common.cmake)

add_definitions (${CAMERA_TEST_CFLAGS})
add_definitions (${CAMERA_TEST_CPPFLAGS})
add_definitions ( -DLINUX_ENABLED )
add_definitions ( -g )
#add_definitions( -DUSE_GRALLOC1 )
add_definitions ( -DDISABLE_META_MODE=1 )
add_definitions ( -DCAMERA_STORAGE_DIR="/data/misc/camera/" )
add_definitions ( -DENABLE_VIDEO_ENCODER)
#########################################libomx_encoder#########################################################
add_library( libomx_encoder SHARED
    OMX_Encoder.cpp
    BufferManager.cpp
)

target_link_libraries (libomx_encoder cutils)
target_link_libraries (libomx_encoder utils)
target_link_libraries (libomx_encoder log)
target_link_libraries (libomx_encoder hardware)
target_link_libraries (libomx_encoder camera_metadata)
target_link_libraries (libomx_encoder glib-2.0)
target_link_libraries (libomx_encoder dl)
target_link_libraries (libomx_encoder stdc++)
target_link_libraries (libomx_encoder atomic)
target_link_libraries (libomx_encoder pthread)
target_link_libraries (libomx_encoder OmxCore)

install (TARGETS libomx_encoder LIBRARY DESTINATION /usr/lib/ )
#########################################camx-hal3-test#########################################################

add_custom_target (autogen_version_camxtest ALL
    COMMAND perl ${CMAKE_CURRENT_LIST_DIR}/version.pl ${CMAKE_CURRENT_LIST_DIR}/g_version.h
    COMMENT "autogen version file"
)

# Files and Build Type
add_executable( camx-hal3-test
     BufferManager.cpp
     QCameraHAL3Device.cpp
     QCameraHAL3Base.cpp
     QCameraHAL3TestSnapshot.cpp
     QCameraHALTestMain.cpp
     QCameraHAL3TestVideo.cpp
     QCameraTestVideoEncoder.cpp
     QCameraHAL3TestConfig.cpp
     QCameraHAL3TestPreview.cpp
     TestLog.cpp
)

add_dependencies( camx-hal3-test
 autogen_version_camxtest
)
# Libraries to compile
target_link_libraries (camx-hal3-test cutils)
target_link_libraries (camx-hal3-test utils)
target_link_libraries (camx-hal3-test log)
target_link_libraries (camx-hal3-test hardware)
target_link_libraries (camx-hal3-test camera_metadata)
target_link_libraries (camx-hal3-test glib-2.0)
target_link_libraries (camx-hal3-test dl)
target_link_libraries (camx-hal3-test stdc++)
target_link_libraries (camx-hal3-test atomic)
target_link_libraries (camx-hal3-test pthread)
target_link_libraries (camx-hal3-test camera_client)
target_link_libraries (camx-hal3-test libomx_encoder)
target_link_libraries (camx-hal3-test binder)

# Include Paths
include_directories (.)
include_directories (${KERNEL_INCDIR}/usr/include)
include_directories (${SYSTEM_INC_PATH}/system/core/include/cutils)
include_directories (${SYSTEM_INC_PATH}/system/core/include/system)
include_directories (${SYSTEM_INC_PATH}/system/core/libsystem/include)
include_directories (${SYSTEM_INC_PATH}/hardware/libhardware/include)
include_directories (${SYSTEM_INC_PATH}/hardware/qcom/media/mm-core/inc)
include_directories (${SYSTEM_INC_PATH}/hardware/qcom/display)
include_directories (${SYSTEM_INC_PATH}/qcom/display)
include_directories (${SYSTEM_INC_PATH}/hardware/libhardware/modules/gralloc)
include_directories (${SYSTEM_INC_PATH}/hardware/libhardware/include)
include_directories (${SYSTEM_INC_PATH}/media/hardware)
include_directories (${SYSTEM_INC_PATH}/libstagefrighthw)
include_directories (${SYSTEM_INC_PATH}/mm-core)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -I$(WORKSPACE)/display/display-hal/libgralloc")

install (TARGETS camx-hal3-test RUNTIME DESTINATION /usr/bin/ )
