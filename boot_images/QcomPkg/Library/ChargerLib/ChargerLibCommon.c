/** @file ChargerLib.c

  Provide access to Charger Libraries

  Copyright (c) 2012-2018, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     ---------------------------------------------------------
02/04/17      pbitra  Enable BMD explicitly
02/04/17      pbitra  Rerun APSD and reconfigure USB ICL in every periodical check
02/04/17      pbitra  Set ICL to 500mA when APSD result is invalid for CC float charger
01/29/18      cs      unknown battery shutdown should also be shutdown with usb and dcin pon edge triggered.
11/27/17      pbitra  Added USB and DCIN PON as edge triggered
08/03/17      pbitra  Boot to HLOS if unknown battery more than threshold, else shutdown
07/24/17      pbitra  Added support for ChargerLib_GetChargerPresence
                      Replaced the call to ChargerLib_GetChargingPath with ChargerLib_GetChargerPresence
05/31/17      sv      Updated to make chargerlib as target independent file.
07/21/17      va      Add Ship mode lib api
07/12/17      cs      display new icon for debug board cases
07/07/17      cs      GetBatteryType cannot be called before FGInit causing batt missing mistrigger
06/21/17      dc      Move Platform Check to QcomChargerDxe and support config override
06/13/17      ll      Disable HW AFP when debug board is detected
06/05/17      cs      pmic chargerapp need to do USBInit before usb enumeration happen.
03/30/17      ll      Implemented level file-logging
03/21/17      dch     Initialize ICL for all charger as 500mA and increase it to 1500mA in case charger is not SDP/Float charger
03/07/17      cs      remove duplicate get errors checking emergency shutdown condition.
03/09/17      ll      Updated code based on latest chargerlib_cfgdata_type
02/15/17      va      Disable HVDCP (QC 2 and 3.0) when weak battery charging and re run APSD for 2.0 and 3.0 
02/06/17      cs      change emergency shutdown Count to 9 to avoid early emmergency shutdown.
                      check destination size bigger or equal to source size for CopyMemS
02/06/17      cs      for emergency shutdown and unknown battery shutdown change from disable PON1 to edge trigger.
01/30/17      sm      Allow device to boot when vbatt is high enough, and batt temp is in operational range
01/27/17      ai      Fix use of retail
01/23/17      sm      CFG parameter Structure changes
12/19/16      sm      Updated for SCHG and FG Protocol APIs' usage
11/14/16      sm      Added changes in ChargerLib_ForceSysShutdown() for preventing PMi8998 to auto power 
                      on upon shutdown with Type-A to C cable attached
11/09/16      cs      added API to check if DCIn is valid
11/08/16      va      addded debug board boot up case and make sure to disable wdog during exit 
10/19/16      sm      Removed the 2 seconds debounce from battery presence check
10/14/16      va      adding threshold soc check when charger is disconnected
10/13/16      cs      add protocol API for setting dcin current limit.
10/12/16      sv      Fixed Array 'FormatString' KW issue. CR:1063098
10/06/16      cs      suspend DCIN for debug board 
09/29/16      va      update to print function names %a and handle %r format specifier for file logging
09/29/16      sm      Changed error action for battery missing and tsense thermal timrout errors
09/20/16      sm      Added changes to handle unknown battery error type based on configuration
09/15/16      sm      Added another action type for debug board case
09/13/16      sm      Removed GetIUsbMaxFromCfg() and  GetMaxUsbCurrent()
08/05/16      va      Adding Charger led indication support
07/28/16      sm      Changed logic in ChargerLib_WasChargerReinserted() and ChargerLib_Exit()
07/26/16      va      Restarting FG Changes on warm boot
07/18/16      sm      Added changes for setting higher current for USB 3.0
07/15/16      cs      Adding Wipower support
07/07/16      va      DEBUG print to print time stamp in seconds
06/29/16      va      Enable charger configs first to enables logs during Initilization of charger app
06/24/16      sm      Added ChargerLib_GetChargingStatus() API
06/23/16      va      Adding support for Charger Fg Peripheral dumps
06/14/16      va      Adding Thermal and File log support to Charger App
05/26/16      va      Update for Profile load and struct mis match 
05/24/16      sm      Corrected ADC channel in GetBatteryId() API
05/10/16      sm      Added back ChargerLib_Exit()
04/26/16      va      Volume Error handling for file Log
04/21/16      sm      Added ChargerLib_EnableWdog
04/12/16      sm      Added ChargerLib_GetBatteryID() API and changes to handle 
                      debug board
03/30/16      va      update global file logging flag 
03/28/16      va      update for file logging, exit api for ChargerLib
03/22/16      va      Update to HW jeita 
02/19/16      sm      Commented out reference to any protocol APIs till design 
                      is finalised
01/27/16      sm      Initial revision
=============================================================================*/

/*===========================================================================*/
/*                  INCLUDE FILES                                            */
/*===========================================================================*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>
#include "Library/SerialPortShLib.h"
#include <Library/QcomTargetLib.h>
#include <api/pmic/pm/pm_uefi.h>
#include <api/pmic/charger/ChargerLibCommon.h>
#include <Library/QcomBaseLib.h>
#include <PrintShLib.h>

#include <Library/BaseMemoryLib.h>
#include "string.h"


/**
  Protocol Dependencies
*/
#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFIPmicFg.h>
#include <Protocol/EFIPmicSchg.h>
#include <Protocol/EFIPmicUsb.h>
#include <Protocol/EFIPmicVersion.h>
#include <Protocol/EFIPmicPwrOn.h>
#include <Protocol/EFIVariableServices.h>
#include <Protocol/EFITsens.h>
#include <Protocol/EFIPmicRTC.h>
#include <Protocol/EFIUsbfnChgSrv.h>
#include <Protocol/EFIPmicRgbLed.h>
#include <Protocol/EFIUsbInitProtocol.h>

/**
  ADC Dependencies
 */
#include <Protocol/EFIAdc.h>

/*===========================================================================*/
/*                  TYPEDEF AND MACRO DEFINITIONS                            */
/*===========================================================================*/
#define BATTERY_EMULATOR_UPPER_THRSHOLD_MV  4450
#define BATTERY_EMULATOR_LOWER_THRSHOLD_MV  3450
#define BATTERY_EMULATOR_BATT_ID            1500
#define BATTERY_EMULATOR_TEMP_C             23
#define BATTERY_EMULATOR_SOC                99

#define VBATT_TOLERANCE                     5 //Battery upper voltage tolerance limit.

#define UNKNOWN_BATT_SHUTDOWN               0
#define UNKNOWN_BATT_BOOT_TO_HLOS           1
#define UNKNOWN_BATT_CONSERVATIVE_CHARGING  2
#define UNKNOWN_BATT_REGULAR_CHARGING       3

#define DEBUG_BOARD_LOWVBAT_SHUTDOWN        0
#define DEBUG_BOARD_LOWVBAT_NOCHARGE        1
#define DEBUG_BOARD_BOOT_TO_HLOS            2

#define CHARGER_COOL_OFF_PERIOD_DEFAULT     300000 //5minutes=1000millisec*60*5=300000 milli sec
#define BOOT_THRESHOLD_VOLT                 3600

#define TWO_SECONDS                         2000000 /*2seconds = 2000000 uSec*/

#define FIVE_SEC_IN_MILLI_SEC               5000
#define TIMEOUT_WAIT_FOR_KEY                0x01

#define MAX_3000_MA                         3000
#define MAX_1500_MA                         1500

#define THERMAL_TIMEOUT_NOTSET              0
#define MIN_TO_SEC                          60

#define MAX_THERMAL_WAIT_PERIOD_IN_MINS     60

#define USB_3_IUSB_MAX                      900 //IUSB_MAX for USB 3.0 port
#define USB_2_IUSB_MAX                      500 //IUSB_MAX for USB 2.0 port
#define USB_DEFAULT_IUSB_MAX                500
#define FCC_CONSERVE_CHG                    500
#define USB_ENUM_WAIT_DURATION              500 /*500 milliseconds*/

#define MAX_EMERGENCY_SHUTDOWN_COUNT        3

#define THERMAL_TIMEOUT_NOTSET              0
#define MIN_TO_SEC                          60

#define MAX_THERMAL_WAIT_PERIOD_IN_MINS     60

#define CHARGERLIB_SHIPMODE_VARIABLE_NAME   L"ShipMode"
/*===========================================================================*/
/*                  FUNCTIONS PROTOTYPES                                     */
/*===========================================================================*/
//STATIC EFI_STATUS ChargerLib_GetAdcData(IN CONST CHAR8 *pszAdcInputName, OUT INT32 *pAdcData);
EFI_STATUS ChargerLib_SetFccMaxCurrent(UINT32 FccMaxCurrent);
EFI_STATUS ChargerLib_SetFvMaxVoltage(UINT32 VddMaxVoltage);

VOID EFIAPI ChargerLibEnableCallback(IN EFI_EVENT Event, IN VOID *Context);
VOID EFIAPI ChargerLibDisableCallback(IN EFI_EVENT Event, IN VOID *Context);
EFI_STATUS ChargerLib_GetTSensTemp( INT32 *TSensTemp);
EFI_STATUS ChargerLib_HandleThermalErrorType(ChargerLibThermalStatus ThermalStatus, CHARGERLIB_CHARGING_ERROR_TYPES *pChargingError);
EFI_STATUS ChargerLib_GetSEllapsedFromBoot(UINT32 *PmicTimeinS);
EFI_STATUS ChargerLib_GetADCReading(CONST CHAR8 *pszInputName, UINT32 uInputNameSize, EfiAdcResultType *pEfiAdcResult);
EFI_STATUS ChargerLib_PrintDCInStatus();
EFI_STATUS ChargerLib_ConfigureUsbCurrentSetting( VOID );
EFI_STATUS ChargerLib_FlushShipMdeVar(UINT16* variable, UINT8 Value);
EFI_STATUS ChargerLib_GetBatteryType(ChgBattType *pBatteryType);
VOID ChargerLibCommon_HWInit(chargerlib_cfgdata_type *pChargerLibConfig);
EFI_STATUS ChargerLib_GetChargerPresence(BOOLEAN *pChargerPresent);

/*===========================================================================*/
/*                  LOCAL VARIABLE DECLARATIONS                              */
/*===========================================================================*/
STATIC EFI_QCOM_PMIC_SCHG_PROTOCOL          *PmicSchgProtocol  = NULL;
STATIC EFI_QCOM_PMIC_FG_BASIC_PROTOCOL      *PmicFgProtocol    = NULL;
STATIC EFI_QCOM_PMIC_PWRON_PROTOCOL         *PmicPwronProtocol = NULL;
STATIC EFI_ADC_PROTOCOL                     *AdcProtocol       = NULL;
STATIC EFI_USBFN_CHG_SRV_PROTOCOL           *UsbChgSrvProtocol = NULL;
STATIC EFI_QCOM_PMIC_USB_PROTOCOL           *PmicUsbProtocol   = NULL;
STATIC EFI_USB_INIT_PROTOCOL                *UsbInitProtocol = NULL;

EFI_BATTERY_CHARGER_INFO_TYPE           ChargerInfo = {0};
EFI_BATTERY_GAUGE_INFO_TYPE             BatteryGaugeInfo = {0};
chargerlib_cfgdata_type                 gChargerLibCfgData = {0};

STATIC ULogHandle                     gULogHandle = NULL;


EFI_EVENT EfiChargerEnableEvent = (EFI_EVENT)NULL;
EFI_EVENT EfiChargerDisableEvent = (EFI_EVENT)NULL;

extern EFI_GUID gEfiEventChargerEnableGuid;
extern EFI_GUID gEfiEventChargerDisableGuid;
extern EFI_GUID gQcomPmicPwrOnProtocolGuid;
extern EFI_GUID gQcomPmicUsbProtocolGuid;
//extern EFI_GUID gEfiUsbfnChgSrvProtocolGuid;

STATIC EFI_PLATFORMINFO_PLATFORM_TYPE  PlatformType;
STATIC BOOLEAN                         gChargingEnabled = FALSE;
STATIC UINT32                          ThermalTimeOutTimeStamp = 0;
STATIC BOOLEAN                         gStartControllerCalled = FALSE;

STATIC CONST CHARGERLIB_HW_CONFIG_INFO ChargerLibHwConfigTable[CHARGERLIB_PLATFORM_HW_CONFIG_MAX]= {
  {EfiBatteryChargerNone,         0x00010000, EfiBatteryGaugeNone,       0x00010000}, //NONE
  {EfiBatteryChargerQcomPmicSchg, 0x00010000, EfiBatteryGaugeQcomPmicFg, 0x00010000}, //PMIC
};

STATIC CONST CHARGERLIB_PLATFORM_HW_CONFIG_INFO ChargerLibPlatformHwInfo[]=
{
  {EFI_PLATFORMINFO_TYPE_MTP,     NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_PMIC},
  {EFI_PLATFORMINFO_TYPE_FLUID,   NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_PMIC},
  {EFI_PLATFORMINFO_TYPE_QRD,     NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_PMIC},
  {EFI_PLATFORMINFO_TYPE_CHI,     NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_PMIC},
  {EFI_PLATFORMINFO_TYPE_SBC,     NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_PMIC},
  {EFI_PLATFORMINFO_TYPE_LIQUID,  NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_PMIC},
  {EFI_PLATFORMINFO_TYPE_RUMI,    NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_NONE},
  {EFI_PLATFORMINFO_TYPE_CDP,     NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_NONE},
  {EFI_PLATFORMINFO_TYPE_CLS,     NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_NONE},
  {EFI_PLATFORMINFO_TYPE_HDK,     NULL,   CHARGERLIB_PLATFORM_HW_CONFIG_PMIC},
};

