#ifndef TZ_INTERFACE_ARMV7_H
#define TZ_INTERFACE_ARMV7_H

/**
   @file tz_interface_armv7.h
   @brief Provide the SYSCALL API infrastructure
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The TZ_SysCall_API_mainpage.dox file contains all descriptions that
      are displayed in the output PDF generated using Doxygen and LaTeX. To
      edit or update any of the file/group text in the PDF, edit the
      TZ_SysCall_API_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2010-2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
09/26/14   sm       Changed to UEFI types
01/25/14   hw       add new tz api to get device's security state
05/08/13   leo      Edited Dxygen comments and markup for Family B updates.
03/14/13   leo      (Tech Pubs) Edited/added doxygen comments and markup.
05/05/13   as       Added Enterprise APIs.
12/12/12   rkk      Added lock/map/umap v2 APIs.
05/29/12   rkk      Added syscall for lock/map AMT chunks, page table init.
05/18/12   sg       Added syscall to restore sec cfg for a device
03/08/12   leo      (Tech Pubs) Edited/added Doxygen comments and markup.
09/12/11   kpa      Added services to support Version rollback prevention
08/20/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_macros_constants
@{ */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/** @name Helper Macros
@{ */
/** General helper macro to create a bitmask from bits low to high. */
#define TZ_MASK_BITS(h,l)     ((0xffffffff >> (32 - ((h - l) + 1))) << l)
/** Helper macro to extract the caller ID from R0. */
#define TZ_SYSCALL_CALLID(r0) ((r0 & TZ_MASK_BITS(31,12)) >> 12)
/** Helper macro to extract the class ID from R0. */
#define TZ_SYSCALL_CLASS(r0)  ((r0 & TZ_MASK_BITS(11,8)) >> 8)
/** Helper macro to extract the IRQ settings from R0. */
#define TZ_SYSCALL_IRQ(r0)    (((r0 & TZ_MASK_BITS(5,5)) >> 5)
/** Helper macro to extract the number of arguments from R0. */
#define TZ_SYSCALL_ARG(r0)    (r0 & TZ_MASK_BITS(3,0))
/**
   Macro used to define a command ID based on the service ID and
   function number.
*/
#define TZ_SYSCALL_CREATE_CMD_ID(s, f) \
  ((UINT32)(((s & 0x3ff) << 10) | (f & 0x3ff)))

/** @} */ /* end_namegroup Helper Macros */

/*----------------------------------------------------------------------------
 * Class IDs
 * -------------------------------------------------------------------------*/

/** @name Class IDs
@{ */

/** System call uses a shared memory buffer. */
#define TZ_CONTROL_CLASSID     0x0
/** Retrieves the TrustZone version. */
#define TZ_VERSION_CLASSID     0x1
/** Uses a register-based interface instead of a shared memory buffer. */
#define TZ_REGISTER_CLASSID    0x2

/** @} */ /* end_namegroup Class IDs */

/*----------------------------------------------------------------------------
 * Service IDs
 * -------------------------------------------------------------------------*/

 /** @name Service IDs
 @{ */

/** Boot (cold boot/warm boot). */
#define TZ_SVC_BOOT               1
/** Peripheral image loading. */
#define TZ_SVC_PIL                2
/** Memory dumping. */
#define TZ_SVC_DUMP               3
/** Resource locking. */
#define TZ_SVC_RES_LOCKING        4
/** I/O protection. */
#define TZ_SVC_IO_ACCESS          5
/** Miscellaneous information services. */
#define TZ_SVC_INFO               6
/** SSD services. */
#define TZ_SVC_SSD                7
/** Fuse services. */
#define TZ_SVC_FUSE               8
/** Power related services. */
#define TZ_SVC_POWER              9
/** Crypto services. */
#define TZ_SVC_CRYPTO             10
/** Secure channel key exchange. */
#define TZ_SVC_SC_KEY_EXCHANGE    11
/** Memory protection service. */
#define TZ_SVC_MEMORY_PROTECTION  12
/** Reserved service ID. */
#define TZ_SVC_RESERVED_0         13
/** Nonsecure debug FIQ. */
#define TZ_SVC_NS_DEBUG_FIQ       14
/** OCMEM service. */
#define TZ_SVC_OCMEM              15

/** Third party secure OS. */
#define TZ_SVC_EXECUTIVE_EXT      250
/** QSEE specific. */
#define TZ_SVC_EXECUTIVE          252
/** QSEE applications. */
#define TZ_SVC_TZAPPS             253
/** OEMs. */
#define TZ_SVC_OEM                254
/** Test. */
#define TZ_SVC_TEST               255

/** @} */ /* end_namegroup Service IDs */

/** @} */ /* end_addtogroup tz_macros_constants */

/*----------------------------------------------------------------------------
 * Command IDs -- Service ID[19:10] and Function ID[9:0]
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_SET_BOOT_ADDR_OLD_ID                  \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x01)

/**
   @ingroup terminate_pwr_collapse

   Called to terminate power collapse. The QSEE configures the
   processor, then issues the Wait For Interrupt (WFI) instruction.

   @note1hang Implementation takes place entirely in Monitor mode.

   @command_id
     0x00000402

   @return
     None.
*/
#define TZ_POWER_COLLAPSE_ID                     \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x02)


/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_BOOT_DEPRECATED_0                     \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x03)

/**
   @ingroup krait_errata_21_workaround_enable

   Monitor call to enable the Krait errata #21 workaround. This
   workaround sets BPCR[FBHTD]=1 and does the required branch
   predictor maintenance.

   @note1hang Implementation takes place entirely in Monitor mode.

   @command_id
     0x00000404

   @return
     E_SUCCESS (always).
*/
#define TZ_KRAIT_ERRATA_21_WORKAROUND_ENABLE_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x04)

/**
   @ingroup cpu_config

   Cold and warm boot time CPU configuration service.

   @command_id
     0x00000405

   @com_struct
     tz_syscall_rsp_s

   @sys_call_params{tz_cpu_config_req_s}
     @table{weak__tz__cpu__config__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_CPU_CONFIG_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x05)

/**
   @ingroup cpu_config_query

   Service for querying CPU configuration.

   @command_id
     0x00000406

   @sys_call_req_params{tz_cpu_config_query_req_s}
     @table{weak__tz__cpu__config__query__req__s}

   @sys_call_rsp_params{tz_cpu_config_query_rsp_s}
     @table{weak__tz__cpu__config__query__rsp__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_CPU_CONFIG_QUERY_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x06)

/**
   @ingroup secure_wdog_disable

   Subroutine used by the HLOS to disable, or enable, the secure watchdog.
   This call is a NOP when debug functionality is disabled.

   @command_id
     0x00000407

   @sys_call_params{tz_secure_wdog_disable_req_s}
     @table{weak__tz__secure__wdog__disable__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SECURE_WDOG_DISABLE_ID                \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x07)

/**
   @ingroup secure_wdog_trigger

   Subroutine used by the HLOS to trigger a secure watchdog and reset
   the device. This call is a NOP when debug functionality is
   disabled.

   @command_id
     0x00000408

   @sys_call_params{tz_secure_wdog_trigger_req_s}
     @table{weak__tz__secure__wdog__trigger__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     An error code if parameters are invalid or watchdog
     reset is not permitted; does not return otherwise.
*/
#define TZ_SECURE_WDOG_TRIGGER_ID                \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x08)

/**
   @ingroup config_hw_for_offline_ram_dump

   Subroutine used by the HLOS to configure hardware registers
   BOOT_PARTITION_SELECT and WDOG_DEBUG_EN for an offline RAM dump.

   @command_id
     0x00000409

   @sys_call_params{tz_config_hw_for_ram_dump_req_s}
     @table{weak__tz__config__hw__for__ram__dump__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; an error code if parameters are invalid or if xPU
     reconfiguration is not permitted.
*/
#define TZ_CONFIG_HW_FOR_RAM_DUMP_ID                \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x09)

/**
   @ingroup video_cpu_set_state

   Controls changing the state of the Venus core (e.g. suspend/resume),
   without full subsystem restarts

   @command_id
     0x0000040a

   @sys_call_params{tz_video_set_state_req_s}
     @table{weak__tz__video__set__state__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     Zero on success; negative value on failure.
*/
#define TZ_VIDEO_SET_STATE_ID TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x0A)

/**
   @ingroup lock_xpu_peripheral

   Subroutine used by external bootloaders to request TZ to lock
   peripheral image area

   @command_id
     0x0000040b

   @sys_call_params{tz_lock_xpu_s}
     @table{weak__tz__lock__xpu__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_XPU_LOCK_ID                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x0b)

/**
   @ingroup spin_with_irqs_fiqs_disabled

   If secure watchdog debug is allowed, calling this syscall masks IRQs and
   FIQs and starts spinning in TZ and won't return. If secure watchdog debug is
   not allowed, the call will return without side effects.

   @command_id
     0x0000040D

   @sys_call_params{tz_spin_with_irqs_fiqs_disabled_s}
     @table{weak__tz__spin__with__irqs__fiqs__disabled__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; an error code if parameters are invalid or if xPU
     reconfiguration is not permitted.
*/

#define TZ_SPIN_WITH_IRQS_FIQS_DISABLED_ID          \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x0D)

/**
   @ingroup set_milestone

   Subroutine used by external bootloaders to indicate the end
   of the bootloading stage, which is the beginning of HLOS booting.

   @command_id
     0x0000040E

   @sys_call_params{tz_set_milestone_s}
     @table{weak__tz__set__milestone__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MILESTONE_SET_ID                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x0E)

/**
   @ingroup Do mode switch

   Used by Apps BL to switch EL1 mode to A64

   @command_id
     0x0000040F

   @sys_call_params{tz_do_modeswitch_s}
     @table{weak__tz__do__modeswitch__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_DO_MODE_SWITCH                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x0F)

/**
   @ingroup force_dload_mode

   Subroutine used by the HLOS to configure hardware register
   TCSR_BOOT_MISC_DETECT to force different dload modes.

   @command_id
     0x00000410

   @sys_call_params{tz_force_dload_mode_req_s}
     @table{weak__tz__force__dload__mode__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; an error code if parameters are invalid or not permitted
*/
#define TZ_FORCE_DLOAD_ID                              \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x10)

/**
   @ingroup set_boot_addr

   Sets the address of execution for the nonsecure execution
   environment after a CPU is brought out of reset.

   @command_id
     0x00000411

   @sys_call_params{tz_set_boot_addr_s}
     @table{weak__tz__set__boot__addr__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SET_BOOT_ADDR_ID                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_BOOT, 0x11)

/**
   @ingroup init_image_syscall

   Verifies whether the memory at the specified Executable and Linkable
   Format (ELF) header pointer is a valid ELF file. Once that has been
   verified, the QSEE copies the ELF header, program header, and hash
   segment into secure memory.

   @command_id
     0x00000801

   @sys_call_params{tz_pil_init_req_s}
     @table{weak__tz__pil__init__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_INIT_ID TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x01)

/**
   @ingroup get_pil_mem_usage

   Confirms whether the memory region used to load the specified
   peripheral image matches the static region that the QSEE supports
   for the specified region, and that the region is appropriate for the
   ELF header specified in the secure Peripheral Image Loader (PIL)
   initialization call.

   @command_id
     0x00000802

   @sys_call_params{tz_pil_mem_req_s}
     @table{weak__tz__pil__mem__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_MEM_ID TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x02)

/** @cond */

/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_PIL_DEPRECATED_0 TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x03)
#define TZ_PIL_DEPRECATED_1 TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x04)
/** @endcond */

