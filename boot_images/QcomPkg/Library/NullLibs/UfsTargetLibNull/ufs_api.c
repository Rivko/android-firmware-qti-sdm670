 /*****************************************************************************
 * ufs_api.c
 *
 * UFS device driver - public API
 *
 * This file implements the top level UFS driver APIs 
 *
 * Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *****************************************************************************/
/*=============================================================================
                        Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/NullLibs/UfsTargetLibNull/ufs_api.c#1 $
$DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     --------------------------------------------------- 
2017-05-22   jt      Add background initialization APIs  
2015-10-07   plc     Change RUMI flag to PRE_SI flag for presi execution
2015-09-22   rh      Adding the FFU interface API
2015-03-26   rh      Fix RPMB support
2015-02-18   rh      Add pre-init reading of WLUN_BOOT
2014-04-22   rh      Adding the pre-prog config option
2013-11-05   rh      Adding the format API
2013-10-22   rh      Move the gear switch to after NOP response
2013-09-25   rh      Allow ufs_open to pen UFS_WLUN_BOOT LU
2013-09-10   rh      Get the device descriptor first prior to open LUs
2013-09-09   rh      Enable setting of the bRefClkFreq
2013-08-12   rh      LU size calculation is not correct
2012-12-18   rh      Initial creation
=============================================================================*/

#include "ufs_api.h"

// Allocate an array to hand out handles
#define MAX_UFS_OPEN_HANDLES        20
#define UFS_HANDLE_MAGIC_ID         0x105189ab
#define UFS_INUSE_TRUE              0xF0

// Find the next free handle and allocate
static ufs_handle_t *ufs_get_handle ()
{
   // Running out of handles, return NULL
   return NULL;
}


// Free the inuse handle
void ufs_free_handle (ufs_handle_t *handle)
{
   return;
}


// Public API 
struct ufs_handle *ufs_open (uint32 core_id, uint32 lun)
{
   return NULL;
}


/* Public API */
int32 ufs_close (struct ufs_handle *handle)
{
   return 0;
}


/* Public API */
int32 ufs_get_device_info (struct ufs_handle *handle, 
                           struct ufs_info_type *info)
{
   return 0;
}


/* Public API */
int32 ufs_configure_device( struct ufs_handle *handle, 
                            struct ufs_config_descr *cfg )
{
   return 0;
}


/* Public API */
int32 ufs_set_bootable (struct ufs_handle *handle, uint32 lun)
{
   return 0;
}


/* Public API */
int32 ufs_set_pon_write_protect (struct ufs_handle *handle)
{
   return 0;
}


/* Public API */
int32 ufs_purge_and_wait( struct ufs_handle *handle)
{
   return 0;
}

/* Public API */
int32 ufs_purge_nonblock_setup (struct ufs_handle *handle)
{
   return 0; 
}

/* Public API */
int32 ufs_read (struct ufs_handle *handle,
                uint8           *buff,
                uint64           lba,
                uint32           blocks)
{
   return 0;
}


/* Public API */
int32 ufs_write (struct ufs_handle *handle,
                 uint8             *buff,
                 uint64             lba,
                 uint32             blocks)
{
   return 0;
}

/* Public API */
int32 ufs_read_ex (struct ufs_handle *handle,
                   uint8             *buff,
                   uint64             lba,
                   uint32             blocks)
{
   return 0;
}


/* Public API */
int32 ufs_write_ex (struct ufs_handle *handle,
                    uint8             *buff,
                    uint64             lba,
                    uint32             blocks)
{
   return 0;
}


/* Public API */
int32 ufs_unmap (struct ufs_handle *handle,
                 uint64             lba,
                 uint32             blocks)
{
   return 0;
}

/* Public API */
int32 ufs_format (struct ufs_handle *handle)
{
   return 0;
}

/* Public API */
int32 ufs_rpmb_read_cnt_pkt (struct ufs_handle *handle, uint8 *resp)
{
   return 0;
}

/* Public API */
int32 ufs_rpmb_send_cmd_pkt (struct ufs_handle *handle, 
                             uint8 *cmd_pkt, 
                             uint8 *rsp_pkt)
{
   return 0;
}

/* Public API */
int32 ufs_rpmb_read_blocks (struct ufs_handle *handle, 
                            uint8 *cmd_pkt, 
                            uint32 pkt_cnt, 
                            uint8 *data_pkt)
{
   return 0;
}

/* Public API */
int32 ufs_rpmb_write_blocks (struct ufs_handle *handle, 
                             uint8 *cmd_pkt, 
                             uint32 pkt_cnt, 
                             uint8 *resp_pkt)
{
   return 0;
}


/* For Tools debugging, get device descriptor string */
/* Public API */
int32 ufs_get_device_info_str (struct ufs_handle *handle,
                               uint32             id,
                               char              *buf,
                               uint32             size)
{
   return 0;
}

int32 ufs_set_refclk_freq (struct ufs_handle *handle, 
                           uint32             freq)
{
   return 0;
}

int32 ufs_poll_xfer (struct ufs_handle *handle )
{
   return 0;
}

int32 ufs_fw_update_write (ufs_handle_t *handle, int32_t size, 
                           int32_t offset, uint8_t *buf)
{
   return 0;
}

int32 ufs_background_init_finish(uint32 core_id, uint32 init_ctxt)
{
   return 0; 
}

int32 ufs_background_init_start(uint32 core_id, uint32 init_ctxt)
{
   return 0; 
}