/*===========================================================================*/
/*                 FUNCTION DEFINITIONS                                      */
/*===========================================================================*/
/**
  Initialize ChargerDXE Library functions

  @param none

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_INVALID_PARAMETER: A Parameter was incorrect.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_NOT_READY:         The physical device is busy or not ready to
                         process this request.
*/
EFI_STATUS ChargerLibCommon_Init( chargerlib_cfgdata_type *pChargerLibConfig )
{
  EFI_STATUS                  Status         = EFI_SUCCESS;
  ChgBattType                 battType  = CHG_BATT_TYPE_INVALID;
  BOOLEAN                     bDbgBoard = FALSE;

  CopyMemS(&gChargerLibCfgData, sizeof(chargerlib_cfgdata_type), pChargerLibConfig, sizeof(chargerlib_cfgdata_type));

  Status |= GetPlatformType(&PlatformType);
  if(EFI_SUCCESS != Status)
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error getting platform type  \r\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  /* Enable USB and DCIN PON triggers to be certain and avoid bad use case*/
  if (!PmicPwronProtocol)
  {
    Status |= gBS->LocateProtocol(&gQcomPmicPwrOnProtocolGuid, NULL, (VOID **)&PmicPwronProtocol);
  }

  if((Status == EFI_SUCCESS) && (NULL != PmicPwronProtocol))
  {
    Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_PON1_REDGE_PON, FALSE);
    Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_DC_CHG_REDGE_PON, FALSE);
    Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_USB_CHG_REDGE_PON, FALSE);
    Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_CBLPWR_FEDGE_PON, FALSE);
  }
  else
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error locating Pwron Protocol = %r \r\n", __FUNCTION__, Status));
  }

  ChargerLibCommon_HWInit(pChargerLibConfig);

  switch (BatteryGaugeInfo.BatteryGaugeHW)
  {
    case EfiBatteryGaugeQcomPmicFg:
      /* Locate FG Protocol */
      if (!PmicFgProtocol)
      {
        Status |= gBS->LocateProtocol( &gQcomPmicFgProtocolGuid,
                                      NULL,
                                     (VOID **)&PmicFgProtocol );
      }

      /* Initialize FG Protocol */
      if((EFI_SUCCESS == Status ) && ( NULL != PmicFgProtocol))
      {
        Status |= PmicFgProtocol->FgBasicGetPmicInfo(&BatteryGaugeInfo.BatteryGaugePmicInfo);
        if(EFI_SUCCESS != Status)
        {
          CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Failed to get PmicIndex for Fuel Gauge : 0x%08X \r\n", __FUNCTION__, Status));
          return Status;
        }

        gChargerLibCfgData.fg_cfg_data.FgDbgCfg =  *(EFI_PM_FG_DEBUG_CFGDATA_TYPE *)&gChargerLibCfgData.dbg_cfg_Data;

        Status |= PmicFgProtocol->FgBasicInit(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex,
                                              &gChargerLibCfgData.fg_cfg_data);
        if(EFI_SUCCESS != Status)
        {
          CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Failed to initialize Fuel Gauge: 0x%08X  \r\n", __FUNCTION__, Status));
          return Status;
        }

        Status = ChargerLib_GetBatteryType(&battType);
        if (EFI_SUCCESS != Status)
        {
          CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Coulen't retrieve battery type, assuming real battery = %r \r\n", __FUNCTION__, Status));
          Status = EFI_SUCCESS;
      }
        else
        {
          bDbgBoard = (battType == CHG_BATT_TYPE_DEBUG_BOARD) ? TRUE : FALSE;
        }
      }
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;

  }
 
  /* Initialize Charger and Gauge HW */
  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      /* Locate SCHG Protocol */
      if (!PmicSchgProtocol)
      {
        Status |= gBS->LocateProtocol( &gQcomPmicSchgProtocolGuid,
                                       NULL,
                                      (VOID **)&PmicSchgProtocol );
      }

      if((EFI_SUCCESS == Status) && (NULL != PmicSchgProtocol))
      {
        Status = PmicSchgProtocol->SchgGetPmicInfo(&ChargerInfo.ChargerPmicInfo);
        if(EFI_SUCCESS != Status)
        {
          CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Failed to get PmicIndex for SCHG : 0x%08X \r\n", __FUNCTION__, Status));
          return Status;
        }

        gChargerLibCfgData.schg_cfg_data.SchgDbgCfg = *(EFI_PM_SCHG_DEBUG_CFGDATA_TYPE *)&gChargerLibCfgData.dbg_cfg_Data;
        
        /* Initialize SCHG Protocol */
        Status = PmicSchgProtocol->SchgInit(ChargerInfo.ChargerPmicInfo.PmicIndex, gChargerLibCfgData.schg_cfg_data, bDbgBoard);
        if(EFI_SUCCESS != Status)
        {
          CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Failed to initialize SCHG : 0x%08X \r\n", __FUNCTION__, Status));
        }
      }
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

/**
  Get Battery Status Information
  @param[out] BattStatusInfo  refer EFI_PM_FG_BATT_STATUS_INFO
  This function gets the battery status, battery SOC, Charge current, battery voltage
  and battery temperature from Fuel Gauge

  @param[out] BatteryStatus  see chargerlib_batt_status_info for more details

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetBatteryStatus(chargerlib_batt_status_info *pBatteryStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_FG_BATT_STATUS_INFO BatteryStatus = {0};

  if(NULL == pBatteryStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  switch (BatteryGaugeInfo.BatteryGaugeHW)
  {
    case EfiBatteryGaugeQcomPmicFg:
      Status = PmicFgProtocol->GetBatteryStatus(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex,
                                                (EFI_PM_FG_BATT_STATUS_INFO *)&BatteryStatus);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  if(EFI_SUCCESS == Status)
  {
    if (sizeof(chargerlib_batt_status_info) >= sizeof(EFI_PM_FG_BATT_STATUS_INFO))
    {
  CopyMemS(pBatteryStatus, sizeof(chargerlib_batt_status_info), &BatteryStatus, sizeof(EFI_PM_FG_BATT_STATUS_INFO));
    }
    else
    {
      Status = EFI_BAD_BUFFER_SIZE;
    }
  }

  return Status;
}

/**
  Get Battery Status Information
  @param[out] BattStatusInfo  refer EFI_PM_FG_BATT_STATUS_INFO
  This function gets the battery status, battery SOC, Charge current, battery voltage
  and battery temperature from Fuel Gauge

  @param[out] BatteryStatus  see chargerlib_batt_status_info for more details

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetBatteryProfileStatus(chargerlib_batt_profile_status *ProfileSts)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!ProfileSts)
  {
    return EFI_INVALID_PARAMETER;
  }

  switch (BatteryGaugeInfo.BatteryGaugeHW)
  {
    case EfiBatteryGaugeQcomPmicFg:
      /* Locate FG Protocol */
      if (!PmicFgProtocol)
      {
        Status |= gBS->LocateProtocol( &gQcomPmicFgProtocolGuid,
                                      NULL,
                                     (VOID **)&PmicFgProtocol );
        if(Status != EFI_SUCCESS || NULL == PmicFgProtocol)
          return EFI_DEVICE_ERROR;
      }
      Status = PmicFgProtocol->GetBattProfileStatus(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex,
                                                    (EFI_PM_FG_BATT_PROFILE_STATUS *)ProfileSts);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

EFI_STATUS ChargerLib_LoadProfile(OPTIONAL chargerlib_batt_profile_data *pBattProfileInfo)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (BatteryGaugeInfo.BatteryGaugeHW)
  {
  case EfiBatteryGaugeQcomPmicFg:
    if (!PmicFgProtocol)
    {
      Status |= gBS->LocateProtocol( &gQcomPmicFgProtocolGuid,
                                    NULL,
                                   (VOID **)&PmicFgProtocol );
    }
    /* Initialize FG Protocol */
    if((EFI_SUCCESS == Status ) && ( NULL != PmicFgProtocol))
    {
      Status |= PmicFgProtocol->FgExtendedInit(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex,
                                               (EFI_PM_FG_BATT_PROFILE_DATA *)pBattProfileInfo);
      if(EFI_SUCCESS != Status)
      {
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Failed to initialize Fuel Gauge: 0x%08X  \r\n", __FUNCTION__, Status));
        return Status;
      }
    }
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }


  return Status;

}

/**
  Enable charger

  @param[in] Enable  TRUE  - Enable Charging
                     FALSE - Disable Charging

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_ChargerEnable(BOOLEAN Enable)
{
  EFI_STATUS Status = EFI_SUCCESS;
  BOOLEAN    BatteryPresent = FALSE;

  Status = ChargerLib_GetBatteryPresence(&BatteryPresent);
  if (BatteryPresent)
  {
    switch (ChargerInfo.ChargerHW)
    {
    case EfiBatteryChargerQcomPmicSchg:
      Status = PmicSchgProtocol->EnableCharger(ChargerInfo.ChargerPmicInfo.PmicIndex, Enable);
      if( (EFI_SUCCESS == Status ) && (FALSE == Enable ))
      {
        /* make sure to turn Off Charging Led if charging is getting disabled */
        if(gChargerLibCfgData.charger_led_config)
        {
          /* Turn Off LED since Charging is disabled */
          ChargerLib_LedOn(FALSE);
        }
      }
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
    }
  }

  gChargingEnabled = Enable;

  return Status;
}

/**
  Set charger max current

  @param[in] MaxCurrent  Max current for charger in mA

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_SetMaxUsbCurrent(UINT32 MaxCurrent)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->SetUsbMaxCurrent(ChargerInfo.ChargerPmicInfo.PmicIndex, MaxCurrent);
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

/**
  Set Battery FCC Max current

  @param[in] FccMaxCurrent  Max current for charger in mA

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_SetFccMaxCurrent(UINT32 FccMaxCurrent)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->SetFccMaxCurrent(ChargerInfo.ChargerPmicInfo.PmicIndex, FccMaxCurrent);
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargerLib_WasChargerReinserted(BOOLEAN *pChargerReinserted)
{
  EFI_STATUS Status = EFI_SUCCESS;
  //boolean aicl_done = FALSE;

  if(!pChargerReinserted)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pChargerReinserted = FALSE;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      Status = PmicSchgProtocol->ChgrSourceReinserted(ChargerInfo.ChargerPmicInfo.PmicIndex, pChargerReinserted);
      break;
    default:
      break;
  }

  if(EFI_SUCCESS == Status)
  {
    if(TRUE == *pChargerReinserted)
    {
      Status = ChargerLib_ConfigureUsbCurrentSetting();
    }
  }
  else
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error checking Charger re-insertion status, Status: 0x%08X\n",__FUNCTION__, Status));
  }

  return Status;
}

EFI_STATUS ChargerLib_ReRunAicl( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      /*
      Status = PmicSchgProtocol->RerunAicl(ChargerInfo.ChargerPmicInfo.PmicIndex);
      */
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

/**
  Set Battery Floating Max Voltage

  @param[in] VddMaxVoltage  Max Voltage for Charger in mV

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_SetFvMaxVoltage(UINT32 VddMaxVoltage)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      Status = PmicSchgProtocol->SetFvMaxVoltage(ChargerInfo.ChargerPmicInfo.PmicIndex, VddMaxVoltage);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

/**
  Get Charger/Power Path

  @param[out] Powerpath

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_INVALID_PARAMETER: Parameter is invalid.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetPowerPath(CHARGERLIB_ATTACHED_CHGR_TYPE *PowerPath)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->GetPowerPath(ChargerInfo.ChargerPmicInfo.PmicIndex, (PM_SCHG_POWER_PATH_TYPE *)PowerPath);
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargerLib_GetChargingPath(CHARGERLIB_ATTACHED_CHGR_TYPE *ChargingPath)
{
  EFI_STATUS Status = EFI_SUCCESS;
  BOOLEAN IsInputValid = FALSE;

  if (!ChargingPath)
  {
    return EFI_INVALID_PARAMETER;
  }
  *ChargingPath = CHARGERLIB_ATTACHED_CHGR__NONE;
  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->UsbinValid(ChargerInfo.ChargerPmicInfo.PmicIndex, &IsInputValid);
    if(Status != EFI_SUCCESS)
    {
      break;
    }
    if (IsInputValid)
    {
      *ChargingPath = CHARGERLIB_ATTACHED_CHGR__USB;
    }
    else
    {
      Status = PmicSchgProtocol->DcinValid(ChargerInfo.ChargerPmicInfo.PmicIndex, &IsInputValid);
      if(Status != EFI_SUCCESS)
      {
        break;
      }
      if (IsInputValid)
      {
        *ChargingPath = CHARGERLIB_ATTACHED_CHGR__DCIN;
        //this is to print out DCIn V and I
        Status |= ChargerLib_PrintDCInStatus();
      }
      else
      {
        *ChargingPath = CHARGERLIB_ATTACHED_CHGR__NONE;
      }
    }
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

/**
  Get Battery Presence

  @param[out] BatteryPresence

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_INVALID_PARAMETER: Parameter is invalid.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetBatteryPresence(BOOLEAN *BatteryPresence)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
         if(!PmicSchgProtocol)
         {
           Status = gBS->LocateProtocol(&gQcomPmicSchgProtocolGuid, NULL, (VOID **)&PmicSchgProtocol);
         }
         
         if((NULL == PmicSchgProtocol ) || (Status != EFI_SUCCESS))
           return EFI_DEVICE_ERROR;
         
         Status = PmicSchgProtocol->IsBatteryPresent(ChargerInfo.ChargerPmicInfo.PmicIndex, BatteryPresence);
         break;
    default:
         Status = EFI_UNSUPPORTED;
         break;
  }

  return Status;
}

/**
ChargerLib_PrintDebugMsg()

@brief
Returns ChargerLib_PrintDebugMsg Flag Status
*/
inline BOOLEAN
ChargerLib_PrintDebugMsg(void)
{
  return ((BOOLEAN)gChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg);
}

/**
ChargerLib_PrintDebugMsgToFile()

@brief
Returns ChargerLib_PrintDebugMsgToFile Flag Status
*/
inline BOOLEAN
ChargerLib_PrintDebugMsgToFile(void)
{
  if (NULL != gULogHandle) return (TRUE);
  return FALSE;
}

