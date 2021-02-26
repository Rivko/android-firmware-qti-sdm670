#ifndef ACCESS_CONTROL_TZ_H
#define ACCESS_CONTROL_TZ_H

/*===========================================================================
Copyright (c) 2010-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssg.tz/1.0.2/securemsm/accesscontrol/api/AccessControlTz.h#1 $
$DateTime: 2018/02/06 03:00:17 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/10   tk      First version.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "tz_syscall_pub.h"
#include "tzbsp_syscall.h"
#include "ACHALxpu.h"
#include "ACCommon.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define MEMORY_OWNERSHIP_TABLE_BUFFER_SIZE             0x400000 //2bytes for each index so it has double size of MEMORY_OWNERSHIP_TABLE_BUFFER_MAX_IDX
#define MEMORY_OWNERSHIP_TABLE_BUFFER_MAX_IDX          0x200000 
#define PARITY_BUFFER_SIZE                             0x40000
#define PARITY_BUFFER_MAX_IDX                          0x40000 
#define MAGIC_COOKIE                                   0x41473ACD

/* Flag definitions up until 8996(xPU v2) */

#define TZBSP_XPU_DISABLE         0x00 /**< Disables an XPU. */
#define TZBSP_XPU_ENABLE          0x01 /**< Enables an XPU. */
#define TZBSP_XPU_NO_INTERRUPTS   0x02 /**< Disables XPU error interrupts. */
#define TZBSP_XPU_MODEM_PROT      0x04 /**< Marks modem only items. */
#define TZBSP_XPU_SEC             0x08 /**< Secure partition */
#define TZBSP_XPU_NON_SEC         0x10 /**< Non Secure partition */
#define TZBSP_XPU_UMR_MSACLROE    (1 << 5) /**< MSA unmapped read-access  */
#define TZBSP_XPU_UMR_MSACLRWE    (1 << 6) /**< MSA unmapped read/write-access  */
#define TZBSP_XPU_CR_MSAE         (1 << 7) /**< Check MSA bit for non-secure registers */
#define TZBSP_RWGE                0x0100 /**< Read/write global enable. */
#define TZBSP_ROGE                0x0200 /**< Read-only global enable. */
#define TZBSP_RWE                 0x0400 /**< Read/write VMID enable. */
#define TZBSP_ROE                 0x0800 /**< Read-only VMID enable. */
#define TZBSP_VMIDCLROE           0x1000 /**< VMID based Read-Only enable for Secure/MSA resource. */
#define TZBSP_VMIDCLRWE           0x2000 /**< VMID based Read-Write enable for Secure/MSA resource. */
#define TZBSP_MSACLROE            0x4000 /**< MSA Read-only enable for secure partitions. */
#define TZBSP_MSACLRWE            0x8000 /**< MSA Read-Write enable for secure partitions. */
#define TZBSP_SCLROE              0x10000 /**< XPU.SCR based secure client Read-Only enable. */
#define TZBSP_SCLRWE              0x20000 /**< XPU.SCR based secure client Read-Write enable. */

/* Flag definitions for 8998 onwards (xPU v3) */
#define TZBSP_XPU_ENABLE          0x01 /**< Enables an XPU. */
#define TZBSP_XPU_NO_INTERRUPTS   0x02 /**< Disables XPU error interrupts. */
#define TZBSP_XPU_MSA_OWNER       0x04 /**< Marks modem only items. */
#define TZBSP_XPU_TZ_OWNER        0x08 /**< Secure partition */
#define TZBSP_XPU_HYP_OWNER       0x10 /**< Non Secure partition */
#define TZBSP_XPU_SP_OWNER        0x20 /**< Secure Processor partition */
#define TZBSP_XPU_NO_OWNER        0x40 /**< Unowned partition */


#define TZBSP_XPU_READ_ACCESS     1 /**< Read only access */
#define TZBSP_XPU_RW_ACCESS       2 /**< Read/Write access */
#define TZBSP_XPU_WRITE_ACCESS    3 /**< Write only access */

/* EBI channel interleaving related constants. */
#define TZBSP_EBI_CH_NONE             0x0
#define TZBSP_EBI_CH_0                0x1
#define TZBSP_EBI_CH_1                0x2
#define TZBSP_EBI_CH_BOTH             0x3 /* Interleaved. */
#define TZBSP_EBI_CH_QUAD             0x4 /* Interleaved - 4 channels */
#define TZBSP_EBI_CH_OCTA             0x5 /* Interleaved - 8 channels */