/**
   @ingroup auth_reset

   Authenticates and releases the specified peripheral from reset.
   The QSEE also protects the memory region for the specified
   peripheral image with a Memory Protection Unit (MPU).

   @command_id
     0x00000805

   @sys_call_params{tz_pil_auth_reset_req_s}
     @table{weak__tz__pil__auth__reset__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_AUTH_RESET_ID                     \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x05)

/**
   @ingroup unlock_xpu

   Stops the specified peripheral, then clears the peripheral image
   location and unlocks the memory area occupied by that region.

   @command_id
     0x00000806

   @sys_call_params{tz_pil_unlock_req_s}
     @table{weak__tz__pil__unlock__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_UNLOCK_XPU_ID                     \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x06)

/**
   @ingroup is_subsys_supported

   Queries whether the QSEE supports secure PIL for a specified
   subsystem.

   @command_id
     0x00000807

   @sys_call_req_params{tz_pil_subsystem_query_req_s}
     @table{weak__tz__pil__subsystem__query__req__s}

   @sys_call_rsp_params{tz_pil_subsystem_query_rsp_s}
     @table{weak__tz__pil__subsystem__query__rsp__s}

   @return
     E_SUCCESS if the request was successful; an error code otherwise.
*/
#define TZ_PIL_IS_SUBSYS_SUPPORTED_ID            \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x07)

/**
   @ingroup is_subsys_mandated

   Queries whether QSEE security settings make it secure to the point
   where a PIL is required for the specified subsystem.

   @command_id
     0x00000808

   @sys_call_params
     Same request and response parameters as for
     #TZ_PIL_IS_SUBSYS_SUPPORTED_ID.

   @return
     E_SUCCESS if the request was successful; an error code otherwise.
*/
#define TZ_PIL_IS_SUBSYS_MANDATED_ID             \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x08)

/**
   @ingroup get_mem_area

   Queries the start and end addresses in which subsystems are allowed
   to be loaded.

   @command_id
     0x00000809

   @sys_call_req_params{tz_pil_mem_area_query_req_s}
     @table{weak__tz__pil__mem__area__query__req__s}

   @sys_call_rsp_params{tz_pil_mem_area_query_rsp_s}
     @table{weak__tz__pil__mem__area__query__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_GET_MEM_AREA_ID                   \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_PIL,  0x09)

/*
   Deprecated: This command is no longer supported.
*/
#define TZ_DUMP_WRITE_LPASS_QDSP6_NMI_ID         \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x01)

/**
   @ingroup set_wdt_buff_addr

   System call for the nonsecure side to set the Watchdog Timer (WDT)
   reset context dump buffer address.

   @command_id
     0x00000c02

   @sys_call_params{tz_dump_set_cpu_ctx_buf_req_s}
     @table{weak__tz__dump__set__cpu__ctx__buf__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_CPU_CTX_BUF_ID               \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x02)

/*
   Deprecated: Use of this command is no longer supported
*/
#define TZ_DUMP_WRITE_MSS_QDSP6_NMI_ID           \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x03)

/**
   @ingroup set_l1_dump_buffer

   System call for the nonsecure side to set the L1 dump buffer address.

   @command_id
     0x00000c04

   @sys_call_params{tz_dump_set_cache_buf_req_s}
     @table{weak__tz__dump__set__cache__buf__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_L1_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x04)

/**
   @ingroup cache_dump_request

   Monitor call to request L0/L1/L2 cache dumping. Cache
   dumping is ignored if fuse settings indicate that secure boot
   is enabled or debug is disabled.

   A request to dump L0/L1 is assumed to indicate a fatal error condition
   in the nonsecure side. D-caches are not cleared before resetting the
   device. Entry to Download mode is controlled by the Download mode
   magic numbers that are controlled by the HLOS.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @command_id
     0x00000c05

   @param[in] r0  Command ID
   @param[in] r1  Token ID.

   @param[in] r2  0 -- Handle an L1 dump request similar to a WDT bark,
                       resulting in an L0/L1 cache dump, CPU context dumps
                       and a device reset. \n
                  1 -- Dump the L1 cache and return to the HLOS only after
                       the dump is done. \n
                  2 -- Only dump the L2 cache and return to HLOS after the
                       dump is done. \n
                  Any other value for r2 returns -4 (E_NOT_SUPPORTED).

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_REQUEST_CACHE_DUMP_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x05)

/**
   @ingroup query_l1_buffer_size

   System call for the nonsecure side to query the L1 dump buffer size.

   @command_id
     0x00000c06

   @sys_call_req_params{tz_dump_query_cache_buf_size_req_s}
     @table{weak__tz__dump__query__cache__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_cache_buf_size_rsp_s}
     @table{weak__tz__dump__query__cache__buf__size__rsp__s}

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_L1_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x06)

/**
   @ingroup set_l2_dump_buffer

   System call for for the nonsecure side to set the L2 dump buffer address.

   @command_id
     0x00000c07

   @sys_call_params{tz_dump_set_cache_buf_req_s}
     @table{weak__tz__dump__set__cache__buf__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_L2_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x07)

/**
   @ingroup query_l2_buffer_size

   System call for the nonsecure side to query the L2 dump buffer size.

   @command_id
     0x00000c08

   @sys_call_req_params{tz_dump_query_cache_buf_size_req_s}
     @table{weak__tz__dump__query__cache__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_cache_buf_size_rsp_s}
     @table{weak__tz__dump__query__cache__buf__size__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_L2_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x08)

/**
   @ingroup set_ocmem_dump_buf

   System call for the nonsecure side to set the OCMEM dump buffer address.

   @command_id
     0x00000c09

   @sys_call_req_params{tz_dump_set_ocmem_buf_req_s}
     @table{weak__tz__dump__set__ocmem__buf__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_OCMEM_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x09)

/**
   @ingroup query_ocmem_dump_buf_size

   System call for the nonsecure side to query the OCMEM dump buffer size.

   @command_id
     0x00000c0A

   @sys_call_req_params{tz_dump_query_ocmem_buf_size_req_s}
     @table{weak__tz__dump__query__ocmem__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_ocmem_buf_size_rsp_s}
     @table{weak__tz__dump__query__ocmem__buf__size__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_OCMEM_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x0A)

/**
   @ingroup security_allows_mem_dump

   System call for the SBL to determine whether memory dumping
   (i.e., DLOAD mode) is allowed.

   @command_id
     0x00000c0b

   @com_struct
     tz_syscall_req_s

   @sys_call_rsp_params{tz_security_allows_mem_dump_rsp_s}
     @table{weak__tz__security__allows__mem__dump__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x0B)

/**
   @ingroup smmu_dump_fault_syndrome

   System call for dumping the SMMU fault syndrome registers.

   @note1hang
     SMMU clocks must be ON.

   @command_id
     0x00000c0c

   @com_struct
     tz_syscall_req_s

   @return
     SUCCESS if the call succeeded; error code otherwise.
*/
#define TZ_DUMP_SMMU_FAULT_REGS_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x0C)

/**
   @ingroup security_is_retail_unlock

   System call for the SBL to determine whether retail unlock is enable.

   @command_id
     0x00000c0d

   @com_struct
     tz_syscall_req_s

   @sys_call_rsp_params{tz_security_is_retail_unlock_rsp_s}
     @table{weak__tz__security__is__retail__unlock__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_IS_RETAIL_UNLOCK_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x0D)

/**
   @ingroup lock_shared_resource

   Locks a shared hardware resource.

   @command_id
     0x00001001

   @sys_call_req_params{tz_lock_resource_req_s}
     @table{weak__tz__lock__resource__req__s}

   @sys_call_rsp_params{tz_lock_resource_rsp_s}
     @table{weak__tz__lock__resource__rsp__s}

   @return
    Zero if the return code status is filled. \n
    E_BAD_ADDRESS if the return parameter address is bad or wrong. \n
    E_NOT_SUPPORTED if the wrong resource type is passed. \n
    E_INVALID_ARG if the lock is not a value of 0 or 1.
*/
#define TZ_RESOURCE_LOCK_ID                              \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_RES_LOCKING, 0x01)


/**
   @ingroup change_resource_config

   System call to change resource configuration dynamically.

   @command_id
     0x00001002

   @sys_call_params{tz_config_resource_req_s}
     @table{weak__tz__config__resource__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_RESOURCE_CONFIG_ID                            \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_RES_LOCKING, 0x02)

/**
   @ingroup io_access_read

   Allows reading of memory areas that fall under Memory/Area/Register
   Protection Unit (xPU) protection but do not need to be secured.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @command_id
     0x00001401

   @param[in]  r0 Command ID.
   @param[in]  r1 Token ID.
   @param[in]  r2 Address to be read.
   @param[in]  r3 Not used.
   @param[out] r0 Zero on failure; otherwise the value at the address.
   @param[out] r1 Zero.
   @param[out] r2 Zero.
   @param[out] r3 Zero.

   @return
     Zero on failure; value at the address otherwise.
*/
#define TZ_IO_ACCESS_READ_ID                           \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_IO_ACCESS, 0x01)

/**
   @ingroup io_access_write

   Allows writing to memory areas that fall under xPU protection
   but do not need to be secured.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @command_id
     0x00001402

   @param[in]  r0 Command ID.
   @param[in]  r1 Token ID.
   @param[in]  r2 Address to be written.
   @param[in]  r3 Value to write to address.
   @param[out] r0 E_INVALID_ARG on failure; zero otherwise.
   @param[out] r1 Zero.
   @param[out] r2 Zero.
   @param[out] r3 Zero.

   @return
     E_INVALID_ARG on failure; zero otherwise.
*/
#define TZ_IO_ACCESS_WRITE_ID                          \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_IO_ACCESS, 0x02)

/*
 * Reserved ID.
 */
#define TZ_IO_ACCESS_RESERVED_3_ID                     \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_IO_ACCESS, 0x03)

/*
 * Reserved ID.
 */
#define TZ_IO_ACCESS_RESERVED_4_ID                     \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_IO_ACCESS, 0x04)

/**
   @ingroup is_svc_avail

   Checks if a system call is available.

   @command_id
     0x00001801

   @sys_call_req_params{tz_info_is_svc_available_req_s}
     @table{weak__tz__info__is__svc__available__req__s}

   @sys_call_rsp_params{tz_info_is_svc_available_rsp_s}
     @table{weak__tz__info__is__svc__available__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_INFO_IS_SVC_AVAILABLE_ID              \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_INFO, 0x01)

/**
   @ingroup get_diag_info

   Copies the QSEE software diagnostic region into the specified buffer.

   @command_id
     0x00001802

   @sys_call_params{tz_info_get_diag_req_s}
     @table{weak__tz__info__get__diag__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_INFO_GET_DIAG_ID                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_INFO, 0x02)

/**
   @ingroup get_feature_ver

   Returns the current version of a specified QSEE feature.

   @command_id
     0x00001803

   @sys_call_req_params{tz_feature_version_req_s}
     @table{weak__tz__feature__version__req__s}

   @sys_call_rsp_params{tz_feature_version_rsp_s}
     @table{weak__tz__feature__version__rsp__s}

   @returns
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_INFO_GET_FEATURE_VERSION_ID           \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_INFO, 0x03)

/**
   @ingroup get_secure_state

   System call to check the security status of the system

   @command_id
     0x00001804

   @sys_call_params{tz_get_secure_state_req_s}
     @table{weak__tz__get__secure__state__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
 */
#define TZ_INFO_GET_SECURE_STATE                     \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_INFO, 0x4)


