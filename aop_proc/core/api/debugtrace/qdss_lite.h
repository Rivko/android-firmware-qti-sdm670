#ifndef QDSSLITE_H
#define QDSSLITE_H

/*=============================================================================

FILE:         QDSSLite.h

DESCRIPTION:  This file has the QDSS Lite API intented for use with
              memory constrained systems.

================================================================================
                Copyright 2016, 2017 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Proprietary and Confidential - Qualcomm Technologies, Inc
==============================================================================*/
#include "DALStdDef.h"

#define QDSS_SUCCESS  0
#define QDSS_ERROR    -1


typedef uint32 STMPortAddrType;

/**
 * Initialize QDSS drivers and hardware
 *
 * This function initializes all the components of the QDSS sub-system
 * Should be called before other functions.
 *
 * @return None
 */
void QDSSInit(void);

/**
 * Enables QDSS trace 
 *
 * This function turns on SWAO debug block and QDSS timsestamp generator
 * If trace is already enabled the function will return DAL_SUCCESS without
 * making any changes.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSTraceEnable(void);


/**
 * Disables QDSS trace 
 *
 * This function turns off QDSS timestamp generator and SWAO debug block
 * If trace is already disabled the function will return DAL_SUCCESS without
 * making any changes.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSTraceDisable(void);


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
int QDSSAllocPort(STMPortAddrType *pPort);


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
int QDSSFreePort(STMPortAddrType port);

/**
 * Output a software event
 *
 * This function outputs a simple software event without parameters
 * to the trace stream.
 *
 * @param port The port to output the event on.
 * @param event_id A unique id for the event
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSEvent(STMPortAddrType port, int event_id);


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
int QDSSWriteOSTPacket(STMPortAddrType port, uint32 ost_header, byte *data, int nLen);


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

static __inline uint32 QDSSOSTHeader(byte entity,byte protocol)
{
   #define QDSS_OST_STARTSIMPLE  0x10
   #define QDSS_OST_VERSION      0x10


   return ( QDSS_OST_STARTSIMPLE << 24) |
      (QDSS_OST_VERSION << 16) | (entity << 8) | protocol; 
}

#define QDSS_OST_ENTITY_ULOG    11


/**
 * Suspend QDSS CX trace. 
 * To be called before CX power collapse.
 *
 * If AOSS ETB is in HWFIFO mode, this function switches it to CBUF mode
 * to avoid trace flow from AOSS to CX domain. 
 * 
 * Also if CX ETB is in HWFIFO mode, this function swithes it to CBUF mode
 * to avoid trace flow to DDR.DDR has to operational when calling this fuction.
 *
 * @return  0 on success,
 *          Otherwise an error code indicating the failure.
 */

int QDSSCXTraceSuspend(void);

/**
 * Resumes QDSS CX trace. 
 * To be called after wakeup from CX power collapse.
 *
 * This fucntion reverses any ETB mode switches that was done 
 * in QDSSCXTraceResume, and drains any data collected in ETB.
 * DDR  has to operational when calling this fuction.
 *
 * @return  0 on success,
 *          Otherwise an error code indicating the failure.
 */
int QDSSCXTraceResume(void);


/**
 * This function determines if QDSS trace to DDR is enabled.
 *
 * @return  TRUE if DDR trace is enabled
 *          FALSE otherise
 */
boolean QDSSIsDDRTraceEnabled(void);



/**
 * This function forces a QDSS timestamp resync
 *
 * @return  0 on success.
 *           Otherwise an error code indicating the failure.
 */

int QDSSTimestampResync(void); 

#endif //QDSSLITE_H
