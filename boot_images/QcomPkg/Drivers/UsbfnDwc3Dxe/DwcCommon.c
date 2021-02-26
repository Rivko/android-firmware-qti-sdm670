/**
 * @file DwcCommon.c
 *
 * @ingroup CommonLayer
 *
 * @brief Device mode common layer implementation.
 *
 * Implements low-level functionality for the Sysnopsys USB core used in function driver implementations.
 *
 * @copyright Copyright (C) 2013-2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/09/17   ck      Work around the issue that LPM Not Rejected During Control Transfer
 02/01/17   pm      Port from BOOT.XF.1.4
 11/09/16   ck      Reset TRB ring for ClearStall
 11/01/16   ck      Enhance debug message and conditionally fix the STALL issue
 09/22/16   ck      Fix ch9 test failure
 07/29/16   ck      Check non-null pointer before free
 01/22/16   ck      Port USB SetConfig fix
 12/18/14   ck      Address software & hardware transfer cancel notification issue
 10/09/14   amitg   Added Memory Barrier so that TRB structure operations 
                    are performed prior passing to controller
 06/26/14   amitg   Address 64 bit issue for FFU enumeration
 06/12/14   ck      Address pending setup pkt and End Transfer issues
 04/30/14   ar      Cleanup for NDEBUG build  
 08/23/13   mliss   Initial import
 =============================================================================*/

#include "DwcCommon.h"
#include "DwcPlatform.h"
#include "DwcHwio.h"
#include "Library/ArmLib.h"

//
// TRB pointer increment/decrement macros
//
#define DWC_INCR_TRB_PTR(ring,trb)                                      \
  {                                                                     \
    (trb)++;                                                            \
    if ((UINT64)(trb) >= ((UINT64)(ring)->Buf + (ring)->BufSize)) {     \
      (trb) = (DWC_TRB *)(ring)->Buf;                                   \
    }                                                                   \
  }

#define DWC_DECR_TRB_PTR(ring,trb)                                      \
  {                                                                     \
    (trb)--;                                                            \
    if ((UINTN)(trb) < (UINTN)(ring)->Buf) {                          \
      (trb) = ((DWC_TRB *)((UINTN)(ring)->Buf + (ring)->BufSize)) - 1; \
    }                                                                   \
  }

//
// Check if a TRB ring is full
//
#define DWC_TRB_RING_FULL_CHECK(ring)                                   \
    if (((ring)->NumTRBs > 0) && ((ring)->Enq == (ring)->Deq)) {        \
      Status = DWC_TRB_RING_FULL;                                       \
      goto ON_EXIT;                                                     \
    }


/*
 * See header file for description.
 */
