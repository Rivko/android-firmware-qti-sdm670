/*============================================================================

FILE:      DalHWIOLogMem.c

DESCRIPTION:
  This file implements the HWIO access memory logging portion of the
  HWIO DAL.

PUBLIC CLASSES:
  __inp_extern
  __inpw_extern
  __inpdw_extern
  __outp_extern
  __outpw_extern
  __outpdw_extern

============================================================================
        Copyright (c) 2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/hwio/src/DalHWIOLogMem.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "comdef.h"



/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * DAL_HWIO_LOG_SIZE
 *
 * Number of entries in the HWIO log.
 */
#define DAL_HWIO_LOG_SIZE 128


/*
 * DalHWIOLogEventType
 *
 */
typedef enum
{
  DAL_HWIO_LOG_NULL,
  DAL_HWIO_LOG_INP08,
  DAL_HWIO_LOG_INP16,
  DAL_HWIO_LOG_INP32,
  DAL_HWIO_LOG_OUT08,
  DAL_HWIO_LOG_OUT16,
  DAL_HWIO_LOG_OUT32,
} DalHWIOLogEventType;


/*
 * DalHWIOLogEntryType
 *
 */
typedef struct
{
  DalHWIOLogEventType eEvent;
  uint32              nAddr;
  uint32              nData;
} DalHWIOLogEntryType;


/*
 * DalHWIOLogType
 *
 */
typedef struct
{
  uint32              nIndex;
  DalHWIOLogEntryType amEntries[DAL_HWIO_LOG_SIZE];
} DalHWIOLogType;



/*=========================================================================
      Data Definitions
==========================================================================*/

/*
 * DalHWIOLog
 *
 * Log
 */
static DalHWIOLogType DalHWIOLog;



/*=========================================================================
      Implementation
==========================================================================*/


/*==========================================================================

  FUNCTION      HWIO_Log

  DESCRIPTION   This function adds a HWIO access log entry.

  PARAMETERS    eEvent - Event type.
                nAddr  - The address being accessed
                nData  - The data being written if this is a write access.

  DEPENDENCIES  None.

  RETURN VALUE  None.

==========================================================================*/

static void HWIO_Log
(
  DalHWIOLogEventType eEvent,
  uint32              nAddr,
  uint32              nData
  
)
{
  /*-----------------------------------------------------------------------*/
  /* Fill in next entry.                                                   */
  /*-----------------------------------------------------------------------*/

  DalHWIOLog.amEntries[DalHWIOLog.nIndex].nAddr = nAddr;
  DalHWIOLog.amEntries[DalHWIOLog.nIndex].eEvent = eEvent;
  DalHWIOLog.amEntries[DalHWIOLog.nIndex].nData = nData;

  /*-----------------------------------------------------------------------*/
  /* Increment log index.                                                  */
  /*-----------------------------------------------------------------------*/

  if (++DalHWIOLog.nIndex >= DAL_HWIO_LOG_SIZE)
  {
    DalHWIOLog.nIndex = 0;
  }

} /* END HWIO_Log */


/*==========================================================================

  FUNCTION      __inp_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

uint8 __inp_extern
(
  uint32 nAddr
)
{
  HWIO_Log(DAL_HWIO_LOG_INP08, nAddr, 0);

  return (*((volatile uint8 *)(nAddr)));

} /* END __inp_extern */


/*==========================================================================

  FUNCTION      __inpw_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

uint16 __inpw_extern
(
  uint32 nAddr
)
{
  HWIO_Log(DAL_HWIO_LOG_INP16, nAddr, 0);

  return (*((volatile uint16 *)(nAddr)));

} /* END __inpw_extern */


/*==========================================================================

  FUNCTION      __inpdw_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

uint32 __inpdw_extern
(
  uint32 nAddr
)
{
  HWIO_Log(DAL_HWIO_LOG_INP32, nAddr, 0);

  return (*((volatile uint32 *)(nAddr)));

} /* END __inpdw_extern */


/*==========================================================================

  FUNCTION      __outp_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

void __outp_extern
(
  uint32 nAddr,
  uint8  nData
)
{
  HWIO_Log(DAL_HWIO_LOG_OUT8, nAddr, nData);

  *((volatile uint8 *)(nAddr)) = nData;

} /* END __outp_extern */


/*==========================================================================

  FUNCTION      __outpw_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

void __outpw_extern
(
  uint32 nAddr,
  uint16 nData
)
{
  HWIO_Log(DAL_HWIO_LOG_OUT16, nAddr, nData);

  *((volatile uint16 *)(nAddr)) = nData;

} /* END __outpw_extern */


/*==========================================================================

  FUNCTION      __outpdw_extern

  DESCRIPTION   See HALhwio.h

==========================================================================*/

void __outpdw_extern
(
  uint32 nAddr,
  uint32 nData
)
{
  HWIO_Log(DAL_HWIO_LOG_OUT32, nAddr, nData);

  *((volatile uint32 *)(nAddr)) = nData;

} /* END __outpdw_extern */




