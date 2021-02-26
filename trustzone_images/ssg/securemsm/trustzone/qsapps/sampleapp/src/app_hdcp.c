/*
  @file app_hdcp.c
  @brief Contains test code for hdcp TZ Kernel API.

*/
/*===========================================================================
  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/
#include "CHdcpEncryption.h"
#include "IHdcpEncryption.h"
#include "qsee_env.h"
#include "qsee_log.h"
#include <stdint.h>
#include "qsee_mmss_hdcp2p2.h"

int test_hdcp_write_key(Object *object)
{
    uint32_t ksXorLc128[4] = {0x6011478f, 0xf737e6c8, 0xa4f203af, 0xf34fdd16};
    uint32_t riv[2] = {0x9a6d1100, 0xa9b76f64};

    return IHdcpEncryption_enable(*object,
                                  IHdcpEncryption_HDCP_VERSION_22,
                                  IHdcpEncryption_HDCP_TXMTR_HDMI,
                                  (const void*)ksXorLc128,
                                  sizeof(ksXorLc128),
                                  (const void *)riv,
                                  sizeof(riv));
}

int test_hdcp_clear_key(Object *object)
{
    return IHdcpEncryption_disable(*object,
                                   IHdcpEncryption_HDCP_VERSION_22,
                                   IHdcpEncryption_HDCP_TXMTR_HDMI);
}

int run_hdcp_key_write_test(void)
{
   Object o;
   int32_t err;

   qsee_log(QSEE_LOG_MSG_DEBUG, "run_hdcp_key_write_test() getting hdcp handle");

   err = qsee_open(CHdcpEncryption_UID, &o);
   if (!Object_isOK(err)) {
      qsee_log(QSEE_LOG_MSG_ERROR, "run_hdcp_key_write_test() qsee_open() failed");
      o = Object_NULL;
      goto bail;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "run_hdcp_key_write_test() running tests");

   err = test_hdcp_write_key(&o);
   if (err == IHdcpEncryption_ERROR_HW_CLOCK_OFF) {
       qsee_log(QSEE_LOG_MSG_DEBUG, "run_hdcp_key_write_test() with display on!");
   } else if (!Object_isOK(err)) {
      qsee_log(QSEE_LOG_MSG_ERROR, "run_hdcp_key_write_test() test_hdcp_write_key() failed w/ %d", err);
      goto bail;
   }

   err = test_hdcp_clear_key(&o);
   if (err == IHdcpEncryption_ERROR_HW_CLOCK_OFF) {
       qsee_log(QSEE_LOG_MSG_DEBUG, "run_hdcp_key_write_test() with display on!");
   } else if (!Object_isOK(err)) {
      qsee_log(QSEE_LOG_MSG_ERROR, "run_hdcp_key_write_test() test_hdcp_clear_key() failed w/ %d", err);
      goto bail;
   }

   IHdcpEncryption_release(o);
   return 0;
bail:
   if (!Object_isNull(o)) {
      IHdcpEncryption_release(o);
   }
   return -1;
}
