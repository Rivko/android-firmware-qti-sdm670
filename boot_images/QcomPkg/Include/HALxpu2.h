#ifndef HALXPU2_H
#define HALXPU2_H
/**

@file   HALxpu2.h

@brief  This module provides the Application Programing Interface (API) 
        to the XPU2 HAL layer.
*/
/*
===========================================================================

FILE:         HALxpu2.h

DESCRIPTION:  Header file for the HAL XPU2 interface.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
03/24/14   sc      Replaced HALxpu2DataTypes.h with HALcomdef.h
09/26/13   sc      Added support to get and set AMT page size
09/25/13   sc      Updated xPU2 instance enum
09/10/13   sc      Updated xPU2 instance enum
08/09/13   sc      Updated xPU2 instance enum
05/15/13   sc      Updated ResourceGroupClientPermType
05/06/13   sc      Updated Reset API
05/02/13   sc      Updated xPU2 instance enum
03/19/13   sc      Provided support for 1 Meg vs 2 Meg page size for EBI0/1 AMT
02/25/13   sc      Added ClearErrorExt
02/08/13   sc      Added RO access granularity for secure access to its partitions
12/05/12   sc      Updated with MMSS_APU
11/27/12   sc      Migrated to use DevConfig
11/06/12   sc      Updated for v2.2
08/27/12   sc      Added new API to config UMR_{W/R}ACR
07/03/12   sc      Added support to get AMTR and AMT access permissions
06/29/12   sc      Fixed the const type qualifier
06/22/12   sc      Supported xPU2 name string
06/05/12   sc      Added read-after-write support
05/29/12   sc      Updated for Elan
05/18/12   sc      Added APIs to config AMT VMID and clear AMT bits
10/26/11   sc      Initial creation for xPU2
12/20/10   sds     Add CFPB Master and SMMU SFPB default slaves
10/04/10   sds     Add GCC and TLMM xPUs
06/21/10   sds     Updates from code review
03/12/10   sds     Added device type for default slave on devices
02/03/10   sds     Integrate from 2H09 branch after 8650A target testing
01/29/10   sds     Update function comments
01/15/10   sds     Move CFPB1/2 back to match existing HAL API, add
                   CFPB0. BB needs CFPB2, so it's not a wasted slot.
01/12/10   sds     Add features for Phantom/BB XPUs
                   (Default slave ACR, additional device properties)
11/06/09   sds     Fix compile warnings
09/17/09   sds     Changed to doxygen style function commenting
08/25/09   sds     Resource group related function name typos
                   Change to resource group permission struct
08/21/09   sds     Added GetErrorReportingConfig()
02/03/09   sds     Created 

===========================================================================
Copyright (c) 2009-2014, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALcomdef.h"
#include <HALxpu2Target.h>

#define  IMC_HS1_XPU_SCR 0x13C00000
#define  IMC_HS0_XPU_SCR 0x13D00000

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
/** XPU2 Permissions */
#define HAL_XPU2_NO_ACCESS    0x0UL /** no access */
#define HAL_XPU2_RO_ACCESS    0x1UL /** read-only access */
#define HAL_XPU2_WO_ACCESS    0x2UL /** write-only access */
#define HAL_XPU2_FULL_ACCESS  0x3UL /** full access */

/** use by HAL_xpu2_ClearErrorExt to clear all the error status bits */
#define HAL_XPU2_ALL_ERROR_STATUS_BITS  0x7FFFFFFE

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/** XPU2 error status enum */
typedef enum
{
  HAL_XPU2_NO_ERROR = 0,       /**< successful operation */
  HAL_XPU2_INVALID_INSTANCE,   /**< out of range xPU2 instance */
  HAL_XPU2_UNSUPPORTED_INSTANCE_FOR_TARGET,
    /**< unsupported xPU2 instance for the current target */
  HAL_XPU2_UNSUPPORTED_HANDLER,/**< unsupported handler */
  HAL_XPU2_INVALID_BASE_ADDR,  /**< invalid xPU2 instance base address */
  HAL_XPU2_INVALID_PARAM,      /**< invalid passed in parameter */
  HAL_XPU2_READ_WRITE_MISMATCH /**< read-after-write values not match */
} HAL_xpu2_Status;


/** XPU2 Resource ID Type */
typedef uint32 HAL_xpu2_ResourceGroupIDType;

