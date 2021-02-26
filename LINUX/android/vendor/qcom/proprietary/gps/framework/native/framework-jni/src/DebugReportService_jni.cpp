/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "LocSvc_HIDL_DebugReportService_jni"
#define LOG_NDEBUG 0
#define LOCATION_CLASS_NAME "android/location/Location"

#include <inttypes.h>
#include "JNIHelp.h"
#include "jni.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"
#include "utils_jni.h"

#if defined LOC_HIDL_VERSION_1_0
#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
#else
#include <vendor/qti/gnss/1.1/ILocHidlGnss.h>
#endif

#define HAS_HORIZONTAL_COMPONENT 1
#define HAS_VERTICAL_COMPONENT 2
#define HAS_SOURCE 4

using namespace std;
using namespace android;

#if defined LOC_HIDL_VERSION_1_0
using ::vendor::qti::gnss::V1_0::ILocHidlGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlDebugReportService;
#else
using ::vendor::qti::gnss::V1_1::ILocHidlGnss;
using ::vendor::qti::gnss::V1_1::ILocHidlDebugReportService;
#endif

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::sp;

static jclass arrayListClass;
static jclass IzatEphmerisDebugReportClass;
static jclass IzatFixStatusDebugReportClass;
static jclass IzatLocationReportClass;
static jclass IzatGpsTimeReportClass;
static jclass IzatXoStateReportClass;
static jclass IzatRfStateReportClass;
static jclass IzatUtcSpecClass;
static jclass IzatPDRReportClass;
static jclass IzatSVHealthReportClass;
static jclass IzatXTRAReportClass;
static jclass IzatErrorRecoveryClass;

static jmethodID method_addToList;
static jmethodID method_ephmerisDebugReportCtor;
static jmethodID method_fixStatusDebugReportCtor;
static jmethodID method_locationReportCtor;
static jmethodID method_gpsTimeReportCtor;
static jmethodID method_xoStateReportCtor;
static jmethodID method_rfStateReportCtor;
static jmethodID method_utcSpecCtor;
static jmethodID method_pdrReportCtor;
static jmethodID method_svhealthReportCtor;
static jmethodID method_xtraReportCtor;
static jmethodID method_errorRecoveryCtor;

/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlDebugReportService> debugReportServiceIface = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        debugReportServiceIface = nullptr;
        ALOGE("%s] ILocHidlDebugReportService died with cookie: %" PRIu64, __FILE__, cookie);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

static sp<ILocHidlDebugReportService>& getDebugReportIface() {
    static uint64_t deathCount = 0;
    if (debugReportServiceIface == nullptr) {
        // HIDL Service
        ALOGD("Get GnssVendor Service.");
        auto gnssVendorHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (gnssVendorHal == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
            ALOGD("Got GnssVendor Service.");

            ALOGD("Get DebugReportService Extension");
#if defined LOC_HIDL_VERSION_1_0
            auto drs = gnssVendorHal->getExtensionLocHidlDebugReportService();
#else
            auto drs = gnssVendorHal->getExtensionLocHidlDebugReportService_1_1();
#endif
            if (!drs.isOk()) {
                ALOGE("Unable to get a handle to DebugReportService extension !");
            } else {
                ALOGD("Got DebugReportService Extension.");
                if (locHidlDeathRecipient == nullptr) {
                    locHidlDeathRecipient = new LocHidlDeathRecipient();
                }
                debugReportServiceIface = drs;
                auto r = debugReportServiceIface->linkToDeath(locHidlDeathRecipient, deathCount++);
                if (!r.isOk() || r == false) {
                    ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                    debugReportServiceIface = nullptr;
                }
            }
        }
    }

    return debugReportServiceIface;
}

