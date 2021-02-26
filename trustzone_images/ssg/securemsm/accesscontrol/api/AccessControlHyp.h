#ifndef ACCESS_CONTROL_HYP_H
#define ACCESS_CONTROL_HYP_H
/*==============================================================================
  @file ACSmmu.h
  Public interface for Access Control SMMU specific functionality

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Technologies Inc. Confidential and Proprietary
  ==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/accesscontrol/api/AccessControlHyp.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $


  when       who      what, where, why
  --------   ---      ------------------------------------
  09/02/14   ps       Created

  ==============================================================================*/
/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include <comdef.h>
#include "ACCommon.h"
#include "tz_syscall_pub.h"


/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define AC_PERM_VALID 0b01
#define AC_PERM_DONTCARE 0b11
#define AC_PERM_INVALID 0b00
#define AC_PERM_READ_BITS  4   /*Bits 5:4 give us info about read permissions */
#define AC_PERM_WRITE_BITS  2  /*Bits 3:2 give us info about write permissions */
#define AC_PERM_EXECUTE_BITS 0 /*Bits 1:0 give us infot about execute permissions */
#define AC_PERM_DEFAULT 0b11000000 /*R W X bits are set to none. We ignore bits 7:6*/ 
#define AC_PERM_RULES_NONE AC_PERM_DEFAULT
#define AC_PERM_RULES_R  (AC_PERM_DEFAULT | (AC_PERM_VALID << AC_PERM_READ_BITS))
#define AC_PERM_RULES_W (AC_PERM_DEFAULT | (AC_PERM_VALID << AC_PERM_WRITE_BITS))
#define AC_PERM_RULES_X (AC_PERM_DEFAULT | (AC_PERM_VALID << AC_PERM_EXECUTE_BITS))
#define AC_PERM_RULES_R_DONTCARE (AC_PERM_DEFAULT | (AC_PERM_DONTCARE << AC_PERM_READ_BITS))
#define AC_PERM_RULES_W_DONTCARE (AC_PERM_DEFAULT | (AC_PERM_DONTCARE << AC_PERM_WRITE_BITS))
#define AC_PERM_RULES_X_DONTCARE (AC_PERM_DEFAULT | (AC_PERM_DONTCARE << AC_PERM_EXECUTE_BITS))
#define AC_MAX_RULE_NAME_LENGTH 30

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

typedef enum
{
    AC_CLEAR_MEMORY_ON_ASSIGN                 = 0,
    AC_CLEAR_MEMORY_ON_UNASSIGN               = 1,
    AC_CLEAR_MEMORY_ONLY_ON_SECURE_DEVICE      = 2,
    AC_CLEAR_MEMORY_LAST,
    AC_CLEAR_MEMORY_UNSUPPORTED = 0x7FFFFFFF,
}ACClearMemoryFlag;

typedef enum
{
    // device non-gathering, non-reordering, no early write acknowledgement
    AC_CACHE_DEVICE_nGnRnE       = 0,   // b0000
    AC_CACHE_DEVICE_nGnRE        = 1,   // b0001
    AC_CACHE_DEVICE_nGRE         = 2,   // b0010
    AC_CACHE_DEVICE_GRE          = 3,   // b0011
    // outer non-cacheable, inner non-cacheable
    AC_CACHE_OUTER_NC_INNER_NC   = 5,   // b0101
    // outer non-cacheable, inner write-through
    AC_CACHE_OUTER_NC_INNER_WT   = 6,   // b0110
    // outer non-cacheable, inner writeback
    AC_CACHE_OUTER_NC_INNER_WB   = 7,   // b0111
    AC_CACHE_OUTER_WT_INNER_NC   = 9,   // b1001
    AC_CACHE_OUTER_WT_INNER_WT   = 10,  // b1010
    AC_CACHE_OUTER_WT_INNER_WB   = 11,  // b1011
    AC_CACHE_OUTER_WB_INNER_NC   = 13,  // b1101
    AC_CACHE_OUTER_WB_INNER_WT   = 14,  // b1110
    AC_CACHE_OUTER_WB_INNER_WB   = 15,  // b1111
    // use stage 1 or outer/inner WB
    AC_MEMORY_ATTR_DEFAULT             = 15, 

    AC_MEMORY_ATTR_SUPPORTED_MAX,
    AC_MEMORY_ATTR_UNSUPPORTED = 0x7FFFFFFF, // force to 32-bit enum
} ACMemoryAttributesType;


