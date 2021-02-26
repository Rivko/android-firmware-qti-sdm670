#ifndef PM_COMM_INC_H
#define PM_COMM_INC_H

/*! \file pm_comm_inc.h
 *  \n
 *  \brief
 *  \n  
 *  &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

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


#endif /* PM_COMM_INC_H */

