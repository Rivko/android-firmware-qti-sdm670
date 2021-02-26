/*===========================================================================
  FILE: tracer_tds.c

  OVERVIEW:     Tracer data stream protocol. Packetizes client's trace data
                and sends it directly using OST base protocol.

  DEPENDENCIES:

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_tds.c#1 $$DateTime: 2018/07/30 01:21:36 $$Author: pwbldsvc $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-19 lht Initial revision.
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "tracer_tds.h"
#include "tracer_ost.h"
#include "tracer_stp.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define SIZE_OF_TDS_HEADER  (sizeof(tracer_cpu_id_t) +      \
                             sizeof(tracer_process_id_t) +  \
                             sizeof(tracer_thread_id_t))

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
//=========================================================================//
void tracer_tds_send(STMTracePortType port,
                     STMTraceDataType opt,
                     tracer_ost_entity_id_t eid,
                     tracer_ost_protocol_id_t pid,
                     tracer_cpu_id_t cpu_id,
                     tracer_process_id_t process_id,
                     tracer_thread_id_t thread_id,
                     uint32 uMsgLen,
                     const uint8 *pMsgData
)
{
   // Implemented as specialized OST function for efficiency of inserting
   // OST head and TDS head without having to repack with Msg_data as a
   // single payload.

   uint8 aData[SIZE_OF_TDS_HEADER];
   uint8 *paData;

   if (NULL == tracer_stp_get_hStm())
   {  // Bail out as early as possible.
      return;
   }

   // Pack the TDS header.
   paData = aData;
   *((tracer_cpu_id_t *)paData) = cpu_id;
   paData += sizeof(tracer_cpu_id_t);
   *((tracer_process_id_t *)paData) = process_id;
   paData += sizeof(tracer_process_id_t);
   *((tracer_thread_id_t *)paData) = thread_id;

#ifndef TRACER_OST_WA_PAD
   // Start of OST frame.
   tracer_ost_send_head(port, opt, eid, pid, SIZE_OF_TDS_HEADER + uMsgLen);

   // Send TDS header as body of OST frame
   tracer_ost_send_body(port, opt, SIZE_OF_TDS_HEADER, aData);

   // Body of OST frame data.
   tracer_ost_send_body(port, opt, uMsgLen, pMsgData);

   // End of OST frame.
   tracer_ost_send_tail(port, opt);
#else
   // TDS header must be 4 bytes.
   tracer_ost_send_workaround(port, opt, eid, pid,
      uMsgLen, pMsgData, SIZE_OF_TDS_HEADER, aData);
#endif

   return;
}


