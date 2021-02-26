
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diag Legacy Service Mapping DLL

GENERAL DESCRIPTION

Implementation of entry point and Initialization functions for Diag_LSM.dll.


EXTERNALIZED FUNCTIONS
Diag_LSM_Init
Diag_LSM_DeInit

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007-2010, 2012-2018 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/qurt/src/Diag_LSM.c#6 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
01/19/18   sa     Add ERR_FATAL when buffer allocation fails
11/28/17   gn     Fixed issue with cmd reg_dereg test case
09/29/17   sa     Remove mask update from diag_lsm_init to avoid updating mask before diagID is set.
08/07/17   sp     Fix KW errors
06/08/17   gn     Added support for diag services to uImage
06/12/17   sp     Add support for microULog debug capability
05/30/17   gn     Changes to support command deregistration
05/12/16   sa     Circular Mode for UserPD buffer
04/24/17   sp     Added support for extended logs in userPD
02/21/17   ph     Tag DIAG ID header to all the diag packets sent out.
02/13/17   nk     Added Log_submit() test case for User-PD
10/13/16   ph     Added handshake in the startup function of new task.
10/03/16   as     Migrated mutex lock calls to qurt_pimutex_lock()
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
06/20/16   sa     Added MSG_SPRINTF stress tests
05/12/16   ph     SMD references removal and code clean up.
01/06/15   ph     Stress test tasks now get created with correct priority.
11/18/15   gn     Added loopback command in user PD. 
07/15/15   xy     Fixed UserPD stress test issue 
07/10/15   rh     Added delayed response UserPD test.
06/18/15   sa      Added support for time sync feature.
04/28/15   vk     Enabled exit from Diag_LSM_Init on timer create failure.
11/25/14   ph     Removed test case DIAGDIAG_STRESS_TEST_ERR_FATAL_ISR.
11/14/14  jtm/vk  Multi-PD Optimizations 
10/09/14   sr     Added stress test in UserPD  
07/06/14   ph     Added support for Diag over STM
03/11/14   sr     Resolved compiler warnings
12/23/13   ph     Included header files required to account for removal of ASSERT macro
10/21/13   sr     Init the mutex diag_lsm_ref_count_mutex only once although
                  diag_LSM_init() is called multiple times.
10/10/12   sg     Print the failure cases in Debug Buffer when Diag_LSM_Init is called
07/05/12   sg     Changes to bringup Diag MultiPD
06/29/10   mad    Eliminating TerminateThread() call altogether. If Diag_LSM_RxThread
                  does not exit gracefully in 100ms, do not do anything.
06/28/10   mad    Signalling Diag_LSM_RxThread to exit gracefully. The thread
                  is terminated if it does not return within 100ms.
06/25/10   mad    Minimizing error-window for the case where CreateFile fails:
                  Instead of calling the blanket De-Init(), just reset what is
                  necessary, before returning from Diag_LSM_Init().
                  Also, check for INVALID_HANDLE_VALUE before calling IOCTL
                  in RegisterForMaskChange().
06/24/10   mad    CreateFile() is called for WDG driver, if Handle value
                  is INVALID OR NULL
04/21/10   sg     Inclusion of diag_v.h for diag_client_maskchange()
04/21/10   sg     Fix to close handles after we terminate the thread
04/20/10   sg     Fixing memory leak with ghDiag_Pkt_Process_Event,ghRxThread
12/03/09   sg     CreateThread() change to return HTHREAD, instead of HANDLE
09/16/09   sg     Included diagpkt.h for the definitions of diag packets
                  Tx and RX Max Sizes macros
06/04/09   mad    Closed Registry key in CreateWaitThread() to fix memory leak.
01/22/09   mad    Now calling Diag_LSM_DeInit() from Diag_LSM_Init(), if
                  windiag driver Open() call fails
01/15/09   mad    Added Synch. Event to signal diag thread reg. processing
                  packet from common table(e.g. mask change)
