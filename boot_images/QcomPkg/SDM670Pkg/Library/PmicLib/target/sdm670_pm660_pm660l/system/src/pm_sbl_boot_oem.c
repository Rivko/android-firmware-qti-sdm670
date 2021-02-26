/*! \file pm_sbl_boot_oem.c
*  \n
*  \brief This file contains PMIC device initialization function where initial PMIC
*  \n SBL settings are configured through the PDM auto-generated code.
*  \n
*  \n &copy; Copyright 2015-2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/18   ks      Configured S2 reset with DVDD_HR for CHIPINFO_FAMILY_QCS605 HP(CR2185381)
01/03/18   st      added pm_sbl_auto_power_on_check to detect false USBIN PON(CR2142179)
12/14/17   st      Updated Single phase buck PVC configuration for APC1 rail(CR2161803)
11/27/17   st      Added API to store external buck info into PMIC Spare register(CR2069770)
10/12/17   sv      Initialize FgSram in pm_sbl_chg_pre_init API.
09/13/17   sv      Added support for DBU1 and DBU2.
07/18/17   ks      RF_CLK2.CLKBUFF_HOLD_CTL  to 0x01 (CR 2076624 )
04/06/17   aab	   Updated pm_driver_post_init(): Turn off EUD Rail if VBUS is detected 
03/07/17   ds	   Removed the call to pm_aop_pre_init()
01/16/17   aab     Updated pm_driver_post_init() to support SDM845
06/01/16   aab     Updated pm_driver_post_init() to add RF_CLK Drive strength configuration if CDP is detected
04/29/15   aab     Updated pm_device_post_init() 
12/04/15   aab     Updated to support MSM8998 target 
12/04/15   aab     Creation
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_sbl_boot_target.h"
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "pm_sbl_boot.h"
#include "pm_device.h"
#include "pm_comm.h"
#include "pm_utils.h"
#include "pm_schg_usb.h"
#include "DDIChipInfo.h"
#include <stdio.h>
#include "boot_logger.h"
#include "CoreVerify.h"
#include "pm_ldo.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"
#include "pm_app_vreg.h"
#include "pm_target_information.h"
#include "pm_clk.h"
#include "pm_uefi.h"
#include "i2c_api.h"
#include "pm_fg_sram.h"
#include "SpmiBusCfg.h"
#include "SpmiCfg.h"
#include "pm_log_utils.h"
#include <string.h>
#include "pm_schg_misc.h"
#include "boothw_target.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
#define PM660L_LCD_VARIENT 0x2

#define DBU1_I2C_SLAVE_ADDR 0x61
#define DBU2_I2C_SLAVE_ADDR 0x60

#define DBU_VSEL0_REG_ADDR 0x00
#define DBU_VSEL1_REG_ADDR 0x01
#define DBU_VSEL_BOUT_MASK 0x7F

#define DBU_CTRL_REG_ADDR 0x02
#define DBU_CTRL_MODE0_MASK 0x01
#define DBU_CTRL_MODE1_MASK 0x02

#define DBU_MODE_APFM_VAL 0x00
#define DBU_MODE_FPWM_VAL 0x01

#define DBU_MODE0_SHIFT 0
#define DBU_MODE1_SHIFT 1

#define DBU_TYPE_HALO 0
#define DBU_TYPE_FAN 3


#define DBU_ID1_ADDR 0x03  // for vendor ID

#define FAN_VENDOR_ID  0x81   //FAN53526 vendor ID
#define PM_VENDOR_ID   0xA8   //PM3003A vendor ID

#define DBU_VSEL_1_1125V 0x52
#define DBU_VSEL_0_95V   0x38
//Voltage conversion formula
//BOUT value = ((VOUT-0.6)*1000)/6.25.  Ex: 0.95V = ((0.95-0.6)*1000)/6.25 = 56 = 0x38

#define I2C_DEFAULT_BUS_FREQ 400
#define SINGLEPHASE 0x03

#define MISC_PMIC_OPT_ADDR 0x95C  			// Address to read single phase or Dual phase buck configuration, this value will be written by PMIC OTP during bootup.
#define MISC_PMIC_CTL2_ADDR  0x958  		// Register Address to store Buck Type(Halo/Fan)
#define MISC_PMIC_CTL2_DBUTYPE_MASK 0x03
#define PMI_INDEX 0
#define APPS1_PORT 2
#define PON_PON_REASON1_USB_CHG 0x10
#define RD_ALTEK_SUBTYPE 0x05

static VOID *i2c_handle  = NULL;
static uint8 DbuPhaseConfig =0;
static uint8 g_dbuType=0;

/*** Added for supporting multi-card tray removal feature */
#define PM_MCT_PAIRING_SLAVE_ID                 2
#define PM_MCT_PAIRING_SD_CARD_SEPARATE         0
#define PM_MCT_PAIRING_SD_CARD_UIM1             1
#define PM_MCT_PAIRING_SD_CARD_UIM2             2
#define PM_MCT_PAIRING_SD_CARD_UIM1_UIM2        3
#define PM_MCT_PAIRING_INFO_PAIRING_MASK        3
#define PM_MCT_PAIRING_INFO_DEBOUNCE_MASK       0xFF
#define PM_MCT_PAIRING_INFO_DET_POLARITY_MASK   1
#define GPIO2_EN_CTL_MASK                       0x80
#define GPIO2_EN_CTL                            0xC146
#define SD_UICC_PAIRING_INFO   	                0xC11A //GPIO2_INT_MID_SEL  -> for setting pairing info
#define SD_CARD_DETECTION_POLARITY              0xC111 //GPIO2_INT_SET_TYPE -> for setting SD card Detection polarity (LOW/HIGH)

