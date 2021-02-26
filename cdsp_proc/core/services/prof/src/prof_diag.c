/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   D I A G N O S T I C     T A S K
                O P E R A T I O N    P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains routines that process Rex-related packets.
  Each procedure handles a specific packet.

  Each procedure has the same job:

    1. Check packet for correctness.

    2. Perform processing required by the packet.

    3. Format a response packet.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2002-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

 $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/16   sm      Add in SLPI support for sdm845 compilation support. 
09/17/08   kcr     Branched from services/rexl4/rexdiag.c#7. This is 
                   the initial qdsp6 port.Removed rex task profiling support
09/08/05   sam     Ported rex task profiling to L4.
03/29/05    ck     Fixed an impossible if condition in the functin
                   rex_diag_taskprofile_handler.
08/11/04    is     Added REX profiling dump support (action=3).
11/20/02   jct     Integrated on target.  Added core dump test functionality
07/21/02    gr     Created.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "diagpkt.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "prof_diag.h"
#include "qurt.h"
#include "stringl.h"

extern void prof_enable_qurt_pmon (uint8 evt0,
                    uint8 evt1,
                    uint8 evt2,
                    uint8 evt3,
                    uint32 pmon_period);
extern void prof_disable_qurt_pmon(void);
extern void prof_enable_task_profile(uint32 task_period);
extern void prof_disable_task_profile(void);
/*===========================================================================

FUNCTION PROF_DIAG_TASKPROFILE_HANDLER

DESCRIPTION
  This function handles the request to start or stop program counter
  profiling.

  action = 1 -> enable task profiling
  action = 2 -> disable task profiling

============================================================================*/
/*lint -esym(765,rex_diag_taskprofile_handler) supress warning 765 */
/*lint -esym(715,pkt_len) pkt_len is not referenced */
PACKED void * prof_diag_taskprofile_handler(PACKED void *req_ptr,
                                           uint16       pkt_len)
{
  DIAG_SUBSYS_PROF_TASKPROFILE_F_req_type *taskprofile_req_ptr;
  DIAG_SUBSYS_PROF_TASKPROFILE_F_rsp_type *taskprofile_rsp_ptr;
  
  taskprofile_req_ptr = (DIAG_SUBSYS_PROF_TASKPROFILE_F_req_type *) req_ptr;
  
  if( (taskprofile_req_ptr->action > 1 ) ) {
    return diagpkt_err_rsp(DIAG_BAD_PARM_F,
                           taskprofile_req_ptr,
                           sizeof(DIAG_SUBSYS_PROF_TASKPROFILE_F_req_type));
  }
  
  taskprofile_rsp_ptr = (DIAG_SUBSYS_PROF_TASKPROFILE_F_rsp_type *)
    diagpkt_alloc(diagpkt_get_cmd_code( req_ptr ),
                  sizeof(DIAG_SUBSYS_PROF_TASKPROFILE_F_rsp_type));
  
   if ( taskprofile_rsp_ptr == NULL ) {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
     ** So just return NULL.
     */
     return NULL;
   }

   (void)memscpy((void*)taskprofile_rsp_ptr,
                sizeof(diagpkt_subsys_header_type),
                (void*)taskprofile_req_ptr,
                sizeof(diagpkt_subsys_header_type));

   switch( taskprofile_req_ptr->action ) {
   case 0:
     prof_disable_task_profile();
     taskprofile_rsp_ptr->status = 1;
     break;
     
   case 1:
     prof_enable_task_profile(taskprofile_req_ptr->task_period);     
     taskprofile_rsp_ptr->status = 1;
     break;    
     
   default:
     taskprofile_rsp_ptr->status = 0;
     break;
   }

   return (void *) taskprofile_rsp_ptr;
} /* rex_diag_taskprofile_handler */

PACKED void * prof_diag_qurt_perfmon_handler(PACKED void *req_ptr,
                                       uint16       pkt_len)
{
   DIAG_SUBSYS_PROF_PERFMON_F_req_type *perfmon_req_ptr;
   DIAG_SUBSYS_PROF_PERFMON_F_rsp_type *perfmon_rsp_ptr;
  
   perfmon_req_ptr = (DIAG_SUBSYS_PROF_PERFMON_F_req_type *) req_ptr;
  
   if( ( perfmon_req_ptr->action == 0 ) ||
       ( perfmon_req_ptr->action > 2 ) ) {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F,
                           perfmon_req_ptr,
                           sizeof(DIAG_SUBSYS_PROF_PERFMON_F_req_type));
   }
  
   perfmon_rsp_ptr = (DIAG_SUBSYS_PROF_PERFMON_F_rsp_type *)
   diagpkt_alloc(diagpkt_get_cmd_code( req_ptr ),
                  sizeof(DIAG_SUBSYS_PROF_PERFMON_F_rsp_type));
  
   if ( perfmon_rsp_ptr == NULL ) {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   (void)memscpy((void*)perfmon_rsp_ptr,
                sizeof(diagpkt_subsys_header_type),
                (void*)perfmon_req_ptr,
                sizeof(diagpkt_subsys_header_type));

   switch( perfmon_req_ptr->action ) {
      case 1:
         prof_enable_qurt_pmon (perfmon_req_ptr->evt0,
                             perfmon_req_ptr->evt1,
                             perfmon_req_ptr->evt2,
                             perfmon_req_ptr->evt3,
                             perfmon_req_ptr->pmon_period);
         perfmon_rsp_ptr->status = 1;
         break;
     
      case 2:
         prof_disable_qurt_pmon ();
         perfmon_rsp_ptr->status = 1;
         break;
     
      default:
         perfmon_rsp_ptr->status = 0;
         break;
   }

   return (void *) perfmon_rsp_ptr;

} 

static const diagpkt_user_table_entry_type prof_diag_tbl[] =
{
  {DIAG_QURT_PERFMON_F,  DIAG_QURT_PERFMON_F,  prof_diag_qurt_perfmon_handler},
  {DIAG_TASKPROFILE_F,   DIAG_TASKPROFILE_F,   prof_diag_taskprofile_handler }
};

//#if defined FEATURE_DIAG_MANUAL_DISPATCH_TABLE
/*lint -esym(714,profdiag_init) supress warning 714 */
/*lint -esym(765,profdiag_init) supress warning 765 */
void profdiag_init (void);

/*===========================================================================
FUNCTION PROFDIAG_INIT

DESCRIPTION
============================================================================*/
void profdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER ((int)DIAG_SUBSYS_OS, prof_diag_tbl);
}
//#else
//DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_Q6_CORE, prof_diag_tbl);
//#endif