static void DebugReportService_class_init(JNIEnv* env, jclass /*clazz*/)
{
    ALOGD("DebugReportService_class_init");

    // get references to the Java method
    jclass arrayListClassLocal;
    jclass IzatEphmerisClassLocal;
    jclass IzatFixStatusClassLocal;
    jclass IzatLocationReportClassLocal;
    jclass IzatGpsTimeReportClassLocal;
    jclass IzatXoStateReportClassLocal;
    jclass IzatRfStateReportClassLocal;
    jclass IzatUtcSpecClassLocal;
    jclass IzatPDRReportClassLocal;
    jclass IzatSVHealthReportClassLocal;
    jclass IzatXTRAReportClassLocal;
    jclass IzatErrorRecoveryClassLocal;

    arrayListClassLocal = env->FindClass("java/util/ArrayList");
    method_addToList  = env->GetMethodID(arrayListClassLocal, "add", "(Ljava/lang/Object;)Z");
    arrayListClass = (jclass) env->NewGlobalRef(arrayListClassLocal);
    env->DeleteLocalRef(arrayListClassLocal);

    IzatEphmerisClassLocal = env->FindClass("com/qti/debugreport/IZatEphmerisDebugReport");
    if (IzatEphmerisClassLocal == nullptr) {
        ALOGE("Cannot find IzatEphmerisDebugReportClass class");
    } else {
        IzatEphmerisDebugReportClass = (jclass) env->NewGlobalRef(IzatEphmerisClassLocal);
        method_ephmerisDebugReportCtor = env->GetMethodID(IzatEphmerisDebugReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;IIJJB)V");
        if (method_ephmerisDebugReportCtor == 0) {
            ALOGE("Failed to get constructor method for IzatEphmerisDebugReportClass");
        }
        env->DeleteLocalRef(IzatEphmerisClassLocal);
    }

    IzatFixStatusClassLocal = env->FindClass("com/qti/debugreport/IZatFixStatusDebugReport");
    if (IzatFixStatusClassLocal == nullptr) {
        ALOGE("Cannot find IZatFixStatusDebugReport class");
    } else {
        IzatFixStatusDebugReportClass = (jclass)  env->NewGlobalRef(IzatFixStatusClassLocal);
        method_fixStatusDebugReportCtor = env->GetMethodID(IzatFixStatusDebugReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;IJ)V");
        if (method_fixStatusDebugReportCtor == 0) {
            ALOGE("Failed to get constructor method for IZatFixStatusDebugReport");
        }
        env->DeleteLocalRef(IzatFixStatusClassLocal);
    }

    IzatLocationReportClassLocal = env->FindClass("com/qti/debugreport/IZatLocationReport");
    if (IzatLocationReportClassLocal == nullptr) {
        ALOGE("Cannot find IZatLocationReport class");
    } else {
        IzatLocationReportClass = (jclass) env->NewGlobalRef(IzatLocationReportClassLocal);
        method_locationReportCtor = env->GetMethodID(IzatLocationReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;IDDFDFI)V");
        if (method_locationReportCtor == 0) {
            ALOGE("Failed to get constructor method for IZatLocationReport");
        }
        env->DeleteLocalRef(IzatLocationReportClassLocal);
    }

    IzatGpsTimeReportClassLocal = env->FindClass("com/qti/debugreport/IZatGpsTimeDebugReport");
    if (IzatGpsTimeReportClassLocal == nullptr) {
        ALOGE("Cannot find IZatGpsTimeDebugReport class");
    } else {
        IzatGpsTimeReportClass = (jclass) env->NewGlobalRef(IzatGpsTimeReportClassLocal);
        method_gpsTimeReportCtor = env->GetMethodID(IzatGpsTimeReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;IJZIIII)V");
        if (method_gpsTimeReportCtor == 0) {
            ALOGE("Failed to get constructor method for IZatDebugReport");
        }
        env->DeleteLocalRef(IzatGpsTimeReportClassLocal);
    }

    IzatXoStateReportClassLocal = env->FindClass("com/qti/debugreport/IZatXoStateDebugReport");
    if (IzatXoStateReportClassLocal == nullptr) {
        ALOGE("Cannot find IZatXoStateDebugReport class");
    } else {
        IzatXoStateReportClass = (jclass) env->NewGlobalRef(IzatXoStateReportClassLocal);
        method_xoStateReportCtor = env->GetMethodID(IzatXoStateReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;I)V");
        if (method_xoStateReportCtor == 0) {
            ALOGE("Failed to get constructor method for IZatXoStateDebugReport");
        }
        env->DeleteLocalRef(IzatXoStateReportClassLocal);
    }

    IzatRfStateReportClassLocal = env->FindClass("com/qti/debugreport/IZatRfStateDebugReport");
    if (IzatRfStateReportClassLocal == nullptr) {
        ALOGE("Cannot find IZatRfStateDebugReport class");
    } else {
        IzatRfStateReportClass = (jclass) env->NewGlobalRef(IzatRfStateReportClassLocal);
        method_rfStateReportCtor = env->GetMethodID(IzatRfStateReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;IJJJJJJJJJJJJJJ)V");
        if (method_rfStateReportCtor == 0) {
            ALOGE("Failed to get constructor method for IZatRfStateDebugReport");
        }
        env->DeleteLocalRef(IzatRfStateReportClassLocal);
    }

    IzatUtcSpecClassLocal = env->FindClass("com/qti/debugreport/IZatUtcSpec");
    if (IzatUtcSpecClassLocal == nullptr) {
        ALOGE("Cannot find IZatUtcSpec class");
    } else {
        IzatUtcSpecClass = (jclass) env->NewGlobalRef(IzatUtcSpecClassLocal);
        method_utcSpecCtor = env->GetMethodID(IzatUtcSpecClass, "<init>", "(JJ)V");
        if (method_utcSpecCtor == 0) {
            ALOGE("Failed to get constructor method for IZatUtcSpec");
        }
        env->DeleteLocalRef(IzatUtcSpecClassLocal);
    }

    IzatPDRReportClassLocal = env->FindClass("com/qti/debugreport/IZatPDRDebugReport");
    if (IzatPDRReportClassLocal == nullptr) {
        ALOGE("Cannot find IZatPDRDebugReport class");
    } else {
        IzatPDRReportClass = (jclass) env->NewGlobalRef(IzatPDRReportClassLocal);
        method_pdrReportCtor = env->GetMethodID(IzatPDRReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;I)V");
        if (method_pdrReportCtor == 0) {
            ALOGE("Failed to get constructor method for IZatPDRDebugReport");
        }
        env->DeleteLocalRef(IzatPDRReportClassLocal);
    }

    IzatSVHealthReportClassLocal = env->FindClass("com/qti/debugreport/IZatSVHealthDebugReport");
    if (IzatSVHealthReportClassLocal == nullptr) {
        ALOGE("Cannot find IZatSVHealthDebugReport class");
    } else {
        IzatSVHealthReportClass = (jclass) env->NewGlobalRef(IzatSVHealthReportClassLocal);
        method_svhealthReportCtor = env->GetMethodID(IzatSVHealthReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;IIIIIIJJJJJJBBB)V");
        if (method_svhealthReportCtor == 0) {
            ALOGE("Failed to get constructor method for IZatSVHealthDebugReport");
        }
        env->DeleteLocalRef(IzatSVHealthReportClassLocal);
    }

    IzatXTRAReportClassLocal = env->FindClass("com/qti/debugreport/IZatXTRADebugReport");
    if (IzatXTRAReportClassLocal == nullptr) {
        ALOGE("Cannot find IZatXTRADebugReport class");
    } else {
        IzatXTRAReportClass = (jclass) env->NewGlobalRef(IzatXTRAReportClassLocal);
        method_xtraReportCtor = env->GetMethodID(IzatXTRAReportClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;BIIIIJIJIBI)V");
        if (method_xtraReportCtor == 0) {
            ALOGE("Failed to get constructor method for IZatXTRADebugReport");
        }
        env->DeleteLocalRef(IzatXTRAReportClassLocal);
    }

    IzatErrorRecoveryClassLocal = env->FindClass("com/qti/debugreport/IZatErrorRecoveryReport");
    if (IzatErrorRecoveryClassLocal == nullptr) {
        ALOGE("Cannot find IZatErrorRecoveryReport class");
    } else {
        IzatErrorRecoveryClass = (jclass) env->NewGlobalRef(IzatErrorRecoveryClassLocal);
        method_errorRecoveryCtor = env->GetMethodID(IzatErrorRecoveryClass, "<init>",
            "(Lcom/qti/debugreport/IZatUtcSpec;Lcom/qti/debugreport/IZatUtcSpec;)V");
        if (method_errorRecoveryCtor == 0) {
            ALOGE("Failed to get constructor method for IZatErrorRecoveryReport");
        }
        env->DeleteLocalRef(IzatErrorRecoveryClassLocal);
    }

    /* Now fetch HIDL interface */
    getDebugReportIface();
}

