/** @file \UsbConfigUsbCores.c

  Function and structure definitions for each USB core supported by this
  platform.

  Copyright (c) 2015 - 2017 QUALCOMM Technologies Inc. All rights reserved.
**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/14/17   amitg    Bus Bandwidth Vote for primary and secondary core and cleanup
 04/26/17   jc       Added missing PMI8998 check to switch primary to SS3 LDO
 03/22/17   pm       Added configuration for sec Clock and core
 03/21/17   amitg    SDM845 UEFI USB Updates
 03/16/17   pm       Enabled the initialization of clk
 03/06/17   amitg    EUD Support
 02/03/17   pm       Vbus Enablement in USB Config Protocol
 02/03/17   pm       Port from Boot.xf.1.4
 01/24/17   pm       Fix the USB clock reference count
 01/19/17   pm       Do not set PIPE3 freq to XO
 12/20/16   sm       Edited input parameter list for EnableOtg PmicUsbProtocol API
 09/22/16   amitg    Napali Pre-Si Updates
 05/06/16   ck       Temp workaround by enable VBUS asap
 04/29/16   ck       Update HS PHY init for host mode
 03/31/16   ck       Disable SS PHY init for BU and correct HS PHY init
 02/23/16   ck       Isolate UsbCore data within the module
 01/22/16   ck       Fix the USB debug message usages
 01/14/16   ck       Enable host mode support in Virtio
 11/24/15   ck       Virtio FBC support
 10/26/15   ck       Init Sdm845 Pre-Si support
=============================================================================*/

#include "UsbConfigUsbCoresPrivate.h"
#include "UsbSharedLib.h"
#include "api/systemdrivers/icbarb.h"


#define FREQ_MHZ(f)     (f * 1000 * 1000)
#define FREQ_KHZ(f)     (f * 1000)

/*
 * USB 3.0 Clock definitions
 */

#define USB3_PIPE_CLK_MIN_FREQ_HZ             FREQ_MHZ(125)

#define USB3_MASTER_CLK_MIN_FREQ_HZ           FREQ_MHZ(120)
#define USB3_AXI_CLK_MIN_FREQ_HZ              FREQ_MHZ(120)
#define USB3_PHY_AUX_CLK_MIN_FREQ_HZ          FREQ_KHZ(1200)

// Mock CLK needs to be set at 19.2 Mhz for Gandalf
#define USB3_MOCK_CLK_MIN_FREQ_HZ             FREQ_KHZ(19200)

#define MICRO_STALL_UNITS(x)  ((x) * 10)
#define MILLI_STALL_UNITS(x)  ((x) * 10000)

#define QUSB_PHY_MAX_STATUS_VERIFY_ATTEMPTS   50


/* Power Domain list */
const CHAR8 SSUsb1PowerDomain[] = "usb30_prim_gdsc";
const CHAR8 SSUsb2PowerDomain[] = "usb30_sec_gdsc";


/*******************************************************************************
 * USB core structures
 ******************************************************************************/

/*
 * USB core enable clock configuration arrays
 */

QCOM_USB_CLOCK SSUsb1EnCoreClocks[] =
{
  //{ "clock name",                     frequency,                    divider }
  {"gcc_cfg_noc_usb3_prim_axi_clk",     USB3_AXI_CLK_MIN_FREQ_HZ,      1 },
  {"gcc_aggre_usb3_prim_axi_clk",       USB3_AXI_CLK_MIN_FREQ_HZ,      1 },
  {"gcc_usb30_prim_master_clk",         USB3_MASTER_CLK_MIN_FREQ_HZ,   1 },
  {"gcc_usb30_prim_sleep_clk",          0,                             1 },
  {"gcc_usb30_prim_mock_utmi_clk",      USB3_MOCK_CLK_MIN_FREQ_HZ,     1 },
  {"gcc_usb_phy_cfg_ahb2phy_clk",       0,                             1 },
  {"gcc_usb3_prim_phy_aux_clk",         USB3_PHY_AUX_CLK_MIN_FREQ_HZ,  1 },
  {"gcc_usb3_prim_phy_pipe_clk",        0,                             1 }
};

QCOM_USB_CLOCK SSUsb2EnCoreClocks[] =
{
  //{ "clock name",                    frequency,                    divider }
  {"gcc_cfg_noc_usb3_sec_axi_clk",     USB3_AXI_CLK_MIN_FREQ_HZ,      1 },
  {"gcc_aggre_usb3_sec_axi_clk",       USB3_AXI_CLK_MIN_FREQ_HZ,      1 },
  {"gcc_usb30_sec_master_clk",         USB3_MASTER_CLK_MIN_FREQ_HZ,   1 },
  {"gcc_usb30_sec_sleep_clk",          0,                             1 },
  {"gcc_usb30_sec_mock_utmi_clk",      USB3_MOCK_CLK_MIN_FREQ_HZ,     1 },
  {"gcc_usb_phy_cfg_ahb2phy_clk",      0,                             1 },
  {"gcc_usb3_sec_phy_aux_clk",         USB3_PHY_AUX_CLK_MIN_FREQ_HZ,  1 },
  {"gcc_usb3_sec_phy_pipe_clk",        0,                             1 }
};

/*
 * USB core disable clock configuration arrays
 */

QCOM_USB_CLOCK SSUsb1DisCoreClocks[] =
{
  {"gcc_cfg_noc_usb3_prim_axi_clk",     USB3_AXI_CLK_MIN_FREQ_HZ,      1 },
  {"gcc_aggre_usb3_prim_axi_clk",       USB3_AXI_CLK_MIN_FREQ_HZ,      1 },
  {"gcc_usb30_prim_master_clk",         USB3_MASTER_CLK_MIN_FREQ_HZ,   1 },
  {"gcc_usb30_prim_sleep_clk",          0,                             1 },
  {"gcc_usb30_prim_mock_utmi_clk",      USB3_MOCK_CLK_MIN_FREQ_HZ,     1 },
  {"gcc_usb_phy_cfg_ahb2phy_clk",       0,                             1 },
  {"gcc_usb3_prim_phy_aux_clk",         USB3_PHY_AUX_CLK_MIN_FREQ_HZ,  1 }, 
  {"gcc_usb3_prim_phy_pipe_clk",        0,                             1 }
};

QCOM_USB_CLOCK SSUsb2DisCoreClocks[] =
{
  {"gcc_cfg_noc_usb3_sec_axi_clk",     USB3_AXI_CLK_MIN_FREQ_HZ,      1 },
  {"gcc_aggre_usb3_sec_axi_clk",       USB3_AXI_CLK_MIN_FREQ_HZ,      1 },
  {"gcc_usb30_sec_master_clk",         USB3_MASTER_CLK_MIN_FREQ_HZ,   1 },
  {"gcc_usb30_sec_sleep_clk",          0,                             1 },
  {"gcc_usb30_sec_mock_utmi_clk",      USB3_MOCK_CLK_MIN_FREQ_HZ,     1 },
  {"gcc_usb_phy_cfg_ahb2phy_clk",      0,                             1 },
  {"gcc_usb3_sec_phy_aux_clk",         USB3_PHY_AUX_CLK_MIN_FREQ_HZ,  1 }, 
  {"gcc_usb3_sec_phy_pipe_clk",        0,                             1 }
};



/*
 * Static Variables
 */
// TLMMProtocol to enable USB HUB
EFI_TLMM_PROTOCOL                             *TLMMProtocol = NULL;

// PMIC GPIO protocol
EFI_QCOM_PMIC_GPIO_PROTOCOL                   *PmicGpioProtocol = NULL;

// NPA Client Handle for SS1 and SS2 Cores
STATIC npa_client_handle                      gPmicNpaClientSS1 = NULL;
STATIC npa_client_handle                      gPmicNpaClientSS2 = NULL;

//NPA client handles for ICB - required for all cores
STATIC npa_client_handle                      gNpaClientSS1Bus = NULL;
STATIC npa_client_handle                      gNpaClientSS2Bus = NULL;

EFI_CLOCK_PROTOCOL                            *ClockProtocol = NULL;
EFI_QCOM_PMIC_USB_PROTOCOL                    *gPmicUsbProtocol = NULL;

extern EFI_GUID                               gQcomTokenSpaceGuid;
extern EFI_PLATFORMINFO_PLATFORM_INFO_TYPE    PlatformInfo;


/*******************************************************************************
 * ICB bandwidth request for USB Port 0 and 1
 ******************************************************************************/
STATIC CONST ICBArb_RequestType usb_icb_request[] = 
{
  {
    .arbType = ICBARB_REQUEST_TYPE_3,
    .arbData.type3 = 
    {
      .uIb = 400000000,        /**< Instantaneous bandwidth in Bytes/second */
      .uAb = 0,                /**< Arbitrated bandwidth in Bytes/second */
      .uLatencyNs = 0          /**< Latency requirement in nanoseconds */
    }
  },
  {
    .arbType = ICBARB_REQUEST_TYPE_3,
    .arbData.type3 = 
    {
      .uIb = 670000000,        /**< Instantaneous bandwidth in Bytes/second */
      .uAb = 670000000,        /**< Arbitrated bandwidth in Bytes/second */
      .uLatencyNs = 0          /**< Latency requirement in nanoseconds */
    }
  }
};

STATIC CONST ICBArb_RequestType usb_icb_clrrequest[] = 
{
  {
    .arbType = ICBARB_REQUEST_TYPE_3,
    .arbData.type3 = 
    {
      .uIb = 0,                /**< Instantaneous bandwidth in Bytes/second */
      .uAb = 0,                /**< Arbitrated bandwidth in Bytes/second */
      .uLatencyNs = 0          /**< Latency requirement in nanoseconds */
    }
  },
  {
    .arbType = ICBARB_REQUEST_TYPE_3,
    .arbData.type3 = 
    {
      .uIb = 0,                /**< Instantaneous bandwidth in Bytes/second */
      .uAb = 0,                /**< Arbitrated bandwidth in Bytes/second */
      .uLatencyNs = 0          /**< Latency requirement in nanoseconds */
    }
  }
};


