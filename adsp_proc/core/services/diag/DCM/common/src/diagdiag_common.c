/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Common Routines

General Description
  Core diagnostic packet processing routines that are common to all targets.

Copyright (c) 2000-2017, 2019 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagdiag_common.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
05/30/17   gn      Changes to support command deregistration
05/03/17   sp      Resolved KW errors
04/09/17   sp      Added stress tests for extended log APIs in rootPD
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
05/12/16   ph      SMD references removal and code clean up.
07/01/15   ph      Fixed memory leak in stress test framework.
06/18/15   sa      Added support for time sync feature.
04/08/15   ps      Diag over sockets 
11/25/14   ph      Removed test case DIAGDIAG_STRESS_TEST_ERR_FATAL_ISR.
10/09/14   sr      Added stress test in UserPD
08/21/14   sr      Added stress test for SPRINTF 
07/06/14   ph      Added support for Diag over STM
07/24/14   xy      Resolve 8994 Diag warnings with new LLVM 7.0.04 toolchain  
07/16/14   xy      Update Diag delayed response command to take parameter  
01/29/14   sa      Delete the sleep timer created by diag stress test task
                   before the task exits.
01/28/14   ph      Added start signal to be used for handshake with osal
				   in the new task created using osal_create_thread().
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
07/25/13   rh      Added 8.5kB test command
11/22/13   sr      Removed PACK from pointer references
09/27/13   xy      Removed code under FEATURE_WINCE
04/18/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove()
03/01/13   rh      Use correct thresholds for buffering mode
02/11/13   sr      Mainling Diag central routing
02/01/13   sg      Fixed Klock work warnings
03/01/13   sg      Migrated to new MMPM apis
12/07/12   is      Include preset_id in set preset_id response
11/15/12   is      Support for preset mask
08/22/12   sr      Support for mask retrieval command
08/24/12   ra      Support for querying Command Registration Tables
07/18/12   sr      Added processor id to msg stress test
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap. 
02/15/12   sg      Increased stack size to 8k
09/28/11   is      Support dynamic sleep voting and non-deferrable Diag timers
08/03/11   hm      QDSP6 specific change commit threshold command
07/19/11   hm      Non-aggregation feature for events
06/14/11   hm      Migrating to REX opaque TCB APIs
06/13/11   sg      Diag Stress test votes against power collapse
                   on lpass when stress test is running
03/04/11   is      Support for RIVA Health and stress test commands
03/01/11   is      Migrate to diag_strlcpy()
02/15/11   vs      Changed registration for qdsp6 for 9k
02/15/11   hm      QPST Cookie Changes decoupling USB header file
02/01/11   hm      Dual Mask Changes
01/24/11   hm      Added Query/Save QPST Cookie Command handler
01/24/11   is      Define SLEEP_PRI when build does not have sleep feature (decoupling)
12/10/10   vs      strncpy for q6 instead of strlcpy
12/06/10   hm      Added Timestamp to Subsystem loopback command response
12/03/10   hm      Added unique identifier field to fix BVT stress test failures
11/22/10   mad     Initialized name for diagdiag_memop_tbl_mutex
07/27/10   vs      Added diag command to switch encoding protocols
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
08/25/10   sg      Fixed compiler warnings
07/15/10   sg      Moved declaration of diagdiag_memop_tbl_mutex to here
                   from diagdiag.c
07/10/10   vs      Added support for diagpeek/poke registration API
07/10/10   vs      Added check for bad length in command handlers
06/22/10   sg      DIAGDIAG_START_STRESS_TEST_F is defined to the appropriate
                   command id on the processor
06/10/10   is      Core image feature flag cleanup
06/04/10   sg      Fix to provide the correct delayed response drop count
                   when diag health commands for delayed responses are sent
04/05/10   JV      Added test cases for QSR messages in the stress test
03/02/10   JV      Reduced stack size for stress test to 1K. Use system heap
                   instead of diag heap to allocate for stress test stack.
03/02/10   JV      New command to enable/disable data aggregation
02/25/10   JV      New command to flush diagbuf.
02/23/10   JV      Fixed bug in event_stress_test_completed
02/18/10   JV      Fix klocwork errors
02/02/10   JV      Relative priority for the ADSP
01/22/10   mad     Stress-test-task sleeps for 1 s before sending out
                   EVENT_DIAG_STRESS_TEST_COMPLETED. This is to ensure that
                   the event gets allocated, and APS does not miss the event.
01/20/10   sg      Relative Priority Changes For Win Mobile
01/18/10   mad     Sending out EVENT_DIAG_STRESS_TEST_COMPLETED before marking
                   stress-test-task for clean-up.
12/17/09   JV      Mainlining the diagdiag stress test and other functioanlity
                   under the DEBUG_DIAG_TEST feature.
12/17/09   JV      Switching over to BLAST APIs in OSAL
12/16/09   cahn    Corrected EVENT_DIAG_STRESS_TEST_COMPLETED event generation
                   logic in support of test automation.
12/09/09   cahn    Fixed stress test on QDSP6 processor. Removed mutexes and
                   resolved compilation error.
12/07/09   cahn    Added Delayed Response to DIAG Health.
12/02/09   cahn    Fixed bug in DIAGDIAG_STRESS_TEST_MSG_1.
12/02/09   JV      Fix for warning that is treated as an error in WM.
11/12/09   cahn    Added variable length log types for DIAG stress test.
11/09/09   cahn    Defined DIAG_DEBUG_TEST feature. Made necessary code changes
                   to avoid compilation errors.
11/04/09   cahn    DIAG Health
09/25/09   sg      Cleaned up diagdiag.c and moved the functions specific to modem
                   to diagdiag_brew.c and removed dip switches
10/22/09   JV      Include queue.h
10/06/09   cahn    Added EVENT_DIAG_STRESS_TEST_COMPLETED for automation support.
10/06/09   vs      Added payload to LOG_STRESS_TEST_C_type
09/15/09   cahn    Added loopback on individual processors.
09/09/09   JV      Changed signature of osal mutex calls.
08/11/09   JV      Set the malloced memory to 0 in the stress test.
08/07/09   vs      Added DIAG_SINGLE_PROC for targets that don't define
                    IMAGE_MODEM_PROC, but are single proc targets.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
08/05/09   JV      task.h is now included in osal.
07/31/09   JV      Removed all the KxMutex calls
07/31/09   JV      Merged Q6 diag code back to mainline
07/10/09    as     Mainlined code under FEATURE_DIAG_HW_ADDR_CHECK
05/12/09    JV     Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
01/15/09    as     Decoupled Err services packets.
10/03/08   vg      Updated code to use PACK() vs. PACKED
11/19/07    pc     Use dword transfers for PEEKD/POKED if address is aligned.
11/19/07    pc     Use word transfers for PEEKW/POKEW if address is aligned.
12/15/06    as     Fixed compiler warnings.
10/31/05    as     Fixed lint errors.
05/19/05    as     Fixed argument name in DIAG POKE DWORD
22/03/05    as     Added type cast in memory poke operations
06/11/04    gr     Added support for event masks.
05/18/04    as     Added security check to diagdiag_get_property and
                   diagdiag_put_property. Removed support for DIAG_USER_CMD_F
                   & DIAG_PERM_USER_CMD_F
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
04/07/03   jct     Added featurization to dissallow certain operations that
                   can access memory (FEATURE_DIAG_DISALLOW_MEM_OPS):
                      DIAG_PEEKB_F
                      DIAG_PEEKW_F
                      DIAG_PEEKD_F
                      DIAG_POKEB_F
                      DIAG_POKEW_F
                      DIAG_POKED_F
                      DIAG_OUTP_F
                      DIAG_OUTPW_F
                      DIAG_INP_F
                      DIAG_INPW_F
01/07/03   djm     add FEATURE_DIAG_RPC support for WCDMA_PLT use
08/20/02   lad     Moved diagpkt_error() to diagpkt.c.
                   Moved diagpkt_mode_chg() to diagcomm_sio.c as
                   diagcomm_mode_chg_pkt().
                   Removed msg_nice[] and log_nice[] references pending a
                   redesign of the load balancing feature.
                   Moved diagpkt_dload() to diagdload.c.
                   Featurized outp/inp routines for off target removal.
06/07/02   lad     Added DIAG_PROTOCOL_LOOPBACK_F.
11/01/01   jal     Support for DIAG_SERIAL_CHG_F (switch to data mode)
09/18/01   jal     Support for DIAG_CONTROL_F (mode reset/offline)
08/20/01   jal     Support for Diag packet: DIAG_TS_F (timestamp),
                   DIAG_SPC_F (service programming code), DIAG_DLOAD_F
                   (start downloader), DIAG_OUTP_F/DIAG_OUTPW_F (IO
                   port byte/word output), DIAG_INP_F/DIAG_INPW_F (IO
                   port byte/word input)
06/27/01   lad     Moved diagpkt_err_rsp() to diagpkt.c.
                   Updated diagpkt_stream_config() for logging service.
                   diagpkt_stream_config() can now return DIAGBUF_SIZ.
04/17/01   lad     Added #include of diagtune.h.
04/06/01   lad     Introduced typedefs for command codes, etc.
                   Updated DIAGPKT_SUBSYS_REQ_DEFINE macros.
                   Added diagpkt_subsys_alloc().
                   Removed diagpkt_process_request since it is not part of the
02/23/01   lad     Created file.

