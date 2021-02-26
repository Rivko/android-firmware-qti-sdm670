sources = \
    utils/src/vpp_queue.c \
    utils/src/vpp_utils.c \
    utils/src/vpp_ion.c \
    core/src/vpp.c \
    core/src/vpp_ip.c \
    core/src/vpp_buf.c \
    core/src/vpp_uc.c \
    core/src/vpp_stats.c \
    core/src/vpp_callback.c \
    core/src/vpp_pipeline.c

AM_CFLAGS = \
    -Wundef \
    -Werror \
    -Wall \
    -Wno-unused-variable \
    -Wno-unused-but-set-variable \
    -Wno-unused-label \
    -Wno-unused-function \
    -Wno-implicit-function-declaration \
    -Wno-strict-aliasing \
    -fno-short-enums \
    -fpic \
    -g -O1 \
    -I../inc \
    -Icore/inc \
    -Iutils/inc \
    -I$(WORKSPACE)/hardware/qcom/media/mm-core/inc/ \
    -I$(WORKSPACE)/system/core/include/ \
    $(GLIB_CFLAGS) \
    -include glib.h \
    -include malloc.h \
    -include stdint.h \
    -Dstrlcat=g_strlcat \
    -DVPP_LIBRARY \
    -D_ANDROID_ \
    -D__unused=__attribute__\(\(__unused__\)\)

AM_LDFLAGS = -ldl -lpthread -lglib-2.0 -llog -lcutils -shared -avoid-version

if TARGET_MSM8998
sources += chip/msm8998/vpp_reg_msm8998.c
endif


if TARGET_MSM8996
sources += chip/msm8996/vpp_reg_msm8996.c
endif

if TARGET_MSM8953
sources += chip/msm8953/vpp_reg_msm8953.c
endif

if TARGET_MSM8952
sources += chip/msm8952/vpp_reg_msm8952.c
endif

if TARGET_MSM8937
sources += chip/msm8937/vpp_reg_msm8937.c
endif

if VPP_TARGET_USES_HVX
sources += ip/hvx/src/vpp_ip_hvx.c \
           ip/hvx/src/vpp_ip_hvx_debug.c
AM_CFLAGS += -Iip/hvx/inc
AM_LDFLAGS += -lvpphvx -l$(DSP_RPC_LIB)
endif

if VPP_TARGET_USES_FRC
sources += ip/frc/src/vpp_ip_frc_core.c \
           ip/frc/src/vpp_ip_frc_mc.c \
           ip/frc/src/vpp_ip_frc_me.c
AM_CFLAGS += -Iip/frc/inc
AM_LDFLAGS += -l$(DSP_RPC_LIB)
endif

if VPP_TARGET_USES_HVX_CORE
sources += ip/hvx_core/src/vpp_ip_hvx_core.c
AM_CFLAGS += -Iip/hvx_core/inc
AM_LDFLAGS += -lvpphvx -l$(DSP_RPC_LIB)
endif

if VPP_TARGET_USES_GPU
sources += ip/gpu/src/vpp_ip_gpu.c
AM_CFLAGS += -Iip/gpu/inc
endif

lib_LTLIBRARIES = libvpplibrary.la
libvpplibrary_la_SOURCES = $(sources)
libvpplibrary_la_CFLAGS = $(AM_CFLAGS)
libvpplibrary_la_LDFLAGS = $(AM_LDFLAGS)
