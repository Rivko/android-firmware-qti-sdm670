#ifndef HALXPU3_H
#define HALXPU3_H
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

$Header: //components/rel/ssg.tz/1.0.2/securemsm/accesscontrol/src/components/xpu/drivers/xpu3/hal/inc/HALxpu3.h#1 $

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
Copyright (c) 2009-2015
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <HALxpu3Target.h>
#include "HALcomdef.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
/** XPU2 Permissions */
#define HAL_XPU3_NO_ACCESS    0x0UL /** no access */
#define HAL_XPU3_RO_ACCESS    0x1UL /** read-only access */
#define HAL_XPU3_WO_ACCESS    0x2UL /** write-only access */
#define HAL_XPU3_FULL_ACCESS  0x3UL /** full access */

/** use by HAL_xpu2_ClearErrorExt to clear all the error status bits */
#define HAL_XPU2_ALL_ERROR_STATUS_BITS  0x7FFFFFFE

#define NUMS_QAD 0x2
#define NUMS_QAD_BITS 0x2

#define HAL_XPU3_MAX_RG_PER_FIELD 32
#define HAL_MAX_RG_STREG_IN_XPU  10
#define HAL_MAX_MSB_ADDR_WIDTH_SUPPORTED 64

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
  HAL_XPU2_READ_WRITE_MISMATCH,    /**< read-after-write values not match */
  HAL_XPU2_UNSUPPORTED_CFG_FOR_XPU /**< unsupported config for XPU */
} HAL_xpu3_Status;