01/15/09   mad    Priority of Rx thread is now equal to diag thread priority
12/03/08   mad    Changes to take out Component services, and use
                   filesystem calls to access windiag driver, for diag
                  stream-driver model. Added an RxThread that waits for
                  mask-change and incoming-packet events.
10/31/08   mad    Fix for crash in Diag_LSM_Log_Init() when Diag_LSM_Init()-DeInit()
                  is called multiple times. Reset global handles etc explicitly to
                  NULL, in Diag_LSM_DeInit(), so they get initialized correctly
                  in the subsequent Diag_LSM_Init() call.
09/03/08   mad    Added reference count to maintain Diag_LSM functionality,
                  when multiple clients in same process call Diag_LSM_Init()
                  and Diag_LSM_DeInit().
04/23/08   JV     Added calls to functions that update masks during bring-up.
                  LSM registers for a mask change during Init.Now calling
                      diagpkt_bindpkt with the UnBind ID during de-init.
04/14/08   JV     Created instance of IDiagPktRsp
03/19/08   JV     Added packet service to init and de-init functions.
02/05/08   mad    Moved declarations for Diag_LSM_Init() and Diag_LSM_DeInit()
                  to Diag_LSM.h. This will enable clients to include
                  Diag_LSM.h and call the functions directly.
01/15/08   mad    Removed explicit linking to a1Host.lib, now using
                  LoadLibrary() and GetProcAddress() to call functions
                  exported by a1Host.dll. This was done to avoid a1Host.dll
                  being loaded implicitly upon loading of Diag_LSM.dll.
11/29/07   mad    Created

===========================================================================*/

/* separating windows-specific includes, in case this file is featurised
and made common later. */

#include "comdef.h"
#include "Diag_LSMi.h"           /* For DIAG_ERR_ULOG_SIZE */
#include "Diag_LSM_Pd.h"
#include "DiagSvc_Malloc.h"      /* For DiagSvc_Malloc_Init */
#include "Diag_LSM_Event_i.h"    /* For Diag_LSM_Event_Init */
#include "Diag_LSM_Log_i.h"      /* For Diag_LSM_Log_Init */
#include "Diag_LSM_Msg_i.h"      /* For Diag_LSM_Msg_Init */
#include "Diag_LSM_Pkt_i.h"      /* For Diag_LSM_Pkt_Init */
#include "diag_shared_i.h"       /* For SYNC_EVENT_DIAG_LSM_PKT_IDX, bindpkt_params_per_process */
#include "diagi_v.h"
#include "qurt_thread.h"
#include "qurt_mutex.h"
#include "qurt_atomic_ops.h"
#include "Diag_LSM.h"            /* For Diag_LSM_DeInit */
#include "Diag_LSM_Qdi_wrapper.h"
#include "stdio.h"
#ifdef DIAG_UIMAGE_MODE
#include "micro_diagbuf_mpd_type.h"
#endif

#ifdef FEATURE_DIAG_STM
#include "Diag_LSM_stm.h"        /* For diag_stm_init */
#endif

#include "osal.h"
#include "stringl.h"
#include "ULogFront.h"
#include "diag_param.h"
#include "servreg_locator.h" /*For servreg API to get process name*/
#include "diag_diagIDi.h"

ULogHandle diag_err_log_handle = NULL;

#ifdef DIAG_UIMAGE_MODE
extern micro_diagbuf_mpd_type *micro_diagmpd_buf;
#endif

extern boolean diag_services_initialized;

/* Action Codes for RegisterForMaskChange() function */
#define MASK_CHANGE_REGISTER     0
#define MASK_CHANGE_DEREGISTER   1

/* Extern section */
extern diagbuf_mpd_type *diagmpd_buf;

/* Static section */
static boolean TerminateWaitThread(void);
static boolean CreateWaitThread(void);
static boolean RegisterForMaskChange(uint32 ActionCode);


/* Global */
int diag_qdi_handle = -1;
qurt_thread_t thread_id;
/* This process's receive thread channel ID */

