/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <pthread.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <inttypes.h>
#include <telephony/ril.h>
#include <cutils/properties.h>
#include <qmi_idl_lib.h>
#include <qmi_client.h>
#include <modules/android/qcril_android_event_context.h>
#include <framework/qcril_log.h>
#include <framework/legacy.h>
#include <framework/qcril_event.h>
#include <translators/android/utils.h>
#include "qcril_reqlist.h"
#include "qcril_qmi_oem_events.h"
#include "qcril_qmi_oemhook_agent.h"
#include "qcril_qmi_oem_reqlist.h"
#include "modules/ims/qcril_qmi_ims_radio_agent.h"
#include "modules/ims/qcril_qmi_ims_misc.h"

extern struct RIL_Env *qcril_response_api[ QCRIL_MAX_INSTANCE_ID ]; /*!< Functions for ril to call */
// TODO REARCH: Do feature sets in a generic way
#define QMI_RIL_SYS_PROP_NAME_SGLTE_CSFB  "persist.vendor.radio.sglte_csfb"
#define QMI_RIL_SYS_PROP_SGLTE_CSFB_LENGTH       4
#define QMI_RIL_SYS_PROP_RAT_OPTION             "persist.vendor.radio.rat_on"
#define QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP "persist.vendor.radio.ims_retry_3gpp"
#define QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP2 "persist.vendor.radio.ims_retry_3gpp2"
#define QCRIL_FEATURE_KDDI_HOLD_ANSWER_ON "persist.vendor.radio.kddi_hold_answ_on"
#define QMI_RIL_SYS_PROP_NAME_OEM_IND_TO_BOTH    "persist.vendor.radio.oem_ind_to_both"
#define QMI_RIL_SYS_PROP_NAME_OEM_SOCKET         "persist.vendor.radio.oem_socket"
#define QCRIL_FEATURE_SAP_SILENT_PIN_VERIFY "persist.vendor.radio.sap_silent_pin"
#define QCRIL_PROCESS_BLOCK_ALLOW_DATA                  "persist.vendor.radio.block_allow_data"
#define QMI_RIL_SYS_PROP_NAME_SUPPRESS_REQ       "persist.vendor.radio.suppress_req"
#define QMI_RIL_SYS_PROP_RIL_DATA_REG_STATE_CONDITIONAL_REPORT    "persist.vendor.radio.data_con_rprt"

#include <cutils/properties.h>

#define QCRIL_REQUEST_SUPPRESS_MAX_LEN 4

struct timeval HEAP_MEM_LIST_PRINT_TIMEVAL_DELAY = {60,0};

/* QCRIL request supress list mutex. */
pthread_mutex_t      qcril_request_supress_list_mutex;
pthread_mutexattr_t  qcril_request_supress_list_mutex_attr;

static qmi_ril_atomic_lock_info_type qmi_ril_common_critical_section;

/* QCRIL android pending unsol resp */
typedef struct qmi_ril_android_pending_unsol_resp_type
{
  boolean valid;
  const char *logstr;
  uint8 instance_id;

  qmi_ril_fw_android_param_copy_approach_type param_copy_arrron;
  void* copied_params;
  size_t copied_param_len;

  int8 num_of_locker;
} qmi_ril_android_pending_unsol_resp_type;

typedef struct
{
  pthread_mutex_t pending_unsol_resp_mutex;
  qmi_ril_android_pending_unsol_resp_type* pending_unsol_resp_list;
} qmi_ril_android_pending_unsol_resp_overview_type;

/*! @brief Structure used to cache QCRIL instance state
*/

typedef enum
{
  QMI_RIL_FTR_MULTI_SIM_UNKNOWN = 0,
  QMI_RIL_FTR_MULTI_SIM_DISABLED,
  QMI_RIL_FTR_DSDS_ENABLED,
  QMI_RIL_FTR_TSTS_ENABLED,
  QMI_RIL_FTR_DSDA_ENABLED
} qcril_qmi_ftr_multi_sim_info_type;

typedef enum
{
  QMI_RIL_FEATURE_UNKNOWN = 0,
  QMI_RIL_FEATURE_DISABLED,
  QMI_RIL_FEATURE_ENABLED
} qcril_qmi_feature_type;

typedef enum
{
  QMI_RIL_KDDI_HOLD_ANSWER_UNKNOWN = 0,
  QMI_RIL_KDDI_HOLD_ANSWER_DISABLED,
  QMI_RIL_KDDI_HOLD_ANSWER_ENABLED
} qcril_qmi_kddi_hold_answer_info_type;


static qcril_qmi_ftr_multi_sim_info_type qmi_ril_multi_sim_ftr_info;
static qcril_qmi_ftr_baseband_info_type qmi_ril_baseband_ftr_info; // inits to QMI_RIL_FTR_BASEBAND_UNKNOWN
static qcril_qmi_ftr_baseband_info_type qmi_ril_sglte_ftr_info; // inits to QMI_RIL_FTR_BASEBAND_UNKNOWN
static qcril_qmi_ftr_sglte_csfb_info_type  qmi_ril_sglte_csfb_ftr_info;
static qcril_qmi_feature_type            qmi_ril_oem_ind_to_both;
static qcril_qmi_feature_type            qmi_ril_ftr_suppress_req;
static qcril_qmi_ftr_rat_enable_option   qmi_ril_rat_enable_option;
static qcril_qmi_kddi_hold_answer_info_type qmi_ril_kddi_hold_answer;
static qcril_qmi_kddi_hold_answer_info_type qmi_ril_kddi_hold_answer;

static qmi_ril_atomic_lock_info_type qmi_ril_common_critical_section;
static qmi_ril_gen_operational_status_type qmi_ril_gen_operational_status;

#define MEM_PADDING "QCRILQMIqcrilqmi"
#define MEM_PADDING_SIZE 16
/* QCRIL Heap Memory list*/
qcril_heap_list_info *qcril_heap_memory_list;
uint32 heap_memory_id;

static pthread_mutex_t qcril_heap_memory_list_mutex;
#define QCRIL_TRACK_HEAP_MEM             "persist.vendor.radio.track_heap_mem"

static boolean is_heap_memory_tracked;
typedef struct
{
    int is_valid;
    pthread_t thread_id;
    char thread_name[QMI_RIL_THREAD_NAME_MAX_SIZE];
} qmi_ril_thread_name_info_type;

static qcril_instance_id_e_type            qmi_ril_process_instance_id;
static uint32_t                            qmi_ril_sim_slot; ///< sim slot associated w/this ril instance
static qcril_modem_stack_id_e_type modem_stack_id;

/* QCRIL internal info */
extern qcril_arb_state_struct_type *qcril_state;


void qcril_get_baseband_name(char *prop_str);
qcril_heap_list_info **qcril_find_heap_memory_locked(void* addr);

//===========================================================================
// qmi_ril_reset_multi_sim_ftr_info
//===========================================================================
void qmi_ril_reset_multi_sim_ftr_info()
{
  qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_MULTI_SIM_UNKNOWN;
} // qmi_ril_reset_multi_sim_ftr_info

//===========================================================================
// qcril_remove_heap_memory_from_list
//===========================================================================
qcril_heap_list_info *qcril_remove_heap_memory_from_list(void* addr)
{
    (void)addr;
    return NULL;

  qcril_heap_list_info **i, *ret;
  QCRIL_MUTEX_LOCK( &qcril_heap_memory_list_mutex, "heap memory list mutex");

  i = qcril_find_heap_memory_locked(addr);

  ret = *i;
  if ( NULL != ret )
  {
    if ( NULL != ret->next )
    {
      ret->next->prev = ret->prev;
    }
    *i = ret->next;

    ret->next = NULL;
    ret->prev = NULL;
  }

  QCRIL_MUTEX_UNLOCK( &qcril_heap_memory_list_mutex, "heap memory list mutex");
  return ret;
} // qcril_remove_heap_memory_from_list

//===========================================================================
//qmi_ril_get_process_instance_id
//===========================================================================
qcril_instance_id_e_type qmi_ril_get_process_instance_id(void)
{
  return qmi_ril_process_instance_id; // this is always thread safe as may only be changed upon ril library load
} // qmi_ril_get_process_instance_id

void qmi_ril_set_process_instance_id
(
 qcril_instance_id_e_type instance_id
)
{
    qmi_ril_process_instance_id = instance_id;
}
//=============================================================================
// FUNCTION: qmi_ril_get_sim_slot
//
// DESCRIPTION:
// returns the sim card slot index associated with current RIL instance
// Note: thread safe as long as its only set durring ril init
//
// RETURN: 0 | 1 - sim card slot index
//=============================================================================
uint32_t qmi_ril_get_sim_slot(void)
{
  return qmi_ril_sim_slot;
}

void qmi_ril_set_sim_slot(uint32_t slot)
{
    qmi_ril_sim_slot = slot;
}
//===========================================================================
// qcril_add_heap_memory_to_list
//===========================================================================
void qcril_add_heap_memory_to_list(qcril_heap_list_info *info)
{
  qcril_heap_list_info **i;
  qcril_heap_list_info *prev = NULL;
  QCRIL_MUTEX_LOCK( &qcril_heap_memory_list_mutex, "heap memory list mutex");

  for (i = &qcril_heap_memory_list; *i; i = &((*i)->next))
  {
    prev = *i;
  }
  *i = info;
  info->next = NULL;
  info->prev = prev;
  QCRIL_MUTEX_UNLOCK( &qcril_heap_memory_list_mutex, "heap memory list mutex");
} // qcril_add_heap_memory_to_list

//===========================================================================
// qcril_print_heap_memory_list
//===========================================================================
void qcril_print_heap_memory_list(void *arg)
{
  qcril_heap_list_info *i;
  (void)arg;
  QCRIL_LOG_INFO(" ************ print heap memory list ************");
  QCRIL_MUTEX_LOCK( &qcril_heap_memory_list_mutex, "heap memory list mutex");
  for (i = qcril_heap_memory_list; i; i = i->next)
  {
    QCRIL_LOG_INFO("\tid: %u, func: %s, line: %u, size: %u", i->mem_id, i->func_name, i->line_number, i->size);
  }
  QCRIL_MUTEX_UNLOCK( &qcril_heap_memory_list_mutex, "heap memory list mutex");
  qcril_setup_timed_callback(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                             qcril_print_heap_memory_list, &HEAP_MEM_LIST_PRINT_TIMEVAL_DELAY, NULL );
} // qcril_print_heap_memory_list

//===========================================================================
// qcril_find_heap_memory_locked
//===========================================================================
qcril_heap_list_info **qcril_find_heap_memory_locked(void* addr)
{
  qcril_heap_list_info **i;

  for (i = &qcril_heap_memory_list; *i ; i = &((*i)->next)) {
    if ((*i)->addr == addr) {
      break;
    }
  }

  return i;
} // qcril_find_heap_memory_locked



/*===========================================================================

  FUNCTION:  qcril_malloc_adv

===========================================================================*/
/*!
    @brief
    Allocate memory from heap.

    @return
    Pointer to allocated memory region.
*/
/*=========================================================================*/
void *qcril_malloc_adv
(
  size_t size,
  const char* func_name,
  int line_num
)
{
  void *mem_ptr = NULL;

  /*-----------------------------------------------------------------------*/

  if ( 1 == is_heap_memory_tracked )
  {
    mem_ptr = (qcril_heap_list_info *)malloc(size + 2 * MEM_PADDING_SIZE);

    if ( NULL != mem_ptr )
    {
      qcril_heap_list_info *heap_memory_info = (qcril_heap_list_info *)malloc(sizeof(*heap_memory_info));
      if ( NULL != heap_memory_info )
      {
        memcpy(mem_ptr, MEM_PADDING, MEM_PADDING_SIZE);
        mem_ptr = (void*)((char*)mem_ptr + MEM_PADDING_SIZE);
        memset( mem_ptr, 0, size );
        memcpy((void*)((char*)mem_ptr+size), MEM_PADDING, MEM_PADDING_SIZE);

        heap_memory_info->addr = mem_ptr;
        strlcpy(heap_memory_info->func_name, func_name,
                (sizeof(heap_memory_info->func_name) / sizeof(heap_memory_info->func_name[0])));
        heap_memory_info->line_number = line_num;
        heap_memory_info->size = size;
        QCRIL_MUTEX_LOCK( &qcril_heap_memory_list_mutex, "heap memory list mutex");
        heap_memory_info->mem_id = heap_memory_id++;
        QCRIL_LOG_INFO( "malloc new memory: id: %d, func: %s", heap_memory_info->mem_id, heap_memory_info->func_name );
        QCRIL_MUTEX_UNLOCK( &qcril_heap_memory_list_mutex, "heap memory list mutex");

        QCRIL_LOG_INFO("Adding new addr %p to memory list", mem_ptr);
        qcril_add_heap_memory_to_list(heap_memory_info);
      }
      else
      {
        memset( mem_ptr, 0, size );
        QCRIL_LOG_ERROR( "Fail to allocate memory" );
      }
    }
    else
    {
      QCRIL_LOG_ERROR( "Fail to allocate memory" );
    }
  }
  else
  {
    mem_ptr = malloc( size );
    if ( NULL != mem_ptr )
    {
      memset( mem_ptr, 0, size );
    }
    else
    {
      QCRIL_LOG_ERROR( "Fail to allocate memory" );
    }
  }

  return mem_ptr;

} /* qcril_malloc_adv */


/*===========================================================================

  FUNCTION:  qcril_free_adv

===========================================================================*/
/*!
    @brief
    Free specified memory region.

    @return
    None.
*/
/*=========================================================================*/
void qcril_free_adv
(
  void *mem_ptr,
  const char* func_name,
  int line_num
)
{

  /*-----------------------------------------------------------------------*/

  if ( mem_ptr == NULL )
  {
      return;
  }

  /*-----------------------------------------------------------------------*/

  if ( 1 == is_heap_memory_tracked )
  {
    QCRIL_LOG_INFO("removing heap memory: %p from list", mem_ptr);
    qcril_heap_list_info *heap_mem_info = qcril_remove_heap_memory_from_list(mem_ptr);
    if (NULL == heap_mem_info)
    {
      QCRIL_LOG_ERROR("Memory %p not found in list. Called from %s line %d", mem_ptr, func_name, line_num);
    }
    else
    {
      // check buffer underflow
      if ( 0 != memcmp((void*)(((char*)mem_ptr) - MEM_PADDING_SIZE), (void*)MEM_PADDING, MEM_PADDING_SIZE ) )
      {
        QCRIL_LOG_FATAL("memory underflow!");
      }
      // check buffer overflow
      if ( 0 != memcmp((void*)((char*)(mem_ptr) + heap_mem_info->size), (void*)MEM_PADDING, MEM_PADDING_SIZE) )
      {
        QCRIL_LOG_FATAL("memory overflow!");
      }

      mem_ptr = (void*)(((char*)mem_ptr) - MEM_PADDING_SIZE);
      free(heap_mem_info);
    }
    if (NULL != mem_ptr)
    {
      free( mem_ptr );
    }
    else
    {
      QCRIL_LOG_DEBUG("Memory pointer is NULL after shifting the padding size");
    }
  }
  else
  {
    if (NULL != mem_ptr)
    {
      free( mem_ptr );
    }
  }

  return;
} /* qcril_free_adv */

