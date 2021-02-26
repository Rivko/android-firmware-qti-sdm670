LOCAL_PATH := $(call my-dir)

MY_LOCAL_PATH := $(LOCAL_PATH)

include $(call all-subdir-makefiles)

LOCAL_PATH := $(MY_LOCAL_PATH)

DATA_TOP_DIR := ${LOCAL_PATH}/..

###################### Target ########################################
include $(CLEAR_VARS)

LOCAL_MODULE               := qcrilDataModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

LOCAL_CFLAGS               += -Wall -Werror -Wno-macro-redefined
LOCAL_CXXFLAGS             += -std=c++14
LOCAL_SRC_FILES            := $(call all-cpp-files-under, src)
LOCAL_SRC_FILES            += $(call all-c-files-under, src)

#Dont compile stub.c for target build
LOCAL_SRC_FILES := $(filter-out qcril_data_stubs.c,$(LOCAL_SRC_FILES))

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/framework
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += hardware/ril/include/telephony
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/data/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc

# Local
LOCAL_C_INCLUDES += $(DATA_TOP_DIR)/include
LOCAL_C_INCLUDES += $(DATA_TOP_DIR)/include/legacy

LOCAL_C_INCLUDES += external/nanopb-c

# This is for development only
LOCAL_CFLAGS += -DQCRIL_DATA_MODULE_DEV

LOCAL_CFLAGS += -DFEATURE_QCRIL_USE_NETCTRL
LOCAL_CFLAGS += -DRIL_REQUEST_SET_INITIAL_ATTACH_APN
LOCAL_CFLAGS += -DFEATURE_DATA_EMBMS
LOCAL_CFLAGS += -DFEATURE_DATA_LQE

LOCAL_SHARED_LIBRARIES += liblqe
LOCAL_SHARED_LIBRARIES += libqdp
LOCAL_SHARED_LIBRARIES += libdsi_netctrl
LOCAL_SHARED_LIBRARIES += libqmi

$(info Done building qcrilDataModule for target...)

include $(BUILD_STATIC_LIBRARY)

###################### Host ############################################
 include $(CLEAR_VARS)

 LOCAL_MODULE               := qcrilDataModule
 LOCAL_MODULE_OWNER         := qti
 LOCAL_PROPRIETARY_MODULE   := true
 LOCAL_MODULE_TAGS          := optional

 LOCAL_CFLAGS               += -Wall -DHOST_EXECUTABLE_BUILD -g -fprofile-arcs -ftest-coverage --coverage -fexceptions
 LOCAL_CXXFLAGS             += -std=c++14
 LOCAL_SRC_FILES            := $(call all-cpp-files-under, src)
 LOCAL_SRC_FILES            += $(call all-c-files-under, src)

 LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/
 LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/framework/
 #LOCAL_C_EXPORT_INCLUDES += $(TARGET_OUT_HEADERS)/qcril/modules/qmi/
 LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
 LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/data/inc
 LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include
 LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc

 # Local
 LOCAL_C_INCLUDES += $(DATA_TOP_DIR)/include
 LOCAL_C_INCLUDES += $(DATA_TOP_DIR)/include/legacy

 LOCAL_C_INCLUDES += external/nanopb-c

 # This is for development only
 LOCAL_CFLAGS += -DQCRIL_DATA_MODULE_DEV

 LOCAL_CFLAGS += -DFEATURE_QCRIL_USE_NETCTRL
 LOCAL_CFLAGS += -DRIL_REQUEST_SET_INITIAL_ATTACH_APN
 LOCAL_CFLAGS += -DFEATURE_DATA_EMBMS
 LOCAL_CFLAGS += -DFEATURE_DATA_LQE
 LOCAL_CFLAGS += -DQCRIL_DATA_OFFTARGET
 LOCAL_CFLAGS += -DFEATURE_DATA_LOG_STDERR
 LOCAL_CFLAGS += -DFEATURE_UNIT_TEST

 LOCAL_SHARED_LIBRARIES += libqcrilFramework
 LOCAL_SHARED_LIBRARIES += libril-qc-hal-qmi

 #$(info Done building qcrilDataModule for host...)

 #include $(BUILD_HOST_STATIC_LIBRARY)

##################################### Export headers #############################
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO      := qcril/modules/data
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../include/request)
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../include/sync)
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../include/module)
LOCAL_COPY_HEADERS         += $(call all-named-files-under,*.h,../include/UnSolMessages)
LOCAL_COPY_HEADERS         += ../include/DataCommon.h

$(info Headers exported...)

include $(BUILD_COPY_HEADERS)
