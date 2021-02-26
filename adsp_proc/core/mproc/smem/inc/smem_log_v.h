#ifndef SMEM_LOG_V_H
#define SMEM_LOG_V_H
/*===========================================================================

            S M E M L O G . H  

DESCRIPTION

Copyright (c) 2005-2009, 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
    A feature (in the smem subsystem) that logs info to a single shared buffer 
    in shared memory. Definitions for SMEM_LOG_EVENT API that can be compiled 
    away to nothing if FEATURE_SMEM_LOG is not defined.

USAGE
    Step 1: Add "use DEBUG" (excluding Quotes) to the build of your choice in 
            msm7500.builds
    Step 2: #include "smem_log.h" in the file you want to use the shared memory
            log buffer.
    Step 3: Start writing to shared memory log buffer with debug value(s) by 
            calling SMEM_LOG_EVENT. Quick example. 
			See services/oncrpc/oncrpctask.c

===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/smem/inc/smem_log_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/11   hwu     Added SMEM log ID for RIVA.
05/25/11   tl      Include smem_target.h for SMEM_*_PROC macros
10/27/09   tl      Added support for 9600: Use SMEM_*_PROC macros
06/10/09   bfc     CMI updates.
04/01/09   ih      Removed inclusion of smem_target.h
03/13/09   tl      Added support for OEM idle messages
03/20/08   sa      Added support for rpc router messages.
08/16/07   ebb     Added smem_log_event_static declaration.
06/29/07   ptm     Add sleep event base.
03/20/07   taw     Include smem_target.h instead of customer.h, comdef.h and
                   target.h.
11/21/06   hwu     Added SMEM_LOG_DCVS_EVENT_BASE.
11/14/06   taw     Added the SMEM_LOG_NUM_ENTRIES define and removed the 
                   arguments to smem_log_init().
05/16/06   ptm     Added new smem event base symbol for ERROR.
03/24/06   ptm     Add init API.
02/22/06   ptm     Add 6 data value API.
01/27/06   ddh     Added new smem event base symbol for DEM
04/20/05   ptm     Add event base symbols for debug, smem, tmc and timetick.
03/17/05   ~SN     Initial Version.

===========================================================================*/
#include "smem_log.h"
#include "smem_target.h"

/*----------------------------------------------------------------------
 *  Event indentifier format:
 *    bits 31-30 are processor ID: 10 => apps, 00 => modem, 
 *                                 01 => LPASS,11 => RIVA
 *    bits 29-28 are continuation number
 *    bits 27-16 are subsystem id (event base)
 *    bits 15-0  are event id
 *----------------------------------------------------------------------*/

#ifdef SMEM_APPS_PROC
#define SMEM_LOG_PROC_ID        0x80000000
#elif  defined(SMEM_MODEM_PROC)
#define SMEM_LOG_PROC_ID        0x00000000
#elif  defined(SMEM_LPASS_PROC)
#define SMEM_LOG_PROC_ID        0x40000000
#elif  defined(SMEM_RIVA_PROC) || defined(SMEM_SLPI_PROC) || \
       defined(SMEM_CDSP_PROC)
#define SMEM_LOG_PROC_ID        0xC0000000
#else
#error "SMEM_LOG_PROC_ID not defined"
#endif

#define SMEM_LOG_CONTINUE_1     0x10000000

/* Maximum number of smem_log_entries */
#define SMEM_LOG_NUM_ENTRIES        2000



#endif /*SMEM_LOG_V_H*/