//===========================================================================
// qmi_ril_init_request_return
//===========================================================================
void qmi_ril_init_request_return(qcril_instance_id_e_type instance_id, qcril_request_return_type *ret)
{
    qcril_arb_state_info_struct_type *s_ptr;
    s_ptr = &qcril_state->info[ instance_id ];
    QCRIL_MUTEX_LOCK( &qcril_state->mutex, "qcril_state_mutex" );

    /* Initialize the structure that the request handler will use to return
       information about the status of the request */
    ret->pri_gw_sim_state_changed = FALSE;
    ret->next_pri_gw_sim_state = s_ptr->pri_gw_sim_state;
    ret->pri_cdma_sim_state_changed = FALSE;
    ret->next_pri_cdma_sim_state = s_ptr->pri_cdma_sim_state;
    ret->sec_gw_sim_state_changed = FALSE;
    ret->next_sec_gw_sim_state = s_ptr->sec_gw_sim_state;
    ret->sec_cdma_sim_state_changed = FALSE;
    ret->next_sec_cdma_sim_state = s_ptr->sec_cdma_sim_state;
    ret->ter_gw_sim_state_changed = FALSE;
    ret->next_ter_gw_sim_state = s_ptr->ter_gw_sim_state;
    ret->ter_cdma_sim_state_changed = FALSE;
    ret->next_ter_cdma_sim_state = s_ptr->ter_cdma_sim_state;

    QCRIL_MUTEX_UNLOCK( &qcril_state->mutex, "qcril_state_mutex" );
} //qmi_ril_clear_thread_name

/*===========================================================================

  FUNCTION:  qcril_state_transition

===========================================================================*/
/*!
    @brief
    Called each time an event handler returns, to process the next state
    of qcril.

    @return
    None.
*/
/*=========================================================================*/
void qcril_state_transition
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type modem_id,
  qcril_evt_e_type event_id,
  const qcril_request_return_type *ret_ptr
)
{
  qcril_arb_state_info_struct_type *s_ptr, *sec_s_ptr;
  boolean pri_gw_sim_state_changed = FALSE, pri_cdma_sim_state_changed = FALSE;
  boolean sec_gw_sim_state_changed = FALSE, sec_cdma_sim_state_changed = FALSE;
  qcril_sim_state_e_type current_pri_gw_sim_state, next_pri_gw_sim_state, current_pri_cdma_sim_state, next_pri_cdma_sim_state;
  qcril_sim_state_e_type current_sec_gw_sim_state, next_sec_gw_sim_state, current_sec_cdma_sim_state, next_sec_cdma_sim_state;
  boolean ter_gw_sim_state_changed = FALSE;
  boolean ter_cdma_sim_state_changed = FALSE;
  qcril_sim_state_e_type current_ter_gw_sim_state;
  qcril_sim_state_e_type next_ter_gw_sim_state;
  qcril_sim_state_e_type current_ter_cdma_sim_state;
  qcril_sim_state_e_type next_ter_cdma_sim_state;

  QCRIL_NOTUSED(modem_id);


  char *sim_state_name[] = { "Absent", "Not ready", "Ready", "PIN", "PUK", "Network personalization", "Error", "Illegal" };

  if(instance_id < QCRIL_MAX_INSTANCE_ID && ret_ptr != NULL)
  {
    /*-----------------------------------------------------------------------*/

    s_ptr = &qcril_state->info[ instance_id ];
    sec_s_ptr = &qcril_state->info[ QCRIL_DSDS_INSTANCE_PAIR( instance_id ) ];

    /*-----------------------------------------------------------------------*/

    QCRIL_MUTEX_LOCK( &qcril_state->mutex, "qcril_state_mutex" );

    current_pri_gw_sim_state = next_pri_gw_sim_state = s_ptr->pri_gw_sim_state;
    current_pri_cdma_sim_state = next_pri_cdma_sim_state = s_ptr->pri_cdma_sim_state;
    current_sec_gw_sim_state = next_sec_gw_sim_state = s_ptr->sec_gw_sim_state;
    current_sec_cdma_sim_state = next_sec_cdma_sim_state = s_ptr->sec_cdma_sim_state;
    current_ter_gw_sim_state = next_ter_gw_sim_state = s_ptr->ter_gw_sim_state;
    current_ter_cdma_sim_state = next_ter_cdma_sim_state = s_ptr->ter_cdma_sim_state;

    /* Check whether the primary GSM/WCDMA SIM state is changed */
    if ( ret_ptr->pri_gw_sim_state_changed && ( ret_ptr->next_pri_gw_sim_state !=  s_ptr->pri_gw_sim_state ) )
    {
      pri_gw_sim_state_changed = TRUE;
      next_pri_gw_sim_state = ret_ptr->next_pri_gw_sim_state;
    }

    /* Check whether the primary CDMA SIM state is changed */
    if ( ret_ptr->pri_cdma_sim_state_changed && ( ret_ptr->next_pri_cdma_sim_state !=  s_ptr->pri_cdma_sim_state ) )
    {
      pri_cdma_sim_state_changed = TRUE;
      next_pri_cdma_sim_state = ret_ptr->next_pri_cdma_sim_state;
    }

    if (qmi_ril_is_feature_supported( QMI_RIL_FEATURE_DSDS ))
    {
      /* Check whether the secondary GSM/WCDMA SIM state is changed */
      if ( ret_ptr->sec_gw_sim_state_changed && ( ret_ptr->next_sec_gw_sim_state !=  s_ptr->sec_gw_sim_state ) )
      {
        sec_gw_sim_state_changed = TRUE;
        next_sec_gw_sim_state = ret_ptr->next_sec_gw_sim_state;
      }

      /* Check whether the secondary CDMA SIM state is changed */
      if ( ret_ptr->sec_cdma_sim_state_changed && ( ret_ptr->next_sec_cdma_sim_state !=  s_ptr->sec_cdma_sim_state ) )
      {
        sec_cdma_sim_state_changed = TRUE;
        next_sec_cdma_sim_state = ret_ptr->next_sec_cdma_sim_state;
      }

      /* Check whether the tertiary GSM/WCDMA SIM state is changed */
      if ( ret_ptr->ter_gw_sim_state_changed && ( ret_ptr->next_ter_gw_sim_state !=  s_ptr->ter_gw_sim_state ) )
      {
        ter_gw_sim_state_changed = TRUE;
        next_ter_gw_sim_state = ret_ptr->next_ter_gw_sim_state;
      }

      /* Check whether the tertiary CDMA SIM state is changed */
      if ( ret_ptr->ter_cdma_sim_state_changed && ( ret_ptr->next_ter_cdma_sim_state !=  s_ptr->ter_cdma_sim_state ) )
      {
        ter_cdma_sim_state_changed = TRUE;
        next_ter_cdma_sim_state = ret_ptr->next_ter_cdma_sim_state;
      }
    }

    QCRIL_LOG_DEBUG( "[%s(%p)] GW SIM(pri): %s --> %s, CDMA SIM(pri): %s --> %s,"
                     "GW SIM(sec): %s --> %s, CDMA SIM(sec): %s --> %s,"
                     "GW SIM(ter): %s --> %s, CDMA SIM(ter): %s --> %s",
                     qcril_log_lookup_event_name( event_id ), event_id,
                     sim_state_name[ s_ptr->pri_gw_sim_state ], sim_state_name[ next_pri_gw_sim_state ],
                     sim_state_name[ s_ptr->pri_cdma_sim_state ], sim_state_name[ next_pri_cdma_sim_state ],
                     sim_state_name[ s_ptr->sec_gw_sim_state ], sim_state_name[ next_sec_gw_sim_state ],
                     sim_state_name[ s_ptr->sec_cdma_sim_state ], sim_state_name[ next_sec_cdma_sim_state ],
                     sim_state_name[ s_ptr->ter_gw_sim_state ], sim_state_name[ next_ter_gw_sim_state ],
                     sim_state_name[ s_ptr->ter_cdma_sim_state ], sim_state_name[ next_ter_cdma_sim_state ] );


    /* Change in Modem State, SIM state or Subscription Config mask. Figure out the states update */
    if ( pri_gw_sim_state_changed || pri_cdma_sim_state_changed
         || sec_gw_sim_state_changed || sec_cdma_sim_state_changed
         || ter_gw_sim_state_changed || ter_cdma_sim_state_changed
       )
    {
      /* Save the Modem State, SIM state */
      s_ptr->pri_gw_sim_state = next_pri_gw_sim_state;
      s_ptr->pri_cdma_sim_state = next_pri_cdma_sim_state;
      s_ptr->sec_gw_sim_state = next_sec_gw_sim_state;
      s_ptr->sec_cdma_sim_state = next_sec_cdma_sim_state;
      s_ptr->ter_gw_sim_state = next_ter_gw_sim_state;
      s_ptr->ter_cdma_sim_state = next_ter_cdma_sim_state;
    }

    QCRIL_MUTEX_UNLOCK( &qcril_state->mutex, "qcril_state_mutex" );
  }
  else
  {
    QCRIL_LOG_FATAL("CHECK FAILED");
  }

} /* qcril_state_transition() */


//===========================================================================
// qmi_ril_fw_dedicated_request_exec_thread
//===========================================================================
void * qmi_ril_fw_dedicated_request_exec_thread(void * param)
{
    pthread_t                                   thread_id;
    qmi_ril_gen_operational_status_type         cur_state;
    char                                        thread_name_buf[ QMI_RIL_THREAD_NAME_MAX_SIZE ];
    qmi_ril_dedicated_request_exec_params_type* exec_params;
    qcril_event_id_t*            dispatch_tbl_entry_ptr;
    RIL_Errno                                   audit_result;
    qcril_request_params_type                   pass_on_params;
    qcril_request_resp_params_type              resp;

    void*                                       param_ptr_to_be_passed;
    int                                         param_len_to_be_passed;

    QCRIL_LOG_FUNC_ENTRY();

    audit_result = RIL_E_GENERIC_FAILURE;

    exec_params = (qmi_ril_dedicated_request_exec_params_type*)param;
    if ( NULL != exec_params )
    {
      thread_id = pthread_self();
      QCRIL_SNPRINTF( thread_name_buf, sizeof(thread_name_buf), "cmd-%d(%d)", (int)exec_params->event_id, ((uint32_t)thread_id) % 1000 );
      qmi_ril_set_thread_name( thread_id, thread_name_buf );

      cur_state = qmi_ril_get_operational_status();
      QCRIL_LOG_INFO( " ..operational state %d", (int) cur_state );

      do
      {
          // lookup executive entry
          if ( qcril_hash_table_lookup( exec_params->event_id , &dispatch_tbl_entry_ptr ) != E_SUCCESS )
          { // this shoud not happen as integrity check should have been done in onRequest
            audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
            break;
          }

          // param setup
          switch ( exec_params->param_copy_arrron )
          {
            case QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT:
              param_ptr_to_be_passed  = (void*)&exec_params->copied_params.four_bytes;
              param_len_to_be_passed  = exec_params->original_data_len;
              break;

            case QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY:
              param_ptr_to_be_passed  = exec_params->copied_params.dynamic_copy;


              param_len_to_be_passed  = exec_params->original_data_len;
              break;

            case QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION:  // passthrough
            default:
              param_ptr_to_be_passed  = NULL;
              param_len_to_be_passed  = QMI_RIL_ZERO;
              break;

          }

          memset( &pass_on_params, 0, sizeof( pass_on_params ));
          pass_on_params.instance_id  = exec_params->instance_id;
          pass_on_params.modem_id     = exec_params->modem_id;
          pass_on_params.event_id     = exec_params->event_id;
          pass_on_params.t            = exec_params->token;
          pass_on_params.data         = param_ptr_to_be_passed;
          pass_on_params.datalen      = param_len_to_be_passed;

          // do exec
          QCRIL_LOG_INFO("Calling dispatch event..");
          if ( qcril_dispatch_event( dispatch_tbl_entry_ptr, &pass_on_params ) == E_NOT_ALLOWED )
          {
            audit_result = RIL_E_RADIO_NOT_AVAILABLE;
            break;
          }
          audit_result = RIL_E_SUCCESS;
      } while (FALSE);

      if ( RIL_E_SUCCESS != audit_result )
      {
        qcril_default_request_resp_params( exec_params->instance_id, exec_params->token, exec_params->event_id, audit_result, &resp );

        /* For internal events*/
        if( !qcril_is_event_in_group(exec_params->event_id, &QCRIL_GRP_ANDROID_RIL_REQ ) )
        {
          QCRIL_LOG_INFO("Internal event in the dedicated thread returned failure. Processing..");
          if (qcril_is_event_in_group(exec_params->event_id, &QCRIL_GRP_IMS))
          {
            imsRadioSendMessage(exec_params->token, ims_MsgType_RESPONSE, qcril_qmi_ims_map_event_to_request(exec_params->event_id), qcril_qmi_ims_map_ril_error_to_ims_error(audit_result), NULL, 0);
          }
          else if ( qcril_is_event_in_group(exec_params->event_id, &QCRIL_GRP_OEM_HOOK ) )
          {
            resp.rild_sock_oem_req = TRUE;
            qcril_send_request_response( &resp );
          }
        }
        else
        {
          QCRIL_LOG_INFO("RIL event in the dedicated thread returned failure. Sending response..");
          qcril_send_request_response( &resp );
        }
      } // otherwise it is taken that respective request handler has responded synchronously or will respond asynchronously

      // cleanup
      qmi_ril_fw_destroy_android_live_params_copy( exec_params->param_copy_arrron,
                                                   exec_params->event_id,
                                                   (void*)&exec_params->copied_params.four_bytes,
                                                   exec_params->copied_params.dynamic_copy );
      qcril_free( exec_params );

      qmi_ril_clear_thread_name( thread_id );
    }

    QCRIL_LOG_INFO( " ..exec res %d", (int) audit_result );

    QCRIL_LOG_FUNC_RETURN();
    return NULL;
} // qmi_ril_fw_dedicated_request_exec_thread