static void DebugReportService_init(JNIEnv* /*env*/, jobject /*obj*/)
{
    ALOGD("DebugReportService_init");

    auto iface = getDebugReportIface();
    if (iface == nullptr) {
        ALOGE("DebugReportService Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->init();
    if (!r.isOk() || r == false) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void DebugReportService_deinit(JNIEnv* env, jobject /*obj*/)
{
    ALOGD("DebugReportService_deinit");

    env->DeleteGlobalRef(arrayListClass);
    env->DeleteGlobalRef(IzatEphmerisDebugReportClass);
    env->DeleteGlobalRef(IzatFixStatusDebugReportClass);
    env->DeleteGlobalRef(IzatLocationReportClass);
    env->DeleteGlobalRef(IzatGpsTimeReportClass);
    env->DeleteGlobalRef(IzatXoStateReportClass);
    env->DeleteGlobalRef(IzatRfStateReportClass);
    env->DeleteGlobalRef(IzatUtcSpecClass);
    env->DeleteGlobalRef(IzatPDRReportClass);
    env->DeleteGlobalRef(IzatSVHealthReportClass);
    env->DeleteGlobalRef(IzatXTRAReportClass);

    auto iface = getDebugReportIface();
    if (iface == nullptr) {
        ALOGE("DebugReportService Iface NULL !");
        return;
    }

    TO_HIDL_SERVICE();
    auto r = iface->deinit();
    if (!r.isOk() || r == false) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

jobject createUtcTimeObject(JNIEnv* env, jlong utcSecondsPart, jlong utcNanoSecPart) {
    jobject utcObj = env->NewObject(IzatUtcSpecClass, method_utcSpecCtor,
        utcSecondsPart, utcNanoSecPart);
    if (utcObj == nullptr) {
        ALOGE("Failed to create IZatUtcSpec object");
    }

    return utcObj;
}

static void populateEphemerisStatusListObj(
        JNIEnv* env, jint maxReports, jobject& ephmerisStatusListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusEphemeris>& ephReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = ephReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusEphemeris& status =
                ephReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jint gpsEpheValidity = status.mGpsEpheValid;
        jint glonassEpheValidity = status.mGloEpheValid;
        jlong bdsEpheValidity = status.mBdsEpheValid;
        jlong galEpheValidity = status.mGalEpheValid;
        jbyte qzssEpheValidity = status.mQzssEpheValid;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject ephmerisReportObj = env->NewObject(IzatEphmerisDebugReportClass, method_ephmerisDebugReportCtor,
                                                   utcLastUpdatedObj, utcLastReportedObj, gpsEpheValidity, glonassEpheValidity,
                                                   bdsEpheValidity, galEpheValidity, qzssEpheValidity);
        if (ephmerisReportObj == nullptr) {
            ALOGE("Failed to create IzatEphmerisDebugReport object");
        } else {
            jboolean jb = env->CallBooleanMethod(ephmerisStatusListObj,
                                                 method_addToList,
                                                 ephmerisReportObj);
            if (jb  == 0) {
                ALOGE("Failed to add IzatEphmerisDebugReport object to list");
            }
        }
    }
}

static void populateFixStatusListObj(
        JNIEnv* env, jint maxReports, jobject& fixStatusListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusPositionFailure>&
        posFailureReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = posFailureReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusPositionFailure& status =
                posFailureReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

       jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
       jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

       jint fixInfoMask = status.mFixInfoMask;
       jlong hepeLimit = status.mHepeLimit;

       jobject utcLastUpdatedObj = createUtcTimeObject(env,
           utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

       jobject utcLastReportedObj = createUtcTimeObject(env,
           utcSecondsPartLastReported, utcNanoSecPartLastReported);

       jobject fixReportObj = env->NewObject(IzatFixStatusDebugReportClass,
                                             method_fixStatusDebugReportCtor,
                                             utcLastUpdatedObj, utcLastReportedObj,
                                             fixInfoMask, hepeLimit);
       if (fixReportObj == nullptr) {
           ALOGE("Failed to create IZatFixStatusDebugReport object");
       } else {
           jboolean jb = env->CallBooleanMethod(fixStatusListObj,
                                                method_addToList,
                                                fixReportObj);
           if (jb  == 0) {
               ALOGE("Failed to add IZatFixStatusDebugReport object");
           }
        }
    }
}

static void populateEpiReportListObj(
        JNIEnv* env, jint maxReports, jobject& epiReportListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusInjectedPosition>&
        injectPosReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = injectPosReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusInjectedPosition& status =
                injectPosReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        // EPI always has source field
        jint validityMask = status.mEpiValidity;
        validityMask |= HAS_SOURCE;

        jdouble lat = status.mEpiLat;
        jdouble lon = status.mEpiLon;
        jfloat horzAccuracy = status.mEpiHepe;
        jdouble alt= status.mEpiAlt;
        jfloat altUnc = status.mEpiAltUnc;
        jint source = status.mEpiSrc;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject epiReportObj = env->NewObject(IzatLocationReportClass, method_locationReportCtor,
                                              utcLastUpdatedObj, utcLastReportedObj, validityMask, lat,
                                              lon, horzAccuracy, alt, altUnc, source);
        if (epiReportObj == nullptr) {
            ALOGE("Failed to create IzatLocationReport object for EPI");
        } else {
            jboolean jb = env->CallBooleanMethod(epiReportListObj,
                                                 method_addToList, epiReportObj);
            if (jb  == 0) {
                ALOGE("Failed to add IzatLocationReport object for EPI to list");
            }
        }
    }
}

static void populateBestLocationListObj(
        JNIEnv* env, jint maxReports, jobject& bestLocationListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusBestPosition>& bestPosReportVec) {

    ALOGV("%s", __FUNCTION__);


    int count = bestPosReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusBestPosition& status =
                bestPosReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jint validityMask = (HAS_HORIZONTAL_COMPONENT | HAS_VERTICAL_COMPONENT);
        jdouble lat = status.mBestLat;
        jdouble lon = status.mBestLon;
        jfloat horzAccuracy = status.mBestHepe;
        jdouble alt= status.mBestAlt;
        jfloat altUnc = status.mBestAltUnc;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject bestLocReportObj = env->NewObject(IzatLocationReportClass,
                                                  method_locationReportCtor,
                                                  utcLastUpdatedObj, utcLastReportedObj,
                                                  validityMask, lat, lon, horzAccuracy,
                                                  alt, altUnc, 0);
        if (bestLocReportObj == nullptr) {
            ALOGE("Failed to create IzatLocationReport object for Best Location");
        } else {
            jboolean jb = env->CallBooleanMethod(bestLocationListObj,
                                                 method_addToList, bestLocReportObj);
            if (jb  == 0) {
                ALOGE("Failed to add IzatLocationReport object for BestLocation to list");
            }
        }
    }
}

static void populateGpsTimeReportListObj(
        JNIEnv* env, jint maxReports, jobject& gpsTimeReportListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusTimeAndClock>&
        timeAndClockReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = timeAndClockReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusTimeAndClock& status =
                timeAndClockReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jint gpsWeek = status.mGpsWeek;
        jlong gpsTimeOfWeekInMs = status.mGpsTowMs;
        jboolean timeValid = (status.mTimeValid == 1 ? JNI_TRUE : JNI_FALSE);
        jint  timeSource = status.mTimeSource;
        jint timeUncertainity = status.mTimeUnc;
        jint clockFreqBias =  status.mClockFreqBias;
        jint clockFreBiasUnc = status.mClockFreqBiasUnc;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject timeReportObj = env->NewObject(IzatGpsTimeReportClass, method_gpsTimeReportCtor,
                                               utcLastUpdatedObj, utcLastReportedObj, gpsWeek,
                                               gpsTimeOfWeekInMs, timeValid, timeSource,
                                               timeUncertainity, clockFreqBias, clockFreBiasUnc);

        if (timeReportObj == nullptr) {
            ALOGE("Failed to create GpsTimeDebugReport object");
        } else {
            jboolean jb = env->CallBooleanMethod(gpsTimeReportListObj, method_addToList,
                                                 timeReportObj);
            if (jb == JNI_FALSE) {
                ALOGE("Failed to add GpsTimeDebugReport object");
            }
        }
    }
}