/*******************************************************************************
 * ICB Master and Slave list for USB Port 0 and USB Port 1
 ******************************************************************************/
STATIC CONST ICBArb_MasterSlaveType usbport0_str_slv_lst[] = 
{
  {
    .eMaster = ICBID_MASTER_APPSS_PROC,
    .eSlave = ICBID_SLAVE_USB3_0
  },
  {
    .eMaster = ICBID_MASTER_USB3_0,
    .eSlave = ICBID_SLAVE_EBI1
  }
};


/*******************************************************************************
 * Core initialization function implementations
 ******************************************************************************/

/*
 * See UsbConfigLibPrivate.h
 */
STATIC
EFI_STATUS
UsbConfigLibOpenProtocols (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG();

  //TODO:CK:Temp. use WRN_CHK to avoid issues
  if (!ClockProtocol) {
    Status = gBS->LocateProtocol(&gEfiClockProtocolGuid, NULL, (VOID**)&ClockProtocol);
    if (EFI_ERROR(Status)) {
      ClockProtocol = NULL;
      WRN_CHK("failed to locate CLOCK protocol");
    }
  }

   if ((!IS_CLS_PLAT(PlatformInfo.platform)) && (!gPmicUsbProtocol)) {
    // Find the PMIC USB charger protocol
    Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&gPmicUsbProtocol);
    if (EFI_ERROR(Status))
    {
      gPmicUsbProtocol = NULL;
      WRN_CHK("Failed to open PMIC USB protocol Status =  (0x%x)", Status);
    }
  }
 
  //Creating NPA sync client for USB Super speed Primary core
  if (!gPmicNpaClientSS1) {
    EFI_PM_DEVICE_INFO_TYPE            PmiDeviceInfo = {0};
    EFI_QCOM_PMIC_VERSION_PROTOCOL    *PmicVersionProtocol = NULL;
    
    // Detect PMI660 presence
    Status = gBS->LocateProtocol(&gQcomPmicVersionProtocolGuid, NULL, (VOID **)&PmicVersionProtocol);
    if (EFI_ERROR(Status))
    {
      DBG(EFI_D_ERROR, "failed to locate PMIC version protocol");
    }
    else
    {
      Status = PmicVersionProtocol->GetPmicInfo(0 /*For PMI 0*/, &PmiDeviceInfo);
      WRN_CHK("failed to read PMIC version");
    }

    // If there is error reading PMIC version or the PMIC reports that PMI 
    // is PMI660, use default SS1 LDO vote for primary USB core.
    // If PMI reports other result (no PMI660), then use alternative SS3 LDO vote
    if (EFI_ERROR(Status) || PmiDeviceInfo.PmicModel == EFI_PMIC_IS_PM660)
    {      
      gPmicNpaClientSS1 = npa_create_sync_client(
        PMIC_NPA_GROUP_ID_USB_SS1, //PMIC_NPA_GROUP_ID_USB_SS1 - Connect to the usb ss1 resource
        "usb_ss1",                 //client name "usb_ss1"
        NPA_CLIENT_REQUIRED);      //NPA_CLIENT_REQUIRED: Request must honor request
    }
    else
    {
      DBG(EFI_D_ERROR, "PMI660 not detected");
      gPmicNpaClientSS1 = npa_create_sync_client(
        PMIC_NPA_GROUP_ID_USB_SS3, //PMIC_NPA_GROUP_ID_USB_SS3 - Connect to the usb ss3 resource
        "usb_ss3",                 //client name "usb_ss3"
        NPA_CLIENT_REQUIRED);      //NPA_CLIENT_REQUIRED: Request must honor request
    }
    if (!gPmicNpaClientSS1)
    {
      DBG(EFI_D_ERROR, "gPmicNpaClientSS1 cannot be created");
    }
  }

  //Create NPA nodes for USB Primary Controller required buses requests
  if (!gNpaClientSS1Bus) {
    gNpaClientSS1Bus = npa_create_sync_client_ex( "/icb/arbiter",
                           "usb_ss1_bus",
                           NPA_CLIENT_VECTOR,
                           sizeof(usbport0_str_slv_lst),
                           (void *)&usbport0_str_slv_lst);

    if (!gNpaClientSS1Bus){
      WRN_CHK("gNpaClientSS1Bus cannot be created");
    }
  }

  if (!PmicGpioProtocol) {
    Status = gBS->LocateProtocol(&gQcomPmicGpioProtocolGuid, NULL, (void **)&PmicGpioProtocol);
    if (EFI_ERROR(Status))
    {
      PmicGpioProtocol = NULL;
      WRN_CHK("Failed to open PMIC GPIO protocol Status =  (0x%x)", Status);
    }
  }

//TODO:CK:Temp. use ON_EXIT, remove Status = EFI_SUCCESS later
//ON_EXIT:
  FNC_LEAVE_MSG();
  Status = EFI_SUCCESS;
  return Status;
}

/**
  Initializes power domains for USB.

  @retval EFI_SUCCESS            USB clocks initialized successfully.
  @retval EFI_NOT_FOUND          Unable to locate clock protocol.

**/
STATIC
EFI_STATUS
EFIAPI
ConfigPowerDomain(
  IN BOOLEAN       bEnable,
  IN CONST CHAR8   *szDomain
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  UINTN        uClockPowerDomainId;

  FNC_ENTER_MSG ();

  if (ClockProtocol)
  {
      Status = ClockProtocol->GetClockPowerDomainID(ClockProtocol, szDomain, &uClockPowerDomainId);
      WRN_CHK ("USBConfigLib:GetClockPowerDomainID failed!");

      if (TRUE == bEnable)
      {
        Status =  ClockProtocol->EnableClockPowerDomain(ClockProtocol, uClockPowerDomainId);
        WRN_CHK ("USBConfigLib:EnableClockPowerDomain failed!");
      }
      else if (FALSE == bEnable)
      {
        Status =  ClockProtocol->DisableClockPowerDomain(ClockProtocol, uClockPowerDomainId);
        WRN_CHK ("USBConfigLib:DisableClockPowerDomain failed!");
      }
  }
  else
  {
      WRN_CHK ("failed to locate Clock Protocol ");
      WRN_CHK ("failed to initialize PowerDomain");
  }

  FNC_LEAVE_MSG ();
  return Status;
}