//===========================================================================
// qmi_ril_fw_create_android_live_params_copy
//===========================================================================
qmi_ril_fw_android_param_copy_approach_type qmi_ril_fw_create_android_live_params_copy(
                                               int android_request_id,
                                               void * android_request_data,
                                               int android_request_data_len,
                                               void* four_byte_storage,
                                               int* four_byte_storage_occupied,
                                               void** sub_created_custom_storage,
                                               int* custom_storage_len,
                                               IxErrnoType* error)
{
  qmi_ril_fw_android_param_copy_approach_type res;
  qmi_ril_fw_android_param_copy_approach_type res_inclanation;
  int len_to_go;
  char* str_access;
  int need_common_clone;
  IxErrnoType status = E_FAILURE;

  void * locally_created_custom_storage;
  int locally_created_custom_storage_len;
  int local_ok;

  RIL_IMS_SMS_Message*  android_send_ims_msg_params;
  RIL_IMS_SMS_Message*  copied_android_send_ims_msg_params;
  RIL_CDMA_SMS_Message* copied_android_cdma_send_ims_param;

  char *copied_android_gw_smsc_address;
  char *copied_android_gw_pdu;
  char ** copied_android_gw_sms_ims_params;

  char** android_send_sms_params;

  char** android_cdma_dtmf_holder;
  char** copied_cdma_dtmf_holder;
  char* copied_cdma_dtmf_str;
  char* copied_cdma_dtmf_on;
  char* copied_cdma_dtmf_off;

  char **android_manual_selection_params;
  char *copied_android_manual_selection_mcc_mnc;
  char *copied_android_manual_selection_rat;
  char ** copied_android_manual_selection_params;
#if (QCRIL_RIL_VERSION < 15)
  RIL_InitialAttachApn* android_initial_attach_apn_params;
  RIL_InitialAttachApn* copied_android_initial_attach_apn_params=NULL;
#else
  RIL_InitialAttachApn_v15* android_initial_attach_apn_params;
  RIL_InitialAttachApn_v15* copied_android_initial_attach_apn_params=NULL;
  char* copied_android_initial_attach_apn_roaming_protocol = NULL;
  char* copied_android_initial_attach_apn_mvno_type = NULL;
  char* copied_android_initial_attach_apn_mvno_match_data = NULL;
#endif
  char* copied_android_initial_attach_apn_apn=NULL;
  char* copied_android_initial_attach_apn_protocol=NULL;
  char* copied_android_initial_attach_apn_username=NULL;
  char* copied_android_initial_attach_apn_password=NULL;

  RIL_SMS_WriteArgs*  android_write_sms_to_sim_msg_params;
  RIL_SMS_WriteArgs*  copied_android_write_sms_to_sim_msg_params;
  char *copied_android_write_sms_to_sim_msg_smsc_address;
  char *copied_android_write_sms_to_sim_msg_pdu;

  RIL_CallForwardInfo *android_query_call_fwd_info_params;
  RIL_CallForwardInfo *copied_android_query_call_fwd_info_params;
  char *copied_android_query_call_fwd_info_number;

  int local_four_byte_storage_occupied;

  char* copied_str;

  char** android_change_barring_pwd_params;
  char *android_ch_bar_pwd_faclity;
  char *android_ch_bar_pwd_old_pwd;
  char *android_ch_bar_pwd_new_pwd;

  char** copied_change_barring_pwd_params;
  char *copied_ch_bar_pwd_faclity;
  char *copied_ch_bar_pwd_old_pwd;
  char *copied_ch_bar_pwd_new_pwd;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_DEBUG("a-r-id %d, a-data 0x%x, a-data-len %d", android_request_id, android_request_data, (int)android_request_data_len );


  res               = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;

  len_to_go         = QMI_RIL_ZERO;
  need_common_clone = FALSE;

  local_four_byte_storage_occupied = QMI_RIL_ZERO;

  locally_created_custom_storage_len = QMI_RIL_ZERO;
  locally_created_custom_storage     = NULL;

  switch ( android_request_id )
  {


   case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
#if (QCRIL_RIL_VERSION < 15)
        android_initial_attach_apn_params = (RIL_InitialAttachApn*)android_request_data;
#else
        android_initial_attach_apn_params = (RIL_InitialAttachApn_v15*)android_request_data;
#endif

        local_ok = FALSE;
        do
        {
          copied_android_initial_attach_apn_params = qcril_malloc(
                                                    sizeof(*copied_android_initial_attach_apn_params ) );

          if ( NULL == copied_android_initial_attach_apn_params )
          {
            status = E_NO_MEMORY;
            break;
          }

          *copied_android_initial_attach_apn_params = *android_initial_attach_apn_params;

          //apn
          str_access = android_initial_attach_apn_params->apn;
          copied_android_initial_attach_apn_apn = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_initial_attach_apn_apn && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }

          //protocol
          str_access = android_initial_attach_apn_params->protocol;
          copied_android_initial_attach_apn_protocol = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_initial_attach_apn_protocol && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }

          //username
          str_access = android_initial_attach_apn_params->username;
          copied_android_initial_attach_apn_username = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_initial_attach_apn_username && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }

          //password
          str_access = android_initial_attach_apn_params->password;
          copied_android_initial_attach_apn_password = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_initial_attach_apn_password && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }
#if (QCRIL_RIL_VERSION >= 15)
          // new field: roaming protocal
          str_access = android_initial_attach_apn_params->roamingProtocol;
          copied_android_initial_attach_apn_roaming_protocol = qmi_ril_util_str_clone(str_access);
          if ( NULL == copied_android_initial_attach_apn_roaming_protocol && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }
          // new field: mvno type
          str_access = android_initial_attach_apn_params->mvnoType;
          copied_android_initial_attach_apn_mvno_type = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_initial_attach_apn_mvno_type && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }
          // new field: mvno match data
          str_access = android_initial_attach_apn_params->mvnoMatchData;
          copied_android_initial_attach_apn_mvno_match_data = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_initial_attach_apn_mvno_match_data && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }
#endif

          copied_android_initial_attach_apn_params->apn = copied_android_initial_attach_apn_apn;
          copied_android_initial_attach_apn_params->protocol = copied_android_initial_attach_apn_protocol;
          copied_android_initial_attach_apn_params->username = copied_android_initial_attach_apn_username;
          copied_android_initial_attach_apn_params->password = copied_android_initial_attach_apn_password;
#if (QCRIL_RIL_VERSION >= 15)
          copied_android_initial_attach_apn_params->roamingProtocol =
                  copied_android_initial_attach_apn_roaming_protocol;
          copied_android_initial_attach_apn_params->mvnoType =
                  copied_android_initial_attach_apn_mvno_type;
          copied_android_initial_attach_apn_params->mvnoMatchData =
                  copied_android_initial_attach_apn_mvno_match_data;
