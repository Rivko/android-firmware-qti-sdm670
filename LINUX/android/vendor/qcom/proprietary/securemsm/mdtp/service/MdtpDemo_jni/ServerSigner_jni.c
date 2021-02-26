
/*
 * @file ServerSigner_jni.c
 * @brief
 * Implementation of JNI functions
 */

/*===========================================================================
  Copyright (c) 2015-2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
 ===========================================================================*/

/*===========================================================================

 EDIT HISTORY FOR FILE

 when         who      what, where, why
 ----------   ---      -------------------------------------------------------
 12/26/2016   ysreekan Removed Firmware lock related sources

 ===========================================================================*/

#define LOG_TAG "mdtp_signer_jni"

#include <jni.h>
#include <utils/Log.h>

#include "openssl/bio.h"
#include "openssl/err.h"
#include "openssl/pem.h"
#include "openssl/pkcs7.h"
#include "openssl/x509v3.h"

#include "mdtp_asn1.h"
#include "ServerSigner_jni.h"

static int getCert(mdtp_buf_t *cert_buf, X509 **cert) {
	BIO *cert_bio = NULL;
	int ret = SIGNER_FAIL;

	do {
		/* Create Cert BIO and fill it with data from the cert_buf */
		cert_bio = BIO_new_mem_buf((void*)cert_buf->data, cert_buf->len);
		if (!cert_bio) {
			ALOGD("%s: cert_bio is null", __func__);
			break;
		}

		*cert = PEM_read_bio_X509(cert_bio, NULL, NULL, NULL);
		if (!*cert) {
			ALOGD("%s: error parsing X509 certificate", __func__);
			break;
		}
		ret = SIGNER_OK;
	} while (0);

	if (cert_bio)
		BIO_free_all(cert_bio);
	return ret;
}

static int getPrivateKey(mdtp_buf_t *key_buf, EVP_PKEY **key) {
	BIO *key_bio = NULL;
	int ret = SIGNER_FAIL;

	do {
		/* Create key BIO and fill it with data from the key_buf */
		key_bio = BIO_new_mem_buf((void*)key_buf->data, key_buf->len);
		if (!key_bio) {
			ALOGD("%s: key_bio is null", __func__);
			break;
		}

		*key = PEM_read_bio_PrivateKey(key_bio, NULL, 0, NULL);
		if (!*key) {
			ALOGD("%s: error parsing PEM private key", __func__);
			break;
		}
		ret = SIGNER_OK;
	} while (0);

	if (key_bio)
		BIO_free_all(key_bio);
	return ret;
}

