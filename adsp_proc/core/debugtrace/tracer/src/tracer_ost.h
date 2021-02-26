#ifndef TRACER_OST_H
#define TRACER_OST_H
/*===========================================================================
  @file tracer_ost.h

OST base protocol over STP.

References:
  MIPI Alliance Specification for Open System Trace (OST) Base Protocol
  Version 0.80.00 - 22 April 2009.

  MIPI Alliance Specification for Transporting OST over STP
  Draft version 0.48.00 - 04 March 2010.

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_ost.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include <stdarg.h>     // va_...

#include "comdef.h"
#include "DDISTMTrace.h"
#include "tracer_entity.h"
#include "tracer.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// For entity id constants see tracer_entity.h
// Protocol ids are defined by the individual entities.

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief Places the OST framing start token and simplified OST
         heading into the client's output stream.

  Generates the start of a framed OST base protocol packet which
  consists of the starting token (a STP marked data packet) for the
  OST framing and the OST base protocol header. The data generated is
  sent to STM for output as a STP packet in the trace output stream
  for the given Client.

  @param port [in]: Client's STM port.
  @param opt [in]: Options to have delivery guaranteed.
  @param eid [in]: Identifier of the entity in which the
                   client was associated with at registration.
  @param pid [in]: The protocol ID associated with the entity.
  @param uPyldLen [in]: Bytes of payload data.

  @return None.
 */
void tracer_ost_send_head(STMTracePortType port,
                          STMTraceDataType opt,
                          tracer_ost_entity_id_t eid,
                          tracer_ost_protocol_id_t pid,
                          uint32 uPyldLen);

/*-------------------------------------------------------------------------*/
/**
  @brief Places the OST end of frame indicator into the Client's
         output stream.

  Generates the end of frame (the STP FLAG or FLAG_TS packet) of the
  framed OST packet. The data generated is sent to STM for output as
  a STP packet in the trace output stream for the given Client.

  @param port [in]: Client's STM port.
  @param opt [in]: Options to have trace data timestamped and/or
                 delivery guaranteed.

  @return None.
 */
void tracer_ost_send_tail(STMTracePortType port,
                          STMTraceDataType opt);

/*-------------------------------------------------------------------------*/
/**
  @brief Places the body of a framed OST packet directly into the
  Client's output stream.

  Trace data sent to STM for output as non-marked, non-timestamped data
  STP packets into the trace output stream for the given Client. Output
  by nibbles in little-endian order.

  @param port [in]: Client's STM port.
  @param opt [in]: Options to have delivery guaranteed.
  @param uMsgLen [in]: Length in bytes of the payload being sent.
  @param pMsgData [in]: Pointer to the first byte of the payload.

  @return None.
 */
void tracer_ost_send_body(STMTracePortType port,
                          STMTraceDataType opt,
                          uint32 uMsgLen,
                          const uint8 *pMsgData);

/*-------------------------------------------------------------------------*/
/**
  @brief Send trace data as a framed OST base protocol packet
         into the Client's trace stream.

  Trace data is encapsulated into a framed OST base protocol
  packet and then sent to STM for output as STP packets in the
  trace output stream for the given Client. Output by nibbles in
  little-endian order.

  @param port [in]: Client's STM port.
  @param opt [in]: Options to have trace data timestamped and/or
                 delivery guaranteed.
  @param entity_id [in]: Identifier of the entity in which the
                   client was associated with at registration.
  @param prot_id [in]: The protocol ID associated with the entity.
  @param uMsgLen [in]: Length in bytes of the payload being sent.
  @param pMsgData [in]: Pointer to the first byte of the payload.

  @return None.
 */
void tracer_ost_send_frame(STMTracePortType port,
                           STMTraceDataType opt,
                           tracer_ost_entity_id_t entity_id,
                           tracer_ost_protocol_id_t prot_id,
                           uint32 uMsgLen,
                           const uint8 *pMsgData);

/*-------------------------------------------------------------------------*/
/**
  @brief SW event with parameters (non-registered clients).

  Issues a SW event in an OST packet. Encapsulated as an independent
  cata stream from the SWEVT entity.

  @return None.
 */
void tracer_ost_send_event_simple_vargs(STMTracePortType port,
                                        tracer_event_id_t event_id,
                                        uint32 num_args,
                                        va_list pArg);

#ifdef TRACER_OST_WA_PAD
/*-------------------------------------------------------------------------*/
/**
  @brief Workaround to HWBug for sending the first set of OST
         frame bytes into the Client's trace stream.

  @return None.
 */
void tracer_ost_send_workaround(STMTracePortType port,
                                STMTraceDataType opt,
                                tracer_ost_entity_id_t entity_id,
                                tracer_ost_protocol_id_t prot_id,
                                uint32 uMsgLen,
                                const uint8 *pMsgData,
                                uint32 uHdrLen,
                                const uint8 *pHdrData);
#endif

#endif /* #ifndef TRACER_OST_H */