#endif
          local_ok = TRUE;
          status = E_SUCCESS;
        } while (FALSE);

        if ( local_ok )
        {
          locally_created_custom_storage      = copied_android_initial_attach_apn_params;
          locally_created_custom_storage_len  = sizeof ( *copied_android_initial_attach_apn_params );

          need_common_clone = FALSE;

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
        }
        else
        {  // rollback
          if( NULL != copied_android_initial_attach_apn_params )
          {
              qcril_free( copied_android_initial_attach_apn_params );
              copied_android_initial_attach_apn_params = NULL;
          }

          if( NULL != copied_android_initial_attach_apn_apn )
          {
              qcril_free( copied_android_initial_attach_apn_apn );
              copied_android_initial_attach_apn_apn = NULL;
          }

          if( NULL != copied_android_initial_attach_apn_protocol )
          {
              qcril_free( copied_android_initial_attach_apn_protocol );
              copied_android_initial_attach_apn_protocol = NULL;
          }

          if( NULL != copied_android_initial_attach_apn_password )
          {
              qcril_free( copied_android_initial_attach_apn_password );
              copied_android_initial_attach_apn_password = NULL;
          }

          if( NULL != copied_android_initial_attach_apn_username )
          {
              qcril_free( copied_android_initial_attach_apn_username );
              copied_android_initial_attach_apn_username = NULL;
          }

#if (QCRIL_RIL_VERSION >= 15)
          if ( NULL != copied_android_initial_attach_apn_roaming_protocol )
          {
            qcril_free( copied_android_initial_attach_apn_roaming_protocol );
            copied_android_initial_attach_apn_roaming_protocol = NULL;
          }

          if ( NULL != copied_android_initial_attach_apn_mvno_type )
          {
            qcril_free( copied_android_initial_attach_apn_mvno_type );
            copied_android_initial_attach_apn_mvno_type = NULL;
          }

          if ( NULL != copied_android_initial_attach_apn_mvno_match_data )
          {
            qcril_free( copied_android_initial_attach_apn_mvno_match_data );
            copied_android_initial_attach_apn_mvno_match_data = NULL;
          }
#endif
          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;


    case RIL_REQUEST_IMS_SEND_SMS:
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        android_send_ims_msg_params = (RIL_IMS_SMS_Message*)android_request_data;

        copied_android_send_ims_msg_params  = NULL;
        copied_android_cdma_send_ims_param  = NULL;
        copied_android_gw_smsc_address      = NULL;
        copied_android_gw_pdu               = NULL;
        copied_android_gw_sms_ims_params    = NULL;

        local_ok = FALSE;
        do
        {
          copied_android_send_ims_msg_params = qcril_malloc( sizeof( *copied_android_send_ims_msg_params ) );

          if ( NULL == copied_android_send_ims_msg_params )
          {
            status = E_NO_MEMORY;
            break;
          }
          *copied_android_send_ims_msg_params = *android_send_ims_msg_params;

          if ( RADIO_TECH_3GPP2 == copied_android_send_ims_msg_params->tech )
          { // cdma
            if ( NULL != android_send_ims_msg_params->message.cdmaMessage )
            {
              copied_android_cdma_send_ims_param = qcril_malloc( sizeof( *copied_android_cdma_send_ims_param ) );
              if ( NULL == copied_android_cdma_send_ims_param )
              {
                status = E_NO_MEMORY;
                break;
              }

              *copied_android_cdma_send_ims_param = *android_send_ims_msg_params->message.cdmaMessage;
              copied_android_send_ims_msg_params->message.cdmaMessage = copied_android_cdma_send_ims_param;
            }
            // else nothing - accept params as is
          }
          else
          { // gwl

            // sms sc
            str_access = ( (char **) android_send_ims_msg_params->message.gsmMessage )[ 0 ];
            copied_android_gw_smsc_address = qmi_ril_util_str_clone( str_access );
            if ( NULL == copied_android_gw_smsc_address && NULL != str_access )
            {
              status = E_INVALID_ARG;
              break;
            }

            // pdu
            str_access = ( (char **) android_send_ims_msg_params->message.gsmMessage )[ 1 ];
            copied_android_gw_pdu = qmi_ril_util_str_clone( str_access );
            if ( NULL == copied_android_gw_pdu && NULL != str_access )
            {
              status = E_INVALID_ARG;
              break;
            }

            copied_android_gw_sms_ims_params = qcril_malloc( 2 * sizeof( char* ) ) ;
            if ( NULL == copied_android_gw_sms_ims_params )
            {
              status = E_NO_MEMORY;
              break;
            }

            copied_android_gw_sms_ims_params[0] = copied_android_gw_smsc_address;
            copied_android_gw_sms_ims_params[1] = copied_android_gw_pdu;

            copied_android_send_ims_msg_params->message.gsmMessage = copied_android_gw_sms_ims_params;
          }

          local_ok = TRUE;
          status = E_SUCCESS;
        } while (FALSE);

        if ( local_ok )
        {
          locally_created_custom_storage      = copied_android_send_ims_msg_params;
          locally_created_custom_storage_len  = sizeof ( *copied_android_send_ims_msg_params );

          need_common_clone = FALSE; // we already did it

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
        }
        else
        {  // rollback
          if( NULL != copied_android_send_ims_msg_params )
          {
              qcril_free( copied_android_send_ims_msg_params );
              copied_android_send_ims_msg_params = NULL;
          }

          if( NULL != copied_android_cdma_send_ims_param )
          {
              qcril_free( copied_android_cdma_send_ims_param );
              copied_android_cdma_send_ims_param = NULL;
          }

          if( NULL != copied_android_gw_smsc_address )
          {
              qcril_free( copied_android_gw_smsc_address );
              copied_android_gw_smsc_address = NULL;
          }

          if( NULL != copied_android_gw_pdu )
          {
              qcril_free( copied_android_gw_pdu );
              copied_android_gw_pdu = NULL;
          }

          if( NULL != copied_android_gw_sms_ims_params )
          {
              qcril_free( copied_android_gw_sms_ims_params );
              copied_android_gw_sms_ims_params = NULL;
          }

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }

      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_SEND_SMS:              // fallthrough
    case RIL_REQUEST_SEND_SMS_EXPECT_MORE:
      if (  NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO  )
      {
        android_send_sms_params = (char**)android_request_data;

        local_ok = FALSE;

        copied_android_gw_smsc_address    = NULL;
        copied_android_gw_pdu             = NULL;
        copied_android_gw_sms_ims_params  = NULL;

        do
        {
          // sms sc
          str_access = android_send_sms_params[ 0 ];
          copied_android_gw_smsc_address = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_gw_smsc_address && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }

          // pdu
          str_access = android_send_sms_params[ 1 ];
          copied_android_gw_pdu = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_gw_pdu && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }

          copied_android_gw_sms_ims_params = qcril_malloc( 2 * sizeof( char* ) ) ;
          if ( NULL == copied_android_gw_sms_ims_params )
          {
            status = E_NO_MEMORY;
            break;
          }

          copied_android_gw_sms_ims_params[0] = copied_android_gw_smsc_address;
          copied_android_gw_sms_ims_params[1] = copied_android_gw_pdu;

          local_ok = TRUE;
          status = E_SUCCESS;

        } while ( FALSE );

        if ( local_ok )
        {
          locally_created_custom_storage      = copied_android_gw_sms_ims_params;
          locally_created_custom_storage_len  = 2 * sizeof( char* );

          need_common_clone = FALSE; // we already did it

          QCRIL_LOG_DEBUG("sms allo 0x%x, 0x%x, 0x%x", copied_android_gw_sms_ims_params, copied_android_gw_smsc_address, copied_android_gw_pdu);

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
        }
        else
        {  // rollback
          if( NULL != copied_android_gw_smsc_address )
          {
              qcril_free( copied_android_gw_smsc_address );
              copied_android_gw_smsc_address = NULL;
          }

          if( NULL != copied_android_gw_pdu )
          {
              qcril_free( copied_android_gw_pdu );
              copied_android_gw_pdu = NULL;
          }

          if( NULL != copied_android_gw_sms_ims_params )
          {
              qcril_free( copied_android_gw_sms_ims_params );
              copied_android_gw_sms_ims_params = NULL;
          }

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_CDMA_SEND_SMS:
      if ( NULL != android_request_data )
      {
        len_to_go  = sizeof( RIL_CDMA_SMS_Message );

        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;

        need_common_clone = TRUE;
        status = E_SUCCESS;
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_WRITE_SMS_TO_SIM:
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        android_write_sms_to_sim_msg_params = (RIL_SMS_WriteArgs*)android_request_data;

        copied_android_write_sms_to_sim_msg_params  = NULL;
        copied_android_write_sms_to_sim_msg_smsc_address = NULL;
        copied_android_write_sms_to_sim_msg_pdu = NULL;

        local_ok = FALSE;
        do
        {
          copied_android_write_sms_to_sim_msg_params = qcril_malloc( sizeof( *copied_android_write_sms_to_sim_msg_params ) );

          if ( NULL == copied_android_write_sms_to_sim_msg_params )
          {
            status = E_NO_MEMORY;
            break;
          }
          *copied_android_write_sms_to_sim_msg_params = *android_write_sms_to_sim_msg_params;

          // sms sc
          str_access = android_write_sms_to_sim_msg_params->smsc;
          copied_android_write_sms_to_sim_msg_smsc_address = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_write_sms_to_sim_msg_smsc_address && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }

          // pdu
          str_access = android_write_sms_to_sim_msg_params->pdu;
          copied_android_write_sms_to_sim_msg_pdu = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_write_sms_to_sim_msg_pdu && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }

          copied_android_write_sms_to_sim_msg_params->smsc = copied_android_write_sms_to_sim_msg_smsc_address;
          copied_android_write_sms_to_sim_msg_params->pdu = copied_android_write_sms_to_sim_msg_pdu;

          local_ok = TRUE;
          status = E_SUCCESS;
        } while (FALSE);

        if ( local_ok )
        {
          locally_created_custom_storage      = copied_android_write_sms_to_sim_msg_params;
          locally_created_custom_storage_len  = sizeof ( *copied_android_write_sms_to_sim_msg_params );

          need_common_clone = FALSE; // we already did it

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
        }
        else
        {  // rollback
          if( NULL != copied_android_write_sms_to_sim_msg_params )
          {
              qcril_free( copied_android_write_sms_to_sim_msg_params );
              copied_android_write_sms_to_sim_msg_params = NULL;
          }

          if( NULL != copied_android_write_sms_to_sim_msg_smsc_address )
          {
              qcril_free( copied_android_write_sms_to_sim_msg_smsc_address );
              copied_android_write_sms_to_sim_msg_smsc_address = NULL;
          }

          if( NULL != copied_android_write_sms_to_sim_msg_pdu )
          {
              qcril_free( copied_android_write_sms_to_sim_msg_pdu );
              copied_android_write_sms_to_sim_msg_pdu = NULL;
          }

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }

      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_SET_SMSC_ADDRESS:                    // fallthrough
    case RIL_REQUEST_SEND_USSD:
      // str
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        str_access = (char*)android_request_data;
        len_to_go  = strlen( str_access )  + QMI_RIL_SINGLE;

        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;

        need_common_clone = TRUE;
        status = E_SUCCESS;
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
      /* only legacy format with RAT needs */
      if ( qmi_ril_is_feature_supported(QMI_RIL_FEATURE_LEGACY_RAT) )
      {
        if (  NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO  )
        {
          android_manual_selection_params = (char**)android_request_data;

          local_ok = FALSE;

          copied_android_manual_selection_mcc_mnc     = NULL;
          copied_android_manual_selection_rat         = NULL;
          copied_android_manual_selection_params      = NULL;

          do
          {
            // mcc mnc
            str_access = android_manual_selection_params[ 0 ];
            copied_android_manual_selection_mcc_mnc = qmi_ril_util_str_clone( str_access );
            if ( NULL == copied_android_manual_selection_mcc_mnc && NULL != str_access )
            {
              status = E_INVALID_ARG;
              break;
            }

            // rat
            str_access = android_manual_selection_params[ 1 ];
            copied_android_manual_selection_rat = qmi_ril_util_str_clone( str_access );
            if ( NULL == copied_android_manual_selection_rat && NULL != str_access )
            {
              status = E_INVALID_ARG;
              break;
            }

            copied_android_manual_selection_params = qcril_malloc( 2 * sizeof( char* ) ) ;
            if ( NULL == copied_android_manual_selection_params )
            {
              status = E_NO_MEMORY;
              break;
            }

            copied_android_manual_selection_params[0] = copied_android_manual_selection_mcc_mnc;
            copied_android_manual_selection_params[1] = copied_android_manual_selection_rat;

            local_ok = TRUE;
            status = E_SUCCESS;

          } while ( FALSE );

          if ( local_ok )
          {
            locally_created_custom_storage      = copied_android_manual_selection_params;
            locally_created_custom_storage_len  = 2 * sizeof( char* );

            need_common_clone = FALSE; // we already did it

            QCRIL_LOG_DEBUG("manual sel allo 0x%x, 0x%x, 0x%x", copied_android_manual_selection_params, copied_android_manual_selection_mcc_mnc, copied_android_manual_selection_rat);

            res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
          }
          else
          {  // rollback
            if( NULL != copied_android_manual_selection_mcc_mnc )
            {
                qcril_free( copied_android_manual_selection_mcc_mnc );
                copied_android_manual_selection_mcc_mnc = NULL;
            }

            if( NULL != copied_android_manual_selection_rat )
            {
                qcril_free( copied_android_manual_selection_rat );
                copied_android_manual_selection_rat = NULL;
            }

            if( NULL != copied_android_manual_selection_params )
            {
                qcril_free( copied_android_manual_selection_params );
                copied_android_manual_selection_params = NULL;
            }

            res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
          }
        }
        else
        {
          status = E_INVALID_ARG;
          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }
      }
      else
      {
        // str
        if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
        {
          str_access = (char*)android_request_data;
          len_to_go  = strlen( str_access )  + QMI_RIL_SINGLE;

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;

          need_common_clone = TRUE;
          status = E_SUCCESS;
        }
        else
        {
          status = E_INVALID_ARG;
          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }
      }
      break;

    case RIL_REQUEST_SET_UICC_SUBSCRIPTION:
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        len_to_go  = sizeof( RIL_SelectUiccSub );

        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;

        need_common_clone = TRUE;
        status = E_SUCCESS;
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_QUERY_CALL_WAITING:            // fallthrough
    case RIL_REQUEST_SET_TTY_MODE:                  // fallthrough
    case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE:   // fallthrough
    case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE:  // fallthrough
    case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:    // fallthrough
    case RIL_REQUEST_SET_BAND_MODE:                 // fallthrough
    case RIL_REQUEST_SET_LOCATION_UPDATES:          // fallthrough
    case RIL_REQUEST_SCREEN_STATE:                  // fallthrough
    case RIL_REQUEST_SEPARATE_CONNECTION:           // fallthrough
    case RIL_REQUEST_HANGUP:                        // fallthrough
    case RIL_REQUEST_RADIO_POWER:                   // fallthrough
    case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED:
      // int
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        len_to_go  = sizeof( int );

        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;

        need_common_clone = TRUE;
        status = E_SUCCESS;
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_DTMF:
    case RIL_REQUEST_DTMF_START:
      // single char
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        len_to_go  = sizeof( char );

        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;

        need_common_clone = TRUE;
        status = E_SUCCESS;
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_CDMA_BURST_DTMF:
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        copied_cdma_dtmf_holder  = NULL;
        copied_cdma_dtmf_str     = NULL;
        copied_cdma_dtmf_on      = NULL;
        copied_cdma_dtmf_off     = NULL;
        android_cdma_dtmf_holder = (char**)android_request_data;

        local_ok                 = FALSE;

        do
        {
          copied_cdma_dtmf_holder = qcril_malloc( 3 * sizeof(char*) );
          if ( NULL == copied_cdma_dtmf_holder )
          {
            status = E_NO_MEMORY;
            break;
          }

          copied_cdma_dtmf_str = qmi_ril_util_str_clone( android_cdma_dtmf_holder[0] );
          if ( NULL == copied_cdma_dtmf_str )
          {
            status = E_INVALID_ARG;
            break;
          }

          copied_cdma_dtmf_on = qmi_ril_util_str_clone( android_cdma_dtmf_holder[1] );
          if ( NULL == copied_cdma_dtmf_on )
          {
            status = E_INVALID_ARG;
            break;
          }

          copied_cdma_dtmf_off = qmi_ril_util_str_clone( android_cdma_dtmf_holder[2] );
          if ( NULL == copied_cdma_dtmf_off )
          {
            status = E_INVALID_ARG;
            break;
          }

          copied_cdma_dtmf_holder[0] = copied_cdma_dtmf_str;
          copied_cdma_dtmf_holder[1] = copied_cdma_dtmf_on;
          copied_cdma_dtmf_holder[2] = copied_cdma_dtmf_off;

          local_ok = TRUE;
          status = E_SUCCESS;

        } while (FALSE);

        if ( local_ok )
        {
          res_inclanation   = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
          need_common_clone = FALSE;

          locally_created_custom_storage      = copied_cdma_dtmf_holder;
          locally_created_custom_storage_len  = 3 * sizeof( char* );
        }
        else
        {
          if( NULL != copied_cdma_dtmf_off )
          {
              qcril_free( copied_cdma_dtmf_off );
              copied_cdma_dtmf_off = NULL;
          }

          if( NULL != copied_cdma_dtmf_on )
          {
              qcril_free( copied_cdma_dtmf_on );
              copied_cdma_dtmf_on = NULL;
          }

          if( NULL != copied_cdma_dtmf_str )
          {
              qcril_free( copied_cdma_dtmf_str );
              copied_cdma_dtmf_str = NULL;
          }

          if( NULL != copied_cdma_dtmf_holder )
          {
              qcril_free( copied_cdma_dtmf_holder );
              copied_cdma_dtmf_holder = NULL;
          }

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY:
      res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        copied_str = qcril_malloc( 26 + 1 ); // 26 per ril.h
        if ( NULL != copied_str )
        {
          locally_created_custom_storage              = copied_str;
          locally_created_custom_storage_len          = 26 + 1;
          memcpy( locally_created_custom_storage, android_request_data, 26 ); // without + 1

          need_common_clone                           = FALSE;
          res_inclanation                             = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
        }
        else
        {
          status = E_NO_MEMORY;
        }
      }
      else
      {
        status = E_INVALID_ARG;
      }
      break;

    case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:     // fallthough
    case RIL_REQUEST_SET_CALL_FORWARD:
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        android_query_call_fwd_info_params = (RIL_CallForwardInfo*)android_request_data;

        copied_android_query_call_fwd_info_params = NULL;
        copied_android_query_call_fwd_info_number = NULL;

        local_ok = FALSE;
        do
        {
          copied_android_query_call_fwd_info_params = qcril_malloc( sizeof( *copied_android_query_call_fwd_info_params ) );

          if ( NULL == copied_android_query_call_fwd_info_params )
          {
            status = E_NO_MEMORY;
            break;
          }
          *copied_android_query_call_fwd_info_params = *android_query_call_fwd_info_params;

          // number
          str_access = android_query_call_fwd_info_params->number;
          copied_android_query_call_fwd_info_number = qmi_ril_util_str_clone( str_access );
          if ( NULL == copied_android_query_call_fwd_info_number && NULL != str_access )
          {
            status = E_INVALID_ARG;
            break;
          }

          copied_android_query_call_fwd_info_params->number = copied_android_query_call_fwd_info_number;

          local_ok = TRUE;
          status = E_SUCCESS;
        } while (FALSE);

        if ( local_ok )
        {
          locally_created_custom_storage      = copied_android_query_call_fwd_info_params;
          locally_created_custom_storage_len  = sizeof ( *copied_android_query_call_fwd_info_params );

          need_common_clone = FALSE; // we already did it

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
        }
        else
        {  // rollback
          if ( NULL != copied_android_query_call_fwd_info_params )
          {
            qcril_free( copied_android_query_call_fwd_info_params );
            copied_android_query_call_fwd_info_params = NULL;
          }
          if ( NULL != copied_android_query_call_fwd_info_number )
          {
            qcril_free( copied_android_query_call_fwd_info_number );
            copied_android_query_call_fwd_info_number = NULL;
          }

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {

        android_change_barring_pwd_params = (char**)android_request_data;

        local_ok = FALSE;

        copied_change_barring_pwd_params = NULL;
        copied_ch_bar_pwd_faclity        = NULL;
        copied_ch_bar_pwd_old_pwd        = NULL;
        copied_ch_bar_pwd_new_pwd        = NULL;

        do
        {
          // facility
          android_ch_bar_pwd_faclity = android_change_barring_pwd_params[ 0 ];
          copied_ch_bar_pwd_faclity = qmi_ril_util_str_clone( android_ch_bar_pwd_faclity );
          if ( NULL == copied_ch_bar_pwd_faclity && NULL != android_ch_bar_pwd_faclity )
          {
            status = E_INVALID_ARG;
            break;
          }

          // old pwd
          android_ch_bar_pwd_old_pwd = android_change_barring_pwd_params[ 1 ];
          copied_ch_bar_pwd_old_pwd = qmi_ril_util_str_clone( android_ch_bar_pwd_old_pwd );
          if ( NULL == copied_ch_bar_pwd_old_pwd && NULL != android_ch_bar_pwd_old_pwd )
          {
            status = E_INVALID_ARG;
            break;
          }

          // new pwd
          android_ch_bar_pwd_new_pwd = android_change_barring_pwd_params[ 2 ];
          copied_ch_bar_pwd_new_pwd = qmi_ril_util_str_clone( android_ch_bar_pwd_new_pwd );
          if ( NULL == copied_ch_bar_pwd_new_pwd && NULL != android_ch_bar_pwd_new_pwd )
          {
            status = E_INVALID_ARG;
            break;
          }

          copied_change_barring_pwd_params = qcril_malloc( 3 * sizeof( char* ) ) ;
          if ( NULL == copied_change_barring_pwd_params )
          {
            status = E_NO_MEMORY;
            break;
          }

          copied_change_barring_pwd_params[0] = copied_ch_bar_pwd_faclity;
          copied_change_barring_pwd_params[1] = copied_ch_bar_pwd_old_pwd;
          copied_change_barring_pwd_params[2] = copied_ch_bar_pwd_new_pwd;

          local_ok = TRUE;
          status = E_SUCCESS;

        } while ( FALSE );

        if ( local_ok )
        {
          locally_created_custom_storage      = copied_change_barring_pwd_params;
          locally_created_custom_storage_len  = 3 * sizeof( char* );

          need_common_clone = FALSE; // we already did it

          QCRIL_LOG_DEBUG("change bar pwd alloc 0x%x", copied_change_barring_pwd_params );

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
        }
        else
        {  // rollback
          if ( NULL != copied_change_barring_pwd_params )
          {
            qcril_free( copied_change_barring_pwd_params );
          }

          if ( NULL != copied_ch_bar_pwd_faclity )
          {
            qcril_free( copied_ch_bar_pwd_faclity );
          }

          if ( NULL != copied_ch_bar_pwd_old_pwd )
          {
            qcril_free( copied_ch_bar_pwd_old_pwd );
          }

          if ( NULL != copied_ch_bar_pwd_new_pwd )
          {
            qcril_free( copied_ch_bar_pwd_new_pwd );
          }

          copied_change_barring_pwd_params = NULL;
          copied_ch_bar_pwd_faclity        = NULL;
          copied_ch_bar_pwd_old_pwd        = NULL;
          copied_ch_bar_pwd_new_pwd        = NULL;

          res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE:
      if ( NULL != android_request_data )
      {
        len_to_go  = sizeof( RIL_CDMA_SMS_Ack );

        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;

        need_common_clone = TRUE;
        status = E_SUCCESS;
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    case RIL_REQUEST_SET_CALL_WAITING:
    case RIL_REQUEST_SMS_ACKNOWLEDGE:
      if ( NULL != android_request_data && android_request_data_len > QMI_RIL_ZERO )
      {
        len_to_go  = 2 * sizeof( int );

        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;

        need_common_clone = TRUE;
        status = E_SUCCESS;
      }
      else
      {
        status = E_INVALID_ARG;
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      }
      break;

    default:
      if ( NULL == android_request_data )
      {
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION;
        len_to_go       = QMI_RIL_ZERO;
      }
      else if ( android_request_data_len <= QMI_RIL_FOUR_BYTES )
      {
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT;
        len_to_go       = android_request_data_len;
      }
      else
      {
        res_inclanation = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
        len_to_go       = android_request_data_len;
      }
      need_common_clone = TRUE;
      status = E_SUCCESS;
      break;
  }

  switch ( res_inclanation )
  {
    case QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY:
      if ( need_common_clone )
      {
        if ( len_to_go > QMI_RIL_ZERO )
        {
          locally_created_custom_storage = qcril_malloc( len_to_go );
          if ( NULL != locally_created_custom_storage )
          {
            memcpy( locally_created_custom_storage, android_request_data, len_to_go );
            locally_created_custom_storage_len = len_to_go;

            res = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
          }
          else
          {
            status = E_NO_MEMORY;
            res = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
          }
        }
        else
        { // unexpected
          status = E_FAILURE;
          res = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
        }
      }
      else
      { // ready to go
        res = QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY;
      }
      break;

    case QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT:
      if ( need_common_clone && len_to_go > QMI_RIL_ZERO && NULL != four_byte_storage)
      {
        memcpy( four_byte_storage, android_request_data, len_to_go );
      }
      local_four_byte_storage_occupied = len_to_go;
      res = QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT;
      break;

    case QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION:
      res = QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION;
      break;

    default:
      res = QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID;
      break;
  }

  if ( NULL != four_byte_storage_occupied )
  {
    *four_byte_storage_occupied = local_four_byte_storage_occupied;
  }
  if ( NULL != sub_created_custom_storage )
  {
    *sub_created_custom_storage = locally_created_custom_storage;
  }
  if ( NULL != custom_storage_len )
  {
    *custom_storage_len = locally_created_custom_storage_len;
  }

  switch ( res )
  {
    case QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT:
      QCRIL_LOG_INFO(".. params for Android request id %d are of plain structure and cloned to preallocated queue buf taking length of %d",
                        android_request_id, (int) local_four_byte_storage_occupied  );
      break;

    case QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY:
      QCRIL_LOG_INFO(".. params for Android request id %d are of complex structure and cloned to 0x%x length %d",
                        android_request_id, locally_created_custom_storage, (int) locally_created_custom_storage_len );
      break;

    case QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION:
      QCRIL_LOG_INFO(".. params for Android request id %d require no copy action (usually means empty payload)", android_request_id);
      break;

    case QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID:  // fallthrough
    default:
      QCRIL_LOG_INFO(".. params for Android request id %d either do no require copying or unsupported case", android_request_id);
      break;
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET( (int)res );
  if(error != NULL)
  {
    *error = status;
  }

  return res;
} // qmi_ril_fw_create_android_live_params_copy

/*=========================================================================
  FUNCTION:  qcril_default_request_resp_params

===========================================================================*/
/*!
    @brief
    Set default values for parameters in RIL request's response

    @return
    None
*/
/*=========================================================================*/
void qcril_default_request_resp_params
(
  qcril_instance_id_e_type instance_id,
  RIL_Token t,
  qcril_evt_e_type request_id,
  RIL_Errno ril_err_no,
  qcril_request_resp_params_type *param_ptr
)
{
    if(instance_id < QCRIL_MAX_INSTANCE_ID && param_ptr != NULL)
    {
        param_ptr->instance_id        = instance_id;
        param_ptr->t                  = t;
        param_ptr->request_id         = request_id;
        param_ptr->request_id_android = qcril_event_get_android_request(request_id);
        param_ptr->ril_err_no         = ril_err_no;
        param_ptr->resp_pkt           = NULL;
        param_ptr->resp_len           = 0;
        param_ptr->logstr             = NULL;
        param_ptr->rild_sock_oem_req  = 0;
    }
    else
    {
        QCRIL_LOG_FATAL("CHECK FAILED");
    }

} /* qcril_default_request_resp_params */

//===========================================================================
// qmi_ril_fw_send_request_response_epilog
//===========================================================================
void qmi_ril_fw_send_request_response_epilog( qcril_instance_id_e_type instance_id,
                                              RIL_Token token,
                                              qcril_evt_e_type android_event_id,
                                              RIL_Errno resp_cause,
                                              void* resp_data,
                                              uint32 resp_data_len,
                                              int is_abnormal_drop,
                                              qcril_request_resp_params_type *param_ptr_ref )
{
  RIL_Token following_token;
  RIL_Token prev_token;
  int       go_on;

  qmi_ril_fw_android_request_kind_execution_overview_type*
                                            android_request_kind_execution_overview;
  uint32_t                                  android_request_handling_caps;
  qmi_ril_fw_android_request_holder_type*   android_request_param_holder;
  qmi_ril_fw_android_request_holder_type*   android_request_prm_hldr_iter;
  uint32_t                                  cap_max_queue_sz;
  uint32_t                                  cap_queue_sz_iter;
  int                                       do_flow_control_follow_up;
  int                                       need_trigger_address_remains;
  int                                       regarded_same;
  uint32_t                                  payload_sz_cur;
  int                                       need_protection;
  int                                       any_oustanding_requests;
  qcril_evt_e_type                          final_android_event_id;
  int                                       ok_compensated_req_id;
  int                                       iter_android_request_id;
  uint32                                    flow_decision_flag = FLOWCONTROL_ONREQUEST_NONE;

  QCRIL_NOTUSED(is_abnormal_drop);
  QCRIL_LOG_FUNC_ENTRY();

  final_android_event_id = android_event_id;

  if ( qcril_reqlist_has_follower( instance_id, token ) && !is_abnormal_drop && NULL != param_ptr_ref )
  {
    if ( !is_abnormal_drop && NULL != param_ptr_ref  )
    {
      qcril_free_req_and_dispatch_follower_req_payload_type *payload;
      payload = qcril_malloc(sizeof(*payload));
      if ( NULL != payload )
      {
        go_on = FALSE;

        payload->t = token;
        payload->token_id = qcril_log_get_token_id(token);
        payload->instance_id = instance_id;
        payload->data = qcril_malloc(sizeof(*param_ptr_ref)); // malloc for qcril_request_resp_params_type
        if ( NULL != payload->data )
        {
          *payload->data = *param_ptr_ref;
          go_on = FALSE;
          if (QMI_RIL_ZERO != resp_data_len )
          {
            payload->data->resp_pkt = qcril_malloc( resp_data_len );
            if ( NULL != payload->data->resp_pkt )
            {
              memcpy(payload->data->resp_pkt, resp_data, resp_data_len);
              go_on = TRUE;
            }
          }
          else
          {
            payload->data->resp_pkt = NULL;
            go_on = TRUE;
          }

          if ( go_on )
          {
            qcril_setup_timed_callback_ex_params( QCRIL_DEFAULT_INSTANCE_ID,
                                                  QCRIL_DEFAULT_MODEM_ID,
                                                  qcril_free_request_and_dispatch_follower_request_cb,
                                                  payload,
                                                  NULL,   // immediate
                                                  NULL );
          }
        }
        if ( NULL != payload && !go_on )
        {
          if ( NULL != payload->data )
          {
            qcril_free( payload->data );
          }
          qcril_free( payload );
          payload = NULL;
        }
      }
    }
    else
    { // follow line
      prev_token = token;
      do
      {
        following_token = qcril_reqlist_get_follower_token( instance_id, prev_token );
        ( void ) qcril_reqlist_free( instance_id , prev_token );
        prev_token = following_token;
      } while ( QMI_RIL_ZERO != prev_token );
    }
  }
  else if (qcril_reqlist_under_follower_handler_exec( instance_id, token )  )
  {
    qcril_deferred_free_req_payload_type *payload;
    payload = qcril_malloc(sizeof(*payload));
    if (payload != NULL)
    {
      payload->t = token;
      payload->token_id = qcril_log_get_token_id(token);
      payload->instance_id = instance_id;
      qcril_setup_timed_callback_ex_params( QCRIL_DEFAULT_INSTANCE_ID,
                                            QCRIL_DEFAULT_MODEM_ID,
                                            qcril_free_request_list_entry_deferred,
                                            (void*) payload,
                                            NULL,   // immediate
                                            NULL );
    }
    else
    {
      QCRIL_LOG_ERROR("failed to allocate the payload for deferred reqlist free");
    }
  }
  else
  { // Remove entry from Reqlist if applicable
    ( void ) qcril_reqlist_free( instance_id , token );
  }

  // flow control s
  if ( param_ptr_ref != NULL )
  {
    flow_decision_flag = param_ptr_ref->flow_decision_flag;
  }

  QCRIL_LOG_DEBUG("flow control decision - %d", flow_decision_flag);
  QCRIL_LOG_DEBUG("main trd %x, cur trd %x", qmi_ril_fw_get_main_thread_id(), pthread_self() );
  QCRIL_LOG_DEBUG("eq %d, under mn thrd %d", pthread_equal( qmi_ril_fw_get_main_thread_id(), pthread_self()) , qmi_ril_fw_android_request_flow_control_overview.in_exec_on_main_thread );
  need_protection = !( ( pthread_equal( qmi_ril_fw_get_main_thread_id(), pthread_self() ) ) && qmi_ril_fw_android_request_flow_control_overview.in_exec_on_main_thread );
  QCRIL_LOG_DEBUG("need protection %d ", need_protection );

  if ( need_protection )
  {
    qmi_ril_fw_android_request_flow_control_info_lock();
  }

  QCRIL_LOG_DEBUG( "in protected zone" );

  do_flow_control_follow_up = FALSE;
  need_trigger_address_remains = FALSE;
  do
  {
    if ( !qcril_is_event_in_group(android_event_id, &QCRIL_GRP_ANDROID_RIL_REQ) )
    { // attempt restoring original android event id from token if event id got altered
      ok_compensated_req_id = FALSE;
      android_request_kind_execution_overview = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ QMI_RIL_ZERO ];
      for ( iter_android_request_id = QMI_RIL_ZERO; iter_android_request_id <= QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID && !ok_compensated_req_id; iter_android_request_id++  )
      {
        if ( token == android_request_kind_execution_overview->token_under_execution )
        {
          final_android_event_id = qcril_android_request_get_internal_event(iter_android_request_id);
          ok_compensated_req_id    = TRUE;
          QCRIL_LOG_DEBUG( "restrored org. a-r-id %p", final_android_event_id );
        }
        else
        {
          android_request_kind_execution_overview++;
        }
      }

      if ( !ok_compensated_req_id )
        break; // not expected
    }
    else
    {
      final_android_event_id = android_event_id;
    }

    int final_android_id = qcril_event_get_android_request(final_android_event_id);
    if (final_android_id > 0) {
        android_request_kind_execution_overview = &qmi_ril_fw_android_request_flow_control_overview.android_request_kind_info[ final_android_id ];
    }
    else {
        QCRIL_LOG_DEBUG("invalid id obtained");
        break;
    }
    

    android_request_handling_caps = android_request_kind_execution_overview->nof_extra_holders_and_caps_and_dynamics;
    cap_max_queue_sz = QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_DECOMPOSE_QUEUE_SZ( android_request_handling_caps );

    if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_FLOW_CONTROL_EXEMPT )
      break; // no flow control action

    QCRIL_LOG_DEBUG( "token under exec %" PRId32 ", completion token id %" PRId32 "", qcril_log_get_token_id( android_request_kind_execution_overview->token_under_execution) , qcril_log_get_token_id (token) ) ;

    /* If request is flow control rejected token_execution will not match token.
       So RIL has to send response to telephony, otherwise don't send response to
       telephony to avoid crash.*/
    if ( android_request_kind_execution_overview->token_under_execution != token )
    {
        break; // not mainstream execution, handler was not invoked
    }

    android_request_kind_execution_overview->token_under_execution = QMI_RIL_ZERO;

    // locate bookkeeping ref
    android_request_param_holder = NULL;
    if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_SINGLE_ONLY )
    {
      android_request_param_holder = &android_request_kind_execution_overview->holders.local_holder;
    }
    else if ( android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE )
    {
      android_request_prm_hldr_iter = android_request_kind_execution_overview->holders.extra_holders;
      for ( cap_queue_sz_iter = QMI_RIL_ZERO; cap_queue_sz_iter < cap_max_queue_sz && NULL == android_request_param_holder; cap_queue_sz_iter++ )
      {
        if ( !(android_request_prm_hldr_iter->param_info_and_len & QMI_RIL_FW_ANDROID_REQUEST_INFO_SLOT_AVAILABLE) && android_request_prm_hldr_iter->token == token )
        {
          android_request_param_holder = android_request_prm_hldr_iter;
        }
        else
        {
          android_request_prm_hldr_iter++;
        }
      }
    }
    else // invalid case
      break;

    if ( NULL == android_request_param_holder )
      break;


    do_flow_control_follow_up = TRUE;
  } while (FALSE);

  // do post
  QCRIL_LOG_DEBUG("resp token-id %" PRId32 ", req-id %d(%d)",
                   qcril_log_get_token_id( token ), final_android_event_id, android_event_id );

  /* Print RIL Message */
  qcril_log_print_ril_message(final_android_event_id, RIL__MSG_TYPE__RESPONSE, resp_data,
                              resp_data_len, resp_cause);

  if (token && !qcril_is_internal_token(token))
  {

    if (!android_ril_try_respond(param_ptr_ref)) {
      qcril_response_api[ instance_id ]->OnRequestComplete( token,
                                                            resp_cause,
                                                            resp_data,
                                                            resp_data_len );
    }
  }
  else
  {
    QCRIL_LOG_DEBUG("internal RIL message");
  }

  if ( do_flow_control_follow_up )
  {
    QCRIL_LOG_DEBUG("do_flow_control_follow_up");
    payload_sz_cur = QMI_RIL_FW_ANDROID_REQUEST_INFO_DECOMPOSE_SZ( android_request_param_holder->param_info_and_len );

    if (  (android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_ALLOW_MULTIPLE) &&
         !(android_request_handling_caps & QMI_RIL_FW_ANDROID_REQUEST_HNDL_CAPS_NO_AUTO_RESPONSE) )
    { // auto response check
      android_request_prm_hldr_iter = android_request_kind_execution_overview->holders.extra_holders;
      for ( cap_queue_sz_iter = QMI_RIL_ZERO; cap_queue_sz_iter < cap_max_queue_sz ; cap_queue_sz_iter++  )
      {
        regarded_same = FALSE;
        if ( android_request_prm_hldr_iter->token && android_request_prm_hldr_iter->token != token )
        { // not self
          regarded_same = qmi_ril_fw_android_request_flow_control_request_holders_have_same_param( android_request_param_holder, android_request_prm_hldr_iter );
        }

        QCRIL_LOG_DEBUG("regarded_same %d",regarded_same);
        if ( regarded_same )
        { // do auto response
          QCRIL_LOG_DEBUG("resp token-id %" PRId32 ", req-id %d",
                          qcril_log_get_token_id(android_request_prm_hldr_iter->token),
                          final_android_event_id);

          qcril_response_api[ instance_id ]->OnRequestComplete( android_request_prm_hldr_iter->token,
                                                                resp_cause,
                                                                resp_data,
                                                                resp_data_len );

          // cleanup
          qmi_ril_fw_android_request_flow_control_release_holder_info_bookref( android_request_prm_hldr_iter, final_android_event_id );
        }
        android_request_prm_hldr_iter++;
      }
    }

    // cleanup for just finsihed execution
    qmi_ril_fw_android_request_flow_control_release_holder_info_bookref( android_request_param_holder, final_android_event_id );

    qmi_ril_fw_android_request_flow_control_overview_request_review_holders( android_request_kind_execution_overview );

    // outstanding left for execution?
    any_oustanding_requests = qmi_ril_fw_android_request_flow_control_find_request_for_execution( android_request_kind_execution_overview, NULL, NULL );
    if ( any_oustanding_requests )
    {
      need_trigger_address_remains = TRUE;
    }
  }
  if ( need_protection )
  {
    qmi_ril_fw_android_request_flow_control_info_unlock();
  }
  QCRIL_LOG_DEBUG( "left zone " );

  if ( need_trigger_address_remains )
  {
    qcril_setup_timed_callback_ex_params( QCRIL_DEFAULT_INSTANCE_ID,
                                          QCRIL_DEFAULT_MODEM_ID,
                                          qmi_ril_fw_android_request_flow_control_trigger_remains,
                                          (void *)(intptr_t)final_android_event_id,
                                          NULL,   // immediate
                                          NULL );
  }
  // flow control e

  QCRIL_LOG_FUNC_RETURN();
} // qmi_ril_fw_send_request_response_epilog