===========================================================================*/
#if defined __cplusplus
  extern "C" {
#endif

#include "diag_f3_tracei.h"
#include "customer.h"

#include "eventi.h"
#include "diaglogi.h"
#include "msgi.h"
#include "comdef.h"
#include "diag_v.h"
#include "diagcmd.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagtarget.h"
#include "event.h"
#include "err.h"
#include "diagcomm_v.h"
#include "diagbuf_v.h"
#include "diag_cfg.h"
#include "stringl.h"
#ifdef DIAG_UIMAGE_MODE
#include "micro_diagbuffer.h"
#endif
#if defined (DIAG_DYNAMIC_SLEEP)
  #include "npa.h" /* For npa_client_handle, etc */
#endif

#if defined(DIAG_QDSP6_APPS_PROC)
#include "diagstub.h" /* For sleep Priority*/
#if defined(DIAG_MMPM_SUPPORT )
#include "diagMmpm.h"
#endif
#endif

#include "assert.h"
#include <stdlib.h>

#include "msg.h"
#include "qw.h"
#include "queue.h"

#include "osal.h"

/* DLOAD QPST COOKIE CHANGES */
#ifdef DIAG_FEATURE_QPST_COOKIE
  #include "msm.h"
  #define QPST_COOKIE_FILE_NAME "/qpst_cookie.cke"
#endif

#if defined (DIAG_FEATURE_EFS2)
  #include "fs_public.h"
#endif

#ifdef FEATURE_DIAG_STRESS_PROFILE
  #include "qurt_cycles.h"
  #include "diag_stm.h"
  extern uint64 diag_sigs_pcycle_total,diag_hdlc_pcycles_total, diag_tracer_pcycles_total;

#endif /* FEATURE_DIAG_STRESS_PROFILE */
#ifdef DIAG_UIMAGE_MODE
#include "micro_msg_diag_service.h"
#endif
/* Internal APIs to handle file system access */
boolean diagdiag_file_write(char *file_name, void *buf, int length);
boolean diagdiag_file_read(char *file_name, void *buf, int length, int offset);

void * diag_dcm_subsys_reg_test (void * req_pkt, uint16 pkt_len);
void * diag_dcm_subsys_reg_test2 (void * req_pkt, uint16 pkt_len);
void * diag_dcm_subsys_reg_test3 (void * req_pkt, uint16 pkt_len);
void * diag_dcm_subsys_reg_test4 (void * req_pkt, uint16 pkt_len);

static const diagpkt_user_table_entry_type diag_dcm_test_reg_tbl_1[] =
{
   {DIAG_DCM_SUBSYS_CMD_REG_TEST, DIAG_DCM_SUBSYS_CMD_REG_TEST, diag_dcm_subsys_reg_test},
   {DIAG_DCM_SUBSYS_CMD_REG_TEST2, DIAG_DCM_SUBSYS_CMD_REG_TEST2, diag_dcm_subsys_reg_test2}   	
};

static const diagpkt_user_table_entry_type diag_dcm_test_reg_tbl_2[] =
{
   {DIAG_DCM_SUBSYS_CMD_REG_TEST3, DIAG_DCM_SUBSYS_CMD_REG_TEST3, diag_dcm_subsys_reg_test3},
};

static const diagpkt_user_table_entry_type diag_dcm_test_reg_tbl_3[] =
{
   {DIAG_DCM_SUBSYS_CMD_REG_TEST4, DIAG_DCM_SUBSYS_CMD_REG_TEST4, diag_dcm_subsys_reg_test4},
};

/* Tell lint that we are OK with the fact that pkt_len and req are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req) */

osal_thread_attr_t diagdiag_stress_attr;

osal_thread_attr_t diag_cmd_req_attr;
int thread_exit_status = 0;
osal_mutex_arg_t diagdiag_memop_tbl_mutex;

#if defined (DIAG_DYNAMIC_SLEEP)
extern npa_client_handle diagtest_npa_handle;
#endif

/* Time Get Function Pointer from diag.c */
extern uint8 (*diag_time_get)(qword time);

#ifdef DIAG_UIMAGE_MODE
extern micro_diagbuf_mpd_type* micro_diagmpd_buf;
#endif

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/*
  These are defined for Querying the Apps processor for registration
  tables.

  The slave table is identified by the port number. Apps stores the
  registration tables of the other processors in the slave table and
  differentiates them by the port number
*/

#if defined (DIAG_IMAGE_APPS_PROC)
  #define DIAGDIAG_MODEM_PORT     0x00  /* For quering the Modem Registration Table on the Apps */
  #define DIAGDIAG_QDSP6_PORT     0x01  /* For quering the QDSP6 Registration Table on the Apps */
  #define DIAGDIAG_RIVA_PORT      0x02  /* For quering the RIVA Registration Table on the Apps */
#endif  /* #if defined (DIAG_IMAGE_APPS_PROC) */

#define DIAGDIAG_NO_PORT        0x00  /* This is a dummy value. We don't need port number for querying Master Table */

/* Variables for Registration Table Retrieval commands */
diagpkt_subsys_cmd_code_type reg_tbl_cmd_code;  /*  Command Code */
diagpkt_subsys_delayed_rsp_id_type reg_tbl_delay_rsp_id = 0; /* Retrieval Delayed Response ID */
boolean reg_tbl_is_master = TRUE; /* Flag for master processor */
uint8 reg_tbl_port_id = DIAGDIAG_NO_PORT; /* Port ID to identify processor in Slave Table */
uint8 reg_tbl_proc_id = 0;

/* Queue for diag stress test */
q_type diag_stress_test_task_q;

/* Stores info, wheather Q is initialised or not */
boolean diag_stress_test_task_q_init = FALSE;

/* Predetermined sequence */
byte pseudo_array[] = {6,2,4,5,3,0,1,2,5,0,3,1,6,4};

/* for repeatabilty use pseudo random numbers */
int pseudo_random(int i)
{
  i=i%(sizeof(pseudo_array)/sizeof(byte));
  if(i<14)
    return pseudo_array[i];
  else
    return 0;
}

extern int diagbuf_commit_threshold;
extern unsigned int event_report_pkt_size;

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];

/*===========================================================================

FUNCTION DIAGDIAG_FILE_WRITE

DESCRIPTION
    This procedure writes the data in "buf" of length "length" to the
    file "file_name". New file will be created if the file is not present.
    Overwritten if the file is present.

RETURN VALUE
    Returns TRUE on success and FALSE on failure
=============================================================================*/
boolean diagdiag_file_write(char *file_name, void *buf, int length)
{
  boolean return_val = FALSE;
  #ifdef DIAG_FEATURE_EFS2
  int file_descriptor, wr_bytes;
  if(buf == NULL)
  {
    return return_val;
  }
  file_descriptor = efs_open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR);
  if(file_descriptor == -1) /* Error opening file */
  {
    return return_val;
  }
  wr_bytes = efs_write(file_descriptor, buf, length);
  if(wr_bytes == -1) /* Error Writing to file */
  {
    (void)efs_close(file_descriptor);
    return return_val;
  }
  (void)efs_close(file_descriptor);
  return (return_val = TRUE);
  #endif

  return return_val;
} /* diagdiag_file_write */

/*===========================================================================

FUNCTION DIAGDIAG_FILE_READ

DESCRIPTION
    This procedure reads the data of length "length" from the file
    "file_name" to the buffer "buf". "offset" indicates the offset
    from the begining of the file to read the data from.

RETURN VALUE
    Returns TRUE on success and FALSE on failure
=============================================================================*/
boolean diagdiag_file_read(char *file_name, void *buf, int length, int offset)
{
  boolean return_val = FALSE;
  #ifdef DIAG_FEATURE_EFS2
  int file_descriptor, rd_bytes;
  if(buf == NULL)
  {
    return return_val;
  }
  file_descriptor = efs_open(file_name, O_RDONLY);
  if(file_descriptor == -1)
  {
    return return_val;
  }
  if(offset != 0)
  {
    rd_bytes = efs_lseek(file_descriptor, offset, SEEK_SET);
    if(rd_bytes != offset)
    {
      (void)efs_close(file_descriptor);
      return return_val;
    }
  }
  rd_bytes = efs_read(file_descriptor, buf, length);
  if(rd_bytes == -1 )
  {
    (void)efs_close(file_descriptor);
    return return_val;
  }
  (void)efs_close(file_descriptor);
  return (return_val = TRUE);
  #endif

  return return_val;
} /* diagdiag_file_read */

/*lint -save -e729 */
/* Symbol 'diag_prop_table' not explicitly initialized */
/* Also there is no reference to filling this table.  */

/*===========================================================================

FUNCTION DIAGDIAG_LOOPBACK

DESCRIPTION
  This procedure echos the request in the response.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_loopback (
  void * req_pkt,
  uint16 pkt_len
)
{
  void * rsp = NULL;

  /* Allocate the same length as the request. */
  rsp = diagpkt_alloc (DIAG_PROTOCOL_LOOPBACK_F, pkt_len);

  if (rsp != NULL) {
    memscpy ((void *) rsp, pkt_len, (void *) req_pkt, pkt_len);
  }

  return (rsp);
}




/*===========================================================================

FUNCTION DIAGDIAG_SUBSYS_LOOPBACK

DESCRIPTION
  This procedure echos the request in the response. This function has equivalent
  functionality to DIAGDIAG_LOOPBACK, except it is used for subsystem commands
  with subsystem dispatch (i.e. cmd code 75).

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_subsys_loopback (
  void * req_pkt,
  uint16 pkt_len
)
{
  /*=========================================================================
   *        RESPONSE PACKET STRUCTURE (Length in Bytes)
   *
   *       CMD_CODE SUBSYS_ID SUBSYS_CMD_CODE          PAYLOAD
   *                                                            TIMESTAMP     ^      STRING
   *      -------------------------------------------------------------------
   *    |             |             |                |                    |                      |
   *    |     1       |      1       |       2        |          8          |   VARIABLE    |
   *    -------------------------------------------------------------------
   *
   *========================================================================*/



  diag_subsys_loopback_rsp_type *rsp = NULL;
  byte * p = NULL;
  byte * req_pkt_byte = NULL;

  #if defined(DIAG_QDSP6_APPS_PROC)
  diagpkt_subsys_header_type *req = (diagpkt_subsys_header_type *) req_pkt;
  #else
  diag_log_event_listener_req_type *req =
    (diag_log_event_listener_req_type *) req_pkt;
  #endif
  diagpkt_subsys_cmd_code_type cmd_code = diagpkt_subsys_get_cmd_code (req);

  /* Allocate the same length as the request. */
  pkt_len = pkt_len + 1 + 8;  // Timestamp needs 8 bytes
  rsp = (diag_subsys_loopback_rsp_type *)diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, cmd_code, pkt_len);

  if (rsp != NULL) {

    rsp->header.opaque_header[0] = *((byte *)req_pkt);  // Copy the CMD_CODE (1 Byte)
    /*===========================================================================
     * We don't have to copy the Subsys Cmd_code and Subsys_ID. They are already
     * copied to the response packet in diagpkt_subsys_alloc function. Get the
     * timestamp directly copied into the response packet.
     *==========================================================================*/
    diag_time_get((unsigned long *)(rsp->ts));

    /*===========================================================================
     * Copying the string which is 4 + 8 bytes away from the base in the reponse
     * packet, 4 bytes away from the base in the request packet as there is no
     * timestamp in the request.
     *==========================================================================*/

    p = (byte *)rsp;
    req_pkt_byte = (byte *)req_pkt;
    memscpy ((void *) (p+12), (pkt_len-1)-12, (void *) (req_pkt_byte+4), (pkt_len-1)-12);
    *(p+(pkt_len-1)) = '\0';
  }

  return (void *) rsp;
}
/*===========================================================================

FUNCTION DIAGDIAG_DELAYED_RSP_TEST

DESCRIPTION
  This procedure tests the delayed response functionality. It sends an   
  immediate response followed by multiple delayed responses. The number
  of delayed rsp generated is passed in by cmd.
   

RETURN VALUE
  None.

============================================================================*/
void *diagdiag_delayed_rsp_test (
  void * req_pkt,
  uint16 pkt_len
)
{
  diagdiag_delayed_rsp_test_req_type *req = NULL;
  uint32 delayed_rsp_id = 0;
  uint16 num = 0;
  int i = 0;
  diagdiag_delayed_rsp_test_rsp_type *rsp = NULL;

  if (pkt_len < sizeof(diagdiag_delayed_rsp_test_req_type)) 
  {
     return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  req = (diagdiag_delayed_rsp_test_req_type *)req_pkt;
  
  if (req != NULL) 
  {
     num = req -> num_rsp;

     if (num == 0 || num > 0x0FFF) 
     {
       return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
     }

     rsp = (diagdiag_delayed_rsp_test_rsp_type *)
            diagpkt_subsys_alloc_v2(DIAG_SUBSYS_DIAG_SERV, DIAGDIAG_STRESS_TEST_DELAYED_RSP, 
                                                 sizeof(diagdiag_delayed_rsp_test_rsp_type));
     if(rsp)
     {
       delayed_rsp_id = rsp->delayed_rsp_id;
       /*According to ICD, rsp count always starts at 0x8000*/
       rsp->response_cnt = 0x8000;
       diagpkt_commit(rsp);
     }
     else
     {
       MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Out of memory- Cannot send the response");
     }

     for (i = 0;i < num; i++) {

        rsp = NULL;       
        rsp = (diagdiag_delayed_rsp_test_rsp_type *)diagpkt_subsys_alloc_v2_delay(DIAG_SUBSYS_DIAG_SERV, DIAGDIAG_STRESS_TEST_DELAYED_RSP, 
                                                                                  delayed_rsp_id, sizeof(diagdiag_delayed_rsp_test_rsp_type));
        if(rsp)
        {
           if (i == (num -1)) 
           {
             /*If the packet is the last delayed response, set the response_cnt to be x0nnn*/
             rsp->response_cnt = i+1;
           }
           else
           {
             rsp->response_cnt = i+1+0x8000;
           }
           diagpkt_delay_commit(rsp);
        }
        else
        {
          MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Out of memory- Cannot send the delayed response");
        }

     }
  }
  return NULL;
}

/*===========================================================================

FUNCTION DIAGDIAG_HEALTH_COMMAND

DESCRIPTION
  This procedure performs operations on the global variables used for diag
  health based on the command code.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_health_command(
  void * req_pkt,
  uint16 pkt_len
)
{
  /* Initialization */
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_health_response_type *rsp = NULL;

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);
  rsp = (diag_health_response_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                              cmd_code, sizeof(diag_health_response_type));

  if ( rsp != NULL )
  {
    switch ( cmd_code )
    {
	  case DIAGDIAG_RESET_DROP_COUNT_LOG:
          diagbuf_reset_drop_count_log();
          rsp->drop_count = diagbuf_get_drop_count_log();
          break;
	  case DIAGDIAG_GET_DROP_COUNT_LOG:
          rsp->drop_count = diagbuf_get_drop_count_log();
          break;
	  case DIAGDIAG_RESET_DROP_COUNT_EVENT:
          event_reset_drop_count_event();
          rsp->drop_count = event_get_drop_count_event();
          break;
	  case DIAGDIAG_GET_DROP_COUNT_EVENT:
          rsp->drop_count = event_get_drop_count_event();
          break;
	  case DIAGDIAG_RESET_DROP_COUNT_F3:
          diagbuf_reset_drop_count_f3();
          rsp->drop_count = diagbuf_get_drop_count_f3();
          break;
	  case DIAGDIAG_GET_DROP_COUNT_F3:
          rsp->drop_count = diagbuf_get_drop_count_f3();
          break;
	  case DIAGDIAG_RESET_DROP_COUNT_DELAY:
          diagpkt_reset_drop_count_delay();
          rsp->drop_count = diagpkt_get_drop_count_delay();
          break;
	  case DIAGDIAG_GET_DROP_COUNT_DELAY:
          rsp->drop_count = diagpkt_get_drop_count_delay();
          break;
	  case DIAGDIAG_GET_ALLOC_COUNT_LOG:
          rsp->drop_count = diagbuf_get_alloc_count_log();
          break;
	  case DIAGDIAG_GET_ALLOC_COUNT_EVENT:
          rsp->drop_count = event_get_alloc_count_event();
          break;
	  case DIAGDIAG_GET_ALLOC_COUNT_F3:
          rsp->drop_count = diagbuf_get_alloc_count_f3();
          break;
	  case DIAGDIAG_GET_ALLOC_COUNT_DELAY:
          rsp->drop_count = diagpkt_get_alloc_count_delay();
          break;
    }
  }
  return rsp;

} /* diagdiag_health_command */

