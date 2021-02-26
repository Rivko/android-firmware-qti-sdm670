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

#include <VerifyAttestation.h>

static const char* StringifyTags(keymaster_tag_t tag) {
    switch (tag) {
    case KM_TAG_INVALID:
        return "KM_TAG_INVALID";
    case KM_TAG_PURPOSE:
        return "KM_TAG_PURPOSE";
    case KM_TAG_ALGORITHM:
        return "KM_TAG_ALGORITHM";
    case KM_TAG_KEY_SIZE:
        return "KM_TAG_KEY_SIZE";
    case KM_TAG_BLOCK_MODE:
        return "KM_TAG_BLOCK_MODE";
    case KM_TAG_DIGEST:
        return "KM_TAG_DIGEST";
    case KM_TAG_PADDING:
        return "KM_TAG_PADDING";
    case KM_TAG_CALLER_NONCE:
        return "KM_TAG_CALLER_NONCE";
    case KM_TAG_MIN_MAC_LENGTH:
        return "KM_TAG_MIN_MAC_LENGTH";
    case KM_TAG_RSA_PUBLIC_EXPONENT:
        return "KM_TAG_RSA_PUBLIC_EXPONENT";
    case KM_TAG_BLOB_USAGE_REQUIREMENTS:
        return "KM_TAG_BLOB_USAGE_REQUIREMENTS";
    case KM_TAG_BOOTLOADER_ONLY:
        return "KM_TAG_BOOTLOADER_ONLY";
    case KM_TAG_ACTIVE_DATETIME:
        return "KM_TAG_ACTIVE_DATETIME";
    case KM_TAG_ORIGINATION_EXPIRE_DATETIME:
        return "KM_TAG_ORIGINATION_EXPIRE_DATETIME";
    case KM_TAG_USAGE_EXPIRE_DATETIME:
        return "KM_TAG_USAGE_EXPIRE_DATETIME";
    case KM_TAG_MIN_SECONDS_BETWEEN_OPS:
        return "KM_TAG_MIN_SECONDS_BETWEEN_OPS";
    case KM_TAG_MAX_USES_PER_BOOT:
        return "KM_TAG_MAX_USES_PER_BOOT";
    case KM_TAG_ALL_USERS:
        return "KM_TAG_ALL_USERS";
    case KM_TAG_USER_ID:
        return "KM_TAG_USER_ID";
    case KM_TAG_USER_SECURE_ID:
        return "KM_TAG_USER_SECURE_ID";
    case KM_TAG_NO_AUTH_REQUIRED:
        return "KM_TAG_NO_AUTH_REQUIRED";
    case KM_TAG_USER_AUTH_TYPE:
        return "KM_TAG_USER_AUTH_TYPE";
    case KM_TAG_AUTH_TIMEOUT:
        return "KM_TAG_AUTH_TIMEOUT";
    case KM_TAG_ALL_APPLICATIONS:
        return "KM_TAG_ALL_APPLICATIONS";
    case KM_TAG_APPLICATION_ID:
        return "KM_TAG_APPLICATION_ID";
    case KM_TAG_APPLICATION_DATA:
        return "KM_TAG_APPLICATION_DATA";
    case KM_TAG_CREATION_DATETIME:
        return "KM_TAG_CREATION_DATETIME";
    case KM_TAG_ORIGIN:
        return "KM_TAG_ORIGIN";
    case KM_TAG_ROLLBACK_RESISTANT:
        return "KM_TAG_ROLLBACK_RESISTANT";
    case KM_TAG_ROOT_OF_TRUST:
        return "KM_TAG_ROOT_OF_TRUST";
    case KM_TAG_ASSOCIATED_DATA:
        return "KM_TAG_ASSOCIATED_DATA";
    case KM_TAG_NONCE:
        return "KM_TAG_NONCE";
    case KM_TAG_AUTH_TOKEN:
        return "KM_TAG_AUTH_TOKEN";
    case KM_TAG_MAC_LENGTH:
        return "KM_TAG_MAC_LENGTH";
    case KM_TAG_KDF:
        return "KM_TAG_KDF";
    case KM_TAG_EC_CURVE:
        return "KM_TAG_EC_CURVE";
    case KM_TAG_ECIES_SINGLE_HASH_MODE:
        return "KM_TAG_ECIES_SINGLE_HASH_MODE";
    case KM_TAG_OS_VERSION:
        return "KM_TAG_OS_VERSION";
    case KM_TAG_OS_PATCHLEVEL:
        return "KM_TAG_OS_PATCHLEVEL";
    case KM_TAG_EXPORTABLE:
        return "KM_TAG_EXPORTABLE";
    case KM_TAG_UNIQUE_ID:
        return "KM_TAG_UNIQUE_ID";
    case KM_TAG_INCLUDE_UNIQUE_ID:
        return "KM_TAG_INCLUDE_UNIQUE_ID";
    case KM_TAG_RESET_SINCE_ID_ROTATION:
        return "KM_TAG_RESET_SINCE_ID_ROTATION";
    case KM_TAG_ALLOW_WHILE_ON_BODY:
        return "KM_TAG_ALLOW_WHILE_ON_BODY";
    case KM_TAG_ATTESTATION_CHALLENGE:
        return "KM_TAG_ATTESTATION_CHALLENGE";
    case KM_TAG_ATTESTATION_APPLICATION_ID:
        return "KM_TAG_ATTESTATION_APPLICATION_ID";
    case KM_TAG_ATTESTATION_ID_BRAND:
        return "KM_TAG_ATTESTATION_ID_BRAND";
    case KM_TAG_ATTESTATION_ID_DEVICE:
        return "KM_TAG_ATTESTATION_ID_DEVICE";
    case KM_TAG_ATTESTATION_ID_PRODUCT:
        return "KM_TAG_ATTESTATION_ID_PRODUCT";
    case KM_TAG_ATTESTATION_ID_SERIAL:
        return "KM_TAG_ATTESTATION_ID_SERIAL";
    case KM_TAG_ATTESTATION_ID_IMEI:
        return "KM_TAG_ATTESTATION_ID_IMEI";
    case KM_TAG_ATTESTATION_ID_MEID:
        return "KM_TAG_ATTESTATION_ID_MEID";
    case KM_TAG_ATTESTATION_ID_MANUFACTURER:
        return "KM_TAG_ATTESTATION_ID_MANUFACTURER";
    case KM_TAG_ATTESTATION_ID_MODEL:
        return "KM_TAG_ATTESTATION_ID_MODEL";
    default:
        return "<Unknown>";
    }
    return "<Unknown>";
}

