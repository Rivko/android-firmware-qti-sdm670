
/*===========================================================================
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/
#include "teetest.h"

#include "IEnv.h"
#include "CEnv_open.h"
#include "ISecureDisplay.h"
#include "CSecureDisplay.h"
#include "INSMem.h"
#include "CNSMem.h"

#define SHIFT_4K 12

#define TEE_TEST_SECDISP_TAG 0x11

typedef struct {
  uint32_t  cmd_id;
  uint64_t  secdisp_buf_pa;
  uint64_t  secdisp_buf_len;
} tee_test_secdisp_cmd_t;

static int tee_test_secdisp(tee_test_secdisp_cmd_t *sd_cmd)
{
  Object TEE_SD_Object, TEE_NSMem_Object;
  uint32_t sessionID = 0;
  uint32_t tagCnt = 0;

  TEST_ENTER;

  TEST_IOPEN(SecureDisplay, TEE_SD_Object);
  TEST_IOPEN(NSMem, TEE_NSMem_Object);

  /* PREVENT SD TO STOP */
  TEST_OK(ISecureDisplay_setStopAllowed(TEE_SD_Object, FALSE));

  /* GET THE SECDISP SESSION ID (0 when no SECDISP session active) */
  TEST_OK(ISecureDisplay_getSession(TEE_SD_Object, &sessionID));

  /* VERIFY THAT THE SECDISP SESSION HAS BEEN STARTED */
  TEST_TRUE((sessionID != 0));

  /* CHECK THAT THE BUFFER IS IN THE SECURE DISPLAY VM */
  TEST_OK(INSMem_isSecureTaggedRange(TEE_NSMem_Object, TEE_TEST_SECDISP_TAG, 
        sd_cmd->secdisp_buf_pa, sd_cmd->secdisp_buf_len));

  /* COUNT THE NUMBER OF 1MB CHUNKS THAT ARE IN THE SECURE DISPLAY VM */
  TEST_OK(INSMem_countMemUsage(TEE_NSMem_Object, TEE_TEST_SECDISP_TAG, &tagCnt));

  /* VERIFY THAT ONLY THE GIVEN BUFFER IS IN THE SECDISP VM */
  TEST_TRUE((tagCnt == (sd_cmd->secdisp_buf_len >> SHIFT_4K)));

  /* ALLOW SD TO STOP */
  TEST_OK(ISecureDisplay_setStopAllowed(TEE_SD_Object, TRUE));

  TEST_ICLOSE(NSMem, TEE_NSMem_Object);
  TEST_ICLOSE(SecureDisplay, TEE_SD_Object);

  TEST_EXIT;
}

int tee_test_secdisp_cmd_hdl(const void *req_ptr, size_t req_len)
{
  tee_test_secdisp_cmd_t *sd_cmd = NULL;

  if (req_len < sizeof(tee_test_secdisp_cmd_t))
      return Object_ERROR_SIZE_IN;

  sd_cmd = (tee_test_secdisp_cmd_t *)req_ptr;

  return tee_test_secdisp(sd_cmd);
}