/**
   @ingroup decrypt_image

   Parses the XML in the encrypted bootloader image and
   decrypts the data in place.

   @command_id
     0x00001c01

   @sys_call_params{tz_ssd_decrypt_img_req_s}
     @table{weak__tz__ssd__decrypt__img__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_IMG_ID                    \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SSD, 0x01)

/**
   @ingroup encrypt_keystore

   Encrypts the keystore with a key derived from the Crypto Engine (CE)
   hardware key.

   @command_id
     0x00001c02

   @sys_call_params{tz_ssd_encrypt_keystore_req_s}
     @table{weak__tz__ssd__encrypt__keystore__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_ENCRYPT_KEY_STORE_ID              \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SSD, 0x02)

/*----------------------------------------------------------------------------
                Deprecated Command ID: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_SSD_DEPRECATED_0                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SSD, 0x03)

/*----------------------------------------------------------------------------
                Deprecated Command ID: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_SSD_DEPRECATED_1                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SSD, 0x04)

/**
   @ingroup protect_keystore

   Loads the Secure Software Download (SSD) keystore into the QSEE
   software for future use.

   @sys_call_req_params{tz_ssd_protect_keystore_req_s}
     @table{weak__tz__ssd__protect__keystore__req__s}

   @sys_call_rsp_params{tz_ssd_protect_keystore_rsp_s}
     @table{weak__tz__ssd__protect__keystore__rsp__s}

   @command_id
     0x00001c05

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_PROTECT_KEYSTORE_ID                  \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SSD, 0x05)

/**
   @ingroup parse_md

   Parses the SSD XML header in a given encrypted image and stores the
   resulting metadata to use for subsequent calls to decrypt image
   fragments.

   @sys_call_req_params{tz_ssd_parse_md_req_s}
     @table{weak__tz__ssd__parse__md__req__s}

   @sys_call_rsp_params{tz_ssd_parse_md_rsp_s}
     @table{weak__tz__ssd__parse__md__rsp__s}

   @par Parse MD response structure (tz_ssd_parse_md_rsp_info_s)
     @table{weak__tz__ssd__parse__md__rsp__info__s}

   @command_id
     0x00001c06

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_PARSE_MD_ID                       \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SSD, 0x06)

/**
   @ingroup decrypt_img_frag

   Decrypts the data in place for a specified fragment of an encrypted image.

   @sys_call_req_params{tz_ssd_decrypt_img_frag_req_s}
     @table{weak__tz__ssd__decrypt__img__frag__req__s}

   @sys_call_rsp_params{tz_ssd_decrypt_frag_rsp_s}
     @table{weak__tz__ssd__decrypt__frag__rsp__s}

   @command_id
     0x00001c07

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_IMG_FRAG_ID               \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SSD, 0x07)

/**
   @ingroup decrypt_elf_seg_frag

   Decrypts the data in place for a specified ELF segment from an
   encrypted image.

   @sys_call_req_params{tz_ssd_decrypt_elf_seg_frag_req_s}
     @table{weak__tz__ssd__decrypt__elf__seg__frag__req__s}

   @sys_call_rsp_params{tz_ssd_decrypt_frag_rsp_s}
     @table{weak__tz__ssd__decrypt__frag__rsp__s}

   @command_id
     0x00001c08

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_ELF_SEG_FRAG_ID               \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SSD, 0x08)


/**
   @ingroup blow_sw_fuse

   Changes the state of the software fuse.

   @command_id
     0x00002001

   @sys_call_params{tz_sw_fuse_req_s}
     @table{weak__tz__sw__fuse__req__s}

   @com_struct
     See #TZ_IS_SW_FUSE_BLOWN_ID for the response structure.

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_BLOW_SW_FUSE_ID                       \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_FUSE, 0x01)

/**
   @ingroup is_sw_fuse_blown

   Queries whether the specified software fuse has been blown.

   @caution1hang This call is not thread safe.

   @command_id
     0x00002002

   @sys_call_params{tz_sw_fuse_rsp_s}
     @table{weak__tz__sw__fuse__rsp__s}

   @com_struct
     See #TZ_BLOW_SW_FUSE_ID for the request structure.

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_IS_SW_FUSE_BLOWN_ID                   \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_FUSE, 0x02)

/**
   @ingroup write_qfprom_fuse

   Writes a Qualcomm Field Programmable Read Only Memory (QFPROM) fuse row.

   @command_id
     0x00002003

   @sys_call_params{tz_qfprom_write_row_s}
     @table{weak__tz__qfprom__write__row__s}

   @com_struct
      tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_WRITE_ROW_ID                   \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_FUSE, 0x03)

/**
   @ingroup write_mult_qfprom_rows

   Writes multiple rows of QFPROM fuses.

   @note1hang This call is only available prior to a
              #TZ_MILESTONE_SET_ID call.

   @command_id
     0x00002004

   @sys_call_params{tz_qfprom_write_multiple_rows_s}
     @table{weak__tz__qfprom__write__multiple__rows__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
 */
#define TZ_QFPROM_WRITE_MULTIPLE_ROWS_ID         \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_FUSE, 0x04)

/**
   @ingroup read_qfprom_fuse

   Reads QFPROM fuses.

   @command_id
     0x00002005

   @sys_call_params{tz_qfprom_read_row_s}
     @table{weak__tz__qfprom__read__row__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_READ_ROW_ID                    \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_FUSE, 0x05)

/**
   @ingroup write_rollback_qfprom_fuse

   Write rollback-specific QFPROM fuses.

   @note1hang This call is only available prior to a
              #TZ_MILESTONE_SET_ID call.

   @command_id
     0x00002006

   @sys_call_params{tz_qfprom_rollback_write_row_s}
     @table{weak__tz__qfprom__rollback__write__row__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_ROLLBACK_WRITE_ROW_ID          \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_FUSE, 0x06)

/**
   @ingroup spmi_disable_bus

   Disables SPMI bus to allow clean shutdowns when deasserting PS_HOLD.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @command_id
     0x00002401

   @param[in] r0  Command ID
   @param[in] r1  Token ID.

   @param[in] r2  Reserved/spare, should be zero.

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_POWER_SPMI_DISABLE_BUS_ID                \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_POWER, 0x01)

/**
   @ingroup get_prng_data

   Retrieve random data from the Pseudo-Random Number Generator (PRNG)
   hardware.

   @command_id
     0x00002801

   @sys_call_params{tz_get_prng_data_s}
     @table{weak__tz__get__prng__data__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_PRNG_GETDATA_ID                         \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_CRYPTO, 0x01)

/**
   @ingroup init_secure_channel

   Writes the key material to the specified address.

   This function returns E_ALREADY_DONE if the keys are already written.

   @command_id
     0x00002c01

   @sys_call_params{tz_sc_init_s}
     @table{weak__tz__sc__init__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS      -- Function was successful. \n
     E_FAILURE      -- Secure channel could not be initialized. \n
     E_ALREADY_DONE -- Secure channel is already established.
*/
#define TZ_SECURE_CHANNEL_INIT_ID                            \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SC_KEY_EXCHANGE, 0x01)

/**
   @ingroup deinit_secure_channel

   Clears the keys and resets the channel state.

   @command_id
     0x00002c02

   @sys_call_params{tz_sc_deinit_s}
     @table{weak__tz__sc__deinit__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; E_FAILURE otherwise.
*/
#define TZ_SECURE_CHANNEL_DEINIT_ID                          \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_SC_KEY_EXCHANGE, 0x02)

/**
   @ingroup protect_memory

   Protects/unprotects a memory range with an MPU.

   @command_id
     0x00003001

   @sys_call_params{tz_protect_memory_s}
     @table{weak__tz__protect__memory__s}

   @com_struct
      tz_syscall_rsp_s

   @return
     E_SUCCESS on success; error code otherwise.
*/
#define TZ_PROTECT_MEMORY                                    \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x01)

/**
   @ingroup install_nonsecure_debug_fiq

   Installs a nonsecure debug FIQ handler.

   @command_id
     0x00003801

   @sys_call_params{tz_config_ns_debug_fiq_req_s}
     @table{weak__tz__config__ns__debug__fiq__req__s}

   @com_struct
      tz_syscall_rsp_s

   @return
      E_SUCCESS if the nonsecure debug FIQ handler and context were
      installed successfully; error code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_CONFIG_ID \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_NS_DEBUG_FIQ, 0x01)

/**
   @ingroup query_nonsecure_debug_fiq_ctx_buf

   Queries the minimum size of a nonsecure debug FIQ context buffer. A pointer
   to the context buffer is passed to the nonsecure handler in register r0.

   @command_id
     0x00003802

   @com_struct
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     E_SUCCESS if the context buffer size query was successfull; error
     code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_CTX_SIZE_ID \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_NS_DEBUG_FIQ, 0x02)

/**
   @ingroup query_nonsecure_debug_fiq_int

   Queries whether an interrupt is allowed to be configured as a nonsecure
   debug FIQ. The QSEE only allows a limited set of chipset-specific interrupts
   to be used for this purpose.

   @command_id
     0x00003803

   @sys_call_params{tz_config_ns_debug_fiq_int_ok_req_s}
     @table{weak__tz__config__ns__debug__fiq__int__ok__req__s}

   @com_struct
      tz_syscall_rsp_s

   @return
     E_SUCCESS if the context buffer size query was successfull; error
     code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_INT_OK_ID \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_NS_DEBUG_FIQ, 0x03)

/**
   @ingroup restore_sec_cfg

   Restores VMIDMT/xPU/SMMU configuration of a device's System Memory
   Management Unit (SMMU) global space.

   @note1hang SMMU clocks must be ON.

   @command_id
     0x00003002

   @sys_call_params{tz_restore_sec_cfg_req_s}
     @table{weak__tz__restore__sec__cfg__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_RESTORE_SEC_CFG  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x02)


/**
   @ingroup mem_protect_get_ptbl_mem_size

   Queries the memory size requirement for multimedia SMMU page
   tables.

   @command_id
     0x00003003

   @sys_call_req_params{tz_memprot_ptbl_mem_size_req_s}
     @table{weak__tz__memprot__ptbl__mem__size__req__s}

   @sys_call_rsp_params{tz_memprot_ptbl_mem_size_rsp_s}
     @table{weak__tz__memprot__ptbl__mem__size__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_GET_PTBL_MEM_SIZE  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x03)


/**
   @ingroup mem_protect_ptbl_mem_init

   Provides memory used for page tables by multimedia SMMU
   devices involved in content protection.

   @command_id
     0x00003004

   @sys_call_params{tz_memprot_ptbl_mem_init_req_s}
     @table{weak__tz__memprot__ptbl__mem__init__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_PTBL_MEM_INIT  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x04)


/**
   @ingroup mem_protect_set_cp_pool_size

   Provide the CP pool size to calculate translation table
   requirement

   @command_id
     0x00003005

   @sys_call_params{tz_memprot_ptbl_mem_init_req_s}
     @table{weak__tz__memprot__set__cp__pool__size__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_SET_CP_POOL_SIZE TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x05)


/**
   @ingroup mem_protect_video_var

   Sets secure Virtual Address Ranges (VAR) in the video device's
   virtual address map.

   @command_id
     0x00003008

   @sys_call_params{tz_memprot_video_var_req_s}
     @table{weak__tz__memprot__video__var__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_VIDEO_VAR TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x08)

/**
   @ingroup vmidmt_set_memtype

   Sets the memtype attribute for a Virtual Machine ID Mapping Table (VMIDMT)
   associated with a given device.

   @command_id
     0x00003009

   @sys_call_params{tz_set_vmidmt_memtype_req_s}
     @table{weak__tz__set__vmidmt__memtype__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_VMIDMT_SET_MEMTYPE  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x09)

/**
   @ingroup disable_mmss_sec

   Disable the QRIB security for the MMSS subsystems

   @command_id
     0x00003020

   @com_struct
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     \c E_SUCCESS on success; an error code otherwise.
 */
#define TZ_DISABLE_MMSS_SEC  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x20)

