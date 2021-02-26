/**
 * @file UsbfnCtrlSmImpl.c
 *
 * @ingroup CtrlSm
 *
 * @brief State function implementation
 *
 * This file implements the state entry and transition functions for use in the
 * control transfer state machine.
 *
 * @copyright Copyright (c) 2013-2014, 2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/18/17   pm      Enhance debug message and conditionally fix the STALL issue
 11/20/14   ml      Migrate to official SuperSpeed support
 06/12/14   ck      Address pending setup pkt and End Transfer issues
 04/30/14   ar      Cleanup for NDEBUG build  
 06/10/13   mliss   Initial implementation
 =============================================================================*/

#include "UsbfnCtrlSm.h"
#include "UsbfnCtrlSmImpl.h"
#include "UsbfnDwc3.h"
#include "UsbfnDwc3Impl.h"
#include "UsbfnDwc3Util.h"
#include "UsbfnDwc3Ch9.h"
#include "DwcHwio.h"


/*******************************************************************************
 * Helper functions
 ******************************************************************************/

EFI_STATUS Usm_EndTransfer_Wait (USM_STATE_MACHINE *Usm, DWC_ENDPOINT_DIR Dir)
{
  EFI_STATUS  Status;
  DWC_STATUS  DwcStatus;

  USB_ASSERT_RETURN(Usm, EFI_INVALID_PARAMETER);

  DBG(EFI_D_INFO, "Ending control transfer on direction %d", Dir);

  // end the control transfer in the specified direction
  Status = UsbfnDwcCancelTransfer(Usm->Usbfn, 0, Dir);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to end control transfer");
  } else {

    // poll until command completes
    DwcStatus = DwcWaitRegBit(
        DWC_ADDRI(Usm->Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(0, Dir)),
        DWC_BMSK(DEPCMD, CMDACT),
        DWC_WAIT_TO_CLEAR,
        DWC_WAIT_TIME_OUT);
    if (DWC_ERROR(DwcStatus)) {
      DBG(EFI_D_ERROR, "Timed out waiting for command to finish");
    }
  }

  return Status;
}


/*******************************************************************************
 * Functions common to multiple states
 ******************************************************************************/

/**
 * @brief Logs that the event is not handled in the current state
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_SAME
 */
USM_STATE Usm_EventNotHandled (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  DBG(EFI_D_VERBOSE, "Event not handled in state %a",
      UsmGetStateName(Usm->CurrentState));
  return USM_STATE_SAME;
}

/**
 * @brief Stall control ep and wait for command to finish
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_Stall_Reset (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  EFI_STATUS  Status;
  DWC_STATUS  DwcStatus;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);

  //
  // must manually stall here (rather than going to SET_STALL state) because the
  // reset needs to be handled after the state transition currently in progress
  //
  Status = UsbfnDwcSetEndpointStallState(Usm->Usbfn, 0, DWC_EP_DIR_OUT, TRUE);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to stall control endpoint in state %a",
        UsmGetStateName(Usm->CurrentState));
  } else {
    // poll until command completes
    DwcStatus = DwcWaitRegBit(
        DWC_ADDRI(Usm->Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(0, DWC_EP_DIR_OUT)),
        DWC_BMSK(DEPCMD, CMDACT),
        DWC_WAIT_TO_CLEAR,
        DWC_WAIT_TIME_OUT);
    if (DWC_ERROR(DwcStatus)) {
      DBG(EFI_D_ERROR, "Timed out waiting for command to finish");
    }
  }

  return USM_STATE_CTRL_SETUP;
}

/**
 * @brief Just transition to CTRL_SETUP
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_NoAction_Disconnect (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  return USM_STATE_CTRL_SETUP;
}

/**
 * @brief Just transition to SET_STALL
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_SET_STALL
 */
USM_STATE Usm_ExternalStall (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  return USM_STATE_SET_STALL;
}


/*******************************************************************************
 * USM_STATE_CTRL_SETUP functions
 ******************************************************************************/

