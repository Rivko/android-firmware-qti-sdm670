#ifndef __DDR_SDI_H__
#define __DDR_SDI_H__

/*=============================================================================

                              DDR HAL
                            Header File
GENERAL DESCRIPTION
This is the header file that describe the DDR status registers to be logged during clock/voltage switch.
this can be extended to log any other BIMC/PHY registers that aid in debugging .

Copyright (c) 2017 Qualcomm Technologies, Inc. 
All rights reserved.


===========================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/02/17   rp      Initial revision.
=============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/

/*==============================================================================
                                  MACROS
==============================================================================*/
#define SDI_REG_LOG_START_ADDR 0x146BF300

#define SDI_COOKIES_STRUCT_BASE 0x146BF610
#define PASS1_COOKIE            0x146BF634
#define PASS2_COOKIE            0x146BF638
#define PASS3_COOKIE            0x146BF63C

/*==============================================================================
                                  DATA
==============================================================================*/

extern uint64 ddr_sdi_reg_log_addr[];

typedef enum
{
  FREEZE_IO_NOT_SET_WDOG_SR_SET = 0xDEADBAE0,
  FREEZE_IO_NOT_SET_WDOG_SR_NOT_SET = 0xDEADBAE1,
  DDR_WENT_INTO_WDOG_SR = 0xCAFED00D,
  DDR_BROUGHT_OUT_OF_SR = 0xCAB0CAFE,
  DDR_NOT_BROUGHT_OUT_OF_SR = 0xCAB0DEAD,
  DDR_TEST_PASSED = 0xCAFE1111,
  DDR_TEST_FAILED = 0xDEAD0000,
  DDR_FULLY_INIT_PASS2_START = 0xDEADBABE,
  DDR_FULLY_INIT_PASS2_COMPLETE = 0xDEADD00D,
  DDR_RECOVER_PASS2_START = 0xCAFECAB0,
  DDR_RECOVER_PASS2_COMPLETE = 0xCAFEBABE,
  DDR_WENT_INTO_PASS3 = 0xDEADDEAD,
  UNUSED = 0xFFFFFFFF,
} SDI_COOKIE_VALUES;

