#ifndef BOOT_ERROR_IF_H
#define BOOT_ERROR_IF_H

/*===========================================================================

                 Boot Loader Error Handler Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the boot
  error handler interface.
    
Copyright 2007-2012, 2014-2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/17   yps     Replaced Macro __FILE__ with __FILE_BASENAME__
06/01/17   daison  Added error code BL_ERR_DDI_SIZE_INVALID
06/24/16   kpa     Added error code BL_ERR_INVALID_DISPLAY_SIZE
06/23/16   digant  Added exception data,instruction,sp aborts code for aarch64
06/22/15   ck      Added error code BL_ERR_SHR_VAR_SIZE
06/22/15   ck      Added error codes BL_ERR_IMAGE_ARCH_UNKNOWN and BL_ERR_IMAGE_NOT_FOUND
06/22/15   as      Added error code BL_ERR_ECC_CONFIG
06/10/15   as      Added error code BL_ERR_DDR_SPD
03/25/15   ck      Added error code BL_ERR_ACCESS_CONTROL
10/29/14   ck      Added error code BL_ERR_INVALID_ENTRY_POINT
10/28/14   ck      Added HOB error codes
06/16/14   ck      Added error code BL_ERR_INT_OVERFLOW
05/09/14   kedara  Added error code BL_ERR_AARCH64_NOT_ENABLED, BL_ERR_MISALIGNMENT
                   BL_ERR_VALUE_TOO_LARGE, BL_ERR_ELF_CLASS_INVALID
04/01/14   wg      Added two new Authentication codes for 
                   SAFE 1.0 API migration
12/06/12   jz      Added error code BL_ERR_GET_CDT_FAIL
09/06/12   kpa     Added error code for Subsystem Self Authentication (ssa)
10/30/11   kpa     Added error code for Qfprom test framework  
09/20/11   kpa     Added error code for tpm hash population. 
09/13/11   kpa     Added error code for Rollback prevention feature 
04/07/11   dh      Added error code for DLOAD
08/04/10   aus     Added error code for invalid header
07/22/10   aus     Added error code for SBL error
07/15/10   dh      Modify BL_VERIFY to fix Klockwork error
06/23/10   plc     Add error code for fail to init PBL interface
05/26/10   plc     Added codes for secboot 3.0 merge, and include 
                   boot_comdef.h only if ! _ARM_ASM_
01/05/09   vtw     Added efs error codes.
09/22/09   vw      Added error codes for sd-boot support.
04/15/08   PS      Added enum type BL_ERR_IMG_SIZE_INVALID in bl_err_type
12/04/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef void (*boot_error_handler_ptr_type)
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      error             /* Enum that describes error type */
);

/*---------------------------------------------------------------------------
  Define the SBL ERROR types. This classifies different types of errors 
  encountered in the SBL.
 ---------------------------------------------------------------------------*/