/**
  Initializes all clocks for a USB core.

  @param [in]  Clocks       Clock array
  @param [in]  ClockCount   Number of clocks in array

  @retval EFI_SUCCESS       USB clocks enabled successfully
  @retval Others            Error enabling at least 1 clock
**/
STATIC
EFI_STATUS
InitUsbClocks (
  IN  UINTN                 ClockCount,
  IN  QCOM_USB_CLOCK        *Clocks
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN       ClkIdx;

  FNC_ENTER_MSG ();

  // validate parameters
  if ((NULL == Clocks) || (0 == ClockCount)) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  if (NULL == ClockProtocol) {
    DBG(EFI_D_ERROR, "ClockProtocol unavailable");
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  // enable each clock
  for (ClkIdx = 0; ClkIdx < ClockCount; ClkIdx++) {
    Status = EnableClock (
                Clocks[ClkIdx].Name,
                Clocks[ClkIdx].Frequency,
                Clocks[ClkIdx].Divider
                );
    if (EFI_ERROR (Status)) {
      DBG(EFI_D_ERROR, "failed to enable clock '%a'", Clocks[ClkIdx].Name);
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
} // end InitUsbClocks



/**
  Disables all clocks for a USB core.

  @param [in]  Clocks       Clock array
  @param [in]  ClockCount   Number of clocks in array

  @retval EFI_SUCCESS       USB clocks disabled successfully
  @retval Others            Error disabling at least 1 clock
**/
STATIC
EFI_STATUS
DisableUsbClocks (
  IN  UINTN                 ClockCount,
  IN  QCOM_USB_CLOCK        *Clocks
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN       ClkIdx;

  FNC_ENTER_MSG ();

  // validate parameters
  if ((NULL == Clocks) || (0 == ClockCount)) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  if (NULL == ClockProtocol) {
    DBG(EFI_D_ERROR, "ClockProtocol unavailable");
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }
  // disable each clock
  for (ClkIdx = 0; ClkIdx < ClockCount; ClkIdx++) {
    Status = DisableClock (Clocks[ClkIdx].Name);
    if (EFI_ERROR (Status)) {
      DBG(EFI_D_ERROR, "failed to disable clock '%a'", Clocks[ClkIdx].Name);
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}



/**
  Enable vbus for primary port (SuperSpeed Port)

  @retval EFI_SUCCESS            Success

**/
EFI_STATUS
EFIAPI
EnableVbusSS1 (
  VOID
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;

  FNC_ENTER_MSG ();

  // For Clamshell Platform Host Mode Enablement is automatically taken care by EC
  if (IS_CLS_PLAT(PlatformInfo.platform)) {
     Status = EFI_SUCCESS;
     goto ON_EXIT;
  }
  else {
    if (NULL == gPmicUsbProtocol) {
      // Find the PMIC USB charger protocol
      Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&gPmicUsbProtocol);
      if (EFI_ERROR(Status))
      {
        gPmicUsbProtocol = NULL;
        WRN_CHK("Failed to open PMIC USB protocol Status =  (0x%x)", Status);
        goto ON_EXIT;
      }
    }

    Status = gPmicUsbProtocol->EnableOtg(TRUE);
    WRN_CHK ("failed to enable Vbus, Status = 0x%x", Status);
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  This API is used for resetting the Link and PHYs using clock control

  @param None

**/
VOID
SynopsysGccReset(
  IN  QCOM_USB_CORE         *UsbCore
)
{
  FNC_ENTER_MSG ();

  if (UsbCore->CoreType == USB_CONFIG_SSUSB1)
  {
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_PRIM_BCR_OFFS, HWIO_GCC_USB30_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_PRIM_BCR_OFFS, HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_PRIM_BCR_OFFS, HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_PRIM_BCR_OFFS, HWIO_GCC_USB3PHY_PHY_PRIM_BCR_BLK_ARES_BMSK);
    gBS->Stall (100);

    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_PRIM_BCR_OFFS, ~HWIO_GCC_USB3PHY_PHY_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_PRIM_BCR_OFFS, ~HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_PRIM_BCR_OFFS, ~HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_PRIM_BCR_OFFS, ~HWIO_GCC_USB30_PRIM_BCR_BLK_ARES_BMSK);
    gBS->Stall (100);
  }
  else if(UsbCore->CoreType == USB_CONFIG_SSUSB2) 
  {
    DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");
    ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);

    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_SEC_BCR_OFFS, HWIO_GCC_USB30_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_SEC_BCR_OFFS, HWIO_GCC_QUSB2PHY_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_SEC_BCR_OFFS, HWIO_GCC_USB3_PHY_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_SEC_BCR_OFFS, HWIO_GCC_USB3PHY_PHY_SEC_BCR_BLK_ARES_BMSK);
    gBS->Stall (100);

    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_SEC_BCR_OFFS, ~HWIO_GCC_USB3PHY_PHY_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_SEC_BCR_OFFS, ~HWIO_GCC_USB3_PHY_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_SEC_BCR_OFFS, ~HWIO_GCC_QUSB2PHY_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_SEC_BCR_OFFS, ~HWIO_GCC_USB30_SEC_BCR_BLK_ARES_BMSK);
    gBS->Stall (100);
  }
  else
  {
    DBG(EFI_D_WARN, "Invalid Core Type");
  }

  FNC_LEAVE_MSG ();
  return;
}


/**
  Initializes High Speed QUSB2 phy for primary core when connected to SNPS Core

  @retval EFI_SUCCESS            High Speed USB phy  initialized
                                 successfully.

**/
EFI_STATUS
InitHSUSBPhy (
  IN QCOM_USB_CORE              *UsbCore
  )
{
  FNC_ENTER_MSG ();

  // Sdm845 RUMI PHY init sequence. 
  if (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI) {
    UINT32 Calibration = 0x1A0;
    UINT32 Clkfreq = 0;
    UINT32 SeBits = HAL_SSUSB_SNPS_R3PC_DCM_CMD_RESET;


    /* Initialization code for QRBTC Rev2*/
    /*d.s QRBTC_USB2_PLL %long 0x00000019    //[1,4]HWIO_QRBTC_USB2_PLL_ADDR
    d.s QRBTC_USB2_PLLCTRL2 %long 0x00000020 //[5,14]HWIO_USB2_PLLCTRL2_ADDR
    d.s QRBTC_USB2_PLLCTRL1 %long 0x00000079 //[4,10]HWIO_USB2_PLLCTRL1_ADDR
    d.s QRBTC_USB2_PLLCTRL3 %long 0x00000000 //[6,18]HWIO_USB2_PLLCTRL3_ADDR
    d.s QRBTC_USB2_PLL %long 0x00000099      //[1,4]HWIO_QRBTC_USB2_PLL_ADDR
    d.s QRTBC_USB2_PLLTEST1 %long 0x00000004 //[2,8]HWIO_USB2_PLLTEST1_ADDR
    d.s QRBTC_USB2_PLL %long 0x000000D9      //[1,4]HWIO_QRBTC_USB2_PLL_ADDR*/

    HAL_USB_WRITE(0, QRBTC_USB2_PLL, 0x00000019);        //[1,4]HWIO_QRBTC_USB2_PLL_ADDR
    gBS->Stall(10);

    HAL_USB_WRITE(0, QRBTC_USB2_PLLCTRL2, 0x00000020);   //[5,14]HWIO_USB2_PLLCTRL2_ADDR
    gBS->Stall(10);                           

    HAL_USB_WRITE(0, QRBTC_USB2_PLLCTRL1, 0x00000079);   //[4,10]HWIO_USB2_PLLCTRL1_ADDR
    gBS->Stall(10);

    HAL_USB_WRITE(0, QRBTC_USB2_PLLCTRL3, 0x00000000);   //[6,18]HWIO_USB2_PLLCTRL3_ADDR
    gBS->Stall(10);

    HAL_USB_WRITE(0, QRBTC_USB2_PLL, 0x00000099);        //[1,4]HWIO_QRBTC_USB2_PLL_ADDR
    gBS->Stall(10);

    HAL_USB_WRITE(0, QRTBC_USB2_PLLTEST1, 0x00000004);   //[2,8]HWIO_USB2_PLLTEST1_ADDR
    gBS->Stall(10);

    HAL_USB_WRITE(0, QRBTC_USB2_PLL, 0x000000D9);        //[1,4]HWIO_QRBTC_USB2_PLL_ADDR



    gBS->Stall(100); // Allow QRBTC to lock PLL

    HAL_USB_WRITE(HS2ADDR, 0x14, 5);

    gBS->Stall(10);

    Clkfreq = HAL_USB_READ(HS2ADDR, 0x14);
    DBG(EFI_D_ERROR, "Init HSUSBPhy USB PHY clock frequency is: %d", Clkfreq);

    gBS->Stall(50);


    /* DCM reset */
    HAL_USB_WRITE(HS2ADDR, 0x20, SeBits);

    gBS->Stall(10);

    HAL_USB_WRITE(HS2ADDR, 0x20, 0x0);

    gBS->Stall(10);

    //shift the DCM
    HAL_USB_WRITE(HS2ADDR, 0x20, Calibration);
    gBS->Stall(50);

   // Report 3.1V rail to Phy (for EUD to kick-in)
   HAL_USB_WRITE(HS2ADDR, 0x28, 0x80);

  }

  if ((PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_RUMI)   &&
      (PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_VIRTIO) &&
      (PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    //Hard reset PHY
    HAL_USB_OR(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_PRIM_BCR_OFFS, HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK);
    gBS->Stall(100);
    HAL_USB_AND(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_PRIM_BCR_OFFS, ~HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK);
    gBS->Stall(100);

    DBG(EFI_D_WARN, "HS Phy Init");
    usb_shared_hs_phy_init();
  }

  FNC_LEAVE_MSG ();
  return EFI_SUCCESS;
}


/**
  Initializes High Speed USB QUSB2 phy for sceondary core when connected to SNPS Core

  @retval EFI_SUCCESS            High Speed USB phy  initialized
                                 successfully.

**/
EFI_STATUS
InitHSUSBPhySec (
  IN QCOM_USB_CORE              *UsbCore
  )
{
  FNC_ENTER_MSG ();

  DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");
  ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);

  // Sdm845 RUMI PHY init sequence for secondary port. 
  if (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI) {
    UINT32 clkfreq_sec        = 0;
    UINT32 TI_PHY_SSC_EN_bit    = 0;
    UINT32 TI_PHY_PIPE_MMCM_SHIFT   = 0;
   
    /*Adding SSC_EN override */
    TI_PHY_SSC_EN_bit = 1;
    outp(HWIO_HS2_USB30_CTRL_ADDR, inp(HWIO_HS2_USB30_CTRL_ADDR) | TI_PHY_SSC_EN_bit << 28);
    gBS->Stall(50); //deo_sleep(50);

    /* Reset PHY POR only */
    outp(HWIO_HS2_LOCAL_RESET_REG_ADDR, 0x1);
    gBS->Stall(100); //deo_sleep(50);
    outp(HWIO_HS2_LOCAL_RESET_REG_ADDR, 0xFFFFFFFF);
    gBS->Stall(50); //deo_sleep(50);

    /* Setting USB3 PIPE output on inverted clock edge */
    outp(HWIO_HS2_USB30_CTRL_ADDR, inp(HWIO_HS2_USB30_CTRL_ADDR)| 0x1);

    /* Put the PHY out of Reset and check the clock */
    outp(HWIO_HS2_LOCAL_RESET_REG_ADDR, 0xFFFFFFFE);
    gBS->Stall(100); //deo_sleep(100);

    /* Shifting ULPI/UTMI clock MMCM */
    outp(HWIO_HS2_USB_CLK_CTRL_ADDR,0x00100000);
    gBS->Stall(10); //deo_sleep(10);
    outp(HWIO_HS2_USB_CLK_CTRL_ADDR,0x00000000);
    gBS->Stall(10); //deo_sleep(10);
    outp(HWIO_HS2_USB_CLK_CTRL_ADDR,0x000001A0); /* MMCM shift value */
    gBS->Stall(50); //deo_sleep(50);
    DBG(EFI_D_ERROR, "UTMI MMCM clock shift value set");

    /* Shifting PIPE3 clock MMCM */
    outp(HWIO_HS2_USB_PIPE3_CLK_CTRL_ADDR,0x00100000);
    gBS->Stall(10); //deo_sleep(10);
    outp(HWIO_HS2_USB_PIPE3_CLK_CTRL_ADDR,0x00000000);
    gBS->Stall(10); //deo_sleep(10);
    TI_PHY_PIPE_MMCM_SHIFT = 0x00010140;
    outp(HWIO_HS2_USB_PIPE3_CLK_CTRL_ADDR, TI_PHY_PIPE_MMCM_SHIFT);
    gBS->Stall(10); //deo_sleep(10);
    DBG(EFI_D_ERROR, "PIPE3 MMCM clock shift value set");

    /* Clear the other Resets */
    outp(HWIO_HS2_LOCAL_RESET_REG_ADDR, 0x0);
    gBS->Stall(50); //deo_sleep(50); 

    /* Put clock selector to read UTMI/ULPI clock */
    outp(HWIO_HS2_CLK_STATUS_SEL_ADDR, 0x00000009);
    gBS->Stall(10); //deo_sleep(10);
   
    clkfreq_sec=inp(HWIO_HS2_CLK_STATUS_ADDR);
    DBG(EFI_D_ERROR, "uw_ssusb_init_QMP_PHY 2: USB PHY UTMI clock frequency is: %d", clkfreq_sec);

    /*Put clock selector to read PIPE3 clock */
    outp(HWIO_HS2_CLK_STATUS_SEL_ADDR,0x0000000B);
    gBS->Stall(10); //deo_sleep(10);
   
    clkfreq_sec=inp(HWIO_HS2_CLK_STATUS_ADDR);
    DBG(EFI_D_ERROR, "uw_ssusb_init_QMP_PHY 2: USB PHY PIPE3 clock frequency is: %d", clkfreq_sec);
   
    /* Put clock selector to read PCLK clock */
    outp(HWIO_HS2_CLK_STATUS_SEL_ADDR,0x0000000A);
    gBS->Stall(10); //deo_sleep(10);
   
    clkfreq_sec=inp(HWIO_HS2_CLK_STATUS_ADDR);
    DBG(EFI_D_ERROR, "uw_ssusb_init_QMP_PHY 2: USB PHY PCLK clock frequency is: %d", clkfreq_sec);
    
    }

  if ((PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_RUMI)   &&
      (PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_VIRTIO) &&
      (PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    //Hard reset PHY
    HAL_USB_OR(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_SEC_BCR_OFFS, HWIO_GCC_QUSB2PHY_SEC_BCR_BLK_ARES_BMSK);
    gBS->Stall(100);
    HAL_USB_AND(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_SEC_BCR_OFFS, ~HWIO_GCC_QUSB2PHY_SEC_BCR_BLK_ARES_BMSK);
    gBS->Stall(100);

    DBG(EFI_D_INFO, "HS Phy Init Sec");
    usb_shared_hs_phy_init_sec();
  }

  FNC_LEAVE_MSG ();
  return EFI_SUCCESS;
}


/**
  Initializes super speed Primary USB phy

  @retval EFI_SUCCESS            Super Speed USB phy  initialized
                                 successfully.

**/
STATIC
EFI_STATUS
InitSSUSBPhy (
  IN QCOM_USB_CORE              *UsbCore
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 MaxSpeedUefiVar;
  UINTN VarSize;

  FNC_ENTER_MSG ();

  // not needed for Sdm845 PRE-SI platform
  if ((PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI)   ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_VIRTIO) ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    DBG(EFI_D_WARN, "Pre-SI: Bypass InitSSUSBPhy");
    goto ON_EXIT;
  }

  if (UsbCore->BaseAddr != USB3_PRI_USB30_PRIM_BASE) {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  //Disable QMP PIPE3 clock branch: turn on the QMP PIPE3 clock output to the controller while the PHY is in 
  //the reset state (prior to releasing PHY_START and PHY_SW_RESET) since this clock also comes back to the 
  //PHY and needs to be sourced from the PHY pipe clock output when the PHY starts up.
  HAL_USB_AND(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PRIM_PHY_PIPE_CBCR_OFFS, ~(UINT32)HWIO_GCC_USB3_PRIM_PHY_PIPE_CBCR_CLK_ENABLE_BMSK);

  // Reset the PHY block
  HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_PRIM_BCR_OFFS, HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_BMSK);
  HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_PRIM_BCR_OFFS, HWIO_GCC_USB3PHY_PHY_PRIM_BCR_BLK_ARES_BMSK);
  gBS->Stall (100);
  HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_PRIM_BCR_OFFS, ~HWIO_GCC_USB3PHY_PHY_PRIM_BCR_BLK_ARES_BMSK);
  HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_PRIM_BCR_OFFS, ~HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_BMSK);
  gBS->Stall (100);

  //Enable QMP PIPE3 clock branch
  HAL_USB_OR(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PRIM_PHY_PIPE_CBCR_OFFS, HWIO_GCC_USB3_PRIM_PHY_PIPE_CBCR_CLK_ENABLE_BMSK);

  if(!usb_shared_ss_phy_init(UsbCore->LaneNum)) {
    DBG(EFI_D_ERROR, "Init SSUSBPhy Enable Error");
    Status = EFI_DEVICE_ERROR; 
    goto ON_EXIT;
  }

  //On success, set UFN max speed to SS
  MaxSpeedUefiVar = HWIO_USB3_PRI_DCFG_DEVSPD_SUPERSPEED_FVAL;

