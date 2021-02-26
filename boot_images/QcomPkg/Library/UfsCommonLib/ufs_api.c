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

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UfsCommonLib/ufs_api.c#1 $
$DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     --------------------------------------------------- 
2017-08-30   jt      Check whether LUN is valid before test unit ready 
2017-05-22   jt      Add background initialization APIs
2016-09-21   jt      Get write protect flags  
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

#include "ufs_osal.h"
#include "ufs_api.h"
#include "ufs_memory.h"
#include "ufs_errno.h"

// Allocate an array to hand out handles
#define MAX_UFS_OPEN_HANDLES        20
#define UFS_HANDLE_MAGIC_ID         0x105189ab
#define UFS_INUSE_TRUE              0xF0

static ufs_handle_t ufs_handle_array[MAX_UFS_OPEN_HANDLES] = {{0}};

// Find the next free handle and allocate
static ufs_handle_t *ufs_get_handle ()
{
   uint32_t i;

   //ufs_osal_sync_enter(&g_ufs_drv_lock);
 
   for (i = 0; i < MAX_UFS_OPEN_HANDLES; i++) {
      if (ufs_handle_array[i].is_inuse) {
         continue;
      }
      else {
         // Set the inuse flag in the handle
         ufs_handle_array[i].is_inuse = UFS_INUSE_TRUE;
         ufs_handle_array[i].id_magic = UFS_HANDLE_MAGIC_ID;
         return &ufs_handle_array[i];
      }
   }

   //ufs_osal_sync_leave(&g_ufs_drv_lock);
  
   // Running out of handles, return NULL
   return NULL;
}


// Free the inuse handle
void ufs_free_handle (ufs_handle_t *handle)
{
   if (NULL == handle) {
      return;
   }
 
   handle->is_inuse = 0;
   handle->id_magic = 0;
}

static int32 ufs_phase1_init(ufs_handle_t *hufs, uint32 lun)
{
   int32 rc = EOK;

   // 1. Call memory initialization
   rc = ufs_mem_init(hufs);
   if (rc != EOK) {
      return rc; 
   }

   //1.5 Get PHY capabilities
   rc = ufs_get_phy_capabilities(hufs);
   if (rc != EOK) {
      return rc;  
   }

   // 2. Wait until device is responding to NOP UPIU
   rc =  ufs_wait_device_awake (hufs);
   if (rc != EOK) {
      return rc; 
   }

   return rc; 
}

static int32 ufs_phase2_init(ufs_handle_t *hufs)
{
   int32 rc = EOK;

   rc = ufs_set_fDeviceInit (hufs);
   if (rc != EOK) {
      return rc;  
   }

   rc = ufs_poll_fDeviceInit (hufs);
   if (rc != EOK) {
      return rc;
   }

   return rc; 
}

// Public API
int32 ufs_background_init_start(uint32 core_id, uint32 init_ctxt)
{
   (void) core_id; 
   (void) init_ctxt; 

   int32 rc = EOK;
   ufs_handle_t *hufs = NULL; 

   // First get a free handle
   hufs = ufs_get_handle();
   if (hufs == NULL) {
      // Out of available handles
      return UFS_EINVAL;
   }

   // Set the LUN used
   hufs->lun = UFS_WLUN_DEVICE;

   // Do the 1st part of the initialization: PHY init, link startup, NOP OUT
   rc = ufs_phase1_init(hufs, UFS_WLUN_DEVICE); 
   if (rc != EOK) {
      goto BGInitEnd; 
   }

   // Setting up a pending transfer
   rc = ufs_setup_pending_xfer (hufs);
   if (rc != EOK) {
      goto BGInitEnd; 
   }

   // Check to see if the device has fDeviceInit set, if not, set it. 
   // In this case, busy code is not an error. 
   rc = ufs_set_fDeviceInit (hufs);
   if (rc == -EBUSY) {
      rc = EOK; 
   }

BGInitEnd:
   if(hufs != NULL) {
      ufs_free_handle(hufs); 
   }
   return rc; 
}