//===========================================================================
//qcril_send_request_response
//===========================================================================
void qcril_send_request_response
(
  qcril_request_resp_params_type *param_ptr
)
{

  QCRIL_LOG_DEBUG("%s","Debug : qcril_send_request_response invoked");
  qcril_instance_id_e_type instance_id;
  char label[ 512 ];

    qmi_ril_oem_hook_response_context_type * oem_hook_response_ctx = NULL;
    qmi_ril_oem_hook_response_context_type * prev_iter = NULL;
    int                                      is_qmi_hook_idl_tunneling_response;
    RIL_Token                                token_param;
    char*                                    substituted_data = NULL;
    char*                                    orig_substituted_data = NULL;
    uint32_t                                 substituted_data_len = 0;
    qmi_idl_service_object_type              qmi_idl_tunneling_service_object;
    qmi_client_error_type                    idl_err = QMI_NO_ERR;
    int                                      successfully_substituted;
    void *                                   actual_resp_pkt;
    size_t                                   actual_resp_len;
    uint32_t                                 encoded_fact = 0;

    uint32_t *                               int32_param;
    uint16_t *                               int16_param;
    uint32                                   log_request_id;
    const char*                              log_evt_name;
    boolean                                  is_oem_response = FALSE;
    qcril_evt_e_type                           android_request_id_for_response;


    if (param_ptr != NULL &&
       (param_ptr->instance_id < QCRIL_MAX_INSTANCE_ID))
    {
          actual_resp_pkt = param_ptr->resp_pkt;
          actual_resp_len = param_ptr->resp_len;

          substituted_data = NULL;

          int oemhook_req_id =  qcril_qmi_oem_get_oemhook_msg(param_ptr->request_id);
          switch ( oemhook_req_id )
          {
            case QCRIL_REQ_HOOK_NV_READ:                    // fall through
            case QCRIL_REQ_HOOK_NV_WRITE:                   // fall through
            case QCRIL_REQ_HOOK_DATA_GO_DORMANT:            // fall through
            case QCRIL_REQ_HOOK_ME_DEPERSONALIZATION:       // fall through
            case QCRIL_REQ_HOOK_REQ_GENERIC:
            case QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN:
            case QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF:
            case QCRIL_REQ_HOOK_CSG_GET_SYS_INFO:
            case QCRIL_REQ_HOOK_UICC_VOLTAGE_STATUS_REQ:
            case QCRIL_REQ_HOOK_PERSONALIZATION_REACTIVATE_REQ:
            case QCRIL_REQ_HOOK_PERSONALIZATION_STATUS_REQ:
            case QCRIL_REQ_HOOK_ENTER_DEPERSONALIZATION_REQ:
            case QCRIL_REQ_HOOK_SLOT_GET_CARD_STATE_REQ:
            case QCRIL_REQ_HOOK_SLOT_GET_ATR_REQ:
            case QCRIL_REQ_HOOK_SLOT_SEND_APDU_REQ:
            case QCRIL_REQ_HOOK_SWITCH_SLOT_REQ:
            case QCRIL_REQ_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ:
            case QCRIL_REQ_HOOK_SLOT_CARD_POWER_REQ:
            case QCRIL_REQ_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ:
            case QCRIL_REQ_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
            case QCRIL_REQ_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
            case QCRIL_REQ_HOOK_GET_SLOTS_STATUS_REQ:
            // DSDS/DSDA/TSTS requests
            case QCRIL_REQ_HOOK_SET_TUNE_AWAY:
            case QCRIL_REQ_HOOK_GET_TUNE_AWAY:
            case QCRIL_REQ_HOOK_SET_PAGING_PRIORITY:
            case QCRIL_REQ_HOOK_GET_PAGING_PRIORITY:
            case QCRIL_REQ_HOOK_SET_DEFAULT_VOICE_SUB:
            case QCRIL_REQ_HOOK_SET_LOCAL_CALL_HOLD:
            case QCRIL_REQ_HOOK_GET_MODEM_CAPABILITY:
            case QCRIL_REQ_HOOK_UPDATE_SUB_BINDING:
            case QCRIL_REQ_HOOK_SET_LTE_TUNE_AWAY:
                android_request_id_for_response = QCRIL_EVT_RIL_REQUEST_OEM_HOOK_RAW;
              break;
            default:
              android_request_id_for_response = param_ptr->request_id;
              break;
          }

          if ( ( qcril_is_event_in_group(param_ptr->request_id, &QCRIL_GRP_OEM_HOOK) ) ||
               ( qcril_is_event_in_group(param_ptr->request_id, &QCRIL_GRP_OEM ) ||
               ( param_ptr->request_id_android == RIL_REQUEST_OEM_HOOK_RAW ) ) )
          {
            is_oem_response = TRUE;
          }

          // oem hook qmi idl tunneling
          token_param                        = param_ptr->t;
          is_qmi_hook_idl_tunneling_response = FALSE;
          oem_hook_response_ctx              = NULL;
          prev_iter                          = NULL;

          pthread_mutex_lock( &qmi_ril_oem_hook_overview.overview_lock_mutex );
          oem_hook_response_ctx = qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root;

          while ( NULL != oem_hook_response_ctx && !is_qmi_hook_idl_tunneling_response )
          {
            QCRIL_LOG_DEBUG("buffer token = %d, received token = %d", oem_hook_response_ctx->original_token, token_param);
            if ( token_param == oem_hook_response_ctx->original_token )
            {  // match
              is_qmi_hook_idl_tunneling_response = TRUE;
              if ( NULL != prev_iter )
              {
                prev_iter->next = oem_hook_response_ctx->next;
              }
              else
              { // top
                qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root = oem_hook_response_ctx->next;
              }
              break;
            }
            else
            {
              prev_iter = oem_hook_response_ctx;
              oem_hook_response_ctx = oem_hook_response_ctx->next;
            }
          }

          pthread_mutex_unlock( &qmi_ril_oem_hook_overview.overview_lock_mutex );

          if ( is_qmi_hook_idl_tunneling_response )
          {
            QCRIL_LOG_DEBUG("qmi_idl_tunneling: responding to req_id = %d, srv_id = %d, msg_id = %d",
                                              oem_hook_response_ctx->ril_request_id,
                                              oem_hook_response_ctx->ril_idl_service_id,
                                              oem_hook_response_ctx->ril_idl_message_id );

            log_evt_name      = NULL;
            log_request_id    = QMI_RIL_ZERO;
            qmi_ril_oem_hook_get_request_id( oem_hook_response_ctx->ril_idl_service_id, oem_hook_response_ctx->ril_idl_message_id, &log_request_id, &log_evt_name );
            if ( NULL != log_evt_name )
            {
              QCRIL_LOG_DEBUG(".. responding to request %s", log_evt_name);
            }
            else
            {
              QCRIL_LOG_DEBUG(".. responding to request with unknown name");
            }

            do
            {
                successfully_substituted = FALSE;
                qmi_idl_tunneling_service_object = qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object( oem_hook_response_ctx->ril_idl_service_id );

                if ( NULL != qmi_idl_tunneling_service_object )
                {
                  idl_err = qmi_idl_get_max_message_len( qmi_idl_tunneling_service_object, QMI_IDL_RESPONSE, oem_hook_response_ctx->ril_idl_message_id, &substituted_data_len  );

                  if ( QMI_NO_ERR == idl_err )
                  {
                    substituted_data = qcril_malloc( substituted_data_len + OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE );

                    /* keep copy of orginal substituted data for freeing it properly */
                    orig_substituted_data = substituted_data;

                    if ( NULL != substituted_data )
                    {
                      encoded_fact = QMI_RIL_ZERO;

                      if ( param_ptr->resp_len > 0 )
                      {
                         idl_err = qmi_idl_message_encode( qmi_idl_tunneling_service_object,
                                                 QMI_IDL_RESPONSE,
                                                 oem_hook_response_ctx->ril_idl_message_id,
                                                 param_ptr->resp_pkt,
                                                 param_ptr->resp_len,
                                                 substituted_data + OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE,
                                                 substituted_data_len,
                                                 &encoded_fact );

                         if( idl_err == QMI_NO_ERR )
                         {
                            /* for VT service, skip the result part, as RIL<-->Telphony interface does not expect result field */
                            if( ( oem_hook_response_ctx->ril_idl_service_id == QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_VT ) ||
                                ( oem_hook_response_ctx->ril_idl_service_id == QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_PRESENCE ) )
                            {
                               substituted_data = substituted_data + OEM_HOOK_QMI_TUNNELING_RESULT_SIZE;
                               encoded_fact = encoded_fact - OEM_HOOK_QMI_TUNNELING_RESULT_SIZE;
                            }

                            QCRIL_LOG_DEBUG("TLV response message size = %d", encoded_fact);
                            qcril_qmi_print_hex(  (unsigned char*)substituted_data + OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE, encoded_fact);
                         }
                         else
                         {
                            QCRIL_LOG_ERROR( "qcril_send_request_response() QMI IDL - decode failed to decode buf with err %d, factual len %d ", (int) idl_err, (int) encoded_fact   );
                            break;
                         }
                      }
                      else
                      {
                         QCRIL_LOG_ERROR( "qcril_send_request_response() QMI IDL - skip decode due to error = %d in response or zero length =%d ", (int) param_ptr->ril_err_no, (int) param_ptr->resp_len   );
                         break;
                      }
                    }
                    else
                    {
                       QCRIL_LOG_ERROR( "qcril_send_request_response() QMI IDL - response decode failed to allocate substitute buf len %d", (int) substituted_data_len   );
                       break;
                    }
                  }
                  else
                  {
                    QCRIL_LOG_ERROR( "qcril_send_request_response() QMI IDL - response decode could not get length for message id %d, err code %d", (int) oem_hook_response_ctx->ril_idl_message_id, (int) idl_err   );
                    break;
                  }
                }
                else
                {
                  QCRIL_LOG_ERROR( "qcril_send_request_response() QMI IDL - response decode not found service object for service id error %d", (int) oem_hook_response_ctx->ril_idl_service_id );
                  break;
                }

                // fill up the tunneling header
                // request id
                if ( NULL != substituted_data )
                {
                  int32_param = (uint32_t*)substituted_data;
                  *int32_param = QCRIL_REQ_HOOK_REQ_GENERIC;


                  // response size
                  int32_param = (uint32_t*) (substituted_data + OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE);
                  *int32_param = encoded_fact + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE +
                                                OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE +
                                                OEM_HOOK_QMI_TUNNELING_ERROR_CODE_SIZE;

                  // service id
                  int16_param = (uint16_t*) ( substituted_data +
                                              OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE +
                                              OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE );
                  *int16_param = oem_hook_response_ctx->ril_idl_service_id;

                  // message id
                  int16_param = (uint16_t*) ( substituted_data +
                                              OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE +
                                              OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE +
                                              OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE );

                  *int16_param = oem_hook_response_ctx->ril_idl_message_id;

                  //error code
                  int16_param = (uint16_t*) ( substituted_data + OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE +
                                                                                       OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE +
                                                                                       OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE +
                                                                                       OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE );
                }

                /* If there was an internal error send internal error, else send the error cause received in the response request */
                if( idl_err == QMI_NO_ERR )
                {
                  // fill up the tunneling header
                  // request id
                  if ( NULL != substituted_data )
                  {
                    int32_param = (uint32_t*)substituted_data;
                    *int32_param = QCRIL_REQ_HOOK_REQ_GENERIC;


                    // response size
                    int32_param = (uint32_t*) (substituted_data + OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE);
                    *int32_param = encoded_fact + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE +
                                                  OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE +
                                                  OEM_HOOK_QMI_TUNNELING_ERROR_CODE_SIZE;

                    // service id
                    int16_param = (uint16_t*) ( substituted_data +
                                                OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE +
                                                OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE );
                    *int16_param = oem_hook_response_ctx->ril_idl_service_id;

                    // message id
                    int16_param = (uint16_t*) ( substituted_data +
                                                OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE +
                                                OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE +
                                                OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE );

                    *int16_param = oem_hook_response_ctx->ril_idl_message_id;

                    //error code
                    int16_param = (uint16_t*) ( substituted_data + OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE +
                                                                                         OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE +
                                                                                         OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE +
                                                                                         OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE );

                    /* If there was an internal error send internal error, else send the error cause received in the response request */
                    if( idl_err == QMI_NO_ERR )
                    {
                      *int16_param = param_ptr->ril_err_no;
                    }
                    else
                    {
                      *int16_param = idl_err;
                    }

                    // for oem hook tunneling requests, always send success, error code will actually indicate success or faliure
                    if( param_ptr->ril_err_no != RIL_E_SUCCESS )
                    {
                       QCRIL_LOG_DEBUG("error_cause received from qmi = %d", param_ptr->ril_err_no);
                       param_ptr->ril_err_no = RIL_E_SUCCESS;
                    }
                  }

                  // finally
                  successfully_substituted = TRUE;
                }

            } while ( FALSE );

            if ( successfully_substituted  )
            {
              actual_resp_pkt = substituted_data;
              QCRIL_LOG_DEBUG("allocated memory size = %d, final msg size = %d", substituted_data_len, encoded_fact + OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE);

              actual_resp_len = encoded_fact + OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE;
              QCRIL_LOG_DEBUG( "qcril_send_request_response() QMI IDL - substitted payload with qmi encoded stream len %d", (int) actual_resp_len   );

              qcril_qmi_print_hex( actual_resp_pkt, actual_resp_len);
            }
            else
            {
              is_qmi_hook_idl_tunneling_response = FALSE;
            }
          }


          // do respond
          instance_id = param_ptr->instance_id;

          if (!qmi_ril_is_multi_sim_oem_hook_request(param_ptr->request_id) &&
              qmi_ril_is_feature_supported(QMI_RIL_FEATURE_OEM_SOCKET) &&
              is_oem_response && !param_ptr->rild_sock_oem_req)
          {
            qcril_qmi_oemhook_agent_send(instance_id,
                                      param_ptr->t,
                                      qcril_qmi_oem_get_oemhook_msg(param_ptr->request_id),
                                      param_ptr->ril_err_no,
                                      actual_resp_pkt,
                                      actual_resp_len);
          }
          else
          {
              // Log the event packet for the response to RIL request
              if ( param_ptr->logstr != NULL )
              {
                QCRIL_SNPRINTF( label, sizeof( label ), "%s - %s, RID %d, Token id %" PRId32 ", %s",
                                qcril_log_lookup_event_name( param_ptr->request_id ),
                                param_ptr->logstr, param_ptr->instance_id,
                                qcril_log_get_token_id( param_ptr->t ),
                                qcril_log_ril_errno_to_str(param_ptr->ril_err_no) );
              }
              else
              {
                QCRIL_SNPRINTF( label, sizeof( label ), "%s - RID %d, Token id %" PRId32 ", %s",
                                qcril_log_lookup_event_name( param_ptr->request_id ),
                                param_ptr->instance_id,
                                qcril_log_get_token_id( param_ptr->t ),
                                qcril_log_ril_errno_to_str(param_ptr->ril_err_no) );
              }

              QCRIL_LOG_CF_PKT_RIL_RES( instance_id, label );

              // Send response to the RIL request
              QCRIL_LOG_DEBUG( "UI <--- %s (%p) Complete --- RIL [RID %d, Token id %" PRId32 ", %s, Len %d %s]\n",
                             qcril_log_lookup_event_name( param_ptr->request_id ),
                             param_ptr->request_id,
                             param_ptr->instance_id,
                             qcril_log_get_token_id( param_ptr->t ),
                             qcril_log_ril_errno_to_str(param_ptr->ril_err_no),
                             actual_resp_len,
                             ( param_ptr->logstr == NULL )? "" : param_ptr->logstr );

              qmi_ril_fw_send_request_response_epilog(
                                                      instance_id,
                                                      param_ptr->t,
                                                      android_request_id_for_response,
                                                      param_ptr->ril_err_no,
                                                      actual_resp_pkt,
                                                      actual_resp_len,
                                                      FALSE,
                                                      param_ptr
                                                      );
        }
    }
    else
    {
      QCRIL_LOG_FATAL("%s","FATAL : CHECK FAILED");
    }

} // qcril_send_request_response

