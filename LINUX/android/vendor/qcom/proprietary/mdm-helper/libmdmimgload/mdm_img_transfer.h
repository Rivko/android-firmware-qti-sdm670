/*
 * ----------------------------------------------------------------------------
 *  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 */

#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <mdm_detect.h>
#define MDM_IMAGE_PATH_PROP "persist.mdm_image_path"
#define MHI_PIPE_SBL_LOGGER "_pipe_24"
#define MHI_PIPE_EFS_SYNC "_pipe_10"
#define MHI_PIPE_SAHARA "_pipe_2"
typedef enum {
  MODE_DEFAULT,
  MODE_FORCE_RAM_DUMPS,
  MODE_IGNORE_RAM_DUMPS
} img_transfer_mode;

bool mdm_img_transfer(char* mdm_name,
                      char* mdm_link,
                      char* mdm_link_extra_data,
                      char* ram_dump_path,
                      char* saved_file_prefix,
                      img_transfer_mode mode);