// Public API
int32 ufs_background_init_finish(uint32 core_id, uint32 init_ctxt)
{
   struct ufs_handle *hdl;
   
   (void) core_id; 
   (void) init_ctxt; 

   int32 rc = EOK;
   ufs_handle_t *hufs;

   // First get a free handle
   hufs = ufs_get_handle();
   if (hufs == NULL) {
      // Out of available handles
      return UFS_EINVAL;
   }

   // Set the LUN used
   hufs->lun = UFS_WLUN_DEVICE;

   // Finish 2nd initialization phase: poll for fDeviceInit completion
   rc = ufs_phase2_init(hufs); 
   if (rc != EOK) {
      goto BGInitEnd; 
   }
   
   hdl = ufs_open(0, UFS_WLUN_DEVICE);
   if (hdl == NULL) {
     return UFS_EINVAL;
   }

BGInitEnd:
   if(hufs != NULL) {
      ufs_free_handle(hufs); 
   }
   return rc; 
}


// Public API 
struct ufs_handle *ufs_open (uint32 core_id, uint32 lun)
{
   int32 rc = EOK;
   ufs_handle_t *hufs;

   // Only a single core is supported 
   if (core_id != 0) {
      return NULL;
   }

   // First get a free handle
   hufs = ufs_get_handle();
   if (hufs == NULL) {
      // Out of available handles
      return NULL;
   }

   // Set the LUN used
   hufs->lun = lun;

   // Phase 1 Init: PHY init, link startup, NOP OUT
   rc = ufs_phase1_init(hufs, lun); 
   if (rc != EOK) {
      ufs_free_handle (hufs);
      return NULL;
   }

   // Phase 2 Init: Check to see if the device has fDeviceInit set, if not, set it
   if (lun != UFS_WLUN_BOOT) {
      rc = ufs_phase2_init(hufs); 
      if (rc != EOK) {
         ufs_free_handle (hufs); 
         return NULL; 
      }
   }

   // Do not check for the DEVICE WLUN
   if (lun != UFS_WLUN_DEVICE && lun != UFS_WLUN_BOOT) {
      // Check to make sure the LUN is a valid LUN 
      rc = ufs_check_valid_lun (hufs, lun & 0xff);
      if (rc != EOK) {
         ufs_free_handle (hufs);
         return NULL;
      }
   }

   // Send TEST UNIT READY 
   rc = ufs_scsi_unit_rdy (hufs);
   if (rc != EOK) {
      ufs_free_handle (hufs);
      return NULL;
   }

   // If the call is for BOOT_WLUN, skip the rest of the operations
   if (lun == UFS_WLUN_BOOT) {
      return hufs;
   }

   if (lun != UFS_WLUN_DEVICE) {
     // Set gear
     rc =  ufs_set_gear (hufs);
     if (rc != EOK) {
        ufs_free_handle (hufs);
        return NULL;
      }
   }

   if (lun != UFS_WLUN_BOOT) {
      // Get the default device descriptor
      rc = ufs_get_descriptor_device (hufs);
      if (rc != EOK) {
         ufs_free_handle (hufs);
         return NULL;
      }
   }
   
   // Send a signle REQUEST SENSE command to get any error code
   rc = ufs_scsi_req_sense (hufs);
   if (rc != EOK) {
      return NULL;
   }

   // Start the unit 
   if (lun == UFS_WLUN_DEVICE) {
      rc = ufs_scsi_start_stop_unit (hufs, 1);
      if (rc != EOK) {
          return NULL;
      }
   }

   if (lun != UFS_WLUN_BOOT) {
      rc = ufs_scsi_inquiry (hufs);
      if (rc != EOK) {
         ufs_free_handle (hufs);
         return NULL;
      }

      rc = ufs_get_descriptor_config (hufs);
      if (rc != EOK) {
         ufs_free_handle (hufs);
         return NULL;
      }
   }

   // Get the unit descriptor for normal storage LUNs
   if (lun <= 0x07) {
      rc = ufs_get_descriptor_unit (hufs, lun);
      if (rc != EOK) {
         ufs_free_handle (hufs);
         return NULL;
      }
   }

