/**
@file SecDbgHw.h
@brief Seecure debug policy specific definitions/routines

This file contains the target specific information for debug policy Provisioning

* Copyright (c) 2014-2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2014/04/14               Initial version
=============================================================================*/

#ifndef SEC_DBG_HW_H
#define SEC_DBG_HW_H

/* Check if the debug policy fuse bit is set */
boolean sec_dbg_is_disable_fuse_bit_set();

/* Set the override bits */
void sec_dbg_override_jtag();

#endif