//De bounce registers which applied to GPIO on SD-Card ‘Removal’ event.
//Written as Number of Cycles of 19.2 MHz clock (~52 ns) Default: 0x03C0 = d’960 x 52ns = 50us
#define SD_REMOVAL_DEBOUNCE_19M_CYCLES_LSB      0x098C // PM660L.MISC.VSET5_LB
#define SD_REMOVAL_DEBOUNCE_19M_CYCLES_MSB      0x098D // PM660L.MISC.VSET5_UB

#define CLOCK_PERIOD         52  //19.2MHz
#define CLOCK_PERIOD_DENOM   1000



/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/

pm_err_flag_type pm_all_rails_on(void);
pm_err_flag_type pm_sbl_dbu_configuration();
pm_err_flag_type pm_store_ext_buck_type();
pm_err_flag_type pm_sbl_pvc_reconfiguration();
pm_err_flag_type pm_oem_pon_reset_cfg(void);
pm_err_flag_type pm_target_multi_card_tray_config (uint8 pairing_info, uint8 debounce, uint8 det_polarity);

pm_err_flag_type
pm_device_pre_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_sbl_specific_data_type *config_param_ptr = NULL;

  config_param_ptr = (pm_sbl_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_SPECIFIC_DATA);
  CORE_VERIFY_PTR(config_param_ptr);

  if(config_param_ptr->rfclk2_config_flag)  // RF_CLK for WLAN
  {
    err_flag |= pm_comm_write_byte(0, 0x5447, 0x0, 0);	// Do NOT follow pin control
    err_flag |= pm_comm_write_byte(0, 0x5448, 0x80, 0);	// Enable pull down for RF_CLK1 peripheral
  }

  return err_flag;
}




pm_err_flag_type
pm_device_post_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  err_flag  = pm_oem_pon_reset_cfg(); //  S1 S2 Reset configurations for different Reset sources
  err_flag |= pm_app_pon_reset_init();
  
  /*check for valid PON keypress */
  err_flag |= pm_app_pwrkey_long_press_check(PM_PON_PWRKEY_DBNC_CHK_AT_LOADER);
  
  /*** Place holder code to enable multi-card removal feature ***/
  //err_flag |= pm_target_multi_card_tray_config(PM_MCT_PAIRING_SD_CARD_UIM1_UIM2, 50, 1);
  
  return err_flag;
}




pm_err_flag_type
pm_driver_pre_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  return err_flag;
}