static void populateXoStateReportListObj(
        JNIEnv* env, jint maxReports, jobject& xoStateReportListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusXoState>& xoStateReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = xoStateReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusXoState& status =
                xoStateReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jint xostate = status.mXoState;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject xoReportObj = env->NewObject(IzatXoStateReportClass, method_xoStateReportCtor,
                                             utcLastUpdatedObj, utcLastReportedObj, xostate);

        if (xoReportObj == nullptr) {
            ALOGE("Failed to create IzatXoStateDebugReport object");
        } else {
            jboolean jb = env->CallBooleanMethod(xoStateReportListObj, method_addToList,
                                                 xoReportObj);
            if (jb == JNI_FALSE) {
                ALOGE("Failed to add IzatXoStateDebugReport object");
            }
        }
    }
}

static void populateRfStateReportListObj(
        JNIEnv* env, jint maxReports, jobject& rfStateReportListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusRfAndParams>& rfParamsReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = rfParamsReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusRfAndParams& status =
                rfParamsReportVec[i];

#ifndef LOC_HIDL_VERSION_1_0
        jlong utcSecondsPartLastUpdated = status.base_1_0.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.base_1_0.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.base_1_0.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.base_1_0.mUtcReported.tv_nsec;

        jint pgaGain = status.base_1_0.mPgaGain;
        jlong adcI = status.base_1_0.mAdcI;
        jlong adcQ = status.base_1_0.mAdcQ;
        jlong jammerMetricGps =status.base_1_0.mJammerGps;
        jlong jammerMetricGlonass = status.base_1_0.mJammerGlo;
        jlong jammerMetricBds = status.base_1_0.mJammerBds;
        jlong jammerMetricGal = status.base_1_0.mJammerGal;
        jlong gpsBpAmplI = status.base_1_0.mGpsBpAmpI;
        jlong gpsBpAmplQ = status.base_1_0.mGpsBpAmpQ;
        jlong gloBpAmplI = status.mGloBpAmpI;
        jlong gloBpAmplQ = status.mGloBpAmpQ;
        jlong bdsBpAmplI = status.mBdsBpAmpI;
        jlong bdsBpAmplQ = status.mBdsBpAmpQ;
        jlong galBpAmplI = status.mGalBpAmpI;
        jlong galBpAmplQ = status.mGalBpAmpQ;
#else
        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jint pgaGain = status.mPgaGain;
        jlong adcI = status.mAdcI;
        jlong adcQ = status.mAdcQ;
        jlong jammerMetricGps =status.mJammerGps;
        jlong jammerMetricGlonass = status.mJammerGlo;
        jlong jammerMetricBds = status.mJammerBds;
        jlong jammerMetricGal = status.mJammerGal;
        jlong gpsBpAmplI = status.mGpsBpAmpI;
        jlong gpsBpAmplQ = status.mGpsBpAmpQ;
        jlong gloBpAmplI = 0;
        jlong gloBpAmplQ = 0;
        jlong bdsBpAmplI = 0;
        jlong bdsBpAmplQ = 0;
        jlong galBpAmplI = 0;
        jlong galBpAmplQ = 0;
#endif

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject rfReportObj = env->NewObject(IzatRfStateReportClass, method_rfStateReportCtor,
                                             utcLastUpdatedObj, utcLastReportedObj, pgaGain,
                                             adcI, adcQ, jammerMetricGps, jammerMetricGlonass,
                                             jammerMetricBds, jammerMetricGal, gpsBpAmplI,
                                             gpsBpAmplQ, gloBpAmplI, gloBpAmplQ, bdsBpAmplI,
                                             bdsBpAmplQ, galBpAmplI, galBpAmplQ);
        if (rfReportObj == nullptr) {
            ALOGE("Failed to create IzatRfStateDebugReport object");
        } else {
            jboolean jb = env->CallBooleanMethod(rfStateReportListObj, method_addToList, rfReportObj);
            if (jb  == 0) {
                ALOGE("Failed to add IzatRfStateDebugReport object");
            }
        }
    }
}

