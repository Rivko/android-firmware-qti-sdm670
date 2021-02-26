if (NOT DEFINED GLIB2_PATH)
    find_path (GLIB2_PATH glib.h PATH_SUFFIXES glib-2.0)
endif ()

if (NOT DEFINED GLIB2_INTERNAL_PATH)
    find_path (GLIB2_INTERNAL_PATH glibconfig.h
        PATHS
                /usr/lib/x86_64-linux-gnu/
                /usr/local/lib/
                /usr/lib/
        PATH_SUFFIXES
                glib-2.0/include)
endif ()

if (NOT DEFINED GLIB2_LIBRARY_PATH)
    find_library (GLIB2_LIBRARY_PATH NAMES glib-2.0
        PATHS
                /usr/lib/x86_64-linux-gnu/
                /usr/local/lib/
                /usr/lib)
endif ()

if (GLIB2_PATH)
        include_directories (${GLIB2_PATH})
endif ()

if (GLIB2_INTERNAL_PATH)
        include_directories (${GLIB2_INTERNAL_PATH})
endif ()

find_path (SYSTEM_INC_PATH pthread.h)
if (SYSTEM_INC_PATH)
	include_directories (${SYSTEM_INC_PATH})
endif()


set (CAMERA_INC_PATH ${SYSTEM_INC_PATH}/camera/)
include_directories (${CAMERA_INC_PATH})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PresilPlatform "x64")
else (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PresilPlatform "Win32")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

IF( NOT PresilConfiguration )
   SET( PresilConfiguration Debug )
ENDIF()

set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --verbose")

set (CAMX_OS linux)


set (CMAKE_SHARED_LIBRARY_PREFIX "")

set (IQSETTING "")

set (CAMX_SYSTEM_PATH ${CMAKE_CURRENT_LIST_DIR}/../camx-lib/system)
set (CAMX_C_INCLUDES
    ${CAMX_CDK_PATH}
    ${CAMX_CDK_PATH}/chi
    ${CAMX_CDK_PATH}/fd
    ${CAMX_CDK_PATH}/common
    ${CAMX_CDK_PATH}/isp
    ${CAMX_CDK_PATH}/node
    ${CAMX_CDK_PATH}/pdlib
    ${CAMX_CDK_PATH}/stats
    ${CAMX_CDK_PATH}/sensor
    ${CAMX_PATH}/src/core
    ${CAMX_PATH}/src/core/chi
    ${CAMX_PATH}/src/core/hal
    ${CAMX_PATH}/src/core/halutils
    ${CAMX_PATH}/src/core/ncs
    ${CAMX_PATH}/src/csl
    ${CAMX_CDK_PATH}/generated/g_chromatix
    ${CAMX_CDK_PATH}/generated/g_parser
    ${CAMX_CDK_PATH}/generated/g_sensor
    ${CAMX_CDK_PATH}
    ${CAMX_PATH}/src/osutils
    ${CAMX_PATH}/src/sdk
    ${CAMX_PATH}/src/swl
    ${CAMX_PATH}/src/swl/sensor
    ${CAMX_PATH}/src/swl/stats
    ${CAMX_PATH}/src/utils
    ${CAMX_PATH}/src/utils/scope
    ${CAMX_SYSTEM_PATH}/stripinglib/fw_core/hld/stripinglibrary/inc
    ${CAMX_SYSTEM_PATH}/stripinglib/fw_core/common/chipinforeader/inc
    ${CAMX_SYSTEM_PATH}/ifestripinglib/stripinglibrary/inc
    ${CAMX_SYSTEM_PATH}/tintlessalgo/inc
    ${CAMX_SYSTEM_PATH}/debugdata/common/inc
    ${CAMX_SYSTEM_PATH}/firmware
    ${CAMX_SYSTEM_PATH}/swprocessalgo
    ${CAMX_SYSTEM_PATH}/defog/inc
    ${CAMX_INCLUDE_PATH}
    ${CAMX_EXT_INCLUDE}
    ${CAMX_EXT_INCLUDE}/system
    ${CAMX_EXT_INCLUDE}/libssc
    ${ANDROID_INCLUDES}
    ${KERNEL_INCDIR}/usr/include
   )

set (CAMX_C_LIBS "")

# Common C flags for the project
set (CAMX_CFLAGS
    -fPIC
    -DCAMX
    -D_LINUX
    -DANDROID
    -DLE_CAMERA
    -DSNS_LE_QCS605
    -DUSE_LIBGBM
    -D_GNU_SOURCE
    -Dstrlcpy=g_strlcpy
    -Dstrlcat=g_strlcat
    -DCAMX_LOGS_ENABLED=1
    -DCAMX_TRACES_ENABLED=0
    -DPTHREAD_TLS=1
    -fno-stack-protector
    -Wall
    -Wno-error
    )

# Common C++ flags for the project
set (CAMX_CPPFLAGS
    -fPIC
    -Wno-invalid-offsetof
    -include stdint.h
    -include sys/ioctl.h
    -include glib.h
    )


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread -lrt -lm -lglib-2.0 -ldl -latomic -lsync")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -lrt -lm -lglib-2.0 -ldl -latomic -lsync")
