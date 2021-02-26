/*=======================================================================*//**
  @file         qusb_dload_cookie.c

  @brief        Implements functionality related to the software download feature.

  @details      
                 
                Copyright (c) 2014 Qualcomm Technologies Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE


  when        who        what, where, why
  ----------  --------   ------------------------------------------------------
  2014-06-27  shreyasr   initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qusb_cookie.h"
#include "qusb_dci_api.h"
 
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

 /*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
 
/*==============================================================================

FUNCTION          QUSB_BOOT_DLOAD_IS_DLOAD_INFO_PID_VALID

DESCRIPTION
Checks if the DLOAD related PID is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the PID is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean qusb_boot_dload_is_dload_info_pid_valid(const qusb_dload_info_type* dload_info_ptr)
{
  if (dload_info_ptr != NULL)
  {
    /* We verify that the data is valid according to the product_id validity */
    if (dload_info_ptr->pid_serial_num_cookie.magic_1 == PID_MAGIC_NUMBER)
    {
      return TRUE;
    }
    else
    { 
      return FALSE;
    }
  }
  else //if NULL
  {
    qusb_error_log(DLOAD_INFO_POINTER_NULL_LOG, 0 , __LINE__);
    return FALSE;
  }
}

/*==============================================================================

FUNCTION          QUSB_BOOT_DLOAD_IS_DLOAD_INFO_SERIAL_NUM_VALID

DESCRIPTION
Checks if the DLOAD related Serial Number is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the Serial Number is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean qusb_boot_dload_is_dload_info_serial_num_valid(const qusb_dload_info_type* dload_info_ptr)
{
  if (dload_info_ptr != NULL)
  {
    /* We verify that the data is valid according to the product_id validity */
    if (dload_info_ptr->pid_serial_num_cookie.magic_2 == SERIAL_NUMBER_MAGIC_NUMBER)
    {
    return TRUE;
    }
    else
    { 
    return FALSE;
    }
  }
  else
  {
    qusb_error_log(DLOAD_INFO_POINTER_NULL_LOG, 0 , __LINE__);
    return FALSE;
  }
}

/*==============================================================================

FUNCTION          QUSB_BOOT_DLOAD_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in HLOS, before going into 
download mode, and read in download mode.

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data

SIDE EFFECTS

==============================================================================*/
qusb_dload_info_type* qusb_boot_dload_get_dload_info_ptr(void)
{
#ifndef BUILD_EHOSTDL
  return (qusb_dload_info_type*)qusb_get_shared_imem_address();
#else
  return NULL;
#endif /* BUILD_EHOSTDL */
}