/**
 * @brief Sets up and starts a control-setup transfer
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
VOID Usm_CtrlSetup_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState)
{
  DWC_STATUS    DwcStatus = DWC_SUCCESS;
  DWC_URB_NODE  *UrbNode = NULL;
  DWC_DEVICE    *Dev;

  FNC_ENTER_MSG();

  USB_ASSERT(Usm);

  Dev = &Usm->Usbfn->DwcDevice;
  if (NULL == Dev) {
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  // check for an existing active transfer
  if (!IsListEmpty(&Usm->Usbfn->ActiveTransfers[USBFN_CTRL_EP][DWC_EP_DIR_OUT])) {
    DBG(EFI_D_ERROR,"The previous URB is still ACTIVE");
    goto ON_EXIT;
  }

  // fetch a free Urb
  UsbfnGetFreeUrb(Usm->Usbfn, &UrbNode);

  if (NULL == UrbNode || NULL == UrbNode->SgList) {
    DBG(EFI_D_ERROR, "no free URB nodes");
    goto ON_EXIT;
  }

  // populate the scatter/gather list for setup packet
  UrbNode->TotalTransactionLength = 8;
  UrbNode->SgList->NumberOfElements = 1;
  UrbNode->SgList->Elements[0].Length  = 8;
  UrbNode->SgList->Elements[0].Address.QuadPart = (UINT64)(Usm->Usbfn->SetupPkt);

  // prepare, update and submit the common layer URB
  DwcStatus = DwcPrepareURB(Dev, &UrbNode->Urb, 0, DWC_EP_DIR_OUT, DWC_TRB_CTL_CONTROL_SETUP,
      DWC_DEVICE_NO_STREAM_ID, UrbNode->TotalTransactionLength, FALSE);
  DWC_ERR_CHK("PrepareUrb Failed: 0x%08x", DwcStatus);

  DwcStatus = DwcUpdateURB(Dev, &UrbNode->Urb, UrbNode->SgList);
  DWC_ERR_CHK("UpdateUrb Failed: 0x%08x", DwcStatus);

  DBG(EFI_D_VERBOSE, "SubmitUrb:Setup");
  DwcStatus = DwcSubmitURB(Dev, &UrbNode->Urb);
  DWC_ERR_CHK("SubmitUrb Failed: 0x%08x", DwcStatus);

  // poll until command completes
  DwcStatus = DwcWaitRegBit(
      DWC_ADDRI(Usm->Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(0, DWC_EP_DIR_OUT)),
      DWC_BMSK(DEPCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  if (DWC_ERROR(DwcStatus)) {
    DBG(EFI_D_ERROR, "Timed out waiting for command to finish");
  }

  // put the URB node on the active transfer list
  InsertTailList(&Usm->Usbfn->ActiveTransfers[USBFN_CTRL_EP][DWC_EP_DIR_OUT], &UrbNode->Link);

ON_EXIT:
  // do nothing on error, we should get a NotReady event at some point
  // which will bring us to stall and retry queueing a setup packet
  FNC_LEAVE_MSG();
}

/**
 * @brief Interpret the setup packet
 *
 * Determine which state to transition to based on the contents of
 * the setup packet just received.
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_DATA or USM_STATE_WAIT_HOST_2
 */
