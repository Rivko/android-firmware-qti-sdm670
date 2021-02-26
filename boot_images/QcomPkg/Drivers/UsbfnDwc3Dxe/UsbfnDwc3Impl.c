/**
 * @file UsbfnDwc3Impl.c
 *
 * This file implements the majority of the synopsys core specifics for the
 * UsbfnIo protocol.
 *
 *
 * @copyright Copyright (C) 2013-2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/09/17   ck      Work around the issue that LPM Not Rejected During Control Transfer
 01/09/17   ck      Fix KW issues
 12/20/16   ck      Init SW Xfer resources when driver is loaded
 11/09/16   ck      Reset TRB ring for ClearStall
 11/01/16   ck      Enhance debug message and conditionally fix the STALL issue
 09/22/16   ck      Fix ch9 test failure
 07/29/16   ck      Set the upper bound of waiting for DisconnectEvt.
 06/22/16   ck      Fix a corner case where the driver tries to remove the same URB twice
 05/23/16   ck      Correct the USBFn message generation behavior when a SetupPkt transfer is cancelled.
 01/22/16   ck      Port the fix for SetConfig handling and fix CV test issues.
 03/25/15   vsb     USB LPM, HS/SS PHY config changes, USB mode toggle, clamp UsbFn to HS if SS PHY failed
 11/20/14   ml      Migrate to official SuperSpeed support
 12/18/14   ck      Address software & hardware transfer cancel notification issue
 10/09/14   amitg   Fixed writing to GSBUSCFG0 without clearing the old value
 10/07/14   wufan   Add support of HS Test Mode for compliance testing 
 07/18/14   ar      Fixed possible null pointer dereference issue
 06/17/14   ck      Address invalid memory access issue regarding fake status xfer
 06/12/14   ck      Address pending setup pkt and End Transfer issues
 06/09/14   ck      Remove LANE0_PWR_PRESENT setting which is done in dci_disconnect
 05/20/14   ar      Add PCD value to override max bus speed 
 04/30/14   ar      Cleanup for NDEBUG build  
 08/23/13   mliss   Cleanup and stabilization
 01/20/11   cching  Initial rework for Synopsys Usbfn driver
 =============================================================================*/

#include "UsbfnDwc3Impl.h"
#include "UsbfnDwc3Ch9.h"
#include "UsbfnDwc3Util.h"
#include "UsbfnCtrlSm.h"
#include "DwcHwio.h"

extern EFI_GUID gQcomTokenSpaceGuid;

/*******************************************************************************
 * Static function declarations
 ******************************************************************************/


/*******************************************************************************
 * Initialization / Configuration
 ******************************************************************************/


/**
 * @brief Find endpoint descriptors
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              Endpoint to check
 * @param [in]  EpDir           Direction to check
 * @param [out] Desc            Non-SS endpoint descriptor (if found)
 * @param [out] SSDesc          SS endpoint & companion descriptor (if found)
 *
 * @retval TRUE   The endpoint is in the configuration
 * @retval FALSE  The endpoint is not in the configuration
 */
STATIC
BOOLEAN
UsbfnIsEpInConfiguration (
  IN  USBFN_DEV                               *Usbfn,
  IN  UINT8                                   Ep,
  IN  UINT8                                   EpDir,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR             **Desc,
  OUT EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR  **SSDesc
  )
{
  BOOLEAN                           EpIsInConfig = FALSE;
  EFI_USB_CONFIG_INFO               *ConfigInfo;
  EFI_USB_SUPERSPEED_CONFIG_INFO    *SSConfigInfo;
  EFI_USB_INTERFACE_INFO            *IfcInfo;
  EFI_USB_SUPERSPEED_INTERFACE_INFO *SSIfcInfo;
  UINTN                             Ifc, NumEps, EpIdx;
  UINT8                             EpAddr;

  if (NULL == Usbfn || NULL == Desc || NULL == SSDesc) {
    DBG(EFI_D_ERROR, "invald parameter");
    return FALSE;
  }

  // clear return pointers
  *Desc = NULL;
  *SSDesc = NULL;

  // check for EP in HS descriptors
  ConfigInfo = Usbfn->DeviceInfo->ConfigInfoTable[USBFN_DEFAULT_CONFIG_INDEX];

  // loop through all endpoint descriptors looking for the correct address
  for (Ifc = 0; Ifc < ConfigInfo->ConfigDescriptor->NumInterfaces; Ifc++) {
    IfcInfo = ConfigInfo->InterfaceInfoTable[Ifc];
    NumEps = IfcInfo->InterfaceDescriptor->NumEndpoints;

    for (EpIdx = 0; EpIdx < NumEps; EpIdx++) {
      EpAddr = IfcInfo->EndpointDescriptorTable[EpIdx]->EndpointAddress;
      if (EpAddr == USB_EPDIR_TO_ADDR(Ep, EpDir)) {
        EpIsInConfig = TRUE;
        *Desc = IfcInfo->EndpointDescriptorTable[EpIdx];
        break; // still need to check SS config
      }
    }
  }

  if (NULL == Usbfn->SSDeviceInfo) {
    goto ON_EXIT;
  }

  // check for EP in SS descriptors
  SSConfigInfo = Usbfn->SSDeviceInfo->ConfigInfoTable[USBFN_DEFAULT_CONFIG_INDEX];

  // loop through all endpoint descriptors looking for the correct address
  for (Ifc = 0; Ifc < SSConfigInfo->ConfigDescriptor->NumInterfaces; Ifc++) {
    SSIfcInfo = SSConfigInfo->InterfaceInfoTable[Ifc];
    NumEps = SSIfcInfo->InterfaceDescriptor->NumEndpoints;

    for (EpIdx = 0; EpIdx < NumEps; EpIdx++) {
      EpAddr = SSIfcInfo->EndpointDescriptorTable[EpIdx]->EndpointDescriptor->EndpointAddress;
      if (EpAddr == USB_EPDIR_TO_ADDR(Ep, EpDir)) {
        EpIsInConfig = TRUE;
        *SSDesc = SSIfcInfo->EndpointDescriptorTable[EpIdx];
        goto ON_EXIT;
      }
    }
  }

ON_EXIT:
  return EpIsInConfig;
}


/**
 * @brief Configures the TX FIFOs and allocates one to each IN endpoint
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
STATIC
VOID
UsbfnDwcConfigFifoMapping (
  IN USBFN_DEV                  *Usbfn
  )
{
  EFI_USB_ENDPOINT_DESCRIPTOR             *Desc;
  EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR  *SSDesc;
  UINT8                                   Ep;
  UINTN                                   DwcBase;
  UINT32                                  RegVal = 0;
  UINT32                                  TxFifoIdx = 0;
  UINT32                                  NextTxFifoAddr, NextTxFifoSize;

  FNC_ENTER_MSG();

  DwcBase = Usbfn->DwcUsb3RegBase;

  // Only change the TX FIFO allocation if SS is possible
  if (Usbfn->SSDeviceInfo) {
    DBG(EFI_D_VERBOSE, "Modifying TX FIFO allocation...");

    // Don't change endpoint 0
    RegVal = DWC_HWIO_INI(DwcBase,GTXFIFOSIZ,0);
    DBG(EFI_D_VERBOSE, "Before: GTXFIFOSIZ[%02d] = 0x%08x", 0, RegVal);

    NextTxFifoAddr = DWC_HWIO_INI(DwcBase, GTXFIFOSIZ, 0) & 0xFFFF;

    // Update all TX FIFOs because they are dependent on each other
    for (TxFifoIdx = 1; TxFifoIdx <= DWC_USB_MAX_ENDPOINT_NUMBER; TxFifoIdx++) {

      // Burst TX FIFOs at front
      if (TxFifoIdx <= USBFN_MAX_BURST_IN_ENDPOINTS) {
        NextTxFifoSize = USBFN_TXFIFO_BURST_SIZE;
      } else {
        NextTxFifoSize = USBFN_TXFIFO_NOBURST_SIZE;
      }

      // Dump original value
      RegVal = DWC_HWIO_INI(DwcBase,GTXFIFOSIZ,TxFifoIdx);
      DBG(EFI_D_VERBOSE, "Before: GTXFIFOSIZ[%02d] = 0x%08x", TxFifoIdx, RegVal);

      // Set value to new address and size (see Section 6.2.6 of databook)
      DWC_HWIO_OUTI(DwcBase, GTXFIFOSIZ, TxFifoIdx,
          ((NextTxFifoAddr << 16) | NextTxFifoSize));
      NextTxFifoAddr += NextTxFifoSize;

      // Dump new value
      RegVal = DWC_HWIO_INI(DwcBase,GTXFIFOSIZ,TxFifoIdx);
      DBG(EFI_D_VERBOSE, "After:  GTXFIFOSIZ[%02d] = 0x%08x", TxFifoIdx, RegVal);
    }

    DBG(EFI_D_VERBOSE, "Done with TX FIFO allocation");
  }

  // Clear any previous mapping (invalid value)
  for (Ep = 0; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++) {
    Usbfn->FifoNumMapping[Ep][DWC_EP_DIR_OUT] = 0xff;
    Usbfn->FifoNumMapping[Ep][DWC_EP_DIR_IN]  = 0xff;
  }

  // Always assign FIFO 0 to both directions of the control endpoint
  Usbfn->FifoNumMapping[USBFN_CTRL_EP][DWC_EP_DIR_OUT] = 0;
  Usbfn->FifoNumMapping[USBFN_CTRL_EP][DWC_EP_DIR_IN]  = 0;

  // Always assign FIFO 0 to all OUT endpoints
  for (Ep = 1; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++) {
    Usbfn->FifoNumMapping[Ep][DWC_EP_DIR_OUT] = 0;
  }

  // First burst TX FIFO
  TxFifoIdx = 1;

  // Assign burst TX FIFOs to burst IN endpoints
  if (Usbfn->SSDeviceInfo) {
    for (Ep = 1; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++) {

      if (UsbfnIsEpInConfiguration(Usbfn, Ep, DWC_EP_DIR_IN, &Desc, &SSDesc) &&
          SSDesc && (SSDesc->EndpointCompanionDescriptor->MaxBurst > 0)) {

        Usbfn->FifoNumMapping[Ep][DWC_EP_DIR_IN] = TxFifoIdx++;
        DBG(EFI_D_VERBOSE, "FifoMaping EP %d IN: %d", Ep,
            Usbfn->FifoNumMapping[Ep][DWC_EP_DIR_IN]);
      }
    }
  }

  // Assign TX FIFOs to remaining IN endpoints
  for (Ep = 1; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++) {

    if (UsbfnIsEpInConfiguration(Usbfn, Ep, DWC_EP_DIR_IN, &Desc, &SSDesc) &&
        (0xff == Usbfn->FifoNumMapping[Ep][DWC_EP_DIR_IN])) {

      Usbfn->FifoNumMapping[Ep][DWC_EP_DIR_IN] = TxFifoIdx++;
      DBG(EFI_D_VERBOSE, "FifoMaping EP %d IN: %d", Ep,
          Usbfn->FifoNumMapping[Ep][DWC_EP_DIR_IN]);
    }
  }

  FNC_LEAVE_MSG();
}

/*
 * See header for documentation.
 */