typedef enum
{
  BL_ERROR_GROUP_BOOT               = 0x01000000,
  BL_ERROR_GROUP_BUSES              = 0x02000000,
  BL_ERROR_GROUP_BAM                = 0x03000000,
  BL_ERROR_GROUP_BUSYWAIT           = 0x04000000,
  BL_ERROR_GROUP_CLK                = 0x05000000,
  BL_ERROR_GROUP_CRYPTO             = 0x06000000,
  BL_ERROR_GROUP_DAL                = 0x07000000,
  BL_ERROR_GROUP_DEVPROG            = 0x08000000,
  BL_ERROR_GROUP_DEVPROG_DDR        = 0x10000000,
  BL_ERROR_GROUP_EFS                = 0x11000000,
  BL_ERROR_GROUP_EFS_LITE           = 0x12000000,
  BL_ERROR_GROUP_FLASH              = 0x13000000,
  BL_ERROR_GROUP_HOTPLUG            = 0x14000000,
  BL_ERROR_GROUP_HSUSB              = 0x15000000,
  BL_ERROR_GROUP_ICB                = 0x16000000,
  BL_ERROR_GROUP_LIMITS             = 0x17000000,
  BL_ERROR_GROUP_MHI                = 0x18000000,
  BL_ERROR_GROUP_PCIE               = 0x20000000,
  BL_ERROR_GROUP_PLATFORM           = 0x21000000,
  BL_ERROR_GROUP_PMIC               = 0x22000000,
  BL_ERROR_GROUP_POWER              = 0x23000000,
  BL_ERROR_GROUP_PRNG               = 0x24000000,
  BL_ERROR_GROUP_QUSB               = 0x25000000,
  BL_ERROR_GROUP_SECIMG             = 0x26000000,
  BL_ERROR_GROUP_SECBOOT            = 0x27000000,
  BL_ERROR_GROUP_SECCFG             = 0x28000000,
  BL_ERROR_GROUP_SMEM               = 0x30000000,
  BL_ERROR_GROUP_SPMI               = 0x31000000,
  BL_ERROR_GROUP_SUBSYS             = 0x32000000,
  BL_ERROR_GROUP_TLMM               = 0x33000000,
  BL_ERROR_GROUP_TSENS              = 0x34000000,
  BL_ERROR_GROUP_HWENGINES          = 0x35000000,
  BL_ERROR_GROUP_IMAGE_VERSION      = 0x36000000,
  BL_ERROR_GROUP_SECURITY           = 0x37000000,
  BL_ERROR_GROUP_STORAGE            = 0x38000000,
  BL_ERROR_GROUP_SYSTEMDRIVERS      = 0x40000000,
  BL_ERROR_GROUP_DDR                = 0x41000000,
  BL_ERROR_GROUP_EXCEPTIONS         = 0x42000000,
  BL_ERROR_GROUP_MPROC              = 0x43000000,
  BL_ERROR_GROUP_BOOT_SAHARA        = 0x44000000,
  BL_ERROR_GROUP_DISPLAY            = 0x45000000,
  BL_ERROR_GROUP_MAX                = 0x7FFFFFFF
} bl_error_group_type;

/*---------------------------------------------------------------------------
 ERRORS FROM BOOT
 ---------------------------------------------------------------------------*/
