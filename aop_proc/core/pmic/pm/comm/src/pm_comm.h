#ifndef PM_COMM_H
#define PM_COMM_H

/*! \file pm_comm.h
 *  \n
 *  \brief
 *  \n  
 *  &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/comm/src/pm_comm.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/14  aks     created as a part of code refactoring

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


pm_err_flag_type  pm_comm_read_byte(uint32 slave_id, 
                                    uint16 addr, 
                                    uint8* data, 
                                    uint8 priority);

pm_err_flag_type  pm_comm_read_byte_mask(uint32 slave_id , 
                                         uint16 addr, 
                                         uint8 mask,  
                                         uint8* data, 
                                         uint8 priority);

pm_err_flag_type  pm_comm_write_byte(uint32 slave_id, 
                                     uint16 addr, 
                                     uint8 data, 
                                     uint8 priority);

pm_err_flag_type  pm_comm_write_byte_mask(uint32 slave_id, 
                                          uint16 addr, 
                                          uint8 mask, 
                                          uint8 value, 
                                          uint8 priority);

pm_err_flag_type  pm_comm_write_byte_array(uint32 slave_id,
                                           uint16 addr, 
                                           uint32 num_bytes, 
                                           uint8* data, 
                                           uint8 priority);

pm_err_flag_type  pm_comm_read_byte_array(uint32 slave_id,
                                          uint16 addr, 
                                          uint32 num_bytes, 
                                          uint8* data, 
                                          uint8 priority);

pm_err_flag_type  pm_comm_get_vrm_id(uint32 slave_id,
                                     uint16 addr,
                                     uint16* vrm_id);

pm_err_flag_type pm_comm_channel_init_internal(void);


#endif /* PM_COMM_H */

