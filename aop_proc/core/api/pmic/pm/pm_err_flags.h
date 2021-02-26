#ifndef _PM_ERR_FLAGS_H
#define _PM_ERR_FLAGS_H
/*! \file 
 *  \n
 *  \brief  pm_lib_err.h ---- PMIC library error codes 
 *  \n
 *  \n This file is defines error codes returned by PMIC library APIs 
 *  \n
 *  \n &copy; Copyright 2003-2011 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/pmic/pm/pm_err_flags.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/27/12   hs      Added PM_ERR_FLAG__SPMI_TRANSCOMM_ERR.
02/10/11   hw      Added PM_ERR_FLAG__INVALID_DISPBACKLIGHT_APP_INDEXED
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
01/07/10   wra     Adding SMBC and BMS error codes
08/10/10   wra     Adding PMIC Application Event error codes
06/28/10   wra     Adding 8660 error codes
12/02/09   jtn     Add error code for keypad
08/06/09   jtn     Add PM_ERR_MAX() macro
07/23/09   wra     added vbatt error definitions.
06/30/09   jtn     Updated file documentation
05/01/09   jtn     New file
=============================================================================*/

/**
 * These are all the error codes returned by PMIC APIs
 *
 * PMIC APIs most commonly return errors if the caller has tried
 * to use parameters that are out of range or if the feature is
 * not supported by the PMIC model detected at run-time.
 */
typedef enum
{
  PM_ERR_FLAG_SUCCESS                  = 0x0000,
  PM_ERR_FLAG_FAILURE                  = 0x0001, //0x01<<0
  PM_ERR_FLAG_INVALID_PARAMETER        = 0x0002, //0x01<<1
  PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = 0x0004, //0x01<<2
  PM_ERR_FLAG_DATA_VERIFY_ERR          = 0x0008, //0x01<<3
  PM_ERR_FLAG_INVALID_CONFIG_SETTING   = 0x0010, //0x01<<4
  PM_ERR_FLAG_INVALID_PERIPHERAL       = 0x0020, //0x01<<5
  PM_ERR_FLAG_PERIPHERAL_ERR           = 0x0040, //0x01<<6
  PM_ERR_FLAG_BUS_ERR                  = 0x0080, //0x01<<7
  PM_ERR_FLAG_OS_ERR                   = 0x0100, //0x01<<8
  PM_ERR_FLAG_INVALID                  = 0x8000,  //0x01<<15

} pm_err_flag_type;


/**
 * This macro is used to return the greater of PM_ERR_FLAG__SUCCESS
 * or whatever error an API may have found.
 */
#define PM_ERR_MAX(a,b) ((a > b) ? (a) : (b))


#endif