typedef enum
{
  BL_ERR_NONE,
  BL_ERR_NULL_PTR_PASSED                     = 0X0001,
  BL_ERR_MISALIGNMENT                        = 0X0002,
  BL_ERR_BUFFER_SIZE_TOO_SMALL               = 0X0003,
  BL_ERR_SUBSYS_BOOT_FAIL                    = 0X0004,
  
  BL_ERR_CODE_ROLLBACK_VERSION_VERIFY_FAIL   = 0X0005,
  BL_ERR_CODE_ROLLBACK_VERSION_BLOW_FAIL     = 0X0006,
  
  BL_ERR_AUTH_DISABLED                       = 0X0007,  
  BL_ERR_HASH_TBL_AUTH_FAIL                  = 0X0008,
  BL_ERR_IMG_SIZE_INVALID                    = 0X0010,
           
  BL_ERR_CDB_COPY_ERROR                      = 0X0011,
  BL_ERR_CDT_SIZE_INVALID                    = 0X0012,
  BL_ERR_CDT_PARAMS_SIZE_INVALID             = 0X0013,
  
  BL_ERR_CLOBBER_PROTECTION_VIOLATION        = 0X0014,
  BL_ERR_FLASH_READ_FAIL                     = 0X0015,
  BL_ERR_IMG_SIZE_TOO_BIG                    = 0X0016,
  BL_ERR_SDC_INVALID_SIZE                    = 0X0017,
  BL_ERR_SDCC_WRITE_FAIL                     = 0X0018,
  BL_ERR_SDC_INVALID_PARTI                   = 0X0020,
  
  BL_ERR_IMG_HASH_FAIL                       = 0X0021,
  BL_ERR_ELF_HASH_MISMATCH                   = 0X0022,
  BL_ERR_INVALID_HASH_PTR                    = 0X0023,
  BL_ERR_TPM_HASH_BLK_STR_FAIL               = 0X0024,
  BL_ERR_INVALID_SECDAT_BUFFER               = 0X0025,
  
  BL_ERR_FLASH_DRIVER                        = 0X0026,
  BL_ERR_BUFFER_SIZE_MISMATCH                = 0X0027,
  BL_ERR_BOOT_FLASH_ACCESS                   = 0X0028,
  BL_ERR_PARTI_TABLE_NOT_FOUND               = 0X0030,
  BL_ERR_PARTI_NAME_NOT_FOUND                = 0X0031,
  BL_ERR_PARTI_ENTRY_NOT_FOUND               = 0X0032,
  BL_ERR_WRG_PARTI_ENTRY_NAME_FMT            = 0X0033,
           
  BL_ERR_FAILED_TO_COPY_IMG_HDR              = 0X0034,        
  BL_ERR_ELF_HDR_READ_FAIL                   = 0X0035,
  BL_ERR_ELF_CLASS_INVALID                   = 0X0036,
  BL_ERR_ELF_INVAL_PARAM                     = 0X0037,
           
  BL_ERR_IMGID_NOT_FOUND                     = 0X0038,
  BL_ERR_IMG_HDR_INVALID                     = 0X0040,
  BL_ERR_IMG_NOT_FOUND                       = 0X0041,
  BL_ERR_IMG_SIZE_ZERO                       = 0X0042,
  BL_ERR_RESOURCE_NOT_AVAILABLE              = 0X0043,

  BL_ERR_MMU_PGTBL_MAPPING_FAIL              = 0X0044,
  BL_ERR_MMU_PGTBL_CPY_FAIL                  = 0X0045,
       
  BL_ERR_QFPROM_TEST_FRAMEWORK_FAIL          = 0X0046,
  BL_ERR_FAIL_ALLOCATE_SMEM                  = 0X0047,
  BL_ERR_INVALID_COOKIE_FILE_NAME            = 0X0048,
  BL_ERR_INVALID_ENTRY_POINT                 = 0X0050,
       
  BL_ERR_BUSY_WAIT_INIT_FAIL                 = 0X0051,
  BL_ERR_CPU_CLK_INIT_FAIL                   = 0X0052,
  BL_ERR_DISABLE_CLOCK_FAIL                  = 0X0053,
  BL_ERR_INIT_GPIO_FOR_TLMM_CONFIG_FAIL      = 0X0054,
     
  BL_ERR_SECBOOT_FTBL_INIT_FAIL              = 0X0055,
       
  BL_ERR_EFS_CLOSE_FAIL                      = 0X0056,
  BL_ERR_EFS_INVALID_HANDLE                  = 0X0057,
       
  BL_ERR_HOTPLUG                             = 0x0058,
  BL_ERR_RAM_DUMP_FAIL                       = 0x0060,
  BL_ERR_INVALID_FILE_NAME                   = 0x0061,
  BL_ERR_COOKIE_TOO_BIG                      = 0x0062,
  BL_ERR_UNSUPPORTED_IMG                     = 0x0063,
  BL_ERR_FEATURE_NOT_SUPPORTED               = 0x0064,
  BL_ERR_IMG_SECURITY_FAIL                   = 0x0065,
  BL_ERR_UNSUPPORTED_HASH_ALGO               = 0x0066,
  BL_ERR_NAND_TRANS_INTERFACE_NOT_FOUND      = 0x0067,
  BL_ERR_SDCC_TRANS_INTERFACE_NOT_FOUND      = 0x0068,
  BL_ERR_INVALID_SECTOR_SIZE                 = 0x0070,
  BL_ERR_GET_CDT_FAIL                        = 0x0071,
  BL_ERR_OTHERS                              = 0x0072,
  BOOT_PRE_DDR_CLK_INIT_FAIL                 = 0x0073,
  BL_ERR_INTEGER_OVERFLOW                    = 0x0074,
  BL_ERR_VSENS_INIT_FAIL                     = 0x0075,
  BL_ERR_VSENSE_COPY_TO_SMEM_FAIL            = 0x0076,
  BL_ERR_IMAGE_ARCH_UNKNOWN                  = 0x0077,
          
  BL_ERR_HOB_CREATE_FAIL                     = 0X0078,
  BL_ERR_HOB_STORE_FAIL                      = 0X0080,
  BL_ERR_HOB_BOOT_MODE_FAIL                  = 0X0081,
  BL_ERR_HOB_GUID_FAIL                       = 0X0082,
          
  BL_ERR_BOOT_SAHARA_INIT_FAIL               = 0X0083,
  BL_ERR_CORE_VERIFY                         = 0X0084,
  BL_ERR_INVALID_ELF_FORMAT                  = 0x0085,
  BL_ERR_INVALID_DISPLAY_SIZE                = 0x0086,
  
  BL_ERR_MAX                                 = 0x7FFF
}bl_error_boot_type;

