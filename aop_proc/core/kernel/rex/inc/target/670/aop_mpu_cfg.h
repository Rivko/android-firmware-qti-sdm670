#ifndef __AOP_MPU_CFG_H__
#define __AOP_MPU_CFG_H__

/*===========================================================================
                             aop_mpu_cfg.h

DESCRIPTION:
  AOP MPU Configuration Table. This defines the memory access attribute and permission for MPU regions.

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
/*
** NOTE:
** - Supported up to 8 regions.
** - Region 7 has highest priority, region 0 has lowest priority.
** - Attributes of overlapping regions will be set to the attributes of
**   the higher priority region (if overlapping regions enabled).
**/
typedef enum
{
    AOP_MPU_RGN_CODE_RAM = 0,    /* to detect null-pointer dereference */

    AOP_MPU_RGN_MSP_STACK_GUARD, /* to detect MSP stack overflow. NOTE: Must be the highest priority */
    AOP_MPU_RGN_PSP_STACK_GUARD, /* to detect PSP stack overflow. NOTE: Must be the highest priority */

    AOP_MPU_RGN_COUNT,
} aop_mpu_rgn_num_t;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
struct aop_mpu_rgn_cfg_s;

extern struct aop_mpu_rgn_cfg_s * const aop_mpu_cfg;


//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- aop_mpu_cfg_init -->
 *
 * @brief Sets up the configuration for MPU regions.
 *
 *  This is used to set the base address and size of the configuration for MPU regions.
 *  This does not program and configure MPU.
 *
 * @param void
 *
 * @return void
 */
void aop_mpu_cfg_init(void);

#endif /* __AOP_MPU_CFG_H__ */

