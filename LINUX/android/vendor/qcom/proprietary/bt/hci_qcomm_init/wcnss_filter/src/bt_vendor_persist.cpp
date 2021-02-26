/*
** Copyright (c) 2016 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
** Copyright (c) 2013, The Linux Foundation. All rights reserved.
*/

#include "bt_vendor_persist.h"

#ifdef BT_NV_SUPPORT
#include "bt_nv.h"
#include <utils/Log.h>

/*===========================================================================
FUNCTION   bt_vendor_nv_read

DESCRIPTION
 Helper Routine to process the nv read command

DEPENDENCIES
  NIL

RETURN VALUE
  RETURN VALUE
  FALSE = failure, else TRUE

SIDE EFFECTS
  None

===========================================================================*/
uint8_t bt_vendor_nv_read
(
  uint8_t nv_item,
  uint8_t * rsp_buf
)
{
  nv_persist_item_type my_nv_item;
  nv_persist_stat_enum_type cmd_result;
  boolean result = FALSE;

  switch(nv_item)
  {
    case NV_BD_ADDR_I:
      cmd_result = (nv_persist_stat_enum_type)bt_nv_cmd(NV_READ_F,  NV_BD_ADDR_I, &my_nv_item);
      ALOGI("CMD result: %d", cmd_result);
      if (NV_SUCCESS != cmd_result)
      {
        ALOGE("Failed to read BD_ADDR from NV");
        /* Send fail response */
        result = FALSE;
      }
      else
      {
        /* copy bytes */
        rsp_buf[0] = my_nv_item.bd_addr[0];
        rsp_buf[1] = my_nv_item.bd_addr[1];
        rsp_buf[2] = my_nv_item.bd_addr[2];
        rsp_buf[3] = my_nv_item.bd_addr[3];
        rsp_buf[4] = my_nv_item.bd_addr[4];
        rsp_buf[5] = my_nv_item.bd_addr[5];

        ALOGI("BD address read for NV_BD_ADDR_I: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
                (unsigned int) my_nv_item.bd_addr[0],(unsigned int) my_nv_item.bd_addr[1],
                (unsigned int) my_nv_item.bd_addr[2],(unsigned int) my_nv_item.bd_addr[3],
                (unsigned int) my_nv_item.bd_addr[4],(unsigned int) my_nv_item.bd_addr[5]);
        result = TRUE;
      }
      break;
  }
  return result;
}
#endif /* End of BT_NV_SUPPORT */
