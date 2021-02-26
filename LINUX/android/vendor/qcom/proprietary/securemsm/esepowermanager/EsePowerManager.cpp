/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "EsePowerManager.h"

namespace vendor {
namespace qti {
namespace esepowermanager {
namespace V1_0 {
namespace implementation {

int nq_node = -1;
static std::mutex m;

std::function<void(sp<EsePowerManagerDeathRecipient>&)> unlink_cb_;
EsePowerManager::EsePowerManager(){
}

void closeNode() {

    if (close(nq_node)) {
        ALOGE("%s: eSE ioctl close failed : %s",__func__, strerror(errno));
    }
    nq_node = -1;
    return;
}
// Methods from ::vendor::qti::proprietary::esepowermanager::V1_0::IEsePowerManager follow.
Return<int32_t> EsePowerManager::powerOn(const sp<IBase>& callback) {
    std::lock_guard<std::mutex> lock(m);
    int retIoctl = -1;
    int32_t ret = -1;
    if (callback == nullptr) {
        ALOGE("callback == nullptr!");
        return ret;
    }

    pid_t pid = IPCThreadState::self()->getCallingPid();

    ALOGD("Power ON - PID=%d",pid);
    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return ret;
        }
    }
    retIoctl = ioctl(nq_node, ESE_GET_PWR, 0);
    if (retIoctl < 0) {
        ALOGE("%s: ESE_GET_PWR failed: %s",__func__, strerror(errno));
        closeNode();
        return ret;
    }


    if (retIoctl > 0) {
        ALOGD("eSE already powered ON");
    } else {
        if (!pidsMap.empty()) {
            //here the pid list is out of sync with the eSE power states.
            ALOGW("pidList out of sync ..."); // nothing to be done here.
            for(std::map<pid_t, struct service_data>::iterator it = pidsMap.begin(); it != pidsMap.end(); ++it) {
                ALOGD("MAP PID=%d !",it->first);
            }
        }
        retIoctl = ioctl(nq_node, ESE_SET_PWR, 0);
        if (retIoctl < 0) {
            ALOGE("%s: ESE_SET_PWR failed: %s",__func__, strerror(errno));
            closeNode();
            return ret;
        }
    }
    data.mDeathRecipient = new EsePowerManagerDeathRecipient(this, pid);
    if (data.mDeathRecipient == NULL) {
        ALOGE("%s: mDeathRecipient NULL",__func__);
        closeNode();
        return ret;
    }
    data.event_cb_ = callback;
    data.event_cb_->linkToDeath(data.mDeathRecipient, 0);
    pidsMap.insert( std::pair<pid_t, struct service_data> (pid, data));
    ret = 1;
    return ret;
}

Return<int32_t> EsePowerManager::powerOff() {
    std::lock_guard<std::mutex> lock(m);
    IPCThreadState* self = IPCThreadState::self();
    pid_t pid = self->getCallingPid();
    ALOGD("Power OFF - PID=%d",pid);

    int retIoctl = -1;
    int32_t ret = -1;
    if (!pidsMap.count(pid)) {
        ALOGE("PID=%d not registered !",pid);
        return ret;
    }
    if (pidsMap[pid].event_cb_ != nullptr && pidsMap[pid].mDeathRecipient != nullptr) {
        pidsMap[pid].event_cb_->unlinkToDeath(pidsMap[pid].mDeathRecipient);
    }
    pidsMap.erase(pid);
    if (!pidsMap.empty()) {
        return 1;
    }
    if (nq_node < 0) {
        ALOGD("%s: eSE file not opened - let's assume it's off",__func__);
        return 0;
    }
    retIoctl = ioctl(nq_node, ESE_SET_PWR, 1);
    if (retIoctl < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
        closeNode();
        return ret;
    }
    ALOGD("eSE powered OFF - PID=%d", pid);

    closeNode();
    ret = 0;
    return ret;
}

Return<int32_t> EsePowerManager::getState() {
    std::lock_guard<std::mutex> lock(m);
    int32_t ret = -1;
    int retIoctl = -1;

    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return ret;
        }
    }
    retIoctl = ioctl(nq_node, ESE_GET_PWR, 0);
    if (retIoctl < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
        closeNode();
        return ret;
    }
    ret = 1;
    if (retIoctl == 0)
        return 0;
    else
        return ret;
}

Return<int32_t> EsePowerManager::killall() {
    std::lock_guard<std::mutex> lock(m);
    int32_t ret = -1;
    int retIoctl = -1;
    pidsMap.clear();
    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return ret;
        }
    }
    retIoctl = ioctl(nq_node, ESE_SET_PWR, 1);
    if (retIoctl < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
        closeNode();
        return ret;
    }
    closeNode();
    ALOGD("eSE powered OFF");
    ret = 0;
    return ret;
}

Return<void> EsePowerManager::ClientDiedNotifier(pid_t mPid) {

    int retIoctl = -1;
    ALOGE("Client died - PID :  %d", mPid);
    pidsMap.erase(mPid);
    if (pidsMap.empty()) {
        if (nq_node != -1) {
            retIoctl = ioctl(nq_node, ESE_SET_PWR, 1);
        } else {
            ALOGE("NQ_NODE not opened - let's assume eSE is off");
            return Void();
        }
        if (retIoctl < 0) {
            ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
        }
        ALOGD("eSE powered OFF -no more Pids in the map");
        closeNode();
    }
    return Void();
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

IEsePowerManager* HIDL_FETCH_IEsePowerManager(const char* /* name */) {
    return new EsePowerManager();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace esepowermanager
}  // namespace qti
}  // namespace vendor