/**
ULogPrint ()

@brief Helper API to print multiple arguments
Returns
*/
inline void EFIAPI
ULogPrint(
  IN  UINTN        LogLevel,
  IN  CONST CHAR8  *Format,
  ...
  )
{
  va_list vlist;
  UINT32  dataCount = 0;
  UINTN   i         = 0;
  BOOLEAN  boEsc = FALSE;
  STATIC EFI_STATUS lastLogResult = EFI_SUCCESS;
  EFI_STATUS CurrentLogResult     = EFI_SUCCESS;
  CHAR8 CurrentFormat[512];
  UINTN CurrentFormatIndex = 0;
  UINT32 size = sizeof(CurrentFormat);
  BOOLEAN appendFlag = FALSE;
  CHAR8 *FnName = NULL;

  if ((LogLevel & gChargerLibCfgData.dbg_cfg_Data.file_log_level_mask) != 0)
  {
      SetMem(CurrentFormat, sizeof(CurrentFormat), 0x00);

      /* If volume corrupted do not attempt to log */
      if (EFI_VOLUME_CORRUPTED == lastLogResult)
          return;

      for (i = 0, dataCount = 0; (NULL != (Format + i)) && (Format[i] != '\0'); i++, CurrentFormatIndex++)
      {
          CurrentFormat[CurrentFormatIndex] = Format[i];
          if (Format[i] == '%' && !boEsc)
          {
              if (Format[i + 1] == 'a')
              {
                  appendFlag = TRUE;
                  va_start(vlist, Format);
                  FnName = (CHAR8*)va_arg(vlist, CHAR8*);
                  AsciiVSPrint((CurrentFormat + CurrentFormatIndex), size, FnName, vlist);
                  CurrentFormatIndex = CurrentFormatIndex + AsciiStrLen(FnName);
                  CurrentFormat[CurrentFormatIndex] = ' ';
                  i += 2;
              }
              else if (Format[i + 1] == 'r')
              {
                  CurrentFormat[++CurrentFormatIndex] = 'd';
                  i++;
                  dataCount++;
              }
              else
              {
                  dataCount++;
              }
          }
          else if (Format[i] == '\\' && !boEsc)
          {
              boEsc = TRUE;
          }
          else if (boEsc)
          {
              boEsc = FALSE;
          }
      }
      /*
      if(appendFlag)
      {
      va_end(vlist);
      }*/
      /* Append end of the string */
      CurrentFormat[CurrentFormatIndex] = '\0';

      //CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a dataCount = %d \r\n",__FUNCTION__, dataCount));

      if (appendFlag == FALSE)
      {
          va_start(vlist, Format);
      }
      //CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a formatstring = %a \r\n",__FUNCTION__, Format));
      //CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a Current Fn Name = %a size = %d \r\n",__FUNCTION__, CurrentFormat, size));
      if (appendFlag)
      {
          CurrentLogResult = (EFI_STATUS)ULogFront_RealTimeVprintf(gULogHandle, dataCount, CurrentFormat, vlist);
      }
      else
      {
          CurrentLogResult = (EFI_STATUS)ULogFront_RealTimeVprintf(gULogHandle, dataCount, Format, vlist);
      }
      if (CurrentLogResult == EFI_VOLUME_CORRUPTED)
      {
          CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a File Log Print Volume Error = %r \r\n", __FUNCTION__, CurrentLogResult));
          lastLogResult = CurrentLogResult;
      }
      else if (CurrentLogResult != EFI_SUCCESS)
      {
          CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a File Log Print Error = %r \r\n", __FUNCTION__, CurrentLogResult));
      }
      else { /* SUCCESS use case */ }

      va_end(vlist);
  }
}


/**
ChargerLib_InitFileLog(BOOLEAN )

@brief  Init File Logging in ULog
Returns Status
*/
EFI_STATUS ChargerLib_InitFileLog(BOOLEAN PrintChgAppDbgMsgToFile)
{
  ULogResult result = 0;
  EFI_STATUS       Status     =  EFI_SUCCESS;
  UINT32           VarSize    =  1;
  BOOLEAN          FileLoggingIsEnabled = TRUE;
  ULOG_CONFIG_TYPE  configType = {0};
  DEBUG(( EFI_D_WARN, "ChargerLib:: %a \r\n", __FUNCTION__));

  if (!RETAIL)
  {
    /* Enable default file logging if production mode is disabled */
    if( TRUE == PrintChgAppDbgMsgToFile )
    {
      Status = gRT->SetVariable (L"EnableFileLogging", &gQcomTokenSpaceGuid,
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                 VarSize, &FileLoggingIsEnabled);
      if (EFI_SUCCESS != Status)
      {      
        DEBUG(( EFI_D_WARN, "ChargerLib:: %a Global File Logging BDS Menu Variable read error = ?%d \r\n", __FUNCTION__, Status));
        return EFI_SUCCESS;
      }

      result = ULogFile_Open(&gULogHandle, LOGFILE_IN_EFS, LOGFILE_SIZE);
      if (0 == result)
      {
        /* Set ULog configuration */
        ULogFile_GetConfig(&gULogHandle, &configType);
        configType.separator = ',';
        configType.PrintTimestamp = TRUE;
        configType.TrimNewline = TRUE;
        ULogFile_SetConfig(&gULogHandle, &configType);
        if (Status != EFI_SUCCESS){
          CHARGER_UART_DEBUG(( EFI_D_ERROR, "ChargerLib:: ULogFile_SetConfig FAILED \r\n"));
        }
        ULOG_RT_PRINTF_1(gULogHandle, "ChargerLib::InitFileLog SUCCESS gULogHandle = %lld", gULogHandle);
        DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog SUCCESS \r\n",  __FUNCTION__));

        /* Print Battery status information Log headers during charging loop */
        /* DEBUG((EFI_D_WARN, "RebootCount,TimeStamp,StateOfCharge,RatedCapacity,Voltage,ChargeCurrent,Temp \r\n"));
        DEBUG((EFI_D_WARN, "TimeStamp,StateOfCharge,RatedCapacity,Voltage,ChargeCurrent,Temp \r\n")); */
      }
      else
      {
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog FAILED \r\n",  __FUNCTION__));
      }
    }
  }

  return EFI_SUCCESS;
}


/**
ChargerLib_GetFileLogHandle(BOOLEAN )

@brief  Returns File long handle if alreayd initilized 
Returns Status
*/
EFI_STATUS ChargerLib_GetLogInfo(chargerlib_loginfo *gFileHandle )
{
  ULogResult result = 0;
  ULOG_CONFIG_TYPE  configType = {0};

  if(!gFileHandle)
    return EFI_INVALID_PARAMETER;

  if(NULL != gULogHandle)
  {
    result = ULogFile_Open(&(gFileHandle->gChargerLogHandle), LOGFILE_IN_EFS, LOGFILE_SIZE);
    if (0 == result)
    {
      /* Set ULog configuration */
      ULogFile_GetConfig(&(gFileHandle->gChargerLogHandle), &configType);
      configType.separator = ',';
      configType.PrintTimestamp = TRUE;
      configType.TrimNewline = TRUE;
      ULogFile_SetConfig(&gFileHandle->gChargerLogHandle, &configType);
      ULOG_RT_PRINTF_1(gFileHandle->gChargerLogHandle, "ChargerLib::InitFileLog SUCCESS gULogHandle = %lld", gFileHandle->gChargerLogHandle);
      DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog SUCCESS \r\n",  __FUNCTION__));
    }
    else
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog FAILED \r\n",  __FUNCTION__));
    }
  }
  gFileHandle->print_charger_app_dbg_msg = gChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg;
  gFileHandle->print_charger_app_dbg_msg_to_file = gChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg_to_file;

   return EFI_SUCCESS;
}

/**
  Gets charger port type

  @param[out] pPortType  Charger port type

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetPortType(CHARGERLIB_CHGR_PORT_TYPE *pPortType)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_PM_SCHG_CHGR_PORT_TYPE  PortType;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      Status  = PmicSchgProtocol->GetChargerPortType(ChargerInfo.ChargerPmicInfo.PmicIndex, &PortType);
      *pPortType = (CHARGERLIB_CHGR_PORT_TYPE)PortType;

      CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a Charger Port type = %d \r\n", __FUNCTION__, PortType));

      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}


EFI_STATUS ChargerLib_HandleNoChargeAndWait(void)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (TRUE == gChargerLibCfgData.no_charge_and_wait)
  {
    /*Disable Charging */
    Status = ChargerLib_ChargerEnable(FALSE);
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Charging Disabled \r\n", __FUNCTION__));
  }
  else
  {
    ChargerLib_ForceSysShutdown(CHGAPP_RESET_AFP);
  }

  return Status;
}

EFI_STATUS ChargerLib_EnableAfpMode(void)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      Status = PmicSchgProtocol->EnableAfpMode(ChargerInfo.ChargerPmicInfo.PmicIndex);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

EFI_STATUS ChargerLib_EnableShipMode(void)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      Status = PmicSchgProtocol->EnableShipMode(ChargerInfo.ChargerPmicInfo.PmicIndex);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

EFI_STATUS ChargerLib_GetBatteryID(INT32 *pBatteryID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EfiAdcResultType AdcResult = {0};

  if(NULL == pBatteryID)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pBatteryID = 0;

  Status = ChargerLib_GetADCReading(ADC_INPUT_BATT_ID_OHMS, sizeof(ADC_INPUT_BATT_ID_OHMS), &AdcResult);

  if (EFI_ERROR(Status))
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: Failed to read ADC ADC_INPUT_BATT_ID_OHMS Channel! \r\n", __FUNCTION__));
    return Status;
  }

  *pBatteryID = AdcResult.nPhysical;
  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a BATT_ID_2 = %d \r\n", __FUNCTION__, *pBatteryID));

  return Status;
}

EFI_STATUS ChargerLib_GetBatteryType(ChgBattType *pBatteryType)
{
  EFI_STATUS Status = EFI_SUCCESS;
  INT32      battery_id = 0;

  if(NULL == pBatteryType)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pBatteryType = CHG_BATT_TYPE_INVALID;

  Status = ChargerLib_GetBatteryID(&battery_id);
  if(EFI_SUCCESS != Status)
  {
    return Status;
  }

  /*Check if battery ID is within real battery range*/
  if((battery_id >= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.SmartBatteryIdMin) &&
     (battery_id <= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.SmartBatteryIdMax))
   {
     *pBatteryType = CHG_BATT_TYPE_SMART;
   }
   else if((battery_id >= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.RegularBatteryIdMin) &&
     (battery_id <= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.RegularBatteryIdMax))
   {
     *pBatteryType = CHG_BATT_TYPE_NORMAL;
   }
   else if((battery_id >= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.DebugBoardBatteryIdMin) &&
     (battery_id <= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.DebugBoardBatteryIdMax))
   {
     *pBatteryType = CHG_BATT_TYPE_DEBUG_BOARD;
   }
   else
   {
     *pBatteryType = CHG_BATT_TYPE_UNKNOWN;
   }

   CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Battery type = %d \r\n", __FUNCTION__, *pBatteryType));

  return Status;
}

EFI_STATUS ChargerLib_UsbSuspend(BOOLEAN Enable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib UsbSuspend = %d \r\n", __FUNCTION__, Enable));

  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->UsbSuspend(ChargerInfo.ChargerPmicInfo.PmicIndex, Enable);
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargerLib_DcinSuspend(BOOLEAN Suspend)
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib DcinSuspend = %d \r\n", __FUNCTION__, Suspend));

  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->DcinSuspend(ChargerInfo.ChargerPmicInfo.PmicIndex, Suspend);
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargerLib_DcinSetPowerLimit(UINT32 PowerInMicroW)
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib powerInMicroW = %d w\r\n", __FUNCTION__, PowerInMicroW));

  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->SetDcinPower(ChargerInfo.ChargerPmicInfo.PmicIndex, PowerInMicroW);
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargerLib_GetDebugBoardAction(CHARGERLIB_ERROR_ACTION_TYPE *pDebugBoardAction)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32     DebugBoardBehaviour = gChargerLibCfgData.debug_board_behavior;

  chargerlib_batt_status_info  BatteryStatus;

  if (!pDebugBoardAction)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
  if( EFI_SUCCESS != Status )
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error getting battery status = %r \r\n", __FUNCTION__, Status));
    return Status;
  }

  /*ignore return type to allow continued booting*/
  ChargerLib_UsbSuspend(TRUE);
  //suspend dcin
  ChargerLib_DcinSuspend(TRUE);

  if(BatteryStatus.BatteryVoltage < gChargerLibCfgData.boot_to_hlos_threshold_in_mv) 
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Debug board voltage = %d mV is below Threshold = %d mV \r\n", __FUNCTION__, BatteryStatus.BatteryVoltage, gChargerLibCfgData.boot_to_hlos_threshold_in_mv));

    switch(DebugBoardBehaviour)
    {
      case DEBUG_BOARD_LOWVBAT_SHUTDOWN:
        Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_PON1_REDGE_PON, TRUE);
		/*For SDM670, charger is in primary pmic, so changing PON1 to edge trigger will not stop rebooting if usb is inserted. We need to actually change it to change usb and dcin pon as edge trigger.
*/
		Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_DC_CHG_REDGE_PON, TRUE);
		Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_USB_CHG_REDGE_PON, TRUE);
		Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_CBLPWR_FEDGE_PON, TRUE);
           *pDebugBoardAction = CHARGERLIB_ERROR_ACTION_SHUTDOWN;
           //display red debug icon
           ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_DEBUG_LOW_SYMBOL, TRUE);
           break;
    case DEBUG_BOARD_LOWVBAT_NOCHARGE:
        *pDebugBoardAction = CHARGERLIB_ERROR_ACTION_DEBUG_BOARD_WAIT;
        ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_DEBUG_LOW_SYMBOL, TRUE);
         break;
    case DEBUG_BOARD_BOOT_TO_HLOS:
        *pDebugBoardAction = CHARGERLIB_ERROR_ACTION_DEBUG_BOARD_GOOD_TO_BOOT;
        ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_DEBUG_BOOT_SYMBOL, TRUE);
        break;
    default:
      
      break;
    }
  }
  else
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a: Debug board voltage = %d mV is above Threshold = %d mV \r\n", __FUNCTION__, BatteryStatus.BatteryVoltage, gChargerLibCfgData.boot_to_hlos_threshold_in_mv));
    *pDebugBoardAction = CHARGERLIB_ERROR_ACTION_DEBUG_BOARD_GOOD_TO_BOOT;
  }

  return EFI_SUCCESS;
}


EFI_STATUS ChargerLib_ConservativeCharging( void )
{
  EFI_STATUS Status = EFI_SUCCESS;

  /*set conservative parameter */
  Status = ChargerLib_SetFvMaxVoltage(gChargerLibCfgData.schg_cfg_data.ChgFvMax - gChargerLibCfgData.conserv_chg_fv_delta);
  Status |= ChargerLib_SetFccMaxCurrent(FCC_CONSERVE_CHG);

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Disabling charging \r\n", __FUNCTION__));
  Status |= ChargerLib_ChargerEnable(FALSE);

  return Status;
}

