#ifndef DIAGTUNE_H
#define DIAGTUNE_H
/*==========================================================================

                 Diagnostic Target Specific Prototypes

Description
  Prototypes for functions and data members that must be implemented to make
  the core diagnostics service work.
  

Copyright (c) 2001-2006,2009-2012,2016 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            Edit History

$PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagtune.h_v   1.11   03 Jun 2002 17:06:42   amcintos  $
$Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/common/src/diagtune.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
05/09/13   sr      Added support for diag mask change notification for 
                   registered users 
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush. 
10/15/12   rh      Changed NRT values for Diag draining. 
07/05/12   sg      Changes to bring up diag Multi PD
02/28/11   sg      Power saving changes
07/12/10   sg      Added log codes for DTV
06/14/10   is      Renamed FEATURE_WCDMA and FEATURE_GSM to internal Diag versions.
05/11/10   is      Mainlined LOG_EQUIP_ID_11_LAST_CODE
04/08/10   vs      Reduced drain timer len to 200
03/06/10   vg      Removed FEATURE_DTV 
01/10/10   vs      Removed include of log_codes_apps.h
09/09/09   JV      Moved diag_heap_size and diagbuf_size to the new diag_cfg.h
09/09/09   as      Moved DIAG packet TX and RX Max size macros to diagpkt.h
08/20/09   vs      Increased drain timer len to 500
07/21/09   mad     Mainlined last log code for equipment ID 1
07/12/09   mad     Featurized feature-dependencies for constants 
                   DIAGBUF_SIZE, DIAG_MAX_RX_PKT_SIZ
07/10/09   as      Removed support for FEATURE_DIAG_SPC_TIMEOUT
03/20/09   mad     Increased diag buf size to 128k
12/21/06   as      Changed diag drain timer timeout.
12/05/06   as      Added timeout for diag drain timer 
05/10/06   bt      Added support for DTV.
09/15/05   as      Changed diag SPC timeout for FEATURE_DIAG_SPC_TIMEOUT.
12/06/04   as      Added support for FTM (FEATURE_FACTORY_TESTMODE)
01/29/04   as      Corrected FEATURE, to fix DIAGBUF_SIZE definition
11/12/02   pj      Increased diagbuf size to 30KB for MS-Based
                   (FEATURE_GPSONE_PE).
05/31/02   ATM     Renamed FEATURE_GPLT to GSM_PLT
05/15/02   gsc     Make top level FEATURE symbol (FEATURE_GSM, FEATURE_WCDMA)
                   multi-mode compliant.
03/08/02   kar     Changes for RPC
02/06/02   ATM     Cleanup - renamed log codes to match convention
02/06/02   ATM     GPLT needed 1X codes temporarily till RPC is fixed
01/24/02   ATM     KOVU needs its own version of this tune file -- we don't need
                   other log codes from 1x/wcdma/etc...
11/16/01   gsc     Incorporated review comments from Ajit for changes
                   related to DIAGBUF_SIZE 
11/15/01   gsc     Added support for PLT (FEATURE_WPLT)
06/27/01   lad     Added multiple equip ID logging support.
                   Updated comments to be more useful.
04/17/01   lad     Added constants for event services.
02/23/01   lad     Created file.

===========================================================================*/

#include "log_codes.h" /* To get default # of items for log equip IDs. */

#if (defined (DIAG_WCDMA) || defined (DIAG_GSM))

#include "log_codes_umts.h"  /* For LOG_UMTS_LAST_C */

#if defined (DIAG_WCDMA)
#include "log_codes_wcdma.h" /* For LOG_WCDMA_LAST_C */
#endif

#if defined (DIAG_GSM)
#include "log_codes_gsm.h"   /* For LOG_GSM_LAST_C */
#endif

#endif /* DIAG_WCDMA || DIAG_GSM */


/* By default, all messages and logs leave this much padding free when
 * allocating an outbound packet.  This way, all logs and messages have the
 * same probability of success, as long as all of those packets are smaller
 * than this number.  If a given packet is larger, it will be at a
 * disadvantage.  The basic algorithm is as follows:
 *
 * if (DIAGBUF_BALANCE_PAD < DIAGBUF_FREE) { 
 *   //go ahead and attempt to alloc 
 * }
 *
 * This way, an attempt to alloc 12 bytes will be just as likely to be
 * successful as an attempt to alloc 100 bytes.
 *
 * However, an attempt to alloc more than the PAD will not benefit from this
 * balancing scheme. */
#define DIAGBUF_BALANCE_PAD 0x140

#define DIAG_IDLE_PROC_CNT 10

/* Amount of time to timeout when a bad SPC
 * is received (10 secs = 10000 msecs). */
/* FEATURE_DIAG_SPC_TIMEOUT is defined in Sirius target */
#define DIAG_TOUT_TIME 10000

/* Search lists, used for the listener API, are allocated in blocks as items
   are inserted.  This constant indicates the increment size of a list. 
   Each node of the list is 8 bytes */
#define DIAG_SEARCHLIST_SIZE_INCR (16)
                                     
#define DIAG_CMD_READ_TOUT_TIMER_LEN    200

/* Time (ms) for sio timeout timer */
#define DIAG_SIO_TIMEOUT_TIMER_LEN 400

/* This specifies the last equipment ID for use in logging services.  This is a
 * 4-bit value, so it cannot exceed 15. */
#define LOG_EQUIP_ID_LAST LOG_EQUIP_ID_LAST_DEFAULT

/* Each equipment ID has a maximum number of items.  Each item takes up a bit
 * in the mask and a byte in the log_nice array.  Therefore, the highest number
 * per equipment ID will cause an allocation of 'n' bytes, where 'n' is defined
 * below ('x' is the last item # for an equipment ID):
 * n(x) = ((x+7) / 8) + x * sizeof(uint16) */
  
#define LOG_EQUIP_ID_1_LAST_CODE LOG_LAST_C   /* EquipID not supported */

#ifdef DIAG_WCDMA
  #define LOG_EQUIP_ID_4_LAST_CODE LOG_WCDMA_LAST_C
#endif

#ifdef DIAG_GSM
  #define LOG_EQUIP_ID_5_LAST_CODE LOG_GSM_LAST_C
#endif

#if (defined (DIAG_WCDMA) || defined (DIAG_GSM))
  #define LOG_EQUIP_ID_7_LAST_CODE LOG_UMTS_LAST_C
#endif

#define LOG_EQUIP_ID_10_LAST_CODE  ( 0x380 + LOG_DTV_BASE_C + 0x0A ) /* Ten additional ones reserved*/
 
#define LOG_EQUIP_ID_11_LAST_CODE LOG_APPS_LAST_C

#endif /* DIAGTUNE_H */