/** XPU2 type */
typedef enum
{
  HAL_XPU2_APCS_MPU , /* 0*/
  HAL_XPU2_CRYPTO0_BAM,
  HAL_XPU2_DCC,
  HAL_XPU2_IPA_0_GSI_TOP,
  HAL_XPU2_IPA,
  HAL_XPU2_MCCC_APU,
  HAL_XPU2_MMSS,
  HAL_XPU2_MMSS_CC,
  HAL_XPU2_BAM_BLSP1_DMA,
  HAL_XPU2_BAM_BLSP2_DMA,
  HAL_XPU2_SKL,  /* 10 */
  HAL_XPU2_SP_SCSR,
  HAL_XPU2_SSC_BLSP_BAM,
  HAL_XPU2_TLMM_XPU_EAST,
  HAL_XPU2_TLMM_XPU_NORTH,
  HAL_XPU2_TLMM_XPU_WEST,
  HAL_XPU2_UFS_ICE,
  HAL_XPU2_MNOC,
  HAL_XPU2_GCC_RPU,
  HAL_XPU2_TCSR_MUTEX,
  HAL_XPU2_TCSR_REGS, /*20 */
  HAL_XPU2_BIMC_APU,
  HAL_XPU2_MCCC_APU2,
  HAL_XPU2_MPM2,
  HAL_XPU2_IMEM_APU,
  HAL_XPU2_RAMBLUR_PIMEM_APU,
  HAL_XPU2_SEC_CTRL_APU,
  HAL_XPU2_SPDM_APU,
  HAL_XPU2_ANOC1_MPU,
  HAL_XPU2_ANOC2_MPU,
  HAL_XPU2_BOOT_ROM, /* 30 */
  HAL_XPU2_CNOC_A1NOC_MPU,
  HAL_XPU2_CNOC_A2NOC_MPU, 
  HAL_XPU2_CNOC_QDSS_MPU,
  HAL_XPU2_CNOC_SNOC_MPU,
  HAL_XPU2_CNOC_SNOC_MS_MPU,
  HAL_XPU2_LPASS_Q6SS_MPU,
  HAL_XPU2_MSS_MPU,
  HAL_XPU2_MSS_Q6_MPU,
  HAL_XPU2_IMEM_MPU,
  HAL_XPU2_PMIC_ARB,  /* 40 */
  HAL_XPU2_QM_MPU_CFG,
  HAL_XPU2_RAMBLUR_PIMEM_MPU,
  HAL_XPU2_RPM_MSTR_MPU,
  HAL_XPU2_CFG_SSC,
  HAL_XPU2_SSC_Q6_MPU,
  HAL_XPU2_BIMC_MPU0,              /**< BIMC DDR0 */
  HAL_XPU2_BIMC_MPU1,              /**< BIMC DDR1 */
  HAL_XPU2_TLMM, /* This is not supported on 8998 */
  HAL_XPU2_MSS_NAV_MPU,
  HAL_XPU2_LPASS_CFG_MPU, /* 50 */
  HAL_XPU2_TURING_Q6_MPU,
  HAL_XPU2_LPASS_SENSOR_BLSP_BAM,
  HAL_XPU2_TLMM_XPU_CENTER,
  HAL_XPU2_TLMM_XPU_SOUTH,
  HAL_XPU2_SDC1_SDCC_ICE,
  HAL_XPU2_TITAN_SS_APU,
  HAL_XPU2_CNOC_AOSS_MPU, 
  HAL_XPU2_LLCC_BROADCAST_MPU,
  HAL_XPU2_BIMC_BROADCAST_MPU = HAL_XPU2_LLCC_BROADCAST_MPU,
  HAL_XPU2_SSC_SDC_MPU, 
  HAL_DC_NOC_BROADCAST_MPU, /* 60 */
  HAL_DC_NOC_NON_BROADCAST_MPU,
  HAL_AOSS_MPU, 
  HAL_XPU2_BIMC_MPU2, 
  HAL_XPU2_BIMC_MPU3,
  HAL_MEMNOC_MS_MPU,
  HAL_SNOC_IPA_MS_MPU,
  HAL_XPU2_PKA_APU,
  HAL_XPU2_QPIC_APU,
  HAL_XPU2_SPMI_FETHCER,
  HAL_XPU2_Q6_TCM_MPU, /* 70 */
  HAL_XPU2_QPIC_MPU,
  HAL_XPU2_LPASS_SSC_Q6_MPU,
  HAL_XPU2_LPASS_SSC_SDC_MPU,
  HAL_XPU2_LPASS_SSC_BLSP_BAM,
  HAL_XPU2_GEMNOC_MS_MPU,
  HAL_XPU2_BAM_BLSP3_DMA,
  HAL_XPU2_DC_NOC_SHRM_MPU,
  HAL_XPU2_BOOT_IMEM_SS_MPU,
  HAL_XPU2_UFS_G4_ICE,
  HAL_XPU2_DRE_MPU,
  HAL_XPU2_BIMC_MPU4,
  HAL_XPU2_BIMC_MPU5,
  HAL_XPU2_BIMC_MPU6,
  HAL_XPU2_BIMC_MPU7,
  HAL_XPU2_COUNT,
  HAL_XPU2_SIZE     = 0x7FFFFFFF /* force to 32-bit enum */
} HAL_xpu2_XPU2Type;

/** XPU2 Resource ID Type */
typedef uint32 HAL_xpu3_ResourceGroupIDType;

/** XPU2 Resource Group Addresses */
typedef struct
{
  uint32 uStartLower; /**< resource group lower 32 bits of the start address */
  uint32 uStartUpper; /**< resource group upper 32 bits of the start address */
  uint32 uEndLower;   /**< resource group lower 32 bits of the end address */
  uint32 uEndUpper;   /**< resource group upper 32 bits of the end address */
} HAL_xpu3_ResourceGroupAddrType;

/** XPU2 config type */
typedef enum
{
  HAL_XPU2_CONFIG_SECURE     = 0x0, /**< secure configuration request */
  HAL_XPU2_CONFIG_NON_SECURE = 0x1, /**< non-secure configuration request */
  HAL_XPU2_CONFIG_MODEM      = 0x2, /**< modem configuration request */
  HAL_XPU2_CONFIG_SP         = 0x3, /**< Secure Processor configuration request */
  HAL_XPU2_CONFIG_UNOWNED    = 0x4, /**< Unowned configuration request for RACR/WACR config. PLEASE ADD OTHER QAD CFG ON TOP */
  HAL_XPU2_CONFIG_COUNT,
  HAL_XPU2_CONFIG_SIZE       = 0x7FFFFFFF,
} HAL_xpu3_ConfigType;

