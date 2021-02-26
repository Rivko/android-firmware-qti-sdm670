/*
** Copyright (c) 2016 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
** Copyright (c) 2013, The Linux Foundation. All rights reserved.
*/

#ifndef BT_VENDOR_PERSIST_H_
#define BT_VENDOR_PERSIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#ifdef BT_NV_SUPPORT

#endif /* BT_NV_SUPPORT */
typedef unsigned char boolean;
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
extern uint8_t bt_vendor_nv_read
(
  uint8_t nv_item,
  uint8_t * rsp_buf
);

#ifdef __cplusplus
}
#endif

#endif /* BT_VENDOR_PERSIST_H_ */

