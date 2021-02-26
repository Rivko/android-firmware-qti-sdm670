#ifndef __DEVICE_INFO_H__
#define __DEVICE_INFO_H__

/*! \file device_info.h
*   \n
*   \brief This file contains PMIC Device Info struct and init function declarations
*   \n
*   \n &copy; Copyright 2010-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===================================================================
			        EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---		------------------------------------------------
04/28/17   aab     Adding pm_device_index_type
04/14/15   al      Adding changes for Hawker support
07/16/14   akm     Comm change Updates

====================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_qc_pmic.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef struct
{
  uint8 primary_pmic_index;
  uint8 interface_pmic_index;
  uint8 secondary_pmic_index;
}pm_device_index_type;



/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/** 
 * @name pm_version_detect 
 *  
 * @brief This function is called to read and store the PMIC Version
 *        information. This function verifies if the PMIC device exists
 *        by reading PMIC peripheral type value and checking it's same
 *        as QC PMIC type value. Then it stores the PMIC device
 *        information such as PMIC's Model type value (Peripheral
 *        Subtype value), All Layer Revision number and Metal Revision
 *        number.
 *  
 * @param None. 
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI errors 
 */
pm_err_flag_type pm_version_detect(void);

/** 
 * @name pm_version_deinit 
 *  
 * @brief This function clears the initialization done flag
 *  
 * @param None. 
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *          else SPMI errors 
 */
pm_err_flag_type pm_version_deinit(void);

/** 
 * @name pm_get_slave_id 
 *  
 * @brief This function is called to get the slave ids of the 
 *        PMIC device. Each PMIC device has 2 slave ids. To get
 *        the first slave id of the PMIC device the
 *        slave_id_index argument is 0 and for second slave id
 *        the slave_id_index argument is 1.
 * 
 * @param[in]  pmic_index.Primary PMIC:0 Secondary PMIC:1
 * @param[in]  slave_id_index.Primary Slave ID:0 Secondary Slave
 *             ID :1
 * @param[out] slave_id_ptr:  
 *                Variable to return to the caller with slave id
 *                value specific to the pmic device and slave id
 *                index.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Error in
 *          returning slave id.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type pm_get_slave_id(uint8 pmic_index, uint8 slave_id_index, uint32* slave_id_ptr);

/** 
 * @name pm_get_num_slaves_per_pmic 
 *  
 * @brief This function is returns the number of slaves on a QC pmic
 *  
 * @param None. 
 *
 * @return  num of slaves on QC pmic on the target
 */


uint8 pm_get_num_slaves_per_pmic(void);



/** 
 * @name pm_get_peripheral_info 
 *  
 * @brief This function is called to get peripheral info of a specific peripheral in
 *        PMIC device. The peripheral info it is going to read are:
 *           peripheral_type
 *           peripheral_subtype
 *           analog_major_version
 *           analog_minor_version
 *           digital_major_version
 *           digital_minor_version
 *  
 * @param[in] sid_index :  Indicate a specific PMIC 
 * @param[in] peripheral_info :  A pointer to where the peripheral information is going to be saved. 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Error in
 *          returning slave id.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 */
pm_err_flag_type  pm_get_peripheral_info(uint8 sid_index, peripheral_info_type *peripheral_info);



#endif /* __DEVICE_INFO_H__ */

