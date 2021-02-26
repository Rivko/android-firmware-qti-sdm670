  /*! @file PmicFgSram.c
 *
 * PMIC Battery Profile SRAM/OTP access functionalities 
 * FG OTP Starts at address 0x0.
 * 256x24 (3 banks of 256x8) byte addressable within a 32 bit word.
 * MSByte of each 32 bit word is invalid and will readback 0x00 and will not be programmed.
 * 
 * FG RAM Starts at address 0x000.
 * 128x32 byte addressable.
 *
 * Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/27/17   pbitra    Fixing Array bound for SRAM data
 07/03/16   va        Do One time configuration on all boot, dvdd reset only on 20th addr data non zero
 06/15/17   va        Detect the bad conditions of FG SRAM (trim registers) and issue VDD hard reset on PM/PMi to recover
 06/15/17   va        Decoupling sram one time configs from profile load
 04/21/16   cs        fixed fail to detecting profile loaded when profile loaded by HLOS and FG reset in uefi case
 04/24/17   va        Fixing DMA profile loading input parameter check 
 03/01/17   va        CC_SOC saturation auto-clear
 01/31/17   va        Adding DMA access for profile load
 01/31/17   va        Adding Rconn configuration for considering compensation
 01/30/17   sm        Updating review comments for updating profile integrity byte
 01/23/17   sm        CFG parameter Structure changes
 11/10/16   va        Removing SOC EMPTY IRQ, Config VBtEmpty threshold
 11/07/16   cs        Added API to disable ESR Pulse
 10/26/16   va        IMA clear sequence update, profile int byte changes
 10/05/16   va        IMA exception handling fix
 09/20/16   sv        fixing klocwork issues
 09/20/16   va        Enable Profile loading, PMIC 2.0 changes, Restart FG changes 
 07/26/16   va        Restarting FG Changes on warm boot 
 05/20/16   va        Updating for battery profile loading using IMA
 04/21/16   va        Updating IMA cfg 
 03/22/16   va        Update to make FG Protocol compile for WP and remove not necessary code
 01/27/16   sm        Added APIs to Configure HW JEITA
 11/20/15   va        Rslow clear sequnce on boot up 
 10/23/15   va        Time stamp for UART logging
 09/25/15   al        Fixing the wait and loop time
 09/09/15   al        Change the loop increment from 100 to 10
 06/04/15   va        Make sure to set Low latency bit false after releasing memory access 
 05/29/15   va        Read Battery Temperature in Fg Init
 05/19/15   va        Fuel Gauge HW workaround for loading battery profile correctly
 05/14/15   al        Replace saved RTC with current time with every boot
 04/14/15   al        Adding change to restart FG is shutdown for more than 24hrs
 04/14/15   va        Disabling Rslow Charg Sequnce for SOC not incresing issue
 04/14/15   va        Restart FG when VBatShadow > VEmpty and Empty SOC INTR is fired
 03/31/15   va        Rslow workaround Changes
 03/13/15   al        Adding changes for optimization
 02/20/15   al        Adding API to read battery ID
 01/26/15   sm        Replaced ADDRESS with ADDR and REGISTER by REG
 01/13/15   va        Correcting VBAT scaling factor
 01/05/15   va        Added Multiple Battery Profile and Profile Parser Status API, Conditional restart workaround encoding fix
 12/17/14   al        Adding SW WA for negative current patching
 12/02/14   va        Conditional restart workaround Changes, SRAM dump log print fix
 10/27/14   va        Update for New Battery Profile Format
 10/06/14   va        Release FG Sram access during UEFI exit
 09/02/14   va        Enable File Logging 
 08/27/14   va        KW Erros Fix.
 05/19/14   va        New file.
 01/19/15   al        Moving helper function to header file
 01/13/15   va        Correcting VBAT scaling factor
 01/05/15   va        Added Multiple Battery Profile and Profile Parser Status API, Conditional restart workaround encoding fix
 12/17/14   al        Adding SW WA for negative current patching
 12/02/14   va        Conditional restart workaround Changes, SRAM dump log print fix
 10/27/14   va        Update for New Battery Profile Format
 10/06/14   va        Release FG Sram access during UEFI exit
 09/02/14   va        Enable File Logging 
 08/27/14   va        KW Erros Fix.
 05/19/14   va        New file.
=============================================================================*/

/**
  PMIC Platform interface
 */

#include <Protocol/EFIPlatformInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include "DDIPlatformInfo.h"

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include "string.h"
#include <Library/QcomTargetLib.h>

/** 
File Log Dependencies 
*/ 
#include "PmicFileLog.h"


/**
  SChg interface
 */
#include "EFIPmicSchg.h"

/**
  FG interface
 */
#include "PmicFgSram.h"
#include "pm_fg_batt_soc.h"
#include "pm_fg_batt_info.h"
#include "pm_fg_mem_if.h"
#include "pm_schg_chgr.h"

#include "pm_rtc.h"
#include "pm_uefi.h"
#include "pm_version.h"
/**
  ADC Dependencies
 */
#include <Protocol/EFIAdc.h>

#include "pm_app_pon.h"
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

/*===========================================================================
                               MACROS
===========================================================================*/

#define FG_SRAM_ADDR_INCREMENT 1
#define FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME 30 //in us
#define FG_MEM_AVAILABLE_RT_STS_POLL_MAX_TIME 9990 // in us (~10 ms)

#define FG_SRAM_PROFILE_RANGE 0x7F
#define FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR    0x4F //refer MDOS for address, data values
#define FG_SRAM_PROFILE_INTEGRITY_OFFSET      0x3
#define FG_SRAM_PROFILE_INTEGRITY_ADDR_RCONN_OFFSET 0x0
#define FG_SRAM_PROFILE_INTEGRITY_RCONN_MASK   0xFF

//#define FG_SRAM_PROFILE_INTEGRITY_OFFSET_MASK 0x01
#define FG_SRAM_PROFILE_INTEGRITY_UEFI_ENABLE      0x03
#define FG_SRAM_PROFILE_INTEGRITY_RESTART_ENABLE   0x04
#define FG_SRAM_PROFILE_INTEGRITY_RESTART_DISABLE  0x00
#define FG_SRAM_PROFILE_INTEGRITY_RESTART_MASK     0x04


//FG Conditional restart register Address
#define FG_SRAM_V_CURR_PREDICTED 0x61 /* Dec 97 */ //0x540 //Offset is 0 and 2 byte size
#define FG_SRAM_VOLTAGE_SHADOW   0x58 /* Dec 88 */ //0x5CC //Offset is 1 and 2 byte size

//Encoding for voltage 5V - 1 LSB (LSB = 5 / 0x7FFF)
#define FG_SRAM_VOLTAGE_ENCODING 152588 // 5 div 2 to powr 15 in mv
#define VOLTAGE_ENCODING_SCALE_MV 1000000

#define FG_SRAM_VOLTAGE_MAX 8000         //8 mV
#define FG_SRAM_VOLTAGE_MAX_ENCODING_VALUE 65535 

#define FG_SRAM_ESR_MAX_IN_OHMS 4000         //8 Ohms

#define FG_SRAM_VOLTAGE_MAX_ENCODING_15BIT_VALUE 32767
#define FG_SRAM_OHMS_MAX_ENCODING_14BIT_VALUE    16383

// BID DETECT Value at start address 0x594 Byte Offset is 1 
// Followed by BID DETECT ADDITIONAL Byte offset 3

#define FG_SRAM_SCRATCH_REG_RTC   0x484 
#define SECS_IN_HOUR              (60*60)
#define MAX_FG_RESTART_24HOUR     24

//#define FG_SRAM_HW_JEITA_ADDR     0x454 

#define FG_SRAM_BID_ENCODING_CONST 9765 //(5/ 2^9 * 1000000)  is in uA

#define PM_FG_POLL_LACTHED_IRQ_TIME_OUT       1600         /* in microsecond - 1.6 S */
#define FG_MEM_LACTCHED_IRQ_STS_POLL_MIN_TIME 30 /* 30 ms */

#define PM_FG_POLL_RT_IRQ_TIME_OUT            10000 /* in microsecond - 10 ms */
#define PM_FG_NO_OF_IMA_RETRIES               3

#define PM_FG_CYCLE_AFFECT_TIME_OUT           40 /* in microsecond */

#define FG_SRAM_VEMPTY_VOLTAGE_MAX            4000 /* 4 mV */
#define FG_SRAM_VEMPTY_VOLTAGE_MAX_ENCODING_VALUE 256 /* 2^ 8*/ 
#define PM_FG_NUM_OF_DMA_ERR_STS_POLL         3
//#define  PM_FG_SHIFT_2_BYTES                  16
#define PM_FG_SHIFT_3_BYTES                   24
#define FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT 5
#define FG_SRAM_PROFILE_VERIFIY_CNT_IN_WORDS 5


#define FG_SRAM_PAUSE_DELAY 40            // in us
#define FG_SRAM_PAUSE_DELAY_MAX_TIME 9990 // in us (~10 ms)

/* K integration cofficicent for Cutoff SoC voltage */
#define FG_SRAM_SP_KICT_ADDR  12
#define FG_SRAM_SP_KICT_ADDR_OFFSET 1
#define FG_SRAM_SP_KICT_VALUE 0x18
#define FG_SRAM_SP_KICT_MASK  0xFF
/* if true for vCutOff, iBtRSlw is used, which is a filtered version, for normal cycles */
#define FG_SRAM_SP_VCTIBTRSLWEN_ADDR  19
#define FG_SRAM_SP_VCTIBTRSLWEN_ADDR_OFFSET 1
#define FG_SRAM_SP_VCTIBTRSLWEN_VALUE 0xC0
#define FG_SRAM_SP_VCTIBTRSLWEN_MASK  0xC0

/* CC_SOC saturation auto-clear “sp.SocCfg.vMCorSatClrEN” = 1 (Addr 19, bit 3 offset 0 ) */
#define FG_SRAM_SP_SAT_CC_CLR_AUTO_ADDR  19
#define FG_SRAM_SP_SAT_CC_CLR_AUTO_ADDR_OFFSET 0
#define FG_SRAM_SP_SAT_CC_CLR_AUTO_VALUE 0x08
#define FG_SRAM_SP_SAT_CC_CLR_AUTO_MASK  0x08

#define FG_SRAM_SP_ESRITRSH_ADDR  2
#define FG_SRAM_SP_ESRITRSH_ADDR_OFFSET 3
#define FG_SRAM_SP_ESRITRSH_VALUE 0x1C
#define FG_SRAM_SP_ESRITRSH_MASK  0xFF

/* esr timer config for dis-charging: maximum value */
#define FG_SRAM_SP_ESRTMRCFGDSC_MAX_ADDR  17
#define FG_SRAM_SP_ESRTMRCFGDSC_MAX_ADDR_OFFSET 0
#define FG_SRAM_SP_ESRTMRCFGDSC_MAX_VALUE 0x60
#define FG_SRAM_SP_ESRTMRCFGDSC_MAX_MASK  0xFF

#define FG_SRAM_ESR_CFG_ADDR                  19
#define FG_SRAM_ESR_CFG_ADDR_OFFSET           0
#define FG_SRAM_ESR_ENABLE_BIT_MASK           1

#define FG_SRAM_VEMPTY_REG_ADDR_OFFSET 3
#define FG_SRAM_VEMPTY_REG_VALUE 0x33 /* 2.8v */
#define FG_SRAM_VEMPTY_REG_MASK  0xFF

#define FG_SRAM_RCONN_COMP_MAX   100
#define FG_SRAM_RCONN_COMP_MIN  -100


#define FG_SRAM_SP_BAT_ESR_ADDR  99 /* 14 bit value offset is 0 */
#define FG_SRAM_BP_DISCHG_RSLW_SOCTH_ADDR  34 /* 8 bit value offset is 0 HALF encoding unsigned */
#define FG_SRAM_BP_DISCHG_RSLW_SOCTH_ADDR_OFFSET 0
#define FG_SRAM_BP_DISCHG_RSLW_SOCTH_REG_MASK  0xFF

#define FG_SRAM_BP_CHG_RSLW_SOCTH_ADDR     51 /* 8 bit value offset is 0 HALF encoding unsigned */
#define FG_SRAM_BP_CHG_RSLW_SOCTH_ADDR_OFFSET 0
#define FG_SRAM_BP_CHG_RSLW_SOCTH_REG_MASK  0xFF

#define FG_SRAM_SP_TRIM_ADDR   0x0 /* First Addr of SRAM */
#define FG_SRAM_SP_TRIM_ZEROED 0x0  /* If Trim is zeroed */

#define FG_SRAM_SP_UN_USED_ADDR   20 /* Un used by SW and HW address of SRAM */
#define FG_SRAM_SP_UN_USED_ZERO   0x0  /* If reg is zeroed */
#define FG_SRAM_SP_RCONNCOMP_BIT_VALUE 0x01

#define FG_SRAM_ENUM_VBAT_EMPTY_OFFSET 2000 /* in mv */
#define FG_SRAM_ENUM_VBAT_EMPTY_MAX    4000 /* in mv */

#define BIT(x) 1 << x

#define FG_SRAM_PROFILE_INTEGRITY_BIT_POS              BIT(0)
#define FG_SRAM_UEFI_LOAD_BIT_POS                      BIT(1)
#define FG_SRAM_UEFI_RESTART_BIT_POS                   BIT(2)
#define FG_SRAM_HLOS_LOAD_BIT_POS                      BIT(3)


#define FG_SRAM_PROFILE_INTEGRITY_CLR_VALUE          0x00
#define FG_SRAM_PROFILE_INTEGRITY_VALUE              0x01
#define FG_SRAM_UEFI_LOAD_VALUE                      0x03
#define FG_SRAM_UEFI_RESTART_VALUE                   0x07
#define FG_SRAM_HLOS_LOAD_VALUE                      0x09
//HLOS will write 0x0F, but we want to ignore bit 2 (0x04) which only gets set when uefi starts profile loading
#define FG_SRAM_HLOS_LOAD_BATTERY_SWAP               0x0B 
#define FG_SRAM_UEFI_PROFILE_REG_MASK                0xFF

//do not look at bit 2, that is for restart FG.
#define FG_SRAM_PROFILE_LOAD_BIT_MASK                0xFB 
#define FG_SRAM_BATTERY_PROFILE_MAX_SIZE_IN_BYTES 0xFF /* 256 bytes current only 220 bytes are used */

#define FOREVER for(;;)

/*=========================================================================
                            GLOBAL VARIABLES
===========================================================================*/
STATIC FgSramState SramState;
STATIC EFI_PM_FG_CFGDATA_TYPE gFgConfigData;
extern FgBattProfile FgProfile;
/*===========================================================================
                               TYPE DEFINITIONS
===========================================================================*/



/*==========================================================================
                        LOCAL API PROTOTYPES
===========================================================================*/
#ifdef _MSC_VER
#pragma warning( disable : 4244)
#endif
EFI_STATUS PmicFgSram_SetState(FgSramState FgSramSt);

EFI_STATUS PmicFgSram_RequestFgSramAccess(UINT32  PmicDeviceIndex, 
                                                       pm_fg_mem_if_mem_intf_cfg  mem_intf_cfg);

EFI_STATUS PmicFgSram_WriteProfileToSram(UINT32  PmicDeviceIndex, 
                                                    FgBattAddrData profileArray[BATTARY_PROFILE_MAX_SIZE]);
EFI_STATUS PmicFgSram_ValidateProfileWrite(UINT32 PmicDeviceIndex,
                                                   FgBattAddrData profileArray[BATTARY_PROFILE_MAX_SIZE]);

void       PmicFgSram_PrintState(FgSramState FgSramSt);

/* EFI_STATUS PmicFgSram_OverwriteSavedRtcSoC(UINT32 PmicDeviceIndex, UINT32 *FgRestartHrs, BOOLEAN *RtcExceedRestart); */

/* EFI_STATUS PmicFgSram_ReadBattParamCache(UINT32 PmicDeviceIndex, FgBattParamCache *fgBattParamCache); */

EFI_STATUS PmicFgSram_ImaWriteBurst(UINT32 PmicDeviceIndex, UINT16 StartAddress, UINT8 NoOFWordToWrite,
                                                FgBattAddrData fg_memif_data[BATTARY_PROFILE_MAX_SIZE]);

EFI_STATUS PmicFgSram_DmaWriteBurst(UINT32 PmicDeviceIndex, UINT16 StartAddress, UINT8 NoOFWordToWrite,
                                                FgBattAddrData fg_memif_data[BATTARY_PROFILE_MAX_SIZE]);

pm_err_flag_type PmicFgSram_ResetIma(uint32 PmicDeviceIndex);

EFI_STATUS PmicFgSram_HandleIfImaException(UINT32  PmicDeviceIndex, BOOLEAN *bExceptionOccured);

EFI_STATUS PmicFgSram_PollImaReady(UINT32 PmicDeviceIndex, BOOLEAN *bImaAccessAvailable);

EFI_STATUS PmicFgSram_ImaReadSingle(UINT32 PmicDeviceIndex, UINT16 ReadAddress, UINT32 *fg_memif_data, BOOLEAN *bBeatCountMatch);

EFI_STATUS PmicFgSram_ImaReadCust(UINT32 PmicDeviceIndex, FgSramAddrData *SramAddrData, UINT32 AddrDataCount);

