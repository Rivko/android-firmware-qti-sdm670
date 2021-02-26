#ifndef TZ_SYSCALL_PUB_H
#define TZ_SYSCALL_PUB_H

/**
   @file tz_syscall_pub.h
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
   Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
06/18/16   rj       Added register log buffer command
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

/*----------------------------------------------------------------------------
 * Documentation
 * -------------------------------------------------------------------------*/

/*   ARMV8 SMC ID bit layout
 ________________________________________________
|    |     |        |          |                 |
| 31 |  30 |29----24|23------16|15--------------0|
|IRQ |AARCH|  Owner | Reserved |    Command ID   |
|____|_____|________|__________|_________________|

*/

/*   Parameter ID bit layout
 ___________________________________________________________________________
|          |     |     |     |     |     |     |     |    |    |    |       |
|31------24|23-22|21-20|19-18|17-16|15-14|13-12|11-10|9--8|7--6|5--4|3-----0|
| reserved | p10 | p9  | p8  | p7  | p6  | p5  | p4  | p3 | p2 | p1 | nargs |
|          |type |type |type |type |type |type |type |type|type|type|       |
|__________|_____|_____|_____|_____|_____|_____|_____|____|____|____|_______|

*/

/** @addtogroup tz_macros_constants
@{ */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/** @name Helper Macros
@{ */
/** General helper macro to create a bitmask from bits low to high. */
#define TZ_MASK_BITS(h,l)     ((0xffffffff >> (32 - ((h - l) + 1))) << l)

/** Helper macro to extract the IRQ settings from SMC ID. */
#define TZ_SYSCALL_IRQ(r0)        ((r0 & TZ_MASK_BITS(31,31)) >> 31)
/** Helper macro to extract the AArch used from SMC ID. */
#define TZ_SYSCALL_AARCH(r0)      ((r0 & TZ_MASK_BITS(30,30)) >> 30)
/** Helper macro to extract the Owning entity from SMC ID. */
#define TZ_SYSCALL_OWNER_ID(r0)   ((r0 & TZ_MASK_BITS(29,24)) >> 24)
/** Helper macro to extract the reserved bits from SMC ID. */
#define TZ_SYSCALL_RESERVED(r0)   ((r0 & TZ_MASK_BITS(23,16)) >> 16)
/** Helper macro to extract the function ID from SMC ID. */
#define TZ_SYSCALL_FUNC_ID(r0)    (r0 & TZ_MASK_BITS(15,0))
/** Helper macro to extract the app ID from SMC ID */
#define TZ_SYSCALL_APP_ID(r0)     ((r0 & TZ_MASK_BITS(15,8)) >> 8)
/** Helper macro to extract SVC ID from function id in SMC ID */
#define TZ_SYSCALL_SVC_ID(r0)     ((r0 & TZ_MASK_BITS(15,8)) >> 8)

/** Helper macro to extract the number of arguments from Param ID */
#define TZ_SYSCALL_NUM_ARGS(r1)   (r1 & TZ_MASK_BITS(3,0))
/** Helper macro to extract an arg type from Param ID. 10 args max */
#define TZ_SYSCALL_ARG_TYPE(r1, n) \
  ((n<10)?((r1 & TZ_MASK_BITS(((n*2)+5),((n*2)+4))) >> ((n * 2) + 4)):0xFFFFFFFF)

/** Helper macro for checking if an owning entity is of type Trusted OS */
#define IS_OWNER_TRUSTED_OS(owner_id) \
  (((owner_id >= 50) && (owner_id <= 63))?TRUE:FALSE)
/** Helper macro for checking if an owning entity is of type TZ APPS */
#define IS_OWNER_TZ_APPS(owner_id) \
  (((owner_id >= 48) && (owner_id <= 49))?TRUE:FALSE)

/* TODO Remove */
/** Helper macro to extract the caller ID from R0. */
#define TZ_SYSCALL_CALLID(r0) ((r0 & TZ_MASK_BITS(31,12)) >> 12)
/** Helper macro to extract the class ID from R0. */
#define TZ_SYSCALL_CLASS(r0)  ((r0 & TZ_MASK_BITS(11,8)) >> 8)
/** Helper macro to extract the IRQ settings from R0. */
#define TZ_SYSCALL_ARG(r0)    (r0 & TZ_MASK_BITS(3,0))

/**
   Macro used to define an SMC ID based on the owner ID,
   service ID, and function number.
*/
#define TZ_SYSCALL_CREATE_SMC_ID(o, s, f) \
  ((UINT32)((((o & 0x3f) << 24 ) | (s & 0xff) << 8) | (f & 0xff)))


#define TZ_SYSCALL_PARAM_NARGS_MASK  TZ_MASK_BITS(3,0)
#define TZ_SYSCALL_PARAM_TYPE_MASK   TZ_MASK_BITS(1,0)

#define TZ_SYSCALL_CREATE_PARAM_ID(nargs, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) \
  ((nargs&TZ_SYSCALL_PARAM_NARGS_MASK)+ \
  ((p1&TZ_SYSCALL_PARAM_TYPE_MASK)<<4)+ \
  ((p2&TZ_SYSCALL_PARAM_TYPE_MASK)<<6)+ \
  ((p3&TZ_SYSCALL_PARAM_TYPE_MASK)<<8)+ \
  ((p4&TZ_SYSCALL_PARAM_TYPE_MASK)<<10)+ \
  ((p5&TZ_SYSCALL_PARAM_TYPE_MASK)<<12)+ \
  ((p6&TZ_SYSCALL_PARAM_TYPE_MASK)<<14)+ \
  ((p7&TZ_SYSCALL_PARAM_TYPE_MASK)<<16)+ \
  ((p8&TZ_SYSCALL_PARAM_TYPE_MASK)<<18)+ \
  ((p9&TZ_SYSCALL_PARAM_TYPE_MASK)<<20)+ \
  ((p10&TZ_SYSCALL_PARAM_TYPE_MASK)<<22))

/**
   Macros used to create the Parameter ID associated with the syscall
 */
