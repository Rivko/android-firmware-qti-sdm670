/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "LocSvc_HIDL_WiFiDBReceiver_jni"
#define LOG_NDEBUG 0
#define LOCATION_CLASS_NAME "android/location/Location"

#define MAX_NUMBER_APS 500

#include <inttypes.h>
#include "JNIHelp.h"
#include "jni.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"
#include "izat_wifi_db_receiver.h"
#include "utils_jni.h"

#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>

using ::vendor::qti::gnss::V1_0::ILocHidlGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlWiFiDBReceiver;
using ::vendor::qti::gnss::V1_0::ILocHidlWiFiDBReceiverCallback;

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::hidl_string;

using namespace android;

static void* sWiFiDBReceiverHandle = NULL;
static JavaVM* sWiFiDBRecCbJavaVm = NULL;
static jobject sCallbacksObj = NULL;

static jmethodID method_onAPListAvailable = NULL;
static jmethodID method_onStatusUpdate = NULL;
static jmethodID method_onServiceRequest = NULL;

static jclass class_APInfo = NULL;
static jclass class_APLocationData = NULL;
static jclass class_APSpecialInfo = NULL;

static uint64_t convertMacStrto48R(const char *mac) {
    if (0 != mac) {

        int hi24, lo24;
        sscanf(mac, "%06X%06X", &lo24, &hi24);
        uint64_t temp = (unsigned)hi24;
        temp <<= 24;
        temp |= (uint64_t)lo24;
        return temp;
    }
    return 0;
}

static void convertMac48RtoStr(char mac_str[], int str_size, uint64_t mac) {
    snprintf(mac_str, str_size, "%06X%06X",
             (unsigned int)(mac & 0xFFFFFF), (unsigned int)(mac >> 24));
}

#define NULL_ENV_POINTER       1
#define UNAVAILABLE_MAC_STRING 2
#define UNAVAILABLE_MAC_CHAR   3

static int TranslateFromObjectToHidl(const jobject locationObject,
                                     ILocHidlWiFiDBReceiver::LocHidlApLocationData *location_data) {
    int result = 0;
    JNIEnv* env = NULL;
    if (sWiFiDBRecCbJavaVm) {
        sWiFiDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    }

    if (NULL == env) {
        result = NULL_ENV_POINTER;
    } else {

        static jfieldID jFieldAPLocationData_mac =
            env->GetFieldID(class_APLocationData, "mMacAddress",
                            "Ljava/lang/String;");
        static jfieldID jFieldAPLocationData_lat =
            env->GetFieldID(class_APLocationData, "mLatitude",
                            "F");
        static jfieldID jFieldAPLocationData_lon =
            env->GetFieldID(class_APLocationData, "mLongitude",
                            "F");
        static jfieldID jFieldAPLocationData_mar =
            env->GetFieldID(class_APLocationData, "mMaxAntenaRange",
                            "F");
        static jfieldID jFieldAPLocationData_he =
            env->GetFieldID(class_APLocationData, "mHorizontalError",
                            "F");
        static jfieldID jFieldAPLocationData_reliability =
            env->GetFieldID(class_APLocationData, "mReliability",
                            "I");
        static jfieldID jFieldAPLocationData_valid =
            env->GetFieldID(class_APLocationData, "mValidBits",
                            "I");

        jstring strMac = (jstring)env->GetObjectField(locationObject,
                                             jFieldAPLocationData_mac);
        if (NULL == strMac) {
            result = UNAVAILABLE_MAC_STRING;
        } else {

            jfloat lat  = env->GetFloatField(locationObject,
                                             jFieldAPLocationData_lat);
            jfloat lon  = env->GetFloatField(locationObject,
                                             jFieldAPLocationData_lon);
            jfloat mar  = env->GetFloatField(locationObject,
                                             jFieldAPLocationData_mar);
            jfloat hepe = env->GetFloatField(locationObject,
                                             jFieldAPLocationData_he);
            jint rel    = env->GetIntField(locationObject,
                                           jFieldAPLocationData_reliability);
            jint valid  = env->GetIntField(locationObject,
                                           jFieldAPLocationData_valid);

            const char *mac = env->GetStringUTFChars(strMac, NULL);
            if (NULL == mac) {
                result = UNAVAILABLE_MAC_CHAR;
            } else {
                location_data->mac_R48b = convertMacStrto48R(mac);
                env->ReleaseStringUTFChars(strMac, NULL);

                location_data->latitude = (float)lat;
                location_data->longitude = (float)lon;
                location_data->max_antenna_range = (float)mar;
                location_data->horizontal_error = (float)hepe;
                location_data->reliability = (uint8_t)rel;
                location_data->valid_bits = (int8_t)valid;
            }
        }
    }

    return result;
}

