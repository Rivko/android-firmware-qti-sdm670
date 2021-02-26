#ifndef SLIMBUSTARGET_H
#define SLIMBUSTARGET_H
/*
===========================================================================

FILE:         SlimBusTarget.h

DESCRIPTION:  This file implements the SLIMbus core driver target-specific
              defines and function prototypes.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/dal/SlimBusTarget.h#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/18/15   MJS     Support for second Slimbus instance.
03/18/15   MJS     Add support for QuRT timed wait.
07/25/13   MJS     Use DALSYS busywait instead of QuRT.
02/15/13   MJS     Add parameter for voltage voting clock gear.
10/25/12   MJS     Support for role reversal.
09/12/11   MJS     Improved logging for error conditions, added DIAG logging.
05/25/11   MJS     Add pause clock support.
04/14/11   MJS     Add logging facility.
06/21/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010, 2011, 2012, 2013, 2015 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include <qurt.h>
#include "SlimBusDiagULog.h"
#include "SlimBusTypes.h"
#include "ddislimbus.h"
#include "timer.h"

#define SLIMBUS_TARGET_NUM_DEVICES 2

#define SLIMBUS_INLINE  static __inline

#define SLIMBUS_OS_H "SlimBusDal.h"

#define SLIMBUS_STATIC_API static

#define SLIMBUS_BUSYWAIT(us) DALSYS_BusyWait(us)

#define SLIMBUS_SLEEP(us) timer_sleep(us, T_USEC, FALSE)

#define SLIMBUS_CLOCK_ON(pDevCtxt)  SlimBus_ClockOn(pDevCtxt)

#define SLIMBUS_CLOCK_OFF(pDevCtxt)  SlimBus_ClockOff(pDevCtxt)

#define SLIMBUS_VOLTAGE_CTRL(pDevCtxt, uClkGear)  SlimBus_VoltageControl(pDevCtxt, uClkGear)

/* Uncomment below to enable invasive unit test */
#if 0
#define SB_TEST_DEFINE() \
struct \
{ \
  DALBOOL ignore_replies; \
  DALBOOL force_nack; \
} _g_sb_debug
#define SB_TEST_DECLARE() \
extern struct \
{ \
  DALBOOL ignore_replies; \
  DALBOOL force_nack; \
} _g_sb_debug
#define SB_TEST_REPLIES_IGNORED() (_g_sb_debug.ignore_replies)
#define SB_TEST_NACK_FORCED() (_g_sb_debug.force_nack)
#else
#define SB_TEST_DEFINE() 
#define SB_TEST_DECLARE() 
#define SB_TEST_REPLIES_IGNORED() (FALSE)
#define SB_TEST_NACK_FORCED() (FALSE)
#endif


/**
 * @brief Turn on the Slimbus clock
 *
 * This function turns on the Slimbus clock
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_ClockOn(struct SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Turn off the Slimbus clock
 *
 * This function turns off the Slimbus clock
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_ClockOff(struct SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Control the voltage rail for Slimbus
 *
 * This function controls the voltage rail for Slimbus 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] uReqClkGear  Requested clock gear
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_VoltageControl(struct SlimBusDevCtxt *pDevCtxt, uint32 uReqClkGear);


#endif /* SLIMBUSTARGET_H */

