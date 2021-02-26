/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <binder/Parcel.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <binder/IPCThreadState.h>
#include "eSEPowerManager.h"

#define LOG_TAG             "eSEPowerManager"
#define nfc_dev_node        "/dev/nq-nci"
#define ESE_SET_PWR         _IOW(0xE9, 0x02, unsigned int)
#define ESE_GET_PWR         _IOR(0xE9, 0x03, unsigned int)

namespace android {

/*
 * This method will print the list of the registered PIDs.
 */

void  eSEPowerManager::printPidsMap() {
    for(std::map<uint32_t, struct service_data>::iterator it = pidsMap.begin(); it != pidsMap.end(); ++it)
    {
        ALOGD("MAP PID=%d !",it->first);
    }
}

/*
 * This method will check if the PidsMap is empty.
 * if it's not the case, it will print all the pids stored.
 */

bool  eSEPowerManager::isPidsMapEmpty() {
    bool status = pidsMap.empty();
    if (status) {
        ALOGD("pidsMap empty");
    } else {
        printPidsMap();
    }
    return status;
}

eSEPowerManager::eSEPowerManager()
{
}

eSEPowerManager::~eSEPowerManager()
{
}

int eSEPowerManager::powerOn(const sp<IeSEPowerManagerCb> &notifier)
{
    IPCThreadState* self = IPCThreadState::self();
    int pid = self->getCallingPid();

    ALOGD("Power ON - PID=%d",pid);
    int ret = -1;

    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return nq_node;
        }
    }
    ret = ioctl(nq_node, ESE_GET_PWR, 0);
    if (ret < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
        return ret;
    }
    if (ret > 0) {
        ALOGD("eSE already powered ON");
    } else {
        if (!isPidsMapEmpty()) {
            //here the pid list is out of sync with the eSE power states.
            ALOGW("pidList out of sync ..."); // nothing to be done here.
        }
        ret = ioctl(nq_node, ESE_SET_PWR, 0);
        if (ret < 0) {
            ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
            return ret;
        }
    }
    struct service_data data = {0};
    data.l_serviceCb = asBinder(notifier);
    data.l_deathNotifier = new ClientDiedNotifier(pid, this);
    if (data.l_serviceCb == 0) {
        ALOGE("%s: No notifier",__func__);
        if (isPidsMapEmpty())
        {
            ret = ioctl(nq_node, ESE_SET_PWR, 1);
            if (ret < 0) {
                ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
                return ret;
            }
            ALOGD("eSE powered OFF - PID=%d", pid);
            if (close(nq_node)) {
                ALOGE("%s: eSE ioctl close failed : %s",__func__, strerror(errno));
            } else {
                nq_node = -1;
            }
        }
        return -1;
    }
    status_t ltdStat = data.l_serviceCb->linkToDeath(data.l_deathNotifier);
    if (ltdStat != NO_ERROR) {
        ALOGE("%s: Link to death failed",__func__);
        return -1;
    }
    pidsMap.insert( std::pair<uint32_t, struct service_data> (pid, data));
    ret = 1;
    return ret;
}

int eSEPowerManager::powerOff()
{
    IPCThreadState* self = IPCThreadState::self();
    int pid = self->getCallingPid();
    ALOGD("Power OFF - PID=%d",pid);
    int ret = -1;
    if (!pidsMap.count(pid)) {
        ALOGE("PID=%d not registered !",pid);
        return -1;
    }
    if (pidsMap[pid].l_serviceCb != 0 && pidsMap[pid].l_deathNotifier != 0) {
        pidsMap[pid].l_serviceCb->unlinkToDeath(pidsMap[pid].l_deathNotifier);
    }
    pidsMap.erase(pid);
    if (!isPidsMapEmpty()) {
        return 1;
    }
    if (nq_node < 0) {
        ALOGE("%s: eSE file not opened - let's assume it's off",__func__);
        return 0;
    }
    ret = ioctl(nq_node, ESE_SET_PWR, 1);
    if (ret < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
        return ret;
    }
    ALOGD("eSE powered OFF - PID=%d", pid);
    ret = 0;
    if (close(nq_node)) {
        ALOGE("%s: eSE ioctl close failed : %s",__func__, strerror(errno));
    } else {
        nq_node = -1;
    }
    return ret;
}

int eSEPowerManager::getState()
{
    int ret;
    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return nq_node;
        }
    }
    ret = ioctl(nq_node, ESE_GET_PWR, 0);
    if (ret < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
    }
    return ret;
}

int eSEPowerManager::killall()
{
    int ret;
    for(std::map<uint32_t, struct service_data>::iterator it = pidsMap.begin(); it != pidsMap.end(); ++it)
    {
        if (pidsMap[it->first].l_serviceCb != 0 && pidsMap[it->first].l_deathNotifier != 0) {
            pidsMap[it->first].l_serviceCb->unlinkToDeath(pidsMap[it->first].l_deathNotifier);
        }
    }
    pidsMap.clear();
    if (nq_node < 0) {
        nq_node = open(nfc_dev_node, O_RDWR);
        if (nq_node < 0) {
            ALOGE("%s: eSE opening failed : %s",__func__, strerror(errno));
            return nq_node;
        }
    }
    ret = ioctl(nq_node, ESE_SET_PWR, 1);
    if (ret < 0) {
        ALOGE("%s: eSE ioctl failed : %s",__func__, strerror(errno));
        return ret;
    }
    if (close(nq_node)) {
        ALOGE("%s: eSE ioctl close failed : %s",__func__, strerror(errno));
    } else {
        nq_node = -1;
    }
    ALOGD("eSE powered OFF");
    return ret;
}

status_t eSEPowerManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
  {
    return BneSEPowerManager::onTransact(code, data, reply, flags);
  }

void eSEPowerManager::ClientDiedNotifier::binderDied(const wp<IBinder> &who) {
    const wp<IBinder> unused = who;
    eSEPowerManager *service = reinterpret_cast <eSEPowerManager*>(mService);

    ALOGW("ESEP - Client died");
    service->pidsMap.erase(mPid);
}

}; // namespace android