/* This process's pid */
int diag_pid;
/*A reference count to maintain the number of
clients in the same process. This will be used to:
1. Prevent re-initialization if already initialized in the same process
2. Prevent De-Initialization if other clients are still using Diag_LSM
*/
uint32 gnDiag_LSM_Ref_Count = 0;
qurt_mutex_t diag_lsm_ref_count_mutex;
static unsigned int diag_lsm_ref_count_mutex_init = FALSE;

#define DIAG_LSM_STACK_SIZE      (1024 * 4)
unsigned long long diag_lsm_stack[DIAG_LSM_STACK_SIZE];


/* Internal function declarations*/

/* To track the time API to be used */
diag_time_api_types diag_time_api_to_use_lsm = 0;

/* Variable related to the time API configuration */
boolean diag_time_initialized_lsm = FALSE;  /* Track if the diag time API is initialized */
static int diag_time_init_begin_lsm = FALSE;  

/* Function pointer to update the time API to be used */
uint8 (*diag_time_get_LSM)(qword);

PACKED void * stress_test_task (
  PACKED void *req_pkt,
    uint16 pkt_len,
    uint16 code
);

void Diag_LSM_stresstest(
   uint32 params
);


PACKED void* Diag_LSM_Health_Command( PACKED void *req_pkt, uint16 pkt_len, uint16 code);
PACKED void* diag_lsm_loopback( PACKED void *req_pkt, uint16 pkt_len, uint16 code );

PACKED void * diag_LSM_test_cmd_handler (
  PACKED void *req_pkt,
  uint16 pkt_len
);
PACKED void* diag_pd_buf_mode_change( PACKED void *req_pkt, uint16 pkt_len, uint16 code);

PACKED void* Diag_LSM_Delay_Test(PACKED void *req_pkt, uint16 pkt_len);