/** VMID related definitions */
/* Maximum allowed VMID value HAL_XPU3_MAX_VMID is defined in HALxpu2Target.h */
#define HAL_XPU2_MASKS_PER_FIELD 32 /**< Number of mask bits per field */
#define HAL_XPU2_PERMS_PER_FIELD 16 /**< Number of access perms per field */

/** number of required VMID mask dwords */
#define HAL_XPU2_MASTER_MASK_WORDS \
        ((HAL_XPU3_MAX_VMID/HAL_XPU2_MASKS_PER_FIELD) + 1)

/** number of required VMID permission dwords */
#define HAL_XPU2_MASTER_PERM_WORDS \
        ((HAL_XPU3_MAX_VMID/HAL_XPU2_PERMS_PER_FIELD) + 1)

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
  } Vmid_Type;
} HAL_xpu3_ResourceGroupPermissionType;


/** XPU2 Resource Group client access permissions */
typedef struct
{
  boolean bSecureClientROEn : NUMS_QAD+1;
    /**< enable secure client read-only access to secure/MSA owned partition?
     *   (RGn_SCR[SCLROE] valid for V2.4 or later;
     *    RGn_MCR[SCLROE] valid for V2.1 or later) */
  boolean bNonSecureClientROEn : NUMS_QAD+1;
    /**< enable non-secure client read-only access to secure/MSA owned
     *   partition?
     *   (RGn_SCR[VMIDCLROE] valid for V2.2 or later;
     *    RGn_MCR[VMIDCLROE] valid for V2.1 or later) */
  boolean bNonSecureClientWOEn : NUMS_QAD+1;
    /**< enable non-secure client read/write access to secure/MSA owned
     *   partition?
     *   (RGn_SCR[VMIDCLRWE] valid for V2.2 or later;
     *    RGn_MCR[VMIDCLRWE] valid for V2.0 or later) */
  boolean bSecureClientWOEn : NUMS_QAD+1;
    /**< enable secure client read/write access to MSA owned partition?
     *   (RGn_MCR[SCLRWE] valid for V2.0 or later) */
} HAL_xpu3_ResourceGroupClientPermType;

/** Complete XPU2 Resource Group Config*/
typedef struct
{
  HAL_xpu3_ResourceGroupAddrType       resAddr;
    /**< resource group address range */
  HAL_xpu3_ResourceGroupPermissionType resPerm;
    /**< resource group vmid permissions */
  HAL_xpu3_ResourceGroupClientPermType clientPerm;
    /**< resource group client access permission */
  HAL_xpu3_ConfigType                  eConfig; 
    /**< indicate if resource group is configured as secure, non-secure, SP,
     *   or MSA */
} HAL_xpu3_ResourceGroupConfigType;


/** set the default configuration during initialization */
typedef struct
{
  HAL_xpu3_ResourceGroupPermissionType *pUnmapRGPerm;
    /**< pointer to the structure to set up the unmapped partition
     *   permissions */
  boolean  bEnableVmidCheck : 1; /**< enable VMID based protection checking? */
    
  boolean  bEnablexPROTNSCheck : 1;
    /**< enable xPROTNS based protection checking? */
    
  boolean  bSecureConfigWriteDisable : 1;
    /**< TRUE sets secure configuration write disable to prevent accidental
     *   update to all secure configuration registers; FALSE permits secure
     *   configuration access to all registers */
  boolean bMSAClientAccessControl : 1;
    /**< TRUE includes incoming client MSA attribute in determining permission
     *   to non-secure resource.  This is for revision 2.6 or later. */
  boolean bSPClientAccessControl : 1;
  /**< TRUE includes incoming client SP attribute in determining permission
   *   to non-secure resource.  This is for revision 2.6 or later. */
  boolean bUnmapMemoryClientROEn : NUMS_QAD+1;
    /**< TRUE permits read-only accesses by MSA client accesses to unmapped
     *   non-secure regions.  This is for revision 2.6 or later. */
  boolean bUnmapMemoryClientWOEn : NUMS_QAD+1;
    /**< TRUE permits read-write accesses by SP client accesses to unmapped
     *   non-secure regions.  This is for revision 2.6 or later. */
  boolean bUnmapMemoryOwnerAPPS :1;
  boolean bUnmapMemoryOwnerSP :1;
  boolean bUnmapMemoryOwnerMSA:1;
  /**< TRUE sets the owner for unmapped area as APPS and client permission is determined by CR registers */
  boolean bUnmapMemoryOwnerSecure :1;
  /**< TRUE sets the owner for unmapped area as TZ and client permission is determined by CR registers */
  boolean bUnmapMemorySecureROE :1;
  /**< TRUE sets the owner for unmapped area as RO for TZ */
  boolean bUnmapMemorySecureWOE :1;
  /**< TRUE sets the owner for unmapped area as RO for TZ */
} HAL_xpu3_DefaultxPU3ConfigType;

