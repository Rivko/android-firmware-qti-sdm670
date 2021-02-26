/*=============================================================================

                 err_qdi_pdr_records.c

GENERAL DESCRIPTION
   Implements QDI layer logging for User PD logs

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2015 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_qdi_pdr_records.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/05/15   din     File created.

=============================================================================*/


/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/
#include <stringl/stringl.h>
#include "err_qdi.h"
#include "err.h"
#include "queue.h"
#include "timer.h"
#include "stdlib.h"

/*****************************************************************************/
/*                         MACRO DECLARATIONS                                */
/*****************************************************************************/

/* Logging Buffer for Each PD */
#define ERR_QDI_PDR_RECORDS_LENGTH_DEFAULT  10

/* Max PDR thrashing - 5 secs for 10 PDRs */
#define MAX_PDR_THRASHING_TIMEOUT_DEFAULT  ( 5 )

/* Assume 19.2 MHz freq for Daltimetick clock */
#define DAL_TIMETICK_19_2_MHZ_FREQ_FACTOR   ( 19.2 * 1000 * 1000)   

/*****************************************************************************/
/*                         DATA DECLARATIONS                                 */
/*****************************************************************************/
extern typeof(unsigned int) const qurtos_max_user_processes;

/* MACRO for locking/unlocking the qmutex*/
#define ERR_QDI_PDR_RECORDS_MUTEX_LOCK(mutex)  \
   qurt_pimutex_lock(&mutex); 
  
#define ERR_QDI_PDR_RECORDS_MUTEX_FREE(mutex)  \
   qurt_pimutex_unlock(&mutex); 

#define ERR_QDI_PDR_RECORDS_MUTEX_INIT(mutex) \
   qurt_pimutex_init(&mutex);


/* Logs are stored as queue of queues. err_qdi_data_log_type.queue_per_pd_log 
 * represents queue of queues. Each queue will have a entry for another queue 
 * determined by queue_single_event.
 *
   +-----------------+     +------------------+     +-----------------+
   |queue_per_pd_log |---->|Link              |---->|Link             |---->NULL
   +-----------------+     +------------------+     +-----------------+
                           |queue_single_event|     |queue_single_event|
                           +-----------------+      +-----------------+
                                    |                        |
                                    |                        |
                           +-----------------+      +-----------------+
                           |single_event     |      |single_event     |      
                           +-----------------+      +-----------------+
                                    |                        |
                                    |                        |
                           +-----------------+      +-----------------+
                           |single_event     |      |single_event     |
                           +-----------------+      +-----------------+ 
                                    |                        |
                                    |                        |
                                   NULL                     NULL
			    

 *  */

/* A single ERR qdi log */
typedef struct
{
  /* q_link_type - Must be the first element */
  q_link_type             node_single_event;
  char                    pfr[ERR_QDI_PFR_LEN];
  uint32                  pid;
  DalTimetickTime64Type   start_time;
  DalTimetickTime64Type   end_time;
  DalTimetickTime64Type   pd_exit_end_time;
} err_qdi_pdr_event_s;

typedef struct
{
  /* q_link_type - Must be the first element */
  q_link_type            node_per_pd_log;
  q_type                 queue_single_event;
  char                   pd_name[ERR_QDI_DEVICE_NAME_LEN];
  uint32                 num_pdr_crash;
} err_qdi_pdr_user_pd_header_s;


typedef struct 
{
  q_type                  queue_per_pd_log;
  qurt_mutex_t            mutex;
  uint64                  threshold_timeout;
  uint32                  num_pdr_records;
  boolean                 initialized;
}err_qdi_pdr_records_data_s;

static err_qdi_pdr_records_data_s  err_qdi_pdr_records;
static DalDeviceHandle*            phTimetickHandle ;


extern void err_qdi_dalcfg_get_err_qdi_pdr_records
( 
  uint32 * threshold_timeout_sec,
  uint32 * num_pdr_records  
);

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION ERR_QDI_PDR_RECORDS_INIT

