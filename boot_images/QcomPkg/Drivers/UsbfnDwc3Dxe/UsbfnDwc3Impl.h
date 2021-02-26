/**
 * @file UsbfnDwc3Impl.h
 *
 * @brief Usb device mode implementation functions
 *
 * This header declares synopsys core specific implementation of much of the
 * functionallity needed to implement the UsbfnIo protocol.
 *
 * @copyright Copyright (C) 2013,2016 - 2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/09/17   ck      Work around the issue that LPM Not Rejected During Control Transfer
 01/22/17   ck      Port the fix for SetConfig handling
 11/20/14   ml      Migrate to official SuperSpeed support
 10/07/14   wufan   Add support of HS Test Mode for compliance testing 
 05/20/14   ar      Add PCD value to override max bus speed 
 08/23/13   mliss   Cleanup and stabilization
 01/20/11   cching  Initial rework for Synopsys Usbfn driver
 =============================================================================*/

#ifndef USBFNDWC3IMPL_H_
#define USBFNDWC3IMPL_H_

#include "UsbfnDwc3.h"


// DWC timeout experience values

#define DWC_1_MICROSECOND             1
#define DWC_1_MILLISECOND             (1000 * DWC_1_MICROSECOND)
#define DWC_1_SECOND                  (1000 * DWC_1_MILLISECOND)

#define DWC_WAIT_TO_CLEAR             0
#define DWC_WAIT_TO_SET               1
#define DWC_WAIT_TIME_OUT             500 * DWC_1_MILLISECOND
#define DWC_SYNC_POLL_INTERVAL        (1 * DWC_1_MILLISECOND)

#define DWC_EVENT_BUFFER_PAGE_NUM     (1)
#define DWC_PAGE_SIZE                 (4096)
#define DWC_EVENT_BUFFER_SIZE         (DWC_EVENT_BUFFER_PAGE_NUM*DWC_PAGE_SIZE)
#define DWC_DEVICE_ONLY_CONFIG        (2)

#define DWC_DEV_SPEED_SS              (4)
#define DWC_DEV_SPEED_HS              (0)
#define DWC_DEV_SPEED_FS              (1)
#define DWC_DEV_SPEED_INVALID         (7)
#define DWC_DEV_SPEED_DEFAULT         DWC_DEV_SPEED_HS
  
#define USBFN_DWC_TRB_DEFAULT_NUM     3

#define USBFN_DELAYED_RESET_MAX_TRIES 100

#define USBFN_MAX_BURST_IN_ENDPOINTS  6 // from HPG, max which allows use of all TX eps
#define USBFN_TXFIFO_BURST_SIZE       0x184
#define USBFN_TXFIFO_NOBURST_SIZE     0x82

typedef struct _USBFN_URB_NODE {

  LIST_ENTRY           Link;
  DWC_URB              Urb;
  UINT8                ZltBuffer[1024]; // buffer to sink zero length packet from
  UINT32               TotalTransactionLength;
                                        // host on OUT endpoint transfers
  SCATTER_GATHER_LIST  *SgList;

  // Used to store client's buffer to return in event
  VOID                 *ClientBuffer;
} DWC_URB_NODE;


/**
 * @brief Free software transfer resources
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
VOID
UsbfnDwcFreeXferRsc (
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Initialize an instance of the SNPS controller core
 *
 * Initialize an instance of the SNPS controller core
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 *
 * @retval EFI_SUCCESS          Initialization successful
 * @retval others               Failed to initialize
 */
