/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "LocSvc_HIDL_WWANDBReceiver_jni"
#define LOG_NDEBUG 0
#define LOCATION_CLASS_NAME "android/location/Location"

#define MAX_NUMBER_BSS 100

#include "JNIHelp.h"
#include "jni.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"
#include "utils_jni.h"

#include <vendor/qti/gnss/1.2/ILocHidlGnss.h>


using ::vendor::qti::gnss::V1_2::ILocHidlGnss;
using ::vendor::qti::gnss::V1_2::ILocHidlWWANDBReceiver;
using ::vendor::qti::gnss::V1_2::ILocHidlWWANDBReceiverCallback;

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::hidl_string;

using namespace android;

static void* sWWANDBReceiverHandle = NULL;
static JavaVM* sWWANDBRecCbJavaVm = NULL;
static jobject sCallbacksObj = NULL;

static jmethodID method_onBSListAvailable = NULL;
static jmethodID method_onStatusUpdate = NULL;
static jmethodID method_onServiceRequest = NULL;

static jclass class_BSInfo = NULL;
static jclass class_BSLocationData = NULL;
static jclass class_BSSpecialInfo = NULL;

#define NULL_ENV_POINTER       1

static int TranslateFromObjectToHidl(const jobject locationObject,
                                     ILocHidlWWANDBReceiver::LocHidlBsLocationData *location_data) {
    int result = 0;
    JNIEnv* env = NULL;
    if (nullptr != sWWANDBRecCbJavaVm) {
        if (sWWANDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
            return result;
        }
    }

    if (NULL == env) {
        result = NULL_ENV_POINTER;
    } else {

        static jfieldID jFieldBSLocationData_cellType =
                env->GetFieldID(class_BSLocationData, "mCellType","I");

        static jfieldID jFieldBSLocationData_cellRegionID1 =
                env->GetFieldID(class_BSLocationData, "mCellRegionID1","I");
        static jfieldID jFieldBSLocationData_cellRegionID2 =
                env->GetFieldID(class_BSLocationData, "mCellRegionID2","I");
        static jfieldID jFieldBSLocationData_cellRegionID3 =
                env->GetFieldID(class_BSLocationData, "mCellRegionID3","I");
        static jfieldID jFieldBSLocationData_cellRegionID4 =
                env->GetFieldID(class_BSLocationData, "mCellRegionID4","I");

        static jfieldID jFieldBSLocationData_lat =
                env->GetFieldID(class_BSLocationData, "mLatitude","F");
        static jfieldID jFieldBSLocationData_lon =
                env->GetFieldID(class_BSLocationData, "mLongitude","F");
        static jfieldID jFieldBSLocationData_valid =
                env->GetFieldID(class_BSLocationData, "mValidBits","I");

        static jfieldID jFieldBSLocationData_hcr =
                env->GetFieldID(class_BSLocationData, "mHorizontalCoverageRadius","F");
        static jfieldID jFieldBSLocationData_hc =
                env->GetFieldID(class_BSLocationData, "mHorizontalConfidence","I");
        static jfieldID jFieldBSLocationData_hr =
                env->GetFieldID(class_BSLocationData, "mHorizontalReliability","I");
        static jfieldID jFieldBSLocationData_alt =
                env->GetFieldID(class_BSLocationData, "mAltitude","F");
        static jfieldID jFieldBSLocationData_alu =
                env->GetFieldID(class_BSLocationData, "mAltitudeUncertainty","F");
        static jfieldID jFieldBSLocationData_alc =
                env->GetFieldID(class_BSLocationData, "mAltitudeConfidence","F");
        static jfieldID jFieldBSLocationData_alr =
                env->GetFieldID(class_BSLocationData, "mAltitudeReliability","I");

        jint cellType = env->GetIntField(locationObject,
                                         jFieldBSLocationData_cellType);

        jint cellRegionID1 = env->GetIntField(locationObject,
                                              jFieldBSLocationData_cellRegionID1);
        jint cellRegionID2 = env->GetIntField(locationObject,
                                              jFieldBSLocationData_cellRegionID2);
        jint cellRegionID3 = env->GetIntField(locationObject,
                                              jFieldBSLocationData_cellRegionID3);
        jint cellRegionID4 = env->GetIntField(locationObject,
                                              jFieldBSLocationData_cellRegionID4);

        jfloat lat = env->GetFloatField(locationObject,
                                        jFieldBSLocationData_lat);
        jfloat lon = env->GetFloatField(locationObject,
                                        jFieldBSLocationData_lon);
        jint valid = env->GetIntField(locationObject,
                                      jFieldBSLocationData_valid);

        jfloat hcr = env->GetFloatField(locationObject,
                                        jFieldBSLocationData_hcr);
        jint hc = env->GetIntField(locationObject,
                                   jFieldBSLocationData_hc);
        jint hr = env->GetIntField(locationObject,
                                   jFieldBSLocationData_hr);
        jfloat alt = env->GetFloatField(locationObject,
                                        jFieldBSLocationData_alt);
        jfloat alu = env->GetFloatField(locationObject,
                                        jFieldBSLocationData_alu);
        jfloat alc = env->GetFloatField(locationObject,
                                        jFieldBSLocationData_alc);
        jint alr = env->GetIntField(locationObject,
                                    jFieldBSLocationData_alr);

        location_data->cellType = (uint8_t)cellType;
        location_data->cellRegionID1 = (uint32_t)cellRegionID1;
        location_data->cellRegionID2 = (uint32_t)cellRegionID2;
        location_data->cellRegionID3 = (uint32_t)cellRegionID3;
        location_data->cellRegionID4 = (uint32_t)cellRegionID4;

        location_data->latitude = (float)lat;
        location_data->longitude = (float)lon;
        location_data->valid_bits = (int8_t)valid;
        location_data->horizontal_coverage_radius = (float)hcr;
        location_data->horizontal_confidence = (uint8_t)hc;
        location_data->horizontal_reliability = (uint8_t)hr;
        location_data->altitude = (float)alt;
        location_data->altitude_uncertainty = (float)alu;
        location_data->altitude_confidence = (uint8_t)alc;
        location_data->altitude_reliability = (uint8_t)alr;
    }

    return result;
}

