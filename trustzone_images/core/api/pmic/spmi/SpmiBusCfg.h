#ifndef __SPMIBUSCFG_H__
#define	__SPMIBUSCFG_H__

/*! \file SpmiBusCfg.h
 *  \n
 *  \brief This module provides configuration options for a SPMI controller
 *  \details    This module provides configuration options for a SPMI controller
 *
 *  \n &copy; Copyright (c) 2004-2017  by Qualcomm Technologies Inc. All Rights Reserved.
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
$DateTime: 2018/10/10 01:12:31 $
$Header: //components/rel/core.tz/2.0.2/api/pmic/spmi/SpmiBusCfg.h#2 $
$Change: 17336083 $ 

                           Edit History
 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
 10/1/13            Initial Version
 ===========================================================================*/


#include "SpmiTypes.h"

//******************************************************************************
// Constants
//******************************************************************************

#define SPMI_LSHIFT(pos) (0x1 << pos)

/**
 * @brief spmi ee id (owner) mask
 */
typedef enum 
{
    SPMI_OWNER_NONE     = 0,
    SPMI_OWNER_APPS     = SPMI_LSHIFT(0),
    SPMI_OWNER_TZ       = SPMI_LSHIFT(1),
    SPMI_OWNER_MSS      = SPMI_LSHIFT(2),
    SPMI_OWNER_LPASS    = SPMI_LSHIFT(3),
    SPMI_OWNER_AOP      = SPMI_LSHIFT(4),
    SPMI_OWNER_SSC      = SPMI_LSHIFT(5),
    SPMI_OWNER_SECPROC  = SPMI_LSHIFT(6),
    SPMI_OWNER_FETCHER  = SPMI_LSHIFT(7),
    SPMI_OWNER_INVALID  = SPMI_LSHIFT(15),
}SpmiBusCfg_OwnerMask;

//******************************************************************************
// Data Structures
//******************************************************************************

typedef struct
{
    uint8  slaveId;         /**< Slave id of the SPMI device */
    uint8  periphId;        /**< Peripheral id on the slave device */
    uint16 irqOwnerMask;    /**< The EE id's mask that will receive this peripheral's interrupt (refer to SpmiBusCfg_OwnerMask enum) */
    uint16 periphOwnerMask; /**< The EE id's mask that has write access to this peripheral (refer to SpmiBusCfg_OwnerMask enum) */
} SpmiBusCfg_ChannelCfg;

typedef struct
{
    uint8  slaveId;  /**< Slave id of the SPMI device */
    uint16 address; /**< Address (Periph id + offset) in the device */
} SpmiBusCfg_Ppid;

typedef struct 
{
    SpmiBusCfg_OwnerMask ownerMask;
    uint32 startAddr;
    uint32 size;
    uint16 startIdx;
    uint16 endIdx;
} SpmiBusCfg_RGConfig;

//******************************************************************************
// Public API Functions
//******************************************************************************

/**
 * @brief Initialize the driver 
 * 
 * This function initializes the Spmi Configuration driver. It is mandatory 
 * to call this function before using any other APIs of this driver
 * 
 * @param[in] initHw  True to initialize static hw configuration
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_DeInit
 */
Spmi_Result SpmiBusCfg_Init(boolean initHw);

/**
 * @brief De-initialize the driver 
 * 
 * This function de-initializes the Spmi Configuration driver.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_Init
 */
Spmi_Result SpmiBusCfg_DeInit(void);

/**
 * @brief Gets the ppid, owner and address information for a SW 
 *        Channel
 * 
 * This function gets the ppid, owner and address information of
 * a channel 
 * 
 * @param[in] busId  The bus to query
 * @param[in] channel  The SW Channel to query
 * @param[out] chanCfg Channel config information such as sid, 
 *                     periph, owner masks
 * @param[out] rgCfg Owner mask and addr for a specific channel
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_GetPeripherialInfo
 */
Spmi_Result SpmiBusCfg_GetChannelInfo(uint8 busId, uint16 channel, SpmiBusCfg_ChannelCfg* chanCfg, SpmiBusCfg_RGConfig* rgCfg);

/**
 * @brief Gets the channel and owner information of a peripheral
 * 
 * This function gets the channel and owner information of a peripheral
 * 
 * @param[in] busId     The bus to query
 * @param[in] slaveId   Slave id of the device
 * @param[in] periphId  Peripheral id
 * @param[out] chanCfg Channel config information such as sid, 
 *                     periph, owner masks
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_GetChannelInfo
 */
Spmi_Result SpmiBusCfg_GetPeripherialInfo(uint8 busId, uint8 slaveId, uint8 periphId, SpmiBusCfg_ChannelCfg *chanCfg);

/**
 * @brief Updates the current channel configuration mode
 * 
 * This function enables or disables the configuration of channels
 * on-the-fly.  If enabled, resources will be configured as peripherals
 * are accessed.
 *
 * @param[in] busId     The bus to query
 * @param[in] enabled   True, to enable dynamic configuration mode
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
Spmi_Result SpmiBusCfg_SetDynamicChannelMode(uint8 busId, boolean enabled);

/**
 * @brief Calculates resource group configuration
 * 
 * This function will return resource group configuration for 
 * SPMI channels without modifying any HW configuration. 
 * 
 * @param[in] busId       The bus to query
 * @param[out] rgCfg      An array of Resource Groups that will 
 *                        be populated
 * @param[out] rgCount    Pointer to number of Resource Groups 
 *                        used
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
Spmi_Result SpmiBusCfg_CalculateRGConfig(uint8 busId, SpmiBusCfg_RGConfig* rgCfg, uint32* rgCount);

/**
 * @brief Returns the number of SPMI channels present 
 * 
 * This function returns the number of SPMI channels 
 * 
 * @param[in] busId       The bus to query 
 * @param[out] numChan    Number of channels
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
Spmi_Result SpmiBusCfg_GetNumChannels(uint8 busId, uint32 *numChan);
/**
 * @brief Gets the sw channel assigned to particular peripheral
 * 
 * This function gets sw channel assigned to particular peripheral
 * a channel 
 * 
 * @param[in] busId  The bus to query
 * @param[in] slaveId  The slaveid to query
 * @param[in] periphId  The periphId to query
 * @param[out] channel SW Channel asigned to periphId with slaveId
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiBusCfg_GetChannel
 */
Spmi_Result SpmiBusCfg_GetChannel(uint8 busId, uint8 slaveId, uint8 periphId, uint16 *channel);

#endif
