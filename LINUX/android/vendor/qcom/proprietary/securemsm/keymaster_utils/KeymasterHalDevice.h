/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef KEYMASTER_3_0_KEYMASTERHALDEVICE_H_
#define KEYMASTER_3_0_KEYMASTERHALDEVICE_H_

#include <map>
#include <vector>
#include <cstdlib>

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <dlfcn.h>
#include <UniquePtr.h>

#include <hardware/keymaster1.h>
#include <hardware/keymaster2.h>
#include <hardware/keymaster_defs.h>
//#include <km_main.h>

#include <KeymasterCryptoDevice.h>
#include <KeymasterBuffer.h>
#include <KeymasterUtils.h>

#define MSEC_TO_SEC(v) ((v) / 1000)

#define USE_NEW_APIS (1 << 0)

#define QTI_KM_SHARED_BUF_LEN (40 * 1024)
#define QTI_KM_INPUT_BUF_LEN (16 * 1024)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))
#define PROPERTY_GET_SLEEP_INTERVAL_USEC 100000

using namespace keymasterutils;

namespace keymasterdevice {

typedef struct _TagToString {
    keymaster_tag_t tag;
    char tagstring[100];
} TagToString_t;

struct CertificateChain_Delete {
    void operator()(keymaster_cert_chain_t *p) {
        if(!p)
            return;
        for(size_t i = 0; i < p->entry_count; i++) {
            if(p->entries[i].data)
                free((void *)p->entries[i].data);
        }
        delete[] p->entries;
        delete p;
    }
};

struct KeymasterParam_Delete {
    void operator()(keymaster_key_param_t *p) {
        if(!p)
            return;
        keymaster_free_param_values(p, 1);
    }
};

struct Keymaster2device_Delete {
    void operator()(keymaster2_device_t *p) {
        if(!p)
            return;
        delete p;
    }
};

struct KeyCharacteristics_Delete {
    void operator()(keymaster_key_characteristics_t *p) {
        if(!p)
            return;
        delete p;
    }
};

struct MallocDeleter {
    void operator()(uint8_t* p) { free(p); }
};

class KeymasterHalDevice {
    public:
        KeymasterHalDevice() : cryptoDevice(new KeymasterCryptoDevice),
                                   utils(new KeymasterUtils) {}
        virtual ~KeymasterHalDevice() {}

        uint32_t get_keymaster_version();
        keymaster_error_t configure(const keymaster_key_param_set_t* params);
        keymaster_error_t generate_key(const keymaster_key_param_set_t* params,
                keymaster_key_blob_t* key_blob, keymaster_key_characteristics_t* characteristics);
        keymaster_error_t get_key_characteristics(const keymaster_key_blob_t* key_blob,
                const keymaster_blob_t* client_id, const keymaster_blob_t* app_data,
                keymaster_key_characteristics_t* characteristics);
        keymaster_error_t import_key(const keymaster_key_param_set_t* params,
                keymaster_key_format_t key_format, const keymaster_blob_t* key_data,
                keymaster_key_blob_t* key_blob, keymaster_key_characteristics_t* characteristics);
        keymaster_error_t export_key(keymaster_key_format_t export_format,
                const keymaster_key_blob_t* key_to_export, const keymaster_blob_t* client_id,
                const keymaster_blob_t* app_data, keymaster_blob_t* export_data);
        keymaster_error_t attest_key(const keymaster_key_blob_t* key_to_attest,
                const keymaster_key_param_set_t* attest_params, keymaster_cert_chain_t* cert_chain);
        keymaster_error_t upgrade_key(const keymaster_key_blob_t* key_to_upgrade,
                const keymaster_key_param_set_t* upgrade_params, keymaster_key_blob_t* upgraded_key);
        keymaster_error_t begin_operation(keymaster_purpose_t purpose,
                const keymaster_key_blob_t* key, const keymaster_key_param_set_t* in_params,
                keymaster_key_param_set_t* out_params, keymaster_operation_handle_t* operation_handle);
        keymaster_error_t update_operation(keymaster_operation_handle_t operation_handle,
                const keymaster_key_param_set_t* in_params, const keymaster_blob_t* input,
                size_t* input_consumed, keymaster_key_param_set_t* out_params,
                keymaster_blob_t* output);
        keymaster_error_t finish_operation(keymaster_operation_handle_t operation_handle,
                const keymaster_key_param_set_t* in_params, const keymaster_blob_t* input,
                const keymaster_blob_t* signature, keymaster_key_param_set_t* out_params,
                keymaster_blob_t* output);
        keymaster_error_t abort_operation(keymaster_operation_handle_t operation_handle);
        keymaster_error_t delete_key(const keymaster_key_blob_t* key);
        keymaster_error_t delete_all_keys(void);
        keymaster_error_t destroy_device_ids(void);

    private:

        char* TagToString(keymaster_tag_t tag);
        void PrintParams(keymaster_key_param_t* params, size_t params_count);

        //keymaster_error_t getCharacteristics(qti_keyBlob_t *keyblob, keymaster_key_characteristics_t* characteristics);
        keymaster_error_t generate_key_common(const keymaster_key_param_set_t* params,
                keymaster_key_blob_t* key_blob);
        keymaster_error_t params_serialize(UniquePtr<KeymasterBuffer> &buffer,
                keymaster_key_param_t* a, size_t params_count);
        UniquePtr<KeymasterCryptoDevice> cryptoDevice;
        UniquePtr<KeymasterUtils> utils;
        std::map<keymaster_operation_handle_t, keymaster_purpose_t> kmmap;
    };

}

#endif /* KEYMASTER_3_0_KEYMASTERHALDEVICE_H_ */

