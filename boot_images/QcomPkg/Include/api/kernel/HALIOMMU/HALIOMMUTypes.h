#ifndef __HAL_IOMMU_TYPES_H__
#define __HAL_IOMMU_TYPES_H__
/*==============================================================================
@file HALIOMMUTypes.h


        Copyright (c) 2008,2010,2014 Qualcomm Technologies, Inc.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

==============================================================================*/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/19/17   amo     Created

=============================================================================*/
/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "Uefi.h"
#include "com_dtypes.h"
#include "DALSys.h"

/*----------------------------------------------------------------------------*/


// cache property
typedef enum {

    // device non-gathering, non-reordering, no early write acknowledgement
    HAL_IOMMU_CACHE_DEVICE_nGnRnE       = 0,   // b0000
    HAL_IOMMU_CACHE_DEVICE_nGnRE        = 1,   // b0001
    HAL_IOMMU_CACHE_DEVICE_nGRE         = 2,   // b0010
    HAL_IOMMU_CACHE_DEVICE_GRE          = 3,   // b0011
    // outer non-cacheable, inner non-cacheable
    HAL_IOMMU_CACHE_OUTER_NC_INNER_NC   = 5,   // b0101
    // outer non-cacheable, inner write-through
    HAL_IOMMU_CACHE_OUTER_NC_INNER_WT   = 6,   // b0110
    // outer non-cacheable, inner writeback
    HAL_IOMMU_CACHE_OUTER_NC_INNER_WB   = 7,   // b0111
    HAL_IOMMU_CACHE_OUTER_WT_INNER_NC   = 9,   // b1001
    HAL_IOMMU_CACHE_OUTER_WT_INNER_WT   = 10,  // b1010
    HAL_IOMMU_CACHE_OUTER_WT_INNER_WB   = 11,  // b1011
    HAL_IOMMU_CACHE_OUTER_WB_INNER_NC   = 13,  // b1101
    HAL_IOMMU_CACHE_OUTER_WB_INNER_WT   = 14,  // b1110
    HAL_IOMMU_CACHE_OUTER_WB_INNER_WB   = 15,  // b1111
    // use stage 1 or outer/inner WB
    HAL_IOMMU_CACHE_DEFAULT             = 15,  

    HAL_IOMMU_CACHE_FORCE_ENUM_32_BIT   = 0x7FFFFFFF  /* Force to 32 bit enum */
} HAL_IOMMUCache_t;

// sharability
typedef enum {
    HAL_IOMMU_SHARE_NONE         = 0,
    HAL_IOMMU_SHARE_OUTER        = 2,
    HAL_IOMMU_SHARE_INNER        = 3,
    // use stage 1 or non-cached 
    HAL_IOMMU_SHARE_DEFAULT      = 0,

    HAL_IOMMU_SHARE_FORCE_ENUM_32_BIT   = 0x7FFFFFFF   /* Force to 32 bit enum */
} HAL_IOMMUShare_t;

// permission
typedef enum {
    HAL_IOMMU_PERM_X      = 0x1,
    HAL_IOMMU_PERM_W      = 0x2,
    HAL_IOMMU_PERM_R      = 0x4,
    HAL_IOMMU_PERM_RWX    = HAL_IOMMU_PERM_R | HAL_IOMMU_PERM_W | HAL_IOMMU_PERM_X,
    HAL_IOMMU_PERM_RW     = HAL_IOMMU_PERM_R | HAL_IOMMU_PERM_W,
    HAL_IOMMU_PERM_RX     = HAL_IOMMU_PERM_R | HAL_IOMMU_PERM_X,
    HAL_IOMMU_PERM_WX     = HAL_IOMMU_PERM_W | HAL_IOMMU_PERM_X,
    // user stage 1 or RWX
    HAL_IOMMU_PERM_DEFAULT= HAL_IOMMU_PERM_RWX,

    HAL_IOMMU_PERM_FORCE_ENUM_32_BIT = 0x7FFFFFFF  /* Force to 32 bit enum */
} HAL_IOMMUPerm_t;