pm_err_flag_type
pm_driver_post_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_device_info_type PmicDeviceInfo;
  uint8 pmic_variant;

  DalPlatformInfoPlatformType platform_type = DALPLATFORMINFO_TYPE_UNKNOWN;
  platform_type = DalPlatformInfo_Platform();

  pm_sbl_specific_data_type *config_param_ptr = NULL;

  config_param_ptr = (pm_sbl_specific_data_type *)pm_target_information_get_specific_info(PM_PROP_SBL_SPECIFIC_DATA);
  CORE_VERIFY_PTR(config_param_ptr);

  if(config_param_ptr->rfclk2_config_flag)
  {
    err_flag |= pm_clk_sw_enable(0, PM_CLK_RF_1, PM_OFF); // Turn off WLAN RF_CLK
  }
  
  pm_get_pmic_variant(PM_DEVICE_1, &pmic_variant);
  err_flag = pm_get_pmic_info(PM_DEVICE_1, &PmicDeviceInfo);
  if( ( PMIC_IS_PM660L == PmicDeviceInfo.ePmicModel) && (PM660L_LCD_VARIENT == pmic_variant) && (1 == PmicDeviceInfo.nPmicAllLayerRevision) && (0 == PmicDeviceInfo.nPmicMetalRevision) )
  {
     pm_log_message("PM660L 1.0 Have LCDB HW issue.. ");
     return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  err_flag |= pm_app_vreg_clear_ocp_status();
  
  //WA: set RF_CLK2.CLKBUFF_HOLD_CTL  to 0x01
  err_flag |= pm_comm_write_byte_mask(0, 0x5545, 0xFF, 0x01, 0);
  
  if(PM_ERR_FLAG__SUCCESS != pm_sbl_dbu_configuration())
  {
       pm_log_message("DBU: Failed to Configure DBU1 and DBU2 "); 
  }
  if(DbuPhaseConfig == SINGLEPHASE)
  {
    err_flag |= pm_sbl_pvc_reconfiguration();
  
	if( err_flag != PM_ERR_FLAG__SUCCESS)
	{
	   pm_log_message("PM: Failed to Configure APC1 PVC Port "); 
	}
  }
  return err_flag;
}



pm_err_flag_type
pm_sbl_chg_pre_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  PmicFgSram_Init(PMI_INDEX);
  /*  Start battery id conversion in continuous mode.
      Next image (UEFI) would need to stop this mode */
  err_flag = pm_sbl_set_batt_id_forced_mode(TRUE);
  
  return err_flag;
}


pm_err_flag_type
pm_sbl_chg_post_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  return err_flag;
}


pm_err_flag_type pm_sbl_i2c_buck_init()
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  i2c_status istatus = I2C_SUCCESS;
  /* Initialize the I2C Core for external bucks DBU1 and DBU2 control*/
  istatus = i2c_open((i2c_instance) I2C_INSTANCE_011, &i2c_handle);  // I2C_INSTANCE_011 is for DBU1 and DBU2 55/56 GPIO's
  if (I2C_SUCCESS != istatus)
  {
    pm_log_message("Failed to initialize I2C");
    err_flag = PM_ERR_FLAG_BUS_ERR;
  }
  return err_flag;
}

pm_err_flag_type pm_sbl_i2c_buck_exit()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    if (I2C_SUCCESS != i2c_close(i2c_handle))
    {
      pm_log_message("Failed to DeInitialize I2C ");
      err_flag = PM_ERR_FLAG_BUS_ERR;
    }
    return err_flag;
}
  
pm_err_flag_type i2c_buck_write_mask(uint8 slaveAddress, uint8 offset, uint8 mask,  uint8 data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  i2c_status istatus = I2C_SUCCESS;
  UINT32    bytes_written    = 0;
  UINT32    bytes_read =0;
  UINT8     writebuf =0;
  UINT8     readbuf =0;
  i2c_slave_config cfg;
  
  cfg.bus_frequency_khz = I2C_DEFAULT_BUS_FREQ;
  cfg.slave_address = slaveAddress;
  cfg.mode = I2C;
  cfg.slave_max_clock_stretch_us = 500;
  cfg.core_configuration1 = 0;
  cfg.core_configuration2 = 0;
  
  
  istatus = i2c_read (i2c_handle, &cfg, offset, 1, &readbuf, 1, &bytes_read, 2500);
  if (I2C_SUCCESS != istatus)
  {
    pm_log_message("I2C Read Failed");
    err_flag = PM_ERR_FLAG_BUS_ERR;
  }
  
  readbuf &= (~mask) ; 
  writebuf = (readbuf|(data & mask));
   
  istatus = i2c_write (i2c_handle, &cfg, offset, 1, &writebuf, 1, &bytes_written, 2500);
  if (I2C_SUCCESS != istatus)
  {
    pm_log_message("I2C Write Failed");
    err_flag = PM_ERR_FLAG_BUS_ERR;
  }
  
  return err_flag;
}