typedef enum
{

    AC_MEMORY_SHAREABLE_NONE         = 0,
    AC_MEMORY_SHAREABLE_OUTER        = 2,
    AC_MEMORY_SHAREABLE_INNER        = 3,
    // use stage 1 or non-cached 
    AC_MEMORY_SHAREABLE_DEFAULT      = 0,
    AC_MEMORY_SHAREABLE_SUPPORTED_MAX,
    AC_MEMORY_SHAREABLE_SIZE = 0x7FFFFFFF,  // force to 32-bit enum

} ACMemoryShareabilityType;

// mapping attributes for populating the Block/Page descriptor
typedef struct
{
    uint64                        uSize; /* In Bytes */
    uint64                        uInputAddr;
    uint64                        uOutputAddr;
    uint32                        uPermissions; 
    ACMemoryShareabilityType      eShareability;
    ACMemoryAttributesType        eMemAttributes;
} ACMapAttributesType;

typedef struct
{
    uint64 uSize; /* In Bytes */
    uint64 uInputAddr;
}ACUnmapAttributesType;


typedef struct 
{
    ACVirtualMachineId eDestinationVm; 
    uint32 uPermissions; 
    ACMemoryShareabilityType eShareability; 
    ACMemoryAttributesType eMemAttributes;
    uint64 ctx;
    uint32 uCtxSize;
}ACDestinationVmAndMemAttrType;

typedef struct
{
    ACMemoryShareabilityType eShareability; 
    ACMemoryAttributesType eMemAttributes;
}ACDestinationAttrType;

typedef struct 
{
    uint64 uSize; /* In Bytes */
    uint64 uInputAddr;
    uint64 uOutputAddr;
}ACMapAddrType;

typedef struct
{
    ACVirtualMachineId eDestinationVm;
    uint8 uPermission;
}ACDestinationInfo;

struct _ACRulesInfo
{
    uint32                            uRuleIndex;
    uint64                            uSourceMask;
    bool32                            bClear;
    bool32                            bAllowExternalCalls;
    bool32                            bClearOnlyOnSecureDevice;
    ACDestinationInfo                   *pDestinationVmAndPerm;
    uint32                            uNumDestinations;
};

typedef struct _ACRulesInfo ACRulesInfo;

typedef struct
{
	ACRulesInfo *pRule;
    uint32 uNumRules;
}ACRulesGroup;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
  @brief ACStaticConfigInit - Initializes AC SMMU static configs 
  @return AC_SUCCESS on success, failure values on Failure
  */
int ACStaticConfigInit(void);

/**
 * Checks if the given list of VMs exclusively own the region specified
 * @return AC_SUCCESS if VMs exclusively own the region, otherwise it returns an error code
 * of type AC_ERROR_CODE
 */
AC_ERROR_CODE ACCheckIfVMsOwnTheRegion(hyp_memprot_dstVM_perm_info_t *pVMList,
        uint32 uNumVms,
        ACMapAddrType *attr,
        bool32 bExternalFlag);

/**
  @brief ACMapIORange: Maps IO region in a VMs pagetable 
  @param eVm: VM enum to map the IO region VM_ALL is not supported for this API
  @param pAttr: Pointer to an array of structures of type 
  ACMapAttributesType
  @param uNumMappings: Number of mappings pointed by pAttr
  @return AC_SUCCESS on success, failure values on Failure
  */
int ACMapIoRange(ACVirtualMachineId eVm, 
        const ACMapAttributesType *pAttr,
        uint32 uNumMappings);

