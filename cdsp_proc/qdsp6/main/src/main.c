/**
@file Main.c

@brief This file contains the user level boot code for LPASS

*/
/*-------------------------------------------------------------------------------------------*/
/* Version information:                                                                      */



/* $Header: //components/rel/dspcore.adsp/4.0/main/src/main.c#4 $    */
/* $DateTime: 2018/05/24 06:24:28 $                                                          */
/* $Author: pwbldsvc $                                                                            */
/*-------------------------------------------------------------------------------------------*/
/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------

07/04/11   mspk    updated the original main.c
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2010 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#include <string.h>
#include <errno.h>
#include <stdio.h>
/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================= */
#include "qurt.h"
#include "engg_timestamp.h"
#include "qcom_timestamp.h"
#include "msg_diag_service.h"

#if defined(SIM)

#include "qurt_elite.h"
#include "EliteMain.h"

/* APR */
#include "aprv2_api_inline.h"
#include "apr_errcodes.h"
#include "aprv2_msg_if.h"
#include "aprv2_api.h"
qurt_elite_signal_t global_wait_signal_at_main;

#endif //#if (SIM)


//#include "icbarb.h"

#if (PLATFORM_INTEGRATION==1)
#include "platform_libs.h"
#endif

#if (SENSORS_INTEGRATION==1)
#include "sensor_pdinit.h"
#endif

#ifdef MALLOC_DEBUG
    #include "dbg_malloc.h"
#endif


#if defined(SIM)
extern void DALSYS_Init(void);
extern void timer_task_init(void);
extern void timer_qdi_init(void);
extern void timer_task(void *);
extern int32 apr_init(void); //int32_t is typedef as int32
#else // !SIM
extern void coremain_main(void);
#endif // SIM
extern unsigned int image_pstart;
extern unsigned int image_vstart;
#define TS_STRING_SIZE 90
static char Engg_ImageCreationDate[TS_STRING_SIZE];
static char Qcom_ImageCreationDate[TS_STRING_SIZE];
static char Adsp_StartAddress[50];

#if (ADSPSYSMON_INTEGRATION == 1)
//ADSP SysMon integration
#include "sysmon_prof.h"
#endif

#if (SENSORS_INTEGRATION==1)
//extern void sns_init();
//extern int sns_sensors_playback_test(void);
#endif // Sensors

#if (VIDEO_INTEGRATION==1)
extern int video_init(void);
extern int video_deinit(void);
volatile boolean RunVideo = TRUE;
#endif

#if (WLAN_INTEGRATION==1)
extern void wls_init(void);
volatile boolean RunWlan = TRUE;
#endif

#if (LOWI_INTEGRATION==1)
extern void lowi_init(void);
volatile boolean RunLowi = TRUE;
#endif

#if (STRESS_TEST==1)
extern void stress_test_init(void);
volatile boolean RunLatency = TRUE;
#endif


#if defined(SIM)
volatile boolean do_clk_regime_init = TRUE;
volatile boolean do_icbarb_tests    = TRUE;
// OpenDSP run time control
volatile boolean adsptest_cosim_present = FALSE;
volatile boolean RunCoremain = TRUE;
volatile boolean RunAudio = TRUE;

int SMP2PSTATUS1;
volatile boolean RunSMP2P = TRUE;
volatile boolean RunSensors = FALSE;
#else
volatile boolean RunSensors = TRUE;
#endif

volatile boolean RunTests = FALSE;

void multi_pd_spawn(void)
{
   qurt_printf("qurt pid is %d\n", qurt_getpid());
   //Audio image names are Macros defined in the scons based on the build rev and other params
   //The User process are spawned based on whether we have both sensor and user pd or just one
#ifdef SPAWN_AUDIO_USERPD
    int cid2=0;
    cid2=qurt_spawn(AUDIO_IMG_NAME);
    qurt_printf("Process Audio launched with ID=%d\n", cid2);
    // MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_HIGH, "Process Audio launched with ID=%d\n", cid2);
#endif
   
}

