/*! \file  pm_pbs_info.c
 *  
 *  \brief  This file contains the pmic PBS info driver implementation.
 *  \details  This file contains the pm_pbs_info_init & pm_pbs_info_store_glb_ctxt
 *  API implementations.
 *  
 *  &copy; Copyright 2014 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/17/17	 czq     Read 9 byte lot_id in one call and optimize parse method.
02/10/17     al      Extend read PBS info from SMEM 
01/20/2017   akm     Updated PON reasons hist API
10/12/2016   akm     Added pm_get_pon_reasons, pm_get_pon_reasons_to_dump API
08/12/2016   akm     Added pm_pbs_header_info API
12/04/2015   aab     Updated  pm_pbs_info_otp_read() and  pm_pbs_info_ram_read() 
04/29/2015   aab     Added support for PMK8001 
07/16/2014   akm     Comm change Updates
11/20/2013   mr      Removed KW errors for Banned Functions API (CR-512648)
11/18/2013   kt      Added test pgm rev to pbs_info struct (increment struct format)
11/01/2013   plc     Update for Arm6 syntax ("attribute")
03/19/2013   kt      Added pm_pbs_info_add_ram_sequence API.
10/19/2012   umr     PBS Core Driver.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs.h"
#include "pm_pbs_driver.h"
#include "pm_pbs_info.h"
#include "pm_version.h"
#include "DALGlbCtxt.h"
#include "CoreVerify.h"
#include "pm_config_sbl.h"
#include "pm_device.h"
#include "pm_utils.h"
#include "pm_log_utils.h"
/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
#define DALGLBCTXT_LOCK_TYPE_SPINLOCK        0x010000

/* Format of the pm_pbs_info_smem_type structure */ 
#define PM_PBS_INFO_SMEM_FORMAT                4
#define PON_RSN_HIST_SIGNATURE                 0x5052534E //PRSN
#define PON_RSN_HIST_VERSION                   0X02
#define PM_PBS_DATASET_INVALID_START_ADDRESS   0xFFFFFFFF
#define PM_PBS_DATASET_START_ADDRESS           0xFFFF
#define PM_PBS_DATASET_START_ADDRESS_MASK      0x0000FFFF
#define PM_PBS_HEADER_START_ADDRESS            0x00
#define PM_PBS_NUM_LOT_ID_RES					9

// To print the pon reason history on UART logs just uncomment the below macro  
// #define PRINT_PON_REASON_HISTORY

/** 
  @struct pm_pbs_info_smem_type
  @brief Structure for the shared memory location which is used
         to store PMIC PBS related information such as PBS Lot
         ID, ROM Version, RAM Version, Fab Id, Wafer Id, X
         coord, Y coord and Test PGM Rev. PBS ROM/RAM Revision
         id and Variant (or Branch) id are stored in last 16
         bits(upper and lower 8 bits) of rom_version and
         ram_version.
 */
typedef struct
{
  uint32         format;                           /* Starts from 1 and increments if we add more data */
  uint8          lot_id[PM_PBS_INFO_NUM_LOT_IDS];  /* PBS Lot Id */
  uint32         rom_version;                      /* PBS ROM Version number */
  uint32         ram_version;                      /* PBS RAM Version number */
  uint32         fab_id;                           /* PBS Fab Id */
  uint32         wafer_id;                         /* PBS Wafer Id */
  uint32         x_coord;                          /* PBS X Coord */
  uint32         y_coord;                          /* PBS Y Coord */
  uint32         test_pgm_rev;                     /* PBS Test PGM Rev */
  pm_pbs_info_mfgid_type  mfg_id;                  /* PBS MGF ID */
} pm_pbs_info_smem_type;

/** 
  @struct pm_pbs_info_glb_ctxt_type
  @brief Global context data structure for sharing the pbs info 
         across processors.
 */
typedef struct
{
  DALGLB_HEADER             dal_glb_header;
  pm_pbs_info_smem_type     pbs_info_glb_arr[PM_MAX_NUM_PMICS];
} pm_pbs_info_glb_ctxt_type;

/* Static global variables to store the pbs info */
static pm_pbs_info_smem_type pm_pbs_info_arr[PM_MAX_NUM_PMICS];

/* Static global variables to store the pbs header info */
pm_pbs_header_info_type pm_pbs_header_info_arr[PM_MAX_NUM_PMICS];
pm_pbs_header_dataset_info_type pm_pbs_header_dataset_arr[PM_MAX_NUM_PMICS] ;
boolean pbs_header_info_init = FALSE;

/* Flag to check if PBS Info driver is initialized */
static boolean pm_pbs_info_initialized = FALSE;

/* Pointer to the PBS_INFO_DATA Structure */
static pm_pbs_info_data_type *pm_pbs_data = NULL;

//Global variable to Store PON reason History before PM device init 
static pm_pbs_pon_reason_history_data_type pon_reason_history;

/* Pointer to the PBS RAM/ROM size and start address Structure */
static pm_pbs_info_data_type **pm_pbs_config_data = NULL;
   

/*==========================================================================

                FUNCTION DEFINITIONS

==========================================================================*/
/** 
 * @name pm_pbs_info_otp_read
 *  
 * @brief This is an internal helper function for reading
 *        PBS info for PBS Peripheral with 128/256 word ROM
 *        and RAM support. This function reads the PBS
 *        ROM/RAM addresses for PMIC PBS Manufacturing IDs
 *        and foundry information such as PBS Lot ID, ROM
 *        Version, RAM Version, Fab Id, Wafer Id, X coord
 *        and Y coord. This function internally calls
 *        pm_pbs_enable/disable_access to enable/disable
 *        PBS ROM/RAM access and pm_pbs_config_access to
 *        configure READ BURST mode access to PBS ROM/RAM.
 *  
 * @param[in]  slave_id. PMIC chip's slave id value.
 * @param[out] pbs_info_ptr: Variable to return to the caller with
 *             PBS info. Please refer to pm_pbs_info_smem_type structure
 *             above for more info on this structure.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *  
 * @dependencies None.
 */
