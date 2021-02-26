
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diag Legacy Service Mapping Test Code

GENERAL DESCRIPTION

Contains test code for Legacy Service Mapping layer for Diagnostic.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2018 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/Diag_LSM_test.c#1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
01/08/18   is    Created

===========================================================================*/

#include "comdef.h"
#include "Diag_LSMi.h"
#include "Diag_LSM.h"
#include "diagpkt.h"
#include "diag_v.h"
#include "diagdiag_v.h"	
#include "Diag_LSM_Qdi_wrapper.h"         /* For diag_lsm_clear_stress_test_q */
#include "msg_diag_service.h"
#include "Diag_LSM_Pd.h"
#include "diagbuf_mpd.h"
#include "diag_param.h"

#ifdef DIAG_UIMAGE_MODE
#include "diag_uimage.h"                  /* For diag_set_uimage_tx_mode() */
#include "micro_diagbuf_mpd_type.h"
#endif

#include "micro_diagbuffer.h"
#include "micro_diagbuffer_i.h"           /* For MICROULOG_DIAG_FORMAT_ASCII */
#include "micro_diagbuffer_client_read.h" /* For micro_diagbuffer_ReadAndFormat_vaddr */
#include "micro_ULog.h"

#include "osal.h"
#include "stdlib.h"
#include "assert.h"
#include "qw.h"
#include "npa.h"
#include "stringl.h"                      /* For memscpy */
#include "stdio.h"


/******************* Local variables **************/
/* Stores info, wheather Q is initialised or not */
boolean diag_stress_test_task_q_init = FALSE;

/* Predetermined sequence for stress test*/
byte pseudo_array[] = { 6, 2, 4, 5, 3, 0, 1, 2, 5, 0, 3, 1, 6, 4 };

/*Queue with stress test task*/
q_type diag_stress_test_task_q;
osal_thread_attr_t diagdiag_stress_attr;

#define DIAG_STRESS_LOG_SIZE MICRO_DIAGBUFFER_MEM_NEEDED(1024)

char diag_stress_micro_diagbuffer_buf[DIAG_STRESS_LOG_SIZE];
micro_diagbuffer_handle diag_stress_micro_diagbuffer_hdl = NULL;

/******************* Externs **************/

#ifdef DIAG_UIMAGE_MODE
extern micro_diagbuf_mpd_type *micro_diagmpd_buf;
#endif

extern diag_pd_type diag_lsm_cur_userpd;

/******************* Internal function declarations **************/
PACKED void * stress_test_task (
  PACKED void *req_pkt,
    uint16 pkt_len,
    uint16 code
);

void Diag_LSM_stresstest(
   uint32 params
);


PACKED void* Diag_LSM_Health_Command( PACKED void *req_pkt, uint16 pkt_len, uint16 code);
PACKED void* diag_lsm_loopback( PACKED void *req_pkt, uint16 pkt_len, uint16 code );

#ifdef DIAG_UIMAGE_MODE
PACKED void* diag_uimage_set_buf_mode_handler( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_uimage_get_buf_mode_handler( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_uimage_set_high_wm_handler( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_uimage_get_high_wm_handler( PACKED void *req_pkt, uint16 pkt_len );
#endif /* DIAG_UIMAGE_MODE */
      
PACKED void * diag_LSM_test_cmd_handler (
  PACKED void *req_pkt,
  uint16 pkt_len
);
PACKED void* diag_pd_buf_mode_change( PACKED void *req_pkt, uint16 pkt_len, uint16 code);

PACKED void* Diag_LSM_Delay_Test(PACKED void *req_pkt, uint16 pkt_len);

PACKED void* diag_lsm_subsys_reg_dereg_test( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_lsm_subsys_reg_test( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_lsm_subsys_reg_test2( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_lsm_subsys_reg_test3( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_lsm_subsys_reg_test4( PACKED void *req_pkt, uint16 pkt_len );


	  

/*===========================================================================

FUNCTION DIAG_LSM_TEST_CMD_HANDLER

DESCRIPTION
  This procedure handles the test command for stress test and health statistic
  commands

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void* diag_LSM_test_cmd_handler(
   PACKED void *req_pkt,
   uint16 pkt_len
   )
{
  uint16 code;
  diag_pd_type user_pd = diag_lsm_cur_userpd;
  diag_LSM_test_cmd_rsp_type *rsp = NULL;
  diag_LSM_test_cmd_req_type *req = (diag_LSM_test_cmd_req_type *)req_pkt;

  if (req_pkt == NULL)
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  if (user_pd < DIAG_PD_MIN || user_pd > DIAG_PD_MAX)
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Delay_Test: Didn't get userPD %d, req_pkt 0x%08X",user_pd, req_pkt);
    return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  code = diagpkt_subsys_get_cmd_code (req);

  switch (req->opcode)
  {
     case DIAGDIAG_STRESS_TEST_OPCODE:
       rsp = stress_test_task(req_pkt, pkt_len, code);
       return ((void *)rsp);
       break;
     case DIAGDIAG_HEALTH_CMD_OPCODE:
       rsp = Diag_LSM_Health_Command(req_pkt, pkt_len, code);
       return ((void *)rsp);
       break;
     case DIAGDIAG_LOOPBACK_CMD_OPCODE:
       rsp = diag_lsm_loopback(req_pkt, pkt_len, code);
       return ((void *)rsp);
       break;
     case DIAGDIAG_PD_BUF_MODE_OPCODE:
       rsp = diag_pd_buf_mode_change(req_pkt, pkt_len, code);
       return ((void *)rsp);
       break;
     #ifdef DIAG_UIMAGE_MODE
     case DIAGDIAG_SET_UIMAGE_BUF_MODE_OPCODE:
        rsp = diag_uimage_set_buf_mode_handler(req_pkt, pkt_len);
        return ((void *)rsp);
        break;
     case DIAGDIAG_GET_UIMAGE_BUF_MODE_OPCODE:
        rsp = diag_uimage_get_buf_mode_handler(req_pkt, pkt_len);
        return ((void *)rsp);
        break;
     case DIAGDIAG_SET_UIMAGE_HIGH_WM_MODE_OPCODE:
        rsp = diag_uimage_set_high_wm_handler(req_pkt, pkt_len);
        return ((void *)rsp);
        break;
     case DIAGDIAG_GET_UIMAGE_HIGH_WM_MODE_OPCODE:
        rsp = diag_uimage_get_high_wm_handler(req_pkt, pkt_len);
        return ((void *)rsp);
        break;
     #endif /* DIAG_UIMAGE_MODE */
     default:
       return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
       break;
  }
  
} /* diag_LSM_test_cmd_handler */


/*===========================================================================

FUNCTION DIAG_LSM_DELAY_TEST

DESCRIPTION
  This function creates a standard command response and a delayed response.

RETURN VALUE
  NULL

============================================================================*/
PACKED void* Diag_LSM_Delay_Test(
   PACKED void *req_pkt,
   uint16 pkt_len
   )
{
  uint32 delayed_rsp_id = 0;
  uint16 cmd_code;
  diag_pd_type user_pd = diag_lsm_cur_userpd;

  if (req_pkt == NULL)
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  if (user_pd < DIAG_PD_MIN || user_pd > DIAG_PD_MAX)
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Delay_Test: Didn't get userPD %d, req_pkt 0x%08X",user_pd, req_pkt);
    return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);
  diagdiag_delayed_rsp_test_rsp_type *rsp = (diagdiag_delayed_rsp_test_rsp_type *)
                                             diagpkt_subsys_alloc_v2(DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                                     sizeof(diagdiag_lsm_delayed_rsp_test_rsp_type));
  MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Sending delayed responses");
  if(rsp)
  {
    delayed_rsp_id = rsp->delayed_rsp_id;
    diagpkt_commit(rsp);
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Out of memory- Cannot send the response");
  }
  
  rsp = NULL;
  
  rsp = (diagdiag_delayed_rsp_test_rsp_type *)diagpkt_subsys_alloc_v2_delay(DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                                            delayed_rsp_id, sizeof(diagdiag_lsm_delayed_rsp_test_rsp_type));
  if(rsp)
  {
    rsp->response_cnt = 1;
    diagpkt_delay_commit(rsp);
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "Out of memory- Cannot send the delayed response");
  }

  return NULL;
  
} /* Diag_LSM_Delay_Test */

/*===========================================================================

FUNCTION DIAG_MPDBUF_MODE_CHANGE

DESCRIPTION
  This function changes the value of 'diagbuf_mpd_mode' which controls UserPD buffer reading

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void* diag_pd_buf_mode_change(
     PACKED void *req_pkt,
     uint16 pkt_len,
     uint16 code
     )
{

  diag_LSM_test_cmd_rsp_type *rsp = NULL;
  uint16 req_len = 0;
  uint8 mode=0;
  diag_LSM_test_cmd_req_type *req = (diag_LSM_test_cmd_req_type *)req_pkt;

  if (req_pkt == NULL)
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  req_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type);

  /*Packet length should be atleast req_length*/
  if (pkt_len < req_len)
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }
  mode = req->cmd_type.pd_buf_mode;
  rsp = (diag_LSM_test_cmd_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                                           code, pkt_len);

  if(rsp!=NULL)
  {
    rsp->opcode = req->opcode;
    rsp->rsp_cmd_type.pd_buf_mode = diag_set_pd_buf_mode(mode);
   // memscpy(rsp,pkt_len,req_pkt,pkt_len);
  }

  return (rsp);
}


/*===========================================================================

FUNCTION DIAG_LSM_HEALTH_COMMAND

DESCRIPTION
  This procedure collects/resets and send LSM health statistics.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void* Diag_LSM_Health_Command(
   PACKED void *req_pkt,
   uint16 pkt_len,
   uint16 code
   )
{
  /* Initialization */
  diagpkt_subsys_cmd_code_type cmd_code;
  uint16 health_rsp_len = 0;
  uint16 req_len = 0;
  diag_LSM_test_cmd_rsp_type *rsp = NULL;

  diag_LSM_test_cmd_req_type *req = (diag_LSM_test_cmd_req_type *)req_pkt;

  if (req_pkt == NULL)
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  cmd_code = req->cmd_type.diag_LSM_health_command;

  req_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type) + sizeof(uint16);
  if (pkt_len != req_len)
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  health_rsp_len = FPOS(diag_LSM_test_cmd_rsp_type, rsp_cmd_type) + sizeof(diag_LSM_health_cmd_type);

  rsp = (diag_LSM_test_cmd_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                                           code, health_rsp_len);

  if (rsp != NULL)
  {
    rsp->opcode = req->opcode; 
    rsp->rsp_cmd_type.health_cmd.cmd_code = cmd_code;
    switch (cmd_code)
    {
       case DIAGDIAG_GET_ALLOC_COUNT_F3_Q6:
       case DIAGDIAG_GET_ALLOC_COUNT_F3_MODEM:
         rsp->rsp_cmd_type.health_cmd.drop_count = diagbuf_mpd_get_msg_alloc_count();
         break;
       case DIAGDIAG_GET_DROP_COUNT_F3_Q6:
       case DIAGDIAG_GET_DROP_COUNT_F3_MODEM:
         rsp->rsp_cmd_type.health_cmd.drop_count = diagbuf_mpd_get_msg_drop_count();
         break;
       case DIAGDIAG_RESET_DROP_COUNT_F3_Q6:
       case DIAGDIAG_RESET_DROP_COUNT_F3_MODEM:
         diagbuf_mpd_reset_msg_alloc_drop_counts();
         rsp->rsp_cmd_type.health_cmd.drop_count = diagbuf_mpd_get_msg_drop_count();
         break;
       case DIAGDIAG_GET_ALLOC_COUNT_LOG_Q6:
       case DIAGDIAG_GET_ALLOC_COUNT_LOG_MODEM:
         rsp->rsp_cmd_type.health_cmd.drop_count = diagbuf_mpd_get_log_alloc_count();
         break;
       case DIAGDIAG_GET_DROP_COUNT_LOG_Q6:
       case DIAGDIAG_GET_DROP_COUNT_LOG_MODEM:
         rsp->rsp_cmd_type.health_cmd.drop_count = diagbuf_mpd_get_log_drop_count();
         break;
       case DIAGDIAG_RESET_DROP_COUNT_LOG_Q6:
       case DIAGDIAG_RESET_DROP_COUNT_LOG_MODEM:
         diagbuf_mpd_reset_log_alloc_drop_counts();
         rsp->rsp_cmd_type.health_cmd.drop_count = diagbuf_mpd_get_log_drop_count();
         break;
    }
  }
  return (rsp);
  
} /* Diag_LSM_Health_Command */


