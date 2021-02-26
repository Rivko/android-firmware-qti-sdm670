/**
 * @mainpage QDSS Tracer API
 * @par QUALCOMM debug subsystem (QDSS) tracer API
 * @par Description:
 * The tracer library provides the interface for software
 * clients to output trace messages and data. It also provices
 * the host/PC tools with access to simple tracer controls.
 *
 * The primary external tracer APIs are in the file tracer.h.
 */
/*===========================================================================
  FILE: tracer.c

  OVERVIEW:     QUALCOMM Debug Subsystem (QDSS) Tracer

  DEPENDENCIES: Qdss hardware initialized (STM, Funnel, and ETB devices).

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer.c#1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-08-18 lht Initial revision.
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stdio.h>      // snprintf, vsnprintf
#include <stdarg.h>     // va_...

#include "comdef.h"

#include "DALStdErr.h"

#include "tracer_malloc.h"
#include "tracer_mutex.h"
#include "tracer_osal.h"
#include "tracer_list.h"
#include "tracer_portmgr.h"

#include "tracer.h"
#include "tracer_event_ids.h"
#include "tracer_config_int.h"

#include "tracer_stp.h"
#include "tracer_ost.h"
#include "tracer_tds.h"

// >>>>>>>>>>>>>>>>>>>>>>>>>>
// The following section are for sw devt/debugging only.
#ifndef TRACER_MSGERR_ON
#define TRACER_MSGERR_ON 0    // Error messages
#endif
#ifndef TRACER_MSGINFO_ON
#define TRACER_MSGINFO_ON 0   // Informational messages
#endif
#ifndef TRACER_TESTING_ON
#define TRACER_TESTING_ON 0   // Development testing
#endif

//<<<<<<<<<<<<<<<<<<

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
//=========================================================================//
/**
  @brief Structure containing client specific information.
 */
struct tracer_client_data_s {

   /** The assigned STM channel (or port) for client's trace
    *  stream.
    */
   STMTracePortType stm_port;

   /** Identifies the target-side entity that provides data to the
    *  OST base protocol layer. Client specified the entity it is
    *  to be associated with. Conveyed with trace data in the OST
    *  base protocol header.
    */
   tracer_ost_entity_id_enum_t entity_id;

   /** Client specified identifier of the payload's protocol within
    *  the given entity. Conveyed with trace data in the OST base
    *  protocol header.
    */
   tracer_ost_protocol_id_t protocol_id;

   /** Identifies the CPU on which the trace data originated. */
   tracer_cpu_id_t cpu_id;

   /** Identifies the process in which the trace data originated. */
   tracer_process_id_t process_id;

   /** Identifies the thread which the generated the trace data. */
   tracer_thread_id_t thread_id;

};

//=========================================================================//
/**
  @brief Handle (pointer to) to client data.
 */
typedef struct tracer_client_data_s *tracer_client_data_handle_t;

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
/** List of handles for registered clients. */
tracer_list_t tracer_client_list;

#if (TRACER_MSGERR_ON || TRACER_MSGINFO_ON || TRACER_TESTING_ON)
/** Handle for tracer itself as a client.
 */
static tracer_client_handle_t client_handle_self;
#endif

/** Used to set client's handle; circumvent compiler
optimization of not setting the reference to a aliased pointer
value.
 */
static tracer_client_handle_t volatile * volatile phVClient;

//=========================================================================//
// Helper macro to dereference client data parameters.
#define CLIENT_DATA(_hC, _val) \
    (((tracer_client_data_handle_t)((_hC)->pData))->_val)

/*-------------------------------------------------------------------------*/
// Translate from tracer options to STM options
#define STM_DOPT(topt) (                                  \
      ( ((topt) & TRACER_OPT_TIMESTAMP) ?                 \
         TRACE_DATA_TIMESTAMPED : TRACE_DATA_NONETYPE ) | \
      ( ((topt) & TRACER_OPT_GUARANTEE) ?                 \
         TRACE_DATA_GUARANTEED : TRACE_DATA_NONETYPE ) )