/** Error reporting options */
typedef enum
{
  HAL_XPU2_ERROR_O_CFG_RPT_EN    = 0x1,
    /**< Report config errors to master */
  HAL_XPU2_ERROR_O_CLIENT_RPT_EN = 0x2,
    /**< Report client errors to master */
  HAL_XPU2_ERROR_O_CFG_INT_EN    = 0x4,
    /**< Generate config port interrupt on errors  */
  HAL_XPU2_ERROR_O_CLIENT_INT_EN = 0x8,
    /**< Generate client port interrupt on errors  */
  HAL_XPU2_ERROR_O_SIZE   = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_xpu3_ErrorOptionsType;

/** Store error options ORed in this */
typedef uint32 HAL_xpu3_ErrorOptionsConfigType;

/** Supported xPU2 device types */
typedef enum
{
  HAL_XPU2_DEVICE_RPU            = 0,           /**< Register protection unit */
  HAL_XPU2_DEVICE_APU            = 1,           /**< Area protection unit */
  HAL_XPU2_DEVICE_MPU            = 2,           /**< Memory protection unit */
  HAL_XPU2_DEVICE_COUNT,
  HAL_XPU2_DEVICE_DNE            = 0x7FFFFFFE,  /**< device does not exist */
  HAL_XPU2_DEVICE_SIZE           = 0x7FFFFFFF,  /* force to 32-bit enum */
} HAL_xpu3_XPUDeviceType;

/** XPU2 device parameters */
typedef struct
{
  HAL_xpu3_XPUDeviceType eXpu2Type; /**< XPU2 type */
  uint32  uResourceGroupCount; /**< Total number of Resource Groups */
  uint32  uNumVMID;            /**< number of VMID supported */
  uint32  uResourceGroupRes;   /**< Minimal RG size (for MPUs) */
  uint64  uResourceGroupResMax;/**< Maximum RG size (for MPUs) */
  boolean bMultiVMID : 1;      /**< Multi-VMID type access control? */
  boolean bFullPerm : 1;       /**< Full permissions (RW/WO/RO/NO)? */
  boolean bQADSupport : NUMS_QAD+1;     /**< QAD support */
  boolean bVMID_ACR : 1;       /**< VMID permissions? */
  const char *szXpu2Name;      /**< pointer to XPU2 string name */
} HAL_xpu3_XPUDeviceParamsType;

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
} HAL_xpu3_ErrorFlagsType;

/** Store error flags ORed in this */
typedef uint32 HAL_xpu3_ErrorFlagsConfigType;

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
  HAL_XPU2_BUS_F_SECURE_RG_MATCH = 0x40000, /**< Secure resource match error */
  HAL_XPU2_BUS_F_NONSECURE_RG_MATCH = 0x80000, /**< Non Secure Apps resource error */
  HAL_XPU2_BUS_F_SP_RG_MATCH = 0x100000, /**< SP resource match error */
  HAL_XPU2_BUS_F_SP           = 0x200000, /**< SP field of the transaction */
  HAL_XPU2_BUS_FLAGS_SIZE     = 0x7FFFFFFF /* force to 32-bit enum*/
} HAL_xpu3_BusErrorFlagsType;

/** Store bus flags ORed in this */
typedef uint32 HAL_xpu3_BusErrorFlagsMaskType;