static pm_err_flag_type pm_pbs_info_otp_read (uint32 slave_id, pm_pbs_info_smem_type *pbs_info_ptr)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32           read_info_temp = 0;
  uint16           pbs_element_info_temp_addr = 0;  /* Address from where PBS Info to be read */

  uint8            lot_info_cnt = PM_PBS_INFO_NUM_LOT_IDS;
  uint8            lot_info_reg_reads_cnt = (PM_PBS_INFO_NUM_LOT_IDS/3);

  pbs_info_ptr->format = PM_PBS_INFO_SMEM_FORMAT;

  /* Configuring to enable PBS core access for ROM reads */
  err_flag = pm_pbs_enable_access(slave_id);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Configuring the base address for reading PBS ROM info.
   * PM_PBS_INFO_ADDR = pm_pbs_data->pbs_otp_start_addr + 4 * [pm_pbs_data->pbs_otp_mem_size - 7];
   */
  pbs_element_info_temp_addr = pm_pbs_data->pbs_otp_start_addr + 4 * (pm_pbs_data->pbs_otp_mem_size - PM_PBS_ROM_INFO_LINE_FROM_END);

  err_flag = pm_pbs_config_access(slave_id, pbs_element_info_temp_addr, PM_PBS_ACCESS_READ_BURST);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Reading the Fab id and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
     return err_flag;
  }

  /* Masking the opcode and reserved bits from temp variable 
     storing the Fab id and TEST_PGM_REV in the pbs info struct */
  pbs_info_ptr->test_pgm_rev = (uint32)(read_info_temp & 0x000000FF);
  pbs_info_ptr->fab_id = (uint32)((read_info_temp>>8) & 0x000000FF);
  read_info_temp = 0;

  /* Reading the Wafer ID, X and Y coords and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
     return err_flag;
  }

  /* Masking the opcode bits from temp variable storing the Wafer ID, X and
     Y coords along with the existing Fab ID in the pbs info struct */
  pbs_info_ptr->wafer_id = (uint32)(read_info_temp & 0x000000FF);
  pbs_info_ptr->x_coord = (uint32)((read_info_temp>>8) & 0x000000FF);
  pbs_info_ptr->y_coord = (uint32)((read_info_temp>>16) & 0x000000FF);
  read_info_temp = 0;

  /* Reading the LOT info and storing it in the pbs info struct */
  while((lot_info_reg_reads_cnt != 0) && (lot_info_cnt > 2))
  {
    /* Reading the LOT info and storing it in temp variable */
    err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);

    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      return err_flag;
    }

    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)(read_info_temp & 0x000000FF);
    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)((read_info_temp >> 8) & 0x000000FF);
    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)((read_info_temp >> 16) & 0x000000FF);

    read_info_temp = 0;

    lot_info_reg_reads_cnt--;
  }

  /* Reading the ROM version info and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Store the ROM version info in the PBS Info struct */
  pbs_info_ptr->rom_version = (uint32)(read_info_temp & 0x0000FFFF);
  read_info_temp = 0;


  /* Configuring to disable PBS core read access */
  err_flag = pm_pbs_disable_access(slave_id);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}



/** 
 * @name pm_pbs_info_ram_read
 *  
 * @brief This is an internal helper function for reading
 *        PBS info for PBS Peripheral with 128/256 word 
 *        RAM support. This function reads the PBS
 *        RAM addresses for PMIC PBS Manufacturing IDs
 *        and foundry information such as PBS Lot ID, ROM
 *        Version, RAM Version, Fab Id, Wafer Id, X coord
 *        and Y coord. This function internally calls
 *        pm_pbs_enable/disable_access to enable/disable
 *        PBS RAM access and pm_pbs_config_access to
 *        configure READ BURST mode access to PBS ROM/RAM.
 *  
 * @param[in]  slave_id. PMIC chip's slave id value.
 * @param[out] pbs_info_ptr: Variable to return to the caller with
 *             PBS info. Please refer to pm_pbs_info_smem_type structure
 *             above for more info on this structure.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *  
 * @dependencies None.
 */
static pm_err_flag_type pm_pbs_info_ram_read (uint32 slave_id, pm_pbs_info_smem_type *pbs_info_ptr)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint32           read_info_temp = 0;
  uint16           pbs_element_info_temp_addr = 0;  /* Address from where PBS Info to be read */


  pbs_info_ptr->format = PM_PBS_INFO_SMEM_FORMAT;

  /* Configuring to enable PBS core access for ROM reads */  
  err_flag = pm_pbs_enable_access(slave_id);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }


  /* Configuring the base address for reading PBS RAM version info */
  pbs_element_info_temp_addr = pm_pbs_data->pbs_ram_start_addr + 4 * (pm_pbs_data->pbs_ram_mem_size - PM_PBS_MEMORY_VER_LINE_FROM_END);

  err_flag = pm_pbs_config_access(slave_id, pbs_element_info_temp_addr, PM_PBS_ACCESS_READ_BURST);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Reading the RAM version info and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Store the RAM version info in the PBS Info struct */
  pbs_info_ptr->ram_version = (uint32)(read_info_temp & 0x0000FFFF);
  read_info_temp = 0;

  /* Configuring to disable PBS core read access */
  err_flag = pm_pbs_disable_access(slave_id);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}


/**
 * @name pm_pbs_info_read_from_periph
 *
 * @brief This is an internal helper function for reading
 *        PBS info for PBS Peripheral with 128/256 word OTP
 *        and RAM support. This function reads PMIC PBS
 *        Manufacturing IDs and foundry information such as
 *        PBS Lot ID, OTP Version, RAM Version, Fab Id,
 *        Wafer Id, X/Y coord from MISC or REV_ID Peripheral.
 *        This function internally calls
 *        pm_pbs_enable/disable_access to enable/disable PBS
 *        OTP/RAM access and pm_pbs_config_access to configure
 *        READ BURST mode access to PBS OTP/RAM.
 *
 * @param[in]  slave_id. PMIC chip's slave id value.
 * @param[out] pbs_info_ptr: Variable to return to the caller with
 *             PBS info. Please refer to pm_pbs_info_smem_type structure
 *             above for more info on this structure.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *
 * @dependencies None.
 */