/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * Internalized Function Definitions
 * ------------------------------------------------------------------------*/
void tracer_init_client_list(void)
{
   tracer_list_init(&(tracer_client_list));
   return;
}
//=========================================================================//
// Tracer internal helper functions.
//=========================================================================//

/*-------------------------------------------------------------------------*/
tracer_return_enum_t _tracer_add_client(
   tracer_client_handle_t *phClient,
   tracer_ost_entity_id_enum_t eid,
   tracer_ost_protocol_id_t pid
)
{
   tracer_client_handle_t hClient;

   hClient = (tracer_client_handle_t) tracer_list_new_node(
      &(tracer_client_list),
      sizeof(struct tracer_client_data_s));

   (*phClient) = hClient;

   if (NULL == hClient)
   {
#if TRACER_MSGERR_ON
      if (NULL != client_handle_self)
      {
         tracer_data(client_handle_self, TRACER_OPT_TIMESTAMP,
            0, "Trace register, add err, E=%d, P=%d", eid, pid);
      }
#endif
      return (TRACER_RETURN_ERROR);
   }
   if (DAL_SUCCESS !=
      DalSTMTrace_NewPort(tracer_stp_get_hStm(),
      &(CLIENT_DATA(hClient, stm_port))))
   {
      tracer_list_delete_node(&(tracer_client_list), hClient);
      phVClient = phClient;
      *phVClient = NULL;

#if TRACER_MSGERR_ON
      if (NULL != client_handle_self)
      {
         tracer_data(client_handle_self, TRACER_OPT_TIMESTAMP,
            0, "Trace register, port err, E=%d, P=%d, %d", eid, pid,
            hClient);
      }
#endif
      return (TRACER_RETURN_ERROR);
   }
   CLIENT_DATA(hClient, entity_id) = eid;
   CLIENT_DATA(hClient, protocol_id) = pid;
   if (TRACER_ENTITY_TDS == eid)
   {
      CLIENT_DATA(hClient, cpu_id) = tracer_osal_cpu_id();
      CLIENT_DATA(hClient, process_id) = tracer_osal_process_id();
      CLIENT_DATA(hClient, thread_id) = tracer_osal_thread_id();
   }

   // Tracer log message upon successful registration.
#if TRACER_MSGINFO_ON
   if (NULL != client_handle_self)
   {
      if (TRACER_ENTITY_TDS == eid)
      {
         tracer_data(client_handle_self, TRACER_OPT_TIMESTAMP,
            0, "Tracer register, TDS, 0x%X,E=%d,P=%d,C=%d,P=%d,T=%d",
            (unsigned int)CLIENT_DATA(*phClient, stm_port), eid, pid,
            CLIENT_DATA(hClient, cpu_id),
            CLIENT_DATA(hClient, process_id),
            CLIENT_DATA(hClient, thread_id));
      }
      else if (0 == eid)
      {
         tracer_data(client_handle_self, TRACER_OPT_TIMESTAMP,
            0, "Tracer register, Raw, 0x%X",
            (unsigned int)CLIENT_DATA(hClient, stm_port));
      }
      else
      {
         tracer_data(client_handle_self, TRACER_OPT_TIMESTAMP,
            0, "Tracer register, Indep, 0x%X,E=%d,P=%d",
            (unsigned int)CLIENT_DATA(hClient, stm_port), eid, pid);
      }
   }
#endif
   return (TRACER_RETURN_SUCCESS);
}