#ifdef DIAG_FEATURE_QPST_COOKIE
/*===========================================================================

FUNCTION DIAGDIAG_SAVE_QPST_COOKIE

DESCRIPTION
  This procedure handles the command to save qpst cookie.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_save_qpst_cookie(
  void * req_pkt,
  uint16 pkt_len
)
{
    diag_save_qpst_cookie_req_type *req = (diag_save_qpst_cookie_req_type *)req_pkt;
    diag_save_qpst_cookie_rsp_type *rsp = NULL;

    byte *temp;
    boolean flag;

    if(req->cmd_version != 1 || req->cookie_length > DLOAD_QPST_COOKIE_SIZE)
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    }

    /* Check if the cookie_length and the actual length of the cookie given are the same  */
    if((pkt_len - FPOS(diag_save_qpst_cookie_req_type, cookie)) != req->cookie_length)
    {
      return diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len);
    }
    rsp = (diag_save_qpst_cookie_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
           DIAG_SAVE_QPST_COOKIE, sizeof(diag_save_qpst_cookie_rsp_type));
    if(rsp == NULL)
    {
      return rsp;
    }
      rsp->header       = req->header;
      rsp->cmd_version  = req->cmd_version;

      /* Write to IRAM */
      temp  = (byte *)DLOAD_QPST_COOKIE_ADDR;
      *temp = req->cookie_length;
      temp++;
      memscpy((void *)temp, req->cookie_length, (void *)(req->cookie), req->cookie_length);

   #if !defined(DIAG_FEATURE_EFS2)
      rsp->error_code   = COOKIE_SAVE_TO_FS_ERROR;
      return rsp;
   #else
      flag = diagdiag_file_write(QPST_COOKIE_FILE_NAME, (void *)&(req->cookie_length),
                              req->cookie_length + FSIZ(diag_save_qpst_cookie_req_type, cookie_length));
      if(!flag)
      {
          rsp->error_code = COOKIE_SAVE_TO_FS_ERROR;
          return rsp;
      }

      rsp->error_code = COOKIE_SAVE_SUCCESS;
      return rsp;
   #endif

} /* diagdiag_save_qpst_cookie */

/*===========================================================================

FUNCTION DIAGDIAG_QUERY_QPST_COOKIE

DESCRIPTION
  This procedure handles the command to query qpst cookie.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_query_qpst_cookie(
  void * req_pkt,
  uint16 pkt_len
)
{
    diag_query_qpst_cookie_req_type *req = (diag_query_qpst_cookie_req_type *)req_pkt;
    diag_query_qpst_cookie_rsp_type *rsp = NULL;
    int rsp_len;
    byte *temp, cookie_length;
    boolean flag;

    if(req->cmd_version != 1)
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    }

    /* If there is no file system, read the cookie from IRAM */
 #ifndef DIAG_FEATURE_EFS2
    temp  = (byte *)DLOAD_QPST_COOKIE_ADDR;
    cookie_length = *temp;
    temp++;
    rsp_len = FPOS(diag_query_qpst_cookie_rsp_type, cookie) + cookie_length;
    rsp = (diag_query_qpst_cookie_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
             DIAG_QUERY_QPST_COOKIE, rsp_len);
    if(rsp == NULL)
    {
      return rsp;
    }
    memscpy((void *)(rsp->cookie), cookie_length,(void *)temp, cookie_length);
    rsp->header         = req->header;
    rsp->cmd_version    = req->cmd_version;
    rsp->error_code     = COOKIE_READ_SUCCESS;
    rsp->cookie_length  = cookie_length;
    return rsp;

 #else /* DIAG_FEATURE_EFS2 */

    flag = diagdiag_file_read(QPST_COOKIE_FILE_NAME, (void *)&cookie_length, FSIZ(diag_query_qpst_cookie_rsp_type, cookie_length), 0);
    if(!flag)
    {
      rsp_len = sizeof(diag_query_qpst_cookie_rsp_type);
      rsp = (diag_query_qpst_cookie_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
             DIAG_QUERY_QPST_COOKIE, rsp_len);
      if(rsp == NULL)
      {
        return rsp;
      }
      rsp->header         = req->header;
      rsp->cmd_version    = req->cmd_version;
      rsp->error_code     = COOKIE_READ_ERROR;
      rsp->cookie_length  = 0;
      rsp->cookie[0]      = 0;
      return rsp;
    }

    /* Compute the reponse length based on the cookie length */
    rsp_len   = FPOS(diag_query_qpst_cookie_rsp_type, cookie) + cookie_length;
    rsp = (diag_query_qpst_cookie_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
             DIAG_QUERY_QPST_COOKIE, rsp_len);

    if(rsp == NULL)
      {
        return rsp;
      }
    rsp->header         = req->header;
    rsp->cmd_version    = req->cmd_version;
    rsp->cookie_length  = cookie_length;
    flag = diagdiag_file_read(QPST_COOKIE_FILE_NAME, (void *)(rsp->cookie), cookie_length,
                                                  FSIZ(diag_query_qpst_cookie_rsp_type, cookie_length));
    if(!flag)// File read error
    {
      rsp->error_code     = COOKIE_READ_ERROR;
      return rsp;
    }
    rsp->error_code     = COOKIE_READ_SUCCESS;
    return rsp;

 #endif /* DIAG_FEATURE_EFS2 */

} /* diagdiag_query_qpst_cookie */

#endif /* DIAG_FEATURE_QPST_COOKIE */


/*===========================================================================

FUNCTION DIAGDIAG_HEALTH_FLOW_CTRL

DESCRIPTION
  This procedure performs operations on Diag health flow control count requests.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_health_flow_ctrl(
  void * req_pkt,
  uint16 pkt_len
)
{
  /* Initialization */
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_health_get_flow_ctrl_rsp_type *rsp = NULL;

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);
  rsp = (diag_health_get_flow_ctrl_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
         cmd_code, sizeof(diag_health_get_flow_ctrl_rsp_type));

  if( rsp != NULL )
  {
    switch ( cmd_code )
    {
      case DIAGDIAG_RESET_FLOW_CTRL_COUNT:
          diagcomm_reset_flow_ctrl_count();
          rsp->version = DIAG_HEALTH_FLOW_CTRL_VER;
          rsp->count[DIAGCOMM_PORT_REMOTE_PROC] = diagcomm_get_flow_ctrl_count(DIAGCOMM_PORT_REMOTE_PROC);
          rsp->count[DIAGCOMM_PORT_EXTERNAL] = 0;
          break;
      case DIAGDIAG_GET_FLOW_CTRL_COUNT:
          rsp->version = DIAG_HEALTH_FLOW_CTRL_VER;
          rsp->count[DIAGCOMM_PORT_REMOTE_PROC] = diagcomm_get_flow_ctrl_count(DIAGCOMM_PORT_REMOTE_PROC);
          rsp->count[DIAGCOMM_PORT_EXTERNAL] = 0;
          break;
    }
  }

  return rsp;

} /* diagdiag_health_flow_ctrl */

/*===========================================================================

FUNCTION DIAGDIAG_SET_PRESET_CMD

DESCRIPTION

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_set_preset_cmd( void * req_pkt, uint16 pkt_len )
{
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_set_preset_id_rsp_type *rsp = NULL;
  diag_set_preset_id_req_type * req = (diag_set_preset_id_req_type *)req_pkt;

  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);

  if( (cmd_code != DIAGDIAG_SET_PRESET_ID_APP) || (req_pkt == NULL) || (pkt_len !=sizeof(diag_set_preset_id_req_type)))
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);


  rsp = (diag_set_preset_id_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
         cmd_code, sizeof(diag_set_preset_id_rsp_type));
  if( rsp )
  {
    rsp->preset_id = req->preset_id;
    rsp->error = diag_set_current_preset_mask_id( req->preset_id );
  }

  return rsp;

} /* diagdiag_set_preset_cmd */


