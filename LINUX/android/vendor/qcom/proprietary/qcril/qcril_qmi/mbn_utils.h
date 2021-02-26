
/******************************************************************************
  @file    mbn_utils.h
  @brief   utils for parsing MBN files

  DESCRIPTION
    Provide interface to get MBN configuration information.

  ---------------------------------------------------------------------------

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __MBN_UTILS_H
#define __MBN_UTILS_H

#include <stdint.h>

#define MBN_PARSE_SUCCESS   1
#define MBN_PARSE_FAILURE   0

/*===========================================================================

  FUNCTION: mcfg_get_oem_version

===========================================================================*/
/*!
    @brief
    This function will get the oem_version for a given MBN file

    @return
    MBN_PARSE_FAILURE -> Failed  MBN_PARSE_SUCCESS -> Success

    @side effect
    If success, the function will allocate the memory, assigned to parameter
    oem_version. The caller should free it after use.
*/
/*=========================================================================*/
int mcfg_get_oem_version(char* file_name, char** oem_version, uint32_t* len);

/*===========================================================================

  FUNCTION: mcfg_get_qc_version

===========================================================================*/
/*!
    @brief
    This function will get the qc_version for a given MBN file

    @return
    MBN_PARSE_FAILURE -> Failed  MBN_PARSE_SUCCESS -> Success

    @side effect
    If success, the function will allocate the memory, assigned to parameter
    qc_version. The caller should free it after use.
*/
/*=========================================================================*/
int mcfg_get_qc_version(char* file_name, char** qc_version, uint32_t* len);

#endif