pm_err_flag_type i2c_buck_read(uint8 slaveAddress, uint8 offset, uint8 *data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  i2c_status istatus = I2C_SUCCESS;
  UINT32    bytes_read =0;
  UINT8     readbuf =0;
  i2c_slave_config cfg;
  
  cfg.bus_frequency_khz = I2C_DEFAULT_BUS_FREQ;
  cfg.slave_address = slaveAddress;
  cfg.mode = I2C;
  cfg.slave_max_clock_stretch_us = 500;
  cfg.core_configuration1 = 0;
  cfg.core_configuration2 = 0;
  
  if(NULL == data)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  istatus = i2c_read (i2c_handle, &cfg, offset, 1, &readbuf, 1, &bytes_read, 2500);
  if (I2C_SUCCESS != istatus)
  {
    pm_log_message("I2C Read Failed");
    err_flag = PM_ERR_FLAG_BUS_ERR;
  }
  *data = readbuf;
  return err_flag;
}


pm_err_flag_type pm_sbl_dbu1_config()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    err_flag |= i2c_buck_write_mask(DBU1_I2C_SLAVE_ADDR, DBU_VSEL1_REG_ADDR, DBU_VSEL_BOUT_MASK,  DBU_VSEL_1_1125V);                       //configure VSEL1 = 1.1125V
    err_flag |= i2c_buck_write_mask(DBU1_I2C_SLAVE_ADDR, DBU_CTRL_REG_ADDR,  DBU_CTRL_MODE1_MASK, (DBU_MODE_APFM_VAL<<DBU_MODE1_SHIFT) );  //configure MODE1 = APFM

    //Configure GPIO1 as output mode and make it high
    err_flag  |= pm_comm_write_byte(2, 0xC040, 0x01, 0);                                                                        // configure PM660L GPIO1 to OUTPUT Mode
    err_flag  |= pm_comm_write_byte(2, 0xC044, 0x80, 0);                                                                        // configure PM660L GPIO1 to high for VSEL=1

    err_flag |= i2c_buck_write_mask(DBU1_I2C_SLAVE_ADDR, DBU_VSEL0_REG_ADDR, DBU_VSEL_BOUT_MASK, DBU_VSEL_0_95V);                          //configure VSEL0 = 0.95V
    err_flag |= i2c_buck_write_mask(DBU1_I2C_SLAVE_ADDR, DBU_CTRL_REG_ADDR, DBU_CTRL_MODE0_MASK, (DBU_MODE_APFM_VAL<<DBU_MODE0_SHIFT) );   //configure MODE0 = APFM
    
    return err_flag;

}

pm_err_flag_type pm_sbl_dbu2_config()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 dbu2Status=0;
    
    err_flag  = pm_comm_read_byte(0, 0x095C, &dbu2Status, 0);         // Read MISC register to know DBU presence
    
    if(dbu2Status == SINGLEPHASE)
    {
        pm_log_message("DBU Log: This is a Single phase configuration, DBU2 not exists");
        return err_flag; // sigle phase configuration did not support DBU2, So retun success
    }
    err_flag |= i2c_buck_write_mask(DBU2_I2C_SLAVE_ADDR, DBU_VSEL0_REG_ADDR, DBU_VSEL_BOUT_MASK, DBU_VSEL_1_1125V);  //configure VSEL0 = 1.1125V
    err_flag |= i2c_buck_write_mask(DBU2_I2C_SLAVE_ADDR, DBU_CTRL_REG_ADDR, DBU_CTRL_MODE0_MASK, (DBU_MODE_APFM_VAL<<DBU_MODE0_SHIFT) );  //configure MODE0 = APFM

    err_flag |= i2c_buck_write_mask(DBU2_I2C_SLAVE_ADDR, DBU_VSEL1_REG_ADDR, DBU_VSEL_BOUT_MASK, DBU_VSEL_1_1125V);  //configure VSEL1 = 1.1125V
    err_flag |= i2c_buck_write_mask(DBU2_I2C_SLAVE_ADDR, DBU_CTRL_REG_ADDR, DBU_CTRL_MODE1_MASK, (DBU_MODE_FPWM_VAL<<DBU_MODE1_SHIFT) );  //configure MODE1 = FPWM
    
    //DBU2 : GPIO12B to be in output mode
    err_flag  |= pm_comm_write_byte(2, 0xCB40, 0x1, 0);  // GPIO12B to output mode
    return err_flag;
}