ON_EXIT:
  //Set the maximum speed supported by controller
  VarSize = sizeof(MaxSpeedUefiVar);
  gRT->SetVariable (L"UsbfnMaxSpeed", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &MaxSpeedUefiVar);

  FNC_LEAVE_MSG ();
  return Status;
}


 /**
  Initializes Super Speed Secondary USB phy

  @retval EFI_SUCCESS            Super Speed USB phy  initialized
                                 successfully.

**/
STATIC
EFI_STATUS
InitSSUSBPhySec (
  IN QCOM_USB_CORE              *UsbCore
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 MaxSpeedUefiVar;
  UINTN VarSize;

  FNC_ENTER_MSG ();

  DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");
  ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);

  // not needed for Sdm845 PRE-SI platform
  if ((PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI)   ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_VIRTIO) ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    DBG(EFI_D_WARN, "Pre-SI: Bypass InitSSUSBPhySec");
    goto ON_EXIT;
  }

  /*if (UsbCore->BaseAddr != USB3_2ND_USB30_PRIM_BASE) {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }*/

  //Disable QMP PIPE3 clock branch: turn on the QMP PIPE3 clock output to the controller while the PHY is in 
  //the reset state (prior to releasing PHY_START and PHY_SW_RESET) since this clock also comes back to the 
  //PHY and needs to be sourced from the PHY pipe clock output when the PHY starts up.
  HAL_USB_AND(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_SEC_PHY_PIPE_CBCR_OFFS, ~(UINT32)HWIO_GCC_USB3_SEC_PHY_PIPE_CBCR_CLK_ENABLE_BMSK);

  // Reset the PHY block
  HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_SEC_BCR_OFFS, HWIO_GCC_USB3_PHY_SEC_BCR_BLK_ARES_BMSK);
  HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_SEC_BCR_OFFS, HWIO_GCC_USB3PHY_PHY_SEC_BCR_BLK_ARES_BMSK);
  gBS->Stall (100);
  HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_SEC_BCR_OFFS, ~HWIO_GCC_USB3PHY_PHY_SEC_BCR_BLK_ARES_BMSK);
  HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_SEC_BCR_OFFS, ~HWIO_GCC_USB3_PHY_SEC_BCR_BLK_ARES_BMSK);
  gBS->Stall (100);

  //Enable QMP PIPE3 clock branch
  HAL_USB_OR(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_SEC_PHY_PIPE_CBCR_OFFS, HWIO_GCC_USB3_SEC_PHY_PIPE_CBCR_CLK_ENABLE_BMSK);

  if(!usb_shared_ss_phy_init_sec()) {
	  DBG(EFI_D_ERROR, "Init SSUSBPhySec Enable Error");
	  Status = EFI_DEVICE_ERROR; 
	  goto ON_EXIT;
  }

  //On success, set UFN max speed to SS
  MaxSpeedUefiVar = HWIO_USB3_2ND_DCFG_DEVSPD_SUPERSPEED_FVAL;

ON_EXIT:
  //Set the maximum speed supported by controller
  VarSize = sizeof(MaxSpeedUefiVar);
  gRT->SetVariable (L"UsbfnMaxSpeed", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &MaxSpeedUefiVar);

  FNC_LEAVE_MSG ();
  return Status;
}

/**
  This API is used for petting EUD after USB LDO 3p1 goes high, 
  so EUD hub does hot show up.

  @param None

**/
VOID
USbEudSwAttachDetSet(VOID)
{
  DBG(EFI_D_INFO, "EUD_SW_ATTACH_DET: 0 -> 1");
  
  if (HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_INM(HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_SW_ATTACH_DET_0_0_BMSK))
  {
    HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_OUTM (HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_SW_ATTACH_DET_0_0_BMSK, 0);
    gBS->Stall(1000);
  }
  HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_OUTM (HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_SW_ATTACH_DET_0_0_BMSK, 1);
}

/**
  Refer UsbConfigUsbCoresPrivate.h for the header description
 **/
