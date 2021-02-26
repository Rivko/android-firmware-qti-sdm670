#ifndef BOOT_FASTCALL_TZ_H
#define BOOT_FASTCALL_TZ_H

/*=============================================================================

                    Boot tz function header File

GENERAL DESCRIPTION
  This header file contains the public interface for tz functions
          
Copyright 2010-2011, 2013 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
02/10/17   kpa      Update Partial hash api id
11/09/16   kpa      Added smc ID's to support partial hash apis
11/23/16   kedara   Added macros for smc status define 
11/09/16   digant   TZ_SECBOOT_STATUS_PARAM_ID and TZ_SECBOOT_STATUS_CMD
06/23/16   digant   reduce XPU violation buffer to 512 bytes
06/02/16   digant   XPU violation query & debug img exit param id, cmd id
04/11/16   kedara   Update boot_fastcall_scm_2arg
03/15/16   kedara   Added TZ_PIMEM_GET_STATUS_CMD, boot_fastcall_scm_2arg
12/15/15   plc      Update cmd/param ID's for milestone and add sec-img-auth ids
11/03/15   plc      Update SMC to armv8 standard spec
10/05/15   ck       Corrected copyright
02/11/14   kedara   Added TZ_OFFLINE_MEM_DUMP_CMD
09/25/13   aus      Added TZ_DEVICE_IS_RETAIL_UNLOCK_CMD for retail unlock check
02/19/13   kedara   Added TZ_SBL_XPU_LOCK to enable TZ xpu locking.
02/11/13   dh       Add TZ_ALLOWS_MEM_DUMP_CMD for ram dump permission check
11/03/11   kedara   Added smc commands to write-multiple, write and read fuses.
08/30/10   kedara   Initial version.

===========================================================================*/


/*===========================================================================
                      TZ fastcall DEFINES
===========================================================================*/ 

/* Return status value from smc api */
#define SMC_SUCCESS 0x0
#define SMC_ERR_SYSCALL_FAILED -2L

/*  Tz service ID computation:
Service Id followed by function id ((uint32)(((s & 0xff) << 8) | (f & 0xff)))
Command Id encodes number of parameters and permissions of each, as provided 
by TZ team.
*/ 
#define TZ_WRITE_FUSE_CMD 0x82000803
#define TZ_WRITE_FUSE_CMD_PARAM_ID  0x884

#define TZ_WRITE_MULT_FUSE_CMD 0x82000804
#define TZ_WRITE_MULT_FUSE_CMD_PARAM_ID  0x824

#define TZ_READ_FUSE_CMD 0x82000805
#define TZ_READ_FUSE_CMD_PARAM_ID  0xA04

#define TZ_WRITE_ROLLBACK_FUSE_CMD 0x82000806
#define TZ_WRITE_ROLLBACK_FUSE_CMD_PARAM_ID  0x884

#define TZ_SBL_XPU_LOCK 0x8200010B
#define TZ_SBL_XPU_LOCK_PARAM_ID  0x004

/* 
#define TZBSP_MILESTONE_SET_ID \
        TZBSP_SYSCALL_CREATE_CMD_ID(0x01, 0x0E)
*/ 
#define TZ_SBL_END_MILESTONE 0x82000114
#define TZ_SBL_END_MILESTONE_PARAM_ID  0x022

/* 
 tz ram dump permission check command
#define TZBSP_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID \
        TZBSP_SYSCALL_CREATE_CMD_ID(0x03, 0x10)
*/ 
#define TZ_ALLOWS_MEM_DUMP_CMD  0x82000310
#define TZ_ALLOWS_MEM_DUMP_CMD_PARAM_ID  0x0

/* 
 tz retail unlock check command
#define TZ_DUMP_IS_RETAIL_UNLOCK_ID \
        TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_DUMP, 0x0D)
*/ 
#define TZ_DEVICE_IS_RETAIL_UNLOCK_CMD  0x8200030D
#define TZ_DEVICE_IS_RETAIL_UNLOCK_CMD_PARAM_ID  0x022

/* 
 For offline ram dumps
 TZBSP_SYSCALL_CREATE_CMD_ID(0x01, 0x09) 
*/ 
#define TZ_OFFLINE_MEM_DUMP_CMD  0x82000109
#define TZ_OFFLINE_MEM_DUMP_CMD_PARAM_ID  0x002

