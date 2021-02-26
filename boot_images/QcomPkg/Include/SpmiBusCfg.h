/**
 * @file:  SpmiBusCfg.h
 * @brief: This module provides configuration options for a SPMI controller
 * 
 * Copyright (c) 2013-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/SpmiBusCfg.h#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/19/15  Initial Version
 */
#ifndef SPMIBUSCFG_H
#define	SPMIBUSCFG_H

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
    SPMI_OWNER_INVALID  = SPMI_LSHIFT(15),
}SpmiBusCfg_OwnerMask;

//******************************************************************************
// Data Structures
//******************************************************************************

typedef struct
{
    uint8 slaveId;     /**< Slave id of the SPMI device */
    uint8 periphId;    /**< Peripheral id on the slave device */
    uint16 irqOwnerMask;    /**< The EE id's mask that will receive this peripheral's interrupt (refer to SpmiBusCfg_OwnerMask enum) */
    uint16 periphOwnerMask; /**< The EE id's mask that has write access to this peripheral (refer to SpmiBusCfg_OwnerMask enum) */
} SpmiBusCfg_ChannelCfg;

typedef struct
{
    uint8 slaveId;  /**< Slave id of the SPMI device */
    uint16 address; /**< Address (Periph id + offset) in the device */
} SpmiBusCfg_Ppid;

typedef struct 
{
    uint8 pvcPortId;              /**< PVC Port id */
    Spmi_AccessPriority priority; /**< SPMI bus command priority */
    SpmiBusCfg_Ppid* ppids;          /**< Peripheral access table */
    uint32 numPpids;              /**< Number of addresses in the ppids array */
} SpmiBusCfg_PvcPortCfg;

typedef struct 
{
    Spmi_AccessPriority priority; /**< SPMI bus command priority */
    SpmiBusCfg_Ppid* ppids;       /**< Peripheral access table */
    uint32 numPpids;              /**< Number of addresses in the ppids array */
} SpmiBusCfg_VrmPortCfg;

typedef struct 
{
    uint8 mgpiPortId;        /**< MGPI Port id */
    uint8 pvcPortId;         /**< PVC Port id */
    boolean posEdgeEn;       /**< Positive edge enable */
    uint8 posEdgePvcIdx;     /**< Positive edge PVC port address index */
    uint8 posEdgeData;       /**< Positive edge data */
    boolean negEdgeEn;       /**< Negative edge enable */
    uint8 negEdgePvcIdx;     /**< Negative edge PVC port address index */
    uint8 negEdgeData;       /**< Negative edge data */
} SpmiBusCfg_MgpiPortCfg;

typedef struct 
{
    boolean posEdgeEn;            /**< Positive edge enable */
    SpmiBusCfg_Ppid posEdgePpid;  /**< Positive edge PPID */
    uint8 posEdgeIrqMask;         /**< Positive edge periph interrupt mask */
    boolean negEdgeEn;            /**< Negative edge enable */
    SpmiBusCfg_Ppid negEdgePpid;  /**< Negative edge PPID */
    uint8 negEdgeIrqMask;         /**< Negative edge periph interrupt mask */
} SpmiBusCfg_VioctlCfg;

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
 * @brief Maps SW channels to a peripherals and sets interrupt ownership on bus 0
 * 
 * This function will map logical SW Channels to the peripherals and set ownership.  
 * The mapping will exist for both the owner and the observer SW channels.
 * 
 * @Note Each owner's channels should be consecutive so they can be grouped together
 *       during SMMU/XPU configuration.
 * 
 * @param[in] busId    The bus to query 
 * @param[in] entries  An array containing the mapping information.
 * @param[in] numEntries  Number of entries in the array
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 */
Spmi_Result SpmiBusCfg_ConfigureChannels(uint8 busId, SpmiBusCfg_ChannelCfg* entries, uint32 numEntries);

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
 * @brief Configures the PVC ports of the PMIC Arbiter 
 * 
 * This function configures the PMIC Arbiter for various PVC 
 * ports. It also enables the listed PVC ports. 
 * 
 * @param[in] busId    The bus to configure
 * @param[in] portCfg  The port configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
Spmi_Result SpmiBusCfg_ConfigurePvcPort(uint8 busId, const SpmiBusCfg_PvcPortCfg* portCfg);


/**
 * @brief Enables or disables all PVC ports
 * 
 * This function will enable or disable the HW PVC ports.  Called after
 * the ports have been configured.
 * 
 * @param[in] busId   The bus to configure
 * @param[in] enable  TRUE to enable, FALSE to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 * 
 * @see SpmiBusCfg_ConfigurePvcPort
 */
Spmi_Result SpmiBusCfg_SetPVCPortsEnabled(uint8 busId, boolean enable);

/**
 * @brief Configures the VRM port of the PMIC Arbiter 
 * 
 * This function configures the PMIC Arbiter for VRM port. 
 * 
 * @param[in] busId    The bus to configure
 * @param[in] portCfg  The port configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
Spmi_Result SpmiBusCfg_ConfigureVrmPort(uint8 busId, const SpmiBusCfg_VrmPortCfg* portCfg);

/**
 * @brief Enables or disables the VRM ports
 * 
 * This function will enable or disable the HW VRM ports. Called
 * after the ports has been configured. 
 * 
 * @param[in] busId   The bus to configure
 * @param[in] enable  TRUE to enable, FALSE to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 * 
 * @see SpmiBusCfg_ConfigureVrmPort
 */
Spmi_Result SpmiBusCfg_SetVrmPortsEnabled(uint8 busId, boolean enable);

/**
 * @brief Configures the MGPI ports of the PMIC Arbiter 
 * 
 * This function configures the PMIC Arbiter for various MGPI 
 * ports. It also enables the listed MGPI ports. 
 * 
 * @param[in] busId    The bus to configure
 * @param[in] portCfg  The port configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
Spmi_Result SpmiBusCfg_ConfigureMgpiPort(uint8 busId, const SpmiBusCfg_MgpiPortCfg* portCfg);

/**
 * @brief Enables or disables the MGPI ports
 * 
 * This function will enable or disable the HW MGPI ports. 
 * Called after the ports has been configured. 
 * 
 * @param[in] busId   The bus to configure
 * @param[in] enable  TRUE to enable, FALSE to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 * 
 * @see SpmiBusCfg_ConfigureMgpiPort
 */
Spmi_Result SpmiBusCfg_SetMgpiPortsEnabled(uint8 busId, boolean enable);

/**
 * @brief Configures the VIOCTL of the SPMI 
 * 
 * This function configures the SPMI Virtual IO controller to
 * pattern match SPMI RCS message based on the peripheral 
 * interrupt mask configured and received.
 * 
 * @param[in] busId      The bus to configure
 * @param[in] vioctlCfg  Vioctl configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
Spmi_Result SpmiBusCfg_ConfigureVioctl(uint8 busId, const SpmiBusCfg_VioctlCfg* vioctlCfg);

#endif
