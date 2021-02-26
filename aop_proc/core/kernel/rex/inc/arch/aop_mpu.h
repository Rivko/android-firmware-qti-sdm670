/*============================================================================

FILE:      aop_mpu.h

DESCRIPTION: AOP MPU type and function declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/arch/aop_mpu.h#1 $
$Date: 2018/02/16 $

============================================================================*/

#ifndef AOP_MPU_H
#define AOP_MPU_H

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "aop_mpu_cfg.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define AOP_MPU_NUM_SUB_RGN     8
#define AOP_MPU_STACK_GUARD_SIZE    32 /* 32 bytes is the smallest region size supported by MPU */

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef enum
{
    AOP_MPU_DISABLE        = 0,
    AOP_MPU_ENABLE         = 1, /* enable MPU */
} aop_mpu_ena_t;

typedef enum
{
    AOP_MPU_DISABLE_HF_NMI = 0,
    AOP_MPU_ENABLE_HF_NMI  = 1, /* enable the MPU during the HardFault handler and NMI handler */
} aop_mpu_hf_nmi_ena_t;

typedef enum
{
    AOP_MPU_DISABLE_BG_RGN = 0,
    AOP_MPU_ENABLE_BG_RGN  = 1, /* enable background region (region -1). If this bit is not set,
                                   any access not covered by any enabled region will cause a fault */
} aop_mpu_priv_def_ena_t;

/*
 * SCS.MPU.RegionAttrSize[0] SZ_ENABLE
 */
typedef enum
{
    AOP_MPU_RGN_DISNABLE       = 0,
    AOP_MPU_RGN_ENABLE         = 1,
} aop_mpu_rgn_ena_t;

/*
 * SCS.MPU.RegionAttrSize[5:1] REGION_SIZE
 */
typedef enum
{
    AOP_MPU_RGN_SIZE_RESERVED1 =  0,
    AOP_MPU_RGN_SIZE_RESERVED2 =  1,
    AOP_MPU_RGN_SIZE_RESERVED3 =  2,
    AOP_MPU_RGN_SIZE_RESERVED4 =  3,
    AOP_MPU_RGN_SIZE_32B       =  4,
    AOP_MPU_RGN_SIZE_64B       =  5,
    AOP_MPU_RGN_SIZE_128B      =  6,
    AOP_MPU_RGN_SIZE_256B      =  7,
    AOP_MPU_RGN_SIZE_512B      =  8,
    AOP_MPU_RGN_SIZE_1KB       =  9,
    AOP_MPU_RGN_SIZE_2KB       = 10,
    AOP_MPU_RGN_SIZE_4KB       = 11,
    AOP_MPU_RGN_SIZE_8KB       = 12,
    AOP_MPU_RGN_SIZE_16KB      = 13,
    AOP_MPU_RGN_SIZE_32KB      = 14,
    AOP_MPU_RGN_SIZE_64KB      = 15,
    AOP_MPU_RGN_SIZE_128KB     = 16,
    AOP_MPU_RGN_SIZE_256KB     = 17,
    AOP_MPU_RGN_SIZE_512KB     = 18,
    AOP_MPU_RGN_SIZE_1MB       = 19,
    AOP_MPU_RGN_SIZE_2MB       = 20,
    AOP_MPU_RGN_SIZE_4MB       = 21,
    AOP_MPU_RGN_SIZE_8MB       = 22,
    AOP_MPU_RGN_SIZE_16MB      = 23,
    AOP_MPU_RGN_SIZE_32MB      = 24,
    AOP_MPU_RGN_SIZE_64MB      = 25,
    AOP_MPU_RGN_SIZE_128MB     = 26,
    AOP_MPU_RGN_SIZE_256MB     = 27,
    AOP_MPU_RGN_SIZE_512MB     = 28,
    AOP_MPU_RGN_SIZE_1GB       = 29,
    AOP_MPU_RGN_SIZE_2GB       = 30,
    AOP_MPU_RGN_SIZE_4GB       = 31,
} aop_mpu_rgn_size_t;

/*
 * SCS.MPU.RegionAttrSize[15:8] SRD (Sub-Region Disable)
 */
