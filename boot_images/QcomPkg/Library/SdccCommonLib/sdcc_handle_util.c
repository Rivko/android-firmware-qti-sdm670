/******************************************************************************
 * File: sdcc_handle_util.c
 *
 * Services:
 *    This file provides SDCC handle services for SDIO and memory devices.
 *
 * Description:
 *    This file contains the SDCC handle specific utility function.
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================
                        Edit History


when         who     what, where, why
----------   ---     ----------------------------------------------------------
2014-11-10   bn      Add null pointer check for sdcc_get_slot_handle
2014-06-18   bn      Initial version. Branched from 8994 SBL

=============================================================================*/
#include "sdcc_handle_util.h"
#include "sdcc_util.h"
#include "SdccCommon.h"

/* counter to indicate the number of available client handle entries */
static INT32 sdcc_client_handle_list_avail_count = SDCC_CLIENT_HANDLE_MAX_NUM;

/* client handle list to save the client access info such as device handle, */
/* partition region and task control block */
static struct sdcc_device
sdcc_client_handle_list[SDCC_CLIENT_HANDLE_MAX_NUM];

/* get the memory address of the first client handle element */
#define SDCC_CLIENT_HANDLE_LIST_START_ADDR   &sdcc_client_handle_list[0]
/* get the memory address of the last client handle element */
#define SDCC_CLIENT_HANDLE_LIST_END_ADDR \
   &sdcc_client_handle_list[SDCC_CLIENT_HANDLE_MAX_NUM - 1]

