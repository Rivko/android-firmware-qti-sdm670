/**
@file SecDbg_oem.h
* Copyright (c) 2010-2014 by Qualcomm, Technologies, Inc.  All Rights Reserved.

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/19/10     sree          Initial version
=============================================================================*/

#ifndef SECDBG_OEM_H
#define SECDBG_OEM_H

/**
 * @brief 
 * Check to skip serial number
 * @retval TRUE to skip serial number check, FALSE otherwise
 *
 */
boolean is_sec_dbg_skip_serial_number(void);

#endif
