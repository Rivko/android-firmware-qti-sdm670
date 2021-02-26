#ifndef DIAGI_H
#define DIAGI_H

/*! 
 @ingroup diag_misc 
  @file diagi.h 
  @brief 
    Diagnostic services miscellaneous header File.
    Provides Diagnostic services miscellaneous APIs  
*/

/*
===========================================================================

    Copyright (c) 1990-2010,2016 by Qualcomm Technologies, Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

===========================================================================

                            Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/diagi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/16   nk      KW warnings fixes
05/21/10   SUG     Doxygenation Changes
09/30/09   JV      Removed FEATURE_DIAG_NV from this public header file. Removed
                   redundant prototype for diag_nv_read().
07/31/09   JV      Added FEATURE_DIAG_NV back (no NV functionality on the Q6)
07/31/09   JV      Merged Q6 diag code back to mainline
07/21/09   mad     Changed prototype of diag_wait to remove osal.h inclusion
07/15/09   mad     Removed un-necessary includes:diag.h,diagbuf.h,log.h
07/14/09   JV      Introduced osal.
07/10/09   as      Mainlined code under FEATURE_DIAG_NV
05/07/09   vk      removed dependencies from diagi_v.h 
12/12/08   vg      CMI creating diagi.h diagi_v.h
10/03/08   vg      Updated code to use PACK() vs. PACKED
11/21/06   as      Moved DIAG internal features from diagi.h to diag.h
08/28/06   as      Added win mobile featurization support.
03/28/06   pc      Changed DIAG_TX_APP_SIG from 0x00800000 to 0x00080000.
10/19/05   as      Modified diag_searchlist_type to make it 4-byte aligned.
07/05/05   sl      Added support for SPC_TIMEOUT to double the timeout value
                   on consecutive incorrect SPCs.
06/16/05   as      New signal for DM to communicate completion.
05/17/05   as      Added new signal for Application processor data.
10/24/01   jal     New signal for SIO to indicate the UART is flushed.
                   New signal for SIO to indicate port closure is complete.
08/20/01   jal     Supported more Diag packets.  Added NV cache typedef,
                   prototypes for diag_set_sp_state(), downloader status.
06/27/01   lad     Various non-functional changes to facilitate update of
                   log services.
04/06/01   lad     Added definitions of DIAG_TASK_xxx sigs to decouple from 
                   task.h.
                   ASYNC con<F2st definitions moved to diagcomm.h?
                   Externalized msg_nice[] and log_nice[] for streaming config 
                   support.
                   Moved prototype of diag_kick_watchdog() to diagtarget.h.
                   Added prototype of diagpkt_process_request() (moved from 
                   diagpkt.h).
                   Updated prototype for diag_do_escaping().
                   Removed prototype for diag_do_unescaping().
                   Removed references to diagpkt_refresh_cache().
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  The only coupling
                   that remains are common services and REX.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/


#include "comdef.h"
#include <string.h>


/*! 
   @name diag Signal Codes    
   @ingroup diag_misc   
*/ 

/*@{*/ /*Start signal codes for diag*/
#define DIAG_NV_CMD_SIG           0x00000004    /*!<
                                                   This signal is specified when
                                                   issuing commands to the
                                                   nv_cmd_q of the Non-volatile
                                                   Database Task.  When the
                                                   command is completed, the
                                                   Non-volatile Database Task
                                                   sets this signal to notify
                                                   the Diagnostic Task.        
                                                */

#define DIAG_HS_CMD_SIG           0x00000008   /*!< 
                                                   This signal is specified when
                                                   issuing commands to the
                                                   hs_cmd_q of the handset 
                                                   driver.  When the command
                                                   is completed, the hs driver
                                                   sets this signal to notify
                                                   the Diagnostic Task.        
                                               */

#define DIAG_MC_CMD_SIG           0x00000010    /*!<
                                                   This signal is specified when
                                                   issuing commands to the 
                                                   mc_cmd_q of the mc task.
                                                   When the command is
                                                   completed, the mc task sets
                                                   this signal to notify the
                                                   Diagnstic Task. 
                                                */

#define DIAG_SND_CMD_SIG          0x00000020    /*!<
                                                   This signal is specified when
                                                   issuing commands to the snd 
                                                   driver. When the command is
                                                   completed, the snd driver 
                                                   sets this signal to notify
                                                   the diag task
                                                */


#define DIAG_ASYNC_BLOCK_SIG      0x00400000    /*!<
                                                    Signal for adding asynchronous
                                                    wait capabilities to diag
                                                    packet handlers. 
                                                */
/*@}*/ /*end signal codes for diag*/

/*! 
  @ingroup diag_misc
  @brief    
    This procedure waits for specified signals to be set.  While it waits, 
    this procedure continues to set the Watchdog report signal and reset 
    the diag_rpt_timer. 
    
    To avoid deadlock situations, this function also waits on the MC task
    sigs.  Since diag doesn't really care when power goes away, the func
    immediately ack's MC.
    
    Depends on previously having used the diag_rpt_timer to time when to
    set the watchdog signal.
    
    @param[in] mask signal mask input the api
    
    @dependencies
    diag_rpt_timer
    
    @sideeffects
    None
    
    @return One of the following error codes 
          <ul> 
             <li> Error Code 0  – on success
             <li> Error Code -1 – on failure
          </ul>
    @sa
    None
 
*/ 
dword diag_wait (dword mask);


/*! 
  @ingroup diag_misc
  The following is used by the packet service to store rsp 
  packets. 
*/

typedef struct
{
  unsigned int pattern;     /*!< Pattern to check validity of committed pointers. */
  unsigned int size;        /*!< Size of usable buffer diagpkt_q_type->pkt */
  unsigned int length;      /*!< Size of packet */

  byte pkt[1];           /*!< Sized by 'length' field. */
}
diagpkt_rsp_type;

#endif /* DIAGI_H */
