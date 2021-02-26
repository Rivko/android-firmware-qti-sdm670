/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
SOTER Client app.
*********************************************************************/
/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/15   dy      Add soter client to support ATTK provision

===========================================================================*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <linux/msm_ion.h>
#include <utils/Log.h>
#include "common_log.h"
#include <sys/mman.h>
#include <getopt.h>
#include "comdef.h"
#include <dlfcn.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

// required for glibc compile
#include <limits.h>

#include <hardware/keymaster_defs.h>
#include <UniquePtr.h>
#include <KeymasterKeyProvision.h>

/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SOTER_CLIENT: "
#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#define LOG_NDDEBUG 0 //Define to enable LOGD
#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG  0 //Define to enable LOGV

#define PUBKEY_BUFF_SIZE               1024
#define DEVICE_ID_SIZE                 16
#define ATTK_COPY_NUM_DEFAULT          3
#define CMD_GENERATE_ATTK_KEY_PAIR     1
#define CMD_VERIFY_ATTK_KEY_PAIR       2
#define CMD_EXPORT_ATTK_PUBLIC_KEY     3
#define CMD_GET_DEVICE_ID              4

using namespace std;

namespace keymasterdevice {

typedef enum {
    ERROR_OK = 0,
    GENERIC_FAILURE = -2000,
    ATTESTATION_VERIFICATION_FAILURE = -2001,
    DEVICE_ID_NOT_FOUND = -2002,
}keybox_error;


static void qsc_usage(void)
{

    printf("*************************************************************\n");
    printf("************           SOTER CLIENT              ************\n");
    printf("*************************************************************\n");
    printf("\n"
        "Usage: soter_client [CMD]\n"
        "CMD can be:\n"
        " \t\t 1: Generate ATTK key pair\n"
        " \t\t 2: Verify ATTK key pair\n"
        " \t\t 3: Export ATTK public key \n"
        " \t\t 4: Get device ID \n"
        "---------------------------------------------------------\n\n\n");
}

int GetDeviceId(void)
{
    uint8_t device_id[DEVICE_ID_SIZE];
    size_t device_id_length = DEVICE_ID_SIZE;

    KeymasterKeyProvision keymaster_provision;

    int result = keymaster_provision.KeyMasterProvisionInit();
    if (result) {
        cerr << "KeyMasterProvisionInit error" << result << endl;
        return GENERIC_FAILURE;
    }

    int ret = keymaster_provision.GetDeviceId(device_id, &device_id_length);
    if (ret != KM_ERROR_OK) {
        LOGE("Failed to GetDeviceId, error %d", ret);
        printf("fail %d\n", ret);
    } else {
        for(size_t i=0; i < device_id_length; i++)
            printf("%02x",device_id[i]);
        printf("\n");
    }

ret_handle:
    return ret;
}

int VerifyAttkKeyPair(void)
{
    int ret = KM_ERROR_UNKNOWN_ERROR;
    uint8_t copy_num = ATTK_COPY_NUM_DEFAULT;

    KeymasterKeyProvision keymaster_provision;

    int result = keymaster_provision.KeyMasterProvisionInit();
    if (result) {
        cerr << "KeyMasterProvisionInit error" << result << endl;
        return GENERIC_FAILURE;
    }

    ret = keymaster_provision.VerifyAttkKeyPair();
    if (ret != KM_ERROR_OK) {
        LOGE("Failed to VerifyAttkKeyPair, error %d", ret);
        printf("fail %d\n", ret);
    } else
        printf("pass\n");

    return KM_ERROR_OK;
}

int ExportAttkKeyPair(void)
{
    unsigned char *pub_key_ptr = NULL;
    size_t PubKeyLen = PUBKEY_BUFF_SIZE;
    int ret = KM_ERROR_UNKNOWN_ERROR;

    KeymasterKeyProvision keymaster_provision;

    int result = keymaster_provision.KeyMasterProvisionInit();
    if (result) {
        cerr << "KeyMasterProvisionInit error" << result << endl;
        return GENERIC_FAILURE;
    }

    UniquePtr<uint8_t> PubKey(reinterpret_cast<uint8_t *>(malloc(PubKeyLen)));
    if (!PubKey.get()) {
        ALOGE("PubKey failed: %zu", PubKeyLen);
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }

    memset(PubKey.get(), 0, PubKeyLen);
    ret = keymaster_provision.ExportAttkKeyPair(PubKey.get(), &PubKeyLen);
    if (ret != KM_ERROR_OK) {
        LOGE("Failed to export attk, error %d", ret);
        printf("fail %d\n", ret);
    } else
        printf("%s", PubKey.get());

    return KM_ERROR_OK;
}

int GenerateAttkKeyPair(void)
{
    int ret = KM_ERROR_UNKNOWN_ERROR;
    uint8_t copy_num = ATTK_COPY_NUM_DEFAULT;

    KeymasterKeyProvision keymaster_provision;

    int result = keymaster_provision.KeyMasterProvisionInit();
    if (result) {
        cerr << "KeyMasterProvisionInit error" << result << endl;
        return GENERIC_FAILURE;
    }

    ret = keymaster_provision.GenerateAttkKeyPair(copy_num);
    if (ret != KM_ERROR_OK) {
        LOGE("Failed to generate attk, error %d", ret);
        printf("fail %d\n", ret);
    } else
        printf("pass\n");

    return KM_ERROR_OK;
}

int RunSoterCmd(char *argv[])
{
    int ret = KM_ERROR_UNKNOWN_ERROR;

    if(argv == NULL) {
        LOGE("No arguments to process, exiting!");
        return KM_ERROR_UNEXPECTED_NULL_POINTER;
    }
    /* CMD_ID */
    switch(atoi(argv[1])) {
    case CMD_GENERATE_ATTK_KEY_PAIR:
        ret = GenerateAttkKeyPair();
        break;
    case CMD_VERIFY_ATTK_KEY_PAIR:
        ret = VerifyAttkKeyPair();
        break;
    case CMD_EXPORT_ATTK_PUBLIC_KEY:
        ret = ExportAttkKeyPair();
        break;
    case CMD_GET_DEVICE_ID:
        ret = GetDeviceId();
        break;
    default:
        LOGD("command not supported.");
        qsc_usage();
        break;
    }

    return ret;
}

}

using namespace keymasterdevice;
int main(int argc, char **argv)
{
    int ret = KM_ERROR_UNKNOWN_ERROR;

    if( !argv || (argc != 2) || (!strcmp(argv[1],"h")))
        qsc_usage();
    else {
        ret = RunSoterCmd(argv);
        if (ret)
            LOGE("qsc_run_soter_cmd failed,ret=%d", ret);
    }
    return ret;
}