#define TZBSP_BLIST_DEVICE_MEMORY       0       /* Non-DDR memory, set at boot */
#define TZBSP_BLIST_DEVICE_MEMORY1      1       /* Device Memory, set at boot. */
#define TZBSP_BLIST_UNUSED              2       /* Unused */
#define TZBSP_BLIST_SECCHANNEL_MSS      3       /* MSS Secure Channel */
#define TZBSP_BLIST_SECCHANNEL_LPASS    4       /* LPASS Secure Channel */
#define TZBSP_BLIST_TZ_STATIC_MEMORY    5       /* Static Memory . */
#define TZBSP_BLIST_ALLOC_0             6       /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_1             7       /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_2             8       /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_3             9       /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_4             10      /* Dynamically allocated. */
#define TZBSP_BLIST_ALLOC_5             11      /* Dynamically allocated. */
#define TZBSP_BLIST_FIXED_SEC_DDR       12      /* Fixed secure DDR, static */
#define TZBSP_BLIST_SHARED_IMEM         13      /* Shared imem, static. */
#define TZBSP_BLIST_TZ_DDR              14      /* Statically allocated. */
#define TZBSP_BLIST_PIL_ALLOC_0			15		/* Dynamically allocated. */
#define TZBSP_BLIST_PIL_ALLOC_1			16		/* Dynamically allocated. */
#define TZBSP_BLIST_PIL_ALLOC_2			17		/* Dynamically allocated. */
#define TZBSP_BLIST_PIL_ALLOC_3			18		/* Dynamically allocated. */	
#define TZBSP_BLIST_PIL_ALLOC_4         19      /* Dynamically allocated. */
#define TZBSP_BLIST_PIL_ALLOC_5         20      /* Dynamically allocated. */
#define TZBSP_BLIST_HLOS_CODE			21      /*Dynamically Locked*/
#define TZBSP_BLIST_DYN_AREA_CERT       22      /* Dynamic area for PIL certificate. */


/* Just adding temporarily so compilation works: REMOVEME*/
#define TZBSP_BLIST_LPASS   TZBSP_BLIST_PIL_ALLOC_0
#define TZBSP_BLIST_VIDEO   TZBSP_BLIST_PIL_ALLOC_1
#define TZBSP_BLIST_WLAN    TZBSP_BLIST_PIL_ALLOC_2
#define TZBSP_BLIST_VPU     TZBSP_BLIST_PIL_ALLOC_3
#define TZBSP_BLIST_GPU     TZBSP_BLIST_PIL_ALLOC_4
#define TZBSP_BLIST_MSS     TZBSP_BLIST_PIL_ALLOC_5
#define TZBSP_BLIST_LPASS_DY_HEAP TZBSP_BLIST_ALLOC_5

#define TZBSP_UNMAPPED_PARTITION -1
#define TZBSP_XPU_DYN_AREA_CERT   1 /**< Dynamic area ID for PIL certificate. */
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

struct _ACMemoryVmInfo
{
    uint64 uStart;
    uint32 uSize;
    uint64 uReadVmMask;
    uint64 uWriteVmMask;
    uint32 uTzPerm;
};

typedef struct
{
    uint64 start;
    uint64 end;
    uint32 uRGNum;
}ACPartition;

typedef struct
{
    ACHAL_xpu_Type eXpuId;
    ACHAL_xpu_DeviceType eXpuIdType;
    uint64 uRangeStart;
    uint64 uRangeEnd;
    ACPartition *partition_info;
    uint32 uNumPartitions;
}ACXpuInfo;

typedef struct 
{
    ACVirtualMachineId eDestinationVm; 
    uint32 uPermissions; 
}ACDestinationVmType;

typedef struct
{
    uint32 uReadVmids;
    uint32 uWriteVmids;
    uint32 uDomain;
    uint32 uFlags;
}ACXpuPermType;

typedef struct _ACMemoryVmInfo ACMemoryVmInfo;

struct _ACPagesInfo
{
    uint32 uNumExclusivePages;
    uint32 uNumSharedPages;
};

typedef struct _ACPagesInfo ACPagesInfo;

/**
 * Resource group configuration for MPUs. All MPUs are multi-VMID.
 */