/** XPU2 Resource Group Addresses */
typedef struct
{
  uint32 uStartLower; /**< resource group lower 32 bits of the start address */
  uint32 uStartUpper; /**< resource group upper 32 bits of the start address */
  uint32 uEndLower;   /**< resource group lower 32 bits of the end address */
  uint32 uEndUpper;   /**< resource group upper 32 bits of the end address */
} HAL_xpu2_ResourceGroupAddrType;

/** XPU2 config type */
typedef enum
{
  HAL_XPU2_CONFIG_SECURE     = 0x0, /**< secure configuration request */
  HAL_XPU2_CONFIG_NON_SECURE = 0x1, /**< non-secure configuration request */
  HAL_XPU2_CONFIG_MODEM      = 0x2, /**< modem configuration request */
  HAL_XPU2_CONFIG_COUNT,
  HAL_XPU2_CONFIG_SIZE       = 0x7FFFFFFF,
} HAL_xpu2_ConfigType;

/** VMID related definitions */
/* Maximum allowed VMID value HAL_XPU2_MAX_VMID is defined in HALxpu2Target.h */
#define HAL_XPU2_MASKS_PER_FIELD 32 /**< Number of mask bits per field */
#define HAL_XPU2_PERMS_PER_FIELD 16 /**< Number of access perms per field */

/** number of required VMID mask dwords */
#define HAL_XPU2_MASTER_MASK_WORDS \
        ((HAL_XPU2_MAX_VMID/HAL_XPU2_MASKS_PER_FIELD) + 1)

/** number of required VMID permission dwords */
#define HAL_XPU2_MASTER_PERM_WORDS \
        ((HAL_XPU2_MAX_VMID/HAL_XPU2_PERMS_PER_FIELD) + 1)

/** XPU2 Resource Group Permissions */
typedef struct
{
  bool32 bMultiVMID;          /**< Indicates which union member is valid */
  union
  {
    struct
    {
      uint32 auVMID[HAL_XPU2_MASTER_MASK_WORDS];     /**< VMID to mask */
      uint32 auVMIDPerm[HAL_XPU2_MASTER_PERM_WORDS]; /**< VMID permissions */
    } multi; /**< Union member for multiple-VMID configuration */
    struct
    {
      bool32 bRWGlobalEn;  /**< Full access global enable */
      bool32 bRWVMIDValid; /**< Full access VMID valid? */
      uint32 uRWVMID;      /**< VMID to be assigned full access */
      bool32 bROGlobalEn;  /**< Read-only access global enable */
      bool32 bROVMIDValid; /**< Read-only access VMID valid? */
      uint32 uROVMID;      /**< VMID to be assigned read-only access */
    } single; /**< Union member for single-VMID configuration */
  } Vmid_Type;
} HAL_xpu2_ResourceGroupPermissionType;

/** XPU2 Resource Group client access permissions */
typedef struct
{
  boolean bSecureClientROEn : 1;
    /**< enable secure client read-only access to secure/MSA owned partition?
     *   (RGn_SCR[SCLROE] valid for V2.4 or later;
     *    RGn_MCR[SCLROE] valid for V2.1 or later) */
  boolean bNonSecureClientROEn : 1;
    /**< enable non-secure client read-only access to secure/MSA owned
     *   partition?
     *   (RGn_SCR[VMIDCLROE] valid for V2.2 or later;
     *    RGn_MCR[VMIDCLROE] valid for V2.1 or later) */
  boolean bMSAClientROEn : 1;
    /**< enable MSA client read-only access to secure/MSA owned partition?
     *   (RGn_SCR[MSACLROE] valid for V2.2 or later;
     *    RGn_MCR[MSACLROE] valid for V2.4 or later) */
  boolean bNonSecureClientRWEn : 1;
    /**< enable non-secure client read/write access to secure/MSA owned
     *   partition?
     *   (RGn_SCR[VMIDCLRWE] valid for V2.2 or later;
     *    RGn_MCR[VMIDCLRWE] valid for V2.0 or later) */
  boolean bMSAClientRWEn : 1;
    /**< enable MSA client read/write access to secure partition?
     *   (RGn_SCR[MSACLRWE] valid for V2.2 or later) */
  boolean bSecureClientRWEn : 1;
    /**< enable secure client read/write access to MSA owned partition?
     *   (RGn_MCR[SCLRWE] valid for V2.0 or later) */
} HAL_xpu2_ResourceGroupClientPermType;