EFI_STATUS PmicFgSram_RestartFg(UINT32 PmicDeviceIndex);

EFI_STATUS PmicFgSram_PollRtIrq(UINT32 PmicDeviceIndex, pm_fg_batt_soc_irq_type irq_type, BOOLEAN *bIrqUpdated);

EFI_STATUS PmicFgSram_PollRisingEdgeIrq(UINT32 PmicDeviceIndex, pm_fg_batt_soc_irq_type irq_type, BOOLEAN *bIrqUpdated);

EFI_STATUS PmicFgSram_ImaReadBurst(UINT32 PmicDeviceIndex, UINT16 ReadAddress, UINT8 noOFWordToRead,
                                                UINT32 fg_memif_data[BATTARY_PROFILE_MAX_SIZE]);

EFI_STATUS PmicFgSram_ImaReadSingleOffset(UINT32 PmicDeviceIndex, UINT16 ReadAddress,
                                               UINT8 *Data, UINT8 Offset);

EFI_STATUS PmicFgSram_ImaWriteSingle(UINT32 PmicDeviceIndex, UINT16 WriteAddress, UINT32 fg_memif_data);

EFI_STATUS PmicFgSram_ImaWriteProfileIntegrity(UINT32 PmicDeviceIndex, UINT16 WriteAddress,
                                               UINT8 Data, UINT8 Offset);

EFI_STATUS PmicFgSram_ImaWriteSingleOffset(UINT32 PmicDeviceIndex, UINT16 WriteAddress,
                                               UINT8 WriteValue, UINT8 Offset, UINT8 mask);

EFI_STATUS PmicFgSram_UpdateRconnComp(UINT32 PmicDeviceIndex);

EFI_STATUS PmicFgSram_VerifySramContent(UINT32 PmicDeviceIndex, BOOLEAN *UnExpectedMatch);

/*==========================================================================
                        GLOBAL API DEFINITION
===========================================================================*/
/**
PmicFgSram_Init()

@brief
Initializes Sram State
*/
EFI_STATUS PmicFgSram_Init(UINT32 PmicDeviceIndex, FgSramState FgSramState, EFI_PM_FG_CFGDATA_TYPE *pFgConfigData)
{
  BOOLEAN bExceptionOccured = FALSE;
  EFI_STATUS Status = EFI_SUCCESS;

  SetMem (&gFgConfigData,  sizeof(EFI_PM_FG_CFGDATA_TYPE), 0x00);
    /* Assign global Config Data */
  CopyMemS(&gFgConfigData, sizeof(EFI_PM_FG_CFGDATA_TYPE), pFgConfigData, sizeof(EFI_PM_FG_CFGDATA_TYPE));

  //PmicFgSram_PrintState(FgSramState);
  if (FG_SRAM_STATUS_INIT == FgSramState || FG_SRAM_STATUS_AVAILABLE == FgSramState ){
    SramState = FgSramState;
  }else{
    SramState = FG_SRAM_STATUS_INIT;
  }

  Status = PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);
  if(bExceptionOccured == TRUE)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Occurred = %d Status = %d \r\n",__FUNCTION__, bExceptionOccured, Status));
  }

  return Status;
}

/**
PmicFgSram_GetState()

@brief
Returns Current Sram State
*/
EFI_STATUS PmicFgSram_GetState(FgSramState *FgSramSt)
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  if ( FG_SRAM_STATUS_INVALID == SramState)
  {
    Status = EFI_DEVICE_ERROR;
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: PmicFgSram_SetState: Error SramState = (%d) \r\n", SramState));
  }
  else{
    *FgSramSt = SramState;
  }

  return Status;
}


/**
PmicFgSram_ConvertVolt()

@brief
Conver SRAM voltage reading 
*/
EFI_STATUS PmicFgSram_ConvertVolt(UINT32 *Data)
{
  UINT32 batt_volt = 0;

  if(!Data)
    return EFI_INVALID_PARAMETER;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe::%a Data = 0x%x \r\n", __FUNCTION__, *Data));

  /* get 0-1 byte as per the MDOS offset 0 and ignoring MSB bit as data is 15 bit */
  batt_volt = (*Data) & 0x7FFF;

  *Data = (FG_SRAM_VOLTAGE_MAX * batt_volt) / FG_SRAM_VOLTAGE_MAX_ENCODING_15BIT_VALUE; //voltage in milliVolt

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe::%a Voltage = %d mv Shifted Data = 0x%x \r\n", __FUNCTION__, *Data, batt_volt ));

  return EFI_SUCCESS;
}


/**
PmicFgSram_ConvertVEmptyIrq()

@brief
   Empty Voltage Conversion
*/
EFI_STATUS PmicFgSram_ConvertVEmptyIrq(UINT32 *Data)
{
  UINT32 VBat = 0;
  UINT32 VoltOFfset = FG_SRAM_ENUM_VBAT_EMPTY_OFFSET; /*2 V = 2000mv*/

  if(!Data)
    return EFI_INVALID_PARAMETER;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Data = 0x%x \r\n", __FUNCTION__, *Data));

  /* get 4th byte as per the MDOS offset 0 */
  VBat = (*Data) >> PM_FG_SHIFT_3_BYTES;

  *Data = VoltOFfset + (VBat * FG_SRAM_VEMPTY_VOLTAGE_MAX )/ FG_SRAM_VEMPTY_VOLTAGE_MAX_ENCODING_VALUE; //Data in milliVolt

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Data = %d mv shitfted Data = 0x%x \r\n", __FUNCTION__, *Data, VBat));

  return EFI_SUCCESS;
}


/**
PmicFgSram_CondRestart()

@brief
Conditional Restart for Fuel Gauge Algorithm 
*/
EFI_STATUS PmicFgSram_CondRestart
(
  UINT32  PmicDeviceIndex, EFI_PM_FG_CFGDATA_TYPE CfgDataType
)
{

  EFI_STATUS        Status    = EFI_SUCCESS;
  pm_err_flag_type  err_flg   = PM_ERR_FLAG_SUCCESS;
  UINT32            VCurrPred = 0, VoltageShadow = 0, VEmpty = 0; 
  INT32             VBattEstDiff = 0;
  FgSramAddrData    SramAddrData[3];

  UINT8 NoOfBytesToRead = 0;

  NoOfBytesToRead = gFgConfigData.FgSramDbgCfg.DumpSramEndAddr - gFgConfigData.FgSramDbgCfg.DumpSramStartAddr;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a VBattEstDiffThreshold %d mv \r\n",__FUNCTION__, CfgDataType.VBattEstDiffThreshold));

  /* DEBUG FG DUMP */
  Status |= PmicFgSram_Dump(PmicDeviceIndex, gFgConfigData.FgSramDbgCfg.DumpSramStartAddr, NoOfBytesToRead);
  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Dump Completed = %r \r\n", __FUNCTION__, Status));

  SetMem(&SramAddrData, sizeof(FgSramAddrData) * sizeof(SramAddrData)/sizeof(FgSramAddrData), 0x00);

  SramAddrData[0].SramAddr = FG_SRAM_V_CURR_PREDICTED;
  SramAddrData[2].SramAddr = FG_SRAM_VEMPTY_IRQ_REG_ADDR;
  SramAddrData[1].SramAddr = FG_SRAM_VOLTAGE_SHADOW;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a VBattEstDiffThreshold %d mv \r\n",__FUNCTION__, CfgDataType.VBattEstDiffThreshold));

  /* Read Required registers from SRAM */
  Status  =  PmicFgSram_ImaReadCust(PmicDeviceIndex, SramAddrData, sizeof(SramAddrData)/sizeof(FgSramAddrData));

  VoltageShadow = SramAddrData[1].SramData;
  VCurrPred     = SramAddrData[0].SramData;
  VEmpty        = SramAddrData[2].SramData;

  PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a VoltageShadow = 0x%x VEmpty = 0x%x VCurrPred = 0x%x \r\n",__FUNCTION__, SramAddrData[1].SramData,
               SramAddrData[2].SramData, VCurrPred, SramAddrData[0].SramData));

  /* Convert predicted voltage value */
  Status  |=  PmicFgSram_ConvertVolt(&VoltageShadow);

  /* Convert Empty IRQ voltage value */
  Status  |=  PmicFgSram_ConvertVEmptyIrq(&VEmpty);

  /* Convert predicted voltage value */
  Status  |=  PmicFgSram_ConvertVolt(&VCurrPred);

  /* Get respective 2 bytes and Convert to number format VCurrPred, VoltageShadow */
  VBattEstDiff = (VCurrPred - VoltageShadow);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a VShadw = %d mv VEmpty = %d mv VPred = %d mv VBattEst = %d mv \r\n",
              __FUNCTION__, VoltageShadow, VEmpty, VCurrPred, VBattEstDiff));

  /* Check if device was shutdown for more than 24hrs TBD or needs discussion */
  /* Status |= PmicFgSram_OverwriteRtcAndDeltaSoC(PmicDeviceIndex, &FgRestartHrs, &RtcExceedRestart); */
  if (ABS(VBattEstDiff) > CfgDataType.VBattEstDiffThreshold /*|| (TRUE == RtcExceedRestart)*/)
  {
    LogMsg("Restarting Battery Fuel Gauge Start ");
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Restarting Fg \r\n",__FUNCTION__));
    Status |= PmicFgSram_RestartFg(PmicDeviceIndex);
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Restarting Fg Completed \r\n", __FUNCTION__));

    Status = PmicFgSram_ImaWriteProfileIntegrity(PmicDeviceIndex, (UINT16)FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR, 
                                             (UINT8) FG_SRAM_PROFILE_INTEGRITY_RESTART_ENABLE, (UINT8)FG_SRAM_PROFILE_INTEGRITY_OFFSET);

    LogMsg("Restarting Battery Fuel Gauge End ");
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Skipped VBattEstDiffThrld = %d mV, VBattEst = %d mv \r\n", __FUNCTION__, CfgDataType.VBattEstDiffThreshold, VBattEstDiff));
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a VCurrPred = %d mv, VoltageShadow = %d mv \r\n", __FUNCTION__,VCurrPred, VoltageShadow));

    Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR,
                                             FG_SRAM_PROFILE_INTEGRITY_RESTART_DISABLE, FG_SRAM_PROFILE_INTEGRITY_OFFSET,
                                             FG_SRAM_PROFILE_INTEGRITY_RESTART_MASK);
  }

  return (Status | err_flg);
}


EFI_STATUS PmicFgSram_RestartFg(UINT32 PmicDeviceIndex)
{
  EFI_STATUS        Status  = EFI_SUCCESS;
  pm_err_flag_type  err_flg = PM_ERR_FLAG_SUCCESS;
  BOOLEAN           bIrqUpdated = FALSE, bExceptionOccured = FALSE;

  Status |= PmicFgSram_PollRtIrq(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY, &bIrqUpdated);
  if((EFI_SUCCESS == Status ) && (bIrqUpdated == TRUE))
  {

    pm_fg_batt_soc_irq_set_trigger(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY, PM_IRQ_TRIGGER_RISING_EDGE);
    pm_fg_batt_soc_irq_enable(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY, TRUE);
    pm_fg_batt_soc_irq_clear(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY);

    /* Issue SOC RESTART */
    err_flg |= pm_fg_batt_soc_restart(PmicDeviceIndex, TRUE);
    /* Wait for SOC cycle */
    gBS->Stall(PM_FG_CYCLE_AFFECT_TIME_OUT);
    /* Wait for SOC_READY IRQ */

    Status |= PmicFgSram_PollRisingEdgeIrq(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY, &bIrqUpdated);

    /* Disable SOC RESTART */
    err_flg |= pm_fg_batt_soc_restart(PmicDeviceIndex, FALSE);

    Status |= PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
    if(TRUE == bExceptionOccured)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry to Single Read Again here \r\n", __FUNCTION__));
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Restarting FG failed = %r \r\n", __FUNCTION__, Status));
    }
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a SOC not ready Error during rstarting FG \r\n", __FUNCTION__));
  }

  return (Status | err_flg);
}


EFI_STATUS PmicFgSram_ReadBattID( AdcRr_BattId *Batt_Id)
{

  EFI_STATUS              Status      = EFI_SUCCESS;
  EfiAdcDeviceChannelType channel;
  EfiAdcResultType        AdcResult   = {0};
  EFI_ADC_PROTOCOL       *pAdcProtocol = NULL;

  if(NULL == Batt_Id)
  {
    return EFI_INVALID_PARAMETER;
  }

  if(!pAdcProtocol)
  {
    Status = gBS->LocateProtocol( &gEfiAdcProtocolGuid,
                                  NULL,
                                  (VOID**) &pAdcProtocol);
  }

  if ((EFI_ERROR(Status)) || (NULL == pAdcProtocol))
  {
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a: Failed to load ADC Protocol! Status = %r \r\n", __FUNCTION__, Status));
    return EFI_DEVICE_ERROR;
  }

  Status = pAdcProtocol->GetChannel( ADC_INPUT_BATT_ID_OHMS, sizeof(ADC_INPUT_BATT_ID_OHMS), &channel );
  if (EFI_ERROR(Status))
  {
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a: Failed to get ADC Channel for BATT_ID! Status = %r \r\n", __FUNCTION__, Status));
    return EFI_DEVICE_ERROR;
  }

  Status = pAdcProtocol->AdcRead( &channel, &AdcResult );
  if ((EFI_ERROR(Status)) || (EFI_ADC_RESULT_INVALID == AdcResult.eStatus))
  {
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a: Failed to read ADC Channel - BATT_ID! Status = %r \r\n", __FUNCTION__, Status));
    return EFI_DEVICE_ERROR;
  }

  Batt_Id->Bid = AdcResult.nPhysical;
  PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a: BATT_ID_2 = %d \r\n", __FUNCTION__, Batt_Id->Bid));

  return Status;
}


/**
PmicFgSram_PollLatchedIrq()

@brief
Poll Given IRQ with time out 
*/
EFI_STATUS PmicFgSram_PollLatchedIrq(UINT32 PmicDeviceIndex, pm_fg_batt_soc_irq_type irq_type, BOOLEAN *bIrqUpdated)
{
  
  EFI_STATUS        Status  = EFI_SUCCESS;
  pm_err_flag_type  err_flg = PM_ERR_FLAG_SUCCESS;
  UINT32            Ttl_spent_time_in_polling = 0;
  BOOLEAN           bIrqUpdateStatus = FALSE;

  if(NULL == bIrqUpdated || irq_type >= PM_IRQ_STATUS_INVALID)
    return EFI_INVALID_PARAMETER;

  *bIrqUpdated = FALSE;

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_POLLING);

  pm_fg_batt_soc_irq_set_trigger(PmicDeviceIndex, irq_type, PM_IRQ_TRIGGER_RISING_EDGE);
  pm_fg_batt_soc_irq_enable(PmicDeviceIndex, irq_type, TRUE);

  //Poll PM_FG_BATT_SOC_SOC_UPDT = 1 during FG cycle 1.5s
  do{
      /* mem available best time is 30ms */
      err_flg |= pm_fg_batt_soc_irq_status(PmicDeviceIndex, irq_type, PM_IRQ_STATUS_LATCHED, &bIrqUpdateStatus);
      if(err_flg == PM_ERR_FLAG_SUCCESS && TRUE == bIrqUpdateStatus)
      {
      
        pm_fg_batt_soc_irq_clear(PmicDeviceIndex, irq_type);
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a SOC_UPDT IRQ ready = %d \r\n",__FUNCTION__, err_flg));
        break;
      }
      else
      {
         PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a err_flg= %d SOC_UPDT IRQ Not Ready = %d \r\n",__FUNCTION__, err_flg, bIrqUpdateStatus));
      }
      if (TRUE == bIrqUpdateStatus)
      {
        *bIrqUpdated = TRUE;
        //PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: %a PM_FG_MEM_IF_IMA_RDY \r\n", __FUNCTION__));
        break;
      }//check for error condition as we do not want to loop forever
      else if (Ttl_spent_time_in_polling >= PM_FG_POLL_LACTHED_IRQ_TIME_OUT)
      {
        Status = EFI_DEVICE_ERROR;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a TimeOut : Ttl_spent_time_in_polling = %d ms \r\n", __FUNCTION__, Ttl_spent_time_in_polling));
        break;
      }

      /* Wait for 30 ms before querying mem available status again */
      gBS->Stall(FG_MEM_LACTCHED_IRQ_STS_POLL_MIN_TIME * 1000);

      Ttl_spent_time_in_polling += FG_MEM_LACTCHED_IRQ_STS_POLL_MIN_TIME;
  }while(TRUE);

  /* Disable latched Interrupt*/
  pm_fg_batt_soc_irq_set_trigger(PmicDeviceIndex, irq_type, PM_IRQ_TRIGGER_ACTIVE_LOW);
  pm_fg_batt_soc_irq_enable(PmicDeviceIndex, irq_type, FALSE);
  pm_fg_batt_soc_irq_clear(PmicDeviceIndex, irq_type);

  return (Status | err_flg);
}


