/** 
  @file  Lpm.h
  @brief LPM events to UEFI drivers .
*/
/*=============================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/22/17   va      created

=============================================================================*/
#ifndef __LPM_H__
#define __LPM_H__

#define LPM_EVENT_EXT_DATA_VERSION 0x01
#define LPM_SIG  SIGNATURE_32 ('L', 'P', 'M', 'C')


/*! \enum LPM_STATE
 *  \LPM State
 */
typedef enum {

  LPM_EXIT,
  LPM_ENTRY,
  LPM_INVALID = 0x7FFFFFFF,
}LPM_STATE;

/*! \enum LPM_EVENT_EXT_DATA
 *  \LPM Extended data
 */
typedef struct {
  UINTN   Signature;
  UINT32   Version;
  EFI_GUID CalleeGuid; /* CalleeGuid for the intended driver, For eg. */
  LPM_STATE  LPMState;
}LPM_EVENT_EXT_DATA;


#endif /*__LPM_H__*/



