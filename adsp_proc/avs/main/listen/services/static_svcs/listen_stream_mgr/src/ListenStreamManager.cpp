/**
@file ListenStreamMgr.cpp

@brief This file contains the implementation for ListenStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamManager.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/18/2012  Sudhir      Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

/*========================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "ListenStreamMgr.h"
#include "ListenStreamMgr_Type.h"
#include "ListenStreamMgr_SessionCmdHandler.h"
#include "ListenStreamMgr_Session.h"
#include "EliteMsg_Custom.h"
#include "ListenStreamMgr_AprIf.h"
#include "ListenStreamMgr_AprDataBase.h"
#include "audio_basic_op_ext.h"
#include "ListenProcSvc.h"
#include "aprv2_ids_services.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/


#define LISTEN_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q 32

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
/*LSM Static service  stack size*/
#define LSM_STAT_SVC_STACK_SIZE 2048

/*LSM system Command Q Mask*/
#define LSM_SVC_CMD_Q_MASK  0x00000001

/* public queue name*/
static char THREAD_NAME[]="LSM";

char LISTEN_STREAM_MGR_SYS_CMD_Q_NAME[] = "LSMsvcC" ;

/*qurt_elite MemoryMap Client*/
uint32_t lsm_memory_map_client;

/*declaration of LSM service object*/
lsm_t lsm_stat_svc_obj;

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
/* static service destroy function */
static void lsm_destroy (lsm_t* pMe);
/* Main work loop for service thread. Pulls msgs off of queues and processes them.*/
static int lsm_workloop(void* pInstance);
/* Process LSM svc CmdQ */
static ADSPResult  lsm_svc_process_cmd_q(lsm_t *pMe);
/*Handler for destroying static service */
static ADSPResult lsm_destroy_svc_handler(lsm_t *pMe) ;
/*Handler for starting static service */
static ADSPResult lsm_start_svc_handler(lsm_t *pMe) ;


/* -----------------------------------------------------------------------
 ** Static variables
 ** ----------------------------------------------------------------------- */

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/**
 * This function returns the LSM service version
 *
 * @param[out] svc_info_ptr pointer to the service info structure
 * @return error code
 */
ADSPResult lsm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  svc_info_ptr->service_id = APRV2_IDS_SERVICE_ID_ADSP_LSM_V;
  svc_info_ptr->api_version = LSM_API_VERSION;
  svc_info_ptr->api_branch_version = LSM_API_BRANCH_VERSION;

  return ADSP_EOK;
}

/* =======================================================================
 **         ListenStreamMgr Instance Create/Destroy/Workloop
 ** ======================================================================= */

ADSPResult aud_create_stat_lsm_svc (uint32_t dummy,void **handle)
{
  ADSPResult result = ADSP_EOK;
  qurt_elite_queue_t  *pQTemp;

  memset(&lsm_stat_svc_obj,0,sizeof(lsm_t));

  lsm_t *pMe = (lsm_t*)&lsm_stat_svc_obj;

  /* Create global cmd queue*/
  pQTemp = (qurt_elite_queue_t *)pMe->lsmCmdQBuf;
  if (ADSP_FAILED(result = qurt_elite_queue_init(LISTEN_STREAM_MGR_SYS_CMD_Q_NAME,
                                                 LISTEN_STREAM_MGR_MAX_SYS_CMD_Q_ELEMENTS, pQTemp)))
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to init LSM Svc message queues!");
#endif
    lsm_destroy(pMe);
    return result;
  }
  pMe->serviceHandle.cmdQ = pQTemp;

  /* set up channel*/
  qurt_elite_channel_init(&pMe->channel);

  /*add svc command queue*/
  if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->serviceHandle.cmdQ, LSM_SVC_CMD_Q_MASK)))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LSM: fail to add Q to channel::%d!", result);
#endif
    return(result);
  }
  // populate me
  pMe->serviceHandle.unSvcId = ELITE_STATLSM_SVCID;

  /*Create all session states here*/
  lsm_create_all_session_states(pMe);

  // Once session is created, initialize data base
  if ( ADSP_FAILED( result = lsm_apr_router_init(pMe)) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LSM: fail to init router result = %d!", result);
#endif
    return(result);
  }

  // Launch the thread
  if (ADSP_FAILED(result = qurt_elite_thread_launch(&(pMe->serviceHandle.threadId), THREAD_NAME, NULL,
                                                    LSM_STAT_SVC_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_STAT_LSM_SVC_PRIO_ID), lsm_workloop, (void*)pMe,
                                                    QURT_ELITE_HEAP_DEFAULT)))
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create LSM Thread!");
#endif
    lsm_destroy(pMe);
    return result;
  }

  *handle = &(pMe->serviceHandle);
  /* register with qurt_elite memory map.*/
  qurt_elite_memorymap_register(&lsm_memory_map_client);

  MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "LSM Created");

  return ADSP_EOK;
}