static pm_err_flag_type pm_pbs_info_read_from_periph (uint32 slave_id, pm_pbs_info_smem_type *pbs_info_ptr)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8            read_info_temp8 = 0; // i = 0;
  uint32           read_info_temp32 = 0;
  uint16           pbs_element_info_temp_addr = 0;  /* Address from where PBS Info to be read */
  uint16           lot_info_addr = 0;
  uint8			   lot_arry[PM_PBS_NUM_LOT_ID_RES + 1] = {0}; //PM_PBS_NUM_LOT_ID_RES + 1 to avoid lot_ptr out of bounds
  uint8*		   lot_ptr = lot_arry;
  int              lot_info_cnt = 11;

  pbs_info_ptr->format = PM_PBS_INFO_SMEM_FORMAT;

  /* Retrieve place where MFG info stored */

  /* Reading the Test_PGM_Rev and storing it in temp variable */
  err_flag = pm_comm_read_byte(slave_id, pm_pbs_data->perph_base_addr + pm_pbs_data->tp_rev, &read_info_temp8, 1);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
     return err_flag;
  }

  pbs_info_ptr->test_pgm_rev = (uint32) read_info_temp8;
  read_info_temp8 = 0;

  /* Reading the Fab id, Wafer id, X/Y Co-ords and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, pm_pbs_data->perph_base_addr + pm_pbs_data->fab_id, 4, (uint8*)&read_info_temp32, 1);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Storing the Fab ID, Wafer ID, X and Y coords in the pbs info struct */
  pbs_info_ptr->fab_id = (uint32)(read_info_temp32 & 0x000000FF);
  pbs_info_ptr->wafer_id = (uint32)((read_info_temp32>>8) & 0x000000FF);
  pbs_info_ptr->x_coord = (uint32)((read_info_temp32>>16) & 0x000000FF);
  pbs_info_ptr->y_coord = (uint32)((read_info_temp32>>24) & 0x000000FF);
  read_info_temp32 = 0;

   /* Reading the Lot info stored in MISC/REV_ID peripheral registers. 12 Lot Ids are
     stored in 9 registers with 6 bits assigned for each lot id. Since the bit
     mask for the lot ids would be the same for every 3 register reads, below
     logic is implemented in while loop. The logic is to read and store 4 lot ids
     spread across 3 registers and repeat the same logic thrice in the loop for
     9 registers */

  /* Starting to read from LOT_ID_01_00 register and decrement to the next register
     addr after every read */

  lot_info_addr = pm_pbs_data->perph_base_addr + pm_pbs_data->lot_id; // LOT_ID_01_00;

  /* The third and final loop will start the register read from LOT_ID_09_08 register
     and the lot info count should be < 9 since we increment the count 4 times per
     loop and the number of lot ids are 12 */

  err_flag = pm_comm_read_byte_array(slave_id, lot_info_addr, PM_PBS_NUM_LOT_ID_RES, lot_arry, 1);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
	 return err_flag;
  }
	 
  while (lot_info_cnt > 2)
  {
	pbs_info_ptr->lot_id[lot_info_cnt--] = *lot_ptr >> 2 & 0x3F;
	pbs_info_ptr->lot_id[lot_info_cnt--] = *(uint16*)lot_ptr++ >> 4 & 0x3F;
	pbs_info_ptr->lot_id[lot_info_cnt--] = *(uint16*)lot_ptr++ >> 6 & 0x3F;
	pbs_info_ptr->lot_id[lot_info_cnt--] = *lot_ptr++ & 0x3F;
  }

  /* Reading the MFG_ID and storing it in temp variable */
  err_flag = pm_comm_read_byte(slave_id, pm_pbs_data->perph_base_addr + pm_pbs_data->mfg_id, &read_info_temp8, 1);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
     return err_flag;
  }
  pbs_info_ptr->mfg_id.major =  ((read_info_temp8 & 0xF0) >> 4);
  pbs_info_ptr->mfg_id.minor =  ((read_info_temp8 & 0x0E) >> 1);
  pbs_info_ptr->mfg_id.shrink = (read_info_temp8 & 0x01);
  read_info_temp8 = 0;

  /* Configuring to enable PBS core access for OTP Reads */
  err_flag = pm_pbs_enable_access(slave_id);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Configuring the base address for reading PBS OTP info */
  pbs_element_info_temp_addr = PM_PBS_ROM_BASE_ADDR + 4 * (pm_pbs_data->pbs_otp_mem_size - PM_PBS_MEMORY_VER_LINE_FROM_END);
  err_flag = pm_pbs_config_access(slave_id, pbs_element_info_temp_addr, PM_PBS_ACCESS_READ_BURST);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Reading the OTP version info and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp32, 1);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Store the OTP Version info in the PBS Info struct */
  pbs_info_ptr->rom_version = (uint32)(read_info_temp32 & 0x0000FFFF);

  /* Configuring to disable PBS core read access */
  err_flag = pm_pbs_disable_access(slave_id);

  return err_flag;
}


/** 
 * @name pm_pbs_info_rom_init 
 *  
 * @brief Please refer to pm_pbs_info.h file for info regarding 
 *        this function.
 */
pm_err_flag_type pm_pbs_info_rom_init (void) 
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  pm_pbs_info_data_type** pm_pbs_temp_data = NULL;
  uint32             slave_id = 0;
  uint8              pmic_index = 0;
  uint8              data = 0;

  DALSYS_memset(pm_pbs_info_arr, 0, sizeof(pm_pbs_info_arr));

  /* Retrieve place where PBS info stored */
  pm_pbs_temp_data = (pm_pbs_info_data_type**)pm_target_information_get_specific_info(PM_PROP_PBS_INFO);

  if(NULL == pm_pbs_temp_data)
  {
     return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  for (pmic_index = 0; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
  {
    /* Get PMIC device primary slave id */
    err_flag = pm_get_slave_id(pmic_index, 0, &slave_id);

    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      /* Second or third PMIC doesn't exist */
      err_flag = PM_ERR_FLAG_SUCCESS;
      continue;
    }

    /* Read PBS Peripheral info and check if PBS peripheral exists */
    err_flag = pm_comm_read_byte(slave_id, PMIO_PBS_CORE_PERPH_TYPE_ADDR, &data, 0);

    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
       return err_flag;
    }

    if (data != PM_HW_MODULE_PBS)
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    pm_pbs_data = pm_pbs_temp_data[pmic_index];

    if(pm_pbs_data == NULL)
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (PM_PBS_INFO_IN_PERIPH == pm_pbs_data->pbs_info_place_holder)
    {
       err_flag = pm_pbs_info_read_from_periph (slave_id, &(pm_pbs_info_arr[pmic_index]));
    }
    else if (PM_PBS_INFO_IN_OTP == pm_pbs_data->pbs_info_place_holder) 
    {
      /* Read all the PMIC's PBS Manufacturing IDs from PBS ROM */
      err_flag = pm_pbs_info_otp_read(slave_id, &(pm_pbs_info_arr[pmic_index]));
    }
    else
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      /* Configuring to disable PBS core read access */
      pm_pbs_disable_access(slave_id);

      return err_flag;
    }
  }

  pm_pbs_info_initialized = TRUE;
  return err_flag;
}