/*===========================================================================

FUNCTION DIAGDIAG_GET_PRESET_CMD

DESCRIPTION

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_get_preset_cmd( void * req_pkt, uint16 pkt_len )
{
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_get_preset_id_rsp_type *rsp = NULL;

  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);

  if( (cmd_code != DIAGDIAG_GET_PRESET_ID_APP) || (req_pkt == NULL) || (pkt_len !=sizeof(diag_get_preset_id_req_type)))
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);


  rsp = (diag_get_preset_id_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
         cmd_code, sizeof(diag_get_preset_id_rsp_type));
  if( rsp )
  {
    rsp->preset_id = diag_get_current_preset_mask_id();
  }

  return rsp;

} /* diagdiag_get_preset_cmd */


/*===========================================================================

FUNCTION DIAGDIAG_FLUSH_BUFFER_HANDLER

DESCRIPTION
  This procedure flushes diagbuf to send out TX traffic.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_flush_buffer_handler(
  void * req_pkt,
  uint16 pkt_len
)
{
  void * rsp = NULL;

  if (pkt_len != sizeof (diagpkt_subsys_header_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }
  /* Allocate the same length as the request. */
  rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_FLUSH_BUFFER, sizeof(diagpkt_subsys_header_type));

  if (rsp)
  {
    if(diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE)
    {
      diag_tx_notify();
    }
  }
  return (rsp);
}

/*===========================================================================

FUNCTION DIAGDIAG_HEALTH_COMMAND

DESCRIPTION
  This procedure flushes diag buffers to send out any TX traffic.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_change_threshold(
  void * req_pkt,
  uint16 pkt_len
)
{
  diagdiag_change_threshold_req_type *req = (diagdiag_change_threshold_req_type *) req_pkt;
  diagdiag_change_threshold_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof( diagdiag_change_threshold_rsp_type );
  static unsigned int event_rpt_size_temp = 0;

  event_rpt_size_temp = event_report_pkt_size;
  if (pkt_len != sizeof (diagdiag_change_threshold_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp = (diagdiag_change_threshold_rsp_type *)diagpkt_subsys_alloc
                            (DIAG_SUBSYS_DIAG_SERV, DIAG_CHANGE_THRESHOLD, rsp_len);
  if (rsp)
  {
    if (!(req->enable_aggregation))
    {
      diagbuf_commit_threshold = 0;
      event_rpt_size_temp = event_report_pkt_size;
      event_report_pkt_size = 0;
    }
    else
    {
      diagbuf_commit_threshold = DIAGBUF_COMMIT_THRESHOLD;
      event_report_pkt_size = event_rpt_size_temp;
    }
  }
  return (rsp);
}


/*===========================================================================

FUNCTION DIAGDIAG_CHANGE_ENC_PROTOCOL

DESCRIPTION
  This procedure changes the encryption protocol that diag uses for TX traffic.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void * diagdiag_change_enc_protocol(
  void * req_pkt,
  uint16 pkt_len
)
{
  diagdiag_change_enc_protocol_req_type *req = (diagdiag_change_enc_protocol_req_type *) req_pkt;
  diagdiag_change_enc_protocol_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof( diagdiag_change_threshold_rsp_type );

  if (pkt_len != sizeof (diagdiag_change_enc_protocol_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp = (diagdiag_change_enc_protocol_rsp_type *)diagpkt_subsys_alloc
                            (DIAG_SUBSYS_DIAG_SERV, DIAG_CHANGE_ENC_PROTOCOL, rsp_len);
  if (rsp)
  {
    if (req->disable_hdlc)
    {
      diag_hdlc_protocol = FALSE;
    }
    else
    {
      diag_hdlc_protocol = TRUE;
    }
    rsp->disable_hdlc = req->disable_hdlc;
  }
  return (rsp);
}


/*============================================================================*/
/* Queue item for diag stress test */
typedef struct
{
  q_link_type link;
  unsigned int seq_num;
  diag_stress_test_status_enum_type status; /* refer to diag_stress_test_status_enum_type*/

  osal_tcb_t tcb;                         /* used to define a task */
  osal_stack_word_type stack[STRESS_TEST_STACK_SIZE];    /* stack needed by the task */

  diagdiag_cmd_req_type req;
}
diag_cmd_req_q_item_type;

/* Queue for diag stress test */
q_type diag_cmd_req_q;

/*===========================================================================

FUNCTION DIAG_CMD_RSP_HANDLER

DESCRIPTION The event has been removed from listening
============================================================================*/
void diagdiag_cmd_rsp_handler (const byte * rsp,
                               unsigned int length,
                               void *param)
{
  MSG_2 (MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Received rsp:0x%X num:%d",
            (rsp) ? *((uint32 *) rsp) : 0, (unsigned int) param);
}

/*===========================================================================

FUNCTION DIAG_CMD_REQ_TASK_STARTUP

DESCRIPTION This procedure calls the diag cmd req
============================================================================*/
void diag_cmd_req_task_startup (uint32 params)
{
  diag_cmd_req_q_item_type *req = (diag_cmd_req_q_item_type *) params;
  boolean success = FALSE;
  int return_val;
  
  return_val=osal_handshake_start(&(req->tcb));
  ASSERT(OSAL_SUCCESS == return_val);

  MSG_2 (MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Injecting req:0x%08X num:%d",
         *(req->req.req), req->seq_num);

  success = diag_cmd_req (req->req.req, req->req.length,
                         diagdiag_cmd_rsp_handler, (void *) req->seq_num);

  MSG_3 (MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Injected req:0x%08X num:%d success:%d",
           *(req->req.req), req->seq_num, success);

   if (osal_thread_get_pri () <= DIAG_PRI)
  {
    /* This is done to make sure that the task gets killed before the
     cleanup function, running in diag context, removes the data from the Q */
     (void) osal_thread_set_pri (DIAG_PRI+1);
  }

  req->status = DIAG_TASK_DONE;
}

/*===========================================================================

FUNCTION DIAG_CMD_REQ_CLEANUP_CB

DESCRIPTION This procedure is a cleanup function. It goes through the
  diag_cmd_req_q, removes the entry from the Q and frees the memory.

============================================================================*/
void
diag_cmd_req_cleanup_cb (void *param)
{
  diag_cmd_req_q_item_type *q_cur_ptr = NULL;
  diag_cmd_req_q_item_type *q_next_ptr = NULL;

  q_cur_ptr = (diag_cmd_req_q_item_type *) q_check (&diag_cmd_req_q);

  while (q_cur_ptr != NULL)
  {
    if (q_cur_ptr->status == DIAG_TASK_DONE)
    {
      /* store the next link */
      q_next_ptr = (diag_cmd_req_q_item_type *)
        q_next (&diag_cmd_req_q, &q_cur_ptr->link);

      /* Remove the data block from the Q */
      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete (&diag_cmd_req_q, &q_cur_ptr->link);
      #else
      q_delete(&q_cur_ptr->link );
      #endif

      /* Free the memory */
      diag_free (q_cur_ptr);

      if (q_cur_ptr != NULL)
      {
        q_cur_ptr = NULL;
      }

      q_cur_ptr = q_next_ptr;
    }
    else
    {
      /* store the next link */
      q_cur_ptr = (diag_cmd_req_q_item_type *)
        q_next (&diag_cmd_req_q, &q_cur_ptr->link);
    }
  }

  /* Deregister's when the count is 0(Q is empty) */
  if (diag_cmd_req_q.cnt == 0)
  {
    (void) diag_idle_processing_unregister (diag_cmd_req_cleanup_cb, &diag_cmd_req_q);
  }

  return;
}

/*===========================================================================

FUNCTION DIAGDIAG_CMD_REQUEST_HANDLER

DESCRIPTION
  This procedure sends a response to acknowledge that the test has been started.
  It then defines task with a requested packet. The task startup functions calls
  the diag_cmd_req function with the packet sent.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void *
diagdiag_cmd_request_handler (void * req_pkt, uint16 pkt_len)
{
  diagdiag_cmd_rsp_type *rsp;
  diagdiag_cmd_req_type *req = (diagdiag_cmd_req_type *) req_pkt;

  const int rsp_len = pkt_len;

  /*  used to store and retrive data block from the Q */
  diag_cmd_req_q_item_type *q_item = NULL;
  static unsigned int seq_num = 0;
  static boolean diag_cmd_req_q_init = FALSE;

  if (pkt_len <= FPOS(diagdiag_cmd_req_type, req))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  if(req->length != (pkt_len - FPOS(diagdiag_cmd_req_type, req)))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /* Allocate the same length as the request */
  rsp = (diagdiag_cmd_rsp_type *) diagpkt_subsys_alloc (
    DIAG_SUBSYS_DIAG_SERV,
    DIAGDIAG_CMD_REQUEST_F,
    rsp_len);

  /* Send the response acknowledging that the packet has been started */
  if (rsp != NULL)
  {
    memscpy ((void *) rsp, rsp_len, (void *) req, rsp_len);
    rsp->length = pkt_len - FPOS (diagdiag_cmd_rsp_type, req);

    if (diag_cmd_req_q_init == FALSE)
    {
      (void) q_init (&diag_cmd_req_q);
      diag_cmd_req_q_init = TRUE;
    }

    /* Get memory from the diag heap */
    q_item = (diag_cmd_req_q_item_type *)
      diag_malloc (FPOS(diag_cmd_req_q_item_type, req) + pkt_len);
	
	

    if (q_item != NULL)
    {
      /* Fill the data */
      memscpy ((void *) &(q_item->req), pkt_len, (void *) rsp, pkt_len);
      q_item->status = DIAG_TASK_INITIALIZED;
      q_item->seq_num = seq_num;
      seq_num++;

      /* Place q_item on the diag_cmd_req_q */
      q_put (&diag_cmd_req_q, q_link (q_item, &(q_item->link)));

      /* We no longer own the memory at q_item. */
      q_item = NULL;
    }

    /* Commit the response before spawning the test task. */
    diagpkt_commit (rsp);

    rsp = NULL;
  }

  /* Get the head of the Q */
  q_item = (diag_cmd_req_q_item_type *) q_check (&diag_cmd_req_q);

  /* Loop until the end is reached */
  while (q_item != NULL)
  {
    if (q_item->status == DIAG_TASK_INITIALIZED)
    {
      int diag_cmd_req_pri_array[32] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
      if (q_item->req.priority.pri_type == RELATIVE_PRI)
      {
        if (DIAG_PRI + q_item->req.priority.pri > SLEEP_PRI)
        {
          q_item->req.priority.pri += DIAG_PRI;
        }
        else
        {
          q_item->req.priority.pri = SLEEP_PRI + 1;
        }
      }


      /* Start the task */
      diag_cmd_req_attr.stack_size = STRESS_TEST_STACK_SIZE;
      diag_cmd_req_attr.start_func = diag_cmd_req_task_startup;
      diag_cmd_req_attr.arg =(void *) (q_item);
      diag_cmd_req_attr.name = "diag_cmd_req_task_startup";
      diag_cmd_req_attr.priority = q_item->req.priority.pri;
      diag_cmd_req_attr.stack_address = q_item->stack;
      diag_cmd_req_attr.p_tskname = "DIAG_CMD";
      diag_cmd_req_attr.suspended = FALSE;
      diag_cmd_req_attr.dog_report_val = 0;

       if ((osal_create_thread(&q_item->tcb, &diag_cmd_req_attr, diag_cmd_req_pri_array)) != 0)
       {
          return NULL;
       }

      /* Mark the task is running */
        /* Mark the task is running */
      if (q_item->status == DIAG_TASK_INITIALIZED)
      {
        q_item->status = DIAG_TASK_RUNNING;
      }

      /* go to the next Q item */
      q_item = q_next (&diag_cmd_req_q, &q_item->link);
    }
  }

  /* Register that the Q has items, and needs to be cleaned */
  (void) diag_idle_processing_register (diag_cmd_req_cleanup_cb, &diag_cmd_req_q);

  return NULL;
} /* diagdiag_cmd_request_handler */
/*===========================================================================

FUNCTION DIAG_STRESSTEST_CLEANUP_CB

DESCRIPTION
  This procedure is a cleanup function. It goes through the
  diag_stress_test_task_q, checks if any data block is ready to cleaned.
  If yes it removes the entry from the Q and frees the memory.

============================================================================*/
void  diag_stresstest_cleanup_cb (void *param)
{
  diag_stress_test_task_q_item_type *q_cur_ptr = NULL;
  diag_stress_test_task_q_item_type *q_next_ptr = NULL;
  int status;

  q_cur_ptr = (diag_stress_test_task_q_item_type *)q_check (&diag_stress_test_task_q);
  while( q_cur_ptr  != NULL )
  {
    /* If the element in the queue is ready to be deleted */
    if( q_cur_ptr->data.status == DIAG_TASK_DONE )
    {
      /* mapped to no-ops for all OSes except QURT */
      osal_thread_join(&(q_cur_ptr->data.tcb), &status);

      /* cleanup OSAL related memory, queues, channel etc */
      osal_delete_thread(&(q_cur_ptr->data.tcb));

      /* store the next link */
      q_next_ptr =(diag_stress_test_task_q_item_type *)q_next
        ( &diag_stress_test_task_q, &q_cur_ptr->link );

      /* Remove the data block from the Q */
      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete( &diag_stress_test_task_q, &q_cur_ptr->link );
      #else
      q_delete  (&q_cur_ptr->link);
      #endif


      /* Free the memory */
      free( q_cur_ptr );

      if( q_cur_ptr != NULL) {
        q_cur_ptr = NULL;
      }
      q_cur_ptr = q_next_ptr;
    }
    else
    {
      /* store the next link */
      q_cur_ptr =(diag_stress_test_task_q_item_type *)q_next
        ( &diag_stress_test_task_q, &q_cur_ptr->link );
    }
  }

  /* Deregister's when the count is 0(Q is empty) */
  if( diag_stress_test_task_q.cnt == 0)
  {
    (void) diag_idle_processing_unregister (&diag_stresstest_cleanup_cb,
      &diag_stress_test_task_q);
  }

  return;
}

