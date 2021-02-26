/*=============================================================================

FILE:         QdssLite.h

DESCRIPTION:  This file has the QDSS Lite API intented for use with
              memory constrained systems.

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#ifndef QDSSLITE_H_
#define QDSSLITE_H_


#include <Library/BaseLib.h>
#include "com_dtypes.h"
#include "QdssStmTrace.h"

/**
 * Initialize QDSS drivers and hardware
 *
 * This function initializes all the components of the QDSS sub-system
 * Should be called before other functions.
 *
 * @return  0 on success,
 *          1 indicating not initialized.
 */
int QdssInit(void);

/**
 * Enables QDSS trace on RPM
 *
 * This function votes the QDSS clocks ON and enables QDSS trace output.
 * If trace is already enabled the function will return DAL_SUCCESS without
 * making any changes.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QdssEnableTrace(void);


/**
 * Disables QDSS trace on RPM
 *
 * This function disables QDSS trace output and votes QDSS clocks OFF.
 * If trace is already disabled the function will return DAL_SUCCESS without
 * making any changes.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QdssDisableTrace(void);

/**
 * Enables SW event(s) on RPM
 *
 * This function enables SW event(s).
 *
 */
int QdssEnableSwEvent(uint32 event_id);

/**
 * DIsables SW event(s) on RPM
 *
 * This function disables SW event(s).
 *
 */
int QdssDisableSwEvent(uint32 event_id);

/**
 * Output a software event
 *
 * This function outputs a simple software event without parameters
 * to the trace stream.
 *
 * //@param port The port to output the event on.
 * @param event_id A unique id for the event
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QdssTraceSwEvent(uint32 event_id);

int QdssTraceSwEventData(uint32 event_id, UINT32 number_args, VA_LIST args);

/**
 * Allocate an STM stimulus port
 *
 * This function allocates a dedicated STM stimulus port for the caller to
 * ouptut trace data.
 *
 * @param pPort OUT parameter for returning the port.
 *              On success, the location pointed to by pPort will have the
 *              newly allocated port.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QdssAllocPort(STMPortAddrType *pPort);


/**
 * Free a previously allocated STM stimulus port
 *
 * This function releases a  STM stimulus port previously allocated for
 * the caller
 *
 * @param port the port to free.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QdssFreePort(STMPortAddrType port);


/**
 * Write an OST data packet
 *
 * This function outputs data to the trace stream,
 * encapsulated in a MIPI OST packet. Default values are
 * used for Entity ID(2) and protocol ID(0)
 *
 * @param port The port to output data on.
 * @param ost_header OST Header formed with the QDSSOSTHeader below
 * @param data A pointer to the start of the data buffer
 * @param len   Length of the data buffer.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QdssWriteOSTPacket(STMPortAddrType port, uint32 ost_header, byte *data, int nLen);


/**
 * Make an OST Header with the given parameters
 *
 * This is a helper function for use with QDSSWriteOSTPacket
 *
 * @param entity   OST entity field
 * @param protocol OST protocol field
 *
 * @return  A 32 bit OST packet header
 */

static __inline uint32 QdssOstHeader(byte entity, byte protocol)
{
   #define QDSS_OST_STARTSIMPLE  0x10
   #define QDSS_OST_VERSION      0x10


   return ( QDSS_OST_STARTSIMPLE << 24) |
      (QDSS_OST_VERSION << 16) | (entity << 8) | protocol;
}

#define QDSS_OST_ENTITY_ULOG    11


/**
 * Sends a single pulse from apps0 CTI onto CTM rail 1
 *
 *
 * @return  1 on success,
 *          otherwise 0, indicating the failure.
 */
int Qdss_CTI_Pulse(void);


#endif  /* QDSSLITE_H_ */
