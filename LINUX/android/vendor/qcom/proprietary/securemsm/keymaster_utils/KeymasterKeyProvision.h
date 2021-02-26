/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef KEYMASTERKEYPROVISION_H_
#define KEYMASTERKEYPROVISION_H_

#include <cstdlib>
#include <map>
#include <vector>
#include <UniquePtr.h>

#include <hardware/keymaster0.h>
#include <hardware/keymaster1.h>
#include <hardware/keymaster2.h>

#include <KeymasterUtils.h>
#include <KeymasterBuffer.h>

using namespace keymasterutils;
using namespace std;

namespace keymasterdevice {

#define MAX_KEYS_IN_KEYBOX  2

typedef struct {
    keymaster_algorithm_t algo;
    keymaster_key_blob_t key;
    keymaster_cert_chain_t cert_chain;
} keybox_t;

typedef struct {
    keymaster_blob_t device_id;
    keybox_t keybox[MAX_KEYS_IN_KEYBOX];
} km_install_keybox_t;

class KeymasterKeyProvision {
 public:
    KeymasterKeyProvision() : utils(new KeymasterUtils) {}
    virtual ~KeymasterKeyProvision() {}

    int KeyMasterProvisionInit();

    int KeyMasterInstallKeybox(km_install_keybox_t *keybox);
    int KeyMasterProvisionDeviceId(string Brand, string Device, string Product,
            string SerialNum, string Manufacturer, string Model,string Imei, string Meid);
    int SetProvisioniningSuccess();
    int CleanProvisioningState();

    int GenerateAttkKeyPair(size_t copy_num);
    int VerifyAttkKeyPair();
    int ExportAttkKeyPair(uint8_t *PubKey, size_t *PubKeySize);
    int GetDeviceId(uint8_t *DeviceId, size_t *DeviceIdSize);
 private:

    int ProvisionDeviceId(const keymaster_key_param_set_t* in_params);
    keymaster_error_t params_serialize(UniquePtr<KeymasterBuffer> &buffer,
            keymaster_key_param_t* a, size_t params_count);
    UniquePtr<KeymasterUtils> utils;
};

}  // namespace keymasterdevice

#endif //KEYMASTERKEYPROVISION_H_
