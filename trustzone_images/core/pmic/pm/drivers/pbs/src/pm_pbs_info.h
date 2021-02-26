#ifndef __PM_PBS_INFO_H__
#define __PM_PBS_INFO_H__

/*! \file  pm_pbs_info.h
 *  
 *  \brief  This file contains the pmic PBS info driver definitions.
 *  \details  This file contains the pm_pbs_info_init & pm_pbs_info_store_glb_ctxt
 *  API definitions.
 *  
 *  &copy; Copyright 2013 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
11/22/2015   aab     Added pm_pbs_mem_write(),  pm_pbs_mem_read(), pm_pbs_ram_dataset_write() 
                     and pm_pbs_ram_dataset_read() and refactored some functions
08/12/2016   akm     Added pm_pbs_header_info API
12/04/2015   aab     Updated pm_pbs_info_data_type 
04/29/2015   aab     Added support for PMK8001 
04/05/2013   kt      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
//#include "pm_err_flags.h"
#include "pm_lib_err.h"
#include "pm_pbs.h"


/*===========================================================================

                    MACRO AND GLOBAL VARIABLES

===========================================================================*/
/* PBS ROM/RAM Size can be 128 or 256 words. */
/* PBS OTP/ROM Start Address */
#define PM_PBS_ROM_BASE_ADDR            0x000
/* PBS RAM Start Address */
#define PM_PBS_RAM_BASE_ADDR            0x400

/* PBS Memory Version stored at the last line */
#define PM_PBS_MEMORY_VER_LINE_FROM_END   1
/* MFG Info stored at the 7th from last line (if present in PBS ROM) */
#define PM_PBS_ROM_INFO_LINE_FROM_END     7

#define PM_PBS_HEADER_SIZE                5
#define PBS_DATASET_INVALID_INDEX         0xFF

#define PM_PBS_SLEEP_DATASET_SIZE_V1      5
#define PM_PBS_PON_DATASET_SIZE_V1        6
#define PM_PBS_RTRR_DATASET_SIZE_V1       0
#define PM_PBS_SPARE_REG_DATASET_SIZE_V1  1
#define PM_PBS_LPG_DATASET_SIZE_V1        12



#define PM_PBS_HEADER_CHECKSUM          0x50425352

typedef enum
{
    PM_PBS_INFO_IN_OTP,
    PM_PBS_INFO_IN_MISC,
    PM_PBS_INFO_INVALID
} pm_pbs_info_place_holder_type;

	
typedef struct
{
    uint32 checksum;              /* Stores a CRC/signature to check if the PBS RAM is already loaded */
    uint8  dataset_start_address; /* Beginning of all the datasets in PBS RAM */
    uint8  header_version;        /* PBS RAM header version*/
    uint8  ram_revision_id;       /* PBS RAM Version ID */
    uint8  ram_branch_id;         /* PBS RAM Branch ID */
} pm_pbs_header_info_type;


typedef struct
{
	uint32 sleep_dataset_address;      /* Start address of the sleep dataset */
        uint32 sleep_dataset_size;         /* sleep dataset size */

	uint32 pon_reason_dataset_address; /* Start address of the PON dataset */
        uint32 pon_reason_dataset_size;    /* pon_reason_dataset size */

	uint32 rtrr_dataset_address;       /* Start address of the RTRR dataset */
        uint32 rtrr_dataset_size;          /* rtrr_dataset size */

	uint32 spare_reg_dataset_address;  /* Start address of the HLOS/Spare Reg dataset */
        uint32 spare_reg_dataset_size;     /* spare_reg_dataset size */

	uint32 lpg_dataset_address;        /* Start address of the LPG dataset */
        uint32 lpg_dataset_size;           /* lpg_dataset size */
} pm_pbs_header_dataset_info_type;
	

typedef enum
{
    PM_PBS_HEADER_VER_1 = 1,
    PM_PBS_HEADER_VER_INVALID
} pm_pbs_header_version_type;



typedef struct
{
    uint16 pbs_otp_start_addr;
    uint16 pbs_otp_mem_size;
    uint16 pbs_ram_start_addr;
    uint16 pbs_ram_mem_size;
    pm_pbs_info_place_holder_type pbs_info_place_holder;
} pm_pbs_info_data_type;