/** Full xPU2 error reporting structure */
typedef struct
{
  HAL_xpu3_ErrorFlagsConfigType uErrorFlags; /**< Error Flags */
  HAL_xpu3_BusErrorFlagsMaskType uBusFlags;  /**< Bus specific flags */
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
} HAL_xpu3_ErrorType;

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** @name Mandatory Interface */
/** @{ */

/** 
@brief Initialization function.

  This function initializes the unmapped partition permissions, the access
  mask table permissions, the xPROTNS and/or VMID based protection checking, 
  and the access to the secure write registers.

@param[in] eXpu2 - The xpu2 to initalize
@param[in] pDefaultConfig - A pointer to the structure for the initialization
                            settings
@param[out] ppszVersion - Pointer to version string.  Currently not populated.

@return The error status of the xPU2.
*/
HAL_xpu3_Status HAL_xpu3_Init
  ( HAL_xpu2_XPU2Type eXpu2,
    const HAL_xpu3_DefaultxPU3ConfigType *pDefaultConfig,
    char ** ppszVersion );

/**
@brief Restore the secure configuration of the selected XPU2 hardware.

@param[in] eXpu2 - The XPU2 hardware to reset.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu3_Status HAL_xpu3_RestoreSecureConfig ( HAL_xpu2_XPU2Type eXpu2 );

/**
@brief Reset the selected XPU2 hardware to a default state.

@param[in] eXpu2 - The XPU2 hardware to reset.

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu3_Status HAL_xpu3_Reset   ( HAL_xpu2_XPU2Type eXpu2 ) ;

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
HAL_xpu3_Status HAL_xpu3_ConfigAccessPermissions
  ( HAL_xpu2_XPU2Type                           eXpu2,
    const HAL_xpu3_ResourceGroupPermissionType *pAccessConfig );


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
HAL_xpu3_Status HAL_xpu3_ConfigUnmappedPartitionPermissions
  ( HAL_xpu2_XPU2Type                           eXpu2,
    const HAL_xpu3_ResourceGroupPermissionType *pUnmapRGPerm );

/**
@brief Enable APSS-attribute based protection checking

   This function enables the APSS-attribute  (SP) based protection checking.

@param[in] eXpu2 - The XPU3 to enable.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/

void HAL_xpu3_EnableAPPSAuth( HAL_xpu2_XPU2Type eXpu2 );

/**
@brief Enable xMssSelfAuth based protection checking

   This function enables the xMssSelfAuth (MSA) based protection checking.

@param[in] eXpu2 - The XPU3 to disable.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
void HAL_xpu3_EnableMSSSelfAuth( HAL_xpu2_XPU2Type eXpu2 );

/**
@brief Enable SP-attribute based protection checking

   This function enables the SP-attribute  (SP) based protection checking.

@param[in] eXpu2 - The XPU3 to enable.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/

void HAL_xpu3_EnableSPAuth( HAL_xpu2_XPU2Type eXpu2 );


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
HAL_xpu3_Status HAL_xpu3_EnableMPUPartition
  ( HAL_xpu2_XPU2Type eXpu2,
    HAL_xpu3_ResourceGroupIDType uResID );

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
HAL_xpu3_Status HAL_xpu3_DisableMPUPartition
  ( HAL_xpu2_XPU2Type eXpu2,
    HAL_xpu3_ResourceGroupIDType uResID );

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
HAL_xpu3_Status HAL_xpu3_ConfigResourceGroup
  ( HAL_xpu2_XPU2Type                       eXpu2,
    HAL_xpu3_ResourceGroupIDType            uResID,
    const HAL_xpu3_ResourceGroupConfigType *pResConfig );


/**
@brief Same as HAL_xpu3_ConfigResourceGroup except without any lock acquire calls 
*/
HAL_xpu3_Status HAL_xpu3_ConfigResourceGroup_NoLock
  ( HAL_xpu2_XPU2Type                       eXpu2,
    HAL_xpu3_ResourceGroupIDType            uResID,
    const HAL_xpu3_ResourceGroupConfigType *pResConfig );
	
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
HAL_xpu3_Status HAL_xpu3_ConfigErrorReporting
  ( HAL_xpu2_XPU2Type               eXpu2,
    HAL_xpu3_ConfigType             eConfig,
    HAL_xpu3_ErrorOptionsConfigType errOpt );
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
void HAL_xpu3_GetDeviceParameters( HAL_xpu2_XPU2Type             eXpu2,
                                   HAL_xpu3_XPUDeviceParamsType *pDevParams );