std::ostream& operator<<(std::ostream& os, const keymaster_key_param_t& param) {
    os << "Tag: " << StringifyTags(param.tag);
    switch (keymaster_tag_get_type(param.tag)) {
    case KM_INVALID:
        os << " Invalid";
        break;
    case KM_UINT_REP:
        os << " (Rep)";
    /* Falls through */
    case KM_UINT:
        os << " Int: " << param.integer;
        break;
    case KM_ENUM_REP:
        os << " (Rep)";
    /* Falls through */
    case KM_ENUM:
        os << " Enum: " << param.enumerated;
        break;
    case KM_ULONG_REP:
        os << " (Rep)";
    /* Falls through */
    case KM_ULONG:
        os << " Long: " << param.long_integer;
        break;
    case KM_DATE:
        os << " Date: " << param.date_time;
        break;
    case KM_BOOL:
        os << " Bool: " << param.boolean;
        break;
    case KM_BIGNUM:
        os << " Bignum: ";
        if (!param.blob.data)
            os << "(null)";
        else
            for (size_t i = 0; i < param.blob.data_length; ++i)
                os << std::hex << std::setw(2) << static_cast<int>(param.blob.data[i]) << std::dec;
        break;
    case KM_BYTES:
        os << " Bytes: ";
        if (!param.blob.data)
            os << "(null)";
        else
            for (size_t i = 0; i < param.blob.data_length; ++i)
                os << std::hex << std::setw(2) << static_cast<int>(param.blob.data[i]) << std::dec;
        break;
    }
    return os;
}

bool operator==(const keymaster_key_param_t& a, const keymaster_key_param_t& b) {
    if (a.tag != b.tag) {
        return false;
    }

    switch (keymaster_tag_get_type(a.tag)) {
    case KM_INVALID:
        return true;
    case KM_UINT_REP:
    case KM_UINT:
        return a.integer == b.integer;
    case KM_ENUM_REP:
    case KM_ENUM:
        return a.enumerated == b.enumerated;
    case KM_ULONG:
    case KM_ULONG_REP:
        return a.long_integer == b.long_integer;
    case KM_DATE:
        return a.date_time == b.date_time;
    case KM_BOOL:
        return a.boolean == b.boolean;
    case KM_BIGNUM:
    case KM_BYTES:
        if ((a.blob.data == NULL || b.blob.data == NULL) && a.blob.data != b.blob.data)
            return false;
        return a.blob.data_length == b.blob.data_length &&
               (memcmp(a.blob.data, b.blob.data, a.blob.data_length) == 0);
    }

    return false;
}