EFI_STATUS ChargerLib_GetUnknownBatteryAction(CHARGERLIB_ERROR_ACTION_TYPE *pUnknownBatteryAction)
{
  EFI_STATUS Status               = EFI_SUCCESS;
  UINT32     UnknownBattBehaviour = gChargerLibCfgData.unknown_battery_behavior;
  chargerlib_batt_status_info  BatteryStatus;  

  switch (UnknownBattBehaviour)
  {
    case UNKNOWN_BATT_SHUTDOWN:
      *pUnknownBatteryAction = CHARGERLIB_ERROR_ACTION_SHUTDOWN_USB_DC_PON_DISABLED;
      break;
    case UNKNOWN_BATT_BOOT_TO_HLOS:
	  /*Boot to HLOS if battery more than threshold. Else shutdown */
      Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
      if( EFI_SUCCESS != Status )
      {
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error getting battery status = %r \r\n", __FUNCTION__, Status));
        return Status;
      }

      /*ignore return type to allow continued booting*/
      ChargerLib_UsbSuspend(TRUE);
      //suspend dcin
      ChargerLib_DcinSuspend(TRUE);
   
      if(BatteryStatus.BatteryVoltage > gChargerLibCfgData.boot_to_hlos_threshold_in_mv)
      {
            CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a: Unknown battery detected, voltage = %d mV is above Threshold = %d mV \r\n", __FUNCTION__, BatteryStatus.BatteryVoltage, gChargerLibCfgData.boot_to_hlos_threshold_in_mv));
            *pUnknownBatteryAction = CHARGERLIB_ERROR_ACTION_GOOD_TO_BOOT;

      }
  	  else
  	  {
            CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a: Unknown battery detected, shutting down. Voltage = %d mV is below Threshold = %d mV \r\n", __FUNCTION__, BatteryStatus.BatteryVoltage, gChargerLibCfgData.boot_to_hlos_threshold_in_mv));
			      *pUnknownBatteryAction = CHARGERLIB_ERROR_ACTION_SHUTDOWN_USB_DC_PON_DISABLED;
  	  }
  
      break;
    case UNKNOWN_BATT_CONSERVATIVE_CHARGING:
      ChargerLib_ConservativeCharging();
      break;
    case UNKNOWN_BATT_REGULAR_CHARGING:
      *pUnknownBatteryAction = CHARGERLIB_ERROR_ACTION_START_CHARGING;
      break;
    default:
      ChargerLib_GetDebugBoardAction( pUnknownBatteryAction );
      break;
  }

  return Status;
}

/*This API initiates different types of reset */
EFI_STATUS ChargerLib_ForceSysShutdown(ChgAppSysShutdownType ShutdownType)
{
  EFI_STATUS Status = EFI_SUCCESS;

  EFI_RESET_TYPE   ResetType = EfiResetPlatformSpecific;

  /*Disable charging*/
  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Disable chrging ShtDwn Type = %d \r\n", __FUNCTION__, ShutdownType));
  ChargerLib_ChargerEnable(FALSE);
  /* Turn Off Charging Led */
  if(gChargerLibCfgData.charger_led_config)
  {
    /* Turn Off Charging */
    ChargerLib_LedOn(FALSE);
  }

  if (!RETAIL)
  {
    /* Flush serial buffer in production mode */
    SerialPortFlush();
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a SerialPortFlush Shutdown Type = %d \r\n", __FUNCTION__, ShutdownType));
    SerialPortFlush();
  }

  /* Clean up */
  Status = ChargerLib_Exit();
  
  // Get the PMIC USB charger protocol if not already available.
  if (NULL == PmicUsbProtocol) {
    Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&PmicUsbProtocol);
    if (EFI_ERROR(Status))
    {
      PmicUsbProtocol = NULL;
      CHARGER_DEBUG(( EFI_D_WARN, "ChargerLib:: %a Failed to open PMIC USB protocol Status = %r \r\n", __FUNCTION__, Status));
    }
  }

  if(PmicUsbProtocol)
  {
    /* PMi8998 auto powers on upon shutdown with Type-A to C cable attached */
    Status |= PmicUsbProtocol->UsbCSetPortRole(EFI_PM_USB_TYPEC_ROLE_UFP);

    /* delay before shutting down in order to ensure that VBUS discharges well below the 1V coarse detect threshold accuracy. */
    gBS->Stall(20000); //stall 20mS
  }

  switch (ShutdownType)
  {
  case CHGAPP_RESET_SHUTDOWN:
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a CHGAPP_RESET_SHUTDOWN.\r\n", __FUNCTION__));
    ResetType = EfiResetShutdown;
    break;
  case CHGAPP_RESET_AFP:
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a CHGAPP_RESET_AFP.\r\n", __FUNCTION__));
    if (EFI_SUCCESS != ChargerLib_EnableAfpMode())
    {
      /*if AFP not supported then enable USB as edge PON trigger and issue shut down */
      if (NULL != PmicPwronProtocol)
      {
        Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_PON1_REDGE_PON, TRUE);
      }
      ResetType = EfiResetCold;
    }
    break;
  case CHGAPP_RESET_COLD:
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a CHGAPP_RESET_COLD.\r\n", __FUNCTION__));
    ResetType = EfiResetCold;
    break;
  /* Ship mode is not supported on LA */
  case CHGAPP_RESET_SHIPMODE:
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a CHGAPP_RESET_TO_SHIPMODE.\r\n", __FUNCTION__));
    if (EFI_SUCCESS != ChargerLib_EnableShipMode())
    {
      /*if Ship mode not supported then disable USB as PON trigger and issue shut down */
      if (NULL != PmicPwronProtocol)
      {
        Status |= PmicPwronProtocol->SetPonTrigger(PM_DEVICE_0, EFI_PM_PON_TRIGGER_DC_CHG, FALSE);
        Status |= PmicPwronProtocol->SetPonTrigger(PM_DEVICE_0, EFI_PM_PON_TRIGGER_USB_CHG, FALSE);
      }
      ResetType = EfiResetShutdown;
    }
    break;

  case CHGAPP_RESET_SHUTDOWN_USB_DC_PON_DISABLED:
     /*Disable USB as PON trigger and issue shut down */
     if (NULL != PmicPwronProtocol)
     {
       /* TBD to make sure on PON1 disabling */
       Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_PON1_REDGE_PON, TRUE);
	   Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_DC_CHG_REDGE_PON, TRUE);
	   Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_USB_CHG_REDGE_PON, TRUE);
	   Status |= PmicPwronProtocol->EnableEdgePonTrigger(PM_DEVICE_0, EFI_PM_PON_OPTION_CBLPWR_FEDGE_PON, TRUE);	   
     }
     ResetType = EfiResetShutdown;
    break;
  case CHGAPP_RESET_NONE:
  case CHGAPP_RESET_INVALID:
  default:
    /*Do nothing*/
    return Status;
  }

  /* Reset  Device*/
  gRT->ResetSystem(ResetType, EFI_SUCCESS, 0, NULL);

  return Status;
}

UINT64 ChargerLib_TimeGet64(void)
{
  UINT64 Result = 0 ;
  Result = GetPerformanceCounter();
  return(Result);
}

UINT64 ChargerLib_CalculateTime()
{
  UINTN  CpuFrequency = 0;
  UINT64 TotalTicks = 0;
  UINT64 Seconds =0;

  /* Debug Print CPU Frequncey */
  TotalTicks   = ChargerLib_TimeGet64();
  /* Debug Print CPU Frequncey */
  CpuFrequency = GetPerformanceCounterProperties(NULL, NULL);
  Seconds = MultU64x32(TotalTicks, 1000000);
  Seconds = DivU64x32(Seconds, CpuFrequency);
  Seconds /= 1000000;

  return Seconds;
}

UINT64  ChargerLib_CalculateTimeMSec()
{
  UINTN  CpuFrequency = 0;
  UINT64 TotalTicks = 0;
  UINT64 Seconds =0;

  /* Debug Print CPU Frequncey */
  TotalTicks   = ChargerLib_TimeGet64();
  /* Debug Print CPU Frequncey */
  CpuFrequency = GetPerformanceCounterProperties(NULL, NULL);
  Seconds = MultU64x32(TotalTicks, 1000000);
  Seconds = DivU64x32(Seconds, CpuFrequency);
  Seconds /= 1000;

  return Seconds;
}

EFI_STATUS ChargerLib_GetThermalStatus( ChargerLibThermalStatus *ThermalStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;
  INT32 CurrTemp = 0;
  STATIC ChargerLibThermalStatus ThermalSts = CHARGERLIB_THERMAL_INIT; /* Start with Init */
  if((NULL == PmicFgProtocol ) || (NULL == ThermalStatus))
    return EFI_DEVICE_ERROR;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      Status = ChargerLib_GetTSensTemp(&CurrTemp);
      if (Status != EFI_SUCCESS)
      {
        CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a Failed = %r \r\n", __FUNCTION__, Status));
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_STATUS_MAX;/*Updated output params*/
        return Status;
      }
    break;

    default:
      Status = EFI_UNSUPPORTED;
      break;
  }
  /*  Handle if any Error */
  if(EFI_ERROR(Status))
    return Status;

  switch(ThermalSts)
  {
    case CHARGERLIB_THERMAL_INIT:
    case CHARGERLIB_THERMAL_OK:
      if (CurrTemp >= gChargerLibCfgData.thermal_configs.tsens_extreme_Temp)
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_CRITICAL;/*Updated output params*/
      }
      else if (CurrTemp >= gChargerLibCfgData.thermal_configs.tsens_high_temp )
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_HIGH;
      }
      else
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_OK;
      }
    break;

    case CHARGERLIB_THERMAL_HIGH:
      if (CurrTemp >= gChargerLibCfgData.thermal_configs.tsens_extreme_Temp)
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_CRITICAL;/*Updated output params*/
      }
      else if ((CurrTemp > gChargerLibCfgData.thermal_configs.tsens_low_temp) && (CurrTemp < gChargerLibCfgData.thermal_configs.tsens_extreme_Temp))
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_HIGH;
      }
      else if (CurrTemp <= gChargerLibCfgData.thermal_configs.tsens_low_temp)
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_OK;
      }
      else
      {
        CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a Error = %d ThermalSts = %d \r\n", __FUNCTION__, CurrTemp, ThermalSts));
      }
    break;

    case CHARGERLIB_THERMAL_CRITICAL:
      /*should not hit this use case as it is already hanlded by then */
      *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_CRITICAL;/*Updated output params*/
    break;

    case CHARGERLIB_THERMAL_HIGH_TIMEOUT:
      /*should not hit this use case as it is already hanlded by then */
      *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_HIGH_TIMEOUT;/*Updated output params*/
    break;

    default:
    break;
  }

  CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a CurrTsensTemp = %d ThermalStatus = %d\r\n", __FUNCTION__, CurrTemp, *ThermalStatus));

  return Status;
}


EFI_STATUS ChargerLib_GetTSensTemp( INT32 *TSensTemp)
{
  EFI_STATUS Status = EFI_SUCCESS;
  INT32 nTempDeciDegC = 0, nTempDegC = 0;

  STATIC EFI_TSENS_PROTOCOL *Tsens = NULL;

  if(NULL == Tsens)
  {
    Status = gBS->LocateProtocol(&gEfiTsensProtocolGuid, NULL, (VOID**)&Tsens);
    if (Status != EFI_SUCCESS)
    {
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a TSENS Protocol Locate Failed %r \r\n", __FUNCTION__, Status));
      return Status;
    }
  }

  /* Loop if current temperature of device is greater than max */
  Status = Tsens->GetMaxTemp(&nTempDeciDegC);
  if (EFI_SUCCESS != Status)
  {
    CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a TSENS Protocol GetMaxTemp Failed = %r \r\n", __FUNCTION__, Status));
    /* break and exit to AFP ??*/
    return Status;
  }

  /*Temp is divide by 10 in C*/
  nTempDegC = nTempDeciDegC / 10;

  *TSensTemp  = nTempDegC;

  return Status;
}

EFI_STATUS ChargerLib_DumpSram(BOOLEAN bDumpExplicit)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  STATIC UINT32 pastTimeinS = 0;
  UINT32        CurrTimeinS = 0;
  STATIC EFI_QCOM_PMIC_RTC_PROTOCOL  *PmicRtcProtocol = NULL;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      if(!RETAIL)
      {
        if(gChargerLibCfgData.fg_cfg_data.FgSramDbgCfg.DumpSram == TRUE)
        {
          if (NULL == PmicRtcProtocol)
          {
            Status = gBS->LocateProtocol(&gQcomPmicRtcProtocolGuid,
                                         NULL,
                                         (VOID **)&PmicRtcProtocol);
          }
          if (EFI_SUCCESS == Status && NULL!= PmicRtcProtocol)
          {
            Status |= PmicRtcProtocol->GetTimeValueRaw(PM_DEVICE_0, &CurrTimeinS);
          }
          CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a GetTimeValueRaw Status = %r CurrTimeinS = %d S pastTimeinS = %d S \r\n", __FUNCTION__, Status, CurrTimeinS, pastTimeinS));

          if(NULL == PmicFgProtocol)
            return EFI_DEVICE_ERROR;

          /* If call for explicit dump then dump immediately */
          if(TRUE == bDumpExplicit)
          {
            Status |= PmicFgProtocol->DumpFgSram(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex);
          }
          else if((CurrTimeinS - pastTimeinS) >= gChargerLibCfgData.fg_cfg_data.FgSramDbgCfg.DumpSramDuration)
          {
            Status |= PmicFgProtocol->DumpFgSram(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex);
            pastTimeinS = CurrTimeinS; /* Update pastTime for next round */
          }
          else{ /* skip dumps as use case now */}
        }
        else
        {
          //CHARGER_DEBUG(( EFI_D_WARN, "PmicDxe:: %s Disabled \r\n",__FUNCTION__));
        }
      }
    break;

    default:
      Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargerLib_DumpChargerPeripheral()
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      if((TRUE == gChargerLibCfgData.dbg_cfg_Data.enable_charger_fg_Dump ) && ( !RETAIL))
      {
        if( EFI_SUCCESS == Status )
        {
          if(NULL != PmicFgProtocol)
            Status = PmicFgProtocol->DumpPeripheral();

          if(NULL != PmicSchgProtocol)
            Status |= PmicSchgProtocol->DumpPeripheral();
        }
      }
    break;

    default:
      Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargerLib_GetHwJeitaStatus(CHARGERLIB_HW_JEITA_STATUS *pHwJeitaStatus)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_PM_FG_BATT_TEMP_STATUS HwJeitaStatus = {0};

  if(!pHwJeitaStatus)
    return EFI_INVALID_PARAMETER;

  switch (BatteryGaugeInfo.BatteryGaugeHW)
  {
    case EfiBatteryGaugeQcomPmicFg:
      Status = PmicFgProtocol->GetHwJeitaStatus(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex, &HwJeitaStatus);
      break;

    default:
      break;
  }

  if (Status == EFI_SUCCESS)
  {
    if (sizeof(CHARGERLIB_HW_JEITA_STATUS) >= sizeof(EFI_PM_FG_BATT_TEMP_STATUS))
    {
  CopyMemS(pHwJeitaStatus, sizeof(CHARGERLIB_HW_JEITA_STATUS), &HwJeitaStatus, sizeof(EFI_PM_FG_BATT_TEMP_STATUS));
    }
    else
    {
      Status = EFI_BAD_BUFFER_SIZE;
    }
  }


  return Status;
}