typedef struct tzbsp_mpu_rg_s
{
  uint16 index;      /* Index of the MPU resource group. */
  /** XPU status bits, currently \c TZBSP_XPU_ENABLE, \c
   * TZBSP_XPU_NO_INTERRUPTS, \c TZBSP_XPU_SEC, \c TZBSP_XPU_NON_SEC and
   * \c TZBSP_XPU_MODEM_PROT are supported.
   */
  uint32 flags;
  uint32 read_vmid;  /* VMIDs able to read this partition. */
  uint32 write_vmid; /* VMIDs able to write this partition. */
  uint64 start;  /* Start of the partition. */
  uint64 end;    /* End of the partition, not included in the range */
} tzbsp_mpu_rg_t;

/**
 * Resource group configuration for APUs/RPUs. Depending on APU/RPU, a resource
 * group is single VMID or multi VMID.
 */
typedef struct tzbsp_rpu_rg_s
{
  /** Index of the APU/RPU resource group. */
  uint16 index;
  /**
   * Valid for single VMID resource groups only. Bits \c TZBSP_RWGE and \c
   * TZBSP_ROGE allow defining read/write global enable and read-only global
   * enable. Bit \c TZBSP_RWE enables the read/write access VMID(s). Bit \c
   * TZBSP_ROE enables the read access VMID(s).
   */
  uint32 flags;
  /**
   * For multi VMID resource groups contains a bitmap of VMIDs that can read
   * the resource. For single VMID resource groups contains a VMID value that
   * can read the resource.
   */
  uint32 read_vmid;
  /**
   * For multi VMID resource groups contains a bitmap of VMIDs that can
   * read/write the resource. For single VMID resource groups contains a VMID
   * value that can read/write the resource.
   */
  uint32 write_vmid;
} tzbsp_rpu_rg_t;

/**
 * Root level structure for XPU configuration.
 */
typedef struct
{
  /** Physical base address of the XPU. */
  uintptr_t phys_addr;
  /** Index of the XPU, @see \c HAL_xpu_XPUType. */
  uint16 id;
  /** XPU status bits, currently \c TZBSP_XPU_ENABLE, \c
   * TZBSP_XPU_NO_INTERRUPTS and \c TZBSP_XPU_MODEM_PROT are supported.
   */
  uint16 flags;
  /** Read access VMIDs for unmapped area */
  uint32 unmapped_rvmid;
  /** Write access VMIDs for unmapped area */
  uint32 unmapped_wvmid;
  /** Superusers */
  uint32 superuser_vmid;
  /** Number of configured resource groups. */
  uint16 nrg;
  union
  {
    const void* any;           /* A dummy to keep compiler happy. */
    const tzbsp_rpu_rg_t* rpu; /* Can be NULL. */
    const tzbsp_mpu_rg_t* mpu; /* Can be NULL. */
  } rg;
} tzbsp_xpu_cfg_t;


typedef struct {
  uint64  start;        // Partition start address
  uint64  end;          // Partition end address
  uint16 index;
} xpu_dyn_locked_rg;
/**
 * Root level structure for QRIB configuration.
 */
typedef struct
{
  uint64 xpu2_qrib_init_addr; /* *_QRIB_XPU2_INIT register address */
  uint64 xpu2_qrib_acr_addr;  /* *_QRIB_XPU2_ACR register address */
  uint64 xpu2_qrib_vmid_en_init_addr; /* *_QRIB_VMIDEN_INIT register address */
  uint8 nsen_init; /* Value of *_XPU2_NSEN_INIT field in *_QRIB_XPU2_INIT register */ 
  uint8 en_tz;  /* Value of *_XPU2_EN_TZ field in *_QRIB_XPU2_INIT register */
  uint32 xpu_acr_vmid; /* Value of *_XPU2_ACR field in *_QRIB_XPU2_ACR register */
  uint8 vmiden_init;   /* Value of *_XPU2_VMIDEN_INIT field in *_QRIB_VMIDEN_INIT register */
  uint32 vmiden_init_en_hv;   /* Value of *_XPU2_VMIDEN_INIT_EN_HV field in *_QRIB_VMIDEN_INIT register */ 
}tzbsp_qrib_cfg_t;

typedef enum
{
  XPU_DISABLE_NONE,
  XPU_DISABLE_NON_MSS,
  XPU_DISABLE_ALL
} xpu_level_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
@brief ACInit - Initializes AC static configs 
@return AC_SUCCESS on success, failure values on Failure
*/
void ACTzInit(void);

#if 0
/**
 * Converts 16-bit VMID read/write bitmasks to 32-bit access bitmask for XPU
 * HAL API, which uses 2-bits per VMID for read/write access bits.
 *
 * @param read_vmids  VMID bitmap for read access.
 *
 * @param write_vmids VMID bitmap for write access.
 *
 * @return The expanded access bitmap, 2-bits per VMID.
 */
