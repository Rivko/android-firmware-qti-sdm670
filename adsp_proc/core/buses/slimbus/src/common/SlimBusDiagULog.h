#ifndef DALSLIMBUSLOG_H
#define DALSLIMBUSLOG_H
/*
===========================================================================

FILE:         DalSlimBusULog.h

DESCRIPTION:  This file defines ULog-based logging data structures and macros
              used by the SLIMbus Device Access Library.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/common/SlimBusDiagULog.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
05/27/15   NSD     Add macro for verbose level log message with eight
                   arguments.
04/30/15   NSD     Add Device ID in Diag logs to differentiate between
                   instances of the SlimBus Core
07/31/13   MJS     Revert change for verbose msg level.
06/19/13   MJS     Make info and verbose same msg level.
05/12/13   MJS     Support for QShrink 2.0.
11/29/12   MJS     Add default ulog name.
05/16/12   MJS     Update logging macros.
09/12/11   MJS     Improved logging for error conditions, added DIAG logging.
04/13/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011, 2012, 2013, 2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "ULogFront.h"
#include "msg.h"


#undef SB_LOG_DECLARE
#undef SB_LOG_INIT
#undef SB_LOG_ERR_0
#undef SB_LOG_ERR_1
#undef SB_LOG_ERR_2
#undef SB_LOG_ERR_3
#undef SB_LOG_ERR_4
#undef SB_LOG_ERR_5
#undef SB_LOG_ERR_6
#undef SB_LOG_ERR_7
#undef SB_LOG_WARN_0
#undef SB_LOG_WARN_1
#undef SB_LOG_WARN_2
#undef SB_LOG_WARN_3
#undef SB_LOG_WARN_4
#undef SB_LOG_WARN_5
#undef SB_LOG_WARN_6
#undef SB_LOG_WARN_7
#undef SB_LOG_INFO_0
#undef SB_LOG_INFO_1
#undef SB_LOG_INFO_2
#undef SB_LOG_INFO_3
#undef SB_LOG_INFO_4
#undef SB_LOG_INFO_5
#undef SB_LOG_INFO_6
#undef SB_LOG_INFO_7
#undef SB_LOG_VERB_0
#undef SB_LOG_VERB_1
#undef SB_LOG_VERB_2
#undef SB_LOG_VERB_3
#undef SB_LOG_VERB_4
#undef SB_LOG_VERB_5
#undef SB_LOG_VERB_6
#undef SB_LOG_VERB_7
#undef SB_LOG_VERB_8


#define SB_LOG_DECLARE() \
  uint32 uLogLevel; \
  ULogHandle hULog

#define SB_LOG_INIT(pDevCtxt) \
if (NULL == (pDevCtxt)->hULog) { \
  DALSYSPropertyVar PropVar; \
  uint32 uLogSize = 4096; \
  const char *pszInstName = "SLIMBUS"; \
  if ( NULL != (pDevCtxt)->pBSP ) pszInstName = (pDevCtxt)->pBSP->pszInstName; \
  SlimBus_LogLevelInit(pDevCtxt); \
  if ( DAL_SUCCESS == DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, \
                                               "log_size", 0, &PropVar ) && \
      DALSYS_PROP_TYPE_UINT32 == PropVar.dwType ) \
    uLogSize = PropVar.Val.dwVal; \
  ULogFront_RealTimeInit(&(pDevCtxt)->hULog, pszInstName, \
                         uLogSize, ULOG_MEMORY_LOCAL, ULOG_LOCK_NONE); \
}

#define SB_LOG_PREFIX_1 "[ERR ] "
#define SB_LOG_PREFIX_2 "[WARN] "
#define SB_LOG_PREFIX_3 "[INFO] "
#define SB_LOG_PREFIX_4 "[INFO] "

#define MSG_BUILD_MASK_8500 MSG_BUILD_MASK_MSG_SSID_QDSP6

#define SB_LOG_DECLARE_PDEVCTXT(pCtxt) \
  SlimBusDevCtxt *pDevCtxt = (pCtxt)

#define SB_LOG_PRINTF(level, count, format, args...) \
  if ( (level) <= (pDevCtxt)->uLogLevel ) \
  { \
    MSG_##count(MSG_SSID_QDSP6, MSG_LEGACY_FATAL>>level, "[DevId: 0x%x] " format, pDevCtxt->Base.DevId, ##args); \
  } \
  ULogFront_RealTimePrintf((pDevCtxt)->hULog, (count - 1), (SB_LOG_PREFIX_##level format), ##args)

#define SB_LOG_ERR_0(format)  SB_LOG_PRINTF( 1, 1,  format)
#define SB_LOG_ERR_1(format, a1)  SB_LOG_PRINTF( 1, 2,  format, a1)
#define SB_LOG_ERR_2(format, a1, a2)  SB_LOG_PRINTF( 1, 3,  format, a1, a2)
#define SB_LOG_ERR_3(format, a1, a2, a3)  SB_LOG_PRINTF( 1, 4,  format, a1, a2, a3)
#define SB_LOG_ERR_4(format, a1, a2, a3, a4)  SB_LOG_PRINTF( 1, 5,  format, a1, a2, a3, a4)
#define SB_LOG_ERR_5(format, a1, a2, a3, a4, a5)  SB_LOG_PRINTF( 1, 6,  format, a1, a2, a3, a4, a5)
#define SB_LOG_ERR_6(format, a1, a2, a3, a4, a5, a6)  SB_LOG_PRINTF( 1, 7,  format, a1, a2, a3, a4, a5, a6)
#define SB_LOG_ERR_7(format, a1, a2, a3, a4, a5, a6, a7)  SB_LOG_PRINTF( 1, 8,  format, a1, a2, a3, a4, a5, a6, a7)

#define SB_LOG_WARN_0(format)  SB_LOG_PRINTF( 2, 1,  format)
#define SB_LOG_WARN_1(format, a1)  SB_LOG_PRINTF( 2, 2,  format, a1)
#define SB_LOG_WARN_2(format, a1, a2)  SB_LOG_PRINTF( 2, 3,  format,  a1, a2)
#define SB_LOG_WARN_3(format, a1, a2, a3)  SB_LOG_PRINTF( 2, 4,  format,  a1, a2, a3)
#define SB_LOG_WARN_4(format, a1, a2, a3, a4)  SB_LOG_PRINTF( 2, 5,  format,  a1, a2, a3, a4)
#define SB_LOG_WARN_5(format, a1, a2, a3, a4, a5)  SB_LOG_PRINTF( 2, 6,  format,  a1, a2, a3, a4, a5)
#define SB_LOG_WARN_6(format, a1, a2, a3, a4, a5, a6)  SB_LOG_PRINTF( 2, 7,  format,  a1, a2, a3, a4, a5, a6)
#define SB_LOG_WARN_7(format, a1, a2, a3, a4, a5, a6, a7)  SB_LOG_PRINTF( 2, 8,  format,  a1, a2, a3, a4, a5, a6, a7)

#define SB_LOG_INFO_0(format)  SB_LOG_PRINTF( 3, 1,  format)
#define SB_LOG_INFO_1(format, a1)  SB_LOG_PRINTF( 3, 2,  format,  a1)
#define SB_LOG_INFO_2(format, a1, a2)  SB_LOG_PRINTF( 3, 3,  format,  a1, a2)
#define SB_LOG_INFO_3(format, a1, a2, a3)  SB_LOG_PRINTF( 3, 4,  format,  a1, a2, a3)
#define SB_LOG_INFO_4(format, a1, a2, a3, a4)  SB_LOG_PRINTF( 3, 5,  format,  a1, a2, a3, a4)
#define SB_LOG_INFO_5(format, a1, a2, a3, a4, a5)  SB_LOG_PRINTF( 3, 6,  format,  a1, a2, a3, a4, a5)
#define SB_LOG_INFO_6(format, a1, a2, a3, a4, a5, a6)  SB_LOG_PRINTF( 3, 7,  format,  a1, a2, a3, a4, a5, a6)
#define SB_LOG_INFO_7(format, a1, a2, a3, a4, a5, a6, a7)  SB_LOG_PRINTF( 3, 8,  format,  a1, a2, a3, a4, a5, a6, a7)

#define SB_LOG_VERB_0(format)  SB_LOG_PRINTF( 4, 1,  format)
#define SB_LOG_VERB_1(format, a1)  SB_LOG_PRINTF( 4, 2,  format,  a1)
#define SB_LOG_VERB_2(format, a1, a2)  SB_LOG_PRINTF( 4, 3,  format,  a1, a2)
#define SB_LOG_VERB_3(format, a1, a2, a3)  SB_LOG_PRINTF( 4, 4,  format,  a1, a2, a3)
#define SB_LOG_VERB_4(format, a1, a2, a3, a4)  SB_LOG_PRINTF( 4, 5,  format,  a1, a2, a3, a4)
#define SB_LOG_VERB_5(format, a1, a2, a3, a4, a5)  SB_LOG_PRINTF( 4, 6,  format,  a1, a2, a3, a4, a5)
#define SB_LOG_VERB_6(format, a1, a2, a3, a4, a5, a6)  SB_LOG_PRINTF( 4, 7,  format,  a1, a2, a3, a4, a5, a6)
#define SB_LOG_VERB_7(format, a1, a2, a3, a4, a5, a6, a7)  SB_LOG_PRINTF( 4, 8,  format,  a1, a2, a3, a4, a5, a6, a7)
#define SB_LOG_VERB_8(format, a1, a2, a3, a4, a5, a6, a7, a8)  SB_LOG_PRINTF( 4, 9,  format,  a1, a2, a3, a4, a5, a6, a7, a8)

#endif /* DALSLIMBUSLOG_H */

