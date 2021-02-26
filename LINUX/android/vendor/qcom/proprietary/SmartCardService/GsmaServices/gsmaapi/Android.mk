LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_SRC_FILES := $(call all-Iaidl-files-under, src) $(call all-java-files-under, src)
LOCAL_AIDL_INCLUDES := src/com/gsma/services/nfc
LOCAL_JAVA_LIBRARIES := com.nxp.nfc.nq
LOCAL_MODULE := com.gsma.services.nfc
LOCAL_REQUIRED_MODULES:= com.gsma.services.nfc.xml

include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_JAVA_LIBRARIES := com.gsma.services.nfc com.nxp.nfc.nq
LOCAL_SRC_FILES := src/com/gsma/services/utils/Handset.java
LOCAL_MODULE := com.gsma.services.utils
LOCAL_REQUIRED_MODULES := com.gsma.services.utils.xml
include $(BUILD_JAVA_LIBRARY)