/** 
 * @name pm_pbs_info_ram_init 
 *  
 * @brief Please refer to pm_pbs_info.h file for info regarding 
 *        this function.
 */
pm_err_flag_type pm_pbs_info_ram_init (void) 
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  pm_pbs_info_data_type** pm_pbs_temp_data = NULL;
  uint32             slave_id = 0;
  uint8              pmic_index = 0;
  uint8              data = 0;


  /* Return error if PBS ROM driver is not already initialized */
  if (!pm_pbs_info_initialized)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
 	

  /* Retrieve place where PBS info stored */
  pm_pbs_temp_data = (pm_pbs_info_data_type**)pm_target_information_get_specific_info(PM_PROP_PBS_INFO);

  if(NULL == pm_pbs_temp_data)
  {
     return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  for (pmic_index = 0; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
  {
    /* Get PMIC device primary slave id */
    err_flag = pm_get_slave_id(pmic_index, 0, &slave_id);

    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      /* Second or third PMIC doesn't exist */
      err_flag = PM_ERR_FLAG_SUCCESS;
      continue;
    }

    /* Read PBS Peripheral info and check if PBS peripheral exists */
    err_flag = pm_comm_read_byte(slave_id, PMIO_PBS_CORE_PERPH_TYPE_ADDR, &data, 0);

    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
       return err_flag;
    }

    if (data != PM_HW_MODULE_PBS)
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    pm_pbs_data = pm_pbs_temp_data[pmic_index];

    if(pm_pbs_data == NULL)
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    /* Read all the PMIC's PBS Manufacturing IDs from PBS ROM */
    if ( pm_pbs_data->pbs_info_place_holder < PM_PBS_INFO_INVALID )
    {
        err_flag = pm_pbs_info_ram_read(slave_id, &(pm_pbs_info_arr[pmic_index]));
    }

    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      /* Configuring to disable PBS core read access */
      pm_pbs_disable_access(slave_id);

      return err_flag;
    }
  }

  pm_pbs_info_initialized = TRUE;
  return err_flag;
}

/** 
 * @name pm_pbs_info_store_glb_ctxt 
 *  
 * @brief Please refer to pm_pbs_info.h file for info regarding 
 *        this function.
 */
pm_err_flag_type pm_pbs_info_store_glb_ctxt (void) 
{
  pm_pbs_info_glb_ctxt_type*  pbs_glb_ctxt_ptr = NULL;
  uint8                       pmic_index = 0;

  /* Return error if PBS driver is not already initialized */
  if (!pm_pbs_info_initialized)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /* Ensure global context has been initialized */
  if(DAL_SUCCESS != DALGLBCTXT_Init())
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /* Allocate the context */
  if(DAL_SUCCESS != DALGLBCTXT_AllocCtxt("PM_PBS_INFO", sizeof(pm_pbs_info_glb_ctxt_type),
                                         DALGLBCTXT_LOCK_TYPE_SPINLOCK, (void **)&pbs_glb_ctxt_ptr))
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  if (pbs_glb_ctxt_ptr == NULL)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  for (pmic_index = 0; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
  {
    /* Fill in the global context with PMIC's PBS info */
    DALSYS_memscpy(&(pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index]), sizeof(pm_pbs_info_smem_type), &(pm_pbs_info_arr[pmic_index]), sizeof(pm_pbs_info_smem_type));
  }

  return PM_ERR_FLAG_SUCCESS;
}

static pm_err_flag_type pm_get_pbs_info_from_smem(uint8 pmic_index, pm_pbs_info_type *pbs_info_ptr)
{
  static pm_pbs_info_glb_ctxt_type *pbs_glb_ctxt_ptr = NULL;

  /* Ensure global context has been initialized */
  if (DAL_SUCCESS != DALGLBCTXT_Init())
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  if (NULL == pbs_glb_ctxt_ptr)
  {
    /* Look-up the global context */
    if (DAL_SUCCESS != DALGLBCTXT_FindCtxt("PM_PBS_INFO", (void **)&pbs_glb_ctxt_ptr))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
  }

  /* Format 0 indicates pbs info read failed in sbl */
  if (pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].format == 0)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /* Major format increments breaks the backward compatibility */
  if (((uint32)((pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].format) >> 16)) != 0)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /* Get the PBS info from global context and return it */
  DALSYS_memcpy(pbs_info_ptr->lot_id, pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].lot_id, sizeof(pbs_info_ptr->lot_id));

  pbs_info_ptr->rom_version = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].rom_version;
  pbs_info_ptr->ram_version = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].ram_version;
  pbs_info_ptr->fab_id = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].fab_id;
  pbs_info_ptr->wafer_id = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].wafer_id;
  pbs_info_ptr->x_coord = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].x_coord;
  pbs_info_ptr->y_coord = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].y_coord;
  pbs_info_ptr->test_pgm_rev = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].test_pgm_rev;
  pbs_info_ptr->mfg_id.major = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].mfg_id.major;
  pbs_info_ptr->mfg_id.minor = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].mfg_id.minor;
  pbs_info_ptr->mfg_id.shrink = pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index].mfg_id.shrink;

  return PM_ERR_FLAG_SUCCESS;
}