static struct sdcc_device *sdcc_client_handle_avail_entry_lookup(void);
static void sdcc_purge_client_handle_entries( UINT32 driveno );
static void sdcc_clear_client_handle_fields( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_init_client_handle_list
*
* Description:
*    This function initializes the client handle list.
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_init_client_handle_list( void )
{
   struct sdcc_device *pch = SDCC_CLIENT_HANDLE_LIST_START_ADDR;

   while ( pch <= SDCC_CLIENT_HANDLE_LIST_END_ADDR )
   {
      pch->hdev = NULL;
      pch->phy_partition_num = 0;
      pch->data_xfer_end_pt = NULL;
      pch++;
   }
} /* sdcc_init_client_handle_list */

/******************************************************************************
* Name: sdcc_client_handle_avail_entry_lookup
*
* Description:
*    This function searches for the first available entry in the client handle
*    list.
*
* Arguments:
*    None
*
* Returns:
*    A pointer to a SDCC device
*
******************************************************************************/
static struct sdcc_device *
sdcc_client_handle_avail_entry_lookup(void)
{
   struct sdcc_device *pch = SDCC_CLIENT_HANDLE_LIST_START_ADDR;

   /* search for the first available entry in the client handle list */
   while ( pch <= SDCC_CLIENT_HANDLE_LIST_END_ADDR )
   {
      if ( NULL == pch->hdev )
      {
         /* just return the memory location of the available entry */
         return pch;
      }
      pch++;
   }
   /* no avail entry */
   return NULL;
} /* sdcc_client_handle_avail_entry_lookup */

/******************************************************************************
* Name: sdcc_purge_client_handle_entries
*
* Description:
*    This function searches for the client handle entries that point to the
*    device handle with the device removed.
*
* Arguments:
*    driveno     [IN] : the drive number that has the device removed
*
* Returns:
*    None
*
******************************************************************************/
static void sdcc_purge_client_handle_entries( UINT32 driveno )
{
   struct sdcc_device *pch = SDCC_CLIENT_HANDLE_LIST_START_ADDR;

   /* go through the client handle list to search for the entries that */
   /* associate with the passed in driveno */
   while ( pch <= SDCC_CLIENT_HANDLE_LIST_END_ADDR )
   {
      if ( pch->hdev && driveno == SDCC_HANDLE_GET_DRIVENO(pch) )
      {
         /* clear all fields in this client handle */
         sdcc_clear_client_handle_fields( pch );
      }

      /* check if client handle list available count reaches max */
      /* if so, it means all client handles point to this driveno that has */
      /* the device removed and client handle list has been purged */
      if ( SDCC_CLIENT_HANDLE_MAX_NUM == sdcc_client_handle_list_avail_count )
      {
         break;
      }

      pch++;
   }
} /* sdcc_purge_client_handle_items */

/******************************************************************************
* Name: sdcc_clear_client_handle_fields
*
* Description:
*    This function clears the fields for the specified client handle.
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device
*
* Returns:
*    None
*
******************************************************************************/
static void sdcc_clear_client_handle_fields( struct sdcc_device *handle )
{
   handle->hdev = NULL;
   handle->phy_partition_num = 0;
   handle->data_xfer_end_pt = NULL;

   sdcc_client_handle_list_avail_count++;
} /* sdcc_clear_client_handle_fields */

/******************************************************************************
* Name: sdcc_register_client_handle
*
* Description:
*    This function registers a client handle with the specified driveno and
*    phy_partition_num.  It returns a pointer to a client handle entry if the
*    registration succeeds.
*
* Arguments:
*    driveno           [IN] : a valid drive number between 0 and the total
*                             number of card slots - 1
*    phy_partition_num [IN] : physical partition number.  Use
*                             \ref SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE
*                             to get a handle representing the entire memory
*                             device.  For eMMC v4.3, the valid boot partition
*                             numbers are 1 and 2.  User area partition number
*                             is 0.
*
* Returns:
*    A pointer to a SDCC device if successful.  NULL if failed.
*
******************************************************************************/
struct sdcc_device *
sdcc_register_client_handle( UINT32 driveno,
                             UINT32 phy_partition_num )
{
   struct sdcc_device *pch = NULL;
   sdcc_slot_type *pslot = NULL;

   /* only proceed to search and update the client handle list */
   /* if client handle list still has available entry */
   if ( sdcc_client_handle_list_avail_count < 1 )
   {
      /* no available entry */
      DEBUG ((EFI_D_ERROR, "SDCC Client handle list is full"));
      return NULL;
   }

   /* find out the next available entry */
   pch = sdcc_client_handle_avail_entry_lookup();

   /* make sure not dereferencing NULL pointer */
   if ( pch )
   {
      /* populate the internal client handle entry */
      pslot = sdcc_get_slot_handle ((INT16)driveno);
      if (NULL == pslot)
      {
         DEBUG ((EFI_D_ERROR, "sdcc_register_client_handle: can't get slot handle\n"));
         return NULL;
      }
      
      pch->hdev = &(pslot->dev);

      pch->phy_partition_num = phy_partition_num;
      sdcc_client_handle_list_avail_count--;
   }

   return pch;
} /* sdcc_register_client_handle */

/******************************************************************************
* Name: sdcc_deregister_client_handle
*
* Description:
*    This function de-registers a client handle.  It clears all client handle
*    fields in an entry and set the handle to NULL.
*
* Arguments:
*    handle     [IN/OUT] : a pointer to a pointer to the SDCC device
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_deregister_client_handle( struct sdcc_device **handle )
{
   sdcc_dev_type  *sdcc_pdev  = NULL;

   if ( NULL == *handle )
   {
      DEBUG ((EFI_D_ERROR, "Handle already de-registered"));
      return;
   }

   sdcc_pdev = (*handle)->hdev;

   /* check if device has been removed */
   if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
   {
      /* purge all client handle entries that have this device removed */
      sdcc_purge_client_handle_entries( sdcc_pdev->driveno );
   }
   else
   {
      sdcc_clear_client_handle_fields( *handle );
   }

   /* set handle to NULL as it should not point to the client handle entry */
   *handle = NULL;
} /* sdcc_deregister_client_handle */

/******************************************************************************
* Name: sdcc_is_device_still_in_use
*
* Description:
*    This function examines if there are any entries in the client handle list
*    still referring to the SDIO/memory device specified in driveno.
*
* Arguments:
*    pdev           [IN] : pointer to sdcc device handle that is in use
*
* Returns:
*    TRUE if SDIO/memory device is still in use.  FALSE otherwise.
*
******************************************************************************/
BOOLEAN sdcc_is_device_still_in_use( sdcc_dev_type *pdev )
{
   struct sdcc_device *pch = SDCC_CLIENT_HANDLE_LIST_START_ADDR;

   /* check if client handle list available count reaches max */
   /* if so, it means no other client referring to the SDIO/memory device */
   /* specified in driveno */
   if ( SDCC_CLIENT_HANDLE_MAX_NUM == sdcc_client_handle_list_avail_count )
   {
      return FALSE;
   }

   /* go through the client handle list to search for the entries that */
   /* associate with the passed in driveno */
   while ( pch <= SDCC_CLIENT_HANDLE_LIST_END_ADDR )
   {
      if ( pch->hdev && pch->hdev == pdev )
      {
         /* find a match */
         return TRUE;
      }
      pch++;
   }
   return FALSE;
} /* sdcc_is_device_still_in_use */

