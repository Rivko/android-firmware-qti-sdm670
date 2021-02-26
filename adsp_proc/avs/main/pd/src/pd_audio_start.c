/*=============================================================================
  @file sns_pd.c

  This file contains sensors PD initialization code.

 *******************************************************************************
 * Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary.
 ********************************************************************************/

/* $Header: //components/rel/avs.adsp/2.8.5/pd/src/pd_audio_start.c#4 $ */
/* $DateTime: 2018/04/04 23:25:35 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  08/06/2014   SL   Integrating file to avs\audio_pd and making changes for audio in user pd
  2013-10-18  JHH   Remove timer and wait forever
  2013-08-22   PK   Increased SNS heap size for Hammerhead
  2013-07-26   VH   Eliminated compiler warnings
  2012-12-02   AG   Initial version
  2013-02-05   AG   Use RCInit, remove previous workaround
============================================================================*/

//NOTE: Audio PD initialization method from root is different on MDM and MSM. For long term, see if they can be converged.
#ifdef MDSPMODE
#include "pd_mon_qurt.h"

void pd_mon_audio(void)
{
    PD_MON_HANDLE hPDAudio;
	hPDAudio = pd_mon_spawn(NULL, AUDIO_IMG_NAME); 
}
#else
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdio.h>
#include <qurt.h>
#include "qurt_timer.h"
#include "qurt_elite.h"
#include "EliteMain.h"
#include "audio_pd_build_version.h"



//#include "sns_init.h"

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
/* Heap size to be defined by each User PD Image owner */
//According to msm8994 spec, we need at least 6.5MB(0x632EA0)

#ifdef SIM
#define AUDIO_PD_HEAP_SIZE         0xFF0000 //TODO: this should have been 0x20000
#elif sdm670
#define AUDIO_PD_HEAP_SIZE         0x300000
#else
#define AUDIO_PD_HEAP_SIZE         0x900000
#endif


// Audio PD heap define for Multi-PD. Note: AUDIO_PD_HEAP_SIZE is to be defined in target.builds
#define GCONST const __attribute__((section(".start")))
static char my_sbrk_heap[AUDIO_PD_HEAP_SIZE] __attribute__((aligned(4096)));
void * GCONST override_heap_Base = &my_sbrk_heap[0];                   // sys_sbrk looks for this symbol
void * GCONST override_heap_Limit = &my_sbrk_heap[AUDIO_PD_HEAP_SIZE]; // sys_sbrk looks for this symbol
unsigned GCONST heapSize = AUDIO_PD_HEAP_SIZE;                         // mem check script looks for this symbol

static char Engg_ImageCreationDate_Pd_Audio[120];
static char Qcom_ImageCreationDate_Pd_Audio[120];

extern void coremain_main(void);
int main (void)
{
    strlcpy(Engg_ImageCreationDate_Pd_Audio,engg_timestamp_pd_audio,120);
    strlcpy(Qcom_ImageCreationDate_Pd_Audio,qcom_timestamp_pd_audio,120);
   	qurt_signal_t foreverWaitAudPD;
	qurt_printf("Inside the audio Image Main function \n");
	qurt_printf("user pid is %d\n", qurt_getpid());
	/* Init global state structure */
	/* Core Init for user PD */
	qurt_printf("Calling AudioPD coremain\n");
	coremain_main();
	qurt_printf("enabling elite framework\n");
#ifdef SIM
 
	elite_framework_sim_execution();
 
#else


	qurt_printf("Core Init for audio image done\n");


	qurt_signal_init(&foreverWaitAudPD);
	/* Currently User Process exit is not supported and the
	       User PD Images will have their exception handlers */
	while(1){
		qurt_signal_wait (&foreverWaitAudPD,((unsigned int) 0xFFFFFFFF), ((unsigned int)QURT_SIGNAL_ATTR_WAIT_ALL)); // wait for ever
	}

	qurt_signal_destroy(&foreverWaitAudPD);
#endif
	return 0; /* never reaches, no user exit handling yet */
}
#endif