DWC_STATUS
DwcGetCoreInfo (
  IN  DWC_DEVICE     *Dev,
  OUT DWC_CORE_INFO  *Info )
{
  if( NULL == Info || NULL == Dev ) {
    TraceErr(CommonTrb, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  // Return Synopsys Core version info
  *Info = Dev->CoreInfo;
  return DWC_SUCCESS;
}


/**
 * @brief Get the physical address for a TRB
 *
 * Calculate the physical address of a particular TRB inside a Trb Ring.
 *
 * @param [in]  Ring              Ring containing Trb
 * @param [in]  Trb               Trb to get physical address of
 * @param [out] PhysicalAddress   Returned physical address
 *
 * @return DWC_SUCCESS            PhysicalAddress is valid
 * @return others                 Failed to calculate PhysicalAddress
 */
STATIC DWC_STATUS
DwcGetTrbPhysicalAddress (
  IN  DWC_TRB_RING          *Ring,
  IN  DWC_TRB               *Trb,
  OUT UINT64                *PhysicalAddress
  )
{
  UINTN    BufferOffset;

  // validate parameters
  if ((NULL == Ring) || (NULL == Trb) || (NULL == PhysicalAddress) ||
      ((UINTN)Trb < (UINTN)Ring->Buf) || ((UINTN)Trb >= (UINTN)Ring->Buf + Ring->BufSize)) {
    TraceErr(CommonTrb, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  BufferOffset     = (UINTN)Trb - (UINTN)Ring->Buf;
  *PhysicalAddress = Ring->BufAddress.Physical + BufferOffset;

  return DWC_SUCCESS;
}


/**
 * @brief Reset a TRB ring
 *
 * This common layer internal function resets a TRB ring to its initial
 * state.  It is only intended to be used after an end transfer command
 * as there is a chance the ring could be in a state that the SyncRing
 * function will not be able to reclaim the TRBs.
 *
 * @param [in] Ring         TRB ring to update
 */
STATIC VOID
DwcResetTrbRing (
  IN  DWC_TRB_RING          *Ring
  )
{
  DWC_TRB   *Trb;

  // validate parameter
  if (NULL == Ring) {
    TraceErr(CommonTrb, "invalid parameter");
    return;
  }

  // reset the ring's pointers
  Ring->NumTRBs = 0;
  Ring->Enq     = (DWC_TRB *)Ring->Buf;
  Ring->Deq     = (DWC_TRB *)Ring->Buf;

  // loop through the TRBs and clear the HWO bit
  Trb = Ring->Enq;
  if (NULL != Trb) {
    do {
        Trb->HWO = 0;
        DWC_INCR_TRB_PTR(Ring, Trb);
    } while (Trb != Ring->Enq);

    // make sure the last TRB in the ring is a link TRB type
    DWC_DECR_TRB_PTR(Ring, Trb);
    Trb->TRBCTL = DWC_TRB_CTL_LINK_TRB;
  }
}


/**
 * @brief Synchronize a TRB ring
 *
 * This common layer internal function updates the dequeue pointer of the
 * TRB ring to follow the last TRB in the ring NOT yet processed by the
 * hardware.  The ShortPktRcvd param is for detection of a "fast-forward"
 * condition.
 *
 * @param [in] Ring         TRB ring to update
 * @param [in] ShortPktRcvd Has a short packet been received?
 *
 * @return DWC_SUCCESS  TRB ring synchronized
 * @return others       Failed to synchronize ring
 */
STATIC DWC_STATUS
DwcSyncTrbRing (
  IN  DWC_TRB_RING          *Ring,
  IN  BOOLEAN               ShortPktRcvd
  )
{
  BOOLEAN   Chain = 0;

  // validate parameters
  if (NULL == Ring || NULL == Ring->Deq) {
    TraceErr(CommonTrb, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  //
  // Loop through the TRBs at the dequeue pointer and advance if they aren't owned by
  // the hardware anymore.  If a short packet has been received and we are in the
  // middle of a chain of TRBs and encounter one with HWO still set, this is a
  // "fast-forward" condition so continue advancing to the end of the chain.
  //
  Chain = (BOOLEAN)Ring->Deq->CHN;
  while ((Ring->NumTRBs > 0) && (NULL != Ring->Deq) && ((!Ring->Deq->HWO) ||
         (Ring->Deq->TRBCTL == DWC_TRB_CTL_LINK_TRB) || (ShortPktRcvd && Chain))) {

    // clear the HWO bit for the case of fast-forward and link TRBs
    Ring->Deq->HWO = 0;

    // link TRBs have CHN cleared, but we need to keep going in fast-forward condition
    if (Ring->Deq->TRBCTL == DWC_TRB_CTL_LINK_TRB) {
        Chain = 1;
    } else {
        Chain = (BOOLEAN)Ring->Deq->CHN;
    }

    Ring->NumTRBs--;
    DWC_INCR_TRB_PTR(Ring, Ring->Deq);
  }

  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
VOID
DwcDumpRegs (
  IN  DWC_DEVICE            *Dev,
  IN  DWC_REG_DESC          RegsToDump[],
  IN  UINT32                RegCount
  )
{
  UINT32    RegVal;
  UINT32    i;

  if ((NULL == Dev) || (NULL == RegsToDump) || (0 == RegCount)) {
    TraceErr(CommonDebug, "invalid parameter");
    return;
  }
  (void)RegVal; // Make compiler happy

  for (i=0; i < RegCount; i++) {
    RegVal = DWC_PLAT_READ_REG(Dev->CoreBaseAddr + RegsToDump[i].Offset);
    TraceInfo(CommonDebug, "%s = 0x%08x", RegsToDump[i].Name, RegVal);
  }
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcInitialize (
  OUT DWC_DEVICE            *Dev,
  IN  UINTN                 BaseAddr
  )
{
  UINT32  EpIndex;

  // validate parameters
  if ((0 == BaseAddr) || (NULL == Dev)) {
    TraceErr(CommonInit, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  Dev->CoreBaseAddr = (UINT32)BaseAddr;
  Dev->CoreInfo.SynopsysID = DWC_HWIO_INF(Dev->CoreBaseAddr, GSNPSID, SYNOPSYSID );

  // get number of event buffers supported
  Dev->DwcUsb3DeviceNumInt = (UINT8)DWC_HWIO_INF(
                                  (UINT32)BaseAddr,
                                  GHWPARAMS1,
                                  GHWPARAMS1_20_15
                                );
  if (Dev->DwcUsb3DeviceNumInt == 0) {
    TraceCrit(CommonInit, "0 event buffers supported by core!");
    return DWC_DEVICE_ERROR;
  }
  TraceInfo(CommonInit, "Core base address: 0x%08x, %d event buffers supported",
      Dev->CoreBaseAddr, Dev->DwcUsb3DeviceNumInt);

  // allocate array of event buffers
  Dev->EvtBuffer = (DWC_EVENT_BUFFER *)DWC_ALLOC(sizeof(DWC_EVENT_BUFFER) * Dev->DwcUsb3DeviceNumInt);
  if (NULL == Dev->EvtBuffer) {
    TraceErr(CommonInit, "memory allocation failed");
    return DWC_OUT_OF_MEMORY;
  }
  Dev->EvtBufferCount = 0;

  // per endpoint data
  for (EpIndex = 0; EpIndex < DWC_USB_MAX_ENDPOINTS; EpIndex++) {
    Dev->Ep[EpIndex].State       = DWC_EP_STATE_UNINITIALIZED;
    Dev->Ep[EpIndex].ResourceID  = DWC_EP_RESOURCE_ID_INVALID;
    Dev->Ep[EpIndex].EvtBufferID = DWC_EP_EVENT_BUFFER_ID_INVALID;
    Dev->Ep[EpIndex].Ring        = NULL;
    Dev->Ep[EpIndex].Urb         = NULL;
  }

  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcDeconfigureNonCtrlEPs (
  IN DWC_DEVICE             *Dev
  )
{
   UINT32 Index;
   const UINT8 NON_CTL_EP_ST_IDX = 2;
  // validate parameters
  if (NULL == Dev) {
    return DWC_INVALID_PARAMETER;
  }

  // clear data and free memory for each enabled endpoint
  for (Index = NON_CTL_EP_ST_IDX; Index <= DWC_USB_MAX_ENDPOINT_NUMBER; Index++) {
    if (Dev->Ep[Index].State != DWC_EP_STATE_UNINITIALIZED) {
      Dev->Ep[Index].State       = DWC_EP_STATE_UNINITIALIZED;
      Dev->Ep[Index].ResourceID  = DWC_EP_RESOURCE_ID_INVALID;
      Dev->Ep[Index].EvtBufferID = DWC_EP_EVENT_BUFFER_ID_INVALID;
      Dev->Ep[Index].MaxPktSize  = 0;

      Dev->Ep[Index].Ring->Buf     = NULL;
      Dev->Ep[Index].Ring->BufSize = 0;
      Dev->Ep[Index].Ring->Enq     = NULL;
      Dev->Ep[Index].Ring->Deq     = NULL;
      Dev->Ep[Index].Ring->NumTRBs = 0;

      if (Dev->Ep[Index].Ring) {
        DWC_FREE(Dev->Ep[Index].Ring);
        Dev->Ep[Index].Ring = NULL;
      }
      Dev->Ep[Index].Urb  = NULL;
      TraceInfo(CommonDebug, "EpIndex %d, State %d", Index, Dev->Ep[Index].State);
    }
  }
  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcCleanDevice (
  IN DWC_DEVICE             *Dev
  )
{
  UINT32 Index;

  // validate parameters
  if (NULL == Dev) {
    return DWC_INVALID_PARAMETER;
  }

  Dev->CoreBaseAddr        = 0;
  Dev->DwcUsb3DeviceNumInt = 0;

  // clear data from initialized event buffers
  for (Index = 0; Index < Dev->EvtBufferCount; Index++) {
    Dev->EvtBuffer[Index].Buf     = NULL;
    Dev->EvtBuffer[Index].NextEvt = NULL;
    Dev->EvtBuffer[Index].Size    = 0;
  }
  Dev->EvtBufferCount = 0;

  // free the event buffer array
  if (Dev->EvtBuffer) {
    DWC_FREE(Dev->EvtBuffer);
    Dev->EvtBuffer = NULL;
  }

  // clear data and free memory for each enabled endpoint
  for (Index = 0; Index <= DWC_USB_MAX_ENDPOINT_NUMBER; Index++) {
    if (Dev->Ep[Index].State != DWC_EP_STATE_UNINITIALIZED) {
      Dev->Ep[Index].State       = DWC_EP_STATE_UNINITIALIZED;
      Dev->Ep[Index].ResourceID  = DWC_EP_RESOURCE_ID_INVALID;
      Dev->Ep[Index].EvtBufferID = DWC_EP_EVENT_BUFFER_ID_INVALID;
      Dev->Ep[Index].MaxPktSize  = 0;

      Dev->Ep[Index].Ring->Buf     = NULL;
      Dev->Ep[Index].Ring->BufSize = 0;
      Dev->Ep[Index].Ring->Enq     = NULL;
      Dev->Ep[Index].Ring->Deq     = NULL;
      Dev->Ep[Index].Ring->NumTRBs = 0;

      if (Dev->Ep[Index].Ring) {
        DWC_FREE(Dev->Ep[Index].Ring);
        Dev->Ep[Index].Ring = NULL;
      }
      Dev->Ep[Index].Urb  = NULL;
    }
  }

  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcConfigureEventBuffer (
  IN  DWC_DEVICE            *Dev,
  IN  DWC_ADDRESS           EvtBuffer,
  IN  UINT32                EvtBufferSize,
  IN  BOOLEAN               EnableInterrupt,
  OUT UINT8                 *EvtBufferIndex
  )
{
  UINT8     EvtBufInd;

  // validate parameters and their alignment
  if ((NULL == Dev) || (NULL == EvtBufferIndex) ||
      (0 == EvtBufferSize) || (0 != (EvtBufferSize & DWC_EVENT_BUFFER_ALIGN_MASK)) ||
      (0 == EvtBuffer.Physical) || (0 == EvtBuffer.Virtual) ||
      (0 != (EvtBuffer.Physical % EvtBufferSize))) {
    TraceErr(CommonInit, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  // make sure we haven't allocated all event buffers already
  if (Dev->EvtBufferCount >= Dev->DwcUsb3DeviceNumInt) {
    TraceErr(CommonInit, "the number of event buffers exceeds the hw limit");
    return DWC_OUT_OF_MEMORY;
  }

  // initialize event buffer structure
  EvtBufInd       = Dev->EvtBufferCount++;
  *EvtBufferIndex = EvtBufInd;

  Dev->EvtBuffer[EvtBufInd].Buf     = (VOID *)EvtBuffer.Virtual;
  Dev->EvtBuffer[EvtBufInd].NextEvt = (UINT32 *)EvtBuffer.Virtual;
  Dev->EvtBuffer[EvtBufInd].Size    = EvtBufferSize;

  // write the event buffer address
  DWC_HWIO_OUTI(
        Dev->CoreBaseAddr,
        GEVNTADRHI,
        EvtBufInd,
        (UINT32)((UINT64)EvtBuffer.Physical >> 32 & 0xFFFFFFFF)
      );
  DWC_HWIO_OUTI(
        Dev->CoreBaseAddr,
        GEVNTADRLO,
        EvtBufInd,
        (UINT32)((UINT64)EvtBuffer.Physical & 0xFFFFFFFF)
      );

  // write the event buffer size (with interrupt mask)
  if (!EnableInterrupt) {
    EvtBufferSize |= DWC_EVENT_BUFFER_SIZE_INT_MASK;
  }
  DWC_HWIO_OUTI(
        Dev->CoreBaseAddr,
        GEVNTSIZ,
        EvtBufInd,
        EvtBufferSize
      );

  // write zero to the event count register to enable event buffer
  DWC_HWIO_OUTI(
        Dev->CoreBaseAddr,
        GEVNTCOUNT,
        EvtBufInd,
        0
      );

  TraceInfo(CommonInit, "event buffer %d, size %d enabled", EvtBufInd, EvtBufferSize & 0x7FFFFFFF);
  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcUpdateEPMaxPktSize(
    IN  DWC_DEVICE            *Dev,
    IN  UINT8                 Ep,
    IN  UINT8                 EpDir,
    IN  UINT32                MaxPktSize
)
{
  UINT8       EpIndex;
  // validate parameters and their alignment
  if ((NULL == Dev) || (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) ||
      (MaxPktSize > DWC_USB_MAXIMUM_MAXPKTSIZE) ||
      (EpDir >= DWC_EP_DIR_MAX) ){
      TraceErr(CommonInit, "invalid parameter");
      return DWC_INVALID_PARAMETER;
  }
  EpIndex = DWC_EP_INDEX (Ep, EpDir);

  // set the max pkt size
  Dev->Ep[EpIndex].MaxPktSize = MaxPktSize;
  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcConfigureEP (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  IN  UINT32                MaxPktSize,
  IN  UINT8                 EvtBufferIndex,
  IN  DWC_ADDRESS           TrbBuffer,
  IN  UINT32                TrbBufferSize
  )
{
  UINT8         EpIndex;
  DWC_TRB_RING  *TrbRing;
  DWC_TRB       *Trb;

  // validate parameters and their alignment
  if ((NULL == Dev) || (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) ||
      (EpDir >= DWC_EP_DIR_MAX) || (EvtBufferIndex >= Dev->EvtBufferCount) ||
      (TrbBufferSize < DWC_TRB_RING_MIN_BUFFER_SIZE) ||
      (0 != (TrbBufferSize & DWC_TRB_ALIGN_MASK)) ||
      (0 == TrbBuffer.Physical) || (0 == TrbBuffer.Virtual) ||
      (MaxPktSize > DWC_USB_MAXIMUM_MAXPKTSIZE) ||
      (0 != (TrbBuffer.Physical % DWC_TRB_ALIGN_SIZE))) {
    TraceErr(CommonInit, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  EpIndex = DWC_EP_INDEX (Ep, EpDir);

  // make sure endpoint isn't already enabled
  if (Dev->Ep[EpIndex].State != DWC_EP_STATE_UNINITIALIZED) {
    TraceErr(CommonInit, "physical endpoint %d already configured", EpIndex);
    return DWC_ALREADY_INITIALIZED;
  }

  // set the max pkt size
  Dev->Ep[EpIndex].MaxPktSize = MaxPktSize;

  // initialize the TRB Ring for this endpoint
  TrbRing = (DWC_TRB_RING *)DWC_ALLOC(sizeof(DWC_TRB_RING));
  if (NULL == TrbRing) {
    TraceErr(CommonInit, "memory allocation failed");
    return DWC_OUT_OF_MEMORY;
  }

  TrbRing->BufAddress = TrbBuffer;
  TrbRing->Buf        = (VOID *)TrbBuffer.Virtual;
  TrbRing->BufSize    = TrbBufferSize;
  TrbRing->Enq        = (DWC_TRB *)TrbBuffer.Virtual;
  TrbRing->Deq        = (DWC_TRB *)TrbBuffer.Virtual;
  TrbRing->NumTRBs    = 0;

  // populate the last trb in the ring as a link TRB
  Trb = (DWC_TRB *)(TrbBuffer.Virtual + TrbBufferSize) - 1;

  Trb->BufPtrLo = (UINT64)TrbRing->BufAddress.Physical & 0xFFFFFFFF;
  Trb->BufPtrHi = (UINT64)TrbRing->BufAddress.Physical >> 32 & 0xFFFFFFFF;
  Trb->BufSize  = 0;
  Trb->PCM1     = 0;
  Trb->Rsvd0    = 0;
  Trb->TRBSTS   = 0;
  Trb->HWO      = 0;
  Trb->LST      = 0;
  Trb->CHN      = 0;
  Trb->CSP      = 0;
  Trb->TRBCTL   = DWC_TRB_CTL_LINK_TRB;
  Trb->ISP_IMI  = 0;
  Trb->IOC      = 0;
  Trb->Rsvd1    = 0;
  Trb->SID_SOFN = 0;
  Trb->Rsvd2    = 0;

  Dev->Ep[EpIndex].Ring = TrbRing;

  // store mapping for event buffer
  Dev->Ep[EpIndex].EvtBufferID = EvtBufferIndex;

  Dev->Ep[EpIndex].State = DWC_EP_STATE_INACTIVE;

  TraceInfo(CommonInit, "physical endpoint %d uses event buffer %d, max packet size %d, TRB buffer size %d, Dev->Ep[%d].Ring 0x%x",
      EpIndex, EvtBufferIndex, MaxPktSize, TrbBufferSize, EpIndex, Dev->Ep[EpIndex].Ring);

  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcSetEndpointStallState(
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  IN  BOOLEAN               IsStall
  )
{
  DWC_STATUS    Status;
  DWC_EP_CMD    EpCmd;
  UINT8         EpIndex;
  DWC_EP_STATE  OldState;

  // validate parameters
  if ((NULL == Dev) || (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) || (EpDir >= DWC_EP_DIR_MAX)) {
    TraceErr(CommonCommand, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  EpIndex = DWC_EP_INDEX(Ep, EpDir);

  // make sure the endpoint is initialized
  if (Dev->Ep[EpIndex].State == DWC_EP_STATE_UNINITIALIZED) {
    TraceErr(CommonCommand, "physical endpoint %d is uninitialized", EpIndex);
    return DWC_ALREADY_INITIALIZED;
  }

  // if the ep is 0, make sure the direction is out and the command is Set Stall
  if (((0 == EpIndex) && !IsStall) || (1 == EpIndex)) {
    TraceErr(CommonCommand, "can only SET stall on the OUT direction of control ep 0");
    return DWC_INVALID_PARAMETER;
  }

  // cache endpoint state to revert on failure
  OldState = Dev->Ep[EpIndex].State;

  // set stall state prior to issuing command
  if (IsStall) {
    Dev->Ep[EpIndex].State = DWC_EP_STATE_STALLED;
  }

  // if there is a transfer in progress it needs to be cancelled prior to issuing a clear stall
  if (!IsStall && (Dev->Ep[EpIndex].State == DWC_EP_STATE_XFER_IN_PROGRESS)) {
    TraceInfo(CommonCommand, "issuing EndXfer command prior to ClearStall");

    // zero the command
    EpCmd.Template.Cmd.Dword  = 0;
    EpCmd.Template.Parameter0 = 0;
    EpCmd.Template.Parameter1 = 0;
    EpCmd.Template.Parameter2 = 0;

    // fill in the command data
    EpCmd.EndXfer.Cmd.CmdTyp = DWC_EP_CMD_END_XFER_VAL;
    EpCmd.EndXfer.Cmd.CmdAct = 1;
    EpCmd.EndXfer.Cmd.CmdIOC = 1;
    EpCmd.EndXfer.Cmd.HiPri_ForceRM = 1;
    EpCmd.EndXfer.Cmd.CmdParam = Dev->Ep[EpIndex].ResourceID;

    // execute the endpoint command
    Status = DwcWriteEndpointCommand(Dev, Ep, EpDir, &EpCmd);
    if (DWC_ERROR(Status)) {
      TraceErr(CommonCommand, "Failed to execute EndXfer command on physical ep %d",
        EpIndex);
      Dev->Ep[EpIndex].State = OldState;
      return Status;
    }

    // wait for the command to complete prior to issuing another
    Status = DwcWaitRegBit(
        DWC_ADDRI(Dev->CoreBaseAddr, DEPCMD, EpIndex),
        DWC_BMSK(DEPCMD, CMDACT),
        0,
        DWC_POLL_TIMEOUT
    );

    if (DWC_ERROR(Status)) {
      TraceErr(CommonCommand, "EndXfer command execution times out");
    }

    // Wait for 1 ms for event to be completed by the hardware
    DWC_PLAT_DELAY(1000);

    Dev->Ep[EpIndex].ResourceID = DWC_EP_RESOURCE_ID_INVALID;
    TraceInfo(CommonDebug, "ResourceID set to ID_INVALID for EpIdx %d", EpIndex);

    // reset the TRB ring after ending a transfer
    DwcResetTrbRing(Dev->Ep[EpIndex].Ring);
  }

  // zero the command
  EpCmd.Template.Cmd.Dword  = 0;
  EpCmd.Template.Parameter0 = 0;
  EpCmd.Template.Parameter1 = 0;
  EpCmd.Template.Parameter2 = 0;

  // fill in the command data
  if (IsStall) {
    EpCmd.SetStall.Cmd.CmdTyp = DWC_EP_CMD_SET_STALL_VAL;
    EpCmd.SetStall.Cmd.CmdAct = 1;
    EpCmd.SetStall.Cmd.CmdIOC = 1;
  } else {
    EpCmd.ClearStall.Cmd.CmdTyp = DWC_EP_CMD_CLEAR_STALL_VAL;
    EpCmd.ClearStall.Cmd.CmdAct = 1;
    EpCmd.ClearStall.Cmd.CmdIOC = 1;
  }

  // execute the endpoint command
  Status = DwcWriteEndpointCommand(Dev, Ep, EpDir, &EpCmd);
  if (DWC_ERROR(Status)) {
    TraceErr(CommonCommand, "Failed to execute Set/Clear Stall command on physical ep %d",
        EpIndex);
    Dev->Ep[EpIndex].State = OldState;
    return Status;
  }

  // wait for the command to complete prior to returning
  Status = DwcWaitRegBit(
      DWC_ADDRI(Dev->CoreBaseAddr, DEPCMD, EpIndex),
      DWC_BMSK(DEPCMD, CMDACT),
      0,
      DWC_POLL_TIMEOUT );
  if (DWC_ERROR(Status)) {
    TraceErr(CommonCommand, "SetStall command execution times out");
    }

  // clear stall state after command completes
  if (!IsStall) {
    Dev->Ep[EpIndex].State = DWC_EP_STATE_INACTIVE;
  }

  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcGetEPState (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  OUT DWC_EP_STATE          *EpState
  )
{
  UINT8       EpIndex;
  DWC_STATUS  Status = DWC_SUCCESS;

  // validate parameters
  if ((NULL == Dev) || (NULL == EpState) ||
      (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) ||
      (EpDir >= DWC_EP_DIR_MAX)) {
    TraceErr(CommonEvent, "invalid parameter");
    Status = DWC_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  EpIndex = DWC_EP_INDEX(Ep,EpDir);
  *EpState = Dev->Ep[EpIndex].State;

ON_EXIT:
  return Status;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcPrepareURB (
  IN     DWC_DEVICE         *Dev,
  IN OUT DWC_URB            *Urb,
  IN     UINT8              Ep,
  IN     UINT8              EpDir,
  IN     UINT8              TrbCtl,
  IN     UINT16             StreamID,
  IN     UINT32             TotalTransactionLength,
  IN     BOOLEAN            UseZlt
  )
{
  UINT8         EpIndex;

  // validate parameters
  if ((NULL == Dev) || (NULL == Urb) ||
      (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) || (EpDir >= DWC_EP_DIR_MAX) ||
      (TrbCtl < DWC_TRB_CTL_NORMAL) || (TrbCtl > DWC_TRB_CTL_ISOCHRONOUS)) {
    TraceErr(CommonTrb, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  EpIndex = DWC_EP_INDEX(Ep, EpDir);

  // validate that TotalTransactionLength is a multiple of MaxPacketSize for OUT endpoints
  if ((DWC_EP_DIR_OUT == EpDir) &&
      (0 != (TotalTransactionLength % Dev->Ep[EpIndex].MaxPktSize)) &&
      ((DWC_TRB_CTL_NORMAL == TrbCtl) || (DWC_TRB_CTL_CONTROL_DATA == TrbCtl))) {
    TraceErr(CommonTrb, "total transaction length[%d] not a multiple of the maximum packet size[%d] on phys ep[%d]",
        TotalTransactionLength, Dev->Ep[EpIndex].MaxPktSize, EpIndex);
    return DWC_INVALID_PARAMETER;
  }

  // make sure the endpoint is initialized
  if (Dev->Ep[EpIndex].State == DWC_EP_STATE_UNINITIALIZED) {
    TraceErr(CommonTrb, "physical endpoint[%d] not configured", EpIndex);
    return DWC_ENDPOINT_UNINITIALIZED;
  }

  // initialize Urb
  Urb->Ep               = Ep;
  Urb->EpDir            = EpDir;
  Urb->TrbCtl           = TrbCtl;
  Urb->StreamID         = StreamID;
  Urb->TotalTransactLen = TotalTransactionLength;
  Urb->UseZlt           = UseZlt;
  Urb->TotalBytesXfered = 0;
  Urb->NumTRBs          = 0;
  Urb->SGLen            = 0;
  Urb->SGBytesXfered    = 0;
  Urb->XferTRBSTS       = DWC_TRB_STS_OK;
  Urb->XferDesc         = NULL;
  Urb->SGList           = NULL;

  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcUpdateURB (
  IN     DWC_DEVICE             *Dev,
  IN OUT DWC_URB                *Urb,
  IN     SCATTER_GATHER_LIST    *SGList
  )
{
  DWC_STATUS              Status = DWC_SUCCESS;
  SCATTER_GATHER_ELEMENT  *SGElement;
  DWC_TRB_RING            *TrbRing;
  DWC_TRB                 *Trb = NULL;
  DWC_TRB                 *RingOriginalEnq;
  UINT32                  UrbXferLen;
  UINT32                  TotalLen;
  UINT32                  TrbLen;
  UINT32                  i;
  UINT8                   EpIndex;
  UINT8                   RingNewTRBs;

  // validate parameters
  if ((NULL == Dev) || (NULL == Urb) || (NULL == SGList)) {
    TraceErr(CommonTrb, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  // make sure SGList has at least one element
  if (0 == SGList->NumberOfElements) {
    TraceErr(CommonTrb, "invalid scatter/gather list");
    return DWC_INVALID_PARAMETER;
  }

  EpIndex = DWC_EP_INDEX(Urb->Ep, Urb->EpDir);

  //
  // fetch and synchronize TRB ring
  //
  // future consideration: Implement monitoring feature to check whether the ring ever
  // changes from this function call.  Due to the manner in which transfers and events
  // are handled, it may be that the ring can never change at this point in the code.
  //
  TrbRing = Dev->Ep[EpIndex].Ring;
  if (DWC_ERROR(DwcSyncTrbRing(TrbRing, FALSE))) {
    TraceErr(CommonTrb, "failed to synchronize TRB ring for physical endpoint %d", EpIndex);
    return DWC_DEVICE_ERROR;
  }

  // make sure the ring has room before we go any further
  if ((TrbRing->NumTRBs > 0) && (TrbRing->Enq == TrbRing->Deq)) {
    TraceErr(CommonTrb, "TRB ring of EpIndex %d is full", EpIndex);
    return DWC_TRB_RING_FULL;
  }

  // initialize data for scatter/gather loop
  Urb->XferDesc      = TrbRing->Enq;
  Urb->NumTRBs       = 0;
  Urb->SGLen         = 0;
  Urb->SGBytesXfered = 0;
  Urb->SGList        = SGList;
  RingOriginalEnq    = TrbRing->Enq;
  RingNewTRBs        = 0;
  UrbXferLen         = 0;

  // warn of improper usage if the total transaction length of a control
  // status state is nonzero
  if ((Urb->TotalTransactLen > 0) &&
      ((DWC_TRB_CTL_CONTROL_STATUS_2 == Urb->TrbCtl) ||
       (DWC_TRB_CTL_CONTROL_STATUS_3 == Urb->TrbCtl))) {
    TraceWarn(CommonTrb, "Control status stage submitted with nonzero length!");
    Urb->TotalTransactLen = 0;
  }

  // only process the scatter/gather list if the total transaction length > 0
  if (Urb->TotalTransactLen > 0) {

    // loop through the scatter/gather list creating TRBs for each element
    for (i = 0; i < SGList->NumberOfElements; i++) {
      SGElement = &SGList->Elements[i];

      // process this scatter/gather element (break into multiple TRBs if too big)
      Trb = TrbRing->Enq;
      TotalLen = 0;
      TrbLen = 0;
      while (TotalLen < SGElement->Length) {

        DWC_TRB_RING_FULL_CHECK(TrbRing);

        // update TRBCTL value if it is Control-Data (only first TRB of data stage uses Control-Data)
        if ((Urb->TrbCtl == DWC_TRB_CTL_CONTROL_DATA) && (Urb->NumTRBs > 0)) {
          Urb->TrbCtl = DWC_TRB_CTL_NORMAL;
        }

        // calculate length for this TRB
        TrbLen = SGElement->Length - TotalLen;
        if (TrbLen > DWC_TRB_MAX_BUFFER_SIZE) {
          TrbLen = DWC_TRB_MAX_BUFFER_SIZE;
        }

        //
        // if this is the last TRB that will fit in the ring before wrapping
        // around set up a link TRB pointing back to the beginning
        //
        if (((UINT64)(Trb + 1)) >= ((UINT64)TrbRing->Buf + TrbRing->BufSize)) {

          //
          // don't start an entire transfer with a link TRB, instead just jump to the
          // beginning of the ring to start the buffer descriptor
          //
          // Not the best way to handle the issue by adding a speical case here.
          // However, since STALLED state is clear later in DwcGetNextEvent and 
          // to change the behavior, we need to take great risk to change how 
          // the EP state are managed and how events are handled.
          //
          if ( (Urb->NumTRBs == 0) && 
               ((Dev->Ep[EpIndex].State == DWC_EP_STATE_INACTIVE) || 
                ((EpIndex == 0) && (Dev->Ep[EpIndex].State == DWC_EP_STATE_STALLED))
               )
             ){

            // make sure the HWO bit of the Link TRB is cleared so the HW won't read it
            Trb->HWO = 0;
            Urb->XferDesc = (DWC_TRB *)TrbRing->Buf;

          } else {

            // populate the link TRB
            Trb->BufPtrLo = (UINT64)TrbRing->BufAddress.Physical & 0xFFFFFFFF;
            Trb->BufPtrHi = (UINT64)TrbRing->BufAddress.Physical >> 32 & 0xFFFFFFFF;
            Trb->BufSize  = 0;
            Trb->PCM1     = 0;
            Trb->Rsvd0    = 0;
            Trb->TRBSTS   = 0;
            Trb->HWO      = 1;
            Trb->LST      = 0;
            Trb->CHN      = 0;
            Trb->CSP      = 0;
            Trb->TRBCTL   = DWC_TRB_CTL_LINK_TRB;
            Trb->ISP_IMI  = 0;
            Trb->IOC      = 0;
            Trb->Rsvd1    = 0;
            Trb->SID_SOFN = Urb->StreamID;
            Trb->Rsvd2    = 0;

            // only increment the Urb's TRB count if the link TRB is part of the transfer
            Urb->NumTRBs++;
          }

          // wrap around the pointers to the start of the ring and increment the TRB count
          Trb          = (DWC_TRB *)TrbRing->Buf;
          TrbRing->Enq = (DWC_TRB *)TrbRing->Buf;
          RingNewTRBs++;
          TrbRing->NumTRBs++;

          DWC_TRB_RING_FULL_CHECK(TrbRing);          
        }

        // fill in the TRB for this segment of data
        Trb->BufPtrLo = (SGElement->Address.QuadPart + TotalLen) & 0xFFFFFFFF;
        Trb->BufPtrHi = (SGElement->Address.QuadPart + TotalLen) >> 32 & 0xFFFFFFFF;
        Trb->BufSize  = TrbLen;
        Trb->PCM1     = 0;
        Trb->Rsvd0    = 0;
        Trb->TRBSTS   = 0;
        Trb->HWO      = 1;
        Trb->LST      = 0;
        Trb->CHN      = 1;
        Trb->CSP      = 0;
        Trb->TRBCTL   = Urb->TrbCtl;
        Trb->ISP_IMI  = 0;
        Trb->IOC      = 0;
        Trb->Rsvd1    = 0;
        Trb->SID_SOFN = Urb->StreamID;
        Trb->Rsvd2    = 0;

        // increment counters and pointers
        TotalLen += TrbLen;

        Urb->NumTRBs++;
        RingNewTRBs++;
        TrbRing->NumTRBs++;

        DWC_INCR_TRB_PTR(TrbRing, TrbRing->Enq);
        Trb = TrbRing->Enq;
      } // end while

      UrbXferLen += TotalLen;
    } // end for
  } // end if

  // create a zero-length TRB for ZLT support and control status stages
  if ((DWC_TRB_CTL_CONTROL_STATUS_2 == Urb->TrbCtl) || (DWC_TRB_CTL_CONTROL_STATUS_3 == Urb->TrbCtl) ||
      ((DWC_EP_DIR_IN == Urb->EpDir) && Urb->UseZlt && (Urb->TotalBytesXfered + UrbXferLen >= Urb->TotalTransactLen))) {

    // make sure Trb points to next available TRB
    Trb = TrbRing->Enq;
    DWC_TRB_RING_FULL_CHECK(TrbRing);

    // append a zero length TRB to transfer
    Trb->BufPtrLo = 0;
    Trb->BufPtrHi = 0;
    Trb->BufSize  = 0;
    Trb->PCM1     = 0;
    Trb->Rsvd0    = 0;
    Trb->TRBSTS   = 0;
    Trb->HWO      = 1;
    Trb->LST      = 0;
    Trb->CHN      = 1;
    Trb->CSP      = 0;
    Trb->TRBCTL   = Urb->TrbCtl;
    Trb->ISP_IMI  = 0;
    Trb->IOC      = 0;
    Trb->Rsvd1    = 0;
    Trb->SID_SOFN = Urb->StreamID;
    Trb->Rsvd2    = 0;

    // increment counters and pointers
    Urb->NumTRBs++;
    RingNewTRBs++;
    TrbRing->NumTRBs++;

    DWC_INCR_TRB_PTR(TrbRing, TrbRing->Enq);
    Trb = TrbRing->Enq;
  }

ON_EXIT:

  //
  // if we ended up here and Trb is not equal to the enq pointer something
  // is seriously wrong, recover as best we can
  //
  if (Trb != TrbRing->Enq) {
    TraceErr(CommonTrb, "FATAL ERROR CREATING TRBS!! Resetting ring and urb");

    TrbRing->Enq       = RingOriginalEnq;
    TrbRing->NumTRBs  -= RingNewTRBs;
    Urb->SGList        = NULL;
    Urb->XferDesc      = NULL;
    Urb->NumTRBs       = 0;
    Urb->SGLen         = 0;
    Urb->SGBytesXfered = 0;

    return DWC_DEVICE_ERROR;
  }

  if (Status == (INT32)DWC_TRB_RING_FULL) {
    TraceInfo(CommonTrb, "TRB ring full, transfering as much as possible");
  }

  // decrement Trb to the last non-link TRB manipulated above
  DWC_DECR_TRB_PTR(TrbRing, Trb);
  if( NULL == Trb ) {
    TraceErr(CommonTrb, "TRB is NULL");
    return DWC_OUT_OF_MEMORY;
  }

  if (Trb->TRBCTL == DWC_TRB_CTL_LINK_TRB) {
    DWC_DECR_TRB_PTR(TrbRing, Trb);
  }

  // store total length of data we put in Urb from this SGList
  Urb->SGLen = UrbXferLen;

  // always set the IOC bit as we need to know when this new block of TRBs is transfered
  Trb->IOC = 1;

  // if this SGList completes the entire transaction set appropriate TRB bits
  if (Urb->TotalBytesXfered + Urb->SGLen >= Urb->TotalTransactLen) {
    Trb->CHN = 0;
    Trb->LST = 1;
  }

  TraceInfo(CommonTrb, "updated urb for phys ep %d with %d new TRBs and %d additional length",
      EpIndex, RingNewTRBs, Urb->SGLen);
  DwcDumpTrbRingEx (Dev, Urb->Ep, Urb->EpDir, 1, Trb);

  //Added Memory Barrier
  ArmDataMemoryBarrier();
  return DWC_SUCCESS;
}

/*
 * See header file for description.
 */
VOID
DwcDumpTrbRingEx (  
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  IN  UINT32                Id,
  IN  DWC_TRB               *Trb
  )
{
  DWC_TRB_RING            *TrbRing;  
  UINT8                   EpIndex;

  // validate parameters
  if ((NULL == Dev) || (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) ||
      (EpDir >= DWC_EP_DIR_MAX)) {
    TraceErr(CommonCommand, "invalid parameter");
    return;
  }

  // get the physical endpoint from the logical one
  EpIndex = DWC_EP_INDEX(Ep, EpDir);

  //
  // Print the TrbRing structure
  //
  TrbRing = Dev->Ep[EpIndex].Ring;

  TraceInfo(
    CommonTrb,
    "Id[%d]: CurTrb[0x%p] TrbRings:Buf 0x%08p, Enq 0x%08p Deq 0x%08p", 
    Id, Trb, TrbRing->Buf,TrbRing->Enq, TrbRing->Deq);

  DwcDumpTrbRing(Dev, Ep, EpDir);
}


/*
 * See header file for description.
 */
VOID
DwcDumpTrbRing (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir
  )
{
  DWC_TRB_RING            *TrbRing;
  DWC_TRB                 *Trb = NULL;
  UINT32                  TotalNumTrb;
  UINT32                  i;
  UINT8                   EpIndex;

  // validate parameters
  if ((NULL == Dev) || (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) ||
      (EpDir >= DWC_EP_DIR_MAX)) {
    TraceErr(CommonCommand, "invalid parameter");
    return;
  }

  // get the physical endpoint from the logical one
  EpIndex = DWC_EP_INDEX(Ep, EpDir);

  //
  // Print the TrbRing structure
  //
  TrbRing = Dev->Ep[EpIndex].Ring;

  TraceInfo(
    CommonTrb,
    "TrbRings:  BufAddr.Vir 0x%08x BufAddr.Phy 0x%08x Buf 0x%08p, BufSize 0x%x, Enq 0x%08p Deq 0x%08p, NumTrb 0x%x",
    TrbRing->BufAddress.Virtual,TrbRing->BufAddress.Physical,
    TrbRing->Buf, TrbRing->BufSize, TrbRing->Enq, TrbRing->Deq, TrbRing->NumTRBs);

  TotalNumTrb = TrbRing->BufSize / sizeof(DWC_TRB);

  Trb = (DWC_TRB *) TrbRing->Buf;
  for(i = 0; i < TotalNumTrb; i++){
    TraceInfo(
      CommonTrb,
      "Trb[%02d] BPTRL 0x%08x BPTRH 0x%08x",
       i,Trb->BufPtrLo, Trb->BufPtrHi);
       
    TraceInfo(
      CommonTrb,
      "TRBSTS 0x%x PCM1 0x%x BUFSIZE 0x%06x",
       Trb->TRBSTS, Trb->PCM1, Trb->BufSize);

    TraceInfo(
      CommonTrb,
      "SID_SOFN 0x%04x IOC 0x%x ISP_IMI 0x%x TRBCTL 0x%02x CSP 0x%x",
      Trb->SID_SOFN, Trb->IOC, Trb->ISP_IMI, Trb->TRBCTL,Trb->CSP);

    TraceInfo(
      CommonTrb,
      "CHN 0x%x LST 0x%x HWO 0x%x",
      Trb->CHN, Trb->LST, Trb->HWO);

    Trb++;
  }
}

/*
 * See header file for description.
 */
DWC_STATUS
DwcSubmitURB (
  IN  DWC_DEVICE            *Dev,
  IN  DWC_URB               *Urb
  )
{
  DWC_STATUS    Status = DWC_SUCCESS;
  DWC_EP_CMD    EpCmd;
  UINT64        XferDescAddr;
  UINT8         EpIndex;
  UINT8         RunStop;

  // validate parameters
  if ((NULL == Dev) || (NULL == Urb) || (Urb->NumTRBs == 0) ||
      (NULL == Urb->XferDesc)) {
    TraceErr(CommonCommand, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  EpIndex = DWC_EP_INDEX(Urb->Ep, Urb->EpDir);

  // zero the command
  EpCmd.Template.Cmd.Dword  = 0;
  EpCmd.Template.Parameter0 = 0;
  EpCmd.Template.Parameter1 = 0;
  EpCmd.Template.Parameter2 = 0;

  // if endpoint is inactive, or if Ep 0 OUT is stalled, issue a start transfer command
  // note the hardware will clear stall without generating event so proceed regardless
  if ((Dev->Ep[EpIndex].State == DWC_EP_STATE_INACTIVE) ||
      ((DWC_EP_INDEX(0,DWC_EP_DIR_OUT) == EpIndex) &&
       (Urb->TrbCtl == DWC_TRB_CTL_CONTROL_SETUP))) {

    // get RunStop bit to see if we need to poll for status
    RunStop = DWC_HWIO_INF(Dev->CoreBaseAddr, DCTL, RUN_STOP);

    // get the physical address of the transfer descriptor for the command
    Status = DwcGetTrbPhysicalAddress(Dev->Ep[EpIndex].Ring, Urb->XferDesc, &XferDescAddr);
    if (DWC_ERROR(Status)) {
      TraceErr(CommonCommand, "failed to get physical address of TD for physical endpoint %d", EpIndex);
      return Status;
    }

    // execute a start transfer command on the endpoint
    EpCmd.StartXfer.TDAddressLo  = XferDescAddr & 0xFFFFFFFF;
    EpCmd.StartXfer.TDAddressHi  = XferDescAddr >> 32 & 0xFFFFFFFF;
    EpCmd.StartXfer.Cmd.CmdTyp   = DWC_EP_CMD_START_XFER_VAL;
    EpCmd.StartXfer.Cmd.CmdIOC   = RunStop;
    EpCmd.StartXfer.Cmd.CmdAct   = 1;
    EpCmd.StartXfer.Cmd.CmdParam = Urb->StreamID;

    TraceInfo(CommonCommand, "executing StartXfer command on physical endpoint %d", EpIndex);
    
    // If 1. the device are not in SS speed and
    //    2. in L1, and
    //    3. EpIndex is one
    // write DCTL[ULSTCHNGREQ] = 8
    if ((DWC_SUPER_SPEED != DWC_HWIO_INF(Dev->CoreBaseAddr, DSTS, CONNECTSPD)) &&
        (DWC_LS_SLEEP == DWC_HWIO_INF(Dev->CoreBaseAddr, DSTS, USBLNKST)) && 
        (1 == EpIndex)) {
      DWC_HWIO_OUTF(Dev->CoreBaseAddr, DCTL, ULSTCHNGREQ, DWC_LS_RECOV);
      TraceWarn(CommonCommand, "StartXfer: wake up controller");
    }
    
    Status = DwcWriteEndpointCommand (Dev, Urb->Ep, Urb->EpDir, &EpCmd);
    if (DWC_ERROR(Status)) {
      TraceErr(CommonCommand, "failed to execute StartXfer command on physical endpoint %d", EpIndex);
    } else {
      Dev->Ep[EpIndex].State = DWC_EP_STATE_START_XFER;
      Dev->Ep[EpIndex].Urb   = Urb;

      // if RunStop bit is clear we must poll the command register for status
      if (RunStop == 0) {
        TraceInfo(CommonCommand, "controller not running, polling for command completion");
        DwcWaitRegBit(
            DWC_ADDRI(Dev->CoreBaseAddr, DEPCMD, EpIndex),
            DWC_BMSK(DEPCMD, CMDACT),
            0,
            DWC_POLL_TIMEOUT
        );
        if (DWC_HWIO_INFI(Dev->CoreBaseAddr, DEPCMD, EpIndex, CMDSTATUS)) {
          TraceErr(CommonCommand, "StartXfer command failed");
          Status = DWC_DEVICE_ERROR;
        } else {
          TraceInfo(CommonCommand, "StartXfer command succeeded");
          Dev->Ep[EpIndex].State = DWC_EP_STATE_XFER_IN_PROGRESS;
        }
      }
    }

  // if endpoint has a transfer in progress, issue an update transfer command
  } else if (Dev->Ep[EpIndex].State == DWC_EP_STATE_XFER_IN_PROGRESS) {

    // execute an update transfer command on the endpoint
    EpCmd.UpdateXfer.Cmd.CmdTyp   = DWC_EP_CMD_UPDATE_XFER_VAL;
    EpCmd.UpdateXfer.Cmd.CmdIOC   = 1;
    EpCmd.UpdateXfer.Cmd.CmdAct   = 1;
    EpCmd.UpdateXfer.Cmd.CmdParam = Dev->Ep[EpIndex].ResourceID;

    TraceInfo(CommonCommand, "executing UpdateXfer command on physical endpoint %d", EpIndex);

    // If 1. the device are not in SS speed and
    //    2. in L1, and
    //    3. EpIndex is one
    // write DCTL[ULSTCHNGREQ] = 8
    if ((DWC_SUPER_SPEED != DWC_HWIO_INF(Dev->CoreBaseAddr, DSTS, CONNECTSPD)) &&
        (DWC_LS_SLEEP == DWC_HWIO_INF(Dev->CoreBaseAddr, DSTS, USBLNKST)) && 
        (1 == EpIndex)) {
      DWC_HWIO_OUTF(Dev->CoreBaseAddr, DCTL, ULSTCHNGREQ, DWC_LS_RECOV);
      TraceWarn(CommonCommand, "UpdateXfer: wake up controller");
    }
    
    Status = DwcWriteEndpointCommand (Dev, Urb->Ep, Urb->EpDir, &EpCmd);
    if (DWC_ERROR(Status)) {
      TraceErr(CommonCommand, "failed to execute UpdateXfer command on physical endpoint %d", EpIndex);
    }

  // endpoint is not in a proper state to perform any action
  } else {
    TraceErr(CommonTrb, "physical endpoint %d is in invalid state %d", EpIndex, Dev->Ep[EpIndex].State);
    Status = DWC_ENDPOINT_INVALID_STATE;
  }

  return Status;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcCancelTransferByEp (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir
  )
{
  DWC_STATUS  Status;
  DWC_EP_CMD  EndCmd;
  UINT8       EpIndex;
  INT32       Count = DWC_MAX_SPIN_COUNT;

  // validate parameters
  if ((NULL == Dev) || (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) ||
      (EpDir >= DWC_EP_DIR_MAX)) {
    TraceErr(CommonCommand, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  EpIndex = DWC_EP_INDEX(Ep, EpDir);

  // make sure we have a transfer to end
  if (Dev->Ep[EpIndex].ResourceID == DWC_EP_RESOURCE_ID_INVALID) {
  	// not always an error as the transfer can be cancelled whenever the host
    // sends a RESET or SET_CONFIG
    TraceWarn(CommonCommand, "no transfer to cancel on physical endpoint %d", EpIndex);
    return DWC_ENDPOINT_NOT_ACTIVE;
  }

  // zero the command
  EndCmd.Template.Cmd.Dword  = 0;
  EndCmd.Template.Parameter0 = 0;
  EndCmd.Template.Parameter1 = 0;
  EndCmd.Template.Parameter2 = 0;

  // execute an end transfer command on the endpoint
  EndCmd.EndXfer.Cmd.CmdTyp        = DWC_EP_CMD_END_XFER_VAL;
  EndCmd.EndXfer.Cmd.HiPri_ForceRM = 1;
  EndCmd.EndXfer.Cmd.CmdIOC        = 1;
  EndCmd.EndXfer.Cmd.CmdAct        = 1;
  EndCmd.EndXfer.Cmd.CmdParam      = Dev->Ep[EpIndex].ResourceID;

  TraceInfo(CommonCommand, "executing EndXfer command on physical endpoint %d, resource ID %d",
      EpIndex, Dev->Ep[EpIndex].ResourceID);

  do {
    Status = DwcWriteEndpointCommand (Dev, Ep, EpDir, &EndCmd);
  } while ((Status == (INT32)DWC_COMMAND_ACTIVE) && --Count);
  if (DWC_ERROR(Status)) {
    TraceErr(CommonCommand, "failed to execute EndXfer command on physical endpoint %d", EpIndex);
  }

  // For correct handling in UEFI, we need to ask MSFT for changes. We need another MSG for
  // indicating Abort is finished. For now, we need handle the abort synchronously

  // wait for the command to be accepted by the hw. 
  Status = DwcWaitRegBit(
    DWC_ADDRI(Dev->CoreBaseAddr, DEPCMD, EpIndex),
    DWC_BMSK(DEPCMD, CMDACT),
    0,
    DWC_POLL_TIMEOUT
    );
  if (DWC_ERROR(Status)) {
    TraceErr(CommonCommand, "EndXfer command execution times out");
  }

  // Wait for 1 ms for event to be completed by the hardware
  DWC_PLAT_DELAY(1000);

  if (Dev->Ep[EpIndex].State != DWC_EP_STATE_STALLED &&
      Dev->Ep[EpIndex].State != DWC_EP_STATE_STALLED_XFER_PENDING ) {
    Dev->Ep[EpIndex].State = DWC_EP_STATE_INACTIVE;
  }
  Dev->Ep[EpIndex].ResourceID = DWC_EP_RESOURCE_ID_INVALID;

  // reset the TRB ring after ending a transfer
  DwcResetTrbRing(Dev->Ep[EpIndex].Ring);

  TraceInfo(CommonEvent, "EndXfer command complete on physical endpoint %d", EpIndex);
  return Status;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcCancelTransferByUrb (
  IN  DWC_DEVICE            *Dev,
  IN  DWC_URB               *Urb
  )
{
  DWC_STATUS  Status;

  // validate parameters
  if ((NULL == Dev) || (NULL == Urb)) {
    TraceErr(CommonCommand, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  Status = DwcCancelTransferByEp(Dev, Urb->Ep, Urb->EpDir);

  return Status;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcCancelAllTransfers (
  IN  DWC_DEVICE            *Dev
  )
{
  DWC_STATUS  Status = DWC_SUCCESS;
  DWC_STATUS  CancelStatus;
  UINT8       Ep, EpDir, EpIndex;

  // validate parameters
  if (NULL == Dev) {
    TraceErr(CommonCommand, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  //
  // loop through all endpoints (except control in and out)
  // and cancel any transfers in progress
  //
  for (Ep = 1; Ep <= DWC_USB_MAX_ENDPOINT_NUMBER; Ep++) {
    for (EpDir = 0; EpDir < DWC_EP_DIR_MAX; EpDir++) {
      EpIndex = DWC_EP_INDEX(Ep, EpDir);

      if (Dev->Ep[EpIndex].State == DWC_EP_STATE_XFER_IN_PROGRESS) {
        CancelStatus = DwcCancelTransferByEp (Dev, Ep, EpDir);

        // if any one cancel fails, try canceling the rest but return failure
        if (DWC_ERROR(CancelStatus)) {
          TraceErr(CommonCommand, "failed to cancel transfer on physical endpoint %d, continuing", EpIndex);
          Status = CancelStatus;
        }
      }
    }
  }

  return Status;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcWriteDeviceCommand (
  IN DWC_DEVICE             *Dev,
  IN DWC_DEV_CMD            *Cmd
  )
{
  // validate parameters
  if ((NULL == Dev) || (NULL == Cmd)) {
    TraceErr(CommonCommand, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  // make sure there isn't a command currently executing
  if (DWC_HWIO_INF(Dev->CoreBaseAddr, DGCMD, CMDACT)) {
    TraceWarn(CommonCommand, "device command is currently executing");
    return DWC_COMMAND_ACTIVE;
  }

  // write the command and parameter registers
  switch (Cmd->Template.Cmd.Reg.CmdTyp) {
    case DWC_DEV_CMD_SET_PERIODIC_PARAMS_VAL:
    case DWC_DEV_CMD_SET_SCRATCHPAD_BUF_LO_VAL:
    case DWC_DEV_CMD_SET_SCRATCHPAD_BUF_HI_VAL:
    case DWC_DEV_CMD_TX_DEVICE_NOTIFY_VAL:
    case DWC_DEV_CMD_SELECTED_FIFO_FLUSH_VAL:
    case DWC_DEV_CMD_SET_EP_NRDY_VAL:
    case DWC_DEV_CMD_RUN_SOC_LOOPBACK_TEST_VAL:
      // write the parameter register and fall through
      DWC_HWIO_OUT(Dev->CoreBaseAddr, DGCMDPAR, Cmd->Template.Parameter);

    case DWC_DEV_CMD_ALL_FIFO_FLUSH_VAL:
      // wrtie the command register
      DWC_HWIO_OUT(Dev->CoreBaseAddr, DGCMD, Cmd->Template.Cmd.Dword);
      break;

    default:
      TraceErr(CommonCommand, "invalid command type");
      return DWC_INVALID_COMMAND_TYPE;
  }

  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcWriteEndpointCommand (
  IN DWC_DEVICE             *Dev,
  IN UINT8                  Ep,
  IN UINT8                  EpDir,
  IN DWC_EP_CMD             *Cmd
  )
{
  UINT8         EpIndex;
  UINT32        RegVal;

  // validate parameters
  if ((NULL == Dev) || (NULL == Cmd) || (Ep > DWC_USB_MAX_ENDPOINT_NUMBER) ||
      (EpDir >= DWC_EP_DIR_MAX)) {
    TraceErr(CommonCommand, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  RegVal = DWC_HWIO_INFI(
      Dev->CoreBaseAddr,
      GUSB2PHYCFG,
      0,
      SUSPENDUSB20
      );
  if(1 == RegVal){
    // need to clear before update, then restore afterwards
    DWC_HWIO_OUTFI(
      Dev->CoreBaseAddr,
      GUSB2PHYCFG,
      0,
      SUSPENDUSB20,
      0
      );
  }

  EpIndex = DWC_EP_INDEX(Ep, EpDir);

  // make sure there isn't a command currently executing
  if (DWC_HWIO_INFI(Dev->CoreBaseAddr, DEPCMD, EpIndex, CMDACT)) {
    TraceWarn(CommonCommand, "physical endpoint %d command already executing", EpIndex);
    return DWC_COMMAND_ACTIVE;
  }

  // write command parameter 2 register and fall through
  DWC_HWIO_OUTI(Dev->CoreBaseAddr, DEPCMDPAR2, EpIndex, Cmd->Template.Parameter2);
  // write command parameter 1 register and fall through
  DWC_HWIO_OUTI(Dev->CoreBaseAddr, DEPCMDPAR1, EpIndex, Cmd->Template.Parameter1);
  // write command parameter 0 register and fall through
  DWC_HWIO_OUTI(Dev->CoreBaseAddr, DEPCMDPAR0, EpIndex, Cmd->Template.Parameter0);

  // write the command and parameter registers
  switch (Cmd->Template.Cmd.Reg.CmdTyp) {
    case DWC_EP_CMD_SET_EP_CONFIG_VAL:
    case DWC_EP_CMD_START_XFER_VAL:
    case DWC_EP_CMD_SET_EP_XFER_RSC_CONFIG_VAL:
    case DWC_EP_CMD_GET_EP_STATE_VAL:
    case DWC_EP_CMD_SET_STALL_VAL:
    case DWC_EP_CMD_CLEAR_STALL_VAL:
    case DWC_EP_CMD_UPDATE_XFER_VAL:
    case DWC_EP_CMD_END_XFER_VAL:
    case DWC_EP_CMD_START_NEW_CONFIG_VAL:
      //Added Memory Barrier so that TRB structure operations are performed prior passing to controller
      ArmDataMemoryBarrier();
      // write the command register
      DWC_HWIO_OUTI(Dev->CoreBaseAddr, DEPCMD, EpIndex, Cmd->Template.Cmd.Dword);
      break;

    default:
      TraceErr(CommonCommand, "invalid command type");
      return DWC_INVALID_COMMAND_TYPE;
  }
  
  TraceInfo(CommonCommand, "0x%x:0x%x:0x%x:0x%x",
    Cmd->Template.Parameter0,
    Cmd->Template.Parameter1,
    Cmd->Template.Parameter2,
    Cmd->Template.Cmd.Dword);

  if(1 == RegVal){
    // restore original value
    DWC_HWIO_OUTFI(
      Dev->CoreBaseAddr,
      GUSB2PHYCFG,
      0,
      SUSPENDUSB20,
      RegVal
      );
  }

  return DWC_SUCCESS;
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcGetEventCount (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  OUT UINT32                *EvtCount
  )
{
  UINT8     EpIndex;
  UINT8     EvtBufIndex;
  UINT16    EvtByteCount;

  // validate parameters
  if ((NULL == Dev) || (NULL == EvtCount) ||
      ((Ep > DWC_USB_MAX_ENDPOINT_NUMBER) && (Ep != DWC_EP_NUM_DEVICE_EVENT)) ||
      (EpDir >= DWC_EP_DIR_MAX)) {
    TraceErr(CommonEvent, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  // get event buffer index
  if (Ep == DWC_EP_NUM_DEVICE_EVENT) {
    EvtBufIndex = DWC_DEVICE_EVENT_BUFFER_INDEX;
  } else {
    EpIndex     = DWC_EP_INDEX(Ep, EpDir);
    EvtBufIndex = Dev->Ep[EpIndex].EvtBufferID;
  }
  if (EvtBufIndex >= Dev->EvtBufferCount) {
    TraceErr(CommonEvent, "invalid event buffer index found, Ep: %d EpDir: %d", Ep, EpDir);
    return DWC_INDEX_OUT_OF_RANGE;
  }

  // read event count register
  EvtByteCount = (UINT16)DWC_HWIO_INI(Dev->CoreBaseAddr, GEVNTCOUNT, EvtBufIndex);
  *EvtCount = EvtByteCount / DWC_BYTES_PER_EVENT;

  if (*EvtCount > 0) {
    TraceInfo(CommonEvent, "%d events available on event buffer %d", *EvtCount, EvtBufIndex);
  }

  return DWC_SUCCESS;
}


/*
 * Helper function to dump event buffer registers.
 *
 * @param [in] Dev              Device instance pointer
 * @param [in] EvtBufInd        Index of event buffer
 */
STATIC VOID
DwcDumpEventBufferRegs (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 EvtBufInd
  )
{
  // For ArmCompiler6,
  // array initialization will generate
  // an instruction, "bl memset" in the assembly. However, the
  // linker is not able to resolve it.
  // Temp. workaround the issue by initi. each element individually.
  DWC_REG_DESC EvtRegs[5];

  // DWC_REG_DESC EvtRegs[] = {
  //   { 0, "DEVTEN    " },
  //   { 0, "GEVNTADRHI" },
  //   { 0, "GEVNTADRLO" },
  //   { 0, "GEVNTSIZ  " },
  //   { 0, "GEVNTCOUNT" }
  // };

  // need to define the offsets this way because the index is a parameter
  EvtRegs[0].Offset = DWC_OFFS(DEVTEN);
  EvtRegs[0].Name   = "DEVTEN    ";
  EvtRegs[1].Offset = DWC_OFFSI(GEVNTADRHI,EvtBufInd);
  EvtRegs[1].Name   = "GEVNTADRHI";
  EvtRegs[2].Offset = DWC_OFFSI(GEVNTADRLO,EvtBufInd);
  EvtRegs[2].Name   = "GEVNTADRLO";
  EvtRegs[3].Offset = DWC_OFFSI(GEVNTSIZ,EvtBufInd);
  EvtRegs[3].Name   = "GEVNTSIZ  ";
  EvtRegs[4].Offset = DWC_OFFSI(GEVNTCOUNT,EvtBufInd);
  EvtRegs[4].Name   = "GEVNTCOUNT";

  TraceInfo(CommonDebug, "Dumping event buffer %d registers:", EvtBufInd);
  DwcDumpRegs(Dev, EvtRegs, sizeof(EvtRegs)/sizeof(EvtRegs[0]));
}


/*
 * See header file for description.
 */
DWC_STATUS
DwcGetNextEvent (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  OUT DWC_EVT               *Evt
  )
{
  UINT8             EpIndex;
  UINT8             EvtBufIndex;
  DWC_EVENT_BUFFER  *EvtBuf;
  UINT16            BytesRead;
  UINT8             EpCmdCmpltType;
  DWC_URB           *Urb;
  DWC_TRB           *Trb;
  DWC_TRB_RING      *TrbRing;
  UINT32            BytesNotXfered;
  UINT32            UrbTrbCnt;
  UINT8             ShortPktRcvd;
  BOOLEAN           SetupPndg;

  // validate parameters
  if ((NULL == Dev) || (NULL == Evt) ||
      ((Ep > DWC_USB_MAX_ENDPOINT_NUMBER) && (Ep != DWC_EP_NUM_DEVICE_EVENT)) ||
      (EpDir >= DWC_EP_DIR_MAX)) {
    TraceErr(CommonEvent, "invalid parameter");
    return DWC_INVALID_PARAMETER;
  }

  // get event buffer
  if (Ep == DWC_EP_NUM_DEVICE_EVENT) {
    EvtBufIndex = DWC_DEVICE_EVENT_BUFFER_INDEX;
  } else {
    EpIndex = DWC_EP_INDEX(Ep, EpDir);
    EvtBufIndex = Dev->Ep[EpIndex].EvtBufferID;
  }
  if (EvtBufIndex >= Dev->EvtBufferCount) {
    TraceErr(CommonEvent, "invalid event buffer index found, Ep: %d EpDir: %d", Ep, EpDir);
    return DWC_INDEX_OUT_OF_RANGE;
  }
  EvtBuf = &Dev->EvtBuffer[EvtBufIndex];

  // read the event from the buffer into Evt (4 bytes)
  *((UINT32 *)Evt) = *EvtBuf->NextEvt;
  TraceInfo(CommonEvent, "Evt: %p", EvtBuf->NextEvt);
  BytesRead = DWC_BYTES_PER_EVENT;

  // increment pointer to next event (with wraparound)
  EvtBuf->NextEvt++;
  if ((UINTN)EvtBuf->NextEvt >= ((UINTN)EvtBuf->Buf + EvtBuf->Size)) {
    EvtBuf->NextEvt = (UINT32 *)EvtBuf->Buf;
  }

  // handle extra data associated with Vendor Device Test LMP Received Event
  if ((Evt->Template.IsDeviceEvent) &&
      (Evt->Device.EventType == DWC_EVT_DEV_VNDR_DEV_TST_LMP_RCVED)) {
    TraceInfo(CommonEvent, "Vendor Device Test LMP Received Event posted");

    // wraparound before reading if the 8 byte LMP data would overflow the buffer
    if (((UINTN)EvtBuf->NextEvt + DWC_BYTES_PER_LMP_RX_EVENT) > ((UINTN)EvtBuf->Buf + EvtBuf->Size)) {
      EvtBuf->NextEvt = (UINT32 *)EvtBuf->Buf;
    }

    Evt->Device.LmpData = *(UINT64 *)EvtBuf->NextEvt;
    BytesRead += DWC_BYTES_PER_LMP_RX_EVENT;

    // increment pointer for data read (2 event's worth)
    EvtBuf->NextEvt += 2;
    if ((UINTN)EvtBuf->NextEvt >= ((UINTN)EvtBuf->Buf + EvtBuf->Size)) {
      EvtBuf->NextEvt = (UINT32 *)EvtBuf->Buf;
    }
  }

  // let hardware know how many bytes from the buffer we processed
  DWC_HWIO_OUTFI(
          Dev->CoreBaseAddr,
          GEVNTCOUNT,
          EvtBufIndex,
          EVNTCOUNT,
          BytesRead
        );

  // process device events before returning them to upper layer
  if (Evt->Template.IsDeviceEvent) {
    if (Evt->Device.EventType == DWC_EVT_DEV_EVENT_BUFFER_OVERFLOW) {
      TraceErr(CommonEvent, "EVENT BUFFER OVERFLOW!! Some events may have been dropped");
      DwcDumpEventBufferRegs(Dev, EvtBufIndex);
    }

  // process endpoint events before returning them to upper layer
  } else {

    //
    // handle the case where multiple endpoints use the same event buffer, in
    // which this event may not be related to the Ep/EpDir parameters
    //
    EpIndex = (UINT8)Evt->Endpoint.PhysEPNum;

    // command completion events
    if (Evt->Endpoint.EventType == DWC_EVT_EP_EP_CMD_COMPLETE) {
      EpCmdCmpltType = (Evt->Endpoint.EventParam &
                        DWC_EP_EVT_CMDCMPLT_PARAM_TYPE_BMSK) >>
                        DWC_EP_EVT_CMDCMPLT_PARAM_TYPE_SHFT;

      // store the transfer resource ID from a start transfer command
      if ((EpCmdCmpltType == DWC_EP_CMD_START_XFER_VAL) &&
          (Dev->Ep[EpIndex].State == DWC_EP_STATE_START_XFER)) {

        if (Evt->Endpoint.EventStatus == DWC_EP_EVT_EVENTSTATUS_SUCCESS) {
          Dev->Ep[EpIndex].State      = DWC_EP_STATE_XFER_IN_PROGRESS;
          Dev->Ep[EpIndex].ResourceID = (Evt->Endpoint.EventParam &
                          DWC_EP_EVT_CMDCMPLT_PARAM_RSCID_BMSK) >>
                          DWC_EP_EVT_CMDCMPLT_PARAM_RSCID_SHFT;
          TraceInfo(CommonEvent, "physical endpoint %d using transfer resource id %d",
              EpIndex, Dev->Ep[EpIndex].ResourceID);
        } else {
          Dev->Ep[EpIndex].State      = DWC_EP_STATE_INACTIVE;
          Dev->Ep[EpIndex].ResourceID = DWC_EP_RESOURCE_ID_INVALID;

          TraceErr(CommonEvent, "StartXfer command failed on physical endpoint %d, EventStatus: %d",
                   EpIndex, Evt->Endpoint.EventStatus);
        }
      }      
    }

    // transfer complete event or transfer in progress event
    if (((Evt->Endpoint.EventType == DWC_EVT_EP_XFER_COMPLETE) ||
         (Evt->Endpoint.EventType == DWC_EVT_EP_XFER_IN_PROGRESS)) &&
        ((Dev->Ep[EpIndex].State == DWC_EP_STATE_XFER_IN_PROGRESS) ||
         (Dev->Ep[EpIndex].State == DWC_EP_STATE_STALLED) ||
         (Dev->Ep[EpIndex].State == DWC_EP_STATE_STALLED_XFER_PENDING))) {

      // fetch and initialize TRB ring and URB
      Urb             = Dev->Ep[EpIndex].Urb;
      TrbRing         = Dev->Ep[EpIndex].Ring;
      Trb             = Urb->XferDesc;
      UrbTrbCnt       = Urb->NumTRBs;
      Urb->XferTRBSTS = DWC_TRB_STS_OK;
      BytesNotXfered  = 0;

      // get the short packet received bit from the event status
      ShortPktRcvd = (Evt->Endpoint.EventStatus & DWC_EVT_EP_XFERCMPLT_EVTSTS_SHORT_PKT_BMSK)
                      >> DWC_EVT_EP_XFERCMPLT_EVTSTS_SHORT_PKT_SHFT;
      if (ShortPktRcvd) {
          TraceInfo(CommonEvent, "Short packet received: possible fast-forward condition");
      }

      SetupPndg = (Trb->TRBSTS == DWC_TRB_STS_SETUP_PENDING);
      if (SetupPndg) {
        TraceInfo(CommonEvent, "Pending Setup received: possible fast-forward condition");
      }

      // step over successful TRBs, stop on last TRB if all successful
      while ((UrbTrbCnt > 1) && (Trb->BufSize == 0) && (Trb->TRBSTS == 0) &&
             ((Trb->HWO == 0) || (Trb->TRBCTL == DWC_TRB_CTL_LINK_TRB))) {
        UrbTrbCnt--;
        DWC_INCR_TRB_PTR(TrbRing, Trb);
      }

      // get status from first unsuccessful TRB
      if ((Trb->BufSize != 0) || (Trb->TRBSTS != 0)) {
        Urb->XferTRBSTS = (UINT8)Trb->TRBSTS;

        //
        // Loop through remaining TRBs totaling the buffer space left unused.  Ignore the HWO
        // bit if a short packet was received due to a possible "fast-forward" condition.
        //
        while ((UrbTrbCnt > 0) && ((Trb->HWO == 0) || ShortPktRcvd || SetupPndg ||
               (Trb->TRBCTL == DWC_TRB_CTL_LINK_TRB))) {
          BytesNotXfered += Trb->BufSize;
          UrbTrbCnt--;
          DWC_INCR_TRB_PTR(TrbRing, Trb);
        }
      }

      // update the URB's transfer lengths
      Urb->SGBytesXfered     = Urb->SGLen - BytesNotXfered;
      Urb->TotalBytesXfered += Urb->SGBytesXfered;     

      // synchronize the TRB ring now that we're done with the TRBs from this transfer
      if (DWC_ERROR(DwcSyncTrbRing(Dev->Ep[EpIndex].Ring, (ShortPktRcvd | SetupPndg)))) {
        TraceErr(CommonTrb, "failed to synchronize trb ring for physical endpoint %d", EpIndex);
      }      

      // perform certain actions only on a complete event
      if (Evt->Endpoint.EventType == DWC_EVT_EP_XFER_COMPLETE) {

        if(SetupPndg && (Urb->TrbCtl == DWC_TRB_CTL_CONTROL_DATA)) {
          TraceInfo(CommonTrb, "SetupPndg is set, and it is DWC_TRB_CTL_CONTROL_DATA. Don't Invalid ResourceID");
        }
        else {
          DwcLogUrbComplete(Urb);

          // "release" resources of transfer
          if (Dev->Ep[EpIndex].State != DWC_EP_STATE_STALLED && Dev->Ep[EpIndex].State != DWC_EP_STATE_STALLED_XFER_PENDING) {
            Dev->Ep[EpIndex].State    = DWC_EP_STATE_INACTIVE;
          }
          Dev->Ep[EpIndex].ResourceID = DWC_EP_RESOURCE_ID_INVALID;
          Dev->Ep[EpIndex].Urb        = NULL;
        }
      }
    }
  } // end internal ep event processing

  //
  // Above if block is for common layer processing of events before they are passed up.
  // If we encounter an error in such processing we can NOT return error because the
  // actual retrieval of the event was successful and is ready for consumption by caller.
  //
  return DWC_SUCCESS;
}

