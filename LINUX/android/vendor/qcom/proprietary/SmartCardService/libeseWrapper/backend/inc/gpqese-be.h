/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __GPQESE_BE_H_
#define __GPQESE_BE_H_
#endif

#include <cstring>
#include <string>
#include <mutex>
#include <cstdlib>
#include <cstdint>
#include <vendor/qti/esepowermanager/1.0/IEsePowerManager.h>
#include <EsePowerManager.h>
#include <TEE_client_api.h>

#include <ese.h>
#include <utils/Log.h>

#define LOG_TAG "GPQeSE-BackEnd"

#define GPQESE_CMD_TRANSCEIVE 0x10000000
#define GPQESE_CMD_OPEN       0x10000001
#define GPQESE_CMD_CLOSE      0x10000002

#define GPQESE_MAX_CAPDU_SIZE 65546
#define GPQESE_MAX_RAPDU_SIZE 65539


static const char *teeName = NULL;
static const TEEC_UUID uuidEseGP =
{
  0x32552B22, 0x89FE, 0x42B4,
  { 0x8A, 0x45, 0xA0, 0xC4, 0xE2, 0xDB, 0x03, 0x26 }
};

static const char *kErrorMessages[] = {
    "0xFFFF0000 TEEC_ERROR_GENERIC",
    "0xFFFF0001 TEEC_ERROR_ACCESS_DENIED",
    "0xFFFF0002 TEEC_ERROR_CANCEL",
    "0xFFFF0003 TEEC_ERROR_ACCESS_CONFLICT",
    "0xFFFF0004 TEEC_ERROR_EXCESS_DATA",
    "0xFFFF0005 TEEC_ERROR_BAD_FORMAT",
    "0xFFFF0006 TEEC_ERROR_BAD_PARAMETERS",
    "0xFFFF0007 TEEC_ERROR_BAD_STATE",
    "0xFFFF0008 TEEC_ERROR_ITEM_NOT_FOUND",
    "0xFFFF0009 TEEC_ERROR_NOT_IMPLEMENTED",
    "0xFFFF000A TEEC_ERROR_NOT_SUPPORTED",
    "0xFFFF000B TEEC_ERROR_NO_DATA",
    "0xFFFF000C TEEC_ERROR_OUT_OF_MEMORY",
    "0xFFFF000D TEEC_ERROR_BUSY",
    "0xFFFF000E TEEC_ERROR_COMMUNICATION",
    "0xFFFF000F TEEC_ERROR_SECURITY",
    "0xFFFF0010 TEEC_ERROR_SHORT_BUFFER",
};

static uint32_t gpqese_transceive(struct EseInterface *ese, const struct EseSgBuffer *tx, uint32_t tx_segs,
                      struct EseSgBuffer *rx, uint32_t rx_segs);
