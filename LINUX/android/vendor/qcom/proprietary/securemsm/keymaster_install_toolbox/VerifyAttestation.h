/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2014 The Android Open Source Project
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


#ifndef KEYMASTER_INSTALL_TOOLBOX_VERIFYATTESTATION_H_
#define KEYMASTER_INSTALL_TOOLBOX_VERIFYATTESTATION_H_

#include <endian.h>
#include <unistd.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/cipher.h>

#include "attestation_record.h"
#include <keymaster_tags.h>
#include <KeymasterHalDevice.h>

//using ::testing::Test;
using std::ifstream;
using std::istreambuf_iterator;
using std::ofstream;
using std::string;
using std::unique_ptr;
using std::vector;
using namespace std;
using namespace keymasterdevice;
using namespace keymaster;



class VerifyAttestation {
public:
    VerifyAttestation(bool provision_device_id, string Brand, string Device, string Product,
            string SerialNum, string Manufacturer, string Model,
            string Imei, string Meid) : device(new KeymasterHalDevice()),
            provision_device_id(provision_device_id), Brand(Brand), Device(Device), Product(Product),
            SerialNum(SerialNum), Manufacturer(Manufacturer), Model(Model),
            Imei(Imei), Meid(Meid) {
        memset((void *)&blob_, 0, sizeof(keymaster_key_blob_t));
        memset((void *)&characteristics_, 0, sizeof(keymaster_key_characteristics_t));
    }
    virtual ~VerifyAttestation() {}

    keymaster_error_t RsaAttestation();
    keymaster_error_t EcAttestation();
private:
    X509* parse_cert_blob(const keymaster_blob_t& blob);
    bool verify_chain(const keymaster_cert_chain_t& chain);
    ASN1_OCTET_STRING* get_attestation_record(X509* certificate);
    bool verify_attestation_record(const string& challenge,
                                          AuthorizationSet expected_sw_enforced,
                                          AuthorizationSet expected_tee_enforced,
                                          uint32_t expected_keymaster_version,
                                          keymaster_security_level_t expected_keymaster_security_level,
                                          const keymaster_blob_t& attestation_cert);
    void FreeCharacteristics();
    void FreeKeyBlob();
    AuthorizationSet hw_enforced();
    AuthorizationSet sw_enforced();
    keymaster_error_t AttestKey(const string& attest_challenge,
                                          keymaster_cert_chain_t* cert_chain);
    keymaster_error_t GenerateKey(const AuthorizationSetBuilder& builder);
    keymaster_error_t Attestation();

    KeymasterHalDevice *device;
    keymaster_key_blob_t blob_;
    keymaster_key_characteristics_t characteristics_;
    bool provision_device_id;
    std::string Brand;
    std::string Device;
    std::string Product;
    std::string SerialNum;
    std::string Manufacturer;
    std::string Model;
    std::string Imei;
    std::string Meid;
};


#endif /* KEYMASTER_INSTALL_TOOLBOX_VERIFYATTESTATION_H_ */
