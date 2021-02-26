#ifndef BOOT_ERRORS_H
#define BOOT_ERRORS_H

/** @file SecBoot_errors.h

  This header file contains declarations and definitions related
  to APPSBL error handling. 
   
  Copyright (c)2010 Qualcomm Technologies Incorporated. All Rights Reserved.
**/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Error code definitions for reporting errors in PBL */
typedef enum
{
  NO_ERR= 0,
  FLASH_DET_OR_RW_ERR,
  FLASH_TIMEOUT_ERR,
  ASSERT_ERR,
  LOAD_RPM_SBL_ERR,
  RPM_SBL_FORMAT_ERR,
  FLASH_SPEC_VER_ERR,
  CERT_CHAIN_VERIFY_ERR,
  SIGNATURE_VERIFY_ERR,
  MISC_OR_EXCEPTION_ABORT_ERR,
  LOAD_DLOAD_ERR,
  BOOT_SELECT_OPTION_ERR,
  SDCC_SPEED_SELECT_ERR,
  OUT_OF_BOUND_ERR,
  NULL_PTR_ERR, /* null pointer error */
  ROLLBACK_ERR, /* Denotes older secure image is being used */
  OEM_NUM_ROOT_CERTS_ERR, /* Denotes the number of OEM root certs were invalid */
  OEM_ROOT_CERT_SEL_ERR,  /* Denotes the OEM root cert selected is invalid */
  INVALID_IMAGE_TYPE_ERR, /* invalid image being authenticated */
  INVALID_PMIC_ERR,       /* no PMIC, or not an expected Qualcomm PMIC connected  */
  PMIC_FLCB_ERR,          /* Error during fast low current boot sequence that leads
                                 to PMIC power down MSM.  */
  USB_ENUM_ERR,           /* USB enumeration failed  */
  MAX_ERR_CODE = 0x7FFFFFFF   /* To ensure it's 32 bits wide */
} err_code_enum_type;

/* Error log structure to store constant data describing error location */
typedef struct boot_error_log_location_type
{
  uint32                   line_num;
  char                    *filename;
  err_code_enum_type   err_code;
} boot_error_log_location_type;

/* Typedef of a function pointer that receives PBL errors */
typedef void (*boot_error_handler_type)
(
  const char* filename_ptr,         /* File this function was called from -
                                      defined by __FILE__ */
  uint32      line,                 /* Line number this function was called
                                      from - defined by  __LINE__ */
  err_code_enum_type  err_code, /* Error code to indicate which error
                                       by toggling UART_DP_TX_DATA pin */
  uint32  err_log                   /* Error log to help debug */

);

/* Typedef of a function pointer that logs non-fatal PBL errors */
typedef boolean (*boot_error_add_log_type)
( 
  const boot_error_log_location_type *log, /* struct providing error  
                                                  location                */
  uint32 d1,                                   /* data1 to be saved       */
  uint32 d2,                                   /* data2 to be saved       */
  uint32 d3                                    /* data3 to be saved       */
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif  /* BOOT_ERRORS_H */