bool operator==(const AuthorizationSet& a, const AuthorizationSet& b) {
    if (a.size() != b.size())
        return false;

    for (size_t i = 0; i < a.size(); ++i)
        if (!(a[i] == b[i]))
            return false;
    return true;
}

bool operator!=(const AuthorizationSet& a, const AuthorizationSet& b) {
    return !(a == b);
}

std::ostream& operator<<(std::ostream& os, const AuthorizationSet& set) {
    if (set.size() == 0)
        os << "(Empty)" << std::endl;
    else {
        os << "\n";
        for (size_t i = 0; i < set.size(); ++i)
            os << set[i] << std::endl;
    }
    return os;
}

X509* VerifyAttestation::parse_cert_blob(const keymaster_blob_t& blob) {
    const uint8_t* p = blob.data;
    return d2i_X509(nullptr, &p, blob.data_length);
}

bool VerifyAttestation::verify_chain(const keymaster_cert_chain_t& chain) {
    for (size_t i = 0; i < chain.entry_count - 1; ++i) {
        keymaster_blob_t& key_cert_blob = chain.entries[i];
        if(i == 0) {
            std::string certtemp;
            certtemp.assign(reinterpret_cast<const char*>(key_cert_blob.data), key_cert_blob.data_length);
        } else if (i==1) {
            std::string certtemp;
            certtemp.assign(reinterpret_cast<const char*>(key_cert_blob.data), key_cert_blob.data_length);
        }
        keymaster_blob_t& signing_cert_blob = chain.entries[i + 1];

        X509_Ptr key_cert(parse_cert_blob(key_cert_blob));
        X509_Ptr signing_cert(parse_cert_blob(signing_cert_blob));
        if (!key_cert.get() || !signing_cert.get())
            return false;

        EVP_PKEY_Ptr signing_pubkey(X509_get_pubkey(signing_cert.get()));
        if (!signing_pubkey.get())
            return false;

        if(X509_verify(key_cert.get(), signing_pubkey.get()) != 1) {
            cerr << "Verification of certificate" << i << "failed" << endl;
            return false;
        }
    }

    keymaster_blob_t& key_cert_blob = chain.entries[2];
    std::string certtemp;
    certtemp.assign(reinterpret_cast<const char*>(key_cert_blob.data), key_cert_blob.data_length);
    return true;
}

// Extract attestation record from cert. Returned object is still part of cert; don't free it
// separately.
ASN1_OCTET_STRING* VerifyAttestation::get_attestation_record(X509* certificate) {
    ASN1_OBJECT_Ptr oid(OBJ_txt2obj(kAttestionRecordOid, 1 /* dotted string format */));
    if (!oid.get())
        return nullptr;

    int location = X509_get_ext_by_OBJ(certificate, oid.get(), -1 /* search from beginning */);
    if (location == -1)
        return nullptr;

    X509_EXTENSION* attest_rec_ext = X509_get_ext(certificate, location);
    if (!attest_rec_ext)
        return nullptr;

    ASN1_OCTET_STRING* attest_rec = X509_EXTENSION_get_data(attest_rec_ext);
    return attest_rec;
}