/**
@brief Read the current access permissions for the xPU2's core registers.


@param[in] eXpu2 - The XPU2 to obtain extended configuration information from
@param[out] pAccessConfig - A pointer to the structure to use to return the
                            XPU2 access permission configuration.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
@sa HAL_xpu2_ConfigAccessPermissions()
*/
void HAL_xpu3_GetAccessPermissions
  ( HAL_xpu2_XPU2Type                     eXpu2,
    HAL_xpu3_ResourceGroupPermissionType *pAccessConfig );

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
void HAL_xpu3_GetUnmappedPartitionPermissions
  ( HAL_xpu2_XPU2Type                     eXpu2,
    HAL_xpu3_ResourceGroupPermissionType *pUnmapRGPerm );

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
void HAL_xpu3_GetResourceGroupConfig
  ( HAL_xpu2_XPU2Type                     eXpu2,
    HAL_xpu3_ResourceGroupIDType          uResID,
    HAL_xpu3_ResourceGroupConfigType     *pResConfig );

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
void HAL_xpu3_GetErrorReportingConfig
  ( HAL_xpu2_XPU2Type                eXpu2,
    HAL_xpu3_ConfigType              eConfig,
    HAL_xpu3_ErrorOptionsConfigType *pErrConfig );

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
bool32 HAL_xpu3_IsError          ( HAL_xpu2_XPU2Type   eXpu2,
                                   HAL_xpu3_ConfigType eConfig );

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
void HAL_xpu3_GetError( HAL_xpu2_XPU2Type                     eXpu2,
                        HAL_xpu3_ConfigType                   eConfig,
                        HAL_xpu3_ErrorType                   *pError );

/**
@brief Clear the xPU2 device error status.

@param[in] eXpu2 - The XPU2 to clear errors on
@param[in] eConfig - which config type to clear errors on

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu3_Status HAL_xpu3_ClearError( HAL_xpu2_XPU2Type    eXpu2,
                                     HAL_xpu3_ConfigType  eConfig );

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
HAL_xpu3_Status HAL_xpu3_ClearErrorExt( HAL_xpu2_XPU2Type    eXpu2,
                                        HAL_xpu3_ConfigType  eConfig,
                                        HAL_xpu3_ErrorFlagsConfigType uErrorFlags );



/**
@brief Get the partitons owned by this domain

@param[in] eXpu2 - The XPU2 to clear the error bit(s)
@param[in] puResIDMaskPtr - Mask of the owningn partitions
@param[in] size           - size of the puResIDMaskPtr array passed
@param[in] pNumOwnership  - Number of partitions owned by this domain

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu3_Status HAL_xpu3_GetResourceGroupOwnerInfo
( 
  HAL_xpu2_XPU2Type                 eXpu2,
  uint32                           *puResIDMaskPtr,
  uint32                           size,
  uint32                           *pNumOwnership
);

/**
@brief Get the free partitions in MPU

@param[in] eXpu2 - The XPU2 ID
@param[in] puResIDMaskPtr - Mask of the owningn partitions
@param[in] size           - size of the puResIDMaskPtr array passed

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/

HAL_xpu3_Status HAL_xpu3_GetFreeResourceGroupInfo
( 
  HAL_xpu2_XPU2Type                 eXpu2,
  uint32                            *pResIDMaskPtr,
  uint32                            size
);



/** @} */


