/*===========================================================================
  FILE: tracer_ost.c

  OVERVIEW: Tracer OST base protocol and framing over STP. Also include
            direct write to STM as STP packets (no OST framing/header).

  NOTE:
    Includes workaround to a HW issue, coded for 32/64/128-bit writes to STM.

  REFERENCES:
    MIPI Alliance Specification for Open System Trace (OST) Base Protocol
    Version 0.80.00 - 22 April 2009.

    MIPI Alliance Specification for Transporting OST over STP
    Draft version 0.48.00 - 04 March 2010.

  DEPENDENCIES:

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_ost.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "stringl/stringl.h"   // memset (not available Venus, LPASS)
#include <stdarg.h>

#include "tracer_ost.h"
#include "tracer_stp.h"
#include "tracer_event_ids.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
#include "tracer_wa.h"

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
typedef uint8 tracer_ost_version_id_t;

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/

//=========================================================================//
// These macros enforce OST sending the correct type of STP packets.

// First 32-bits that start an OST frame: start token, OST ver, entity id,
// and protocol id.
#define OST_32BIT_HEAD(eid, pid) \
      ( (TRACER_OST_TOKEN_START & 0x000000FF) |         \
       (((tracer_ost_version_id_t)TRACER_OST_VERSION << 8) & 0x0000FF00) | \
       (((eid) << 16) & 0x00FF0000) | \
       (((pid) << 24) & 0xFF000000) )

// Start of frame, always in a "marked, non-timestamped" data STP packet.
#define OST_SEND_HEAD(port, opt, eid, pid) \
   STP_SEND_DATA(32, (port), \
      (((opt) | TRACE_DATA_MARKED) & (~TRACE_DATA_TIMESTAMPED)), \
      OST_32BIT_HEAD(eid,pid) )

// Body of frame, always in "non-marked, non-timestamped" data STP packet.
#define OST_SEND_BODY(port, opt, len, pDat) \
   tracer_stp_send_data((port), \
      ((opt) & (~(TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED))), \
      (len), (pDat))

// End of frame, using a FLAG or FLAG_TS STP packets, "marked" option invalid
#ifdef IMITATE_STPPKTS
#if (TRACER_WA_ALIGNBYTE == 16)
   #define OST_SEND_TAIL(port, opt) \
      STP_SEND_FLAG(port, (opt) | TRACE_DATA_TIMESTAMPED); \
      STP_SEND_FLAG(port, (opt) | TRACE_DATA_TIMESTAMPED); \
      STP_SEND_FLAG(port, (opt) & (~TRACE_DATA_TIMESTAMPED)); \
      STP_SEND_FLAG(port, (opt) & (~TRACE_DATA_TIMESTAMPED))
#elif (TRACER_WA_ALIGNBYTE == 8)
   #define OST_SEND_TAIL(port, opt) \
      STP_SEND_FLAG(port, (opt) & (~TRACE_DATA_MARKED)); \
      STP_SEND_FLAG(port, (opt) & (~TRACE_DATA_MARKED))
#else
   #define OST_SEND_TAIL(port, opt) \
      STP_SEND_FLAG(port, (opt) & (~TRACE_DATA_MARKED))
#endif
#else
#if (TRACER_WA_ALIGNBYTE == 16)
   #define OST_SEND_TAIL(port, opt) \
      STP_SEND_FLAG(port, ((opt) | TRACE_DATA_TIMESTAMPED) & (~TRACE_DATA_MARKED));
#else
   #define OST_SEND_TAIL(port, opt) \
      STP_SEND_FLAG(port, (opt) & (~TRACE_DATA_MARKED))
#endif
#endif

//=========================================================================//
#if (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTWA)

#ifdef IMITATE_STPPKTS
#if (TRACER_WA_ALIGNBYTE == 16)
   #define OST_SEND_HEAD_WA(bitlen, port, opt, pData) \
      STP_SEND_DATA(32, (port), \
         ((opt) | TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED), \
         *((uint32*)pData)); \
      STP_SEND_DATA(32, (port), \
         ((opt) | TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED), \
         *(((uint32*)pData)+1)); \
      STP_SEND_DATA(32, (port), \
         (((opt) | TRACE_DATA_MARKED) & (~TRACE_DATA_TIMESTAMPED)), \
         *(((uint32*)pData)+2)); \
      STP_SEND_DATA(32, (port), \
         (((opt) | TRACE_DATA_MARKED) & (~TRACE_DATA_TIMESTAMPED)), \
         *(((uint32*)pData)+3))
#elif (TRACER_WA_ALIGNBYTE == 8)
   #define OST_SEND_HEAD_WA(bitlen, port, opt, pData) \
      STP_SEND_DATA(32, (port), \
         (((opt) | TRACE_DATA_MARKED) & (~TRACE_DATA_TIMESTAMPED)), \
          *((uint32*)pData)); \
      STP_SEND_DATA(32, (port), \
         (((opt) | TRACE_DATA_MARKED) & (~TRACE_DATA_TIMESTAMPED)), \
         *(((uint32*)pData)+1))
#else
   #define OST_SEND_HEAD_WA(bitlen, port, opt, pData) \
      STP_SEND_DATA(32, (port), \
         (((opt) | TRACE_DATA_MARKED) & (~TRACE_DATA_TIMESTAMPED)), \
          *((uint32*)pData))
#endif
#else
#if (TRACER_WA_ALIGNBYTE == 16)
   #define OST_SEND_HEAD_WA(bitlen, port, opt, pData) \
      STP_SEND_DATABLK(bitlen, (port), \
         ((opt) | TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED), \
         ((uint32*)pData))
#else
   #define OST_SEND_HEAD_WA(bitlen, port, opt, pData) \
      STP_SEND_DATABLK(bitlen, (port), \
         (((opt) | TRACE_DATA_MARKED) & (~TRACE_DATA_TIMESTAMPED)), \
         ((uint32*)pData))
#endif
#endif

#endif   // (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTWA)

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
//=========================================================================//
void tracer_ost_send_head(STMTracePortType port,
                          STMTraceDataType opt,
                          tracer_ost_entity_id_t eid,
                          tracer_ost_protocol_id_t pid,
                          uint32 uPyldLen)
{
#if (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTBASIC)
   uint8 data8;
#endif

   if (NULL != hSTM)
   {
      OST_SEND_HEAD(port, opt, eid, pid);

#if (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTBASIC)

      // OST base header length fields.
      if (256 > uPyldLen)
      {  // Only include the 1-byte length field.
         data8 = (uint8)(uPyldLen & 0x0FF);
         OST_SEND_BODY(port, opt, 1, &data8);
      }
      else
      {  // 1-byte length and 4-byte ext. length fields
         data8 = 0;
         OST_SEND_BODY(port, opt, 1, &data8);
         OST_SEND_BODY(port, opt, 4, (uint8 *)&uPyldLen);
      }

#elif (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTWA)

      // Workaround 4-byte length field
      OST_SEND_BODY(port, opt, 4, (uint8 *)&uPyldLen);

#endif
   }

   return;
}

//=========================================================================//
void tracer_ost_send_body(STMTracePortType port,
                          STMTraceDataType opt,
                          uint32 uMsgLen,
                          const uint8 *pMsgData)
{
   if (NULL != hSTM)
   {
      OST_SEND_BODY(port, opt, uMsgLen, pMsgData);
   }
   return;
}

//=========================================================================//
void tracer_ost_send_tail(STMTracePortType port,
                          STMTraceFlagType opt)
{
   if (NULL != hSTM)
   {
      OST_SEND_TAIL(port, opt);
   }
   return;
}

//=========================================================================//
//=========================================================================//
#if (TRACER_OST_FORMAT == TRACER_OST_FMT_WA_PREPAD)
#define SIZE_OF_OST_HEADER   8

void tracer_ost_send_workaround(STMTracePortType port,
                                STMTraceDataType opt,
                                tracer_ost_entity_id_t eid,
                                tracer_ost_protocol_id_t pid,
                                uint32 uMsgLen,
                                const uint8 *pMsgData,
                                uint32 uHdrLen,
                                const uint8 *pHdrData)
{
   align_info_s ais;    // Info for packing data into aligned buffer.
   char *pAlignChar;    // Pointer to traverse aligned buffer.
   uint32 prepad_length = 0;  // Bytes of prepadding needed.
   int ii;

   if (NULL != hSTM)
   {
      // Clear and init prepadding (and post-padding).
      memset(ais.buffer, 0, sizeof(ais.buffer));

      // Pack proper sized header with pre-padding into an aligned
      // buffer.
      get_align_info(&ais, (uint32)pMsgData);
      if (ais.nonAlignBytes >= uMsgLen)
      {
         ais.nonAlignBytes = uMsgLen;
      }

      // Add start of OST frame bytes. */
      ais.pAligned[0] = OST_32BIT_HEAD(eid, pid);

      // Calculate prepad needed.
      prepad_length = ( TRACER_WA_ALIGNBYTE -
         ( (SIZE_OF_OST_HEADER + uHdrLen + ais.nonAlignBytes) &
           (TRACER_WA_ALIGNBYTE - 1) ) ) & (TRACER_WA_ALIGNBYTE - 1);

      ii = uMsgLen - ais.nonAlignBytes;
      if ( (0 < ii) && (prepad_length >= ii))
      {
         prepad_length -= ii;
         ais.nonAlignBytes = uMsgLen;
      }
      // Add length fields.
      ais.pAligned[1] = ((prepad_length << 24) & 0xFF000000) |
                        ((uHdrLen + uMsgLen) & 0x00FFFFFF);

      // Add prepadding bytes.
      pAlignChar = ((char*)(ais.pAligned)) + SIZE_OF_OST_HEADER +
                   prepad_length;

      // Add TDS header bytes.
      for (ii = 0; ii < uHdrLen; ii++)
      {
         *pAlignChar = pHdrData[ii];
         pAlignChar++;
      }

      // Nonaligned msg bytes (always <= uMsgLen)
      for (ii = 0; ii < ais.nonAlignBytes; ii++)
      {
         pAlignChar[ii] = pMsgData[ii];
         uMsgLen--;
      }
      pMsgData += ii;
      pAlignChar += ii;

