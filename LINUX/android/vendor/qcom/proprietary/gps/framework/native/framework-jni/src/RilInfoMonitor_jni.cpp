/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "LocSvc_HIDL_RilInfoMonitor_jni"
#define LOG_NDEBUG 0

#include "JNIHelp.h"
#include "jni.h"
#include "utils_jni.h"
#include <utils/Log.h>

#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
#include <vendor/qti/gnss/1.0/ILocHidlRilInfoMonitor.h>

using ::vendor::qti::gnss::V1_0::ILocHidlGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlRilInfoMonitor;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::sp;

using namespace android;

static jobject jRilInfoMonitor = NULL;

/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlRilInfoMonitor> gnssRilInfoMonitorIface = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) override {
        gnssRilInfoMonitorIface = nullptr;
        ALOGE("%s] ILocHidlRilInfoMonitor died", __FILE__);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

static sp<ILocHidlRilInfoMonitor>& getRilInfoMonitorIface() {
    static uint64_t deathCount = 0;
    if (gnssRilInfoMonitorIface == nullptr) {
        auto gnssVendorHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (gnssVendorHal == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
            ALOGD("Got GnssVendor Service.");

            auto ril = gnssVendorHal->getExtensionLocHidlRilInfoMonitor();

            if (!ril.isOk()) {
                ALOGE("Unable to get a handle to RilInfoMonitor extension !");
            } else {
                ALOGD("Got RilInfoMonitor Extension.");
                if (locHidlDeathRecipient == nullptr) {
                    locHidlDeathRecipient = new LocHidlDeathRecipient();
                }
                gnssRilInfoMonitorIface = ril;
                auto r = gnssRilInfoMonitorIface->linkToDeath(locHidlDeathRecipient, deathCount++);
                if (!r.isOk() || r == false) {
                    ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                    gnssRilInfoMonitorIface = nullptr;
                }
            }
        }
    }
    return gnssRilInfoMonitorIface;
}

static void instanceInit(JNIEnv *env, jobject obj) {

    ALOGD("%s", __FUNCTION__);

    if (NULL == jRilInfoMonitor) {
        jRilInfoMonitor = env->NewGlobalRef(obj);
    }

    auto iface = getRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("gnssRilInfoMonitorIface null !");
        return;
    }
    TO_HIDL_SERVICE();
    auto r = iface->init();
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void cinfoInject(JNIEnv* /*env*/, jobject /*obj*/,
                        jint cid, jint lac, jint mnc,
                        jint mcc, jboolean roaming) {

    ALOGD("%s", __FUNCTION__);

    auto iface = getRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("gnssRilInfoMonitorIface null !");
        return;
    }
    TO_HIDL_SERVICE();
    auto r = iface->cinfoInject(cid, lac, mnc, mcc, roaming);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void oosInform(JNIEnv* /*env*/, jobject /*obj*/) {

    ALOGD("%s", __FUNCTION__);

    auto iface = getRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("gnssRilInfoMonitorIface null !");
        return;
    }
    TO_HIDL_SERVICE();
    auto r = iface->oosInform();
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static void niSuplInit(JNIEnv* env, jobject /*obj*/,
                       jbyteArray data, jint length) {

    ALOGD("%s", __FUNCTION__);

    auto iface = getRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("gnssRilInfoMonitorIface null !");
        return;
    }
    if (length < 1) {
        ALOGE("Invalid length %d", length);
        return;
    }

    char* buf = new char[length + 1];
    if (buf == nullptr) {
        ALOGE("new allocation failed, fatal error.");
        return;
    }
    memset(buf, 0, length+1);
    jbyte* bytes = (jbyte *)env->GetPrimitiveArrayCritical(data, 0);
    for (int i=0; i < length; i++) {
        buf[i] = (char)bytes[i];
    }
    env->ReleasePrimitiveArrayCritical(data, bytes, JNI_ABORT);

    TO_HIDL_SERVICE();
    auto r = iface->niSuplInit(buf);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
    delete[] buf;
}

static void chargerStatusInject(JNIEnv* /*env*/, jobject /*obj*/,
                                jint status) {

    ALOGD("%s", __FUNCTION__);

    auto iface = getRilInfoMonitorIface();
    if (iface == nullptr) {
        ALOGE("gnssRilInfoMonitorIface null !");
        return;
    }
    TO_HIDL_SERVICE();
    auto r = iface->chargerStatusInject(status);
    if (!r.isOk()) {
        ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
    }
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"native_rm_init", "()V", (void *)instanceInit},
    {"native_rm_cinfo_inject", "(IIIIZ)V", (void *)cinfoInject},
    {"native_rm_oos_inform", "()V", (void *)oosInform},
    {"native_rm_ni_supl_init", "([BI)V", (void *)niSuplInit},
};

int register_RilInfoMonitor(JNIEnv* env) {
    return jniRegisterNativeMethods(env, "com/qualcomm/location/RilInfoMonitor",
                                    sMethods, NELEM(sMethods));
}