static int TranslateFromObjectToHidl(const jobject specialInfoObject,
                                     ILocHidlWWANDBReceiver::LocHidlBsSpecialInfo *special_info) {
    int result = 0;
    JNIEnv* env = NULL;
    if (nullptr != sWWANDBRecCbJavaVm) {
        if (sWWANDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
            return result;
        }
    }

    if (NULL == env) {
        result = NULL_ENV_POINTER;
    } else {

        static jfieldID jFieldBSSpecialInfo_cellType =
                env->GetFieldID(class_BSSpecialInfo, "mCellType","I");

        static jfieldID jFieldBSSpecialInfo_cellRegionID1 =
                env->GetFieldID(class_BSSpecialInfo, "mCellRegionID1","I");
        static jfieldID jFieldBSSpecialInfo_cellRegionID2 =
                env->GetFieldID(class_BSSpecialInfo, "mCellRegionID2","I");
        static jfieldID jFieldBSSpecialInfo_cellRegionID3 =
                env->GetFieldID(class_BSSpecialInfo, "mCellRegionID3","I");
        static jfieldID jFieldBSSpecialInfo_cellRegionID4 =
                env->GetFieldID(class_BSSpecialInfo, "mCellRegionID4","I");
        static jfieldID jFieldBSSpecialInfo_info =
                env->GetFieldID(class_BSSpecialInfo, "mInfo", "I");

        jint cellType = env->GetIntField(specialInfoObject,
                                         jFieldBSSpecialInfo_cellType);

        jint cellRegionID1 = env->GetIntField(specialInfoObject,
                                              jFieldBSSpecialInfo_cellRegionID1);
        jint cellRegionID2 = env->GetIntField(specialInfoObject,
                                              jFieldBSSpecialInfo_cellRegionID2);
        jint cellRegionID3 = env->GetIntField(specialInfoObject,
                                              jFieldBSSpecialInfo_cellRegionID3);
        jint cellRegionID4 = env->GetIntField(specialInfoObject,
                                              jFieldBSSpecialInfo_cellRegionID4);

        jint info  = env->GetIntField(specialInfoObject,
                                      jFieldBSSpecialInfo_info);

        special_info->cellType = (uint8_t)cellType;
        special_info->cellRegionID1 = (uint32_t)cellRegionID1;
        special_info->cellRegionID2 = (uint32_t)cellRegionID2;
        special_info->cellRegionID3 = (uint32_t)cellRegionID3;
        special_info->cellRegionID4 = (uint32_t)cellRegionID4;

        special_info->info = (uint8_t)info;
    }
    return result;
}