#if (TRACER_WA_ALIGNBYTE == 4)
      OST_SEND_HEAD_WA(32, port, opt, ais.pAligned);
      ais.pAligned += 1;
      while ((uint32)ais.pAligned < (uint32)pAlignChar)
      {
         tracer_ost_send_body(port, opt, 4, (uint8 *)(ais.pAligned));
         ais.pAligned += 1;
      }
#elif (TRACER_WA_ALIGNBYTE == 8)
      OST_SEND_HEAD_WA(64, port, opt, ais.pAligned);
      ais.pAligned += 2;
      while ((uint32)ais.pAligned < (uint32)pAlignChar)
      {
         tracer_ost_send_body(port, opt, 8, (uint8 *)(ais.pAligned));
         ais.pAligned += 2;
      }
#elif (TRACER_WA_ALIGNBYTE == 16)
      OST_SEND_HEAD_WA(128, port, opt, ais.pAligned);
      ais.pAligned += 4;
      while ((uint32)ais.pAligned < (uint32)pAlignChar)
      {
         tracer_ost_send_body(port, opt, 16, (uint8 *)(ais.pAligned));
         ais.pAligned += 4;
      }
#endif

      // OST payload
      tracer_ost_send_body(port, opt, uMsgLen, pMsgData);

      // End of OST frame.
      tracer_ost_send_tail(port, opt);
   }
   return;
}