static int TranslateFromObjectToHidl(const jobject splecialInfoObject,
                                     ILocHidlWiFiDBReceiver::LocHidlApSpecialInfo *special_info) {
    int result = 0;
    JNIEnv* env = NULL;
    if (sWiFiDBRecCbJavaVm) {
        sWiFiDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    }

    if (NULL == env) {
        result = NULL_ENV_POINTER;
    } else {

        static jfieldID jFieldAPSpecialInfo_mac =
            env->GetFieldID(class_APSpecialInfo, "mMacAddress",
                            "Ljava/lang/String;");
        static jfieldID jFieldAPSpecialInfo_info =
            env->GetFieldID(class_APSpecialInfo, "mInfo", "I");
        jstring strMac = (jstring)env->GetObjectField(splecialInfoObject,
                                             jFieldAPSpecialInfo_mac);
        if (NULL == strMac) {
            result = UNAVAILABLE_MAC_STRING;
        } else {

            jint info  = env->GetIntField(splecialInfoObject,
                                          jFieldAPSpecialInfo_info);

            const char *mac = env->GetStringUTFChars(strMac, NULL);
            if (NULL == mac) {
                result = UNAVAILABLE_MAC_CHAR;
            } else {
                special_info->mac_R48b = convertMacStrto48R(mac);
                env->ReleaseStringUTFChars(strMac, NULL);

                special_info->info = (uint8_t)info;
            }
        }
    }
    return result;
}

static void TranslateHidlToObject(const ILocHidlWiFiDBReceiverCallback::LocHidlApInfo& ap, jobject& apInfoObject) {
    JNIEnv* env = NULL;
    if (sWiFiDBRecCbJavaVm) {
        sWiFiDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    }

    if (env) {

        if (class_APInfo) {
            jmethodID jConstructorAPInfo = env->GetMethodID(class_APInfo,
                                                            "<init>",
                                                            "()V");
            static jfieldID jFieldMac = env->GetFieldID(class_APInfo,
                                                        "mMacAddress",
                                                        "Ljava/lang/String;");
            static jfieldID jFieldCellType = env->GetFieldID(class_APInfo,
                                                             "mCellType", "I");
            static jfieldID jFieldCell1 = env->GetFieldID(class_APInfo,
                                                          "mCellRegionID1", "I");
            static jfieldID jFieldCell2 = env->GetFieldID(class_APInfo,
                                                          "mCellRegionID2", "I");;
            static jfieldID jFieldCell3 = env->GetFieldID(class_APInfo,
                                                          "mCellRegionID3", "I");
            static jfieldID jFieldCell4 = env->GetFieldID(class_APInfo,
                                                          "mCellRegionID4", "I");
            static jfieldID jFieldSSID = env->GetFieldID(class_APInfo,
                                                         "mSSID", "[B");
            apInfoObject = env->NewObject(class_APInfo, jConstructorAPInfo, NULL);
            if (apInfoObject) {
                char macStr[13];
                convertMac48RtoStr(macStr, 13, ap.mac_R48b);
                jstring jStrMac = env->NewStringUTF(macStr);
                if (jStrMac) {
                    env->SetObjectField(apInfoObject, jFieldMac, jStrMac);
                    env->SetIntField(apInfoObject, jFieldCellType, ap.cell_type);
                    env->SetIntField(apInfoObject, jFieldCell1, ap.cell_id1);
                    env->SetIntField(apInfoObject, jFieldCell2, ap.cell_id2);
                    env->SetIntField(apInfoObject, jFieldCell3, ap.cell_id3);
                    env->SetIntField(apInfoObject, jFieldCell4, ap.cell_id4);
                    jbyteArray jssid = env->NewByteArray(ap.ssid_valid_byte_count);
                    if (jssid) {
                        env->SetByteArrayRegion(jssid, 0, ap.ssid_valid_byte_count,
                                                (jbyte*) ap.ssid.c_str());
                        env->SetObjectField(apInfoObject, jFieldSSID, jssid);
                        // Managing overwhelming number of requests
                        env->DeleteLocalRef(jssid);
                    }
                }
                // Managing overwhelming number of requests
                env->DeleteLocalRef(jStrMac);
            }
        }
    }
}