bool VerifyAttestation::verify_attestation_record(const string& challenge,
                                      AuthorizationSet expected_sw_enforced,
                                      AuthorizationSet expected_tee_enforced,
                                      uint32_t expected_keymaster_version,
                                      keymaster_security_level_t expected_keymaster_security_level,
                                      const keymaster_blob_t& attestation_cert) {

    X509_Ptr cert(parse_cert_blob(attestation_cert));
    if (!cert.get()) {
        cout << "parse_cert_blob failed" << endl;
        return false;
    }

    ASN1_OCTET_STRING* attest_rec = get_attestation_record(cert.get());
    if (!attest_rec) {
        cout << "get_attestation_record failed" << endl;
        return false;
    }

    AuthorizationSet att_sw_enforced;
    AuthorizationSet att_tee_enforced;
    uint32_t att_attestation_version;
    uint32_t att_keymaster_version;
    keymaster_security_level_t att_attestation_security_level;
    keymaster_security_level_t att_keymaster_security_level;
    keymaster_blob_t att_challenge = {};
    keymaster_blob_t att_unique_id = {};
    keymaster_error_t ret = parse_attestation_record(attest_rec->data, attest_rec->length, &att_attestation_version,
                               &att_attestation_security_level, &att_keymaster_version,
                               &att_keymaster_security_level, &att_challenge, &att_sw_enforced,
                               &att_tee_enforced, &att_unique_id);
    if (ret) {
        cout << "parse_attestation_record failed: " << ret << endl;
        return false;
    }

    if(att_attestation_version != 2) {
        cout << "att_attestation_version failed" << att_attestation_version << endl;
        return false;
    }

    if(att_attestation_security_level != KM_SECURITY_LEVEL_TRUSTED_ENVIRONMENT) {
        cout << "att_attestation_security_level failed" << att_attestation_security_level << endl;
        return false;
    }

    if(expected_keymaster_version != att_keymaster_version) {
        cout << "att_keymaster_version failed: " << att_attestation_version
                << "expected_keymaster_version: " << expected_keymaster_version << endl;
        return false;
    }

    if(expected_keymaster_security_level != att_keymaster_security_level) {
        cout << "att_keymaster_security_level failed: " << att_keymaster_security_level
                << "expected_keymaster_security_level: " << expected_keymaster_security_level << endl;
        return false;
    }

    if(challenge.length() != att_challenge.data_length) {
        cout << "att_challenge.data_length failed: " << att_challenge.data_length
                << "challenge.length(): " << challenge.length() << endl;
        return false;
    }

    if(memcmp(challenge.data(), att_challenge.data, challenge.length())) {
        cout << "Challenge comparison failed" << endl;
        return false;
    }

    string attestation_application_id("Key provisioning Verification");
    expected_sw_enforced.push_back(TAG_ATTESTATION_APPLICATION_ID, attestation_application_id.data(),
            attestation_application_id.length());

    keymaster_key_blob_usage_requirements_t usage;
    if (expected_tee_enforced.GetTagValue(TAG_BLOB_USAGE_REQUIREMENTS, &usage))
        att_tee_enforced.push_back(TAG_BLOB_USAGE_REQUIREMENTS, usage);

    // Add TAG_INCLUDE_UNIQUE_ID to the relevant attestation list, because that tag is not included
    // in the attestation.
    if (expected_tee_enforced.GetTagValue(TAG_INCLUDE_UNIQUE_ID))
        att_tee_enforced.push_back(TAG_INCLUDE_UNIQUE_ID);

    if(provision_device_id) {
        expected_tee_enforced.push_back(TAG_ATTESTATION_ID_BRAND, Brand.c_str(),
                Brand.size());
        expected_tee_enforced.push_back(TAG_ATTESTATION_ID_DEVICE, Device.c_str(),
                Device.size());
        expected_tee_enforced.push_back(TAG_ATTESTATION_ID_PRODUCT, Product.c_str(),
                Product.size());
        expected_tee_enforced.push_back(TAG_ATTESTATION_ID_SERIAL, SerialNum.c_str(),
                SerialNum.size());
        if(Imei.size())
            expected_tee_enforced.push_back(TAG_ATTESTATION_ID_IMEI, Imei.c_str(),
                Imei.size());
        if(Meid.size())
            expected_tee_enforced.push_back(TAG_ATTESTATION_ID_MEID, Meid.c_str(),
                Meid.size());
        expected_tee_enforced.push_back(TAG_ATTESTATION_ID_MANUFACTURER, Manufacturer.c_str(),
                    Manufacturer.size());
        expected_tee_enforced.push_back(TAG_ATTESTATION_ID_MODEL, Model.c_str(),
                    Model.size());
    }


    att_sw_enforced.Sort();
    expected_sw_enforced.Sort();
    if(expected_sw_enforced != att_sw_enforced) {
        cout << "att_sw_enforced" << endl;
        cout << att_sw_enforced << endl;
        cout << "expected_sw_enforced" << endl;
        cout << expected_sw_enforced << endl;
        cout << "SW enforced failed" << endl;
        return false;
    }

    att_tee_enforced.Sort();
    expected_tee_enforced.Sort();
    if(expected_tee_enforced != att_tee_enforced) {
        cout << "TEE enforced failed" << endl;
        cout << "att_tee_enforced" << endl;
        cout << att_tee_enforced << endl;
        cout << "expected_tee_enforced" << endl;
        cout << expected_tee_enforced << endl;
        return false;
    }

    return true;
}

void VerifyAttestation::FreeCharacteristics() {
    keymaster_free_characteristics(&characteristics_);
}

void VerifyAttestation::FreeKeyBlob() {
    if(blob_.key_material)
        free(const_cast<uint8_t*>(blob_.key_material));
    blob_.key_material = NULL;
}

