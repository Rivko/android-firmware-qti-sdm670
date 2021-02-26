#ifndef __PMIC_SBL_CONFIG_H__
#define __PMIC_SBL_CONFIG_H__

/*! \file  pm_sbl_config.h
 *  
 *  \brief  Contains PMIC Set Mode driver declaration
 *  \details  This file contains the Set Mode driver API. These interfaces 
 *  should not be called directly. All entry into these interfaces should 
 *  from the application level set mode interfaces.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: PM8026-x.x-OTPx.x-MSM8x26-07012013v1 - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2013-2016 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/23/16   aab     Updated driver to support Mask write
09/01/15   aab     Updated driver to support SBL Conditional configuration feature
07/13/15   pb      Moved Settings validation content to separate test file
06/23/15   aab     Added support for msm8996 V2/V3 and dropped support for V1
04/29/15   aab     Added support for PMK8001  
04/17/15   aab     Added support for msm8996 V2/V1 compatibility 
11/10/14   aab     updated PSI data location (Entry point) address
07/03/14   plc     Update PM_BL_PSI_ADDR to be in RPM CODE RAM, not DATA_RAM
06/24/14   akm     Updates for PSI image Creation
08/07/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_qc_pmic.h"
#include "com_dtypes.h"         /* Containse basic type definitions */
#include "pm_err_flags.h"     /* Containse the error definitions */
#include "pm_pbs_driver.h"
#include "Target_cust.h"
#include "pm_config_target.h"

#ifdef CUST_H
	#include CUST_H
#endif

//This signature is to verify that pmic.mbn is loaded before configuring the registers: Value of 0x31495350 is PSI2
#define PSI_SIGNATURE 0x32495350

#define PM_SBL_16BITADD_8BITDATA_REGULAR 2
#define REV_ID_COMMON 0xFFFFFFFF
#define PM_BL_PSI_ADDR SCL_PMIC_CONFIG_BUF_BASE   //PSI data location (Entry point): 


/*=============================================================================

DEFINITIONS

=============================================================================*/

typedef enum
{
   PM_SBL_WRITE,
   PM_SBL_DELAY,
   PM_SBL_PBS_RAM,
   PM_SBL_NOP,
   PM_SBL_OPERATION_INVALID
}pm_sbl_reg_operation_type;

 

typedef enum  
{
   EQUAL,
   GREATER,
   GREATER_OR_EQUAL,
   LESS,
   LESS_OR_EQUAL,
   REV_ID_OPERATION_INVALID
}pm_sbl_cond_operation_type;


//Table A: Main Sbl sequence
typedef struct
{
   uint8  sid;
   uint8  data;
   uint16 address;
   uint8 mask;
   pm_sbl_reg_operation_type reg_operation;
   uint8  cond_start_index;
   uint8  cond_end_index;
}pm_sbl_seq_type;


//Table B: Conditional config table
typedef struct
{
   uint8  cond_sid;
   uint8  cond_data;
   uint8  cond_mask; 
   uint16 cond_address;
   pm_sbl_cond_operation_type cond_operation;
}pm_sbl_cond_seq_type;
   
   
   
//SBL config info
typedef struct
{
   uint32 signature;          // PSI_SIGNATURE 
   uint8  major_ver;          // PSI Major Version
   uint8  minor_ver;          // PSI Minor Version
   uint8  num_target_config;  // Number of target configurations
   uint8  cond_num_entries;      // Total number of conditional entries on pm_sbl_cond_seq_type table
}pm_sbl_config_info_type;
   



typedef struct
{
   pm_sbl_seq_type*           pm_sbl_seq;
   pm_pbs_ram_data_type*      pm_pbs_ram_seq;
   pm_sbl_cond_seq_type*      pm_sbl_cond_seq;
}pm_sbl_pdm_data_type;

typedef struct
{
   pm_sbl_config_info_type* pm_psi_info;
   pm_sbl_pdm_data_type* pdm_data; 
}pm_sbl_pdm_config_type;


/** 
  @struct pm_pdm_chip_info_type
  @brief Stores the PMIC's chip revision.
 */
typedef struct
{
  uint32         pdmChipRevision;          /**< chip Revision in a format that PDM configuration is generated*/
} pm_pdm_chip_info_type;


/*=============================================================================

                              FUNCTION PROTOTYPES

=============================================================================*/

/*Generic function that reads SBL Config data and PBS RAM data from DAL Config and writes it to PMIC */
pm_err_flag_type pm_sbl_config(void);

pm_err_flag_type pm_resolve_chip_revision(uint32 slave_id, boolean* valid_slave_id, uint32* pmic_revision );

pm_err_flag_type pm_sbl_validate_cond_config(   pm_sbl_config_info_type *sbl_config_info_ds,
                                                pm_sbl_cond_seq_type   *cond_ds_ptr, 
                                                uint8    cond_start_index,
                                                uint8    cond_end_index,
                                                boolean  *valid_config_flag  
                                );
                                             
#endif // __PMIC_SBL_CONFIG_H__

