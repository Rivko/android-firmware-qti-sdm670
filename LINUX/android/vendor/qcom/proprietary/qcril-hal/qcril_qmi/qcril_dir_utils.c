/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    qcril_file_utils.c
  @brief   Provides interface to communicate with files

  DESCRIPTION

******************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "qcril_log.h"

#define  QCRIL_DIR_MAX_DIRS 500

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
)
{
    DIR           *dir_desc;
    struct dirent *entry;
    int           cur_num_of_files  = 0;
    int           file_name_len     = 0;
    int           cur_num_of_dirs   = 0;
    int           dir_name_len      = 0;
    int           root_dir_name_len = 0;
    int           ret               = E_FAILURE;

    do {
        if (!dir || !file_names || !number_of_files ||
            !max_file_names || !dir_names || !number_of_dirs ||
            !max_dir_names)
        {
            break;
        }

        root_dir_name_len = strlen(dir);
        dir_desc = opendir(dir);

        if (dir_desc)
        {
            while ((entry = readdir(dir_desc)))
            {
                if (strcmp(entry->d_name, "..") && strcmp(entry->d_name,"."))
                {
                    if (entry->d_type == DT_DIR)
                    {
                        if (cur_num_of_dirs < max_dir_names)
                        {
                            dir_name_len = strlen(entry->d_name);
                            dir_names[cur_num_of_dirs] =
                                        malloc(root_dir_name_len + dir_name_len + 2);
                            strlcpy(dir_names[cur_num_of_dirs], dir,
                                    root_dir_name_len + dir_name_len + 2);
                            strlcat(dir_names[cur_num_of_dirs], entry->d_name,
                                    root_dir_name_len + dir_name_len + 2);
                            strlcat(dir_names[cur_num_of_dirs], "/",
                                    root_dir_name_len + dir_name_len + 2);
                            QCRIL_LOG_INFO("retrieved dir[%d]: %s", cur_num_of_dirs, dir_names[cur_num_of_dirs]);
                            cur_num_of_dirs++;
                        }
                    }
                    else
                    {
                        if (cur_num_of_files < max_file_names)
                        {
                            file_name_len = strlen(entry->d_name);
                            file_names[cur_num_of_files] =
                                    malloc(root_dir_name_len + file_name_len + 1);
                            strlcpy(file_names[cur_num_of_files], dir,
                                    root_dir_name_len + 1);
                            strlcat(file_names[cur_num_of_files], entry->d_name,
                                    root_dir_name_len + file_name_len + 1);
                            QCRIL_LOG_INFO("retrieved file: %s", file_names[cur_num_of_files]);
                            cur_num_of_files++;
                        }
                    }
                }
            }

            closedir (dir_desc);
            ret = E_SUCCESS;
        }
        else
        {
            QCRIL_LOG_DEBUG("Open dir failed %s with error: %d", dir, errno);
        }

    }while (0);

    if (number_of_files && number_of_dirs)
    {
        *number_of_files = cur_num_of_files;
        *number_of_dirs  = cur_num_of_dirs;
    }

    return ret;
}

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
)
{
    int           cur_num_of_files    = 0;
    int           cur_num_of_dirs     = 0;
    int           cur_max_dir_names   = QCRIL_DIR_MAX_DIRS;
    int           cur_max_file_names  = max_file_names;
    int           pending_num_of_dirs = 0;
    int           cur_dir_index       = 0;
    int           i;
    int           ret                 = E_FAILURE;

    char         *dir_names[QCRIL_DIR_MAX_DIRS]   = {NULL};
    char         **cur_dir  = dir_names;
    char         **cur_file = file_names;


    QCRIL_LOG_FUNC_ENTRY();
    do {

        if (!dir || !file_names || !number_of_files ||
            !max_file_names)
        {
            break;
        }

        dir_names[cur_dir_index] = (char *) dir;//use conversion as dir_names is dynamic char array
        cur_max_dir_names--;
        pending_num_of_dirs = 1;
        cur_dir = cur_dir + 1;
        *number_of_files = 0;

        while (pending_num_of_dirs && cur_dir_index < QCRIL_DIR_MAX_DIRS &&
               (cur_max_dir_names > 0) && (cur_max_file_names > 0))
        {
            ret = qcril_dir_retrieve_all_files(dir_names[cur_dir_index],
                                         cur_file,
                                         &cur_num_of_files,
                                         cur_max_file_names,
                                         cur_dir,
                                         &cur_num_of_dirs,
                                         cur_max_dir_names);

            cur_dir_index++;
            pending_num_of_dirs --;
            *number_of_files += cur_num_of_files;
            cur_dir  = cur_dir  + cur_num_of_dirs;
            cur_max_dir_names = cur_max_dir_names - cur_num_of_dirs;

            cur_file = cur_file + cur_num_of_files;
            cur_max_file_names = cur_max_file_names - cur_num_of_files;

            pending_num_of_dirs = pending_num_of_dirs + cur_num_of_dirs;
            QCRIL_LOG_DEBUG("Current index %d", cur_dir_index);
            QCRIL_LOG_DEBUG("cur_max_dir_names %d", cur_max_dir_names);
            QCRIL_LOG_DEBUG("cur_num_of_dirs %d", cur_num_of_dirs);
            QCRIL_LOG_DEBUG("cur_max_file_names %d", cur_max_file_names);
            QCRIL_LOG_DEBUG("cur_num_of_files %d", cur_num_of_files);
        }



    } while (0);

    // TODO: verify i = 1
    for (i = 1; i < QCRIL_DIR_MAX_DIRS; ++i)
    {
      if (dir_names[i])
      {
        free(dir_names[i]);
        dir_names[i] = NULL;
      }

    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}


