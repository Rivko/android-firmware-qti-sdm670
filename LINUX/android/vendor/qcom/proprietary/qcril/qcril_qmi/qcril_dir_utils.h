/******************************************************************************

  @file    qcril_file_utils.h
  @brief   Provides interface to communicate with files

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_DIR_UTILS_H
#define QCRIL_DIR_UTILS_H


/*===========================================================================

  FUNCTION qcril_dir_retrieve_all_files

===========================================================================*/
/*!
    @brief
    retrieve all file names under certain directory

    @return
    0 for success
*/
/*=========================================================================*/
int qcril_dir_retrieve_all_files
(
    char  *dir,
    char **file_names,
    int   *number_of_files,
    int    max_file_names,
    char **dir_names,
    int   *number_of_dirs,
    int    max_dir_names
);


/*===========================================================================

  FUNCTION qcril_dir_retrieve_all_files_recursively

===========================================================================*/
/*!
    @brief
    retrieve all files under current dir recursively

    @return
    0 for success
*/
/*=========================================================================*/
int qcril_dir_retrieve_all_files_recursively
(
    char  *dir,
    char **file_names,
    int   *number_of_files,
    int    max_file_names
);


#endif /* QCRIL_DIR_UTILS_H */