PACKED void* diag_lsm_subsys_reg_dereg_test( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_lsm_subsys_reg_test( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_lsm_subsys_reg_test2( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_lsm_subsys_reg_test3( PACKED void *req_pkt, uint16 pkt_len );
PACKED void* diag_lsm_subsys_reg_test4( PACKED void *req_pkt, uint16 pkt_len );
diag_pd_type diag_lsm_cur_userpd = DIAG_PD_NONE;
uint8 diag_get_diagID_LSM(void);

/*===========================================================================
FUNCTION   Diag_LSM_Init

DESCRIPTION
  Initializes the Diag Legacy Service Mapping Layer.

DEPENDENCIES
  Successful initialization of Diag in User PD requires Diag_LSM_Init to be
  called in UserPD.
  This function can be called multiple times; mutex initialization should happen only once

RETURN VALUE
  FALSE = failure, else TRUE

SIDE EFFECTS
  None

===========================================================================*/

boolean Diag_LSM_Init(byte *pParam)
{
  uint8* qsr4_guid_ptr = NULL;
  int return_val = 0;
  char* process_name;
  if (qurt_atomic_compare_and_set(&diag_lsm_ref_count_mutex_init, FALSE, TRUE))
  {
    qurt_pimutex_init(&diag_lsm_ref_count_mutex);
  }
  qurt_pimutex_lock(&diag_lsm_ref_count_mutex);
  if (0 == gnDiag_LSM_Ref_Count)
  {
    diag_pid = qurt_getpid();
    if (0 > diag_qdi_handle)
    {
      diag_qdi_handle = qurt_qdi_open("/dev/diag");
      if (diag_qdi_handle < 0)
      {
        ERR_FATAL("Diag_LSM_Init : QDI open failed ",0,0,0);
      }

      ULogFront_RealTimeInit(&diag_err_log_handle,"DiagErrorsLSM", DIAG_ERR_ULOG_SIZE, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
      ULogFront_RealTimePrintf(diag_err_log_handle,0,"LSM diag init");
	  
      //Now need to initialize the diagbuf used for the PD
      //This has to be done this way, so the user PD can get a pointer to the shared buffer that is created
      return_val = diag_lsm_get_diag_buf(&diagmpd_buf, DIAG_MULTIPD_BUF_SIZE, diag_pid);
      if (return_val < 0)
      {
        ERR_FATAL("Failed to get diagmpd_buf for Userpd, Size =%d ret_val=%d ", DIAG_MULTIPD_BUF_SIZE,return_val,0);
      }
#ifdef DIAG_UIMAGE_MODE
	  ULogFront_RealTimePrintf(diag_err_log_handle,0,"Diag_LSM_Init : Invoking root for shared handle and buffer");
    
	   /* Initialize micro diagbuf for this PD by invoking root to allocate a shared buffer*/
	  if (UIMAGE_UPD_DIAGBUFFER_SIZE <= 0)
      {
	    ULogFront_RealTimePrintf(diag_err_log_handle,0,"Diag_LSM_Init : UIMAGE_UPD_DIAGBUFFER_SIZE is zero or less than 0");
      }
	  else
	  {
	    return_val = diag_lsm_get_micro_diag_buf(&micro_diagmpd_buf, MICRO_ULOG_MEM_NEEDED(UIMAGE_UPD_DIAGBUFFER_SIZE));
        if (return_val < 0)
        {
          ERR_FATAL("Failed to get micro_diagmpd_buf for Userpd, Size =%d ret_val=%d ", UIMAGE_UPD_DIAGBUFFER_SIZE,return_val ,0);
			
        }
      }
#endif 


      if ( diag_lsm_init_event_queue(&diagmpd_buf, DIAG_MPD_EVENT_HEAP_SIZE) < 0 )
      {
         ERR_FATAL("Diag_LSM_Init : Failed to setup event structs",0,0,0); 
      }

      if (osal_create_diag_timer_ex(&diagmpd_buf->diagbuf_mpd_drain_timer, osal_thread_self(), diagbuf_mpd_set_drain_timer, 0) != OSAL_SUCCESS)
      {
	 ERR_FATAL("Failed to create drain timer for Userpd ", 0,0 ,0);        
      }
    }
    if (!DiagSvc_Malloc_Init())
    {
      ERR_FATAL("Diag_LSM_Init : SVC malloc failed ",0,0,0); 
    }

    if(!RegisterForMaskChange(MASK_CHANGE_REGISTER))
    {    
      ERR_FATAL("Diag_LSM_Init : registerformaskchange failed ",0,0,0); 
    }
    
    /*request for process_name from TMS and then make QDI call to let rootPD request diagID from APPS*/
    process_name=servreg_get_local_domain();
    if(!diag_lsm_request_diagID(process_name,qsr4_guid_ptr))
    {
      ERR_FATAL("Diag_LSM_Init : Request for diagID from rootpd failed",0,0,0); 
    }

    if (!CreateWaitThread())
    {
      ERR_FATAL("Diag_LSM_Init : createwaitthread failed ",0,0,0); 
    }

    if (!Diag_LSM_Pkt_Init())
    {
      ERR_FATAL("Diag_LSM_Init : lsm pkt init failed",0,0,0); 
    }
    
    if (!Diag_LSM_Log_Init())
    {
      ERR_FATAL("Diag_LSM_Init : lsm log init failed ",0,0,0);
    }

    if (!Diag_LSM_Event_Init())
    {
      ERR_FATAL("Diag_LSM_Init : lsm event init failed ",0,0,0); 
    }

    if (!Diag_LSM_Msg_Init())
    {
      ERR_FATAL("Diag_LSM_Init : lsm msg init failed ",0,0,0); 
    }

    diag_services_initialized = TRUE;

#ifdef FEATURE_DIAG_STM
    diag_stm_init();
#endif

    if(FALSE == diag_time_initialized_lsm)
    {
      diag_time_init_LSM();
    }
  } /*end if(!InterlockedCompareExchange(&gnDiag_LSM_Ref_Count,1,0) */
  gnDiag_LSM_Ref_Count += 1;
  qurt_pimutex_unlock(&diag_lsm_ref_count_mutex);
  return (TRUE);
}                               /* Diag_LSM_Init */

/*===========================================================================

FUNCTION    Diag_LSM_DeInit

DESCRIPTION
  De-Initialize the Diag service.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE = failure, else TRUE.

SIDE EFFECTS
  None

===========================================================================*/

boolean Diag_LSM_DeInit(void)
{
  boolean bReturn = TRUE;
  qurt_pimutex_lock(&diag_lsm_ref_count_mutex);
  if (1 < gnDiag_LSM_Ref_Count)
  {
    //Someone's still using diag, so just count down and go on with business...
    gnDiag_LSM_Ref_Count -= 1;
    qurt_pimutex_unlock(&diag_lsm_ref_count_mutex);
    return (bReturn);
  }
  else if (0 == gnDiag_LSM_Ref_Count)
  {
    qurt_pimutex_unlock(&diag_lsm_ref_count_mutex);
    return (TRUE);
  }

  DiagSvc_Malloc_Exit();

  if (!Diag_LSM_Event_DeInit() || !Diag_LSM_Pkt_DeInit() || !Diag_LSM_Log_DeInit() || !Diag_LSM_Msg_DeInit())
    bReturn = FALSE;

  if (!RegisterForMaskChange(MASK_CHANGE_DEREGISTER))
    bReturn = FALSE;

  if (!TerminateWaitThread())
  {
    bReturn = FALSE;
  }

  if (!osal_delete_timer(&diagmpd_buf->diagbuf_mpd_drain_timer))
  {
    bReturn = FALSE;
  }

  if (0 != qurt_qdi_close(diag_qdi_handle))
  {
    bReturn = FALSE;
  }
  diag_qdi_handle = -1;
  //We uninitialized things because it's the last one, so mark this as having 0 references.
  gnDiag_LSM_Ref_Count = 0;
  qurt_pimutex_unlock(&diag_lsm_ref_count_mutex);
  return (bReturn);
}     /* Diag_LSM_DeInit */



void Diag_LSM_RxThread(void *parameter)
{
  int return_val = 0;
  byte Req_Buf[DIAG_MAX_RX_PKT_SIZ];
  dword num_bytes_read = 0;
  int status = 0;
  diag_pd_type user_pd = DIAG_PD_NONE;
  
  diag_set_cur_userpd();
  user_pd = diag_lsm_cur_userpd;

  /*
  * diag_set_cur_userpd() might be blocking to get the userPD from TMS API
  * Only after the userPD is set, corresponding tables needs to be dispatched
  * Hence below userPD command registrations have been moved from RCInitTask to Diag_LSM_RxThread
  */

  if ((user_pd >= DIAG_PD_MIN) && (user_pd <= DIAG_PD_MAX))
  {
    switch (user_pd)
    {
      case DIAG_PD_AUDIO:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, test_tbl_audio);
        DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F, DIAG_SUBSYS_DIAG_SERV, test_tbl_v2_audio);
        break;
      case DIAG_PD_WLAN:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, test_tbl_wlan);
        DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F, DIAG_SUBSYS_DIAG_SERV, test_tbl_v2_wlan);
        break;
      case DIAG_PD_SENSOR:
        DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_DIAG_SERV, test_tbl_sensor);
        DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F, DIAG_SUBSYS_DIAG_SERV, test_tbl_v2_sensor);
        break;
      default:
        break;
    }
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Diag_LSM_RxThread: Didn't get userPD %d",user_pd);
  }
  do
  {
    return_val = diag_lsm_wait_for_event();
    if (return_val & SYNC_EVENT_DIAG_LSM_PKT_IDX)
    {
      num_bytes_read = diag_lsm_read_data(Req_Buf, DIAG_MAX_RX_PKT_SIZ);

      if (num_bytes_read <= DIAG_MAX_RX_PKT_SIZ)
      {
        diagpkt_LSM_process_request((void *)Req_Buf, (uint16)num_bytes_read, NULL, NULL);
      }
    }


	/* Do not ask for mask if diagID is not yet set */  
    if(diagmpd_buf->diagID != DIAGID_UNKNOWN_ID)
    {
      if (return_val & DIAG_EVENTSVC_MASK_CHANGE)
      {
        event_update_mask();
      }
      if (return_val & DIAG_LOGSVC_MASK_CHANGE)
      {
        log_update_mask();
      }
      if (return_val & DIAG_MSGSVC_MASK_CHANGE)
      {
        msg_update_mask();
      }
    }

    if(return_val & DIAG_DIAGID_CHANGE)
    {
      boolean status=FALSE;
      status = diag_get_diagID_LSM();
      /* Update the masks now if diagID was updated. */
      if(status == TRUE)
      {
        event_update_mask();
        log_update_mask();
        msg_update_mask();
      }
    }
	
    if(return_val & DIAG_TIME_API_CHANGE)
    {
      diag_set_time_api_LSM();
    }
#ifdef FEATURE_DIAG_STM
    if (return_val & DIAG_STM_MODE_CHANGE)
    {
      diag_stm_update_mode_LSM();
    }
#endif
    if (return_val & DIAG_KILL_RX_THREAD)
    {
      qurt_thread_exit(status);
    }
    if (return_val & DIAG_KILL_STRESS_TEST_THREAD)
    {
      diag_LSM_stresstest_cleanup_cb();
    }
  }while (1);

}
/*===========================================================================
DESCRIPTION
   Creates synchronization events for incoming packets and mask change.
   Creates the thread that waits on these synchronization events.
===========================================================================*/