EFI_STATUS ChargerLib_GetBatteryTempStatus(CHARGERLIB_BATT_TEMP_STATUS *pBattTempStatus)
{
  //Battery temperature outside of operational range, extreme cold or hot condition (below -15 , above 70 degree, configurable) (AFP) -- BatteryTemperatureOutOfRangeError
  EFI_STATUS Status      = EFI_SUCCESS;
  CHARGERLIB_HW_JEITA_STATUS   HwJeitaStatus = {0};
  chargerlib_batt_status_info  BatteryStatus = {0};

  if (!pBattTempStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pBattTempStatus = ChargerLib_Batt_Temp_Normal;

  Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
  if(EFI_SUCCESS != Status )
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a Error getting battery status = %r \r\n", __FUNCTION__, Status));
    return Status;
  }
  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a BatteryTemp = %d C \r\n", __FUNCTION__, BatteryStatus.BatteryTemperature));

  Status = ChargerLib_GetHwJeitaStatus( &HwJeitaStatus );
  if((TRUE == HwJeitaStatus.JeitaHardCold) || (TRUE == HwJeitaStatus.JeitaHardHot))
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a BatteryTemp out-of-range = %d C \r\n",__FUNCTION__, BatteryStatus.BatteryTemperature));
    *pBattTempStatus = ChargerLib_Batt_Temp_OutsideChargingRange;
  }

  if(( BatteryStatus.BatteryTemperature < gChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempLowLimit) || 
     ( BatteryStatus.BatteryTemperature > gChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempHighLimit))
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a BatteryTemp = %d C, Operational Range lower limit = %d, upper limit = %d \r\n", 
                __FUNCTION__, BatteryStatus.BatteryTemperature, gChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempLowLimit,
                  gChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempHighLimit));

    *pBattTempStatus = ChargerLib_Batt_Temp_OutsideOperationalRange;
  }

  if(EFI_SUCCESS != Status)
  {
    *pBattTempStatus = ChargerLib_Batt_Temp_Invalid;
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a BatteryTemp invalid = %r \r\n",__FUNCTION__, Status));
  }

  return Status;
}

EFI_STATUS ChargerLib_GetBatteryVoltageStatus(CHARGERLIB_VBATT_STATUS *pBatteryVoltageStatus)
{
  EFI_STATUS                   Status      = EFI_SUCCESS;
  STATIC UINT32                EmergencyShutdownCounter = 0;
  chargerlib_batt_status_info  BatteryStatus = {0};

  if (!pBatteryVoltageStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pBatteryVoltageStatus = ChargerLib_VBatt_Normal;

  Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
  if( EFI_SUCCESS != Status )
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error getting battery status = %r \r\n", __FUNCTION__, Status));
    return Status;
  }

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Battery voltage = %d mV \r\n", __FUNCTION__, BatteryStatus.BatteryVoltage));

  /* Return status if vbatt is good */
  if ((UINT32)BatteryStatus.BatteryVoltage > (gChargerLibCfgData.schg_cfg_data.ChgFvMax + gChargerLibCfgData.batt_volt_lim_high_delta))
  {
    EmergencyShutdownCounter = 0;

    Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
    if( EFI_SUCCESS != Status )
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error getting battery status = %r \r\n", __FUNCTION__, Status));
      return Status;
    }

    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Current Battery voltage =  %d mV \r\n", __FUNCTION__, BatteryStatus.BatteryVoltage));

    /* Check for High Voltage Limit */
    if (((UINT32)(BatteryStatus.BatteryVoltage - VBATT_TOLERANCE)) > (gChargerLibCfgData.schg_cfg_data.ChgFvMax + gChargerLibCfgData.batt_volt_lim_high_delta))
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Current Battery voltage too high %d mV, Threshold = %d mV  \r\n", __FUNCTION__,
                                BatteryStatus.BatteryVoltage, (gChargerLibCfgData.schg_cfg_data.ChgFvMax + gChargerLibCfgData.batt_volt_lim_high_delta)));
      *pBatteryVoltageStatus = ChargerLib_VBatt_TooHigh;
    }
  }
  /* Check for vbatt below emergency shutdown limit */
  else if ((UINT32)BatteryStatus.BatteryVoltage < gChargerLibCfgData.emergency_shutdown_vbatt)
  {
    if(BatteryStatus.ChargeCurrent > 0) /* Battery is discharging */
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Current BattVoltage = %d mV is below EmergencyShutdownVbatt = %d mV ", __FUNCTION__,
                    BatteryStatus.BatteryVoltage, gChargerLibCfgData.emergency_shutdown_vbatt));
	  CHARGER_DEBUG((EFI_D_WARN, " EmergencyShutdownCounter = %d \r\n", EmergencyShutdownCounter));
      if(EmergencyShutdownCounter >= MAX_EMERGENCY_SHUTDOWN_COUNT)
      {
        *pBatteryVoltageStatus = ChargerLib_VBatt_BelowThreshold;
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a EmergencyShutdownCounter Exceeded : \r\n", __FUNCTION__ ));
		CHARGER_DEBUG((EFI_D_WARN, " Curr Battery voltage = %d mV is below threshold = %d mV \r\n",
          BatteryStatus.BatteryVoltage, gChargerLibCfgData.emergency_shutdown_vbatt ));
      }
      else
      {
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a EmergencyShutdownCounter = %d \r\n", __FUNCTION__, EmergencyShutdownCounter));
      }
      EmergencyShutdownCounter++;
    }
    else
    {
      EmergencyShutdownCounter = 0;
    }
  }
  else
  {
    EmergencyShutdownCounter = 0;
  }

  return Status;
}

EFI_STATUS ChargerLib_GetErrors(BOOLEAN vbattChecking, CHARGERLIB_CHARGING_ERROR_TYPES  *pChargingError)
{
  EFI_STATUS                        Status            = EFI_SUCCESS;
  ChargerLibThermalStatus           ThermalStatus     = CHARGERLIB_THERMAL_STATUS_MAX;
  CHARGERLIB_BATT_TEMP_STATUS       BattTempStatus    = ChargerLib_Batt_Temp_Normal;
  CHARGERLIB_VBATT_STATUS           BattVoltageStatus = ChargerLib_VBatt_Normal;
  BOOLEAN                           ChargerPresent    = FALSE;

  BOOLEAN     BatteryPresent = FALSE;
  ChgBattType BatteryType = CHG_BATT_TYPE_INVALID;

  STATIC BOOLEAN FirstTimeUnkownBattDetected = FALSE;

  if (!pChargingError)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pChargingError = CHARGERLIB_CHARGING_ERROR_NONE;

  /* Check Critical errors first */

  /* Battery Presence Detection */
  Status = ChargerLib_GetBatteryPresence(&BatteryPresent);
  if (!BatteryPresent)
  {
    *pChargingError = CHARGERLIB_CHARGING_ERROR_BATTERY_NOT_DETECTED;
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR Battery not detected \r\n", __FUNCTION__));
    return Status;//Got Critical error, return
  }

  ChargerLib_GetBatteryType(&BatteryType);
  if(CHG_BATT_TYPE_DEBUG_BOARD == BatteryType)
  {
    *pChargingError = CHARGERLIB_CHARGING_ERROR_DEBUG_BOARD;
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Debug board detected. \r\n", __FUNCTION__));
    return Status; //Not real battery, no need to check other errors
  }
  else if(CHG_BATT_TYPE_UNKNOWN == BatteryType)
  {
    if(FALSE == FirstTimeUnkownBattDetected)
    { 
	  /* Make sure to enable BMD - ideally will be enabled during charger app Init profile load */
       ChargerLib_EnableBMD();	
	  /*If this is the first time unknown battery is detected, return error type as 
        Unknown Battery so that required initial configurations can be done*/
      *pChargingError = CHARGERLIB_CHARGING_ERROR_UNKNOWN_BATTERY;
      FirstTimeUnkownBattDetected = TRUE;
      CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Unknown battery detected. \r\n", __FUNCTION__));
      return Status;
    }
    else
    {/*if Unknown battery was detected earlier, return error type as unknown battery 
       so that GetAction will return required action, otherwise look for other errors*/
      if(UNKNOWN_BATT_SHUTDOWN == gChargerLibCfgData.unknown_battery_behavior)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_UNKNOWN_BATTERY;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Unknown battery detected. \r\n", __FUNCTION__));
        return Status;
      }
    }
  }
  else
  {
    //No error to be returned for regular/smart batteries.
  }

  //Check battery temperature related errors
  if(EFI_SUCCESS == ChargerLib_GetBatteryTempStatus(&BattTempStatus))
  {
    if(ChargerLib_Batt_Temp_OutsideOperationalRange == BattTempStatus)
    {
      *pChargingError = CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_OPERATIONAL_RANGE;
      CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR: Battery temperature out of operational range \r\n", __FUNCTION__));
      return Status; //Got Critical error, return
    }
  }
  else
  {
    *pChargingError = CHARGERLIB_CHARGING_ERROR_FG_ERROR;
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR ChargerLib_GetBatteryTempStatus FG Error \r\n", __FUNCTION__));
  }

  if (vbattChecking)
  {
    /* Check Voltage errors*/
    if(EFI_SUCCESS == ChargerLib_GetBatteryVoltageStatus(&BattVoltageStatus))
    {
      if(ChargerLib_VBatt_TooHigh == BattVoltageStatus)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_VBATT_OUTSIDE_RANGE;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR Battery voltage out of range \r\n", __FUNCTION__));
        return Status;
      }
      else if(ChargerLib_VBatt_BelowThreshold == BattVoltageStatus)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_LOW_VBATT;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR Battery voltage below threshold \r\n", __FUNCTION__));
        return Status;
      }
    }
    else
    {
      *pChargingError = CHARGERLIB_CHARGING_ERROR_FG_ERROR;
       CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR FG Error from  ChargerLib_GetBatteryVoltageStatus \r\n", __FUNCTION__));
    }
  }
  
  /* Check Charge Source Presence */
  Status = ChargerLib_GetChargerPresence(&ChargerPresent);
  if (FALSE == ChargerPresent)
  {
    *pChargingError = CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED;
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib::%a ERROR Charging source not detected \r\n", __FUNCTION__));
  }

  if(*pChargingError != CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED)
  {
    /* Check for errors that require to stop charging */
    if(ChargerLib_Batt_Temp_OutsideChargingRange == BattTempStatus) 
    {
      *pChargingError = CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_CHARGING_RANGE;
      CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR Battery temperature out of charging range \r\n", __FUNCTION__));
      //return Status;
    }
  }
  
  /* Check MSM tesens temp if enabled */
  if(TRUE == gChargerLibCfgData.sw_thermal_mitigation_enable)
  {
    Status |= ChargerLib_GetThermalStatus(&ThermalStatus);
    if (EFI_SUCCESS == Status)
    {
      CHARGERLIB_CHARGING_ERROR_TYPES thermalError = CHARGERLIB_CHARGING_ERROR_TSENSE_OK;
      /* Charging Error will be updated if tesens critical or time out error take place */
      Status |= ChargerLib_HandleThermalErrorType(ThermalStatus, &thermalError);
      if (EFI_SUCCESS != Status)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ChargerLib_HandleThermalErrorType Failed = %r \r\n",__FUNCTION__, Status));
      }
      if((thermalError == CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH )
         || (thermalError == CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT)
         || (thermalError == CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL))
      {
        *pChargingError = thermalError;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Tsense pChargingError = %d \r\n",__FUNCTION__, pChargingError));
      }
            
	  if ( (thermalError == CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH) &&  (FALSE == ChargerPresent))
      {
        chargerlib_batt_status_info BatteryStatus = { 0 };
        //need to check soc or vbatt, if vbatt low, no charger presense then report no charger presense to shutdown
        Status |= ChargerLib_GetBatteryStatus(&BatteryStatus);
        if (gChargerLibCfgData.soc_based_boot == TRUE)
        {
          if (BatteryStatus.StateOfCharge < gChargerLibCfgData.boot_to_hlos_threshold_in_soc)
          {
            *pChargingError = CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED;
          }
        }
        else
        {
          if (BatteryStatus.BatteryVoltage < gChargerLibCfgData.boot_to_hlos_threshold_in_mv)
          {
            *pChargingError = CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED;
          }
        }
      }
     //no need to handle else condition
     //CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH chargerPresent will be considered as thermal Error
     //no matter vbatt or soc threshold.
     //if there is no thermalError, the do not overwrite pChargingError, so no action here either.
    }
    else
    {
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a ChargerLib_GetThermalStatus Failed %r \r\n", __FUNCTION__,Status));
      /* break and exit to AFP ??*/
      *pChargingError = CHARGERLIB_DEVICE_ERROR;
      return Status;
    }
  }

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a pChargingError = %d \r\n", __FUNCTION__, *pChargingError));
  return Status;
}

