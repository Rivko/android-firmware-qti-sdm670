#ifndef HALNOC_H
#define HALNOC_H
/**

@file   HALnoc.h

@brief  This module provides the Application Programing Interface (API) 
        to the NOC HAL layer.
*/
/*
===========================================================================

FILE:         HALnoc.h

DESCRIPTION:  Header file for the HAL NOC interface.

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/HALnoc.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2016/09/07   sds     Created for QNoC4

===========================================================================
           Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/** NOC device info structure */
typedef struct
{
  uint8_t *base_addr;
  uint32_t num_masters; /**< QoS masters */
  uint32_t qos_offset;
  uint32_t qos_size;
} HAL_noc_InfoType;

typedef struct
{
  uint32_t bandwidth;
  uint32_t saturation;
} HAL_noc_LimiterType;

typedef struct
{
  uint32_t low_priority;
  uint32_t high_priority;
  uint32_t bandwidth;
  uint32_t saturation;
} HAL_noc_RegulatorType;

typedef struct
{
  bool read;  /**< Consider read traffic */
  bool write; /**< Consider write traffic */
} HAL_noc_RegulatorModeType;

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** @name Mandatory Interface */
/** @{ */

/** 
@brief Initialization function.

@param[inout] info - A pointer to the NOC info structure

@remarks The base address and non-hardware derived information must
         be prepopulated in the info structure.
*/
void HAL_noc_Init( HAL_noc_InfoType * info );

/**
@brief This function resets the selected NOC hardware to a default state.

@param[in] info - A pointer to the NOC info structure

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_Reset( HAL_noc_InfoType * info ) ;

/**
@brief Save the current NOC state.

@param[in] info - A pointer to the NOC info structure

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
@sa HAL_noc_Restore()
*/
void HAL_noc_Save( HAL_noc_InfoType * info );

/**
@brief Restore an NOC to a previously saved state.

@param[in] info - A pointer to the NOC info structure

@pre HAL_noc_Init() and HAL_noc_Save() must have been called.
@pre HAL_noc_Save() must be called for each restore required.

@sa HAL_noc_Init()
@sa HAL_noc_Save()
*/
void HAL_noc_Restore( HAL_noc_InfoType * info );

/** @} */

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/** @name Configuration Interface */
/** @{ */

/**
@brief Set the default priority for the given qos generator

@param[in] info -       A pointer to the NOC info structure
@param[in] master_port - The port number of the master to modify
@param[in] priority -    The priority value to use

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_SetDefaultPriority( HAL_noc_InfoType *info,
                                 uint32_t          master_port,
                                 uint32_t          priority );

/**
@brief Set bandwidth to use with the limiter block.

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master to modify
@param[in] config -      A pointer to the limiter configuration to use

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_SetLimiter( HAL_noc_InfoType *   info,
                         uint32_t             master_port,
                         HAL_noc_LimiterType *config );

/**
@brief Enable/Disable the limiter block

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master to modify
@param[in] enable -      Enable or disable limiting

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
@sa HAL_noc_SetLimiterBandwidth()
@sa HAL_noc_DisableLimiter()
*/
void HAL_noc_EnableLimiter( HAL_noc_InfoType *info,
                            uint32_t          master_port,
                            bool              enable );

/**
@brief Set regulator block configuration

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master to modify
@param[in] config -      A pointer to the regulator configuration to use

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_SetRegulator( HAL_noc_InfoType *     info,
                           uint32_t               master_port,
                           HAL_noc_RegulatorType *config );

/**
@brief Set the regulator block mode

Set the regulator block mode traffic type.

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master to modify
@param[in] mode -        A pointer to the mode configuration to set

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
@sa HAL_noc_SetRegulatorBandwidth()
@sa HAL_noc_DisableRegulator()
*/
void HAL_noc_SetRegulatorMode( HAL_noc_InfoType *         info,
                               uint32_t                   master_port,
                               HAL_noc_RegulatorModeType *mode );

/**
@brief Enable/Disable urgency forwarding

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master to modify
@param[in] enable -      Enable or disable this urgency forwarding

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()

@rem QSB masters only
*/
void HAL_noc_EnableUrgencyForwarding( HAL_noc_InfoType *info,
                                      uint32_t          master_port,
                                      bool              enable );

/** @} */

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/** @name Data Interface */
/** @{ */

/**
@brief Get the default priority for the given qos generator

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master
@param[inout] priority - A pointer to the location to store the priority

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
bool HAL_noc_GetDefaultPriority( HAL_noc_InfoType *info,
                                 uint32_t          master_port,
                                 uint32_t *        priority );

/**
@brief Get the configured bandwidth for use with the limiter block.

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master
@param[in] config -      A pointer to the limiter configuration to use

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
bool HAL_noc_GetLimiter( HAL_noc_InfoType *   info,
                         uint32_t             master_port,
                         HAL_noc_LimiterType *config );

/**
@brief Is the limiter block enabled?

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
@sa HAL_noc_SetLimiterBandwidth()
@sa HAL_noc_DisableLimiter()
*/
bool HAL_noc_IsLimiterEnabled( HAL_noc_InfoType *info,
                               uint32_t          master_port );

/**
@brief Get the current regulator configuration

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master
@param[in] config -      A pointer to the location to store the regulator configuration

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
bool HAL_noc_GetRegulator( HAL_noc_InfoType *     info,
                           uint32_t               master_port,
                           HAL_noc_RegulatorType *config );

/**
@brief Get the regulator block mode

Set the regulator block mode traffic type.

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master
@param[in] mode -        A pointer to the location to store the regulator mode

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
@sa HAL_noc_SetRegulatorBandwidth()
@sa HAL_noc_DisableRegulator()
*/
bool HAL_noc_GetRegulatorMode( HAL_noc_InfoType *         info,
                               uint32_t                   master_port,
                               HAL_noc_RegulatorModeType *mode );

/**
@brief Is urgency forwarding enabled?

@param[in] info -        A pointer to the NOC info structure
@param[in] master_port - The port number of the master

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
bool HAL_noc_IsUrgencyForwardingEnabled( HAL_noc_InfoType *info,
                                         uint32_t          master_port );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef HALNOC_H */