uint32 tzbsp_vmids_to_perm(uint16 read_vmids, uint16 write_vmids);
#endif

/**
 * Locks an EBI1 memory region and enables it as a secure area in the
 * blacklist.
 *
 * @param [in] id  The area to be locked. Must be one of the blacklist
 *                 item IDs. Currently \c TZBSP_BLIST_MSS and
 *                 \c TZBSP_BLIST_LPASS are supported.
 * @param [in] start  Start address of the region to be locked.
 * @param [in] end    End address of the region to be locked.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_xpu_lock_area(uint32 id, uintnt start, uintnt end,
                               uint32 rvmid, uint32 wvmid);

/**
 * Unlocks an EBI1 memory region and disables it as a secure area in the
 * blacklist.
 *
 * @param [in] id  The area to be unlocked. Must be one of the blacklist
 *                 item IDs. Currently \c TZBSP_BLIST_MSS and
 *                 \c TZBSP_BLIST_LPASS are supported.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_xpu_unlock_area(uint32 id);

/**
 * Dynamically reconfigures an RPU/APU resource group VMID assignment. If the
 * configuration is for secure partition, then setting \c TZBSP_ROGE in the RPU
 * flags will enable \c VMIDCLROE and setting \c TZBSP_RWGE in the RPU flags
 * will enable \c VMIDCLRWE.
 *
 * @param [in] xpu_id   HAL ID of the RPU/APU.
 * @param [in] rpu      The RPU to reconfigure
 * @param [in] sec      If \c TRUE, then partition is configured as secure
 *                      partition. If \c FALSE, then partition is configured
 *                      as VMID based partition.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_rpu_reconfigure(uint32 xpu_id, const tzbsp_rpu_rg_t *rpu,
                          boolean sec);

/**
 * Dynamically reconfigures an RPU/APU resource group VMID and domain assignment. 
 * This api is an extension to tzbsp_rpu_reconfigure() api. This API can be used
 * to assign the domain TZBSP_XPU_MODEM_PROT  or  TZBSP_XPU_SEC or TZBSP_XPU_NON_SEC
 * to a particular resource group or partition. 
 * Note : Domain cannot be changed by TZ if it is already TZBSP_XPU_MODEM_PROT.
 *
 * @param [in] xpu_id   HAL ID of the RPU/APU.
 * @param [in] rpu      The RPU to reconfigure
 * @param [in] domain   TZBSP_XPU_MODEM_PROT  or  TZBSP_XPU_SEC or TZBSP_XPU_NON_SEC
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_rpu_reconfigure_ext(uint32 xpu_id, const tzbsp_rpu_rg_t *rpu,
                              uint32 domain);

/**
 * Syscall API to enable/disable xpu violations becoming error fatals
 * This can also be used to check if xpu violations are currently error fatals
 * or not by sending \c TZ_XPU_VIOLATION_ERR_FATAL_NOOP as the parameter
 * TZ may choose to force enable/disable xpu violations as error fatals by
 * default. It may also choose to disallow HLOS from overriding this
 * configuration. So, HLOS must always check the output parameter (enabled)
 * to see the current behavior and the return code to see if it was allowed to
 * carry out the operation (-E_NOT_SUPPORTED is returned if HLOS isn't allowed
 * to change the behavior).
 *
 * @param [in] config     Enable/Disable/Noop
 * @param [in] spare      Unused
 *
 * @param[out] status     Enable/Disable
 *
 * @param [in] buf_size_sz   Size of the response buffer.
 *
 * @return  Zero on success.
 *          Negative error code otherwise.
 */
int tzbsp_xpu_config_violation_err_fatal(
                               tz_xpu_violation_err_fatal_t  config,
                               uint32                        spare,
                               tzbsp_smc_rsp_t               *rsp );
/**
 * API to configure a MPU partition 
 *
 * @param [in] xpu_id   HAL ID of the MPU.
 * @param [in] mpu      The MPU to reconfigure
 *
 * @return  Zero on success.
 *          Negative error code otherwise.
 */
int tzbsp_xpu_mpu_reconfigure(uint32 xpu_id, const tzbsp_mpu_rg_t *mpu); 


/**
 * API to read if xPU violation are treated as fatal or not 
 *
 * @return  tz_xpu_violation_err_fatal_t status
 *          
 */
tz_xpu_violation_err_fatal_t tzbsp_xpu_get_violation_err_fatal_status(void);