boolean CreateWaitThread(void)
{
  boolean ret = TRUE;
  int ret_value = 0;
  qurt_thread_attr_t tattr;

  unsigned int stackbase;

  stackbase = (unsigned int)diag_lsm_stack;
  qurt_thread_attr_init(&tattr);
  qurt_thread_attr_set_stack_size(&tattr, ((DIAG_LSM_STACK_SIZE * 8) - 8));
  qurt_thread_attr_set_stack_addr(&tattr, (void *)((stackbase + 7) & (~7)));
  qurt_thread_attr_set_priority(&tattr, DIAG_PRI - 1);
  qurt_thread_attr_set_name(&tattr, "DIAG_LSM");
  ret_value = qurt_thread_create(&thread_id, &tattr, Diag_LSM_RxThread, NULL);

  if (ret_value == QURT_EFAILED)
  {
    ret = FALSE;
  }

  return (ret);
} /* CreateWaitThread() */

/*===========================================================================
DESCRIPTION
   Closes synchronization events for incoming packets and mask change.
   Signals the thread (Diag_LSM_RxThread) that waits on these synchronization
   events, to return.
===========================================================================*/
boolean TerminateWaitThread(void)
{
  int status;
  boolean bReturn = TRUE;
  qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_KILL_RX_THREAD);

  qurt_thread_join(thread_id, &status);
  return (bReturn);
} /* TerminateWaitThread() */