/* ===========================================================
 *   HIDL Callbacks : ILocHidlWiFiDBReceiverCallback.hal
 * ===========================================================*/
struct LocHidlWiFiDBReceiverCallback :
  public ILocHidlWiFiDBReceiverCallback {

  Return<void> apListUpdateCallback(
          const hidl_vec<ILocHidlWiFiDBReceiverCallback::LocHidlApInfo>& apInfoList,
          int32_t apListSize) override;
  Return<void> statusUpdateCallback(
          bool status, const hidl_string& reason) override;
  Return<void> serviceRequestCallback() override;
  Return<void> attachVmOnCallback() override;
};

Return<void> LocHidlWiFiDBReceiverCallback::apListUpdateCallback(
         const hidl_vec<LocHidlApInfo>& apInfoList, int32_t apListSize) {

   FROM_HIDL_SERVICE();

   JNIEnv* env = NULL;
   if (sWiFiDBRecCbJavaVm) {
       sWiFiDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
   }

   if (env && sCallbacksObj && method_onAPListAvailable) {
       if (class_APInfo) {
         jobjectArray jAPList = (jobjectArray)env->NewObjectArray(apListSize,
                                                                  class_APInfo,
                                                                  NULL);
         if (jAPList) {
             for (int32_t ii = 0; ii < apListSize; ii++) {
                 jobject jObjAPInfo = NULL;
                 auto& info = apInfoList[ii];
                 TranslateHidlToObject(info, jObjAPInfo);
                 if (jObjAPInfo) {
                     env->SetObjectArrayElement(jAPList, ii, jObjAPInfo);
                 }
                 // Managing overwhelming number of requests
                 env->DeleteLocalRef(jObjAPInfo);
             }
             env->CallVoidMethod(sCallbacksObj,
                                 method_onAPListAvailable,
                                 jAPList);
             // Managing overwhelming number of requests
             env->DeleteLocalRef(jAPList);
             checkAndClearExceptionsFromCallback(env, __FUNCTION__);
         }
      }
   }
   return Void();
}

Return<void> LocHidlWiFiDBReceiverCallback::statusUpdateCallback(
         bool status, const hidl_string& reason) {

  FROM_HIDL_SERVICE();

  JNIEnv* env = NULL;
  if (sWiFiDBRecCbJavaVm) {
      sWiFiDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  }

  if (env && sCallbacksObj && method_onStatusUpdate) {
      jstring jErrStr = env->NewStringUTF(reason.c_str());
      env->CallVoidMethod(sCallbacksObj,
                          method_onStatusUpdate,
                          status,
                          jErrStr);
      checkAndClearExceptionsFromCallback(env, __FUNCTION__);
  }
  return Void();
}

Return<void> LocHidlWiFiDBReceiverCallback::serviceRequestCallback() {

  FROM_HIDL_SERVICE();

  JNIEnv* env = NULL;
  if (sWiFiDBRecCbJavaVm) {
      sWiFiDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  }
  if (env && sCallbacksObj && method_onServiceRequest) {
      env->CallVoidMethod(sCallbacksObj,
                          method_onServiceRequest);
      checkAndClearExceptionsFromCallback(env, __FUNCTION__);
  }
  return Void();
}

Return<void> LocHidlWiFiDBReceiverCallback::attachVmOnCallback() {

  FROM_HIDL_SERVICE();

  void* clientData = NULL;

  sWiFiDBRecCbJavaVm = AndroidRuntime::getJavaVM();
  JNIEnv *callbackEnv = NULL;
  JavaVMAttachArgs args = {
              JNI_VERSION_1_6,
              "WFD Service Callback Thread",
              NULL
          };
  jint attachResult = sWiFiDBRecCbJavaVm->AttachCurrentThread(&callbackEnv, &args);
  if (attachResult != 0) {
      ALOGE("Callback thread attachment error: %d", attachResult);
      callbackEnv = NULL;
  }

  return Void();
}


/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlWiFiDBReceiver> wifiDBReceiverIface = nullptr;
static sp<ILocHidlWiFiDBReceiverCallback> wifiDBReceiverCallback = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        wifiDBReceiverIface = nullptr;
        ALOGE("%s] ILocHidlWiFiDBReceiver died with cookie: %" PRIu64, __FILE__, cookie);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

