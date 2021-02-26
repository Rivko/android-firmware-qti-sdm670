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

#ifndef QTI_KEYMASTER_CRYPTO_H_
#define QTI_KEYMASTER_CRYPTO_H_

#include <stdlib.h>

#include <hardware/keymaster1.h>
#include <hardware/keymaster2.h>

#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/asn1.h>
#include <openssl/stack.h>
#include <UniquePtr.h>

namespace keymasterdevice {

typedef struct _keymaster_ec_sig_internal_t {
    uint8_t *sig_r;
    size_t sig_r_len;
    uint8_t *sig_s;
    size_t sig_s_len;
} keymaster_ec_sig_internal_t;

typedef struct _keymaster_export_internal_t {
    keymaster_algorithm_t alg;
    uint8_t *rsa_modulus;
    size_t rsa_modulus_size;
    uint8_t *rsa_public_exponent;
    size_t rsa_public_exponent_size;
    uint8_t *ec_public_x;
    size_t ec_public_x_size;
    uint8_t *ec_public_y;
    size_t ec_public_y_size;
} keymaster_export_internal_t;

typedef struct _keymaster_ec_import_internal_t {
    uint8_t *privkey;
    size_t privkey_size;
    uint8_t *pubkey_x;
    size_t pubkey_x_len;
    uint8_t *pubkey_y;
    size_t pubkey_y_len;
    uint32_t key_size;
} keymaster_ec_import_internal_t;

template<typename T, void (*FreeFunc)(T*)> struct OpenSslObjectDeleter {
    void operator()(T* p) {
        FreeFunc(p);
    }
};

#define DEFINE_OPENSSL_OBJECT_POINTER(name)                                                        \
    typedef OpenSslObjectDeleter<name, name##_free> name##_Delete;                                 \
    typedef UniquePtr<name, name##_Delete> name##_Ptr;

DEFINE_OPENSSL_OBJECT_POINTER(ASN1_INTEGER)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_OBJECT)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_OCTET_STRING)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_TIME)
DEFINE_OPENSSL_OBJECT_POINTER(BN_CTX);
DEFINE_OPENSSL_OBJECT_POINTER(EC_GROUP);
DEFINE_OPENSSL_OBJECT_POINTER(EC_KEY);
DEFINE_OPENSSL_OBJECT_POINTER(EC_POINT);
DEFINE_OPENSSL_OBJECT_POINTER(ENGINE);
DEFINE_OPENSSL_OBJECT_POINTER(EVP_PKEY);
DEFINE_OPENSSL_OBJECT_POINTER(PKCS8_PRIV_KEY_INFO);
DEFINE_OPENSSL_OBJECT_POINTER(RSA);
DEFINE_OPENSSL_OBJECT_POINTER(X509)
DEFINE_OPENSSL_OBJECT_POINTER(X509_EXTENSION)
DEFINE_OPENSSL_OBJECT_POINTER(X509_NAME)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_BIT_STRING)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_INTEGER)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_OBJECT)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_OCTET_STRING)
DEFINE_OPENSSL_OBJECT_POINTER(ASN1_TIME)

struct ECDSA_SIG_Delete {
    void operator()(ECDSA_SIG* p) const {
        ECDSA_SIG_free(p);
    }
};

struct BIO_Delete {
    void operator()(BIO* p) const {
        BIO_free(p);
    }
};

typedef UniquePtr<BIO, BIO_Delete> BIO_Ptr;

typedef OpenSslObjectDeleter<BIGNUM, BN_free> BIGNUM_Delete;
typedef UniquePtr<BIGNUM, BIGNUM_Delete> BIGNUM_Ptr;
typedef UniquePtr<EVP_PKEY, EVP_PKEY_Delete> Unique_EVP_PKEY;
typedef UniquePtr<RSA, RSA_Delete> Unique_RSA;
typedef UniquePtr<BIO, BIO_Delete> Unique_BIO;

/**
 * Many OpenSSL APIs take ownership of an argument on success but don't free the argument
 * on failure. This means we need to tell our scoped pointers when we've transferred ownership,
 * without triggering a warning by not using the result of release().
 */
#define OWNERSHIP_TRANSFERRED(obj) \
        typeof (obj.release()) _dummy __attribute__((unused)) = obj.release()

class KeymasterCryptoDevice {
    public:
        KeymasterCryptoDevice() {}
        virtual ~KeymasterCryptoDevice() {}
        /**
         * Converts PKCS8 blob to internal format required by keymaster TA.
         *
         * \param[in] import_alg The algorithm type of the keyblob to be imported
         *
         * \param[in] key_data The key data to be imported
         *
         * \param[out] ibuf Internal buffer required by keymaster TA
         *
         * Returns: 0 on success or an error code less than 0.
         */
        keymaster_error_t pkcs8_to_internal(keymaster_algorithm_t import_alg,
                const keymaster_blob_t *key_data, keymaster_ec_import_internal_t *ibuf);

        /**
         * Converts internal format to X.509 certificate
         * The X.509 cert only contains SubjectPublicKeyInfo, no other fields
         * are present.
         *
         * \param[in] export_key Internal buffer constains the data to be exported
         *
         * \param[out] export_data Exported key in X.509 format
         *
         * Returns: 0 on success or an error code less than 0.
         */
        keymaster_error_t internal_to_x509(keymaster_export_internal_t *export_key,
                keymaster_blob_t* export_data);

        /**
         * Converts EC ASN.1 DER encoded signature format to internal EC signature format
         *
         * \param[in] signature EC Signature in ASN.1 DER encoded format
         *
         * \param[out] ibuf Internal buffer contains the signature info
         *
         * Returns: 0 on success or an error code less than 0.
         */
        keymaster_error_t ecsig_to_internal(const keymaster_blob_t* signature,
                keymaster_ec_sig_internal_t *ibuf);

        /**
         * Converts internal EC signature format to X.509
         *
         * \param[in] ibuf Internal buffer contains the signature info
         *
         * \param[out] output Signature in X.509 format
         *
         * Returns: 0 on success or an error code less than 0.
         */
        keymaster_error_t internal_to_ecsig(keymaster_ec_sig_internal_t *ibuf,
                keymaster_blob_t* output);
};

}
#endif /* QTI_KEYMASTER_CRYPTO_H_ */