//===========================================================================
// qmi_ril_util_str_clone
//===========================================================================
char* qmi_ril_util_str_clone( char * org_str )
{
  char* res;
  int   len;

  if ( NULL != org_str )
  {
    len = strlen( org_str );
    res = qcril_malloc( len + 1 );
    if ( NULL != res )
    {
      memcpy( res, org_str, len + 1 );
    }
  }
  else
  {
    res = NULL;
  }

  return res;
} // qmi_ril_util_str_clone


//===========================================================================
// qmi_ril_is_multi_sim_oem_hook_request
//===========================================================================
/*!
    @brief
    Checks if it is a multi sim OEM_HOOK request that
    is supposed to be responded on rild socket.

    @return
    TRUE, if multi sim OEM_HOOK request.
    FALSE, otherwise.
*/
/*=========================================================================*/
int qmi_ril_is_multi_sim_oem_hook_request (qcril_evt_e_type req_res_id)
{
  int res = FALSE;
  if (req_res_id == QCRIL_EVT_HOOK_UNSOL_VOICE_SYSTEM_ID ||
          req_res_id == QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY ||
          req_res_id == QCRIL_EVT_HOOK_SET_DEFAULT_VOICE_SUB ||
          req_res_id == QCRIL_EVT_HOOK_SET_LOCAL_CALL_HOLD ||
          req_res_id == QCRIL_EVT_HOOK_GET_MODEM_CAPABILITY ||
          req_res_id == QCRIL_EVT_HOOK_UPDATE_SUB_BINDING)
  {
      res = TRUE;
  }
  else
  {
      res = FALSE;
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
} // qmi_ril_is_multi_sim_oem_hook_request


//===========================================================================
//qmi_ril_enter_critical_section
//===========================================================================
void qmi_ril_enter_critical_section(void)
{
  pthread_mutex_lock( &qmi_ril_common_critical_section.lock_mutex );
} // qmi_ril_enter_critical_section
//===========================================================================
//qmi_ril_leave_critical_section
//===========================================================================
void qmi_ril_leave_critical_section(void)
{
  pthread_mutex_unlock( &qmi_ril_common_critical_section.lock_mutex );
} // qmi_ril_leave_critical_section

//===========================================================================
//qmi_ril_get_operational_status
//===========================================================================
qmi_ril_gen_operational_status_type qmi_ril_get_operational_status(void)
{
  qmi_ril_gen_operational_status_type res;

  qmi_ril_enter_critical_section();
  res = qmi_ril_gen_operational_status;
  qmi_ril_leave_critical_section();

  return res;
} //qmi_ril_get_operational_status

//===========================================================================
//qmi_ril_set_operational_status
//===========================================================================
void qmi_ril_set_operational_status( qmi_ril_gen_operational_status_type new_status )
{
  QCRIL_LOG_INFO( "new_status %d", new_status );

  qmi_ril_enter_critical_section();
  qmi_ril_gen_operational_status = new_status;
  qmi_ril_leave_critical_section();
} // qmi_ril_set_operational_status

//===========================================================================
// qmi_ril_is_feature_supported
//===========================================================================
int qmi_ril_is_feature_supported(int feature)
{
  int res = FALSE;
  char prop_str[ PROPERTY_VALUE_MAX ];

  switch ( feature )
  {
    case QMI_RIL_FEATURE_FUSION_CSFB:
    case QMI_RIL_FEATURE_APQ:
    case QMI_RIL_FEATURE_MSM:
    case QMI_RIL_FEATURE_SVLTE2:
    case QMI_RIL_FEATURE_SGLTE:
    case QMI_RIL_FEATURE_DSDA:
    case QMI_RIL_FEATURE_DSDA2:
      if ( QMI_RIL_FTR_BASEBAND_UNKNOWN ==  qmi_ril_baseband_ftr_info)
      {
        *prop_str = 0;

        qcril_get_baseband_name(prop_str);

        if ( strcmp(prop_str, "apq") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_APQ;
        }
        else if ( strcmp(prop_str, "mdm") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_APQ;
        }
        else if ( strcmp(prop_str, "mdm2") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_MDM2;
        }
        else if ( strcmp(prop_str, "msm") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_MSM;
        }
        else if ( strcmp(prop_str, "csfb") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_CSFB;
        }
        else if ( strcmp(prop_str, "svlte2a") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_SVLTE2;
        }
        else if ( strcmp(prop_str, "sglte2") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_SGLTE;
        }
        else if ( strcmp(prop_str, "sglte") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_SGLTE;
        }
        else if ( strcmp(prop_str, "dsda") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_DSDA;
        }
        else if ( strcmp(prop_str, "dsda2") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_DSDA2;
        }
        else if ( strcmp(prop_str, "auto") == 0)
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_APQ;
        }
        else
        {
          qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_NONE;
        }
      }

      if ( QMI_RIL_FEATURE_FUSION_CSFB == feature && QMI_RIL_FTR_BASEBAND_CSFB == qmi_ril_baseband_ftr_info)
      {
        res = TRUE;
      }
      else if ( QMI_RIL_FEATURE_APQ == feature && QMI_RIL_FTR_BASEBAND_APQ == qmi_ril_baseband_ftr_info)
      {
        res = TRUE;
      }
      else if ( QMI_RIL_FEATURE_APQ == feature && QMI_RIL_FTR_BASEBAND_MDM2 == qmi_ril_baseband_ftr_info)
      {
        res = TRUE;
      }
      else if ( QMI_RIL_FEATURE_MSM == feature && QMI_RIL_FTR_BASEBAND_MSM == qmi_ril_baseband_ftr_info)
      {
        res = TRUE;
      }
      else if ( QMI_RIL_FEATURE_SVLTE2 == feature && QMI_RIL_FTR_BASEBAND_SVLTE2 == qmi_ril_baseband_ftr_info)
      {
        res = TRUE;
      }
      else if ( QMI_RIL_FEATURE_SGLTE == feature && QMI_RIL_FTR_BASEBAND_SGLTE == qmi_ril_baseband_ftr_info)
      {
        res = TRUE;
      }
      else if ( QMI_RIL_FEATURE_DSDA == feature && QMI_RIL_FTR_BASEBAND_DSDA == qmi_ril_baseband_ftr_info)
      {
        res = TRUE;
      }
      else if ( QMI_RIL_FEATURE_DSDA2 == feature && QMI_RIL_FTR_BASEBAND_DSDA2 == qmi_ril_baseband_ftr_info)
      {
        res = TRUE;
      }
      else
      {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_SGLTE2:
      if ( QMI_RIL_FTR_BASEBAND_UNKNOWN == qmi_ril_sglte_ftr_info)
      {
        *prop_str = 0;
        property_get( QMI_RIL_SYS_PROP_NAME_BASEBAND, prop_str, "" );
        if ( strcmp(prop_str, "sglte2") == 0)
        {
          qmi_ril_sglte_ftr_info = QMI_RIL_FTR_BASEBAND_SGLTE2;
        }
      }

      switch ( qmi_ril_sglte_ftr_info )
      {
        case QMI_RIL_FTR_BASEBAND_SGLTE2:
          res = TRUE;
          break;

        default:
          res = FALSE;
          break;
      }

      break;
    case QMI_RIL_FEATURE_SGLTE_CSFB:
      if ( QMI_RIL_FTR_SGLTE_CSFB_UNKNOWN == qmi_ril_sglte_csfb_ftr_info )
      {
        *prop_str = 0;
        property_get( QMI_RIL_SYS_PROP_NAME_SGLTE_CSFB, prop_str, "" );
        if ( strncmp(prop_str, "true", QMI_RIL_SYS_PROP_SGLTE_CSFB_LENGTH ) == 0)
        {
          qmi_ril_sglte_csfb_ftr_info = QMI_RIL_FTR_SGLTE_CSFB_YES;
        }
        else
        {
          qmi_ril_sglte_csfb_ftr_info = QMI_RIL_FTR_SGLTE_CSFB_NO;
        }
      }

      switch ( qmi_ril_sglte_csfb_ftr_info )
      {
        case QMI_RIL_FTR_SGLTE_CSFB_YES:
          res = TRUE;
          break;

        default:
          res = FALSE;
          break;
      }

      break;

    case QMI_RIL_FEATURE_DSDS:
    case QMI_RIL_FEATURE_TSTS:
      if ( QMI_RIL_FTR_MULTI_SIM_UNKNOWN == qmi_ril_multi_sim_ftr_info )
      {
        *prop_str = 0;
        property_get( QMI_RIL_SYS_PROP_NAME_MULTI_SIM, prop_str, "" );
        if ( strncmp(prop_str, "dsds", QMI_RIL_SYS_PROP_LENGTH_MULTI_SIM ) == 0)
        {
          qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_DSDS_ENABLED;
        }
        else if ( strncmp(prop_str, "tsts", QMI_RIL_SYS_PROP_LENGTH_MULTI_SIM ) == 0)
        {
          qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_TSTS_ENABLED;
        }
        else if ( strncmp(prop_str, "dsda", QMI_RIL_SYS_PROP_LENGTH_MULTI_SIM ) == 0)
        {
          qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_DSDA_ENABLED;
        }
        else
        {
          qmi_ril_multi_sim_ftr_info = QMI_RIL_FTR_MULTI_SIM_DISABLED;
        }
      }

      if ( (feature == QMI_RIL_FEATURE_DSDS) &&
           ( ( qmi_ril_multi_sim_ftr_info == QMI_RIL_FTR_DSDS_ENABLED) ||
             ( qmi_ril_multi_sim_ftr_info == QMI_RIL_FTR_DSDA_ENABLED) ) )
      {
          // DSDS/DSDA is enabled
          res = TRUE;
      }
      else if ( (feature == QMI_RIL_FEATURE_TSTS) &&
                (qmi_ril_multi_sim_ftr_info == QMI_RIL_FTR_TSTS_ENABLED) )
      {
          // TSTS is enabled
          res = TRUE;
      }
      else
      {

          res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_LEGACY_RAT:
    case QMI_RIL_FEATURE_COMBINE_RAT:
      if ( QMI_RIL_FTR_RAT_UNKNOWN == qmi_ril_rat_enable_option)
      {
        *prop_str = 0;
        property_get( QMI_RIL_SYS_PROP_RAT_OPTION, prop_str, "" );
        if ( !strncmp(prop_str, "legacy", 6) )
        {
          qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_LEGACY;
        }
        else if ( !strncmp(prop_str, "combine", 7) )
        {
          qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_COMBINE;
        }
        else
        {
          qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_DISBLE;
        }
      }

      if ( (feature == QMI_RIL_FEATURE_LEGACY_RAT) &&
            (qmi_ril_rat_enable_option == QMI_RIL_FTR_RAT_LEGACY) )
      {
        res = TRUE;
      }
      else if ( (feature == QMI_RIL_FEATURE_COMBINE_RAT) &&
            (qmi_ril_rat_enable_option == QMI_RIL_FTR_RAT_COMBINE) )
      {
        res = TRUE;
      }
      else
      {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_KDDI_HOLD_ANSWER:
      res = FALSE;
      char prop[ PROPERTY_VALUE_MAX ];

      if ( QMI_RIL_KDDI_HOLD_ANSWER_UNKNOWN == qmi_ril_kddi_hold_answer)
      {
        property_get( QCRIL_FEATURE_KDDI_HOLD_ANSWER_ON, prop, "" );
        if ((strlen(prop) > 0) && atoi(prop))
        {
          qmi_ril_kddi_hold_answer = QMI_RIL_KDDI_HOLD_ANSWER_ENABLED;
        }
        else
        {
          qmi_ril_kddi_hold_answer = QMI_RIL_KDDI_HOLD_ANSWER_DISABLED;
        }
      }

      switch ( qmi_ril_kddi_hold_answer )
      {
        case QMI_RIL_KDDI_HOLD_ANSWER_ENABLED:
          res = TRUE;
          break;

        case QMI_RIL_KDDI_HOLD_ANSWER_DISABLED:
        default:
          res = FALSE;
          break;
      }
      break;

    case QMI_RIL_FEATURE_SHM:
#ifdef FEATURE_QCRIL_SHM
      res = TRUE;
#else
      res = FALSE;
#endif
      break;

    case QMI_RIL_FEATURE_IMS:
    /* SMS over IMS is supported only if FEATURE_QCRIL_IMS is defined */
#ifdef FEATURE_QCRIL_IMS
      res = TRUE;
#else
      res = FALSE;
#endif
    break;

    case QMI_RIL_FEATURE_EMBMS:
#ifdef FEATURE_DATA_EMBMS
      res = TRUE;
#else
      res = FALSE;
#endif
    break;

    case QMI_RIL_FEATURE_POWER_ON_OPTIMIZATION:
    /* RIL controlled to put MODEM ONLINE
     * only if QMI_RIL_FEATURE_POWER_ON_OPTIMIZATION
     * is defined */
    res = qcril_qmi_nas_is_feature_power_opt_enable();
    break;

    case QMI_RIL_FEATURE_VOIP_VT:
     /* only if modify call initiate is defined */
    #if !defined(QMI_RIL__RIL_VT_VOLTE_EMULATED)
    res = TRUE;
    #endif
    break;

    case QMI_RIL_FEATURE_IMS_RETRY_3GPP:
      // feature is enabled (true) by default, eg. if property set to anything
      // other than "false" or "0", including if its missing (unset).
      *prop_str = 0;
      property_get(QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP, prop_str, "");
      res = ( (strcmp(prop_str, "false") != 0) && (strcmp(prop_str, "0") != 0) );
      QCRIL_LOG_DEBUG("property %s = \"%s\", %d",
                       QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP, prop_str, res);
      break;

    case QMI_RIL_FEATURE_IMS_RETRY_3GPP2:
      // feature is enabled (true) by default, eg. if property set to anything
      // other than "false" or "0", including if its missing (unset).
      *prop_str = 0;
      property_get(QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP2, prop_str, "");
      res = ( (strcmp(prop_str, "false") != 0) && (strcmp(prop_str, "0") != 0) );
      QCRIL_LOG_DEBUG("property %s = \"%s\", %d",
                       QMI_RIL_SYS_PROP_NAME_IMS_RETRY_ON_3GPP2, prop_str, res);
      break;

    case QMI_RIL_FEATURE_PLAIN_ANDROID:
#if defined(QMI_RIL_UNDER_PLAIN_ANDROID)
    res = TRUE;
#else
    res = FALSE;
#endif
      break;

    case QMI_RIL_FEATURE_OEM_IND_TO_BOTH:
      // feature is enabled (true) by default, eg. if property set to anything
      // other than "false" or "0", including if its missing (unset).
      if (QMI_RIL_FEATURE_UNKNOWN == qmi_ril_oem_ind_to_both)
      {
        *prop_str = 0;
        property_get(QMI_RIL_SYS_PROP_NAME_OEM_IND_TO_BOTH, prop_str, "");
        if ( (strcmp(prop_str, "false") != 0) && (strcmp(prop_str, "0") != 0) )
        {
          qmi_ril_oem_ind_to_both = QMI_RIL_FEATURE_ENABLED;
        }
        else
        {
          qmi_ril_oem_ind_to_both = QMI_RIL_FEATURE_DISABLED;
        }
      }

      if (QMI_RIL_FEATURE_ENABLED == qmi_ril_oem_ind_to_both)
      {
        res = TRUE;
      }
      else
      {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_OEM_SOCKET:
      // feature is enabled (true) by default, eg. if property set to anything
      // other than "false" or "0", including if its missing (unset).
      *prop_str = 0;
      property_get(QMI_RIL_SYS_PROP_NAME_OEM_SOCKET, prop_str, "");
      res = ( (strcmp(prop_str, "false") != 0) && (strcmp(prop_str, "0") != 0) );
      break;

    case QMI_RIL_FEATURE_POSIX_CLOCKS:
#ifdef HAVE_POSIX_CLOCKS
      res = TRUE;
#else
      res = FALSE;
#endif
    break;

    case QMI_RIL_FEATURE_SAP_SILENT_PIN_VERIFY:
      /* feature is disabled by default, eg. if property set to anything
         other than "true" or "1", including if its missing (unset). */
      *prop_str = 0;
      property_get(QCRIL_FEATURE_SAP_SILENT_PIN_VERIFY, prop_str, "");
      res = ( (strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0) );
      break;

    case QMI_RIL_FEATURE_BLOCK_ALLOW_DATA:
      *prop_str = 0;
      property_get(QCRIL_PROCESS_BLOCK_ALLOW_DATA, prop_str, "");
      if( ( strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0) )
      {
        res = TRUE;
      }
      else
      {
        res = FALSE;
      }
      break;

    case QMI_RIL_FEATURE_SRLTE:
      res = qcril_qmi_is_srlte_supported();
      break;

    case QMI_RIL_FEATURE_ATEL_STKCC:
#if RIL_UNSOL_ON_SS == 11038
      res = FALSE;
#else
      res = TRUE;
#endif
      break;

    case QMI_RIL_FEATURE_SUPPRESS_REQ:
      /* feature is disabled by default, eg. if property set to anything
         other than "true" or "1", including if its missing (unset). */
      if (QMI_RIL_FEATURE_UNKNOWN == qmi_ril_ftr_suppress_req)
      {
        *prop_str = 0;
        property_get(QMI_RIL_SYS_PROP_NAME_SUPPRESS_REQ, prop_str, "");
        if ((strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0))
        {
          qmi_ril_ftr_suppress_req = QMI_RIL_FEATURE_ENABLED;
        }
        else
        {
          qmi_ril_ftr_suppress_req = QMI_RIL_FEATURE_DISABLED;
        }
      }

      switch ( qmi_ril_ftr_suppress_req )
      {
        case QMI_RIL_FEATURE_ENABLED:
          res = TRUE;
          break;

        default:
          res = FALSE;
          break;
      }
      break;

      case QMI_RIL_FEATURE_RIL_DATA_REG_STATE_CONDITIONAL_REPORT:

     /* RIL to report DATA_REG_STATE not only looking for
     preffered technology reported through DSD. It should
     look for RAT on which IMS registerred*/
      *prop_str = 0;
      property_get(QMI_RIL_SYS_PROP_RIL_DATA_REG_STATE_CONDITIONAL_REPORT, prop_str, "");
      res = ( (strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0) );
      break;

    default:
      res = FALSE;
      break;
  }

  return res;
} // qmi_ril_is_feature_supported

/*===========================================================================

  FUNCTION:  qcril_get_baseband_name

===========================================================================*/
/*!
    @brief
    retrieve baseband info

    @return
    None
*/
/*=========================================================================*/
void qcril_get_baseband_name(char *prop_str)
{
    char *link_name = NULL;
    if (prop_str)
    {
        property_get( QMI_RIL_SYS_PROP_NAME_BASEBAND, prop_str, "" );

        if (!strcmp("mdm", prop_str) || !strcmp("mdm2", prop_str))
        {
            link_name = qcril_qmi_get_esoc_link_name();
            if (link_name)
            {
                QCRIL_LOG_INFO("link name %s ", link_name);
                if (!strcmp("HSIC", link_name))
                {
                    strlcpy(prop_str, "mdm", PROPERTY_VALUE_MAX);
                }
                else
                {
                    strlcpy(prop_str, "mdm2", PROPERTY_VALUE_MAX);
                }
            }
        }
    }

    return;
}

//===========================================================================
// qcril_qmi_mgr_modem_state_updated
//===========================================================================
void qcril_qmi_mgr_modem_state_updated(qcril_instance_id_e_type instance_id, qcril_modem_state_e_type new_modem_state)
{
  qcril_arb_state_info_struct_type *s_ptr;

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );

  qmi_ril_enter_critical_section();
  s_ptr = &qcril_state->info[ instance_id ];
  s_ptr->modem_state = new_modem_state;
  qmi_ril_leave_critical_section();

  QCRIL_LOG_FUNC_RETURN_WITH_RET(new_modem_state);
} // qcril_qmi_mgr_modem_state_updated

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_update_modem_stack_id

===========================================================================*/
void qcril_qmi_nas_update_modem_stack_id(uint8_t stack_id)
{
    QCRIL_LOG_FUNC_ENTRY();

    qmi_ril_enter_critical_section();
    modem_stack_id = stack_id;
    qmi_ril_leave_critical_section();

    qcril_qmi_pdc_set_sub_id(stack_id);
    QCRIL_LOG_DEBUG("[MSIM] Modem stack id %d", (int) stack_id);
}

void qcril_qmi_print_hex(void *msg, int msg_len) {
	QCRIL_NOTUSED(msg);
	QCRIL_NOTUSED(msg_len);
}

/*=========================================================================
  FUNCTION:  qcril_qmi_get_modem_stack_id

===========================================================================*/
qcril_modem_stack_id_e_type qcril_qmi_get_modem_stack_id()
{
    qcril_modem_stack_id_e_type stack_id;

    qmi_ril_enter_critical_section();
    stack_id = modem_stack_id;
    qmi_ril_leave_critical_section();

    QCRIL_LOG_FUNC_RETURN_WITH_RET(stack_id);
    return stack_id;
}

void qmi_ril_reset_baseband_rat_option(void)
{
  qmi_ril_rat_enable_option = QMI_RIL_FTR_RAT_UNKNOWN;
  qmi_ril_baseband_ftr_info = QMI_RIL_FTR_BASEBAND_UNKNOWN;
}