/**
@brief Check for an xPU2 error condition. Do not take the mutex lock

   This function checks for the xPU2 error condition per the config type.

@param[in] eXpu2 - The XPU2 to obtain error information from
@param[in] eConfig - which config type to obtain the error info

@return The error status of the xPU2.

@retval TRUE  The xPU2 has recorded an error.
@retval FALSE The xPU2 has no recorded errors.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
bool32 HAL_xpu3_IsError_NoLock( HAL_xpu2_XPU2Type   eXpu2,
                                HAL_xpu3_ConfigType eConfig );

/**
@brief Clear the xPU2 device error status. Do not take the mutex lock

@param[in] eXpu2 - The XPU2 to clear errors on
@param[in] eConfig - which config type to clear errors on

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu3_Status HAL_xpu3_ClearError_NoLock( HAL_xpu2_XPU2Type    eXpu2,
                                            HAL_xpu3_ConfigType  eConfig );


/**
@brief Get the pending xPU2 error per config type. Do not take the mutex lock

   This includes: the error type (access control or decode) and the relevant
   bus parameters for the errant request.

@param[in]  eXpu2 - The XPU2 to obtain error information from
@param[in]  eConfig - which config type to obtain the error info
@param[out] pError - A pointer to the structure to use to return the xPU2 error

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
void HAL_xpu3_GetError_NoLock( HAL_xpu2_XPU2Type                     eXpu2,
                               HAL_xpu3_ConfigType                   eConfig,
                               HAL_xpu3_ErrorType                   *pError );

/**
@brief Get xPU2 hardware device parameters. Do not take the mutex lock

   This function reads the xPU2 device operating parameters from the hardware.
   This includes device type, device variant, and resource group information.

@param[in]  eXpu2 - The XPU2 to get parameters for
@param[out] pDevParams - A pointer to the structure to store the device
                         parameters

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
void HAL_xpu3_GetDeviceParameters_NoLock( HAL_xpu2_XPU2Type             eXpu2,
                                          HAL_xpu3_XPUDeviceParamsType *pDevParams );

/**
@brief Read the current access configuration for a specific resource group.
	Do not take the mutex lock

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
void HAL_xpu3_GetResourceGroupConfig_NoLock
  ( HAL_xpu2_XPU2Type                     eXpu2,
    HAL_xpu3_ResourceGroupIDType          uResID,
    HAL_xpu3_ResourceGroupConfigType     *pResConfig );


/**
@brief Get the partitons owned by this domain

@param[in] eXpu2 - The XPU2 to clear the error bit(s)
@param[in] puResIDMaskPtr - Mask of the owningn partitions
@param[in] size           - size of the puResIDMaskPtr array passed
@param[in] pNumOwnership - Number of partitions owned by this domain

@return The error status of the xPU2.

@pre HAL_xpu2_Init() must have been called.

@sa HAL_xpu2_Init()
*/
HAL_xpu3_Status HAL_xpu3_GetResourceGroupOwnerInfo_NoLock
( 
  HAL_xpu2_XPU2Type                 eXpu2,
  uint32                           *puResIDMaskPtr,
  uint32                            size,
  uint32                           *pNumOwnership
);

/**
@brief Clears the RG owner of the particular RG in given XPU.
       Used by devcfg_noac configuration to clear XBL_SEC configured XPU
	   
@param[in]  eXpu2 enum of the XPU to be acted upon
@param[in] resid - RG of the XPU to be cleared
@param[out] Status - Success or failure of the configuration

@see HAL_xpu3_ClearRG_Owner()
*/
HAL_xpu3_Status  HAL_xpu3_ClearRG_Owner(HAL_xpu2_XPU2Type eXpu2, uint32 resid);

/**
@brief Sets the XPU RG's start and end address to 0xFFFF FFFF(meaning no address range protection)
@param[in]  eXpu2 enum of the XPU to be acted uponfiguration

@see HAL_xpu3_ResetRG_addr()
*/
HAL_xpu3_Status HAL_xpu3_ResetRG_addr(HAL_xpu2_XPU2Type eXpu2);


HAL_xpu3_Status HAL_xpu3_ConfigUnmapPartition
(
  HAL_xpu2_XPU2Type                 eXpu2,
  const HAL_xpu3_DefaultxPU3ConfigType  *pDefaultConfig
);

/**
@brief Find the target specific XPU module information 

@param[in] eXpu - the xPU2 to search for

@returns Internal error status.
*/

HAL_xpu3_Status HAL_xpu3_GetLSB(HAL_xpu2_XPU2Type eXpu, uint32 *uLSB);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef HALXPU2_H */