/*===========================================================================
FUNCTION DIAGDIAG_IS_VALID_REG_TBL_CMD

DESCRIPTION
  This procedure is used to validate the incoming command (and parameters)
  for retreiving the registration tables.

  Also assigns values to is_mater ( denotes whether it is a master proc )
  and the port_id (for querying slave table)

============================================================================*/
boolean
diagdiag_is_valid_reg_tbl_cmd()
{
  boolean is_valid = FALSE;

  reg_tbl_is_master = TRUE;
  reg_tbl_port_id = DIAGDIAG_NO_PORT;

#if defined (DIAG_IMAGE_APPS_PROC)
  /* Checking if the opcodes and the ports are valid */
  if(reg_tbl_cmd_code == DIAGDIAG_GET_CMD_REG_TBL_APPS)
  {
    switch(reg_tbl_proc_id)
    {
       case DIAG_APP_PROC:
         reg_tbl_is_master = TRUE;
         is_valid = TRUE;
         break;

       case DIAG_MODEM_PROC:
         if(NUM_PORT_TYPES > DIAGDIAG_MODEM_PORT)
         {
           reg_tbl_is_master = FALSE;
           is_valid = TRUE;
           reg_tbl_port_id = DIAGDIAG_MODEM_PORT;
         }
         break;

       case DIAG_QDSP6_PROC:
         if(NUM_PORT_TYPES > DIAGDIAG_QDSP6_PORT)
         {
           reg_tbl_is_master = FALSE;
           is_valid = TRUE;
           reg_tbl_port_id = DIAGDIAG_QDSP6_PORT;
         }
         break;

       case DIAG_RIVA_PROC:
         if(NUM_PORT_TYPES > DIAGDIAG_RIVA_PORT)
         {
           reg_tbl_is_master = FALSE;
           is_valid = TRUE;
           reg_tbl_port_id = DIAGDIAG_RIVA_PORT;
         }
         break;
    }
  }
#endif

  if(reg_tbl_cmd_code == DIAGDIAG_GET_CMD_REG_TBL)
  {
    is_valid = TRUE;
  }
  return is_valid;
}

