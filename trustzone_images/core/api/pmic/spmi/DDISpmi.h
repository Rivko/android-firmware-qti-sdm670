#ifndef __DDISPMI_H__
#define	__DDISPMI_H__


/**
 * @file:  DDISpmi.h
 * @brief: DAL interface for the SPMI Driver
 * 
 * Copyright (c) 2013 - 2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
  */
/*===========================================================================

                      EDIT HISTORY FOR FILE
  
 $DateTime: 2018/02/06 02:57:17 $
 $Header: //components/rel/core.tz/2.0.2/api/pmic/spmi/DDISpmi.h#1 $
 $Change: 15399902 $ 
 
 when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
 
 ===========================================================================*/

#include "SpmiTypes.h"
#include "DalDevice.h"

#define DALSPMI_INTERFACE_VERSION DALINTERFACE_VERSION(2,0)
#define DALSPMI_DEVICE_ID "DALDEVICEID_SPMI_DEVICE"

//******************************************************************************
// Data Structures
//******************************************************************************

typedef struct
{
    struct DalDevice dalDeviceHdl;
    
    DALResult (*SpmiReadLong) (DalDeviceHandle* h, uint32 slaveId, Spmi_AccessPriority priority, uint32 address, uint8* data, uint32 len, uint32* bytesRead);
    DALResult (*SpmiWriteLong) (DalDeviceHandle* h, uint32 slaveId, Spmi_AccessPriority priority, uint32 address, uint8* data, uint32 len);
    DALResult (*SpmiCommand) (DalDeviceHandle* h, uint32 slaveId, Spmi_AccessPriority priority, Spmi_Command cmd);
    DALResult (*SpmiRegisterIsr) (DalDeviceHandle* h, uint16 periph, uint32 mask, const SpmiIsr isr, const void* ctx);
    DALResult (*SpmiUnregisterIsr) (DalDeviceHandle* h, uint16 periph, uint32 mask);
    DALResult (*SpmiReadModifyWriteLongByte) (DalDeviceHandle* h, uint32 slaveId, Spmi_AccessPriority priority, uint32 address, uint32 data, uint32 mask, uint8* bytesWritten);
    DALResult (*SpmiConfigureFetcherChannel) (DalDeviceHandle *h, uint16 periph, DALBOOL enableInt, uint32 *channel);
} DalSpmi;

typedef struct DalSpmiHandle
{
    uint32 dalHandleId;
    const DalSpmi* vTbl;
    void* clientCtx;
} DalSpmiHandle;

//******************************************************************************
// Public API Functions
//******************************************************************************

#define DAL_SpmiDeviceAttach(notused, hdl) \
        DAL_StringDeviceAttachEx(NULL, DALSPMI_DEVICE_ID, DALSPMI_INTERFACE_VERSION, hdl)

/**
 * @brief Reads from a SPMI slave device 
 * 
 * This function reads data from a SPMI Slave device.
 * 
 * @param[in] h          Dal device handle
 * @param[in] priority   Requested priority with which the command needs 
 *                       to be sent on the SPMI bus
 * @param[in] slaveId    Slave id of the device
 * @param[in] address    16-bit register address on the SPMI slave device
 *                       Consists of upper 8-bit peripheral id and lower 8-bit
 *                       register offset.
 * @param[in] data       Buffer to fill with data
 * @param[in] len        Number of bytes to read
 * 
 * @param[out] bytesRead  Number of bytes read on the bus.  Can be NULL if
 *                        this information is not needed.
 * 
 * @return  DAL_SUCCESS on success, error code on error
 */
static __inline DALResult DalSpmi_ReadLong(DalDeviceHandle* h,
                                           uint32 slaveId,
                                           Spmi_AccessPriority priority,
                                           uint32 address,
                                           uint8* data,
                                           uint32 len,
                                           uint32* bytesRead)
{
    return ((DalSpmiHandle*) h)->vTbl->SpmiReadLong(h,
                                                    slaveId,
                                                    priority,
                                                    address,
                                                    data,
                                                    len,
                                                    bytesRead);
}

/**
 * @brief Writes to a SPMI slave device 
 * 
 * This function writes data to the SPMI Slave device.
 * 
 * @param[in] h          Dal device handle
 * @param[in] priority   Requested priority with which the command needs 
 *                       to be sent on the SPMI bus
 * @param[in] slaveId    Slave id of the device
 * @param[in] address    16-bit register address on the SPMI slave device
 *                       Consists of upper 8-bit peripheral id and lower 8-bit
 *                       register offset.
 * @param[in] data       Data buffer containing data to write
 * @param[in] len        Number of bytes to write
 *  
 * @return  DAL_SUCCESS on success, error code on error
 */
static __inline DALResult DalSpmi_WriteLong(DalDeviceHandle* h,
                                            uint32 slaveId,
                                            Spmi_AccessPriority priority,
                                            uint32 address,
                                            uint8* data,
                                            uint32 len)
{
    return ((DalSpmiHandle*) h)->vTbl->SpmiWriteLong(h,
                                                     slaveId,
                                                     priority,
                                                     address,
                                                     data,
                                                     len);
}