static void TranslateHidlToObject(const ILocHidlWWANDBReceiverCallback::LocHidlBsInfo& bs, jobject& bsInfoObject) {
    JNIEnv* env = NULL;
    if (nullptr != sWWANDBRecCbJavaVm) {
        if (sWWANDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
            return;
        }
    }

    if (env) {

        if (class_BSInfo) {
            jmethodID jConstructorBSInfo = env->GetMethodID(class_BSInfo,"<init>","()V");
            static jfieldID jFieldCellType = env->GetFieldID(class_BSInfo,
                                                             "mCellType", "I");
            static jfieldID jFieldCell1 = env->GetFieldID(class_BSInfo,
                                                          "mCellRegionID1", "I");
            static jfieldID jFieldCell2 = env->GetFieldID(class_BSInfo,
                                                          "mCellRegionID2", "I");
            static jfieldID jFieldCell3 = env->GetFieldID(class_BSInfo,
                                                          "mCellRegionID3", "I");
            static jfieldID jFieldCell4 = env->GetFieldID(class_BSInfo,
                                                          "mCellRegionID4", "I");
            bsInfoObject = env->NewObject(class_BSInfo, jConstructorBSInfo, NULL);
            if (bsInfoObject) {
                env->SetIntField(bsInfoObject, jFieldCellType, bs.cell_type);
                env->SetIntField(bsInfoObject, jFieldCell1, bs.cell_id1);
                env->SetIntField(bsInfoObject, jFieldCell2, bs.cell_id2);
                env->SetIntField(bsInfoObject, jFieldCell3, bs.cell_id3);
                env->SetIntField(bsInfoObject, jFieldCell4, bs.cell_id4);
            }
        }
    }
}

/* ===========================================================
 *   HIDL Callbacks : ILocHidlWWANDBReceiverCallback.hal
 * ===========================================================*/
struct LocHidlWWANDBReceiverCallback :
  public ILocHidlWWANDBReceiverCallback {

  Return<void> bsListUpdateCallback(
          const hidl_vec<ILocHidlWWANDBReceiverCallback::LocHidlBsInfo>& bsInfoList,
          int32_t bsListSize) override;
  Return<void> statusUpdateCallback(
          bool status, const hidl_string& reason) override;
  Return<void> serviceRequestCallback() override;
  Return<void> attachVmOnCallback() override;
};

Return<void> LocHidlWWANDBReceiverCallback::bsListUpdateCallback(
         const hidl_vec<LocHidlBsInfo>& bsInfoList, int32_t bsListSize) {

   FROM_HIDL_SERVICE();

   JNIEnv* env = NULL;
   if (nullptr != sWWANDBRecCbJavaVm) {
       if (sWWANDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
           return Void();
       }
   }

   if (env && sCallbacksObj && method_onBSListAvailable) {
       if (class_BSInfo) {
           jobjectArray jBSList = (jobjectArray)env->NewObjectArray(bsListSize,
                                                                    class_BSInfo,
                                                                    NULL);
           if (jBSList) {
               for (int32_t i = 0; i < bsListSize; i++) {
                   jobject jObjBSInfo = NULL;
                   auto& info = bsInfoList[i];
                   TranslateHidlToObject(info, jObjBSInfo);
                   if (jObjBSInfo) {
                       env->SetObjectArrayElement(jBSList, i, jObjBSInfo);
                   }
                   // Managing overwhelming number of requests
                   env->DeleteLocalRef(jObjBSInfo);
               }
               env->CallVoidMethod(sCallbacksObj,
                                   method_onBSListAvailable,
                                   jBSList);
               // Managing overwhelming number of requests
               env->DeleteLocalRef(jBSList);
               checkAndClearExceptionsFromCallback(env, __FUNCTION__);
           }
       }
   }
   return Void();
}