/*===========================================================================
DESCRIPTION
   Used to register/deregister this client process with Diag driver,
   for mask-change synchronization event.
   (Diag Driver adds the event handle corresponding to the unique synch. event
   name to its table, and sets the event when there is a mask change. For
   De-registration, the entry is removed from the list.)
===========================================================================*/
boolean RegisterForMaskChange(uint32 ActionCode)
{
  boolean status = TRUE;
  if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
  {
    int IOCtrlCode = DIAG_IOCTL_ERROR;
    bindpkt_params_per_process *pbind_req_send =
       malloc(sizeof(bindpkt_params_per_process));
    if (pbind_req_send)
    {
      pbind_req_send->count = 1;
      pbind_req_send->cmd_code = 0;
      pbind_req_send->subsys_id = 0;
      pbind_req_send->cmd_code_lo = 0;
      pbind_req_send->cmd_code_hi = 0;
      pbind_req_send->proc_id = 0;
      pbind_req_send->event_id = 0;
      pbind_req_send->log_code = 0;
      pbind_req_send->client_id = diag_pid;
      switch (ActionCode)
      {
         case MASK_CHANGE_REGISTER:
           IOCtrlCode = DIAG_IOCTL_MASK_REG;
           break;
         case MASK_CHANGE_DEREGISTER:
           IOCtrlCode = DIAG_IOCTL_MASK_DEREG;
           break;
         default:
           break;
      }

      if (!diag_lsm_ioctl(IOCtrlCode, (byte *)pbind_req_send, sizeof(bindpkt_params_per_process), NULL, 0, NULL))
      {
        status = FALSE;
      }

      free(pbind_req_send);
    }
    else
    {
      status = FALSE;
    }
  }
  else
  {
    status = FALSE;
  }

  return (status);
}


