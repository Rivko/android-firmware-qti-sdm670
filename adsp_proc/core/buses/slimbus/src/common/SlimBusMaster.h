#ifndef SLIMBUSMASTER_H
#define SLIMBUSMASTER_H
/*
===========================================================================

FILE:         SlimBusMaster.h

DESCRIPTION:  This file contains the master function prototypes 
              for the SLIMbus core driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/common/SlimBusMaster.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/26/14   MJS     Purge satellite channels for non-responsive satellite.
03/19/14   MJS     Enhanced support for MDM restart.
03/21/13   MJS     Support for standalone satellite hardware configuration.
10/26/12   MJS     Add interrupt disable function.
09/21/12   MJS     Add interrupt enable function.
06/13/12   MJS     Support for standlone satellite low-power management.
12/11/11   MJS     Remove references to DAL.
10/12/11   AV      Added Slimbus SSR feature.
09/30/11   AV      Moved Slimbus to MIS process. 
05/24/11   MJS     Add pause clock support, separate hardware init function.
05/24/11   MJS     Add master-specific interrupt handling function.
11/01/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010, 2011, 2012, 2013, 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

struct SlimBusIsrCtxt;

/**
 * @brief Checks if the driver binary supports master-specific logic 
 *
 * This function checks whether the compiled driver binary supports 
 * master-specific logic
 * 
 * @return TRUE if master-specfic logic is supported, FALSE otherwise 
 */
extern SBBOOL SlimBus_IsMasterSupported( void );

/**
 * @brief Process a master-specific received message
 *
 * Process a master-specific received message 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] length  Length of the message
 * @param[in] data  First word of the received message
 * @param[in] pDataBuf  Pointer to the second word in the 
 *       received message buffer
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
extern SBResult SlimBus_ProcessMasterRxMsg
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 length,
  uint32 data,
  uint32 *pDataBuf
);

/**
 * @brief Handle master-specific interrupts
 *
 * This function processes master-specific interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pIsrCtxt  Pointer to the interrupt context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
extern SBResult SlimBus_HandleMasterInterrupts
(
  SlimBusDevCtxt *pDevCtxt,
  struct SlimBusIsrCtxt *pIsrCtxt
);

/**
 * @brief Mark internal state as having recovered from pause 
 *        clock
 *
 * This function marks the internal state as having recovered 
 * from pause clock. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_RecoverFromPauseClock(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Perform master-specific device driver initialization 
 *
 * This function performs master-specific device driver 
 * initialization 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
extern SBResult SlimBus_DeviceInitMaster(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Register callback for Subsystem Restart(SSR)
 *
 * This function registers callbacks for Subsystem Restart (SSR)
 * 
 * @param[in] h Client handle to the device
 * 
 * @return SB_SUCCESS 
 */
extern SBResult SlimBus_RegisterSSRcb(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Reset routine before SSR restart is performed.
 *  
 * This function does the cleanup on Subsystem Reset 
 * 
 * @param[in] pDevCtxt      Device Ctxt 
 * 
 * @return None 
 */
extern void SlimBus_SSReset(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Restart routine after SSR restart is done.
 *  
 * This function does the re-initialization on Subsystem Restart
 * 
 * @param[in] pDevCtxt      Device Ctxt 
 * 
 * @return None 
 */
extern void SlimBus_SSRestart(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Callback for SlimBus master workloop for handling 
 *        remote reconfiguration sequences
 *
 * Callback function for SlimBus master workloop for handling 
 * remote reconfiguration sequences 
 * 
 * @param[in] hEvent  Workloop event handle
 * @param[in] pVoid  Workloop callback context
 * 
 * @return SB_SUCCESS 
 */
extern SBResult SlimBusMasterWorkLoopHandler(SlimBusEventHandle hEvent, void *pVoid);

/**
 * @brief Signal that next reconfiguration sequence 
 *        can be sent
 *
 * This function signals either a waiting thread or the 
 * master workloop to process the next pending 
 * reconfiguration sequence.
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
extern void SlimBus_SignalNextReconfig(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Determine whether any satellite clients have active 
 *        data channels
 *
 * This function determines whether any clients corresponding to
 * a particular satellite have active data channels 
 * 
 * @param[in] paClientCtxt  Pointer to array of client context 
 *       pointers
 * @param[in] uNumClientSlots  Number of slots in the array 
 * 
 * @return TRUE if any clients have active data channels, 
 *         FALSE otherwise
 */
extern SBBOOL SlimBus_AreSatChannelsActive
(
  SlimBusBasicClientCtxt **paClientCtxt,
  uint32 uNumClientSlots
);

/**
 * @brief Queue master message event, send message and wait for 
 *        the event
 *
 * This function queues the event structure for a 
 * master-workloop generated message, sends the message and 
 * waits for the message completion event. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pBuf  Pointer to the message buffer
 * 
 * @return SLIMBUS_ERR_NONE on success, error code on error 
 */
extern SlimBusErrorType SlimBus_MasterSendAndWaitForMsgEvent
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 *pBuf
);

#endif /* SLIMBUSMASTER_H */

