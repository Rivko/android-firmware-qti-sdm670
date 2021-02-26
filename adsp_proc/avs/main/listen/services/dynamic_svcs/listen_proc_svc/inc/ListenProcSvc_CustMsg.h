/**
  @file ListenSvc_CustMsg.h
  @brief Listen processing service Custom messages header.
*/

/*==============================================================================
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/inc/ListenProcSvc_CustMsg.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  11/26/12    Sudhir   Initial version
==============================================================================*/
#ifndef ELITE_LISTEN_PROCSVC_CUSTMSG_H
#define ELITE_LISTEN_PROCSVC_CUSTMSG_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "Elite.h"
#include "adsp_lsm_api.h"

/*------------------------------------------------------------------------------
 * Global definitions/forward declarations
 *----------------------------------------------------------------------------*/
/**  ID for an internal Start command */
#define ELITEMSG_CUSTOM_LISTEN_START      0x00001000

/** @brief internal start command */
typedef elite_msg_custom_header_t elite_msg_custom_start_t;


/**  ID for an internal Stop command */
#define ELITEMSG_CUSTOM_LISTEN_STOP      0x00001001

/** @brief internal Stop command */
typedef elite_msg_custom_header_t elite_msg_custom_stop_t;

/**
 *ID for an internal EOB command*/
#define ELITEMSG_CUSTOM_LISTEN_EOB                         0x0000100A

/** @brief internal EOB command 
*/
typedef elite_msg_custom_header_t elite_msg_custom_eob_t;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_LISTEN_PROCSVC_CUSTMSG_H*/