/*===========================================================================

FUNCTION DIAG_LSM_LOOPBACK

DESCRIPTION
  This procedure echos the request in the response.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void* diag_lsm_loopback(
  PACKED void *req_pkt,
  uint16 pkt_len,
  uint16 code
  )
{

  diag_LSM_test_cmd_rsp_type *rsp = NULL;
  uint16 req_len = 0;
  
  if (req_pkt == NULL)
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }
  
  req_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type);
  
  /*Packet length should be atleast req_length*/
  if (pkt_len < req_len)
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp = (diag_LSM_test_cmd_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                                           code, pkt_len);
  if(rsp)
  {
    memscpy(rsp,pkt_len,req_pkt,pkt_len);
  }

  return ((void *)rsp);
  
} /* diag_lsm_loopback */


/*===========================================================================
 
 FUNCTION diag_uimage_set_buf_mode_handler
 
 DESCRIPTION
   This function handles the command which changes the pd uimage buffer's  
   circular buffering nd threshold buffering modes. 
       
 RETURN VALUE
   Pointer to response packet.
 
 ============================================================================*/
 #ifdef DIAG_UIMAGE_MODE
 PACKED void* diag_uimage_set_buf_mode_handler(
      PACKED void *req_pkt,
      uint16 pkt_len)
 {
 
   diag_LSM_test_cmd_rsp_type *rsp = NULL;
   uint16 req_len = 0;
   uint8 mode = 0;
   diag_LSM_test_cmd_req_type *req = (diag_LSM_test_cmd_req_type *)req_pkt;
   
   if (req_pkt == NULL)
   {
     return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
   }
 
   req_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type) + sizeof(uint8);
 
   if (pkt_len != req_len)
   {
     return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
   }
   
   mode = req->cmd_type.uimage_buf_mode;
   rsp = (diag_LSM_test_cmd_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                                            USER_PD_STRESS_TEST_CMD_CODE, req_len);
 
   if( rsp!=NULL )
   {
     rsp->opcode = req->opcode;
     rsp->rsp_cmd_type.uimage_buf_mode = diag_set_uimage_tx_mode(mode);
   }
 
   return (rsp);
   
 } /* diag_uimage_set_buf_mode_handler */
 
 
 /*===========================================================================
 
 FUNCTION diag_uimage_set_buf_mode_handler
 
 DESCRIPTION
   This function retrieves the pd uimage buffer's current buffering mode
   (circular buffering or threshold buffering).      
   
 RETURN VALUE
   Pointer to response packet.
 
 ============================================================================*/
 PACKED void* diag_uimage_get_buf_mode_handler(
      PACKED void *req_pkt,
      uint16 pkt_len)
 {
 
   diag_LSM_test_cmd_rsp_type *rsp = NULL;
   uint16 req_len = 0;
   uint16 rsp_len = 0;
   diag_LSM_test_cmd_req_type *req = (diag_LSM_test_cmd_req_type *)req_pkt;
   
   if (req_pkt == NULL)
   {
     return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
   }
 
   req_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type);
   rsp_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type) + sizeof(uint8);
 
   /*Packet length should be at least req_length*/
   if (pkt_len < req_len)
   {
     return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
   }
   
   rsp = (diag_LSM_test_cmd_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                                            USER_PD_STRESS_TEST_CMD_CODE, rsp_len);
 
   if( rsp!=NULL )
   {
     rsp->opcode = req->opcode;
     rsp->rsp_cmd_type.uimage_buf_mode = diag_get_uimage_tx_mode();
   }
 
   return (rsp);
   
 } /* diag_uimage_get_buf_mode_handler */
 
 
 /*===========================================================================
 
 FUNCTION diag_uimage_set_high_wm_handler
 
 DESCRIPTION
   This function changes the high watermark value that controls draining
   of micro Diag buffer while in threshold buffering mode.
 
 RETURN VALUE
   Pointer to response packet.
 
 ============================================================================*/
 PACKED void* diag_uimage_set_high_wm_handler(
      PACKED void *req_pkt,
      uint16 pkt_len)
 {
   diag_LSM_test_cmd_rsp_type *rsp = NULL;
   uint16 req_len = 0;
   uint8 high_wm_per = 0;
   diag_LSM_test_cmd_req_type *req = (diag_LSM_test_cmd_req_type *)req_pkt;
   
   if (req_pkt == NULL)
   {
     return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
   }
 
   req_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type) + sizeof(uint8);
 
   if (pkt_len != req_len)
   {
     return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
   }
   
   high_wm_per = req->cmd_type.uimage_high_wm_per;
   
   diag_set_uimage_tx_high_param_wm(high_wm_per);
   
   rsp = (diag_LSM_test_cmd_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                                            USER_PD_STRESS_TEST_CMD_CODE, req_len);
 
   if( rsp!=NULL )
   {
     rsp->opcode = req->opcode;
     rsp->rsp_cmd_type.uimage_high_wm_per = diag_get_uimage_tx_high_param_wm();
   }
 
   return (rsp);
   
 } /* diag_uimage_set_high_wm_handler */
 
 
 /*===========================================================================
 
 FUNCTION diag_uimage_get_high_wm_handler
 
 DESCRIPTION
   This function retrieves the high watermark value that controls draining
   of micro Diag buffer while in threshold buffering mode.
 
 RETURN VALUE
   Pointer to response packet.
 
 ============================================================================*/
 PACKED void* diag_uimage_get_high_wm_handler(
      PACKED void *req_pkt,
      uint16 pkt_len)
 {
   diag_LSM_test_cmd_rsp_type *rsp = NULL;
   uint16 req_len = 0;
   uint16 rsp_len = 0;
   diag_LSM_test_cmd_req_type *req = (diag_LSM_test_cmd_req_type *)req_pkt;
   
   if (req_pkt == NULL)
   {
     return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
   }
 
   req_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type);
   rsp_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type) + sizeof(uint8);
 
   /*Packet length should be at least req_length*/
   if (pkt_len < req_len)
   {
     return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
   }
      
   rsp = (diag_LSM_test_cmd_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                                            USER_PD_STRESS_TEST_CMD_CODE, rsp_len);
 
   if( rsp!=NULL )
   {
     rsp->opcode = req->opcode;
     rsp->rsp_cmd_type.uimage_high_wm_per = diag_get_uimage_tx_high_param_wm();
   }
 
   return (rsp);
   
 } /* diag_uimage_get_high_wm_handler */
 #endif /* DIAG_UIMAGE_MODE */
 
 