/*
  Destroys the instance of the LSM
  @param[in]  pMe  The instance to destroy
  @return
  None
  @dependencies
  None.
 */
void lsm_destroy(lsm_t *pMe)
{
  qurt_elite_memorymap_unregister(lsm_memory_map_client);
  for ( int i = 0; i < LSM_MAX_SESSION_ID; i++ )
  {
    lsm_session_destroy_state(pMe, i);
  }
  elite_svc_deinit_cmd_queue(pMe->serviceHandle.cmdQ);
  qurt_elite_channel_destroy(&pMe->channel);
}


/* This function is the main work loop for the service. Commands from Apps
  are handled with the highest priority,
  service.
 */
static int lsm_workloop(void* pInstance)
{
  lsm_t *pMe = (lsm_t*)pInstance;       // instance structure
  ADSPResult result=ADSP_EOK;
  /* set up mask for listening to the cmd queues.*/
  /* Currently it listens only on CmdQ*/
  pMe->curr_bit_field = qurt_elite_queue_get_channel_bit(pMe->serviceHandle.cmdQ) ;
  for(;;)
  {
    /* block on cmdQ */
    pMe->channel_status = qurt_elite_channel_wait(&(pMe->channel), pMe->curr_bit_field);

    if ((pMe->channel_status) & (qurt_elite_queue_get_channel_bit(pMe->serviceHandle.cmdQ))) 
    {
      result = lsm_svc_process_cmd_q(pMe);
    }
    /* If Service is destroyed, return immediately */
    /* This will come during static svc destroy */
    if(result == ADSP_ETERMINATED)
    {
      return ADSP_EOK;
    }
  } // forever loop

  return result;
}

/*
  This function proceeses the commands pushed on the LSM CmdQ
  @param[in]  pMe  The instance to LSM service
  @return
  ADSP_EOK/ADSP_EFAILED
  @dependencies
  None.
 */

static ADSPResult lsm_svc_process_cmd_q(lsm_t *pMe)
{
  ADSPResult result;
  uint32_t opcode;

  result = qurt_elite_queue_pop_front(pMe->serviceHandle.cmdQ, (uint64_t*)&(pMe->msg));
  opcode = pMe->msg.unOpCode;
  //process opcode
  switch (opcode)
  {
    case  ELITE_CMD_START_SERVICE:
    {
      result = lsm_start_svc_handler(pMe);
      break;
    }
    case ELITE_CMD_DESTROY_SERVICE:
    {
      result = lsm_destroy_svc_handler(pMe);
      break;
    }
    case ELITE_APR_PACKET:
    {
      result = lsm_svc_and_session_apr_pkt_handler(pMe);
      break;
    }
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"LSM: unsupported opcode 0x%x", opcode);
      /*print error*/
      lsm_print_error_msg(&pMe->msg);
      /*return payload*/
      elite_msg_finish_msg( & (pMe->msg), ADSP_EUNSUPPORTED);
      break;
    }
  }
  return result;
}

/* Message Handler to ELITE_CMD_START_SERVICE in sys Q */
static ADSPResult  lsm_start_svc_handler(lsm_t *pMe)
{
  ADSPResult result;
  /****************************************************************
   ** Register ISR call back function with APR
   ******************************************************************/
  elite_apr_handle_t handle = 0;
  char aLsmName[] = "qcom.audio.lsm";
  uint32_t ulLsmNameSize = strlen( aLsmName);
  uint16_t usRetAddr = 0;
  if (ADSP_FAILED( result = elite_apr_if_register_by_name( &handle, &usRetAddr, aLsmName, ulLsmNameSize, &lsm_apr_callback_function,NULL)))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Fail to register with LSM  0x%8x", result );
#endif
    return result;
  }

  if (ADSP_FAILED( result = lsm_set_apr_handle( handle) ))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "LSM: Fail to set handle 0x%8x", result);
#endif
  }

  elite_msg_finish_msg( & (pMe->msg), ADSP_EOK);

  MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "LSM SVC started");

  return result;
}

/* Message handler ELITE_CMD_DESTROY_SERVICE */
static ADSPResult lsm_destroy_svc_handler(lsm_t *pMe)
{
  elite_msg_any_t msg = pMe->msg;

  lsm_destroy(pMe);

  elite_msg_finish_msg( &msg, ADSP_EOK );
  /* send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked. */
  return ADSP_ETERMINATED;
}