EFI_STATUS
UsbfnDwcCoreInitImpl(
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief The dispatch function for device and endpoint events
 *
 * Reads a hardware event and dispatches it based on type.
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
VOID
UsbfnDwcEventHandler(
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Handle the SET_SEL standard request
 *
 * After receiving SET_SEL, issue Set Periodic Parameter device command.
 * The SNPS controller databook (6.3.1.6.1) indicates that if the device
 * is enabled for U1 and U2, the U2PEL should be programmd into into
 * SystemExitLatency. If the device is enabled only for U1, the U1PEL should
 * be programmed. However, Windows sends SET_SEL request before sending
 * SetFeature(U1_ENABLE) and/or SetFeature(U1_ENABLE). To simplify the logic,
 * U2PEL is always used.
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 *
 * @retval EFI_SUCCESS          Initialization successful
 * @retval others               Failed to initialize
 */
EFI_STATUS
UsbfnDwcHandleSetSel(
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Handle a request to enable U1 link state
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
EFI_STATUS
UsbfnDwcHandleU1Enable(
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Handle a request to enable U2 link state
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
EFI_STATUS
UsbfnDwcHandleU2Enable(
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Handle the SET_ADDRESS standard request
 *
 * Program the address assigned by the host into the SNPS controller
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 *
 * @retval EFI_SUCCESS          Initialization successful
 * @retval others               Failed to initialize
 */
EFI_STATUS
UsbfnDwcHandleSetAddress(
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Perform the necessary steps when USB RESET
 * event is received
 *
 * Modify the SNPS controller settings required when USB RESET
 * event is received
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Evt             Common layer reset event
 *
 * @retval EFI_SUCCESS          The operation completes successfully
 * @retval others               The operation fails
 */
EFI_STATUS
UsbfnDwcHandleReset(
  IN USBFN_DEV                  *Usbfn,
  IN DWC_EVT                    *Evt
  );


/**
 * @brief Perform the necessary steps when DISCONNECT
 * event is received
 *
 * Modify the SNPS controller settings required when DISCONNECT
 * event is received
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 *
 * @retval EFI_SUCCESS          The operation completes successfully
 * @retval others               The operation fails
 */
EFI_STATUS
UsbfnDwcHandleDisconnect(
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Handle a request to enable HS test mode.
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
EFI_STATUS
UsbfnDwcEnableHsTestMode(
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Get a free Urb node from the Urb node pool
 *
 * Get a free Urb node from the Urb node pool.
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [out] UrbNode         The Urbnode which will be returned to the client
 *
 * @retval EFI_SUCCESS          The operation completes successfully
 * @retval others               The operation fails
 */
VOID
UsbfnGetFreeUrb(
  IN  USBFN_DEV                 *Usbfn,
  OUT DWC_URB_NODE              **UrbNode
  );


/**
 * @brief Remove a Urbnode from the active transfer list
 *
 * Remove the Urbnode from the active transfer list
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint to which the Urb node belongs to
 * @param [in]  EpDir           The direction of the endpoint
 * @param [out] UrbNode         The Urbnode which will be removed from the active
 *                              transfer list
 */
VOID
UsbfnDwcRemoveActiveTransferFromList(
  IN  USBFN_DEV                 *Usbfn,
  IN  UINT8                     Ep,
  IN  UINT8                     EpDir,
  OUT DWC_URB_NODE              **UrbNode
  );


/**
 * @brief Generate EfiUsbMsgEndpointStatusChangedRx or EfiUsbMsgEndpointStatusChangedTx
 * message to inform the client that the transfer has been completed
 *
 * See UpdateDeviceInfo for an overall description of request hijacking.
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint of which the stall state to be modified.
 * @param [in]  EpDir           The direction of the endpoint specifed by Ep
 * @param [in]  Canceled        Whether the transfer was cancelled or completed
 *
 * @retval EFI_SUCCESS          Initialization successful
 * @retval others               Failed to initialize
 */
EFI_STATUS
GenerateXferCompltMsg(
  IN  USBFN_DEV                 *Usbfn,
  IN  UINT8                     Ep,
  IN  UINT8                     EpDir,
  IN  BOOLEAN                   Canceled
  );


/**
 * @brief transfer data in the data stage or ack in the status stage of a control transfer
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint of which the stall state to be modified.
 * @param [in]  EpDir           The direction of the endpoint specifed by Ep
 * @param [in]  TrbCtl          The TRB control code associated with the transfer
 *
 * @retval EFI_SUCCESS          Initialization successful
 * @retval others               Failed to initialize
 */
EFI_STATUS
UsbfnDwcSendCtrlXfer(
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN UINT8                      EpDir,
  IN CONST TRB_CTL_TYPE         TrbCtl
  );


/**
 * @brief Stall/Clear Stall an ep point on the SNPS controller.
 *
 * Stall / Clear Stall the ep point specified.
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint of which the stall state to be modified.
 * @param [in]  EpDir           The direction of the endpoint specifed by Ep
 * @param [in]  State           TRUE: endpoint is stalled. FALSE: endpoint is unstalled.
 *
 * @retval EFI_SUCCESS          Initialization successful
 * @retval others               Failed to initialize
 */
EFI_STATUS
UsbfnDwcSetEndpointStallState (
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN DWC_ENDPOINT_DIR           EpDir,
  IN BOOLEAN                    State
  );


/**
 * @brief Set the Configuration or Interface specified by the host into the SNPS
 * controller
 *
 * Set the Configuration or Interface specified by the host into the SNPS
 * controller
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 *
 * @retval EFI_SUCCESS          The operation completes successfully
 * @retval others               The operation fails
 */
EFI_STATUS
UsbfnDwcSetConfigIfc(
  IN  USBFN_DEV                 *Usbfn
  );


/**
 * @brief Configure an endpoint used in the current configuration
 *
 * Configure an endpoint used in the current configuration.
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint to be configured
 * @param [in]  EpDir           The direction of the endpoint to be configured
 * @param [in]  EpType          The type of the endpoint
 * @param [in]  MaxPktLength    The max packet size of the endpoint
 * @param [in]  StreamCapable   Indication that the endpoint is stream-capable
 * @param [in]  BurstSize       The burst size of the endpoint
 * @param [in]  bInterval       The bInterval value for the endpoint.Only used
 *                              if the endpoint type is INTERRUPT
 *
 * @retval EFI_SUCCESS          The operation completes successfully
 * @retval others               The operation fails
 */
EFI_STATUS
UsbfnDwcConfigEndpoints(
  IN  USBFN_DEV                 *Usbfn,
  IN  UINT8                     Ep,
  IN  UINT8                     EpDir,
  IN  UINT8                     EpType,
  IN  UINT16                    MaxPktLength,
  IN  BOOLEAN                   StreamCapable,
  IN  UINT8                     BurstSize,
  IN  UINT8                     bInterval
  );


/**
 * @brief Perform the steps to set up a USB transfer
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint on which the transfer will be queued
 * @param [in]  EpDir           The direction of the endpoint
 * @param [in]  BufferSize      The size of the transfer buffer
 * @param [in]  Buffer          The transfer buffer (may not be from clinet)
 * @param [in]  ClientBuffer    The actual buffer passed by client
 *
 * @retval EFI_SUCCESS          The transfer has been queued
 * @retval others               Failed to queue transfer
 */
EFI_STATUS
UsbfnDwcTransfer(
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN UINT8                      EpDir,
  IN UINT32                     BufferSize,
  IN VOID                       *Buffer,
  IN VOID                       *ClientBuffer
  );


/**
 * @brief Abort transfers on the endpoint
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint on which the transfer was queued
 * @param [in]  EpDir           The direction of the endpoint
 *
 * @retval EFI_SUCCESS          The operation completes successfully
 * @retval others               The operation fails
 */
EFI_STATUS
UsbfnDwcCancelTransfer(
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN UINT8                      EpDir
  );


/**
 * @brief Request DMA to stop for all of the endpoints which currently having
 *  pending transfers, except for physicall endpoints 0 & 1.
 *
 * Clear the stalled state for all endpoints
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
VOID
UsbfnDwcCancelActiveTransfers (
  IN USBFN_DEV                  *Usbfn
  );


/**
 * @brief Clear the stalled state for all endpoints
 *
 * Clear the stalled state for all endpoints
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 *
 */ 
VOID
UsbfnDwcClearAllStallEndpoints (
  IN USBFN_DEV                  *Usbfn
  );
#endif /* USBFNDWC3IMPL_H_ */