#define TZ_SYSCALL_CREATE_PARAM_ID_0 0
#define TZ_SYSCALL_CREATE_PARAM_ID_1(p1) \
  TZ_SYSCALL_CREATE_PARAM_ID(1, p1, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_2(p1, p2) \
  TZ_SYSCALL_CREATE_PARAM_ID(2, p1, p2, 0, 0, 0, 0, 0, 0, 0, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_3(p1, p2, p3) \
  TZ_SYSCALL_CREATE_PARAM_ID(3, p1, p2, p3, 0, 0, 0, 0, 0, 0, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_4(p1, p2, p3, p4) \
  TZ_SYSCALL_CREATE_PARAM_ID(4, p1, p2, p3, p4, 0, 0, 0, 0, 0, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_5(p1, p2, p3, p4, p5) \
  TZ_SYSCALL_CREATE_PARAM_ID(5, p1, p2, p3, p4, p5, 0, 0, 0, 0, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_6(p1, p2, p3, p4, p5, p6) \
  TZ_SYSCALL_CREATE_PARAM_ID(6, p1, p2, p3, p4, p5, p6, 0, 0, 0, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_7(p1, p2, p3, p4, p5, p6, p7) \
  TZ_SYSCALL_CREATE_PARAM_ID(7, p1, p2, p3, p4, p5, p6, p7, 0, 0, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_8(p1, p2, p3, p4, p5, p6, p7, p8) \
  TZ_SYSCALL_CREATE_PARAM_ID(8, p1, p2, p3, p4, p5, p6, p7, p8, 0, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_9(p1, p2, p3, p4, p5, p6, p7, p8, p9) \
  TZ_SYSCALL_CREATE_PARAM_ID(9, p1, p2, p3, p4, p5, p6, p7, p8, p9, 0)
#define TZ_SYSCALL_CREATE_PARAM_ID_10(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) \
  TZ_SYSCALL_CREATE_PARAM_ID(10, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
   Macro used to obtain the Parameter ID associated with the syscall
 */
#define TZ_SYSCALL_GET_PARAM_ID(CMD_ID)   CMD_ID ## _PARAM_ID

/** @} */ /* end_namegroup Helper Macros */

/** @name Parameter ID values
@{ */

/** A parameter of type value */
#define TZ_SYSCALL_PARAM_TYPE_VAL              0x0
/** A parameter of type buffer read-only */
#define TZ_SYSCALL_PARAM_TYPE_BUF_RO           0x1
/** A parameter of type buffer read-write */
#define TZ_SYSCALL_PARAM_TYPE_BUF_RW           0x2

/** @} */ /* end_namegroup Parameter ID values */

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
 * Owning Entity IDs (defined by ARM SMC doc)
 * -------------------------------------------------------------------------*/

/** @name Owning Entity IDs
 @{ */

/** ARM Architecture call ID */
#define TZ_OWNER_ARM                     0
/** CPU service call ID */
#define TZ_OWNER_CPU                     1
/** SIP service call ID */
#define TZ_OWNER_SIP                     2
/** OEM service call ID */
#define TZ_OWNER_OEM                     3
/** Standard service call ID */
#define TZ_OWNER_STD                     4

/** Values 5-47 are reserved for future use */

/** Trusted Application call IDs */
#define TZ_OWNER_TZ_APPS                 48
#define TZ_OWNER_TZ_APPS_RESERVED        49
/** Trusted OS Call IDs */
#define TZ_OWNER_QSEE_OS                 50
#define TZ_OWNER_MOBI_OS                 51
#define TZ_OWNER_OS_RESERVED_3           52
#define TZ_OWNER_OS_RESERVED_4           53
#define TZ_OWNER_OS_RESERVED_5           54
#define TZ_OWNER_OS_RESERVED_6           55
#define TZ_OWNER_OS_RESERVED_7           56
#define TZ_OWNER_OS_RESERVED_8           57
#define TZ_OWNER_OS_RESERVED_9           58
#define TZ_OWNER_OS_RESERVED_10          59
#define TZ_OWNER_OS_RESERVED_11          60
#define TZ_OWNER_OS_RESERVED_12          61
#define TZ_OWNER_OS_RESERVED_13          62
#define TZ_OWNER_OS_RESERVED_14          63

/** @} */ /* end_namegroup Owning Entity IDs */

/*----------------------------------------------------------------------------
 * Service IDs
 * -------------------------------------------------------------------------*/

 /** @name Service IDs
 @{ */

#define TZ_SVC_INFO_CALL_ID   255

/** ARM Architecture call groups */
#define TZ_SVC_ARM_INFO           TZ_SVC_INFO_CALL_ID /* ARM syscall info   */

/** CPU service call groups */
#define TZ_SVC_CPU_INFO           TZ_SVC_INFO_CALL_ID /* CPU syscall info   */

/** SIP service call groups */
#define TZ_SVC_BOOT               1       /* Boot (cold boot/warm boot).    */
#define TZ_SVC_PIL                2       /* Peripheral image loading.      */
#define TZ_SVC_DUMP               3       /* Memory dumping.                */
#define TZ_SVC_RES_LOCKING        4       /* Resource locking.              */
#define TZ_SVC_IO_ACCESS          5       /* I/O protection.                */
#define TZ_SVC_INFO               6       /* Misc. information services.    */
#define TZ_SVC_SSD                7       /* SSD services.                  */
#define TZ_SVC_FUSE               8       /* Fuse services.                 */
#define TZ_SVC_POWER              9       /* Power related services.        */
#define TZ_SVC_CRYPTO             10      /* Crypto services.               */
#define TZ_SVC_SC_KEY_EXCHANGE    11      /* Secure channel key exchange.   */
#define TZ_SVC_MEMORY_PROTECTION  12      /* Memory protection service.     */
#define TZ_SVC_RESERVED_0         13      /* Reserved service ID.           */
#define TZ_SVC_NS_DEBUG_FIQ       14      /* Nonsecure debug FIQ.           */
#define TZ_SVC_OCMEM              15      /* OCMEM service.                 */
#define TZ_SVC_ES                 16      /* Enterprise-Security service    */
#define TZ_SVC_HDCP               17      /* HDCP service.                  */
#define TZ_SVC_TEST_1             253     /* TZT calls (continued)          */
#define TZ_SVC_TEST_0             254     /* TZT calls                      */
#define TZ_SVC_SIP_INFO           TZ_SVC_INFO_CALL_ID /* SIP syscall info   */

/** OEM service call groups */
#define TZ_SVC_OEM_INFO           TZ_SVC_INFO_CALL_ID /* OEM syscall info   */

/** Standards call groups */
#define TZ_SVC_STANDARD_INFO      TZ_SVC_INFO_CALL_ID /* Standard info      */

/** Trusted OS: QSEE call groups */
#define TZ_SVC_APP_MGR            1       /* Application management         */
#define TZ_SVC_LISTENER           2       /* Listener service management    */
#define TZ_SVC_EXTERNAL           3       /* External image loading         */
#define TZ_SVC_RPMB               4       /* Replay Protected Memory Block  */
#define TZ_SVC_KEYSTORE           5       /* Keystore management            */
#define TZ_SVC_TZAPPS             253     /* TODO remove: Legacy ID         */
#define TZ_SVC_EXECUTIVE          254     /* TODO remove: Legacy ID         */
#define TZ_SVC_QSEE_OS_INFO       TZ_SVC_INFO_CALL_ID /* Trusted OS info    */

/** Trusted OS: Mobicore call groups */
#define TZ_SVC_EXECUTIVE_EXT      1       /* Third party OS                 */
#define TZ_SVC_MOBI_OS_INFO       TZ_SVC_INFO_CALL_ID /* Trusted OS info    */

/** Trusted Application call groups */
#define TZ_SVC_APP_ID_PLACEHOLDER 0     /* SVC bits will contain App ID     */

/** @} */ /* end_namegroup Service IDs */

/** @} */ /* end_addtogroup tz_macros_constants */

/*----------------------------------------------------------------------------
 * Command IDs -- Service ID[19:10] and Function ID[9:0]
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_SET_BOOT_ADDR_OLD_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x01)

/**
   @ingroup terminate_pwr_collapse

   Called to terminate power collapse. The QSEE configures the
   processor, then issues the Wait For Interrupt (WFI) instruction.

   @note1hang Implementation takes place entirely in Monitor mode.

   @owner_id
     0x00000002

   @command_id
     0x00000102

   @param_id
    0x00000001

   @return
     None.
*/
#define TZ_POWER_COLLAPSE_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x02)

#define TZ_POWER_COLLAPSE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )



/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_BOOT_DEPRECATED_0                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x03)

/**
   @ingroup krait_errata_21_workaround_enable

   Monitor call to enable the Krait errata #21 workaround. This
   workaround sets BPCR[FBHTD]=1 and does the required branch
   predictor maintenance.

   @note1hang Implementation takes place entirely in Monitor mode.

   @owner_id
     0x00000002

   @command_id
     0x00000104

   @param_id
     None.

   @return
     E_SUCCESS (always).
*/
#define TZ_KRAIT_ERRATA_21_WORKAROUND_ENABLE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x04)

#define TZ_KRAIT_ERRATA_21_WORKAROUND_ENABLE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup cpu_config

   Cold and warm boot time CPU configuration service.

   @owner_id
     0x00000002

   @command_id
     0x00000105

   @param_id
     0x00000004

   @com_struct
     tz_syscall_rsp_s

   @sys_call_params{tz_cpu_config_req_s}
     @table{weak__tz__cpu__config__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_CPU_CONFIG_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x05)

#define TZ_CPU_CONFIG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup cpu_config_query

   Service for querying CPU configuration.

   @owner_id
     0x00000002

   @command_id
     0x00000106

   @param_id
     0x00002004

   @sys_call_req_params{tz_cpu_config_query_req_s}
     @table{weak__tz__cpu__config__query__req__s}

   @sys_call_rsp_params{tz_cpu_config_query_rsp_s}
     @table{weak__tz__cpu__config__query__rsp__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_CPU_CONFIG_QUERY_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x06)

#define TZ_CPU_CONFIG_QUERY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup secure_wdog_disable

   Subroutine used by the HLOS to disable, or enable, the secure watchdog.
   This call is a NOP when debug functionality is disabled.

   @owner_id
     0x00000002

   @command_id
     0x00000107

   @param_id
     0x00000001

   @sys_call_params{tz_secure_wdog_disable_req_s}
     @table{weak__tz__secure__wdog__disable__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SECURE_WDOG_DISABLE_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x07)

#define TZ_SECURE_WDOG_DISABLE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup secure_wdog_trigger

   Subroutine used by the HLOS to trigger a secure watchdog and reset
   the device. This call is a NOP when debug functionality is
   disabled.

   @owner_id
     0x00000002

   @command_id
     0x00000108

   @param_id
     0x00000001

   @sys_call_params{tz_secure_wdog_trigger_req_s}
     @table{weak__tz__secure__wdog__trigger__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     An error code if parameters are invalid or watchdog
     reset is not permitted; does not return otherwise.
*/
#define TZ_SECURE_WDOG_TRIGGER_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x08)

#define TZ_SECURE_WDOG_TRIGGER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup config_hw_for_offline_ram_dump

   Subroutine used by the HLOS to configure hardware registers
   BOOT_PARTITION_SELECT and WDOG_DEBUG_EN for an offline RAM dump.

   @owner_id
     0x00000002

   @command_id
     0x00000109

   @param_id
     0x00000002

   @sys_call_params{tz_config_hw_for_ram_dump_req_s}
     @table{weak__tz__config__hw__for__ram__dump__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; an error code if parameters are invalid or if xPU
     reconfiguration is not permitted.
*/
#define TZ_CONFIG_HW_FOR_RAM_DUMP_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x09)

#define TZ_CONFIG_HW_FOR_RAM_DUMP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup video_cpu_set_state

   Controls changing the state of the Venus core (e.g. suspend/resume),
   without full subsystem restarts

   @owner_id
     0x00000002

   @command_id
     0x0000010a

   @param_id
     0x00000204

   @sys_call_params{tz_video_set_state_req_s}
     @table{weak__tz__video__set__state__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     Zero on success; negative value on failure.
*/
#define TZ_VIDEO_SET_STATE_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0A)

#define TZ_VIDEO_SET_STATE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup lock_xpu_peripheral

   Subroutine used by external bootloaders to request TZ to lock
   peripheral image area

   @owner_id
     0x00000002

   @command_id
     0x0000010b

   @param_id
     None.

   @sys_call_params{tz_lock_xpu_s}
     @table{weak__tz__lock__xpu__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_XPU_LOCK_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0b)

// TODO Syscall does not exist
#define TZ_XPU_LOCK_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup spin_with_irqs_fiqs_disabled

   If secure watchdog debug is allowed, calling this syscall masks IRQs and
   FIQs and starts spinning in TZ and won't return. If secure watchdog debug is
   not allowed, the call will return without side effects.

   @owner_id
     0x00000002

   @command_id
     0x0000010D

   @param_id
     0x00000001

   @sys_call_params{tz_spin_with_irqs_fiqs_disabled_s}
     @table{weak__tz__spin__with__irqs__fiqs__disabled__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; an error code if parameters are invalid or if xPU
     reconfiguration is not permitted.
*/

#define TZ_SPIN_WITH_IRQS_FIQS_DISABLED_ID          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0D)

#define TZ_SPIN_WITH_IRQS_FIQS_DISABLED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup set_milestone

   Subroutine used by external bootloaders to indicate the end
   of the bootloading stage, which is the beginning of HLOS booting.

   @owner_id
     0x00000002

   @command_id
     0x0000010E

   @param_id
     0x00000003

   @sys_call_params{tz_set_milestone_s}
     @table{weak__tz__set__milestone__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MILESTONE_SET_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0E)

#define TZ_MILESTONE_SET_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup Do mode switch

   Used by Apps BL to switch EL1 mode to A64

   @owner_id
     0x00000002

   @command_id
     0x0000010F

   @param_id
     0x00000021

   @sys_call_params{tz_do_modeswitch_s}
     @table{weak__tz__do__modeswitch__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_DO_MODE_SWITCH                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x0F)

#define TZ_DO_MODE_SWITCH_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup force_dload_mode

   Subroutine used by the HLOS to configure hardware register
   TCSR_BOOT_MISC_DETECT to force different dload modes.

   @owner_id
     0x00000002

   @command_id
     0x00000110

   @param_id
     0x00000002

   @sys_call_params{tz_force_dload_mode_req_s}
     @table{weak__tz__force__dload__mode__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; an error code if parameters are invalid or not permitted
*/
#define TZ_FORCE_DLOAD_ID                              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x10)

#define TZ_FORCE_DLOAD_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup set_boot_addr

   Sets the address of execution for the nonsecure execution
   environment after a CPU is brought out of reset.

   @owner_id
     0x00000002

   @command_id
     0x00000111

   @param_id
     0x00000006

   @sys_call_params{tz_set_boot_addr_s}
     @table{weak__tz__set__boot__addr__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_SET_BOOT_ADDR_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x11)

#define TZ_SET_BOOT_ADDR_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup Do mode switch. Legacy API does not have buffer len
   after buffer.

   Used by Apps BL to switch EL1 mode to A64

   @owner_id
     0x00000002

   @command_id
     0x00000112

   @param_id
     0x00000022

   @sys_call_params{tz_do_modeswitch_s}
     @table{weak__tz__do__modeswitch__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_DO_MODE_SWITCH_LEGACY                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_BOOT, 0x12)

#define TZ_DO_MODE_SWITCH_LEGACY_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_BUF_RO )

/**
   @ingroup init_image_syscall

   Verifies whether the memory at the specified Executable and Linkable
   Format (ELF) header pointer is a valid ELF file. Once that has been
   verified, the QSEE copies the ELF header, program header, and hash
   segment into secure memory.

   @owner_id
     0x00000002

   @command_id
     0x00000201

   @param_id
     0x00000082

   @sys_call_params{tz_pil_init_req_s}
     @table{weak__tz__pil__init__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_INIT_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x01)

#define TZ_PIL_INIT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup get_pil_mem_usage

   Confirms whether the memory region used to load the specified
   peripheral image matches the static region that the QSEE supports
   for the specified region, and that the region is appropriate for the
   ELF header specified in the secure Peripheral Image Loader (PIL)
   initialization call.

   @owner_id
     0x00000002

   @command_id
     0x00000202

   @param_id
     0x00000003

   @sys_call_params{tz_pil_mem_req_s}
     @table{weak__tz__pil__mem__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_MEM_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x02)

#define TZ_PIL_MEM_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/** @cond */

/*----------------------------------------------------------------------------
                Deprecated Command IDs: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_PIL_DEPRECATED_0 TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x03)
#define TZ_PIL_DEPRECATED_1 TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x04)
/** @endcond */

/**
   @ingroup auth_reset

   Authenticates and releases the specified peripheral from reset.
   The QSEE also protects the memory region for the specified
   peripheral image with a Memory Protection Unit (MPU).

   @owner_id
     0x00000002

   @command_id
     0x00000205

   @param_id
     0x00000001

   @sys_call_params{tz_pil_auth_reset_req_s}
     @table{weak__tz__pil__auth__reset__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_AUTH_RESET_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x05)

#define TZ_PIL_AUTH_RESET_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup unlock_xpu

   Stops the specified peripheral, then clears the peripheral image
   location and unlocks the memory area occupied by that region.

   @owner_id
     0x00000002

   @command_id
     0x00000206

   @param_id
     0x00000001

   @sys_call_params{tz_pil_unlock_req_s}
     @table{weak__tz__pil__unlock__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_PIL_UNLOCK_XPU_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x06)

#define TZ_PIL_UNLOCK_XPU_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup is_subsys_supported

   Queries whether the QSEE supports secure PIL for a specified
   subsystem.

   @owner_id
     0x00000002

   @command_id
     0x00000207

   @param_id
     0x00000083

   @sys_call_req_params{tz_pil_subsystem_query_req_s}
     @table{weak__tz__pil__subsystem__query__req__s}

   @sys_call_rsp_params{tz_pil_subsystem_query_rsp_s}
     @table{weak__tz__pil__subsystem__query__rsp__s}

   @return
     E_SUCCESS if the request was successful; an error code otherwise.
*/
#define TZ_PIL_IS_SUBSYS_SUPPORTED_ID            \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x07)

#define TZ_PIL_IS_SUBSYS_SUPPORTED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup is_subsys_mandated

   Queries whether QSEE security settings make it secure to the point
   where a PIL is required for the specified subsystem.

   @owner_id
     0x00000002

   @command_id
     0x00000208

   @param_id
     0x00000083

   @sys_call_params
     Same request and response parameters as for
     #TZ_PIL_IS_SUBSYS_SUPPORTED_ID.

   @return
     E_SUCCESS if the request was successful; an error code otherwise.
*/
#define TZ_PIL_IS_SUBSYS_MANDATED_ID             \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x08)

#define TZ_PIL_IS_SUBSYS_MANDATED_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup get_mem_area

   Queries the start and end addresses in which subsystems are allowed
   to be loaded.

   @owner_id
     0x00000002

   @command_id
     0x00000209

   @param_id
     0x00000083

   @sys_call_req_params{tz_pil_mem_area_query_req_s}
     @table{weak__tz__pil__mem__area__query__req__s}

   @sys_call_rsp_params{tz_pil_mem_area_query_rsp_s}
     @table{weak__tz__pil__mem__area__query__rsp__s}

   @return
     E_SUCCESS if the request was successful; error code otherwise.
*/
#define TZ_PIL_GET_MEM_AREA_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x09)

#define TZ_PIL_GET_MEM_AREA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup tzbsp_pil_modem_restart

   System call for modem start and stop. This is 8916
   specific but added in pil group on HLOS request.

   @owner_id
     0x00000002

   @command_id
     0x0000020A

   @param_id
     0x00000002

    @sys_call_params{tz_pil_modem_restart_req_s}
     @table{weak__tz__pil__modem__restart__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
 */
  
#define TZ_PIL_MSS_RESTART_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL, 0x0A)

#define TZ_PIL_MSS_RESTART_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )
  

/**
   @ingroup share_memory

   Configures xPUs to share/unshare HLOS memory with a specified subsystem. 

   @owner_id
     0x00000002

   @command_id
     0x0000020B

   @param_id
     0x00000004

   @sys_call_params{tz_pil_share_memory_s}
     @table{weak__tz__pil__share_memory__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_PIL_SHARE_MEMORY_ID              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL,  0x0B)

#define TZ_PIL_SHARE_MEMORY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/*
   Deprecated: This command is no longer supported.
*/
#define TZ_DUMP_WRITE_LPASS_QDSP6_NMI_ID         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x01)

/**
   @ingroup set_wdt_buff_addr

   System call for the nonsecure side to set the Watchdog Timer (WDT)
   reset context dump buffer address.

   @owner_id
     0x00000002

   @command_id
     0x00000302

   @param_id
     0x00000022

   @sys_call_params{tz_dump_set_cpu_ctx_buf_req_s}
     @table{weak__tz__dump__set__cpu__ctx__buf__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_CPU_CTX_BUF_ID               \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x02)

#define TZ_DUMP_SET_CPU_CTX_BUF_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/*
   Deprecated: Use of this command is no longer supported
*/
#define TZ_DUMP_WRITE_MSS_QDSP6_NMI_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x03)

/**
   @ingroup set_l1_dump_buffer

   System call for the nonsecure side to set the L1 dump buffer address.

   @owner_id
     0x00000002

   @command_id
     0x00000304

   @param_id
     0x00000022

   @sys_call_params{tz_dump_set_cache_buf_req_s}
     @table{weak__tz__dump__set__cache__buf__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_L1_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x04)

#define TZ_DUMP_SET_L1_DUMP_BUF_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

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

   @owner_id
     0x00000002

   @command_id
     0x00000305

   @param_id
     0x00000001

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
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x05)

#define TZ_DUMP_REQUEST_CACHE_DUMP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_l1_buffer_size

   System call for the nonsecure side to query the L1 dump buffer size.

   @owner_id
     0x00000002

   @command_id
     0x00000306

   @param_id
     0x00000083

   @sys_call_req_params{tz_dump_query_cache_buf_size_req_s}
     @table{weak__tz__dump__query__cache__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_cache_buf_size_rsp_s}
     @table{weak__tz__dump__query__cache__buf__size__rsp__s}

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_L1_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x06)

#define TZ_DUMP_QUERY_L1_DUMP_BUF_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup set_l2_dump_buffer

   System call for for the nonsecure side to set the L2 dump buffer address.

   @owner_id
     0x00000002

   @command_id
     0x00000307

   @param_id
     0x00000002

   @sys_call_params{tz_dump_set_cache_buf_req_s}
     @table{weak__tz__dump__set__cache__buf__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_L2_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x07)

#define TZ_DUMP_SET_L2_DUMP_BUF_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_l2_buffer_size

   System call for the nonsecure side to query the L2 dump buffer size.

   @owner_id
     0x00000002

   @command_id
     0x00000308

   @param_id
     0x00000083

   @sys_call_req_params{tz_dump_query_cache_buf_size_req_s}
     @table{weak__tz__dump__query__cache__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_cache_buf_size_rsp_s}
     @table{weak__tz__dump__query__cache__buf__size__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_L2_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x08)

#define TZ_DUMP_QUERY_L2_DUMP_BUF_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup set_ocmem_dump_buf

   System call for the nonsecure side to set the OCMEM dump buffer address.

   @owner_id
     0x00000002

   @command_id
     0x00000309

   @param_id
     0x00000022

   @sys_call_req_params{tz_dump_set_ocmem_buf_req_s}
     @table{weak__tz__dump__set__ocmem__buf__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SET_OCMEM_DUMP_BUF_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x09)

#define TZ_DUMP_SET_OCMEM_DUMP_BUF_ID_PARAM_ID \
    TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_ocmem_dump_buf_size

   System call for the nonsecure side to query the OCMEM dump buffer size.

   @owner_id
     0x00000002

   @command_id
     0x0000030A

   @param_id
     0x00000083

   @sys_call_req_params{tz_dump_query_ocmem_buf_size_req_s}
     @table{weak__tz__dump__query__ocmem__buf__size__req__s}

   @sys_call_rsp_params{tz_dump_query_ocmem_buf_size_rsp_s}
     @table{weak__tz__dump__query__ocmem__buf__size__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_QUERY_OCMEM_DUMP_BUF_SIZE_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0A)

#define TZ_DUMP_QUERY_OCMEM_DUMP_BUF_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup security_allows_mem_dump

   System call for the SBL to determine whether memory dumping
   (i.e., DLOAD mode) is allowed.

   @owner_id
     0x00000002

   @command_id
     0x0000030b

   @param_id
     0x00000022

   @com_struct
     tz_syscall_req_s

   @sys_call_rsp_params{tz_security_allows_mem_dump_rsp_s}
     @table{weak__tz__security__allows__mem__dump__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0B)

#define TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup smmu_dump_fault_syndrome

   System call for dumping the SMMU fault syndrome registers.

   @note1hang
     SMMU clocks must be ON.

   @owner_id
     0x00000002

   @command_id
     0x0000030c

   @param_id
     0x00000204

   @com_struct
     tz_syscall_req_s

   @return
     SUCCESS if the call succeeded; error code otherwise.
*/
#define TZ_DUMP_SMMU_FAULT_REGS_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0C)

#define TZ_DUMP_SMMU_FAULT_REGS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup security_is_retail_unlock

   System call for the SBL to determine whether retail unlock is enable.

   @owner_id
     0x00000002

   @command_id
     0x0000030d

   @param_id
     0x00000022

   @com_struct
     tz_syscall_req_s

   @sys_call_rsp_params{tz_security_is_retail_unlock_rsp_s}
     @table{weak__tz__security__is__retail__unlock__rsp__s}

   @return
     SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_DUMP_IS_RETAIL_UNLOCK_ID \
        TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_DUMP, 0x0D)

#define TZ_DUMP_IS_RETAIL_UNLOCK_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup lock_shared_resource

   Locks a shared hardware resource.

   @owner_id
     0x00000002

   @command_id
     0x00000401

   @param_id
     None.

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
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING, 0x01)

// TODO Syscall does not exist
#define TZ_RESOURCE_LOCK_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup change_resource_config

   System call to change resource configuration dynamically.

   @owner_id
     0x00000002

   @command_id
     0x00000402

   @param_id
     0x00000002

   @sys_call_params{tz_config_resource_req_s}
     @table{weak__tz__config__resource__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_RESOURCE_CONFIG_ID                            \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING, 0x02)

#define TZ_RESOURCE_CONFIG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
  @ingroup blsp_modify_ownership

   @owner_id
     0x00000002

  @command_id
    0x00000403

  @param_id
    0x00000022

  @sys_call_req_params{tz_modify_ownership_req_s}
     @table{weak__tz__modify__ownership__req__s}

  @com_struct
     tz_syscall_rsp_s

  @return
    E_SUCCESS if success; else error code.
*/
#define TZ_BLSP_MODIFY_OWNERSHIP_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING, 0x03)

#define TZ_BLSP_MODIFY_OWNERSHIP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup unlock_debug_register
   
   System call to unlock debug register for QDSS
   
   @note1hang TZ will keep a persistent vote on QDSS clocks after calling this API
   
   @command_id
     0x00001004
     
   @com_struct
     tz_syscall_rsp_s     
     
   @return
     E_SUCCESS on success
*/  
#define TZ_UNLOCK_DBG_REG                                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_RES_LOCKING, 0x04)

#define TZ_UNLOCK_DBG_REG_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0
    
  
/**
   @ingroup io_access_read

   Allows reading of memory areas that fall under Memory/Area/Register
   Protection Unit (xPU) protection but do not need to be secured.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @owner_id
     0x00000002

   @command_id
     0x00000501

   @param_id
     0x00000001

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
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x01)

#define TZ_IO_ACCESS_READ_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup io_access_write

   Allows writing to memory areas that fall under xPU protection
   but do not need to be secured.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @owner_id
     0x00000002

   @command_id
     0x00000502

   @param_id
     0x00000002

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
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x02)

#define TZ_IO_ACCESS_WRITE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/*
 * Reserved ID.
 */
#define TZ_IO_ACCESS_RESERVED_3_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x03)

/*
 * Reserved ID.
 */
#define TZ_IO_ACCESS_RESERVED_4_ID                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_IO_ACCESS, 0x04)

/**
   @ingroup is_svc_avail

   Checks if a system call is available.

   @owner_id
     0x00000002

   @command_id
     0x00000601

   @param_id
     0x00000083

   @sys_call_req_params{tz_info_is_svc_available_req_s}
     @table{weak__tz__info__is__svc__available__req__s}

   @sys_call_rsp_params{tz_info_is_svc_available_rsp_s}
     @table{weak__tz__info__is__svc__available__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_INFO_IS_SVC_AVAILABLE_ID              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x01)

#define TZ_INFO_IS_SVC_AVAILABLE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup get_diag_info

   Copies the QSEE software diagnostic region into the specified buffer.

   @owner_id
     0x00000002

   @command_id
     0x00000602

   @param_id
     0x00000022

   @sys_call_params{tz_info_get_diag_req_s}
     @table{weak__tz__info__get__diag__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_INFO_GET_DIAG_ID                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x02)

#define TZ_INFO_GET_DIAG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup get_feature_ver

   Returns the current version of a specified QSEE feature.

   @owner_id
     0x00000002

   @command_id
     0x00000603

   @param_id
     0x00000083

   @sys_call_req_params{tz_feature_version_req_s}
     @table{weak__tz__feature__version__req__s}

   @sys_call_rsp_params{tz_feature_version_rsp_s}
     @table{weak__tz__feature__version__rsp__s}

   @returns
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_INFO_GET_FEATURE_VERSION_ID           \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x03)

#define TZ_INFO_GET_FEATURE_VERSION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1(TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup tzos_register_log_buffer

   Register the secure application log buffer.

   @smc_id
     0x32000106

   @param_id
     0x00000022

   @sys_call_params{tz_os_register_log_buffer_req_s}
     @table{weak__tz__os__register__log__buffer__req__s}

   @sys_call_params{tz_os_rsp_s}
     @table{weak__tz__os__rsp__s}

   @return
     Zero on success, negative value on failure.
 */
#define TZ_OS_REGISTER_LOG_BUFFER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x06)

#define TZ_OS_REGISTER_LOG_BUFFER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )


/**
   @ingroup get_secure_state

   System call to check the security status of the system

   @owner_id
     0x00000002

   @command_id
     0x00000604

   @param_id
     0x00000022

   @sys_call_params{tz_get_secure_state_req_s}
     @table{weak__tz__get__secure__state__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
 */
#define TZ_INFO_GET_SECURE_STATE                     \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x4)

#define TZ_INFO_GET_SECURE_STATE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/** TODO: Shifted to SVC_PIL. Remove this ID once APPS changes are promoted.
	Changed ID from TZ_MSS_RESTART to TZBSP_MSS_RESTART to avoid compiler error.
	 @ingroup tzbsp_modem_restart
	System call for modem start and stop. as this is 8916	
	specific, adding in miscellaneous group.
	
	@owner_id
    0x00000002

	@command_id
		0x00000605
		
	@param_id
	  None.

	@sys_call_params{tz_modem_restart_req_s}
		@table{weak__tz__modem__restart__req__s}
		
	@com_struct
		 tz_syscall_rsp_s
	
	@return
	E_SUCCESS on success; a negative value on failure.
*/
#define TZBSP_MSS_RESTART_ID                     \
	CMD_IDTZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_INFO, 0x5)
// TODO Syscall does not exist
#define TZBSP_MSS_RESTART_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup decrypt_image

   Parses the XML in the encrypted bootloader image and
   decrypts the data in place.

   @owner_id
     0x00000002

   @command_id
     0x00000701

   @param_id
     0x000000A2

   @sys_call_params{tz_ssd_decrypt_img_req_s}
     @table{weak__tz__ssd__decrypt__img__req__s}

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_IMG_ID                    \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x01)

#define TZ_SSD_DECRYPT_IMG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW)

/**
   @ingroup encrypt_keystore

   Encrypts the keystore with a key derived from the Crypto Engine (CE)
   hardware key.

   @owner_id
     0x00000002

   @command_id
     0x00000702

   @param_id
     0x000000A2

   @sys_call_params{tz_ssd_encrypt_keystore_req_s}
     @table{weak__tz__ssd__encrypt__keystore__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_ENCRYPT_KEY_STORE_ID              \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x02)

#define TZ_SSD_ENCRYPT_KEY_STORE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW)

/*----------------------------------------------------------------------------
                Deprecated Command ID: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_SSD_DEPRECATED_0                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x03)

/*----------------------------------------------------------------------------
                Deprecated Command ID: Do not reuse
 * -------------------------------------------------------------------------*/
#define TZ_SSD_DEPRECATED_1                      \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x04)

/**
   @ingroup protect_keystore

   Loads the Secure Software Download (SSD) keystore into the QSEE
   software for future use.

   @sys_call_req_params{tz_ssd_protect_keystore_req_s}
     @table{weak__tz__ssd__protect__keystore__req__s}

   @sys_call_rsp_params{tz_ssd_protect_keystore_rsp_s}
     @table{weak__tz__ssd__protect__keystore__rsp__s}

   @owner_id
     0x00000002

   @command_id
     0x00000705

   @param_id
     0x00000224

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_PROTECT_KEYSTORE_ID                  \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x05)

#define TZ_SSD_PROTECT_KEYSTORE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

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

   @owner_id
     0x00000002

   @command_id
     0x00000706

   @param_id
     0x00000284

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_PARSE_MD_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x06)

#define TZ_SSD_PARSE_MD_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup decrypt_img_frag

   Decrypts the data in place for a specified fragment of an encrypted image.

   @sys_call_req_params{tz_ssd_decrypt_img_frag_req_s}
     @table{weak__tz__ssd__decrypt__img__frag__req__s}

   @sys_call_rsp_params{tz_ssd_decrypt_frag_rsp_s}
     @table{weak__tz__ssd__decrypt__frag__rsp__s}

   @owner_id
     0x00000002

   @command_id
     0x00000707

   @param_id
     0x00002806

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_IMG_FRAG_ID               \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x07)

#define TZ_SSD_DECRYPT_IMG_FRAG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup decrypt_elf_seg_frag

   Decrypts the data in place for a specified ELF segment from an
   encrypted image.

   @sys_call_req_params{tz_ssd_decrypt_elf_seg_frag_req_s}
     @table{weak__tz__ssd__decrypt__elf__seg__frag__req__s}

   @sys_call_rsp_params{tz_ssd_decrypt_frag_rsp_s}
     @table{weak__tz__ssd__decrypt__frag__rsp__s}

   @owner_id
     0x00000002

   @command_id
     0x00000708

   @param_id
     0x00028008

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_SSD_DECRYPT_ELF_SEG_FRAG_ID               \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SSD, 0x08)

#define TZ_SSD_DECRYPT_ELF_SEG_FRAG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_8( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup blow_sw_fuse

   Changes the state of the software fuse.

   @owner_id
     0x00000002

   @command_id
     0x00000801

   @param_id
     0x00000001

   @sys_call_params{tz_sw_fuse_req_s}
     @table{weak__tz__sw__fuse__req__s}

   @com_struct
     See #TZ_IS_SW_FUSE_BLOWN_ID for the response structure.

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_BLOW_SW_FUSE_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x01)

#define TZ_BLOW_SW_FUSE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup is_sw_fuse_blown

   Queries whether the specified software fuse has been blown.

   @caution1hang This call is not thread safe.

   @owner_id
     0x00000002

   @command_id
     0x00000802

   @param_id
     0x00000083

   @sys_call_params{tz_sw_fuse_rsp_s}
     @table{weak__tz__sw__fuse__rsp__s}

   @com_struct
     See #TZ_BLOW_SW_FUSE_ID for the request structure.

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_IS_SW_FUSE_BLOWN_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x02)

#define TZ_IS_SW_FUSE_BLOWN_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup write_qfprom_fuse

   Writes a Qualcomm Field Programmable Read Only Memory (QFPROM) fuse row.

   @owner_id
     0x00000002

   @command_id
     0x00000803

   @param_id
     0x00000884

   @sys_call_params{tz_qfprom_write_row_s}
     @table{weak__tz__qfprom__write__row__s}

   @com_struct
      tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_WRITE_ROW_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x03)

#define TZ_QFPROM_WRITE_ROW_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup write_mult_qfprom_rows

   Writes multiple rows of QFPROM fuses.

   @note1hang This call is only available prior to a
              #TZ_MILESTONE_SET_ID call.

   @owner_id
     0x00000002

   @command_id
     0x00000804

   @param_id
     0x00000824

   @sys_call_params{tz_qfprom_write_multiple_rows_s}
     @table{weak__tz__qfprom__write__multiple__rows__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
 */
#define TZ_QFPROM_WRITE_MULTIPLE_ROWS_ID         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x04)

#define TZ_QFPROM_WRITE_MULTIPLE_ROWS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup read_qfprom_fuse

   Reads QFPROM fuses.

   @owner_id
     0x00000002

   @command_id
     0x00000805

   @param_id
     0x00000A04

   @sys_call_params{tz_qfprom_read_row_s}
     @table{weak__tz__qfprom__read__row__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_READ_ROW_ID                    \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x05)

#define TZ_QFPROM_READ_ROW_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup write_rollback_qfprom_fuse

   Write rollback-specific QFPROM fuses.

   @note1hang This call is only available prior to a
              #TZ_MILESTONE_SET_ID call.

   @owner_id
     0x00000002

   @command_id
     0x00000806

   @param_id
     0x00000884

   @sys_call_params{tz_qfprom_rollback_write_row_s}
     @table{weak__tz__qfprom__rollback__write__row__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_QFPROM_ROLLBACK_WRITE_ROW_ID          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x06)

#define TZ_QFPROM_ROLLBACK_WRITE_ROW_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup is_sw_fuse_blown

   Queries whether the specified software fuse has been blown.
   Legacy API signature uses rsp buffer to return result.

   @caution1hang This call is not thread safe.

   @owner_id
     0x00000002

   @command_id
     0x00000802

   @param_id
     0x00000083

   @sys_call_params{tz_sw_fuse_rsp_s}
     @table{weak__tz__sw__fuse__rsp__s}

   @com_struct
     See #TZ_BLOW_SW_FUSE_ID for the request structure.

   @return
     Zero on success; a negative value on failure.
*/
#define TZ_IS_SW_FUSE_BLOWN_LEGACY_ID                   \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_FUSE, 0x07)

#define TZ_IS_SW_FUSE_BLOWN_LEGACY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup spmi_disable_bus

   Disables SPMI bus to allow clean shutdowns when deasserting PS_HOLD.

   @note1hang Implementation takes place entirely in Monitor mode. \n
   @note1hang This command uses a register-based QSEE interface.

   @owner_id
     0x00000002

   @command_id
     0x00000901

   @param_id
     None.

   @param[in] r0  Command ID
   @param[in] r1  Token ID.

   @param[in] r2  Reserved/spare, should be zero.

   @return
     E_SUCCESS if the call succeeded; an error code otherwise.
*/
#define TZ_POWER_SPMI_DISABLE_BUS_ID                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_POWER, 0x01)

// TODO Syscall not yet implemented
#define TZ_POWER_SPMI_DISABLE_BUS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup get_prng_data

   Retrieve random data from the Pseudo-Random Number Generator (PRNG)
   hardware.

   @owner_id
     0x00000002

   @command_id
     0x00000A01

   @param_id
     0x00000022

   @sys_call_params{tz_get_prng_data_s}
     @table{weak__tz__get__prng__data__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; a negative value on failure.
*/
#define TZ_PRNG_GETDATA_ID                         \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_CRYPTO, 0x01)

#define TZ_PRNG_GETDATA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup init_secure_channel

   Writes the key material to the specified address.

   This function returns E_ALREADY_DONE if the keys are already written.

   @owner_id
     0x00000002

   @command_id
     0x00000B01

   @param_id
     None.

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
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SC_KEY_EXCHANGE, 0x01)

// TODO Syscall does not exist
#define TZ_SECURE_CHANNEL_INIT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup deinit_secure_channel

   Clears the keys and resets the channel state.

   @owner_id
     0x00000002

   @command_id
     0x00000B02

   @param_id
     None.

   @sys_call_params{tz_sc_deinit_s}
     @table{weak__tz__sc__deinit__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; E_FAILURE otherwise.
*/
#define TZ_SECURE_CHANNEL_DEINIT_ID                          \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_SC_KEY_EXCHANGE, 0x02)

// TODO Syscall does not exist
#define TZ_SECURE_CHANNEL_DEINIT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup protect_memory

   Protects/unprotects a memory range with an MPU.

   @owner_id
     0x00000002

   @command_id
     0x00000C01

   @param_id
     0x00000005

   @sys_call_params{tz_protect_memory_s}
     @table{weak__tz__protect__memory__s}

   @com_struct
      tz_syscall_rsp_s

   @return
     E_SUCCESS on success; error code otherwise.
*/
#define TZ_PROTECT_MEMORY                                    \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x01)

#define TZ_PROTECT_MEMORY_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup install_nonsecure_debug_fiq

   Installs a nonsecure debug FIQ handler.

   @owner_id
     0x00000002

   @command_id
     0x00000E01

   @param_id
     0x00000256

   @sys_call_params{tz_config_ns_debug_fiq_req_s}
     @table{weak__tz__config__ns__debug__fiq__req__s}

   @com_struct
      tz_syscall_rsp_s

   @return
      E_SUCCESS if the nonsecure debug FIQ handler and context were
      installed successfully; error code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_CONFIG_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_NS_DEBUG_FIQ, 0x01)

#define TZ_NS_DEBUG_FIQ_CONFIG_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RO, TZ_SYSCALL_PARAM_TYPE_BUF_RO, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_nonsecure_debug_fiq_ctx_buf

   Queries the minimum size of a nonsecure debug FIQ context buffer. A pointer
   to the context buffer is passed to the nonsecure handler in register r0.

   @owner_id
     0x00000002

   @command_id
     0x00000E02

   @param_id
     0x00000022

   @com_struct
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     E_SUCCESS if the context buffer size query was successfull; error
     code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_CTX_SIZE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_NS_DEBUG_FIQ, 0x02)

#define TZ_NS_DEBUG_FIQ_CTX_SIZE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup query_nonsecure_debug_fiq_int

   Queries whether an interrupt is allowed to be configured as a nonsecure
   debug FIQ. The QSEE only allows a limited set of chipset-specific interrupts
   to be used for this purpose.

   @owner_id
     0x00000002

   @command_id
     0x00000E03

   @param_id
     0x00000083

   @sys_call_params{tz_config_ns_debug_fiq_int_ok_req_s}
     @table{weak__tz__config__ns__debug__fiq__int__ok__req__s}

   @com_struct
      tz_syscall_rsp_s

   @return
     E_SUCCESS if the context buffer size query was successfull; error
     code otherwise.
*/
#define TZ_NS_DEBUG_FIQ_INT_OK_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_NS_DEBUG_FIQ, 0x03)

#define TZ_NS_DEBUG_FIQ_INT_OK_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup restore_sec_cfg

   Restores VMIDMT/xPU/SMMU configuration of a device's System Memory
   Management Unit (SMMU) global space.

   @note1hang SMMU clocks must be ON.

   @owner_id
     0x00000002

   @command_id
     0x00000C02

   @param_id
     0x00000002

   @sys_call_params{tz_restore_sec_cfg_req_s}
     @table{weak__tz__restore__sec__cfg__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_RESTORE_SEC_CFG  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x02)

#define TZ_RESTORE_SEC_CFG_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_get_ptbl_mem_size

   Queries the memory size requirement for multimedia SMMU page
   tables.

   @owner_id
     0x00000002

   @command_id
     0x00000C03

   @param_id
     0x00000083

   @sys_call_req_params{tz_memprot_ptbl_mem_size_req_s}
     @table{weak__tz__memprot__ptbl__mem__size__req__s}

   @sys_call_rsp_params{tz_memprot_ptbl_mem_size_rsp_s}
     @table{weak__tz__memprot__ptbl__mem__size__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_GET_PTBL_MEM_SIZE  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x03)

#define TZ_MEM_PROTECT_GET_PTBL_MEM_SIZE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_ptbl_mem_init

   Provides memory used for page tables by multimedia SMMU
   devices involved in content protection.

   @owner_id
     0x00000002

   @command_id
     0x00000C04

   @param_id
     0x00000805

   @sys_call_params{tz_memprot_ptbl_mem_init_req_s}
     @table{weak__tz__memprot__ptbl__mem__init__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_PTBL_MEM_INIT  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x04)

#define TZ_MEM_PROTECT_PTBL_MEM_INIT_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL)

/**
   @ingroup mem_protect_set_cp_pool_size

   Provide the CP pool size to calculate translation table
   requirement

   @owner_id
     0x00000002

   @command_id
     0x00000C05

   @param_id
     0x00000204

   @sys_call_params{tz_memprot_ptbl_mem_init_req_s}
     @table{weak__tz__memprot__set__cp__pool__size__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_SET_CP_POOL_SIZE TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x05)

#define TZ_MEM_PROTECT_SET_CP_POOL_SIZE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_video_var

   Sets secure Virtual Address Ranges (VAR) in the video device's
   virtual address map.

   @owner_id
     0x00000002

   @command_id
     0x00000C08

   @param_id
     0x00002006

   @sys_call_params{tz_memprot_video_var_req_s}
     @table{weak__tz__memprot__video__var__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_VIDEO_VAR TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x08)

#define TZ_MEM_PROTECT_VIDEO_VAR_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup vmidmt_set_memtype

   Sets the memtype attribute for a Virtual Machine ID Mapping Table (VMIDMT)
   associated with a given device.

   @owner_id
     0x00000002

   @command_id
     0x00000C09

   @param_id
     0x00000003

   @sys_call_params{tz_set_vmidmt_memtype_req_s}
     @table{weak__tz__set__vmidmt__memtype__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_VMIDMT_SET_MEMTYPE  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x09)

#define TZ_VMIDMT_SET_MEMTYPE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup disable_mmss_sec

   Disable the QRIB security for the MMSS subsystems

   @owner_id
     0x00000002

   @command_id
     0x00000C20

   @param_id
     0x00000000

   @com_struct
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     \c E_SUCCESS on success; an error code otherwise.
 */
#define TZ_DISABLE_MMSS_SEC  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x20)

#define TZ_DISABLE_MMSS_SEC_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup mem_protect_lock2

  Protects or removes protection from a list of memory chunks.

   @owner_id
     0x00000002

   @command_id
     0x00000C0A

   @param_id
     0x00002026

   @sys_call_params{tz_memprot_lock2_req_s}
     @table{weak__tz__memprot__lock2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_LOCK2   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0A)

#define TZ_MEM_PROTECT_LOCK2_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_map2

   Maps a list of memory chunks (sets up VA to PA mapping) on a
   QSEE-managed context bank.

   @owner_id
     0x00000002

   @command_id
     0x00000C0B

   @param_id
     0x000008A5

   @sys_call_params{tz_memprot_map2_req_s}
     @table{weak__tz__memprot__map2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_MAP2    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0B)

#define TZ_MEM_PROTECT_MAP2_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_unmap2

   Removes VA to PA mapping on a QSEE-managed context bank.

   @owner_id
     0x00000002

   @command_id
     0x00000C0C

   @param_id
     0x00000224

   @sys_call_params{tz_memprot_unmap2_req_s}
     @table{weak__tz__memprot__unmap2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_UNMAP2  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0C)

#define TZ_MEM_PROTECT_UNMAP2_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_tlbinval

   Invalidates Translation Lookaside Buffer (TLB) entries associated with a
   QSEE-managed context bank.

   @owner_id
     0x00000002

   @command_id
     0x00000C0D

   @param_id
     0x00002006

   @sys_call_params{tz_memprot_tlbinval_req_s}
     @table{weak__tz__memprot__tlbinval__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_TLBINVALIDATE  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0D)

#define TZ_MEM_PROTECT_TLBINVALIDATE_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

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

   @owner_id
     0x00000002

   @command_id
     0x00000C0E

   @param_id
     0x00000204

   @sys_call_req_params{tz_xpu_violation_err_fatal_req_s}
     @table{weak__tz__xpu__violation__err__fatal__req__s}

   @sys_call_rsp_params{tz_xpu_violation_err_fatal_rsp_s}
     @table{weak__tz__pil__subsystem__query__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_CONFIG_XPU_VIOLATION_ERR_FATAL                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0E)

#define TZ_CONFIG_XPU_VIOLATION_ERR_FATAL_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup secure_display_ctrl

   Enable/disable the secure display

   @owner_id
     0x00000002

   @command_id
     0x00000C0F

   @param_id
     0x00000001

   @sys_call_params{tz_memprot_sd_ctrl_req_s}
     @table{weak__tz__memprot__sd__ctrl__req__s}

   @com_struct
      tz_common_rsp_s

   @return
     E_SUCCESS on success; error code otherwise.
*/
#define TZ_MEM_PROTECT_SD_CTRL                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x0F)

#define TZ_MEM_PROTECT_SD_CTRL_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup config_xpu_violation_err_fatal

   Legacy API returns status in response buffer.

   Subroutine used by the HLOS to enable/disable xPU violations becoming fatal
   errors. This can also be used to check whether xPU violations are currently
   fatal errors by sending TZ_XPU_VIOLATION_ERR_FATAL_NOOP as the parameter.

   The TZ may choose to force enable/disable xPU violations as fatal errors by
   default. It may also choose to disallow the HLOS from overriding this
   configuration. So, the HLOS must always check the output parameter (enabled)
   to see the current behavior and the return code to see if it was allowed to
   carry out the operation (-E_NOT_SUPPORTED is returned if the HLOS is not
   allowed to change the behavior).

   @owner_id
     0x00000002

   @command_id
     0x00000C0E

   @param_id
     0x00000204

   @sys_call_req_params{tz_xpu_violation_err_fatal_req_s}
     @table{weak__tz__xpu__violation__err__fatal__req__s}

   @sys_call_rsp_params{tz_xpu_violation_err_fatal_rsp_s}
     @table{weak__tz__pil__subsystem__query__rsp__s}

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_CONFIG_XPU_VIOLATION_ERR_FATAL_LEGACY                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x10)

#define TZ_CONFIG_XPU_VIOLATION_ERR_FATAL_LEGACY_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_lock2

  Protects or removes protection from a list of memory chunks.

   @owner_id
     0x00000002

   @command_id
     0x00000C0A

   @param_id
     0x00002026

   @sys_call_params{tz_memprot_lock2_req_s}
     @table{weak__tz__memprot__lock2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_LOCK2_FLATTENED   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x11)

#define TZ_MEM_PROTECT_LOCK2_FLATTENED_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_6( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_map2

   Maps a list of memory chunks (sets up VA to PA mapping) on a
   QSEE-managed context bank.

   @owner_id
     0x00000002

   @command_id
     0x00000C0B

   @param_id
     0x000008A5

   @sys_call_params{tz_memprot_map2_req_s}
     @table{weak__tz__memprot__map2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_MAP2_FLATTENED    TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x12)

#define TZ_MEM_PROTECT_MAP2_FLATTENED_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_8( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL,    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_unmap2

   Removes VA to PA mapping on a QSEE-managed context bank.

   @owner_id
     0x00000002

   @command_id
     0x00000C0C

   @param_id
     0x00000224

   @sys_call_params{tz_memprot_unmap2_req_s}
     @table{weak__tz__memprot__unmap2__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_UNMAP2_FLATTENED  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x13)

#define TZ_MEM_PROTECT_UNMAP2_FLATTENED_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup secure_display_ctrl

   Enable/disable the secure display

   @owner_id
     0x00000002

   @command_id
     0x00000C0F

   @param_id
     0x00000001

   @sys_call_params{tz_memprot_sd_ctrl_req_s}
     @table{weak__tz__memprot__sd__ctrl__req__s}

   @com_struct
      tz_common_rsp_s

   @return
     E_SUCCESS on success; error code otherwise.
*/
#define TZ_MEM_PROTECT_SD_CTRL_REG_RSP_ID                       \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x14)

#define TZ_MEM_PROTECT_SD_CTRL_REG_RSP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup mem_protect_dev_var

   Sets secure Virtual Address Ranges (VAR) in the requested
   device's virtual address map in an specific device.

   @owner_id
     0x00000002

   @command_id
     0x00000C10

   @param_id
     None.

   @sys_call_params{tz_memprot_dev_var_req_s}
     @table{weak__tz__memprot__dev__var__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
*/
#define TZ_MEM_PROTECT_DEVICE_VAR                        \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_MEMORY_PROTECTION, 0x15)

// TODO Syscall does not exist
#define TZ_MEM_PROTECT_DEVICE_VAR_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
   @ingroup ocmem_lock_region

   System call to lock an OCMEM region.

   @owner_id
     0x00000002

   @command_id
     0x00000F01

   @param_id
     0x00000004

   @sys_call_params{tz_ocmem_lock_region_req_s}
     @table{weak__tz__ocmem__lock__region__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_LOCK_REGION       TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_OCMEM, 0x1)

#define TZ_OCMEM_LOCK_REGION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_4( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup ocmem_unlock_region

   System call to unlock an OCMEM region.

   @owner_id
     0x00000002

   @command_id
     0x00000F02

   @param_id
     0x00000003

   @sys_call_params{tz_ocmem_unlock_region_req_s}
     @table{weak__tz__ocmem__unlock__region__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_UNLOCK_REGION     TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_OCMEM, 0x2)

#define TZ_OCMEM_UNLOCK_REGION_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup ocmem_enable_mem_dump

   System call to enable memory dumping for an OCMEM region.

   @owner_id
     0x00000002

   @command_id
     0x00000F03

   @param_id
     0x00000003

   @sys_call_params{tz_ocmem_enable_mem_dump_req_s}
     @table{weak__tz__ocmem__enable__mem__dump__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_ENABLE_MEM_DUMP   TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_OCMEM, 0x3)

#define TZ_OCMEM_ENABLE_MEM_DUMP_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup ocmem_disable_mem_dump

   System call to disable memory dumping for an OCMEM region.

   @owner_id
     0x00000002

   @command_id
     0x00000F04

   @param_id
     0x00000003

   @sys_call_params{tz_ocmem_disable_mem_dump_req_s}
     @table{weak__tz__ocmem__disable__mem__dump__req__s}

   @com_struct
     tz_common_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_OCMEM_DISABLE_MEM_DUMP  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_OCMEM, 0x4)

#define TZ_OCMEM_DISABLE_MEM_DUMP_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tz_es_save_partition_hash

   Saves the eMMC partition kernel/system hash (SHA-256).
   This service is used by the LK to save the kernel partition hash
   and by the HLOS to save the system partition hash.

   @owner_id
     0x00000002

   @command_id
     0x00001001

   @param_id
     0x000000082

   @sys_call_req_params{tz_es_save_partition_hash_req_s}
     @table{weak__tz__es__save__partition__hash__req__s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_ES_SAVE_PARTITION_HASH_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_ES, 0x1)

#define TZ_ES_SAVE_PARTITION_HASH_ID_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_3( \
    TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
    TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup tz_es_save_partition_hash

   Saves the eMMC partition kernel/system hash (SHA-256).
   This service is used by the LK to save the kernel partition hash
   and by the HLOS to save the system partition hash.

   @owner_id
     0x00000002

   @command_id
     0x00001001

   @param_id
     0x000000082

   @sys_call_req_params{tz_es_save_partition_hash_req_s}
     @table{weak__tz__es__save__partition__hash__req__s}

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_ES_SAVE_PARTITION_HASH_LEGACY_ID TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_ES, 0x2)

#define TZ_ES_SAVE_PARTITION_HASH_LEGACY_ID_PARAM_ID TZ_SYSCALL_CREATE_PARAM_ID_2( \
    TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW )

/**
   @ingroup tz_hdcp_write_registers

   This service is used to write registers in the 
   HDCP HDMI Secure resource group. Additionally, whenever this syscall 
   is invoked, TZ will attempt to update the MMSS_HDMI_HDCP_CTRL2 register 
   to reflect if HDCP HDMI ecryption is to be enforced. 

   Five registers can be written to with one invocation of this syscall.  
   Note that the order in which these registers are written is not guanenteed, 
   so the caller should take care to invoke this syscall multiple times if 
   a certain order is required (as may be the case with the HWIO_MMSS_HDMI_HDCP_SHA_CTRL 
   and HWIO_MMSS_HDMI_HDCP_SHA_DATA registers).

   @sys_call_req_params{tz_hdcp_write_registers_req_s}
     @table{weak__tz__hdcp__write__registers__req__s}

   @owner_id
     0x00000002

   @command_id
     0x00001101

   @param_id
     0x0000000A

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZ_HDCP_WRITE_REGISTERS TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_HDCP, 0x1)

#define TZ_HDCP_WRITE_REGISTERS_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_10( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   @ingroup external_ee_passthrough

   Passthrough to third party secure execution environments.

   @owner_id
     0x00000002

   @command_id
     0x0000FA01

   @param_id
     0x000000A3

   @com_struct
     tz_syscall_req_s \n
     tz_syscall_rsp_s

   @return
     Returns any value from the QSEE.
*/
#define TZ_EXECUTIVE_EXT_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_MOBI_OS, TZ_SVC_EXECUTIVE_EXT, 0x01)

#define TZ_EXECUTIVE_EXT_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
   DEPRECATED SYSCALL: DO NOT REUSE
*/
#define TZ_EXECUTIVE_ID                                \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_EXECUTIVE, 0x01)

#define TZ_EXECUTIVE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

#ifdef _MSC_VER
#define __attribute__(x)
#pragma pack(push, 1)
#endif
/** @addtogroup tz_datatypes
@{ */

/**
   TODO Remove once legacy interface is removed
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
} __attribute__ ((packed)) tz_syscall_req_t;


/**
   TODO Remove once legacy interface is removed
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
} __attribute__ ((packed)) tz_syscall_rsp_t;

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
} __attribute__ ((packed)) tz_set_boot_addr_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_set_milestone_s
@{
*/
typedef struct tz_set_milestone_s
{
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

} __attribute__ ((packed)) tz_set_milestone_t;

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
  hlos_boot_params_t hlos_boot_info;
                                   /**< hlos boot info. */
} __attribute__ ((packed)) tz_do_modeswitch_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_secure_wdog_disable_req_s
@{
*/
typedef struct tz_secure_wdog_disable_req_s
{
  BOOLEAN disable;                 /**< If TRUE, the secure WDOG is
                                        disabled; enabled otherwise. */
} __attribute__ ((packed)) tz_secure_wdog_disable_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_secure_wdog_trigger_req_s
@{
*/
typedef struct tz_secure_wdog_trigger_req_s
{
  BOOLEAN disable_debug;           /**< If TRUE, the watchdog debug image
                                        is disabled (i.e., it will not run
                                        after a watchdog reset). */
} __attribute__ ((packed)) tz_secure_wdog_trigger_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lock_xpu_s
@{
*/
typedef struct tz_lock_xpu_s
{
  UINT32 elf_header_addr;          /**< Address to which the QSEE will
                                   extract elf header associated with
                                   proc id given as second argument. */
  UINT32 program_header;

  UINT32 proc_id;                  /**< Proc id associated with elf header  */

  UINT32 dummy;
} __attribute__ ((packed)) tz_lock_xpu_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_hw_for_ram_dump_req_s
@{
*/
typedef struct tz_config_hw_for_ram_dump_req_s
{
  UINT32 disable_wd_dbg;           /**< If TRUE, the watchdog debug image
                                        is disabled (i.e., it will not run
                                        after a watchdog reset). */

  UINT32 boot_partition_sel;       /**< Value programmed into the
                                        BOOT_PARTION_SELECT register. */
} __attribute__ ((packed)) tz_config_hw_for_ram_dump_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_force_dload_mode_req_s
@{
*/
typedef struct tz_force_dload_mode_req_s
{
  UINT32 config;                  /**< Value programmed into the
                                        TCSR_BOOT_MISC_DETECT register. */

  UINT32 spare;                  /**< Spare */
} __attribute__ ((packed)) tz_force_dload_mode_req_t;

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
  UINT32 state;                    /**< Requested video state, one
                                        of #tz_video_state_e_type*/

  UINT32 spare;                    /**< Reserved for future use.
                                        Should be zero */
} __attribute__ ((packed)) tz_video_set_state_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_hw_for_ram_dump_req_s
@{
*/
typedef struct tz_spin_with_irqs_fiqs_disabled_s
{
  UINT32 spare;                       /**< Reserved for future use. */
} __attribute__ ((packed)) tz_spin_with_irqs_fiqs_disabled_t;

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
  UINT32               proc;           /**< Processor identifier for
                                            the current operation.
                                            See #tz_pil_proc_e_type. */
  UINT8*               image_hdr_ptr;  /**< Pointer to the physical
                                            address location of the
                                            ELF header for the given
                                            processor identifier. */
} __attribute__ ((packed)) tz_pil_init_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_req_s
@{
*/
typedef struct tz_pil_mem_req_s
{
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
} __attribute__ ((packed)) tz_pil_mem_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_auth_reset_req_s
@{
*/
typedef struct tz_pil_auth_reset_req_s
{
  UINT32               proc;        /**< Processor identifier for the
                                         current operation. See
                                         #tz_pil_proc_e_type. */
} __attribute__ ((packed)) tz_pil_auth_reset_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_unlock_req_s
@{
*/
typedef struct tz_pil_unlock_req_s
{
  UINT32               proc;        /**< Processor ientifier for the
                                         current operation. See
                                         #tz_pil_proc_e_type. */
} __attribute__ ((packed)) tz_pil_unlock_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_subsystem_query_req_s
@{
*/
typedef struct tz_pil_subsystem_query_req_s
{
  UINT32               proc;        /** Processor/subsystem
                                        identifier. See
                                        #tz_pil_proc_e_type. */

} __attribute__ ((packed)) tz_pil_subsystem_query_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_subsystem_query_rsp_s
@{
*/
typedef struct tz_pil_subsystem_query_rsp_s
{
  UINT32               verdict;     /**< TRUE (1) if the answer to
                                         the query is true; FALSE
                                         (0) otherwise. */
} __attribute__ ((packed)) tz_pil_subsystem_query_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_area_query_req_s
@{
*/
typedef struct tz_pil_mem_area_query_req_s
{
  UINT32               spare;       /**< Unused; should be zero.   */

} __attribute__ ((packed)) tz_pil_mem_area_query_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_mem_area_query_rsp_s
@{
*/
typedef struct tz_pil_mem_area_query_rsp_s
{
  UINT32               start;       /**< Start address of relocatable
                                         PIL region (inclusive). */

  UINT32               end;         /**< End address of relocatable
                                         PIL region (exclusive). */
} __attribute__ ((packed)) tz_pil_mem_area_query_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_pil_modem_restart_s
@{
*/
typedef struct tz_pil_modem_restart_s
{
  UINT32 config;                  /**< Value programmed into the
                                        MSS_RESTART register. */

  UINT32 spare;                  /**< Spare */
} __attribute__ ((packed)) tz_pil_modem_restart_t;

/** @} */ /* end_weakgroup */



/**
   @weakgroup weak_ tz_pil_share_memory_s
@{
*/
typedef struct tz_pil_share_memory_s
{
  struct  tz_syscall_req_s  common_req;
  UINT32  start;       /**< Start address of the memory. start=0 and size=0 means share full HLOS memory */
  UINT32  size;         /**< length of the memory. start=0 and size=0 means share full HLOS memory */
  UINT32  proc;        /**< Processor identifier for the current operation. See #tz_pil_proc_e_type. */
  UINT32  share_type;   /**< a bitwise OR of the TZ_PIL_SHARE_TYPE_* definitions. 
                            Zero to revoke sharing of previously shared memory. */
  struct  tz_syscall_rsp_s  common_rsp;
} __attribute__ ((packed)) tz_pil_share_memory_t;
/** @} */ /* end_weakgroup */

#define TZ_PIL_SHARE_TYPE_READ 0x1
#define TZ_PIL_SHARE_TYPE_WRITE 0x2


/*----------------------------------------------------------------------------
 * DUMP Service Structures
 * -------------------------------------------------------------------------*/
/**
   @weakgroup weak_tz_pil_qdsp6_nmi_req_s
@{
*/
typedef struct tz_pil_qdsp6_nmi_req_s
{
  UINT32               val;         /**< Value for the LPASS Hexagon
                                         NMI register. Bits other
                                         than the least significant
                                         one are ignored by the
                                         call. */
} __attribute__ ((packed)) tz_pil_qdsp6_nmi_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_cpu_ctx_buf_req_s
@{
*/
typedef struct tz_dump_set_cpu_ctx_buf_req_s
{
  UINT32*              addr;        /**< Address of the WDT reset
                                         context dump buffer. When a
                                         nonsecure WDT expires, the
                                         CPU contexts are saved to
                                         this buffer. The entire
                                         buffer must be in nonsecure
                                         memory. */

  UINT32               size;        /**< Size of the WDT reset context
                                         dump buffer in bytes. */
} __attribute__ ((packed)) tz_dump_set_cpu_ctx_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_cache_buf_req_s
@{
*/
typedef struct tz_dump_set_cache_buf_req_s
{
  struct tz_syscall_req_s  common_req;
  UINT32*              addr;        /**< Address of L1 or L2 dump buffer.

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
} __attribute__ ((packed)) tz_dump_set_cache_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_cache_buf_size_req_s
@{
*/
typedef struct tz_dump_query_cache_buf_size_req_s
{
  struct tz_syscall_req_s  common_req;
  UINT32               spare;       /**< Spare field for future use;
                                         must be zero. */
} __attribute__ ((packed)) tz_dump_query_cache_buf_size_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_cache_buf_size_rsp_s
@{
*/
typedef struct tz_dump_query_cache_buf_size_rsp_s
{
  struct tz_syscall_rsp_s  common_rsp;
  UINT32               size;        /**< After a successfull call, the L1/L2
                                         dump size in bytes is stored
                                         here. */
} __attribute__ ((packed)) tz_dump_query_cache_buf_size_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_set_ocmem_buf_req_s
@{
*/
typedef struct tz_dump_set_ocmem_buf_req_s
{
  UINT32*                       addr;
    /**< Address of OCMEM dump buffer. When a nonsecure WDT as an FIQ
         expires, or if the nonsecure side requests OCMEM dumping, OCMEM
         data is saved to this buffer.

         The entire buffer must be in nonsecure memory and must be 4KB aligned. */

  UINT32                        size;
    /**< Size of the dump buffer in bytes. */
} __attribute__ ((packed)) tz_dump_set_ocmem_buf_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_ocmem_buf_size_req_s
@{
*/
typedef struct tz_dump_query_ocmem_buf_size_req_s
{
  UINT32               spare;       /**< Spare field for future use;
                                         must be zero. */
} __attribute__ ((packed)) tz_dump_query_ocmem_buf_size_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_dump_query_ocmem_buf_size_rsp_s
@{
*/
typedef struct tz_dump_query_ocmem_buf_size_rsp_s
{
  UINT32                        size;        /**< After a successfull call, the
                                                  OCMEM dump size in bytes is
                                                  stored here. */
} __attribute__ ((packed)) tz_dump_query_ocmem_buf_size_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_security_allows_mem_dump_rsp_s
@{
*/
typedef struct tz_security_allows_mem_dump_rsp_s
{
  UINT32                      allowed;
                                    /**< If non-zero, the security policy
                                         permits dumping memory for
                                         debugging. */
} __attribute__ ((packed)) tz_security_allows_mem_dump_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_security_is_retail_unlock_rsp_s
@{
*/
typedef struct tz_security_is_retail_unlock_rsp_s
{
  UINT32                      enabled;
                                    /**< If non-zero, the retail unlock
                                         is enable. */
} __attribute__ ((packed)) tz_security_is_retail_unlock_rsp_t;

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
  UINT32               res;         /**< Resource to lock. See
                                         #TZ_XPU_RESOURCE_ET. */

  UINT32               lock;        /**< Set to 1 to lock the resource
                                         and 0 to unlock it. */
} __attribute__ ((packed)) tz_lock_resource_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_lock_resource_rsp_s
@{
*/
typedef struct tz_lock_resource_rsp_s
{
  INT32                ret;         /**< Return code: 0 means success;
                                         E_BUSY means the resource is
                                         locked; other negative
                                         values mean failure. */
} __attribute__ ((packed)) tz_lock_resource_rsp_t;

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
  UINT32               res;         /**< Resource to configure. See
                                         #TZ_XPU_RESOURCE_ET. */

  UINT32               arg;         /**< Argument to the resource
                                         configuration. */
} __attribute__ ((packed)) tz_config_resource_req_t;

/** @} */ /* end_weakgroup */

/** @addtogroup tz_datatypes
@{ */

/* Enumerate BLSP subsystem owners */
typedef enum
{
  tzbsp_blsp_owner_none = 0,
  tzbsp_blsp_owner_apss = 1,
  tzbsp_blsp_owner_adsp = 2
} tzbsp_blsp_owner_ss_type;
/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_modify_ownership_req_s
@{
*/
typedef struct tz_modify_blsp_ownership_req_s
{
  UINT32                   blsp_periph_num;         
                                    /* blsp peripheral number. */
  tzbsp_blsp_owner_ss_type    owner_ss;         
                                    /* Owner subsystem. See tzbsp_blsp_owner_ss_type */
} __attribute__ ((packed)) tz_modify_blsp_ownership_req_t;
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
  UINT32               id;          /**< Command ID of the queried service. */
} __attribute__ ((packed)) tz_info_is_svc_available_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_info_is_svc_available_rsp_s
@{
*/
typedef struct tz_info_is_svc_available_rsp_s
{
  BOOLEAN              found;       /**< If TRUE, the searched service
                                         was found; FALSE otherwise. */
} __attribute__ ((packed)) tz_info_is_svc_available_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_info_get_diag_req_s
@{
*/
typedef struct tz_info_get_diag_req_s
{
  UINT8*               buffer;      /**< Pointer to the buffer to where the
                                         QSEE software copies the
                                         diagnostic area. */
  UINT32               buffer_size; /**< Size of the buffer in bytes.
                                         This must be at least the
                                         size of the diagnostic
                                         area. If the buffer is larger
                                         than the diagnostic area, the
                                         remaining bytes are ignored
                                         by the QSEE software. */
} __attribute__ ((packed)) tz_info_get_diag_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_feature_version_req_s
@{
*/
typedef struct tz_feature_version_req_s
{
  UINT32               feature_id;  /**< Feature for which to request
                                         the version information.
                                         See #tz_feature_t. */
} __attribute__ ((packed)) tz_feature_version_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_feature_version_rsp_s
@{
*/
typedef struct tz_feature_version_rsp_s
{
  UINT32               version;     /**< Version of the specified
                                         feature, or zero if the
                                         feature is undefined. */
} __attribute__ ((packed)) tz_feature_version_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_get_secure_state_req_s
@{
*/
typedef struct tz_get_secure_state_req_s
{
  UINT32*                       status_ptr;  /**< Pointer to the query status struct. */
  UINT32                        status_len;  /**< Length of the query status struct. */
}__attribute__ ((packed)) tz_get_secure_state_req_t;

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
  UINT32               img_ptr;     /**< Double pointer to the image. */
  UINT32               img_len_ptr; /**< Pointer to the length of the
                                         image. */
} __attribute__ ((packed)) tz_ssd_decrypt_img_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_encrypt_keystore_req_s
@{
*/
typedef struct tz_ssd_encrypt_keystore_req_s
{
  UINT32               keystore_ptr;      /**< Double pointer to the
                                               keystore. */
  UINT32               keystore_len_ptr;  /**< Pointer to the length
                                               of the keystore. */
} __attribute__ ((packed)) tz_ssd_encrypt_keystore_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_protect_keystore_req_s
@{
*/
typedef struct tz_ssd_protect_keystore_req_s
{
  UINT32*                       keystore_ptr;  /**< Pointer to SSD keystore. */
  UINT32                        keystore_len;  /**< Length of the keystore. */
} __attribute__ ((packed)) tz_ssd_protect_keystore_req_t;

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
  UINT32                        status;       /**< Status of the load keystore
                                                   call.  See
                                                   #tz_ssd_pks_return_type.
                                               */
} __attribute__ ((packed)) tz_ssd_protect_keystore_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_parse_md_req_s
@{
*/
typedef struct tz_ssd_parse_md_req_s
{
  UINT32                        md_len;       /**< Length of the metadata. */
  UINT32*                       md;           /**< Pointer to the metadata to
                                                   parse. The memory must be
                                                   word-aligned, as the type
                                                   implies. */
} __attribute__ ((packed)) tz_ssd_parse_md_req_t;

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
  UINT32*                       md_end_ptr;   /**< Pointer to the first memory
                                                   after the SSD metadata.
                                                   This pointer is always
                                                   word-aligned. If the
                                                   image was encrypted all at
                                                   once, this is the
                                                   start of the encrypted
                                                   data. */
} __attribute__ ((packed)) tz_ssd_parse_md_rsp_info_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_ssd_parse_md_rsp_s
@{
*/
typedef struct tz_ssd_parse_md_rsp_s
{
  struct tz_ssd_parse_md_rsp_info_s    rsp;   /**< Parse MD response
                                                   structure. */
} __attribute__ ((packed)) tz_ssd_parse_md_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_decrypt_img_frag_req_s
@{
*/
typedef struct tz_ssd_decrypt_img_frag_req_s
{
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
  UINT32*                       frag;         /**< Encrypted fragment.
                                                   The fragment must
                                                   be word-aligned, as
                                                   noted by the
                                                   UINT32* type. */
} __attribute__ ((packed)) tz_ssd_decrypt_frag_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ssd_decrypt_elf_seg_frag_req_s
@{
*/
typedef struct tz_ssd_decrypt_elf_seg_frag_req_s
{
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
  UINT32*                       frag;         /**< Encrypted fragment.
                                                   The fragment must
                                                   be word-aligned, as
                                                   noted by the
                                                   UINT32* type. */
} __attribute__ ((packed)) tz_ssd_decrypt_elf_seg_frag_req_t;

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
  UINT32                        status;       /**< Status of the decrypt
                                                   request.  See
                                                   #tz_ssd_df_return_type.
                                               */
} __attribute__ ((packed)) tz_ssd_decrypt_frag_rsp_t;

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
  UINT32               fuse_id;     /**< Fuse ID. See
                                         #tz_sw_fuse_t. */
} __attribute__ ((packed)) tz_sw_fuse_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_sw_fuse_rsp_s
@{
*/
typedef struct tz_sw_fuse_rsp_s
{
  BOOLEAN              fuse_blown;  /**< If TRUE, the specified fuse
                                         was blown; FALSE
                                         otherwise. */
} __attribute__ ((packed)) tz_sw_fuse_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_write_row_s
@{
*/
typedef struct tz_qfprom_write_row_s
{
  UINT32               raw_row_address;    /**< Raw row address of the fuse
                                                to blow. */
  UINT32*              row_data;           /**< Pointer to the data buffer. */
  UINT32               bus_clk_khz;        /**< Bus clock speed in KHz. */
  UINT32*              qfprom_api_status;  /**< Status code from the QFPROM. */
} __attribute__ ((packed)) tz_qfprom_write_row_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_write_multiple_rows_s
@{
*/
typedef struct tz_qfprom_write_multiple_rows_s
{
  UINT32*              row_array;          /**< Row array. */
  UINT32               row_array_size;     /**< Size of the row array. */
  UINT32               bus_clk_khz;        /**< Bus clock in KHz. */
  UINT32*              qfprom_api_status;  /**< Status code from the QFPROM. */
} __attribute__ ((packed)) tz_qfprom_write_multiple_rows_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_read_row_s
@{
*/
typedef struct tz_qfprom_read_row_s
{
  UINT32               row_address;        /**< Row address of the fuse to
                                                read. */
  INT32                addr_type;          /**< Indicates whether the address
                                                is raw corrected. */
  UINT32*              row_data;           /**< Pointer to the buffer that is
                                                to hold the read data. */
  UINT32*              qfprom_api_status;  /**< Status code from the QFPROM. */
} __attribute__ ((packed)) tz_qfprom_read_row_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_qfprom_rollback_write_row_s
@{
*/
typedef struct tz_qfprom_rollback_write_row_s
{
  UINT32               raw_row_address;    /**< Raw row address of the fuse
                                                to blow. */
  UINT32*              row_data;           /**< Pointer to the data buffer. */
  UINT32               bus_clk_khz;        /**< Bus clock in KHz. */
  UINT32*              qfprom_api_status;  /**< Status code from the QFPROM. */
} __attribute__ ((packed)) tz_qfprom_rollback_write_row_t;

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
  UINT8*                out_buf;     /**< Pointer to the buffer to fill
                                          with data from the PRNG. */
  UINT32                out_buf_sz;  /**< Size of the output buffer. */
} __attribute__ ((packed)) tz_get_prng_data_t;

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
  UINT32                ssid;        /**< Subsystem ID. See
                                          #tz_sc_ss_e_type. */
  void*                 address_ptr; /**< Pointer to the address location. */
  UINT32                len;         /**< Length of the allocated memory. */
  int*                  status_ptr;  /**< Return status. */
} __attribute__ ((packed)) tz_sc_init_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_sc_deinit_s
@{
*/
typedef struct tz_sc_deinit_s
{
  UINT32               ssid;          /**< Subsystem ID. */
  int*                 status_ptr;    /**< Return status. */
} __attribute__ ((packed)) tz_sc_deinit_t;

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
  UINT32               start;       /**< Start address of the range. */
  UINT32               end;         /**< End address of the range. */
  UINT32               tag_id;      /**< Tag to determine the
                                         permissions. */
  BOOLEAN              lock;        /**< TRUE to protect; FALSE
                                         to unprotect. */
} __attribute__ ((packed)) tz_protect_memory_t;

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
  UINT32                        device;
                                          /**< Device to be restored;
                                               see #tz_device_e_type. */
  UINT32                        spare;       /**< Spare. */
} __attribute__ ((packed)) tz_restore_sec_cfg_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_set_vmidmt_memtype_req_s
@{
*/
typedef struct tz_set_vmidmt_memtype_req_s
{
  UINT32                        device;
                                             /**< Device to be configured;
                                               see #tz_device_e_type. */
  UINT32                        vmid_idx;    /**< Index for the memtype to be set;
                                               0xFF for all indecies. */
  UINT32                        memtype;     /**< Memory type value to be set. */
} __attribute__ ((packed)) tz_set_vmidmt_memtype_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_video_var_req_s
@{
*/
typedef struct tz_memprot_video_var_req_s
{
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
} __attribute__ ((packed)) tz_memprot_video_var_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_dev_var_req_s
@{
*/
typedef struct tz_memprot_dev_var_req_s
{
  UINT32                     device_id;
  /**< ID of the device where this memory is to be mapped.
       See #tz_device_e_type. */
  tz_va_range_t              *va_list;
    /**< a list of start address and size of the secure memory ranges in the device's
         virtual address space. */
  UINT32                     list_size;
    /**< Size of VA range list. */
} __attribute__ ((packed)) tz_memprot_dev_var_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_size_req_s
@{
*/
typedef struct tz_memprot_ptbl_mem_size_req_s
{
   UINT32                        spare;      /**< Spare field for future use,
                                                  must be zero. */
} __attribute__ ((packed)) tz_memprot_ptbl_mem_size_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_size_rsp_s
@{
*/
typedef struct tz_memprot_ptbl_mem_size_rsp_s
{
  UINT32                       mem_size;    /**< Required size of the
                                                 page table memory
                                                 region in bytes. */
  INT32                        ret;         /**< Extended return code; 0 for
                                                 success; negative values
                                                 mean failure. */
} __attribute__ ((packed)) tz_memprot_ptbl_mem_size_rsp_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_ptbl_mem_init_req_s
@{
*/
typedef struct tz_memprot_ptbl_mem_init_req_s
{
  UINT32                        addr;       /**< Start address of the page
                                                 table memory region
                                                 (16 KB aligned).  */
  UINT32                        size;       /**< Size of page table memory
                                                 region in bytes. */
  UINT32                        spare;      /**< Spare field for future use;
                                                 must be zero. */
} __attribute__ ((packed)) tz_memprot_ptbl_mem_init_req_t;

/** @} */ /* end_weakgroup */


/**
   @weakgroup weak_tz_memprot_set_cp_pool_size_req_s
@{
*/
typedef struct tz_memprot_set_cp_pool_size_req_s
{
   UINT32                        size;       /**< CP pool size */
   UINT32                        spare;      /**< Spare field for future use,
                                                  must be zero. */
} __attribute__ ((packed)) tz_memprot_set_cp_pool_size_req_t;

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
  TZ_MEM_USAGE_CP_SD               = 0x4, /**< Memory used for Secure
                                                  Display data. */
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
  UINT32*             chunk_list;     /**< Array of chunk addresses. Each entry
                                           is a 32-bit physical address.
                                           chunk_list_size gives the number of
                                           entries in the array. */
  UINT32              chunk_list_size;/**< Number of chunks in the chunk
                                           list.*/
  UINT32              chunk_size;     /**< Size of each memory chunk (in bytes)
                                           in the chunk list. */
} __attribute__ ((packed)) tz_memprot_chunks_info_t;

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
} __attribute__ ((packed)) tz_memprot_map_info_t;

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
  tz_memprot_chunks_info_t      chunks;    /**< Memory chunk list. */
  UINT32                        mem_usage; /**< Hint on the memory usage.
                                                See #tz_mem_usage_e_type. */
  UINT32                        lock;      /**< 1 (TRUE) to lock (protect);
                                                0 (FALSE) to unlock (unprotect). */
  UINT32                        flags;     /**< Flags field for future use;
                                                must be zero when no flags are
                                                set. */
} __attribute__ ((packed)) tz_memprot_lock2_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_map2_req_s
@{
*/
typedef struct tz_memprot_map2_req_s
{
  tz_memprot_chunks_info_t       chunks;    /**< Memory chunk list. */
  tz_memprot_map_info_t          map;       /**< Map-to information. */
  UINT32                         flags;     /**< Flags field, which is a bitwise
                                                 OR of TZ_MEMPROT_FLAG_* flags.
                                                 Currently supported flags:
                                                 TLBINVALIDATE \n
                                                 Must be zero when no flags are
                                                 set. */
} __attribute__ ((packed)) tz_memprot_map2_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_unmap2_req_s
@{ */
typedef struct tz_memprot_unmap2_req_s
{
  tz_memprot_map_info_t         map;        /**< Mapped-at information. */
  UINT32                        flags;      /**< Flags field, which is a bitwise
                                                 OR of TZ_MEMPROT_FLAG_* flags.
                                                 Currently supported flags:
                                                 TLBINVALIDATE \n
                                                 Must be zero when no flags are
                                                 set. */
} __attribute__ ((packed)) tz_memprot_unmap2_req_t;

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
  INT32                        ret;        /**< Extended return
                                                code; 0 for success;
                                                negative values
                                                mean failure. */
} __attribute__ ((packed)) tz_common_rsp_t;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_memprot_tlbinval_req_s
@{ */
typedef struct tz_memprot_tlbinval_req_s
{
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
} __attribute__ ((packed)) tz_memprot_tlbinval_req_t;

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
  tz_xpu_violation_err_fatal_t       config;     /**< Enable, disable, or NOOP.
                                                      See tz_xpu_violation_err_fatal_t. */
  UINT32                             spare;      /**< Unused. */
} __attribute__ ((packed)) tz_xpu_violation_err_fatal_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_xpu_violation_err_fatal_rsp_s
@{
*/
typedef struct tz_xpu_violation_err_fatal_rsp_s
{
  tz_xpu_violation_err_fatal_t status   ;     /**< Enable or disable. */
} __attribute__ ((packed)) tz_xpu_violation_err_fatal_rsp_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_memprot_sd_ctrl_req_s
@{ */
typedef struct tz_memprot_sd_ctrl_req_s
{
  tz_syscall_req_t      common_req;   /**< Common request structure. */
  UINT32                enable;    /**< 1 to enable, 0 to disable */
} __attribute__ ((packed)) tz_memprot_sd_ctrl_req_t;

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
  UINT32                        client_id;   /**< OCMEM client requesting the
                                                  memory region. See
                                                  #tz_ocmem_client_e_type. */

  UINT32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  UINT32                        size;        /**< Size of the requested region. */

  UINT32                        mode;        /**< Requested mode (wide or
                                                  narrow). See
                                                  #tz_ocmem_op_mode_e_type. */
}__attribute__ ((packed)) tz_ocmem_lock_region_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_unlock_region_req_s
@{
*/
typedef struct tz_ocmem_unlock_region_req_s
{
  UINT32                        client_id;   /**< OCMEM client that currently
                                                  owns the region. See
                                                  #tz_ocmem_client_e_type. */

  UINT32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  UINT32                        size;        /**< Size of requested region. */
}__attribute__ ((packed)) tz_ocmem_unlock_region_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_enable_mem_dump_req_s
@{
*/
typedef struct tz_ocmem_enable_mem_dump_req_s
{
  UINT32                        client_id;   /**< OCMEM client that currently
                                                  has this region locked. See
                                                  #tz_ocmem_client_e_type. */

  UINT32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  UINT32                        size;        /**< Size of the region. */
}__attribute__ ((packed)) tz_ocmem_enable_mem_dump_req_t;

/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_ocmem_disable_mem_dump_req_s
@{
*/
typedef struct tz_ocmem_disable_mem_dump_req_s
{
  UINT32                        client_id;   /**< OCMEM client that currently
                                                  has this region locked. See
                                                  #tz_ocmem_client_e_type */

  UINT32                        offset;      /**< Start offset of the memory
                                                  region in OCMEM. */

  UINT32                        size;        /**< Size of the region. */
}__attribute__ ((packed)) tz_ocmem_disable_mem_dump_req_t;

/** @} */ /* end_weakgroup */
 
/** @addtogroup tz_datatypes
@{ */ 
 
/** eMMC partition ID for the enterprise security servcie save hash. */
typedef enum
{
    TZ_ES_PARTITION_ID_KERNEL = 0,       /**< Kernel partition ID. */
    TZ_ES_PARTITION_ID_SYSTEM = 1,       /**< System partition ID. */
    TZ_ES_PARTITION_ID_RECOVERY = 2,     /**< Recovery partition ID. */
    TZ_ES_MAX_PARTITIONS                 /**< Maximum number of partitions. */
}tz_partition_id_e_type;

/** @} */ /* end_addtogroup tz_datatypes */

/**
   @weakgroup weak_tz_es_save_partition_hash_req_s
@{
*/
typedef struct tz_es_save_partition_hash_req_s
{
  UINT32                        partition_id;
                                /**< Partition ID (system, recovery, or kernel).
                                     See tz_partition_id_e_type. */

  UINT8                         hash[32]; /**< Hash digest for SHA-256. */
}__attribute__ ((packed)) tz_es_save_partition_hash_req_t;

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
  const UINT32* vector; /**< Base address of the nonsecure FIQ handler. The
                             handler executes in nonsecure SVC mode. */
  const UINT32* stack; /**< Stack for the nonsecure FIQ handler. The
                            QSEE software never modifies the stack. */
  UINT32* ctx_buf; /**< Context buffer for the nonsecure FIQ handler. Passed
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
} __attribute__ ((packed)) tz_config_ns_debug_fiq_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_ctx_size_rsp_s
@{
*/
typedef struct tz_config_ns_debug_fiq_ctx_size_rsp_s
{
  UINT32 size; /**< Context size in bytes. */
} __attribute__ ((packed)) tz_config_ns_debug_fiq_ctx_size_rsp_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_int_ok_req_s
@{
*/
typedef struct tz_config_ns_debug_fiq_int_ok_req_s
{
  UINT32 intnum; /**< QGIC interrupt number. */
} __attribute__ ((packed)) tz_config_ns_debug_fiq_int_ok_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_config_ns_debug_fiq_int_ok_rsp_s
@{
*/
typedef struct tz_config_ns_debug_fiq_int_ok_rsp_s
{
  UINT32 verdict; /**< Contains 1 if the interrupt is supported as an FIQ
                       debug interrupt; 0 otherwise. */
} __attribute__ ((packed)) tz_config_ns_debug_fiq_int_ok_rsp_t;
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
  UINT32                        id;   /**< ID of the configured item. */
  UINT32                        arg0; /**< Generic argument 0. */
  UINT32                        arg1; /**< Generic argument 1. */
  UINT32                        arg2; /**< Generic argument 2. */
} __attribute__ ((packed)) tz_cpu_config_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_cpu_config_query_req_s
@{
*/
typedef struct tz_cpu_config_query_req_s
{
  UINT32                        id;   /**< ID of the queried item. */
  UINT32                        arg0; /**< Generic argument 0. */
  UINT32                        arg1; /**< Generic argument 1. */
  UINT32                        arg2; /**< Generic argument 2. */
} __attribute__ ((packed)) tz_cpu_config_query_req_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_cpu_config_query_rsp_s
@{
*/
typedef struct tz_cpu_config_query_rsp_s
{
  UINT32                        ret0; /**< Generic return value 0. */
  UINT32                        ret1; /**< Generic return value 1. */
  UINT32                        ret2; /**< Generic return value 2. */
  UINT32                        ret3; /**< Generic return value 3. */
} __attribute__ ((packed)) tz_cpu_config_query_rsp_t;
/** @} */ /* end_weakgroup */

/**
   @weakgroup weak_tz_hdcp_write_registers_req_s
@{
*/
typedef struct tz_hdcp_write_registers_req_s
{
  UINT32                        addr1;     /**< Address of register to write. */  
  UINT32                        value1;    /**< Value to be written. */
  UINT32                        addr2;     
  UINT32                        value2;  
  UINT32                        addr3;     
  UINT32                        value3;   
  UINT32                        addr4;     
  UINT32                        value4; 
  UINT32                        addr5;     
  UINT32                        value5;     
} __attribute__ ((packed)) tz_hdcp_write_registers_req_t;

/** @} */ /* end_weakgroup */
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif
