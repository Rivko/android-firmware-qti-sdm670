/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "LocSvc_HIDL_IzatProvider_jni"
#define LOG_NDEBUG 0

#include <string>
#include <jni.h>
#include <JNIHelp.h>
#include "utils_jni.h"
#include <android_runtime/AndroidRuntime.h>
#include <utils/Log.h>
#include <map>
#include <inttypes.h>

#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
#include <vendor/qti/gnss/1.0/ILocHidlIzatProvider.h>
#include <vendor/qti/gnss/1.0/ILocHidlIzatProviderCallback.h>

using ::vendor::qti::gnss::V1_0::ILocHidlGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatProvider;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatProviderCallback;
using ::vendor::qti::gnss::V1_0::LocHidlIzatStreamType;
using ::vendor::qti::gnss::V1_0::LocHidlIzatLocation;
using ::vendor::qti::gnss::V1_0::LocHidlIzatProviderStatus;
using ::vendor::qti::gnss::V1_0::LocHidlIzatRequest;
using ::vendor::qti::gnss::V1_0::LocHidlNetworkPositionSourceType;
using ::vendor::qti::gnss::V1_0::LocHidlIzatHorizontalAccuracy;

using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::sp;

using namespace android;
using namespace std;

/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlGnss> gnssVendorHal = nullptr;
static sp<ILocHidlIzatProvider> networkProviderIface = nullptr;
static sp<ILocHidlIzatProvider> fusedProviderIface = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        gnssVendorHal = nullptr;
        networkProviderIface = nullptr;
        fusedProviderIface = nullptr;
        ALOGE("%s] ILocHidl service died with cookie: %" PRIu64, __FILE__, cookie);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

class IzatProviderGlue;
static IzatProviderGlue* networkProvider;
static IzatProviderGlue* fusedProvider;

static jmethodID method_onLocationChanged;
static jmethodID method_onStatusChanged;

static sp<ILocHidlGnss>& getVendorHal() {
    static uint64_t deathCount = 0;
    if (gnssVendorHal == nullptr) {
        // HIDL Service
        auto vendorGnss = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (vendorGnss == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
            ALOGD("Got GnssVendor Service.");

            if (locHidlDeathRecipient == nullptr) {
                locHidlDeathRecipient = new LocHidlDeathRecipient();
            }
            gnssVendorHal = vendorGnss;
            auto r = gnssVendorHal->linkToDeath(locHidlDeathRecipient, deathCount++);
            if (!r.isOk() || r == false) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                gnssVendorHal = nullptr;
            } else {
                auto izat = gnssVendorHal->getExtensionLocHidlIzatNetworkProvider();

                if (!izat.isOk()) {
                    ALOGE("Unable to get a handle to IzatProvider extension !");
                } else {
                    networkProviderIface = izat;
                    ALOGD("Got IzatProvider Extension.");
                }

                izat = gnssVendorHal->getExtensionLocHidlIzatFusedProvider();

                if (!izat.isOk()) {
                    ALOGE("Unable to get a handle to IzatProvider extension !");
                } else {
                    fusedProviderIface = izat;
                    ALOGD("Got IzatProvider Extension.");
                }
            }
        }
    }

    return gnssVendorHal;
}

inline static IzatProviderGlue**
providerTypeToNativeObj(jint providerType) {
    switch (static_cast<LocHidlIzatStreamType>(providerType)) {
    case LocHidlIzatStreamType::FUSED:
        return &fusedProvider;
    case LocHidlIzatStreamType::NETWORK:
        return &networkProvider;
    default:
        return NULL;
    }
}

inline static int providerStatusLookUp(const LocHidlIzatProviderStatus status) {
    switch (status) {
    case LocHidlIzatProviderStatus::GNSS_STATUS_SESSION_BEGIN:
        return 4; // GNSS_SESSION_BEGIN
    case LocHidlIzatProviderStatus::GNSS_STATUS_SESSION_END:
        return 5; // GNSS_SESSION_END
    case LocHidlIzatProviderStatus::GNSS_STATUS_ENGINE_ON:
        return 6; // GNSS_ENGINE_ON
    case LocHidlIzatProviderStatus::GNSS_STATUS_ENGINE_OFF:
        return 7; // GNSS_ENGINE_OFF
    default:
        return -1;
    }
}

static IzatProviderGlue* createGnssIzatRequest(LocHidlIzatRequest& request, jint providerType,
                                               jint numUpdates, jlong interval,
                                               jfloat smallestDisplacement, jint accuracy) {
    IzatProviderGlue** provider = providerTypeToNativeObj(providerType);

    if (provider && *provider != NULL) {
        request.provider = static_cast<LocHidlIzatStreamType>(providerType);
        request.numUpdates = numUpdates;
        request.timeIntervalBetweenFixes = interval;
        request.smallestDistanceBetweenFixes = smallestDisplacement;
        request.suggestedHorizontalAccuracy = static_cast<LocHidlIzatHorizontalAccuracy>(accuracy);

        ALOGD ("Provider Type: %d; Num Updates : %d; Interval : %" PRId64
               "Displacement : %f; Accuracy : %d", request.provider,
               request.numUpdates, request.timeIntervalBetweenFixes,
               request.smallestDistanceBetweenFixes, request.suggestedHorizontalAccuracy);

        return *provider;
    }
    return NULL;
}

/* ===========================================================
 *   HIDL Callbacks : ILocHidlIzatProviderCallback.hal
 * ===========================================================*/
class IzatProviderGlue : public ILocHidlIzatProviderCallback {
    LocHidlIzatStreamType mStreamType;
    const jobject mJavaProvider;
    bool mEnabled;
    sp<ILocHidlIzatProvider>& mNativeProvider;
public:
    inline IzatProviderGlue(LocHidlIzatStreamType streamType, jobject javaProvider) :
        mStreamType(streamType), mJavaProvider(javaProvider), mEnabled(false),
        mNativeProvider((mStreamType == LocHidlIzatStreamType::NETWORK) ?
                networkProviderIface : fusedProviderIface) {
        if (mNativeProvider != nullptr) {
            TO_HIDL_SERVICE();
            auto r = mNativeProvider->init(this);
            if (!r.isOk() || (r == false)) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
            }
        }
    }
    inline ~IzatProviderGlue() {
        JNIEnv* env = android::AndroidRuntime::getJNIEnv();
        if (env != NULL && mJavaProvider != NULL) {
            env->DeleteGlobalRef(mJavaProvider);
        }
        // We don't call getNativeProvider here, it will try to connect to service again.
        // If service has already died, just leave it be.
        if (mNativeProvider != nullptr) {
            TO_HIDL_SERVICE();
            auto r = mNativeProvider->deinit();
            if (!r.isOk()) {
                ALOGW("nativeProvider->deinit() of stream type %d failed", mStreamType);
            }
        }
    }
    inline sp<ILocHidlIzatProvider>& getNativeProvider() {
        getVendorHal();
        return mNativeProvider;
    }
    inline void setEnabled(bool isEnabled) {
        mEnabled = isEnabled;
    }

    Return<void> onLocationChanged(const LocHidlIzatLocation& location) override {
        FROM_HIDL_SERVICE();
        if (location.hasLatitude && location.hasLongitude && location.hasHorizontalAccuracy &&
            mEnabled && mJavaProvider && method_onLocationChanged) {

            JNIEnv* env = android::AndroidRuntime::getJNIEnv();
            if (env != NULL) {
                jlong utcTime = location.utcTimestampInMsec;
                jlong elapsedRealTimeNanos = location.elapsedRealTimeInNanoSecs;
                jdouble latitude = location.latitude;
                jdouble longitude = location.longitude;
                jboolean hasAltitude = location.hasAltitudeWrtEllipsoid;
                jdouble altitude = location.altitudeWrtEllipsoid;
                jboolean hasVerticalUnc = location.hasVertUnc;
                jfloat verticalUnc = location.vertUnc;
                jboolean hasSpeed = location.hasSpeed;
                jfloat speed = location.speed;
                jboolean hasSpeedUnc = location.hasSpeedUnc;
                jfloat speedUnc = location.speedUnc;
                jboolean hasBearing = location.hasBearing;
                jfloat bearing = location.bearing;
                jboolean hasBearingUnc = location.hasBearingUnc;
                jfloat bearingUnc = location.bearingUnc;
                jboolean hasAccuracy = location.hasHorizontalAccuracy;
                jfloat accuracy = location.horizontalAccuracy;
                jshort positionSource = -1;

                switch (location.networkPositionSource) {
                case LocHidlNetworkPositionSourceType::WIFI:
                    positionSource = 1;
                    break;
                case LocHidlNetworkPositionSourceType::CELL:
                    positionSource = 0;
                    break;
                }

                env->CallVoidMethod(mJavaProvider, method_onLocationChanged,
                                    utcTime, elapsedRealTimeNanos, latitude,
                                    longitude, hasAltitude, altitude, hasVerticalUnc,
                                    verticalUnc, hasSpeed, speed,
                                    hasSpeedUnc, speedUnc,
                                    hasBearing, bearing,
                                    hasBearingUnc, bearingUnc,
                                    hasAccuracy, accuracy, positionSource);
            }
        }
        return Void();
    }

    Return<void> onStatusChanged(LocHidlIzatProviderStatus status) {
        FROM_HIDL_SERVICE();
        JNIEnv* env = android::AndroidRuntime::getJNIEnv();
        jint javaStatus = providerStatusLookUp(status);
        if (mStreamType == LocHidlIzatStreamType::FUSED && mJavaProvider &&
            method_onStatusChanged && env && javaStatus != -1) {
            env->CallVoidMethod(mJavaProvider, method_onStatusChanged, javaStatus);
        }
        return Void();
    }
};

// Java to native calls
static void onJavaClassLoad(JNIEnv* env, jclass clazz) {
    method_onLocationChanged = env->GetMethodID(clazz,
                                                "onLocationChanged", "(JJDDZDZFZFZFZFZFZFS)V");
    method_onStatusChanged = env->GetMethodID(clazz,
                                              "onStatusChanged", "(I)V");
    // Setting the values of LocHidlIzatStreamType::NETWORK / LocHidlIzatStreamType::FUSED
    env->SetStaticIntField(clazz,
                           env->GetStaticFieldID(clazz, "IZAT_STREAM_NETWORK", "I"),
                           static_cast<int>(LocHidlIzatStreamType::NETWORK));
    env->SetStaticIntField(clazz,
                           env->GetStaticFieldID(clazz, "IZAT_STREAM_FUSED", "I"),
                           static_cast<int>(LocHidlIzatStreamType::FUSED));
    env->SetStaticIntField(clazz,
                           env->GetStaticFieldID(clazz, "IZAT_HORIZONTAL_FINE", "I"),
                           static_cast<int>(LocHidlIzatHorizontalAccuracy::FINE));
    env->SetStaticIntField(clazz,
                           env->GetStaticFieldID(clazz, "IZAT_HORIZONTAL_BLOCK", "I"),
                           static_cast<int>(LocHidlIzatHorizontalAccuracy::BLOCK));
    env->SetStaticIntField(clazz,
                           env->GetStaticFieldID(clazz, "IZAT_HORIZONTAL_NONE", "I"),
                           static_cast<int>(LocHidlIzatHorizontalAccuracy::NONE));
}

static void onJavaStaticLoad(JNIEnv* /*env*/, jclass /*clazz*/) {
    getVendorHal();
}

static void onJavaInstanceInit(JNIEnv *env, jobject obj, int providerType) {
    if (networkProviderIface == nullptr || fusedProviderIface == nullptr) {
        ALOGE("networkProviderIface or fusedProviderIface is null");
        return;
    }

    IzatProviderGlue** provider = providerTypeToNativeObj(providerType);
    if (provider && *provider == nullptr) {
        *provider = new IzatProviderGlue(static_cast<LocHidlIzatStreamType>(providerType),
                                         env->NewGlobalRef(obj));
    }
}

