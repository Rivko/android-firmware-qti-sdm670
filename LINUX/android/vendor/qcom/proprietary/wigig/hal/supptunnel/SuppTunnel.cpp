/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include <android-base/logging.h>
#include "SuppTunnel.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace wigig {
namespace supptunnel {
namespace V1_0 {
namespace implementation {

SuppTunnel::SuppTunnel()
{
    // nothing to do
}

SuppTunnel::SuppTunnel(SuppConnector *connector)
:mConnector(connector)
{
    mConnector->registerCallback(this);
}

SuppTunnel::~SuppTunnel()
{
    mConnector->registerCallback(NULL);
}

// Methods from ::vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnel follow.
Return<void> SuppTunnel::startSupplicant(startSupplicant_cb _hidl_cb) {
    SuppTunnelStatus status = {SuppTunnelStatusCode::FAILURE_UNKNOWN, ""};
    int ret = mConnector->startSupplicant();

    if (!ret)
        status = {SuppTunnelStatusCode::SUCCESS, ""};
    _hidl_cb(status);
    return Void();
}

Return<void> SuppTunnel::killSupplicant(killSupplicant_cb _hidl_cb) {
    SuppTunnelStatus status = {SuppTunnelStatusCode::FAILURE_UNKNOWN, ""};
    int ret = mConnector->killSupplicant();

    if (!ret)
        status = {SuppTunnelStatusCode::SUCCESS, ""};
    _hidl_cb(status);
    return Void();
}

Return<void> SuppTunnel::doCommand(const hidl_string& command, doCommand_cb _hidl_cb) {
    char reply[SUPP_REPLY_BUF_SIZE] = {0};
    size_t replylen = sizeof(reply);
    SuppTunnelStatus status = {SuppTunnelStatusCode::FAILURE_UNKNOWN, ""};
    hidl_string replyStr;

    int ret = mConnector->sendCommand(command.c_str(), reply, &replylen);
    if (!ret) {
        replyStr = hidl_string(reply, replylen);
        status = {SuppTunnelStatusCode::SUCCESS, ""};
    }
    _hidl_cb(status, replyStr);
    return Void();
}

Return<void> SuppTunnel::registerCallback(const sp<ISuppTunnelCallback>& callback, registerCallback_cb _hidl_cb) {
    SuppTunnelStatus status = {SuppTunnelStatusCode::FAILURE_UNKNOWN, ""};

    int ret = addCallback(callback);
    if (ret)
        LOG(ERROR) << "failed to add callback: " << ret;
    else
        status = {SuppTunnelStatusCode::SUCCESS, ""};

    _hidl_cb(status);
    return Void();
}

Return<void> SuppTunnel::setConfigOption(const hidl_string& name, const hidl_string& value, setConfigOption_cb _hidl_cb) {
    SuppTunnelStatus status = {SuppTunnelStatusCode::FAILURE_UNKNOWN, ""};
    int ret = mConnector->setConfigOption(name.c_str(), value.c_str());

    if (!ret)
        status = {SuppTunnelStatusCode::SUCCESS, ""};
    _hidl_cb(status);
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

ISuppTunnel* HIDL_FETCH_ISuppTunnel(const char* /* name */) {
    return new SuppTunnel();
}

// implementation of ISuppConnectorCallback

void SuppTunnel::onEvent(char *buf, size_t buflen)
{
    hidl_string evt;

    if (!buf || !buflen) {
        LOG(ERROR) << "onEvent invalid parameters";
        return;
    }

    evt = hidl_string(buf, buflen);
    // Invoke the |onEvent| method on all registered callbacks.
    callWithEachCallback(std::bind(
        &ISuppTunnelCallback::onEvent, std::placeholders::_1,
        evt));
}

// private methods follow

/**
 * Add callback to the corresponding list after linking to
 * death on the corresponding hidl object reference.
 */
template <class CallbackType>
int registerForDeathAndAddCallbackHidlObjectToList(
    const android::sp<CallbackType> &callback,
    const std::function<void(const android::sp<CallbackType> &)>
    &on_hidl_died_fctor,
    std::vector<android::sp<CallbackType>> &callback_list)
{
    // TODO register for HIDL object death notifications
    callback_list.push_back(callback);
    return 0;
}

int SuppTunnel::addCallback(const sp<ISuppTunnelCallback> &callback)
{
    // register for death notification before we add it to our list
    auto on_hidl_died_fctor = std::bind(
        &SuppTunnel::removeCallback, this, std::placeholders::_1);

    return registerForDeathAndAddCallbackHidlObjectToList<
        ISuppTunnelCallback>(callback, on_hidl_died_fctor, mCallbacks);
}

void SuppTunnel::removeCallback(const sp<ISuppTunnelCallback> &callback)
{
    mCallbacks.erase(
        std::remove(mCallbacks.begin(), mCallbacks.end(), callback),
        mCallbacks.end());
}

void SuppTunnel::callWithEachCallback(
    const std::function<Return<void>(sp<ISuppTunnelCallback>)> &method)
{
    for (const auto &callback : mCallbacks) {
        if (!method(callback).isOk()) {
            LOG(ERROR) << "Failed to invoke HIDL callback";
        }
    }
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace supptunnel
}  // namespace wigig
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
