#ifndef __PM_COMM_H__
#define __PM_COMM_H__

/*! \file pm_comm.h
 *  \n
 *  \brief
 *  \n  
 *  &copy; Copyright 2014-2016 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/16   aab     Added debug helper functions to get and print number of SPMI transaction
07/16/14  akm     created as a part of Comm Changes

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_resources_and_types.h"
#include "pm_version.h" 
#include "bare_metal_info.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
 typedef enum CommType
{
    FIRST_INVALID_COMM_INTERFACE = -1,
    SPMI_1_0Type,
    Transcomm_Type,
    SSBI_2_0TypeLite = 0 ,
    SSBI_2_0Type,
    INVALID_COMM_INTERFACE
}CommType;
 
 /** 
   @struct pm_comm_info_type
   @brief  This struct is used to store the PMIC Comm 
           information.
  */
typedef struct
{
    uint32 slave_id;
    uint32 pmic_index;
    CommType mCommType;
}pm_comm_info_type;



/*! \brief This function initializes the comm info structure
 *  \param[in] None
 *
 *  \return None.
 *
 *  <b>Dependencies</b>
 *  \li DAL framework.
 */
void pm_comm_info_init(void);


/*! \brief This function returns the comminfo struct(periph slave id, commtype) 
 *  \param[in] pmic_chip_index - pmic index from the total num of pmics
 *
 *\param[in] slave_id_index - primary / secondary slave 
 *
 *  \return pm_comm_info_type.
 *
 */

pm_comm_info_type* pm_comm_get_comm_info(uint32 pmic_chip_index, uint32 slave_id_index);


pm_err_flag_type  pm_comm_read_byte(uint32 slave_id , 
                                         uint16 addr, 
                                         uint8* data, 
                                         uint8 priority);

pm_err_flag_type  pm_comm_read_byte_mask(uint32 slave_id , 
                                         uint16 addr, 
                                         uint8 mask,  
                                         uint8* data, 
                                         uint8 priority);

pm_err_flag_type  pm_comm_write_byte(uint32 slave_id , 
                                          uint16 addr, 
                                          uint8 data, 
                                          uint8 priority);

pm_err_flag_type  pm_comm_write_byte_mask(uint32 slave_id , 
                                               uint16 addr, 
                                               uint8 mask, 
                                               uint8 value, 
                                               uint8 priority);

pm_err_flag_type  pm_comm_write_byte_array(uint32 slave_id,
                                                uint16 addr , 
                                                uint32 num_bytes, 
                                                uint8* data, 
                                                uint8 priority) ;

pm_err_flag_type  pm_comm_read_byte_array ( uint32 slave_id ,
                                                uint16 addr , 
                                                uint32 num_bytes, 
                                                uint8* data, 
                                                uint8 priority) ;
pm_err_flag_type pm_comm_channel_init_internal(void);


uint32 pm_log_num_spmi_transaction(uint32 initial_num_spmi_trans, boolean verbose_log);


pm_err_flag_type pm_comm_read_byte_array_dbg( uint32 slave_id,
                                               uint16 addr, 
                                               uint32 numBytes, 
                                               uint8* data, 
                                               uint8 priority );

pm_err_flag_type  pm_comm_write_byte_array_dbg( uint32 slave_id, 
                                                uint16 addr,
                                                uint32 numBytes,
                                                uint8* data, 
                                                uint8 priority );

#endif /* __PM_COMM_H__ */