typedef enum {
    HAL_IOMMU_UNMAPPED,
    HAL_IOMMU_MAPPED,
    HAL_IOMMU_PARTIAL_MAPPED,
    HAL_IOMMU_INVALID_MAP_STATUS
} HAL_IOMMUMapStatus_t;

// memory map attributes
typedef struct {
    uint64_t     size;
    uint64_t     inputAddr;
    uint64_t     outputAddr;
    HAL_IOMMUPerm_t     perm;
    HAL_IOMMUShare_t    shareAttr;
    HAL_IOMMUCache_t    cacheAttr;
} HAL_IOMMUMapAttr_t;

typedef struct {
    uint32_t    inputBitSize;
    uint32_t    outputBitSize;
} HAL_IOMMUPageTableAttr_t;

typedef enum {
  HAL_IOMMU_ERR_OK = 0,
  HAL_IOMMU_ERR_HAL_SUCCESS = 0,
  HAL_IOMMU_ERR_GENERIC_ERROR,
  HAL_IOMMU_ERR_INVALID_PARAM,
  HAL_IOMMU_ERR_NULL_POINTER,
  HAL_IOMMU_ERR_OUT_OF_RANGE,
  HAL_IOMMU_ERR_REDUNDANT_CONFIGURATION,
  HAL_IOMMU_ERR_VMID_NOT_FOUND,
  HAL_IOMMU_ERR_VMID_NOT_INIT,
  HAL_IOMMU_ERR_INIT_PT_FAILED,
  HAL_IOMMU_ERR_REGISTER_MMU_FAILED,
  HAL_IOMMU_ERR_DEREGISTER_MMU_FAILED,
  HAL_IOMMU_ERR_GET_PT_ATTR_FAILED,
  HAL_IOMMU_ERR_INVALID_CACHE_ATTR,
  HAL_IOMMU_ERR_MAP_FAILED,
  HAL_IOMMU_ERR_UNMAP_FAILED,
  HAL_IOMMU_ERR_INIT_CLOCK_FAILED,
  HAL_IOMMU_ERR_ENABLE_CLOCK_FAILED,
  HAL_IOMMU_ERR_DISABLE_CLOCK_FAILED,
  HAL_IOMMU_ERR_PAGETABLE_NOT_INIT,
  HAL_IOMMU_ERR_MALLOC_FAILED,
  HAL_IOMMU_ERR_GET_IORT_INFO_FAILED,
  HAL_IOMMU_ERR_UNSUPPORTED_INSTANCE_FOR_TARGET,
  HAL_IOMMU_ERR_DOMAIN_IN_USE,
  HAL_IOMMU_ERR_SID_NOT_FOUND,
  HAL_IOMMU_ERR_DEVICE_INCOMPATIBLE_WITH_DOMAIN,
  HAL_IOMMU_ERR_RESULT_SIZE = 0x7FFFFFFF, // force to 32-bit enum
} HAL_IOMMUResult_t;

typedef enum HAL_IOMMUCtxBankType
{
  HAL_IOMMU_CB_S2_CTX,
  HAL_IOMMU_CB_S1_CTX_S2_BYPASS,
  HAL_IOMMU_CB_S1_CTX_S2_FAULT,
  HAL_IOMMU_CB_S1_CTX_S2_NESTED,
} HAL_IOMMUCtxBankType;


struct hal_iommu_fault_info {
    void *p_domain;
    void *FAR;
    DALSYSPropertyHandle device_handle;
    struct {
        unsigned WNR:1;
        unsigned IND:1;
        unsigned PRV:1;
        unsigned MULTI:1;
        unsigned ASYNC:1;
        unsigned PTWF:1;
        unsigned RESERVED:26;
    } flags;
    enum {
        UNSUPPORTED_UPSTREAM_TRANS,
        ADDRESS_SIZE_FAULT,
        TLB_MATCH_CONFLICT,
        EXTERNAL_FAULT,
        PERMISSION_FAULT,
        ACCESS_FLAG_FAULT,
        TRANSLATION_FAULT,
    } type;
};

struct hal_iommu_domain_aarch64 {
    uint64_t TTBR0;
    uint64_t TTBR1;
    uint32_t MAIR0;
    uint32_t MAIR1;
    uint32_t SCTLR;
    uint32_t TCR;
};