static pm_err_flag_type pm_get_pbs_info_from_otp(uint8 pmic_index, pm_pbs_info_type *pbs_info_ptr)
{
  if ((pm_pbs_info_initialized == FALSE) || (pm_pbs_info_arr[pmic_index].format != PM_PBS_INFO_SMEM_FORMAT))
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  if (pm_pbs_info_arr[pmic_index].format != PM_PBS_INFO_SMEM_FORMAT)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  //get the PBS info from static global variable and return it
  DALSYS_memscpy(pbs_info_ptr->lot_id, sizeof(pbs_info_ptr->lot_id), pm_pbs_info_arr[pmic_index].lot_id, sizeof(pbs_info_ptr->lot_id));

  pbs_info_ptr->rom_version = pm_pbs_info_arr[pmic_index].rom_version;
  pbs_info_ptr->ram_version = pm_pbs_info_arr[pmic_index].ram_version;
  pbs_info_ptr->fab_id = pm_pbs_info_arr[pmic_index].fab_id;
  pbs_info_ptr->wafer_id = pm_pbs_info_arr[pmic_index].wafer_id;
  pbs_info_ptr->x_coord = pm_pbs_info_arr[pmic_index].x_coord;
  pbs_info_ptr->y_coord = pm_pbs_info_arr[pmic_index].y_coord;
  pbs_info_ptr->test_pgm_rev = pm_pbs_info_arr[pmic_index].test_pgm_rev;
  pbs_info_ptr->mfg_id.major = 0;
  pbs_info_ptr->mfg_id.minor = 0;
  pbs_info_ptr->mfg_id.shrink = 0;

  return PM_ERR_FLAG_SUCCESS;
}

pm_err_flag_type pm_get_pbs_info(uint8 pmic_index, pm_pbs_info_type *pbs_info_ptr)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;

  if ((NULL == pbs_info_ptr) || (pmic_index >= PM_MAX_NUM_PMICS))
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /*read PBS info from SMEM where OTP can't be accessed*/
  if (PM_IMG_IS_LOADER == pm_get_img_type())
  {
    err_flag = pm_get_pbs_info_from_otp(pmic_index, pbs_info_ptr);
  }
  else if (PM_IMG_IS_CORE == pm_get_img_type())
  {
    err_flag = pm_get_pbs_info_from_smem(pmic_index, pbs_info_ptr);
  }
  else
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  return err_flag;
}


pm_err_flag_type pm_pbs_header_info_init(void)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint32             slave_id = 0;
  uint8              pmic_index = 0;
  uint8              data = 0;

  if(pbs_header_info_init != TRUE)
  { 
    
    pm_pbs_config_data = (pm_pbs_info_data_type**)pm_target_information_get_specific_info(PM_PROP_PBS_INFO);
	
  for (pmic_index = 0; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
  {
    /* Get PMIC device primary slave id */
    err_flag = pm_get_slave_id(pmic_index, 0, &slave_id);
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      /* Second or third PMIC doesn't exist */
      err_flag = PM_ERR_FLAG_SUCCESS;
      continue;
    }

    /* Read PBS Peripheral info and check if PBS peripheral exists */
    err_flag = pm_comm_read_byte(slave_id, PMIO_PBS_CORE_PERPH_TYPE_ADDR, &data, 0);
        if ((err_flag != PM_ERR_FLAG_SUCCESS) || (data != PM_HW_MODULE_PBS))
    {
           continue;
    }

    /* Retrieve Header version and checksum*/
    err_flag = pm_pbs_header_version_info(slave_id, &(pm_pbs_header_info_arr[pmic_index]));

        err_flag |= pm_pbs_header_dataset_info(slave_id, pm_pbs_header_info_arr[pmic_index].dataset_start_address , &(pm_pbs_header_dataset_arr[pmic_index]), pm_pbs_header_info_arr[pmic_index].header_version);
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      return err_flag;
    }
  }
  }
  
  if(err_flag == PM_ERR_FLAG_SUCCESS)
  {
     pbs_header_info_init = TRUE;
  }

  return err_flag;
}


pm_err_flag_type pm_pbs_header_version_info(uint8 slave_id, pm_pbs_header_info_type *pbs_header_ptr)
{
    pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
    uint32             read_data[2]               = {0};
    uint32             header_start_address       = PM_PBS_HEADER_START_ADDRESS;
    uint8   pmic_index                            = (uint8)(slave_id / 2); 

    err_flag |=  pm_pbs_mem_read(slave_id, pm_pbs_config_data[pmic_index]->pbs_ram_start_addr + header_start_address, read_data, 2); 
	
	
	if (err_flag != PM_ERR_FLAG_SUCCESS)
	{
		return err_flag;
	}

    pbs_header_ptr->checksum = (uint32)read_data[0];

    if(pbs_header_ptr->checksum != PM_PBS_HEADER_CHECKSUM)
	{
	    return err_flag;
	}
	
    pbs_header_ptr->dataset_start_address = (uint32)((read_data[1] >> 24) & 0x000000FF);
    pbs_header_ptr->header_version        = (uint32)((read_data[1] >>16) & 0x000000FF);
    pbs_header_ptr->ram_revision_id       = (uint32)((read_data[1] >>8) & 0x000000FF); 
    pbs_header_ptr->ram_branch_id         = (uint32)(read_data[1] & 0x000000FF);

        return err_flag;
	}


	