EFI_STATUS ChargerLib_EnableHWJeita(BOOLEAN Enable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      if(TRUE == Enable)
      {
        if(!PmicFgProtocol)
        {
          Status = gBS->LocateProtocol(&gQcomPmicFgProtocolGuid, NULL, (VOID **)&PmicFgProtocol);
        }

        if((NULL == PmicFgProtocol ) || (Status != EFI_SUCCESS))
          return EFI_DEVICE_ERROR;

        /* Configure HW JEITA Parameters before enabling JEITA */
        Status = PmicFgProtocol->ConfigureHwJeita(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex);
        if(EFI_SUCCESS == Status)
        {
          CHARGER_DEBUG(( EFI_D_WARN, "ChargerLib:: %a HW JEITA is configured \r\n", __FUNCTION__));
        }
        else
        {
          CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Error when configuring HW JEITA parameters %r \r\n", __FUNCTION__, Status));
        }

        if(Status != EFI_SUCCESS)
          return EFI_DEVICE_ERROR;
      }

      if(!PmicSchgProtocol)
      {
        Status = gBS->LocateProtocol(&gQcomPmicSchgProtocolGuid, NULL, (VOID **)&PmicSchgProtocol);
      }

      if((NULL == PmicSchgProtocol) || (Status != EFI_SUCCESS))
      {
        return EFI_DEVICE_ERROR;
      }

      Status = PmicSchgProtocol->EnableHwJeita(ChargerInfo.ChargerPmicInfo.PmicIndex, Enable);
      break;

    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

EFI_STATUS GetSdpMaxCurrent(UINT32 *pSdpMaxCurrent)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_USBFN_CHG_SRV_SPEED_TYPE                SdpSpeed = USB_CHG_SPD_UNKNOWN;

  if(NULL == pSdpMaxCurrent)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pSdpMaxCurrent = USB_2_IUSB_MAX;

  if (!UsbInitProtocol)
  {
    Status = gBS->LocateProtocol(&gEfiUsbInitProtocolGuid, NULL, (VOID **)&UsbInitProtocol);
  }

  if (!UsbChgSrvProtocol)
  {
    Status |= gBS->LocateProtocol(&gEfiUsbfnChgSrvProtocolGuid, NULL, (VOID **)&UsbChgSrvProtocol);
  }

  if((EFI_SUCCESS == Status) && (NULL != UsbInitProtocol) && (NULL != UsbChgSrvProtocol))
  {
    if((FALSE == gStartControllerCalled))
    {
      Status |= UsbInitProtocol->StartController(UsbInitProtocol, 0, EFI_USB_PERIPHERAL_MODE);

      Status |= UsbChgSrvProtocol->StartController(UsbChgSrvProtocol);

      if(EFI_SUCCESS == Status)
      {
        /* Store Enumeration done state, since we will need to call StopController in case of any error and charger swap*/
        gStartControllerCalled = TRUE;
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a success from StartController API, Status: %r \r\n", __FUNCTION__, Status));
      }
      else
      {
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a error from StartController API, Status: %r \r\n", __FUNCTION__, Status));
      }
    }
    else
    {
      WaitForTimeoutNoKey (USB_ENUM_WAIT_DURATION);
    }

    Status |= UsbChgSrvProtocol->GetSpeed(UsbChgSrvProtocol, &SdpSpeed);
    if(EFI_SUCCESS == Status)
    {/*If Status is not success, SDP */
      if(USB_CHG_SPD_SUPER == SdpSpeed)
      {
        *pSdpMaxCurrent = USB_3_IUSB_MAX;
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a CHARGERLIB_CHG_PORT_SDP_CHARGER 3.0 Detected, ChargerCurrent = %d mA \r\n", __FUNCTION__, *pSdpMaxCurrent));
        return Status;
      }
    }
    else
    {
       CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a error getting SDP Speed, Status: : %r \r\n", __FUNCTION__, Status));
    }
  }
  else
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a error locating UsbChgSrv Protocol, Status: : %r \r\n", __FUNCTION__, Status));
    /*if the protocol is not supported, we will fall back to USB2.0 and return SUCCESS*/
    Status = EFI_SUCCESS;
  }

  *pSdpMaxCurrent = USB_2_IUSB_MAX;
  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a CHARGERLIB_CHG_PORT_SDP_CHARGER 2.0 Detected, ChargerCurrent = %d mA \r\n", __FUNCTION__, *pSdpMaxCurrent));

  return Status;
}


EFI_STATUS ChargerLib_GetChgWdogStatus( CHARGERLIB_CHG_WDOG_STS_TYPE *pChgWdogSts)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == pChgWdogSts)
    return EFI_INVALID_PARAMETER;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      /* Locate SCHG Protocol */
      if (!PmicSchgProtocol)
      {
        Status |= gBS->LocateProtocol( &gQcomPmicSchgProtocolGuid,
                                      NULL,
                                     (VOID **)&PmicSchgProtocol );
      }

      if((EFI_SUCCESS == Status) && (NULL != PmicSchgProtocol))
      {
        Status = PmicSchgProtocol->GetChgWdogStatus(ChargerInfo.ChargerPmicInfo.PmicIndex, (EFI_PM_SCHG_WDOG_STS_TYPE *)pChgWdogSts);
      }
      break;
    default:
      return EFI_UNSUPPORTED;
      break;

  }

  return Status;
}

EFI_STATUS ChargerLib_InitializeCharging( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  CHARGERLIB_CHG_WDOG_STS_TYPE  ChgWdogSts = CHARGERLIB_CHG_WDOG_STS_INVALID;
  EFI_SCHG_HVDCP_CFGDATA_TYPE pSchgHvdcpDataType = {0};
  EFI_SCHG_APSD_CFGDATA_TYPE apsd_cfg = {0};
  pSchgHvdcpDataType.bEnHvdcp = FALSE;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Entry \r\n", __FUNCTION__));

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      if (!PmicSchgProtocol)
      {
        Status |= gBS->LocateProtocol( &gQcomPmicSchgProtocolGuid,
                                       NULL,
                                      (VOID **)&PmicSchgProtocol );
      }
  
      if((EFI_SUCCESS != Status) || (NULL == PmicSchgProtocol))
      {
        return EFI_UNSUPPORTED;
      }
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  Status = PmicSchgProtocol->ConfigHvDcp(ChargerInfo.ChargerPmicInfo.PmicIndex, &pSchgHvdcpDataType);

  /* Disable HVDCP or QC 2.0 3.0 detection in UEFI */
  if(Status != EFI_SUCCESS)
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Disable HVDCP failed %r \r\n", __FUNCTION__, Status));
  }

  //Rerun APSD when Charging needs to happen
  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Rerunning APSD\r\n", __FUNCTION__));
  apsd_cfg.bApsdRerun = TRUE;
  Status = PmicSchgProtocol->ConfigApsd(ChargerInfo.ChargerPmicInfo.PmicIndex, &apsd_cfg);
  if(Status != EFI_SUCCESS)
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Rerun APSD failed %r \r\n", __FUNCTION__, Status));
  }

  Status = ChargerLib_GetChgWdogStatus(&ChgWdogSts);
  if((EFI_SUCCESS == Status) && (CHARGERLIB_CHG_WDOG_STS_BITE_CHARGING_DISABLED == ChgWdogSts))
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a WDOG Bite happened, Toggling charging command to re-enable charging. \r\n", __FUNCTION__));
    Status = PmicSchgProtocol->SetChargeCmdBit(ChargerInfo.ChargerPmicInfo.PmicIndex, FALSE);
    gChargingEnabled = FALSE;
  }

  /* Set USB ICL mode and Re-Run APSD if needed on UEFI Charging */
  Status |= ChargerLib_ConfigureUsbCurrentSetting();

  if( FALSE == gChargingEnabled )
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Enabling charging \r\n", __FUNCTION__));
    Status |= ChargerLib_ChargerEnable(TRUE);

    gChargingEnabled = TRUE;
  }

  return Status;
}

EFI_STATUS ChargerLib_GetErrorAction( CHARGERLIB_CHARGING_ERROR_TYPES  ChargingErrorType, CHARGERLIB_ERROR_ACTION_TYPE *pErrorAction)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  chargerlib_batt_status_info BatteryStatus = {0};

  if(!pErrorAction)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pErrorAction = CHARGERLIB_ERROR_ACTION_NONE;
  
  Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
  if( EFI_SUCCESS != Status )
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error getting battery status = %r \r\n", __FUNCTION__, Status));
    return Status;
  }

  switch (ChargingErrorType)
  {
    case CHARGERLIB_CHARGING_ERROR_NONE:
    case CHARGERLIB_CHARGING_ERROR_TSENSE_OK:
        /* No Action just continue */
        break;
    case CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL:
       // case ChargerLib_ChargingError_TsensCritical:
        ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_TSENS_CRITICAL_SYMBOL, TRUE);    
    case CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_OPERATIONAL_RANGE:
        *pErrorAction = CHARGERLIB_ERROR_ACTION_CRITICAL_SHUTDOWN;
        break;
    case CHARGERLIB_CHARGING_ERROR_BATTERY_NOT_DETECTED:
         ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_NOBATTERY, TRUE);
         *pErrorAction = CHARGERLIB_ERROR_ACTION_SHUTDOWN;
         break;
    case CHARGERLIB_CHARGING_ERROR_LOW_VBATT:
         /* Emergency Low VBatt shutdown */
         ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_LOWBATTERY, TRUE);
         *pErrorAction = CHARGERLIB_ERROR_ACTION_SHUTDOWN_USB_DC_PON_DISABLED;
      break;
    case CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT:
         ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_TSENS_THERMAL_SYMBOL, TRUE);
    case CHARGERLIB_CHARGING_ERROR_ADC_ERROR:
    case CHARGERLIB_CHARGING_ERROR_FG_ERROR:
    case CHARGERLIB_CHARGING_ERROR_VBATT_OUTSIDE_RANGE:
    case CHARGERLIB_CHARGING_ERROR_CHARGING_TIMEOUT:
    case CHARGERLIB_DEVICE_ERROR:
        *pErrorAction = CHARGERLIB_ERROR_ACTION_SHUTDOWN;
        break;
    case CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED:
        if(gChargerLibCfgData.soc_based_boot == TRUE)
        {
          if(BatteryStatus.StateOfCharge < gChargerLibCfgData.boot_to_hlos_threshold_in_soc) 
          {
            ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_NOCHARGER, TRUE);
            *pErrorAction = CHARGERLIB_ERROR_ACTION_SHUTDOWN;
          }
          else
          {//if already reached threshold,it's not an error and goot to boot 
            *pErrorAction = CHARGERLIB_ERROR_ACTION_GOOD_TO_BOOT;
          }
        }
        else
        {
          if(BatteryStatus.BatteryVoltage < gChargerLibCfgData.boot_to_hlos_threshold_in_mv) 
          {
            ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_NOCHARGER, TRUE);
            *pErrorAction = CHARGERLIB_ERROR_ACTION_SHUTDOWN;
          }
          else
          {//if already reached threshold,it's not an error and goot to boot 
            *pErrorAction = CHARGERLIB_ERROR_ACTION_GOOD_TO_BOOT;
          }
        }
       break;

    case CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_CHARGING_RANGE:
        if(gChargerLibCfgData.soc_based_boot == TRUE)
        {
          if(BatteryStatus.StateOfCharge < gChargerLibCfgData.boot_to_hlos_threshold_in_soc) 
          {
            *pErrorAction = CHARGERLIB_ERROR_ACTION_NO_CHARGE_WAIT;
          }
          else
          {//if already reached threshold,it's not an error and goot to boot 
            *pErrorAction = CHARGERLIB_ERROR_ACTION_GOOD_TO_BOOT;
            CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Temp outside charging range good soc boot to hlos \r\n", __FUNCTION__));
          }
        }
        else
        {
          if(BatteryStatus.BatteryVoltage < gChargerLibCfgData.boot_to_hlos_threshold_in_mv) 
          {
            *pErrorAction = CHARGERLIB_ERROR_ACTION_NO_CHARGE_WAIT;
          }
          else
          {//if already reached threshold,it's not an error and goot to boot 
            CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Temp outside charging range good vbatt boot to hlos \r\n", __FUNCTION__));
            *pErrorAction = CHARGERLIB_ERROR_ACTION_GOOD_TO_BOOT;
          }
        }
       break;
    case CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH:
         *pErrorAction = CHARGERLIB_ERROR_ACTION_TSENSE_HIGH_WAIT;
         break;
    case CHARGERLIB_CHARGING_ERROR_DEBUG_BOARD:
         Status = ChargerLib_GetDebugBoardAction(pErrorAction);
         break;
    case CHARGERLIB_CHARGING_ERROR_UNKNOWN_BATTERY:
         Status = ChargerLib_GetUnknownBatteryAction(pErrorAction);
         break;
  }

  return Status;
}

EFI_STATUS ChargerLib_EnableWdog(BOOLEAN Enable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      Status = PmicSchgProtocol->EnableChgWdog(ChargerInfo.ChargerPmicInfo.PmicIndex, Enable);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

EFI_STATUS ChargerLib_PetChgWdog( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      Status = PmicSchgProtocol->PetChgWdog(ChargerInfo.ChargerPmicInfo.PmicIndex);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

/**
  Charger Lib Exit

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_Exit( VOID )
{
  EFI_STATUS     Status      = EFI_SUCCESS;
  STATIC BOOLEAN ChgExitDone = FALSE;

  if(FALSE == ChgExitDone)
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLibExit \r\n", __FUNCTION__));
    /* ChargerExit is called only once */
    ChgExitDone = TRUE;
    switch (BatteryGaugeInfo.BatteryGaugeHW)
    {
      case EfiBatteryGaugeQcomPmicFg:
        if(!RETAIL)
        {
          if(gChargerLibCfgData.dbg_cfg_Data.enable_charger_fg_Dump)
            ChargerLib_DumpChargerPeripheral();
        }

        /* Explicit dump during exit */
        ChargerLib_DumpSram(TRUE);

        if(NULL != PmicFgProtocol)
        {
          Status  = PmicFgProtocol->FgBasicExit(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex);
          if((gChargerLibCfgData.soc_based_boot == TRUE) || (gChargerLibCfgData.fg_cfg_data.LoadBatteryProfile == TRUE))
          {
            Status |= PmicFgProtocol->FgExtendedExit(BatteryGaugeInfo.BatteryGaugePmicInfo.PmicIndex);
          }
        }
        if(NULL != PmicSchgProtocol)
        {
          Status = PmicSchgProtocol->SchgExit(ChargerInfo.ChargerPmicInfo.PmicIndex);
        }
          /* Turn Off Charging Led */
        if(gChargerLibCfgData.charger_led_config)
        {
          /* Make sure to Turn Off LED during exit */
          ChargerLib_LedOn(FALSE);
        }
        /* This is to make sure wdog actions are taken in terms of charger app skips charging as SBL have enabled charger wdog */
        if(CHARGERLIB_CHG_WDOG_LEAVE_ENABLED_ON_EXIT == gChargerLibCfgData.enable_charger_wdog)
        { //Pet the watchdog if feature is enabled, before booting to HLOS
          ChargerLib_PetChgWdog();
        }
        else
        { /* Disable Charger Watchdog before booting to HLOS*/
          ChargerLib_EnableWdog( FALSE );
        }

      break;

      default:
        Status = EFI_UNSUPPORTED;
      break;
    }
  }

  if(TRUE == gStartControllerCalled)
  {
    if (!UsbChgSrvProtocol)
    {
      Status |= gBS->LocateProtocol(&gEfiUsbfnChgSrvProtocolGuid, NULL, (VOID **)&UsbChgSrvProtocol);
    }

    Status |= UsbChgSrvProtocol->StopController(UsbChgSrvProtocol);

    if (!UsbInitProtocol)
    {
      Status |= gBS->LocateProtocol(&gEfiUsbInitProtocolGuid, NULL, (VOID **)&UsbInitProtocol);
    }

    Status |= UsbInitProtocol->StopController(UsbInitProtocol, 0, EFI_USB_PERIPHERAL_MODE);

    if(EFI_SUCCESS != Status)
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error checking Charger re-insertion status, Status: 0x%08X\n",__FUNCTION__, Status));
    }

    gStartControllerCalled = FALSE;
  }

  return Status;
}