typedef enum
{
  BL_ERR_DDR_SET_PARAMS_FAIL                         = 0X0001,
  BL_ERR_DDR_INFO_NOT_SET                            = 0X0003,
  BL_ERR_PASS_DDR_TRAINING_DATA_TO_DDR_DRIVER_FAIL   = 0X0004,
  BL_ERR_DDR_TRAINING_DATA_SIZE_INVALID              = 0X0005,
  BL_ERR_INVALID_CLOCK_PLAN                          = 0x0006,
  BL_ERR_DDR_NULL_TRAINED_PARAMS                     = 0x0007,
  BL_ERR_DDR_SPD                                     = 0x0008,
  BL_ERR_DDI_SIZE_INVALID                            = 0x0009,
  BL_ERR_DDR_MAX                                     = 0x7FFF
}bl_error_ddr_type;

typedef enum
{
  BL_ERR_UNDEF_INSTR            = 0X0001,
  BL_ERR_SWI                    = 0X0002,
  BL_ERR_PREFETCH_ABORT         = 0X0003,
  BL_ERR_DATA_ABORT             = 0X0004,
  BL_ERR_RESERVED_HANDLER       = 0X0005,
  BL_ERR_IRQ                    = 0X0006,
  BL_ERR_FIQ                    = 0X0007,
  BL_ERR_C_LIB_RT               = 0X0008,
  BL_ERR_SYS                    = 0X0009,
  BL_EXCEPTION_I_ABORT          = 0x0021,      /* Exception from an instruction abort without a change in level */
  BL_EXCEPTION_D_ABORT          = 0x0025,      /* Exception from a data abort without a change in level */
  BL_ERR_EXCEPTION_MAX          = 0x7FFF
}bl_error_exceptions_type;

/* The BL error callback function */
typedef void (*bl_error_callback_func_type)
(
  void *data_ptr
);

/* Structure for storing error handler callback data */
typedef struct bl_error_callback_node_type
{
  struct
  {
    bl_error_callback_func_type cb_func;
    void *data_ptr;
  } data;

  struct bl_error_callback_node_type *next_ptr;
} bl_error_callback_node_type;

/* The BL error interface */
typedef struct boot_error_if_type
{
/*===========================================================================

**  Function :  error_handler

** ==========================================================================
*/
/*!
* 
*   @brief
*     This function handles boot errors.
* 
*   @par Dependencies
*     None
*   
*   @retval
*     None
* 
*   @par Side Effects
*     None
* 
*/
boot_error_handler_ptr_type error_handler;

/*===========================================================================

**  Function :  install_callback

** ==========================================================================
*/
/*!
* 
*   @brief
*     Installs a callback that will be executed when an error occurs.
* 
*     NOTE: The caller must allocate a node for use in the error callback
*           linked list data structure, but does not need to initialize
*           the structure, as this function does the initialization of
*           the linked list data structure.
* 
*   @par Dependencies
*     None
*   
*   @retval
*     None
* 
*   @par Side Effects
*     None
* 
*/
  void (*install_callback)
  (
    bl_error_callback_func_type cb_func,  /* The callback function */
    void *data_ptr,       /* Data to pass to the callback function */
    bl_error_callback_node_type *node_ptr /* Data structure allocated by
                                        caller to store linked list node */
  );
} boot_error_if_type;

/* The global boot error interface structure */
extern boot_error_if_type bl_err_if;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Macro :  bl_verify

** ==========================================================================
*/
/*!
* 
* @brief
*   Given a boolean expression, this macro will verify the condition is TRUE
*   and do nothing if the condition is TRUE, otherwise it will call the
*   SBL error handler.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   This macro never returns if the condition is FALSE.
* 
*/
/*The forever loop after error_handler will never be executed, it is added to fix klockwork warning*/
#define BL_VERIFY( xx_exp, error_type ) \
            do { \
               if( !(xx_exp) ) \
               { \
                 bl_err_if.error_handler( __FILE_BASENAME__, __LINE__, \
                                        ( (uint32) ( error_type ) ) ); \
                 while(1) \
                 { \
                 } \
               } \
            } while(0)

/*===========================================================================

**  Macro :  bl_err_fatal

** ==========================================================================
*/
/*!
* 
* @brief
*   This macro calls the error handler.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   This macro never returns.
* 
*/
/*The forever loop after error_handler will never be executed, it is added to fix klockwork warning*/
#define BL_ERR_FATAL( error_type )  \
            do{ \
                bl_err_if.error_handler( __FILE_BASENAME__, __LINE__, \
                                       ( (uint32) error_type ) ); \
                while(1) \
                { \
                } \
              } while (0)

#endif  /* BOOT_ERROR_IF_H */