static void populateErrorRecoveriesListObj(
        JNIEnv* env, jint maxReports, jobject& errorRecoveriesListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusErrRecovery>&
        errRecoveryReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = errRecoveryReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusErrRecovery& status =
                errRecoveryReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject errorRecoveryReportObj = env->NewObject(IzatErrorRecoveryClass, method_errorRecoveryCtor,
                                              utcLastUpdatedObj, utcLastReportedObj);
        if (errorRecoveryReportObj == nullptr) {
            ALOGE("Failed to create IzatErrorRecoveryReport object");
        } else {
            jboolean jb = env->CallBooleanMethod(errorRecoveriesListObj, method_addToList, errorRecoveryReportObj);
            if (jb  == 0) {
                ALOGE("Failed to add IzatErrorRecoveryReport object");
            }
        }
    }
}

static void populatePdrReporListObj(
        JNIEnv* env, jint maxReports, jobject& pdrReportListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusPdr>& pdrReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = pdrReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusPdr& status =
                pdrReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jint pdrMask = status.mFixInfoMask;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject pdrReportObj = env->NewObject(IzatPDRReportClass, method_pdrReportCtor,
                                              utcLastUpdatedObj, utcLastReportedObj, pdrMask);
        if (pdrReportObj == nullptr) {
            ALOGE("Failed to create IzatPDRDebugReport object");
        } else {
            jboolean jb = env->CallBooleanMethod(pdrReportListObj, method_addToList, pdrReportObj);
            if (jb  == 0) {
                ALOGE("Failed to add IzatPDRDebugReport object");
            }
        }
    }
}