/*===========================================================================
FUNCTION DIAGDIAG_GET_REG_TABLE

DESCRIPTION
  This procedure returns the commands that are registered with a given
  processor based on the request. It calls diagpkt_get_registration_table()
  to get the corresponding command registration table. It sends more than
  one response - An immediate response that contains the number of commands
  that are registered and one or more delayed response(s) containg the
  command registration information. This is because the response packet
  may exceed the total permissible length for a packet

============================================================================*/
void * diagdiag_get_reg_table (
  void * req_pkt,
  uint16 pkt_len
)
{
  diagdiag_get_reg_tbl_req_type *req = NULL;  /* The request packet */
  diagdiag_get_reg_tbl_imm_rsp_type *imm_rsp = NULL; /* The immediate reponse packet */

  req = (diagdiag_get_reg_tbl_req_type *)req_pkt;

  /* Basic Sanity Check */
  if(req == NULL)
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  /* If the packet length doesn't match */
  if(pkt_len != sizeof(diagdiag_get_reg_tbl_req_type))
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  reg_tbl_cmd_code = diagpkt_subsys_get_cmd_code(req);
  reg_tbl_proc_id = req->proc_id;

  /* Check if the command code is valid or not */
  if( !diagdiag_is_valid_reg_tbl_cmd() )
  {
    return ((void *)diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  /* Create an immeditate reponse */
  imm_rsp = (diagdiag_get_reg_tbl_imm_rsp_type *)diagpkt_subsys_alloc_v2(DIAG_SUBSYS_DIAG_SERV, reg_tbl_cmd_code, sizeof(diagdiag_get_reg_tbl_imm_rsp_type));

  if(imm_rsp != NULL)
  {
    imm_rsp->cmd_count = diagpkt_get_reg_cmd_count();  /* The number of commands registered */
    imm_rsp->proc_id = reg_tbl_proc_id; /* Copying the processor id to the immediate response*/
    reg_tbl_delay_rsp_id = diagpkt_subsys_get_delayed_rsp_id(imm_rsp); /* Get the delayed response id */
    diagpkt_commit((void *)imm_rsp);  /* Commit the immediate reponse */

     /* Setting the internal signal to start sending Delayed Response(s) containing the registration table */
    (void)diag_set_internal_sigs( DIAG_INT_REG_TBL_RSP_SIG );
  }

  return NULL;
}

#ifdef FEATURE_DIAG_STRESS_PROFILE 
#include "npa.h" 
static npa_query_handle timer_test_CpuQueryHandle = NULL;
static npa_query_type qres;
/*this function will return you the CPU freq in KHz*/
uint32 qurt_timer_test_get_cpu_freq(void)
{
  uint32 cpu_freq = -1;
  if( NULL == timer_test_CpuQueryHandle )
  {
    timer_test_CpuQueryHandle = npa_create_query_handle("/clk/cpu");
  }
  
  /* Get the current CPU frequency. */
  if( NPA_QUERY_SUCCESS == npa_query( timer_test_CpuQueryHandle,
                                      NPA_QUERY_CURRENT_STATE,
                                      &qres ) );
  cpu_freq = qres.data.value;

  return cpu_freq;
}
#endif /* FEATURE_DIAG_STRESS_PROFILE */

/*===========================================================================

FUNCTION DIAGDIAG_TEST_TASK

DESCRIPTION
  This procedure is a startup function. Floods the link with the requested
  msg type packets.

============================================================================*/
void diagdiag_test_task (
  uint32 params
)
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  osal_timer_t sleep_timer;

  diag_stress_test_tcb_stack_type *local_data = (diag_stress_test_tcb_stack_type *)params;

  /* Pointers to stress test logs */
  LOG_STRESS_TEST_C_type * log_ptr;
  LOG_STRESS_TEST_64_type * log_ptr_64;
  LOG_STRESS_TEST_128_type * log_ptr_128;
  LOG_STRESS_TEST_256_type * log_ptr_256;
  LOG_STRESS_TEST_1K_type * log_ptr_1K;
  LOG_STRESS_TEST_2K_type * log_ptr_2K;
  LOG_STRESS_TEST_4K_type * log_ptr_4K;
  LOG_STRESS_TEST_6K_type * log_ptr_6K;
  LOG_STRESS_TEST_8K5_type * log_ptr_8K;

  uint32 version_id = 10;           /*Used to generate extended packets*/
  uint16 log_index = 0;             /* Index used to fill char array */
  EVENT_PAYLOAD_INFO_type payload;  /* Store the payload info */
  uint32 payload_stress_test_complete = 0; /* payload for stress test complete event */
  boolean timer_init = FALSE;       /* timer initialized or not */
  int test_type = 0;   /* Stores the test type info requested */
  int num_iter_bef_sleep = 0; /* Keeps track of number of iterations before sleep */
  int i = 1;
  int N = local_data->task_info.num_iterations; /* Total number of iterations */
  int unique_id = local_data->unique_id; /* Get the task ID */
  boolean ext_flag = FALSE; /* Flag for extended API usage */

  
#ifdef FEATURE_DIAG_STRESS_PROFILE
  uint64 pcycle_total=0, pcycle_start, pcycle_end;
  uint32 ms_time=0;
  uint32 cpu_freq=0;
#endif /* FEATURE_DIAG_STRESS_PROFILE */
  
/* Vote against sleep for stress test */
#if defined( DIAG_DYNAMIC_SLEEP )
  if( diagtest_npa_handle )
  {
    npa_issue_required_request( diagtest_npa_handle, 1);
  }

#elif defined (DIAG_MMPM_SUPPORT)
  int client_id = 0;
  client_id = diag_mmpm_register();
  /* Vote against power collapse on LPass when the test starts */
  diag_mmpm_config(client_id);
#endif

  /*Start the handshake so that the task is ready to start */
  return_val=osal_handshake_start(&(local_data->tcb));
  ASSERT(OSAL_SUCCESS == return_val);

  for(i=1; i <= N;i++)
  {

    /* Special Case, message type is generated using random */
    if(local_data->task_info.priority.test_type == DIAGDIAG_STRESS_TEST_MSG_PSEUDO_RANDOM)
    {
      /* Pick the random msg type from the array */
      test_type = pseudo_random(i);
    }
    else {
      test_type = local_data->task_info.priority.test_type;
    }

#ifdef FEATURE_DIAG_STRESS_PROFILE
    cpu_freq = qurt_timer_test_get_cpu_freq();
    pcycle_start = qurt_get_core_pcycles();
#endif /* FEATURE_DIAG_STRESS_PROFILE */

    /**********************************************************************************
     *
     * In all the following cases, the priority field is replaced by unique id field
     * to help the automation scripts differentiate between test tasks while parsing
     * the test logs.
     *
     **********************************************************************************/   
    /* Reset ext_flag each time */
    ext_flag = FALSE;
    switch (test_type) {
      case DIAGDIAG_STRESS_TEST_MSG:
        MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Test MSG with no arg ");
        break;
      case DIAGDIAG_STRESS_TEST_MSG_1:
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_1 Iter %d\n",i);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_2:
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_2 Iter %d uid %d \n",i,unique_id);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_3:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_3 Iter %d uid %d num_iter %d\n",i,
          unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_4:
        MSG_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_4 Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_5:
        MSG_5(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_5 Iter %d uid %d num_iter %d procid %d %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID,5);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_6:
        MSG_6(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MSG_6 Iter %d uid %d num_iter %d procid %d %d %d\n",
          i,unique_id,N,DIAG_MY_PROC_ID,5,6);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_STR:
        //TODO
        break;
      case DIAGDIAG_STRESS_TEST_MSG_LOW:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_LOW, "MSG_LOW Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_MED:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_MED, "MSG_MED Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_HIGH:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Client MSG_HIGH Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_ERROR:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"MSG_ERROR Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_MSG_FATAL:
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_FATAL, "MSG_FATAL Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_ERR:
        ERR( "ERR Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_ERR_FATAL:
        {
          int j;

          /* The ERR_FATAL test needs to make sure panic mode works.
             Generate a few messages and see if they make it out. */
          for (j = 0; j < 10; j++)
          {
            MSG_2 (MSG_SSID_DIAG, MSG_LEGACY_FATAL,
                    "Panic mode test %d uid %d", j, unique_id);
          }
        }
        ERR_FATAL("ERR_FATAL Iter %d uid %d num_iter %d \n",i,unique_id,N);
        break;
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
        if (log_ptr_128 != NULL) {
          log_ptr_128->iteration = i;
          log_ptr_128->task_priority =unique_id;
          log_ptr_128->req_iterations = N;
          for ( log_index = 0; log_index < 29; log_index++ ) {
              log_ptr_128->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_128);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_256:
        log_ptr_256 = (LOG_STRESS_TEST_256_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_256_type));
        if (log_ptr_256 != NULL) {
          log_ptr_256->iteration = i;
          log_ptr_256->task_priority =unique_id;
          log_ptr_256->req_iterations = N;
          for ( log_index = 0; log_index < 61; log_index++ ) {
              log_ptr_256->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_256);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_1K:
        log_ptr_1K = (LOG_STRESS_TEST_1K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_1K_type));
        if (log_ptr_1K != NULL) {
          log_ptr_1K->iteration = i;
          log_ptr_1K->task_priority =unique_id;
          log_ptr_1K->req_iterations = N;
          for ( log_index = 0; log_index < 253; log_index++ ) {
              log_ptr_1K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_1K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_2K:
        log_ptr_2K = (LOG_STRESS_TEST_2K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
            sizeof(LOG_STRESS_TEST_2K_type));
        if (log_ptr_2K != NULL) {
          log_ptr_2K->iteration = i;
          log_ptr_2K->task_priority =unique_id;
          log_ptr_2K->req_iterations = N;
          for ( log_index = 0; log_index < 509; log_index++ ) {
              log_ptr_2K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_2K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_4K:
        log_ptr_4K = (LOG_STRESS_TEST_4K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_4K_type));
        if (log_ptr_4K != NULL) {
          log_ptr_4K->iteration = i;
          log_ptr_4K->task_priority =unique_id;
          log_ptr_4K->req_iterations = N;
          for ( log_index = 0; log_index < 1021; log_index++ ) {
              log_ptr_4K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_4K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_6K:
        log_ptr_6K = (LOG_STRESS_TEST_6K_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_6K_type));
        if (log_ptr_6K != NULL) {
          log_ptr_6K->iteration = i;
          log_ptr_6K->task_priority =unique_id;
          log_ptr_6K->req_iterations = N;
          for ( log_index = 0; log_index < 1533; log_index++ ) {
              log_ptr_6K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_6K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_8K5:
        log_ptr_8K = (LOG_STRESS_TEST_8K5_type *) log_alloc (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_8K5_type));
        if (log_ptr_8K != NULL) {
          log_ptr_8K->iteration = i;
          log_ptr_8K->task_priority =unique_id;
          log_ptr_8K->req_iterations = N;
          for ( log_index = 0; log_index < (sizeof(log_ptr_8K->payload)/sizeof(log_ptr_8K->payload[0])); log_index++ ) {
              log_ptr_8K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_8K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_QSR_MSG:
        QSR_MSG(300000001, MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Test QSR_MSG with no arg ");
        break;
      case DIAGDIAG_STRESS_TEST_QSR_MSG_1:
        QSR_MSG_1(300000002, MSG_SSID_DIAG, MSG_LEGACY_HIGH, "QSR_MSG_1 Iter %d\n",i);
        break;
      case DIAGDIAG_STRESS_TEST_QSR_MSG_2:
        QSR_MSG_2(300000003, MSG_SSID_DIAG, MSG_LEGACY_HIGH, "QSR_MSG_2 Iter %d uid %d \n",i,unique_id);
        break;
      case DIAGDIAG_STRESS_TEST_QSR_MSG_3:
        QSR_MSG_3(300000004, MSG_SSID_DIAG, MSG_LEGACY_HIGH, "QSR_MSG_3 Iter %d uid %d num_iter %d\n",i,
          unique_id,N);
        break;
      case DIAGDIAG_STRESS_TEST_QSR_MSG_4:
        QSR_MSG_4(300000005, MSG_SSID_DIAG, MSG_LEGACY_HIGH, "QSR_MSG_4 Iter %d uid %d num_iter %d procid %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID);
        break;
      case DIAGDIAG_STRESS_TEST_QSR_MSG_5:
        QSR_MSG_5(300000006, MSG_SSID_DIAG, MSG_LEGACY_HIGH, "QSR_MSG_5 Iter %d uid %d num_iter %d procid %d %d\n",i,
          unique_id,N,DIAG_MY_PROC_ID,5);
        break;
      case DIAGDIAG_STRESS_TEST_LOG_EXT:
        log_ptr = (LOG_STRESS_TEST_C_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
            sizeof(LOG_STRESS_TEST_C_type), 1, &version_id);
        if (log_ptr != NULL) 
        {
          log_ptr->iteration = i;
          log_ptr->task_priority = unique_id;
          log_ptr->req_iterations = N;
          log_commit(log_ptr);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_64_EXT:
        log_ptr_64 = (LOG_STRESS_TEST_64_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_64_type), 1, &version_id);
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
        log_ptr_128 = (LOG_STRESS_TEST_128_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_128_type), 1, &version_id);
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
        log_ptr_256 = (LOG_STRESS_TEST_256_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                             sizeof(LOG_STRESS_TEST_256_type), 1, &version_id);
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
        log_ptr_1K = (LOG_STRESS_TEST_1K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_1K_type), 1, &version_id);
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
        log_ptr_2K = (LOG_STRESS_TEST_2K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
            sizeof(LOG_STRESS_TEST_2K_type), 1, &version_id);
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
      case DIAGDIAG_STRESS_TEST_LOG_4K_EXT:        
        log_ptr_4K = (LOG_STRESS_TEST_4K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_4K_type), 1, &version_id);
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
        log_ptr_6K = (LOG_STRESS_TEST_6K_type *) log_alloc_ext (LOG_DIAG_STRESS_TEST_C,
                                                            sizeof(LOG_STRESS_TEST_6K_type), 1, &version_id);
        if (log_ptr_6K != NULL) 
        {
          log_ptr_6K->iteration = i;
          log_ptr_6K->task_priority =unique_id;
          log_ptr_6K->req_iterations = N;
          for ( log_index = 0; log_index < 1533; log_index++ ) 
          {
              log_ptr_6K->payload[log_index] = 0x7EABBA7E;
          }
          log_commit(log_ptr_6K);
        }
        break;
      case DIAGDIAG_STRESS_TEST_LOG_SUBMIT_EXT_64:
         ext_flag = TRUE;
         /* Continue into next case statement with ext_flag set */
      case DIAGDIAG_STRESS_TEST_LOG_SUBMIT_64:
         {
           log_ptr_64 = (LOG_STRESS_TEST_64_type *)malloc(sizeof(LOG_STRESS_TEST_64_type));
           if (log_ptr_64)
           {
             log_set_length(log_ptr_64,sizeof(LOG_STRESS_TEST_64_type));
             log_set_code(log_ptr_64,LOG_DIAG_STRESS_TEST_C);
             log_set_timestamp(log_ptr_64);
           
             log_ptr_64->iteration = i;
             log_ptr_64->task_priority = unique_id;
             log_ptr_64->req_iterations = N;
             for ( log_index = 0; log_index < 13; log_index++ ) {
               log_ptr_64->payload[log_index] = 0x7EABBA7E;
             }

             if(!ext_flag)
             {
               log_submit(log_ptr_64);
             }
             else
             {
               log_submit_ext(log_ptr_64,1,&version_id);
             }

             free(log_ptr_64);
           }
           else
           {
             MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Malloc failed in log_submit() test case.Iter %d\n",i);
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
        MICRO_MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MICRO_MSG_2_EXT Iter %d uid %d \n",i,unique_id);
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
#ifdef FEATURE_DIAG_STRESS_PROFILE
    /* To-do: Account for drops and remove those totals from time */
    pcycle_end = qurt_get_core_pcycles();
    pcycle_total += pcycle_end - pcycle_start;
    ms_time += ((pcycle_end-pcycle_start)/cpu_freq);
#endif /* FEATURE_DIAG_STRESS_PROFILE */

   num_iter_bef_sleep++;

   if( (local_data->task_info.num_iterations_before_sleep > 0)
     && (num_iter_bef_sleep == local_data->task_info.num_iterations_before_sleep) ) {

      if(timer_init == FALSE) {
        /* Defines a timer */
        return_val = osal_create_diag_timer (&sleep_timer, osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG);
        ASSERT(OSAL_SUCCESS == return_val);
        timer_init = TRUE;
      }

      /* Wait for sleep_duration. This blocks the current task, and
         different task with which is ready to run starts executing */
      (void) osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer,
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
  if(timer_init == FALSE)
  {
     /* Defines a timer */
    return_val = osal_create_diag_timer (&sleep_timer, osal_thread_self(),DIAG_STRESS_TEST_SLEEP_SIG);
    ASSERT(OSAL_SUCCESS == return_val);     
     timer_init = TRUE;
  }
   (void) osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer, STRESS_TASK_SLEEP_TO_COMPLETE);

    /* Clear the signals. The block on the current task is cleared */
  return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

  if(osal_thread_get_pri() <= DIAG_PRI)
  {
    /* This is done to make sure that the task gets killed before the
     cleanup function, running in diag context, removes the data from the Q */
    (void) osal_thread_set_pri(DIAG_PRI+1);
  }

  /* Notify that a stress test task has completed for automation */
  event_report_payload( EVENT_DIAG_STRESS_TEST_COMPLETED , sizeof(int32), &payload_stress_test_complete );

#ifdef FEATURE_DIAG_STRESS_PROFILE
  {
    /* To-do: Report drop counts directly based on type of test */
    uint64 diag_sigs_pcycle_count = diag_sigs_pcycle_total;
    uint64 diag_hdlc_pcycles_count = diag_hdlc_pcycles_total;
    uint64 diag_tracer_pcycles_count = diag_tracer_pcycles_total;
        
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Stress test complete. %d iterations\n",N);
    {
      uint64 pcycle_avg = pcycle_total/N;
      uint32 ms_avg = ms_time/N;
      
      if(pcycle_avg > (uint64)0x00000000FFFFFFFF)
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Pcycle average overflow\n",unique_id);
      }
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Pcycle average per iteration is: %d\n",unique_id,(uint32)pcycle_avg);
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d MS average per iteration is: %d\n",unique_id,ms_avg);
      
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag Task Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_sigs_pcycle_count/N));

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag HDLC Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_hdlc_pcycles_count/N));

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag Tracer Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_tracer_pcycles_count/N));

    }
    
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d CPU frequency is: %d\n",unique_id,cpu_freq);
    
    
#ifdef FEATURE_DIAG_STM
    /* Temp assist for STM testing; print to USB for quick results */
    if(diag_stm_enabled)
    {
      diag_stm_enabled=0;
      
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Stress test complete. %d iterations\n",N);
      {
        uint64 pcycle_avg = pcycle_total/N;
        uint32 ms_avg = ms_time/N;
        
        if(pcycle_avg > (uint64)0x00000000FFFFFFFF)
        {
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Pcycle average overflow\n",unique_id);
        }
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Pcycle average per iteration is: %d\n",unique_id,(uint32)pcycle_avg);
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d MS average per iteration is: %d\n",unique_id,ms_avg);

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag Task Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_sigs_pcycle_count/N));

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag HDLC Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_hdlc_pcycles_count/N));

      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d Diag Tracer Pcycle average per iteration is: %d\n",unique_id,(uint32)(diag_tracer_pcycles_count/N));
      }
      
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "UID %d CPU frequency is: %d\n",unique_id,cpu_freq);
      
      diag_stm_enabled=1;
    }
#endif /* FEATURE_DIAG_STM */
  }

#endif /* FEATURE_DIAG_STRESS_PROFILE */  
/* Vote for sleep for diag stress test */
#if defined( DIAG_DYNAMIC_SLEEP )
  if( diagtest_npa_handle )
  {
    npa_cancel_request( diagtest_npa_handle );
  }

#elif defined(DIAG_MMPM_SUPPORT )
  /*Sleep for few seconds before voting for power collpase so that diag can drain the
  event stress test completed on lpass*/
  (void) osal_timed_wait(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &sleep_timer, 10000);

    /* Clear the signals. The block on the current task is cleared */
  return_val = osal_reset_sigs(osal_thread_self(), DIAG_STRESS_TEST_SLEEP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Vote for power collapse on LPass when the test completes */
  diag_mmpm_release(client_id);
  diag_mmpm_deregister(client_id);
#endif


  /* mark the task to be cleaned */
  local_data->status = DIAG_TASK_DONE;

     /* Delete the sleep timer */
   if( timer_init == TRUE)
   {
      return_val = osal_delete_timer(&sleep_timer); 
      ASSERT(OSAL_SUCCESS == return_val);
   }

  /* mapped to no-ops for all OSes except QURT */
  osal_thread_exit(thread_exit_status);

} /* stress_test_start_up */


/*===========================================================================

FUNCTION DIAGDIAG_STRESS_TEST

DESCRIPTION
  This procedure sends a response to acknowledge that the test has been started.
  It then defines "num_tasks" tasks, with a requested priorities.
  The task startup functions tests flooding the link using the requested
  parameters.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
void *
diagdiag_stress_test (void * req_pkt, uint16 pkt_len)
{

  DIAGDIAG_STRESS_TEST_rsp_type *rsp;
  DIAGDIAG_STRESS_TEST_req_type *req = (DIAGDIAG_STRESS_TEST_req_type *) req_pkt;

  if((req->num_tasks) > MAX_32BIT_VALUE/sizeof(diag_per_task_test_info))
  {
      return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  uint32 size_of_num_tasks = sizeof(diag_per_task_test_info) * (req->num_tasks);

  if(FPOS(DIAGDIAG_STRESS_TEST_rsp_type,test) > MAX_32BIT_VALUE - size_of_num_tasks)
  {
      return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }
  
  const uint32 rsp_len = FPOS(DIAGDIAG_STRESS_TEST_rsp_type,test) + size_of_num_tasks;

  /*  used to store and retrive data block from the Q */
  diag_stress_test_task_q_item_type *q_item = NULL;
  int i=0;

  if (pkt_len != rsp_len)
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp = (DIAGDIAG_STRESS_TEST_rsp_type *) diagpkt_subsys_alloc (
  DIAG_SUBSYS_DIAG_SERV,
  DIAGDIAG_START_STRESS_TEST_F,
  rsp_len
  );

  /* Send the response acknowledging that the packet has been started */
  if (rsp != NULL)
  {
    memscpy ((void *) rsp, rsp_len, (void *) req, rsp_len);

    /* Initialize the queue */
    if(diag_stress_test_task_q_init == FALSE)
    {
      (void) q_init(&diag_stress_test_task_q);
      diag_stress_test_task_q_init = TRUE;
    }
#ifdef FEATURE_DIAG_STRESS_PROFILE
    // Reset diag pcycles counters
    diag_sigs_pcycle_total = 0;
    diag_hdlc_pcycles_total = 0; 
    diag_tracer_pcycles_total = 0;
#endif
    for(i=0; i<req->num_tasks; i++)
    {
      /* Get memory from the system heap */
      q_item =(diag_stress_test_task_q_item_type *) malloc(
        sizeof(diag_stress_test_task_q_item_type));

      if(q_item != NULL)
      {
         memset(q_item,0x0,sizeof(diag_stress_test_task_q_item_type));

        /* Initialize the link field */
        (void) q_link(q_item, &(q_item->link) );

        /* Fill the data */
        q_item->data.task_info = req->test[i];
        q_item->data.status = DIAG_TASK_INITIALIZED;
    q_item->data.unique_id = i;   /* Task IDs range from 0 to num_tasks-1 */

        /* Place q_item on the diag_stress_test_task_q */
        q_put (&diag_stress_test_task_q, &(q_item->link));
      } else { /* Failed  system malloc, handle this error */
          diagpkt_commit(rsp);
          rsp = NULL;
          MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Failed to allocate q_item for stress test.\n");
          return NULL;
      }
    }

    /* Commit the response before spawning the test task. */
    diagpkt_commit(rsp);

    rsp = NULL;
  }

  /* Get the head of the Q */
  q_item = (diag_stress_test_task_q_item_type *)q_check (&diag_stress_test_task_q);

  /* Loop until the end is reached */
  while( q_item != NULL )
  {
    /* if the task is not yet started */
    if( q_item->data.status == DIAG_TASK_INITIALIZED )
    {
      int diagdiag_stress_pri_array[32] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

      if(q_item->data.task_info.priority.pri_type == RELATIVE_PRI)
      {
#if  defined(DIAG_QDSP6_APPS_PROC)
         if (DIAG_PRI + q_item->data.task_info.priority.pri < SLEEP_PRI)
        {
          q_item->data.task_info.priority.pri += DIAG_PRI;
        }
        else
        {
          q_item->data.task_info.priority.pri = SLEEP_PRI;
        }

#else

        if( DIAG_PRI + q_item->data.task_info.priority.pri > SLEEP_PRI)
        {
          q_item->data.task_info.priority.pri += DIAG_PRI;
        } else
        {
          q_item->data.task_info.priority.pri = SLEEP_PRI + 1;
        }
#endif

      }

      /* Start the task */
      /* stack is of type unsigned long long' */
      diagdiag_stress_attr.stack_size = (STRESS_TEST_STACK_SIZE * sizeof(unsigned long long) );
      diagdiag_stress_attr.start_func = diagdiag_test_task;
      diagdiag_stress_attr.arg = (void *) (&(q_item->data));
      diagdiag_stress_attr.name = "diagdiag_test_task";
      diagdiag_stress_attr.priority = q_item->data.task_info.priority.pri;
      diagdiag_stress_attr.stack_address = q_item->data.stack;
      diagdiag_stress_attr.p_tskname = "DIAG_STRESS";
      diagdiag_stress_attr.suspended = FALSE;
      diagdiag_stress_attr.dog_report_val = 0;

      /* Mark the task is running BEFORE creating the thread for multi-threaded case */
      if( q_item->data.status == DIAG_TASK_INITIALIZED )
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
    q_item = q_next (&diag_stress_test_task_q, &q_item->link);
  }

  /* Register that the Q has items,  and needs to be cleaned */
  (void) diag_idle_processing_register (diag_stresstest_cleanup_cb, &diag_stress_test_task_q);

  return NULL;

}  /* diagdiag_stress_test */




/*===========================================================================

FUNCTION diag_dcm_cmd_reg_table

DESCRIPTION
  This procedure tests Registers and Deregisters given registration tables.

RETURN VALUE
  None.

============================================================================*/

void diag_dcm_cmd_reg_test_table(diagpkt_subsys_cmd_code_type cmd_code, uint8 idx)
{
  if(cmd_code == DIAG_DCM_SUBSYS_CMD_REG)
  	{
  	  switch (idx)
  	  	{
	  	  case DIAG_CMD_REG_TEST_TABLE1:
		    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_dcm_test_reg_tbl_1);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE2:
		    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_dcm_test_reg_tbl_2);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE3:
		    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, diag_dcm_test_reg_tbl_3);
		    break;		  
  	  	}
  	}
  
  if(cmd_code == DIAG_DCM_SUBSYS_CMD_DEREG)
  	{
  	  switch (idx)
  	  	{
	  	  case DIAG_CMD_REG_TEST_TABLE1:
		    DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_dcm_test_reg_tbl_1);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE2:
		    DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_dcm_test_reg_tbl_2);
		    break;
	  	  case DIAG_CMD_REG_TEST_TABLE3:
		    DIAGPKT_DISPATCH_TABLE_DEREGISTER (DIAG_SUBSYS_DIAG_SERV, diag_dcm_test_reg_tbl_3);
		    break;		  
  	  	}
  	}

}