/**
   @ingroup mem_protect_lock2

  Protects or removes protection from a list of memory chunks.

   @command_id
     0x0000300A

   @sys_call_params{tz_memprot_lock2_req_s}
     @table{weak__tz__memprot__lock2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_LOCK2   TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x0A)

/**
   @ingroup mem_protect_map2

   Maps a list of memory chunks (sets up VA to PA mapping) on a
   QSEE-managed context bank.

   @command_id
     0x0000300B

   @sys_call_params{tz_memprot_map2_req_s}
     @table{weak__tz__memprot__map2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_MAP2    TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x0B)


/**
   @ingroup mem_protect_unmap2

   Removes VA to PA mapping on a QSEE-managed context bank.

   @command_id
     0x0000300C

   @sys_call_params{tz_memprot_unmap2_req_s}
     @table{weak__tz__memprot__unmap2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_UNMAP2  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x0C)

/**
   @ingroup mem_protect_tlbinval

   Invalidates Translation Lookaside Buffer (TLB) entries associated with a
   QSEE-managed context bank.

   @command_id
     0x0000300D

   @sys_call_params{tz_memprot_tlbinval_req_s}
     @table{weak__tz__memprot__tlbinval__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_TLBINVALIDATE  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x0D)

/**
   @ingroup config_xpu_violation_err_fatal

   Subroutine used by the HLOS to enable/disable xPU violations becoming fatal
   errors. This can also be used to check whether xPU violations are currently
   fatal errors by sending TZ_XPU_VIOLATION_ERR_FATAL_NOOP as the parameter.

   The TZ may choose to force enable/disable xPU violations as fatal errors by
   default. It may also choose to disallow the HLOS from overriding this
   configuration. So, the HLOS must always check the output parameter (enabled)
   to see the current behavior and the return code to see if it was allowed to
   carry out the operation (-E_NOT_SUPPORTED is returned if the HLOS is not
   allowed to change the behavior).

   @command_id
     0x0000030E

   @sys_call_req_params{tz_xpu_violation_err_fatal_req_s}
     @table{weak__tz__xpu__violation__err__fatal__req__s}

   @sys_call_rsp_params{tz_xpu_violation_err_fatal_rsp_s}
     @table{weak__tz__pil__subsystem__query__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_CONFIG_XPU_VIOLATION_ERR_FATAL                \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x0E)

/**
   @ingroup mem_protect_dev_var

   Sets secure Virtual Address Ranges (VAR) in the requested
   device's virtual address map in an specific device.

   @command_id
     0x0000300F

   @sys_call_params{tz_memprot_dev_var_req_s}
     @table{weak__tz__memprot__dev__var__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_DEVICE_VAR                        \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_MEMORY_PROTECTION, 0x0F)

/**
   @ingroup ocmem_lock_region

   System call to lock an OCMEM region.

   @command_id
     0x00003C01

   @sys_call_params{tz_ocmem_lock_region_req_s}
     @table{weak__tz__ocmem__lock__region__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_LOCK_REGION       TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_OCMEM, 0x1)

/**
   @ingroup ocmem_unlock_region

   System call to unlock an OCMEM region.

   @command_id
     0x00003C02

   @sys_call_params{tz_ocmem_unlock_region_req_s}
     @table{weak__tz__ocmem__unlock__region__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_UNLOCK_REGION     TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_OCMEM, 0x2)

/**
   @ingroup ocmem_enable_mem_dump

   System call to enable memory dumping for an OCMEM region.

   @command_id
     0x00003C03

   @sys_call_params{tz_ocmem_enable_mem_dump_req_s}
     @table{weak__tz__ocmem__enable__mem__dump__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_ENABLE_MEM_DUMP   TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_OCMEM, 0x3)

/**
   @ingroup ocmem_disable_mem_dump

   System call to disable memory dumping for an OCMEM region.

   @command_id
     0x00003C04

   @sys_call_params{tz_ocmem_disable_mem_dump_req_s}
     @table{weak__tz__ocmem__disable__mem__dump__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_DISABLE_MEM_DUMP  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_OCMEM, 0x4)

/**
   @ingroup external_ee_passthrough

   Passthrough to third party secure execution environments.

   @command_id
     0x0003E801

   @com_struct
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     Returns any value from the QSEE.
*/
#define TZ_EXECUTIVE_EXT_ID \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_EXECUTIVE_EXT, 0x01)

/**
   @ingroup qsee_passthrough

   Passthrough to the QSEE enhanced kernel layer.

   @command_id
     0x0003F001

   @com_struct
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     Returns any value from the QSEE.
*/
#define TZ_EXECUTIVE_ID                                \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_EXECUTIVE, 0x01)

/**
   @ingroup  qsee_apps_passthrough

   Passthrough to the QSEE secure applications.

   @command_id
     0x0003F401

   @com_struct
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     Returns any value from the QSEE.
*/
#define TZ_TZAPPS_SEND_CMD_ID                      \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_TZAPPS, 0x01)

/* This definition is provided as an example for OEMs and does not
 * need to be included in the syscall documentation
 */
#define TZ_OEM_TEST_ID \
  TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_OEM, 0x01)

#pragma pack(push)
#pragma pack(1)  /* For Packing structures */

/** @addtogroup tz_datatypes
@{ */

/**
   Common system call request header used in all system calls.
 */
typedef struct tz_syscall_req_s
{
   UINT32  len;                 /**< Total length of available memory
                                     for requests and responses. */

   UINT32  command_start_off;   /**< Offset of command-specific data
                                     from the beginning of the buffer. */

   UINT32  response_write_off;  /**< Start offset of the memory to
                                     where the response is to be
                                     written.  The offset is computed
                                     from the start of this
                                     structure. */

   UINT32  command_id;          /**< Unique identifier that
                                     establishes which command follows
                                     this structure. */
} tz_syscall_req_t;


/**
   Common system call response header used in all system calls that
   have responses.
 */
typedef struct tz_syscall_rsp_s
{
   UINT32  len;                 /**< Total length of the response. */

   UINT32  response_start_off;  /**< Start offset of the memory to where
                                     the response information is to be
                                     written. The offset is computed
                                     from the start of this
                                     structure. */

   INT32   status;              /**< Status of the syscall_req
                                     executed: \n
                                     0 means the command was not completed \n
                                     1 means the command was completed */
} tz_syscall_rsp_t;

/**
   VA range structure in set VA range API.
 */
typedef struct tz_va_range_s
{
  UINT32  va_start; /**< VA Start Address. */
  UINT32  va_size;  /**< VA range size. */
}tz_va_range_t;

/** @} */ /* end_addtogroup tz_datatypes */
/*----------------------------------------------------------------------------
 * Control Class Structures
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * BOOT Service Structures
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_macros_constants
@{ */

/**
   @name HLOS Start Address Slots
   HLOS start address slots provided by Core service
   TZ_CORE_SVC_START_ADDR_REQ_ID.
   @{
*/
/** CPU address is AArch64. */
#define TZ_BOOT_ADDR_AARCH64               0x00000001
/** CPU initial boot. */
#define TZ_BOOT_ADDR_COLD                  0x00000002
/** CPU subsequent boots/resets. */
#define TZ_BOOT_ADDR_WARM                  0x00000004

/** @} */ /* end_name_group HLOS Start Address Slots */

/** @} */ /* end_addtogroup tz_macros_constants */