jbyteArray JNICALL Java_com_qualcomm_qti_securemsm_mdtp_ServerSigner_CMS_CMSBuilder_nativeCreate(
		JNIEnv *env, jobject obj, jbyteArray jRootCACert, jbyteArray jIsvCACert,
		jbyteArray jIsvCert, jbyteArray jIsvPrivateKey, jbyteArray jData) {

	BIO *data_bio = NULL;
	BIO *signedData_bio = NULL;
	EVP_PKEY *isvPrivateKey = NULL;
	PKCS7 *signedData = NULL;
	X509 *rootCACert = NULL;
	X509 *isvCACert = NULL;
	X509 *isvCert = NULL;

	mdtp_buf_t rootCACert_buf = {0,0};
	mdtp_buf_t isvCACert_buf = {0,0};
	mdtp_buf_t isvCert_buf = {0,0};
	mdtp_buf_t privateKey_buf = {0,0};
	mdtp_buf_t data_buf = {0,0};
	mdtp_buf_t signedData_buf = {0,0};

	jbyteArray jresponse = NULL;

	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	STACK_OF(X509) *addCerts = sk_X509_new_null();
	if (!addCerts) {
		ALOGD("sk_X509_new_null failed");
		return NULL;
	}

	if (!jRootCACert || !jIsvCACert || !jIsvCert || !jIsvPrivateKey || !jData) {
		ALOGD("Input parameter = null %p %p %p %p %p", jRootCACert,jIsvCACert,jIsvCert,jIsvPrivateKey,jData);
		sk_X509_pop_free(addCerts, X509_free);
		return NULL;
	}

	do {
		/* Load Root Cert */
		rootCACert_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jRootCACert, NULL);
		rootCACert_buf.len = (uint32_t)(*env)->GetArrayLength(env, jRootCACert);
		if (SIGNER_OK != getCert(&rootCACert_buf, &rootCACert)) {
			ALOGD("getCert rootCACert failed");
			break;
		}

		/* Load ISV CA Cert */
		isvCACert_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIsvCACert, NULL);
		isvCACert_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIsvCACert);
		if (SIGNER_OK != getCert(&isvCACert_buf, &isvCACert)) {
			ALOGD("getCert isvCACert failed");
			break;
		}

		/* Load ISV Cert */
		isvCert_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIsvCert, NULL);
		isvCert_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIsvCert);
		if (SIGNER_OK != getCert(&isvCert_buf, &isvCert)) {
			ALOGD("getCert isvCert failed");
			break;
		}

		/* Load ISV Private Key */
		privateKey_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIsvPrivateKey, NULL);
		privateKey_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIsvPrivateKey);
		if (SIGNER_OK != getPrivateKey(&privateKey_buf, &isvPrivateKey)) {
			ALOGD("getPrivateKey isvPrivateKey failed");
			break;
		}

		/* Load data to sign */
		data_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jData, NULL);
		data_buf.len = (uint32_t)(*env)->GetArrayLength(env, jData);

		ALOGD("data_buf.len= %d",data_buf.len);

		data_bio = BIO_new_mem_buf((void*)data_buf.data, data_buf.len);
		if (!data_bio) {
			ALOGD("BIO_new_mem_buf failed");
			break;
		}

		sk_X509_push(addCerts, rootCACert);
		sk_X509_push(addCerts, isvCACert);
		sk_X509_push(addCerts, isvCert);

		/* Sign using SHA256 */
		signedData = PKCS7_sign(NULL, NULL, addCerts, NULL, PKCS7_PARTIAL);
		if (!signedData) {
			ALOGD("PKCS7_sign() failed");
			break;
		}

		if (!PKCS7_add_signature(signedData, isvCert, isvPrivateKey, EVP_sha256())) {
			ALOGD("PKCS7_add_signature() failed");
			break;
		}

		if (SIGNER_OK != PKCS7_final(signedData, data_bio, PKCS7_BINARY)) {
			ALOGD("PKCS7_final() failed");
			break;
		}

		signedData_bio = BIO_new(BIO_s_mem());
		if (!i2d_PKCS7_bio(signedData_bio, signedData)) {
			ALOGD("i2d_PKCS7_bio() failed");
			break;
		}

		signedData_buf.len = BIO_get_mem_data(signedData_bio, &signedData_buf.data);
		if (!signedData_buf.len || !signedData_buf.data) {
			ALOGD("BIO_get_mem_data() failed");
			break;
		}

		jresponse = (*env)->NewByteArray(env, signedData_buf.len);
		(*env)->SetByteArrayRegion(env, jresponse, 0, signedData_buf.len, (jbyte*)signedData_buf.data);

	} while(0);

	if (!jresponse) {
		ALOGD("%s", ERR_error_string(ERR_get_error(), NULL));
	}

	/* Release openssl objects */
	sk_X509_pop_free(addCerts, X509_free);
	X509_free(rootCACert);
	X509_free(isvCACert);
	X509_free(isvCert);
	EVP_PKEY_free(isvPrivateKey);
	if (signedData_bio) {
		BIO_set_close(signedData_bio, BIO_CLOSE);
		BIO_free(signedData_bio);
	}

	/* Release JNI objects */
	if (!rootCACert_buf.data)
		(*env)->ReleaseByteArrayElements(env, jRootCACert, (jbyte*)rootCACert_buf.data, 0);

	if (isvCACert_buf.data)
		(*env)->ReleaseByteArrayElements(env, jIsvCACert, (jbyte*)isvCACert_buf.data, 0);

	if (isvCert_buf.data)
		(*env)->ReleaseByteArrayElements(env, jIsvCert, (jbyte*)isvCert_buf.data, 0);

	if (privateKey_buf.data)
		(*env)->ReleaseByteArrayElements(env, jIsvPrivateKey, (jbyte*)privateKey_buf.data, 0);

	return jresponse;
}