pm_err_flag_type pm_pbs_header_dataset_info(uint8 slave_id, uint32 start_address, pm_pbs_header_dataset_info_type *pbs_header_dataset_ptr, pm_pbs_header_version_type pbs_header_version)
{
    pm_err_flag_type   err_flag               = PM_ERR_FLAG_SUCCESS;
    uint8  sleep_data_set_index               = PBS_DATASET_INVALID_INDEX;
    uint8  pon_reason_dataset_index           = PBS_DATASET_INVALID_INDEX;
    uint8  rtrr_dataset_index                 = PBS_DATASET_INVALID_INDEX;
    uint8  spare_reg_dataset_index            = PBS_DATASET_INVALID_INDEX;
    uint8  lpg_dataset_index                  = PBS_DATASET_INVALID_INDEX;
    uint32  read_data[PM_PBS_HEADER_SIZE]     = {0};
    uint8   pmic_index                        = (uint8)(slave_id / 2); 

    if(PM_PBS_HEADER_VER_1 >= PM_PBS_HEADER_VER_INVALID)
    { 
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    err_flag |=  pm_pbs_mem_read(slave_id, pm_pbs_config_data[pmic_index]->pbs_ram_start_addr + start_address , read_data, PM_PBS_HEADER_SIZE);
	if (err_flag != PM_ERR_FLAG_SUCCESS)
	{
	  return err_flag;
	}
	
    //check for valid header version
    if(pbs_header_version == PM_PBS_HEADER_VER_1)
    {
       sleep_data_set_index      = 0;
       pon_reason_dataset_index  = 1;
       rtrr_dataset_index        = 2;
       spare_reg_dataset_index   = 3;
       lpg_dataset_index         = 4;

       pm_pbs_header_dataset_arr[pmic_index].sleep_dataset_size      = PM_PBS_SLEEP_DATASET_SIZE_V1;    
       pm_pbs_header_dataset_arr[pmic_index].pon_reason_dataset_size = PM_PBS_PON_DATASET_SIZE_V1;      
       pm_pbs_header_dataset_arr[pmic_index].rtrr_dataset_size       = PM_PBS_RTRR_DATASET_SIZE_V1;     
       pm_pbs_header_dataset_arr[pmic_index].spare_reg_dataset_size  = PM_PBS_SPARE_REG_DATASET_SIZE_V1;
       pm_pbs_header_dataset_arr[pmic_index].lpg_dataset_size        = PM_PBS_LPG_DATASET_SIZE_V1;      
}
    /*  
    //Example for a new header version: 
    else if(pbs_header_version == PM_PBS_HEADER_VER_2)
{
       sleep_data_set_index       = 0;
       pon_reason_dataset_index   = 1;
       rtrr_dataset_index         = PBS_DATASET_INVALID_INDEX;
       spare_reg_dataset_index    = 3;
       lpg_dataset_index          = 4;
    
       pm_pbs_header_dataset_arr[pmic_index].sleep_dataset_size      = PM_PBS_SLEEP_DATASET_SIZE_V2;    
       pm_pbs_header_dataset_arr[pmic_index].pon_reason_dataset_size = PM_PBS_PON_DATASET_SIZE_V2;      
       pm_pbs_header_dataset_arr[pmic_index].rtrr_dataset_size       = PM_PBS_RTRR_DATASET_SIZE_V2;     
       pm_pbs_header_dataset_arr[pmic_index].spare_reg_dataset_size  = PM_PBS_SPARE_REG_DATASET_SIZE_V2;
       pm_pbs_header_dataset_arr[pmic_index].lpg_dataset_size        = PM_PBS_LPG_DATASET_SIZE_V2;
     }
    */
    
    
    //Update dataset address
    if(sleep_data_set_index !=  PBS_DATASET_INVALID_INDEX)
    {
        if(read_data[sleep_data_set_index] == PM_PBS_DATASET_INVALID_START_ADDRESS)
        { 
          pbs_header_dataset_ptr->sleep_dataset_address = PM_PBS_DATASET_START_ADDRESS;
        }
        else       
    {
          pbs_header_dataset_ptr->sleep_dataset_address = ((read_data[sleep_data_set_index] & PM_PBS_DATASET_START_ADDRESS_MASK )) - (uint32)pm_pbs_config_data[pmic_index]->pbs_ram_start_addr;
        }
    }
	
    if(pon_reason_dataset_index !=  PBS_DATASET_INVALID_INDEX)
    {
        if(read_data[pon_reason_dataset_index] == PM_PBS_DATASET_INVALID_START_ADDRESS)
        { 
          pbs_header_dataset_ptr->pon_reason_dataset_address = PM_PBS_DATASET_START_ADDRESS;
        }
        else    
	{
          pbs_header_dataset_ptr->pon_reason_dataset_address = ((read_data[pon_reason_dataset_index] & PM_PBS_DATASET_START_ADDRESS_MASK)) - (uint32)pm_pbs_config_data[pmic_index]->pbs_ram_start_addr ;
        }
	}
	
    if(rtrr_dataset_index !=  PBS_DATASET_INVALID_INDEX)
    {
        if(read_data[rtrr_dataset_index] == PM_PBS_DATASET_INVALID_START_ADDRESS)
        { 
          pbs_header_dataset_ptr->rtrr_dataset_address = PM_PBS_DATASET_START_ADDRESS;
        }
        else    
	{
          pbs_header_dataset_ptr->rtrr_dataset_address = ((read_data[rtrr_dataset_index] & PM_PBS_DATASET_START_ADDRESS_MASK)) - (uint32)pm_pbs_config_data[pmic_index]->pbs_ram_start_addr ;
        }
	}

    if(spare_reg_dataset_index !=  PBS_DATASET_INVALID_INDEX)
    {
        if(read_data[spare_reg_dataset_index] == PM_PBS_DATASET_INVALID_START_ADDRESS)
	{
          pbs_header_dataset_ptr->spare_reg_dataset_address = PM_PBS_DATASET_START_ADDRESS;
        }
        else    
	   {
          pbs_header_dataset_ptr->spare_reg_dataset_address = ((read_data[spare_reg_dataset_index] & PM_PBS_DATASET_START_ADDRESS_MASK)) - (uint32)pm_pbs_config_data[pmic_index]->pbs_ram_start_addr;
        }
	   }

    if(lpg_dataset_index !=  PBS_DATASET_INVALID_INDEX)
    {
        if(read_data[lpg_dataset_index] == PM_PBS_DATASET_INVALID_START_ADDRESS)
	   {
          pbs_header_dataset_ptr->lpg_dataset_address = PM_PBS_DATASET_START_ADDRESS;
       }
	   else
	   {
          pbs_header_dataset_ptr->lpg_dataset_address = ((read_data[lpg_dataset_index] & PM_PBS_DATASET_START_ADDRESS_MASK)) - (uint32)pm_pbs_config_data[pmic_index]->pbs_ram_start_addr;
	   }	    
	}
	
    //if(xxx_dataset_index !=  PBS_DATASET_INVALID_INDEX)
    //{
    //  pbs_header_dataset_ptr->xxx_dataset_address = read_data[xxx_dataset_index];
    //}

	return err_flag;
}


pm_err_flag_type pm_pbs_ram_dataset_read(uint32 pmic_index, uint32* dataset_ptr, uint32 dataset_ptr_size, pm_pbs_header_dataset_type dataset_type)
{
    pm_err_flag_type err_flag           = PM_ERR_FLAG_SUCCESS;
    uint32           start_address      = 0;
    uint32           dataset_size                  = 0;
    uint32           slave_id                      = 0;

    if (dataset_ptr == NULL || dataset_type >= PM_PBS_HEADER_INVALID)
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if(pbs_header_info_init != TRUE)
    {
      err_flag |= pm_pbs_header_info_init();
      if (err_flag != PM_ERR_FLAG_SUCCESS)
	{
            return err_flag;
         }
	}

	switch(dataset_type)
    {
      case PM_PBS_HEADER_LPG_DATASET:
           start_address = pm_pbs_header_dataset_arr[pmic_index].lpg_dataset_address;
           dataset_size  = pm_pbs_header_dataset_arr[pmic_index].lpg_dataset_size;
		   break;
		   
      case PM_PBS_HEADER_SPARE_REG_DATASET:
           start_address = pm_pbs_header_dataset_arr[pmic_index].spare_reg_dataset_address;         
           dataset_size  = pm_pbs_header_dataset_arr[pmic_index].spare_reg_dataset_size;
		   break;
		   
      case PM_PBS_HEADER_PON_REASON_DATASET:
           start_address = pm_pbs_header_dataset_arr[pmic_index].pon_reason_dataset_address;
           dataset_size  = pm_pbs_header_dataset_arr[pmic_index].pon_reason_dataset_size;
		   break;
		   
      case PM_PBS_HEADER_SLEEP_DATASET:
           start_address = pm_pbs_header_dataset_arr[pmic_index].sleep_dataset_address;
           dataset_size  = pm_pbs_header_dataset_arr[pmic_index].sleep_dataset_size;
		   break;
		   
	  default:
	       return err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    } 

    //Check dataset size
    if (dataset_ptr_size < dataset_size)
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    //Read PBS RAM only if start address of dataset is configured else return 0 for all values set during memset
    if(start_address != 0xFFFF )
    {
	/* Retrieve all the pon data information from PBS ram */
       err_flag |= pm_get_slave_id(pmic_index, 0, &slave_id);
       err_flag |= pm_pbs_mem_read(slave_id, pm_pbs_config_data[pmic_index]->pbs_ram_start_addr + start_address, dataset_ptr, dataset_size);
    }
		
        return err_flag;    
	}

pm_err_flag_type pm_pbs_ram_dataset_write(uint32 pmic_index, uint32* dataset_ptr, uint32 dataset_ptr_size, pm_pbs_header_dataset_type dataset_type)
{
    pm_err_flag_type err_flag        = PM_ERR_FLAG_SUCCESS;
    uint32           start_address   = 0;
    uint8            data_size       = 0;
    uint32  slave_id          = 0;
        
    pm_pbs_ram_dataset_type *local_dataset_ptr = NULL;
	
    if (dataset_ptr == NULL || dataset_ptr_size == 0)
	{
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
 
    if (pmic_index >= PM_MAX_NUM_PMICS || dataset_type >= PM_PBS_HEADER_INVALID)
    {
       return PM_ERR_FLAG_INVALID_PARAMETER;
    }
 
    if (pbs_header_info_init != TRUE)
    {
       err_flag = pm_pbs_header_info_init();
		if (err_flag != PM_ERR_FLAG_SUCCESS)
		{
		  return err_flag;
		}
    }

    pm_pbs_ram_dataset_type local_pbs_ram_lpg_dataset[pm_pbs_header_dataset_arr[pmic_index].lpg_dataset_size];
    pm_pbs_ram_dataset_type local_pbs_ram_spare_reg_dataset[pm_pbs_header_dataset_arr[pmic_index].spare_reg_dataset_size];

		switch(dataset_type)
		{
		  case PM_PBS_HEADER_LPG_DATASET:
           start_address     = pm_pbs_header_dataset_arr[pmic_index].lpg_dataset_address;
           data_size         = pm_pbs_header_dataset_arr[pmic_index].lpg_dataset_size;
           local_dataset_ptr = local_pbs_ram_lpg_dataset;
			   break;
			   
		  case PM_PBS_HEADER_SPARE_REG_DATASET:
           start_address     = pm_pbs_header_dataset_arr[pmic_index].spare_reg_dataset_address;         
           data_size         = pm_pbs_header_dataset_arr[pmic_index].spare_reg_dataset_size;
           local_dataset_ptr = local_pbs_ram_spare_reg_dataset;
			   break;
      default:
           return err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    err_flag |= pm_get_slave_id(pmic_index, 0, &slave_id);
    err_flag |= pm_pbs_mem_write(slave_id, pm_pbs_config_data[pmic_index]->pbs_ram_start_addr + start_address, (uint32*)local_dataset_ptr, data_size);
			   
    return err_flag;
}



pm_err_flag_type pm_pbs_mem_read(uint8 slave_id, uint32 start_address, uint32* data_ptr, uint32 data_size)
{
    pm_err_flag_type   err_flag              = PM_ERR_FLAG_SUCCESS;
    uint32  read_data                        = 0;
    uint8   i                                = 0; 
    uint8   pmic_index                       = (uint8)(slave_id / 2); 
               
    if(data_ptr == NULL || data_size == 0 )
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
               
    //check PBS RAM read address is within range
    if((start_address + data_size) > (pm_pbs_config_data[pmic_index]->pbs_ram_start_addr + (4 * (pm_pbs_config_data[pmic_index]->pbs_ram_mem_size))))
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }
			   
    /* Configuring to enable PBS core access for ROM reads */
    err_flag |= pm_pbs_config_access(slave_id, start_address , PM_PBS_ACCESS_READ_BURST);
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
        return err_flag;
    }

    err_flag |= pm_pbs_enable_access(slave_id);
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
    err_flag |= pm_pbs_disable_access(slave_id);
        return err_flag;
		 }

    /* Perform PBS read*/
    for (i =0 ; i < data_size ; i++)
    {
       err_flag |= pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8 *)&read_data, 1);
       if (err_flag != PM_ERR_FLAG_SUCCESS)
       {
          err_flag |= pm_pbs_disable_access(slave_id);
    return err_flag;
	}
       data_ptr[i] = (uint32)read_data;
}

	/* Configuring to disable PBS core read access */
	err_flag |= pm_pbs_disable_access(slave_id);

	return err_flag;
}


