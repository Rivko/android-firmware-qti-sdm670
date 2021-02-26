/*=============================================================================
  @file pd_audio_spawn.c

  This file contains sensors PD initialization code.

*******************************************************************************
* Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved
* Qualcomm Technologies Confidential and Proprietary.
********************************************************************************/

/* $Header: //components/rel/dspcore.adsp/4.0/main/src/pd_audio_spawn.c#1 $ */
/* $DateTime: 2017/03/30 23:23:54 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-06   AJ   pd spwan for audio

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "qurt.h"
#include "pd_mon_qurt.h"
#include "qurt_elite.h"

/* APR */
#include "aprv2_api_inline.h"
#include "apr_errcodes.h"
#include "aprv2_msg_if.h"
#include "aprv2_api.h"

void pd_mon_audio(void)
{
    PD_MON_HANDLE hPdAudio;
   PD_MON_RESTART restart = PD_MON_RESTART_ALWAYS;
   hPdAudio=pd_mon_spawn(&restart, AUDIO_IMG_NAME);
   if (hPdAudio == 0){
    qurt_printf("%s: failed to spawn ... restart: %d \n", __func__, restart);
  }
  else{
  qurt_printf("%s: Protection Domain Launched %s\n", __func__, AUDIO_IMG_NAME);
}
}

void fatal_debug_error()
{

#if defined(__qdsp6__) && !defined(SIM)
   ERR_FATAL("FATAL_ERR: Force crash Q6 due to AFE signal miss", 0, 0, 0);
#endif

   return; /* After entering ERR_FATAL(), execution never comes back here */
}