/**
 * API to disable a particular XPU
 *
 * @param [in] address  Base address of the XPU to disable
 *
 * @return  Zero on success.
 *          Negative error code otherwise.
 */
int tzbsp_xpu_disable(uintptr_t xpu_base_address);

/**
 * Restore full VMIDMT configuration for all the VMIDMT instances
 * corresponding to a device.
 */
int ACVmidmtRestoreDevice(tz_device_e_type device);

int ACXpuRestoreConfig(tz_device_e_type device);

/**
  @brief ACPrintXpuLogs - Print XPU logs without error fatal 
  @return AC_SUCCESS on success, failure values on Failure
  */
uint32 ACPrintXpuLogs(void);

/**
  @brief ACLogMutexLock - Grabs the lock 
  @return AC_SUCCESS on success, failure values on Failure
  */
uint32 ACLogMutexLock(void);

/**
  @brief ACLogMutexUnLock - Releases the lock 
  @return AC_SUCCESS on success, failure values on Failure
  */
uint32 ACLogMutexUnLock(void);


/**
  @brief ACInuseRWMutexLock - Grabs the Lock. Lock acquired before reading/writing the ACDataBase InUse Bi.
  @return AC_SUCCESS on success, failure values on Failure
  */
uint32 ACInUseRWMutexLock(void);

/**
  @brief ACInuseRWMutexUnLock - Releases the lock. Lock Released after reading/writing the ACDataBase InUse Bit.
  @return AC_SUCCESS on success, failure values on Failure
  */
uint32 ACInUseRWMutexUnLock(void);

uint32 ACValidations(const hyp_memprot_ipa_info_t                 *IPAinfo,
        uint32                                       IPAinfolistsize,
        const uint32                                 *sourceVMlist,
        uint32                                       srcVMlistsize,
        const hyp_memprot_dstVM_perm_info_t          *destVMlist,
        uint32                                       dstVMlistsize);

uint32 ACValidateParams(uint64 uStart, uint64 uSize, uint32 eVm);

int tzbsp_mpu_lock_area(uint32 mpu, uint32 index, uint64 start,
                        uint64 end, uint32 rvmids, uint32 wvmids);

int tzbsp_mpu_unlock_area(uint32 mpu, uint32 index);

/* Dynamically unlocks an rpu resource group. After unlocking the
 * previously protected memory area is accessible by all masters (unless other
 * areas overlap with the unlocked area).
 *
 * @param [in] mpu    RPU HAL index.
 * @param [in] index  Resource group index.
 *
 * @return Zero on success, otherwise an error code.
 */
int tzbsp_rpu_unlock_area(uint32 rpu_id, uint32 index);


int tzbsp_unlock_all_xpus();


/**
 * @brief ACTZAssign - Maintains the bookkeeping in TZ for HLOS owned memory
 * and maps/unmaps xPU partitions
 * @param [in] IPAinfo              The mappings information
 * @param [in] IPAinfolistsize      Size of it mappings
 * @param [in]  sourceVMlist    Source VM list
 * @param [in] srcVMlistsize        Size of the source VM list
 * @param [in] destVMlist           Destination List
 * @param [in] dstVMlistsize        Destination List size
 * @return AC_SUCCESS on success, failure values on Failure
 */
AC_ERROR_CODE ACTZAssign(const hyp_memprot_ipa_info_t                 *IPAinfo_tz,
        uint32                                       IPAinfolistsize,
        const uint32                                 *sourceVMlist,
        uint32                                       srcVMlistsize,
        const hyp_memprot_dstVM_perm_info_t          *destVMlist,
        uint32                                       dstVMlistsize);

/**
 * @brief ACGetMemoryVmPerm - Gets the permission for the particular mapping & VM
 * @param [in] uStart              Start address
 * @param [in] uSize               Size 
 * @param [in] uVm                 VM whose permission is to be found
 * @param [out] pRetVal            Returns the permission here
 * @return AC_ERROR_CODE
 */
uint32 ACGetMemoryVmPerm(uint64 uStart, uint64 uSize, uint32 uVm, uint32 *pRetVal);


/**
 * @brief ACMarkMemoryInUse - Mark a particular region in use or not in use by TZ
 * @param [in] uStart              Start address
 * @param [in] uSize               Size (start/size has to be aligned to 4K)
 * @param [in] uInUse              Boolean to set in use or not in use
 * @return AC_SUCCESS on success, failure values on Failure
 */