/** Complete XPU2 Resource Group Config*/
typedef struct
{
  HAL_xpu2_ResourceGroupAddrType       resAddr;
    /**< resource group address range */
  HAL_xpu2_ResourceGroupPermissionType resPerm;
    /**< resource group vmid permissions */
  HAL_xpu2_ResourceGroupClientPermType clientPerm;
    /**< resource group client access permission */
  HAL_xpu2_ConfigType                  eConfig; 
    /**< indicate if resource group is configured as secure, non-secure
     *   or MSA */
} HAL_xpu2_ResourceGroupConfigType;

/** set the default configuration during initialization */
typedef struct
{
  HAL_xpu2_ResourceGroupPermissionType *pUnmapRGPerm;
    /**< pointer to the structure to set up the unmapped partition
     *   permissions */
  HAL_xpu2_ResourceGroupPermissionType *pAccessMaskTablePerm;
    /**< pointer to the structure to set up the access mask table permissions.
     *   Only valid for MPU and AMT is enabled. */
  boolean  bEnableSecureAMT : 1;
    /**< enable AMT to manage secure memory.  This setting is only supported
     *   for EBI0/1 MPU memory controller */
  boolean  bEnableNonSecureAMT : 1;
    /**< enable AMT to manage non-secure memory.  This setting is only
     *   supported for EBI0/1 MPU memory controller */
  boolean  bEnableVmidCheck : 1; /**< enable VMID based protection checking? */
  boolean  bEnablexPROTNSCheck : 1;
    /**< enable xPROTNS based protection checking? */
  boolean  bSecureConfigWriteDisable : 1;
    /**< TRUE sets secure configuration write disable to prevent accidental
     *   update to all secure configuration registers; FALSE permits secure
     *   configuration access to all registers */
  boolean bEnableSecureAMT2MegPageSize : 1;
    /**< TRUE enables the AMT page size to 2 Meg page to manage the secure
     *   memory.  Supports maximum of 8G memory.  This setting is only
     *   supported for EBI0/1 MPU memory controller and is for revision 2.5
     *   and forward. */
  boolean bEnableNonSecureAMT2MegPageSize : 1;
    /**< TRUE enables the AMT page size to 2 Meg page to manage the non-secure
     *   memory.  Supports maximum of 8G memory.  This setting is only
     *   supported for EBI0/1 MPU memory controller and is for revision 2.5
     *   and forward. */
} HAL_xpu2_DefaultxPU2ConfigType;