/**
   @weakgroup weak_tz_set_boot_addr_s
@{
*/
typedef struct tz_set_boot_addr_s
{
  struct tz_syscall_req_s common_req;
                                   /**< Common request structure. */
  UINT32 boot_addr;                /**< The address to which the QSEE
                                        is to jump after CPU
                                        initialization.  It must be a
                                        physical address, but can be
                                        NULL. Addresses in the secure
                                        memory are not accepted and
                                        cause the service to return
                                        E_BAD_ADDRESS. */
  UINT32 affl0_bmsk;               /**< A bitmask of the CPUs
                                        identified by the AFFL0 field
                                        of the MPIDR. A value of zero
                                        is not allowed.*/
  UINT32 affl1_bmsk;               /**< A bitmask of the CPUs
                                        identified by the AFFL1 field
                                        of the MPIDR. A value of zero
                                        is not allowed. */
  UINT32 affl2_bmsk;               /**< A bitmask of the CPUs
                                        identified by the AFFL2 field
                                        of the MPIDR. A value of zero
                                        is not allowed. */
  UINT32 affl3_bmsk;               /**< A bitmask of the CPUs
                                        identified by the AFFL3 field
                                        of the MPIDR. A value of zero
                                        is not allowed. */
  UINT32 flags;                    /**< Combination of TZ_BOOT_ADDR_*
                                        values. The same address can
                                        be set to multiple HLOS start
                                        address slots by bitwise ORing
                                        the values. Unused bits are
                                        ignored and must be kept at
                                        zero. A value of zero is not
                                        allowed. */
} tz_set_boot_addr_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_set_milestone_s
@{
*/
typedef struct tz_set_milestone_s
{
  struct tz_syscall_req_s common_req;
                                   /**< Common request structure. */

  UINT32 e_entry;                  /**< Address to which the QSEE
                                        is to jump after the milestone call.
                                        It must be a physical address.
                                        Addresses in the secure memory are
                                        not accepted and cause the service
                                        to return E_BAD_ADDRESS. */
  UINT32 mode;                     /**< Used on targets that support
                                        flashless boot to provide boot
                                        mode information.  */
  UINT32 ei_class;                 /**< Info from ELF header of image
                                        to jump to after the milestone
                                        call.  */

} tz_set_milestone_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_do_modeswitch_s
@{
*/

typedef struct hlos_boot_params_s
{
  UINT64 el1_x0;
  UINT64 el1_x1;
  UINT64 el1_x2;
  UINT64 el1_x3;
  UINT64 el1_x4;
  UINT64 el1_x5;
  UINT64 el1_x6;
  UINT64 el1_x7;
  UINT64 el1_x8;
  UINT64 el1_elr;
} hlos_boot_params_t;

typedef struct tz_do_modeswitch_s
{
  struct tz_syscall_req_s common_req;
                                   /**< Common request structure. */

  hlos_boot_params_t hlos_boot_info;
                                   /**< hlos boot info. */
} tz_do_modeswitch_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_secure_wdog_disable_req_s
@{
*/
typedef struct tz_secure_wdog_disable_req_s
{
  struct tz_syscall_req_s common_req;
                                   /**< Common request structure. */

  BOOLEAN disable;                 /**< If TRUE, the secure WDOG is
                                        disabled; enabled otherwise. */
} tz_secure_wdog_disable_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_secure_wdog_trigger_req_s
@{
*/
typedef struct tz_secure_wdog_trigger_req_s
{
  struct tz_syscall_req_s common_req;
                                   /**< Common request structure. */

  BOOLEAN disable_debug;           /**< If TRUE, the watchdog debug image
                                        is disabled (i.e., it will not run
                                        after a watchdog reset). */
} tz_secure_wdog_trigger_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lock_xpu_s
@{
*/
typedef struct tz_lock_xpu_s
{
  struct tz_syscall_req_s    common_req;
                                   /**< Common request structure. */

  UINT32 elf_header_addr;          /**< Address to which the QSEE will
                                   extract elf header associated with
                                   proc id given as second argument. */
  UINT32 program_header;

  UINT32 proc_id;                  /**< Proc id associated with elf header  */

  UINT32 dummy;
} tz_lock_xpu_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_hw_for_ram_dump_req_s
@{
*/
typedef struct tz_config_hw_for_ram_dump_req_s
{
  struct tz_syscall_req_s common_req;
                                   /**< Common request structure. */

  UINT32 disable_wd_dbg;           /**< If TRUE, the watchdog debug image
                                        is disabled (i.e., it will not run
                                        after a watchdog reset). */

  UINT32 boot_partition_sel;       /**< Value programmed into the
                                        BOOT_PARTION_SELECT register. */
} tz_config_hw_for_ram_dump_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_force_dload_mode_req_s
@{
*/
typedef struct tz_force_dload_mode_req_s
{
  struct tz_syscall_req_s common_req;
                                   /**< Common request structure. */

  UINT32 config;                  /**< Value programmed into the
                                        TCSR_BOOT_MISC_DETECT register. */

  UINT32 spare;                  /**< Spare */
} tz_force_dload_mode_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/** Video Core State. */
typedef enum
{
  TZ_VIDEO_STATE_SUSPEND = 0,      /**< Assert reset to the video core. */
  TZ_VIDEO_STATE_RESUME,           /**< Re-initialize video HW, if required,
                                        and de-assert video core reset. */
} tz_video_state_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_video_set_state_req_s
@{
*/
typedef struct tz_video_set_state_req_s
{
  struct tz_syscall_req_s common_req;
                                   /**< Common request structure. */

  UINT32 state;                    /**< Requested video state, one
                                        of #tz_video_state_e_type*/

  UINT32 spare;                    /**< Reserved for future use.
                                        Should be zero */
} tz_video_set_state_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_hw_for_ram_dump_req_s
@{
*/
typedef struct tz_spin_with_irqs_fiqs_disabled_s
{
  struct tz_syscall_req_s common_req; /**< Common request structure. */
  UINT32 spare;                       /**< Reserved for future use. */
} tz_spin_with_irqs_fiqs_disabled_t;

/** @} */ /* end_weakgroup */


/** @addtogroup tz_datatypes
@{ */

/*----------------------------------------------------------------------------
 * PIL Service Structures
 * -------------------------------------------------------------------------*/
/**
    Peripheral types.
 */
typedef enum
{
  TZ_PIL_AUTH_MODEM_PROC,       /**< @deprecated This enum member
                                        is deprecated. Use
                                        TZ_PIL_AUTH_MODEM_FW_PROC. */
  TZ_PIL_AUTH_QDSP6_PROC,       /**< LPASS Hexagon@tm processor. */
  TZ_PIL_AUTH_SPS_PROC,         /**< @deprecated SPS processor. */
  TZ_PIL_AUTH_EXEC,             /**< QSEE generic executive image. */
  TZ_PIL_AUTH_MODEM_SW_PROC,    /**< Modem software processor. */
  TZ_PIL_AUTH_MODEM_FW_PROC,    /**< Modem firmware processor. */
  TZ_PIL_AUTH_WLAN_PROC,        /**< Riva processor. */
  TZ_PIL_AUTH_SEC_APP,          /**< QSEE software secure applications. */
  TZ_PIL_AUTH_GNSS_PROC,        /**< GNSS processor. */
  TZ_PIL_AUTH_VIDEO_PROC,       /**< Video processor */
  TZ_PIL_AUTH_VPU_PROC,         /**< VPU processor */
  TZ_PIL_AUTH_BCSS_PROC,        /**< BCSS processor */
  TZ_PIL_AUTH_UNSUPPORTED_PROC  /**< Processor not supported. */
} tz_pil_proc_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_pil_init_req_s
@{
*/
typedef struct tz_pil_init_req_s
{
  struct tz_syscall_req_s  common_req;
                                       /**< Common request structure. */

  UINT32               proc;           /**< Processor identifier for
                                            the current operation.
                                            See #tz_pil_proc_e_type. */
  UINT32               image_hdr_ptr;  /**< Pointer to the physical
                                            address location of the
                                            ELF header for the given
                                            processor identifier. */
} tz_pil_init_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_req_s
@{
*/
typedef struct tz_pil_mem_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */

  UINT32               proc;        /**< Processor identifier for the
                                         current operation. See
                                         #tz_pil_proc_e_type. */

  UINT32               start_addr;  /**< The HLOS can load the image
                                         in any memory location, so
                                         this is used to take the
                                         difference between the
                                         program header addresses and
                                         start_addr. */

  UINT32               image_len;   /**< Total length of the image in
                                         memory. */
} tz_pil_mem_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_auth_reset_req_s
@{
*/
typedef struct tz_pil_auth_reset_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */

  UINT32               proc;        /**< Processor identifier for the
                                         current operation. See
                                         #tz_pil_proc_e_type. */
} tz_pil_auth_reset_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_unlock_req_s
@{
*/
typedef struct tz_pil_unlock_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */

  UINT32               proc;        /**< Processor ientifier for the
                                         current operation. See
                                         #tz_pil_proc_e_type. */
} tz_pil_unlock_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_subsystem_query_req_s
@{
*/
typedef struct tz_pil_subsystem_query_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */

  UINT32               proc;        /** Processor/subsystem
                                        identifier. See
                                        #tz_pil_proc_e_type. */

} tz_pil_subsystem_query_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_subsystem_query_rsp_s
@{
*/
typedef struct tz_pil_subsystem_query_rsp_s
{
  struct tz_syscall_rsp_s  common_rsp;
                                    /**< Common response structure. */

  UINT32               verdict;     /**< TRUE (1) if the answer to
                                         the query is true; FALSE
                                         (0) otherwise. */
} tz_pil_subsystem_query_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_area_query_req_s
@{
*/
typedef struct tz_pil_mem_area_query_req_s
{
  struct tz_syscall_req_s     common_req;
                                    /**< Common request structure. */

  UINT32               spare;       /**< Unused; should be zero.   */

} tz_pil_mem_area_query_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_area_query_rsp_s
@{
*/
typedef struct tz_pil_mem_area_query_rsp_s
{
  struct tz_syscall_rsp_s     common_rsp;
                                    /**< Common response structure. */

  UINT32               start;       /**< Start address of relocatable
                                         PIL region (inclusive). */

  UINT32               end;         /**< End address of relocatable
                                         PIL region (exclusive). */
} tz_pil_mem_area_query_rsp_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * DUMP Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_pil_qdsp6_nmi_req_s
@{
*/
typedef struct tz_pil_qdsp6_nmi_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */

  UINT32               val;         /**< Value for the LPASS Hexagon
                                         NMI register. Bits other
                                         than the least significant
                                         one are ignored by the
                                         call. */
} tz_pil_qdsp6_nmi_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_cpu_ctx_buf_req_s
@{
*/
typedef struct tz_dump_set_cpu_ctx_buf_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */

  UINT32               addr;        /**< Address of the WDT reset
                                         context dump buffer. When a
                                         nonsecure WDT expires, the
                                         CPU contexts are saved to
                                         this buffer. The entire
                                         buffer must be in nonsecure
                                         memory. */

  UINT32               size;        /**< Size of the WDT reset context
                                         dump buffer in bytes. */
} tz_dump_set_cpu_ctx_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_cache_buf_req_s
@{
*/
typedef struct tz_dump_set_cache_buf_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure */

  UINT32               addr;        /**< Address of L1 or L2 dump buffer.

                                         When a nonsecure WDT that is
                                         configured as a Fast Interrupt
                                         Request (FIQ) expires, or if the
                                         nonsecure side requests L1 dumping,
                                         L1 caches are saved to this buffer.

                                         The entire buffer must be in nonsecure
                                         memory and must be 4 KB aligned.
                                         L2 is dumped only on a nonsecure
                                         side request. */

  UINT32               size;        /**< Size of the L1 dump buffer in
                                         bytes. */
} tz_dump_set_cache_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_cache_buf_size_req_s
@{
*/
typedef struct tz_dump_query_cache_buf_size_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure */

  UINT32               spare;       /**< Spare field for future use;
                                         must be zero. */
} tz_dump_query_cache_buf_size_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_cache_buf_size_rsp_s
@{
*/
typedef struct tz_dump_query_cache_buf_size_rsp_s
{

  struct tz_syscall_rsp_s  common_rsp;
                                    /**< Common response structure. */

  UINT32               size;        /**< After a successfull call, the L1/L2
                                         dump size in bytes is stored
                                         here. */
} tz_dump_query_cache_buf_size_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_ocmem_buf_req_s
@{
*/
typedef struct tz_dump_set_ocmem_buf_req_s
{
  UINT32                        addr;
    /**< Address of OCMEM dump buffer. When a nonsecure WDT as an FIQ
         expires, or if the nonsecure side requests OCMEM dumping, OCMEM
         data is saved to this buffer.

         The entire buffer must be in nonsecure memory and must be 4KB aligned. */

  UINT32                        size;
    /**< Size of the dump buffer in bytes. */
} tz_dump_set_ocmem_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_ocmem_buf_size_req_s
@{
*/
typedef struct tz_dump_query_ocmem_buf_size_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure */

  UINT32               spare;       /**< Spare field for future use;
                                         must be zero. */
} tz_dump_query_ocmem_buf_size_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_ocmem_buf_size_rsp_s
@{
*/
typedef struct tz_dump_query_ocmem_buf_size_rsp_s
{
  tz_syscall_rsp_t              common_rsp;  /**< Common response structure. */

  UINT32                        size;        /**< After a successfull call, the
                                                  OCMEM dump size in bytes is
                                                  stored here. */
} tz_dump_query_ocmem_buf_size_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_security_allows_mem_dump_rsp_s
@{
*/
typedef struct tz_security_allows_mem_dump_rsp_s
{
  struct tz_syscall_rsp_s     common_rsp;
                                    /**< Common request structure. */

  UINT32                      allowed;
                                    /**< If non-zero, the security policy
                                         permits dumping memory for
                                         debugging. */
} tz_security_allows_mem_dump_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_security_is_retail_unlock_rsp_s
@{
*/
typedef struct tz_security_is_retail_unlock_rsp_s
{
  struct tz_syscall_rsp_s     common_rsp;
                                    /**< Common request structure. */

  UINT32                      enabled;
                                    /**< If non-zero, the retail unlock
                                         is enable. */
} tz_security_is_retail_unlock_rsp_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * RES_LOCKING Service Structures
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_datatypes
@{ */

/**
   Hardware resources available for configuration and locking by
   the QSEE.

   @note1hang The CE numbering is logical and does not map to actual
              hardware CE naming directly.
*/
typedef enum
{
  TZ_RESOURCE_CE1    = 1, /**< @deprecated This enum member is deprecated.
                                  Use TZ_RESOURCE_CE_MSS. */
  TZ_RESOURCE_CE_MSS = 1, /**< Modem CE resource. */
  TZ_RESOURCE_CE2    = 2, /**< @deprecated This enum member is
                                  deprecated. Use TZ_RESOURCE_CE_AP. */
  TZ_RESOURCE_CE_AP  = 2, /**< Application processor's CE resource. */
  TZ_RESOURCE_RNG    = 3  /**< Random number generator. */
} TZ_XPU_RESOURCE_ET;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_lock_resource_req_s
@{
*/
typedef struct tz_lock_resource_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */

  UINT32               res;         /**< Resource to lock. See
                                         #TZ_XPU_RESOURCE_ET. */

  UINT32               lock;        /**< Set to 1 to lock the resource
                                         and 0 to unlock it. */
} tz_lock_resource_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lock_resource_rsp_s
@{
*/
typedef struct tz_lock_resource_rsp_s
{
  struct tz_syscall_rsp_s  common_rsp;
                                    /**< Common response structure. */

  INT32                ret;         /**< Return code: 0 means success;
                                         E_BUSY means the resource is
                                         locked; other negative
                                         values mean failure. */
} tz_lock_resource_rsp_t;

/** @} */ /* end_weakgroup */


/** @addtogroup tz_macros_constants
@{ */

/** @name Crypto Engine Access Method
@{ */

/**
   Application processor accesses the crypto engine using registers.
*/
#define TZ_RESCFG_AP_CE_REGISTER_USE  0
/**
   Application processor accesses the crypto engine using the data mover.
*/
#define TZ_RESCFG_AP_CE_ADM_USE       1

/** @} */ /* end_namegroup Crypto Engine Access Method */

/** @} */ /* end_addtogroup tz_macros_constants */


/**
   @weakgroup weak_tz_config_resource_req_s
@{
*/
typedef struct tz_config_resource_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */

  UINT32               res;         /**< Resource to configure. See
                                         #TZ_XPU_RESOURCE_ET. */

  UINT32               arg;         /**< Argument to the resource
                                         configuration. */
} tz_config_resource_req_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * INFO Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_info_is_svc_available_req_s
@{
*/
typedef struct tz_info_is_svc_available_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */
  UINT32               id;          /**< Command ID of the queried service. */
} tz_info_is_svc_available_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_info_is_svc_available_rsp_s
@{
*/
typedef struct tz_info_is_svc_available_rsp_s
{
  struct tz_syscall_rsp_s  common_rsp;
                                    /**< Common response structure. */
  BOOLEAN              found;       /**< If TRUE, the searched service
                                         was found; FALSE otherwise. */
} tz_info_is_svc_available_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_info_get_diag_req_s
@{
*/
typedef struct tz_info_get_diag_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */
  UINT32               buffer;      /**< Pointer to the buffer to where the
                                         QSEE software copies the
                                         diagnostic area. */
  UINT32               buffer_size; /**< Size of the buffer in bytes.
                                         This must be at least the
                                         size of the diagnostic
                                         area. If the buffer is larger
                                         than the diagnostic area, the
                                         remaining bytes are ignored
                                         by the QSEE software. */
} tz_info_get_diag_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_feature_version_req_s
@{
*/
typedef struct tz_feature_version_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */
  UINT32               feature_id;  /**< Feature for which to request
                                         the version information.
                                         See #tz_feature_t. */
} tz_feature_version_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_feature_version_rsp_s
@{
*/
typedef struct tz_feature_version_rsp_s
{
  struct tz_syscall_rsp_s  common_rsp;
                                    /**< Common response structure. */
  UINT32               version;     /**< Version of the specified
                                         feature, or zero if the
                                         feature is undefined. */
} tz_feature_version_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_get_secure_state_req_s
@{
*/
typedef struct tz_get_secure_state_req_s
{
  tz_syscall_req_t              common_req;  /**< Common request structure. */

  UINT32                        status_ptr;  /**< Pointer to the query status struct. */
  UINT32                        status_len;  /**< Length of the query status struct. */
}tz_get_secure_state_req_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * SSD Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_ssd_decrypt_img_req_s
@{
*/
typedef struct tz_ssd_decrypt_img_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */
  UINT32               img_ptr;     /**< Double pointer to the image. */
  UINT32               img_len_ptr; /**< Pointer to the length of the
                                         image. */
} tz_ssd_decrypt_img_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_encrypt_keystore_req_s
@{
*/
typedef struct tz_ssd_encrypt_keystore_req_s
{
  struct tz_syscall_req_s  common_req;    /**< Common request structure. */
  UINT32               keystore_ptr;      /**< Double pointer to the
                                               keystore. */
  UINT32               keystore_len_ptr;  /**< Pointer to the length
                                               of the keystore. */
} tz_ssd_encrypt_keystore_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_protect_keystore_req_s
@{
*/
typedef struct tz_ssd_protect_keystore_req_s
{
  struct tz_syscall_req_s       common_req;    /**< Common request
                                                    structure. */
  UINT32                        keystore_ptr;  /**< Pointer to SSD keystore. */
  UINT32                        keystore_len;  /**< Length of the keystore. */
} tz_ssd_protect_keystore_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/**
   Return types for the Load Keystore command.
*/
typedef enum
{
  TZ_SSD_PKS_SUCCESS            = 0, /**< Successful return. */
  TZ_SSD_PKS_INVALID_PTR        = 1, /**< Keystore pointer invalid. */
  TZ_SSD_PKS_INVALID_LEN        = 2, /**< Keystore length incorrect. */
  TZ_SSD_PKS_UNALIGNED_PTR      = 3, /**< Keystore pointer not word
                                             aligned. */
  TZ_SSD_PKS_PROTECT_MEM_FAILED = 4, /**< Failure when protecting
                                             the keystore memory.*/
  TZ_SSD_PKS_INVALID_NUM_KEYS   = 5, /**< Unsupported number of
                                          keys passed. If a valid
                                          pointer to nonsecure
                                          memory is passed that
                                          is not a keystore, this is
                                          a likely return code. */
  TZ_SSD_PKS_DECRYPT_FAILED     = 6  /**< Keystore could not be
                                          decrypted. */
} tz_ssd_pks_return_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_ssd_protect_keystore_rsp_s
@{
*/
typedef struct tz_ssd_protect_keystore_rsp_s
{
  struct tz_syscall_rsp_s       common_rsp;   /**< Common response
                                                   structure. */
  UINT32                        status;       /**< Status of the load keystore
                                                   call.  See
                                                   #tz_ssd_pks_return_type.
                                               */
} tz_ssd_protect_keystore_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_parse_md_req_s
@{
*/
typedef struct tz_ssd_parse_md_req_s
{
  struct tz_syscall_req_s       common_req;   /**< Common request structure. */
  UINT32                        md_len;       /**< Length of the metadata. */
  UINT32                        md;           /**< Pointer to the metadata to
                                                   parse. The memory must be
                                                   word-aligned, as the type
                                                   implies. */
} tz_ssd_parse_md_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/**
   Return types for the parse metadata command.
*/
typedef enum
{
  TZ_SSD_PMD_ENCRYPTED             = 0,    /**< Parsing was successful
                                                   and the image is
                                                   encrypted. */
  TZ_SSD_PMD_NOT_ENCRYPTED         = 1,    /**< Parsing was successful
                                                   and the image is not
                                                   encrypted. */
  TZ_SSD_PMD_NO_MD_FOUND           = 3,    /**< No SSD metadata
                                                   start tag was found
                                                   in the given
                                                   memory. This
                                                   proably indicates
                                                   that the image is
                                                   not encrypted. It
                                                   could also indicate
                                                   the start tag was
                                                   truncated. */
  TZ_SSD_PMD_BUSY                  = 4,    /**< Parsing was not attempted
                                                   because no parsing context
                                                   was available at this
                                                   time. */
  TZ_SSD_PMD_BAD_MD_PTR_OR_LEN     = 5,    /**< Could not search the
                                                   memory because the
                                                   pointer or length was
                                                   bad. */
  TZ_SSD_PMD_PARSING_INCOMPLETE    = 6,    /**< The parsed metadata
                                                   is incomplete. Call
                                                   again with a larger
                                                   buffer to scan. */
  TZ_SSD_PMD_PARSING_FAILED        = 7,    /**< There was a parsing
                                                   error in the metadata. */
  TZ_SSD_PMD_SETUP_CIPHER_FAILED   = 8,    /**< There was a problem
                                                   finding the correct key
                                                   in the keystore or
                                                   initializing the crypto
                                                   hardware. */
} tz_ssd_pmd_return_type;

/** @} */ /* end_addtogroup tz_datatypes */


/**
   @weakgroup weak_tz_ssd_parse_md_rsp_info_s
@{
*/
typedef struct tz_ssd_parse_md_rsp_info_s
{
  UINT32                        status;       /**< Status of request. See
                                                   #tz_ssd_pmd_return_type.
                                               */
  UINT32                        md_ctx_id;    /**< Context ID to use for a
                                                   decrypt fragment call. */
  UINT32                        md_end_ptr;   /**< Pointer to the first memory
                                                   after the SSD metadata.
                                                   This pointer is always
                                                   word-aligned. If the
                                                   image was encrypted all at
                                                   once, this is the
                                                   start of the encrypted
                                                   data. */
} tz_ssd_parse_md_rsp_info_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_ssd_parse_md_rsp_s
@{
*/
typedef struct tz_ssd_parse_md_rsp_s
{
  struct tz_syscall_rsp_s       common_rsp;   /**< Common response
                                                   structure. */
  struct tz_ssd_parse_md_rsp_info_s    rsp;   /**< Parse MD response
                                                   structure. */
} tz_ssd_parse_md_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_decrypt_img_frag_req_s
@{
*/
typedef struct tz_ssd_decrypt_img_frag_req_s
{
  struct tz_syscall_req_s       common_req;   /**< Common request structure. */
  UINT32                        md_ctx_id;    /**< Context ID given in the
                                                   parse metadata call. */
  UINT32                        last_frag;    /**< Whether the passed
                                                   fragment is the
                                                   last one (1 = last
                                                   fragment). */
  UINT32                        frag_len;     /**< Length of the
                                                   fragment. The
                                                   fragment length must
                                                   be a multiple of the
                                                   encryption block
                                                   size.  */
  UINT32                        frag;         /**< Encrypted fragment.
                                                   The fragment must
                                                   be word-aligned, as
                                                   noted by the
                                                   UINT32* type. */
} tz_ssd_decrypt_frag_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_decrypt_elf_seg_frag_req_s
@{
*/
typedef struct tz_ssd_decrypt_elf_seg_frag_req_s
{
  struct tz_syscall_req_s       common_req;   /**< Common request structure. */
  UINT32                        md_ctx_id;    /**< Context ID given in the
                                                   parse metadata call. */
  UINT32                        segment_num;  /**< ELF segment number
                                                   for the given
                                                   fragment. Note that
                                                   all the fragments
                                                   for a given ELF
                                                   segment must be
                                                   received before
                                                   going to the next
                                                   segment. */
  UINT32                        last_seg;     /**< Whether the current
                                                   segment is the last
                                                   one (1 = last segment). */
  UINT32                        last_frag;    /**< Whether the passed
                                                   fragment is the last
                                                   one for a given
                                                   segment_num (1 =
                                                   last fragment). */
  UINT32                        frag_len;     /**< Length of the fragment.
                                                   The fragment length
                                                   must be a multiple of the
                                                   encryption block size
                                                   unless the last_frag flag
                                                   is passed.  */
  UINT32                        frag;         /**< Encrypted fragment.
                                                   The fragment must
                                                   be word-aligned, as
                                                   noted by the
                                                   UINT32* type. */
} tz_ssd_decrypt_elf_seg_frag_req_t;

/** @} */ /* end_weakgroup */



/** @addtogroup tz_datatypes
@{ */

/**
   Return types for the Decrypt (image/ELF segment) Fragment command.
*/
typedef enum
{
  TZ_SSD_DF_SUCCESS               = 0,     /**< Fragment decrypted
                                                   successfully. */
  TZ_SSD_DF_BAD_FRAG_PTR_OR_LEN   = 1,     /**< Fragment pointer or
                                                   length incorrect. */
  TZ_SSD_DF_CTX_NOT_FOUND         = 2,     /**< Fragment context
                                                   not found in
                                                   currently available
                                                   contexts. */
  TZ_SSD_DF_LEN_ALIGNMENT_FAILED  = 3,     /**< Length alignment of the
                                                   fragment is incorrect. */
  TZ_SSD_DF_DECRYPTION_FAILED     = 4      /**< Problem with decryption. */
} tz_ssd_df_return_type;

/** @} */ /* end_addtogroup tz_datatypes */


/**
   @weakgroup weak_tz_ssd_decrypt_frag_rsp_s
@{
*/
typedef struct tz_ssd_decrypt_frag_rsp_s
{
  struct tz_syscall_rsp_s       common_req;   /**< Common response
                                                   structure. */
  UINT32                        status;       /**< Status of the decrypt
                                                   request.  See
                                                   #tz_ssd_df_return_type.
                                               */
} tz_ssd_decrypt_frag_rsp_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * FUSE Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_sw_fuse_req_s
@{
*/
typedef struct tz_sw_fuse_req_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */
  UINT32               fuse_id;     /**< Fuse ID. See
                                         #tz_sw_fuse_t. */
} tz_sw_fuse_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_sw_fuse_rsp_s
@{
*/
typedef struct tz_sw_fuse_rsp_s
{
  struct tz_syscall_rsp_s  common_rsp;
                                    /**< Common response structure. */
  BOOLEAN              fuse_blown;  /**< If TRUE, the specified fuse
                                         was blown; FALSE
                                         otherwise. */
} tz_sw_fuse_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_write_row_s
@{
*/
typedef struct tz_qfprom_write_row_s
{
  struct tz_syscall_req_s  common_req;     /**< Common request structure. */
  UINT32               raw_row_address;    /**< Raw row address of the fuse
                                                to blow. */
  UINT32               row_data;           /**< Pointer to the data buffer. */
  UINT32               bus_clk_khz;        /**< Bus clock speed in KHz. */
  UINT32               qfprom_api_status;  /**< Status code from the QFPROM. */
} tz_qfprom_write_row_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_write_multiple_rows_s
@{
*/
typedef struct tz_qfprom_write_multiple_rows_s
{
  struct tz_syscall_req_s  common_req;     /**< Common request structure. */
  UINT32               row_array;          /**< Row array. */
  UINT32               row_array_size;     /**< Size of the row array. */
  UINT32               bus_clk_khz;        /**< Bus clock in KHz. */
  UINT32               qfprom_api_status;  /**< Status code from the QFPROM. */
} tz_qfprom_write_multiple_rows_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_read_row_s
@{
*/
typedef struct tz_qfprom_read_row_s
{
  struct tz_syscall_req_s  common_req;     /**< Common request structure. */
  UINT32               row_address;        /**< Row address of the fuse to
                                                read. */
  INT32                addr_type;          /**< Indicates whether the address
                                                is raw corrected. */
  UINT32               row_data;           /**< Pointer to the buffer that is
                                                to hold the read data. */
  UINT32               qfprom_api_status;  /**< Status code from the QFPROM. */
} tz_qfprom_read_row_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_rollback_write_row_s
@{
*/
typedef struct tz_qfprom_rollback_write_row_s
{
  struct tz_syscall_req_s  common_req;     /**< Common request structure. */
  UINT32               raw_row_address;    /**< Raw row address of the fuse
                                                to blow. */
  UINT32               row_data;           /**< Pointer to the data buffer. */
  UINT32               bus_clk_khz;        /**< Bus clock in KHz. */
  UINT32               qfprom_api_status;  /**< Status code from the QFPROM. */
} tz_qfprom_rollback_write_row_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * CRYPTO Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_get_prng_data_s
@{
*/
typedef struct tz_get_prng_data_s
{
  struct tz_syscall_req_s   common_req;
                                     /**< Common request structure. */
  UINT32                out_buf;     /**< Pointer to the buffer to fill
                                          with data from the PRNG. */
  UINT32                out_buf_sz;  /**< Size of the output buffer. */
} tz_get_prng_data_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * SC_KEY_EXCHANGE Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_sc_init_s
@{
*/
typedef struct tz_sc_init_s
{
  struct tz_syscall_req_s   common_req;
                                     /**< Common request structure. */
  UINT32                ssid;        /**< Subsystem ID. See
                                          #tz_sc_ss_e_type. */
  UINT32                address_ptr; /**< Pointer to the address location. */
  UINT32                len;         /**< Length of the allocated memory. */
  int                   status_ptr;  /**< Return status. */
} tz_sc_init_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_sc_deinit_s
@{
*/
typedef struct tz_sc_deinit_s
{
  struct tz_syscall_req_s  common_req;
                                      /**< Common request structure. */
  UINT32               ssid;          /**< Subsystem ID. */
  int                  status_ptr;    /**< Return status. */
} tz_sc_deinit_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * MEMORY_PROTECTION Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_protect_memory_s
@{
*/
typedef struct tz_protect_memory_s
{
  struct tz_syscall_req_s  common_req;
                                    /**< Common request structure. */
  UINT32               start;       /**< Start address of the range. */
  UINT32               end;         /**< End address of the range. */
  UINT32               tag_id;      /**< Tag to determine the
                                         permissions. */
  BOOLEAN              lock;        /**< TRUE to protect; FALSE
                                         to unprotect. */
} tz_protect_memory_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * Memory protection service structures
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_datatypes
@{ */

/**
  * Memory protection-related device types.
  */
typedef enum
{
  TZ_DEVICE_VIDEO        =  0,      /**< Video subsystem. */
  TZ_DEVICE_MDSS         =  1,      /**< MDSS subsystem. */
  TZ_DEVICE_LPASS        =  2,      /**< Low-power audio subsystem. */
  TZ_DEVICE_MDSS_BOOT    =  3,      /**< MDSS subsystem at cold boot. */
  TZ_DEVICE_USB1_HS      =  4,      /**< High speed USB. */
  TZ_DEVICE_OCMEM        =  5,      /**< OCMEM registers. @newpage */
  TZ_DEVICE_LPASS_CORE   =  6,
  TZ_DEVICE_VPU          =  7,
  TZ_DEVICE_COPSS_SMMU   =  8,
  TZ_DEVICE_USB3_0       =  9,
  TZ_DEVICE_USB3_1       = 10,
  TZ_DEVICE_PCIE_0       = 11,
  TZ_DEVICE_PCIE_1       = 12,
  TZ_DEVICE_BCSS         = 13,
  TZ_DEVICE_VCAP         = 14,
  TZ_DEVICE_PCIE20       = 15,
  TZ_DEVICE_IPA          = 16,
  TZ_DEVICE_APPS         = 17,
  TZ_DEVICE_GPU          = 18,
  TZ_DEVICE_COUNT,
  TZ_DEVICE_MAX = 0x7FFFFFFF,
}tz_device_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_restore_sec_cfg_req_s
@{
*/

typedef struct tz_restore_sec_cfg_req_s
{
  tz_syscall_req_t           common_req;  /**< Common request structure. */
  UINT32                        device;
                                          /**< Device to be restored;
                                               see #tz_device_e_type. */
  UINT32                        spare;       /**< Spare. */
} tz_restore_sec_cfg_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_set_vmidmt_memtype_req_s
@{
*/
typedef struct tz_set_vmidmt_memtype_req_s
{
  tz_syscall_req_t              common_req;  /**< Common request structure. */
  UINT32                        device;
                                             /**< Device to be configured;
                                               see #tz_device_e_type. */
  UINT32                        vmid_idx;    /**< Index for the memtype to be set;
                                               0xFF for all indecies. */
  UINT32                        memtype;     /**< Memory type value to be set. */
} tz_set_vmidmt_memtype_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_video_var_req_s
@{
*/
typedef struct tz_memprot_video_var_req_s
{
  tz_syscall_req_t           common_req;
    /**< Common request structure. */
  UINT32                     cp_start;
    /**< Start address of the secure memory (CP) range in the device's virtual
         address space. */
  UINT32                     cp_size;
    /**< Size of the secure memory range (CP) in the device's virtual address
         space. The end address is calculated as: cp_end = [cp_start +
         cp_size - 1]. */
  UINT32                     cp_nonpixel_start;
    /**< Start address of nonpixel data in the device's virtual address space.
         Set cp_start < cp_nonpixel_start < cp_end cp_nonpixel so that the
         range does not overlap with the firmware image.*/
  UINT32                     cp_nonpixel_size;
    /**< Size of nonpixel data in the device's virtual address space. The end
         address is calculated as: cp_nonpixel_end = [cp_nonpixel_start +
         cp_nonpixel_size - 1]. Also, set cp_nonpixel_start < cp_nonpixel_end
         @le cp_end cp_nonpixel so that the range does not overlap with the
         firmware image. */
} tz_memprot_video_var_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_dev_var_req_s
@{
*/
typedef struct tz_memprot_dev_var_req_s
{
  tz_syscall_req_t           common_req;
    /**< Common request structure. */
  UINT32                     device_id;
  /**< ID of the device where this memory is to be mapped.
       See #tz_device_e_type. */
  tz_va_range_t              *va_list;
    /**< a list of start address and size of the secure memory ranges in the device's
         virtual address space. */
  UINT32                     list_size;
    /**< Size of VA range list. */
} tz_memprot_dev_var_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_size_req_s
@{
*/
typedef struct tz_memprot_ptbl_mem_size_req_s
{
   tz_syscall_req_t              common_req; /**< Common request structure. */
   UINT32                        spare;      /**< Spare field for future use,
                                                  must be zero. */
} tz_memprot_ptbl_mem_size_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_size_rsp_s
@{
*/
typedef struct tz_memprot_ptbl_mem_size_rsp_s
{
  tz_syscall_rsp_t             common_rsp;  /**< Response header. */
  UINT32                       mem_size;    /**< Required size of the
                                                 page table memory
                                                 region in bytes. */
  INT32                        ret;         /**< Extended return code; 0 for
                                                 success; negative values
                                                 mean failure. */
} tz_memprot_ptbl_mem_size_rsp_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_init_req_s
@{
*/
typedef struct tz_memprot_ptbl_mem_init_req_s
{
  tz_syscall_req_t              common_req; /**< Common request structure. */
  UINT32                        addr;       /**< Start address of the page
                                                 table memory region
                                                 (16 KB aligned).  */
  UINT32                        size;       /**< Size of page table memory
                                                 region in bytes. */
  UINT32                        spare;      /**< Spare field for future use;
                                                 must be zero. */
} tz_memprot_ptbl_mem_init_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_set_cp_pool_size_req_s
@{
*/
typedef struct tz_memprot_set_cp_pool_size_req_s
{
   tz_syscall_req_t              common_req; /**< Common request structure. */
   UINT32                        size;       /**< CP pool size */
   UINT32                        spare;      /**< Spare field for future use,
                                                  must be zero. */
} tz_memprot_set_cp_pool_size_req_t;

/** @} */ /* end_weakgroup */


/** @addtogroup tz_datatypes
@{ */

/** Enumerates memory usage. */
typedef enum
{
  TZ_MEM_USAGE_CP_VIDEO_BITSTREAM  = 0x1, /**< Memory used for video
                                                  bistream (compressed)
                                                  CP data. */
  TZ_MEM_USAGE_CP_VIDEO_PIXEL      = 0x2, /**< Memory used for video
                                                  pixel (uncompressed)
                                                  CP data. */
  TZ_MEM_USAGE_CP_VIDEO_NONPIXEL   = 0x3, /**< Memory used for video
                                                  nonpixel CP data. */
  TZ_MEM_USAGE_COUNT,
  TZ_MEM_USAGE_UNKNOWN  = 0x7fffffff
}tz_mem_usage_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_memprot_chunks_info_s
@{
*/
typedef struct tz_memprot_chunks_info_s
{
  UINT32              chunk_list;     /**< Array of chunk addresses. Each entry
                                           is a 32-bit physical address.
                                           chunk_list_size gives the number of
                                           entries in the array. */
  UINT32              chunk_list_size;/**< Number of chunks in the chunk
                                           list.*/
  UINT32              chunk_size;     /**< Size of each memory chunk (in bytes)
                                           in the chunk list. */
} tz_memprot_chunks_info_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_map_info_s
@{
*/
typedef struct tz_memprot_map_info_s
{
  UINT32              device_id;    /**< ID of the device where this memory is
                                         to be mapped. See #tz_device_e_type.
                                         The usage hint provided while
                                         protecting the memory can be used to
                                         validate this request. */
  UINT32              ctxt_bank_id; /**< Context bank ID where this memory
                                         is to be (un)mapped. These
                                         correspond to context bank numbers
                                         and are unique only for the device.
                                         Values start from 0, which corresponds
                                         to CB0. */
  UINT32              dva_start;    /**< Start address in the device's virtual
                                         address space to which to map. */
  UINT32              size;         /**< Size in bytes of the virtual address
                                         range to map. The end address is:
                                         dva_end = dva_start + size - 1. */
} tz_memprot_map_info_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_macros_constants
@{ */


/** @name Memory Protection Method Flags
@{ */

/**
   TLB invalidate as part of a map/unmap operation.
*/
#define TZ_MEMPROT_FLAG_TLBINVALIDATE       0x00000001

/** @} */ /* end_namegroup Memory Protection Method Flags */

/** @} */ /* end_addtogroup tz_macros_constants */

/**
   @weakgroup weak_tz_memprot_lock2_req_s
@{
*/
typedef struct tz_memprot_lock2_req_s
{
  tz_syscall_req_t              common_req;/**< Common request structure. */
  tz_memprot_chunks_info_t      chunks;    /**< Memory chunk list. */
  UINT32                        mem_usage; /**< Hint on the memory usage.
                                                See #tz_mem_usage_e_type. */
  UINT32                        lock;      /**< 1 (TRUE) to lock (protect);
                                                0 (FALSE) to unlock (unprotect). */
  UINT32                        flags;     /**< Flags field for future use;
                                                must be zero when no flags are
                                                set. */
} tz_memprot_lock2_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_map2_req_s
@{
*/
typedef struct tz_memprot_map2_req_s
{
  tz_syscall_req_t               common_req;/**< Common request structure. */
  tz_memprot_chunks_info_t       chunks;    /**< Memory chunk list. */
  tz_memprot_map_info_t          map;       /**< Map-to information. */
  UINT32                         flags;     /**< Flags field, which is a bitwise
                                                 OR of TZ_MEMPROT_FLAG_* flags.
                                                 Currently supported flags:
                                                 TLBINVALIDATE \n
                                                 Must be zero when no flags are
                                                 set. */
} tz_memprot_map2_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_unmap2_req_s
@{ */
typedef struct tz_memprot_unmap2_req_s
{
  tz_syscall_req_t              common_req; /**< Common request structure. */
  tz_memprot_map_info_t         map;        /**< Mapped-at information. */
  UINT32                        flags;      /**< Flags field, which is a bitwise
                                                 OR of TZ_MEMPROT_FLAG_* flags.
                                                 Currently supported flags:
                                                 TLBINVALIDATE \n
                                                 Must be zero when no flags are
                                                 set. */
} tz_memprot_unmap2_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/**
 Common response type for QSEE blocking kernel layer system calls.
 Individual system call headers indicate whether they use this response
 structure.
*/
typedef struct tz_common_rsp_s
{
  tz_syscall_rsp_t             common_rsp; /**< Response header. */
  INT32                        ret;        /**< Extended return
                                                code; 0 for success;
                                                negative values
                                                mean failure. */
} tz_common_rsp_t;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_memprot_tlbinval_req_s
@{ */
typedef struct tz_memprot_tlbinval_req_s
{
  tz_syscall_req_t      common_req;   /**< Common request structure. */
  UINT32                device_id;    /**< ID of the device that is to have its
                                           TLB invalidated.
                                           See #tz_device_e_type. */
  UINT32                ctxt_bank_id; /**< Context bank ID that is to have its
                                           TLB invalidated. These correspond to
                                           context bank numbers and are unique
                                           only for the device. Values start
                                           from 0, which corresponds to CB0. */
  UINT32                flags;        /**< Flags field for future use;
                                           must be zero when no flags are set. */
  UINT32                spare;        /**< Spare field for future use;
                                           must be zero. */
} tz_memprot_tlbinval_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/** Configure whether xPU violations are to be error fatal. */
typedef enum
{
  TZ_XPU_VIOLATION_ERR_FATAL_ENABLE  = 0, /**< xPU violations are error fatal. */
  TZ_XPU_VIOLATION_ERR_FATAL_DISABLE = 1, /**< xPU violations are not error
                                               fatal. */
  TZ_XPU_VIOLATION_ERR_FATAL_NOOP    = 2, /**< No operation. */
} tz_xpu_violation_err_fatal_t;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_xpu_violation_err_fatal_req_s
@{
*/
typedef struct tz_xpu_violation_err_fatal_req_s
{
  tz_syscall_req_t                   common_req; /**< Common request structure. */
  tz_xpu_violation_err_fatal_t       config;     /**< Enable, disable, or NOOP.
                                                      See tz_xpu_violation_err_fatal_t. */
  UINT32                             spare;      /**< Unused. */
} tz_xpu_violation_err_fatal_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_xpu_violation_err_fatal_rsp_s
@{
*/
typedef struct tz_xpu_violation_err_fatal_rsp_s
{
  struct tz_syscall_rsp_s     common_rsp;
                                     /**< Common response structure. */
  tz_xpu_violation_err_fatal_t status   ;     /**< Enable or disable. */
} tz_xpu_violation_err_fatal_rsp_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * OCMEM commands
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_datatypes
@{ */

/** Enumerates OCMEM client IDs. */
typedef enum
{
  TZ_OCMEM_CLNT_UNUSED,       /**< Default client when unused. */
  TZ_OCMEM_CLNT_GRAPHICS,     /**< Maps to Oxili Virtual Machine ID (VMID). */
  TZ_OCMEM_CLNT_VIDEO,        /**< Maps to Venus CP_VMID. */
  TZ_OCMEM_CLNT_LP_AUDIO,     /**< Maps to ADSP_VMID. */
  TZ_OCMEM_CLNT_SENSORS,      /**< Maps to ADSP_VMID. */
  TZ_OCMEM_CLNT_BLAST,        /**< Maps to ADSP_VMID. */
  TZ_OCMEM_CLNT_DEBUG,        /**< Maps to HLOS_VMID; for debugging only. */
}tz_ocmem_client_e_type;

/** Enumerates the OCMEM operation mode. */
typedef enum
{
  TZ_OCMEM_OP_MODE_WIDE,      /**< Wide mode. */
  TZ_OCMEM_OP_MODE_THIN,      /**< Thin mode. */
  TZ_OCMEM_OP_MODE_NOT_SET,   /**< Operation mode not set. */
}tz_ocmem_op_mode_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_ocmem_lock_region_req_s
@{
*/
typedef struct tz_ocmem_lock_region_req_s
{
  tz_syscall_req_t              common_req;  /**< Common request structure. */

  UINT32                        client_id;   /**< OCMEM client requesting the
                                                  memory region. See
                                                  #tz_ocmem_client_e_type. */

  UINT32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  UINT32                        size;        /**< Size of the requested region. */

  UINT32                        mode;        /**< Requested mode (wide or
                                                  narrow). See
                                                  #tz_ocmem_op_mode_e_type. */
}tz_ocmem_lock_region_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_unlock_region_req_s
@{
*/
typedef struct tz_ocmem_unlock_region_req_s
{
  tz_syscall_req_t              common_req;  /**< Common request structure. */

  UINT32                        client_id;   /**< OCMEM client that currently
                                                  owns the region. See
                                                  #tz_ocmem_client_e_type. */

  UINT32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  UINT32                        size;        /**< Size of requested region. */
}tz_ocmem_unlock_region_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_enable_mem_dump_req_s
@{
*/
typedef struct tz_ocmem_enable_mem_dump_req_s
{
  tz_syscall_req_t              common_req;  /**< Common request structure. */

  UINT32                        client_id;   /**< OCMEM client that currently
                                                  has this region locked. See
                                                  #tz_ocmem_client_e_type. */

  UINT32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  UINT32                        size;        /**< Size of the region. */
}tz_ocmem_enable_mem_dump_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_disable_mem_dump_req_s
@{
*/
typedef struct tz_ocmem_disable_mem_dump_req_s
{
  tz_syscall_req_t              common_req;  /**< Common request structure. */

  UINT32                        client_id;   /**< OCMEM client that currently
                                                  has this region locked. See
                                                  #tz_ocmem_client_e_type */

  UINT32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  UINT32                        size;        /**< Size of the region. */
}tz_ocmem_disable_mem_dump_req_t;

/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * NS_FIQ_DEBUG Service Structures
 * -------------------------------------------------------------------------*/

/** @addtogroup tz_macros_constants
@{ */

/** @name Nonsecure Debug FIQ Flags
@{ */

/** Nonsecure debug FIQ is level triggered. */
#define TZ_NS_DEBUG_FIQ_LEVEL_TRIGGERED              0x0
/** Nonsecure debug FIQ is edge triggered. */
#define TZ_NS_DEBUG_FIQ_EDGE_TRIGGERED               0x1
/** Nonsecure debug FIQ handler runs in the Thumb state. */
#define TZ_NS_DEBUG_FIQ_THUMB                        0x2

/** @} */ /* end_namegroup Nonsecure Debug FIQ Flags */

/** @} */ /* end_addtogroup tz_macros_constants */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_req_s
@{
*/
typedef struct tz_config_ns_debug_fiq_req_s
{
  struct tz_syscall_req_s  common_req; /**< Common request structure. */
  UINT32  vector; /**< Base address of the nonsecure FIQ handler. The
                             handler executes in nonsecure SVC mode. */
  UINT32  stack; /**< Stack for the nonsecure FIQ handler. The
                            QSEE software never modifies the stack. */
  UINT32  ctx_buf; /**< Context buffer for the nonsecure FIQ handler. Passed
                        in to the handler via register r0. The TrustZone
                        software always modifies the entire context. */
  UINT32  ctx_size; /**< Context buffer size in bytes. */
  UINT32 intnum; /**< Number of the debug interrupt on the Qualcomm Generic
                      Interrupt Controller (QGIC). The QSEE software only
                      allows using designated interrupts as debug FIQs and
                      returns an error code if the interrupt is not one of
                      the supported ones. */
  UINT32 flags; /**< Configuration flags; a bitwise OR of the
                     TZ_NS_FIQ_DEBUG_* definitions. The handler is assumed
                     to execute in ARM mode. For Thumb mode handlers, flag
                     TZ_NS_FIQ_DEBUG_THUMB must be set. */
} tz_config_ns_debug_fiq_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_ctx_size_rsp_s
@{
*/
typedef struct tz_config_ns_debug_fiq_ctx_size_rsp_s
{
  struct tz_syscall_rsp_s common_rsp; /**< Common response structure */
  UINT32 size; /**< Context size in bytes. */
} tz_config_ns_debug_fiq_ctx_size_rsp_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_int_ok_req_s
@{
*/
typedef struct tz_config_ns_debug_fiq_int_ok_req_s
{
  struct tz_syscall_req_s common_req; /**< Common request structure */
  UINT32 intnum; /**< QGIC interrupt number. */
} tz_config_ns_debug_fiq_int_ok_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_int_ok_rsp_s
@{
*/
typedef struct tz_config_ns_debug_fiq_int_ok_rsp_s
{
  struct tz_syscall_rsp_s common_rsp; /**< Common response structure. */
  UINT32 verdict; /**< Contains 1 if the interrupt is supported as an FIQ
                       debug interrupt; 0 otherwise. */
} tz_config_ns_debug_fiq_int_ok_rsp_t;
/** @} */ /* end_weakgroup */

/*----------------------------------------------------------------------------
 * CPU Configuration Service
 * -------------------------------------------------------------------------*/

/**
   @weakgroup weak_tz_cpu_config_req_s
@{
*/
typedef struct tz_cpu_config_req_s
{
  tz_syscall_req_t              common_req; /**< Common request structure */
  UINT32                        id;   /**< ID of the configured item. */
  UINT32                        arg0; /**< Generic argument 0. */
  UINT32                        arg1; /**< Generic argument 1. */
  UINT32                        arg2; /**< Generic argument 2. */
} tz_cpu_config_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_cpu_config_query_req_s
@{
*/
typedef struct tz_cpu_config_query_req_s
{
  tz_syscall_req_t              common_req; /**< Common request structure */
  UINT32                        id;   /**< ID of the queried item. */
  UINT32                        arg0; /**< Generic argument 0. */
  UINT32                        arg1; /**< Generic argument 1. */
  UINT32                        arg2; /**< Generic argument 2. */
} tz_cpu_config_query_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_cpu_config_query_rsp_s
@{
*/
typedef struct tz_cpu_config_query_rsp_s
{
  tz_syscall_rsp_t              common_rsp; /**< Common response structure */
  UINT32                        ret0; /**< Generic return value 0. */
  UINT32                        ret1; /**< Generic return value 1. */
  UINT32                        ret2; /**< Generic return value 2. */
  UINT32                        ret3; /**< Generic return value 3. */
} tz_cpu_config_query_rsp_t;
/** @} */ /* end_weakgroup */

#pragma pack(pop) /* Packed structures till here */
#endif