/**
  @brief ACUnmapIORange: Unmaps IO region from a VMs pagetable 
  @param eVm: VM enum to unmap the IO region from
  @param puAddress: Pointer to an array of addresseses to be unmapped
  @param uNumAddresses: Number of addresses pointed by puAddress
  @return AC_SUCCESS on success, failure values on Failure
  */
int ACUnmapIoRange(ACVirtualMachineId eVm, 
        const ACUnmapAttributesType *pUnmapAttributes,
        uint32 uNumMappings);



/**
IMPORTANT: This is to be used only by HYPX
@brief ACHlosMapMemoryRange: Unmaps from HLOS & maps to HLOS as RO or RW
@param pMappings: Pointer to an array of structures of type 
ACMapAddrType
@param uNumMappings: Number of mappings pointed by pMappings
@param uPerm: The permission for the mappings
@return AC_SUCCESS on success, failure values on Failure
*/
AC_ERROR_CODE ACHlosMapMemoryRange(ACMapAddrType *pMappings,
        uint32 uNumMappings,
        uint32 uPerm);

/**
  @brief ACMapMemoryRangeExt: Maps memory range in a pagetable. This API is used when 
  different permissions nneds to be given to different DestinationVMs
  VM_ALL is not supported
  @param eDestinationVmidList: List of all the destination VMIDs
  @param uNumDestinationVmid: Number of VMIDs in the destination lists
  @param eSourceVmidList: List of all the source VMIDs
  @param uNumSourceVmid: Number of VMIDs in the source lists
  @param pAttr: Pointer to an array of structures of type 
  ACMapAttributesType
  @param uNumMappings: Number of mappings pointed by pAttr
  @return AC_SUCCESS on success, failure values on Failure
  */
int ACMapMemoryRangeExt(ACDestinationVmAndMemAttrType *pACDestinationVmAndMemAttrList, 
        uint32 uNumDestinationVmAndMemAttr,
        ACVirtualMachineId *pSourceVmList,
        uint32 uNumSourceVm,
        ACMapAddrType *pMappings,
        uint32 uNumMappings);

/**
  @brief ACMapMemoryRange: Maps memory range in a pagetable. This API is used when 
  different permissions nneds to be given to different DestinationVMs 
  VM_ALL is not supported
  @param eDestinationVmidList: List of all the destination VMIDs
  @param uNumDestinationVmid: Number of VMIDs in the destination lists
  @param eSourceVmidList: List of all the source VMIDs
  @param uNumSourceVmid: Number of VMIDs in the source lists
  @param pAttr: Pointer to an array of structures of type 
  ACMapAttributesType
  @param uNumMappings: Number of mappings pointed by pAttr
  @return AC_SUCCESS on success, failure values on Failure
  */
  // THIS CALL IS TREATED AS EXTERNAL CALL ONLY
  // USE "Ext" API for internal purposes
int ACMapMemoryRange(hyp_memprot_dstVM_perm_info_t *pDestinationVMList, 
        uint32 uNumDestinationVm,
        uint32 *pSourceVmList,
        uint32 uNumSourceVm,
        hyp_memprot_ipa_info_t *pIPAinfo,
        uint32 uNumMappings);



/**
  IMPORTANT NOTE : ACHlosMapMemoryRange function declaration 
  needs to be in Sync with HYPX definition.
  */
typedef AC_ERROR_CODE (*ACHandler) (ACMapAddrType *pMappings,
        uint32 uNumMappings,
        uint32 uPerm); 


void * ACAllocateMemory(uint64 uSize);

void ACFreeMemory(void *pNode, uint64 uSize);

/**
  @brief ACDeleteSHMBridge: Marks the memory as shared in HYP database
  @param uHandle: Handle of the SHM bridge
  @return AC_SUCCESS on success, failure values on Failure
  */
int ACHypDeleteSHMBridge(uint64 uHandle, ACVirtualMachineId eCallerVmId);

void *ACSmmuGetSharedMemoryTable(void);

#endif /* #ifndef ACCESS_CONTROL_HYP_H */