EFI_STATUS ChargerLib_RecordThermalStartTimeOut()
{
  EFI_STATUS Status   = EFI_SUCCESS;

  /*Update time stamp to global */
  Status = ChargerLib_GetSEllapsedFromBoot(&ThermalTimeOutTimeStamp);
  if (Status == EFI_SUCCESS)
  {
    CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a ThermalTimeOutTimeStamp = %d Seconds \r\n",__FUNCTION__, ThermalTimeOutTimeStamp));
  }
  else
  {
    CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerApp:: %a Error = %d \r\n",__FUNCTION__, Status));
  }

  return Status;
}

EFI_STATUS ChargerLib_GetSEllapsedFromBoot(UINT32 *PmicTimeinS)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 PmicTime = 0;
  STATIC EFI_QCOM_PMIC_RTC_PROTOCOL  *pPmicRtc = NULL;

  if(!PmicTimeinS)
    return EFI_INVALID_PARAMETER;

  if(!pPmicRtc)
  {
    Status = gBS->LocateProtocol( &gQcomPmicRtcProtocolGuid,
                                  NULL,
                                  (VOID**) &pPmicRtc);
    if (Status != EFI_SUCCESS) {
        return Status;
    }
  }

  Status |= pPmicRtc->GetTimeValueRaw(0, &PmicTime);
  if (Status != EFI_SUCCESS)
  {
    CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error = %d \r\n",__FUNCTION__, Status));
  }

  *PmicTimeinS = PmicTime;

  return Status;
}

EFI_STATUS ChargerLib_HandleThermalHigh(CHARGERLIB_CHARGING_ERROR_TYPES *pChargingError)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 PmicCurrTimeinS = 0;
  UINT32 ConfigThermalTimeOutinS = 0;

  if(NULL == pChargingError)
    return EFI_INVALID_PARAMETER;

  /* Assign default - but do not need to override charge none error type unless it is time out*/
  /* *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_OK; */

  /* Note Thermal Start Time */
  if(THERMAL_TIMEOUT_NOTSET == ThermalTimeOutTimeStamp)
  {
    /*Record start Time in Seconds */
    Status |= ChargerLib_RecordThermalStartTimeOut();
    *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH;
  }
  else
  {
    /*Timeout in seconds */
    ConfigThermalTimeOutinS = gChargerLibCfgData.thermal_configs.tsens_timeout_mins * MIN_TO_SEC;

    Status |= ChargerLib_GetSEllapsedFromBoot(&PmicCurrTimeinS);
    if((PmicCurrTimeinS - ThermalTimeOutTimeStamp ) >= ConfigThermalTimeOutinS)
    {
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %aChgAppHandleThermalHigh TimeOut: Shutdown device \r\n", __FUNCTION__));
      /*DEBUG */
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a PmicCurrTimeinS = %d s, ThermalTimeOutTimeStamp =  %d s, ",
                               __FUNCTION__, PmicCurrTimeinS, ThermalTimeOutTimeStamp ));
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ConfigThermalTimeOutinS = %d s \r\n", ConfigThermalTimeOutinS));							   
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT;
    }
    else
    { /*DEBUG */
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a PmicCurrTimeinS = %d s, ThermalTimeOutTimeStamp = %d s, ",
                               __FUNCTION__, PmicCurrTimeinS, ThermalTimeOutTimeStamp));
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, " ConfigThermalTimeOutinS = %d s  Status = %d \r\n",
                              ConfigThermalTimeOutinS, Status));
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH;
    }
  }
  return Status;
}


EFI_STATUS ChargerLib_HandleThermalErrorType(ChargerLibThermalStatus ThermalStatus, CHARGERLIB_CHARGING_ERROR_TYPES *pChargingError)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == pChargingError)
    return EFI_INVALID_PARAMETER;

  /* Assign default - but do not need to override charge none error type unless it is time out*/
  /* *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_OK;*/

  switch(ThermalStatus)
  {
    /*Thermal CRITICAL shutodwn with AFP */
    case CHARGERLIB_THERMAL_CRITICAL:
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL;
      break;

    /*Themal timeout during HIGH thermal zone, shutdown */
    case CHARGERLIB_THERMAL_HIGH_TIMEOUT:
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT;
      break;

    case CHARGERLIB_THERMAL_OK:
      /* Do not need to override charger error none here keeping below for easy reading */
      /* *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_OK; */

      /*Reset TimeOut Stamp*/
      if(THERMAL_TIMEOUT_NOTSET != ThermalTimeOutTimeStamp)
      {
        /* Enable Charging */
        Status |= ChargerLib_ChargerEnable(TRUE);
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Thermal Time Record Reset \r\n", __FUNCTION__));
        ThermalTimeOutTimeStamp = 0;
      }
      break;

    /* Themal HIGH to wait for thermal to come down TsensLowTemp */
    case CHARGERLIB_THERMAL_HIGH:
      /* Disable Charging - take action will diable charging */
      /* Status |= ChargerLib_ChargerEnable(FALSE); */
      Status |= ChargerLib_HandleThermalHigh(pChargingError);
      if(EFI_SUCCESS != Status)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL;
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib_HandleThermalHigh Error = %r Setting Thermal Time out Timer \r\n", __FUNCTION__, Status));
      }
    break;

    default:
    case CHARGERLIB_THERMAL_STATUS_MAX:
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a ThermalStatus = %d \r\n", __FUNCTION__, ThermalStatus));
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL;
    break;
  }

  CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a ThermalStatus = %d \r\n", __FUNCTION__, ThermalStatus));
  return Status;
}

EFI_STATUS ChargerLib_GetChargingStatus(BOOLEAN *pChargingEnabled)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_SCHG_CHARGING_STATUS ChargingSts = {0};

  if(!pChargingEnabled)
    return EFI_INVALID_PARAMETER;

  *pChargingEnabled = FALSE;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      /* Locate SCHG Protocol */
      if (!PmicSchgProtocol)
      {
        Status |= gBS->LocateProtocol( &gQcomPmicSchgProtocolGuid,
                                      NULL,
                                     (VOID **)&PmicSchgProtocol );
      }

      if((EFI_SUCCESS == Status ) && (NULL != PmicSchgProtocol))
      {
        Status |= PmicSchgProtocol->GetChargingStatus(ChargerInfo.ChargerPmicInfo.PmicIndex, &ChargingSts);
      }
      if(EFI_SUCCESS == Status )
      {
        *pChargingEnabled = ChargingSts.bChargingEnabled;
      }
      break;

    default:
      break;
  }

  return Status;
}

EFI_STATUS ChargerLib_PrintDCInStatus()
{
  EFI_STATUS Status = EFI_SUCCESS;
  EfiAdcResultType AdcResult = { 0 };

  Status = ChargerLib_GetADCReading(ADC_INPUT_DC_IN, sizeof(ADC_INPUT_DC_IN), &AdcResult);

  if (EFI_ERROR(Status))
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: Failed to get ADC reading from ADC_INPUT_DC_IN!\n", __FUNCTION__));
    return Status;
  }

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: DCInVoltage = %d\n", __FUNCTION__, AdcResult.nPhysical));

  Status = ChargerLib_GetADCReading(ADC_INPUT_DC_IN_I, sizeof(ADC_INPUT_DC_IN_I), &AdcResult);
  if (EFI_ERROR(Status))
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: Failed to get ADC reading from ADC_INPUT_DC_IN_I!\n", __FUNCTION__));
    return Status;
  }

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: DCInCurrent = %d\n", __FUNCTION__, AdcResult.nPhysical));
  return Status;
}

EFI_STATUS ChargerLib_GetADCReading(CONST CHAR8 *pszInputName, UINT32 uInputNameSize, EfiAdcResultType *pEfiAdcResult)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EfiAdcDeviceChannelType channel;

  if (NULL == pszInputName || NULL == pEfiAdcResult)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (!AdcProtocol)
  {
    Status = gBS->LocateProtocol(&gEfiAdcProtocolGuid,
      NULL,
      (VOID**)&AdcProtocol);
  }

  if ((EFI_ERROR(Status)) || (NULL == AdcProtocol))
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: Failed to load ADC Protocol!\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  Status = AdcProtocol->GetChannel(pszInputName, uInputNameSize, &channel);
  if (EFI_ERROR(Status))
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: Failed to get ADC Channel!\n", __FUNCTION__ ));
    return EFI_DEVICE_ERROR;
  }

  Status = AdcProtocol->AdcRead(&channel, pEfiAdcResult);
  if ((EFI_ERROR(Status)) || (EFI_ADC_RESULT_INVALID == pEfiAdcResult->eStatus))
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: Failed to read ADC Channel!\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

EFI_STATUS ChargerLib_SetUSbICLMode(CHARGERLIB_USB_ICL_MODE_TYPE IclModeType)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      /* Locate SCHG Protocol */
      if (!PmicSchgProtocol)
      {
        Status |= gBS->LocateProtocol( &gQcomPmicSchgProtocolGuid,
                                      NULL,
                                     (VOID **)&PmicSchgProtocol );
      }

      if( EFI_SUCCESS == Status )
      {
        Status = PmicSchgProtocol->SetUsbIclMode(ChargerInfo.ChargerPmicInfo.PmicIndex, (EFI_PM_SCHG_USB_ICL_MODE_TYPE) IclModeType);
      }
      break;
    default:
      break;
  }

  return Status;
}

EFI_STATUS ChargerLib_ConfigureUsbCurrentSetting( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32     MaxCurrent = 0;
  CHARGERLIB_CHGR_PORT_TYPE PortType = CHARGERLIB_CHG_PORT_INVALID;
  CHARGERLIB_USB_ICL_MODE_TYPE IclModeType = CHARGERLIB_USB_ICL_MODE_INVALID;
  UINT32     UsbInputCurrentLimitInMa = 0;
  static CHARGERLIB_CHGR_PORT_TYPE PrevPortType = CHARGERLIB_CHG_PORT_INVALID;
  static BOOLEAN RunFirstTime = FALSE;

  Status |= ChargerLib_GetPortType(&PortType);

  if ((Status == EFI_SUCCESS) &&
      (PortType == PrevPortType) && 
      (RunFirstTime == TRUE))
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Port type = %d, no change, skip configuration) \r\n", __FUNCTION__, PortType));
    return Status;
  }

  if(EFI_SUCCESS == Status)
  {
    if(CHARGERLIB_CHG_PORT_SDP_CHARGER == PortType)
    {
      Status = GetSdpMaxCurrent(&MaxCurrent);
      if(USB_3_IUSB_MAX == MaxCurrent)
      {
        IclModeType = CHARGERLIB_USB_ICL_MODE_SDP_3_0;
        UsbInputCurrentLimitInMa = USB_3_IUSB_MAX;
      }
      else
      {
        IclModeType = CHARGERLIB_USB_ICL_MODE_SDP_2_0;
        UsbInputCurrentLimitInMa = USB_2_IUSB_MAX;
      }

      /*Override Status*/
      Status = EFI_SUCCESS;
    }
    else if (CHARGERLIB_CHG_PORT_INVALID == PortType)
    {
      IclModeType = CHARGERLIB_USB_ICL_MODE_SDP_2_0;
      UsbInputCurrentLimitInMa = USB_2_IUSB_MAX;
    }
    else
    {
      IclModeType = CHARGERLIB_USB_ICL_MODE_WALL_CHARGERS;
    
      //If port type is float charger, set the IUSB to default (500mA)
      //Else(DCP/OCP/CDP/HVDCP), set it to 1500mA
      if( PortType == CHARGERLIB_CHG_PORT_FLOAT_CHARGER)
      {      
        UsbInputCurrentLimitInMa = USB_DEFAULT_IUSB_MAX;
      }
      else
      {
        UsbInputCurrentLimitInMa = MAX_1500_MA;
      }    
    }
  }

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a Setting ICL MOde to:  %d \r\n", __FUNCTION__, IclModeType));
  Status = ChargerLib_SetUSbICLMode(IclModeType);

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a Setting ICL to %d mA (port type = %d) \r\n", __FUNCTION__, UsbInputCurrentLimitInMa, PortType));
  Status |= ChargerLib_SetMaxUsbCurrent(UsbInputCurrentLimitInMa); 
    
  if(Status != EFI_SUCCESS)
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a Failed to configure ICL\r\n", __FUNCTION__));
  }
  else
  {
    //Update Previous Port Type when all configuration happen successfully
    PrevPortType = PortType;
    if (RunFirstTime == FALSE)
    {
      RunFirstTime = TRUE;
    }
  }
  
  return Status;
}

