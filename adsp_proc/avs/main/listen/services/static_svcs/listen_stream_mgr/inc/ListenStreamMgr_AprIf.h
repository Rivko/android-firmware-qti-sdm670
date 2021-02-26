/**
@file ListenStreamMgr_AprIf.h
@brief This file declares APR ISR functions of ListenStreamMgr.
*/


/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/inc/ListenStreamMgr_AprIf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/18/2012  Sudhir  Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef LISTEN_STREAM_MGR_APR_IF_H
#define LISTEN_STREAM_MGR_APR_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"
#include "Elite.h"
#include "EliteAprIf.h"
/* Listen */
#include "ListenStreamMgr.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/
static elite_apr_handle_t lsm_apr_handle = 0;

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------
inline functions
------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

/**
  Used only by the LSM to set the APR handle.

  The LSM obtains a handle from the APR when it registers with the APR. It
  uses this handle to either request or free an APR packet.

  @param[in] handle  APR handle of the LSM.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
static inline ADSPResult lsm_set_apr_handle(elite_apr_handle_t handle )
{
   if ( 0 != lsm_apr_handle ) 
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LSM: AprHandle Already Set 0x%lx, 0x%lx", (uint32_t) lsm_apr_handle, (uint32_t) handle );
      return ADSP_EFAILED;
   }
   lsm_apr_handle = handle;
   return ADSP_EOK;
}

/**
  Gets the APR handle that belongs to the LSM.

  @return
  None.

  @dependencies
  None.
*/
static inline elite_apr_handle_t lsm_get_apr_handle( )
{
   return lsm_apr_handle;
}

/**
  Callback function invoked by the APR when a packet arrives, and the packet's
  destination domain is aDSP and the service is LSM.

  Currently, this function is used by only the LSM.

  @param[in] packet         Pointer to the APR packet. The APR packet is
                            already copied from shared memory into a memory
                            pool managed by the APR, and it can be reused by
                            the LSM.
  @param[in] dispatch_data  Registered service-defined data pointer provided
                            to the callback function when that callback
                            function is invoked.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
int32_t lsm_apr_callback_function(elite_apr_packet_t* packet, void* dispatch_data);



/** @} */ /* end_addtogroup listen_stream */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef LISTEN_STREAM_MGR_APR_IF_H