/*===========================================================================

FUNCTION diag_dcm_subsys_reg_dereg_test

DESCRIPTION
    This procedure tests Registration and Deregistration of Core PD registration tables.

RETURN VALUE
  Pointer to response packet

============================================================================*/

void * diag_dcm_subsys_reg_dereg_test (void * req_pkt, uint16 pkt_len)
{	
	diag_dcm_cmd_reg_test_rsp_type *rsp = NULL;	
	diag_dcm_cmd_reg_test_rsp_type *req = (diag_dcm_cmd_reg_test_rsp_type *) req_pkt;
	diagpkt_subsys_cmd_code_type cmd_code = 0;

	if((pkt_len != sizeof(diag_dcm_cmd_reg_test_rsp_type)) || (req_pkt == NULL))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

	cmd_code = diagpkt_subsys_get_cmd_code (req);

	if(req->table1 <= DIAG_CMD_REG_TEST_MAX_TABLE)
		diag_dcm_cmd_reg_test_table(cmd_code,req->table1);
	
    if(req->table2 <= DIAG_CMD_REG_TEST_MAX_TABLE)
		diag_dcm_cmd_reg_test_table(cmd_code,req->table2);
	
	if(req->table3 <= DIAG_CMD_REG_TEST_MAX_TABLE)
		diag_dcm_cmd_reg_test_table(cmd_code,req->table3);
	  
	rsp = (diag_dcm_cmd_reg_test_rsp_type *)diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, cmd_code, sizeof(diag_dcm_cmd_reg_test_rsp_type));

	if(rsp)
    {
      memscpy(rsp,pkt_len,req_pkt,pkt_len);
    }
	
	return (void *) rsp;
}