/**
 * @brief Send a command to SPMI slave device
 * 
 * This function sends a command to SPMI slave device.
 * 
 * @param[in] h         Dal device handle
 * @param[in] priority  Priority with which the command needs to be sent on
 *                      the SPMI bus
 * @param[in] slaveId   Slave Id of the device
 * @param[in] cmd       The command to send
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
static __inline DALResult DalSpmi_Command(DalDeviceHandle* h,
                                          uint32 slaveId,
                                          Spmi_AccessPriority priority,
                                          Spmi_Command cmd)
{
    return ((DalSpmiHandle*) h)->vTbl->SpmiCommand(h,
                                                   slaveId,
                                                   priority,
                                                   cmd);
}

/**
 * @brief Registers an ISR
 * 
 * This function registers an ISR with SPMI driver.
 * 
 * @param[in] h        Dal device handle
 * @param[in] slaveId  Slave Id of the device
 * @param[in] periph   Peripheral address
 * @param[in] mask     Mask of extended interrupt bits. Every bit represents 
 *                     an interrupt bit within a peripheral.
 *    Example:
 *     0x01 means register for intrpt bit 0 of the peripheral
 *     0x03 means register for intrpt bit 0 and bit 1
 *     0x05 means register for intrpt bit 0 and bit 2
 *     ...
 *     0xFF means you are registering for all the interrupts within
 *          the peripheral
 * 
 * @param[in] isr ISR routine associated with the interrupt
 * @param[in] ctx ISR context which will be copied as a parameter 
 *                on isr invocation
 * 
 * @return SPMI_INTR_CTLR_SUCCESS on success, error code on error
 * 
 * @see SpmiBus_Unregister()
 */
static __inline DALResult DalSpmi_RegisterIsr(DalDeviceHandle* h,
                                              uint16 periph,
                                              uint32 mask,
                                              const SpmiIsr isr,
                                              const void* ctx)
{
    return ((DalSpmiHandle*) h)->vTbl->SpmiRegisterIsr(h,
                                                       periph,
                                                       mask,
                                                       isr,
                                                       ctx);
}

/**
 * @brief Deregisters an ISR
 * 
 * This function deregisters an ISR with SPMI driver.
 * 
 * @param[in] h        Dal device handle
 * @param[in] periph Physical Peripheral address. 12 bits 
 *                   (4 bit SID + 8 bit upper peripheral address)
 * @param[in] mask   extended interrupt bits
 * 
 * @return SPMI_INTR_CTLR_SUCCESS on success, error code on error
 * 
 * @see SpmiBus_Unregister()
 */
static __inline DALResult DalSpmi_UnregisterIsr(DalDeviceHandle* h,
                                                uint16 periph,
                                                uint32 mask)
{
    return ((DalSpmiHandle*) h)->vTbl->SpmiUnregisterIsr(h,
                                                         periph,
                                                         mask);
}

/**
 * @brief Reads Modify Writes a byte from SPMI slave device
 * 
 * This function reads a byte from SPMI slave device, modifies it 
 * and writes it back to SPMI Slave device.
 * 
 * @param[in] h          Dal device handle
 * @param[in] slaveId    Slave id of the device
 * @param[in] priority   Requested priority with which the command needs 
 *                       to be sent on the SPMI bus
 * @param[in] address    16-bit register address on the SPMI slave device
 *                       Consists of upper 8-bit peripheral id and lower 8-bit
 *                       register offset.
 * @param[in] data       Data byte to be written
 * @param[in] mask       Mask of the bits to modify
 * 
 * @param[out] bytesWritten  Number of bytes written on the bus.  Can be NULL if
 *                           this information is not needed.
 * 
 * @return  DAL_SUCCESS on success, error code on error
 * 
 */
static __inline DALResult DalSpmi_ReadModifyWriteLongByte(DalDeviceHandle* h,
                                                          uint32 slaveId,
                                                          Spmi_AccessPriority priority,
                                                          uint32 address,
                                                          uint32 data,
                                                          uint32 mask,
                                                          uint8* bytesWritten)
{
    return ((DalSpmiHandle*) h)->vTbl->SpmiReadModifyWriteLongByte(h,
                                                                   slaveId,
                                                                   priority,
                                                                   address,
                                                                   data,
                                                                   mask,
                                                                   bytesWritten);
}

/**
 * @brief Configure the Channel used for the SPMI Fetcher
 * 
 * This function configures the channel used by the SPMI Fetcher
 * by looking up the channel index based on the slave id and 
 * optionally enabling the transaction done interrupt. 
 * 
 * @param[in] h          Dal device handle
 * @param[in] periph     Physical Peripheral address. 16 bits 
 *                       (8 bit vSID + 8 bit upper peripheral
 *                       address)
 * @param[in] enableInt  Whether to enable the transaction done 
 *                       interupt for the channel
 * @param[out] channel   Pointer to store the channel index
 * 
 * @return  DAL_SUCCESS on success, error code on error
 * 
 */
static __inline DALResult DalSpmi_ConfigureFetcherChannel(DalDeviceHandle* h,
                                                          uint16 periph,
                                                          DALBOOL enableInt,
                                                          uint32 *channel)
{
    return ((DalSpmiHandle*) h)->vTbl->SpmiConfigureFetcherChannel(h,
                                                                   periph,
                                                                   enableInt,
                                                                   channel);
}

#endif