/**
  @brief Number of LOT IDs. Each Lot id represents an Ascii 
         value.
 */
#define PM_PBS_INFO_NUM_LOT_IDS   12

/*  
  @struct pm_pbs_info_mfgid_type  
  @brief  Stores the PMIC Manufacture ID Info. 
          Major Version, Minor Version and Shrink/Non-Shrink info. 
 */ 
typedef struct 
{ 
  uint32  major : 4;                        /* FT Major revision */ 
  uint32  minor : 3;                        /* FT Minor revision */ 
  uint32  shrink : 1;                       /* FT Shrink/Non-Shrink */ 
  uint32  reserved : 24;                    /* FT Shrink/Non-Shrink */ 
} pm_pbs_info_mfgid_type; 


/** 
  @struct pm_pbs_info_type 
  @brief Stores the PMIC PBS related information such as PBS Lot
         ID, ROM Version, RAM Version, Fab Id, Wafer Id, X
         coord, Y coord and Test PGM Rev. PBS ROM/RAM Revision
         id and Variant (or Branch) id are stored in last 16
         bits(upper and lower 8 bits) of rom_version and
         ram_version.
 */
typedef struct
{
  uint8          lot_id[PM_PBS_INFO_NUM_LOT_IDS];  /**< PBS lot id */
  uint32         rom_version;                      /**< PBS ROM Version number */
  uint32         ram_version;                      /**< PBS RAM Version number */
  uint32         fab_id;                           /**< PBS Fab Id */
  uint32         wafer_id;                         /**< PBS Wafer Id */
  uint32         x_coord;                          /**< PBS X Coord */
  uint32         y_coord;                          /**< PBS Y Coord */
  uint32         test_pgm_rev;                     /**< PBS Test PGM Rev */
  pm_pbs_info_mfgid_type  mfg_id;                  /**< PBS MGF ID */
} pm_pbs_info_type;




/*===========================================================================

      Function Definitions

============================================================================*/


/**
 * @name pm_pbs_header_info_init
 *
 * @brief 
 *        This function internally validates the PBS Core peripheral
 *        info to determine the valid PMIC Chips and calls an internal
 *        helper function to read PBS RAM header details and stores it in static
 *        global variable. This function is called during pm_device_init
 *        after the PBS RAM data is loaded.
 *
 * @param None.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not
 *          supported.
 *          else SPMI errors
 */
pm_err_flag_type pm_pbs_header_info_init(void);

/**
 * @name pm_pbs_header_version_info
 *
 * @brief 
 *        This function is used to abstract the checksum, header version, PBS RAM version,
 *        start address of all dataset / End of executable code from PBS RAM and stores it in static
 *        global variable. This function is called during pm_pbs_header_info_init
 *        after the PBS RAM data is loaded.
 *
 * @param slave id of the PMICs data being retrieved 
 * @param pm_pbs_header_info_type ptr to store the header version info of specific PMIC.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not
 *          supported.
 *          else SPMI errors
 */
pm_err_flag_type pm_pbs_header_version_info(uint8 slave_id, pm_pbs_header_info_type *pbs_header_ptr);

/**
 * @name pm_pbs_header_dataset_info
 *
 * @brief 
 *        This function is used to abstract SLEEP,PON reasons, RTRR, HLOS(spare reg),LPG dataset start address 
 *        from PBS RAM and stores it in static global variable. This function is called during pm_pbs_header_info_init
 *        after the PBS RAM data is loaded.
 *
 * @param slave_id - slave id of the PMICs data being retrieved 
 * @param start_address - address in the PBS RAM where the beginning of dataset are being stored/ executable code ends 
 * @param pbs_header_dataset_ptr - ptr to store header dataset information of the specific PMIC 
 * @param pm_pbs_header_version_type 
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not
 *          supported.
 *          else SPMI errors
 */
pm_err_flag_type pm_pbs_header_dataset_info(uint8 slave_id, uint32 start_address, pm_pbs_header_dataset_info_type *pbs_header_dataset_ptr, pm_pbs_header_version_type pbs_header_version);