/**
PmicFgSram_PollRtIrq()

@brief
Poll Given IRQ with time out 
*/
EFI_STATUS PmicFgSram_PollRisingEdgeIrq(UINT32 PmicDeviceIndex, pm_fg_batt_soc_irq_type irq_type, BOOLEAN *bIrqUpdated)
{
  
  EFI_STATUS        Status  = EFI_SUCCESS;
  pm_err_flag_type  err_flg = PM_ERR_FLAG_SUCCESS;
  UINT32            Ttl_spent_time_in_polling = 0;
  BOOLEAN           bIrqUpdateStatus = FALSE;

  if(NULL == bIrqUpdated || irq_type >= PM_IRQ_STATUS_INVALID)
    return EFI_INVALID_PARAMETER;

  *bIrqUpdated = FALSE;

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_POLLING);

  //Poll FG_MEM_AVAIL_RT_STS = 1 
  do{
      /* mem available best time is 30ms */
      //err_flg |= pm_fg_batt_soc_irq_status(PmicDeviceIndex, irq_type, PM_IRQ_STATUS_RT, &bIrqUpdateStatus);
      err_flg |=  pm_fg_batt_soc_irq_status(PmicDeviceIndex, irq_type, PM_IRQ_STATUS_LATCHED, &bIrqUpdateStatus);
      if(err_flg != PM_ERR_FLAG_SUCCESS)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a pm_fg_batt_soc_irq_status Error= %d \r\n", __FUNCTION__,err_flg));
        break;
      }
      if (TRUE == bIrqUpdateStatus)
      {
        *bIrqUpdated = TRUE;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a irq_type = %d RDY \r\n", __FUNCTION__, irq_type));
        break;
      }//check for error condition as we do not want to loop forever
      else if (Ttl_spent_time_in_polling >= PM_FG_POLL_RT_IRQ_TIME_OUT)
      {
        Status = EFI_DEVICE_ERROR;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a TimeOut : Ttl_spent_time_in_polling = %d ms \r\n", __FUNCTION__,Ttl_spent_time_in_polling));
        break;
      }else{/* InValid use case */}
      /* DEBUG PRINT during restart FG */
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a waiting for irq_type %d = %d ms \r\n", __FUNCTION__,irq_type, Ttl_spent_time_in_polling));

      /* Wait for 30 us before querying mem available status again */
      gBS->Stall(FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME * 1000);

      Ttl_spent_time_in_polling += FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME;
  }while(TRUE);

  /* Disable latched Interrupt*/
  pm_fg_batt_soc_irq_set_trigger(PmicDeviceIndex, irq_type, PM_IRQ_TRIGGER_ACTIVE_LOW);
  pm_fg_batt_soc_irq_enable(PmicDeviceIndex, irq_type, FALSE);
  pm_fg_batt_soc_irq_clear(PmicDeviceIndex, irq_type);

  return (Status | err_flg);
}


/**
PmicFgSram_PollRtIrq()

@brief
Poll Given IRQ with time out 
*/
EFI_STATUS PmicFgSram_PollRtIrq(UINT32 PmicDeviceIndex, pm_fg_batt_soc_irq_type irq_type, BOOLEAN *bIrqUpdated)
{
  
  EFI_STATUS        Status  = EFI_SUCCESS;
  pm_err_flag_type  err_flg = PM_ERR_FLAG_SUCCESS;
  UINT32            Ttl_spent_time_in_polling = 0;
  BOOLEAN           bIrqUpdateStatus = FALSE;

  if(NULL == bIrqUpdated || irq_type >= PM_IRQ_STATUS_INVALID)
    return EFI_INVALID_PARAMETER;

  *bIrqUpdated = FALSE;

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_POLLING);

  //Poll FG_MEM_AVAIL_RT_STS = 1 
  do{
      /* mem available best time is 30us */
      err_flg |= pm_fg_batt_soc_irq_status(PmicDeviceIndex, irq_type, PM_IRQ_STATUS_RT, &bIrqUpdateStatus);
      if(err_flg != PM_ERR_FLAG_SUCCESS)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a pm_fg_batt_soc_irq_status Error= %d \r\n", __FUNCTION__,err_flg));
        break;
      }
      if (TRUE == bIrqUpdateStatus)
      {
        *bIrqUpdated = TRUE;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a irq_type = %d RDY \r\n", __FUNCTION__, irq_type));
        break;
      }//check for error condition as we do not want to loop forever
      else if (Ttl_spent_time_in_polling >= PM_FG_POLL_RT_IRQ_TIME_OUT)
      {
        Status = EFI_DEVICE_ERROR;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a TimeOut : Ttl_spent_time_in_polling = %d ms \r\n", __FUNCTION__,Ttl_spent_time_in_polling));
        break;
      }
      /* DEBUG PRINT during restart FG */
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a waiting for irq_type %d = %d ms \r\n", __FUNCTION__,irq_type, Ttl_spent_time_in_polling));

      /* Wait for 30 us before querying mem available status again */
      gBS->Stall(FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME);

      Ttl_spent_time_in_polling += FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME;
  }while(TRUE);

  return (Status | err_flg);
}


EFI_STATUS PmicFgSram_EnableCharging(UINT32 PmicDeviceIndex, BOOLEAN bSet)
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  STATIC EFI_QCOM_PMIC_SCHG_PROTOCOL  *PmicSchgProtocol = NULL;
  /* Locate SCHG Protocol */
  if (!PmicSchgProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomPmicSchgProtocolGuid,
                                  NULL,
                                 (VOID **)&PmicSchgProtocol );
  }
  
  if(EFI_SUCCESS == Status )
  {/* Initialize SCHG Protocol */
  
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a bSet = %d Enable/Disable \r\n", __FUNCTION__, bSet));
    Status |= PmicSchgProtocol->EnableCharger(PM_DEVICE_1, bSet);
  }

  return Status;
}

/**
PmicFgSram_ImaWriteProfileIntegrity()

@brief
Write Profile Integrity Offset byte from given Sram Address
*/
EFI_STATUS PmicFgSram_ImaWriteProfileIntegrity(UINT32 PmicDeviceIndex, UINT16 WriteAddress,
                                               UINT8 Data, UINT8 Offset)
{
  EFI_STATUS Status         = EFI_SUCCESS;
  UINT8      ProfileIntByte = 0;

    Status = PmicFgSram_ImaReadSingleOffset(PmicDeviceIndex, (UINT16)WriteAddress,
                                                   &ProfileIntByte, (UINT8)Offset);

    /* 1 bit profile integrity bit which indicate valid profile is present 
       2 bit UEFI loaded profile 
       3 bit UEFI restarted FG 
       4 bit HLOS loaded profile */
    if(ProfileIntByte > 0x0F )
    {
      /* Not valid value so UEFI overwriting Int byte valuce since UEFI loaded profile */
      //Data = Data; /* 0x03 or 0x07*/
    }
    else
    {
      /* UEFI Loaded profile */
      Data = (Data | ProfileIntByte);/* 0x03 or Data (0x04) , 0x04 or data (0x07) */
    }
    /* Write byte Now */
    Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, WriteAddress,
                                             Data, Offset, FG_SRAM_UEFI_PROFILE_REG_MASK);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ProfileIntByte read = 0x%x Wrote = 0x%x \r\n", __FUNCTION__, ProfileIntByte, Data));

  return (Status);
}


/**
PmicFgSram_ImaWriteSingleOffset()

@brief
Write 1 Offset byte from given Sram Address
*/
EFI_STATUS PmicFgSram_ImaWriteSingleOffset(UINT32 PmicDeviceIndex, UINT16 WriteAddress,
                                               UINT8 WriteValue, UINT8 Offset, UINT8 mask)
{
  EFI_STATUS       Status    = EFI_SUCCESS;
  UINT8            ReadData8 = 0;
  UINT32           ReadData  = 0;
  UINT32           WriteData = 0;

  Status = PmicFgSram_ImaRead(PmicDeviceIndex, WriteAddress, &ReadData);

  /* Status = PmicFgSram_ImaReadSingleOffset(PmicDeviceIndex, WriteAddress,
                                                 &ReadData8, Offset);*/

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a WriteValue = 0x%x, Offset = 0x%x, mask= 0x%x ReadData = 0x%x \r\n", __FUNCTION__, WriteValue, Offset, mask, ReadData));

    /* get required byte */
    ReadData8 = (UINT8) (ReadData >> (Offset * NUM_BITS_IN_BYTE));

    if(mask != 0xFF) /* If mask is 0xFF then just write the value */
    {
      WriteValue = ((ReadData8 & (~mask) ) | WriteValue);
    }

    ReadData = ~(0xFF << (Offset * NUM_BITS_IN_BYTE)) & ReadData;

    WriteData = (WriteValue << (Offset * NUM_BITS_IN_BYTE)) | ReadData;

    /* Write Now */
    Status = PmicFgSram_ImaWriteSingle(PmicDeviceIndex, WriteAddress, WriteData);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a WriteData = 0x%x WriteValue = 0x%x ReadData8 = 0x%x\r\n", __FUNCTION__, WriteData, WriteValue, ReadData8));

  return ( Status);
}


EFI_STATUS PmicFgSram_ImaRead(UINT32 PmicDeviceIndex, UINT16 ReadAddress, UINT32 *fg_memif_data)
{
  EFI_STATUS Status          = EFI_SUCCESS;
  UINT32     ReadData        = 0;
  BOOLEAN    bBeatCountMatch = FALSE;
  UINT8      NoOfImaRetries  = 0;

  if(!fg_memif_data || ReadAddress > FG_SRAM_END_ADDR )
  {
    return EFI_INVALID_PARAMETER;
  }

  do{
    /* Read Word*/
    Status = PmicFgSram_ImaReadSingle(PmicDeviceIndex, ReadAddress, &ReadData, &bBeatCountMatch);
    if(Status != EFI_SUCCESS)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Single Ima Read Failed = %r \r\n", __FUNCTION__, Status));
      break;
    }

    if (TRUE == bBeatCountMatch)
    {
      /* Read is good and in single FG cycle */
      break;
    }
    else
    {
      NoOfImaRetries++; /* Upkeep counter */
      ReadData = 0; /* Reset Input */
      bBeatCountMatch = FALSE; /* Reset Input */
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a FG Beat Count not match Retry IMA Rd  = %d \r\n", __FUNCTION__, NoOfImaRetries));
      /* continue; not needed but intend to continue */
     }
  }while(NoOfImaRetries < PM_FG_NO_OF_IMA_RETRIES);

  if((NoOfImaRetries > PM_FG_NO_OF_IMA_RETRIES ) || (bBeatCountMatch == FALSE))
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a FG Beat Count not match Read Failure = %d \r\n", __FUNCTION__));
    Status = EFI_DEVICE_ERROR;
  }
  else
  {
    /* Read is good and Assign Read Data */
    *fg_memif_data = ReadData;
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Single Ima ReadData = 0x%x NoOfImaRetries = %d \r\n", __FUNCTION__, ReadData, NoOfImaRetries));
  }

  return Status;

}

/**
PmicFgSram_ImaReadSingleOffset()

@brief
Read 1 Offset byte from given Sram Address
*/
EFI_STATUS PmicFgSram_ImaReadSingleOffset(UINT32 PmicDeviceIndex, UINT16 ReadAddress,
                                               UINT8 *Data, UINT8 Offset)
{
  EFI_STATUS Status   = PM_ERR_FLAG_SUCCESS;
  UINT32     ReadData = 0, mask = 0, TempData = 0;

  if(!Data || ReadAddress > FG_SRAM_END_ADDR || Offset > FG_SRAM_RD_WR_OFFSET_WIDTH)
  {
    return EFI_INVALID_PARAMETER;
  }

  mask = 0xFF << (Offset * NUM_BITS_IN_BYTE);

  Status = PmicFgSram_ImaRead(PmicDeviceIndex, ReadAddress, &ReadData);
  if(Status != EFI_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Single Ima Read Failed = %r \r\n", __FUNCTION__, Status));
  }
  else
  {
    TempData = (ReadData & mask) >> (Offset * NUM_BITS_IN_BYTE);
    /* Read is good and Assign Read Data */
    *Data = (UINT8) TempData;

    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a TempData = 0x%x Mask = 0x%x ReadData = 0x%x \r\n", __FUNCTION__, TempData, mask, ReadData));

    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Single Ima ReadAddress = 0x%x ReadData = 0x%x Offset Data = 0x%x Offset = %d \r\n", __FUNCTION__, ReadAddress, ReadData, *Data, Offset));
  }

  return Status;
}


/**
PmicFgSram_VerifyProfileMatch()

@brief
Match Profile said bytes with SRAM contents
*/
EFI_STATUS PmicFgSram_VerifyProfileMatch(
  UINT32 PmicDeviceIndex, AdcRr_BattId Battery_Id,
  OUT BOOLEAN *bProfileMatch)
{
  EFI_STATUS   Status       = EFI_SUCCESS;
  UINT32       ProfileIndex = 0;
  UINT32       SramRdProfileArray[BATTARY_PROFILE_MAX_SIZE];
  FgBattAddrData *SingleprofileArray = NULL;;
  UINT32       ChkCounter   = 0, MemCmpCnt = 0;

  if(!bProfileMatch)
  {
    return EFI_INVALID_PARAMETER;
  }

  /* Set default value */
  *bProfileMatch = TRUE;

  /* Read Battery ID from ADC and compare it with profile Battery ID */
  Status |= PmicFgSram_ReadBattID(&Battery_Id);
  if(Status == EFI_SUCCESS)
  {
    //Batt_Id.BatteryIdTolerance = gBattIdCfgDa.BatteryIdTolerance;
    Status |= PmicFgSram_PickProfile(&Battery_Id, &FgProfile, &ProfileIndex);
  }
  if((EFI_SUCCESS == Status ) && (ProfileIndex < MAX_NUM_OF_BATT_PROFILE) &&
    (NULL != FgProfile.Pd[ProfileIndex].addrData))
  {
    SingleprofileArray = FgProfile.Pd[ProfileIndex].addrData;
    /* burst read*/
    /* memset Read Array to zero */
    SetMem(SramRdProfileArray, sizeof(UINT32) * BATTARY_PROFILE_MAX_SIZE, 0x00);

    Status = PmicFgSram_ImaReadBurst(PmicDeviceIndex, (UINT16)FG_SRAM_PROFILE_START_ADDR,
                                      (UINT8)FG_SRAM_PROFILE_VERIFIY_CNT_IN_WORDS,
                                      SramRdProfileArray);
    if(Status != EFI_SUCCESS)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a PmicFgSram_ImaReadBurst Error = %d \r\n", __FUNCTION__, Status));
      return EFI_DEVICE_ERROR;
    }
    /* compare bytes */
    for(ChkCounter = 0; ChkCounter <= FG_SRAM_PROFILE_VERIFIY_CNT_IN_WORDS; ChkCounter++)
    {
      if (0 == (SramRdProfileArray[ChkCounter] ^ (SingleprofileArray + ChkCounter)->Data))
      {
        /* Nop*/
      }
      else
      {
        MemCmpCnt++;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a SramRdProfileArray[%d] = 0x%x profileArray[%d].Data = 0x%x\r\n", __FUNCTION__,
                     ChkCounter, SramRdProfileArray[ChkCounter], ChkCounter, (SingleprofileArray + ChkCounter)->Data));
        //break; /* Continue to know how many bytes mis match we have */
      }
    }
    if (0 == MemCmpCnt){
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a memcmp Success count = %d \r\n", __FUNCTION__,ChkCounter));
      *bProfileMatch = TRUE;
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a memcmp Failed at count = %d, MemCmpCnt = %d \r\n",  __FUNCTION__,ChkCounter, MemCmpCnt));
      *bProfileMatch = FALSE;
    }
    SingleprofileArray = NULL;
  }

  return Status;
}


/**
PmicFgSram_ReadIntegrityBit()

@brief
Read Integrity Bit Satus from Sram Address
*/
EFI_STATUS PmicFgSram_ReadIntegrityBit(UINT32 PmicDeviceIndex, BOOLEAN *bIntergrityBit)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8      Data   = 0;

  if(!bIntergrityBit)
  {
    return EFI_INVALID_PARAMETER;
  }

  /* Set default value */
  *bIntergrityBit = FALSE;

  Status = PmicFgSram_ImaReadSingleOffset(PmicDeviceIndex, (UINT16)FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR,
                                                 &Data, (UINT8)FG_SRAM_PROFILE_INTEGRITY_OFFSET);

  if(EFI_SUCCESS == Status )
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Integrity byte address = %d data = 0x%x \r\n", __FUNCTION__,FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR, Data));
    Data = (Data & FG_SRAM_PROFILE_LOAD_BIT_MASK);
    /* Integrity reg is having non zero value on first boot read due to FTRIM over write mostly need to remove below on */
    switch(Data)
    {
      case FG_SRAM_PROFILE_INTEGRITY_VALUE:
      case FG_SRAM_UEFI_LOAD_VALUE:
        /* Integrity bit is already set */
        *bIntergrityBit = TRUE;
      break;
      case FG_SRAM_HLOS_LOAD_VALUE:
        /* Integrity bit is already set by HLOS */
        *bIntergrityBit = TRUE;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a HLOS have loaded battery Profile in earlier boot. \r\n", __FUNCTION__));
      break;
      case FG_SRAM_HLOS_LOAD_BATTERY_SWAP:
      //case 11:
        /* Integrity bit is already set by HLOS */
        *bIntergrityBit = TRUE;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a HLOS,UEFI have loaded Battery Profile in earlier boot. \r\n", __FUNCTION__));
      break;
      case FG_SRAM_PROFILE_INTEGRITY_CLR_VALUE:
      default:
      break;
    }
  }
  return Status;
}

/**
PmicFgSram_SramHealthy()

@brief
Verify if SRAM content are health 
*/
EFI_STATUS PmicFgSram_SramHealthy(UINT32 PmicDeviceIndex)
{
  EFI_STATUS  Status          = EFI_SUCCESS;
  BOOLEAN     UnExpectedMatch = FALSE;
  pm_err_flag_type  err_flag = PM_ERR_FLAG_SUCCESS;
  UINT32      PSHoldSHFT = 0;
  UINTN       PSHoldAddress = 0;

  Status = PmicFgSram_VerifySramContent(PmicDeviceIndex, &UnExpectedMatch);
  if((EFI_SUCCESS == Status ) && (TRUE == UnExpectedMatch))
  {
    /* Issue Dvdd Reset */
    /* PM: PS-HOLD dVdd H-Reset 
       PMI: GP1 dVdd-shutdown */
    err_flag  = pm_pon_ps_hold_cfg(PM_DEVICE_0, PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET);
    err_flag |= pm_pon_gp_reset_cfg( PmicDeviceIndex, PM_PON_RESET_SOURCE_GP1, PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN);

    //TODO: Cleanup, only one place should have this address cfg or pcd
    PSHoldAddress = PcdGet32 (PcdPsHoldAddress);

    /* This register controls the PSHOLD value.496
      1: Asserted
      0: Deasserted */
    MmioWrite32( PSHoldAddress, 0 << PSHoldSHFT );

    FOREVER /* Till device reset */
    {
      gBS->Stall(10000); /* 10 ms*/
    }
    /* If reset, it should never get here, if we do loop ! */
    /* CpuDeadLoop(); */
  }
  else
  {
    //PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a SRAM contents are good \r\n", __FUNCTION__, Status));
    DEBUG(( EFI_D_WARN, "PmicDxe:: %a SRAM contents are good \r\n", __FUNCTION__, Status));
  }

  return Status;
}
/**
PmicFgSram_VerifySramContent()

@brief
Verify Given SRAM content
*/
EFI_STATUS PmicFgSram_VerifySramContent(UINT32 PmicDeviceIndex, BOOLEAN *UnExpectedMatch)
{
  EFI_STATUS  Status          = EFI_SUCCESS;
  UINT32      FgUnUsedData    = 0;

  if(!UnExpectedMatch)
    return EFI_INVALID_PARAMETER;

  *UnExpectedMatch = FALSE;

  /* Read given address */
  Status = PmicFgSram_ImaRead(PmicDeviceIndex, FG_SRAM_SP_UN_USED_ADDR, &FgUnUsedData);
  if(Status != EFI_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Single Ima Read Failed = %r \r\n", __FUNCTION__, Status));
    return Status;
  }

  /* If read and unexpected value match then device needs to do dvdd reset to re cover SRAM trim and other contents 
     Ideally this content need to be compared with OTM trim values to be sure and current OTP read API is not available
     verifying address 0 is all 0s’ and verifying 20 is non zero */
  if(FgUnUsedData != FG_SRAM_SP_UN_USED_ZERO)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a FgUnUsedData = %d \r\n", __FUNCTION__,  FgUnUsedData));
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a FG SRAM corrupted Resetting device \r\n", __FUNCTION__));
    DEBUG(( EFI_D_WARN, "PmicDxe:: %a FgUnUsedData = 0x%x \r\n", __FUNCTION__, FgUnUsedData));
    DEBUG(( EFI_D_WARN, "PmicDxe:: %a FG SRAM corrupted Resetting device  \r\n", __FUNCTION__));

    *UnExpectedMatch = TRUE;

  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a FgUnUsedData = 0x%x \r\n", __FUNCTION__, FgUnUsedData));
    DEBUG(( EFI_D_WARN, "PmicDxe:: %a FgUnUsedData = 0x%x \r\n", __FUNCTION__, FgUnUsedData));
  }

  return Status;
}


/**
PmicFgSram_SetOneTimeSramCfgs()

@brief
Program FG SRAM one time cfgs
*/
EFI_STATUS PmicFgSram_SetOneTimeSramCfgs(UINT32 PmicDeviceIndex)
{
  EFI_STATUS        Status         = EFI_SUCCESS;
  pm_err_flag_type  err_flag       = PM_ERR_FLAG_SUCCESS;
  UINT8             VBtEmptyValue  = 0;

  err_flag |= pm_fg_batt_info_set_esr_pull_down_cfg(PmicDeviceIndex, PM_FG_BATT_INFO_ESR_PULL_DOWN_CFG, (uint8 )PM_FG_BATT_INFO_ESR_MEAS_CUR_180MA);

    /* Setting default parameter data */
    /* RAM.sp.KiCt = 0.00585936  = 0x18
    WA Increase Cut-off KiCoeff       */
    Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_SP_KICT_ADDR,
                                             FG_SRAM_SP_KICT_VALUE, FG_SRAM_SP_KICT_ADDR_OFFSET, FG_SRAM_SP_KICT_MASK);
    Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_SP_VCTIBTRSLWEN_ADDR,
                                             FG_SRAM_SP_VCTIBTRSLWEN_VALUE, FG_SRAM_SP_VCTIBTRSLWEN_ADDR_OFFSET, FG_SRAM_SP_VCTIBTRSLWEN_MASK);

    Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_SP_SAT_CC_CLR_AUTO_ADDR,
                                             FG_SRAM_SP_SAT_CC_CLR_AUTO_VALUE, FG_SRAM_SP_SAT_CC_CLR_AUTO_ADDR_OFFSET, FG_SRAM_SP_SAT_CC_CLR_AUTO_MASK);
  
  /* WA 3.20 Update ESR Pulse Amplitude and Qualification Threshold */
  Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_SP_ESRITRSH_ADDR,
                                           FG_SRAM_SP_ESRITRSH_VALUE, FG_SRAM_SP_ESRITRSH_ADDR_OFFSET, FG_SRAM_SP_ESRITRSH_MASK);

  Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_SP_ESRTMRCFGDSC_MAX_ADDR,
                                           FG_SRAM_SP_ESRTMRCFGDSC_MAX_VALUE, FG_SRAM_SP_ESRTMRCFGDSC_MAX_ADDR_OFFSET, FG_SRAM_SP_ESRTMRCFGDSC_MAX_MASK);

    /* Configure VBtEmpty threshold */
    if((gFgConfigData.VBatEmpty == 0 )|| (gFgConfigData.VBatEmpty > FG_SRAM_ENUM_VBAT_EMPTY_MAX))
    {
      VBtEmptyValue = FG_SRAM_VEMPTY_REG_VALUE ;
    }
    else
    {
      VBtEmptyValue = ((gFgConfigData.VBatEmpty - FG_SRAM_ENUM_VBAT_EMPTY_OFFSET) * FG_SRAM_VEMPTY_VOLTAGE_MAX_ENCODING_VALUE ) / FG_SRAM_VEMPTY_VOLTAGE_MAX;/* Data in milliVolt */
    }
    Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_VEMPTY_IRQ_REG_ADDR,
                                             VBtEmptyValue, FG_SRAM_VEMPTY_REG_ADDR_OFFSET, FG_SRAM_VEMPTY_REG_MASK);

  return (Status |err_flag);
}

/**
PmicFgSram_LoadBattProfile()

@brief
Load Battery Profile in Sram
*/
EFI_STATUS PmicFgSram_LoadBattProfile
(
  UINT32  PmicDeviceIndex,
  IN FgBattProfile *Profile,
  IN FgSramAddrDataEx * AddrDataPairEx, IN UINT32 AddrDataCount,
  AdcRr_BattId *Batt_Id
)
{
  EFI_STATUS  Status          = EFI_SUCCESS;
  pm_err_flag_type  err_flag = PM_ERR_FLAG_SUCCESS;
  /* AdcRr_BattId Batt_Id = {0} ; */
  BOOLEAN  bValidProfile          = FALSE;
  UINT32   ProfileIndex           = 0;
  BOOLEAN  bBatIdInToleranceLimit = FALSE;
  BOOLEAN  bExceptionOccured      = FALSE;
  BOOLEAN  bIrqUpdated            = FALSE;
 // UINT8    VBtEmptyValue          = 0;
  UINT8    NoOfBytesToRead        = 0;

  NoOfBytesToRead = gFgConfigData.FgSramDbgCfg.DumpSramEndAddr - gFgConfigData.FgSramDbgCfg.DumpSramStartAddr;

  /* DEBUG Dump before profile load if enabled */
  if(!RETAIL)
  {
    if(gFgConfigData.FgSramDbgCfg.DumpSram == TRUE)
	{
	  Status |= PmicFgSram_Dump(PmicDeviceIndex, gFgConfigData.FgSramDbgCfg.DumpSramStartAddr, NoOfBytesToRead);
	  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Dump Completed = %r \r\n", __FUNCTION__, Status));
	}
  }

  if( !Profile || !Batt_Id || !AddrDataPairEx || (AddrDataCount > FG_SRAM_END_ADDR))
    return EFI_INVALID_PARAMETER;

  /* TBD Debug SRAM dump before loading profile */

  /* Validate profile for check sum, if check sum validates then only load profile other wise just retrieve Batt ID */

  /* Enable after System verify checksum */
  Status |= PmicFgSram_ValidateCheckSum(Profile, &bValidProfile);
  if (TRUE == bValidProfile && Status == EFI_SUCCESS)
  {
    /*PMIC_FILE_UART_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Loading Battery Profile for Fuel Gauge Start Time Stamp = %lld us \r\n",__FUNCTION__,
                   ));*/
    LogMsg("Loading Battery Profile for Fuel Gauge Start ");
    /* Release the memory access  */
    Status |= PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);
    /* Read Battery ID from ADC and compare it with profile Battery ID */
    Status |= PmicFgSram_ReadBattID(Batt_Id);
    if(Status == EFI_SUCCESS)
    {
      //Batt_Id->BatteryIdTolerance = gBattIdCfgDa.BatteryIdTolerance;
      Status |= PmicFgSram_PickProfile(Batt_Id, Profile, &ProfileIndex);
      bBatIdInToleranceLimit = PmicFgSram_BatteryIdInToleranceLimit(Batt_Id, Profile->Pd[ProfileIndex].Pp.BatteryId);
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ProfileIndex = %d bBatIdInToleranceLimit = %d \r\n",__FUNCTION__,
                   ProfileIndex, bBatIdInToleranceLimit));
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Battery Id read Error = %r \r\n",__FUNCTION__, Status));
      /* use defualt Profile Index 0 to load profile since battery id do not match or read error */
      ProfileIndex = 0;
      bBatIdInToleranceLimit = TRUE;
      Status = EFI_SUCCESS;
    }
    /* Save selected profile */
    Profile->SelectedProfile = ProfileIndex;

    if (TRUE == bBatIdInToleranceLimit )
    {
      //Set Sram module internal state 
      PmicFgSram_SetState(FG_SRAM_STATUS_LOAD_PROFILE);

      /* wait for SOC_UPDT IRQ */
      /* TBD Enable after System verify checksum */
      Status |= PmicFgSram_PollLatchedIrq(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_UPDT, &bIrqUpdated);
      /* wait for SOC_RDY IRQ */
      Status |= PmicFgSram_PollRtIrq(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY, &bIrqUpdated);
      if((EFI_SUCCESS == Status ) && (bIrqUpdated == TRUE))
      {
        /* Write the profile in the volatile memory */
        Status |= PmicFgSram_WriteProfileToSram(PmicDeviceIndex, Profile->Pd[ProfileIndex].addrData);
        /* Make sure that the register Profile_check has the Integrity Profile set to 1 */

       /* Update integrity bit */
        Status |= PmicFgSram_ImaWriteProfileIntegrity(PmicDeviceIndex, (UINT16)FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR, 
                                             (UINT8) FG_SRAM_PROFILE_INTEGRITY_UEFI_ENABLE, (UINT8)FG_SRAM_PROFILE_INTEGRITY_OFFSET);
        if(gFgConfigData.RconnComp != 0) /* 0 is considered as disabled */
        {
          /* Check for valid supported range */
          if((gFgConfigData.RconnComp < FG_SRAM_RCONN_COMP_MAX) && (gFgConfigData.RconnComp > FG_SRAM_RCONN_COMP_MIN))
          {
            Status |= PmicFgSram_UpdateRconnComp(PmicDeviceIndex);
          }
          else
          {
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Error Rconn invalid range +/- 100mOhms \r\n",__FUNCTION__));
          }
        }
        /* Disable Charging */
        Status |= PmicFgSram_EnableCharging(PmicDeviceIndex, FALSE);
        /* Issue SOC RESTART */

        pm_fg_batt_soc_irq_set_trigger(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY, PM_IRQ_TRIGGER_RISING_EDGE);
        pm_fg_batt_soc_irq_enable(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY, TRUE);
        pm_fg_batt_soc_irq_clear(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY);

        err_flag |= pm_fg_batt_soc_restart(PmicDeviceIndex, TRUE);
        /* Wait for SOC cycle */
        gBS->Stall(PM_FG_CYCLE_AFFECT_TIME_OUT);

        /* Wait for SOC_READY IRQ */
        Status |= PmicFgSram_PollRisingEdgeIrq(PmicDeviceIndex, PM_FG_BATT_SOC_SOC_READY, &bIrqUpdated);

        /* Disable SOC RESTART */
        err_flag |= pm_fg_batt_soc_restart(PmicDeviceIndex, FALSE);

        /* Enable Charging */
        Status |= PmicFgSram_EnableCharging(PmicDeviceIndex, TRUE);
      }
      else
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Poll SOC_UPDT/SOC_RDY failed = %r bIrqUpdated = %d \r\n",__FUNCTION__, Status, bIrqUpdated));
      }
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Profile InValid Bat ID Batt_Id.Bid = %d, Profile->BatteryId = %d Status = %r \r\n",__FUNCTION__,
                   Batt_Id->Bid, Profile->Pd[ProfileIndex].Pp.BatteryId, Status));
      /* Do nothing since FG already must have profile loaded during PBS sequnce */
    }
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a InValid Profile/CheckSum \r\n",__FUNCTION__));
  }

  /*Clear scratch register for delta SoC and saved time*/
#if 0 
  /* TBD */
  Status |= PmicFgSram_OverwriteRtcAndDeltaSoC(PmicDeviceIndex, NULL, NULL);
#endif

  /*Update FG batt Cache for Temperature */
  /* Status |= PmicFgSram_UpdateCache(&fgBattParamCache); */

  //Print profile name which is valid and getting loaded
  PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: Battery Profile Name flashed = %s \r\n", Profile->Pd[ProfileIndex].Pp.BatteryName));
  PMIC_UART_DEBUG(( EFI_D_WARN, "PmicDxe: Battery Profile Name flashed = %a \r\n", Profile->Pd[ProfileIndex].Pp.BatteryName));

  //Set Sram module internal state to available
  PmicFgSram_SetState(FG_SRAM_STATUS_AVAILABLE);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a End \r\n",__FUNCTION__));

  /* Debug SRAM dump After loading profile */
  /*Status |= PmicFgSram_VerifyRegs(PmicDeviceIndex);*/

#if 0
  /* DEBUG Dump after profile load if enabled */
  if(!RETAIL)
  {
    if(gFgConfigData.FgSramDbgCfg.DumpSram == TRUE)
    {
      Status |= PmicFgSram_Dump(PmicDeviceIndex, gFgConfigData.FgSramDbgCfg.DumpSramStartAddr, NoOfBytesToRead);
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Dump Completed = %r \r\n", __FUNCTION__, Status));
    }
  }
#endif
  LogMsg("Loading Battery Profile for Fuel Gauge End ");

  return (Status | err_flag);
}


EFI_STATUS PmicFgSram_PickProfile
(
  AdcRr_BattId *Batt_Id,
  FgBattProfile *Profile,
  UINT32 *ProfileCount
)
{
  UINT32 count = 0;
  BOOLEAN ProfileValid = FALSE;

  for(count = 0; (count < MAX_NUM_OF_BATT_PROFILE ) && (count < Profile->ProfileCount )
              && (NULL != Profile->Pd[count].addrData); count++)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe::%a Count = (0x%x) Batt_Id.Bid= (0x%x) Profile[%d].BatteryId = (0x%x)\r\n",__FUNCTION__,
                        count, Batt_Id->Bid, count, Profile->Pd[count].Pp.BatteryId));
    ProfileValid = PmicFgSram_BatteryIdInToleranceLimit(Batt_Id, Profile->Pd[count].Pp.BatteryId);
    if(ProfileValid == TRUE)
    {
      *ProfileCount = count;
      break;
    }
  }

  if(ProfileValid == FALSE || count == MAX_NUM_OF_BATT_PROFILE){
    /* Make default profile as first */
  *ProfileCount = 0;
  }
  //Assign for which profile is selected
  Profile->SelectedProfile = *ProfileCount;

  return EFI_SUCCESS;

}


/**
PmicFgSram_BatteryIDInToleranceLimit()

@brief
Check Battery id tolerance limit against battery id from sram and profile 
*/
BOOLEAN PmicFgSram_BatteryIdInToleranceLimit(AdcRr_BattId *Batt_Id, UINT32 Profile_Batt_Id)
{
  UINT32 lower_limit = 0, higher_limit = 0;

  if(!Batt_Id)
    return FALSE;

  lower_limit  = Profile_Batt_Id - ((Profile_Batt_Id * Batt_Id->BatteryIdTolerance)/ 100);
  higher_limit = Profile_Batt_Id + ((Profile_Batt_Id * Batt_Id->BatteryIdTolerance)/ 100);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe::%a lower_limit = %d, higher_limit = %d Tolerance = %d \r\n", __FUNCTION__,lower_limit, higher_limit, Batt_Id->BatteryIdTolerance));

  if ((Batt_Id->Bid >= lower_limit) && (Batt_Id->Bid <= higher_limit))
    return TRUE;
  else
    return FALSE;
}


/*==========================================================================
                        LOCAL  API DEFINITION
===========================================================================*/
/**
PmicFgSram_SetState()

@brief
Set Sram State
*/
EFI_STATUS PmicFgSram_SetState(FgSramState FgSramSt)
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  if ( FG_SRAM_STATUS_INVALID == SramState)
  {
    Status = EFI_DEVICE_ERROR;
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: PmicFgSram_SetState: Error SramState = (%d) \r\n", SramState));
  }
  else{
    SramState = FgSramSt;
    //PmicFgSram_PrintState(FgSramSt);
    //PmicFgSram_PrintState(SramState);
  }

  return Status;
}

/**
PmicFgSram_PrintState()

@brief
Debug Print Sram State
*/
void PmicFgSram_PrintState(FgSramState FgSramSt)
{
  switch(FgSramSt){
    case FG_SRAM_STATUS_INIT:
      PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: FG_SRAM_STATUS_INIT SramState: = (%d) \r\n", SramState));
    break;
    case FG_SRAM_STATUS_LOAD_PROFILE:
      PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: FG_SRAM_STATUS_LOAD_PROFILE SramState: = (%d) \r\n", SramState));
    break;
    case FG_SRAM_STATUS_IN_USE:
      PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: FG_SRAM_STATUS_IN_USE SramState: = (%d) \r\n", SramState));
    break;
    case FG_SRAM_STATUS_POLLING:
      PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: FG_SRAM_STATUS_POLLING SramState: = (%d) \r\n", SramState));
    break;
    case FG_SRAM_STATUS_AVAILABLE:
      PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: FG_SRAM_STATUS_AVAILABLE SramState: = (%d) \r\n", SramState));
    break;
    case FG_SRAM_STATUS_INVALID:
      PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: FG_SRAM_STATUS_INVALID SramState: = (%d) \r\n", SramState));
    break;
    default:
      PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: FG_SRAM_STATUS_DEFAULT SramState: = (%d) \r\n", SramState));
      break;
  }
  return;
}


/**
PmicFgSram_ResetIma()

@brief
Reset IMA sequence for clearing IMA exception status
*/
pm_err_flag_type PmicFgSram_ResetIma(uint32 PmicDeviceIndex)
{
  pm_err_flag_type err_flg = PM_ERR_FLAG_SUCCESS;
  pm_fg_memif_ima_sts fg_ima_sts;
  UINT32              nPauseDelay = 0;
  BOOLEAN             bExceptionSts = FALSE;
  UINT8               fg_memif_data = 0;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Trigger FG IMA Reset.\r\n", __FUNCTION__));

  /* Set AICS Clear */
  err_flg  = pm_fg_mem_if_set_ima_cfg_value(PmicDeviceIndex, 0x14);

  err_flg |= pm_fg_mem_if_set_mem_intf_ctl_value(PmicDeviceIndex, 0x00);

  err_flg |= pm_fg_mem_if_set_mem_intf_cfg_value(PmicDeviceIndex, 0xA0);

  /* Pause for 40 us*/
  gBS->Stall(FG_SRAM_PAUSE_DELAY);

  do
  {
    fg_memif_data = 0;
    err_flg |= pm_fg_mem_if_write_addr_msb(PmicDeviceIndex, 0x00);
    err_flg |= pm_fg_mem_if_write_data3(PmicDeviceIndex, fg_memif_data);
    err_flg |= pm_fg_mem_if_read_data_reg3(PmicDeviceIndex, &fg_memif_data);

    /* Pause for 40 us*/
    gBS->Stall(FG_SRAM_PAUSE_DELAY);
    err_flg |= pm_fg_mem_if_get_ima_sts(PmicDeviceIndex, &fg_ima_sts);
    nPauseDelay += FG_SRAM_PAUSE_DELAY;
    if(nPauseDelay > FG_SRAM_PAUSE_DELAY_MAX_TIME)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Reset.Timeout nPauseDelay = %lld us \r\n", __FUNCTION__, nPauseDelay));
      err_flg = PM_ERR_FLAG_FAILURE;
      break;
    }

    bExceptionSts = fg_ima_sts.excep_sts.iacs_err | fg_ima_sts.excep_sts.xct_type_err | fg_ima_sts.excep_sts.be_burstwr_warn |
                    fg_ima_sts.excep_sts.data_rd_err | fg_ima_sts.excep_sts.data_wr_err | fg_ima_sts.excep_sts.addr_burst_wrap | fg_ima_sts.excep_sts.addr_stable_err;

    fg_memif_data = 0;

    err_flg |= pm_fg_mem_if_get_ima_hw_sts(PmicDeviceIndex, &fg_memif_data);

  }while((err_flg == PM_ERR_FLAG_SUCCESS ) && ((bExceptionSts == TRUE ) || (fg_memif_data != 0x00)));

  /* Make sure to have Read access */
  err_flg  = pm_fg_mem_if_set_ima_cfg_value(PmicDeviceIndex, 0x10);

  /* Make sure to have Single access */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl_value(PmicDeviceIndex, 0x00);

  /* Pause for 40 us*/
  gBS->Stall(FG_SRAM_PAUSE_DELAY);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Trigger FG IMA Reset.Completed \r\n", __FUNCTION__));

  return err_flg;
}


/**
PmicFgSram_ClearDmaLog()

@brief
Clears DMA Log. Required as a Enhancement routine to fix issue 
on PMI8998 v2.0 
*/
pm_err_flag_type 
PmicFgSram_ClearDmaLog(uint32 PmicDeviceIndex)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   boolean dma_err_sts = FALSE;
   uint8 poll_count    = 0;

   err_flag |= pm_fg_memif_get_dma_err_sts(PmicDeviceIndex, &dma_err_sts); //Check for DMA error
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      return err_flag;
   }

   if (dma_err_sts)
   {
      err_flag |= pm_fg_memif_dma_clear(PmicDeviceIndex, TRUE); //Clear DMA
      for (poll_count=0; poll_count < PM_FG_NUM_OF_DMA_ERR_STS_POLL; poll_count++) //Poll for DMA err status
      {
         err_flag |= pm_fg_memif_get_dma_err_sts(PmicDeviceIndex, &dma_err_sts);
         if ((dma_err_sts == FALSE) || (err_flag != PM_ERR_FLAG_SUCCESS))
         {
            break;
         }
      }

      if ((dma_err_sts == TRUE)|| (err_flag != PM_ERR_FLAG_SUCCESS))
      {
         PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a DMA Clear Log failed \r\n", __FUNCTION__));
         return err_flag = PM_ERR_FLAG_FAILURE;
      }
      else
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a DMA Clear SUCCESS \r\n", __FUNCTION__));
      }
      err_flag |= pm_fg_memif_dma_clear(PmicDeviceIndex, FALSE);//reset
   }
   
   return err_flag;
}

/**
PmicFgSram_HandleIfImaException()

@brief
handle IMA access exception 
*/
EFI_STATUS PmicFgSram_HandleIfImaException
(
  UINT32  PmicDeviceIndex, BOOLEAN *bExceptionOccured
)
{
  EFI_STATUS       Status   = EFI_SUCCESS;
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;
  pm_fg_memif_ima_sts fg_ima_sts ;
  BOOLEAN      bMemExceptionRtSts = FALSE;
  BOOLEAN      bExceptionSts = FALSE;

  if(!bExceptionOccured)
    return EFI_INVALID_PARAMETER;

  SetMem(&fg_ima_sts, sizeof(pm_fg_memif_ima_sts), 0x00);

  *bExceptionOccured = FALSE;

  /* Handle DMA error if any */
  err_flg = PmicFgSram_ClearDmaLog(PmicDeviceIndex);
  if (err_flg != PM_ERR_FLAG_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe::%a Clear DMA Failed \r\n", __FUNCTION__));
    return err_flg;
  }

  /* Handle if IMA exception  */
  err_flg |= pm_fg_mem_if_get_ima_sts(PmicDeviceIndex, &fg_ima_sts);
  err_flg |= pm_fg_mem_if_irq_status(PmicDeviceIndex, PM_FG_MEM_IF_MEM_XCP, PM_IRQ_STATUS_RT, &bMemExceptionRtSts);
  if(err_flg != PM_ERR_FLAG_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe::%a Error = %d bMemExceptionRtSts = %d \r\n", __FUNCTION__, err_flg, bMemExceptionRtSts ));
  }

  bExceptionSts = fg_ima_sts.excep_sts.iacs_err | fg_ima_sts.excep_sts.xct_type_err | fg_ima_sts.excep_sts.be_burstwr_warn |
                  fg_ima_sts.excep_sts.data_rd_err | fg_ima_sts.excep_sts.data_wr_err | fg_ima_sts.excep_sts.addr_burst_wrap | fg_ima_sts.excep_sts.addr_stable_err;

  if((TRUE == bExceptionSts) || (TRUE == bMemExceptionRtSts))
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Occurred bMemExceptionRtSts = %d bExceptionSts = %d Resetting IMA \r\n",__FUNCTION__, 
                bMemExceptionRtSts, bExceptionSts));
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a iacs_err = %d , xct_type_err = %d , be_burstwr_warn = %d,",
               __FUNCTION__,fg_ima_sts.excep_sts.iacs_err, fg_ima_sts.excep_sts.xct_type_err,fg_ima_sts.excep_sts.be_burstwr_warn));
    PMIC_DEBUG(( EFI_D_WARN, "data_rd_err = %d , data_wr_err = %d , addr_burst_wrap = %d , addr_stable_err = %d \r\n",
               fg_ima_sts.excep_sts.data_rd_err, fg_ima_sts.excep_sts.data_wr_err, fg_ima_sts.excep_sts.addr_burst_wrap, fg_ima_sts.excep_sts.addr_stable_err));
    *bExceptionOccured = TRUE;
    Status |= PmicFgSram_ResetIma(PmicDeviceIndex);
  }
  else
  {
    /* nop*/
  }

  return Status;

}

EFI_STATUS PmicFgSram_PollImaReady(UINT32 PmicDeviceIndex, BOOLEAN *bImaAccessAvailable)
{
  EFI_STATUS          Status   = EFI_SUCCESS;
  pm_err_flag_type    err_flg  = PM_ERR_FLAG_SUCCESS;
  pm_fg_memif_ima_sts fg_ima_sts;
  UINT32              Ttl_spent_time_in_polling = 0;
  BOOLEAN             Mem_Available_status = FALSE;

  if(NULL == bImaAccessAvailable)
    return EFI_INVALID_PARAMETER;

  SetMem(&fg_ima_sts, sizeof(pm_fg_memif_ima_sts), 0x00);

  *bImaAccessAvailable = FALSE;

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_POLLING);

  //Poll FG_MEM_AVAIL_RT_STS = 1 
  do{
      /* mem available best time is 30us */
      err_flg = pm_fg_mem_if_get_ima_sts(PmicDeviceIndex, &fg_ima_sts);
      err_flg = pm_fg_mem_if_irq_status(PmicDeviceIndex, PM_FG_MEM_IF_IMA_RDY, PM_IRQ_STATUS_RT, &Mem_Available_status);
      if(err_flg != PM_ERR_FLAG_SUCCESS)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a pm_fg_mem_if_get_ima_sts Error= %d \r\n", __FUNCTION__, err_flg));
        break;
      }
      if ((TRUE == fg_ima_sts.opr_sts.iacs_rdy) || (TRUE == Mem_Available_status))
      {
        *bImaAccessAvailable = TRUE;
        //PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: %a PM_FG_MEM_IF_IMA_RDY \r\n", __FUNCTION__));
        break;
      }//check for error condition as we do not want to loop forever
      else if (Ttl_spent_time_in_polling >= FG_MEM_AVAILABLE_RT_STS_POLL_MAX_TIME)
      {
        Status = EFI_DEVICE_ERROR;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a TimeOut : Ttl_spent_time_in_polling = %d us \r\n", __FUNCTION__,Ttl_spent_time_in_polling));
        break;
      }else
      {
        /* does not need to be handled */
      }

      /*wait for 30 us before querying mem available status again */
      gBS->Stall(FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME);

      Ttl_spent_time_in_polling += FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME;
  }while(TRUE);

  //Set Sram module internal state 
  if (FALSE == *bImaAccessAvailable){
    //Clear memory access bit request i.e. RIF_MEM_ACCESS_REQ = 0
    //PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);

    //might need to read cycle streach bit and clear it here
    PmicFgSram_SetState(FG_SRAM_STATUS_AVAILABLE); //when time out for polling access request 
  }
  else 
    PmicFgSram_SetState(FG_SRAM_STATUS_IN_USE);

  return (Status |= err_flg);
}


EFI_STATUS PmicFgSram_PollDmaReady(UINT32 PmicDeviceIndex, BOOLEAN *bImaAccessAvailable)
{
  EFI_STATUS          Status   = EFI_SUCCESS;
  pm_err_flag_type    err_flg  = PM_ERR_FLAG_SUCCESS;
  UINT32              Ttl_spent_time_in_polling = 0;
  BOOLEAN             Mem_Available_status = FALSE;

  if(NULL == bImaAccessAvailable)
    return EFI_INVALID_PARAMETER;

  *bImaAccessAvailable = FALSE;

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_POLLING);

  //Poll FG_MEM_AVAIL_RT_STS = 1 
  do{
      /* mem available best time is 30us */
      err_flg = pm_fg_mem_if_irq_status(PmicDeviceIndex, PM_FG_MEM_IF_DMA_GNT, PM_IRQ_STATUS_RT, &Mem_Available_status);
      if(err_flg != PM_ERR_FLAG_SUCCESS)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a pm_fg_mem_if_get_dma_sts Error= %d \r\n", __FUNCTION__, err_flg));
        break;
      }
      if (TRUE == Mem_Available_status)
      {
        *bImaAccessAvailable = TRUE;
        break;
      }//check for error condition as we do not want to loop forever
      else if (Ttl_spent_time_in_polling >= FG_MEM_AVAILABLE_RT_STS_POLL_MAX_TIME)
      {
        Status = EFI_DEVICE_ERROR;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: %a TimeOut Ttl_spent_time_in_polling = %d us \r\n", __FUNCTION__,Ttl_spent_time_in_polling));
        break;
      }else
      {
        /* does not need to be handled */
}

      /*wait for 30 us before querying mem available status again */
      gBS->Stall(FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME);

      Ttl_spent_time_in_polling += FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME;
  }while(TRUE);

  //Set Sram module internal state 
  if (FALSE == *bImaAccessAvailable){
    //might need to read cycle streach bit and clear it here
    PmicFgSram_SetState(FG_SRAM_STATUS_AVAILABLE); //when time out for polling access request 
  }
  else 
    PmicFgSram_SetState(FG_SRAM_STATUS_IN_USE);

  return (Status |= err_flg);
}

/**
PmicFgSram_ImaReadSingle()

@brief
Reads 4 bytes (UINT32) in given Sram address using IMA handshake
*/
EFI_STATUS PmicFgSram_ImaReadSingle(UINT32 PmicDeviceIndex, UINT16 ReadAddress, UINT32 *fg_memif_data, BOOLEAN *bBeatCountMatch)
{
  EFI_STATUS       Status        = EFI_SUCCESS;
  pm_err_flag_type err_flg       = PM_ERR_FLAG_SUCCESS;
  BOOLEAN          bMemAvailable = FALSE;
  BOOLEAN          bExceptionOccured = FALSE;
  UINT8            FgBeatCount1 = 0, FgBeatCount2 = 0;
  UINT32           nExcpCnt      = 0;

  if(!bBeatCountMatch || !fg_memif_data || ReadAddress > FG_SRAM_END_ADDR)
    return EFI_INVALID_PARAMETER;

  do
  {
    *bBeatCountMatch = FALSE;

    /* Request FG_MEM_IF access i.e. RIF_MEM_ACCESS_REQ = 1 */
    Status = PmicFgSram_RequestFgSramAccess(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ);

    /* Select IMA access type  */
    err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

    /* Request Burst access */
    err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE);

    /* Requet read access */
    err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FG_MEMORY_ACCESS_INTF_CTL_READ_ACCESS);

    /* Poll for IMA readiness  */
    Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
    if(Status != EFI_SUCCESS)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed = %r \r\n", __FUNCTION__, Status));
    }

    if(TRUE == bMemAvailable)
    {
      /* Enable all byte read */
      err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);

      err_flg |= pm_fg_mem_if_get_fg_beat_count(PmicDeviceIndex, &FgBeatCount1);

      /* Write Read Location Address */
      err_flg |=  pm_fg_mem_if_write_addr(PmicDeviceIndex, ReadAddress);

      bMemAvailable = FALSE; /* Reset Var */

      /* Poll for IMA readiness  */
      Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
      if(Status != EFI_SUCCESS)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed = %r \r\n", __FUNCTION__, Status));
      }

      if(TRUE == bMemAvailable)
      {
        /* Read Data */
        err_flg |=  pm_fg_mem_if_read_data_reg(PmicDeviceIndex, fg_memif_data);
        /*Check Integrity of data by exception status */
        Status |= PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
        if(TRUE == bExceptionOccured)
        {
          if(nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT)
          {
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry to single Read Again here \r\n", __FUNCTION__));
            /* 3 trials before giving up burst read  use continue with 3 static count and update write addr */
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry ReadAddress = %d, nExcpCnt = %d \r\n", __FUNCTION__, ReadAddress, nExcpCnt));
            /* Write Location Address again to make sure we are re writing to same address where IMA exception occurred */
            err_flg |=  pm_fg_mem_if_write_addr(PmicDeviceIndex, ReadAddress);
            continue;
          }
          else
          {
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry Exceeded nExcpCnt = %d \r\n", __FUNCTION__, nExcpCnt));
            Status = EFI_DEVICE_ERROR;/* Failure report */
            break;
          }
        }
        else
        {
          /* Good use case */
          err_flg |= pm_fg_mem_if_get_fg_beat_count(PmicDeviceIndex, &FgBeatCount2);
        }
      }
      else
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed updating read address \r\n", __FUNCTION__));
      }
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed for initiating read xaction \r\n", __FUNCTION__));
    }

    /* Make sure FG_MEM_IF is available i.e. RIF_MEM_ACCESS_REQ = 0 */
    /* err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ, FG_MEMORY_ACCESS_REQ_FALSE); */
    PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex,  &bExceptionOccured);
  }while((bExceptionOccured == TRUE ) && (++nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT));

  /* Make sure FG_MEM_IF is available i.e. RIF_MEM_ACCESS_REQ = 0 */
  PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex,  &bExceptionOccured);

  /* Update FG beat count for data read inegrity typically if beat count reads are not same then Redo the read again */
  *bBeatCountMatch = (FgBeatCount1 == FgBeatCount2) ? TRUE : FALSE;

  return (err_flg | Status);
}

/**
PmicFgSram_ImaReadBurst()

@brief
Reads 4 bytes (UINT32) in given Sram address using IMA handshake
*/
EFI_STATUS PmicFgSram_ImaReadBurst(UINT32 PmicDeviceIndex, UINT16 ReadAddress, UINT8 noOFWordToRead,
                                                UINT32 fg_memif_data[BATTARY_PROFILE_MAX_SIZE])
{
  EFI_STATUS       Status        = EFI_SUCCESS;
  pm_err_flag_type err_flg       = PM_ERR_FLAG_SUCCESS;
  BOOLEAN          bMemAvailable = FALSE;
  UINT8            ReadCount     = 0;
  UINT32           ReadData      = 0;
  UINT16           StartAddress  = ReadAddress;
  BOOLEAN          bExceptionOccured = FALSE;
  UINT32           nExcpCnt      = 0;

  if(StartAddress > FG_SRAM_END_ADDR || noOFWordToRead > FG_SRAM_END_ADDR)
  {
    return EFI_INVALID_PARAMETER;
  }

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Entry = %r \r\n", __FUNCTION__, Status));

  SetMem(fg_memif_data, sizeof(UINT32) * BATTARY_PROFILE_MAX_SIZE, 0x00);

  /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Set Mem Done = %r \r\n", __FUNCTION__, Status));*/

  /* Request FG_MEM_IF access i.e. RIF_MEM_ACCESS_REQ = 1 */
  Status = PmicFgSram_RequestFgSramAccess(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ);

  /* Select IMA access type  */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

  /* Request Burst access */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_BURST);

  /* Requet read access */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FG_MEMORY_ACCESS_INTF_CTL_READ_ACCESS);

  /* Poll for IMA readiness  */
  Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
  if(Status != EFI_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed = %r \r\n", __FUNCTION__, Status));
  }

  if(TRUE == bMemAvailable)
  {

    err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);

    /* Write Read Location Address */
    err_flg |=  pm_fg_mem_if_write_addr(PmicDeviceIndex, StartAddress);

    /* Poll for IMA readiness  */
    Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
    if(Status != EFI_SUCCESS)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed = %r \r\n", __FUNCTION__, Status));
    }

    if(TRUE == bMemAvailable)
    {
      //bMemAvailable = FALSE; /* Reset Var */
      do{
        /*Check Integrity of data by exception status */
        Status = PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
        if(TRUE == bExceptionOccured)
        {
          if(nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT)
          {
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry to burst Read Again here \r\n", __FUNCTION__));
            /* 3 trials before giving up burst read  use continue with 3 static count and update write addr */
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry CurrAddr = %d, ReadCount = %d, nExcpCnt = %d \r\n", __FUNCTION__, StartAddress,ReadCount, nExcpCnt));
            /* Write Location Address again to make sure we are re writing to same address where IMA exception occurred */
            err_flg =  pm_fg_mem_if_write_addr(PmicDeviceIndex, StartAddress);
            nExcpCnt++;
            continue;
          }
          else
          {
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry Exceeded nExcpCnt = %d \r\n", __FUNCTION__, nExcpCnt));
            Status = EFI_DEVICE_ERROR;/* Failure report */
            break;
          }
        }
        else
        {
          /* Good use case */
          /* Read Data */ /* TBD update here for checking FG beat count to make sure the data in from same signle FG cycle */
          err_flg =  pm_fg_mem_if_read_data_reg(PmicDeviceIndex, &ReadData);
          //Update Read Profile array 
          fg_memif_data[ReadCount] = ReadData;
          /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a pm_fg_mem_if_read_data_reg[%d] = 0x%x fg_memif_data[%d] = 0x%x \r\n", __FUNCTION__, ReadCount, ReadData, ReadCount, fg_memif_data[ReadCount])); */
          StartAddress += FG_SRAM_ADDR_INCREMENT;
          ReadCount++;
          nExcpCnt = 0;
         }
        }while((StartAddress < (BATTARY_PROFILE_MAX_SIZE - 1))
           && (ReadCount < BATTARY_PROFILE_MAX_SIZE -1) 
           && (ReadCount <= (noOFWordToRead - 1))
           && (PM_ERR_FLAG_SUCCESS == err_flg));
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed updating read address \r\n", __FUNCTION__));
    }
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed for initiating read xaction \r\n", __FUNCTION__));
  }

  /* Read last byte */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE);
  err_flg |=  pm_fg_mem_if_read_data_reg(PmicDeviceIndex, &ReadData);
  //Update Read Profile array if count is good
  if (ReadCount < BATTARY_PROFILE_MAX_SIZE)
  {
    fg_memif_data[ReadCount] = ReadData;
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ReadCount = %d \r\n", __FUNCTION__, ReadCount));
  }

  /* Make sure FG_MEM_IF is available i.e. RIF_MEM_ACCESS_REQ = 0 */
  PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Exit = %r \r\n", __FUNCTION__, Status));

  return (err_flg | Status);
}


/**
PmicFgSram_ImaReadCust()

@brief
Reads given address pairs in given Sram address using IMA handshake
*/
EFI_STATUS PmicFgSram_ImaReadCust(UINT32 PmicDeviceIndex, FgSramAddrData *SramAddrData, UINT32 AddrDataCount)
{
  EFI_STATUS       Status        = EFI_SUCCESS;
  pm_err_flag_type err_flg       = PM_ERR_FLAG_SUCCESS;
  BOOLEAN          bMemAvailable = FALSE;
  UINT8            ReadCount     = 0;
  UINT32           ReadData      = 0;
  BOOLEAN          bExceptionOccured = FALSE;
  UINT32           nExcpCnt      = 0;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Entry \r\n", __FUNCTION__));

  if(!SramAddrData)
    return EFI_INVALID_PARAMETER;

  if(AddrDataCount >= FG_SRAM_MAX_SIZE)
  {
    return EFI_INVALID_PARAMETER;
  }

  /* Request FG_MEM_IF access i.e. RIF_MEM_ACCESS_REQ = 1 */
  Status = PmicFgSram_RequestFgSramAccess(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ);

  /* Select IMA access type  */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

  /* Request Single Access */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE);

  /* Requet read access */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FG_MEMORY_ACCESS_INTF_CTL_READ_ACCESS);

  /* Poll for IMA readiness */
  Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
  if(Status != EFI_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed = %r \r\n", __FUNCTION__, Status));
  }

  if(TRUE == bMemAvailable)
  {
    err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);
    do
    {
      bMemAvailable = FALSE;

      /* Write Read Location Address */
      err_flg =  pm_fg_mem_if_write_addr(PmicDeviceIndex, SramAddrData[ReadCount].SramAddr);
      /* Poll for IMA readiness  */
      Status = PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
      if(Status != EFI_SUCCESS)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed = %r \r\n", __FUNCTION__, Status));
      }
      /* Check Integrity of data by exception status */
      Status = PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
      if(TRUE == bExceptionOccured)
      {
        if(nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT)
        {
          PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry to burst Read Again here \r\n", __FUNCTION__));
          /* 3 trials before giving up burst read  use continue with 3 static count and update write addr */
          PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry CurrAddr = %d, ReadCount = %d, nExcpCnt = %d \r\n", __FUNCTION__, SramAddrData[ReadCount].SramAddr, ReadCount, nExcpCnt));
          /* Write Location Address again to make sure we are re writing to same address where IMA exception occurred */
          nExcpCnt++;
          continue;
        }
        else
        {
          PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry Exceeded nExcpCnt = %d \r\n", __FUNCTION__, nExcpCnt));
          Status = EFI_DEVICE_ERROR;/* Failure report */
          break;
        }
      }
      else if(TRUE == bMemAvailable)
      {
        /* Read Data */ /* TBD update here for checking FG beat count to make sure the data in from same signle FG cycle */
        err_flg =  pm_fg_mem_if_read_data_reg(PmicDeviceIndex, &ReadData);
        /* Update Read Profile array */
        SramAddrData[ReadCount].SramData = ReadData;
        /* Update Read Count */
        ReadCount++;
        nExcpCnt = 0;
        /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a pm_fg_mem_if_read_data_reg[%d] = 0x%x fg_memif_data[%d] = 0x%x \r\n", __FUNCTION__, ReadCount, ReadData, ReadCount, fg_memif_data[ReadCount])); */
      }
      else
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed for initiating read xaction \r\n", __FUNCTION__));
        break; /* Exception could not check and polling failed so break */
      }
    }while((ReadCount < AddrDataCount) && (PM_ERR_FLAG_SUCCESS == err_flg));
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed updating read address \r\n", __FUNCTION__));
  }
  /* Make sure FG_MEM_IF is available i.e. RIF_MEM_ACCESS_REQ = 0 */
  /* err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ, FG_MEMORY_ACCESS_REQ_FALSE); */
  PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Exit = %r \r\n", __FUNCTION__, Status));

  return (err_flg | Status);
}

EFI_STATUS PmicFgSram_Dump(UINT32  PmicDeviceIndex, UINT32 DumpSramStartAddr,
                                   UINT8 noOFWordToRead)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32     RdDumpArray[BATTARY_PROFILE_MAX_SIZE];
  UINT32     ReadCount = 0;
  UINT16     endSramAddress      = BATTARY_PROFILE_MAX_SIZE;
  UINT16     sramAddressIncrement = DumpSramStartAddr;
  BOOLEAN    bExceptionOccured = FALSE;
  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Entry = %r \r\n", __FUNCTION__, Status));

  Status |= PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
  if(TRUE == bExceptionOccured)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry to Read Again \r\n", __FUNCTION__));
  }

  /* If feature is not enabled do not dump */
  if(FALSE == gFgConfigData.FgSramDbgCfg.DumpSram)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Disabled \r\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  Status = PmicFgSram_ImaReadBurst(PmicDeviceIndex, DumpSramStartAddr, noOFWordToRead, RdDumpArray);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a PmicFgSram_ImaReadBurst completed  = %r \r\n", __FUNCTION__, Status));

  if(EFI_SUCCESS == Status)
  {
    /* Dump only to File */
    //PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: SRAM Dump Start * \r\n"));
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a SRAM Dump Start \r\n", __FUNCTION__));
    //Print the Dump now
    sramAddressIncrement = (UINT16)DumpSramStartAddr;
    for(ReadCount = 0; (ReadCount <= (noOFWordToRead)) && (sramAddressIncrement <= endSramAddress); ReadCount++)
    {
      //Print proper address value pair below print would do 4 byte print
      /*
      PMIC_FILE_DEBUG(( EFI_D_WARN, "%03X, %02X %02X %02X %02X \r\n", sramAddressIncrement, 
      (RdDumpArray[ReadCount] & 0xFF),  (RdDumpArray[ReadCount] >> 8)& 0xFF, (RdDumpArray[ReadCount] >> 16)& 0xFF, 
      (RdDumpArray[ReadCount] >> 24) & 0xFF));*/
     PMIC_DEBUG_SIMPLE(( EFI_D_WARN, "%03d %02X %02X %02X %02X \r\n", sramAddressIncrement, 
      (RdDumpArray[ReadCount] & 0xFF),  (RdDumpArray[ReadCount] >> 8)& 0xFF, (RdDumpArray[ReadCount] >> 16)& 0xFF, 
      (RdDumpArray[ReadCount] >> 24) & 0xFF));
      //Increment for printing 
       sramAddressIncrement += FG_SRAM_ADDR_INCREMENT;
    }
    
    //PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: SRAM Dump End * \r\n"));
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a SRAM Dump End \r\n", __FUNCTION__));
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a SRAM Dump Failed = %r DumpSramStartAddr = %d noOFWordToRead = %d \r\n", __FUNCTION__, Status, DumpSramStartAddr, noOFWordToRead));
  }

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Exit = %r \r\n", __FUNCTION__, Status));

  return Status;
}

/**
PmicFgSram_ImaWriteSingle()

@brief
Writes 4 bytes (UINT32) in given Sram address using IMA handshake
*/
EFI_STATUS PmicFgSram_ImaWriteSingle(UINT32 PmicDeviceIndex, UINT16 WriteAddress, UINT32 fg_memif_data)
{
  EFI_STATUS       Status        = EFI_SUCCESS;
  pm_err_flag_type err_flg       = PM_ERR_FLAG_SUCCESS;
  BOOLEAN          bMemAvailable = FALSE;
  BOOLEAN          bExceptionOccured = FALSE;
  UINT32           nExcpCnt      = 0;

  do{
    /* Request FG_MEM_IF access i.e. RIF_MEM_ACCESS_REQ = 1 */
    Status = PmicFgSram_RequestFgSramAccess(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ);

    /* Select IMA access type  */
    err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

    /* Poll for IMA readiness  */
    Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
    if(Status != EFI_SUCCESS)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed for initiating write xaction = %r \r\n", __FUNCTION__, Status));
    }

    if(TRUE == bMemAvailable)
    {
      /* Request Single access */
      err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE);

      /* Requet read access */
      err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FG_MEMORY_ACCESS_INTF_CTL_WRITE_ACCESS);

      /* Write Location Address */
      err_flg |=  pm_fg_mem_if_write_addr(PmicDeviceIndex, WriteAddress);

      err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);

      /* Write Data */
      err_flg |=  pm_fg_mem_if_write_data(PmicDeviceIndex, fg_memif_data);

      bMemAvailable = FALSE; /* Reset Var */

      /* Poll for IMA readiness  */
      Status = PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
      if(Status != EFI_SUCCESS)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed = %r \r\n", __FUNCTION__, Status));
      }

      if(TRUE == bMemAvailable)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Addr = %d Write Success \r\n", __FUNCTION__, WriteAddress));
      }
      else
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed after write \r\n", __FUNCTION__));
        /*Check Integrity of data by exception status */
        Status = PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
        if(TRUE == bExceptionOccured)
        {
          if(nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT)
          {
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry to burst Write Again here \r\n", __FUNCTION__));
            /* 3 trials before giving up burst read  use continue with 3 static count and update write addr */
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry CurrAddr = %d, nExcpCnt = %d \r\n", __FUNCTION__, WriteAddress, nExcpCnt));
            /* Write Location Address again to make sure we are re writing to same address where IMA exception occurred */
            err_flg =  pm_fg_mem_if_write_addr(PmicDeviceIndex, WriteAddress);
            continue;
          }
          else
          {
            PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry Exceeded nExcpCnt = %d \r\n", __FUNCTION__, nExcpCnt));
            Status = EFI_DEVICE_ERROR;/* Failure report */
            break;
          }
        }
        else
        {
          /* Good use case */
        }
      }
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed before write \r\n", __FUNCTION__));
    }
    /* Make sure FG_MEM_IF is available i.e. RIF_MEM_ACCESS_REQ = 0 */
    PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);
  }while((bExceptionOccured == TRUE ) && (++nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT));

  return (err_flg | Status);
}



/**
PmicFgSram_DmaWriteBurst()

@brief
Writes 4 bytes (UINT32) in given Sram address using DMA handshake
*/
EFI_STATUS PmicFgSram_DmaWriteBurst(UINT32 PmicDeviceIndex, UINT16 StartAddress, UINT8 NoOFWordToWrite,
                                                FgBattAddrData fg_memif_data[BATTARY_PROFILE_MAX_SIZE])
{
  EFI_STATUS       Status        = EFI_SUCCESS;
  pm_err_flag_type err_flg       = PM_ERR_FLAG_SUCCESS;
  BOOLEAN          bMemAvailable = FALSE;
  UINT8            ProfileBytes[FG_SRAM_BATTERY_PROFILE_MAX_SIZE_IN_BYTES];
  UINT32           ByteCnt = 0, wordCnt = 0;
  UINT32           ProfileByteCnt = 0;
  UINT32           mask           = 0xFF;

  /* Validate Input Params */
  if((StartAddress > FG_SRAM_END_DMA_ADDR )
      || (StartAddress < FG_SRAM_START_DMA_ADDR)
      || (NoOFWordToWrite > (FG_SRAM_END_DMA_PROFILE_ADDR - FG_SRAM_START_DMA_PROFILE_ADDR)))
  {
    return EFI_INVALID_PARAMETER;
  }

  /* memset Read Array to zero */
  SetMem(ProfileBytes, FG_SRAM_BATTERY_PROFILE_MAX_SIZE_IN_BYTES, 0x00);

  if(sizeof(ProfileBytes) < (NoOFWordToWrite * NUM_BYTE_IN_WORD))
  {
    return EFI_INVALID_PARAMETER;
  }

  /* Get profile in one byte array to write as spmi long transaction */
  for(wordCnt = 0, ProfileByteCnt = 0; (wordCnt <= NoOFWordToWrite)
                                        && (ProfileByteCnt < FG_SRAM_BATTERY_PROFILE_MAX_SIZE_IN_BYTES)
                                        && (wordCnt < BATTARY_PROFILE_MAX_SIZE); wordCnt++)
  {
    /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a fg_memif_data[wordCnt].Data = 0x%x \r\n", __FUNCTION__, fg_memif_data[wordCnt].Data )); */
    for(ByteCnt= 0; (ByteCnt < NUM_BYTE_IN_WORD )&& (ProfileByteCnt < FG_SRAM_BATTERY_PROFILE_MAX_SIZE_IN_BYTES); ByteCnt++)
    {
      ProfileBytes[ProfileByteCnt] = (UINT8) ((fg_memif_data[wordCnt].Data & (mask << (ByteCnt  * NUM_BITS_IN_BYTE))) >> (ByteCnt * NUM_BITS_IN_BYTE));
      /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ByteCnt = %d wordCnt = %d mask = 0x%x Profile Byte = 0x%x \r\n", __FUNCTION__, ByteCnt, wordCnt, (mask << (ByteCnt  * NUM_BITS_IN_BYTE)), ProfileBytes[ProfileByteCnt] )); */
      ProfileByteCnt++;
    }
    //PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ByteCnt = %d wordCnt = %d mask = 0x%x \r\n", __FUNCTION__, ByteCnt, wordCnt, mask));
  }
  /* Debug print for profile bytes */
  /* for(ByteCnt = 0; (ByteCnt < (NoOFWordToWrite * NUM_BYTE_IN_WORD) ) && (ByteCnt < FG_SRAM_BATTERY_PROFILE_MAX_SIZE_IN_BYTES); ByteCnt++)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: Address = %d Profile Byte = 0x%x \r\n",ByteCnt, ProfileBytes[ByteCnt] ));
    //err_flg  = pm_fg_memif_dma_write_byte(PmicDeviceIndex, StartAddress + ByteCnt, ProfileBytes[ByteCnt]);
  }*/

  /* Set DMA ctl as partition layout to access DMA partition */
  err_flg = pm_fg_memif_dma_ctl(PmicDeviceIndex, PM_FG_MEMIF_DMA_CTL_PARTITION);

  /* Request FG_MEM_IF access i.e. RIF_MEM_ACCESS_REQ = 1 */
  Status = PmicFgSram_RequestFgSramAccess(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ);

  /* Select DMA access type  */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_DMA);

  /* Pause Mem Arb and set lo latency for mem arb */
  err_flg |= pm_fg_memif_arb_ctl(PmicDeviceIndex, PM_FG_MEMIF_ARB_REQ, TRUE);
  err_flg |= pm_fg_memif_arb_ctl(PmicDeviceIndex, PM_FG_MEMIF_ARB_LO_LATENCY_EN, TRUE);

  /* Poll for IMA readiness  */
  Status |= PmicFgSram_PollDmaReady(PmicDeviceIndex, &bMemAvailable);
  if(Status != EFI_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a DMA Polling Failed for initiating write xaction = %r \r\n", __FUNCTION__, Status));
  }

  if(TRUE == bMemAvailable)
  {
    /* can print byte or long on spmi */
    /* for(ByteCnt = 0; (ByteCnt < (NoOFWordToWrite * NUM_BYTE_IN_WORD) ) && (ByteCnt < FG_SRAM_BATTERY_PROFILE_MAX_SIZE_IN_BYTES); ByteCnt++)
    {
      err_flg  = pm_fg_memif_dma_write_byte(PmicDeviceIndex, StartAddress + ByteCnt, ProfileBytes[ByteCnt]);
    }*/
    /* Below long spmi write will falsh profile to FG SRAM */
    err_flg |= pm_fg_memif_dma_write_long(PmicDeviceIndex, StartAddress, ProfileBytes, (NoOFWordToWrite * NUM_BYTE_IN_WORD));
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a DMA Long write Status %d \r\n", __FUNCTION__, err_flg));
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a DMA Polling Failed for DMA write xaction \r\n", __FUNCTION__));
  }
  /* Make sure FG_MEM_IF is available i.e. RIF_MEM_ACCESS_REQ = 0 */
  PmicFgSram_ReleaseFgSramDmaAccess(PmicDeviceIndex);

  return (err_flg | Status);
}


/**
PmicFgSram_ImaWriteBurst()

@brief
Writes 4 bytes (UINT32) in given Sram address using IMA handshake
*/
EFI_STATUS PmicFgSram_ImaWriteBurst(UINT32 PmicDeviceIndex, UINT16 StartAddress, UINT8 NoOFWordToWrite,
                                                FgBattAddrData fg_memif_data[BATTARY_PROFILE_MAX_SIZE])
{
  EFI_STATUS       Status        = EFI_SUCCESS;
  pm_err_flag_type err_flg       = PM_ERR_FLAG_SUCCESS;
  BOOLEAN          bMemAvailable = FALSE;
  BOOLEAN          bExceptionOccured = FALSE;
  UINT32           nExcpCnt  = 0;
  UINT8            WriteCount     = 0;
  UINT16           StartWriteAddress  = StartAddress;

  /* Validate input params */
  if((StartAddress > FG_SRAM_END_ADDR) || (NoOFWordToWrite > FG_SRAM_END_ADDR))
  {
    return EFI_INVALID_PARAMETER;
  }

  /* Request FG_MEM_IF access i.e. RIF_MEM_ACCESS_REQ = 1 */
  Status = PmicFgSram_RequestFgSramAccess(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ);

  /* Select IMA access type  */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

  /* Poll for IMA readiness  */
  Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
  if(Status != EFI_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed for initiating write xaction = %r \r\n", __FUNCTION__, Status));
  }

  if(TRUE == bMemAvailable)
  {
    /* Request Single access */
    err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_BURST);

    /* Requet read access */
    err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FG_MEMORY_ACCESS_INTF_CTL_WRITE_ACCESS);

    err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);

    /* Write Location Address */
    err_flg |=  pm_fg_mem_if_write_addr(PmicDeviceIndex, StartWriteAddress);

    do{

      bMemAvailable = FALSE; /* Reset Var */

      /* Write Data */
      err_flg =  pm_fg_mem_if_write_data(PmicDeviceIndex, fg_memif_data[WriteCount].Data);

      /* Poll for IMA readiness  */
      Status = PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
      if(Status != EFI_SUCCESS)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed = %r \r\n", __FUNCTION__, Status));
      }
      /* Check Integrity of data by exception status */
      Status = PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
      if(TRUE == bExceptionOccured)
      {
        if(nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT)
        {
          PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry to burst Read Again here \r\n", __FUNCTION__));
          /* 3 trials before giving up burst read  use continue with 3 static count and update write addr */
          PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry CurrAddr = %d, WriteCount = %d, nExcpCnt = %d \r\n", __FUNCTION__, StartWriteAddress,WriteCount, nExcpCnt));
          /* Write Location Address again to make sure we are re writing to same address where IMA exception occurred */
          err_flg =  pm_fg_mem_if_write_addr(PmicDeviceIndex, StartWriteAddress);
          nExcpCnt++;
          continue;
        }
        else
        {
          PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Exception Retry Exceeded nExcpCnt = %d \r\n", __FUNCTION__, nExcpCnt));
          Status = EFI_DEVICE_ERROR;/* Failure report */
          break;
        }
      }
      else if(TRUE == bMemAvailable)
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Addr = %d Write Data = 0x%x Success \r\n", __FUNCTION__, StartWriteAddress,fg_memif_data[WriteCount].Data ));
        WriteCount++;
        StartWriteAddress+=FG_SRAM_ADDR_INCREMENT;
        nExcpCnt = 0;
      }
      else
      {
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed after write \r\n", __FUNCTION__));
        break; /* break since SW could not catch exp and polling failed */
      }
    }while(((StartWriteAddress < BATTARY_PROFILE_MAX_SIZE)
           && (WriteCount < BATTARY_PROFILE_MAX_SIZE) && (WriteCount <= NoOFWordToWrite) 
           && (PM_ERR_FLAG_SUCCESS == err_flg)));
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a IMA Polling Failed before write \r\n", __FUNCTION__));
  }

  /* Make sure FG_MEM_IF is available i.e. RIF_MEM_ACCESS_REQ = 0 */
  PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);

  return (err_flg | Status);
}


/**
PmicFgSram_ReleaseFgSramImaAccess()

@brief
Release Sram access, Clears memory access bit 
*/
EFI_STATUS PmicFgSram_ReleaseFgSramImaAccess
(
  UINT32  PmicDeviceIndex,
  BOOLEAN *bExceptionOccured
)
{
  EFI_STATUS       Status   = EFI_SUCCESS;
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;

  if(!bExceptionOccured)
    return EFI_INVALID_PARAMETER;

  /* After write completion clear access bit request i.e. RIF_MEM_ACCESS_REQ = 0 */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ, FG_MEMORY_ACCESS_REQ_FALSE);
  //err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

  /* Handle if IMA exception  */
  Status |= PmicFgSram_HandleIfImaException(PmicDeviceIndex, bExceptionOccured);

  /* Make sure to enable All byte write again in case previosuly APIs changed */
  err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);

  /* Request Single access */
  err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE);

  /* Set Sram module internal state */
  Status = PmicFgSram_SetState(FG_SRAM_STATUS_AVAILABLE);

  return (Status | err_flg );
}


/**
PmicFgSram_ReleaseFgSramDmaAccess()

@brief
Release Sram access, Clears memory access bit 
*/
EFI_STATUS PmicFgSram_ReleaseFgSramDmaAccess
(
  UINT32  PmicDeviceIndex
)
{
  EFI_STATUS       Status   = EFI_SUCCESS;
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;

  /* After write completion clear access bit request i.e. RIF_MEM_ACCESS_REQ = 0 */
  err_flg = pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ, FG_MEMORY_ACCESS_REQ_FALSE);
  err_flg |= pm_fg_memif_arb_ctl(PmicDeviceIndex, PM_FG_MEMIF_ARB_REQ, FALSE);
  err_flg |= pm_fg_memif_arb_ctl(PmicDeviceIndex, PM_FG_MEMIF_ARB_LO_LATENCY_EN, FALSE);

  /* Set Sram module internal state */
  Status = PmicFgSram_SetState(FG_SRAM_STATUS_AVAILABLE);

  return (Status | err_flg );
}

/**
PmicFgSram_RequestFgSramAccess()

@brief
Request Sram access, Sets memory access bit 
*/
EFI_STATUS PmicFgSram_RequestFgSramAccess
(
  UINT32  PmicDeviceIndex, pm_fg_mem_if_mem_intf_cfg  mem_intf_cfg
)
{
  EFI_STATUS       Status   = EFI_SUCCESS;
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;

  /*Enables RIF memory interface and the RIF Memory Access Mode.  1 */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, mem_intf_cfg, FG_MEMORY_ACCESS_REQ_TRUE);

  //Set Sram module internal state 
  Status = PmicFgSram_SetState(FG_SRAM_STATUS_IN_USE);

  return (Status |= err_flg);
}

/**
PmicFgSram_PollFgSramAccess()

@brief
Poll Sram memory access untill timeout or returns when memory access is permitted 
*/
EFI_STATUS PmicFgSram_PollFgSramAccess
(
  UINT32  PmicDeviceIndex, boolean * SramAccessStatus
)
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  boolean Mem_Available_status = FALSE;
  pm_err_flag_type err_flg = PM_ERR_FLAG_SUCCESS;
  uint16 Ttl_spent_time_in_polling = 0;
  BOOLEAN bExceptionOccured;

  *SramAccessStatus = FALSE;

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_POLLING);

  //Poll FG_MEM_AVAIL_RT_STS = 1 
  do{
      // mem available best time is 150 so first time this call is supposed to fail and successive call may return mem available status as TRUE
      err_flg |=  pm_fg_mem_if_irq_status(PmicDeviceIndex, PM_FG_MEM_IF_IMA_RDY, PM_IRQ_STATUS_RT, &Mem_Available_status);
      if ( TRUE == Mem_Available_status )
      {
        *SramAccessStatus = TRUE;
        //PMIC_FILE_DEBUG(( EFI_D_WARN, "PmicDxe: PmicFgSram_PollFgSramAccess PM_FG_MEM_IF_IMA_RDY : = (%d) \r\n", Mem_Available_status));
        break;
      }//check for error condition as we do not want to loop forever
      else if (Ttl_spent_time_in_polling >= FG_MEM_AVAILABLE_RT_STS_POLL_MAX_TIME)
      {
        *SramAccessStatus = FALSE;
        Status = EFI_DEVICE_ERROR;
        PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: PmicFgSram_PollFgSramAccess TimeOut : Ttl_spent_time_in_polling = (%d) \r\n", Ttl_spent_time_in_polling));
        break;
      }

      /*wait for 150 ms before querying mem available status again */
      gBS->Stall(FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME * 1000);

      Ttl_spent_time_in_polling += FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME;
  }while(TRUE);

  //Set Sram module internal state 
  if (FALSE == *SramAccessStatus){
    //Clear memory access bit request i.e. RIF_MEM_ACCESS_REQ = 0
    PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);

    //might need to read cycle streach bit and clear it here
    PmicFgSram_SetState(FG_SRAM_STATUS_AVAILABLE); //when time out for polling access request 
  }
  else 
    PmicFgSram_SetState(FG_SRAM_STATUS_IN_USE);

  return (Status |= err_flg);
}


/**
PmicFgSram_WriteProfileToSram()

@brief
Writes Profile Array to Sram
*/
EFI_STATUS PmicFgSram_WriteProfileToSram
(
  UINT32 PmicDeviceIndex,
  FgBattAddrData profileArray[BATTARY_PROFILE_MAX_SIZE]
)
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;
  UINT16           CurrSramProfileAddress = FG_SRAM_START_DMA_PROFILE_ADDR + FG_SRAM_START_DMA_PROFILE_OFFSET;
  UINT8            NoOFWordToWrite = FG_SRAM_PROFILE_END_ADDR - FG_SRAM_PROFILE_START_ADDR;

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_LOAD_PROFILE);

  /* Do Burst write */
  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Loading Profile to FG SRAM NoOFWordToWrite = %d ProfileAddr = 0x%x \r\n", __FUNCTION__,NoOFWordToWrite, CurrSramProfileAddress));

  Status = PmicFgSram_DmaWriteBurst(PmicDeviceIndex, CurrSramProfileAddress, NoOFWordToWrite, profileArray);

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Loading Profile to FG SRAM Complete \r\n", __FUNCTION__));

  /* For debug */
  Status |= PmicFgSram_ValidateProfileWrite(PmicDeviceIndex, profileArray);

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_IN_USE);

  return (Status |= err_flg);

}


/**
PmicFgSram_ValidateProfileWrite()

@brief
Validates Profile Array flashed to Sram
*/
EFI_STATUS PmicFgSram_ValidateProfileWrite(UINT32 PmicDeviceIndex,
                           FgBattAddrData profileArray[BATTARY_PROFILE_MAX_SIZE])
{
  EFI_STATUS       Status  = EFI_SUCCESS;
  UINT16           CurrSramProfileAddress = FG_SRAM_PROFILE_START_ADDR;
  UINT32           ChkCounter = 0, MemCmpCnt = 0;
  BOOLEAN          ProfileValidWrite = FALSE;
  UINT32           RdprofileArray[BATTARY_PROFILE_MAX_SIZE/FG_SRAM_ADDR_INCREMENT];

  /* memset Read Array to zero */
  SetMem(RdprofileArray, sizeof(UINT32) *(BATTARY_PROFILE_MAX_SIZE/FG_SRAM_ADDR_INCREMENT), 0x00);

  Status = PmicFgSram_ImaReadBurst(PmicDeviceIndex, CurrSramProfileAddress,
                                    FG_SRAM_PROFILE_END_ADDR - FG_SRAM_PROFILE_START_ADDR,
                                    RdprofileArray);

  if(Status != EFI_SUCCESS)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a PmicFgSram_ImaReadBurst Error = %d \r\n", __FUNCTION__, Status));
    return EFI_DEVICE_ERROR;
  }

  for(ChkCounter = 0; ChkCounter <= (FG_SRAM_PROFILE_END_ADDR - FG_SRAM_PROFILE_START_ADDR); ChkCounter++)
  {
    if (0 == (RdprofileArray[ChkCounter] ^  profileArray[ChkCounter].Data) )
    {
      /* Nop*/
    }
    else
    {
      MemCmpCnt++;
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a RdprofileArray[%d] = 0x%x profileArray[%d].Data = 0x%x\r\n", __FUNCTION__,
                   ChkCounter, RdprofileArray[ChkCounter], ChkCounter, profileArray[ChkCounter].Data));
      //break; /* continue to know how many bytes mismatch we have */
    }
  }
  if (0 == MemCmpCnt){
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a memcmp Success count = %d \r\n", __FUNCTION__,ChkCounter));
    ProfileValidWrite = TRUE;
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a memcmp Failed at count = %d MemCmpCnt = %d \r\n",  __FUNCTION__,ChkCounter, MemCmpCnt));
    ProfileValidWrite = FALSE;
  }

  return Status;
}


/**
PmicFgSram_ValidateCheckSum

@brief 
Verifies Battery Profile Provided Checksum with battery profile data 
**/
EFI_STATUS PmicFgSram_ValidateCheckSum(FgBattProfile *Profile, BOOLEAN *bValid)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 chksum = 0, count = 0, byteCount = 0, ProfCnt = 0;
  int    cross = 0;
  long   lngDiv = 32768;

  if (NULL == bValid)
    return EFI_INVALID_PARAMETER;

  do
  {
    chksum = 0;
    //Validate check sum here for 128 bytes 32 * 4 
    for(count = 0; count < 56; count++)
    {
      /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: ReadData[%d] = (0x%x) \r\n", count, Profile->Pd[ProfCnt].addrData[count].Data)); */
      for(byteCount = 0; byteCount < 4; byteCount++)
      {
        if (chksum/lngDiv)
          cross = 1;
        else
          cross = 0;
        chksum = ((((chksum * 2) & 0xFFFF) + cross) ^ ((Profile->Pd[ProfCnt].addrData[count].Data >> (byteCount * NUM_BITS_IN_BYTE)) & 0xFF));
        /* PMIC_DEBUG(( EFI_D_INFO, "PmicDxe: sourceCount: (0x%x) = (0x%x) chksum = (0x%x)\r\n",(Profile->addrData[count].Data >> (byteCount * NUM_BITS_IN_BYTE)) & 0xFF,  chksum));*/
      }
    }

    if (chksum == Profile->Pd[ProfCnt].Pp.CheckSum)
    {
      *bValid = TRUE;
    }
    else
    {
      *bValid = FALSE;
      Status  = EFI_LOAD_ERROR;
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Profile Checksum = (0x%x) Calculated Checksum = (0x%x) incorrect for profile Count = %d \r\n",
                __FUNCTION__, Profile->Pd[ProfCnt].Pp.CheckSum, chksum, ProfCnt));
      break;
    }
  }while((++ProfCnt < Profile->ProfileCount ) && (ProfCnt < MAX_NUM_OF_BATT_PROFILE));

  return Status;

}


/**
PmicFgSram_UpdateRconnComp

@brief 
Update Rconn Compensasation for charge and discharge to FG 
**/
EFI_STATUS PmicFgSram_UpdateRconnComp(UINT32 PmicDeviceIndex)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32   ReadBtEsr             = 0;
  INT32    ReadBtEsrConv         = 0;
  UINT8    ReadDischgRslowSocth  = 0;
  UINT8    ReadchgRslowSocth     = 0;
  INT32    ReadDischgRslowSocthI = 0;
  INT32    ReadchgRslowSocthI    = 0;
  INT32    RslowDischgCorrected  = 0;
  INT32    RslowChgCorrected     = 0;
  UINT8    RslowDischgCorrectedU = 0;
  UINT8    RslowChgCorrectedU    = 0;
  UINT32   ReadProfileWord       = 0;
  UINT8    RconnCompByte         = 0;

  /* Read ESR */
  Status = PmicFgSram_ImaRead(PmicDeviceIndex, FG_SRAM_SP_BAT_ESR_ADDR, &ReadBtEsr);
  ReadBtEsrConv = ((ReadBtEsr & 0x3FFF ) * FG_SRAM_ESR_MAX_IN_OHMS)/ FG_SRAM_OHMS_MAX_ENCODING_14BIT_VALUE;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a FG_SRAM_SP_BAT_ESR_ADDR %d = 0x%x ReadBtEsrConv = %d mOhms \r\n",__FUNCTION__,FG_SRAM_SP_BAT_ESR_ADDR, ReadBtEsr, ReadBtEsrConv));

  if (0 == ReadBtEsrConv + gFgConfigData.RconnComp)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ReadBtEsrConv 0x%x + RconnComp = %d is zero \r\n",__FUNCTION__, ReadBtEsrConv, gFgConfigData.RconnComp));
    return EFI_SUCCESS;
  }

  /* Read Rslow charge and dis charge */
  Status |= PmicFgSram_ImaReadSingleOffset(PmicDeviceIndex, (UINT16)FG_SRAM_BP_DISCHG_RSLW_SOCTH_ADDR,
                                                 &ReadDischgRslowSocth, (UINT8)FG_SRAM_BP_DISCHG_RSLW_SOCTH_ADDR_OFFSET );
  Status |= PmicFgSram_ImaReadSingleOffset(PmicDeviceIndex, (UINT16)FG_SRAM_BP_CHG_RSLW_SOCTH_ADDR,
                                                 &ReadchgRslowSocth, (UINT8)FG_SRAM_BP_CHG_RSLW_SOCTH_ADDR_OFFSET);
  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a DischgRslowAddr %d = 0x%x, chgRslowAddr %d = 0x%x \r\n",__FUNCTION__, FG_SRAM_BP_DISCHG_RSLW_SOCTH_ADDR, ReadDischgRslowSocth,
                                                                            FG_SRAM_BP_CHG_RSLW_SOCTH_ADDR, ReadchgRslowSocth));

  ReadDischgRslowSocthI = (INT32)ReadDischgRslowSocth;
  ReadchgRslowSocthI    = (INT32)ReadchgRslowSocth;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ReadDischgRslowSocthI = %d, ReadchgRslowSocthI = %d \r\n",__FUNCTION__, ReadDischgRslowSocthI , ReadchgRslowSocthI));

  RslowDischgCorrected = (ReadDischgRslowSocth * ReadBtEsrConv )/ (ReadBtEsrConv + gFgConfigData.RconnComp); 
  RslowChgCorrected    = (ReadchgRslowSocth * ReadBtEsrConv )/ (ReadBtEsrConv + gFgConfigData.RconnComp);

  RslowDischgCorrectedU = (UINT8) RslowDischgCorrected;
  RslowChgCorrectedU =  (UINT8)RslowChgCorrected;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a RslowDischgCorrected = %d, RslowChgCorrected = %d \r\n",__FUNCTION__, RslowDischgCorrected , RslowChgCorrected));
  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a RslowDischgCorrectedU = %d, RslowChgCorrectedU = %d \r\n",__FUNCTION__, RslowDischgCorrectedU , RslowChgCorrectedU));

  /* Override encoded values back into original locations */
  Status |= PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_BP_DISCHG_RSLW_SOCTH_ADDR,
                                           (UINT8)RslowDischgCorrected, FG_SRAM_BP_DISCHG_RSLW_SOCTH_ADDR_OFFSET, FG_SRAM_BP_DISCHG_RSLW_SOCTH_REG_MASK);

  Status |= PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_BP_CHG_RSLW_SOCTH_ADDR,
                                           (UINT8)RslowChgCorrected, FG_SRAM_BP_CHG_RSLW_SOCTH_ADDR_OFFSET, FG_SRAM_BP_CHG_RSLW_SOCTH_REG_MASK);

  /* Update Profile Integrity Rconn Bit (79 word byte 0 bit 0) */

  /* Read Profile word */
  Status = PmicFgSram_ImaRead(PmicDeviceIndex, FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR, &ReadProfileWord);

  RconnCompByte = ReadProfileWord >> PM_FG_SHIFT_3_BYTES;
  RconnCompByte = RconnCompByte | FG_SRAM_SP_RCONNCOMP_BIT_VALUE;

  /* update byte 0*/
  if(EFI_SUCCESS == Status)
  {
    Status |= PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR,
                                           RconnCompByte, FG_SRAM_PROFILE_INTEGRITY_ADDR_RCONN_OFFSET, FG_SRAM_PROFILE_INTEGRITY_RCONN_MASK);
  }
  else
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Rconn update Failed = %rx \r\n",__FUNCTION__, Status));
  }

  return Status;
}


/**
PmicFgSram_GetHALFEncode

@brief 
This function takes a fixed point value which is a floating poing value * 1000, e.g.
input value of 125 means 0.125 and 134045 means 134.045 and convert it to a 16 bit half
float encoded integer value.

**/
EFI_STATUS PmicFgSram_GetHALFEncode(INT32 value, UINT16* pEncodedValue)
{
  //in the fixedPointDecimalTextBox it is the floating point value * 1000
  INT32 integerValue = 0;
  INT32 exponent = 0;
  INT32 temp_value = 0;
  INT32 mantissa = 0;
  INT32 i = 0;
  EFI_STATUS status = EFI_SUCCESS;
  INT32 signBit = 0;

  if (pEncodedValue == NULL)
  {
    status = EFI_INVALID_PARAMETER;
    goto ExitFunction;
  }
  if (value < 0)
  {
    signBit = 1;
    value = (-value);
  }
  integerValue = value / 1000;
  if (integerValue > 0)
  {
    //5 bit for exponent, so bigest is 31.
    //if integerValue > 0, then n >= 0, so exponent start from 15
    for (i = 15; i <= 31; i++)
    {
      INT32 n = i - 15;
      temp_value = 1 << n;
      if (temp_value >= integerValue)
      {
        exponent = i;
        break;
      }
    }
    if (temp_value > integerValue)
    {
      //find the exponent that will make the 
      //2^(exp-15) one step smaller than intergerValue.
      exponent = i - 1;
    }
    mantissa = value - (1 << (exponent - 15)) * 1000;
    if (mantissa < 0)
    {
      status = EFI_INVALID_PARAMETER;
      goto ExitFunction;
    }
    //findout mantissa hex value
    //mantissa = remainder * 2^(10 - (expn - 15));
    if (exponent <= 25)
    {
      mantissa = mantissa * (1 << (25 - exponent));
    }
    else
    {
      mantissa = mantissa * 1000 / (1 << (exponent - 25));
      mantissa /= 1000;
    }
    mantissa = (mantissa + 1000 / 2) / 1000;
    
  }
  else
  {
    //it is less or =1
    for (i = 0; i < 15; i++)
    {
      temp_value = 1000 / (1 << i);
      if (temp_value <= value)
      {
        //we found the exponent and it is actually -exponent
        exponent = i;
        break;
      }
    }
    //findout the mantisa since exponent should be a negative value, so
    //since value is all less than 1000 here so no worry of overflowing.
    mantissa = value * 1000 - (1000 * 1000 / (1 << exponent));
    mantissa = mantissa  * (1 << (10 + exponent));

    mantissa /= 1000000;
    exponent = 15 - exponent;
  }

  *pEncodedValue = (UINT16)((exponent << 11) | (signBit << 10) | mantissa);
ExitFunction:
  return status;
}


/**
PmicFgSram_ReadBattParamCache()

@brief
Read Battery Parameter Cache data from FG SRAM 
*/
#if 0
EFI_STATUS PmicFgSram_ReadBattParamCache(UINT32 PmicDeviceIndex, FgBattParamCache *fgBattParamCache)
{
  EFI_STATUS   Status   = EFI_SUCCESS;
  UINT32       ReadData = 0;
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;
  if(!fgBattParamCache)
    return EFI_INVALID_PARAMETER;

  //Requet read access 
  err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FALSE);

  /*Read voltage and temperature*/
  err_flg |= PmicFgSram_ReadData(PmicDeviceIndex, FG_SRAM_TEMP_REG_ADDR, &ReadData);
  fgBattParamCache->FgBattParam_CachedTemperature = ReadData;

  err_flg |= PmicFgSram_ReadData(PmicDeviceIndex, FG_SRAM_VOLT_REG_ADDR, &ReadData);
  fgBattParamCache->FgBattParam_CachedBattVoltage = ReadData;

  /* Update Temperature Read Value Flag */
  fgBattParamCache->FgBattParam_TemperatureValid = TRUE;
  fgBattParamCache->FgBattParam_VbattValid = TRUE;

  return Status;
}
#endif

/*This API overwrites the FG-SRAM scratch register with current RTC HRs if device has been shutdown for more than FgRestartHrs */
/*If FgRestartHrs and RtcExceedRestart are NULL then it will simply overwrite scratch register with current RTC HRs */
#if 0 
 /* TBD */
EFI_STATUS PmicFgSram_OverwriteRtcAndDeltaSoC(UINT32 PmicDeviceIndex, UINT32 *FgRestartHrs, BOOLEAN *RtcExceedRestart)
{
  pm_err_flag_type  err_flag   = PM_ERR_FLAG_SUCCESS;
  
  pm_hal_rtc_time_type  hal_time;
  UINT32 CurrentRtcHrs = 0;
  UINT32 SavedRtcDeltaSoC =0;
  UINT32 SavedRtcHrs = 0;
  UINT32 SavedDeltaSoc = 0;

  //read and store current RTC
  err_flag |= pm_rtc_hal_rtc_get_time(PM_DEVICE_0, &hal_time);
  CurrentRtcHrs = hal_time.sec / SECS_IN_HOUR;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: CurrentRtcHrs = %d \r\n", CurrentRtcHrs));

  //check for fuel gauge restart flag
  if (NULL != RtcExceedRestart && NULL != FgRestartHrs)
  {
    //read saved RTC from scratch register. Compare with with current RTC for restart condition
    err_flag |=  PmicFgSram_ReadData(PmicDeviceIndex, FG_SRAM_SCRATCH_REG_RTC, &SavedRtcDeltaSoC);
    SavedRtcHrs = SavedRtcDeltaSoC >> 16;
    SavedDeltaSoc = SavedRtcDeltaSoC & 0xFF;

    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: SavedRtcDeltaSoC = %d and SavedRtcHrs = %d \r\n", SavedRtcDeltaSoC, SavedRtcHrs));
    /*saved RTC should always be less than current RTC */
    SavedRtcHrs = (SavedRtcHrs > CurrentRtcHrs) ? CurrentRtcHrs : SavedRtcHrs;
    *RtcExceedRestart = ((CurrentRtcHrs - SavedRtcHrs) > *FgRestartHrs) ? TRUE : FALSE;
    /*If FG doesn't need to be re-started then updated only current RTC */
    SavedRtcDeltaSoC = (TRUE == *RtcExceedRestart) ? (CurrentRtcHrs << 16) : ((CurrentRtcHrs << 16) | SavedDeltaSoc);
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: RtcExceedRestart = %d \r\n", *RtcExceedRestart));
    /*overwrite stored RTC and SoC scratch register with current RTC Hrs*/
    err_flag |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, TRUE);
    err_flag |=  PmicFgSram_WriteData(PmicDeviceIndex, FG_SRAM_SCRATCH_REG_RTC, SavedRtcDeltaSoC);
    err_flag |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FALSE);
  }
  else
  {
    /*overwrite stored RTC and SoC scratch register with current RTC Hrs*/
    SavedRtcDeltaSoC = CurrentRtcHrs << 16;
    err_flag |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, TRUE);
    err_flag |=  PmicFgSram_WriteData(PmicDeviceIndex, FG_SRAM_SCRATCH_REG_RTC, SavedRtcDeltaSoC);
    err_flag |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FALSE);
  }

  return (EFI_STATUS)err_flag;
}
#endif


EFI_STATUS PmicFgSram_EnableESRPulse(UINT32 PmicDeviceIndex, BOOLEAN enable)
{
  EFI_STATUS       Status = EFI_SUCCESS;

  Status = PmicFgSram_ImaWriteSingleOffset(PmicDeviceIndex, 
    FG_SRAM_ESR_CFG_ADDR,
    enable, 
    FG_SRAM_ESR_CFG_ADDR_OFFSET,
    FG_SRAM_ESR_ENABLE_BIT_MASK);

  return Status;
}

#ifdef _MSC_VER
#pragma warning( default : 4244)
#endif