//Test code start -->
#define PM_DBU_MSG_LEN 110
void print_dbu_type()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 vendorId1 = 0;
    uint8 vendorId2 = 0;
	static char temp_str1[PM_DBU_MSG_LEN] = "";
    err_flag = i2c_buck_read(DBU1_I2C_SLAVE_ADDR, DBU_ID1_ADDR, &vendorId1);      //Read DBU1 VENDOR_ID 
    
    if(err_flag!= PM_ERR_FLAG__SUCCESS)
    {
       pm_log_message("DBU: I2C Failed to read Vendor ID");   
    }
    else
    {
       if(vendorId1 == PM_VENDOR_ID)
       {
           pm_log_message("DBU: DBU1 is HALO buck");
		   g_dbuType = DBU_TYPE_HALO;
       }
       else if(vendorId1 == FAN_VENDOR_ID)
       {
           pm_log_message("DBU: DBU1 is FAN buck");
		   g_dbuType = DBU_TYPE_FAN;
       }
       if(DbuPhaseConfig != SINGLEPHASE)
       {
           err_flag |= i2c_buck_read(DBU2_I2C_SLAVE_ADDR, DBU_ID1_ADDR, &vendorId2);      //Read DBU2 VENDOR_ID
           if(err_flag!= PM_ERR_FLAG__SUCCESS)
           {
              pm_log_message("DBU: DBU2 I2C Failed to read Vendor ID");   
           }
           else
           {
               if(vendorId2 == PM_VENDOR_ID)
               {
                   pm_log_message("DBU: DBU2 is HALO buck");
               }
               else if(vendorId2 == FAN_VENDOR_ID)
               {
                   pm_log_message("DBU: DBU2 is FAN buck");
               }
           }
       }
	   snprintf(temp_str1, PM_DBU_MSG_LEN, "DBU log: DBU1 vendor ID: 0x%2x, DBU2 vendor ID: 0x%2x",vendorId1, vendorId2 );
	   pm_log_message(temp_str1);
    }
	
}

void print_dbu_status()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    static char temp_str1[PM_DBU_MSG_LEN] = "";

    uint8 dbu1_mode = 0;
    uint8 dbu1_vsel0 = 0;
    uint8 dbu1_vsel1 = 0;
   
    uint8 dbu2_mode = 0;
    uint8 dbu2_vsel0 = 0;
    uint8 dbu2_vsel1 = 0;
  
    pm_log_message("DBU: Printing DBU1 and DBU2 status");   
    err_flag = i2c_buck_read(DBU1_I2C_SLAVE_ADDR, DBU_VSEL1_REG_ADDR, &dbu1_vsel1);
    err_flag = i2c_buck_read(DBU1_I2C_SLAVE_ADDR, DBU_VSEL0_REG_ADDR, &dbu1_vsel0);
    err_flag = i2c_buck_read(DBU1_I2C_SLAVE_ADDR, DBU_CTRL_REG_ADDR, &dbu1_mode);
    snprintf(temp_str1, PM_DBU_MSG_LEN, "DBU1 vsel1: 0x%2x, vsel0: 0x%2x, mode: 0x%2x ", dbu1_vsel1,dbu1_vsel0,dbu1_mode);
    pm_log_message(temp_str1);
    if(DbuPhaseConfig != SINGLEPHASE)
    {
      err_flag = i2c_buck_read(DBU2_I2C_SLAVE_ADDR, DBU_VSEL1_REG_ADDR, &dbu2_vsel1);
      err_flag = i2c_buck_read(DBU2_I2C_SLAVE_ADDR, DBU_VSEL0_REG_ADDR, &dbu2_vsel0);
      err_flag = i2c_buck_read(DBU2_I2C_SLAVE_ADDR, DBU_CTRL_REG_ADDR, &dbu2_mode);
      snprintf(temp_str1, PM_DBU_MSG_LEN, "DBU2 vsel1: 0x%2x, vsel0: 0x%2x, mode: 0x%2x", dbu2_vsel1, dbu2_vsel0, dbu2_mode);
      pm_log_message(temp_str1);
    }

}
// Test code end  <------