#if defined(SIM)
int main(void)
{

#if defined(SIM)
    DALSYS_Init();
	timer_task_init();
	//start timer_task thread
	static char ATSThreadName[] = "ATS_Timer";
	static char pATSTimerStack[4096];
	qurt_thread_t Tid;
	qurt_thread_attr_t attr;
	qurt_thread_attr_init (&attr);
	qurt_thread_attr_set_stack_size (&attr, 4096);
	qurt_thread_attr_set_stack_addr (&attr, pATSTimerStack);
	qurt_thread_attr_set_priority (&attr, (unsigned short)5);
	qurt_thread_attr_set_name(&attr, (char*)ATSThreadName);
	qurt_thread_create(&Tid, &attr, timer_task, NULL);
    timer_qdi_init();
#else // !SIM
    if(RunCoremain)
    {
        coremain_main();
    }
#endif // SIM

#ifdef SPAWN_AUDIO_USERPD
  apr_init();
  printf("after apr_init\n");
  multi_pd_spawn();
  printf("after multi pd spawn\n");
  qurt_signal2_t stop_signal;
  qurt_signal2_init(&stop_signal);
  qurt_signal2_wait_all(&stop_signal,0xFFFFFFFF);
#endif //SPAWN_AUDIO_USERPD

#if (BAREBONE_ADSP == 1)
    for (;;) /*NULL*/;
#endif
    
#if (BAREBONE_ADSP != 1)
    
  #if (PLATFORM_INTEGRATION==1)
    if (0 != pl_init())
    {
        qurt_printf("\n Platform init pl_init() failed \n"); 
        ERR_FATAL("QDSP6 Main.c: pl_init() failed ",0,0,0);
    }
  #endif

#if (SENSORS_INTEGRATION==1)    
    if (RunSensors)
    {
         // multi-pd spawn
        pd_sensor_init();
    }
#endif

#ifdef DALTF_APTTEST_ON
    apttests_daltf_init();
#endif

#if (ADSPSYSMON_INTEGRATION == 1)
    /*
     * ADSP SysMon:
     *  Starts a thread for sysmon
     *  Needs header 'sysmon_prof.h'
     */
    // sysmon_profiler_init();
#endif
#endif // BAREBONE_ADSP


#if (BAREBONE_ADSP != 1)
#ifndef SPAWN_AUDIO_USERPD // don't run elite_framework_start() if Audio is in User PD, AUdio has to init and run from PD
    if(RunAudio)
    {
        if (ADSP_EOK != elite_framework_start())
        {
#if defined(SIM)
            elite_framework_deinit();
#endif
            qurt_printf("\n elite_framework_start() failed \n"); 
            ERR_FATAL("QDSP6 Main.c: elite_framework_start() failed ",0,0,0);
        }
    }
#endif // SPAWN_AUDIO_USERPD
#if (VIDEO_INTEGRATION==1)
    if (RunVideo)
    {
        video_init();
    }
#endif //video

#if (LOWI_INTEGRATION==1)
    if (RunLowi)
    {
        lowi_init();
    }
#endif //lowi


#if (WLAN_INTEGRATION==1)
    if (RunWlan)
    {
        wls_init();
    }
#endif //wlan

#if (STRESS_TEST==1)
   MSG( MSG_SSID_QDSP6, MSG_LEGACY_HIGH, "Initializing stress test");
   if (RunLatency)
   {
       stress_test_init();
   }
#endif //latency

#ifndef SIM
  #ifndef SPAWN_AUDIO_USERPD 
    // back ground indefinite wait starts
    qurt_elite_channel_t aChannel;
    qurt_elite_channel_init( &aChannel);

    if (ADSP_FAILED( qurt_elite_signal_init( &global_wait_signal_at_main)))
    {
        qurt_printf("\n qurt_elite_signal_init() failed to create signal \n"); 
        ERR_FATAL("QDSP6 Main.c: qurt_elite_signal_init() failed ",0,0,0);
    }
    if (ADSP_FAILED( qurt_elite_channel_add_signal( &aChannel, &global_wait_signal_at_main, 0) ) )
    {
        qurt_printf("\n qurt_elite_channel_add_signal() failed to add signal \n"); 
        ERR_FATAL("QDSP6 Main.c: qurt_elite_channel_add_signal() failed ",0,0,0);
    }
    uint32 unListenerChannelBits = qurt_elite_signal_get_channel_bit(&global_wait_signal_at_main);
  #else // SPAWN_AUDIO_USERPD
    qurt_signal2_t stop_signal;
    qurt_signal2_init(&stop_signal);
  #endif

   
  #ifdef SPAWN_AUDIO_USERPD
    //wait_forever();
    MSG(MSG_SSID_QDSP6, MSG_LEGACY_HIGH, "main.c: enter wait_forever");
    qurt_signal2_wait_all(&stop_signal,0xFFFFFFFF);
    MSG(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "main.c: exit wait_forever");
  
  #else
   // Block here foreever on target
    (void) qurt_elite_channel_wait(&aChannel, unListenerChannelBits );
  #endif //SPAWN_AUDIO_USERPD

#endif // !SIM

  #ifndef SPAWN_AUDIO_USERPD
   if (ADSP_EOK != elite_framework_stop())
   {
#if defined(SIM)
        elite_framework_deinit();
#endif
        qurt_printf("\n elite_framework_stop() failed \n"); 
        ERR_FATAL("QDSP6 Main.c: elite_framework_stop() failed ",0,0,0);
   }

    if (ADSP_EOK != elite_framework_deinit())
    {
        qurt_printf("\n elite_framework_deinit() failed \n"); 
        ERR_FATAL("QDSP6 Main.c: elite_framework_deinit() failed ",0,0,0);
    }
  #endif // SPAWN_AUDIO_USERPD

#if (VIDEO_INTEGRATION == 1)
    if (ADSP_EOK != video_deinit())
    {
        qurt_printf("\n video_deinit() failed \n"); 
        ERR_FATAL("QDSP6 Main.c: video_deinit() failed ",0,0,0);
    }
#endif


#if (PLATFORM_INTEGRATION==1)
    pl_deinit();
#endif
#endif // BAREBONE_ADSP
    return 0;
}
#endif

void image_creation_timestamp(void)
{
    strlcpy(Engg_ImageCreationDate,engg_timestampstring,TS_STRING_SIZE);
    strlcpy(Qcom_ImageCreationDate,qcom_timestampstring,TS_STRING_SIZE);
    snprintf ( Adsp_StartAddress, 50, "*ADSP_START_ADDRESS: PA-0X%x VA-0X%x", image_pstart, image_vstart  );
    qurt_printf("\n*ADSP_STARTADDRESS - %s*",Adsp_StartAddress);
    qurt_printf("\n**********************************\n* DSP Image Creation Date: %s\n*******************************************************\n",Engg_ImageCreationDate);
    MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_MED, "Engineering Image Creation Date: %s\n", Engg_ImageCreationDate);
    MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_MED, "Qcom Image Creation Date: %s\n", Qcom_ImageCreationDate);
}