/*
 For image authentication
*/
#define TZ_PIL_SEC_IMG_VERIFY_METADATA_ID  0x82000210
#define TZ_PIL_SEC_IMG_VERIFY_METADATA_ID_PARAM_ID  0x21048

#define TZ_PIL_SEC_IMG_HASH_SEGMENTS_ID  0x82000211
#define TZ_PIL_SEC_IMG_HASH_SEGMENTS_ID_PARAM_ID  0x083


#define TZ_PIL_SEC_IMG_PARTIAL_HASH_ID  0x82000215
#define TZ_PIL_SEC_IMG_PARTIAL_HASH_ID_PARAM_ID  0x446


#define TZ_PIL_SEC_IMG_VALIDATE_ELF_ID  0x82000212
#define TZ_PIL_SEC_IMG_VALIDATE_ELF_ID_PARAM_ID  0x043

#define TZ_PIL_SEC_IMG_VALID_SEG_ID  0x82000213
#define TZ_PIL_SEC_IMG_VALID_SEG_ID_PARAM_ID  0x043

#define TZ_PIL_SECBOOT_VERIFY_SIG_ID  0x82000214
#define TZ_PIL_SECBOOT_VERIFY_SIG_ID_PARAM_ID  0x00008017

#define TZ_PIMEM_INIT_CMD  0x82000115
#define TZ_PIMEM_INIT_CMD_PARAM_ID  0x0
#define TZ_PIMEM_GET_STATUS_CMD  0x82000116

#define TZ_UNLOCK_CMD 0x82000118
#define TZ_UNLOCK_CMD_PARAM_ID 0x1

/* 
 Get status of secure boot for the device
*/
#define TZ_SECBOOT_STATUS_CMD  0x82000119
#define TZ_SECBOOT_STATUS_PARAM_ID  0x1

/*
 For query xpu violation in error handling
*/
#define TZ_QUERY_XPU_VIOLATION_ID 0x82000117
#define TZ_QUERY_XPU_VIOLATION_PARAM_ID 0x022
#define TZ_XPU_RESULT_BUFFER_SIZE 0x200


/* 
 Return back to QSEE from ddr self refresh apis
*/
#define TZ_DEBUG_IMG_EXIT_CMD  0x82000312
#define TZ_DEBUG_IMG_EXIT_PARAM_ID  0x1

/*
  Response buffer struct
*/

typedef struct 
{
  uint64 reg[3];
} boot_tzt_result_regs_t;

typedef struct
{
  uint64 arg[7];
} boot_tz_indirect_arg_list_t;

/*===========================================================================
                      TZ PIL XPU CONFIG DEFINES
===========================================================================*/

#define BOOT_TZBSP_PIL_AUTH_LPASS_QDSP6_PROC 0x1

/*===========================================================================
                             Macros
===========================================================================*/

#define boot_fastcall_tz(smc_id, param_id, arg0, arg1, arg2, arg3, resp_buf) \
  boot_scm((uintnt)(smc_id), (uintnt)(param_id), (uintnt)(arg0), \
  (uintnt)(arg1), (uintnt)(arg2), (uintnt)(arg3), 0, (resp_buf))

#define boot_fastcall_tz_no_rsp(smc_id, param_id, arg0, arg1, arg2, arg3) \
  boot_scm((uintnt)(smc_id), (uintnt)(param_id), (uintnt)(arg0), \
  (uintnt)(arg1), (uintnt)(arg2), (uintnt)(arg3), 0, NULL)

/*===========================================================================

**  Function : boot_fastcall_scm
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This api  makes a smc call to Tz module and passes 2 arguments
*    along with Tz service id and token. It also populates SMC
*    call response in buffer
*      
* @param[in] xxid: tz service Id
*            xx0, xx1: uint32 arguments passed to TZ
*            resp_buff_ptr: SMC call response values.
*  
* @par Dependencies
*    None
* 
* @retval
*    status of operation.
* 
* @par Side Effects
*    None
* 
*/
int boot_scm
( 
  uintnt smc_id,
  uintnt param_id,
  uintnt arg0,
  uintnt arg1,
  uintnt arg2,
  uintnt arg3,
  uintnt session_id,
  boot_tzt_result_regs_t* resp_buff_ptr
);

#endif /* BOOT_FASTCALL_TZ_H */