static void populateSvHealthReportListObj(
        JNIEnv* env, jint maxReports, jobject& svHealthReportListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusSvHealth>& svHealthReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = svHealthReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusSvHealth& status =
                svHealthReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jint gpsGoodMask = status.mGpsGoodMask;
        jint glonassGoodMask = status.mGloGoodMask;
        jlong bdsGoodMask = status.mBdsGoodMask;
        jlong galGoodMask = status.mGalGoodMask;
        jbyte qzssGoodMask = status.mQzssGoodMask;

        jint gpsBadMask = status.mGpsBadMask;
        jint glonassBadMask = status.mGloBadMask;
        jlong bdsBadMask = status.mBdsBadMask;
        jlong galBadMask = status.mGalBadMask;
        jbyte qzssBadMask = status.mQzssBadMask;

        jint gpsUnknownMask = status.mGpsUnknownMask;
        jint glonassUnknownMask = status.mGloUnknownMask;
        jlong bdsUnknownMask = status.mBdsUnknownMask;
        jlong galUnknownMask = status.mGalUnknownMask;
        jbyte qzssUnknownMask = status.mQzssUnknownMask;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject svHealthReportObj = env->NewObject(IzatSVHealthReportClass, method_svhealthReportCtor,
                                                   utcLastUpdatedObj, utcLastReportedObj, gpsGoodMask, gpsBadMask,
                                                   gpsUnknownMask, glonassGoodMask, glonassBadMask,
                                                   glonassUnknownMask, bdsGoodMask, bdsBadMask, bdsUnknownMask,
                                                   galGoodMask, galBadMask, galUnknownMask, qzssGoodMask,
                                                   qzssBadMask, qzssUnknownMask);
        if (svHealthReportObj == nullptr) {
            ALOGE("Failed to create IzatSVHealthDebugReport object");
        } else {
            jboolean jb = env->CallBooleanMethod(svHealthReportListObj, method_addToList, svHealthReportObj);
            if (jb  == 0) {
                ALOGE("Failed to add IzatSVHealthDebugReport object");
            }
        }
    }
}