Return<void> LocHidlWWANDBReceiverCallback::statusUpdateCallback(
         bool status, const hidl_string& reason) {

    FROM_HIDL_SERVICE();

    JNIEnv* env = NULL;
    if (nullptr != sWWANDBRecCbJavaVm) {
        if (sWWANDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
            return Void();
        }
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

Return<void> LocHidlWWANDBReceiverCallback::serviceRequestCallback() {

    FROM_HIDL_SERVICE();

    JNIEnv* env = NULL;
    if (nullptr != sWWANDBRecCbJavaVm) {
        if (sWWANDBRecCbJavaVm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
            return Void();
        }
    }

    if (env && sCallbacksObj && method_onServiceRequest) {
        env->CallVoidMethod(sCallbacksObj,
                            method_onServiceRequest);
        checkAndClearExceptionsFromCallback(env, __FUNCTION__);
    }
    return Void();
}

Return<void> LocHidlWWANDBReceiverCallback::attachVmOnCallback() {

    FROM_HIDL_SERVICE();

    void* clientData = NULL;

    sWWANDBRecCbJavaVm = AndroidRuntime::getJavaVM();
    JNIEnv *callbackEnv = NULL;
    JavaVMAttachArgs args = {
        JNI_VERSION_1_6,
        "WWD Service Callback Thread",
        NULL
    };
    jint attachResult = sWWANDBRecCbJavaVm->AttachCurrentThread(&callbackEnv, &args);
    if (0 != attachResult) {
        ALOGE("Callback thread attachment error: %d", attachResult);
        callbackEnv = NULL;
    }

    return Void();
}


/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlGnss> gnssVendorHal = nullptr;
static sp<ILocHidlWWANDBReceiver> wwanDBReceiverIface = nullptr;
static sp<ILocHidlWWANDBReceiverCallback> wwanDBReceiverCallback = nullptr;

static void WWANDBReceiver_class_init(JNIEnv* env, jclass clazz)
{
    // get references to the Java methods
    method_onBSListAvailable = env->GetMethodID(clazz,
                                                "onBSListAvailable",
                                                "([Lcom/qti/wwandbreceiver/BSInfo;)V");
    method_onStatusUpdate = env->GetMethodID(clazz, "onStatusUpdate", "(ZLjava/lang/String;)V");
    method_onServiceRequest = env->GetMethodID(clazz, "onServiceRequest", "()V");

    /* Now fetch HIDL interface */

    gnssVendorHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
    if (gnssVendorHal == nullptr) {
        ALOGE("Unable to get GnssVendor service\n");
        return;
    }

    auto wdr = gnssVendorHal->getExtensionLocHidlWWANDBReceiver();
    if (!wdr.isOk()) {
        ALOGE("Unable to get a handle to WWANDBReceiver extension !");
    } else {
        wwanDBReceiverIface = wdr;
    }
}

static void WWANDBReceiver_init(JNIEnv *env, jobject obj){

    if (nullptr == sCallbacksObj) {
        sCallbacksObj = env->NewGlobalRef(obj);
    }
    if (nullptr == class_BSInfo) {
        jclass class_BSInfo_temp = env->FindClass("com/qti/wwandbreceiver/BSInfo");
        if (!class_BSInfo_temp) {
            ALOGE("Class not found : com/qti/wwandbreceiver/BSInfo");
        } else {
            class_BSInfo = (jclass)env->NewGlobalRef(class_BSInfo_temp);
        }
    }
    if (nullptr == class_BSLocationData) {
        jclass class_BSLocationData_temp = env->FindClass("com/qti/wwandbreceiver/BSLocationData");
        if (!class_BSLocationData_temp) {
            ALOGE("Class not found : com/qti/wwandbreceiver/BSLocationData");
        } else {
            class_BSLocationData = (jclass)env->NewGlobalRef(class_BSLocationData_temp);
        }
    }
    if (nullptr == class_BSSpecialInfo) {
        jclass class_BSSpecialInfo_temp = env->FindClass("com/qti/wwandbreceiver/BSSpecialInfo");
        if (!class_BSSpecialInfo_temp) {
            ALOGE("Class not found : com/qti/wwandbreceiver/BSSpecialInfo");
        } else {
            class_BSSpecialInfo = (jclass)env->NewGlobalRef(class_BSSpecialInfo_temp);
        }
    }

    if (wwanDBReceiverIface == nullptr) {
        ALOGE("WWANDBReceiver Iface NULL !");
        return;
    }

    if (wwanDBReceiverCallback == nullptr) {
        wwanDBReceiverCallback = new LocHidlWWANDBReceiverCallback();
    }

    TO_HIDL_SERVICE();
    auto r = wwanDBReceiverIface->init(wwanDBReceiverCallback);
    if (!r.isOk() || r == false) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        return;
    }
    wwanDBReceiverIface->registerWWANDBUpdater(wwanDBReceiverCallback);
}

static void WWANDBReceiver_deinit(JNIEnv *env, jobject /*obj*/)
{
    // clear Global References if any
    if (sCallbacksObj) {
        env->DeleteGlobalRef(sCallbacksObj);
    }
    sCallbacksObj = NULL;

    if (class_BSInfo) {
        env->DeleteGlobalRef(class_BSInfo);
    }
    class_BSInfo = NULL;

    if (class_BSLocationData) {
        env->DeleteGlobalRef(class_BSLocationData);
    }
    class_BSLocationData = NULL;

    if (class_BSSpecialInfo) {
        env->DeleteGlobalRef(class_BSSpecialInfo);
    }
    class_BSSpecialInfo = NULL;

    if (wwanDBReceiverIface == nullptr) {
        ALOGE("WWANDBReceiver Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    wwanDBReceiverIface->unregisterWWANDBUpdater();
}


static jint WWANDBReceiver_requestBSList(JNIEnv* /*env*/, jobject /*obj*/,
                                         int expire_in_days) {
    jint result = 0;

    if (wwanDBReceiverIface == nullptr) {
        ALOGE("WWANDBReceiver Iface NULL !");
        return result;
    }

    TO_HIDL_SERVICE();
    wwanDBReceiverIface->sendBSListRequest((int32_t)expire_in_days);
    return result;
}

static jint WWANDBReceiver_pushWWANDB(JNIEnv* env, jobject /*obj*/,
                                      jobjectArray j_loc_list,
                                      jobjectArray j_spl_list,
                                      int days_valid) {
    int result = 0;

    std::vector<ILocHidlWWANDBReceiver::LocHidlBsLocationData> bsLocList;
    jsize j_loc_list_len = 0;
    std::vector<ILocHidlWWANDBReceiver::LocHidlBsSpecialInfo> splList;
    jsize j_spl_list_len = 0;

    if (wwanDBReceiverIface == nullptr) {
        result = 1;
    } else {
        ALOGD("DBRecJNI: pushWWANDB\n");

        if (j_loc_list) {
            j_loc_list_len = env->GetArrayLength(j_loc_list);
            if ((int)j_loc_list_len > MAX_NUMBER_BSS)
            {
                ALOGD("JNI hidl pushWWANDB: Max location Data size exceeded %d",
                      (int)j_loc_list_len);
                result = 4;
            }else{
                ALOGD("JNI hidl pushWWANDB: Location Data size %d", (int)j_loc_list_len);
                for (size_t ii = 0; ii <(size_t)j_loc_list_len; ii++) {
                    ILocHidlWWANDBReceiver::LocHidlBsLocationData locData;
                    jobject objBSLocationData = env->GetObjectArrayElement(j_loc_list, ii);
                    if (NULL == objBSLocationData) {
                        result = 3;
                        break;
                    }

                    result = TranslateFromObjectToHidl(objBSLocationData, &locData);
                    bsLocList.push_back(locData);
                    env->DeleteLocalRef(objBSLocationData);
                    if (0 != result) {
                        break;
                    }
                }
            }
        } else {
            ALOGD("hidl pushWWANDB: NO Location Data");
        }

        if (0 == result) {

            if (j_spl_list) {
                j_spl_list_len = env->GetArrayLength(j_spl_list);
                for (size_t ii = 0; ii < (size_t)j_spl_list_len; ii++) {
                    ILocHidlWWANDBReceiver::LocHidlBsSpecialInfo splInfo;
                    jobject objBSSpecialInfo = env->GetObjectArrayElement(j_spl_list, ii);
                    if (NULL == objBSSpecialInfo) {
                        result = 6;
                        break;
                    }

                    result = TranslateFromObjectToHidl(objBSSpecialInfo, &splInfo);
                    splList.push_back(splInfo);
                    env->DeleteLocalRef(objBSSpecialInfo);
                    if (0 != result) {
                        break;
                    }
                }
            } else {
                ALOGD("pushWWANDB: Fail to push location Data");
            }

            if (0 == result) {
                TO_HIDL_SERVICE();
                wwanDBReceiverIface->pushBSWWANDB(bsLocList, j_loc_list_len, splList,
                                                  j_spl_list_len, days_valid);
            }
        }
    }
    return result;
}

static JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    {"native_wwan_db_receiver_class_init",
     "()V",
     reinterpret_cast<void*>(WWANDBReceiver_class_init)},
    {"native_wwan_db_receiver_init",
     "()V",
     reinterpret_cast<void*>(WWANDBReceiver_init)},
    {"native_wwan_db_receiver_deinit",
     "()V",
     reinterpret_cast<void*>(WWANDBReceiver_deinit)},
    {"native_wwan_db_receiver_request_bs_list",
     "(I)I",
     reinterpret_cast<void*>(WWANDBReceiver_requestBSList)},
    {"native_wwan_db_receiver_push_ad",
     "([Lcom/qti/wwandbreceiver/BSLocationData;[Lcom/qti/wwandbreceiver/BSSpecialInfo;I)I",
     reinterpret_cast<void*>(WWANDBReceiver_pushWWANDB)}
};

int register_WWANDBReceiver(JNIEnv* env)
{
    return jniRegisterNativeMethods(env,
                                    "com/qualcomm/location/izat/wwandbreceiver/WWANDBReceiver",
                                    sMethods,
                                    NELEM(sMethods));
}