DESCRIPTION
  Initiliazes Error QDI PDR records.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void err_qdi_pdr_records_init(void )
{
  /* Initialize mutex */
  ERR_QDI_PDR_RECORDS_MUTEX_INIT(err_qdi_pdr_records.mutex);

  ERR_QDI_PDR_RECORDS_MUTEX_LOCK(err_qdi_pdr_records.mutex);
  
  err_qdi_dalcfg_get_err_qdi_pdr_records(
                         (uint32 *) &(err_qdi_pdr_records.threshold_timeout), 
		          &err_qdi_pdr_records.num_pdr_records);
  
  if ( err_qdi_pdr_records.threshold_timeout == 0 )
  {
    err_qdi_pdr_records.threshold_timeout = MAX_PDR_THRASHING_TIMEOUT_DEFAULT;
  }

  if ( err_qdi_pdr_records.num_pdr_records == 0 )
  {
    err_qdi_pdr_records.num_pdr_records = ERR_QDI_PDR_RECORDS_LENGTH_DEFAULT;
  }
  
 
  /* Init Timetick handle */
  DalTimetick_Attach("SystemTimer", &phTimetickHandle);

  if (NULL != phTimetickHandle)
  {
    DalTimetick_CvtToTimetick64( phTimetickHandle, 
                               err_qdi_pdr_records.threshold_timeout,
                               T_SEC,
                               &err_qdi_pdr_records.threshold_timeout );
  }
  else
  {
    err_qdi_pdr_records.threshold_timeout *= DAL_TIMETICK_19_2_MHZ_FREQ_FACTOR;
  }

  /* Initialize List */
  q_init(&(err_qdi_pdr_records.queue_per_pd_log));
  
  err_qdi_pdr_records.initialized = TRUE;

  ERR_QDI_PDR_RECORDS_MUTEX_FREE(err_qdi_pdr_records.mutex);

} /* err_qdi_pdr_records_init */

/*===========================================================================

FUNCTION ERR_QDI_PDR_RECORDS_COMPARATOR_FUNC

DESCRIPTION
  Used by queue logic to find out the node with a given value.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if successful
  FALSE if unsuccessful

SIDE EFFECTS
  None

===========================================================================*/
static int err_qdi_pdr_records_comparator_func
(
  void * link,
  void * compare_val
)
{
  err_qdi_pdr_user_pd_header_s * temp_node = (err_qdi_pdr_user_pd_header_s*)link;
  
  if ( temp_node == NULL || compare_val == NULL )
    return 0;

  if ( strncmp((char *)compare_val,temp_node->pd_name, 
                         ERR_QDI_DEVICE_NAME_LEN ) == 0 )
  {
    return 1;
  }

  return 0;

} /* err_qdi_pdr_records_comparator_func */


