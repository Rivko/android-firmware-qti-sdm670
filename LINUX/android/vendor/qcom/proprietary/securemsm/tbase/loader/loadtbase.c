/**
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * t-base loader application.
 */

#include <stdlib.h>
#include <utils/Log.h>
#include "common_log.h"
#include "QSEEComAPI.h"

//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int32_t ret;            /* return value */
  struct QSEECom_handle *g_QSEEComHandle = NULL;

  LOGI("Starting tbaseLoader");

  do {

    if (2 == argc)
    {
      ret = QSEECom_load_external_elf(&g_QSEEComHandle,
                                    "/firmware/image",
                                    argv[1]);

      if (ret != 0) {
        ret = QSEECom_load_external_elf(&g_QSEEComHandle,
                                    "/system/etc/firmware",
                                    argv[1]);
      }
      if (ret == 0) {
        LOGI("Loading of TEE %s kernel succeeded.", argv[1]);
        break;
      }
    }

    ret = QSEECom_load_external_elf(&g_QSEEComHandle,
                                    "/firmware/image",
                                    "mc_v2");

    if (ret != 0) {
      ret = QSEECom_load_external_elf(&g_QSEEComHandle,
                                    "/system/etc/firmware",
                                    "mc_v2");
    }
    if (ret == 0) {
      LOGI("Loading of t-base v2 succeeded.");
      break;
    }

    ret = QSEECom_load_external_elf(&g_QSEEComHandle,
                                    "/firmware/image",
                                    "mobicore12");

    if (ret != 0) {
      ret = QSEECom_load_external_elf(&g_QSEEComHandle,
                                    "/system/etc/firmware",
                                    "mobicore12");
    }
    if (ret == 0) {
      LOGI("Loading of t-base v1.2 succeeded.");
      break;
    }

    ret = QSEECom_load_external_elf(&g_QSEEComHandle,
                                    "/firmware/image",
                                    "mobicore");

    if (ret == 0) {
      LOGI("Loading of MobiCore v1.0/v1.1 succeeded.");
      break;
    }
  } while (0);

  if (ret != 0) {
    LOGE("Failed to load external TEE Kernel.");
  }
  if (g_QSEEComHandle) {
    close((long)g_QSEEComHandle->ion_sbuffer);
    free(g_QSEEComHandle);
  }

  return ret;
}