/*-------------------------------------------------------------------------*/
void _tracer_unreg(tracer_client_handle_t *phClient)
{
   if (NULL != *phClient)
   {
      if (0 < tracer_client_list.nodeCount)
      {
         DalSTMTrace_FreePort(tracer_stp_get_hStm(),
            CLIENT_DATA(*phClient, stm_port));
#if TRACER_MSGINFO_ON
         if (NULL != client_handle_self)
         {
            tracer_data(client_handle_self, TRACER_OPT_TIMESTAMP,
               0, "Unreg 0x%X: EP=%d,%d; CPT=%d,%d,%d",
               (unsigned int)CLIENT_DATA(*phClient, stm_port),
               CLIENT_DATA(*phClient, entity_id),
               CLIENT_DATA(*phClient, protocol_id),
               CLIENT_DATA(*phClient, cpu_id),
               CLIENT_DATA(*phClient, process_id),
               CLIENT_DATA(*phClient, thread_id));
         }
#endif
         if (0 == tracer_list_delete_node(&(tracer_client_list),
            (*phClient)))
         {  // Circumvent compiler optimizations to ensure assignment.
            phVClient = phClient;
            *phVClient = NULL;
         }
      }
   }
   return;
}
/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
// Criteria allowing trace output to be generated for a given
// registered client. Check _hClient prior to call, must not be NULL.
boolean _is_tracer_go(tracer_client_handle_t _hClient)
{
   return ( ( tcfg_is_entity_on(CLIENT_DATA(_hClient, entity_id)) &&
              tcfg_is_op_enabled() && tcfg_is_tracer_initialized() ) ? TRUE : FALSE );
}

/*-------------------------------------------------------------------------*/
// Criteria allowing sw event to be generated for a given registered
// client. Do check prior to call, _hClient must not be NULL.
boolean _is_event_go(tracer_client_handle_t hClient,
                              tracer_event_id_t eventId)
{
   if (!_is_tracer_go(hClient))
   {
      return FALSE;
   }
   return (tcfg_is_event_on(eventId));
}
//=========================================================================//
// Tracer external APIs.
//=========================================================================//


/*-------------------------------------------------------------------------*/
void tracer_set_owner(tracer_client_handle_t hClient,
                      tracer_thread_id_t owner_id)
{
   CLIENT_DATA(hClient, thread_id) = owner_id;
   return;
}

/*-------------------------------------------------------------------------*/
tracer_return_enum_t tracer_register(
   tracer_client_handle_t *phClient,
   tracer_ost_entity_id_enum_t eid,
   tracer_ost_protocol_id_t pid
)
{
   tracer_return_enum_t retval;
   *phClient = NULL;

   if (TRACER_ENTITY_SWEVT == eid)
   {  // Internal tracer use only.
      return (TRACER_RETURN_ERROR);
   }

   tcfg_tracer_init();

   tracer_mutex_lock();
   if (!tcfg_is_tracer_initialized())
   {
      retval = TRACER_RETURN_ERROR;
   }
   else
   {

#if (TRACER_MSGERR_ON || TRACER_MSGINFO_ON || TRACER_TESTING_ON)
      if (NULL == client_handle_self)
      {
         // Tracer register self as a client.
         _tracer_add_client(&client_handle_self,
            TRACER_ENTITY_DEFAULT, TRACER_PROTOCOL_DEFAULT);
      }
#endif
      retval = _tracer_add_client(phClient, eid, pid);
   }
   tracer_mutex_unlock();
   return (retval);
}

/*-------------------------------------------------------------------------*/
void tracer_unregister(tracer_client_handle_t *phClient)
{
   tracer_mutex_lock();
   _tracer_unreg(phClient);

#if (TRACER_MSGERR_ON || TRACER_MSGINFO_ON || TRACER_TESTING_ON)
   if ((1 == tracer_client_list.nodeCount) &&
       (tracer_client_list.pHead == client_handle_self))
   {
      _tracer_unreg(&(client_handle_self));
   }
#endif
   tracer_mutex_unlock();
}