/*=============================================================================

FUNCTION ERR_QDI_PDR_RECORDS_USER_INFO

DESCRIPTION
  This function saves a error log event packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void * err_qdi_pdr_records_user_err_info
(
  err_qdi_internal_s * dataobj
)
{
  err_qdi_pdr_user_pd_header_s  * temp_events_per_pd;
  err_qdi_pdr_event_s           * temp_events, *temp_events_last;
  char temp_pd_name[ERR_QDI_DEVICE_NAME_LEN];
	   
  if (err_qdi_pdr_records.initialized != TRUE )
    err_qdi_pdr_records_init();

  ERR_QDI_PDR_RECORDS_MUTEX_LOCK(err_qdi_pdr_records.mutex);

  strlcpy( temp_pd_name, dataobj->pd_name, 
                              ERR_QDI_DEVICE_NAME_LEN);
 
  /* Search for the item in the queue */
  temp_events_per_pd = (err_qdi_pdr_user_pd_header_s *) q_linear_search(
		  &(err_qdi_pdr_records.queue_per_pd_log),
		  err_qdi_pdr_records_comparator_func,
		  temp_pd_name);

  if ( temp_events_per_pd == NULL )
  {
    temp_events_per_pd = calloc ( 1, sizeof(err_qdi_pdr_user_pd_header_s));

    if ( temp_events_per_pd == NULL )
    {
      /* Logging failed */
      ERR_QDI_PDR_RECORDS_MUTEX_FREE(err_qdi_pdr_records.mutex);
      return NULL ;
    }

    strlcpy( temp_events_per_pd->pd_name, temp_pd_name, 
                              ERR_QDI_DEVICE_NAME_LEN);
    q_init(&(temp_events_per_pd->queue_single_event));

    temp_events_per_pd->num_pdr_crash = 1;

    /* Insert in the queue */
    (void) q_link( temp_events_per_pd, &(temp_events_per_pd->node_per_pd_log) );
    q_put(&(err_qdi_pdr_records.queue_per_pd_log), 
		    &(temp_events_per_pd->node_per_pd_log) );
  }
  else
  {
    temp_events_per_pd->num_pdr_crash++;
  }
  
  /* Check if queue has reached its max size */
  if ( q_cnt(&(temp_events_per_pd->queue_single_event)) == 
		                  err_qdi_pdr_records.num_pdr_records ) 
  {
    /* Pop the first item on the queue, and reuse it. temp_events cannot be NULL */
    temp_events = q_get(&(temp_events_per_pd->queue_single_event));
  }
  /* List hasn't reached MAX size allowed, dynamically allocate a new entry */
  else
  {
    temp_events = calloc ( 1, sizeof(err_qdi_pdr_event_s));
  }
  if ( temp_events == NULL )
  {
    /* Logging failed */
    ERR_QDI_PDR_RECORDS_MUTEX_FREE(err_qdi_pdr_records.mutex);
    return NULL;
  }

  /* Populate event */
  temp_events->pid              = dataobj->pid;
  temp_events->start_time       = dataobj->start_time;
  temp_events->end_time         = dataobj->end_time;
  temp_events->pd_exit_end_time = 0;
  strlcpy(temp_events->pfr, dataobj->pfr, ERR_QDI_PFR_LEN);

  /* Insert in the queue */
    (void) q_link( temp_events, &(temp_events->node_single_event) );
    q_put((&(temp_events_per_pd->queue_single_event)), 
		   (&(temp_events->node_single_event)) );

  /* Thrashing logic */
  /* Check again if queue has reached its max size */
  if ( q_cnt(&(temp_events_per_pd->queue_single_event)) == 
		                  err_qdi_pdr_records.num_pdr_records )
  {
 	  
    /* Peek the first item on the queue, temp_events_last cannot be NULL */
    temp_events_last = q_check(&(temp_events_per_pd->queue_single_event));
    
    if ( temp_events_last !=NULL /* keep KW Happy */ && 
	 temp_events->end_time - temp_events_last->end_time < 
		                   err_qdi_pdr_records.threshold_timeout )
    {
      ERR_FATAL ( "User PDR thrashing. PID : %u, Num PDRs : %u, Timeout : %u",
                temp_events->pid, err_qdi_pdr_records.num_pdr_records,
                err_qdi_pdr_records.threshold_timeout );
    }
  }
  
  ERR_QDI_PDR_RECORDS_MUTEX_FREE(err_qdi_pdr_records.mutex);  

  return temp_events;
  
} /* err_qdi_pdr_records_user_err_info */

/*=============================================================================

FUNCTION ERR_QDI_PDR_UPDATE_PD_EXIT_END_TIME
DESCRIPTION
  This function updates pd exit end time to pdr record

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void err_qdi_pdr_update_pd_exit_end_time
(
  void * temp_events,
  DalTimetickTime64Type   pd_exit_end_time
)
{
  if ( temp_events == NULL )
    return;

  err_qdi_pdr_event_s * temp = (err_qdi_pdr_event_s *)temp_events;
  temp->pd_exit_end_time     = pd_exit_end_time;

} /* err_qdi_pdr_update_pd_exit_end_time */
