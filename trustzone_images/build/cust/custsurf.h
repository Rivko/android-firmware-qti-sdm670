#ifndef CUSTSURF_H
#define CUSTSURF_H
/*===========================================================================

DESCRIPTION
  Configuration for SURF Target using MSM6500

  Copyright (c) 2004  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custsurf.h#1 $ $DateTime: 2018/02/06 03:02:41 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/06   hn      Removed C++ style comments for L4 kernel build.
07/19/05   grl     Added FEATURE_TMC_TRM and changed to FEATURE_TMC_TCXOMGR
07/13/05   adm     Merged in from mainline ver#3
06/28/05   ptm     Remove // comments for L4 merge.
06/14/05   grl     Added FEATURE_TCXOMGR_TMC define.
11/11/02/  cah     Added GSM features for 6300.
01/30/04/  ems     Ported from MSM6100
===========================================================================*/

/* ------------------------------------------------------------------------
** Defined for Leo - Likely Not Needed
** ------------------------------------------------------------------------ */
/* 32KHz Crystal oscilator. If not defined 4.096MHz oscillator is default. */
#define FEATURE_32K_XTAL

/* Provides support for a SURF with the MSM3300 ASIC.
*/
#define FEATURE_SURF3300

/* Use 19.68MHz TCXO clock as vocoder clock for EVRC mode.
*/
#define FEATURE_TCXO_CLOCK

/* ------------------------------------------------------------------------
** Features that will change for Cougar
** ------------------------------------------------------------------------ */

/* Use a 4MB Flash part for the first FLASH
*/
#define FEATURE_4MB_ROM

/* -----------------------------------------------------------------------
** MSM6050 Specific
** ----------------------------------------------------------------------- */

/* For MSM6050, the scan lines are split into GPIO2 and GPIO3
*/
#undef FEATURE_SPLIT_SCAN_LINES

/* 20 is not ready for prime time(per venkatt)#define FEATURE_CM20_INTERFACE */
/* Enables support for association of call-back functions with REX timers.
*/
#define FEATURE_REX_TIMER_EX
/* Enable Multimode architecture.
*/
#define FEATURE_MULTIMODE_ARCH

/* Reqd for OEM */
/* Enables CM clients to interface with CM3.0.
 * (this should be always turned on going forward as we will be using only
 * CM3.0 and shim layer will not be supported)
*/
#define FEATURE_CM_MULTIMODE
/* Turns on asynchronous procedure call support. Allows the user to queue
 * a function to be called in the context of a specific task. When the
 * task for which the APC is queued is scheduled for execution next,
 * the APC is called immediately.
*/
#define FEATURE_REX_APC

#ifdef FEATURE_GSM
/* GSM Multimode Operation.
*/
#define FEATURE_DUALMODE_BASELINE

/* GSM Supplementary Services.
*/
#define FEATURE_SUPS
#endif /* FEATURE_GSM*/

/* Enable MDSP generic half slot logging feature.
*/
#define FEATURE_MDSPSVC_LOG

/*---------------------------------------------------------------------------
                     TMC supported Tasks
---------------------------------------------------------------------------*/
#define FEATURE_SLEEP                              /* Support Sleep Task */

#define FEATURE_DOG                                  /* Support Dog Task */

#define FEATURE_UI                                    /* Support UI Task */

#define FEATURE_SND_TASK                             /* Support SND Task */

#define FEATURE_HS                                    /* Support HS Task */

#define FEATURE_NV                                    /* Support NV Task */

#define FEATURE_DIAG_TASK                           /* Support DIAG Task */

/*---------------------------------------------------------------------------
                     TMC Specific Feature Defs
---------------------------------------------------------------------------*/

/* Turns on initialization and shutdown of the TCXOMgr service within TMC.
*/                           
#define FEATURE_TMC_TCXOMGR

/* Turns on initialization of the TRM service within TMC. 
 */
#define FEATURE_TMC_TRM

/* No smem logging in boot code */
#ifdef BUILD_BOOT_CHAIN
  #undef FEATURE_SMEM_LOG
#endif

#endif /* CUSTSURF_H */