VOID
UsbfnDwcFreeXferRsc (
  IN USBFN_DEV                  *Usbfn
  )
{
  LIST_ENTRY    *ListEntry;
  DWC_URB_NODE  *UrbNode;

  if (NULL == Usbfn) {
    DBG(EFI_D_ERROR, "invalid parameter");
    return;
  }

  // If SetupPkt hasn't been allocated, the UrbPool is uninitialized too
  if (NULL != Usbfn->SetupPkt) {
    UncachedFreePool(Usbfn->SetupPkt);

    // Loop through the UrpPool unlinking and freeing each
    while (!IsListEmpty(&Usbfn->UrbPool)) {
      ListEntry = GetFirstNode(&Usbfn->UrbPool);
      UrbNode = BASE_CR(ListEntry, DWC_URB_NODE, Link);
      RemoveEntryList(ListEntry);

      if (NULL != UrbNode->SgList) {
        FreePool(UrbNode->SgList);
      }
      FreePool(UrbNode);
    }
  }
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcCoreInitImpl (
  IN  USBFN_DEV                 *Usbfn
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  DWC_STATUS       DwcStatus;
  UINT32           RegVal;
  UINT8            Index,Ep,EpDir;
  UINT32           InterrupEnableMask = 0;
  DWC_EP_CMD       EpCmd;
  DWC_DEVICE       *Dev;
  DWC_ADDRESS      DwcAdr;
  UINT32           DwcUsb3RegBase;
  UINT8            UefiVar;
  UINTN            UefiVarSize;

  FNC_ENTER_MSG();

  (void)RegVal; // Make compiler happy

  DBG(EFI_D_VERBOSE,"DwcUsb3RegBase = 0x%x, Usb30Qscratch = 0x%x",Usbfn->DwcUsb3RegBase,Usbfn->Usb30Qscratch);

  DwcUsb3RegBase = Usbfn->DwcUsb3RegBase;

  // 0. Initialize Common Layer Data
  DwcStatus = DwcInitialize (&Usbfn->DwcDevice,DwcUsb3RegBase);
  DWC_ERR_CHK("DwcInitialize failed: 0x%08x", DwcStatus);
  Dev = &Usbfn->DwcDevice;

  // 1. Reset the device core
  DWC_HWIO_OUTF(DwcUsb3RegBase,DCTL,CSFTRST,1);

  Status = DwcWaitRegBit(
      DWC_ADDR(DwcUsb3RegBase, DCTL),
      DWC_BMSK(DCTL, CSFTRST),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  ERR_CHK("Unable to reset the device core");

  // Set LPM NYET Response Threshold
  DWC_HWIO_OUTF(DwcUsb3RegBase,DCTL,LPM_NYET_THRES,0xF);

  // Set HIRD Threshold
  DWC_HWIO_OUTF(DwcUsb3RegBase,DCTL,HIRDTHRES,0x10);


  // 2. AXI master interface configuration - set burst length
  RegVal = DWC_HWIO_IN(DwcUsb3RegBase, GSBUSCFG0);
  RegVal |= (DWC_BMSK(GSBUSCFG0, INCR4BRSTENA) | DWC_BMSK(GSBUSCFG0, INCR8BRSTENA) | DWC_BMSK(GSBUSCFG0, INCR16BRSTENA));
  RegVal = DWC_HWIO_OUT(
    DwcUsb3RegBase,
    GSBUSCFG0,
    RegVal);
  DBG(EFI_D_VERBOSE, "GSBUSCFG0 = 0x%x",RegVal);

  DWC_HWIO_OUTF(DwcUsb3RegBase, GSBUSCFG1, PIPETRANSLIMIT, 0xE);
  RegVal = DWC_HWIO_IN(DwcUsb3RegBase,GSBUSCFG1);
  DBG(EFI_D_VERBOSE, "GSBUSCFG1 = 0x%x",RegVal);

  // 3. Use default value for GTXTHRCFG / GRXTHRCFG
  RegVal = DWC_HWIO_IN(DwcUsb3RegBase,GTXTHRCFG);
  DBG(EFI_D_VERBOSE, "GTXTHRCFG = 0x%x",RegVal);
  RegVal = DWC_HWIO_IN(DwcUsb3RegBase,GRXTHRCFG);
  DBG(EFI_D_VERBOSE, "GRXTHRCFG = 0x%x",RegVal);

  // 4. Read Synopsys ID to find the core version.
  RegVal = DWC_HWIO_IN(DwcUsb3RegBase,GSNPSID);
  DBG(EFI_D_VERBOSE, "Synopsys ID = 0x%x",RegVal);

  // 5. User ID can be programmed if needed

  // 6. Set ENBLSLPM and SUSPENDUSB20 to zero
  DWC_HWIO_OUTFI(Usbfn->DwcUsb3RegBase, GUSB2PHYCFG, 0, ENBLSLPM, 0);
  DWC_HWIO_OUTFI(Usbfn->DwcUsb3RegBase, GUSB2PHYCFG, 0, SUSPENDUSB20, 0);
  RegVal = DWC_HWIO_INI(DwcUsb3RegBase,GUSB2PHYCFG,0);
  DBG(EFI_D_VERBOSE, "GUSB2PHYCFG[0] = 0x%x",RegVal);

  // 7. Clear Elastic Buffer Mode in GUSB3PIPECTL
  DWC_HWIO_OUTFI(DwcUsb3RegBase,GUSB3PIPECTL,0,ELASTIC_BUFFER_MODE,0);
  RegVal = DWC_HWIO_INI(DwcUsb3RegBase,GUSB3PIPECTL,0);
  DBG(EFI_D_VERBOSE, "GUSB3PIPECT[0] = 0x%x",RegVal);

  // 8. Initialize the TX FIFO allocation and mapping
  UsbfnDwcConfigFifoMapping(Usbfn);

  // Dump values for GTXFIFOSIZn/GRXFIFOSIZ0
  for(Index = 0; Index < 32; ++Index){
      RegVal = DWC_HWIO_INI(DwcUsb3RegBase,GTXFIFOSIZ,Index);
      DBG(EFI_D_VERBOSE, "GTXFIFOSIZ[%02d] = 0x%08x",Index,RegVal);
      RegVal = DWC_HWIO_INI(DwcUsb3RegBase,GRXFIFOSIZ,Index);
      DBG(EFI_D_VERBOSE, "GRXFIFOSIZ[%02d] = 0x%08x",Index,RegVal);
  }

  // 9. Program the core to be in the Device only mode and
  // set U2RSTECN
  DWC_HWIO_OUTF(DwcUsb3RegBase,GCTL,U2RSTECN,1);
  DWC_HWIO_OUTF(DwcUsb3RegBase,GCTL,PRTCAPDIR,0x2);
  DWC_HWIO_OUTF(DwcUsb3RegBase,GCTL,PWRDNSCALE,0x2);
  RegVal = DWC_HWIO_IN(DwcUsb3RegBase,GCTL);
  DBG(EFI_D_VERBOSE, "GCTL = 0x%x", RegVal);

  // 10. Enable LPM
  DWC_HWIO_OUTF(DwcUsb3RegBase,DCFG,LPMCAP,1);

  // Fetch USB maximum speed override from PCD database
  UefiVarSize = sizeof(UefiVar);
  Status = gRT->GetVariable(L"UsbfnMaxSpeed", &gQcomTokenSpaceGuid, NULL, &UefiVarSize, &UefiVar);
  if (EFI_ERROR(Status)) {
    Usbfn->MaxSpeed = DWC_DEV_SPEED_DEFAULT;
  }
  else
  {
    Usbfn->MaxSpeed = UefiVar;
  }
  DBG (EFI_D_INFO, "Max UsbFN speed: %d", UefiVar);

  // set max speed to Super if we have SuperSpeed descriptors
  if (Usbfn->SSDeviceInfo && (Usbfn->MaxSpeed == DWC_DEV_SPEED_SS)) {
    DWC_HWIO_OUTF(DwcUsb3RegBase,DCFG,DEVSPD,DWC_DEV_SPEED_SS);
  } else {
    DWC_HWIO_OUTF(DwcUsb3RegBase,DCFG,DEVSPD,DWC_DEV_SPEED_HS);
  }
  RegVal = DWC_HWIO_IN(DwcUsb3RegBase,DCFG);
  DBG(EFI_D_VERBOSE, "DCFG = 0x%x", RegVal);

  // 11. Enable interrupts
  InterrupEnableMask |=
      (
          DWC_BMSK(DEVTEN, DISSCONNEVTEN)       |
          DWC_BMSK(DEVTEN, USBRSTEVTEN)         |
          DWC_BMSK(DEVTEN, CONNECTDONEEVTEN)    |
          DWC_BMSK(DEVTEN, CMDCMPLTEN)          |
          DWC_BMSK(DEVTEN, WKUPEVTEN)           |
          DWC_BMSK(DEVTEN, HIBERNATIONREQEVTEN) |
          DWC_BMSK(DEVTEN, U3L2L1SUSPEN)        |
          DWC_BMSK(DEVTEN, ERRTICERREVTEN)      |
          DWC_BMSK(DEVTEN, EVNTOVERFLOWEN)
      );
  DWC_HWIO_OUT(DwcUsb3RegBase, DEVTEN, InterrupEnableMask);

  // 12 Issue DEPSTARTCFG
  ZeroMem(&EpCmd, sizeof(EpCmd));
  EpCmd.StartNewConfig.Cmd.CmdTyp = DWC_EP_CMD_START_NEW_CONFIG_VAL;
  EpCmd.StartNewConfig.Cmd.CmdAct = 1;
  DwcStatus = DwcWriteEndpointCommand (Dev,0,DWC_EP_DIR_OUT,&EpCmd);
  DWC_ERR_CHK("Failed to write StartNewConfig command: 0x%08x", DwcStatus);
  //poll until cmdact becomes zero
  Status = DwcWaitRegBit(
      DWC_ADDRI(DwcUsb3RegBase, DEPCMD, 0),
      DWC_BMSK(DEPCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  ERR_CHK("DEPSTARTCFG Fails");

  // 13. Config Dev Evt Buffer
  // Allocate event buffer once
  if (NULL == Usbfn->EvtBuffer) {
    Usbfn->EvtBuffer = UncachedAllocateAlignedPool(DWC_EVENT_BUFFER_SIZE, DWC_EVENT_BUFFER_SIZE);
    if (NULL == Usbfn->EvtBuffer) {
      Status = EFI_OUT_OF_RESOURCES;
      DBG(EFI_D_ERROR, "out of memory");
      goto ON_EXIT;
    }
    Usbfn->EvtBufferSize = DWC_EVENT_BUFFER_SIZE;
  }

  DwcAdr.Physical = (UINTN)Usbfn->EvtBuffer;
  DwcAdr.Virtual  = (UINTN)Usbfn->EvtBuffer;
  DwcStatus = DwcConfigureEventBuffer (Dev, DwcAdr, DWC_EVENT_BUFFER_SIZE,
        DWC_EVT_BUFFER_DISABLE_INTR, &Usbfn->EvtBuffIndex);
  DWC_ERR_CHK("DwcConfigureEventBuffer failed: 0x%08x", DwcStatus);

  // 14. Config Phy Ep 0 & 1
  for(Ep = 0; Ep < 1; Ep++){
      for(EpDir = 0; EpDir < USBFN_ENDPOINT_DIR_MAX; EpDir++){

          ZeroMem(&EpCmd, sizeof(EpCmd));
          EpCmd.SetEPConfig.Cmd.CmdTyp = DWC_EP_CMD_SET_EP_CONFIG_VAL;
          EpCmd.SetEPConfig.Cmd.CmdAct = 1;
          EpCmd.SetEPConfig.UsbEpNum =
              (Ep << DWC_EP_CMD_SET_EP_CONFIG_USB_ENDPT_NUM_SHIFT) |
              (EpDir << DWC_EP_CMD_SET_EP_CONFIG_USB_ENDPT_DIR_SHIFT);
          EpCmd.SetEPConfig.FifoNum  = 0;
          EpCmd.SetEPConfig.XferNRdyEn = 1;
          EpCmd.SetEPConfig.XferCmplEn = 1;
          EpCmd.SetEPConfig.MaxPacketSz = 512;
          EpCmd.SetEPConfig.BrstSiz = 0;
          EpCmd.SetEPConfig.EPType = DWC_EP_TYPE_CONTROL;
          EpCmd.SetEPConfig.IntrNum = Usbfn->EvtBuffIndex;

          // Save the current ep config
          CopyMem(&((Usbfn->EpConfig)[Ep][EpDir]), &EpCmd, sizeof(DWC_EP_CMD));
          DBG(EFI_D_VERBOSE, "ConfigEp: EpCmd.Reg 0x%x, Param0 0x%x, Param1 0x%x, Param2 0x%x",
              EpCmd.Template.Cmd.Dword,
              EpCmd.Template.Parameter0,
              EpCmd.Template.Parameter1,
              EpCmd.Template.Parameter2
          );


          DwcStatus = DwcWriteEndpointCommand (Dev,Ep,EpDir,&EpCmd);
          DWC_ERR_CHK("Failed to configure ep %d, dir %d: 0x%08x", Ep, EpDir, DwcStatus);

          //poll until cmdact becomes zero
          Status = DwcWaitRegBit(
                DWC_ADDRI(DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(Ep,EpDir)),
                DWC_BMSK(DEPCMD, CMDACT),
                DWC_WAIT_TO_CLEAR,
                DWC_WAIT_TIME_OUT);
          ERR_CHK("Unable write SetEPConfigCmd");

          // Configure Endpoint resources
          ZeroMem(&EpCmd, sizeof(EpCmd));
          EpCmd.SetEPXferRscConfig.Cmd.CmdTyp = DWC_EP_CMD_SET_EP_XFER_RSC_CONFIG_VAL;
          EpCmd.SetEPXferRscConfig.Cmd.CmdAct = 1;
          EpCmd.SetEPXferRscConfig.NumXferRes = 1;

          DwcStatus = DwcWriteEndpointCommand (Dev,Ep,EpDir,&EpCmd);
          DWC_ERR_CHK("Failed to set resource config for ep %d, dir %d: 0x%0x", Ep, EpDir, DwcStatus);

          // poll until cmdact becomes zero
          Status = DwcWaitRegBit(
              DWC_ADDRI(DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(Ep,EpDir)),
              DWC_BMSK(DEPCMD, CMDACT),
              DWC_WAIT_TO_CLEAR,
              DWC_WAIT_TIME_OUT);
          ERR_CHK("Unable to write SetEPXferRscConfigCmd");

          // allocate TRB buffers on first call
          if (NULL == Usbfn->TrbBuf[Ep][EpDir]) {
            Usbfn->TrbBuf[Ep][EpDir] = UncachedAllocateAlignedPool(
              USBFN_DWC_TRB_DEFAULT_NUM * sizeof(DWC_TRB), DWC_TRB_ALIGN_SIZE);
            if (NULL == Usbfn->TrbBuf[Ep][EpDir]) {
              Status = EFI_OUT_OF_RESOURCES;
              DBG(EFI_D_ERROR, "out of memory");
              goto ON_EXIT;
            }
            Usbfn->TrbNum[Ep][EpDir] = USBFN_DWC_TRB_DEFAULT_NUM;
          }

          // configure the common layer endpoint
          DwcAdr.Physical = (UINTN)Usbfn->TrbBuf[Ep][EpDir];
          DwcAdr.Virtual  = (UINTN)Usbfn->TrbBuf[Ep][EpDir];
          DwcStatus = DwcConfigureEP (Dev, Ep, EpDir, 512,
              Usbfn->EvtBuffIndex, DwcAdr,
              USBFN_DWC_TRB_DEFAULT_NUM * sizeof(DWC_TRB)
          );
          DWC_ERR_CHK("DwcConfigureEP failed for phys ep %d: 0x%08x", EpDir, DwcStatus);

          DBG(EFI_D_INFO, "DwcConfigureEP : Ep %d, EpDir %d, BufferIndex %d, Trb Ring 0x08%x, TrbBufferSize %d",
              Ep,EpDir,Usbfn->EvtBuffIndex,(Usbfn->TrbBuf)[Ep][EpDir],(Usbfn->TrbNum)[Ep][EpDir] * sizeof(DWC_TRB));
      }
  }

  // 15. Enable physical endpoint 0 & 1
  DWC_HWIO_OUT(DwcUsb3RegBase, DALEPENA, 0x3);

  // 16. Start the Control Transfer State Machine (queues first setup packet)
  Status = UsmInitialize(Usbfn);
  ERR_CHK("Failed to activate control state machine");

  // 17. Inform client of cable attach
  UsbfnXtachCb(Usbfn, TRUE);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcSetConfigIfc (
  IN  USBFN_DEV                 *Usbfn
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  DWC_STATUS DwcStatus;
  DWC_EP_CMD EpCmd;
  DWC_DEVICE *Dev;
  UINT32     DwcUsb3RegBase;

  FNC_ENTER_MSG();

  Dev = &Usbfn->DwcDevice;
  DwcUsb3RegBase = Usbfn->DwcUsb3RegBase;

  // Disable all endpoints except physical ep 0 & 1
  DWC_HWIO_OUT(Usbfn->DwcUsb3RegBase,DALEPENA,0x3);

  // Cancel all the transfers except for phy. ep 0 & 1
  UsbfnDwcCancelActiveTransfers(Usbfn);

  //
  // Issue DEPCFG for phy endpt 1 using the current endpoint characteristics
  // to re-initialize the TX FIFO allocation
  //
  ZeroMem(&EpCmd, sizeof(DWC_EP_CMD));
  CopyMem(&EpCmd, &(Usbfn->EpConfig[USBFN_CTRL_EP][DWC_EP_DIR_IN]), sizeof(DWC_EP_CMD));
  EpCmd.SetEPConfig.Cmd.CmdTyp = DWC_EP_CMD_SET_EP_CONFIG_VAL;
  EpCmd.SetEPConfig.Cmd.CmdAct = 1;
  EpCmd.SetEPConfig.ConfigAction = DWC_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_MODIFY;

  DBG(EFI_D_VERBOSE, "EpCmd.Reg 0x%x, Param0 0x%x, Param1 0x%x, Param2 0x%x",
      EpCmd.Template.Cmd.Dword,
      EpCmd.Template.Parameter0,
      EpCmd.Template.Parameter1,
      EpCmd.Template.Parameter2
  );

  DwcStatus = DwcWriteEndpointCommand(Dev, 0, DWC_EP_DIR_IN, &EpCmd);
  DWC_ERR_CHK("DwcWriteEndpointCommand (SetEPConfig) failed: 0x%08x", DwcStatus);

  //poll until cmdact becomes zero
  Status = DwcWaitRegBit(
      DWC_ADDRI(DwcUsb3RegBase, DEPCMD, 1),
      DWC_BMSK(DEPCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  ERR_CHK("Failed to re-initialize the TX FIFO allocation");

  // Re-initialize transfer resources through phy. ep 0
  ZeroMem(&EpCmd, sizeof(DWC_EP_CMD));
  EpCmd.StartNewConfig.Cmd.CmdTyp = DWC_EP_CMD_START_NEW_CONFIG_VAL;
  EpCmd.StartNewConfig.Cmd.CmdAct = 1;
  EpCmd.StartNewConfig.Cmd.CmdParam = 2;

  DBG(EFI_D_VERBOSE, "EpCmd.Reg 0x%x, Param0 0x%x, Param1 0x%x, Param2 0x%x",
        EpCmd.Template.Cmd.Dword,
        EpCmd.Template.Parameter0,
        EpCmd.Template.Parameter1,
        EpCmd.Template.Parameter2
    );

  DwcStatus = DwcWriteEndpointCommand(Dev, 0, DWC_EP_DIR_OUT, &EpCmd);
  DWC_ERR_CHK("DwcWriteEndpointCommand (StartNewConfig) failed: 0x%08x", DwcStatus);

  //poll until cmdact becomes zero
  Status = DwcWaitRegBit(
      DWC_ADDRI(DwcUsb3RegBase, DEPCMD, 0),
      DWC_BMSK(DEPCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  ERR_CHK("Failed to re-initialize the transfer resources");

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcConfigEndpoints (
  IN  USBFN_DEV                 *Usbfn,
  IN  UINT8                     Ep,
  IN  UINT8                     EpDir,
  IN  UINT8                     EpType,
  IN  UINT16                    MaxPktLength,
  IN  BOOLEAN                   StreamCapable,
  IN  UINT8                     BurstSize,
  IN  UINT8                     bInterval
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  DWC_STATUS  DwcStatus;
  UINT32      RegVal;
  DWC_DEVICE  *Dev = &(Usbfn->DwcDevice);
  DWC_EP_CMD  EpCmd;
  DWC_ADDRESS DwcAdr;
  UINT32      DwcUsb3RegBase;

  FNC_ENTER_MSG ();

  DwcUsb3RegBase = Usbfn->DwcUsb3RegBase;

  ZeroMem(&EpCmd, sizeof(DWC_EP_CMD));

  EpCmd.SetEPConfig.Cmd.CmdTyp = DWC_EP_CMD_SET_EP_CONFIG_VAL;
  EpCmd.SetEPConfig.Cmd.CmdAct = 1;

  EpCmd.SetEPConfig.UsbEpNum =
      Ep << DWC_EP_CMD_SET_EP_CONFIG_USB_ENDPT_NUM_SHIFT |
      EpDir << DWC_EP_CMD_SET_EP_CONFIG_USB_ENDPT_DIR_SHIFT;

  if(EpType == UsbEndpointInterrupt){
    EpCmd.SetEPConfig.bInterval_m1 = bInterval - 1;
  }

  if(StreamCapable) {
    EpCmd.SetEPConfig.StrmCap = 1;
    EpCmd.SetEPConfig.StrmEvtEn = 1;
  }

  EpCmd.SetEPConfig.XferNRdyEn   = 0;
  EpCmd.SetEPConfig.XferInProgEn = 0;
  EpCmd.SetEPConfig.XferCmplEn   = 1;
  EpCmd.SetEPConfig.IntrNum      = Usbfn->EvtBuffIndex;
  EpCmd.SetEPConfig.ConfigAction = DWC_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_INIT;
  EpCmd.SetEPConfig.BrstSiz      = BurstSize;
  EpCmd.SetEPConfig.FifoNum      = Usbfn->FifoNumMapping[Ep][EpDir];
  EpCmd.SetEPConfig.MaxPacketSz  = MaxPktLength;
  EpCmd.SetEPConfig.EPType       = EpType;

  // Save the current ep config
  CopyMem(&((Usbfn->EpConfig)[Ep][EpDir]), &EpCmd, sizeof(DWC_EP_CMD));
  DBG(EFI_D_VERBOSE, "DEPCFG: EpCmd.Reg 0x%x, Param0 0x%x, Param1 0x%x, Param2 0x%x",
      EpCmd.Template.Cmd.Dword,
      EpCmd.Template.Parameter0,
      EpCmd.Template.Parameter1,
      EpCmd.Template.Parameter2
  );
  DwcStatus = DwcWriteEndpointCommand(Dev, Ep, EpDir, &EpCmd);
  DWC_ERR_CHK("Failed to set config for ep %d, dir %d: 0x%08x", Ep, EpDir, DwcStatus);

  Status = DwcWaitRegBit(
      DWC_ADDRI(DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(Ep,EpDir)),
      DWC_BMSK(DEPCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  ERR_CHK("DwcWaitRegBit failed: 0x%08x", Status);

  // Issue DEPXFERCFG
  ZeroMem(&EpCmd, sizeof(DWC_EP_CMD));
  EpCmd.SetEPXferRscConfig.Cmd.CmdTyp = DWC_EP_CMD_SET_EP_XFER_RSC_CONFIG_VAL;
  EpCmd.SetEPXferRscConfig.Cmd.CmdAct = 1;
  EpCmd.SetEPXferRscConfig.NumXferRes = 1;

  DwcStatus = DwcWriteEndpointCommand(Dev, Ep, EpDir, &EpCmd);
  DWC_ERR_CHK("Failed to set transfer resource for ep %d, dir %d: 0x%08x", Ep, EpDir, DwcStatus);

  Status = DwcWaitRegBit(
      DWC_ADDRI(DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(Ep,EpDir)),
      DWC_BMSK(DEPCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT);
  ERR_CHK("DwcWaitRegBit failed: 0x%08x", Status);

  // Enable the endpoint
  RegVal = DWC_HWIO_IN(Usbfn->DwcUsb3RegBase,DALEPENA);
  RegVal |= (1 << DWC_EP_INDEX(Ep,EpDir));
  DWC_HWIO_OUT(Usbfn->DwcUsb3RegBase, DALEPENA, RegVal);

  RegVal = DWC_HWIO_IN(Usbfn->DwcUsb3RegBase,DALEPENA);
  DBG(EFI_D_INFO,"DALEPENA: 0x%x",RegVal);

  // Allocate TRB buffer
  if (NULL == Usbfn->TrbBuf[Ep][EpDir]) {
    Usbfn->TrbBuf[Ep][EpDir] = UncachedAllocateAlignedPool(
      USBFN_DWC_TRB_DEFAULT_NUM * sizeof(DWC_TRB), DWC_TRB_ALIGN_SIZE);
    if (NULL == Usbfn->TrbBuf[Ep][EpDir]) {
      Status = EFI_OUT_OF_RESOURCES;
      DBG(EFI_D_ERROR, "out of memory");
      goto ON_EXIT;
    }
    Usbfn->TrbNum[Ep][EpDir] = USBFN_DWC_TRB_DEFAULT_NUM;
  }

  // Configure the common layer endpoint
  DwcAdr.Physical = (UINTN)Usbfn->TrbBuf[Ep][EpDir];
  DwcAdr.Virtual  = (UINTN)Usbfn->TrbBuf[Ep][EpDir];
  DwcStatus = DwcConfigureEP(Dev, Ep, EpDir, MaxPktLength, Usbfn->EvtBuffIndex, DwcAdr,
      Usbfn->TrbNum[Ep][EpDir] * sizeof(DWC_TRB)
  );
  DWC_ERR_CHK("DwcConfigureEP failed for ep %d, dir %d: 0x%08x", Ep, EpDir, DwcStatus);

  DBG(EFI_D_INFO, "Ep %d, EpDir %d, EpType %d, MaxPktLength %d, StreamCap %d, "
    "BurstSize %d, bInterval %d, BufferIndex %d, TrbsPointer 0x%x, TrbBufferSize %d",
    Ep, EpDir, EpType, MaxPktLength, StreamCapable, BurstSize, bInterval,
    Usbfn->EvtBuffIndex,
    (Usbfn->TrbBuf)[Ep][EpDir],
    (Usbfn->TrbNum)[Ep][EpDir] * sizeof(DWC_TRB));

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*******************************************************************************
 * Event Handling
 ******************************************************************************/


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcHandleU1Enable (
  IN  USBFN_DEV                 *Usbfn
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();
  if (NULL == Usbfn) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  //
  // Don't allow the device to initialize the U1 transition but will
  // accept the transition initiated from the link partner
  //
  DWC_HWIO_OUTF(Usbfn->DwcUsb3RegBase, DCTL, ACCEPTU1ENA, 1);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcHandleU2Enable (
  IN  USBFN_DEV                 *Usbfn
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();
  if (NULL == Usbfn) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  //
  // Don't allow the device to initialize the U2 transition but will
  // accept the transition initiated from the link partner
  //
  DWC_HWIO_OUTF(Usbfn->DwcUsb3RegBase, DCTL, ACCEPTU2ENA, 1);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcEnableHsTestMode (
  IN  USBFN_DEV                 *Usbfn
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();
  if (NULL == Usbfn ) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  switch ( Usbfn->HsTestMode ) {
    case USB_TEST_J_STATE:
    case USB_TEST_K_STATE:
    case USB_TEST_SE0_NAK:
    case USB_TEST_PACKET:
    case USB_TEST_FORCE_ENABLE:

      // set the test mode bits
      DWC_HWIO_OUTF(Usbfn->DwcUsb3RegBase, DCTL, TSTCTL, Usbfn->HsTestMode );

      DBG ( EFI_D_INFO, "USB in test mode: %d", Usbfn->HsTestMode );
      break;

    default:
      DBG ( EFI_D_ERROR, "Test mode not supported: 0x%x", Usbfn->HsTestMode );
      Status = EFI_UNSUPPORTED;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}




/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcHandleSetSel (
  IN  USBFN_DEV                 *Usbfn
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  DWC_STATUS        DwcStatus;
  DWC_DEV_CMD       DevCmd;
  DWC_DEVICE        *Dev = &Usbfn->DwcDevice;
  USB_SET_SEL_DATA  *SelData;
  UINT16            SelValue;

  FNC_ENTER_MSG ();

  SelData = &Usbfn->ExitLatency;

  DBG(EFI_D_INFO, "U1SEL 0x%02x, U1PEL 0x%02x, U2SEL 0x%04x, U2PEL 0x%04x",
      SelData->U1SEL, SelData->U1PEL, SelData->U2SEL, SelData->U2PEL);

  // Value to program based on enabled Ux states
  if (DWC_HWIO_INF(Usbfn->DwcUsb3RegBase, DCTL, ACCEPTU2ENA)) {
    SelValue = (SelData->U2PEL > USBFN_MAX_UXSEL_USEC) ?
      USBFN_MAX_UXSEL_USEC : SelData->U2PEL;
  } else if (DWC_HWIO_INF(Usbfn->DwcUsb3RegBase, DCTL, ACCEPTU1ENA)) {
    SelValue = (SelData->U1PEL > USBFN_MAX_UXSEL_USEC) ?
      USBFN_MAX_UXSEL_USEC : SelData->U1PEL;
  } else {
    SelValue = 0;
  }

  // Write the command to tell hardware about new SEL value
  ZeroMem(&DevCmd, sizeof(DWC_DEV_CMD));
  DevCmd.SetPeriodicParams.Cmd.CmdTyp = DWC_DEV_CMD_SET_PERIODIC_PARAMS_VAL;
  DevCmd.SetPeriodicParams.Cmd.CmdAct = 1;
  DevCmd.SetPeriodicParams.SystemExitLatency = SelValue;

  DwcStatus = DwcWriteDeviceCommand(Dev, &DevCmd);
  DWC_ERR_CHK("DwcWriteDeviceCommand SetPeriodicParams failed: 0x%08x", DwcStatus);

  Status = DwcWaitRegBit(
      DWC_ADDR(Usbfn->DwcUsb3RegBase, DGCMD),
      DWC_BMSK(DGCMD, CMDACT),
      DWC_WAIT_TO_CLEAR,
      DWC_WAIT_TIME_OUT
  );
  ERR_CHK("Set SEL Fails");

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcHandleSetAddress (
  IN  USBFN_DEV                 *Usbfn
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  DWC_HWIO_OUTF(Usbfn->DwcUsb3RegBase,DCFG,DEVADDR,Usbfn->Address);

  DBG(EFI_D_INFO,"New Address is 0x%x",Usbfn->Address);

  FNC_LEAVE_MSG ();
  return Status;
}


/**
 * @brief Process a connection event
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 */
STATIC
EFI_STATUS
UsbfnDwcHandleConnectDone (
  IN USBFN_DEV                  *Usbfn
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  DWC_STATUS  DwcStatus;
  UINT8       Ep,EpDir;
  DWC_EP_CMD  EpCmd;
  DWC_DEVICE  *Dev = &(Usbfn->DwcDevice);

  FNC_ENTER_MSG();

  // Clear the flag to indicate that we should now wait for a
  // disconnect event to occur should the cable be detached
  // if HandleDisconnet is running, don't clear DisconnectEvtPosted.
  // This is because the ConnectionDone event belongs to the previous
  // attach and DisconnectEvtPosted relies on DisconnectEvtPosted
  // to terminate
  if (!Usbfn->Disconnecting) {
    Usbfn->DisconnectEvtPosted = FALSE;
  }

  // 1. Read DSTS to determine the speed
  DWC_DEVICE_CONNECT_SPEED const Spd =
      (DWC_DEVICE_CONNECT_SPEED)DWC_HWIO_INF(Usbfn->DwcUsb3RegBase, DSTS, CONNECTSPD);

  DBG(EFI_D_INFO, "The Detected Speed is %a (0x%x)",PrintBusSpeed(Spd),Spd);

  if(DWC_SUPER_SPEED == Spd){
    Usbfn->Speed = UsbBusSpeedSuper;
  } else if (DWC_HIGH_SPEED == Spd){
    Usbfn->Speed = UsbBusSpeedHigh;
  } else if (DWC_LOW_SPEED == Spd){
    Usbfn->Speed = UsbBusSpeedLow;
  } else if (DWC_FULL_SPEED == Spd || DWC_FULL_SPEED_48 == Spd){
    Usbfn->Speed = UsbBusSpeedFull;
  } else {
    USB_ASSERT_GOTO(FALSE, ON_EXIT);
  }

  // 2. Program RAMClkSel
  DWC_HWIO_OUTF(Usbfn->DwcUsb3RegBase, GCTL, RAMCLKSEL, 0);

  // 3. Issue DEPCFG for endpoint 0 and 1
  for(Ep = 0; Ep < 1; Ep++){
    for(EpDir = 0; EpDir < USBFN_ENDPOINT_DIR_MAX; EpDir++){

      ZeroMem(&EpCmd, sizeof(EpCmd));

      EpCmd.SetEPConfig.Cmd.CmdTyp = DWC_EP_CMD_SET_EP_CONFIG_VAL;
      EpCmd.SetEPConfig.Cmd.CmdAct = 1;

      EpCmd.SetEPConfig.UsbEpNum =
          (Ep << DWC_EP_CMD_SET_EP_CONFIG_USB_ENDPT_NUM_SHIFT) |
          (EpDir << DWC_EP_CMD_SET_EP_CONFIG_USB_ENDPT_DIR_SHIFT);
      EpCmd.SetEPConfig.FifoNum  = 0;
      EpCmd.SetEPConfig.XferNRdyEn = 1;
      EpCmd.SetEPConfig.XferCmplEn = 1;
      EpCmd.SetEPConfig.MaxPacketSz = UsbEpMaxPacketSize[UsbEndpointControl][Usbfn->Speed];
      EpCmd.SetEPConfig.BrstSiz = 0;
      EpCmd.SetEPConfig.EPType = DWC_EP_TYPE_CONTROL;
      EpCmd.SetEPConfig.IntrNum = Usbfn->EvtBuffIndex;
      EpCmd.SetEPConfig.ConfigAction = DWC_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_MODIFY;

      // save current ep config setting
      CopyMem(&((Usbfn->EpConfig)[Ep][EpDir]), &EpCmd, sizeof(DWC_EP_CMD));
      DBG(EFI_D_VERBOSE, "ConfigEp: EpCmd.Reg 0x%x, Param0 0x%x, Param1 0x%x, Param2 0x%x",
          EpCmd.Template.Cmd.Dword,
          EpCmd.Template.Parameter0,
          EpCmd.Template.Parameter1,
          EpCmd.Template.Parameter2
      );

      DwcStatus = DwcWriteEndpointCommand (Dev,Ep,EpDir,&EpCmd);
      DWC_ERR_CHK("DwcWriteEndpointCommand (SetEPConfig) failed: 0x%08x", DwcStatus);

      //poll until cmdact becomes zero
      Status = DwcWaitRegBit(
          DWC_ADDRI(Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(Ep,EpDir)),
          DWC_BMSK(DEPCMD, CMDACT),
          DWC_WAIT_TO_CLEAR,
          DWC_WAIT_TIME_OUT);
      ERR_CHK("Unable reconfigure EP %d, EPDIR %d", Ep, EpDir);
    }
  }

  // 4. Suspend the unused core if desired
  /*
  if(Spd == DWC_SUPER_SPEED){
    DWC_HWIO_OUTFI(
        Usbfn->DwcUsb3RegBase,
        GUSB2PHYCFG,
        0,
        SUSPENDUSB20,
          1
    );
  } else {
    DWC_HWIO_OUTFI(
        Usbfn->DwcUsb3RegBase,
        GUSB3PIPECTL,
        0,
        SUSPENDENABLE,
          1
    );
  }
  */
  // (AG) Comment for now, since we are clearing it on disconnect
  // 5. LANE0_PWR_PRESENT OFF
  //DWC_HWIO_OUTF(
  //    Usbfn->Usb30Qscratch,
  //    SS_PHY_CTRL,
  //    LANE0_PWR_PRESENT,
  //    0
  //);

  // 7. Notify client about the speed
  UsbfnSpeedCb(Usbfn, Usbfn->Speed);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcHandleReset (
  IN USBFN_DEV                  *Usbfn,
  IN DWC_EVT                    *Evt
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  USM_STATE   CurrentState = UsmGetCurrentState();

  FNC_ENTER_MSG();

  // 1. Pass event to the control state machine
  Status = UsmProcessEvent(Evt);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Control state machine failed to process reset event");
    goto ON_EXIT;
  }

  // Control state machine must be in correct state to proceed
  if(CurrentState != USM_STATE_CTRL_SETUP) {
    DBG(EFI_D_ERROR,"Cannot Handle Reset! Current Ctrl State is %a",
        UsmGetStateName(CurrentState));
    goto ON_EXIT;
  }

  // 2. End all of the unfinished transfer
  DBG(EFI_D_INFO,"Cancelling all transfers");
  UsbfnDwcCancelActiveTransfers(Usbfn);

  // 3. Loop all endpoints and issue DEPCSTALL clear
  DBG(EFI_D_INFO,"Unstall all endpoints");
  UsbfnDwcClearAllStallEndpoints(Usbfn);

  // 4. Set DevAddr to 0
  DBG(EFI_D_INFO,"Set DevAddr to zero");
  DWC_HWIO_OUTF(Usbfn->DwcUsb3RegBase,DCFG,DEVADDR,0);

  // 5. Reset the common layer
  DwcDeconfigureNonCtrlEPs(&Usbfn->DwcDevice);

  // inform the client a reset is generated
  UsbfnDwcCoreReset(Usbfn);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcHandleDisconnect (
  IN USBFN_DEV                  *Usbfn
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  DWC_STATUS    DwcStatus;
  DWC_DEVICE    *Dev;
  DWC_EP_STATE  CtrlOutState;
  DWC_EP_STATE  CtrlInState;
  DWC_EVT       DiscoEvt;
  UINT32        EvtCnt;
  UINT8         Ep, EpDir;
  CONST UINT8   MAX_WAIT_CNT = 50;
  UINT8         WaitCnt = 0;

  FNC_ENTER_MSG();

  if (NULL == Usbfn) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  // If the ChgPortType is not SDP or CDP, the DWC_DEVICE is not initialized.
  // RS bit is also not set. Simply notify client for disconnect.
  if (Usbfn->ChgPortType == DCI_DWC_CHG_PORT_SDP ||
      Usbfn->ChgPortType == DCI_DWC_CHG_PORT_CDP) {
    Dev = &Usbfn->DwcDevice;
    Usbfn->Disconnecting = TRUE;

    //
    // In order to process the disconnect event the state machine will need to
    // end any non-setup stage transfers. To do so, the common layer needs to
    // read the StartXfer command complete event to obtain the hardware resource
    // id needed for the EndXfer command.  Therefore, if a start transfer command
    // has been executed but the disconnect event shows up before the command
    // complete event, we need to manually read subsequent events until the
    // endpoints are in the correct state to process the disconnect.
    //

    // Get current control endpoint states
    DwcGetEPState(Dev, 0, DWC_EP_DIR_OUT, &CtrlOutState);
    DwcGetEPState(Dev, 0, DWC_EP_DIR_IN, &CtrlInState);

    while (CtrlOutState == DWC_EP_STATE_START_XFER ||
           CtrlInState  == DWC_EP_STATE_START_XFER) {
      DBG(EFI_D_INFO, "Control endpoint not in correct state, processing event");

      // Process an event
      UsbfnDwcEventHandler(Usbfn);

      // Update endpoint states
      DwcGetEPState(Dev, 0, DWC_EP_DIR_OUT, &CtrlOutState);
      DwcGetEPState(Dev, 0, DWC_EP_DIR_IN, &CtrlInState);
    }

    // Process all remaining events, first pass.
    gBS->Stall(10 * 1000); // 10 ms
    Status = DwcGetEventCount(&Usbfn->DwcDevice, DWC_EP_NUM_DEVICE_EVENT, 0, &EvtCnt);

    // Set an upper limit process time.
    while (!EFI_ERROR(Status) && (WaitCnt < MAX_WAIT_CNT) && (!Usbfn->DisconnectEvtPosted || EvtCnt)) {
      // Process an event
      UsbfnDwcEventHandler(Usbfn);

      gBS->Stall(10 * 1000); // 10 ms
      Status = DwcGetEventCount(&Usbfn->DwcDevice, DWC_EP_NUM_DEVICE_EVENT, 0, &EvtCnt);

      WaitCnt++;
    }

    // End all transfers
    UsbfnDwcCancelActiveTransfers(Usbfn);

    // Process all remaining events, second pass
    gBS->Stall(10 * 1000); // 10 ms
    Status = DwcGetEventCount(&Usbfn->DwcDevice, DWC_EP_NUM_DEVICE_EVENT, 0, &EvtCnt);

    WaitCnt = 0;
    while (!EFI_ERROR(Status) && (WaitCnt < MAX_WAIT_CNT) && (!Usbfn->DisconnectEvtPosted || EvtCnt)) {
      // Process an event
      UsbfnDwcEventHandler(Usbfn);

      gBS->Stall(10 * 1000); // 10 ms
      Status = DwcGetEventCount(&Usbfn->DwcDevice, DWC_EP_NUM_DEVICE_EVENT, 0, &EvtCnt);

      WaitCnt++;
    }

    // Fabricate a disconnect event for the state machine to process
    DiscoEvt.Device.IsDeviceEvent = 1;
    DiscoEvt.Device.EventType = DWC_EVT_DEV_DISCONNECT_EVENT;

    // Pass the disconnect event to the state machine to get it into setup state
    Status = UsmProcessEvent(&DiscoEvt);
    ERR_CHK("Control state machine failed to process disconnect event");

    // Process any events generated from processing the disconnect
    gBS->Stall(100 * 1000); // 10 ms
    Status = DwcGetEventCount(&Usbfn->DwcDevice, DWC_EP_NUM_DEVICE_EVENT, 0, &EvtCnt);

    while (!EFI_ERROR(Status) && EvtCnt) {
      // Process an event
      UsbfnDwcEventHandler(Usbfn);

      gBS->Stall(10 * 1000); // 10 ms
      Status = DwcGetEventCount(&Usbfn->DwcDevice, DWC_EP_NUM_DEVICE_EVENT, 0, &EvtCnt);
    }

    // Clear Run/Stop
    DBG(EFI_D_INFO, "Stopping the controller\n"
        "    DSTS.CoreIdle = %d", DWC_HWIO_INF(Usbfn->DwcUsb3RegBase, DSTS, COREIDLE));
    DWC_HWIO_OUTF(Usbfn->DwcUsb3RegBase, DCTL, RUN_STOP, 0);

    // Poll DevCtrlHlt until set
    DwcStatus = DwcWaitRegBit(
        DWC_ADDR(Usbfn->DwcUsb3RegBase, DSTS),
        DWC_BMSK(DSTS, DEVCTRLHLT),
        DWC_WAIT_TO_SET,
        DWC_WAIT_TIME_OUT);
    if (DWC_ERROR(DwcStatus)) {
      Status = EFI_TIMEOUT;
      DBG(EFI_D_ERROR, "Timed out waiting for controller to halt");
    }

    // Clean up DwcDevice for reinitialization on attach
    DwcStatus = DwcCleanDevice(&Usbfn->DwcDevice);
    if (DWC_ERROR(DwcStatus)) {
      Status = EFI_DEVICE_ERROR;
      DBG(EFI_D_ERROR, "Failed to clean up common layer");
    }

    // Return active setup transfer to the pool
    UsbfnDwcCancelTransfer(Usbfn, USBFN_CTRL_EP, DWC_EP_DIR_OUT);

    // Free Trb Buffers
    for (Ep = 0; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++) {
      for (EpDir = 0; EpDir < USBFN_ENDPOINT_DIR_MAX; EpDir++) {
        if (NULL != Usbfn->TrbBuf[Ep][EpDir]) {
          UncachedFreeAlignedPool(Usbfn->TrbBuf[Ep][EpDir]);
          Usbfn->TrbBuf[Ep][EpDir] = NULL;
        }
      }
    }

    Usbfn->Disconnecting = FALSE;
  }

  // Inform client of cable detach
  UsbfnXtachCb(Usbfn, FALSE);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Dispatch function for specific device events handling
 *
 * Dispatch function for specific device events handling
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Evt             The device event to be handled
 *
 * @return EFI_SUCCESS          The operation completes successfully
 * @return others               The operation fails
 */
STATIC
EFI_STATUS
UsbfnDwcHandleDevEvent (
  IN USBFN_DEV                  *Usbfn,
  IN DWC_EVT                    *Evt
  )
{
  EFI_STATUS            Status     = EFI_SUCCESS;
  DWC_EVT_DEV_EVT_TYPE  DevEvtType = (DWC_EVT_DEV_EVT_TYPE)Evt->Device.EventType;

  USB_ASSERT_RETURN((Evt->Device.IsDeviceEvent == TRUE), EFI_INVALID_PARAMETER);

  DBG(EFI_D_INFO,"Event Type %a",PrintDevEvtType(DevEvtType));
  (void)DevEvtType;  // Make compiler happy

  switch ( Evt->Device.EventType )
  {
  case DWC_EVT_DEV_USB_RESET:
    {
      Status = UsbfnDwcHandleReset(Usbfn, Evt);
    }
    break;
  case DWC_EVT_DEV_CONNECT_DONE:
    {
      Status = UsbfnDwcHandleConnectDone(Usbfn);
    }
    break;
  case DWC_EVT_DEV_USB_LINK_STATE_CHANGE:
    {
#ifndef MDEPKG_NDEBUG
      DWC_USB_LINK_STATE LinkState = (DWC_USB_LINK_STATE)(Evt->Device.EventInfo & 0x7);
      BOOLEAN IsSSEvt = (Usbfn->Speed == UsbBusSpeedSuper) ? TRUE : FALSE;

      DBG(EFI_D_INFO,"Link State %a (0x%x)", PrintLinkState(IsSSEvt, LinkState), LinkState);
#endif // MDEPKG_NDEBUG
    }
    break;
  case DWC_EVT_DEV_DISCONNECT_EVENT:
    {
      Usbfn->DisconnectEvtPosted = TRUE;
    }
    break;
  case DWC_EVT_DEV_U3L2L1_SUSPEND:
    {
      UsbfnDwcCorePortStatusChanged(Usbfn, TRUE);
    }
    break;
  case DWC_EVT_DEV_RESUME_REMOTE_WAKE_UP:
    {
      UsbfnDwcCorePortStatusChanged(Usbfn, FALSE);
    }
    break;
  case DWC_EVT_DEV_EVENT_BUFFER_OVERFLOW:
    USB_ASSERT_RETURN(FALSE, EFI_BUFFER_TOO_SMALL); // unrecoverable error
  default:
    DBG(EFI_D_WARN, "Event %a (0x%x) is not handled",  PrintDevEvtType(DevEvtType), Evt->Device.EventType);
    break;
  }

  return Status;
}


/**
 * @brief Handle non-control endpoint events
 *
 * Handle the endpoint event for bulk and interrupt endpoints
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint to be configured
 * @param [in]  EpDir           The direction of the endpoint to be configured
 * @param [in]  Evt             The enndpoint event to be handled
 *
 * @return EFI_SUCCESS          The operation completes successfully
 * @return others               The operation fails
 */
STATIC
EFI_STATUS
UsbfnDwcHandleBulkIntrEpEvent (
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN UINT8                      EpDir,
  IN DWC_EVT                    *Evt
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (NULL == Usbfn || NULL == Evt) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  FNC_ENTER_MSG();

  switch (Evt->Endpoint.EventType) {
  case DWC_EVT_EP_XFER_COMPLETE:
    GenerateXferCompltMsg(Usbfn, Ep, EpDir, FALSE);
    break;
  case DWC_EVT_EP_XFER_NOT_READY:
  {
    DBG(EFI_D_INFO,"Ep %d, Epdir %d, DWC_EVT_EP_XFER_NOT_READY",Ep,EpDir);
  }
  break;
  case DWC_EVT_EP_EP_CMD_COMPLETE:
  {
    DBG(EFI_D_INFO,"Ep %d, Epdir %d, DWC_EVT_EP_EP_CMD_COMPLETE",Ep,EpDir);

    UINT16 EvtType = (Evt->Endpoint.EventParam >> 8);
    DBG(EFI_D_INFO,"Command Type = %a (0x%x)",PrintEpCommandType(EvtType), EvtType);
    (void)EvtType; // Make compiler happy
  }
  break;
  case DWC_EVT_EP_STREAM:
    DBG(EFI_D_ERROR, "Received stream event on non-streaming endpoint!");
  default:
    DBG(EFI_D_INFO, "Evt %a is not handled",PrintEpEvtType(Evt->Endpoint.EventType));
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief The dispatch function for endpoint events
 *
 * Send control endpoint events to the stage machine and all other events
 * to UsbfnDwcHandleBulkIntrEpEvent.
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Evt             The enndpoint event to be handled
 */
STATIC
EFI_STATUS
UsbfnDwcHandleEpEvent (
  IN USBFN_DEV                  *Usbfn,
  IN DWC_EVT                    *Evt
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8      Ep,EpDir;
  UINT32     EvtVal;

  FNC_ENTER_MSG();

  DWC_EP_FROM_INDEX(Evt->Endpoint.PhysEPNum, Ep, EpDir);

  EvtVal = Evt->Template.Rsvd0;
  EvtVal = (EvtVal << 1) | Evt->Template.IsDeviceEvent;
  DBG(EFI_D_INFO,"Handle EP event[0x%x] (%a) for EP %d, EPDir %d: Evt.Param 0x%x, "
      "EvtStatus 0x%x, EvtPhyEpNum 0x%x",
      EvtVal,
      PrintEpEvtType(Evt->Endpoint.EventType),
      Ep,
      EpDir,
      Evt->Endpoint.EventParam,
      Evt->Endpoint.EventStatus,
      Evt->Endpoint.PhysEPNum
  );

  // Enable test mode after acknowledging the SET_FEATURE request
  if ( DWC_EVT_EP_XFER_COMPLETE == Evt->Endpoint.EventType &&
       Usbfn->HsTestMode != USB_TEST_DISABLE ) {

       Status = UsbfnDwcEnableHsTestMode (Usbfn);
       ERR_CHK("Failed to enable HS test mode ");
  }

  // pass control endpoint events to the state machine
  if(Ep == 0) {
    Status = UsmProcessEvent(Evt);
    ERR_CHK("Failed to process control ep event");

  // process events for non-control endpoints
  } else {
    Status = UsbfnDwcHandleBulkIntrEpEvent(Usbfn,Ep,EpDir,Evt);
    ERR_CHK("Failed to process non control ep event");
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcEventHandler (
  IN USBFN_DEV                  *Usbfn
  )
{
  EFI_STATUS   Status;
  DWC_STATUS   DwcStatus;
  UINT32       EvtCnt;
  DWC_EVT      Evt;
  DWC_DEVICE   *Dev = &Usbfn->DwcDevice;

  // check for new events, only one event buffer is used
  DwcStatus = DwcGetEventCount(Dev, DWC_EP_NUM_DEVICE_EVENT, 0, &EvtCnt);
  DWC_ERR_CHK("Failed to get event count: 0x%08x", DwcStatus);

  if(EvtCnt > 0){

    // only process 1 event per poll (regardless of how many are available)
    DwcStatus = DwcGetNextEvent(Dev, DWC_EP_NUM_DEVICE_EVENT, 0, &Evt);
    DWC_ERR_CHK("Failed to get next event: 0x%08x", DwcStatus);

    if(Evt.Template.IsDeviceEvent){
      Status = UsbfnDwcHandleDevEvent(Usbfn, &Evt);
      ERR_CHK("Failed to process device event");
    } else {
      Status = UsbfnDwcHandleEpEvent(Usbfn, &Evt);
      ERR_CHK("Failed to process endpoint event");
    }
  }

ON_EXIT:
  return;
}


/*******************************************************************************
 * Transfer Handling
 ******************************************************************************/


/*
 * See header for documentation.
 */
VOID
UsbfnGetFreeUrb (
  IN  USBFN_DEV                 *Usbfn,
  OUT DWC_URB_NODE              **UrbNode
  )
{
  LIST_ENTRY *ListEntry;

  DBG(EFI_D_VERBOSE,"Total Urb Node in UrbPool %d before get node",Usbfn->UrbPoolSize);
  if(IsListEmpty(&Usbfn->UrbPool)){
      *UrbNode = NULL;
      goto ON_EXIT;
  }

  ListEntry = GetFirstNode(&Usbfn->UrbPool);
  *UrbNode = BASE_CR(ListEntry, DWC_URB_NODE, Link);
  RemoveEntryList(ListEntry);

  Usbfn->UrbPoolSize -= 1;
  DBG(EFI_D_VERBOSE,"Total Urb Node in UrbPool %d after get node",Usbfn->UrbPoolSize);

  USB_ASSERT_GOTO(*UrbNode, ON_EXIT);

ON_EXIT:
  return;
}


/**
 * @brief Initialize a UrbNode from the transfer buffer passed by the client
 *
 * initialize a UrbNode based on the transfer buffer passed in by the client
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint on which the transfer will be queued.
 * @param [in]  EpDir           The direction of the endpoint
 * @param [in]  Buffer          The transfer buffer
 * @param [in]  BufferSize      The size of the transfer buffer
 * @param [out] UrbNode         The Urbnode which will be returned to the client
 *
 * @return EFI_SUCCESS          The operation completes successfully
 * @return others               The operation fails
 */
STATIC
EFI_STATUS
UsbfnDwcCreateUrbNodeFromBuffer (
  IN  USBFN_DEV                 *Usbfn,
  IN  UINT8                     Ep,
  IN  UINT8                     EpDir,
  IN  VOID                      *Buffer,
  IN  UINT32                    BufferSize,
  OUT DWC_URB_NODE              **UrbNode
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      MaxPktSize, Remainder, PadSize = 0;
  UINT8       EpIndex;

  FNC_ENTER_MSG();

  if (NULL == Usbfn || NULL == Buffer || NULL == UrbNode) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  UsbfnGetFreeUrb(Usbfn, UrbNode);
  if (NULL == *UrbNode) {
    Status = EFI_OUT_OF_RESOURCES;
    DBG(EFI_D_ERROR, "no free URB nodes");
    goto ON_EXIT;
  }

  EpIndex = DWC_EP_INDEX(Ep, EpDir);
  MaxPktSize = Usbfn->DwcDevice.Ep[EpIndex].MaxPktSize;

  // use ZLT buffer if client submits a 0 length OUT transfer
  if (Ep > 0 && EpDir == DWC_EP_DIR_OUT && BufferSize == 0) {

    (*UrbNode)->TotalTransactionLength = MaxPktSize;
    (*UrbNode)->SgList->NumberOfElements = 1;
    (*UrbNode)->SgList->Elements[0].Length = MaxPktSize;
    (*UrbNode)->SgList->Elements[0].Address.QuadPart = (UINT64)(*UrbNode)->ZltBuffer;

  } else {

    // calculate size of padding buffer if a nonzero buffer was specified
    if (BufferSize != 0) {
      Remainder = (BufferSize % MaxPktSize);
      if ((Remainder != 0) || Usbfn->PipePolicy.ZLT[Ep][EpDir]) {
        PadSize = MaxPktSize - Remainder;
      }
    }

    // prepare the SGLIST: one element for client buffer, second for padding buffer on out transfers
    (*UrbNode)->SgList->Elements[0].Length = BufferSize;
    (*UrbNode)->SgList->Elements[0].Address.QuadPart = (UINT64)Buffer;

    if (EpDir == DWC_EP_DIR_OUT && PadSize > 0) {
      (*UrbNode)->TotalTransactionLength = BufferSize + PadSize;
      (*UrbNode)->SgList->NumberOfElements = 2;
      (*UrbNode)->SgList->Elements[1].Length = PadSize;
      (*UrbNode)->SgList->Elements[1].Address.QuadPart = (UINT64)((*UrbNode)->ZltBuffer);
    } else {
      (*UrbNode)->TotalTransactionLength = BufferSize;
      (*UrbNode)->SgList->NumberOfElements = 1;
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcRemoveActiveTransferFromList (
  IN  USBFN_DEV                 *Usbfn,
  IN  UINT8                     Ep,
  IN  UINT8                     EpDir,
  OUT DWC_URB_NODE              **UrbNode
  )
{
  LIST_ENTRY   *ListEntry;

  if(IsListEmpty(&(Usbfn->ActiveTransfers)[Ep][EpDir])){
      *UrbNode = NULL;
      return;
  }
  ListEntry = GetFirstNode(&(Usbfn->ActiveTransfers)[Ep][EpDir]);
  *UrbNode = BASE_CR(ListEntry, DWC_URB_NODE, Link);
  RemoveEntryList(ListEntry);
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcCancelActiveTransfers (
  IN USBFN_DEV                  *Usbfn
  )
{
  UINT8        Ep, EpDir;
  FNC_ENTER_MSG();

  if (!Usbfn) {
    goto ON_EXIT;
  }

  for (Ep = 1; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++) {
    for (EpDir = 0; EpDir < USBFN_ENDPOINT_DIR_MAX; EpDir++) {
      if (!IsListEmpty(&(Usbfn->ActiveTransfers[Ep][EpDir]))) {
        UsbfnDwcCancelTransfer(Usbfn, Ep, EpDir);
      } // if !IsListEmpy
    } // for EpDir
  } // for Ep

ON_EXIT:
  FNC_LEAVE_MSG();
  return;
}


/**
 * @brief Perform common layer processing of an URB
 *
 * @param [in]  Usbfn           The context of the SNPS controller core
 * @param [in]  Ep              The endpoint of which the stall state to be modified.
 * @param [in]  EpDir           The direction of the endpoint specifed by Ep
 * @param [in]  UrbNode         DWC_URB_NODE with the transfer to submit
 * @param [in]  TrbCtl          The TRB control code associated with the transfer
 *
 * @retval EFI_SUCCESS          Initialization successful
 * @retval others               Failed to initialize
 */
STATIC
EFI_STATUS
UsbfnDwcSubmitUrbNode (
  IN  USBFN_DEV                 *Usbfn,
  IN  UINT8                     Ep,
  IN  UINT8                     EpDir,
  IN  DWC_URB_NODE              *UrbNode,
  IN  CONST TRB_CTL_TYPE        TrbCtl
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  DWC_STATUS   DwcStatus = DWC_SUCCESS;
  DWC_DEVICE   *Dev = &(Usbfn->DwcDevice);
  BOOLEAN      UseZlt = FALSE;
  UINTN        MaxPktSize;
  UINT8        EpIndex;

  FNC_ENTER_MSG();
  if (NULL == Usbfn || NULL == UrbNode) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // determine if ZLT is neccessary
  EpIndex = DWC_EP_INDEX(Ep, EpDir);
  MaxPktSize = Usbfn->DwcDevice.Ep[EpIndex].MaxPktSize;

  if ((0 == UrbNode->SgList->Elements[0].Length) ||
      (EpDir == DWC_EP_DIR_IN && Usbfn->PipePolicy.ZLT[Ep][EpDir] &&
       (0 == (UrbNode->TotalTransactionLength % MaxPktSize)))) {
    UseZlt = TRUE;
  }
  DBG(EFI_D_INFO, "UseZlt = %a", UseZlt ? "TRUE" : "FALSE");

  // prepare, update and submit the common layer URB
  DwcStatus = DwcPrepareURB(Dev, &UrbNode->Urb, Ep, EpDir, TrbCtl, DWC_DEVICE_NO_STREAM_ID,
      UrbNode->TotalTransactionLength, UseZlt);
  DWC_ERR_CHK("PrepareUrb Failed: 0x%08x", DwcStatus);

  DwcStatus = DwcUpdateURB(Dev, &UrbNode->Urb, UrbNode->SgList);
  DWC_ERR_CHK("UpdateUrb Failed: 0x%08x", DwcStatus);

  DwcStatus = DwcSubmitURB(Dev, &UrbNode->Urb);
  DWC_ERR_CHK("SubmitUrb Failed: 0x%08x", DwcStatus);

  // put the URB node on the active transfer list
  InsertTailList(&(Usbfn->ActiveTransfers)[Ep][EpDir], &UrbNode->Link);

  DBG(EFI_D_INFO, "Num Elem %d, Buffer Addr 0x%llx, Buffer Len %d, ZLT %a",
      UrbNode->SgList->NumberOfElements,
      UrbNode->SgList->Elements[0].Address.QuadPart,
      UrbNode->SgList->Elements[0].Length,
      UseZlt ? "TRUE" : "FALSE");

ON_EXIT:
  if (DWC_ERROR(DwcStatus)) {
      Status = EFI_DEVICE_ERROR;
  }
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcSetEndpointStallState (
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN DWC_ENDPOINT_DIR           EpDir,
  IN BOOLEAN                    State
  )
{
  EFI_STATUS Status    = EFI_SUCCESS;
  DWC_STATUS DwcStatus = DWC_SUCCESS;
  DWC_DEVICE *Dev      = NULL;

  FNC_ENTER_MSG();

  if (!Usbfn) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  DBG(EFI_D_INFO,"StallState %d, Ep %d, EpDir %d",State,Ep,EpDir);

  Dev = &Usbfn->DwcDevice;

  if (!Dev) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  // if stall is on control endpoint, let state machine process stall
  if (Ep == 0) {
    Status = UsmExternalSetStall();
  } else {
    DWC_EP_STATE EpState      = DWC_EP_STATE_UNINITIALIZED;

    DwcStatus = DwcGetEPState(Dev, Ep, EpDir, &EpState);
    if (DWC_ERROR(DwcStatus)) {
      DBG(EFI_D_ERROR, "Failed to get ep state on EP %d, EPDir %d, DwcStatus 0x%x", Ep, EpDir, DwcStatus);
    }

    DwcStatus = DwcSetEndpointStallState(Dev, Ep, EpDir, State);
    DWC_ERR_CHK("Failed to set stall state: 0x%08x", DwcStatus);

    Status = DwcWaitRegBit(
        DWC_ADDRI(Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(Ep,EpDir)),
        DWC_BMSK(DEPCMD, CMDACT),
        DWC_WAIT_TO_CLEAR,
        DWC_WAIT_TIME_OUT);
    ERR_CHK("Fail to set stall state Ep %d Dir %d", Ep, EpDir);

    // If there is a pending xfer, it will be canceled during UNSTALL.
    // Let client know its transfer is canceled.
    if (!State && EpState == DWC_EP_STATE_XFER_IN_PROGRESS) {
      DWC_URB_NODE *UrbNode = NULL;
      // if there is a transfer in progress it will be canceled.
      DBG(EFI_D_INFO, "Inform client xfer cancelled on ep %d, dir %d", Ep, EpDir);
      // Retrieve the finished URB from the endpoint
      UsbfnDwcRemoveActiveTransferFromList(Usbfn, Ep, EpDir, &UrbNode);
      // Resubmit the URB
	  if(UrbNode != NULL){
		Status = UsbfnDwcSubmitUrbNode(Usbfn, UrbNode->Urb.Ep, UrbNode->Urb.EpDir, UrbNode, UrbNode->Urb.TrbCtl);
		ERR_CHK("Failed to submit transfer");
	  }
	  else{
		DBG(EFI_D_ERROR, "Cannot find active xfer");
	  }
    }
  }

ON_EXIT:
  if (DWC_ERROR(DwcStatus)) {
    Status = EFI_DEVICE_ERROR;
  }
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcClearAllStallEndpoints (
  IN USBFN_DEV                  *Usbfn
  )
{
  DWC_STATUS   DwcStatus;
  UINT8        Ep, EpDir;
  DWC_DEVICE   *Dev = &Usbfn->DwcDevice;
  DWC_EP_STATE EpState;

  FNC_ENTER_MSG();

  if (!Usbfn) {
    goto ON_EXIT;
  }

  for(Ep = 0; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++){
    for(EpDir = 0; EpDir < USBFN_ENDPOINT_DIR_MAX; EpDir++){
      EpState   = DWC_EP_STATE_UNINITIALIZED;
      DwcStatus = DwcGetEPState(Dev, Ep, EpDir, &EpState);

      if (DWC_ERROR(DwcStatus)) {
        DBG (EFI_D_ERROR, "Failed to get ep state on EP %d, EPDir %d, DwcStatus 0x%x", Ep, EpDir, DwcStatus);
        continue;
      }

      if(DWC_EP_STATE_STALLED == EpState || DWC_EP_STATE_STALLED_XFER_PENDING == EpState){
          UsbfnDwcSetEndpointStallState(Usbfn, Ep, (DWC_ENDPOINT_DIR) EpDir, FALSE);
      }
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return;
}


/*
 * See header for documentation.
 */
EFI_STATUS
GenerateXferCompltMsg (
  IN  USBFN_DEV                 *Usbfn,
  IN  UINT8                     Ep,
  IN  UINT8                     EpDir,
  IN  BOOLEAN                   Canceled
  )
{
  EFI_STATUS               Status = EFI_SUCCESS;
  EFI_USBFN_MESSAGE_NODE*  NewMsg = NULL;
  DWC_URB*                 Urb = NULL;
  DWC_URB_NODE*            UrbNode = NULL;
  LIST_ENTRY*              FreeMsgEntry = NULL;

  FNC_ENTER_MSG ();

  DBG (EFI_D_INFO, "EP %d, EPDir %d", Ep, EpDir);

  // Retrieve the finished URB from the endpoint
  UsbfnDwcRemoveActiveTransferFromList(Usbfn, Ep, EpDir, &UrbNode);

  if (!UrbNode) {
    DBG(EFI_D_ERROR,"Expecting Data on Ep %d EpDir %d, but there is no Active Transfer",Ep,EpDir);
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  Urb = &(UrbNode->Urb);

  // Store the SEL data stage and handle the new values
  if (Usbfn->StoreSELData) {
    DBG(EFI_D_INFO, "Storing System Exit Latency values");
    Usbfn->StoreSELData = FALSE;

    if (Urb->TotalBytesXfered == sizeof(USB_SET_SEL_DATA)) {
      CopyMem(&Usbfn->ExitLatency,
              (VOID *) FORM_UINTN_FROM_2_UINT32(Urb->XferDesc->BufPtrHi, Urb->XferDesc->BufPtrLo),
          sizeof(USB_SET_SEL_DATA));
    } else {
      DBG(EFI_D_ERROR, "SET_SEL data stage unexpected length %d",
          Urb->TotalBytesXfered);
    }
    UsbfnDwcHandleSetSel(Usbfn);
  }

  // Don't return event to client under hijacking conditions
  if (Usbfn->HijackData.HijackNextRxEvent && (EpDir == DWC_EP_DIR_RX)) {
    DBG(EFI_D_WARN, "Hijacking RX completion event");
    Usbfn->HijackData.HijackNextRxEvent = FALSE;
    goto ON_EXIT;
  }
  if (Usbfn->HijackData.HijackNextTxEvent && (EpDir == DWC_EP_DIR_TX)) {
    DBG(EFI_D_WARN, "Hijacking TX completion event");
    Usbfn->HijackData.HijackNextTxEvent = FALSE;
    goto ON_EXIT;
  }

  // In the case of transfer cancel, don't return the event to the client
  // if the transfer being canceled is the SetupPkt
  if (Canceled && Urb->XferDesc && FORM_UINTN_FROM_2_UINT32(Urb->XferDesc->BufPtrHi, Urb->XferDesc->BufPtrLo) == ((UINTN) Usbfn->SetupPkt)) {
    DBG(EFI_D_INFO, "No return to client: Setup");
    goto ON_EXIT;
  }

  // Get a free Message from the memory pool
  FreeMsgEntry = GetFirstNode(&(Usbfn->UsbfnMessagePool));
  USB_ASSERT_RETURN(FreeMsgEntry, EFI_DEVICE_ERROR);
  RemoveEntryList(FreeMsgEntry);
  NewMsg = BASE_CR(FreeMsgEntry, EFI_USBFN_MESSAGE_NODE, Link);

  // Fill in the message
  NewMsg->Message = (EpDir == DWC_EP_DIR_RX) ?
      EfiUsbMsgEndpointStatusChangedRx : EfiUsbMsgEndpointStatusChangedTx;

  // Return appropriate buffer and length
  if (Usbfn->HijackData.HijackThisRequest) {
    Usbfn->HijackData.HijackThisRequest = FALSE;
    NewMsg->Data.XferCbResult.Buffer           = Usbfn->HijackData.HijackedBuffer;
    NewMsg->Data.XferCbResult.BytesTransferred = Usbfn->HijackData.HijackedBufferLength;
    Usbfn->HijackData.HijackedBuffer       = NULL;
    Usbfn->HijackData.HijackedBufferLength = 0;
  } else {

    //
    // The client expects the same buffer submitted. In the case of
    // status stages the TRB's BufPtrs are 0, so use the pointer we stored
    // when the client submitted the transfer.
    //
    // Urb->XferDesc will be NULL if this is a fake Status xfer used to handle
    // setup pending since update urb function is not called.
    if (Urb->XferDesc && FORM_UINTN_FROM_2_UINT32(Urb->XferDesc->BufPtrHi, Urb->XferDesc->BufPtrLo) != 0 && Urb->TotalBytesXfered > 0) {
      NewMsg->Data.XferCbResult.Buffer =
        (VOID *) FORM_UINTN_FROM_2_UINT32(Urb->XferDesc->BufPtrHi, Urb->XferDesc->BufPtrLo);
    } else {
      NewMsg->Data.XferCbResult.Buffer = UrbNode->ClientBuffer;
    }
    NewMsg->Data.XferCbResult.BytesTransferred = Urb->TotalBytesXfered;
  }

  if(Canceled){
    NewMsg->Data.XferCbResult.TransferStatus = UsbTransferStatusAborted;
  } else {
    NewMsg->Data.XferCbResult.TransferStatus = (0 == Urb->XferTRBSTS) ? UsbTransferStatusComplete : UsbTransferStatusAborted;
  }
  NewMsg->Data.XferCbResult.EndpointIndex = Urb->Ep;
  NewMsg->Data.XferCbResult.EndpointDirection = (EFI_USBFN_ENDPOINT_DIRECTION)Urb->EpDir;


  // Generate a fake status complete event
  if (Usbfn->ByPassStatusStage) {
    Usbfn->ByPassStatusStage = FALSE;
    NewMsg->Data.XferCbResult.BytesTransferred = 0;
    NewMsg->Data.XferCbResult.TransferStatus = UsbTransferStatusComplete;
  }

  // Data stage failed due to pending setup. Mask the case from client. Not the
  // the best solution, but it does not require client to make any change
  if ((Ep == 0) && (Urb->XferTRBSTS == DWC_TRB_STS_SETUP_PENDING) && (Urb->TrbCtl == DWC_TRB_CTL_CONTROL_DATA)) {
    Usbfn->ByPassStatusStage                   = TRUE;
    NewMsg->Data.XferCbResult.TransferStatus   = UsbTransferStatusComplete;
    NewMsg->Data.XferCbResult.BytesTransferred = Urb->TotalTransactLen;
    DBG(EFI_D_INFO, "EP = %d, EPDIR = %d, Usbfn->ByPassStatusStage = %d", Ep, EpDir, Usbfn->ByPassStatusStage);
  }

  // Status stage failed due to pending setup. Mask the case from client. It's OK here since
  // no additional steps needed to be done by either client or the USBFn
  if ((Ep == 0) && (Urb->XferTRBSTS == DWC_TRB_STS_SETUP_PENDING) && (Urb->TrbCtl == DWC_TRB_CTL_CONTROL_STATUS_2 || Urb->TrbCtl == DWC_TRB_CTL_CONTROL_STATUS_3)) {
    Usbfn->ByPassStatusStage = FALSE;
    NewMsg->Data.XferCbResult.BytesTransferred = 0;
    NewMsg->Data.XferCbResult.TransferStatus = UsbTransferStatusComplete;
  }


  InsertTailList(&(Usbfn->UsbfnMessages), &NewMsg->Link);

  DBG(
    EFI_D_INFO,
    "Message = %d, Buffer = 0x%08x, BytesTransferred = 0x%x",
      NewMsg->Message,
      NewMsg->Data.XferCbResult.Buffer,
    NewMsg->Data.XferCbResult.BytesTransferred);

  DBG(
    EFI_D_INFO,
    "TransferStatus %d, EndpointIndex %d, EndpointDirection %d",
      NewMsg->Data.XferCbResult.TransferStatus,
      NewMsg->Data.XferCbResult.EndpointIndex,
    NewMsg->Data.XferCbResult.EndpointDirection);

ON_EXIT:
  // put completed URB back into pool
  if (UrbNode) {
    DBG(EFI_D_VERBOSE,"Total Urb Node in UrbPool before insert %d",Usbfn->UrbPoolSize);
    InsertTailList(&(Usbfn->UrbPool),&UrbNode->Link);
    Usbfn->UrbPoolSize += 1;
    DBG(EFI_D_VERBOSE,"Total Urb Node in UrbPool after insert %d",Usbfn->UrbPoolSize);
  }
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcSendCtrlXfer (
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN UINT8                      EpDir,
  IN CONST TRB_CTL_TYPE         TrbCtl
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  LIST_ENTRY   *ListEntry;
  DWC_URB_NODE *UrbNode;

  FNC_ENTER_MSG();
  DBG(EFI_D_INFO,"EP %d EPDir %d, CtrlCode %d",Ep, EpDir, TrbCtl);

  // check for an existing active transfer
  if (!IsListEmpty(&(Usbfn->ActiveTransfers[Ep][EpDir]))) {
    DBG(EFI_D_ERROR,"The previous URB is still ACTIVE");
    Status = EFI_NOT_READY;
    goto ON_EXIT;
  }

  // check for pending transfer before continuing
  if (IsListEmpty(&(Usbfn->PendingXfer[Ep][EpDir]))) {
    Status = EFI_NOT_READY;
    DBG(EFI_D_ERROR, "No Pending CTRL Xfer");
    goto ON_EXIT;
  }

  // get the first pending transfer from the list
  ListEntry = GetFirstNode(&(Usbfn->PendingXfer[Ep][EpDir]));
  USB_ASSERT_RETURN(ListEntry, EFI_DEVICE_ERROR);
  RemoveEntryList(ListEntry);

  UrbNode = BASE_CR(ListEntry, DWC_URB_NODE, Link);

  // submit the transfer
  Status = UsbfnDwcSubmitUrbNode(Usbfn, Ep, EpDir, UrbNode, TrbCtl);
  ERR_CHK("Failed to submit transfer");

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcTransfer (
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN UINT8                      EpDir,
  IN UINT32                     BufferSize,
  IN VOID                       *Buffer,
  IN VOID                       *ClientBuffer
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  DWC_URB_NODE *UrbNode = NULL;

  FNC_ENTER_MSG();

  if (!Usbfn->IsAttached) {
    DBG(EFI_D_ERROR, "Ep %d, EpDir %d: USB cable not attached, cannot transfer data", Ep, EpDir);
    Status = EFI_NOT_READY;
    goto ON_EXIT;
  }

  DBG(EFI_D_INFO,"Data on (Ep %d EpDir %d): Buffer Addr 0x%x, Buffer Size %d",
      Ep, EpDir, Buffer, BufferSize);

  Status = UsbfnDwcCreateUrbNodeFromBuffer(Usbfn,Ep,EpDir,Buffer,BufferSize,&UrbNode);
  if (EFI_ERROR(Status) || !UrbNode) {
    DBG(EFI_D_ERROR, "Failed to create URB node");
    goto ON_EXIT;
  }
  UrbNode->ClientBuffer = ClientBuffer;

  // place control transfers on the pending list so they will be submitted
  // at the correct time by the control transfer state machine
  if(Ep == 0){
    if (Usbfn->ByPassStatusStage) {
      DWC_DEVICE  *Dev = &Usbfn->DwcDevice;

      // Insert the status directly to the active transfer list, and it will be
      // handled when the CMD COMPLT event for the EndXfer is received.
      // Set Ep and EpDir from the client.
      // Set XferDesc to NULL, which is used as an indicator for fake
      // status xfer
      UrbNode->Urb.Ep       = Ep;
      UrbNode->Urb.EpDir    = EpDir;
      UrbNode->Urb.XferDesc = NULL;
      InsertTailList(&((Usbfn->ActiveTransfers)[Ep][EpDir]), &UrbNode->Link);

      // Cancel the xfer in the data stage associated with the status
      if (DWC_ERROR(DwcCancelTransferByEp(Dev, 0, DWC_EP_DIR_IN))){
        DBG(EFI_D_ERROR, "SetupPndg is TRUE. But DwcCancelTransferByUrb failed for physical endpoint 1");
      }
      else {
        DBG(EFI_D_INFO, "SetupPndg is TRUE. Issue DwcCancelTransferByUrb on physical endpoint 1");
      }
    }
    else {
      InsertTailList(&((Usbfn->PendingXfer)[Ep][EpDir]), &UrbNode->Link);
    }
  // submit non-control transfers right away
  } else {
    Status = UsbfnDwcSubmitUrbNode(Usbfn, Ep, EpDir, UrbNode, DWC_TRB_CTL_NORMAL);
    ERR_CHK("Failed to submit transfer");
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  if (EFI_ERROR(Status)) {
    // put the URB Node back in the pool
    if (UrbNode) {
      InsertTailList(&(Usbfn->UrbPool),&UrbNode->Link);
      Usbfn->UrbPoolSize += 1;
    }
  }
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsbfnDwcCancelTransfer (
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      Ep,
  IN UINT8                      EpDir
  )
{
  DWC_DEVICE   *Dev           = NULL;
  EFI_STATUS   Status         = EFI_SUCCESS;
  DWC_STATUS   DwcStatus      = DWC_SUCCESS;
  UINT32       CmdStatus      = 0;

  FNC_ENTER_MSG();

  if (!Usbfn) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // There should always be setup pkt queued on EP 0 if the cable is attached
  if (Usbfn->IsAttached && Ep == 0 &&
      USM_STATE_CTRL_SETUP == UsmGetCurrentState()) {
    DBG(EFI_D_INFO,"setup Pkt should not be aborted");
    return EFI_SUCCESS;
  }

  Dev = &Usbfn->DwcDevice;

  if (!Dev) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  if (!IsListEmpty(&(Usbfn->ActiveTransfers[Ep][EpDir]))) {
    // Only issue the end command if cable is attached (i.e. controller running)
    if (Usbfn->IsAttached) {
      DwcStatus = DwcCancelTransferByEp(Dev,Ep,EpDir);
      if (DWC_ERROR(DwcStatus)) {
        if (DWC_ENDPOINT_NOT_ACTIVE == DwcStatus) {
          // The xfer is in the active transfer list, but not queued in the
          // hardware. Remove the xfer from the active xfer list
          DwcStatus = DWC_SUCCESS;
        }
        goto ON_EXIT;
      }

      // poll until the command is finished executing
      DwcStatus = DwcWaitRegBit(
          DWC_ADDRI(Usbfn->DwcUsb3RegBase, DEPCMD, DWC_EP_INDEX(Ep,EpDir)),
          DWC_BMSK(DEPCMD, CMDACT),
          DWC_WAIT_TO_CLEAR,
          DWC_WAIT_TIME_OUT);
      if (DWC_ERROR(DwcStatus)) {
        DBG(EFI_D_ERROR, "End Transfer Command on Ep %d, EpDir %d timed out",Ep,EpDir);
        DwcStatus = DWC_SUCCESS;
        goto ON_EXIT;
      }

      // send notification to client if the command completed successfully
      CmdStatus = DWC_HWIO_INFI(
          Usbfn->DwcUsb3RegBase,
          DEPCMD,
          DWC_EP_INDEX(Ep,EpDir),
          CMDSTATUS);
      if (CmdStatus != DWC_EP_EVT_EVENTSTATUS_SUCCESS) {
        DBG(EFI_D_ERROR, "End Transfer Command on Ep %d, EpDir %d failed",Ep,EpDir);
      }
    } // IsAttached

    // Always return UrbNode to pool and notify client
    GenerateXferCompltMsg(Usbfn, Ep, EpDir, TRUE);
  } else {
    DBG(EFI_D_INFO, "No transfer to end");
  }

ON_EXIT:
  // The only time we should return error for cancel transfer
  // is when DwcCancelTransferByEp return error status other than
  // DWC_ENDPOINT_NOT_ACTIVE. This is because once DwcCancelTransferByEp
  // succeeds, the xfer is no longer tracked by common layer.
  if(DWC_SUCCESS != DwcStatus){
    Status = EFI_DEVICE_ERROR;
  }
  FNC_LEAVE_MSG();
  return Status;
}