/*===========================================================================

FUNCTION diag_dcm_subsys_reg_test

DESCRIPTION
      This procedure echos the request in the response.

RETURN VALUE
  Pointer to response packet

============================================================================*/

void * diag_dcm_subsys_reg_test (void * req_pkt, uint16 pkt_len)
{
  void * rsp = NULL;
  uint8 req_len_min = 0;

  req_len_min = sizeof(diagpkt_subsys_header_type);
  
  if((req_pkt == NULL) || (pkt_len < req_len_min))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));

  /* Allocate the same length as the request. */
  rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_DCM_SUBSYS_CMD_REG_TEST, pkt_len);
	
  if (rsp != NULL) {
    memscpy ((void *) rsp, pkt_len, (void *) req_pkt, pkt_len);
  }
	
  return (rsp);
}

/*===========================================================================

FUNCTION diag_dcm_subsys_reg_test

DESCRIPTION
  This procedure is used to test registration and deregistration functionality 
  and simply echos the request in the response.

RETURN VALUE
  Pointer to response packet

============================================================================*/

void * diag_dcm_subsys_reg_test2 (void * req_pkt, uint16 pkt_len)
{
  void * rsp = NULL;
  uint8 req_len_min = 0;

  req_len_min = sizeof(diagpkt_subsys_header_type);
  
  if((req_pkt == NULL) || (pkt_len < req_len_min))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  
  /* Allocate the same length as the request. */
  rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_DCM_SUBSYS_CMD_REG_TEST2, pkt_len);
	
  if (rsp != NULL) {
	memscpy ((void *) rsp, pkt_len, (void *) req_pkt, pkt_len);
  }
	
  return (rsp);
}

/*===========================================================================

FUNCTION diag_dcm_subsys_reg_test

DESCRIPTION
  This procedure is used to test registration and deregistration functionality 
  and simply echos the request in the response.

RETURN VALUE
  Pointer to response packet

============================================================================*/

void * diag_dcm_subsys_reg_test3 (void * req_pkt, uint16 pkt_len)
{
  void * rsp = NULL;
  uint8 req_len_min = 0;

  req_len_min = sizeof(diagpkt_subsys_header_type);
  
  if((req_pkt == NULL) || (pkt_len < req_len_min))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  
  /* Allocate the same length as the request. */
  rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_DCM_SUBSYS_CMD_REG_TEST3, pkt_len);
	
  if (rsp != NULL) {
	memscpy ((void *) rsp, pkt_len, (void *) req_pkt, pkt_len);
  }
	
  return (rsp);
}

/*===========================================================================

FUNCTION diag_dcm_subsys_reg_test

DESCRIPTION
  This procedure is used to test registration and deregistration functionality 
  and simply echos the request in the response.

RETURN VALUE
  Pointer to response packet

============================================================================*/

void * diag_dcm_subsys_reg_test4 (void * req_pkt, uint16 pkt_len)
{
  void * rsp = NULL;
  uint8 req_len_min = 0;

  req_len_min = sizeof(diagpkt_subsys_header_type);
  
  if((req_pkt == NULL) || (pkt_len < req_len_min))
  	return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  
  /* Allocate the same length as the request. */
  rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_DCM_SUBSYS_CMD_REG_TEST4, pkt_len);
	
  if (rsp != NULL) {
	memscpy ((void *) rsp, pkt_len, (void *) req_pkt, pkt_len);
  }
	
  return (rsp);
}
/* Tell lint that we are OK with the fact that pkt_len and req are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint +esym(715,pkt_len,req) */

/* Define and register the dispatch table for common diagnostic packets */
#define DISPATCH_DECLARE(func) \
extern void * func(void * req_pkt, word len)

DISPATCH_DECLARE (event_inbound_pkt);

DISPATCH_DECLARE (log_process_legacy_logmask);
DISPATCH_DECLARE (log_process_config_pkt);
DISPATCH_DECLARE (log_on_demand_pkt);

DISPATCH_DECLARE (msg_pkt_process_config);
DISPATCH_DECLARE (msg_pkt_legacy_req);
DISPATCH_DECLARE (ext_msg_pkt_config_adv);
DISPATCH_DECLARE (diagdiag_ext_event_report_ctrl);

DISPATCH_DECLARE (event_mask_get_handler);
DISPATCH_DECLARE (event_mask_set_handler);


static const diagpkt_user_table_entry_type diagdiag_common_tbl[] =
{
  /* Place these at the top of the list for faster search. */
  {DIAG_MSG_F, DIAG_MSG_F, msg_pkt_legacy_req},

  {DIAG_EXT_MSG_CONFIG_F, DIAG_EXT_MSG_CONFIG_F, msg_pkt_process_config},
  {DIAG_LOG_CONFIG_F, DIAG_LOG_CONFIG_F, log_process_config_pkt},
  {DIAG_LOG_ON_DEMAND_F, DIAG_LOG_ON_DEMAND_F, log_on_demand_pkt},
  {DIAG_LOGMASK_F, DIAG_LOGMASK_F, log_process_legacy_logmask},
  {DIAG_EVENT_REPORT_F, DIAG_EVENT_REPORT_F, event_inbound_pkt},
  {DIAG_EVENT_MASK_GET_F, DIAG_EVENT_MASK_GET_F, event_mask_get_handler },
  {DIAG_EVENT_MASK_SET_F, DIAG_EVENT_MASK_SET_F, event_mask_set_handler },

};

static const diagpkt_user_table_entry_type diag_subsys_common_tbl[] =
{
   {DIAG_FLUSH_BUFFER, DIAG_FLUSH_BUFFER, diagdiag_flush_buffer_handler},
   {DIAG_CHANGE_ENC_PROTOCOL, DIAG_CHANGE_ENC_PROTOCOL, diagdiag_change_enc_protocol},
   {DIAG_EXT_EVENT_REPORT_CTRL, DIAG_EXT_EVENT_REPORT_CTRL, diagdiag_ext_event_report_ctrl},
   {DIAG_EXT_LOG_CONFIG, DIAG_EXT_LOG_CONFIG, log_process_config_pkt},
   {DIAG_EXT_MSG_CONFIG_ADV, DIAG_EXT_MSG_CONFIG_ADV, ext_msg_pkt_config_adv},
   {DIAG_DCM_SUBSYS_CMD_REG, DIAG_DCM_SUBSYS_CMD_DEREG, diag_dcm_subsys_reg_dereg_test}
};

static const diagpkt_user_table_entry_type diagdiag_subsys_tbl[] =
{
  {DIAGDIAG_START_STRESS_TEST_F, DIAGDIAG_START_STRESS_TEST_F, diagdiag_stress_test},
  {DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK, DIAGDIAG_STRESS_TEST_SUBSYS_LOOPBACK, diagdiag_subsys_loopback},
  {DIAGDIAG_RESET_DROP_COUNT_LOG, DIAGDIAG_GET_ALLOC_COUNT_F3, diagdiag_health_command},
  {DIAGDIAG_RESET_DROP_COUNT_DELAY, DIAGDIAG_GET_ALLOC_COUNT_DELAY, diagdiag_health_command},
  {DIAGDIAG_GET_FLOW_CTRL_COUNT, DIAGDIAG_RESET_FLOW_CTRL_COUNT, diagdiag_health_flow_ctrl},
  {DIAGDIAG_LOG_MASK_RETRIEVAL, DIAGDIAG_LOG_MASK_RETRIEVAL, log_pkt_get_local_masks},
  {DIAGDIAG_MSG_MASK_RETRIEVAL, DIAGDIAG_MSG_MASK_RETRIEVAL, msg_pkt_get_local_masks},
  {DIAGDIAG_EVENT_MASK_RETRIEVAL, DIAGDIAG_EVENT_MASK_RETRIEVAL,event_pkt_get_local_masks},
  {DIAGDIAG_RETRIEVE_SSID_RANGE, DIAGDIAG_RETRIEVE_SSID_RANGE, msg_pkt_get_local_ssid_range},
  {DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE, DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE,log_pkt_get_local_equipid_range},
  {DIAG_CHANGE_THRESHOLD, DIAG_CHANGE_THRESHOLD, diagdiag_change_threshold},
  {DIAGDIAG_GET_TIME_API, DIAGDIAG_GET_TIME_API, diag_get_time_api_request}  
};

#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) \
   && !defined (DIAG_IMAGE_QDSP6_PROC)
static const diagpkt_user_table_entry_type diagdiag_tbl[] =
{
   {DIAG_PROTOCOL_LOOPBACK_F, DIAG_PROTOCOL_LOOPBACK_F, diagdiag_loopback},
};
#endif
/* Delayed Response Table */
static const diagpkt_user_table_entry_type diagdiag_delayed_rsp_tbl[] = 
{
  {DIAGDIAG_STRESS_TEST_DELAYED_RSP, DIAGDIAG_STRESS_TEST_DELAYED_RSP, diagdiag_delayed_rsp_test}
};

/* Delayed Response Table for Registration Table Retrieval */
static const diagpkt_user_table_entry_type diagdiag_get_reg_tbl_delay[] =
{
  {DIAGDIAG_GET_CMD_REG_TBL, DIAGDIAG_GET_CMD_REG_TBL, diagdiag_get_reg_table}
};

void diagdiag_init (void)
  {

  int return_val =  0;

  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_COMMON_PROC, DIAGPKT_NO_SUBSYS_ID,
        diagdiag_common_tbl);

  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_COMMON_PROC, DIAG_SUBSYS_DIAG_SERV,
        diag_subsys_common_tbl);

#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) \
      && !defined (DIAG_IMAGE_QDSP6_PROC)

  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, diagdiag_tbl);
#endif

	DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_MY_PROC_ID, DIAG_SUBSYS_DIAG_SERV,
		diagdiag_subsys_tbl);

	DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(DIAG_SUBSYS_CMD_VER_2_F,
										   DIAG_SUBSYS_DIAG_SERV,
										   diagdiag_get_reg_tbl_delay);

	/*register table for delayed response*/
	DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY_PROC(DIAG_MY_PROC_ID, DIAG_SUBSYS_CMD_VER_2_F, DIAG_SUBSYS_DIAG_SERV,
		diagdiag_delayed_rsp_tbl);

	diagdiag_memop_tbl_mutex.name = "MUTEX_DIAGDIAG_MEMOP_CS";
	return_val = osal_init_mutex(&diagdiag_memop_tbl_mutex);

	ASSERT(OSAL_SUCCESS == return_val);

  }

#if defined __cplusplus
  }
#endif
