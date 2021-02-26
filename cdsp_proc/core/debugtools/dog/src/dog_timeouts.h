#ifndef DOG_TIMEOUTS_H
#define DOG_TIMEOUTS_H
/** vi: tw=128 ts=3 sw=3 et
@file dog_timeouts.c
@brief This file contains the API details for the Dog Services, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2017 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/dog/src/dog_timeouts.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#if defined(__cplusplus)
extern "C"
{
#endif

/* DOG runs off Sleep clock with Freq: 32 KHz */
/* sclk = 32KHz, 0.03125ms period, 1ms = 32sclk */
#define MS_TO_SCLK(ms)  ((ms)<<5) /* x32 */

/* Default DOG_SCALING_FACTOR = 1 (Kick: 200ms, Bark: 500ms, NMI: 700ms, Bite: 800ms) */
/* User should change this constant in the SCONS file ONLY */
#if !defined(DOG_SCALING_FACTOR)
#define DOG_SCALING_FACTOR 1 
#endif

#define DOG_HB_KICK_PERIOD                  5
/* Actual DOG_DEFER_KICK_TIME_MS = (200 * DOG_SCALING_FACTOR) */
#define DOG_DEFER_KICK_TIME_MS              200  // milliseconds period for HAL_dogDeferKickTimerCb

#define DOG_BARK_ENABLE_TIMEOUT_MS          (500 * DOG_SCALING_FACTOR)
#define DOG_NMI_ENABLE_TIMEOUT_MS           (DOG_BARK_ENABLE_TIMEOUT_MS + 200)
#define DOG_BITE_ENABLE_TIMEOUT_MS          (DOG_BARK_ENABLE_TIMEOUT_MS + 300)

#define DOG_BARK_ENABLE_TIMEOUT_SCLK        MS_TO_SCLK(DOG_BARK_ENABLE_TIMEOUT_MS)
#define DOG_NMI_ENABLE_TIMEOUT_SCLK         MS_TO_SCLK(DOG_NMI_ENABLE_TIMEOUT_MS)
#define DOG_BITE_ENABLE_TIMEOUT_SCLK        MS_TO_SCLK(DOG_BITE_ENABLE_TIMEOUT_MS)

#define DOG_BITE_ENABLE_TIMEOUT_NOW_SCLK    0x0001           // BITE "NOW"

#define DOG_NMI_ENABLE_TIMEOUT_SCLK_FORCE   MS_TO_SCLK(50)   // 50ms
#define DOG_BITE_ENABLE_TIMEOUT_SCLK_FORCE  MS_TO_SCLK(150)  // 150ms 

#define DOG_STARTUP_GRACE_TIME_SEC          40 // sec period for HAL_dogStartupGraceFailureCb

#if defined(__cplusplus)
}
#endif

#endif //DOG_TIMEOUTS_H