typedef enum
{
    AOP_MPU_RGN_SREN           = 0,    /* Enable all Sub-Regions */
    AOP_MPU_RGN_SRD0           = 1<<0, /* Disable Sub-Region 0 (lowest address) */
    AOP_MPU_RGN_SRD1           = 1<<1, /* Disable Sub-Region 1 */
    AOP_MPU_RGN_SRD2           = 1<<2, /* Disable Sub-Region 2 */
    AOP_MPU_RGN_SRD3           = 1<<3, /* Disable Sub-Region 3 */
    AOP_MPU_RGN_SRD4           = 1<<4, /* Disable Sub-Region 4 */
    AOP_MPU_RGN_SRD5           = 1<<5, /* Disable Sub-Region 5 */
    AOP_MPU_RGN_SRD6           = 1<<6, /* Disable Sub-Region 6 */
    AOP_MPU_RGN_SRD7           = 1<<7, /* Disable Sub-Region 7 (highest address) */
} aop_mpu_rgn_srd_t;

/*
 * SCS.MPU.RegionAttrSize[18:16] S/C/B (Shareable/Cacheable/Bufferable)
 */
typedef enum
{
    AOP_MPU_RGN_ATTR_NO_SCB    = 0,
    AOP_MPU_RGN_ATTR_B         = 1, /*                         Bufferable */
    AOP_MPU_RGN_ATTR_C         = 2, /*             Cacheable              */
    AOP_MPU_RGN_ATTR_CB        = 3, /*             Cacheable & Bufferable */
    AOP_MPU_RGN_ATTR_S         = 4, /* Shareable                          */
    AOP_MPU_RGN_ATTR_SB        = 5, /* Shareable             & Bufferable */
    AOP_MPU_RGN_ATTR_SC        = 6, /* Shareable & Cacheable              */
    AOP_MPU_RGN_ATTR_SCB       = 7, /* Shareable & Cacheable & Bufferable */
} aop_mpu_rgn_attr_t;

/*
 * SCS.MPU.RegionAttrSize[26:24] AP (Access Permission)
 */
typedef enum
{
    AOP_MPU_RGN_AP_NO_ACCESS   = 0,
    AOP_MPU_RGN_AP_RW          = 1,
    AOP_MPU_RGN_AP_RO          = 5,
} aop_mpu_rgn_ap_t;

/*
 * SCS.MPU.RegionAttrSize[28] XN (Execute-Never)
 */
typedef enum
{
    AOP_MPU_RGN_EXECUTABLE     = 0,
    AOP_MPU_RGN_NON_EXECUTABLE = 1,
} aop_mpu_rgn_xn_t;

typedef struct
{
    bool               enable;
    aop_mpu_rgn_size_t size;            /* set at runtime */
    uint8_t            sub_rgn_disable; /* set at runtime */
    aop_mpu_rgn_ap_t   permission;
    bool               execute_disable;
} aop_mpu_rgn_attr_size_t;

typedef struct aop_mpu_rgn_cfg_s
{
    unsigned base_addr; /* set at runtime. NOTE: Must be a multiple of its size */
    aop_mpu_rgn_num_t  rgn_id;
    aop_mpu_rgn_attr_size_t attr_size;
} aop_mpu_rgn_cfg_t;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- aop_mpu_enable -->
 *
 * @brief Enables/disables MPU.
 *
 * @param enable - TRUE to enable MPU, FALSE to disable MPU
 *
 * @return void
 */
void aop_mpu_enable(bool enable);

/**
 * <!-- aop_mpu_enable_region -->
 *
 * @brief enables/disables memory protection for the specified region.
 *
 * @param region - an MPU region name enum
 * @param enable - TRUE to enable protection for the region, FALSE to disable
 *
 * @return void
 */
void aop_mpu_enable_region(aop_mpu_rgn_num_t region, bool enable);

/**
 * <!-- aop_mpu_enable_psp_stack_guard -->
 *
 * @brief Enables/disables memory protection for the specified base address of PSP stack.
 *
 * @param enable - TRUE to enable protection for the region, FALSE to disable.
 * @param base_addr - 32-byte aligned base address of PSP stack.
 *
 * @return void
 */
void aop_mpu_enable_psp_stack_guard(bool enable, unsigned base_addr);

/**
 * <!-- aop_mpu_init -->
 *
 * @brief initializes MPU and enables it.
 *
 * @param void
 *
 * @return void
 */
void aop_mpu_init(void);

#endif /* AOP_MPU_H */

