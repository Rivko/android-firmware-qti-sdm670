/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task 

General Description
  This file stubs the functions used for island buffering modes.

Copyright (c) 2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/diag_micro_buffering_stub.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/08/18   is      Stubs functions for island buffering mode
===========================================================================*/
#ifdef DIAG_UIMAGE_MODE

#include "comdef.h"            /* For byte and boolean */
#include "diag_defs.h"         /* For DIAG_TX_MODE_INVALID */
#include "msg_diag_service.h"
#include "msg_mask.h"
#include "msg.h"


 void micro_diagbuffer_drain_check( void )
 {
   return;
   
 } /* micro_diagbuffer_drain_check */
 
 
uint8 diag_set_uimage_tx_mode( uint8 mode )
{
   MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_uimage_tx_mode - feature not supported");
   return (uint8)DIAG_TX_MODE_NOT_SUPPORTED;
   
} /* diag_set_uimage_tx_mode */


uint8 diag_get_uimage_tx_mode( void )
{
  MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_get_uimage_tx_mode - feature not supported");
  return (uint8)DIAG_TX_MODE_NOT_SUPPORTED;

}  /* diag_get_uimage_tx_mode */


 boolean diag_set_uimage_tx_high_param_wm( uint8 high_per_value )
 {
   MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_uimage_tx_high_param_wm - feature not supported");
   return FALSE;
   
 } /* diag_set_uimage_tx_high_param_wm */
 
 
 uint8 diag_get_uimage_tx_high_param_wm( void )
 {
   MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_get_uimage_tx_high_param_wm - feature not supported");
   return 0;
   
 } /* diag_get_uimage_tx_high_param_wm */
  
#endif /* DIAG_UIMAGE_MODE */

