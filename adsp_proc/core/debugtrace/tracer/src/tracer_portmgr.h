#ifndef TRACER_PORTMGR_H
#define TRACER_PORTMGR_H
/*===========================================================================
  @file tracer_portmgr.h

Tracer management of a pool of STM ports.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_portmgr.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "comdef.h"
#include "DDISTMTrace.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
#ifndef TRACER_PORT_POOL_SIZE
#define TRACER_PORT_POOL_SIZE  4
#endif

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

struct portMgr_s
{
   int nextKey;         // Round robin of next available port.
   int inUseCount;      // Track number of port in use simultaneously.
   int inUseHiMark;     // High watermark for number of ports in use.

   STMTracePortType trashPort; // Garbage collection STM port
   STMTracePortType d32Port;   // STM port for single D32 writes
   struct
   {
      int inUseFlag;          // Port is being used.
      STMTracePortType port;  // STM port number.
   } pool[TRACER_PORT_POOL_SIZE];    // Pool of port for handleless calls.
   boolean bInitialized;
};

extern struct portMgr_s gPortMgr;
extern struct portMgr_s gPortMgrMicro;



/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief Initializes the pool of ports for use.
  @return TRUE = Initialization completed.
  @return FALSE = Initialization failed.                                           .
 */
boolean tracer_portmgr_init(void);

/*-------------------------------------------------------------------------*/
/**
  @brief Gets and reserves an available STM port for use.

  The port manager must be initialized for a valid port & key pair
  to be returned. If port manager is not initialized a invalid
  port zero (0) is returned.

  @param key [in]: pointer for a key value.
  @param key [out]: key value for port pairing.
  @return STM port
  @warning Must call tracer_port_free with key identifier value
           when done.
 */
STMTracePortType tracer_port_get(int *key);

/*-------------------------------------------------------------------------*/
/**
  @brief Frees the STM port previously obtained and reserved.

  @param key [in]: value of the key identifier when port was
             obtained.
  @return None.
 */
void tracer_port_free(int key);

/*-------------------------------------------------------------------------*/
/**
  @brief High watermark of port simultaneously reserved (in use
         at a given time).

  @return Maximum count of ports reserved at any given time.
 */
int tracer_port_himark(void);

/*-------------------------------------------------------------------------*/
/**
  @brief A generic STM port for single 32-bit writes.
  @return
 */
STMTracePortType tracer_port_d32(void);

extern boolean tracer_log_all_to_ssc_qdss;

#endif /* #ifndef TRACER_PORTMGR_H */