/** Error reporting options */
typedef enum
{
  HAL_XPU2_ERROR_O_DECODE_EN     = 0x1,
    /**< Record decode errors */
  HAL_XPU2_ERROR_O_INTERRUPT_EN  = 0x2,
    /**< Generate interrupt on errors */
  HAL_XPU2_ERROR_O_CLIENT_RPT_EN = 0x4,
    /**< Report client errors to master */
  HAL_XPU2_ERROR_O_CFG_RPT_EN    = 0x8,
    /**< Report config errors to master */
  HAL_XPU2_ERROR_O_NS_RES_CLIENT_EN = 0x10,
    /**< Non-secure resource client error */
  HAL_XPU2_ERROR_O_NS_RES_CFG_EN = 0x20,
    /**< Non-secure resource config error */
  HAL_XPU2_ERROR_O_CLIENT_INT_EN = 0x40,
    /**< Generate client port interrupt on errors (for V2.1 or later) */
  HAL_XPU2_ERROR_O_CFG_INT_EN    = 0x80,
    /**< Generate config port interrupt on errors (for V2.1 or later) */
  HAL_XPU2_ERROR_O_SIZE   = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_xpu2_ErrorOptionsType;

/** Store error options ORed in this */
typedef uint32 HAL_xpu2_ErrorOptionsConfigType;

/** Supported xPU2 device types */
typedef enum
{
  HAL_XPU2_DEVICE_RPU            = 0,           /**< Register protection unit */
  HAL_XPU2_DEVICE_APU            = 1,           /**< Area protection unit */
  HAL_XPU2_DEVICE_MPU            = 2,           /**< Memory protection unit */
  HAL_XPU2_DEVICE_COUNT,
  HAL_XPU2_DEVICE_DNE            = 0x7FFFFFFE,  /**< device does not exist */
  HAL_XPU2_DEVICE_SIZE           = 0x7FFFFFFF,  /* force to 32-bit enum */
} HAL_xpu2_XPUDeviceType;

/** AMT page size */
typedef enum
{
  HAL_XPU2_AMT_1MB_PAGE,
  HAL_XPU2_AMT_2MB_PAGE,
  HAL_XPU2_AMT_DNE_PAGE          = 0x7FFFFFFE,  /**< AMT page size does not exist */
  HAL_XPU2_AMT_SIZE              = 0x7FFFFFFF,  /* force to 32-bit enum */
} HAL_xpu2_AMTPageSizeType;

/** XPU2 device parameters */
typedef struct
{
  HAL_xpu2_XPUDeviceType eXpu2Type; /**< XPU2 type */
  uint32  uResourceGroupCount; /**< Total number of Resource Groups */
  uint32  uNumVMID;            /**< number of VMID supported */
  uint32  uResourceGroupRes;   /**< Minimal RG size (for MPUs) */
  boolean bMultiVMID : 1;      /**< Multi-VMID type access control? */
  boolean bFullPerm : 1;       /**< Full permissions (RW/WO/RO/NO)? */
  boolean bAMTSupport : 1;     /**< Support access mask table? (for MPUs) */
  boolean bMSASupport : 1;     /**< Support modem self authentication? */
  HAL_xpu2_AMTPageSizeType eAMTPageSize; /**< AMT page size */
  const char *szXpu2Name;      /**< pointer to XPU2 string name */
} HAL_xpu2_XPUDeviceParamsType;

/** XPU2 Error flags */
typedef enum
{
  HAL_XPU2_ERROR_F_CONFIG_PORT = 0x1,  /**< Configuration port error */
  HAL_XPU2_ERROR_F_CLIENT_PORT = 0x2,  /**< Client port error */
  HAL_XPU2_ERROR_F_MULTIPLE    = 0x4,  /**< Multiple errors */
  HAL_XPU2_ERROR_F_MULTI_CONFIG_PORT = 0x8,
    /**< For revision 2.1 or later: Multiple configuration port errors */
  HAL_XPU2_ERROR_F_MULTI_CLIENT_PORT = 0x10,
    /**< For revision 2.1 or later: Multiple client port errors */
  HAL_XPU2_ERROR_FLAGS_SIZE    = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_xpu2_ErrorFlagsType;

/** Store error flags ORed in this */
typedef uint32 HAL_xpu2_ErrorFlagsConfigType;

/** Bus related error flags */
typedef enum
{
  HAL_XPU2_BUS_F_ERROR_AC     = 0x1,
    /**< Bus reports access control error */
  HAL_XPU2_BUS_F_ERROR_DCD    = 0x2,    /**< Bus reports decode error */
  HAL_XPU2_BUS_F_ASHARED      = 0x4,    /**< ASHARED Field */
  HAL_XPU2_BUS_F_AINNERSHARED = 0x8,    /**< AINNERSHARED Field */
  HAL_XPU2_BUS_F_APRIV        = 0x10,   /**< APRIV Field */
  HAL_XPU2_BUS_F_APROTNS      = 0x20,   /**< APROTNS Field */
  HAL_XPU2_BUS_F_AINST        = 0x40,   /**< AINST Field */
  HAL_XPU2_BUS_F_AWRITE       = 0x80,   /**< AWRITE Field */
  HAL_XPU2_BUS_F_AOOO         = 0x100,  /**< AOOO Field */
  HAL_XPU2_BUS_F_AFULL        = 0x200,  /**< AFULL Field */
  HAL_XPU2_BUS_F_ABURST       = 0x400,  /**< ABURST Field */
  HAL_XPU2_BUS_F_BURSTLEN     = 0x800,  /**< Client burst length error */
  HAL_XPU2_BUS_F_ARDALLOCATE  = 0x1000, /**< ARDALLOCATE field */
  HAL_XPU2_BUS_F_AEXCLUSIVE   = 0x2000, /**< AEXCLUSIVE field */
  HAL_XPU2_BUS_F_ARDBEADNDXEN = 0x4000, /**< ARDBEADNDXEN field */
  HAL_XPU2_BUS_F_AMSSSELFAUTH = 0x8000, /**< AMSSSELFAUTH field */
  HAL_XPU2_BUS_F_SAVERESTORE_IN_PROG = 0x10000,
    /**< save/restore in progress during error */
  HAL_XPU2_BUS_F_MSA_RG_MATCH = 0x20000, /**< MSA resource match error */
  HAL_XPU2_BUS_F_SECURE_RG_MATCH = 0x40000,
    /**< Secure resource match error */
  HAL_XPU2_BUS_F_NONSECURE_RG_MATCH = 0x80000,
    /**< Non-Secure resource match error */
  HAL_XPU2_BUS_FLAGS_SIZE     = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_xpu2_BusErrorFlagsType;

/** Store bus flags ORed in this */
typedef uint32 HAL_xpu2_BusErrorFlagsMaskType;

/** Full xPU2 error reporting structure */
typedef struct
{
  HAL_xpu2_ErrorFlagsConfigType uErrorFlags; /**< Error Flags */
  HAL_xpu2_BusErrorFlagsMaskType uBusFlags;  /**< Bus specific flags */
  uint32 uPhysicalAddressLower32;
    /**< The lower 32 bits of the physical address of the errant request */
  uint32 uPhysicalAddressUpper32;
    /**< The upper 32 bits of the physical address of the errant request */
  uint32 uMasterId;        /**< The master ID of the offending master */
  uint32 uAVMID;
    /**< The virtual master ID of the offending master */
  uint32 uATID;            /**< The ATID field of the errant request */
  uint32 uABID;            /**< The ABID field of the errant request */
  uint32 uAPID;            /**< The APID field of the errant request */
  uint32 uALen;            /**< The ALEN field of the errant request */
  uint32 uASize;           /**< The ASIZE field of the errant request */
  uint32 uAMemType;        /**< The AMEMTYPE field of the errant request */
  uint32 uAPReqPriority;
    /**< The APREQPRIORITY field of the errant request */
} HAL_xpu2_ErrorType;

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** @name Mandatory Interface */
/** @{ */

/** 
@brief Initialization function.

  This function initializes the unmapped partition permissions, the access
  mask table permissions, the AMT enabling to manage secure or non-secure
  memory, the xPROTNS and/or VMID based protection checking, and the access to
  the secure write registers.

@param[in] eXpu2 - The xpu2 to initalize
@param[in] pDefaultConfig - A pointer to the structure for the initialization
                            settings
@param[out] ppszVersion - Pointer to version string.  Currently not populated.

@return The error status of the xPU2.
*/
HAL_xpu2_Status HAL_xpu2_Init
  ( uint64 baseAddr,
    const HAL_xpu2_DefaultxPU2ConfigType *pDefaultConfig,
    char ** ppszVersion );

/**
@brief Reset the selected XPU2 hardware to a default state.

@param[in] eXpu2 - The XPU2 hardware to reset.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu2_Status HAL_xpu2_Reset   ( uint64 baseAddr ) ;

/**
@brief Save the current xPU2 state.  Currently stub out.

@param[in] eXpu2 - The XPU2 state to save.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_Restore()
*/
void HAL_xpu2_Save    ( uint64 baseAddr );

/**
@brief Restore an xPU2 to a previously saved state.  Currently stub out.

@param[in] eXpu2 -The XPU2 to have its state restored from a previous save.

@pre HAL_xpu2_Init() and HAL_xpu2_Save() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_Save()
*/
void HAL_xpu2_Restore ( uint64 baseAddr );
/** @} */

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/** @name Configuration Interface */
/** @{ */

/**
@brief Configure access to the xPU2 core registers.

   The purpose of this function is to configure access permissions to
   the XPU2's configuration registers. The only acceptable access permissions
   are Full Access and no access. Read only and Write only will be interpreted
   as no access for this operation.

@param[in] eXpu2 - The XPU2 to configure
@param[in] pAccessConfig - A pointer to the XPU2 access permission config
                           struct to use.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_GetAccessPermissions()
*/
HAL_xpu2_Status HAL_xpu2_ConfigAccessPermissions
  ( uint64 baseAddr,
    const HAL_xpu2_ResourceGroupPermissionType *pAccessConfig );

/**
@brief Configure access mask table permissions.

   This function configures the access mask table permissions.  The
   configuration is only valid for MPU and when access mask table is supported.

@param[in] eXpu2 - The XPU2 to configure
@param[in] pAMTPerm - A pointer to the structure to use to set up the AMT
                      access control permissions.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_GetAccessMaskTablePermissions()
*/
HAL_xpu2_Status HAL_xpu2_ConfigAccessMaskTablePermissions
  ( uint64 baseAddr,
    const HAL_xpu2_ResourceGroupPermissionType *pAMTPerm );

/**
@brief Configure unmapped partition access control permissions.

   This function configures the unmapped partition access control permissions.
   The configuration is only valid for MPU.

@param[in] eXpu2 - The XPU2 to configure
@param[in] pUnmapRGPerm - A pointer to the structure to use to set up the unmapped
                          partition access control permissions.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_GetUnmappedPartitionPermissions()
*/
HAL_xpu2_Status HAL_xpu2_ConfigUnmappedPartitionPermissions
  ( uint64 baseAddr,
    const HAL_xpu2_ResourceGroupPermissionType *pUnmapRGPerm );

/**
@brief Enable xMssSelfAuth based protection checking

   This function enables the xMssSelfAuth (MSA) based protection checking.

@param[in] eXpu2 - The XPU2 to disable.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
void HAL_xpu2_EnableMSSSelfAuth( uint64 baseAddr);

/**
@brief Enable MPU partition

   This function enables the MPU partition with the specified resource group
   to be used in client permission determination.  This functionality is
   supported in revision 2.1 or later.

@param[in] eXpu2 - The XPU2 to enable.
@param[in] uResID - The resource group to set up.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu2_Status HAL_xpu2_EnableMPUPartition
  ( uint64 baseAddr,
    HAL_xpu2_ResourceGroupIDType uResID );

/**
@brief Disable MPU partition

   This function disables the MPU partition with the specified resource group
   and the content of xPU_RGn_* has no effect on access permission
   determination.  This functionality is supported in revision 2.1 or later.

@param[in] eXpu2 - The XPU2 to disable.
@param[in] uResID - The resource group to set up.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu2_Status HAL_xpu2_DisableMPUPartition
  ( uint64 baseAddr,
    HAL_xpu2_ResourceGroupIDType uResID );

/**
@brief Configure resource group access control.

   The purpose of this function is to allow for a one step setup for a
   resource group; start and end address, as well as any permissions for the
   resource group.  The common use for this function would be initial setup
   for a freshly initialized XPU2.

@param[in] eXpu2 - The XPU2 to set up a resource group in.
@param[in] uResID - The resource group to set up.
@param[in] pResConfig - A pointer to the configuration structure to use for the
                        resource group to be configured.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_GetResourceGroupConfig()
*/
HAL_xpu2_Status HAL_xpu2_ConfigResourceGroup
  ( uint64 baseAddr,
    HAL_xpu2_ResourceGroupIDType            uResID,
    const HAL_xpu2_ResourceGroupConfigType *pResConfig );

/**
@brief Configure the access mask table

   This function configures the access mask table with the specified array of
   the mask attributes and the number of elements in the mask attribute
   array.  The mask attribute array contains the bit position to set the 
   associated 1 Meg address range that is managed using AMT.  For example,
   the mask attribute array may have [0, 100, 4095] which sets bits 0, 100 
   and 4095 to 1.  This is only valid for MPU and AMT mode is enabled.  Each
   bit in the mask attribute corresponds to 1 Meg address space.  For
   instance, bit 0 represents address range between address 0 to 1 Meg;
   bit 1 represents address range between address 1 Meg to 2 Meg, and so on.

@param[in] eXpu2 - The XPU2 to enable.
@param[in] pMaskAttribArray - A pointer to the access mask array that contains
                              the bits to configure
@param[in] uNumElements - The number of elements in the mask table array

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ClearAccessMaskTable()
@sa HAL_xpu2_GetAccessMaskTable()
*/
HAL_xpu2_Status HAL_xpu2_ConfigAccessMaskTable
  ( uint64 baseAddr,
    const uint32                 *pMaskAttribArray,
    uint32                        uNumElements );

/**
@brief Clear access mask table bits

   This function clears the access mask table of the specified bits in the
   array of the mask attributes and the number of elements in the mask
   attributes array.  The mask attributes array contains the bit position to be
   cleared.  For example, the mask attribute array may have [0, 100, 4095]
   which clears bits 0, 100 and 4095.  This is only valid for MPU and when AMT
   mode is supported.

@param[in] eXpu2 - The XPU2 to clear the AMT bits
@param[in] pMaskAttribArray - A pointer to the access mask array that contains
                              the bits to clear
@param[in] uNumElements - The number of elements in the mask table array

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigAccessMaskTable()
*/
HAL_xpu2_Status HAL_xpu2_ClearAccessMaskTable
  ( uint64 baseAddr,
    const uint32               *pMaskAttribArray,
    uint32                      uNumElements );

/**
@brief Dynamically set the Access Mask Table (AMT) page size

   This function sets the AMT page size associated with each AMT register bit
   dynamically.  The supported AMT page size is 1MB or 2MB for revision 2.5 and
   forward.  The default AMT page size for the xPU2 instance that supports AMT
   mode is 1MB which covers maximum 4GB memory.  Similarly, 2MB AMT page size
   setting covers maximum 8GB memory.  This setting is only supported for
   EBI0/1 MPU memory controller.

@param[in] eXpu2 - The XPU2 to set the AMT page size
@param[in] eAMTPageSize - The AMT page size to be set
@param[in] eConfig - which config type to set the AMT page size

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_GetDeviceParameters()
*/
HAL_xpu2_Status HAL_xpu2_ConfigAccessMaskTablePageSize
  ( uint64 baseAddr,
    HAL_xpu2_AMTPageSizeType    eAMTPageSize,
    HAL_xpu2_ConfigType         eConfig );

/**
@brief Configure xPU2 error options.

   This includes whether or not:
   @li Client errors are reported to the requesting master and target slave
   @li Config errors are reported to the requesting master and target slave
   @li Interrupts are generated when errors occur
   @li Decode errors to the configuration port generate xPU2 errors
   @li Non-secure resource client error is enabled for secure configuration
   @li Non-secure resource config error is enabled for secure configuration

@param[in] eXpu2 - The XPU2 to configure device options on
@param[in] eConfig - which config type to set the error options
@param[in] errOpt - The error options configuration

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_GetErrorReportingConfig()
*/
HAL_xpu2_Status HAL_xpu2_ConfigErrorReporting
  ( uint64 baseAddr,
    HAL_xpu2_ConfigType             eConfig,
    HAL_xpu2_ErrorOptionsConfigType errOpt );
/** @} */

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/** @name Data Interface */
/** @{ */

/**
@brief Get xPU2 hardware device parameters.

   This function reads the xPU2 device operating parameters from the hardware.
   This includes device type, device variant, and resource group information.

@param[in]  eXpu2 - The XPU2 to get parameters for
@param[out] pDevParams - A pointer to the structure to store the device
                         parameters

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
void HAL_xpu2_GetDeviceParameters( uint64 baseAddr,
                                   HAL_xpu2_XPUDeviceParamsType *pDevParams );

/**
@brief Read the current access permissions for the xPU2's core registers.


@param[in] eXpu2 - The XPU2 to obtain extended configuration information from
@param[out] pAccessConfig - A pointer to the structure to use to return the
                            XPU2 access permission configuration.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigAccessPermissions()
*/
void HAL_xpu2_GetAccessPermissions
  ( uint64 baseAddr,
    HAL_xpu2_ResourceGroupPermissionType *pAccessConfig );

/**
@brief Read the current access mask table permissions.

   This function reads the access mask table permissions.  This operation is
   only valid for MPU and when AMT mode is supported.

@param[in] eXpu2 - The XPU2 to obtain the AMT access control permission info
@param[out] pAMTPerm - A pointer to the structure to use to return the AMT
                      access control permissions.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigAccessMaskTablePermissions
*/
void HAL_xpu2_GetAccessMaskTablePermissions
  ( uint64 baseAddr,
    HAL_xpu2_ResourceGroupPermissionType *pAMTPerm );

/**
@brief Read the current unmapped partition access control permissions.

   This function reads the unmapped partition access control permissions.
   This operation is only valid for MPU.

@param[in] eXpu2 - The XPU2 to obtain the unmapped partition access control
                   permission info
@param[out] pUnmapRGPerm - A pointer to the structure to use to return the
                           unmapped partition access control permissions.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigUnmappedPartitionPermissions
*/
void HAL_xpu2_GetUnmappedPartitionPermissions
  ( uint64 baseAddr,
    HAL_xpu2_ResourceGroupPermissionType *pUnmapRGPerm );

/**
@brief Read the current access configuration for a specific resource group.

@param[in]  eXpu2 - The XPU2 to obtain the resource group configuration info
@param[in]  uResID - The resource group to read.
@param[in/out] pResConfig - A pointer to the structure to use to return the
                            resource group configuration.
                            For single VMID, RWVMID/ROVMID info is required
                            to get the resource group configuration.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigResourceGroup()
*/
void HAL_xpu2_GetResourceGroupConfig
  ( uint64 baseAddr,
    HAL_xpu2_ResourceGroupIDType          uResID,
    HAL_xpu2_ResourceGroupConfigType     *pResConfig );

/**
@brief Read the bits set in access mask table

   This function reads the bits set info from the access mask table.  If
   \c pMaskAttribArray is NULL, the actual number of elements is returned in
   \c puNumElements.  Otherwise, \c pMaskAttribArray contains the AMT bits
   info.  For example, pMaskAttribArray may have [0, 100, 4095] which
   indicates bits 0, 100 and 4095 are set in the access mask table.  This is
   only valid for MPU and when AMT mode is supported.

@param[in] eXpu2 - The XPU2 to obtain the AMT bits info
@param[in/out] pMaskAttribArray - A pointer to the access mask array that
                                  contains the AMT bits set info.
                                  If NULL, actual number of elements is
                                  returned in puNumElements.
@param[in/out] puNumElements - A pointer to the number of elements in the mask
                               table array

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigAccessMaskTable()
@sa HAL_xpu2_ClearAccessMaskTable()
*/
void HAL_xpu2_GetAccessMaskTable
  ( uint64 baseAddr,
    uint16                     *pMaskAttribArray,
    uint32                     *puNumElements );

/**
@brief Check if access mask table bits are set.

   This function checks if all the access mask table bits are set in
   \c pMaskAttribArray.  The mask attribute array contains the bit position
   to be checked that the associated 1 Meg address range is managed using AMT.
   For example, the mask attribute array may have [0, 100, 4095] which means
   bits 0, 100 and 4095 are to be checked if the corresponding bits in AMT
   register are set to 1.  This is only valid for MPU and AMT mode is enabled.

@param[in] eXpu2 - The XPU2 to check the AMT bits.
@param[in] pMaskAttribArray - A pointer to the access mask array that contains
                              the bits to be checked
@param[in] uNumElements - The number of elements in the mask table array

@return TRUE if all AMT bits in the mask attibute array are set.
        FALSE otherwise.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigAccessMaskTable()
@sa HAL_xpu2_GetAccessMaskTable()
*/
bool32 HAL_xpu2_AreAMTBitsSet
  ( uint64 baseAddr,
    const uint16                 *pMaskAttribArray,
    uint32                        uNumElements );

/**
@brief Get the current xPU2 error options.

   This includes whether or not:
   @li Client errors are reported back to the requesting master
   @li Config errors are reported back to the requesting master
   @li Interrupts are generated when errors occur
   @li Decode errors to the configuration port generate xPU2 errors
   @li Non-secure resource client error is enabled for secure configuration
   @li Non-secure resource config error is enabled for secure configuration

@param[in]  eXpu2 - The xPU2 to get device options from
@param[in] eConfig - which config type to get the error options from
@param[out] pDevConfig - A pointer to the location to store the device options
                    configuration

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigErrorReporting()
*/
void HAL_xpu2_GetErrorReportingConfig
  ( uint64 baseAddr,
    HAL_xpu2_ConfigType              eConfig,
    HAL_xpu2_ErrorOptionsConfigType *pErrConfig );

/**
@brief Check for an xPU2 error condition.

   This function checks for the xPU2 error condition per the config type.

@param[in] eXpu2 - The XPU2 to obtain error information from
@param[in] eConfig - which config type to obtain the error info

@return The error status of the xPU2.

@retval TRUE  The xPU2 has recorded an error.
@retval FALSE The xPU2 has no recorded errors.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
bool32 HAL_xpu2_IsError          ( uint64 baseAddr,
                                   HAL_xpu2_ConfigType eConfig );

/**
@brief Get the pending xPU2 error per config type.

   This includes: the error type (access control or decode) and the relevant
   bus parameters for the errant request.

@param[in]  eXpu2 - The XPU2 to obtain error information from
@param[in]  eConfig - which config type to obtain the error info
@param[out] pError - A pointer to the structure to use to return the xPU2 error

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
void HAL_xpu2_GetError( uint64 baseAddr,
                        HAL_xpu2_ConfigType                   eConfig,
                        HAL_xpu2_ErrorType                   *pError );

/**
@brief Clear the xPU2 device error status.

@param[in] eXpu2 - The XPU2 to clear errors on
@param[in] eConfig - which config type to clear errors on

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu2_Status HAL_xpu2_ClearError( uint64 baseAddr,
                                     HAL_xpu2_ConfigType  eConfig );

/**
@brief Clear the specific xPU2 error bit(s).

@param[in] eXpu2 - The XPU2 to clear the error bit(s)
@param[in] eConfig - which config type to clear error bit(s) on
@param[in] uErrorFlags - the ORed error status bit(s) to be cleared or
                         HAL_XPU2_ALL_ERROR_STATUS_BITS to clear all
                         error status bits

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu2_Status HAL_xpu2_ClearErrorExt( uint64 baseAddr,
                                        HAL_xpu2_ConfigType  eConfig,
                                        HAL_xpu2_ErrorFlagsConfigType uErrorFlags );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef HALXPU2_H */

