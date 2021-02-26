#ifndef __RPMBLISTENER_H__
#define __RPMBLISTENER_H__

/** @file RpmbListener.h

 * This file provides header file for eMMC RPMB partition Listener interface
 * on the non-secure side
 *
 * Copyright (c) 2012-2013, 2015, 2017 Qualcomm Technologies, Inc. 
 * All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     ---------------------------------------------------------
  2017-07-14   jt      Make listener size 4K aligned 
  2015-04-17   jt/rm   Add support for ufs rpmb
  2015-01-28   rm      Add RPMBListenerAllocMem
  2013-04-05   jt      Adjust listener size 
  2012-11-02   bb      Initial Version

 =============================================================================*/

#ifndef RPMB_LISTENER_BUFFER_MAX
  #define RPMB_LISTENER_BUFFER_MAX  (140*1024) // 140K
#endif

/* This function allocates memory for the RPMB Listener */
EFI_STATUS EFIAPI RPMBListenerAllocMem(void);

/* Initializes the RPMB Listener */
EFI_STATUS EFIAPI RPMBListenerInit(UINT32 MediaId);

#endif /* __RPMBLISTENER_H_ */