/*===========================================================================

FUNCTION diag_lsm_cmd_reg_table

DESCRIPTION
  This procedure tests Registers and Deregisters given registration tables.

RETURN VALUE
  None.

============================================================================*/

PACKED void diag_lsm_cmd_reg_test_table(diagpkt_subsys_cmd_code_type cmd_code, uint8 idx)
{

  switch(cmd_code)
  {
    case DIAG_LSM_SUBSYS_CMD_REG_WLAN:
    {
      switch (idx)
      {
        case DIAG_CMD_REG_TEST_TABLE1:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_1_wlan);
        break;
        case DIAG_CMD_REG_TEST_TABLE2:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_2_wlan);
        break;
        case DIAG_CMD_REG_TEST_TABLE3:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_3_wlan);
        break;
      }
      break;
    }
    case DIAG_LSM_SUBSYS_CMD_DEREG_WLAN:
    {
      switch (idx)
      {
        case DIAG_CMD_REG_TEST_TABLE1:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_1_wlan);
        break;
        case DIAG_CMD_REG_TEST_TABLE2:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_2_wlan);
        break;
        case DIAG_CMD_REG_TEST_TABLE3:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_3_wlan);
        break;
      }
      break;
    }
    case DIAG_LSM_SUBSYS_CMD_REG_AUDIO:
    {
      switch (idx)
      {
        case DIAG_CMD_REG_TEST_TABLE1:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_1_audio);
        break;
        case DIAG_CMD_REG_TEST_TABLE2:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_2_audio);
        break;
        case DIAG_CMD_REG_TEST_TABLE3:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_3_audio);
        break;
      }
      break;
    }
    case DIAG_LSM_SUBSYS_CMD_DEREG_AUDIO:
  	{
  	  switch (idx)
  	  	{
	  	  case DIAG_CMD_REG_TEST_TABLE1:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_1_audio);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE2:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_2_audio);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE3:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_3_audio);
		    break;		  
  	  	}
      break;
  	}
    case DIAG_LSM_SUBSYS_CMD_REG_SENSOR:
    {
      switch (idx)
      {
        case DIAG_CMD_REG_TEST_TABLE1:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_1_sensor);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE2:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_2_sensor);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE3:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_3_sensor);
		    break;		  
  	  	}
      break;
  	}
    case DIAG_LSM_SUBSYS_CMD_DEREG_SENSOR:
  	{
  	  switch (idx)
  	  	{
	  	  case DIAG_CMD_REG_TEST_TABLE1:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_1_sensor);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE2:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_2_sensor);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE3:
        DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_lsm_test_reg_tbl_3_sensor);
		    break;		  
  	  	}
      break;
  	}
  }
} /* diag_lsm_cmd_reg_test_table */


/*===========================================================================

FUNCTION diag_lsm_subsys_reg_dereg_test

DESCRIPTION
  This procedure tests Registration and Deregistration of User PD registration tables.

RETURN VALUE
  Pointer to response packet.

============================================================================*/

PACKED void* diag_lsm_subsys_reg_dereg_test( PACKED void *req_pkt, uint16 pkt_len )
{
  diag_lsm_cmd_reg_test_rsp_type *rsp = NULL;
  diagpkt_subsys_cmd_code_type cmd_code = 0;
  diag_lsm_cmd_reg_test_rsp_type *req = (diag_lsm_cmd_reg_test_rsp_type *)req_pkt;
  
  if((pkt_len != sizeof(diag_lsm_cmd_reg_test_rsp_type)) || (req_pkt == NULL))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
 
 if(req->table1 <= DIAG_CMD_REG_TEST_MAX_TABLE)
     diag_lsm_cmd_reg_test_table(cmd_code,req->table1);
 
 if(req->table2 <= DIAG_CMD_REG_TEST_MAX_TABLE)
	 diag_lsm_cmd_reg_test_table(cmd_code,req->table2);
 
 if(req->table3 <= DIAG_CMD_REG_TEST_MAX_TABLE)
     diag_lsm_cmd_reg_test_table(cmd_code,req->table3);

  
  rsp = (diag_lsm_cmd_reg_test_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                                           cmd_code, sizeof(diag_lsm_cmd_reg_test_rsp_type));

  if(rsp)
  {
    memscpy(rsp,pkt_len,req_pkt,pkt_len);
  }

  return ((void *)rsp);
  
} /* diag_lsm_subsys_reg_dereg_test */


/*===========================================================================

FUNCTION diag_LSM_subsys_reg_test

DESCRIPTION
  This procedure is used to test registration and deregistration functionality 
  and simply echos the request in the response.

RETURN VALUE
  Pointer to response packet.

============================================================================*/

PACKED void* diag_lsm_subsys_reg_test( PACKED void *req_pkt, uint16 pkt_len )
{

  void *rsp = NULL;
  uint8 req_len_min = 0;
  diagpkt_subsys_cmd_code_type cmd_code = 0;

  req_len_min = sizeof(diagpkt_subsys_header_type);
  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
  
  if((req_pkt == NULL) || (pkt_len < req_len_min))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  rsp = diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,cmd_code, pkt_len);
  if(rsp)
  {
    memscpy(rsp,pkt_len,req_pkt,pkt_len);
  }

  return ((void *)rsp);
  
} /* diag_lsm_subsys_reg_test */