pm_err_flag_type pm_pbs_mem_write(uint8 slave_id, uint32 start_address, uint32* data_ptr, uint32 data_size)
{
    pm_err_flag_type err_flag           = PM_ERR_FLAG_SUCCESS;
    uint8   i                    = 0; 
    uint8 pmic_index = (uint8)(slave_id / 2); 
    
    if(data_ptr == NULL || data_size == 0 )
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    //check PBS RAM write address is within range
    if((start_address + data_size) > (pm_pbs_config_data[pmic_index]->pbs_ram_start_addr + (4 * (pm_pbs_config_data[pmic_index]->pbs_ram_mem_size - PM_PBS_MEMORY_VER_LINE_FROM_END))) ||
	    ((start_address) < (pm_pbs_config_data[pmic_index]->pbs_ram_start_addr)) )
    {
       return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    /* Configuring to enable PBS core access for ROM reads */   
    err_flag |= pm_pbs_config_access(slave_id, start_address , PM_PBS_ACCESS_WRITE_BURST);
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
        return err_flag;
    }
    
    err_flag |= pm_pbs_enable_access(slave_id);
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
       err_flag |= pm_pbs_disable_access(slave_id);
        return err_flag;
    }

    /* Perform PBS write*/
    for (i =0 ; i < data_size ; i++)
    {
       err_flag |= pm_comm_write_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_WR_DATA0_ADDR, 4, (uint8*)&data_ptr[i], 1);
        if (err_flag != PM_ERR_FLAG_SUCCESS)
        {
          err_flag |= pm_pbs_disable_access(slave_id);
          return err_flag;
        }
    }

    /* Configuring to disable PBS core write access */
    err_flag |= pm_pbs_disable_access(slave_id);

    return err_flag;
    
}