STATIC
EFI_STATUS
SSUsb1InitCommon (
  IN  QCOM_USB_CORE_INTERFACE         *This,
  IN  BOOLEAN                          ResetUsbOnly
  )
{
  EFI_STATUS     Status   =  EFI_SUCCESS;
  BOOLEAN        bSsPhyFailed = FALSE;
  UINTN          BaseAddr;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG ();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  if (!ClockProtocol) goto ON_EXIT;

  UsbCore = USB_CORE_FROM_IFC(This);
  
  BaseAddr = UsbCore->BaseAddr;

  if (!ResetUsbOnly) {
    // Init Dal Lib
    DALSYS_InitMod(NULL);

    //Turn on power rails
    if (UsbCore->CoreType == USB_CONFIG_SSUSB1)
    {
      if (NULL != gPmicNpaClientSS1)
      {
        npa_issue_required_request(gPmicNpaClientSS1, PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE);   //Sends the request to RPM, no return
        DBG(EFI_D_INFO, "NPA Request to set SS1 PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE");
      }
      else {
      Status = EFI_INVALID_PARAMETER;
      WRN_CHK("USB NPA SS1 Client unavailable.Aborting");
      }
    }

    //Enable USB power domain
    ConfigPowerDomain(TRUE, SSUsb1PowerDomain);

    if (UsbCore->CoreType == USB_CONFIG_SSUSB1)
    {
      //Vote for Bus
      if (gNpaClientSS1Bus) {
        npa_issue_vector_request( gNpaClientSS1Bus,
                                 (sizeof(usb_icb_request)/sizeof(npa_resource_state)), 
                                 ((npa_resource_state *)usb_icb_request) );
      }
      else {
        DBG (EFI_D_ERROR, "gNpaClientSS1Bus is NULL)");
      }
    }
    if (0 == UsbCore->ClkRefCnt) {
      // Enable clocks first, without that disable clocks does not work
      Status = InitUsbClocks (UsbCore->EnClockCount, UsbCore->EnClocks);
      WRN_CHK ("failed to initialize USB clocks");
    }

    // Disable clocks for super speed controller
    Status = DisableUsbClocks (UsbCore->DisClockCount, UsbCore->DisClocks);
    WRN_CHK ("failed to disable USB clocks");

    // Enable clocks for super speed controller
    Status = InitUsbClocks (UsbCore->EnClockCount, UsbCore->EnClocks);
    ERR_CHK ("failed to initialize USB clocks");
    
    USbEudSwAttachDetSet();

    //Increment reference count
    UsbCore->ClkRefCnt++;
  }

  /*3. 
    To avoid ambiguity in the initial state, put the core in power collapse, then bring the core out of power collapse
    Applies to primary core only
  */
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_PRIM_GDSCR_OFFS, HWIO_GCC_USB30_PRIM_GDSCR_SW_COLLAPSE_BMSK);
    gBS->Stall (100);
    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_PRIM_GDSCR_OFFS, ~HWIO_GCC_USB30_PRIM_GDSCR_SW_COLLAPSE_BMSK);
    gBS->Stall (100);
  
  // Hard Reset the USB Link and PHYs using GCC control
  SynopsysGccReset (UsbCore);

  //GCC_USB_SS_PHY_LDO_EN is enabled by default

  //Write 0x11 to the AHB2PHY bridge CSR PERIPH_SS_AHB2PHY_TOP_CFG 
  //to enable one-wait-state writes and reads prior to writing or reading the QMP USB3 PHY CSRs.
  HAL_USB_WRITE (PERIPH_SS_AHB2PHY_NORTH_BASE, HWIO_PERIPH_SS_AHB2PHY_NORTH_AHB2PHY_TOP_CFG_OFFS, 0x11);

  Status = InitHSUSBPhy(UsbCore);
  WRN_CHK("InitHSUSBPhy failed: 0x%08x", Status);

  // Dont enable the SS USB Phy for the high speed only configuration
  if (!FeaturePcdGet(HighSpeedOnlySupported))
  {
    Status = InitSSUSBPhy(UsbCore);
    if (Status != EFI_SUCCESS)
    {
      bSsPhyFailed = TRUE;
      DBG( EFI_D_WARN, "InitSSUSBPhy failed: 0x%08x. Falling back to HS PHY only", Status);
      Status = EFI_SUCCESS;
    }
  }

  /*
     11. HS only: if operating without the SS PHY, follow this sequence to disable the pipe clock requirement.
    a. usb30_reg_general_cfg[PIPE_UTMI_CLK_DIS] = 0x1
    b. Wait 1 탎.
    c. usb30_reg_general_cfg[PIPE_UTMI_CLK_SEL] = 0x1
    d. usb30_reg_general_cfg [PIPE3_PHYSTATUS_SW] = 0x1
    e. Wait 1 탎.
    f. usb30_reg_general_cfg[PIPE_UTMI_CLK_DIS] = 0x0
    In this case, program the software to skip all SuperSpeed configurations in 
    the link controller and PHY, otherwise the core could get stuck.
  */
  if ((((UsbCore->CoreType == USB_CONFIG_SSUSB1) && bSsPhyFailed)) || (FeaturePcdGet(HighSpeedOnlySupported)))
  {
    DBG( EFI_D_INFO, "Disabling SS PHY");
    HAL_USB_OR (BaseAddr, HWIO_USB3_PRI_GENERAL_CFG_OFFS, HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK);
    gBS->Stall (MICRO_STALL_UNITS(10));
    HAL_USB_OR (BaseAddr, HWIO_USB3_PRI_GENERAL_CFG_OFFS, HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_SEL_BMSK);
    HAL_USB_OR (BaseAddr, HWIO_USB3_PRI_GENERAL_CFG_OFFS, HWIO_USB3_PRI_GENERAL_CFG_PIPE3_PHYSTATUS_SW_BMSK);
    //When PIPE3_PHYSTATUS is high, Setting the PIPE3_SET_PHYSTATUS_SW bit will assert the PIPE_PHYSTATUS controller input 
    //So clear PHY status here
    HAL_USB_AND (BaseAddr, HWIO_USB3_PRI_GENERAL_CFG_OFFS, ~HWIO_USB3_PRI_GENERAL_CFG_PIPE3_SET_PHYSTATUS_SW_BMSK);
    gBS->Stall (MICRO_STALL_UNITS(10));
    HAL_USB_AND (BaseAddr, HWIO_USB3_PRI_GENERAL_CFG_OFFS, ~HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK);
  }
  else
  {
    /* Set DELAYP1TRANS to 0 */
    HAL_USB_AND (BaseAddr, HWIO_USB3_PRI_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OFFS(0), ~HWIO_USB3_PRI_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_BMSK);
  }

  /*
    15. To save power, enable the hardware-based clock gating (not relevant for PBL):
    a. usb30_reg_cgctl[DBM_FSM_EN] = 0x1
  */
  HAL_USB_OR (BaseAddr, HWIO_USB3_PRI_CGCTL_REG_OFFS, HWIO_USB3_PRI_CGCTL_REG_DBM_FSM_EN_BMSK);
  
  
ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
} // end SynopsysInitCommon


/**
  Refer UsbConfigUsbCoresPrivate.h for the header description
 **/
STATIC
EFI_STATUS
SSUsb2InitCommon (
  IN  QCOM_USB_CORE_INTERFACE         *This,
  IN  BOOLEAN                          ResetUsbOnly
  )
{
  EFI_STATUS     Status   =  EFI_SUCCESS;
  BOOLEAN        bSsPhyFailed = FALSE;
  UINTN          BaseAddr;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG ();

  DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  if (!ClockProtocol) goto ON_EXIT;

  UsbCore = USB_CORE_FROM_IFC(This);

  ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);
  
  BaseAddr = UsbCore->BaseAddr;

  if (!ResetUsbOnly) {
    // Init Dal Lib
    DALSYS_InitMod(NULL);

    //Turn on power rails
    if (UsbCore->CoreType == USB_CONFIG_SSUSB2)
    {
      if (NULL != gPmicNpaClientSS2)
      {                                               
        npa_issue_required_request(gPmicNpaClientSS2, PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE);   //Sends the request to RPM, no return
        DBG(EFI_D_INFO, "NPA Request to set SS2 PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE");
      }
      else {
      Status = EFI_INVALID_PARAMETER;
      WRN_CHK("USB NPA SS2 Client unavailable.Aborting");
      }
    }
    
    //Enable USB power domain
    ConfigPowerDomain(TRUE, SSUsb2PowerDomain);

    if (UsbCore->CoreType == USB_CONFIG_SSUSB2)
    { 
      //Vote for Bus
      if (gNpaClientSS2Bus) {
        npa_issue_vector_request( gNpaClientSS2Bus,
                                 (sizeof(usb_icb_request)/sizeof(npa_resource_state)), 
                                 ((npa_resource_state *)usb_icb_request) );
      }
      else {
        DBG (EFI_D_ERROR, "gNpaClientSS2Bus is NULL)");
      }
    }

    if (0 == UsbCore->ClkRefCnt) {
      // Enable clocks first, without that disable clocks does not work
      Status = InitUsbClocks (UsbCore->EnClockCount, UsbCore->EnClocks);
      WRN_CHK ("failed to initialize USB clocks");
    }

    // Disable clocks for super speed controller
    Status = DisableUsbClocks (UsbCore->DisClockCount, UsbCore->DisClocks);
    WRN_CHK ("failed to disable USB clocks");

    // Enable clocks for super speed controller
    Status = InitUsbClocks (UsbCore->EnClockCount, UsbCore->EnClocks);
    ERR_CHK ("failed to initialize USB clocks");

    USbEudSwAttachDetSet();

    //Increment reference count
    UsbCore->ClkRefCnt++;
  }

  /*3. 
    To avoid ambiguity in the initial state, put the core in power collapse, then bring the core out of power collapse
    Applies to primary core only
  */
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_SEC_GDSCR_OFFS, HWIO_GCC_USB30_SEC_GDSCR_SW_COLLAPSE_BMSK);
    gBS->Stall (100);
    HAL_USB_AND (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_SEC_GDSCR_OFFS, ~HWIO_GCC_USB30_SEC_GDSCR_SW_COLLAPSE_BMSK);
    gBS->Stall (100);
  
  // Hard Reset the USB Link and PHYs using GCC control
  SynopsysGccReset (UsbCore);

  //GCC_USB_SS_PHY_LDO_EN is enabled by default

  //Write 0x11 to the AHB2PHY bridge CSR PERIPH_SS_AHB2PHY_TOP_CFG 
  //to enable one-wait-state writes and reads prior to writing or reading the QMP USB3 PHY CSRs.
  HAL_USB_WRITE (PERIPH_SS_AHB2PHY_NORTH_BASE, HWIO_PERIPH_SS_AHB2PHY_NORTH_AHB2PHY_TOP_CFG_OFFS, 0x11);

  Status = InitHSUSBPhySec(UsbCore);
  WRN_CHK("InitHSUSBPhySec failed: 0x%08x", Status);

  // Dont enable the SS USB Phy for the high speed only configuration
  if (!FeaturePcdGet(HighSpeedOnlySupported))
  {
    Status = InitSSUSBPhySec(UsbCore);
    if (Status != EFI_SUCCESS)
    {
      bSsPhyFailed = TRUE;
      DBG( EFI_D_WARN, "InitSSUSBPhySec failed: 0x%08x. Falling back to HS PHY only", Status);
      Status = EFI_SUCCESS;
    }
  }

  /*
   11. HS only: if operating without the SS PHY, follow this sequence to disable the pipe clock requirement.
    a. usb30_reg_general_cfg[PIPE_UTMI_CLK_DIS] = 0x1
    b. Wait 1 탎.
    c. usb30_reg_general_cfg[PIPE_UTMI_CLK_SEL] = 0x1
    d. usb30_reg_general_cfg [PIPE3_PHYSTATUS_SW] = 0x1
    e. Wait 1 탎.
    f. usb30_reg_general_cfg[PIPE_UTMI_CLK_DIS] = 0x0
    In this case, program the software to skip all SuperSpeed configurations in 
    the link controller and PHY, otherwise the core could get stuck.
  */
  if ((((UsbCore->CoreType == USB_CONFIG_SSUSB2) && bSsPhyFailed)) || (FeaturePcdGet(HighSpeedOnlySupported)))
  {
    DBG( EFI_D_INFO, "Disabling SS PHY");
    HAL_USB_OR (BaseAddr, HWIO_USB3_2ND_GENERAL_CFG_OFFS, HWIO_USB3_2ND_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK);
    gBS->Stall (MICRO_STALL_UNITS(10));
    HAL_USB_OR (BaseAddr, HWIO_USB3_2ND_GENERAL_CFG_OFFS, HWIO_USB3_2ND_GENERAL_CFG_PIPE_UTMI_CLK_SEL_BMSK);
    HAL_USB_OR (BaseAddr, HWIO_USB3_2ND_GENERAL_CFG_OFFS, HWIO_USB3_2ND_GENERAL_CFG_PIPE3_PHYSTATUS_SW_BMSK);
    //When PIPE3_PHYSTATUS is high, Setting the PIPE3_SET_PHYSTATUS_SW bit will assert the PIPE_PHYSTATUS controller input 
    //So clear PHY status here
    HAL_USB_AND (BaseAddr, HWIO_USB3_2ND_GENERAL_CFG_OFFS, ~HWIO_USB3_2ND_GENERAL_CFG_PIPE3_SET_PHYSTATUS_SW_BMSK);
    gBS->Stall (MICRO_STALL_UNITS(10));
    HAL_USB_AND (BaseAddr, HWIO_USB3_2ND_GENERAL_CFG_OFFS, ~HWIO_USB3_2ND_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK);
  }
  else
  {
    /* Set DELAYP1TRANS to 0 */
    HAL_USB_AND (BaseAddr, HWIO_USB3_2ND_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OFFS(0), ~HWIO_USB3_2ND_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_BMSK);
  }

  /*
    15. To save power, enable the hardware-based clock gating (not relevant for PBL):
    a. usb30_reg_cgctl[DBM_FSM_EN] = 0x1
  */
  HAL_USB_OR (BaseAddr, HWIO_USB3_2ND_CGCTL_REG_OFFS, HWIO_USB3_2ND_CGCTL_REG_DBM_FSM_EN_BMSK);
  
  
ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
} // end SynopsysInitCommon



