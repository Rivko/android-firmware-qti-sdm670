#ifndef QDSS_H
#define QDSS_H
/*=========================================================================
                       QDSS PUBLIC API

GENERAL DESCRIPTION
   The APIs expose minimal functionalies to initialize and check status
   of the QDSS tracer framework.

EXTERNALIZED FUNCTIONS
   QDSSInit
   QDSSisEnabled
   QDSSFlush

INITIALIZATION AND SEQUENCING REQUIREMENTS
   QDSSInit is always invoked in ALL builds where QDSS peripheral is present
regardless of the efuse setting which might disable run time access to QDSS.
QDSSOn and QDSSOff needs to be explicitly invoked by tracer to enable QDSS
clocks.
               Copyright 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential
==========================================================================*/
/*==========================================================================
 $Header: //components/rel/core.qdsp6/2.1.c4/api/debugtrace/qdss.h#1 $
==========================================================================*/

#include "comdef.h"

/**
Input: None

Description:
   Initializes QDSS drivers and places QDSS software state in
   the READY state. However, QDSS clocks are not turned ON
   unless this API is invoked by a client.

Return: None
**/
void QDSSInit(void);


/**
Input:None

Description:
   Checks if QDSS is in IN_USE state. The IN_USE state indicates
   whether QDSS clocks are turned on/off.
Return:
   TRUE -  QDSS clocks are enabled and there are tracer clients using QDSS.
   FALSE - QDSS clocks are disabled and no tracer client is using QDSS.
**/
boolean QDSSisInUse(void);



/**
Input: None

Description:
   Flushes pending trace data out of buffers in various QDSS components and
   drains them into the QDSS sink.

Return: None
**/
extern void QDSSFlush(void);

/**
Input: None

Description:
   Request to turn ON QDSS clocks. The request can be denied if QDSS peripheral
   is inaccessible based on fuse settings or due to a software override. If successful
   the QDSS software state is placed in IN_USE state. A ON request in the IN_USE state also
   returns SUCCESS.

Return:
   0: SUCCESS - request was accepted
  -1: FAILURE - request denied
**/
int32 QDSSOn(void);

/**
Input: None

Description:
   Request to turn OFF QDSS clocks. If successful, the QDSS software state is placed in READY.
   A OFF request in the READY state returns a SUCCESS(0).

Return:
   0: SUCCESS - request was accepted
  -1: FAILURE - request denied
**/
int32 QDSSOff(void);


/**
 * Returns QDSS HWIO virtual address pages that should be locked down 
 * in TLB when running island mode.
 *
 * @param pVirtAddr    [out]Pointer to an array of uint32.
 *                     On return this array will have the
 *                     virtual addreses of pages to be 
 *                     locked down in TLB.
 * 
 * @param nAddrs       [in]Size of pVirtAddr array passed in -
 *                     i.e the maximum number of VirtualAddrs it can hold
 *
 * @param pnAddrsReq   [out]Pointer to an int.
 *                      On return this location will have 
 *                     the number of VirtAddrs returned in the pVirtAddr array
 *
 *                     If the array size is not sufficient to hold 
 *                     this value will indicate the size required.
 *
 * @return  0 on success. A non-zero error code on failure
 */
int QDSSGetUImageHWIOAddrs(uint32 *pVirtAddr,int nAddrs, int *pnAddrsReq); 

/**
 * Returns the STM stimulus port base virtual address for Sensor PD.
 *
 * @param pVirtualAddr
 *
 * @return  0 on success. A non-zero error code on failure
 */
int QDSSGetSTMBaseAddrSensor(uint32 *pVirtualAddr); 



/**
 * Returns the STM stimulus port base virtual address for ADSP base image
 *
 * @param pVirtualAddr
 *
 * @return  0 on success. A non-zero error code on failure
 */

int QDSSGetSTMBaseAddrADSP(uint32 *pVirtualAddr); 


/**
 * This function executes the ssc low power handshake enter sequence
 *
 * To be called before SLPI enters retention mode. Equivalent function
 * is called from qurt for full power collapse enter.
 */
void qdss_lpm_enter(void);


/**
 * This function executes the ssc low power handshake exit sequence
 *
 * To be called before SLPI exits retention mode. Equivalent function
 * is called from qurt for full power collapse exit.
 */
void qdss_lpm_exit(void); 

/**
 * This function executes the ssc island enter handshake sequence
 *
 * To be called while SSC enters island.
 */

void qdss_ssc_island_enter(void);

/**
 * This function executes the ssc island exit handshake sequence
 *
 * To be called when SSC exits island.
 */
void qdss_ssc_island_exit(void);


#endif /* QDSS_H */