pm_err_flag_type 
pm_pbs_mem_get_dataset_size(uint32  pmic_index, uint32* dataset_size, pm_pbs_header_dataset_type dataset_type)
{
    pm_err_flag_type err_flag           = PM_ERR_FLAG_SUCCESS;

   if(dataset_size == NULL)
   {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }

   if(pbs_header_info_init != TRUE)
	{
	  err_flag = pm_pbs_header_info_init();
	}
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
       return err_flag;
   }

	switch(dataset_type)
    {
      case PM_PBS_HEADER_LPG_DATASET:
          *dataset_size  = pm_pbs_header_dataset_arr[pmic_index].lpg_dataset_size;
		   break;
      case PM_PBS_HEADER_SPARE_REG_DATASET:
          *dataset_size  = pm_pbs_header_dataset_arr[pmic_index].spare_reg_dataset_size;
          break;
     case PM_PBS_HEADER_PON_REASON_DATASET:
          *dataset_size  = pm_pbs_header_dataset_arr[pmic_index].pon_reason_dataset_size;
          break;
     case PM_PBS_HEADER_SLEEP_DATASET:
          *dataset_size  = pm_pbs_header_dataset_arr[pmic_index].sleep_dataset_size;
		   break;
      default:
           return err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

   return err_flag;
}


pm_err_flag_type 
pm_pbs_get_pon_reason_hist_dataset(void)
{

   pm_err_flag_type err_flag                    = PM_ERR_FLAG_SUCCESS;
   boolean valid_sid                            = FALSE;
   uint32 slave_id                              = 0; 
   uint8 pmic_index                             = 0; 
   uint8                           copy_data_count        = 0;
   pm_model_type                   pm_model;
   uint32 rev_id_read;
   uint32                          pon_dataset_size       = 0;

   pon_reason_history.signature = PON_RSN_HIST_SIGNATURE; 
   pon_reason_history.version   = PON_RSN_HIST_VERSION;
   pon_reason_history.max_no_of_pmics = PM_MAX_NUM_PMICS;
   pon_reason_history.no_of_pon_reasons_per_pmics = NO_OF_PON_RSN_PER_PMIC;

   for (pmic_index =0 ; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
    {
		pm_get_slave_id(pmic_index, 0, &slave_id);
		pm_resolve_chip_revision(slave_id, &valid_sid, &rev_id_read);
        pm_model =  pm_get_pmic_model(pmic_index);
        if (valid_sid == FALSE)
	    {
           continue;  //PMIC does not exist
	    }

        pon_reason_history.pbs_pon_rsn_hist_data[pmic_index].pmic_id = pm_model;

        pm_pbs_mem_get_dataset_size(pmic_index, &pon_dataset_size, PM_PBS_HEADER_PON_REASON_DATASET);			//Get pon hist dataset size
        uint32 pon_hist_dataset[pon_dataset_size] ;													//Allocate mem
        DALSYS_memset(pon_hist_dataset, 0,  pon_dataset_size * sizeof(uint32));
	
        pm_pbs_ram_dataset_read(pmic_index, pon_hist_dataset, pon_dataset_size, PM_PBS_HEADER_PON_REASON_DATASET); //Get pon dataset

        for(copy_data_count =0; copy_data_count < NO_OF_PON_RSN_PER_PMIC; copy_data_count++  )
		{
            pon_reason_history.pbs_pon_rsn_hist_data[pmic_index].pon_data[copy_data_count] = pon_hist_dataset[copy_data_count];
			#ifdef PRINT_PON_REASON_HISTORY   //To get PON Reason History on UART logs just define this macro
		    if((copy_data_count%2)&&(copy_data_count !=0))
			{
		       pm_log_message("PM:%d RSN:%d: 0x%08llx%08llx",pmic_index,copy_data_count/2,pon_hist_dataset[copy_data_count],pon_hist_dataset[copy_data_count-1]);
			}
			#endif
		}
	}
   
   return err_flag;

}


pm_err_flag_type 
pm_pbs_get_pon_reason_history(pm_pbs_pon_reason_history_data_type *pon_reason_history_ptr )
{
  
    pm_err_flag_type  err_flag = PM_ERR_FLAG_SUCCESS;
  
    //Copy the data from Local variable to the memory dump location
    DALSYS_memscpy(pon_reason_history_ptr, sizeof(pm_pbs_pon_reason_history_data_type), &pon_reason_history, sizeof(pm_pbs_pon_reason_history_data_type));

	return err_flag;
    
}
