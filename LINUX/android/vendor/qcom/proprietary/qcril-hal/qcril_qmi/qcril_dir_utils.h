/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    qcril_file_utils.h
  @brief   Provides interface to communicate with files

  DESCRIPTION

******************************************************************************/

#ifndef QCRIL_DIR_UTILS_H
#define QCRIL_DIR_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

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
    const char  *dir,
    char **file_names,
    int   *number_of_files,
    int    max_file_names
);

#ifdef __cplusplus
}
#endif

#endif /* QCRIL_DIR_UTILS_H */