/*===========================================================================

FUNCTION diag_LSM_subsys_reg_test2

DESCRIPTION
  This procedure is used to test registration and deregistration functionality 
  and simply echos the request in the response.

RETURN VALUE
  Pointer to response packet.

============================================================================*/

PACKED void* diag_lsm_subsys_reg_test2( PACKED void *req_pkt, uint16 pkt_len )
{

  void *rsp = NULL;
  uint8 req_len_min = 0;
  diagpkt_subsys_cmd_code_type cmd_code = 0;

  req_len_min = sizeof(diagpkt_subsys_header_type);
  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
  
  if((req_pkt == NULL) || (pkt_len < req_len_min))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  rsp = diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,cmd_code, pkt_len);
  if(rsp)
  {
    memscpy(rsp,pkt_len,req_pkt,pkt_len);
  }

  return ((void *)rsp);
  
} /* diag_lsm_subsys_reg_test2 */

/*===========================================================================

FUNCTION diag_LSM_subsys_reg_test3

DESCRIPTION
  This procedure is used to test registration and deregistration functionality 
  and simply echos the request in the response.

RETURN VALUE
  Pointer to response packet.

============================================================================*/

PACKED void* diag_lsm_subsys_reg_test3( PACKED void *req_pkt, uint16 pkt_len )
{

  void *rsp = NULL;
  uint8 req_len_min = 0;
  diagpkt_subsys_cmd_code_type cmd_code = 0;

  req_len_min = sizeof(diagpkt_subsys_header_type);
  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
  
  if((req_pkt == NULL) || (pkt_len < req_len_min))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  rsp = diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,cmd_code, pkt_len);
  if(rsp)
  {
    memscpy(rsp,pkt_len,req_pkt,pkt_len);
  }
  return ((void *)rsp);
  
} /* diag_lsm_subsys_reg_test3 */

/*===========================================================================

FUNCTION diag_LSM_subsys_reg_test4

DESCRIPTION
  This procedure is used to test registration and deregistration functionality 
  and simply echos the request in the response.

RETURN VALUE
  Pointer to response packet.

============================================================================*/

PACKED void* diag_lsm_subsys_reg_test4( PACKED void *req_pkt, uint16 pkt_len )
{

  void *rsp = NULL;
  uint8 req_len_min = 0;
  diagpkt_subsys_cmd_code_type cmd_code = 0;

  req_len_min = sizeof(diagpkt_subsys_header_type);
  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
  
  if((req_pkt == NULL) || (pkt_len < req_len_min))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  rsp = diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,cmd_code, pkt_len);
  if(rsp)
  {
    memscpy(rsp,pkt_len,req_pkt,pkt_len);
  }
  return ((void *)rsp);
  
} /* diag_lsm_subsys_reg_test4 */


/*===========================================================================

FUNCTION STRESS_TEST_TASK

DESCRIPTION
  This procedure sends a response to acknowledge that the test has been started.
  It then defines "num_tasks" tasks, with a requested priorities.
  The task startup functions tests flooding the link using the requested
  parameters.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void* stress_test_task(
   PACKED void *req_pkt,
  uint16 pkt_len,
  uint16 code
   )
{
  diag_LSM_test_cmd_rsp_type *rsp;
  diag_LSM_test_cmd_req_type *req = (diag_LSM_test_cmd_req_type *)req_pkt;
  int rsp_len = 0;
  /*  used to store and retrive data block from the Q */
  diag_stress_test_task_q_item_type *q_item = NULL;
  int i = 0;

    if (req == NULL)
    {
      return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
    }

  rsp_len = FPOS(diag_LSM_test_cmd_req_type, cmd_type) + FPOS(diag_LSM_stress_test_type, test)
     + sizeof(diag_per_task_test_info) * (req->cmd_type.stress_test_info.num_tasks);

  if (pkt_len != rsp_len)
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

    rsp = (diag_LSM_test_cmd_rsp_type *) diagpkt_subsys_alloc (
    DIAG_SUBSYS_DIAG_SERV,
    code,
    rsp_len
    );

  /* Send the response acknowledging that the packet has been started */
  if (rsp != NULL)
  {
    memscpy((void *)rsp, rsp_len, (void *)req, rsp_len);

    /* Initialize the queue */
    if (diag_stress_test_task_q_init == FALSE)
    {
      (void)q_init(&diag_stress_test_task_q);
      diag_stress_test_task_q_init = TRUE;
    }

    for (i = 0; i < req->cmd_type.stress_test_info.num_tasks; i++)
    {
      /* Get memory from the system heap */
      q_item = (diag_stress_test_task_q_item_type *)malloc(
         sizeof(diag_stress_test_task_q_item_type));

      if (q_item != NULL)
      {
        memset(q_item, 0x0, sizeof(diag_stress_test_task_q_item_type));

        /* Initialize the link field */
        (void)q_link(q_item, &(q_item->link));

        /* Fill the data */
        q_item->data.task_info = req->cmd_type.stress_test_info.test[i];
        q_item->data.status = DIAG_TASK_INITIALIZED;
        q_item->data.unique_id = i;   /* Task IDs range from 0 to num_tasks-1 */

        /* Place q_item on the diag_stress_test_task_q */
        q_put(&diag_stress_test_task_q, &(q_item->link));
      }
      else
      { /* Failed  system malloc, handle this error */
        diagpkt_commit(rsp);
        rsp = NULL;
        MSG_4(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Failed to allocate q_item for stress test. cmd %d %d %d %d\n", req->header.opaque_header[0], 
              req->header.opaque_header[1], req->header.opaque_header[2], req->header.opaque_header[3]);
            return NULL;
      }
    }

    /* Commit the response before spawning the test task. */
    diagpkt_commit(rsp);

    rsp = NULL;
  }

  /* Get the head of the Q */
  q_item = (diag_stress_test_task_q_item_type *)q_check(&diag_stress_test_task_q);

  /* Loop until the end is reached */
  while (q_item != NULL)
  {
    /* if the task is not yet started */
    if (q_item->data.status == DIAG_TASK_INITIALIZED)
    {
      int diagdiag_stress_pri_array[32] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

      if (q_item->data.task_info.priority.pri_type == RELATIVE_PRI)
      {
          uint16 sleep_pri = SLEEP_PRI;
          uint16 diag_pri  = osal_thread_get_pri();

          if(diag_pri < sleep_pri )
          {
            /*1 is highest prio and 255 is lowest */
            if ( diag_pri + q_item->data.task_info.priority.pri < sleep_pri )
            {
                q_item->data.task_info.priority.pri += diag_pri;
            }
            else
            {
                q_item->data.task_info.priority.pri = sleep_pri - 1;
            }
          } 
          else
          {
            /*1 is lowest prio and 255 is highest */
            if ( diag_pri + q_item->data.task_info.priority.pri < sleep_pri )
          {
                /*Priority cannot be negative*/
                q_item->data.task_info.priority.pri = sleep_pri + 1 ;
          } 
          else
          {
                q_item->data.task_info.priority.pri += diag_pri ;
            }
          }
        }

      /* Start the task */
      /* stack is of type unsigned long long' */
      diagdiag_stress_attr.stack_size = (STRESS_TEST_STACK_SIZE * sizeof(unsigned long long));
      diagdiag_stress_attr.start_func = Diag_LSM_stresstest;
      diagdiag_stress_attr.arg = (void *)(&(q_item->data));
      diagdiag_stress_attr.name = "Diag_LSM_stresstest";
      diagdiag_stress_attr.priority = q_item->data.task_info.priority.pri;
      diagdiag_stress_attr.stack_address = q_item->data.stack;
      diagdiag_stress_attr.p_tskname = "DIAG_LSM_TEST";
      diagdiag_stress_attr.suspended = FALSE;
      diagdiag_stress_attr.dog_report_val = 0;

      /* Mark the task is running BEFORE creating the thread for multi-threaded case */
      if (q_item->data.status == DIAG_TASK_INITIALIZED)
      {
        q_item->data.status = DIAG_TASK_RUNNING;
      }

      if ((osal_create_thread(&(q_item->data.tcb), &diagdiag_stress_attr, diagdiag_stress_pri_array)) != 0)
      {
#ifdef FEATURE_Q_NO_SELF_QPTR
          q_delete( &diag_stress_test_task_q, &q_item->link );
#else
          q_delete  (&q_item->link);
#endif
          free(q_item);  
          return NULL;
      }

    }
    /* go to the next Q item */
    q_item = q_next(&diag_stress_test_task_q, &q_item->link);
  }

  return (NULL);
  
} /* stress_test_task */


