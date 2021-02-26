LOCAL_PATH:= $(call my-dir)

#Generate Diag shared library
include $(CLEAR_VARS)
libdiag_includes:= \
        $(LOCAL_PATH)/../include \

LOCAL_C_INCLUDES := $(libdiag_includes)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES:= \
	diag_lsm.c \
	diag_lsm_dci.c \
	ts_linux.c \
	diag_lsm_event.c \
	diag_lsm_log.c \
        msg_arrays_i.c \
	diag_lsm_msg.c \
	diag_lsm_pkt.c \
	diagsvc_malloc.c \
	diag_logger.c \
	diag_qshrink4_db_parser.c \
	diag_lsm_qdss.c \
	diag_lsm_adpl.c \

LOCAL_COPY_HEADERS_TO := diag/include
LOCAL_COPY_HEADERS := ../include/diagi.h
LOCAL_COPY_HEADERS += ../include/diaglogi.h
LOCAL_COPY_HEADERS += ../include/diag_lsm.h
LOCAL_COPY_HEADERS += ../include/diag_lsm_dci.h
LOCAL_COPY_HEADERS += ../include/diag.h
LOCAL_COPY_HEADERS += ../include/diagcmd.h
LOCAL_COPY_HEADERS += ../include/diagpkt.h
LOCAL_COPY_HEADERS += ../include/event_defs.h
LOCAL_COPY_HEADERS += ../include/event.h
LOCAL_COPY_HEADERS += ../include/log_codes.h
LOCAL_COPY_HEADERS += ../include/log.h
LOCAL_COPY_HEADERS += ../include/msgcfg.h
LOCAL_COPY_HEADERS += ../include/msg.h
LOCAL_COPY_HEADERS += ../include/msg_pkt_defs.h
LOCAL_COPY_HEADERS += ../include/msgtgt.h
LOCAL_COPY_HEADERS += ../include/msg_qsr.h
LOCAL_COPY_HEADERS += ../include/msg_arrays_i.h

LOCAL_CFLAGS += -DANDROID
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_STATIC_LIBRARIES += libtime_genoff
LOCAL_MODULE:= libdiag
LOCAL_MODULE_TAGS := optional debug
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

# Generate Diag static library
include $(CLEAR_VARS)
libdiag_includes:= \
        $(LOCAL_PATH)/../include

LOCAL_C_INCLUDES:= $(libdiag_includes)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES:= \
	diag_lsm.c \
	diag_lsm_dci.c \
	ts_linux.c \
	diag_lsm_event.c \
	diag_lsm_log.c \
	msg_arrays_i.c \
	diag_lsm_msg.c \
	diag_lsm_pkt.c \
	diagsvc_malloc.c \
	diag_logger.c \
	diag_qshrink4_db_parser.c \
	diag_lsm_qdss.c \
	diag_lsm_adpl.c \


LOCAL_CFLAGS += -DANDROID
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_STATIC_LIBRARIES += libtime_genoff
LOCAL_MODULE:= libdiag
LOCAL_MODULE_TAGS := optional debug
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_STATIC_LIBRARY)

# System Image Diag Shared Lib
include $(CLEAR_VARS)
libdiag_includes:= \
        $(LOCAL_PATH)/../include \

LOCAL_C_INCLUDES := $(libdiag_includes)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES:= \
	diag_lsm.c \
	diag_lsm_dci.c \
	ts_linux.c \
	diag_lsm_event.c \
	diag_lsm_log.c \
	msg_arrays_i.c \
	diag_lsm_msg.c \
	diag_lsm_pkt.c \
	diagsvc_malloc.c \
	diag_logger.c \
	diag_qshrink4_db_parser.c \
	diag_lsm_qdss.c \
	diag_lsm_adpl.c \

LOCAL_CFLAGS += -DANDROID
LOCAL_CFLAGS += -DDIAG_DISABLE_TIME_SERVICES
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_MODULE:= libdiag_system
LOCAL_MODULE_TAGS := optional debug
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)