pm_err_flag_type pm_sbl_dbu_configuration()
{
	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    err_flag = pm_sbl_i2c_buck_init();   //initialise the I2C
    if(err_flag!= PM_ERR_FLAG__SUCCESS)
    {
       pm_log_message("DBU: Failed to Initialise I2C "); 
       return err_flag;
    }
    err_flag |= pm_sbl_dbu1_config();    //Configure DBU1
    
     //check for single phase or dual phase buck
    err_flag  |= pm_comm_read_byte(0, MISC_PMIC_OPT_ADDR, &DbuPhaseConfig, 0);
    if(err_flag != PM_ERR_FLAG__SUCCESS)
    {
        pm_log_message("DBU: Failed to read buck configuration "); 
        return err_flag;
    }
    
    if(DbuPhaseConfig != SINGLEPHASE)
    {
    err_flag |= pm_sbl_dbu2_config();    //Configure DBU2
    }
    
    print_dbu_type();
    print_dbu_status();
	err_flag |= pm_store_ext_buck_type(); 			// Store DBU type in Misc register
    err_flag |= pm_sbl_i2c_buck_exit();  			        //Deinitilise I2C
    return err_flag;
}

/*** Store the Type of DBU to MISC register 0x0958
MISC_PMIC_CTL2_ADDR:
_________________________________________
|  ---  |         |         | DBU  Type |
|___|___|____|____|____|____|_____|_____|

      DBU Type    
00 -> Halo       
11 -> FAN        
***/
pm_err_flag_type pm_store_ext_buck_type(){
	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    
	//Write the DBU TYPE in MISC_PMIC_CTL2_ADDR Register
	err_flag |= pm_comm_write_byte_mask(0, MISC_PMIC_CTL2_ADDR, MISC_PMIC_CTL2_DBUTYPE_MASK, g_dbuType, 0);
	return err_flag;
}
/* Allow runtime PVC Port configuration for APC1 rail and SPMI Configuration for DDR rail based upon phase(SINGLEPHASE/DUALPHASE)
	APC1 SINGLEPHASE S2A                
		 DUALPHASE   S3A
*/
pm_err_flag_type pm_sbl_pvc_reconfiguration(){
	pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
	uint32 index = 0;
	SpmiCfg_PvcPortCfg** pvc_port_cfg_ptr  = NULL;
	uint32* pvc_port_cfg_size              = NULL;
	SpmiCfg_ChannelCfg* spmi_cfg_ptr	   = NULL;
	uint32* spmi_cfg_size                  = NULL;
	
	pvc_port_cfg_ptr = (SpmiCfg_PvcPortCfg**)pm_target_information_get_specific_info(PM_PROP_PVC_PORT_CFG);
	pvc_port_cfg_size = (uint32*)pm_target_information_get_specific_info(PM_PROP_PVC_PORT_CFG_SIZE);
	
	if(pvc_port_cfg_ptr == NULL || pvc_port_cfg_size == NULL){
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }
	
	spmi_cfg_ptr = (SpmiCfg_ChannelCfg*)pm_target_information_get_specific_info(PM_PROP_SPMI_CHANNEL_CFG);
    spmi_cfg_size = (uint32*)pm_target_information_get_specific_info(PM_PROP_SPMI_CHANNEL_CFG_SIZE);
    
	if(spmi_cfg_ptr == NULL || spmi_cfg_size == NULL){
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

	//Update the DDR rail ownership to AOP (S3A_CTRL)
	for(index = 0; index < spmi_cfg_size[0]; index++){
		if((spmi_cfg_ptr[index].slaveId == 1) && (spmi_cfg_ptr[index].periphId == 0x1A )){
			spmi_cfg_ptr[index].irqOwnerMask    = SPMI_OWNER_AOP;
			spmi_cfg_ptr[index].periphOwnerMask = SPMI_OWNER_AOP;
			break;
		}
	}
	//Re-configure PVC Port of APC1
	for(index = 0; index < pvc_port_cfg_size[0]; index++){
		if(pvc_port_cfg_ptr[index] != NULL){
			if(pvc_port_cfg_ptr[index]->pvcPortId == APPS1_PORT){
				//Update the Ppid array with S2A register address
				pvc_port_cfg_ptr[index]->ppids[0].address = 0x1740;
				pvc_port_cfg_ptr[index]->ppids[1].address = 0x1741;
				pvc_port_cfg_ptr[index]->ppids[2].address = 0x1745;
				pvc_port_cfg_ptr[index]->ppids[3].address = 0x1746;
				break;
			}
		}else{
			return PM_ERR_FLAG_INVALID_PARAMETER;
		}
	}
	
	return err_flag;
}
//WA: [8998/SDM660]WA for Erroneous USBIN auto-power-on(CR2142179) 
pm_err_flag_type 
pm_app_auto_power_on_check(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_pon_reason_type pon_reason;
   pm_pon_on_state_reason_type on_reason;
   pm_schg_misc_sysok_reason_type sysok_reason;
   boolean usbin_status = FALSE;
   
   //If the SYS_OK reason is wi-power(DC-IN), then skip this WA
   err_flag = pm_schg_misc_get_sysok_reason(device_index, &sysok_reason);
   if((PM_ERR_FLAG__SUCCESS == err_flag) && (sysok_reason  == PM_SCHG_MISC_SYSOK_REASON__DCIN))
   {
	   return PM_ERR_FLAG__SUCCESS;
   }
  
   err_flag = pm_pon_get_pon_reason(device_index, &pon_reason);
   err_flag |=  pm_pon_get_on_reason(device_index, &on_reason);
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
       pm_log_message("ERROR, pm_pon_get_pon_reason()");
       return err_flag;
   }		   		   
   
   if(((*((uint8*)&pon_reason))==PON_PON_REASON1_USB_CHG) && (on_reason.warm_seq == FALSE))
   {
       err_flag |= pm_schg_usb_irq_status(device_index, PM_SCHG_USB_IRQ_USBIN_UV, PM_IRQ_STATUS_RT, &usbin_status);    
       if (err_flag != PM_ERR_FLAG_SUCCESS)
       {
           pm_log_message("ERROR, pm_schg_usb_irq_status()");
           return err_flag;
       }		   		   
       if(usbin_status == TRUE)
       {
           pm_log_message("ERROR, Device is shutting down due to false USBIN power on!");
           boot_hw_powerdown();
       }
   }
  
   return err_flag;
}