EFI_STATUS ChargerLib_LedOn( BOOLEAN bLedOn)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_RGB_STATUS_TYPE  RgbStatus = {0};
  STATIC EFI_QCOM_PMIC_RGB_LED_PROTOCOL *PmicRgbLedProtocol= NULL;
  BOOLEAN ledOnStatus = FALSE;
  STATIC UINT32 RgbPmicIndex =0;

  switch (ChargerInfo.ChargerHW)
  {
    case EfiBatteryChargerQcomPmicSchg:
      if(!PmicRgbLedProtocol)
      {
        Status = gBS->LocateProtocol(&gQcomPmicRgbLedProtocolGuid, NULL, (VOID **)&PmicRgbLedProtocol);
      }
      if((Status != EFI_SUCCESS) || (NULL == PmicRgbLedProtocol ))
      {
        CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a Protocol Failed = %r \r\n", __FUNCTION__, Status));
        return EFI_DEVICE_ERROR;
      }
      Status = PmicRgbLedProtocol->GetRgbLedPmicIndex(EFI_PM_RGB_CHARGER_LED, &RgbPmicIndex);
      if (Status != EFI_SUCCESS)
      {
          CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a PmicRgbLedProtocol GetRgbLedPmicIndex Failed = %r \r\n", __FUNCTION__, Status));
          return Status;
      }
      if(bLedOn == TRUE)
      {
        /* TURN on LED If only needed other wise keep it ON untill OFF is requested */
        Status |= PmicRgbLedProtocol->GetStatus(RgbPmicIndex, EFI_PM_RGB_1, &RgbStatus);
        if (Status != EFI_SUCCESS)
        {
          CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a PmicRgbLedProtocol GetStatus Failed = %r \r\n", __FUNCTION__, Status));
          return Status;
        }
        if((RgbStatus.VoltSource == EFI_PM_RGB_VOLTAGE_SOURCE_VPH )&&
          (RgbStatus.EnabledRgbMask  == EFI_PM_RGB_LED_RED ))
        {
          CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a Charging indication LED is ON  = %r \r\n", __FUNCTION__, Status));
          ledOnStatus = TRUE;
        }
        /* If Read status is different that passed on then change led state */
        if(ledOnStatus != bLedOn)
        {
          /* Change Led state irrespective of charging status called to make sure of charging status */
          Status |= PmicRgbLedProtocol->ConfigRgbLed(RgbPmicIndex, EFI_PM_RGB_1, EFI_PM_RGB_LED_RED, EFI_PM_RGB_VOLTAGE_SOURCE_VPH, EFI_PM_RGB_DIM_LEVEL_MID, bLedOn);
          /*  Handle if any Error */
          if (Status != EFI_SUCCESS)
          {
            CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a Failed = %r \r\n", __FUNCTION__, Status));
            return Status;
          }
          else
          {
            CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a LED TURNED ON = %r \r\n", __FUNCTION__, Status));
          }
        }
      }
      else
      {
        /* Turn OFF LED irrespective of charging status */
        Status |= PmicRgbLedProtocol->ConfigRgbLed(RgbPmicIndex, EFI_PM_RGB_1, EFI_PM_RGB_LED_RED, EFI_PM_RGB_VOLTAGE_SOURCE_VPH, EFI_PM_RGB_DIM_LEVEL_MID, bLedOn);
        /*  Handle if any Error */
        if (Status != EFI_SUCCESS)
        {
          CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a Failed = %r \r\n", __FUNCTION__, Status));
          return Status;
        }
        else
        {
          CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a LED TURNED OFF = %r \r\n", __FUNCTION__, Status));
        }
      }
    break;

    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

EFI_STATUS ChargerLib_ToggleWipowerShutDownLatch()
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->ToggleWipowerSDLatch(ChargerInfo.ChargerPmicInfo.PmicIndex);
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargerLib_SetDcinCurrent(UINT32 DcinCurrentInmA)
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib Current = %d mA\r\n", __FUNCTION__, DcinCurrentInmA));

  switch (ChargerInfo.ChargerHW)
  {
  case EfiBatteryChargerQcomPmicSchg:
    Status = PmicSchgProtocol->SetDcinCurrent(ChargerInfo.ChargerPmicInfo.PmicIndex, DcinCurrentInmA);
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS ChargeLib_IsDcinValid(BOOLEAN* pIsValid)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!pIsValid)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (!PmicSchgProtocol)
  {
    Status = gBS->LocateProtocol(&gQcomPmicSchgProtocolGuid,
      NULL,
      (VOID **)&PmicSchgProtocol);

    if((Status != EFI_SUCCESS) || (NULL == PmicSchgProtocol ))
    {
      CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a pmicSchgProtocol Failed = %r \r\n", __FUNCTION__, Status));
      return EFI_DEVICE_ERROR;
    }
  }

  Status = PmicSchgProtocol->DcinValid(ChargerInfo.ChargerPmicInfo.PmicIndex, pIsValid);

  return Status;
}


EFI_STATUS ChargerLib_GetShipMode(BOOLEAN* Enabled)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8  EnShipMode = 0;
  UINTN  DataSize = 0;
  UINT16 UniStr[64] = {0};

  AsciiStrToUnicodeStr((CHAR8 *)gChargerLibCfgData.shipmode_cfg.shipMode_str, UniStr);

  /* Read Factory Mode variable  */
  DataSize = sizeof(EnShipMode);
  Status = gRT->GetVariable (UniStr,
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &EnShipMode );
  /* Assign only if variable exist in fv*/
  EnShipMode = (Status == EFI_SUCCESS) ? EnShipMode : 0;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Status = 0x%08X \r\n", __FUNCTION__, Status));
  if(Status == EFI_NOT_FOUND)
  {
    EnShipMode = 0;
    Status |= ChargerLib_FlushShipMdeVar(UniStr, EnShipMode);
    if(Status == EFI_SUCCESS)
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship Mode variable for first time read failure - SUCCESS = %r \r\n", __FUNCTION__, Status));
    }
    else
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship Mode variable for first time failed = %r \r\n",__FUNCTION__, Status));
      return Status;
    }
  }
  if(EnShipMode)
  {
    *Enabled = TRUE;
    /* Clear FV variable */
    EnShipMode = 0;
    Status |= ChargerLib_FlushShipMdeVar(UniStr, EnShipMode);
    if(Status == EFI_SUCCESS)
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship mode Clearing variable SUCCESS = %r \r\n",__FUNCTION__, Status));
    }
    else
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship mode Clearing variable failed = %r \r\n", __FUNCTION__,Status));
    }
  }
  else
  {
    *Enabled = FALSE;
  }
  return Status;
}

/* Flush the NV Storage tables before turning ON write protection
 * Do this only if the protocol available. During initial stages the
 * the protocol might not be installed */

EFI_STATUS ChargerLib_FlushShipMdeVar(UINT16* variable, UINT8 Value)
{
  EFI_VARIABLESERVICES_PROTOCOL *VariableServicesProtocol = NULL;
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN   DataSize = sizeof(Value);
  UINT32  Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS);

  if (NULL == variable)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gRT->SetVariable(variable,
                            &gQcomTokenSpaceGuid,
                            Attributes,
                            DataSize,
                            &Value);
  Status |= gBS->LocateProtocol(&gEfiVariableServicesProtocolGuid, NULL, (VOID **)&VariableServicesProtocol);

  if (VariableServicesProtocol != NULL)
  {
    Status |= VariableServicesProtocol->FlushVariableNV(VariableServicesProtocol);
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Clearing Factory Mode variable flushed SUCCESS = 0x%08X \r\n", __FUNCTION__, Status));
  }
  else
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship Mode variable flushed Failed = %r \r\n", __FUNCTION__, Status));
  }

  return Status;
}

void ChargerLibCommon_HWInit(chargerlib_cfgdata_type *pChargerLibConfig)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32  plat_index = 0;
  CHARGERLIB_PLATFORM_HW_CONFIG_TYPE config_index = CHARGERLIB_PLATFORM_HW_CONFIG_INVALID;
  UINT32  HwInfoSize = (UINT32)(sizeof(ChargerLibPlatformHwInfo)/sizeof(CHARGERLIB_PLATFORM_HW_CONFIG_INFO));

  //Initialize Charger and Battery GaugeInfo
  ChargerInfo.ChargerHW           = EfiBatteryChargerNone;
  ChargerInfo.Version             = 0x00010000;
  BatteryGaugeInfo.BatteryGaugeHW = EfiBatteryGaugeNone;
  BatteryGaugeInfo.Version        = 0x00010000;

  //Read Flag from Config Item to see if we need override the setting
  if (pChargerLibConfig->plat_hw_cfg_override < CHARGERLIB_PLATFORM_HW_CONFIG_INVALID)
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Charger PlatHWConfig = %d, (overrided by CFG item)\r\n", 
                   __FUNCTION__, pChargerLibConfig->plat_hw_cfg_override));

    config_index = pChargerLibConfig->plat_hw_cfg_override;

    ChargerInfo.ChargerHW =           ChargerLibHwConfigTable[config_index].charger_hw_type;
    ChargerInfo.Version =             ChargerLibHwConfigTable[config_index].charger_hw_version;
    BatteryGaugeInfo.BatteryGaugeHW = ChargerLibHwConfigTable[config_index].gauge_hw_type;
    BatteryGaugeInfo.Version        = ChargerLibHwConfigTable[config_index].gauge_hw_version;
  }
  else
  {
    //Assign HWInfo Type based on the table
    for (plat_index = 0; plat_index < HwInfoSize; plat_index++)
    {
      if (ChargerLibPlatformHwInfo[plat_index].PlatformType == PlatformType)
      {
        //Execute HW Config Detect Function if available
        if (ChargerLibPlatformHwInfo[plat_index].pHwConfigDetectFunc != NULL)
        {
          Status = ChargerLibPlatformHwInfo[plat_index].pHwConfigDetectFunc(&ChargerInfo, &BatteryGaugeInfo);
          if (EFI_SUCCESS != Status)
          {
              CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a ERROR: Platform HW detection function failed %r \r\n", 
                   __FUNCTION__, Status));
          }
        }
        //Else, perform static assignment
        else
        {
          config_index = ChargerLibPlatformHwInfo[plat_index].StaticHwConfigIndex;
          if (config_index < CHARGERLIB_PLATFORM_HW_CONFIG_INVALID)
          {
            ChargerInfo.ChargerHW =           ChargerLibHwConfigTable[config_index].charger_hw_type;
            ChargerInfo.Version =             ChargerLibHwConfigTable[config_index].charger_hw_version;
            BatteryGaugeInfo.BatteryGaugeHW = ChargerLibHwConfigTable[config_index].gauge_hw_type;
            BatteryGaugeInfo.Version        = ChargerLibHwConfigTable[config_index].gauge_hw_version;
            CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a PlatformType = %d has Static PlatHWConfig = %d \r\n", 
                           __FUNCTION__, PlatformType, config_index));
          }
          else
          {
            CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a ERROR: Invalid Static Platform HW assignment \r\n", 
                           __FUNCTION__, Status));
          }
        }
        break;
      }
    }
  }

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a PlatformType = %d: ChargerHW = %d, GaugeHW = %d \r\n", 
                 __FUNCTION__, PlatformType, ChargerInfo.ChargerHW, BatteryGaugeInfo.BatteryGaugeHW));

  return;
}

EFI_STATUS ChargerLib_IsChargingSupported(BOOLEAN *pChargingSupported)
{
  EFI_STATUS Status = EFI_SUCCESS;
  //Checking Configuration
  if ((EfiBatteryChargerNone == ChargerInfo.ChargerHW) ||      
      (EfiBatteryGaugeNone == BatteryGaugeInfo.BatteryGaugeHW))
  {
    *pChargingSupported = FALSE;
  }
  else
  {
    *pChargingSupported = TRUE;
  }
  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a PlatformType = %d: ChargerHW = %d, GaugeHW = %d \r\n", 
                 __FUNCTION__, PlatformType, ChargerInfo.ChargerHW, BatteryGaugeInfo.BatteryGaugeHW));

  //Add HW Validation here in the future

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Support Charging = %d\r\n", __FUNCTION__, *pChargingSupported));
  return Status;
}

EFI_STATUS ChargerLib_GetChargerPresence(BOOLEAN *pChargerPresent)
{
  EFI_STATUS Status = EFI_SUCCESS;
  BOOLEAN    DcInValid = FALSE;
  EFI_PM_SCHG_USB_CONNECTOR_TYPE  UsbConnectorType;
  EFI_PM_USB_TYPEC_PORT_STATUS TypeCStatus;

  if (!PmicUsbProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomPmicUsbProtocolGuid,
                                  NULL,
                                 (VOID **)&PmicUsbProtocol );

    if((EFI_SUCCESS != Status) || (NULL == PmicUsbProtocol))
    {
      CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a PmicUsbProtocol Failed = %r \r\n", __FUNCTION__, Status));
      return EFI_DEVICE_ERROR;
    }
  }
 
  Status = PmicUsbProtocol->UsbinValid(pChargerPresent);
  if ((TRUE == *pChargerPresent) && (EFI_SUCCESS == Status))
  {
    if (!PmicSchgProtocol)
    {
      Status = gBS->LocateProtocol(&gQcomPmicSchgProtocolGuid,
                                  NULL,
                                 (VOID **)&PmicSchgProtocol);
 
      if((Status != EFI_SUCCESS) || (NULL == PmicSchgProtocol ))
      {
        CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a pmicSchgProtocol Failed = %r \r\n", __FUNCTION__, Status));
        return EFI_DEVICE_ERROR;
      }
    }
   
    Status = PmicSchgProtocol->GetChargerConnectorType(ChargerInfo.ChargerPmicInfo.PmicIndex, &UsbConnectorType);
    if((UsbConnectorType == EFI_PM_SCHG_USB_CONNECTOR_TYPE_TYPEC) && (EFI_SUCCESS == Status))
    {
      Status = PmicUsbProtocol->UsbCGetPortState(&TypeCStatus);

      if( (FALSE == TypeCStatus.ccout_out_hiz_sts) && (EFI_SUCCESS == Status))
      {
        //Print Warning Message if Type-C CCOUT is Hi-Z (FALSE) but Vbus is valid
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a WARN: Invalid CC Status. ccout_out_hiz_sts = %d \n\r", __FUNCTION__, TypeCStatus.ccout_out_hiz_sts));

	    if (gChargerLibCfgData.cc_float_charging == FALSE)
	    {
		    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a WARN: cc_float_charging is FALSE, shutdown \n\r", __FUNCTION__));
		   
		    *pChargerPresent = FALSE;
	    }
	    else
	    {
	 	   CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a WARN: cc_float_charging is TRUE. Allow charging with 500mA ICL \n\r", __FUNCTION__));				  
	    }
      }
    } //PM_SCHG_USB_CONNECTOR_TYPE_TYPEC

  } //pChargerPresent
  
  Status |= ChargeLib_IsDcinValid(&DcInValid);
 
  if (EFI_SUCCESS == Status)
  {
    (*pChargerPresent) |= DcInValid;
  }  
  return Status;
}

EFI_STATUS ChargerLib_ContinueCharging( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  CHARGERLIB_ATTACHED_CHGR_TYPE ChargePath = CHARGERLIB_ATTACHED_CHGR__NONE;

  //If USB Charging is active, continuously Checking Port Status and configure ICL correctly
  Status = ChargerLib_GetChargingPath(&ChargePath);
  if (Status != EFI_SUCCESS)
  {
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib::%a ERROR Get Charging Path. %r\n",__FUNCTION__, Status));
    return Status;
  }

  if (ChargePath == CHARGERLIB_ATTACHED_CHGR__USB)
  {
    Status = ChargerLib_ConfigureUsbCurrentSetting();
    if(Status != EFI_SUCCESS)
    {
      CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib::%a ERROR Configure USB Charging Current setting. %r\n",__FUNCTION__, Status));
    }
  }

  return Status;
}

/**
  Enable BMD

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_EnableBMD( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!PmicFgProtocol)
  {
    Status = gBS->LocateProtocol(&gQcomPmicFgProtocolGuid, NULL, (VOID **)&PmicFgProtocol);
  }
  
  if((NULL == PmicFgProtocol ) || (Status != EFI_SUCCESS))
    return EFI_DEVICE_ERROR;

  Status |= PmicFgProtocol->FgEnableBMD(ChargerInfo.ChargerPmicInfo.PmicIndex);

  return Status;
}