/*===========================================================================

FUNCTION DIAG_LSM_STRESSTEST_CLEANUP_CB

DESCRIPTION
  This procedure is a cleanup function. It goes through the
  diag_stress_test_task_q, checks if any data block is ready to cleaned.
  If yes it removes the entry from the Q and frees the memory.

============================================================================*/
void diag_LSM_stresstest_cleanup_cb(void)
{
  diag_stress_test_task_q_item_type *q_cur_ptr = NULL;
  diag_stress_test_task_q_item_type *q_next_ptr = NULL;
  int status;

  q_cur_ptr = (diag_stress_test_task_q_item_type *)q_check(&diag_stress_test_task_q);
  while (q_cur_ptr != NULL)
  {
    // If the element in the queue is ready to be deleted
    if (q_cur_ptr->data.status == DIAG_TASK_DONE)
    {
      // mapped to no-ops for all OSes except QURT
      osal_thread_join(&(q_cur_ptr->data.tcb), &status);

      // cleanup OSAL related memory, queues, channel etc
      osal_delete_thread(&(q_cur_ptr->data.tcb));

      // store the next link
      q_next_ptr = (diag_stress_test_task_q_item_type *)q_next
         (&diag_stress_test_task_q, &q_cur_ptr->link);

      // Remove the data block from the Q
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete(&diag_stress_test_task_q, &q_cur_ptr->link);
#else
      q_delete(&q_cur_ptr->link);
#endif


      // Free the memory
      free(q_cur_ptr);

      if (q_cur_ptr != NULL)
      {
        q_cur_ptr = NULL;
      }
      q_cur_ptr = q_next_ptr;
    }
    else
    {
      // store the next link
      q_cur_ptr = (diag_stress_test_task_q_item_type *)q_next
         (&diag_stress_test_task_q, &q_cur_ptr->link);
    }
  }
} /* diag_LSM_stresstest_cleanup_cb */


/*===========================================================================
FUNCTION PSEUDO_RANDOM

DESCRIPTION
  This function returns a random number from pseudo array[].
  (for repeatabilty use pseudo random numbers)
 
PARAMETERS
  i    Any integer value
 
============================================================================*/
int pseudo_random(int i)
{
  i = i % (sizeof(pseudo_array) / sizeof(byte));
  if (i < 14)
    return (pseudo_array[i]);
  else
    return (0);

} /* pseudo_random */


/*===========================================================================

FUNCTION DIAG_LSM_STRESSTEST

DESCRIPTION
  This procedure is a startup function. Floods the link with the requested
  test type packets.
 
PARAMETERS
  params   This is of type diag_stress_test_tcb_stack_type and has information
           about stress test (test type, number of iterations etc)
 
============================================================================*/
void Diag_LSM_stresstest(
   uint32 params
   )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  osal_timer_t sleep_timer;
  uint32 diag_stress_micro_diagbuffer_bufsize;

  if (params == NULL)
  {
    return;
  }

  diag_stress_test_tcb_stack_type *local_data = (diag_stress_test_tcb_stack_type *)params;

  /* Pointers to stress test logs */
  LOG_STRESS_TEST_C_type *log_ptr;
  LOG_STRESS_TEST_64_type *log_ptr_64;
  LOG_STRESS_TEST_128_type *log_ptr_128;
  LOG_STRESS_TEST_256_type *log_ptr_256;
  LOG_STRESS_TEST_1K_type *log_ptr_1K;
  LOG_STRESS_TEST_2K_type *log_ptr_2K;
  LOG_STRESS_TEST_4K_type *log_ptr_4K;
  LOG_STRESS_TEST_6K_type *log_ptr_6K;
  LOG_DIAG_ULOG_type * log_ptr_microulog;

  uint16 log_index = 0;             /* Index used to fill char array */
  EVENT_PAYLOAD_INFO_type payload;  /* Store the payload info */
  uint32 payload_stress_test_complete = 0; /* payload for stress test complete event */
  uint32 version_id = 10;                  /* Used to generate extended packets */
  boolean timer_init = FALSE;       /* timer initialized or not */
  int test_type = 0;   /* Stores the test type info requested */
  int num_iter_bef_sleep = 0; /* Keeps track of number of iterations before sleep */
  int i = 1;
  int N = local_data->task_info.num_iterations; /* Total number of iterations */
  int unique_id = local_data->unique_id; /* Get the task ID */

#if defined(DIAG_MMPM_SUPPORT )
  int client_id = 0;
#endif

/* Vote against sleep for stress test */
#if defined( DIAG_DYNAMIC_SLEEP )
  if (diagtest_npa_handle)
  {
    npa_issue_required_request(diagtest_npa_handle, 1);
  }

#elif defined (DIAG_MMPM_SUPPORT)
  client_id = diag_mmpm_register();
  /* Vote against power collapse on LPass when the test starts */
  diag_mmpm_config(client_id);
