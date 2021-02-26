#ifndef SLIMBUSBAM_H
#define SLIMBUSBAM_H
/*
===========================================================================

FILE:         SlimBusBam.h

DESCRIPTION:  This file contains the function prototypes for SLIMbus 
              core driver BAM functionality.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/common/SlimBusBam.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------- 
03/29/17   RC      Added the API to clean the bam handles after BAM reset.
05/04/16   MJS     Add BAM deinit support.
02/17/16   MJS     Fix BAM descriptor coverage gap in Rx message queue.
10/14/15   MJS     Code portability changes for message FIFO index type.
05/05/15   NSD     Add API to get the number of used BAM IO Vector slots
08/08/14   MJS     64-bit support.
06/06/14   MJS     Add API to get the number of free IO vector slots.
02/28/14   MJS     Streamline ISR and don't lock device mutex in ISR.
08/19/13   MJS     Support for combined port DMA interrupt.
02/05/13   dj      Fix Code analysis warnings
11/29/12   MJS     Support BAM deferred processing within client context.
07/29/12   MJS     Updates for driver restructuring.
04/09/12   MJS     Fix SAL warnings.
01/24/12   MJS     Restructure for BAM library.
01/16/12   MJS     Change ResetBAM() parameters.
12/07/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBus.h"

/**
 * @brief Setup and configure message queues
 *
 * This function sets up and configures message queues, if 
 * message queue functionality is enabled. 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device
 * 
 * @return SB_SUCCESS if setup was successful, error code 
 *         otherwise
 */
_IRQL_requires_(DISPATCH_LEVEL)
extern SBResult SlimBus_SetupMsgQueues(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Transfer a single BAM IO vector
 *
 * This function transfers a single BAM IO vector
 * 
 * @param[in] pBamCtxt  Pointer to the BAM context
 * @param[in] uAddr  Physical start address for the transfer
 * @param[in] uSize  Size of the transfer in bytes
 * @param[in] pUser  User-defined pointer associated with the 
 *               transfer
 * @param[in] uFlags  Flags for the transfer
 * 
 * @return SB_SUCCESS on sucess, error code on error
 */
extern SBResult SlimBus_BamTransferIOVec
(
  SlimBusBamCtxt *pBamCtxt,
  SlimBusMsgIdxType uAddr,
  uint32 uSize,
  void *pUser,
  uint32 uFlags
);

/**
 * @brief Get number of free IO vector slots
 *
 * This function retrieves the number of free BAM IO vector
 * slots
 * 
 * @param[in] pBamCtxt  Pointer to the BAM context
 * 
 * @return The number of free BAM IO vector slots
 */
extern uint32 SlimBus_BamGetIOVecFreeSlots
(
  SlimBusBamCtxt *pBamCtxt
);

/**
 * @brief Get number of used IO vector slots
 *
 * This function retrieves the number of occupied BAM IO vector
 * slots
 * 
 * @param[in] pBamCtxt  Pointer to the BAM context
 * 
 * @return The number of used BAM IO vector slots
 */
extern uint32 SlimBus_BamGetIOVecActiveSlots
(
  SlimBusBamCtxt *pCtxt
);
/**
 * @brief Drain the BAM descriptor FIFO of IO vectors
 *
 * This function drains the BAM desriptor FIFO of IO vectors
 * 
 * @param[in] pMsgFifo  Pointer to the message FIFO to drain of 
 *                 IO vectors
 * 
 * @return SB_SUCCESS on sucess, error code on error
 */
extern SBResult SlimBus_EmptyBamIOVec(SlimBusMsgFifoType *pMsgFifo);

/**
 * @brief Reset the BAM hardware
 *
 * This function resets the BAM hardware
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
extern SBResult SlimBus_ResetBAM(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Detach from the BAM driver
 *
 * This function detaches from the BAM driver
 * 
 * @param[in,out] ppBamCtxt  Pointer to the pointer to the BAM 
 *       context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_DetachBAM(SlimBusBamCtxt **ppBamCtxt);


/**
 * @brief Disconnect the BAM port
 *
 * This function disconnects the Slimbus BAM port
 * 
 * @param[in] pPort  Pointer to the master port structure
 */
extern void SlimBus_DisconnectBAM(SlimBusMasterPortType *pPort);

/**
 * @brief Unregister a BAM port event
 *
 * This function unregisters a BAM port event
 * 
 * @param[in] pPort  Pointer to the master port structure
 */
extern void SlimBus_UnregisterBamPortEvent(SlimBusMasterPortType *pPort);

/**
 * @brief Clear port Bamhandle 
 *
 * This function update the BAM handle to NULL 
 * 
 * @param[in] pPort  Pointer to the master port structure
 */
void SlimBus_ClearBamhandle(SlimBusMasterPortType *pPort);

/**
 * @brief Get one processed descriptor for a BAM port
 *
 * This function retrieves on processed descriptor for a BAM
 * port
 * 
 * @param[in] pPort  Pointer to the master port structure
 */
extern SBBOOL SlimBus_GetOnePipeDesc(SlimBusMasterPortType *pPort);

/**
 * @brief Signal the client for a BAM event
 *
 * This function signals the client for a BAM event
 * 
 * @param[in] pPort  Pointer to the master port structure 
 */
extern void SlimBus_SignalBamEvent(SlimBusMasterPortType *pPort);

/**
  @brief Read the triggered events for a BAM pipe

  This function reads the events that have been triggered for a 
  BAM pipe.

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM
        pipe to read the event queue.  For bi-directional ports,
        there is one BAM pipe for each of the transmit and
        receive directions.
  @param[out] peEvent  Pointer to the location to store the BAM 
        events.

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_GetBamEvent(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamEventType *peEvent);

/**
 * @brief Initialize the BAM driver
 *
 * This function initializes the BAM driver and related software 
 * structures 
 * 
 * @param pDevCtxt Pointer to the SlimBus device context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_BamInit(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief De-initialize the BAM handle
 *
 * This function de-initializes the handle to the BAM driver
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_BamDeinit(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Perform deferred BAM processing that cannot be done 
 *        during BAM callbacks in order to avoid deadlock
 *
 * This function performs deferred BAM processing that cannot be 
 * done during BAM callbacks in order to avoid deadlock. 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device context
 */
extern void SlimBus_BamDoDeferredWork(SlimBusDevCtxt *pDevCtxt);

#endif /* SLIMBUSBAM_H */