static void onJavaInstanceDeinit(JNIEnv* /*env*/, jobject /*obj*/, int providerType) {
    IzatProviderGlue** provider = providerTypeToNativeObj(providerType);

    if (provider) {
        if (*provider) {
            delete *provider;
        }
        *provider = nullptr;
    }
}

static void onEnable(JNIEnv* /*env*/, jobject /*obj*/, jint providerType) {
    IzatProviderGlue** provider = providerTypeToNativeObj(providerType);

    if (provider != nullptr && *provider != nullptr) {
        sp<ILocHidlIzatProvider> nativeProvider = (*provider)->getNativeProvider();
        if (nativeProvider != nullptr) {
            TO_HIDL_SERVICE();
            auto r =nativeProvider->onEnable();
            if (!r.isOk() || (r == false)) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
            } else {
                (*provider)->setEnabled(true);
            }
        }
    }
}

static void onDisable(JNIEnv* /*env*/, jobject /*obj*/, jint providerType) {
    IzatProviderGlue** provider = providerTypeToNativeObj(providerType);

    if (provider != nullptr && *provider != nullptr) {
        sp<ILocHidlIzatProvider> nativeProvider = (*provider)->getNativeProvider();
        if (nativeProvider != nullptr) {
            TO_HIDL_SERVICE();
            auto r = nativeProvider->onDisable();
            if (!r.isOk() || (r == false)) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
            } else {
                (*provider)->setEnabled(false);
            }
        }
    }
}

static void onAddRequest(JNIEnv* /*env*/, jobject /*obj*/, jint providerType,
                         jint numUpdates, jlong interval,
                         jfloat smallestDisplacement, jint accuracy) {
    LocHidlIzatRequest request = {};
    IzatProviderGlue* provider = createGnssIzatRequest(request, providerType, numUpdates,
                                                       interval, smallestDisplacement,
                                                       accuracy);

    if (provider != nullptr) {
        sp<ILocHidlIzatProvider> nativeProvider = provider->getNativeProvider();
        if (nativeProvider != nullptr) {
            TO_HIDL_SERVICE();
            auto r = nativeProvider->onAddRequest(request);
            if (!r.isOk() || (r == false)) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
            }
        }
    }
}

static void onRemoveRequest(JNIEnv* /*env*/, jobject /*obj*/, jint providerType,
                         jint numUpdates, jlong interval,
                         jfloat smallestDisplacement, jint accuracy) {
    LocHidlIzatRequest request = {};
    IzatProviderGlue* provider = createGnssIzatRequest(request, providerType, numUpdates,
                                                       interval, smallestDisplacement,
                                                       accuracy);

    if (provider != nullptr) {
        sp<ILocHidlIzatProvider> nativeProvider = provider->getNativeProvider();
        if (nativeProvider != nullptr) {
            TO_HIDL_SERVICE();
            auto r = nativeProvider->onRemoveRequest(request);
            if (!r.isOk() || (r == false)) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
            }
        }
    }
}

static JNINativeMethod sMethods[] = {
    {"nativeOnClassLoad", "()V", (void*)onJavaClassLoad},
    {"nativeOnStaticLoad", "()V", (void*)onJavaStaticLoad},
    {"nativeOnInstanceInit", "(I)V", (void*)onJavaInstanceInit},
    {"nativeOnInstanceDeinit", "(I)V", (void *) onJavaInstanceDeinit},
    {"nativeOnEnableProvider","(I)V", (void*)onEnable},
    {"nativeOnDisableProvider","(I)V", (void*)onDisable},
    {"nativeOnAddRequest","(IIJFI)V", (void*)onAddRequest},
    {"nativeOnRemoveRequest", "(IIJFI)V", (void*)onRemoveRequest}
};


int register_IzatProvider(JNIEnv* env) {
    return jniRegisterNativeMethods(env,
                                    "com/qualcomm/location/izatprovider/IzatProvider",
                                    sMethods, NELEM(sMethods));
}
