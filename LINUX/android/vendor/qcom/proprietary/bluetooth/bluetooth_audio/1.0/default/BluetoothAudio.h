/***************************************************************************
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************/
#ifndef COM_QUALCOMM_QTI_BLUETOOTH_AUDIO_V1_0_BLUETOOTHAUDIO_H
#define COM_QUALCOMM_QTI_BLUETOOTH_AUDIO_V1_0_BLUETOOTHAUDIO_H

#include <com/qualcomm/qti/bluetooth_audio/1.0/IBluetoothAudio.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace com {
namespace qualcomm {
namespace qti {
namespace bluetooth_audio {
namespace V1_0 {
namespace implementation {

using ::com::qualcomm::qti::bluetooth_audio::V1_0::IBluetoothAudio;
using ::com::qualcomm::qti::bluetooth_audio::V1_0::IBluetoothAudioCallbacks;
using ::com::qualcomm::qti::bluetooth_audio::V1_0::Status;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::hidl_death_recipient;
using ::android::wp;

using CodecCfg_cb  = std::function<void(Status status, hidl_vec<uint8_t>&cfg)>;

typedef void (*start_stream_request_t)(void);
typedef void (*suspend_stream_request_t)(void);
typedef void (*stop_stream_request_t)(void);
typedef void (*check_a2dp_ready_request_t)(void);
typedef void (*get_mcast_status_request_t)(void);
typedef void (*get_connected_devices_request_t)(void);
typedef void (*get_connect_status_request_t)(void);
typedef void (*get_codec_config_callback_t)(void);
typedef void (*get_sink_latency_request_t)(void);

typedef struct {
start_stream_request_t hidl_start_req_cb;
suspend_stream_request_t hidl_suspend_req_cb;
stop_stream_request_t hidl_stop_req_cb;
check_a2dp_ready_request_t hidl_check_a2dp_ready_req_cb;
get_mcast_status_request_t hidl_mcast_status_req_cb;
get_connected_devices_request_t hidl_get_connected_dev_req_cb;
get_connect_status_request_t hidl_get_connect_status_req_cb;
get_codec_config_callback_t hidl_get_codec_cfg_req_cb;
get_sink_latency_request_t hidl_get_sink_latency_req_cb;
}hidl_callback_t;

typedef void(*bt_stack_init_t)(hidl_callback_t *callbacks);
typedef void(*bt_stack_deinit_t)(Status status);
typedef void(*bt_stack_on_stream_started_t)(Status status);
typedef void(*bt_stack_on_stream_suspended_t)(Status status);
typedef void(*bt_stack_on_stream_stopped_t)(Status status);
typedef void(*bt_stack_on_get_codec_config_t)(
               Status status, const char *cfg, size_t len);
typedef void(*bt_stack_on_get_connected_devices_t)(uint8_t num_dev);
typedef void(*bt_stack_on_get_connect_status_t)(Status status);
typedef void(*bt_stack_on_check_a2dp_ready_t)(Status status);
typedef void(*bt_stack_on_get_mcast_status_t)(uint8_t value);
typedef void(*bt_stack_on_get_sink_latency_t)(uint16_t latency);
typedef struct {
 bt_stack_init_t bt_stack_init;
 bt_stack_deinit_t bt_stack_deinit;
 bt_stack_on_stream_started_t bt_stack_on_stream_started;
 bt_stack_on_stream_suspended_t bt_stack_on_stream_suspended;
 bt_stack_on_stream_stopped_t bt_stack_on_stream_stopped;
}bt_lib_stream_control_t;

using OnClientDead = std::function<void(void)>;
struct BluetoothDeathRecipient : hidl_death_recipient {
    BluetoothDeathRecipient(const OnClientDead& clientDead) {mOnClientDead = clientDead;}
    virtual void serviceDied(
        uint64_t /*cookie*/,
        const wp<::android::hidl::base::V1_0::IBase>& /*who*/) {
      mOnClientDead();
      has_died_ = true;
    }
    OnClientDead mOnClientDead;
    bool getHasDied() const { return has_died_; }
    void setHasDied(bool has_died) { has_died_ = has_died; }

   private:
    bool has_died_;
};

class BluetoothAudio : public IBluetoothAudio {
 public:
    // Methods from ::com::qualcomm::qti::bluetooth_audio::V1_0::IBluetoothAudio follow.
    BluetoothAudio();
    Return<void> initialize_callbacks(const sp<IBluetoothAudioCallbacks>& callback) override;
    Return<void> a2dp_on_started(Status status) override;
    Return<void> a2dp_on_suspended(Status status) override;
    Return<void> a2dp_on_stopped(Status status) override;
    Return<void> a2dp_on_get_codec_config(Status status, const hidl_vec<uint8_t>& codec_cfg) override;
    Return<void> a2dp_on_check_ready(Status status) override;
    Return<void> a2dp_on_get_multicast_status(uint8_t state) override;
    Return<void> a2dp_on_get_num_connected_devices(uint8_t num_dev) override;
    Return<void> a2dp_on_get_connection_status(Status status) override;
    Return<void> a2dp_on_get_sink_latency(uint16_t sink_latency) override;
    Return<void> deinitialize_callbacks() override;
    static void get_codec_config_callback();
    static void start_stream_request_callback();
    static void suspend_stream_request_callback();
    static void stop_stream_request_callback();
    static void check_a2dp_ready_request_callback();
    static void get_mcast_status_request_callback();
    static void get_connected_devices_request_callback();
    static void get_connect_status_request_callback();
    static void get_sink_latency_request_callback();
    void cleanup();
 private:
    ::android::sp<IBluetoothAudioCallbacks> stream_cb_;
    ::android::sp<BluetoothDeathRecipient> deathRecipient;
    std::function<void(sp<BluetoothDeathRecipient>&)> unlink_cb_;
    hidl_callback_t hidl_callback_;
    void *lib_handle;
    bt_lib_stream_control_t *bt_lib_control;
    bt_stack_init_t lib_init;
    bt_stack_deinit_t lib_deinit;
    bt_stack_on_stream_started_t on_started;
    bt_stack_on_stream_suspended_t on_suspended;
    bt_stack_on_stream_stopped_t on_stopped;
    bt_stack_on_get_codec_config_t on_get_codec_cfg;
    bt_stack_on_get_connected_devices_t on_get_connected_devices;
    bt_stack_on_get_connect_status_t on_get_connect_status;
    bt_stack_on_check_a2dp_ready_t on_check_a2dp_ready;
    bt_stack_on_get_mcast_status_t on_get_mcast_status;
    bt_stack_on_get_sink_latency_t on_get_sink_latency;
    int initialize_btlib(hidl_callback_t *cb);
    void deinitialize_btlib();
    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

extern "C" IBluetoothAudio* HIDL_FETCH_IBluetoothAudio(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth_audio
}  // namespace qti
}  // namespace qualcomm
}  // namespace com

#endif  // COM_QUALCOMM_QTI_BLUETOOTH_AUDIO_V1_0_BLUETOOTHAUDIO_H
