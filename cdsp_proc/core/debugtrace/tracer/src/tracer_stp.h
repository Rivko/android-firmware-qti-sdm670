#ifndef TRACER_STP_H
#define TRACER_STP_H
/*===========================================================================
  @file tracer_stp.h

  Tracer STP inteface support. Higher layer interface to the STM drivers.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_stp.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "DalDevice.h"
#include "DDISTMTrace.h"

#include "tracer.h"
#include "tracer_wa.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

#if (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTWA)
//=========================================================================//
// For packing data into an aligned buffer.
typedef struct
{
   uint32 buffer[12];
      // Allocate at least 48 bytes = 16 bytes for aligning self, plus
      // 32 possible bytes of total start of frame data to get aligned.
      // Worst case is the 16-byte alignment case = 4 bytes start +
      // 4 byte length + 15 bytes prepad + 4 bytes of TDS +
      // 5 bytes of msg data misaligned = 32 bytes total.
   uint32 *pAligned;    // Pointer to aligned boundary

   uint32 *pMsgBoundary;
   uint32 offset;
   uint32 nonAlignBytes;

} align_info_s;
#endif

/*---------------------------------------------------------------------------
 * Definitions
 * ------------------------------------------------------------------------*/
extern DALDEVICEHANDLE hSTM;  /**< Handle to STM for trace output. */

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
//=========================================================================//

/*-------------------------------------------------------------------------*/
#if (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTWA)
void get_align_info (align_info_s *ainfo, uint32 msgAddr);
#endif

/*-------------------------------------------------------------------------*/
/**
  @brief Initializes the STM device handle.
 */
tracer_return_enum_t tracer_stp_attach_stm(void);

/**
  @brief Get the STM device handle.
  @return Handle to the STM device.
 */
DALDEVICEHANDLE tracer_stp_get_hStm(void);


/**
  @brief Get the SLPI STM device handle.
  @return Handle to the SLPI STM device.
 */
DALDEVICEHANDLE tracer_stp_get_hStmSLPI(void);


/*-------------------------------------------------------------------------*/
/**
  @brief Macros for writing to the QDSS STM's (System Trace Macrocell)
  appropriate stimulus port to generate desired STP (System Trace Protocol)
  packets.

  Calls the DAL STM trace driver functions to generate STP packets
  on the specified port with the specified transaction type (invariant
  timing or guaranteed) and payload


  @param bitlen: Size of payload "data" bits. Valid values: 8, 16, 32.
  @param port: STM port value
  @param opt: Type of packet (Marked, Timestamped) and
               type of transaction (Invariant type or Guaranteed)
  @param dat: Payload bytes.
  @param hSTM: Valid STM device handle.
  @return DALResult DAL standard error values (DAL_SUCCESS, DAL_ERROR, ...).
  @dependencies
        Must have a valid handle to the STM, DALDEVICEHANDLE hSTM.
 */

#define STP_SEND_DATA(bitlen, port, opt, dat) \
   DalSTMTrace_Data##bitlen(hSTM, (port), (opt), (dat))

#define STP_SEND_DATA32(port , opt, dat) \
   DalSTMTrace_Data32(hSTM, (port), (opt), (dat))

#define STP_SEND_FLAG(port, opt) \
   DalSTMTrace_Flag(hSTM, (port), (opt))

/*-------------------------------------------------------------------------*/
#if (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTWA)

#ifdef IMITATE_STPPKTS

   #if (TRACER_WA_ALIGNBYTE == 16)
      #define STP_SEND_DATABLK(bitlen, port, opt, pDat) \
         DalSTMTrace_Data32(hSTM, (port), (opt)| TRACE_DATA_TIMESTAMPED, (pDat)[0]); \
         DalSTMTrace_Data32(hSTM, (port), (opt)| TRACE_DATA_TIMESTAMPED, (pDat)[1]); \
         DalSTMTrace_Data32(hSTM, (port), (opt), (pDat)[2]); \
         DalSTMTrace_Data32(hSTM, (port), (opt), (pDat)[3]);
   #elif (TRACER_WA_ALIGNBYTE == 8)
      #define STP_SEND_DATABLK(bitlen, port, opt, pDat) \
         DalSTMTrace_Data32(hSTM, (port), (opt), (pDat)[0]); \
         DalSTMTrace_Data32(hSTM, (port), (opt), (pDat)[1]);
   #else
      #define STP_SEND_DATABLK(bitlen, port, opt, pDat) \
         DalSTMTrace_Data32(hSTM, (port), (opt), (pDat)[0]);
   #endif

#else // !IMITATE_STPPKTS

   #if ((TRACER_WA_ALIGNBYTE == 16) || (TRACER_WA_ALIGNBYTE == 8))
      //bitlen = {64, 128}
      #define STP_SEND_DATABLK(bitlen, port, opt, pDat) \
         DalSTMTrace_DataBlk##bitlen(hSTM, (port), (opt), (pDat));
   #else
      //bitlen = {32}
      #define STP_SEND_DATABLK(bitlen, port, opt, pDat) \
         STP_SEND_DATA(bitlen, port, opt, *((uint32*)pDat));

   #endif

#endif // IMITATE_STPPKTS

#endif // (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTWA)


/**
  @brief Places trace data into the trace stream on the given port.

  Trace data is sent to STM for output as STP packets in the trace
  output stream on the given port.

  @param port [in]: STM port.
  @param opt [in]: Options to have trace data marked,
                 timestamped and/or delivery guaranteed.
  @param uMsgLen [in]: Length in bytes of the payload being
                 sent.
  @param pMsgData [in]: Pointer to the first byte of the
                   payload.
  @return None.
 */
void tracer_stp_send_data(STMTracePortType port,
                          STMTraceDataType opt,
                          uint32 uMsgLen,
                          const uint8 *pMsgData);

/*-------------------------------------------------------------------------*/
/**
  @brief Places a SW Event into the trace stream on the given port.

  A SW Event is sent to STM for output as STP packets in the trace
  output stream on the given port.

  @param port [in]: STM port.
  @param event_id [in]: Event id value

  @return None.
 */
void tracer_stp_send_event(STMTracePortType port,
                           tracer_event_id_t event_id);

#endif /* #ifndef TRACER_STP_H */
