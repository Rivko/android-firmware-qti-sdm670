#ifndef TRACER_TDS_H
#define TRACER_TDS_H
/*===========================================================================
  @file tracer_tds.h

Tracer's TDS (tracer data stream) protocol.

               Copyright (c) 2011 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_tds.h#1 $ */
/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "comdef.h"
#include "DDISTMTrace.h"
#include "tracer.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
// Current tracer data stream (TDS) protocol version (incl. in OST header)
// Defined in tracer_entity.h
// #define TRACER_PROTOCOL_TDS <number>

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

/**
  @brief Send trace data into the client's trace stream using
  the TDS protocol.

  The client's trace data is first encapsulated into a TDS protocol
  packet, then into a framed OST base protocol packet. Finally, this
  packet is sent to STM for output as STP packets in the trace output
  stream for the given client.

  @param port : Client's STM port.
  @param opt : Options to have data timestamped and/or delivery guaranteed.
  @param eid : Client's associated entity ID.
  @param pid : Protocol ID of the entity.
  @param cpu_id : Identifies the CPU on which this trace data originated.
  @param process_id : Identifies the Process in which this trace data
      originated
  @param thread_id : Identifies the sw thread which generated this trace data.
  @param uMsgLen : Bytes of data pointed to by pMsg_data that are to be sent.
  @param pMsgData : Point to the block of data to send.

  @return None.
 */

void tracer_tds_send(STMTracePortType port,
                     STMTraceDataType opt,
                     tracer_ost_entity_id_t eid,
                     tracer_ost_protocol_id_t pid,
                     tracer_cpu_id_t cpu_id,
                     tracer_process_id_t process_id,
                     tracer_thread_id_t thread_id,
                     uint32 uMsgLen,
                     const uint8 *pMsgData);

#endif /* #ifndef TRACER_TDS_H */