uint32 ACMarkMemoryInUse(uint64 uStart, uint64 uSize, bool32 uInUse);

/**
 * @brief ACGetMemoryPerm - Gets a list of information for the memory range passed
 * @param [in] uStart              Start address
 * @param [in] uSize               Size 
 * @param [in] pVMInfo             pointer to struct ACMemoryVmInfo, where the results will be stored (Initialized to 0)
 * @param [in] uVMInfoSize         size of the array passed
 * @return Error code, if the size of the struct is not enough to populate the information
 */
uint32 ACGetMemoryPerm(uint64 uStart, uint64 uSize, ACMemoryVmInfo *pVMInfo, uint32 uVMInfoSize);


/**
 * @brief ACGetNumPagesOwnedByVM - Returns the number of pages owned by the VM
 * @param [in] uVm                 VM
 * @param [in] pPagesInfo          pointer to struct ACPagesInfo, where the results will be stored
 * @return AC error codes
 */
uint32 ACGetNumPagesOwnedByVM(uint32 uVm, ACPagesInfo *pPagesInfo);

/**
 * @brief ACIsMemoryOwnedByVMs - Used to check if VMs own the region or not, also can check for exclusivity
 * @param [in] uStart              Start address
 * @param [in] uSize               Size 
 * @param [in] uVmMask             VM Mask
 * @param [in] bExclusive          Checks if memory is exclusively owned by VMs specified in uVmMask
 * @param [out] pRetVal            TRUE or FALSE depending on the result
 * @return AC_ERROR_CODE
 */
uint32 ACIsMemoryOwnedByVMs(uint64 uStart,
        uint64 uSize,
        uint64 uVmMask,
        boolean bExclusive,
        boolean *pRetVal);

/**
 * @brief ACIsMemoryOwnedByAnyVMs - Used to check if any specified VMs own the region or not, also can check for exclusivity ownership
 * @param [in] uStart              Start address
 * @param [in] uSize               Size 
 * @param [in] uVmMask             VM Mask (1<<VM1) | (1<<VM2) 
 * @param [in] bExclusive          Check for exclusive ownership flag
 * @param [out] pRetVal            TRUE or FALSE depending on the result
 * @return AC_ERROR_CODE
 */
uint32 ACIsMemoryOwnedByAnyVMs(uint64 uStart,
        uint64 uSize,
        uint64 uVmMask,
        boolean bExclusive,
        boolean *pRetVal);

/**
 * @brief ACTzClearVmMemory - Clear all the memory pages belonging to the specified VM. Can be used only with CP VMs.
 * Note that if VM has shared regions those will not be cleared and the function will return
 * with an error AC_ERR_MEMORY_IS_SHARED.
 * @param [in] eVm            VM
 */
AC_ERROR_CODE ACTzClearVmMemory(ACVirtualMachineId eVm);

int ACTzPostMilestoneConfig(void);

/** API is called by system debug image in first path of secure
 *  WDT bite reset and it clears all CPZ and TZ-owned buffers
 *  including secure application memory region.
 *  
 *  @retval returns E_SUCCESS if successful, negative values
 *          otherwise.
 */
int ACClearSecureMem(void);

/**
 * Protects the spedified
 * 
 * @param [in]  uStart      start of the region which needs to be protected
 * @param [in]  uEnd        end of the region which needs to be protected
 * @param [in]  eVM         Destination VM 
 * @return \c E_SUCCESS on success 
 *            else failure 
 */

AC_ERROR_CODE ACIOAssign(uint64 uStart, uint64 uEnd, hyp_memprot_dstVM_perm_info_t  *pDestinationInfo, uint32 uNumDestination);


/**
 * Allows the address specified for modem/MSA=1 access in pre-defined RG in modem MS-MPU (Used on SDI pass1)
 * 
 * @param [in]  uStart      start of the region which needs to be protected
 * @param [in]  uEnd        end of the region which needs to be protected
 * @return \c E_SUCCESS on success 
 *            else failure 
 */

int ACTZMapMBAInModemMSMPU(uint64 start, uint64 end);

/**
 * @brief ACIsCPZExclusiveMem    Returns if the given memory overlaps with CPZ memory.
 * @param [in] uStartAddr        start address
 * @param [in] uSize             Memory size
 * @param [out] pRetVal          TRUE or FALSE depending on CP VMs ownership
 */
int ACIsCPZExclusiveMem(uint64 uStartAddr, uint64 uSize, boolean *pRetVal);

#endif /* ACCESS_CONTROL_TZ_H */