/**
  Performs Host mode specific initialization on a Synopsys super speed USB Primary core.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core successfully initialized
  @retval EFI_UNSUPPORTED   USB core not supported
  @retval Others            Error encountered initializing USB core
**/
STATIC
EFI_STATUS
SynopsysInitHostConfig (
  IN  QCOM_USB_CORE         *UsbCore
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       BaseAddr;

  FNC_ENTER_MSG ();

  if (NULL == UsbCore) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  
  BaseAddr = UsbCore->BaseAddr;
  
  //2. Set usb30_reg_gctl:[PRTCAPDIR] = 0x1 (host), [U2EXIT_LFPS] = 0x1
  HAL_USB_AND(BaseAddr,HWIO_USB3_PRI_GCTL_OFFS,~HWIO_USB3_PRI_GCTL_PRTCAPDIR_BMSK);
  HAL_USB_OR (BaseAddr, HWIO_USB3_PRI_GCTL_OFFS, HWIO_USB3_PRI_GCTL_U2EXIT_LFPS_BMSK | ( (1 << HWIO_USB3_PRI_GCTL_PRTCAPDIR_SHFT)) );

  //3. Enable the wake on connect event for the port:
  HAL_USB_OR (BaseAddr, HWIO_USB3_PRI_PORTSC_20_REGS_p_PORTSC_20_OFFS(0), HWIO_USB3_PRI_PORTSC_20_REGS_p_PORTSC_20_WCE_BMSK );
  HAL_USB_OR (BaseAddr, HWIO_USB3_PRI_PORTSC_30_REGS_p_PORTSC_30_OFFS(0), HWIO_USB3_PRI_PORTSC_30_REGS_p_PORTSC_30_WCE_BMSK );

  //4. Set GRXTHRCFG based on the case 8000615753 values
  HAL_USB_OR (BaseAddr, HWIO_USB3_PRI_GRXTHRCFG_OFFS,  (3 << HWIO_USB3_PRI_GRXTHRCFG_USBMAXRXBURSTSIZE_SHFT) |
  (3 << HWIO_USB3_PRI_GRXTHRCFG_USBRXPKTCNT_SHFT) | HWIO_USB3_PRI_GRXTHRCFG_USBRXPKTCNTSEL_BMSK);

  //5. Set the bus configuration 1K page pipe limit:
  //TODO:CK:HWIO_GSBUSCFG1_DESADRSPC_BMSK & HWIO_GSBUSCFG1_DATADRSPC_BMSK is not defined.
  //HAL_USB_AND(BaseAddr, HWIO_GSBUSCFG1_ADDR(0), ~HWIO_GSBUSCFG1_DESADRSPC_BMSK | ~HWIO_GSBUSCFG1_DATADRSPC_BMSK );
  HAL_USB_OR(BaseAddr, HWIO_USB3_PRI_GSBUSCFG1_OFFS, (0xE << HWIO_USB3_PRI_GSBUSCFG1_PIPETRANSLIMIT_SHFT) | HWIO_USB3_PRI_GSBUSCFG1_EN1KPAGE_BMSK );

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
} // end SynopsysInitHost


/**
  Performs Host mode specific initialization on a Synopsys super speed USB Secondary core.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core successfully initialized
  @retval EFI_UNSUPPORTED   USB core not supported
  @retval Others            Error encountered initializing USB core
**/
STATIC
EFI_STATUS
SynopsysInitHostConfigSec (
  IN  QCOM_USB_CORE         *UsbCore
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       BaseAddr;

  FNC_ENTER_MSG ();

  DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");
  
  if (NULL == UsbCore) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);
  
  BaseAddr = UsbCore->BaseAddr;
  
  //2. Set usb30_reg_gctl:[PRTCAPDIR] = 0x1 (host), [U2EXIT_LFPS] = 0x1
  HAL_USB_AND(BaseAddr, HWIO_USB3_2ND_GCTL_OFFS, ~HWIO_USB3_2ND_GCTL_PRTCAPDIR_BMSK);
  HAL_USB_OR (BaseAddr, HWIO_USB3_2ND_GCTL_OFFS, HWIO_USB3_2ND_GCTL_U2EXIT_LFPS_BMSK | ( (1 << HWIO_USB3_2ND_GCTL_PRTCAPDIR_SHFT)) );

  //3. Enable the wake on connect event for the port:
  HAL_USB_OR (BaseAddr, HWIO_USB3_2ND_PORTSC_20_REGS_p_PORTSC_20_OFFS(0), HWIO_USB3_2ND_PORTSC_20_REGS_p_PORTSC_20_WCE_BMSK );
  HAL_USB_OR (BaseAddr, HWIO_USB3_2ND_PORTSC_30_REGS_p_PORTSC_30_OFFS(0), HWIO_USB3_2ND_PORTSC_30_REGS_p_PORTSC_30_WCE_BMSK );

  //4. Set GRXTHRCFG based on the case 8000615753 values
  HAL_USB_OR (BaseAddr, HWIO_USB3_2ND_GRXTHRCFG_OFFS,  (3 << HWIO_USB3_2ND_GRXTHRCFG_USBMAXRXBURSTSIZE_SHFT) |
  (3 << HWIO_USB3_2ND_GRXTHRCFG_USBRXPKTCNT_SHFT) | HWIO_USB3_2ND_GRXTHRCFG_USBRXPKTCNTSEL_BMSK);

  //5. Set the bus configuration 1K page pipe limit:
  //TODO:CK:HWIO_GSBUSCFG1_DESADRSPC_BMSK & HWIO_GSBUSCFG1_DATADRSPC_BMSK is not defined.
  //HAL_USB_AND(BaseAddr, HWIO_GSBUSCFG1_ADDR(0), ~HWIO_GSBUSCFG1_DESADRSPC_BMSK | ~HWIO_GSBUSCFG1_DATADRSPC_BMSK );
  HAL_USB_OR(BaseAddr, HWIO_USB3_2ND_GSBUSCFG1_OFFS, (0xE << HWIO_USB3_2ND_GSBUSCFG1_PIPETRANSLIMIT_SHFT) | HWIO_USB3_2ND_GSBUSCFG1_EN1KPAGE_BMSK );

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
} // end SynopsysInitHostSec



/**
  Refer UsbConfigUsbCoresPrivate.h for the header description
**/
STATIC
EFI_STATUS
Usb30InitHost (
  IN  QCOM_USB_CORE_INTERFACE         *This
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameters");
  }

  UsbCore = USB_CORE_FROM_IFC(This);
  
  /*if (UsbCore->CoreType == USB_CONFIG_SSUSB1) {
    Status = EnableVbusSS1();
    ERR_CHK("Failed to initialize VbusSS1");
  }*/

  if(UsbCore->CoreType == USB_CONFIG_SSUSB1)
  Status = SynopsysInitHostConfig(UsbCore);
  else if(UsbCore->CoreType == USB_CONFIG_SSUSB2)
  	Status = SynopsysInitHostConfigSec(UsbCore);
  else
  	DBG(EFI_D_WARN, "Invalid Core Type");

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
  Refer UsbConfigUsbCoresPrivate.h for the header description
**/
STATIC
EFI_STATUS
Usb30GetVbusSt (
  IN  QCOM_USB_CORE_INTERFACE         *This,
  OUT UINT32                          *VbusSt
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;

  FNC_ENTER_MSG();

  if (!This || !VbusSt) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameters");
  }

  // For Clamshell Platform Host Mode Enablement is automatically taken care by EC
  if (IS_CLS_PLAT(PlatformInfo.platform)) {
      Status = EFI_SUCCESS;
      goto ON_EXIT;
  }
  else {
      if (NULL == gPmicUsbProtocol) {
        // Find the PMIC USB charger protocol
        Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&gPmicUsbProtocol);
        if (EFI_ERROR(Status)) {
          gPmicUsbProtocol = NULL;
          WRN_CHK("Failed to open PMIC USB protocol Status =  (0x%x)", Status);
          goto ON_EXIT;
        }
      }

  Status = gPmicUsbProtocol->GetOtgStatus((EFI_PM_USB_OTG_STATUS_TYPE *)VbusSt);
  WRN_CHK ("failed to get Vbus Status, Status = 0x%x", Status);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
  Refer UsbConfigUsbCoresPrivate.h for the header description