boolean diag_time_init_LSM()
{
  if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diag_time_init_begin_lsm, FALSE, TRUE))
    return FALSE;

  diag_time_get_LSM = &time_get;
  diag_time_initialized_lsm = TRUE;
  
  return TRUE;
}


boolean diag_set_time_api_LSM()
{
  boolean status = TRUE;
  uint8 time_api;
  if(diag_qdi_handle>=DIAG_FD_MIN_VALUE)
  {
    dword Len_Recvd = 0;
    if(!(diag_lsm_ioctl(DIAG_IOCTL_GET_TIME_API, NULL,0, &time_api,sizeof(time_api), &Len_Recvd)) || Len_Recvd != sizeof(time_api) ) 
    {
      switch(time_api)
      {
        case DIAG_TIME_GET:
        {
          diag_time_get_LSM = time_get;
          status = TRUE;
        }
        break;
        case DIAG_TIME_GET_UPTIME:
        {
           diag_time_get_LSM = time_get_uptime;
           status = TRUE;
        }
        break;
        case DIAG_TIME_GET_TBD:
        {
          /* Remove this error fatal after TIME SYNC Phase 2 implementation*/
          ERR_FATAL("Invalid Time API to be set : %d", diag_time_api_to_use_lsm,0,0);
        }
        break;        
        default:
        {
          status = FALSE;
        }
        break;
      }  
    }
  }
  else
  {
    status = FALSE;
  }
  return status;
}
/*===========================================================================
FUNCTION   diag_set_cur_userpd

DESCRIPTION
    This function updates the diag_lsm_cur_userpd with current userPD inited
    On Successful call, diag_lsm_cur_userpd = current userPD [AUDIO/WLAN/SENSOR],
    else diag_lsm_cur_userpd = DIAG_PD_NONE

DEPENDENCIES
    TMS API call should give the correct domain information "mdm/modem/audio_pd"

RETURN VALUE
    NONE

SIDE EFFECTS
    This function is blocking on TMS API servreg_get_local_domain()
===========================================================================*/

void diag_set_cur_userpd()
{
  char *domain= NULL;

  /*
  * Function to get the current userPD that is being initialized
  * This might be a blocking call, hence not called during rc_init
  */

  /* Ex: domain = "mdm/modem/audio_pd" */
  domain = servreg_get_local_domain();
  if ( domain == NULL )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "servreg_get_local_domain() returned null");
    return;
  }

  diag_lsm_cur_userpd = diag_get_pd_name(domain);

  return;
}
/*===========================================================================

FUNCTION diag_get_diagID_LSM

DESCRIPTION
  This procedure retieves the diag ID of the current process. It queries the root and
  gets the diagID assigned to the current process.

PARAMETERS
  None

RETURN VALUE
  TRUE if diagID is assigned to the current process.
  FALSE if unable to retrive diagID.

============================================================================*/
boolean diag_get_diagID_LSM()
{
  boolean status = FALSE;
  uint8 diagID = DIAGID_UNKNOWN_ID;
  char* process_name = NULL;

  process_name= servreg_get_local_domain();

  if(diag_qdi_handle>=DIAG_FD_MIN_VALUE)
  {
    dword Len_Recvd =0;
    if( !(diag_lsm_ioctl(DIAG_IOCTL_GET_DIAG_ID, (byte *)process_name, strlen(process_name)+1, &diagID, sizeof(diagID), &Len_Recvd)) || (Len_Recvd != sizeof(diagID)) )
    {
      return status;
    }
    if(diagmpd_buf)
    {
      diagmpd_buf->diagID = diagID;
      status = TRUE;
    }
  }
  return status;
}


