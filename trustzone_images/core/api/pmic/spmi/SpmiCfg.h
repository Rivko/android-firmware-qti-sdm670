#ifndef __SPMICFG_H__
#define	__SPMICFG_H__

/*! \file SpmiCfg.h
 *  \n
 *  \brief This module provides configuration options for a SPMI controller
 *  \details This module provides configuration options for a SPMI controller
 *
 *  \n &copy; Copyright (c) 2004-2017  by Qualcomm Technologies Inc. All Rights Reserved.
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
$DateTime: 2018/10/10 01:12:31 $
$Header: //components/rel/core.tz/2.0.2/api/pmic/spmi/SpmiCfg.h#2 $
$Change: 17336083 $ 

                           Edit History

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
 11/3/14            Automatic channel assignment
 10/1/13            Initial Version
 ===========================================================================*/


#include "SpmiBusCfg.h"

#define SPMICFG_LEGACY_BUS_ID 0

typedef SpmiBusCfg_ChannelCfg SpmiCfg_ChannelCfg;
typedef SpmiBusCfg_Ppid       SpmiCfg_Ppid;
typedef SpmiBusCfg_RGConfig   SpmiCfg_RGConfig;
typedef SpmiBusCfg_OwnerMask  SpmiCfg_OwnerMask;

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
 * @see SpmiCfg_DeInit
 */
inline Spmi_Result SpmiCfg_Init(boolean initHw)
{
    return SpmiBusCfg_Init(initHw);
}

/**
 * @brief De-initialize the driver 
 * 
 * This function de-initializes the Spmi Configuration driver.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_Init
 */
inline Spmi_Result SpmiCfg_DeInit(void)
{
    return SpmiBusCfg_DeInit();
}

/**
 * @brief Gets the ppid, owner and address information for a SW 
 *        Channel
 * 
 * This function gets the ppid, owner and address information of
 * a channel 
 * 
 * @param[in] channel  The SW Channel to query
 * @param[out] slaveId   Slave id of the device mapped to the channel
 * @param[out] chanCfg Channel config information such as sid, 
 *                     periph, owner masks
 * @param[out] rgCfg Owner mask and addr for a specific channel
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_GetPeripherialInfo
 */
inline Spmi_Result SpmiCfg_GetChannelInfo(uint16 channel, SpmiCfg_ChannelCfg* chanCfg, SpmiCfg_RGConfig* rgCfg)
{
    return SpmiBusCfg_GetChannelInfo(SPMICFG_LEGACY_BUS_ID, channel, chanCfg, rgCfg);
}

/**
 * @brief Gets the channel and owner information of a peripheral
 * 
 * This function gets the channel and owner information of a peripheral
 * 
 * @param[in] slaveId   Slave id of the device
 * @param[in] periphId  Peripheral id
 * @param[out] chanCfg Channel config information such as sid, 
 *                     periph, owner masks
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_GetChannelInfo
 */
inline Spmi_Result SpmiCfg_GetPeripherialInfo(uint8 slaveId, uint8 periphId, SpmiCfg_ChannelCfg *chanCfg)
{
    return SpmiBusCfg_GetPeripherialInfo(SPMICFG_LEGACY_BUS_ID, slaveId, periphId, chanCfg);
}

/**
 * @brief Updates the current channel configuration mode
 * 
 * This function enables or disables the configuration of channels
 * on-the-fly.  If enabled, resources will be configured as peripherals
 * are accessed.
 *
 * @param[in] enabled   True, to enable dynamic configuration mode
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
inline Spmi_Result SpmiCfg_SetDynamicChannelMode(boolean enabled)
{
    return SpmiBusCfg_SetDynamicChannelMode(SPMICFG_LEGACY_BUS_ID, enabled);
}

/**
 * @brief Calculates resource group configuration
 * 
 * This function will return resource group configuration for 
 * SPMI channels without modifying any HW configuration. 
 * 
 * @param[out] rgCfg      An array of Resource Groups that will 
 *                        be populated
 * @param[out] rgCount    Pointer to number of Resource Groups 
 *                        used
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
inline Spmi_Result SpmiCfg_CalculateRGConfig(SpmiCfg_RGConfig* rgCfg, uint32* rgCount)
{
    return SpmiBusCfg_CalculateRGConfig(SPMICFG_LEGACY_BUS_ID, rgCfg, rgCount);
}

/**
 * @brief Returns the number of SPMI channels present
 * 
 * This function returns the number of SPMI channels  
 *  
 * @param[out] numChan Number of channels
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
inline Spmi_Result SpmiCfg_GetNumChannels(uint32 *numChan)
{
    return SpmiBusCfg_GetNumChannels(SPMICFG_LEGACY_BUS_ID, numChan);
}


/**
 * @brief Gets the sw channel assigned to particular peripheral
 * 
 * This function gets sw channel assigned to particular peripheral
 * a channel 
 * 
 * @param[in] slaveId  The slaveid to query
 * @param[in] periphId  The periphId to query
 * @param[out] channel SW Channel asigned to periphId with slaveId
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
inline Spmi_Result SpmiCfg_GetChannel(uint8 slaveId, uint8 periphId, uint16* channel)
{
    return SpmiBusCfg_GetChannel(SPMICFG_LEGACY_BUS_ID, slaveId, periphId, channel);
}

#endif
