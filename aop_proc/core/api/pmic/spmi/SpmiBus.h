/**
 * @file:  SpmiBus.h
 * @brief: This module provides the interface to the SPMI bus driver software.
 *         It provides APIs to do I/O and interrupt handling on SPMI bus.
 * 
 * Copyright (c) 2013-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2018/02/16 04:48:18 $
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/pmic/spmi/SpmiBus.h#1 $
 * $Change: 15485400 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 10/1/13  Initial Version
 */
#ifndef SPMIBUS_H
#define	SPMIBUS_H

#include "SpmiTypes.h"

/**
 * @brief Token needed for registering and unregistering ISRs.
 * 
 *        The fields are for internal use in the SPMI driver.
 *        Users should treat the object as a blackbox.
 */
typedef struct 
{
    uint8 bid;
    uint8 chan;
    uint32 id;
} SpmiToken;

/**
 * @brief Initialize the driver 
 * 
 * This function initializes the Spmi driver. It is mandatory 
 * to call this function before using any other APIs of this driver
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBus_DeInit
 */
Spmi_Result SpmiBus_Init(void);

/**
 * @brief De-initialize the driver 
 * 
 * This function de-initializes the Spmi driver.  After this call any
 * outstanding transaction will finish and SPMI bus will be disabled.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBus_Init
 */
Spmi_Result SpmiBus_DeInit(void);

/**
 * @brief Reads from a SPMI slave device 
 * 
 * This function reads data from a SPMI Slave device.
 * 
 * @param[in] vSlaveId   Virtual slave Id of the device (see SpmiEncodeVSid)
 * @param[in] priority   Requested priority with which the command needs 
 *                       to be sent on the SPMI bus
 * @param[in] address    16-bit register address on the SPMI slave device
 *                       Consists of upper 8-bit peripheral id and lower 8-bit
 *                       register offset.
 * @param[in] data       Buffer to fill with data
 * @param[in] len        Number of bytes to read
 * @param[out] bytesRead  Number of bytes read on the bus.  Can be NULL if
 *                        this information is not needed.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
Spmi_Result SpmiBus_ReadLong(uint32 vSlaveId,
                             Spmi_AccessPriority priority,
                             uint32 address,
                             uint8* data,
                             uint32 len,
                             uint32* bytesRead);
/**
 * @brief Writes to a SPMI slave device 
 * 
 * This function writes data to the SPMI Slave device.
 * 
 * @param[in] vSlaveId   Virtual slave Id of the device (see SpmiEncodeVSid)
 * @param[in] priority   Requested priority with which the command needs 
 *                       to be sent on the SPMI bus
 * @param[in] address    16-bit register address on the SPMI slave device
 *                       Consists of upper 8-bit peripheral id and lower 8-bit
 *                       register offset.
 * @param[in] data       Data buffer containing data to write
 * @param[in] len        Number of bytes to write
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
Spmi_Result SpmiBus_WriteLong(uint32 vSlaveId,
                              Spmi_AccessPriority priority,
                              uint32 address,
                              uint8* data,
                              uint32 len);
                            
/**
 * @brief Read-Modify-Write a byte from SPMI slave device
 *
 * This function reads a byte from SPMI slave device, modifies it
 * and writes it back to SPMI Slave device.
 * 
 * @param[in] vSlaveId   Virtual slave Id of the device (see SpmiEncodeVSid)
 * @param[in] priority   Priority with which the command needs to be sent on
 *                       the SPMI bus
 * @param[in] address    16-bit register address on the SPMI slave device
 *                       Consists of upper 8-bit peripheral id and lower 8-bit
 *                       register offset.
 * @param[in] data       Data to write
 * @param[in] mask       Mask of the bits which needs to be modified
 * @param[out] byteWritten  Set to the actual value written on the bus
 *
 * @return  SPMI_SUCCESS on success, error code on error
 */                       
Spmi_Result SpmiBus_ReadModifyWriteLongByte(uint32 vSlaveId,
                                            Spmi_AccessPriority priority,
                                            uint32 address,
                                            uint32 data,
                                            uint32 mask,
                                            uint8 *byteWritten);
                            
/**
 * @brief Returns VRM id mapped to the given slaveId and 
 *        periphId
 *
 * This function returns the VRM id programmed in PMIC_ARB 
 * corresponding to given slaveId and periphId 
 * 
 * @param[in] vSlaveId   Virtual slave Id of the device (see SpmiEncodeVSid)
 * @param[in] periphId   PMIC Peripheral Id 
 * @param[out] vrmId  Set to the VRM id mapped to given periphId
 *                    and slaveId
 *
 * @return  SPMI_SUCCESS on success, error code on error
 */                       
Spmi_Result SpmiBus_GetVrmId(uint8 vSlaveId, uint8 periphId, uint16* vrmId);

#endif