static void populateXtraReportListObj(
        JNIEnv* env, jint maxReports, jobject& xtraReportListObj,
        hidl_vec<ILocHidlDebugReportService::LocHidlSystemStatusXtra>& xtraReportVec) {

    ALOGV("%s", __FUNCTION__);

    int count = xtraReportVec.size();

    for (int i=0; i < count && i < maxReports; i++) {

        ILocHidlDebugReportService::LocHidlSystemStatusXtra& status =
                xtraReportVec[i];

        jlong utcSecondsPartLastUpdated = status.mUtcTime.tv_sec;
        jlong utcNanoSecPartLastUpdated = status.mUtcTime.tv_nsec;

        jlong utcSecondsPartLastReported = status.mUtcReported.tv_sec;
        jlong utcNanoSecPartLastReported = status.mUtcReported.tv_nsec;

        jbyte validityBit =status.mXtraValidMask;
        jint gpsXtraAge = status.mGpsXtraAge;
        jint gloXtraAge= status.mGloXtraAge;
        jint bdsXtraAge = status.mBdsXtraAge;
        jint galXtraAge = status.mGalXtraAge;
        jint qzssXtraAge = status.mQzssXtraAge;
        jint gpsXtraValidity = status.mGpsXtraValid;
        jint gloXtraValidity = status.mGloXtraValid;
        jlong bdsXtraValidity = status.mBdsXtraValid;
        jlong galXtraValidity =status.mGalXtraValid;
        jbyte qzssXtraValidity = status.mQzssXtraValid;

        jobject utcLastUpdatedObj = createUtcTimeObject(env,
            utcSecondsPartLastUpdated, utcNanoSecPartLastUpdated);

        jobject utcLastReportedObj = createUtcTimeObject(env,
            utcSecondsPartLastReported, utcNanoSecPartLastReported);

        jobject xtraReportObj = env->NewObject(IzatXTRAReportClass, method_xtraReportCtor,
                                               utcLastUpdatedObj, utcLastReportedObj,
                                               validityBit, gpsXtraValidity, gpsXtraAge,
                                               gloXtraValidity, gloXtraAge, bdsXtraValidity, bdsXtraAge,
                                               galXtraValidity, galXtraAge, qzssXtraValidity, qzssXtraAge);
        if (xtraReportObj == nullptr) {
            ALOGE("Failed to create IzatXTRADebugReport object");
        } else {
            jboolean jb = env->CallBooleanMethod(xtraReportListObj, method_addToList, xtraReportObj);
            if (jb  == 0) {
                ALOGE("Failed to add IzatXTRADebugReport object");
            }
        }
    }
}

static void DebugReportService_getReport(JNIEnv* env, jobject /*obj*/,
        jint maxReports, jobject ephmerisStatusListObj,
        jobject fixStatusListObj, jobject epiReportListObj, jobject bestLocationListObj,
        jobject gpsTimeReportListObj, jobject xoStateReportListObj, jobject rfStateReportListObj,
        jobject errorRecoveriesListObj, jobject pdrReportListObj, jobject svHealthReportListObj,
        jobject xtraReportListObj)
{
    ALOGV("%s", __FUNCTION__);

    if (maxReports <= 0) {

        ALOGE("Invalid maxReports requested %d", maxReports);
        return;
    }
    auto iface = getDebugReportIface();
    if (iface == nullptr) {
        ALOGE("DebugReportService Iface NULL !");
        return;
    }

    if (maxReports == 1) {
        ALOGD("DebugReportService_getReport, Get Latest 1 sec data.");
    } else if (maxReports != 1) {
        ALOGD("DebugReportService_getReport, Get latest all available data.");
    }

    TO_HIDL_SERVICE();
    ILocHidlDebugReportService::LocHidlSystemStatusReports allReports = {};
#ifndef LOC_HIDL_VERSION_1_0
    auto r = iface->getReport_1_1(
            maxReports,
            [&allReports](const ILocHidlDebugReportService::LocHidlSystemStatusReports& reports) {
                allReports = reports;
            });
#else
    auto r = iface->getReport(
            maxReports,
            [&allReports](const ILocHidlDebugReportService::LocHidlSystemStatusReports& reports) {
                allReports = reports;
            });
#endif

    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        return;
    }

    // Now populate the lists
