LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

#LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES := src/com/qualcomm/wfd/client/AdvancedPreferences.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/DeviceListFragment.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/Preferences.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/ServiceUtil.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/SurfaceActivity.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/WFDClientActivity.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/WfdClientUtils.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/WfdSurface.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/Whisperer.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/CombinedNetManager.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/NetDeviceIF.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/NetManagerIF.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/P2pWfdDeviceInfo.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/ResultListener.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/WifiDirectBroadcastReceiver.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/WifiP2pNetDevice.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/WifiP2pNetManager.java

LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/ConnectionInfoIF.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/LanDevice.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/LanManager.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/TcpCommand.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/TcpCommandClient.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/TcpCommandServer.java
LOCAL_SRC_FILES += src/com/qualcomm/wfd/client/net/WifiWigigP2pConnectionInfo.java

LOCAL_CERTIFICATE := platform

LOCAL_PACKAGE_NAME := WfdClient

LOCAL_CERTIFICATE := platform

LOCAL_JAVA_LIBRARIES := WfdCommon

#LOCAL_STATIC_JAVA_LIBRARIES := wigig_manager

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
