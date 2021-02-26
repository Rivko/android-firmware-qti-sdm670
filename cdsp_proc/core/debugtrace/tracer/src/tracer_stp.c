/*===========================================================================
  FILE: tracer_stp.c

  OVERVIEW: Tracer STP interface. Writes to STM to generate STP packets.

  NOTE:
    Includes workaround to a HW issue, coded for 32/64/128-bit writes to STM.

  DEPENDENCIES:

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_stp.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "stringl/stringl.h"   // memset (not available Venus, LPASS)

#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "tracer_event_ids.h"
#include "tracer_stp.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Extern Variable Definitions
 * ------------------------------------------------------------------------*/
extern DALDEVICEHANDLE hSTM;  /**< Handle to STM for trace output. */
extern DALDEVICEHANDLE hSTMSLPI;  /**< Handle to STM for trace output. */


//=========================================================================//
tracer_return_enum_t tracer_stp_attach_stm(void)
{
   if (DAL_SUCCESS
       != DAL_STMTraceDeviceAttach("DALDEVICEID_STM_TRACE", &hSTM)) {
      return TRACER_RETURN_ERROR;
   }

#ifdef TRACER_SLPI
   if (DAL_SUCCESS
       != DAL_STMTraceDeviceAttach("DALDEVICEID_STM_TRACE_SLPI", &hSTMSLPI)) {
      return TRACER_RETURN_ERROR;
   }
#else
   hSTMSLPI = hSTM;
#endif

   return TRACER_RETURN_SUCCESS;

}

DALDEVICEHANDLE tracer_stp_get_hStm(void)
{
   return hSTM;
}

DALDEVICEHANDLE tracer_stp_get_hStmSLPI(void)
{
   return hSTMSLPI;
}


//=========================================================================//
void tracer_stp_send_event_wa(STMTracePortType port,
                              tracer_event_id_t event_id)
{
   uint8 buff[32];
   uint8 *alignBuff;
   uint32 code_len;

    alignBuff= (uint8*) (((uint32)buff + 15) & (~0x0F));

   *((uint32*)alignBuff) = ((event_id << 8) & 0xFFFFFF00) |
                           (TRACER_OST_TOKEN_SWEVENT & 0x0FF);
   code_len = 4;

#if (TRACER_WA_ALIGNBYTE == 8)
   *((uint32*) (alignBuff + 4)) = (TRACER_OST_TOKEN_END & 0x0FF);
   code_len = 8;
#elif (TRACER_WA_ALIGNBYTE == 16)
   *((uint32*) (alignBuff +  4)) = (TRACER_OST_TOKEN_END & 0x0FF);
   *((uint32*) (alignBuff +  8)) = (TRACER_OST_TOKEN_END & 0x0FF);
   *((uint32*) (alignBuff + 12)) = (TRACER_OST_TOKEN_END & 0x0FF);
   code_len = 16;
#endif

   tracer_stp_send_data(port,
      (TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED),
      code_len, alignBuff);
   return;
}

