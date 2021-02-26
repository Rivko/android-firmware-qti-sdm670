/**
 * @file:  SpmiCfg.h
 * @brief: This module provides configuration options for the SPMI controller
 * @depricated Use SpmiBusCfg.h
 * 
 * Copyright (c) 2013-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/SpmiCfg.h#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */
#ifndef SPMICFG_H
#define	SPMICFG_H

#include "SpmiBusCfg.h"

#define SPMICFG_LEGACY_BUS_ID 0

typedef SpmiBusCfg_ChannelCfg SpmiCfg_ChannelCfg;
typedef SpmiBusCfg_Ppid       SpmiCfg_Ppid;
typedef SpmiBusCfg_PvcPortCfg SpmiCfg_PvcPortCfg;
typedef SpmiBusCfg_VrmPortCfg SpmiCfg_VrmPortCfg;
typedef SpmiBusCfg_MgpiPortCfg SpmiCfg_MgpiPortCfg;
typedef SpmiBusCfg_VioctlCfg SpmiCfg_VioctlCfg;
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
 * @brief Maps SW channels to a peripherals and sets interrupt ownership
 * 
 * This function will map logical SW Channels to the peripherals and set ownership.  
 * The mapping will exist for both the owner and the observer SW channels.
 * 
 * @Note Each owner's channels should be consecutive so they can be grouped together
 *       during SMMU/XPU configuration.
 * 
 * @param[in] entries  An array containing the mapping information.
 * @param[in] numEntries  Number of entries in the array
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_ConfigurePeripherals
 */
inline Spmi_Result SpmiCfg_ConfigureChannels(SpmiCfg_ChannelCfg* entries, uint32 numEntries)
{
    return SpmiBusCfg_ConfigureChannels(SPMICFG_LEGACY_BUS_ID, entries, numEntries);
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
 * @brief Configures the PVC ports of the PMIC Arbiter 
 * 
 * This function configures the PMIC Arbiter for various PVC 
 * ports. It also enables the listed PVC ports. 
 * 
 * @param[in] portCfg  The port configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
inline Spmi_Result SpmiCfg_ConfigurePvcPort(const SpmiCfg_PvcPortCfg* portCfg)
{
    return SpmiBusCfg_ConfigurePvcPort(SPMICFG_LEGACY_BUS_ID, portCfg);
}


/**
 * @brief Enables or disables all PVC ports
 * 
 * This function will enable or disable the HW PVC ports.  Called after
 * the ports have been configured.
 * 
 * @param[in] enable  TRUE to enable, FALSE to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 * 
 * @see SpmiCfg_ConfigurePvcPort
 */
inline Spmi_Result SpmiCfg_SetPVCPortsEnabled(boolean enable)
{
    return SpmiBusCfg_SetPVCPortsEnabled(SPMICFG_LEGACY_BUS_ID, enable);
}

/**
 * @brief Configures the VRM port of the PMIC Arbiter 
 * 
 * This function configures the PMIC Arbiter for VRM port.
 * 
 * @param[in] portCfg  The port configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
inline Spmi_Result SpmiCfg_ConfigureVrmPort(const SpmiCfg_VrmPortCfg* portCfg)
{
    return SpmiBusCfg_ConfigureVrmPort(SPMICFG_LEGACY_BUS_ID, portCfg);
}

/**
 * @brief Enables or disables the VRM ports
 * 
 * This function will enable or disable the HW VRM ports. Called
 * after the ports have been configured. 
 * 
 * @param[in] enable  TRUE to enable, FALSE to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 * 
 * @see SpmiCfg_ConfigureVrmPort
 */
inline Spmi_Result SpmiCfg_SetVrmPortsEnabled(boolean enable)
{
    return SpmiBusCfg_SetVrmPortsEnabled(SPMICFG_LEGACY_BUS_ID, enable);
}

/**
 * @brief Configures the MGPI ports of the PMIC Arbiter 
 * 
 * This function configures the PMIC Arbiter for various MGPI 
 * ports. It also enables the listed MGPI ports. 
 *  
 * @param[in] portCfg  The port configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
inline Spmi_Result SpmiCfg_ConfigureMgpiPort(const SpmiCfg_MgpiPortCfg* portCfg)
{
    return SpmiBusCfg_ConfigureMgpiPort(SPMICFG_LEGACY_BUS_ID, portCfg);
}

/**
 * @brief Enables or disables the MGPI ports
 * 
 * This function will enable or disable the HW MGPI ports. 
 * Called after the ports have been configured. 
 * 
 * @param[in] enable  TRUE to enable, FALSE to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 * 
 * @see SpmiCfg_ConfigureMgpiPort
 */
inline Spmi_Result SpmiCfg_SetMgpiPortsEnabled(boolean enable)
{
    return SpmiBusCfg_SetMgpiPortsEnabled(SPMICFG_LEGACY_BUS_ID, enable);
}

/**
 * @brief Configures the VIOCTL of the SPMI 
 * 
 * This function configures the SPMI Virtual IO controller to
 * pattern match SPMI RCS message based on the peripheral 
 * interrupt mask configured and received. 
 *  
 * @param[in] vioctlCfg  Vioctl configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
inline Spmi_Result SpmiCfg_ConfigureVioctl(const SpmiCfg_VioctlCfg* vioctlCfg)
{
    return SpmiBusCfg_ConfigureVioctl(SPMICFG_LEGACY_BUS_ID, vioctlCfg);
}

#endif