/*-------------------------------------------------------------------------*/
tracer_return_enum_t tracer_data(
   tracer_client_handle_t hClient,
   tracer_options_t opt,
   uint32 data_len,
   const char* pData,
   ...
)
{
   const uint8 *my_msg;
   ALIGN(4) char msg_buffer[132]; // Size arbitrary chosen
   char *newBuff;
   va_list pArgs;

   if (NULL == hClient)
   {
      return TRACER_RETURN_ERROR;
   }
   if (!_is_tracer_go(hClient))
   {
      return TRACER_RETURN_SUCCESS;  // Silently discard.
   }

   newBuff = NULL;
   if (0 == data_len)
   {
      int rval;
      va_start(pArgs, pData);
      rval = vsnprintf(&msg_buffer[0], sizeof(msg_buffer), pData, pArgs);
      va_end(pArgs);
      if (0 > rval)
      {  // Assume input or encoding error. Spit out a token output.
         data_len = 1;
         msg_buffer[0] = '!';
         my_msg = (uint8 *)msg_buffer;
      }
      else if (rval >= sizeof(msg_buffer))
      {  // Assume truncated, try again.
         newBuff = (char *)_tracer_malloc(rval + 1);
         if (NULL == newBuff)
         {  // Or should I spit out token again?
            return TRACER_RETURN_ERROR;
         }
         // Don't bother checking length again. Use maximum alloc'd.
         va_start(pArgs, pData);
         data_len = vsnprintf(&newBuff[0], rval + 1, pData, pArgs);
         va_end(pArgs);
         my_msg = (uint8 *)newBuff;
      }
      else
      {
         data_len = rval;
         my_msg = (uint8 *)msg_buffer;
      }
   }
   else
   {
      my_msg = (uint8 *)pData;
   }

   switch (CLIENT_DATA(hClient, entity_id))
   {
      case TRACER_ENTITY_TDS: // TDS stream
         tracer_tds_send(
            CLIENT_DATA(hClient, stm_port), STM_DOPT(opt),
            CLIENT_DATA(hClient, entity_id),
            CLIENT_DATA(hClient, protocol_id),
            CLIENT_DATA(hClient, cpu_id),
            CLIENT_DATA(hClient, process_id),
            CLIENT_DATA(hClient, thread_id),
            data_len, my_msg);
         break;

      case TRACER_ENTITY_NONE:   // Raw stream
         tracer_stp_send_data(
            CLIENT_DATA(hClient, stm_port), STM_DOPT(opt),
            data_len, my_msg);
         break;

      default: // Independent stream
         tracer_ost_send_frame(
            CLIENT_DATA(hClient, stm_port), STM_DOPT(opt),
            CLIENT_DATA(hClient, entity_id),
            CLIENT_DATA(hClient, protocol_id),
            data_len, my_msg);
         break;
   }
   if (NULL != newBuff)
   {
      _tracer_free(newBuff);
   }

   return TRACER_RETURN_SUCCESS;
}

/*-------------------------------------------------------------------------*/
tracer_return_enum_t tracer_event(tracer_client_handle_t hClient,
                                    tracer_event_id_t event_id)
{
#if TRACER_SWEVT
   if (NULL == hClient)
   {
      return TRACER_RETURN_ERROR;
   }
   if (!_is_event_go(hClient, event_id))
   {
      return TRACER_RETURN_SUCCESS;  // Silently discard.
   }
   tracer_stp_send_event(CLIENT_DATA(hClient, stm_port), event_id);
   return TRACER_RETURN_SUCCESS;
#else
   return TRACER_RETURN_ERROR;
#endif
}


static void tracer_log_event_to_port(STMTracePortType port, tracer_event_id_t id, uint32 num_args,...)
{
  va_list pArgs;

  va_start(pArgs, num_args);
  tracer_ost_send_event_simple_vargs(port,id, num_args, pArgs);
  va_end(pArgs);
}



void tracer_log_sync_event(STMTracePortType port, tracer_event_id_t id, uint64 tag)
{
  tracer_log_event_to_port(port,
                           id,
                           2,
                           (uint32)tag,
                           (uint32)(tag >> 32));
}
