#ifndef TRACER_CONFIG_INT_H
#define TRACER_CONFIG_INT_H
/** =========================================================================
  @file tracer_config_int.h
  @brief QUALCOMM Debug Subsystem (QDSS) Tracer configuration
         (internal interface)

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_config_int.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <stddef.h>
#include "tracer_config.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

boolean tcfg_is_tracer_initialized(void);
boolean tcfg_is_event_initialized(void);

int32 tcfg_get_entity_count(void);
uint32 tcfg_get_event_count(void);

void * tcfg_cpy_event_tag(void * dest, size_t destBytes);
void * tcfg_cpy_entity_op_ctrl(void *dest, size_t destBytes);
void * tcfg_cpy_event_op_ctrl(void *dest, uint8 group, size_t destBytes);

boolean tcfg_is_op_enabled(void);
boolean tcfg_is_entity_on(tracer_ost_entity_id_enum_t _eid);
boolean tcfg_is_event_on(tracer_event_id_t _eid);

void tcfg_event_set_default(void);

void tcfg_tracer_init(void);

tracer_cmdresp_t tcfg_set_systest_ctl(uint32 value);

void tracer_output_ctrl_notify_all(tracer_op_state_t state);

boolean _is_event_simple_go(tracer_event_id_t eventId);


#endif /* #ifndef TRACER_CONFIG_INT_H */