#endif

  /*Start the handshake so that the task is ready to start */
  return_val=osal_handshake_start(&(local_data->tcb));
  ASSERT(OSAL_SUCCESS == return_val);

  for (i = 1; i <= N; i++)
  {

    /* Special Case, test type is generated using random */
    if (local_data->task_info.priority.test_type == DIAGDIAG_STRESS_TEST_MSG_PSEUDO_RANDOM)
    {
      /* Pick the random test type from the array */
      test_type = pseudo_random(i);
    }
    else
    {
      test_type = local_data->task_info.priority.test_type;
    }

    /**********************************************************************************
     *
     * In all the following cases, the priority field is replaced by unique id field 
     * to help the automation scripts differentiate between test tasks while parsing 
     * the test logs.
     *
     **********************************************************************************/
    switch (test_type)
    {
      /* =================================
            Message stress tests
      ================================= */
       case DIAGDIAG_STRESS_TEST_MSG:
         MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Test MSG with no arg ");
         break;
       case DIAGDIAG_STRESS_TEST_MSG_1:
         MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_1 Iter %d\n", i);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_2:
         MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_2 Iter %d uid %d \n",i,unique_id);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_3:
         MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_3 Iter %d uid %d num_iter %d\n", i,
               unique_id, N);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_4:
         MSG_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_4 Iter %d uid %d num_iter %d procid %d\n", i,
               unique_id, N, DIAG_MY_PROC_ID);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_5:
         MSG_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_5 Iter %d uid %d num_iter %d procid %d %d\n", i,
               unique_id, N, DIAG_MY_PROC_ID, 5);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_6:
         MSG_6(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_6 Iter %d uid %d num_iter %d procid %d  %d  %d\n",
               i, unique_id, N, DIAG_MY_PROC_ID, 0, 0);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_STR:
         //TODO
         break;
        /* =================================
            SPRINTF stress tests
        ================================= */
       case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_1:
         MSG_SPRINTF_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_1 Iter %d\n", i);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_2:
         MSG_SPRINTF_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_2 Iter %d uid %d\n", i, unique_id);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_3:
         MSG_SPRINTF_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_3 Iter %d uid %d num_iter %d \n", i, unique_id, N);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_4:
         MSG_SPRINTF_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_4 Iter %d uid %d num_iter %d %s \n", i, unique_id, N, "4");
         break;
       case DIAGDIAG_STRESS_TEST_MSG_SPRINTF_5:
         MSG_SPRINTF_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MSG_SPRINTF_5 Iter %d uid %d num_iter %d %s %s \n",i, unique_id,N,"4", "5");
         break;
        /* =================================
            Message level stress tests
        ================================= */
       case DIAGDIAG_STRESS_TEST_MSG_LOW:
         MSG_LOW("MSG_LOW Iter %d uid %d num_iter %d \n", i, unique_id, N);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_MED:
         MSG_MED("MSG_MED Iter %d uid %d num_iter %d \n", i, unique_id, N);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_HIGH:
         MSG_HIGH("Client MSG_HIGH Iter %d uid %d num_iter %d \n", i, unique_id, N);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_ERROR:
         MSG_ERROR("MSG_ERROR Iter %d uid %d num_iter %d \n", i, unique_id, N);
         break;
       case DIAGDIAG_STRESS_TEST_MSG_FATAL:
         MSG_FATAL("MSG_FATAL Iter %d uid %d num_iter %d \n", i, unique_id, N);
         break;
       case DIAGDIAG_STRESS_TEST_ERR:
         ERR("ERR Iter %d uid %d num_iter %d \n", i, unique_id, N);
         break;
       case DIAGDIAG_STRESS_TEST_ERR_FATAL:
         {
           int j;

          /* The ERR_FATAL test needs to make sure panic mode works.  
             Generate a few messages and see if they make it out. */
          for (j = 0; j < 10; j++)
          {
            MSG_FATAL ("Panic mode test %d uid %d", j, unique_id, 0);
          }
        }
        ERR_FATAL("ERR_FATAL Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
       /* =================================
            Events stress tests
        ================================= */
      case DIAGDIAG_STRESS_TEST_EVENT_NO_PAYLOAD:
        event_report(EVENT_DIAG_STRESS_TEST_NO_PAYLOAD);
        break;
      case DIAGDIAG_STRESS_TEST_EVENT_WITH_PAYLOAD:
        payload.iteration = i;
        payload.task_priority = unique_id;
        payload.req_iterations = N;
        event_report_payload(EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD,
          sizeof(EVENT_PAYLOAD_INFO_type),(void *) &payload);
        break;
        /* =================================
            Logs stress tests
        ================================= */
      case DIAGDIAG_STRESS_TEST_LOG:
        log_ptr = (LOG_STRESS_TEST_C_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
            sizeof(LOG_STRESS_TEST_C_type));
        if (log_ptr != NULL) {
          log_ptr->iteration = i;
          log_ptr->task_priority = unique_id;
          log_ptr->req_iterations = N;
          log_commit(log_ptr);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_64:
        log_ptr_64 = (LOG_STRESS_TEST_64_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_64_type));
        if (log_ptr_64 != NULL) {
          log_ptr_64->iteration = i;
          log_ptr_64->task_priority =unique_id;
          log_ptr_64->req_iterations = N;
          for ( log_index = 0; log_index < 13; log_index++ ) {
              log_ptr_64->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_64);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_128:
        log_ptr_128 = (LOG_STRESS_TEST_128_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_128_type));
         if (log_ptr_128 != NULL)
         {
           log_ptr_128->iteration = i;
           log_ptr_128->task_priority = unique_id;
           log_ptr_128->req_iterations = N;
           for (log_index = 0; log_index < 29; log_index++)
           {
             log_ptr_128->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_128);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_256:
         log_ptr_256 = (LOG_STRESS_TEST_256_type *)log_alloc(LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_256_type));
         if (log_ptr_256 != NULL)
         {
           log_ptr_256->iteration = i;
           log_ptr_256->task_priority = unique_id;
           log_ptr_256->req_iterations = N;
           for (log_index = 0; log_index < 61; log_index++)
           {
             log_ptr_256->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_256);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_1K:
         log_ptr_1K = (LOG_STRESS_TEST_1K_type *)log_alloc(LOG_DIAG_STRESS_TEST_C,
                                                           sizeof(LOG_STRESS_TEST_1K_type));
         if (log_ptr_1K != NULL)
         {
           log_ptr_1K->iteration = i;
           log_ptr_1K->task_priority = unique_id;
           log_ptr_1K->req_iterations = N;
           for (log_index = 0; log_index < 253; log_index++)
           {
             log_ptr_1K->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_1K);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_2K:
         log_ptr_2K = (LOG_STRESS_TEST_2K_type *)log_alloc(LOG_DIAG_STRESS_TEST_C,
                                                           sizeof(LOG_STRESS_TEST_2K_type));
         if (log_ptr_2K != NULL)
         {
           log_ptr_2K->iteration = i;
           log_ptr_2K->task_priority = unique_id;
           log_ptr_2K->req_iterations = N;
           for (log_index = 0; log_index < 509; log_index++)
           {
             log_ptr_2K->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_2K);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_4K:
         log_ptr_4K = (LOG_STRESS_TEST_4K_type *)log_alloc(LOG_DIAG_STRESS_TEST_C,
                                                           sizeof(LOG_STRESS_TEST_4K_type));
         if (log_ptr_4K != NULL)
         {
           log_ptr_4K->iteration = i;
           log_ptr_4K->task_priority = unique_id;
           log_ptr_4K->req_iterations = N;
           for (log_index = 0; log_index < 1021; log_index++)
           {
             log_ptr_4K->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_4K);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_6K:
         log_ptr_6K = (LOG_STRESS_TEST_6K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_6K_type));
         if (log_ptr_6K != NULL)
         {
           log_ptr_6K->iteration = i;
           log_ptr_6K->task_priority = unique_id;
           log_ptr_6K->req_iterations = N;
           for (log_index = 0; log_index < 1533; log_index++)
           {
             log_ptr_6K->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_6K);
         }
         break;
	   case DIAGDIAG_STRESS_TEST_LOG_SUBMIT_64:
         log_ptr_64 = (LOG_STRESS_TEST_64_type *) malloc (sizeof(LOG_STRESS_TEST_64_type));
         if (log_ptr_64)
	     {
           log_set_length(log_ptr_64, sizeof(LOG_STRESS_TEST_64_type));
           log_set_code(log_ptr_64, LOG_DIAG_STRESS_TEST_C);
           log_set_timestamp(log_ptr_64);

           log_ptr_64->iteration = i;
           log_ptr_64->task_priority = unique_id;
           log_ptr_64->req_iterations = N;
           for (log_index = 0; log_index < 13; log_index++)
           {
             log_ptr_64->payload[log_index] = 0x7EABBA7E;
           }

           log_submit(log_ptr_64);
           free(log_ptr_64);
         }
         else
         {
           MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Malloc failed in log_submit() test case.Iter %d \n",i);
         }
         break;
       /* =================================
            Extended Logs stress tests
       ================================= */
       case DIAGDIAG_STRESS_TEST_LOG_EXT:
         log_ptr = (LOG_STRESS_TEST_C_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_C_type), DIAG_EXT_API_VERSION_1, &version_id);
         if (log_ptr != NULL)
         {
           log_ptr->iteration = i;
           log_ptr->task_priority = unique_id;
           log_ptr->req_iterations = N;
           log_commit(log_ptr);
	     }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_64_EXT:
         log_ptr_64 = (LOG_STRESS_TEST_64_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_64_type), DIAG_EXT_API_VERSION_1, &version_id);
         if (log_ptr_64 != NULL)
         {
           log_ptr_64->iteration = i;
           log_ptr_64->task_priority =unique_id;
           log_ptr_64->req_iterations = N;
           for ( log_index = 0; log_index < 13; log_index++ )
           {
             log_ptr_64->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_64);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_128_EXT:
         log_ptr_128 = (LOG_STRESS_TEST_128_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_128_type), DIAG_EXT_API_VERSION_1, &version_id);
         if (log_ptr_128 != NULL)
         {
           log_ptr_128->iteration = i;
           log_ptr_128->task_priority =unique_id;
           log_ptr_128->req_iterations = N;
           for ( log_index = 0; log_index < 29; log_index++ )
           {
             log_ptr_128->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_128);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_256_EXT:
         log_ptr_256 = (LOG_STRESS_TEST_256_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_256_type), DIAG_EXT_API_VERSION_1, &version_id);
         if (log_ptr_256 != NULL)
         {
           log_ptr_256->iteration = i;
           log_ptr_256->task_priority =unique_id;
           log_ptr_256->req_iterations = N;
           for ( log_index = 0; log_index < 61; log_index++ )
           {
             log_ptr_256->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_256);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_1K_EXT:
         log_ptr_1K = (LOG_STRESS_TEST_1K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_1K_type), DIAG_EXT_API_VERSION_1, &version_id);
         if (log_ptr_1K != NULL)
         {
           log_ptr_1K->iteration = i;
           log_ptr_1K->task_priority =unique_id;
           log_ptr_1K->req_iterations = N;
           for ( log_index = 0; log_index < 253; log_index++ )
           {
             log_ptr_1K->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_1K);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_2K_EXT:
         log_ptr_2K = (LOG_STRESS_TEST_2K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_2K_type), DIAG_EXT_API_VERSION_1, &version_id);
         if (log_ptr_2K != NULL)
         {
           log_ptr_2K->iteration = i;
           log_ptr_2K->task_priority =unique_id;
           log_ptr_2K->req_iterations = N;
           for ( log_index = 0; log_index < 509; log_index++ )
           {
             log_ptr_2K->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_2K);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_4K_EXT:
         log_ptr_4K = (LOG_STRESS_TEST_4K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_4K_type), DIAG_EXT_API_VERSION_1, &version_id);
         if (log_ptr_4K != NULL)
         {
           log_ptr_4K->iteration = i;
           log_ptr_4K->task_priority =unique_id;
           log_ptr_4K->req_iterations = N;
           for ( log_index = 0; log_index < 1021; log_index++ )
           {
             log_ptr_4K->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_4K);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_6K_EXT:
         log_ptr_6K = (LOG_STRESS_TEST_6K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C, sizeof(LOG_STRESS_TEST_6K_type), DIAG_EXT_API_VERSION_1, &version_id);
         if (log_ptr_6K != NULL)
         {
           log_ptr_6K->iteration = i;
           log_ptr_6K->task_priority =unique_id;
           log_ptr_6K->req_iterations = N;
           for ( log_index = 0; log_index < 1533; log_index++ ) {
             log_ptr_6K->payload[log_index] = 0x7EABBA7E;
           }
           log_commit(log_ptr_6K);
         }
         break;
       case DIAGDIAG_STRESS_TEST_LOG_SUBMIT_EXT_64:
         log_ptr_64 = (LOG_STRESS_TEST_64_type *) malloc (sizeof(LOG_STRESS_TEST_64_type));
         if (log_ptr_64)
         {
           log_set_length(log_ptr_64, sizeof(LOG_STRESS_TEST_64_type));
           log_set_code(log_ptr_64, LOG_DIAG_STRESS_TEST_C);
           log_set_timestamp(log_ptr_64);

           log_ptr_64->iteration = i;
           log_ptr_64->task_priority = unique_id;
           log_ptr_64->req_iterations = N;
           for (log_index = 0; log_index < 13; log_index++)
           {
             log_ptr_64->payload[log_index] = 0x7EABBA7E;
           }

           log_submit_ext(log_ptr_64, DIAG_EXT_API_VERSION_1, &version_id);
           free(log_ptr_64);
         }
         else
         {
           MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Malloc failed in log_submit() test case.Iter %d \n",i);
         }
         break;
       case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_LOG_EXT:
       {
         unsigned long int outputFormat;
         uint8* payload_buf=NULL;
         unsigned long int bytes_written=0;
         micro_diagbuffer_result status;
         micro_ULogResult rval = MICRO_ULOG_SUCCESS;
         uint32 pkt_len=FPOS(LOG_DIAG_ULOG_type, buf );

         if (!diag_stress_micro_diagbuffer_hdl)
         {
           status = micro_diagbuffer_create((micro_ULogHandle *)&diag_stress_micro_diagbuffer_hdl, "micro_diagbuf_stress",
                      diag_stress_micro_diagbuffer_buf, &diag_stress_micro_diagbuffer_bufsize, sizeof(diag_stress_micro_diagbuffer_buf), MICRO_DIAGBUFFER_LOCKABLE);

           ASSERT(MICRO_DIAGBUFFER_SUCCESS == status);
	
           micro_diagbuffer_enable((micro_ULogHandle)diag_stress_micro_diagbuffer_hdl);
         }

         MICRO_DIAGBUFFER_RT_PRINTF_2((micro_ULogHandle)diag_stress_micro_diagbuffer_hdl, "micro_diagbuffer stress test iter %d max Iter %d", i, N );

         if(i == N)
         {
           do
           {
             payload_buf= (uint8 *)malloc(DIAG_ULOG_MAX_PAYLOAD_SIZE);

             if(payload_buf)
             {
               rval= micro_diagbuffer_ReadAndFormat_vaddr((micro_ULogHandle)diag_stress_micro_diagbuffer_hdl, MICROULOG_DIAG_FORMAT_ASCII,
                         &outputFormat, DIAG_ULOG_MAX_PAYLOAD_SIZE, (char *)payload_buf, &bytes_written, NULL);
               if((MICRO_ULOG_SUCCESS == rval) && (bytes_written > 0))
               {
                 pkt_len += bytes_written;
                 log_ptr_microulog = (LOG_DIAG_ULOG_type *) log_alloc_ext (LOG_DIAG_ULOG_PAYLOAD,
                                                                    pkt_len , DIAG_EXT_API_VERSION_3, diag_stress_micro_diagbuffer_hdl);
                 if(log_ptr_microulog)
                 {
                   log_ptr_microulog->output_format = outputFormat;
                   log_ptr_microulog->output_length = bytes_written;
                   memscpy(&log_ptr_microulog->buf[0], bytes_written, payload_buf, bytes_written );
                   log_commit_ext(log_ptr_microulog);
                 }
                 else
                 {
                   MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "unable to allocate log packet");
                   free(payload_buf);
                   break;
                 }
               }
               else
               {
                 MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Unable to format the micro diagbuffer logs: error no = %d bytes_written=%d", rval, bytes_written);
               }
               free(payload_buf);
             }
           }while((MICRO_ULOG_SUCCESS == rval) && (bytes_written > 0));
         }
       }
         break;
#ifdef DIAG_UIMAGE_MODE
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_EXT:
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_EXT with no arg ");
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_1_EXT:
        MICRO_MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_1_EXT Iter %d\n",i);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_2_EXT:
        MICRO_MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_2_EXT Iter %d uid %d \n",i,diag_island_mode_check());
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_3_EXT:
        MICRO_MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_3_EXT Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_4_EXT:
        MICRO_MSG_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_4_EXT Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_5_EXT:
        MICRO_MSG_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_5_EXT Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID,5);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_6_EXT:
        MICRO_MSG_6(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_6_EXT Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID,5,6);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_7_EXT:
        MICRO_MSG_7(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_7_EXT Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID,5,6,7);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_8_EXT:
        MICRO_MSG_8(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_8_EXT Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID,5,6,7,8);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_9_EXT:
        MICRO_MSG_9(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_9_EXT Iter %d uid %d num_iter %d procid %d %d %d\n",
          i,unique_id,N,DIAG_MY_PROC_ID,5,6,7,8,9);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_MSG_9:
        MICRO_MSG_9(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"MICRO_MSG_9_EXT Iter %d uid %d num_iter %d procid %d %d %d\n",
          i,unique_id,N,DIAG_MY_PROC_ID,5,6,7,8,9);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG(micro_diagmpd_buf->handle, MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0 , NULL, "MICRO_DIAGBUFFER_MSG_EXT with no arg ");
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_1_EXT:
		if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_1(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0 , NULL, "MICRO_DIAGBUFFER_MSG_1_EXT Iter %d\n",i);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_2_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_2(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0, NULL, "MICRO_DIAGBUFFER_MSG_2_EXT Iter %d uid %d \n",i,unique_id);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_3_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_3(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0, NULL, "MICRO_DIAGBUFFER_MSG_3_EXT Iter %d uid %d num_iter %d \n",i,unique_id,N);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_4_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_4(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0,NULL,"MICRO_DIAGBUFFER_MSG_4_EXT Iter %d uid %d num_iter %d procid %d\n",i,
								   unique_id,N,DIAG_MY_PROC_ID);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_5_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_5(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0,NULL,"MICRO_DIAGBUFFER_MSG_5_EXT Iter %d uid %d num_iter %d procid %d\n",i,
								   unique_id,N,DIAG_MY_PROC_ID,5);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_6_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_6(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0,NULL,"MICRO_DIAGBUFFER_MSG_6_EXT Iter %d uid %d num_iter %d procid %d\n",i,
								   unique_id,N,DIAG_MY_PROC_ID,5,6);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_7_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_7(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0,NULL,"MICRO_DIAGBUFFER_MSG_7_EXT Iter %d uid %d num_iter %d procid %d\n",i,
								   unique_id,N,DIAG_MY_PROC_ID,5,6,7);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_8_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_8(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH, 0,NULL,"MICRO_DIAGBUFFER_MSG_8_EXT Iter %d uid %d num_iter %d procid %d\n",i,
								   unique_id,N,DIAG_MY_PROC_ID,5,6,7,8);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_DIAGBUFFER_MSG_9_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_MSG_9(micro_diagmpd_buf->handle,MSG_SSID_DIAG, MSG_LEGACY_HIGH,0,NULL, "MICRO_DIAGBUFFER_MSG_9_EXT Iter %d uid %d num_iter %d procid %d %d %d\n",
								   i,unique_id,N,DIAG_MY_PROC_ID,5,6,7,8,9);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_ULOG_MSG_1_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_RT_PRINTF_1((micro_ULogHandle)micro_diagmpd_buf->handle, "MICRO_ULOG_MSG_1 Iter %d\n",i);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_ULOG_MSG_2_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_RT_PRINTF_2((micro_ULogHandle)micro_diagmpd_buf->handle, "MICRO_ULOG_MSG_2 Iter %d uid %d \n",i,unique_id);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_ULOG_MSG_3_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_RT_PRINTF_3((micro_ULogHandle)micro_diagmpd_buf->handle, "MICRO_ULOG_MSG_3 Iter %d uid %d num_iter %d \n",i,unique_id,N);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_ULOG_MSG_4_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_RT_PRINTF_4((micro_ULogHandle)micro_diagmpd_buf->handle,"MICRO_ULOG_MSG_4 Iter %d uid %d num_iter %d procid %d\n",i,
										  unique_id,N,DIAG_MY_PROC_ID);
		}
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_ULOG_MSG_5_EXT:
	    if(micro_diagmpd_buf)
		{
			MICRO_DIAGBUFFER_RT_PRINTF_5((micro_ULogHandle)micro_diagmpd_buf->handle,"MICRO_ULOG_MSG_5 Iter %d uid %d num_iter %d procid %d\n",i,
										  unique_id,N,DIAG_MY_PROC_ID,5);
		}
        break;
	  case DIAGDIAG_STRESS_TEST_MICRO_EVENT_EXT_NO_PAYLOAD:        
        event_report(EVENT_DIAG_STRESS_TEST_NO_PAYLOAD);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_EVENT_EXT_WITH_PAYLOAD:        
        payload.iteration = i;
        payload.task_priority = unique_id;
        payload.req_iterations = N;
        event_report_payload(EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD,
                                sizeof(EVENT_PAYLOAD_INFO_type), (void *)&payload);
       break;
      case DIAGDIAG_STRESS_TEST_MICRO_EVENT_NO_PAYLOAD:        
        event_report(EVENT_DIAG_STRESS_TEST_NO_PAYLOAD);
        break;
      case DIAGDIAG_STRESS_TEST_MICRO_EVENT_WITH_PAYLOAD:        
        payload.iteration = i;
        payload.task_priority = unique_id;
        payload.req_iterations = N;
        event_report_payload(EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD,
                                sizeof(EVENT_PAYLOAD_INFO_type), (void *)&payload);
       break;
      case DIAGDIAG_STRESS_TEST_MICRO_LOG_EXT:
      {
          uint8 *log_pkt;

          log_pkt = (uint8 *)malloc(sizeof(LOG_STRESS_TEST_64_type) + sizeof(log_header_type)); 
          if (log_pkt != NULL)
          {
            log_ptr_64 = (LOG_STRESS_TEST_64_type *)(log_pkt + sizeof(log_header_type));
            log_set_length(log_pkt, sizeof(LOG_STRESS_TEST_64_type));
            log_set_code(log_pkt, LOG_DIAG_STRESS_TEST_C);
						log_set_timestamp(log_pkt);

            log_ptr_64->iteration = i;
            log_ptr_64->task_priority = unique_id;
            log_ptr_64->req_iterations = N;
            for (log_index = 0; log_index < 13; log_index++)
            {
              log_ptr_64->payload[log_index] = 0x7EABBA7E;
            }
            log_submit_ext(log_pkt, 1, &version_id);

            free(log_pkt);
          }
        }
         break;
      case DIAGDIAG_STRESS_TEST_MICRO_LOG:
      {
          uint8 *log_pkt;

          log_pkt = (uint8 *)malloc(sizeof(LOG_STRESS_TEST_64_type) + sizeof(log_header_type)); 
          if (log_pkt != NULL)
          {
            log_ptr_64 = (LOG_STRESS_TEST_64_type *)(log_pkt + sizeof(log_header_type));
            log_set_length(log_pkt, sizeof(LOG_STRESS_TEST_64_type));
            log_set_code(log_pkt, LOG_DIAG_STRESS_TEST_C);
						log_set_timestamp(log_pkt);

            log_ptr_64->iteration = i;
            log_ptr_64->task_priority = unique_id;
            log_ptr_64->req_iterations = N;
            for (log_index = 0; log_index < 13; log_index++)
            {
              log_ptr_64->payload[log_index] = 0x7EABBA7E;
            }
            log_submit_ext(log_pkt, 0, NULL);

            free(log_pkt);
          }
        }
         break;		