   if (lun == UFS_WLUN_RPMB) {
      rc = ufs_get_descriptor_rpmb (hufs);
      if (rc != EOK) {
         ufs_free_handle (hufs);
         return NULL;
      }
   }

   rc = ufs_get_bBootLunEn (hufs);
   if (rc != EOK) {
      ufs_free_handle (hufs);
      return NULL;
   }

   rc = ufs_get_bConfigDescrLock (hufs);
   if (rc != EOK) {
      ufs_free_handle (hufs);
      return NULL;
   }

   if (lun != UFS_WLUN_BOOT) {
      rc = ufs_get_descriptor_geometry (hufs);
      if (rc != EOK) {
         ufs_free_handle (hufs);
         return NULL;
      }
   }

   return hufs;
}


/* Public API */
int32 ufs_close (struct ufs_handle *handle)
{
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   ufs_free_handle (handle);
   return EOK;
}


/* Public API */
int32 ufs_get_device_info (struct ufs_handle *handle, 
                           struct ufs_info_type *info)
{
   int32 rc = EOK;

   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }
   
   osal_memset (info, 0, sizeof (struct ufs_info_type));
   ufs_fill_in_device_info (handle, info);

   info->dLuTotalBlocks = info->qLogicalBlockCount;

   rc = ufs_get_flags(handle, info); 
   if (rc != EOK) {
      return rc;
   }

   return EOK;
}


/* Public API */
int32 ufs_configure_device( struct ufs_handle *handle, 
                            struct ufs_config_descr *cfg )
{
   int32 rc = EOK;

   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   rc = ufs_do_config_device (handle, cfg);
   if (rc != EOK) {
      return rc;
   }

   // Disable pre-program mode if it is requested
   if (cfg->qVendorConfigCode != 0) {
      rc = ufs_vend_pprog_ctrl (handle, cfg->qVendorConfigCode);
   }

   return rc;
}


/* Public API */
int32 ufs_set_bootable (struct ufs_handle *handle, uint32 lun)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Allow only LUN 0 - Disable, 1 - Boot A or 2 - Boot B
   if (lun > UFS_LUN_BOOT_B) {
      return -EINVAL;
   }  

   // Set the boot enable flag in the attributes bBootLunEn
   rc = ufs_set_bBootLunEn (handle, lun);
   if (rc != EOK) {
      return -EIO;
   }

   return rc;
}


/* Public API */
int32 ufs_set_pon_write_protect (struct ufs_handle *handle)
{
   int32 rc = EOK;

   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   rc = ufs_set_fPowerOnWPEn (handle);
   return rc;
}


/* Public API */
int32 ufs_purge_and_wait( struct ufs_handle *handle)
{
   int32 rc = EOK;

   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   rc = ufs_set_n_wait_fPurgeEnable (handle);
   return rc;
}

/* Public API */
int32 ufs_purge_nonblock_setup (struct ufs_handle *handle)
{
   int32 rc = EOK;

   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Setup a pending operation
   rc = ufs_setup_pending_xfer (handle);
   if (rc != EOK) {
      return rc;
   }

   rc = ufs_set_n_wait_fPurgeEnable (handle);
   return rc;
}

/* Public API */
int32 ufs_read (struct ufs_handle *handle,
                uint8           *buff,
                uint64           lba,
                uint32           blocks)
{
   int32 rc = EOK;

   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_scsi_read (handle, buff, lba, blocks);
   return rc;
}


/* Public API */
int32 ufs_write (struct ufs_handle *handle,
                 uint8             *buff,
                 uint64             lba,
                 uint32             blocks)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_scsi_write (handle, buff, lba, blocks);
   return rc;
}

/* Public API */
int32 ufs_read_ex (struct ufs_handle *handle,
                   uint8             *buff,
                   uint64             lba,
                   uint32             blocks)
{
   int32 rc = EOK;

   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Setting up a pending transfer
   rc = ufs_setup_pending_xfer (handle);
   if (rc != EOK) {
      return rc;
   }

   // Call the SCSI command building function
   rc = ufs_scsi_read (handle, buff, lba, blocks);
   return rc;
}