pm_err_flag_type pm_oem_pon_reset_cfg()
 {
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  DalDeviceHandle *phPlatformInfoDevice;
  DalChipInfoFamilyType eChipInfoFamilyType;
  DalPlatformInfoPlatformInfoType platformInfo = {DALPLATFORMINFO_TYPE_UNKNOWN,};
  DALResult result;
  pm_app_pon_reset_cfg_type reset_type = PM_APP_PON_CFG_WARM_RESET;
  /* Get the chip family - this is primarily needed to confirm that CDT types are */
  eChipInfoFamilyType = DalChipInfo_ChipFamily();
  if (eChipInfoFamilyType == CHIPINFO_FAMILY_QCS605)
  {
    /* Get the platform info */
    result = DAL_PlatformInfoDeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfoDevice);
    if (result != DAL_SUCCESS)
    {
      pm_log_message("pm_overrider_reset_cfg_oem, ERROR DALAttach Failed");
      err_flag |= (pm_err_flag_type)result;
    }
    else
    {
      result = DalPlatformInfo_GetPlatformInfo(phPlatformInfoDevice, &platformInfo);
      if (result != DAL_SUCCESS)
      {
        pm_log_message("pm_overrider_reset_cfg_oem, ERROR DALPlatform Failed");
        err_flag |= (pm_err_flag_type)result;
      }
    }
    if(DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && RD_ALTEK_SUBTYPE == platformInfo.subtype)
    {
      reset_type = PM_APP_PON_CFG_DVDD_HARD_RESET; 
    }
  }
  
    //These configurations is only used for development phones and should be commented out for production phones
  err_flag |= pm_app_pon_pshold_cfg(PM_APP_PON_CFG_WARM_RESET);
  err_flag |= pm_app_pon_reset_cfg( PM_APP_PON_RESET_SOURCE_KPDPWR, reset_type, 10256, 2000); //PON KPDPWR PON Reset configuration
  err_flag |= pm_app_pon_reset_cfg( PM_APP_PON_RESET_SOURCE_RESIN_AND_KPDPWR, PM_APP_PON_CFG_DVDD_HARD_RESET, 10256, 2000); //PON RESIN_AND_KPDPWR PON Reset configuration
  
  return err_flag;
 }
 