static sp<ILocHidlWiFiDBReceiver>& getWifiDBReceiverIface() {
    static uint64_t deathCount = 0;
    if (wifiDBReceiverIface == nullptr) {
        auto gnssVendorHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (gnssVendorHal == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
            auto wdr = gnssVendorHal->getExtensionLocHidlWiFiDBReceiver();

            if (!wdr.isOk()) {
                ALOGE("Unable to get a handle to WiFiDBReceiver extension !");
            } else {
                if (locHidlDeathRecipient == nullptr) {
                    locHidlDeathRecipient = new LocHidlDeathRecipient();
                }
                wifiDBReceiverIface = wdr;
                auto r = wifiDBReceiverIface->linkToDeath(locHidlDeathRecipient, deathCount++);
                if (!r.isOk() || r == false) {
                    ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                    wifiDBReceiverIface = nullptr;
                }
            }
        }
    }
    return wifiDBReceiverIface;
}

static void WiFiDBReceiver_class_init(JNIEnv* env, jclass clazz)
{
    // get references to the Java methods
    method_onAPListAvailable = env->GetMethodID(clazz,
                                                "onAPListAvailable",
                                                "([Lcom/qti/wifidbreceiver/APInfo;)V");
    method_onStatusUpdate = env->GetMethodID(clazz, "onStatusUpdate", "(ZLjava/lang/String;)V");
    method_onServiceRequest = env->GetMethodID(clazz, "onServiceRequest", "()V");

    /* Now fetch HIDL interface */
    getWifiDBReceiverIface();
}

static void WiFiDBReceiver_init(JNIEnv *env, jobject obj){

    if (sCallbacksObj == NULL) {
        sCallbacksObj = env->NewGlobalRef(obj);
    }
    if (class_APInfo == NULL) {
        jclass class_APInfo_temp = env->FindClass("com/qti/wifidbreceiver/APInfo");
        if (!class_APInfo_temp) {
            ALOGE("Class not found : com/qti/wifidbreceiver/APInfo");
        } else {
            class_APInfo = (jclass)env->NewGlobalRef(class_APInfo_temp);
        }
    }
    if (class_APLocationData == NULL) {
        jclass class_APLocationData_temp = env->FindClass("com/qti/wifidbreceiver/APLocationData");
        if (!class_APLocationData_temp) {
            ALOGE("Class not found : com/qti/wifidbreceiver/APLocationData");
        } else {
            class_APLocationData = (jclass)env->NewGlobalRef(class_APLocationData_temp);
        }
    }
    if (class_APSpecialInfo == NULL) {
        jclass class_APSpecialInfo_temp = env->FindClass("com/qti/wifidbreceiver/APSpecialInfo");
        if (!class_APSpecialInfo_temp) {
            ALOGE("Class not found : com/qti/wifidbreceiver/APSpecialInfo");
        } else {
            class_APSpecialInfo = (jclass)env->NewGlobalRef(class_APSpecialInfo_temp);
        }
    }

    auto iface = getWifiDBReceiverIface();
    if (iface == nullptr) {
      ALOGE("WiFiDBReceiver Iface NULL !");
      return;
    }

    if (wifiDBReceiverCallback == nullptr) {
        wifiDBReceiverCallback = new LocHidlWiFiDBReceiverCallback();
    }

    TO_HIDL_SERVICE();
    auto r = iface->init(wifiDBReceiverCallback);
    if (!r.isOk() || r == false) {
      ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
      return;
    }
    auto rt = iface->registerWiFiDBUpdater(wifiDBReceiverCallback);
    if (!rt.isOk()) {
      ALOGE("Error invoking HIDL API [%s]", rt.description().c_str());
      return;
    }
}