**/
STATIC
EFI_STATUS
Usb30EnableVbus (
  IN  QCOM_USB_CORE_INTERFACE         *This
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameters");
  }

  UsbCore = USB_CORE_FROM_IFC(This);
  
  // Enable Vbus for the Primary Core (host mode)
  if(UsbCore->CoreType == USB_CONFIG_SSUSB1) {
  Status = EnableVbusSS1();
  ERR_CHK("Failed to initialize VbusSS1");
  }
  else if(UsbCore->CoreType == USB_CONFIG_SSUSB2) { 
    //Host Mode not Supported, used only for Debuggers
	Status = EFI_UNSUPPORTED;
  }
  else
  	DBG(EFI_D_WARN, "Invalid Core Type");


ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
  Performs device mode specific initialization on a Synopsys super speed USB Primary core.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core successfully initialized
  @retval EFI_UNSUPPORTED   USB core not supported
  @retval Others            Error encountered initializing USB core
**/
STATIC
EFI_STATUS
SynopsysInitDevice (
  IN  QCOM_USB_CORE         *UsbCore
  )
{

  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       BaseAddr;

  FNC_ENTER_MSG ();

  if (NULL == UsbCore) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  BaseAddr = UsbCore->BaseAddr;
  USB_ASSERT_GOTO ((BaseAddr != 0), ON_EXIT);

  if (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI) {
    // Reset Device Core.
    HAL_USB_OR(BaseAddr, HWIO_USB3_PRI_DCTL_OFFS, HWIO_USB3_PRI_DCTL_CSFTRST_BMSK);
    while (HAL_USB_READ(BaseAddr, HWIO_USB3_PRI_DCTL_OFFS) & HWIO_USB3_PRI_DCTL_CSFTRST_BMSK) { ; }
    InitHSUSBPhy(UsbCore);
    goto ON_EXIT;
  }

  /*
    16. Enable the hardware LPM:
    usb30_reg_gusb2phycfg_regs_p_gusb2phycfg[ENBLSLPM] = 0x1
  */
  HAL_USB_OR( BaseAddr, HWIO_USB3_PRI_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OFFS (0), HWIO_USB3_PRI_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_ENBLSLPM_BMSK);
  
ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
  Performs device mode specific initialization on a Synopsys super speed USB Secondary core.

  @param [in]  UsbCore      The USB core instance

  @retval EFI_SUCCESS       USB core successfully initialized
  @retval EFI_UNSUPPORTED   USB core not supported
  @retval Others            Error encountered initializing USB core
**/
STATIC
EFI_STATUS
SynopsysInitDeviceSec (
  IN  QCOM_USB_CORE         *UsbCore
  )
{

  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       BaseAddr;

  FNC_ENTER_MSG ();

  DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");
  
  if (NULL == UsbCore) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);

  BaseAddr = UsbCore->BaseAddr;
  USB_ASSERT_GOTO ((BaseAddr != 0), ON_EXIT);

  if (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI) {
    // Reset Device Core.
    HAL_USB_OR(BaseAddr, HWIO_USB3_2ND_DCTL_OFFS, HWIO_USB3_2ND_DCTL_CSFTRST_BMSK);
    while (HAL_USB_READ(BaseAddr, HWIO_USB3_2ND_DCTL_OFFS) & HWIO_USB3_2ND_DCTL_CSFTRST_BMSK) { ; }
    InitHSUSBPhySec(UsbCore);
    goto ON_EXIT;
  }

  /*
    16. Enable the hardware LPM:
    usb30_reg_gusb2phycfg_regs_p_gusb2phycfg[ENBLSLPM] = 0x1
  */
  HAL_USB_OR( BaseAddr, HWIO_USB3_2ND_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OFFS (0), HWIO_USB3_2ND_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_ENBLSLPM_BMSK);
  
ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
  Refer UsbConfigUsbCoresPrivate.h for the header description
**/
STATIC
EFI_STATUS
Usb30InitDevice (
  IN  QCOM_USB_CORE_INTERFACE         *This
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  if(UsbCore->CoreType == USB_CONFIG_SSUSB1)
  Status = SynopsysInitDevice(UsbCore);
  else if(UsbCore->CoreType == USB_CONFIG_SSUSB2)
  	Status = SynopsysInitDeviceSec(UsbCore);
  else
  	DBG(EFI_D_WARN, "Invalid Core Type");

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description*/
STATIC
EFI_STATUS
SSUsb1EnterLpm (
  IN  QCOM_USB_CORE_INTERFACE         *This
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  // not needed for Sdm845 PRE-SI platform
  if ((PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI)   ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_VIRTIO) ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    DBG(EFI_D_WARN, "Pre-SI: Bypass SynopsysPrimEnterLpm");
    goto ON_EXIT;
  }

  if (!UsbCore->IsCoreInLPM)
  {
    // Reference Count 0 implies that we have not enabled clocks
    if (UsbCore->ClkRefCnt == 0)
    {
      // Enable clocks first, without that disable clocks doesn't work
      Status = InitUsbClocks (UsbCore->EnClockCount, UsbCore->EnClocks);
      WRN_CHK ("failed to enable USB clocks");
      // Increment reference count
      UsbCore->ClkRefCnt++;
    }
    
    //Assert controller & PHYs resets
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_PRIM_BCR_OFFS, HWIO_GCC_USB30_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_PRIM_BCR_OFFS, HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_PRIM_BCR_OFFS, HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_PRIM_BCR_OFFS, HWIO_GCC_USB30_PRIM_BCR_BLK_ARES_BMSK);

    Status = DisableUsbClocks (UsbCore->DisClockCount, UsbCore->DisClocks);
    WRN_CHK ("failed to disable USB clocks");

    // Disable USB Foot Switch (Power domain)
    ConfigPowerDomain(FALSE, SSUsb1PowerDomain);

    //  Enable SW PowerCollapse for USB30 Controller
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_PRIM_GDSCR_OFFS, HWIO_GCC_USB30_PRIM_GDSCR_SW_COLLAPSE_BMSK);

    if (NULL != gPmicNpaClientSS1) 
    {
      npa_issue_required_request(gPmicNpaClientSS1, PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND);   //Sends the request to RPM, no return
      DBG(EFI_D_INFO, "SSUsb1EnterLpm : NPA Request to set PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND");
    }

    //Remove bus votes
    if (gNpaClientSS1Bus) {
        npa_issue_vector_request( gNpaClientSS1Bus,
                                 (sizeof(usb_icb_clrrequest)/sizeof(npa_resource_state)), 
                                 ((npa_resource_state *)usb_icb_clrrequest) );
    }

    // Decrement reference count
    UsbCore->ClkRefCnt--;
    UsbCore->IsCoreInLPM = TRUE;

    // Unknown Reference Count, Faulty scenario
    if (UsbCore->ClkRefCnt > 1)
    {
      WRN_CHK ("SSUsb1EnterLpm: Wrong clock Reference count= 0x%x", UsbCore->ClkRefCnt);
    }
  }
  else
  {
    WRN_CHK ("SSUsb1EnterLpm: Synopsys Core already in LPM");
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description*/
STATIC
EFI_STATUS
SSUsb2EnterLpm (
  IN  QCOM_USB_CORE_INTERFACE         *This
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);

  // not needed for Sdm845 PRE-SI platform
  if ((PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI)   ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_VIRTIO) ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    DBG(EFI_D_WARN, "Pre-SI: Bypass SynopsysPrimEnterLpm");
    goto ON_EXIT;
  }

  if (!UsbCore->IsCoreInLPM)
  {
    // Reference Count 0 implies that we have not enabled clocks
    if (UsbCore->ClkRefCnt == 0)
    {
      // Enable clocks first, without that disable clocks doesn't work
      Status = InitUsbClocks (UsbCore->EnClockCount, UsbCore->EnClocks);
      WRN_CHK ("failed to enable USB clocks");
      // Increment reference count
      UsbCore->ClkRefCnt++;
    }
    
    //Assert controller & PHYs resets
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_SEC_BCR_OFFS, HWIO_GCC_USB30_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_SEC_BCR_OFFS, HWIO_GCC_QUSB2PHY_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3_PHY_SEC_BCR_OFFS, HWIO_GCC_USB3_PHY_SEC_BCR_BLK_ARES_BMSK);
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB3PHY_PHY_SEC_BCR_OFFS, HWIO_GCC_USB30_SEC_BCR_BLK_ARES_BMSK);

    Status = DisableUsbClocks (UsbCore->DisClockCount, UsbCore->DisClocks);
    WRN_CHK ("failed to disable USB clocks");

    // Disable USB Foot Switch (Power domain)
	ConfigPowerDomain(FALSE, SSUsb2PowerDomain);

    //  Enable SW PowerCollapse for USB30 Controller
    HAL_USB_OR (GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_USB30_SEC_GDSCR_OFFS, HWIO_GCC_USB30_SEC_GDSCR_SW_COLLAPSE_BMSK);

    if (NULL != gPmicNpaClientSS2)
    {
      npa_issue_required_request(gPmicNpaClientSS2, PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND);   //Sends the request to RPM, no return
      DBG(EFI_D_INFO, "SSUsb2EnterLpm : NPA Request to set PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND");
    }

    //Remove bus votes
    if (gNpaClientSS2Bus) {
        npa_issue_vector_request( gNpaClientSS2Bus,
                                 (sizeof(usb_icb_clrrequest)/sizeof(npa_resource_state)), 
                                 ((npa_resource_state *)usb_icb_clrrequest) );
    }

    // Decrement reference count
    UsbCore->ClkRefCnt--;
    UsbCore->IsCoreInLPM = TRUE;

    // Unknown Reference Count, Faulty scenario
    if (UsbCore->ClkRefCnt > 1)
    {
      WRN_CHK ("SSUsb2EnterLpm: Wrong clock Reference count= 0x%x", UsbCore->ClkRefCnt);
    }
  }
  else
  {
    WRN_CHK ("SSUsb2EnterLpm: Synopsys Core already in LPM");
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description*/
STATIC
EFI_STATUS
Usb30ExitLpm (
  IN  QCOM_USB_CORE_INTERFACE         *This
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  // not needed for Sdm845 PRE-SI platform
  if ((PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI)   ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_VIRTIO) ||
      (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    DBG(EFI_D_WARN, "Pre-SI: Bypass SynopsysExitLpm");
    goto ON_EXIT;
  }

  // Perform GCC reset for controller and PHYs
  SynopsysGccReset (UsbCore);

ON_EXIT:
  FNC_LEAVE_MSG();
  return EFI_SUCCESS;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description*/
EFI_STATUS
Usb30Reset (
  IN  QCOM_USB_CORE_INTERFACE         *This
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description */
STATIC
EFI_STATUS
Usb30SetPhyLane(
  IN QCOM_USB_CORE_INTERFACE           *This,
  IN UINT8                              LaneNum
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  UsbCore->LaneNum = LaneNum;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description */
STATIC
EFI_STATUS
Usb30GetBaseAddr(
  IN   QCOM_USB_CORE_INTERFACE           *This,
  OUT  UINTN                             *BaseAddr
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!This || !BaseAddr) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  *BaseAddr = UsbCore->BaseAddr;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description */
STATIC
EFI_STATUS
Usb30ResetSSPhy(
  IN   QCOM_USB_CORE_INTERFACE           *This
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  if(UsbCore->CoreType == USB_CONFIG_SSUSB1)
  InitSSUSBPhy(UsbCore);
  else if(UsbCore->CoreType == USB_CONFIG_SSUSB2)
  	InitSSUSBPhySec(UsbCore);
  else
  	DBG(EFI_D_WARN, "Invalid Core Type");

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description */ 
STATIC
EFI_STATUS
SSUsb1SetPhyNonDrivingMode(
  IN   QCOM_USB_CORE_INTERFACE           *This,
  IN   BOOLEAN                            NonDrivingMode
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;
  UINTN BaseAddr;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);
  
  BaseAddr = UsbCore->BaseAddr;

  // power off the PHY for high-z
  if (NonDrivingMode)
  {
    HAL_USB_WRITE(PERIPH_SS_AHB2PHY_NORTH_BASE, HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OFFS, 0x23);
  }
  else
  {
    // in order to put the phy back to the NORMAL mode, we have to re-init the HS phy.
    InitHSUSBPhy(UsbCore);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description */ 
STATIC
EFI_STATUS
SSUsb2SetPhyNonDrivingMode(
  IN   QCOM_USB_CORE_INTERFACE           *This,
  IN   BOOLEAN                            NonDrivingMode
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;
  UINTN BaseAddr;

  FNC_ENTER_MSG();

  DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);
  
  BaseAddr = UsbCore->BaseAddr;

  // power off the PHY for high-z
  if (NonDrivingMode)
  {
    HAL_USB_WRITE(PERIPH_SS_AHB2PHY_NORTH_BASE, HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PWR_CTRL1_OFFS, 0x23);
  }
  else
  {
    // in order to put the phy back to the NORMAL mode, we have to re-init the HS phy.
    InitHSUSBPhySec(UsbCore);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description */
STATIC
EFI_STATUS
SSUsb1InitHSUSBPhyKeepPwrDwn (
  IN QCOM_USB_CORE_INTERFACE           *This
  )
{
  FNC_ENTER_MSG ();

  // Sdm845 RUMI PHY init sequence.
  if (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI) {
      DBG(EFI_D_INFO, "Return without any configuration for RUMI");
      goto ON_EXIT;
  }

  if ((PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_RUMI)   &&
      (PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_VIRTIO) &&
      (PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    //Hard reset PHY
    HAL_USB_OR(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_PRIM_BCR_OFFS, HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK);
    gBS->Stall(100);
    HAL_USB_AND(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_PRIM_BCR_OFFS, ~HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK);
    gBS->Stall(100);

    DBG(EFI_D_INFO, "HS Phy Init");

    // Keep Port Power Down
    HAL_USB_WRITE(PERIPH_SS_AHB2PHY_NORTH_BASE, HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OFFS, 0x23);
    gBS->Stall(100);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_ANALOG_CONTROLS_TWO_OFFS,
      0x03);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_CLOCK_INVERTERS_OFFS,
      0x7C);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_CMODE_OFFS,
      0x80);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_PLL_LOCK_DELAY_OFFS,
      0x0A);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE2_OFFS,
      0x09);
  }

  ON_EXIT:
  FNC_LEAVE_MSG ();
  return EFI_SUCCESS;
}


/* Refer UsbConfigUsbCoresPrivate.h for the header description */
STATIC
EFI_STATUS
SSUsb2InitHSUSBPhyKeepPwrDwn (
  IN QCOM_USB_CORE_INTERFACE           *This
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG ();

  DBG(EFI_D_ERROR, "Specific to Secondary core, should not come here for SDM670");

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCore = USB_CORE_FROM_IFC(This);

  ASSERT(UsbCore->BaseAddr == USB3_PRI_USB30_PRIM_BASE);

  // Sdm845 RUMI PHY init sequence.
  if (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI) {
      DBG(EFI_D_INFO, "Return without any configuration for RUMI");
      goto ON_EXIT;
  }

  if ((PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_RUMI)   &&
      (PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_VIRTIO) &&
      (PlatformInfo.platform != EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    //Hard reset PHY
    HAL_USB_OR(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_SEC_BCR_OFFS, HWIO_GCC_QUSB2PHY_SEC_BCR_BLK_ARES_BMSK);
    gBS->Stall(100);
    HAL_USB_AND(GCC_CLK_CTL_REG_REG_BASE, HWIO_GCC_QUSB2PHY_SEC_BCR_OFFS, ~HWIO_GCC_QUSB2PHY_SEC_BCR_BLK_ARES_BMSK);
    gBS->Stall(100);

    DBG(EFI_D_INFO, "HS Phy Init");

    // Keep Port Power Down
    HAL_USB_WRITE(PERIPH_SS_AHB2PHY_NORTH_BASE, HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PWR_CTRL1_OFFS, 0x23);
    gBS->Stall(100);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_ANALOG_CONTROLS_TWO_OFFS,
      0x03);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_CLOCK_INVERTERS_OFFS,
      0x7C);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_CMODE_OFFS,
      0x80);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_PLL_LOCK_DELAY_OFFS,
      0x0A);

    HAL_USB_WRITE(
      PERIPH_SS_AHB2PHY_NORTH_BASE,
      HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE2_OFFS,
      0x09);
  }

  ON_EXIT:
  FNC_LEAVE_MSG ();
  return EFI_SUCCESS;
}


