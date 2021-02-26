/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  F3 Trace DAL Device Configuration Initialization
                
GENERAL DESCRIPTION
  Contains handlers to support F3 message trace retrieval from the EFS.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be called after any initialization routines with higher precedence
  (eg post-compilation) such as NV.

Copyright (c) 2012-2013,2016-2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULEW
$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/f3_trace/src/diag_f3_trace_cmd.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
11/22/13   sr      Removed PACK from pointer references 
01/25/13   rh      Added ADSP handler 
12/11/12   rh      Initial file creation

===========================================================================*/

#if defined __cplusplus
  extern "C" {
#endif

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "msg.h"
#include "diag_f3_tracei.h"
#include "diag_f3_trace_cmd.h"
#include "diagdiag_v.h"
 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

uint8                   err_f3_trace_buffer[DIAG_F3_TRACE_BUFFER_SIZE];

/* Initialize Core F3 trace handler */
diag_f3_trace_handle core_f3_trace = { FALSE, {0},  FALSE, DIAG_F3_TRACE_MSG_TIME, &(err_f3_trace_buffer[0]), 0 , DIAG_F3_TRACE_BUFFER_SIZE};

#ifdef DIAG_UIMAGE_MODE
/* Initialize uImage F3 trace handler */
extern diag_f3_trace_handle uimage_f3_trace;
#endif

extern boolean diag_f3_trace_buffer_init(diag_f3_trace_handle *h);
extern boolean diag_f3_trace_time_init(void);
extern int diag_f3_trace_time_initialized;

void * diag_f3_trace_cmd_handle(void * req_pkt, uint16 pkt_len);

static const diagpkt_user_table_entry_type diag_f3_trace_tbl[] = {
  {DIAG_F3_TRACE_SET_CONFIG, DIAG_F3_TRACE_SET_CONFIG, diag_f3_trace_cmd_handle}
};

/*===========================================================================

FUNCTION diag_f3_trace_cmd_reg

DESCRIPTION
  Registers the command to perform runtime configuration of f3 trace

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void diag_f3_trace_cmd_reg(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC(DIAG_MY_PROC_ID, DIAG_SUBSYS_DIAG_SERV, diag_f3_trace_tbl);
}

/*===========================================================================

FUNCTION diag_f3_trace_tbl

DESCRIPTION
  Handles the command to perform runtime configuration of f3 trace

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void * diag_f3_trace_cmd_handle(void * req_pkt, uint16 pkt_len)
{
  diag_f3_trace_cmd_hdr_req_type *hdr = (diag_f3_trace_cmd_hdr_req_type *)req_pkt;
  void * rsp_pkt = NULL;
  diagpkt_subsys_cmd_code_type cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);

  if(pkt_len < sizeof(diag_f3_trace_cmd_hdr_req_type)) {
    return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len);
  }

  switch(hdr->cmd) {
  case DIAG_F3_TRACE_CONFIG:
    if(pkt_len < sizeof(diag_f3_trace_config_cmd_type)) {
      rsp_pkt = diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    }
    else {
      diag_f3_trace_config_rsp_type *rsp;
      diag_f3_trace_config_cmd_type *cmd = (diag_f3_trace_config_cmd_type *)req_pkt;
      diag_f3_trace_init_configs(cmd->control, cmd->detail);
      rsp = (diag_f3_trace_config_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, cmd_code, sizeof(diag_f3_trace_config_rsp_type));
      if (rsp)
        rsp->hdr.cmd = DIAG_F3_TRACE_CONFIG;
      rsp_pkt = rsp;
    }
    break;
  default:
    rsp_pkt = diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
  }

  return rsp_pkt;
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_INIT

DESCRIPTION
  Initialization function for this module.
  Initializes both core F3 trace handle and (if enabled) uImage F3 trace handle.
  Registers the packet handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_f3_trace_init(void)
{

#if !defined(FEATURE_ERR_F3_TRACE_STANDALONE)

#if defined(ERR_F3_TRACE_TO_SMEM)
   /* SMEM Trace log size needs:
        size of err_f3_trace_record_count (for # of records)
     +
     DIAG_F3_TRACE_BUFFER_SIZE (binary data)
    */
   diag_f3_trace_smem_log_buf =
                              (char* )smem_alloc(SMEM_ERR_F3_TRACE_LOG, sizeof(uint32)+DIAG_F3_TRACE_BUFFER_SIZE);

   //register the function with err
   err_crash_cb_register(diag_f3_trace_save_to_smem);
#endif /* ERR_F3_TRACE_TO_SMEM */

#endif /* if !FEATURE_ERR_F3_TRACE_STANDALONE */

   if (!core_f3_trace.buffer_initialized)
   {
      /* Initialize the buffer */
      diag_f3_trace_buffer_init(&core_f3_trace);
   }
   
   if (!diag_f3_trace_time_initialized)
   {
      /* Initialize the buffer */
      diag_f3_trace_time_init();
   }

#ifdef DIAG_UIMAGE_MODE
   if (!uimage_f3_trace.buffer_initialized)
   {
     /* Initialize the buffer */
     diag_f3_trace_buffer_init(&uimage_f3_trace);
   }
#endif

   diag_f3_trace_cmd_reg();
} /* END diag_f3_trace_init */

#if defined __cplusplus
  }
#endif