#ifndef LOC_HIDL_VERSION_1_0
    if (!allReports.base_1_0.mSuccess) {
        ALOGE("Failed to get debug reports !");
        return;
    }

    populateEphemerisStatusListObj(
            env, maxReports, ephmerisStatusListObj, allReports.base_1_0.mEphemerisVec);
    populateFixStatusListObj(
            env, maxReports, fixStatusListObj, allReports.base_1_0.mPositionFailureVec);
    populateEpiReportListObj(
            env, maxReports, epiReportListObj, allReports.base_1_0.mInjectedPositionVec);
    populateBestLocationListObj(
            env, maxReports, bestLocationListObj, allReports.base_1_0.mBestPositionVec);
    populateGpsTimeReportListObj(
            env, maxReports, gpsTimeReportListObj, allReports.base_1_0.mTimeAndClockVec);
    populateXoStateReportListObj(
            env, maxReports, xoStateReportListObj, allReports.base_1_0.mXoStateVec);
    populateRfStateReportListObj(
            env, maxReports, rfStateReportListObj, allReports.mRfAndParamsVec_1_1);
    populateErrorRecoveriesListObj(
            env, maxReports, errorRecoveriesListObj, allReports.base_1_0.mErrRecoveryVec);
    populatePdrReporListObj(
            env, maxReports, pdrReportListObj, allReports.base_1_0.mPdrVec);
    populateSvHealthReportListObj(
            env, maxReports, svHealthReportListObj, allReports.base_1_0.mSvHealthVec);
    populateXtraReportListObj(
            env, maxReports, xtraReportListObj, allReports.base_1_0.mXtraVec);
#else
    if (!allReports.mSuccess) {
        ALOGE("Failed to get debug reports !");
        return;
    }

    populateEphemerisStatusListObj(
            env, maxReports, ephmerisStatusListObj, allReports.mEphemerisVec);
    populateFixStatusListObj(
            env, maxReports, fixStatusListObj, allReports.mPositionFailureVec);
    populateEpiReportListObj(
            env, maxReports, epiReportListObj, allReports.mInjectedPositionVec);
    populateBestLocationListObj(
            env, maxReports, bestLocationListObj, allReports.mBestPositionVec);
    populateGpsTimeReportListObj(
            env, maxReports, gpsTimeReportListObj, allReports.mTimeAndClockVec);
    populateXoStateReportListObj(
            env, maxReports, xoStateReportListObj, allReports.mXoStateVec);
    populateRfStateReportListObj(
            env, maxReports, rfStateReportListObj, allReports.mRfAndParamsVec);
    populateErrorRecoveriesListObj(
            env, maxReports, errorRecoveriesListObj, allReports.mErrRecoveryVec);
    populatePdrReporListObj(
            env, maxReports, pdrReportListObj, allReports.mPdrVec);
    populateSvHealthReportListObj(
            env, maxReports, svHealthReportListObj, allReports.mSvHealthVec);
    populateXtraReportListObj(
            env, maxReports, xtraReportListObj, allReports.mXtraVec);
#endif
}

static JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    {"native_debugreport_class_init",
     "()V",
     reinterpret_cast<void*>(DebugReportService_class_init)},
    {"native_debugreport_init",
     "()V",
     reinterpret_cast<void*>(DebugReportService_init)},
    {"native_debugreport_deinit",
     "()V",
     reinterpret_cast<void*>(DebugReportService_deinit)},
    {"native_debugreport_getReport",
     "(ILjava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;)V",
     reinterpret_cast<void*>(DebugReportService_getReport)}
};

int register_DebugReportService(JNIEnv* env)
{
    ALOGD("register_DebugReportService");
    return jniRegisterNativeMethods(env,
                                    "com/qualcomm/location/izat/debugreport/DebugReportService",
                                    sMethods,
                                    NELEM(sMethods));
}