/**
 * @name pm_pbs_ram_dataset_read
 *
 * @brief 
 *        This function is used to abstract PON,LPG,SLEEP, SPARE_REG  dataset DATA from PBS RAM 
 *
 * @param pmic_index    - slave id of the PMICs data being retrieved 
 * @param dataset_ptr   - ptr to get dataset information of the specific PMIC 
 * @param dataset_type  - 
 *                                  PM_PBS_PON_REASONS_DATASET_TYPE
 *                                  PM_PBS_LPG_DATASET_TYPE
 *                                  PM_PBS_SPARE_REG_DATASET_TYPE
 *                                  PM_PBS_SLEEP_DATASET_TYPE
 *  
 * @example usage: 
 *        uint32  pon_dataset_size = 0;
 *        pm_pbs_mem_get_dataset_size(0, &pon_dataset_size, PM_PBS_HEADER_PON_REASON_DATASET);          //Get pon hist dataset size
 *        uint32 pon_hist_dataset[pon_dataset_size];                                                    //Allocate mem
 *        pm_pbs_mem_dataset_read(0, pon_hist_dataset, dataset_size, PM_PBS_HEADER_PON_REASON_DATASET); //Get pon dataset
 *    
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not
 *          supported.
 *          else SPMI errors
 */
pm_err_flag_type pm_pbs_ram_dataset_read(uint32 pmic_index, uint32* dataset_ptr, uint32 dataset_ptr_size, pm_pbs_header_dataset_type dataset_type);


/**
 * @name pm_pbs_ram_dataset_write
 *
 * @brief 
 *        This function is used to write  LPG,SPARE_REG dataset DATA to PBS RAM 
 *
 * @param pmic_index        - Index of the PMICs data being retrieved 
 * @param dataset_ptr   - ptr to get dataset information of the specific PMIC 
 * @param dataset_type - 
 *                                  PM_PBS_LPG_DATASET_TYPE
 *                                  PM_PBS_SPARE_REG_DATASET_TYPE
  * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not
 *          supported.
 *          else SPMI errors
 */
pm_err_flag_type pm_pbs_ram_dataset_write(uint32 pmic_index, uint32* dataset_ptr, uint32 dataset_ptr_size, pm_pbs_header_dataset_type dataset_type);




/**
 * @name pm_pbs_mem_read_data
 *
 * @brief 
 *        This function is used to read data from PBS RAM 
 *
 * @param slave_id          - slave id of the PMIC 
 * @param start_address  - pointer to the start address of PBS RAM to read from
 * @param data_ptr         - pointer to the data to hold the read data
 * @param data_size        - size to the data to be read
 
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not  supported.
 *          else SPMI errors
 */
pm_err_flag_type pm_pbs_mem_read(uint8 slave_id, uint32 start_address, uint32* data_ptr, uint32 data_size);


/**
 * @name pm_pbs_mem_write_data
 *
 * @brief 
 *        This function is used to write data to PBS RAM 
 *
 * @param slave_id          - slave id of the PMIC 
 * @param start_address  - pointer to the start address of PBS RAM to write data
 * @param data_ptr         - pointer to the data to to be written to PBS RAM
 * @param data_size        - size to the data to be written
 
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not  supported.
 *          else SPMI errors
 */
pm_err_flag_type pm_pbs_mem_write(uint8 slave_id, uint32 start_address, uint32* data_ptr, uint32 data_size);



/**
 * @name pm_pbs_mem_get_dataset_size
 *
 * @brief 
 *        This function is used to return dataset size 
 *
 * @param pmic_index     - PMIC index 
 * @param dataset_size   - pointer to hold dataset size 
 * @param dataset_type   - pm_pbs_header_dataset_type
 *  
 *  @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = PBS peripheral is not  supported.
 *          else SPMI errors
 */
pm_err_flag_type 
pm_pbs_mem_get_dataset_size(uint32  pmic_index, uint32* dataset_size, pm_pbs_header_dataset_type dataset_type);

#endif /* __PM_PBS_INFO_H__ */