typedef struct
{
  /* CABO GLOBAL registers */

  uint32 CABO0_CABO_GLOBAL_IDLE_STATUS;
  uint32 CABO1_CABO_GLOBAL_IDLE_STATUS;

  uint32 CABO0_CABO_GLOBAL_DEVICE_STATUS;
  uint32 CABO1_CABO_GLOBAL_DEVICE_STATUS;

  uint32 CABO0_CABO_GLOBAL_ISU_ACH_STATUS;
  uint32 CABO1_CABO_GLOBAL_ISU_ACH_STATUS;

  uint32 CABO0_CABO_GLOBAL_ISU_WCH_STATUS;
  uint32 CABO1_CABO_GLOBAL_ISU_WCH_STATUS;
  
  uint32 CABO0_CABO_GLOBAL_ISU_RCH_STATUS;
  uint32 CABO1_CABO_GLOBAL_ISU_RCH_STATUS;
  
  uint32 CABO0_CABO_GLOBAL_ISU_CMDBUF_STATUS;
  uint32 CABO1_CABO_GLOBAL_ISU_CMDBUF_STATUS;

  uint32 CABO0_CABO_GLOBAL_CBU_0_RDCQ_STATUS;
  uint32 CABO1_CABO_GLOBAL_CBU_0_RDCQ_STATUS;

  uint32 CABO0_CABO_GLOBAL_CBU_0_WRCQ_STATUS;
  uint32 CABO1_CABO_GLOBAL_CBU_0_WRCQ_STATUS;

  uint32 CABO0_CABO_GLOBAL_MPE_STATUS;
  uint32 CABO1_CABO_GLOBAL_MPE_STATUS;
  
  uint32 CABO0_CABO_GLOBAL_SHKE_STATUS;
  uint32 CABO1_CABO_GLOBAL_SHKE_STATUS;

  uint32 CABO0_CABO_GLOBAL_SHKE_FSW_STATUS;
  uint32 CABO1_CABO_GLOBAL_SHKE_FSW_STATUS;

  uint32 CABO0_CABO_GLOBAL_SHKE_WDOG_STATUS;
  uint32 CABO1_CABO_GLOBAL_SHKE_WDOG_STATUS;  
  
  uint32  CABO0_CABO_GLOBAL_OSU_STATUS;
  uint32  CABO1_CABO_GLOBAL_OSU_STATUS;

  uint32 CABO0_CABO_GLOBAL_OSU_DQS_FIFO_STATUS;
  uint32 CABO1_CABO_GLOBAL_OSU_DQS_FIFO_STATUS;

  uint32 CABO0_CABO_CBU_TOP_CMD_STATUS;
  uint32 CABO1_CABO_CBU_TOP_CMD_STATUS;

  /* CABO MPE registers */  
  uint32 CABO0_CABO_MPE_CMD_STATUS;
  uint32 CABO1_CABO_MPE_CMD_STATUS;

  uint32 CABO0_CABO_MPE_APM_STATUS;
  uint32 CABO1_CABO_MPE_APM_STATUS;
  
  uint32 CABO0_CABO_GLOBAL_DDR_CLK_PERIOD_CFG;
  uint32 CABO1_CABO_GLOBAL_DDR_CLK_PERIOD_CFG;

  uint32 CABO0_CABO_SHKE_SW_CMD_STATUS;
  uint32 CABO1_CABO_SHKE_SW_CMD_STATUS;

  uint32 CABO0_CABO_SHKE_PDT_DIT_STATUS;
  uint32 CABO1_CABO_SHKE_PDT_DIT_STATUS;

  uint32 CABO0_CABO_SHKE_ZQCAL_STATUS;
  uint32 CABO1_CABO_SHKE_ZQCAL_STATUS;

  uint32 CABO0_CABO_GLOBAL_ISU_CMDBUF_STATUS0;
  uint32 CABO1_CABO_GLOBAL_ISU_CMDBUF_STATUS0;
  
  uint32 CABO0_CABO_GLOBAL_ISU_WRBUF_STATUS;
  uint32 CABO1_CABO_GLOBAL_ISU_WRBUF_STATUS;

  uint32 CABO0_CABO_GLOBAL_SHKE_BANK_UNAVAIL_STATUS;
  uint32 CABO1_CABO_GLOBAL_SHKE_BANK_UNAVAIL_STATUS;
  
  uint32 CABO0_CABO_ADDR_DECERR_ESYN_0;
  uint32 CABO1_CABO_ADDR_DECERR_ESYN_0;

  uint32 CABO0_CABO_ADDR_DECERR_ESYN_1;
  uint32 CABO1_CABO_ADDR_DECERR_ESYN_1;
  
  uint32 CABO0_CABO_MEM_BUSHANG_ERR_ESYN;
  uint32 CABO1_CABO_MEM_BUSHANG_ERR_ESYN;

  uint32 CABO0_CABO_RDQS_ERR_ESYN;
  uint32 CABO1_CABO_RDQS_ERR_ESYN;

  uint32 CABO0_CABO_REFRESH_ERR_ESYN;
  uint32 CABO1_CABO_REFRESH_ERR_ESYN;

  uint32 CABO0_CABO_MPE_IDLE_STATUS;
  uint32 CABO1_CABO_MPE_IDLE_STATUS;

  /* DDRCC registers */
  uint32 DDRCC_CH01_DDRCC_PCC_PC_STA;
  
}DDR_SDI_CSR_LOGS;

typedef struct
{
  SDI_COOKIE_VALUES p_cookie_value;
}SDIInfo; 

typedef struct
{
  SDIInfo *pass1_cookie;
  SDIInfo *pass2_cookie;
  SDIInfo *pass3_cookie;
  DDR_SDI_CSR_LOGS *sdi_csr_log;
}DDR_SDI_LOGS;

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

void ddr_sdi_save_reg_log (void);

#endif  // __DDR_SDI_H__