static void WiFiDBReceiver_deinit(JNIEnv *env, jobject /*obj*/)
{
    // clear Global References if any
    if (sCallbacksObj) env->DeleteGlobalRef(sCallbacksObj);
    sCallbacksObj = NULL;

    if (class_APInfo) env->DeleteGlobalRef(class_APInfo);
    class_APInfo = NULL;

    if (class_APLocationData) env->DeleteGlobalRef(class_APLocationData);
    class_APLocationData = NULL;

    if (class_APSpecialInfo) env->DeleteGlobalRef(class_APSpecialInfo);
    class_APSpecialInfo = NULL;

    auto iface = getWifiDBReceiverIface();
    if (iface == nullptr) {
      ALOGE("WiFiDBReceiver Iface NULL !");
      return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->unregisterWiFiDBUpdater();
    if (!r.isOk()) {
      ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
      return;
    }
}


static jint WiFiDBReceiver_requestAPList(JNIEnv* /*env*/, jobject /*obj*/,
                                         int expire_in_days) {
    jint result = 0;

    auto iface = getWifiDBReceiverIface();
    if (iface == nullptr) {
      ALOGE("WiFiDBReceiver Iface NULL !");
      return result;
    }

    TO_HIDL_SERVICE();
    auto r = iface->sendAPListRequest((int32_t)expire_in_days);
    if (!r.isOk()) {
      ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
    return result;
}

static jint WiFiDBReceiver_pushWiFiDB(JNIEnv* env, jobject /*obj*/,
                                      jobjectArray j_loc_list,
                                      jobjectArray j_spl_list,
                                      int days_valid) {
    int result = 0;

    std::vector<ILocHidlWiFiDBReceiver::LocHidlApLocationData> apLocList;
    jsize j_loc_list_len = 0;
    std::vector<ILocHidlWiFiDBReceiver::LocHidlApSpecialInfo> splList;
    jsize j_spl_list_len = 0;

    auto iface = getWifiDBReceiverIface();
    if (iface == nullptr) {
        result = 1;
    } else {
        ALOGD("DBRecJNI: pushDB\n");

        if (j_loc_list) {
            j_loc_list_len = env->GetArrayLength(j_loc_list);
            if ((int)j_loc_list_len > MAX_NUMBER_APS)
            {
              ALOGD("JNI hidl pushWiFiDB: Max location Data size exceeded %d", (int)j_loc_list_len);
              result = 4;
            }
            else
            {
              ALOGD("JNI hidl pushWiFiDB: Location Data size %d", (int)j_loc_list_len);
              for (size_t ii = 0; ii <(size_t)j_loc_list_len; ii++) {
                ILocHidlWiFiDBReceiver::LocHidlApLocationData locData;
                jobject objAPLocationData = env->GetObjectArrayElement(j_loc_list, ii);
                if (NULL == objAPLocationData) {
                    result = 3;
                    break;
                }

                result = TranslateFromObjectToHidl(objAPLocationData, &locData);
                apLocList.push_back(locData);
                env->DeleteLocalRef(objAPLocationData);
                if (0 != result) {
                    break;
                }
              }
            }
        } else {
            ALOGD("hidl pushWiFiDB: NO Location Data");
        }

        if (0 == result) {

            if (j_spl_list) {
                j_spl_list_len = env->GetArrayLength(j_spl_list);
                for (size_t ii = 0; ii < (size_t)j_spl_list_len; ii++) {
                  ILocHidlWiFiDBReceiver::LocHidlApSpecialInfo splInfo;
                  jobject objAPSpecialInfo = env->GetObjectArrayElement(j_spl_list, ii);
                  if (NULL == objAPSpecialInfo) {
                      result = 6;
                      break;
                  }

                  result = TranslateFromObjectToHidl(objAPSpecialInfo, &splInfo);
                  splList.push_back(splInfo);
                  env->DeleteLocalRef(objAPSpecialInfo);
                  if (0 != result) {
                      break;
                  }
              }
          } else {
              ALOGD("pushWiFiDB: Fail to push location Data");
          }

          if (0 == result) {
              TO_HIDL_SERVICE();
              auto r = iface->pushAPWiFiDB(apLocList, j_loc_list_len, splList,
                      j_spl_list_len, days_valid);
              if (!r.isOk()) {
                  ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
              }
          }
      }
    }
    return result;
}

static JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    { "native_wifi_db_receiver_class_init",
     "()V",
     reinterpret_cast<void*>(WiFiDBReceiver_class_init)},
    {"native_wifi_db_receiver_init",
     "()V",
     reinterpret_cast<void*>(WiFiDBReceiver_init)},
    {"native_wifi_db_receiver_deinit",
     "()V",
     reinterpret_cast<void*>(WiFiDBReceiver_deinit)},
    {"native_wifi_db_receiver_request_ap_list",
     "(I)I",
     reinterpret_cast<void*>(WiFiDBReceiver_requestAPList)},
    {"native_wifi_db_receiver_push_ad",
     "([Lcom/qti/wifidbreceiver/APLocationData;[Lcom/qti/wifidbreceiver/APSpecialInfo;I)I",
     reinterpret_cast<void*>(WiFiDBReceiver_pushWiFiDB)}
};

int register_WiFiDBReceiver(JNIEnv* env)
{
    return jniRegisterNativeMethods(env,
                                    "com/qualcomm/location/izat/wifidbreceiver/WiFiDBReceiver",
                                    sMethods,
                                    NELEM(sMethods));
}