jbyteArray JNICALL Java_com_qualcomm_qti_securemsm_mdtp_ServerSigner_ASN1_MdtpMsgBuilder_nativeBuildSetStateMsg(
		JNIEnv *env, jobject obj, jboolean jActive, jboolean jSimLockSelect,
		jbyteArray jIdToken, jbyteArray jIsvPin, jbyteArray jIsvDeviceId, jbyteArray jIsvFriendlyName)  {

	mdtp_buf_t idToken_buf = {0,0};
	mdtp_buf_t isvPin_buf = {0,0};
	mdtp_buf_t isvDeviceId_buf = {0,0};
	mdtp_buf_t isvFriendlyName_buf = {0,0};
	mdtp_buf_t msg_buf = {0,0};
	int res;
	jbyteArray jresponse = NULL;

	if (!jIdToken) {
		ALOGD("Input parameter jIdToken = null");
		return NULL;
	}

	if (!jIsvPin && jActive) {
		ALOGD("Input parameter jIsvPin = null");
		return NULL;
	}

	if (!jIsvDeviceId && jActive) {
		ALOGD("Input parameter jIsvDeviceId = null");
		return NULL;
	}

	if (!jIsvFriendlyName && jActive) {
		ALOGD("Input parameter jIsvFriendlyName = null");
		return NULL;
	}

	idToken_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIdToken, NULL);
	idToken_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIdToken);

	isvPin_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIsvPin, NULL);
	isvPin_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIsvPin);

	isvDeviceId_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIsvDeviceId, NULL);
	isvDeviceId_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIsvDeviceId);

	isvFriendlyName_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIsvFriendlyName, NULL);
	isvFriendlyName_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIsvFriendlyName);

	if (SIGNER_OK == mdtp_build_set_state_msg(jActive, jSimLockSelect, &idToken_buf, &isvPin_buf, &isvDeviceId_buf, &isvFriendlyName_buf, &msg_buf)) {
		jresponse = (*env)->NewByteArray(env, msg_buf.len);
		(*env)->SetByteArrayRegion(env, jresponse, 0, msg_buf.len, (jbyte*)msg_buf.data);
		free(msg_buf.data);
	}

	(*env)->ReleaseByteArrayElements(env, jIdToken, (jbyte*)idToken_buf.data, 0);
	(*env)->ReleaseByteArrayElements(env, jIsvPin, (jbyte*)isvPin_buf.data, 0);
	(*env)->ReleaseByteArrayElements(env, jIsvDeviceId, (jbyte*)isvDeviceId_buf.data, 0);
	(*env)->ReleaseByteArrayElements(env, jIsvFriendlyName, (jbyte*)isvFriendlyName_buf.data, 0);

	return jresponse;
}

jbyteArray JNICALL Java_com_qualcomm_qti_securemsm_mdtp_ServerSigner_ASN1_MdtpMsgBuilder_nativeBuildSetLockMsg(
		JNIEnv * env, jobject obj, jboolean jLock, jboolean jSimLockSelect, jbyteArray jIdToken) {
	mdtp_buf_t idToken_buf = {0,0};
	mdtp_buf_t msg_buf = {0,0};
	int res;
	jbyteArray jresponse = NULL;

	if (!jIdToken) {
		ALOGD("Input parameter = null %p", jIdToken);
		return NULL;
	}

	idToken_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIdToken, NULL);
	idToken_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIdToken);

	if (SIGNER_OK == mdtp_build_set_lock_msg(jLock, jSimLockSelect, &idToken_buf, &msg_buf)) {
		jresponse = (*env)->NewByteArray(env, msg_buf.len);
		(*env)->SetByteArrayRegion(env, jresponse, 0, msg_buf.len, (jbyte*)msg_buf.data);
		free(msg_buf.data);
	}

	(*env)->ReleaseByteArrayElements(env, jIdToken, (jbyte*)idToken_buf.data, 0);
	return jresponse;
}

jbyteArray JNICALL Java_com_qualcomm_qti_securemsm_mdtp_ServerSigner_ASN1_MdtpMsgBuilder_nativeBuildSetKillMsg(
		JNIEnv *env, jobject obj, jboolean jKill, jboolean jSimSelect, jbyteArray jIdToken) {
	mdtp_buf_t idToken_buf = {0,0};
	mdtp_buf_t msg_buf = {0,0};
	int res;
	jbyteArray jresponse = NULL;

	if (!jIdToken) {
		ALOGD("Input parameter = null %p", jIdToken);
		return NULL;
	}

	idToken_buf.data = (uint8_t*)(*env)->GetByteArrayElements(env, jIdToken, NULL);
	idToken_buf.len = (uint32_t)(*env)->GetArrayLength(env, jIdToken);

	if (SIGNER_OK == mdtp_build_set_kill_msg(jKill, jSimSelect, &idToken_buf, &msg_buf)) {
		jresponse = (*env)->NewByteArray(env, msg_buf.len);
		(*env)->SetByteArrayRegion(env, jresponse, 0, msg_buf.len, (jbyte*)msg_buf.data);
		free(msg_buf.data);
	}

	(*env)->ReleaseByteArrayElements(env, jIdToken, (jbyte*)idToken_buf.data, 0);
	return jresponse;
}