AuthorizationSet VerifyAttestation::hw_enforced() {
    return AuthorizationSet(characteristics_.hw_enforced);
}

AuthorizationSet VerifyAttestation::sw_enforced() {
    return AuthorizationSet(characteristics_.sw_enforced);
}

keymaster_error_t VerifyAttestation::AttestKey(const string& attest_challenge,
                                            keymaster_cert_chain_t* cert_chain) {
    AuthorizationSet attest_params;

    attest_params.push_back(TAG_ATTESTATION_CHALLENGE, attest_challenge.data(),
                            attest_challenge.length());
    string attestation_application_id("Key provisioning Verification");
    attest_params.push_back(TAG_ATTESTATION_APPLICATION_ID, attestation_application_id.data(),
            attestation_application_id.length());

    if(provision_device_id) {
        attest_params.push_back(TAG_ATTESTATION_ID_BRAND, Brand.c_str(),
                Brand.size());
        attest_params.push_back(TAG_ATTESTATION_ID_DEVICE, Device.c_str(),
                Device.size());
        attest_params.push_back(TAG_ATTESTATION_ID_PRODUCT, Product.c_str(),
                Product.size());
        attest_params.push_back(TAG_ATTESTATION_ID_SERIAL, SerialNum.c_str(),
                SerialNum.size());
        if(Imei.size())
            attest_params.push_back(TAG_ATTESTATION_ID_IMEI, Imei.c_str(),
                Imei.size());
        if(Meid.size())
            attest_params.push_back(TAG_ATTESTATION_ID_MEID, Meid.c_str(),
                Meid.size());
        attest_params.push_back(TAG_ATTESTATION_ID_MANUFACTURER, Manufacturer.c_str(),
                Manufacturer.size());
        attest_params.push_back(TAG_ATTESTATION_ID_MODEL, Model.c_str(),
                Model.size());
    }

    return device->attest_key(&blob_, &attest_params, cert_chain);
}

keymaster_error_t VerifyAttestation::GenerateKey(const AuthorizationSetBuilder& builder) {
    AuthorizationSet params(builder.build());

    FreeKeyBlob();
    FreeCharacteristics();
    return device->generate_key(&params, &blob_, &characteristics_);
}

keymaster_error_t VerifyAttestation::Attestation() {
    keymaster_error_t ret = KM_ERROR_UNKNOWN_ERROR;
    keymaster_cert_chain_t cert_chain;
    ret = AttestKey("challenge", &cert_chain);
    if(ret)
        return ret;

    if((cert_chain.entry_count != 3) && (cert_chain.entry_count != 4)){
        cout << "entry_count failed" << cert_chain.entry_count << endl;
        return KM_ERROR_VERIFICATION_FAILED;
    }

    if(!verify_chain(cert_chain)) {
        cout << "verify_chain failed" << endl;
        return KM_ERROR_VERIFICATION_FAILED;
    }

    uint32_t expected_keymaster_version;
    keymaster_security_level_t expected_keymaster_security_level;

    expected_keymaster_version = 3;
    expected_keymaster_security_level = KM_SECURITY_LEVEL_TRUSTED_ENVIRONMENT;
    if(!verify_attestation_record(
        "challenge", sw_enforced(), hw_enforced(), expected_keymaster_version,
        expected_keymaster_security_level, cert_chain.entries[0])) {
        cout << "verify_attestation_record failed" << endl;
        return KM_ERROR_VERIFICATION_FAILED;
    }

    keymaster_free_cert_chain(&cert_chain);
    return KM_ERROR_OK;
}

keymaster_error_t VerifyAttestation::RsaAttestation() {
    keymaster_error_t ret = KM_ERROR_UNKNOWN_ERROR;

    ret = GenerateKey(AuthorizationSetBuilder()
                                           .RsaSigningKey(256, 3)
                                           .Digest(KM_DIGEST_NONE)
                                           .Padding(KM_PAD_NONE));
    if(ret) {
        cout << "RSA GenerateKey failed with ret" << ret << endl;
        return ret;
    }

    return Attestation();
}

keymaster_error_t VerifyAttestation::EcAttestation() {
    keymaster_error_t ret = KM_ERROR_UNKNOWN_ERROR;

    ret = GenerateKey(AuthorizationSetBuilder().EcdsaSigningKey(256).Digest(
                               KM_DIGEST_SHA_2_256).Authorization(TAG_INCLUDE_UNIQUE_ID));
    if(ret) {
        cout << "ECC GenerateKey failed with ret" << ret << endl;
        return ret;
    }

    return Attestation();
}