/* Refer to UsbConfigUsbCoresPublic.h for header description*/
EFI_STATUS
UsbConfigCoresInit(
  IN  UINTN                      CoreNum,
  OUT QCOM_USB_CORE_INTERFACE  **UsbCoreInterface
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;
  STATIC UINTN FlagToCallAPIOnlyOnce = 0; 

  FNC_ENTER_MSG();

  if (!UsbCoreInterface) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters: CoreNum %d, UsbCore %p", CoreNum, UsbCoreInterface);
  }

  UsbCore = AllocateZeroPool(sizeof(QCOM_USB_CORE));

  if (!UsbCore) {
    Status = EFI_OUT_OF_RESOURCES;
    ERR_CHK("Out of Resources");
  }

  if (CoreNum == USB_CORE_0_SDM_670) {
    UsbCore->CoreInterface.InitCommon = SSUsb1InitCommon;
    UsbCore->CoreInterface.InitDevice = Usb30InitDevice;
    UsbCore->CoreInterface.InitHost = Usb30InitHost;
    UsbCore->CoreInterface.GetVbusStatus = Usb30GetVbusSt;
    UsbCore->CoreInterface.EnableVbus = Usb30EnableVbus;
    UsbCore->CoreInterface.Reset = Usb30Reset;
    UsbCore->CoreInterface.CoreEnterLPM = SSUsb1EnterLpm;
    UsbCore->CoreInterface.CoreExitLPM = Usb30ExitLpm;
    UsbCore->CoreInterface.SetPhyLane = Usb30SetPhyLane;
    UsbCore->CoreInterface.GetBaseAddress = Usb30GetBaseAddr;
    UsbCore->CoreInterface.ResetSSPhy = Usb30ResetSSPhy;
    UsbCore->CoreInterface.SetNonDrivingMode = SSUsb1SetPhyNonDrivingMode;
    UsbCore->CoreInterface.InitHsPhyPwrDwn = SSUsb1InitHSUSBPhyKeepPwrDwn;

    *UsbCoreInterface = &UsbCore->CoreInterface;

    UsbCore->CoreType = USB_CONFIG_SSUSB1;
    UsbCore->BaseAddr = USB3_PRI_USB30_PRIM_BASE;
    UsbCore->EnClockCount = sizeof(SSUsb1EnCoreClocks) / sizeof(QCOM_USB_CLOCK);
    UsbCore->EnClocks = SSUsb1EnCoreClocks;
    UsbCore->DisClockCount = sizeof(SSUsb1DisCoreClocks) / sizeof(QCOM_USB_CLOCK);
    UsbCore->DisClocks = SSUsb1DisCoreClocks;
    UsbCore->ClkRefCnt = 0;
    UsbCore->IsCoreInLPM = FALSE;
    UsbCore->LaneNum = 0;
  }
  else {
    DBG(EFI_D_WARN, "Invalid Core Type");
  }

  //Open EFI Protocols only once
  if(FlagToCallAPIOnlyOnce == 0 && CoreNum < USB_CORE_MAX_NUM_SDM_670) {
    Status = UsbConfigLibOpenProtocols();
    ERR_CHK("UsbConfigLibOpenProtocols failed %r",Status);
	FlagToCallAPIOnlyOnce = 1;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Refer to UsbConfigUsbCoresPublic.h for header description*/
EFI_STATUS
UsbConfigCoresDeinit(
  IN QCOM_USB_CORE_INTERFACE  *UsbCoreInterface
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE *UsbCore;

  FNC_ENTER_MSG();

  if (!UsbCoreInterface) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters: CoreNum %d, UsbCore %p", UsbCoreInterface);
  }

  UsbCore = USB_CORE_FROM_IFC(UsbCoreInterface);

  if (UsbCore)
    FreePool(UsbCore);

ON_EXIT:
  return Status;
}