/**
* pm_target_multi_card_tray_config configures the scratch registers to configure the OEM multi-card tray settings
* These settings are used in PBS RAM to handle the UICC/SDCARD removal triggers to shut down the LDOs accordingly
*
* INPUT-1: pairing_info
*           00 - Default config, SD card is handles separately
*                 01 - SD card is paired with UIM1
*                 01 - SD card is paired with UIM2
*                 01 - SD card is paired with UIM1 and UIM2
* INPUT-2: debounce  
*                 debounce delay in micro seconds.
* INPUT-3: det_polarity
*                 0 - Low
*                 1 - High
*
* Return: This function returns SPMI errors if any during communication to PMIC
* 
* Example code:
* To enable MCT when paired with UIM._UIM2_SDCARD
* err_flag |= pm_target_multi_card_tray_config(PM_MCT_PAIRING_SD_CARD_UIM1_UIM2, 50, 0); 
*/
pm_err_flag_type pm_target_multi_card_tray_config (uint8 pairing_info, uint8 debounce, uint8 det_polarity)
{
      pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
      uint16 DbCycles = 0;
      uint8  DbCyclesLSB =0;
      uint8  DbCyclesMSB =0;
      
      err_flag |= pm_comm_write_byte_mask(PM_MCT_PAIRING_SLAVE_ID, GPIO2_EN_CTL, GPIO2_EN_CTL_MASK, 0x00, 1);  // Disable GPIO2
      err_flag |= pm_comm_write_byte_mask (PM_MCT_PAIRING_SLAVE_ID, SD_CARD_DETECTION_POLARITY, PM_MCT_PAIRING_INFO_DET_POLARITY_MASK, det_polarity, 1);
      
      //Programming Debounce delay = (Debounce delay in us) / (Clock cycle time in us)
      DbCycles = (uint16) ((uint32)debounce * CLOCK_PERIOD_DENOM) / CLOCK_PERIOD;
      DbCyclesLSB = (DbCycles & 0xFF);
      DbCyclesMSB = (DbCycles >>8);
      err_flag |= pm_comm_write_byte_mask (PM_MCT_PAIRING_SLAVE_ID, SD_REMOVAL_DEBOUNCE_19M_CYCLES_LSB, PM_MCT_PAIRING_INFO_DEBOUNCE_MASK, DbCyclesLSB, 1);
      err_flag |= pm_comm_write_byte_mask (PM_MCT_PAIRING_SLAVE_ID, SD_REMOVAL_DEBOUNCE_19M_CYCLES_MSB, PM_MCT_PAIRING_INFO_DEBOUNCE_MASK, DbCyclesMSB, 1);
      err_flag |= pm_comm_write_byte_mask (PM_MCT_PAIRING_SLAVE_ID, SD_UICC_PAIRING_INFO, PM_MCT_PAIRING_INFO_PAIRING_MASK, pairing_info, 1);
      
      err_flag |= pm_comm_write_byte_mask(PM_MCT_PAIRING_SLAVE_ID, GPIO2_EN_CTL, GPIO2_EN_CTL_MASK, 0x80, 1);  // Enable GPIO2
      return err_flag;
}
 
 