#endif		
       default:
         break;
    }

    num_iter_bef_sleep++;

    if ((local_data->task_info.num_iterations_before_sleep > 0)
        && (num_iter_bef_sleep == local_data->task_info.num_iterations_before_sleep))
    {

      if (timer_init == FALSE)
      {
        /* Defines a timer */
        osal_create_diag_timer(&sleep_timer, osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG);
        timer_init = TRUE;
      }

      /* Wait for sleep_duration. This blocks the current task, and
         different task with which is ready to run starts executing */
      (void)osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer,
                            local_data->task_info.sleep_duration);

      /* Clear the signals. The block on the current task is cleared */
      return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      num_iter_bef_sleep = 0;
    }
  } /* end of for loop */

  /* Sleep before sending out EVENT_DIAG_STRESS_TEST_COMPLETED,
   to make sure the event can be allocated and is received by APS.
   In some test-cases, APS misses the event because it is sent out too 
   early. */
  if (timer_init == FALSE)
  {
    /* Defines a timer */
    osal_create_diag_timer(&sleep_timer, osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG);
    timer_init = TRUE;
  }
  (void)osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer, STRESS_TASK_SLEEP_TO_COMPLETE);

  /* Clear the signals. The block on the current task is cleared */
  return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
  if (osal_thread_get_pri() <= DIAG_PRI)
  {
    /* This is done to make sure that the task gets killed before the
     cleanup function, running in diag context, removes the data from the Q */
    (void)osal_thread_set_pri(DIAG_PRI + 1);
  }
  /* Notify that a stress test task has completed for automation */
  event_report_payload(EVENT_DIAG_STRESS_TEST_COMPLETED, sizeof(int32), &payload_stress_test_complete);
  /* mark the task to be cleaned */
  local_data->status = DIAG_TASK_DONE;
/* Vote for sleep for diag stress test */
#if defined( DIAG_DYNAMIC_SLEEP )
  if (diagtest_npa_handle)
  {
    npa_cancel_request(diagtest_npa_handle);
  }

#elif defined(DIAG_MMPM_SUPPORT )
  /*Sleep for few seconds before voting for power collpase so that diag can drain the 
  event stress test completed on lpass*/

  (void)osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer, 10000);

  /* Clear the signals. The block on the current task is cleared */
  return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Vote for power collapse on LPass when the test completes */
  diag_mmpm_release(client_id);
  diag_mmpm_deregister(client_id);
#endif


  /* Delete the sleep timer */
  if (timer_init == TRUE)
  {
    return_val = osal_delete_timer(&sleep_timer);
    ASSERT(OSAL_SUCCESS == return_val);
  }
  /* mark the task to be cleaned */
  local_data->status = DIAG_TASK_DONE;

  /* Clean the stress test queue */
  diag_lsm_clear_stress_test_q();

  
  /* mapped to no-ops for all OSes except QURT */

  osal_thread_exit(0);
  
} /* Diag_LSM_stresstest */