#endif   // TRACER_OST_FORMAT==..

//=========================================================================//
void tracer_ost_send_frame(STMTracePortType port,
                           STMTraceDataType opt,
                           tracer_ost_entity_id_t eid,
                           tracer_ost_protocol_id_t pid,
                           uint32 uMsgLen,
                           const uint8 *pMsgData)
{
   if (NULL != hSTM)
   {
#if !(TRACER_OST_FORMAT == TRACER_OST_FMT_WA_PREPAD)
      // Start of OST frame */
      tracer_ost_send_head(port, opt, eid, pid, uMsgLen);

      // OST payload
      tracer_ost_send_body(port, opt, uMsgLen, pMsgData);

      // End of OST frame.
      tracer_ost_send_tail(port, opt);
#else
      tracer_ost_send_workaround(port, opt, eid, pid,
         uMsgLen, pMsgData, 0, NULL);
#endif
   }
   return;
}

//=========================================================================//
void tracer_ost_send_event_simple_vargs(STMTracePortType port,
                                tracer_event_id_t event_id,
                                uint32 num_args,
                                va_list pArgs)
{

#if (4 >= TRACER_WA_ALIGNMENT)
   int ii;
   tracer_ost_send_head(port, TRACE_DATA_NONETYPE,
      TRACER_ENTITY_SWEVT, TRACER_PROTOCOL_SWEVT,
      (uint32)((num_args + 1) * sizeof(uint32)));  // id + each arg

   STP_SEND_DATA32(port, TRACE_DATA_NONETYPE, event_id);
   for(ii = 0; ii < num_args; ii++)
   {
      STP_SEND_DATA32(port, TRACE_DATA_NONETYPE, va_arg(pArgs, uint32));
   }

   tracer_ost_send_tail(port, TRACE_DATA_TIMESTAMPED);
#else
#define TRACER_MAX_EVENT_ARGS  15
   uint32 data[TRACER_MAX_EVENT_ARGS + 1];
   int ii;

   if (TRACER_MAX_EVENT_ARGS < num_args)
   {
      num_args = TRACER_MAX_EVENT_ARGS;
   }
   data[0] = event_id;
   for(ii = 0; ii < num_args; ii++)
   {
      data[ii + 1] = va_arg(pArgs, uint32);
   }
   

   tracer_ost_send_workaround(port, TRACE_DATA_NONETYPE,
      TRACER_ENTITY_SWEVT, TRACER_PROTOCOL_SWEVT,
      (uint32)((num_args + 1) * sizeof(uint32)),
      (uint8 *)data, 0, NULL);
#endif

   return;
}