USM_STATE Usm_CtrlSetup_XferComplete (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  USM_STATE    NextState = USM_STATE_SAME;
  DWC_URB_NODE *UrbNode = NULL;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((!Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  // fetch the recently completed transfer
  UsbfnDwcRemoveActiveTransferFromList(Usm->Usbfn, 0, DWC_EP_DIR_OUT, &UrbNode);

  USB_ASSERT_RETURN((IsListEmpty(&Usm->Usbfn->ActiveTransfers[USBFN_CTRL_EP][DWC_EP_DIR_OUT])), USM_STATE_SAME);

  // perform some sanity checks on completed URB
  if ((NULL == UrbNode) || (UrbNode->Urb.TotalBytesXfered != 8) ||
      (UrbNode->Urb.SGList->Elements[0].Address.QuadPart != (UINT64)Usm->Usbfn->SetupPkt)) {
    DBG(EFI_D_ERROR, "Completed URB fails checks, stalling EP 0");
    NextState = USM_STATE_SET_STALL;
  } else {

    // process the request
    UsbfnDwcCoreProcessSetupPkt(Usm->Usbfn, Usm->Usbfn->SetupPkt,
        &Usm->HasDataStage, &Usm->DataDir);

    // put the Urb back in the pool
    InsertTailList(&Usm->Usbfn->UrbPool, &UrbNode->Link);
    Usm->Usbfn->UrbPoolSize += 1;

    // determine next state
    if (Usm->HasDataStage) {
      NextState = USM_STATE_CTRL_DATA;
    } else {
      NextState = USM_STATE_WAIT_HOST_2;
    }
  }

  return NextState;
}

/**
 * @brief Shouldn't happen, go to stall
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_SET_STALL
 */
USM_STATE Usm_CtrlSetup_XferNotReady (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  return USM_STATE_SET_STALL;
}


/*******************************************************************************
 * USM_STATE_WAIT_HOST_2 functions
 ******************************************************************************/

/**
 * @brief Do nothing
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
VOID Usm_WaitHost2_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState)
{
  DBG(EFI_D_VERBOSE, "Waiting for host");
}

/**
 * @brief Shouldn't happen, go to stall
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_SET_STALL
 */
USM_STATE Usm_WaitHost2_XferComplete (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  return USM_STATE_SET_STALL;
}

/**
 * @brief Choose next state based on contents of event
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_STATUS_2 or USM_STATE_SET_STALL
 */
USM_STATE Usm_WaitHost2_XferNotReady (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  USM_STATE   NextState = USM_STATE_SAME;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((!Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  // verify that a status stage is requested
  if ((Evt->Endpoint.EventStatus & DWC_EP_EVT_EVENTSTATUS_STAGE_BMSK) ==
       DWC_EP_EVT_EVENTSTATUS_STAGE_STATUS) {
    NextState = USM_STATE_CTRL_STATUS_2;
  } else {
    NextState = USM_STATE_SET_STALL;
  }

  return NextState;
}


/*******************************************************************************
 * USM_STATE_CTRL_STATUS_2 functions
 ******************************************************************************/

/**
 * @brief Starts the control-status-2 transfer
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
VOID Usm_CtrlStatus2_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState)
{
  DWC_STATUS  DwcStatus;

  USB_ASSERT(Usm);

  // Send the status back to host
  UsbfnDwcSendCtrlXfer(Usm->Usbfn, 0, DWC_EP_DIR_IN, DWC_TRB_CTL_CONTROL_STATUS_2);

  // poll until command completes
  DwcStatus = DwcWaitRegBit(
      DWC_ADDRI(Usm->Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(0, DWC_EP_DIR_IN)),
      DWC_BMSK(DEPCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  if (DWC_ERROR(DwcStatus)) {
    DBG(EFI_D_ERROR, "Timed out waiting for command to finish");
  }
}

/**
 * @brief Generates XferComplete notification for client
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_CtrlStatus2_XferComplete (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  EFI_STATUS  Status;
  UINT8       Ep, EpDir;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((!Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  // let the client know that the status stage is done.
  DWC_EP_FROM_INDEX(Evt->Endpoint.PhysEPNum, Ep, EpDir);
  Status = GenerateXferCompltMsg(Usm->Usbfn, Ep, EpDir, FALSE);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to generate XferComplete notification");
  }

  return USM_STATE_CTRL_SETUP;
}

/**
 * @brief Shouldn't happen, go to stall
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_SET_STALL
 */
USM_STATE Usm_CtrlStatus2_XferNotReady (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  return USM_STATE_SET_STALL;
}

/**
 * @brief End the current status 2 transfer
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_CtrlStatus2_Reset_Disconnect (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);

  return USM_STATE_CTRL_SETUP;
}


/*******************************************************************************
 * USM_STATE_CTRL_DATA functions
 ******************************************************************************/

/**
 * @brief Send control data stage
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
VOID Usm_CtrlData_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState)
{
  Usm->DataStageSubmitted = FALSE;
}

/**
 * @brief Sends XferComplete notification to client
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_WAIT_HOST_3
 */
USM_STATE Usm_CtrlData_XferComplete (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  EFI_STATUS  Status;
  UINT8       Ep, EpDir;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((!Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  DWC_EP_FROM_INDEX(Evt->Endpoint.PhysEPNum, Ep, EpDir);
  Status = GenerateXferCompltMsg(Usm->Usbfn, Ep, EpDir, FALSE);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to send XferComplete notification to client");
  }

  return USM_STATE_WAIT_HOST_3;
}

/**
 * @brief 
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP or USM_STATE_SAME
 */
USM_STATE Usm_BypassSts_Handle_PendingSetup  (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  UINT8       Ep, EpDir;
  EFI_STATUS  Status;
  
  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((!Evt->Template.IsDeviceEvent), USM_STATE_SAME);  

  DWC_EP_FROM_INDEX(Evt->Endpoint.PhysEPNum, Ep, EpDir);  
  
  if (Evt->Endpoint.EventType == DWC_EVT_EP_EP_CMD_COMPLETE) {
    UINT8 EpCmdCmpltType = (Evt->Endpoint.EventParam & DWC_EP_EVT_CMDCMPLT_PARAM_TYPE_BMSK) >> DWC_EP_EVT_CMDCMPLT_PARAM_TYPE_SHFT;
    if (EpCmdCmpltType == DWC_EP_CMD_END_XFER_VAL) {
      DBG(EFI_D_INFO, "Usm_CtrlData_Handle_PendingSetup Ep %d, EpDir %d", Ep, EpDir);
      Status = GenerateXferCompltMsg(Usm->Usbfn, Ep, DWC_EP_DIR_OUT, FALSE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to generate XferComplete notification");
      }
      return USM_STATE_CTRL_SETUP;
    }  
  }
  return USM_STATE_SAME;
}

/**
 * @brief Resend control data stage
 *
 * If the direction of the NotReady event matches the control data stage
 * direction, try again to send the transfer.  If there is a direction
 * mismatch end any current transfer and stall the endpoint.
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return Next state to transition into
 */
USM_STATE Usm_CtrlData_XferNotReady (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  USM_STATE   NextState;
  EFI_STATUS  Status;
  DWC_STATUS  DwcStatus;
  UINT8       Ep, EpDir;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((!Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  DWC_EP_FROM_INDEX(Evt->Endpoint.PhysEPNum, Ep, EpDir);
  if ((Ep == 0) && (EpDir == Usm->DataDir)) {

    // only submit transfer on the first not ready event
    if (!Usm->DataStageSubmitted) {

      // send the data submitted for transfer by the client
      Status = UsbfnDwcSendCtrlXfer(Usm->Usbfn, Ep, EpDir, DWC_TRB_CTL_CONTROL_DATA);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to send control data stage");
      } else {

        // poll until command completes
        DwcStatus = DwcWaitRegBit(
            DWC_ADDRI(Usm->Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(Ep, EpDir)),
            DWC_BMSK(DEPCMD, CMDACT),
            DWC_WAIT_TO_CLEAR,
            DWC_WAIT_TIME_OUT);
        if (DWC_ERROR(DwcStatus)) {
          DBG(EFI_D_ERROR, "Timed out waiting for command to finish");
        } else {
          Usm->DataStageSubmitted = TRUE;
        }
      }
    }
    NextState = USM_STATE_SAME;
  } else {

    // event received on unexpected direction
    Status = UsbfnDwcCancelTransfer(Usm->Usbfn, 0, Usm->DataDir);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to end transfer on direction %d", Usm->DataDir);
    }
    NextState = USM_STATE_SET_STALL;
  }

  return NextState;
}

/**
 * @brief End the current data transfer
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_CtrlData_Reset_Disconnect (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  Usm_EndTransfer_Wait(Usm, Usm->DataDir);

  return USM_STATE_CTRL_SETUP;
}


/*******************************************************************************
 * USM_STATE_WAIT_HOST_3 functions
 ******************************************************************************/

/**
 * @brief Do nothing
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
VOID Usm_WaitHost3_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState)
{
  DBG(EFI_D_VERBOSE, "Waiting for host");
}

/**
 * @brief Shouldn't happen, go to stall
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_SET_STALL
 */
USM_STATE Usm_WaitHost3_XferComplete (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  return USM_STATE_SET_STALL;
}

/**
 * @brief Determine next state based on stage and direction of event
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_DATA, USM_STATE_CTRL_STATUS_3 or USM_STATE_SET_STALL
 */
USM_STATE Usm_WaitHost3_XferNotReady (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  USM_STATE   NextState;
  BOOLEAN     IsData;
  UINT8       EpDir;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((!Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  EpDir = Evt->Endpoint.PhysEPNum & 0x1;

  // lower 2 bits of EventStatus indicate which stage the event pertains to
  IsData = ((Evt->Endpoint.EventStatus & DWC_EP_EVT_EVENTSTATUS_STAGE_BMSK) ==
      DWC_EP_EVT_EVENTSTATUS_STAGE_DATA) ? TRUE : FALSE;

  if (IsData) {

    // back to CTRL_DATA if the direction matches
    if (EpDir == Usm->DataDir) {
      NextState = USM_STATE_CTRL_DATA;
    } else {
      NextState = USM_STATE_SET_STALL;
    }
  } else {
    if (Usm->Usbfn->ByPassStatusStage) {
      NextState = USM_STATE_BYPASS_STATUS;
    }
    else {
      NextState = USM_STATE_CTRL_STATUS_3;
    }
  }

  return NextState;
}


/*******************************************************************************
 * USM_STATE_CTRL_STATUS_3 functions
 ******************************************************************************/

/**
 * @brief Starts the control-status-3 transfer
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
VOID Usm_CtrlStatus3_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState)
{
  UINT8   StatusDir;

  USB_ASSERT(Usm);

  // send the status back to host (opposite direction as data stage)
  StatusDir = (Usm->DataDir == DWC_EP_DIR_OUT) ? DWC_EP_DIR_IN : DWC_EP_DIR_OUT;
  UsbfnDwcSendCtrlXfer(Usm->Usbfn, 0, StatusDir, DWC_TRB_CTL_CONTROL_STATUS_3);
}

/**
 * @brief Sends XferComplete notification to client
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_CtrlStatus3_XferComplete (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  EFI_STATUS  Status;
  UINT8       Ep, EpDir;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((!Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  // let the client know that the status stage is done.
  DWC_EP_FROM_INDEX(Evt->Endpoint.PhysEPNum, Ep, EpDir);
  Status = GenerateXferCompltMsg(Usm->Usbfn, Ep, EpDir, FALSE);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to generate XferComplete notification");
  }

  return USM_STATE_CTRL_SETUP;
}

/**
 * @brief Shouldn't happen, go to stall
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_SET_STALL
 */
USM_STATE Usm_CtrlStatus3_XferNotReady (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  return USM_STATE_SET_STALL;
}

/**
 * @brief End the current status 3 transfer
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_CtrlStatus3_Reset_Disconnect (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  DWC_ENDPOINT_DIR  StatusDir;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  // status direction is opposite data direction
  StatusDir = (Usm->DataDir == DWC_EP_DIR_OUT) ? DWC_EP_DIR_IN : DWC_EP_DIR_OUT;

  Usm_EndTransfer_Wait(Usm, StatusDir);

  return USM_STATE_CTRL_SETUP;
}


/*******************************************************************************
 * USM_STATE_SET_STALL functions
 ******************************************************************************/

/**
 * @brief Bypass Status Stage
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
VOID Usm_BypassSts_Entry(USM_STATE_MACHINE *Usm, USM_STATE PrevState)
{
   DBG(EFI_D_VERBOSE, "Bypassing Status Stage");
}

/**
 * @brief Stall the control endpoint
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
VOID Usm_SetStall_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState)
{
  DWC_STATUS  DwcStatus;
  DWC_DEVICE  *Dev;

  USB_ASSERT(Usm);
  Dev = &Usm->Usbfn->DwcDevice;
  USB_ASSERT(Dev);

  // set stall and don't wait
  DwcStatus = DwcSetEndpointStallState(Dev, 0, DWC_EP_DIR_OUT, TRUE);
  if (DWC_ERROR(DwcStatus)) {
    DBG(EFI_D_ERROR, "Failed to set stall state: 0x%08x", DwcStatus);
  }
}

/**
 * @brief Stall has been executed, wait for it to finish
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_SetStall_Reset (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  DWC_STATUS DwcStatus;

  USB_ASSERT_RETURN((Usm && Evt), USM_STATE_SAME);
  USB_ASSERT_RETURN((Evt->Template.IsDeviceEvent), USM_STATE_SAME);

  DwcStatus = DwcWaitRegBit(
      DWC_ADDRI(Usm->Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(0, DWC_EP_DIR_OUT)),
      DWC_BMSK(DEPCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  if (DWC_ERROR(DwcStatus)) {
    DBG(EFI_D_ERROR, "Timed out waiting for command to finish");
  }
  return USM_STATE_CTRL_SETUP;
}

/**
 * @brief Move on to CTRL_SETUP state
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return USM_STATE_CTRL_SETUP
 */
USM_STATE Usm_SetStall_CmdComplete (USM_STATE_MACHINE *Usm, DWC_EVT *Evt)
{
  return USM_STATE_CTRL_SETUP;
}

