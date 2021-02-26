#ifndef SIORS232_MDD_DEFS_H    //{
#define SIORS232_MDD_DEFS_H    1  

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S I O R S 2 3 2  M D D  D E F S

GENERAL DESCRIPTION


INITIALIZATION AND SEQUENCING REQUIREMENTS

  sio_rs232_dev_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 1999-2000, 2007-2008 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/mdd/inc/siors232_mdd_defs.h#1 $

when         who        what, where, why
-----------  ---------  -----------------------------------------------------
21-Jul-2008  cmihalik   Initial revision.
===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/
#include "target.h"                /* Target specific definitions          */
#include "comdef.h"                /* Definitions for byte, word, etc.     */

#include "queue.h"                 /* Queue declaration package            */
#include "sio.h"                   /* Serial I/O service definitions       */

//  If there is is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))              //{

#include "err.h"                   /* Error services definitions           */
#include "dsm.h"                   /* DS Memory Pool (Rawdata only)        */
#include "assert.h"                /* ASSERT macro unitilities             */
#include "siors232_mdd.h"

#if (defined(FEATURE_TCXO_SHUTDOWN) || defined(FEATURE_UP_CLOCK_SWITCHING))
#include "sleep.h"
#endif

#ifdef SIORS_USES_DIAGDSM
#include "diagdsm.h"               /* Spinoff of the DSM.h file            */
#endif

//----------------------------------------------------------------------------
//  is_rx_stream
//----------------------------------------------------------------------------

static inline boolean     
is_rx_stream( sio_stream_type stream_type )
{
  return(( stream_type == SIO_STREAM_RXTX    ) || 
         ( stream_type == SIO_STREAM_RX_ONLY ));
}

//----------------------------------------------------------------------------
//  is_tx_stream
//----------------------------------------------------------------------------

static inline boolean     
is_tx_stream( sio_stream_type stream_type )
{
  return(( stream_type == SIO_STREAM_RXTX    ) || 
         ( stream_type == SIO_STREAM_TX_ONLY ));
}

/*===========================================================================

                FEATURE FLAG RESOLUTION - Keep the mess here

===========================================================================*/
#if (defined(FEATURE_DS_IS2000) || defined(FEATURE_HDR) || defined(FEATURE_DATA) ) 
#define SIORS_HAS_PACKET_MODE
#endif

#if defined (FEATURE_DS) || defined (FEATURE_DATA)
#define SIORS_HAS_AUTODETECT
#endif

/* Compile time error generation */
#if defined(FEATURE_AUTOBAUD)
#error "AUTOBAUD is no longer supported"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================
 General constant and variable definitions.
===========================================================================*/

/* Guard time that is needed before and after the Escape Characters in order
   for the escape characters to be detected as escape sequence.  This time
   is also an upper bound on the amount of time that can pass between the
   escape characters.  Typically guard-time is 1s, but expire in .9s so that
   if application's timing is off (and/or phone's timing is off) escape 
   detection still works.  Phone's timing should not be off by more than 5ms,
   unless there are some extreme performance problems.                     */
#define  SIO_ESC_GUARD_TIME         900

/* The escape character.                                                   */
#define  SIO_ESC_CHAR                               '+'

/* PPP Flag character value.                                               */
#define SIO_ASYNC_FLAG        0x7E 

#define DISCARD_CHECK_INTERVAL                      20   /* 20 milli-seconds */
#define SIO_PACKET_DETECTION_TIMEOUT                2    /* 2 milli-seconds */

#endif //}

#endif //}

