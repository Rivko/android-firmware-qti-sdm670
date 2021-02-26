/*========================================================================
   This file has declarations of Afe static service data

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEStaticSvc.h#1 $
 ====================================================================== */

#ifndef _AFE_STATICSVC_H_
#define _AFE_STATICSVC_H_

/*==========================================================================
  the Include files
  ========================================================================== */
#include "AFEDeviceDriver.h"
#include "AFEDevService.h"
#include "AFEInternal.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteSvc.h"
#include "qurt_elite.h"
#include "AFESvcAprIf.h"
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"
#include "AFEPortManagement.h"
#include "AFEVfrHandler.h"
#include "AFEMmpm.h"

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
/**
 * AFE Static Service Thread Name
 */
static char AFE_STATIC_THREAD_NAME[]            =  "AfeS";


/**
 * Static declarations
 */

//Declarations for command Q, responseQ, AFE service object and zero buffer
QURT_ELITE_ALIGN(static char afe_cmdQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_SVC_MAX_CMD_Q_ELEMENTS)],8);
QURT_ELITE_ALIGN(static char afe_rspQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_SVC_MAX_RSP_Q_ELEMENTS)],8);

static aud_stat_afe_svc_t  afe_stat_svc_object_global;


#ifdef __AANC_FILE_IO__
FILE *fp_8k, *fp_client_input;
#endif



/**
 * global access to AFE Svc
 */
aud_stat_afe_svc_t            *afe_svc_global_ptr;

/*==========================================================================
  Function declarations
  ========================================================================== */
ADSPResult afe_svc_custom_msg_handler(void* pAudStatAfeSvc, elite_msg_any_t* pMsg);
ADSPResult afe_svc_apr_msg_handler (void* pAudStatAfeSvc, elite_msg_any_t* pMsg);
void afe_svc_ssr_callback_deregister();
void afe_svc_ssr_callback_register();

#endif /* AFESTATICSVC_H_ */
