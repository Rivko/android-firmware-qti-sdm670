/***************************************************************************
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************/
#include "BluetoothAudio.h"
#include <dlfcn.h>
#include <utils/Log.h>

namespace com {
namespace qualcomm {
namespace qti {
namespace bluetooth_audio {
namespace V1_0 {
namespace implementation {

BluetoothAudio *btAudio = nullptr;
BluetoothAudio::BluetoothAudio()
    : deathRecipient(new BluetoothDeathRecipient(
                 std::bind(&BluetoothAudio::cleanup, this))) {}
// Methods from ::com::qualcomm::qti::bluetooth_audio::V1_0::IBluetoothAudio follow.

Return<void> BluetoothAudio::initialize_callbacks(const sp<IBluetoothAudioCallbacks>& callback) {
    btAudio = new BluetoothAudio();
    btAudio->deathRecipient->setHasDied(false);
    callback->linkToDeath(btAudio->deathRecipient, 0);
    btAudio->stream_cb_ = callback;
    ALOGI("BluetoothAudio:initialize_callbacks");
    btAudio->lib_handle = NULL;
    btAudio->hidl_callback_.hidl_start_req_cb = btAudio->start_stream_request_callback;
    btAudio->hidl_callback_.hidl_suspend_req_cb = btAudio->suspend_stream_request_callback;
    btAudio->hidl_callback_.hidl_stop_req_cb = btAudio->stop_stream_request_callback;
    btAudio->hidl_callback_.hidl_check_a2dp_ready_req_cb = btAudio->check_a2dp_ready_request_callback;
    btAudio->hidl_callback_.hidl_mcast_status_req_cb = btAudio->get_mcast_status_request_callback;
    btAudio->hidl_callback_.hidl_get_connected_dev_req_cb = btAudio->get_connected_devices_request_callback;
    btAudio->hidl_callback_.hidl_get_connect_status_req_cb = btAudio->get_connect_status_request_callback;
    btAudio->hidl_callback_.hidl_get_codec_cfg_req_cb = btAudio->get_codec_config_callback;
    btAudio->hidl_callback_.hidl_get_sink_latency_req_cb = btAudio->get_sink_latency_request_callback;

    ALOGV("BluetoothAudio:calling init btlib");
    if (btAudio->initialize_btlib(&btAudio->hidl_callback_) == 1) {
        ALOGI("BluetoothAudio:init btlib loaded");
    }

    unlink_cb_ = [callback](sp<BluetoothDeathRecipient>& death_recipient) {
    if (death_recipient->getHasDied())
       ALOGI("Skipping unlink call, service died.");
    else
       callback->unlinkToDeath(death_recipient);
    };

    return Void();
}

Return<void> BluetoothAudio::a2dp_on_started(Status status) {
    ALOGI("BluetoothAudio:a2dp_on_started");
    if (btAudio != nullptr && btAudio->on_started) {
        ALOGV("BluetoothAudio:a2dp_on_started");
        btAudio->on_started(status);
    }
    return Void();
}

Return<void> BluetoothAudio::a2dp_on_suspended(Status status) {
    ALOGI("BluetoothAudio:a2dp_on_suspended");
    if (btAudio != nullptr && btAudio->on_suspended) {
        ALOGV("BluetoothAudio:a2dp_on_suspended");
        btAudio->on_suspended(status);
    }
    return Void();
}

Return<void> BluetoothAudio::a2dp_on_stopped(Status status) {
    ALOGI("BluetoothAudio:a2dp_on_stopped");
    if (btAudio != nullptr && btAudio->on_stopped) {
        ALOGV("BluetoothAudio:a2dp_on_stopped");
        btAudio->on_stopped(status);
    }
    return Void();
}

Return<void> BluetoothAudio::a2dp_on_get_codec_config(Status status, const hidl_vec<uint8_t>& codec_cfg) {
    Status ret = status;
    uint8_t length = 0;
    const char *p_buf = nullptr;
    if (status == Status::SUCCESS) {
        ALOGV("BluetoothAudio:codec cfg returned status = %hhu",status);
        p_buf = static_cast<char *>(malloc(codec_cfg.size()));
        LOG_ALWAYS_FATAL_IF(p_buf == NULL,
                        "malloc failed in get_codec_config_callback");
        if (p_buf == NULL)
          return Void();
        memcpy(const_cast<char *>(p_buf), codec_cfg.data(), codec_cfg.size());
        length = codec_cfg.size();
    }
    if (btAudio != nullptr && btAudio->on_get_codec_cfg) {
        ALOGV("BluetoothAudio:on_get_codec_cfg: len  = %d",length);
        btAudio->on_get_codec_cfg(ret, p_buf, length);
    }
    if (p_buf != nullptr)
      free(const_cast<char *>(p_buf));
    return Void();
}

Return<void> BluetoothAudio::a2dp_on_check_ready(Status status) {
    ALOGI("BluetoothAudio:a2dp_on_check_ready");
    if (btAudio != nullptr && btAudio->on_check_a2dp_ready)
        btAudio->on_check_a2dp_ready(status);
    return Void();
}

Return<void> BluetoothAudio::a2dp_on_get_multicast_status(uint8_t state) {
    ALOGI("BluetoothAudio:a2dp_on_get_multicast_status");
    uint8_t ret = state;
    if (btAudio != nullptr && btAudio->on_check_a2dp_ready)
        btAudio->on_get_mcast_status(ret);
    return Void();
}

Return<void> BluetoothAudio::a2dp_on_get_num_connected_devices(uint8_t num_dev) {
    ALOGI("BluetoothAudio:a2dp_on_get_num_connected_devices");
    uint8_t ret = num_dev;
    if (btAudio != nullptr && btAudio->on_get_connected_devices)
        btAudio->on_get_connected_devices(ret);
    return Void();
}

Return<void> BluetoothAudio::a2dp_on_get_connection_status(Status status) {
    ALOGI("BluetoothAudio:a2dp_on_get_connection_status");
    Status ret = status;
    if (btAudio != nullptr && btAudio->on_get_connect_status)
        btAudio->on_get_connect_status(ret);
    return Void();
}

Return<void> BluetoothAudio::a2dp_on_get_sink_latency(uint16_t sink_latency) {
    ALOGI("BluetoothAudio:a2dp_on_get_connection_status");
    uint16_t latency = sink_latency;
    if (btAudio != nullptr && btAudio->on_get_sink_latency)
        btAudio->on_get_sink_latency(latency);
    return Void();
}
Return<void> BluetoothAudio::deinitialize_callbacks() {
    ALOGI("BluetoothAudio:deinitialize_callbacks");
    unlink_cb_(btAudio->deathRecipient);
    deinitialize_btlib();
    if (btAudio != nullptr) {
        delete btAudio;
    }
    btAudio = nullptr;
    ALOGD("BluetoothAudio:deinitialize_callbacks: Exit");
    return Void();
}

void BluetoothAudio::cleanup() {
    ALOGI("BluetoothAudio::cleanup");
    btAudio->deathRecipient->setHasDied(true);
    if (btAudio != nullptr)
        deinitialize_btlib();
    else
       ALOGI("BluetoothAudio::cleanup already done");
    btAudio = nullptr;
}

int BluetoothAudio::initialize_btlib(hidl_callback_t *hidl_cb) {
    ALOGI("BluetoothAudio:initialize_btlib");
    btAudio->lib_handle = dlopen("libbthost_if.so", RTLD_NOW);
    if (!btAudio->lib_handle) {
        ALOGE("Failed to load ipc library");
        return 0;
    } else {
        btAudio->lib_init = (bt_stack_init_t)dlsym(lib_handle, "bt_stack_init");
        btAudio->lib_deinit = (bt_stack_deinit_t)dlsym(lib_handle, "bt_stack_deinit");
        btAudio->on_started = (bt_stack_on_stream_started_t)dlsym(lib_handle, "bt_stack_on_stream_started");
        btAudio->on_suspended = (bt_stack_on_stream_suspended_t)dlsym(lib_handle, "bt_stack_on_stream_suspended");
        btAudio->on_stopped = (bt_stack_on_stream_stopped_t)dlsym(lib_handle, "bt_stack_on_stream_stopped");
        btAudio->on_get_codec_cfg = (bt_stack_on_get_codec_config_t)dlsym(lib_handle,"bt_stack_on_get_codec_cfg");
        btAudio->on_get_connected_devices = (bt_stack_on_get_connected_devices_t)dlsym(lib_handle, "bt_stack_on_get_connected_devices");
        btAudio->on_get_connect_status = (bt_stack_on_get_connect_status_t)dlsym(lib_handle, "bt_stack_on_get_connection_status");
        btAudio->on_check_a2dp_ready = (bt_stack_on_check_a2dp_ready_t)dlsym(lib_handle, "bt_stack_on_check_a2dp_ready");
        btAudio->on_get_mcast_status = (bt_stack_on_get_mcast_status_t)dlsym(lib_handle, "bt_stack_on_get_mcast_status");
        btAudio->on_get_sink_latency = (bt_stack_on_get_sink_latency_t)dlsym(lib_handle, "bt_stack_on_get_sink_latency");

        if (btAudio->lib_init) {
            ALOGI("Initialize BT IPC lib with callbackes");
            //bt_lib_control->bt_stack_init(lib_cb);
            btAudio->lib_init(hidl_cb);
            return 1;
        } else {
            ALOGE("Failed to get symbols");
        }
    }
    return 0;
}

void BluetoothAudio::deinitialize_btlib() {
    ALOGI("BluetoothAudio::deinitialize_btlib");
    if (btAudio != nullptr && btAudio->lib_deinit) {
        btAudio->lib_deinit(Status::SUCCESS);
        dlclose(btAudio->lib_handle);
        btAudio->lib_handle = nullptr;
    }
}

void BluetoothAudio::get_codec_config_callback() {
    ALOGV("BluetoothAudio:get_codec_config_callback");
    if (btAudio != nullptr) {
        auto ret = btAudio->stream_cb_->a2dp_get_codec_config();
        if (!ret.isOk()) {
            ALOGE("get_codec_config_callback: client has died");
        }
    }
    return;
}
void BluetoothAudio::start_stream_request_callback() {
    ALOGW("BluetoothAudio:start_stream_request_callback");
    if (btAudio != nullptr && btAudio->on_started) {
        auto ret = btAudio->stream_cb_->a2dp_start_stream_req();
        if (!ret.isOk()) ALOGE("start_request_callback: client has died");
    }
    else
        ALOGE("BlueetoothAudio - either btAudio is nullptr or library not loaded");
    return;
}
void BluetoothAudio::suspend_stream_request_callback() {
    ALOGW("BluetoothAudio:suspend_stream_request_callback");
    if (btAudio != nullptr && btAudio->on_suspended) {
        auto ret = btAudio->stream_cb_->a2dp_suspend_stream_req();
        if (!ret.isOk()) ALOGE("suspend_request_callback: client has died");
    }
    else
        ALOGE("BluetoothAudio - either btAudio is nullptr or library not loaded");
    return;
}
void BluetoothAudio::stop_stream_request_callback() {
    ALOGW("BluetoothAudio:stop_stream_request_callback");
    if (btAudio != nullptr && btAudio->on_stopped) {
        auto ret = btAudio->stream_cb_->a2dp_stop_stream_req();
        if (!ret.isOk()) ALOGE("stop_request_callback: client has died");
    }
    else
        ALOGE("BlueetoothAudio - either btAudio is nullptr or library not loaded");
    return;
}
void BluetoothAudio::check_a2dp_ready_request_callback() {
    ALOGD("BluetoothAudio:check_a2dp_ready_callback");
    if (btAudio != nullptr && btAudio->on_check_a2dp_ready) {
        auto ret = btAudio->stream_cb_->a2dp_check_ready();
        if (!ret.isOk()) {
            ALOGE("check_a2dp_ready_callback: client has died");
        }
    }
    else
        ALOGE("BlueetoothAudio - either btAudio is nullptr or library not loaded");
    return;
}
void BluetoothAudio::get_mcast_status_request_callback() {
    ALOGV("BluetoothAudio:get_mcast_status_request_callback");
    if (btAudio != nullptr && btAudio->on_get_mcast_status) {
        auto ret = btAudio->stream_cb_->a2dp_get_multicast_status();
        if (!ret.isOk()) ALOGE("get_mcast_status_request_callback: client has died");
    }
    else
        ALOGE("BlueetoothAudio - either btAudio is nullptr or library not loaded");
    return;
}
void BluetoothAudio::get_connected_devices_request_callback() {
    ALOGW("BluetoothAudio:get_connected_devices_request_callback");
    if (btAudio != nullptr && btAudio->on_get_connected_devices) {
        auto ret = btAudio->stream_cb_->a2dp_get_num_connected_devices();
        if (!ret.isOk()) ALOGE("get_connected_devices_request_callback: client has died");
    }
    else
        ALOGE("BlueetoothAudio - either btAudio is nullptr or library not loaded");
    return;
}
void BluetoothAudio::get_connect_status_request_callback() {
    ALOGV("BluetoothAudio:get_connect_status_request_callback");
    if (btAudio != nullptr && btAudio->on_get_connect_status) {
        auto ret = btAudio->stream_cb_->a2dp_get_connection_status();
        if (!ret.isOk()) ALOGE("get_connect_status_request_callback: client has died");
    }
    else
        ALOGE("BlueetoothAudio - either btAudio is nullptr or library not loaded");
    return;
}

void BluetoothAudio::get_sink_latency_request_callback() {
    ALOGW("BluetoothAudio:get_ink_latency_request_callback");
    if (btAudio != nullptr && btAudio->on_get_sink_latency) {
        auto ret = btAudio->stream_cb_->a2dp_get_sink_latency();
        if (!ret.isOk()) ALOGE("get_sink_latency_callback: client has died");
    }
    else
        ALOGE("BlueetoothAudio - either btAudio is nullptr or library not loaded");
    return;
}
// Methods from ::android::hidl::base::V1_0::IBase follow.

IBluetoothAudio* HIDL_FETCH_IBluetoothAudio(const char* /* name */) {
    return new BluetoothAudio();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth_audio
}  // namespace qti
}  // namespace qualcomm
}  // namespace com
