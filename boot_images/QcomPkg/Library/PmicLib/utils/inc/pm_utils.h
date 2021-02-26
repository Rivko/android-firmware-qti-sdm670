#ifndef __PM_UTILS_H__
#define __PM_UTILS_H__

/*! \file
 *  
 *  \brief  pm_utils.h ----This file contain PMIC wrapper function of DALSYS_Malloc()
 *  \details his file contain PMIC wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2012-2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/17   czq     Use Spare reg PON_PERPH_RB_SPARE instead of PON_DVDD_RB_SPARE
03/20/14   aab     Added pm_boot_adc_get_mv()
06/11/13   hs      Adding settling time for regulators.
06/20/12   hs      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "DDIAdc.h"

#define PON_PERPH_RB_SPARE_DEVICE_INIT_MASK      0x10   //PERPH SPARE 4 for device init 
#define PON_PERPH_RB_SPARE_DRIVER_INIT_MASK      0x20   //PERPH SPARE 5 for driver init 

typedef enum
{
  PM_IMG_IS_LOADER,
  PM_IMG_IS_CORE,
  PM_IMG_INVALID,
}pm_img_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
extern void pm_malloc(uint32 dwSize, void **ppMem);

extern uint64 pm_convert_time_to_timetick(uint64 time_us);

extern uint64 pm_convert_timetick_to_time(uint64 time_tick);

pm_err_flag_type
pm_busy_wait ( uint32 uS );

uint8
pm_utils_get_upper_idx(uint32 data_value, uint32 *data_list, uint8 list_count);

uint32 
pm_memory_allocated(void);

pm_err_flag_type 
pm_sbl_get_batt_id(uint32 *batt_id_ohms);

/*This API configures the PMIC PS_HOLD register for normal shutdown and then drops the PS_HOLD low*/
pm_err_flag_type 
pm_device_shutdown(void);

//Check if the pm_driver_init is done
boolean 
pm_driver_init_status(void);

//check if pm_device_init is done
boolean 
pm_device_init_status(void);

pm_err_flag_type  
pm_sbl_daladc_fg_set_temp_threshold(uint8 pmic_index,  AdcFGThresholdType adc_fg_threshold, int temp_value); 

pm_img_type pm_get_img_type(void);

pm_err_flag_type 
pm_sbl_set_batt_id_forced_mode(boolean enable);


#endif // __PM_UTILS_H__