enum hal_iommu_vmsa {
    HAL_IOMMU_ARM_ARCH_AARCH64,
    HAL_IOMMU_ARM_ARCH_INVALID,
};

enum hal_iommu_mmu_arch {
    HAL_IOMMU_ARCH_MMU500,
    HAL_IOMMU_ARCH_QSMMU,
    HAL_IOMMU_ARCH_INVALID
};

struct hal_iommu_domain_config {
    enum hal_iommu_vmsa vmsa;
    union {
        struct hal_iommu_domain_aarch64 domain_cfg64;
    } config;
};

enum hal_iommu_bypass_domain_type {
    HAL_IOMMU_AARCH64_NON_CCA_DOMAIN_TYPE,
    HAL_IOMMU_AARCH64_CCA_DOMAIN_TYPE,
    HAL_IOMMU_DOMAIN_TYPE_INVALID
};

typedef void (*fp_hal_iommu_fault_handler_t)(void *p_context, struct hal_iommu_fault_info *p_fault_info);

typedef HAL_IOMMUResult_t (*fp_hal_iommu_domain_create_t)(void **pp_domain);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_domain_delete_t)(void *p_domain);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_domain_attach_device_t)(void *p_domain, void *client_handle, uint32_t arid, uint32_t mask);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_domain_detach_device_t)(void *p_domain, void *client_handle, uint32_t arid, uint32_t mask);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_domain_configure_t)(void *p_domain, struct hal_iommu_domain_config *p_domain_cfg);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_flush_domain_t)(void *p_domain);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_flush_domain_va_list_t)(void *p_domain, boolean last_level, uint32_t number, uint64_t *va_list);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_register_fault_handler_t)(void *p_domain, fp_hal_iommu_fault_handler_t handler, void *p_context);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_configure_bypass_domain_t)(void *p_domain, enum hal_iommu_bypass_domain_type type);
typedef HAL_IOMMUResult_t (*fp_hal_iommu_idiom_detach_t)(void *client_handle, uint32_t arid, uint32_t mask);

typedef struct HAL_IOMMU_fptable_s {
    fp_hal_iommu_domain_create_t                    domain_create_func;
    fp_hal_iommu_domain_delete_t                    domain_delete_func;
    fp_hal_iommu_domain_attach_device_t             domain_attach_func;
    fp_hal_iommu_domain_detach_device_t             domain_detach_func;
    fp_hal_iommu_domain_configure_t                 domain_configure_func;
    fp_hal_iommu_flush_domain_t                     flush_domain_func;
    fp_hal_iommu_flush_domain_va_list_t             flush_domain_va_list_func;
    fp_hal_iommu_register_fault_handler_t           register_fault_handler_func;    
    fp_hal_iommu_configure_bypass_domain_t          config_bypass_domain_func;
    fp_hal_iommu_idiom_detach_t                     idiom_detach_func; 
} HAL_IOMMU_fptable_t;


#define HAL_IOMMU_INVALID_ADDR 0xFFFFFFFF
#define HAL_IOMMU_INVALID_ARID ((uint32_t)0xFFFFFFFF)
#define HAL_IOMMU_INVALID_CB_IDX ((uint32_t)0xFFFFFFFF)

#define HAL_IOMMU_S2CR_TYPE_TRANSLATION_CONTEXT 0
#define HAL_IOMMU_S2CR_TYPE_BYPASS_MODE 1
#define HAL_IOMMU_S2CR_TYPE_FAULT_CONTEXT 2

#define HAL_IOMMU_VALID_MASK 0x80000000
#define HAL_IOMMU_SID_MASK_SHIFT 16
#define HAL_IOMMU_CB_UNUSED_VMID 0xFF

#define HAL_IOMMU_BYPASS_NON_CCA_SCTLR_VAL 0x000000E0
#define HAL_IOMMU_BYPASS_CCA_SCTLR_VAL 0x009F00E0
#define HAL_IOMMU_BYPASS_NON_CCA_TCR_VAL 0x0
#define HAL_IOMMU_BYPASS_CCA_TCR_VAL 0x0

#endif /* __HAL_IOMMU_TYPES_H__ */