/* Public API */
int32 ufs_write_ex (struct ufs_handle *handle,
                    uint8             *buff,
                    uint64             lba,
                    uint32             blocks)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Setting up a pending transfer
   rc = ufs_setup_pending_xfer (handle);
   if (rc != EOK) {
      return rc;
   }

   // Call the SCSI command building function
   rc = ufs_scsi_write (handle, buff, lba, blocks);
   return rc;
}


/* Public API */
int32 ufs_unmap (struct ufs_handle *handle,
                 uint64             lba,
                 uint32             blocks)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_scsi_unmap (handle, lba, blocks);
   return rc;
}

/* Public API */
int32 ufs_format (struct ufs_handle *handle)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_scsi_format (handle);
   return rc;
}

/* Public API */
int32 ufs_rpmb_read_cnt_pkt (struct ufs_handle *handle, uint8 *resp)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // This API only work with RPMB WLUN
   if (handle->lun != UFS_WLUN_RPMB) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_mem_rpmb_read_write_cnt (handle, resp);
   return rc;
}

/* Public API */
int32 ufs_rpmb_send_cmd_pkt (struct ufs_handle *handle, 
                             uint8 *cmd_pkt, 
                             uint8 *rsp_pkt)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // This API only work with RPMB WLUN
   if (handle->lun != UFS_WLUN_RPMB) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_mem_rpmb_msg_out (handle, cmd_pkt);
   if (rc != EOK) {
      return rc;
   }

   rc = ufs_mem_rpmb_msg_in (handle, rsp_pkt);
   return rc;
}

/* Public API */
int32 ufs_rpmb_read_blocks (struct ufs_handle *handle, 
                            uint8 *cmd_pkt, 
                            uint32 pkt_cnt, 
                            uint8 *data_pkt)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // This API only work with RPMB WLUN
   if (handle->lun != UFS_WLUN_RPMB) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_mem_rpmb_read_data (handle, pkt_cnt, cmd_pkt, data_pkt);
   return rc;
}

/* Public API */
int32 ufs_rpmb_write_blocks (struct ufs_handle *handle, 
                             uint8 *cmd_pkt, 
                             uint32 pkt_cnt, 
                             uint8 *resp_pkt)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // This API only work with RPMB WLUN
   if (handle->lun != UFS_WLUN_RPMB) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_mem_rpmb_write_data (handle, pkt_cnt, cmd_pkt, resp_pkt);
   return rc;
}


/* For Tools debugging, get device descriptor string */
/* Public API */
int32 ufs_get_device_info_str (struct ufs_handle *handle,
                               uint32             id,
                               char              *buf,
                               uint32             size)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL || buf == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Call the SCSI command building function
   rc = ufs_get_descriptor_str (handle, id, buf, size);
   return rc;
}

int32 ufs_set_refclk_freq (struct ufs_handle *handle, 
                           uint32             freq)
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   // Default clock is 19.2MHz
   if (freq == 0) {
      freq = 19200000;
   }

   // All current board design support 19.2MHz reference clock only
   if (freq != 19200000) {
      return -EINVAL;
   }


   // Set the bRefClkFreq
   rc = ufs_set_bRefClkFreq (handle, freq);

   return rc;
}

int32 ufs_poll_xfer (struct ufs_handle *handle )
{
   int32 rc = EOK;
   // Check input parameter
   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   rc = ufs_finish_pending_xfer (handle);
   return rc;
}

int32 ufs_fw_update_write (ufs_handle_t *handle, int32_t size, 
                           int32_t offset, uint8_t *buf)
{
   int32 rc = EOK;

   if (handle == NULL) {
      return -EINVAL;
   }

   // Check inuse flag
   if (handle->is_inuse != UFS_INUSE_TRUE ||
       handle->id_magic != UFS_HANDLE_MAGIC_ID) {
      return -EINVAL;
   }

   // Make sure the host controlle interface is ready
   if (handle->phc == NULL) {
      return -EINVAL;
   }

   rc = ufs_scsi_write_buf (handle, buf, 0x0e, 0x00, offset, size);
   return rc;
}


