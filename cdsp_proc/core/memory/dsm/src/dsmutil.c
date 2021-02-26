/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              D S M U T I L

GENERAL DESCRIPTION
  Utility functions for DMSS Data Services memory pool module.
  Function utilities here are designed for WCDMA.

EXTERNALIZED FUNCTIONS

  dsm_sdu_to_pkt_chain_byte()
    Copy the given SDU to an allocating DSM item packet in size bytes.
    
  dsm_sdu_to_pkt_chain_bit()
    Copy the given SDU to an allocating DSM item packet in size bits.
    
  dsm_pkt_to_sdu_chain_byte()
    Pullup whole packet in dsm item chain into the provided SDU buffer.
    it returns the copied length in bytes.
    
  dsm_pkt_to_sdu_chain_bit()
    Pullup whole packet in dsm item chain into the provided SDU buffer.
    it returns the copied length in bits.
    
  dsm_bit_length_packet()
    Return the number of payload bits in the passed packet buffer chain.
    The bit length is stored in the app_field in the dsm_item_type structure
    when dsm_sdu_to_pkt_chain_bit() is used to create the packet.
    
  dsm_create_empty_pkt()
    Create an empty DSM item chain according to the required packet size.

  dsm_set_sdu_cnf()
    Allow user to turn on/off of the SDU confirm flag in an SDU packet.
    The LSB (bit) of the app_field is dedicated to the CNF flag. 
  
  dsm_write_sdu_mui()
    Write MUI (sequence number) in the app_field.
    This MUI uses the second byte of the app_filed.
    
  App_field usage:
    UL
      SDU MUI : |byte4|byte3|MUI byte|byte1|
      SDU CNF BIT : least significant bit
    DL
      Least 16 significant bits record packet length in bit.
      Most bit of the most 16 significant bits for AMR CRC error indication.

Copyright (c) 2000, 2001, 2004, 2008 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
  $PVCSPath:  L:/src/asw/MSM5200/dsm/vcs/dsmutil.c_v   1.7   06 Sep 2001 20:17:06   tliou  $
  $Header: //components/rel/core.qdsp6/2.1.c4/memory/dsm/src/dsmutil.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
03/04/11  ms      Reworked dsmutili_new_buffer_w_thresh API
02/04/11  ms      Updates to dsmutili_new_buffer_w_thresh - allocate from
                  specific DSM pool based on pool_size requested by DSM client
06/23/10  ms      Added typedef dsm_pool_level_evt_type
06/07/10  ms      Added support to log EVENT_DSM_POOL_LEVEL
06/03/10  ms      Added new API - dsmutili_new_buffer_w_thresh
01/25/10  ar      removed dsm_create_dup_empty_pkt() api as its no longer
                  used
10/16/09  sm      Remove unwanted featurizations and do associated clean up
01/09/09  sm      Fixed pool id check in dsmutili_new_buffer_flexible_by_pool_id().
                  Also, make use of macro DSM_FLEXIBLE_LARGE_ITEM_THRESHOLD in
                  function dsmutili_new_buffer_flexible() to determine best fit
                  dsm pool.
08/28/08  ar      Added function dsmutili_new_buffer_flexible_by_pool_id()
                  under feature FEATURE_DSM_DYNAMIC_POOL_SELECTION
07/11/08  sm      Replaced feature name FEATURE_DSM_DATA_PATH_OPT with 
                  FEATURE_DSM_DYNAMIC_POOL_SELECTION 
06/26/08  sm      1. Added dsmutili_new_buffer_flexible() to allocate DSM item
                     based on the size requested under feature FEATURE_DSM_DATA_PATH_OPT
                  2. Added dsmutili_new_packet_flexible() to allocate chain(if required)
                     of DSM item based on the size requested and select pool based on
                     requested size
                  3. Added dsmutili_new_packet() to allocate chain(if required)
                     of DSM item based on the size requested using indicated
                     pool_id
                  4. Renamed dsm_create_empty_pkt() under FEATURE_WCDMA_DL_DATA_PATH_OPT
                     to dsm_create_dup_empty_pkt() and removed featurization.
05/29/08  ar      Modified dsm_create_empty_pkt() to release DSM_POOL_LOCK in
                  case of error scenarios
04/24/08  ar      1.Changed macro name from DSMI_DS_SMALL_ITEM_SIZ to
                  DSM_DS_SMALL_ITEM_SIZ
                  2.Type casted sdu_ptr->dup_ptr->app_ptr to uint32 
                  as part of the compilation warning fix
03/21/08  ar      Added function dsm_create_empty_pkt() under feature
                  FEATURE_WCDMA_DL_DATA_PATH_OPT. Function fetches large DSM
                  items from the DSM pool and allocates memory from that DSM
                  item to packets.
03/14/08  ar      Changed code featurization from FEATURE_MBMS to FEATURE_MODEM_MBMS
02/07/08  vr      Support for FEATURE_MBMS. Added utility function to 
                  extract and return SFN of MCCH SDU
08/20/04  ttl     Added support to FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
09/06/01  ttl     Restored lost code due to code merging.
                  Added more condition checking messages.
06/07/01  ttl     Fixed the bug to use dsm_length_packet() to read the length
                  of the packet created by dsm_create_empty_pkt().
04/03/01  ttl     Include customer.h. Add more comments.
10/09/00  ttl     Creat.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */

#include "comdef.h"                /* Definitions for byte, word, etc.     */
#include "customer.h"              /* Customer Specific Features           */
#include "queue.h"                 /* Queue declaration package            */
#include "dsm.h"                   /* Data Service Memory pool definitions */
#include "msg.h"                   /* message declaration                  */
#include "err.h"
#include "memory.h"
#include "assert.h"
#include "dsmutil.h"
#include "event_defs.h"
#include "event.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
// Macro for feature FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
#define CRC_STATUS_BIT      0x80000000

#define SDU_CNF_BIT         0x00000001
#define SDU_CNF_MASK        0xFFFFFFFE
#define SDU_MUI_BYTE_MASK   0xFFFF00FF
#define SDU_MUI_BYTE        0x0000FF00

// Macros for FEATURE_MODEM_MBMS
#define SFN_MOD_MASK        0xFFFF
#define DSMUTIL_SFN_GT( sfn1, sfn2) \
    (((sfn1) != (sfn2)) && ((((sfn1) - (sfn2)) & 0x800) == 0))

/*=========================================================================**
** These event structures are often instantiated on the stack.  Because    **
** of this, they should be relatively small.  If for some reason they      **
** need to grow a significant amount, they should be moved from stack      **
** space to heap space.                                                    **
**=========================================================================*/

#if defined(FEATURE_DSM_LARGE_ITEMS) && defined(FEATURE_DSM_DUP_ITEMS)
/***************************
**  EVENT_DSM_POOL_LEVEL  **
****************************/
typedef PACKED struct
{
  /* Version field 
     Bit 0:3 Version
     Bit 4:7 Reserved
   */
  uint8  version;
  /* DSM level information
     Bit 0: DSM pool threshold low
            0: Threshold low for Small DSM pool
            1: Threshold low for Large DSM pool
   */
  uint8 info;
}
dsm_pool_level_evt_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/*===========================================================================
FUNCTION DSMUTILI_NEW_BUFFER_FLEXIBLE()

DESCRIPTION
  This function will return a pointer to the allocated DSM item based on
  the size requested.

  Size of the DSM item allocated can be smaller than requested if:
    1. requested size is bigger than LARGE POOL SIZE
    2. UE runs out of Best fit (and larger) DSM POOL and fall_back is set to TRUE

  Caller of this function is expected to make sure whether the requested size is
  indeed allocated or not. 
  DSM_ITEM_SIZE() can be used to get the size of the DSM item allocated.


DEPENDENCIES
  None.


PARAMETERS
  dsm_size_requested - memory size requested by the caller 
  fall_back - boolean to indicate whether this API should try to
              allocate from smaller pool if it runs out of best fit pool

RETURN VALUE
  NULL: If UE runs out of DSM items
  dsm_item_type *: pointer to the allocated DSM item

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsmutili_new_buffer_flexible
(
  dsm_mempool_id_type pool_id,  
  uint32 dsm_size_requested,
  boolean fall_back,
  const char * file,
  uint32 line
)
{
  dsm_item_type *allocated_dsm_ptr;

#ifdef FEATURE_DSM_DYNAMIC_POOL_SELECTION
 
  /* select pool if according to the memory size requested */
  if (dsm_size_requested > DSM_FLEXIBLE_LARGE_ITEM_THRESHOLD)
  {
    if (NULL == (allocated_dsm_ptr = dsmi_new_buffer(DSM_DS_LARGE_ITEM_POOL, file, line)))
    {
      if (TRUE == fall_back)
      {
        allocated_dsm_ptr = dsmi_new_buffer(DSM_DS_SMALL_ITEM_POOL, file, line);
      }
    }
  }
  else /* allocate small Item */
  {
    if (NULL == (allocated_dsm_ptr = dsmi_new_buffer(DSM_DS_SMALL_ITEM_POOL, file, line)))
    {
      allocated_dsm_ptr = dsmi_new_buffer(DSM_DS_LARGE_ITEM_POOL, file, line);
    }
  }
#else

  allocated_dsm_ptr = dsmi_new_buffer(pool_id, file, line);

#endif  

  return(allocated_dsm_ptr);
}

/*===========================================================================
FUNCTION DSMUTILI_NEW_BUFFER_W_THRESH()

DESCRIPTION
  This function will return a pointer to the allocated DSM item based on
  the size requested if the free count of the DSM pool exceeds the
  specified threshold.

  Size of the DSM item allocated can be smaller than requested if:
    1. DSM free count is smaller than specified threshold
    2. requested size is bigger than LARGE POOL SIZE
    3. UE runs out of Best fit (and larger) DSM POOL and fall_back is set to TRUE

  Caller of this function is expected to make sure whether the requested size is
  indeed allocated or not. 
  DSM_ITEM_SIZE() can be used to get the size of the DSM item allocated.


DEPENDENCIES
  None.


PARAMETERS
  pool_size - DSM_DS_SMALL_ITEM_SIZ or DSM_DS_LARGE_ITEM_SIZ as
              requested by the caller 
  fall_back - boolean to indicate whether this API should try to
              allocate from smaller pool if it runs out of best fit pool
  threshold - uint16 - If DSM item free count <= threshold, then do not
              allocate from that DSM pool

RETURN VALUE
  NULL: If UE runs out of DSM items
  dsm_item_type *: pointer to the allocated DSM item

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsmutili_new_buffer_w_thresh
(
  uint32 pool_size,
  boolean fall_back,
  uint16 threshold,
  const char * file,
  uint32 line
)
{
  dsm_item_type *allocated_dsm_ptr = NULL;
  dsm_pool_mgmt_table_type *table; /* table pointer */
  dsm_mempool_id_type pool_id; /* pool id */
  /* Populate the event structure */
  dsm_pool_level_evt_type dsm_pool_level_evt;

  dsm_pool_level_evt.version = 1;

  /* Choose appropriate table based on pool_size */
  if (pool_size == DSM_DS_SMALL_ITEM_SIZ)
  {
    pool_id = DSM_DS_SMALL_ITEM_POOL;
    table = (dsm_pool_mgmt_table_type*)DSM_DS_SMALL_ITEM_POOL;
  }
  else
  {
    pool_id = DSM_DS_LARGE_ITEM_POOL;
    table = (dsm_pool_mgmt_table_type*)DSM_DS_LARGE_ITEM_POOL;
  }

  /* If DSM pool count <= threshold, then switch to the other
     DSM pool */
  if (table->free_count <= threshold)
  {
    if (pool_size == DSM_DS_LARGE_ITEM_SIZ)
    {
      dsm_pool_level_evt.info = 1; /* Set Bit 0 to 1 to indicate large pool threshold low */
    }
    else
    {
      dsm_pool_level_evt.info = 0; /* Set Bit 0 to 0 to indicate small pool threshold low */
    }

    /* Ship the DSM Pool Level event off to the DM. */
    event_report_payload(EVENT_DSM_POOL_LEVEL,
                         sizeof(dsm_pool_level_evt),
                         (void*)&dsm_pool_level_evt);

    if (fall_back && (pool_size == DSM_DS_LARGE_ITEM_SIZ))
    {
      pool_id = DSM_DS_SMALL_ITEM_POOL;
      pool_size = DSM_DS_SMALL_ITEM_SIZ;
      table = (dsm_pool_mgmt_table_type*)DSM_DS_SMALL_ITEM_POOL;

      /* If DSM pool count > threshold, only then allocate from 
         Small Item Pool */
      if (table->free_count > threshold)
      {
        allocated_dsm_ptr = dsmi_new_buffer(pool_id, file, line);
      }
      else
      {
        dsm_pool_level_evt.info = 0; /* Set Bit 0 to 0 to indicate small pool threshold low */
        /* Ship the DSM Pool Level event off to the DM. */
        event_report_payload(EVENT_DSM_POOL_LEVEL,
                             sizeof(dsm_pool_level_evt),
                             (void*)&dsm_pool_level_evt);
      }
    }
  }
  else
  {
    allocated_dsm_ptr = dsmi_new_buffer(pool_id, file, line);
  }

  return(allocated_dsm_ptr);
}

/*===========================================================================
FUNCTION DSMUTILI_NEW_BUFFER_FLEXIBLE_BY_POOL_ID()

DESCRIPTION
  This function will return a pointer to the allocated DSM item. 
  If the requested item pool_id is DSM_DS_SMALL_ITEM_POOL or 
  DSM_DS_LARGE_ITEM_POOL,
    it uses the dsmutili_new_buffer_flexible() to allocate a new dsm item 
  Else 
    it uses the dsmi_new_buffer() to get the requested item.
  

  Size of the DSM item allocated can be smaller than requested if:
    1. requested size is bigger than LARGE POOL SIZE
    2. UE runs out of Best fit (and larger) DSM POOL and fall_back is set to TRUE

  Caller of this function is expected to make sure whether the requested size is
  indeed allocated or not. 
  DSM_ITEM_SIZE() can be used to get the size of the DSM item allocated.

DEPENDENCIES
  None.

PARAMETERS
  pool_id - DSM Pool from which new item should be allocated
  dsm_size_requested - memory size requested by the caller 
  fall_back - boolean to indicate whether this API should try to
              allocate from smaller pool if it runs out of best fit pool

RETURN VALUE
  NULL: If UE runs out of DSM items
  dsm_item_type *: pointer to the allocated DSM item

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsmutili_new_buffer_flexible_by_pool_id
(
  dsm_mempool_id_type pool_id,
  uint32 dsm_size_requested,
  boolean fall_back,
  const char * file,
  uint32 line
)
{
  dsm_item_type *allocated_dsm_ptr;

#ifdef FEATURE_DSM_DYNAMIC_POOL_SELECTION

  if ((pool_id == DSM_DS_SMALL_ITEM_POOL) || 
      (pool_id == DSM_DS_LARGE_ITEM_POOL))
  {
    allocated_dsm_ptr = 
      dsmutili_new_buffer_flexible(
        pool_id, dsm_size_requested, fall_back, file, line);
  }
  else
  {
    allocated_dsm_ptr = dsmi_new_buffer(pool_id, file, line);
  }
  return(allocated_dsm_ptr);

#else

  allocated_dsm_ptr = dsmi_new_buffer(pool_id, file, line);
  return (allocated_dsm_ptr);
  
#endif
}


/*===========================================================================

FUNCTION dsmutili_new_packet

DESCRIPTION
    Based on the dsm_size_requested, this function will allocate a dsm items
    which can hold 'dsm_size_requested'.
    This function will do necessary chaining of DSM items if requested size
    doesnt fit in one DSM item.
    
    Total Size of the DSM chain is expected to be equal or greater than
    requested size.

DEPENDENCIES
    None

PARAMETERS
  pool_id - DSM Pool from which new item should be allocated

  dsm_size_requested - requested size of the buffer

RETURN VALUE
  Chain of DSM pointers w/ empty payload such that total size of chain is 
  equal to or greater than dsm_size_requested. It returns NULL if it fails 
  to allocate requested size.

SIDE EFFECTS
  Caller of this function is expected to have necessary handling whenever this
  function returns chain of DSM items.

===========================================================================*/
dsm_item_type *dsmutili_new_packet
(
  dsm_mempool_id_type pool_id,
  uint32 dsm_size_requested,
  const char * file,
  uint32 line
)
{
  dsm_item_type *head_dsm_ptr = NULL, *tail_dsm_ptr = NULL;
  dsm_item_type *allocated_dsm_ptr = NULL;
  int16 remaining_size = (int16) dsm_size_requested;
  uint16 allocated_dsm_size = DSM_POOL_ITEM_SIZE(pool_id);

  while (remaining_size > 0)
  {
    if (NULL == (allocated_dsm_ptr = dsmi_new_buffer(pool_id, file, line)))
    {
      /* No free DSM items are available in a given pool id,
         exit out of the loop */
      break;
    }

    if (remaining_size != (int16) dsm_size_requested)
    {
      tail_dsm_ptr->pkt_ptr = allocated_dsm_ptr;
      tail_dsm_ptr = allocated_dsm_ptr;
    }
    else
    {
      head_dsm_ptr = allocated_dsm_ptr;
      tail_dsm_ptr = allocated_dsm_ptr;
    }

    /* update remaining size */
    remaining_size -= allocated_dsm_size;
  }

  /* check whether a given pool has run out of the free items.
     If yes, then free any allocated dsm items */
  if (NULL != allocated_dsm_ptr)
  {
    return(head_dsm_ptr);
  }
  else
  {
    dsm_free_packet(&head_dsm_ptr);
    head_dsm_ptr = NULL;
    return(head_dsm_ptr);
  }
}



/*===========================================================================

FUNCTION dsm_new_packet_flexible

DESCRIPTION            
  Based on the dsm_size_requested, function dsmutili_new_packet_flexible() 
  will identify the best pool (SMALL or LARGE) and allocates first item 
  from that pool.

  This function will return a pointer to the DSM item chain based on
  the size requested.
  
  Total Size of the DSM chain is expected to be equal or greater than
  requested size.

DEPENDENCIES
  None

PARAMETERS
  dsm_size_requested - requested size of the buffer

RETURN VALUE
  Chain of DSM pointers w/ empty payload such that total size of chain is 
  equal to or greater than dsm_size_requested. It returns NULL if it fails 
  to allocate requested size.

SIDE EFFECTS
  Caller of this function is expected to have necessary handling whenever this
  function returns chain of DSM items.

===========================================================================*/
dsm_item_type *dsmutili_new_packet_flexible
(
  dsm_mempool_id_type pool_id,
  uint32 dsm_size_requested,
  const char * file,
  uint32 line
)
{
#ifdef FEATURE_DSM_DYNAMIC_POOL_SELECTION
  dsm_item_type *head_dsm_ptr = NULL, *tail_dsm_ptr = NULL;
  dsm_item_type *allocated_dsm_ptr = NULL;
  int16 remaining_size = dsm_size_requested;
  uint16 allocated_size = 0;

  while (remaining_size > 0)
  {
    /* select pool if according to the memory size requested */
    if (remaining_size > DSM_FLEXIBLE_LARGE_ITEM_THRESHOLD)
    {
      allocated_size = DSM_DS_LARGE_ITEM_SIZ;
      if (NULL == (allocated_dsm_ptr = dsmi_new_buffer(DSM_DS_LARGE_ITEM_POOL, file, line)))
      {
        allocated_size = DSM_DS_SMALL_ITEM_SIZ;
        if (NULL == (allocated_dsm_ptr = dsmi_new_buffer(DSM_DS_SMALL_ITEM_POOL, file, line)))
        {
          /* All the free items have been consumed from both the pools,
             exit out of the loop */
          break;
        }
      }
    }
    else /* allocate SMALL Item */
    {
      allocated_size = DSM_DS_SMALL_ITEM_SIZ;
      if (NULL == (allocated_dsm_ptr = dsmi_new_buffer(DSM_DS_SMALL_ITEM_POOL, file, line)))
      {
        allocated_size = DSM_DS_LARGE_ITEM_SIZ;
        if (NULL == (allocated_dsm_ptr = dsmi_new_buffer(DSM_DS_LARGE_ITEM_POOL, file, line)))
        {
          /* All the free items have been consumed from both the pools,
             exit out of do..while loop */
          break;
        }
      }
    }

    if ((uint32)remaining_size != dsm_size_requested)
    {
      tail_dsm_ptr->pkt_ptr = allocated_dsm_ptr;
      tail_dsm_ptr = allocated_dsm_ptr;
    }
    else
    {
      head_dsm_ptr = allocated_dsm_ptr;
      tail_dsm_ptr = allocated_dsm_ptr;
    }
    /* update remaining size */
    remaining_size -= allocated_size;
  }

  /* check whether both the pools have run out of the free items.
     If yes, then free any allocated dsm items */
  if (NULL != allocated_dsm_ptr)
  {
    return(head_dsm_ptr); 
  }
  else
  {
    dsm_free_packet(&head_dsm_ptr);
    head_dsm_ptr = NULL;
    return(head_dsm_ptr);
  }
#else
  return (dsmutili_new_packet(pool_id, dsm_size_requested, __FILE__,__LINE__));
#endif
}

/*===========================================================================

FUNCTION DSM_SDU_TO_PKT_CHAIN_BYTE
DESCRIPTION
  Copy the given SDU to an allocating DSM item packet in size bytes.

DEPENDENCIES
  None.

RETURN VALUE
  The allocated dsm item packet chain pointer. 
  If failed, it returns NULL pointer.

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dsm_sdu_to_pkt_chain_byte
(
  void   *buf,                    /* Ptr. to bytes to copy                */
  word    size                    /* size in bytes of copying             */
)
{
  dsm_item_type *pkt_head_ptr = NULL; /* Address of ptr to head of packet  */

  if ((dsm_pushdown_tail(&pkt_head_ptr, 
                         buf, 
                         size, 
                         DSM_DS_SMALL_ITEM_POOL)) != size)
  {
    /* Failed to allocated needed dsm items. Release those dsm    */
    /* item being allocated and set pkt_head_ptr to NULL          */
    dsm_free_packet(&pkt_head_ptr);
    pkt_head_ptr = NULL;
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Failed to obtain enough memory");
  }
  return (pkt_head_ptr);
}


/*===========================================================================

FUNCTION DSM_SDU_TO_PKT_CHAIN_BIT

DESCRIPTION            
  Copy the given SDU to an allocating DSM item packet in size bits.
  
  In addition, the total written bit size of this packet chain will be 
  stored in the app_field of the packet header.

DEPENDENCIES
  None.
  
RETURN VALUE
  The number of bits that were pushed on to the tail end of the last item.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsm_sdu_to_pkt_chain_bit
(
  void   *buf,                    /* Ptr. to bytes to copy                */
  word    size                    /* size in bits of copying              */
)
{
  dsm_item_type *pkt_head_ptr = NULL; /* Address of ptr to head of packet  */
  uint16         size_tmp;


  /* convert size in bits into in bytes */
  size_tmp = (size + 7) >> 3;

  pkt_head_ptr = dsm_sdu_to_pkt_chain_byte(buf, size_tmp);

  if (pkt_head_ptr != NULL)
  {
    size_tmp = (uint16)pkt_head_ptr->app_field;
    size_tmp += size;
    /* update packet length into app_field in bits.*/
    pkt_head_ptr->app_field &= 0xFFFF0000;
    pkt_head_ptr->app_field |= size_tmp;
  }

  return (pkt_head_ptr);
}


/*===========================================================================

FUNCTION DSM_PKT_TO_SDU_CHAIN_BYTE
DESCRIPTION
  Pullup whole packet in chain into provided SDU buffer and return the copied
  length in bytes.

DEPENDENCIES
  None.

RETURN VALUE
  The number of bytes that read into the provided buffer

SIDE EFFECTS
  The last byte being pulled out from a dsm item, this dsm item is returned 
  to the free pool. 

===========================================================================*/
uint16 dsm_pkt_to_sdu_chain_byte
(
  dsm_item_type **pkt_head_ptr,    /* Address of ptr to head of packet     */
  void          *buf               /* Ptr. to bytes to copy                */
)
{
  return (dsm_pullup(
    pkt_head_ptr, buf, (uint16) dsm_length_packet(*pkt_head_ptr)));
}


/*===========================================================================

FUNCTION DSM_PKT_TO_SDU_CHAIN_BIT

DESCRIPTION            
  Pullup whole packet in chain into provided SDU buffer and return the copied
  length in bits.
  
  The bit size is read from the app_field in the packet header.

DEPENDENCIES
  None.
  
RETURN VALUE
  The number of bits read into the provided buffer.

SIDE EFFECTS
  The last byte being pulled out from a dsm item, this dsm item is returned 
  to the free pool. 
===========================================================================*/
word dsm_pkt_to_sdu_chain_bit
(
  dsm_item_type **pkt_head_ptr,     /* Address of ptr to head of packet     */
  void          *buf                /* Ptr. to bits to copy                 */
)
{
  uint16  size = 0, size_tmp, size_read;

  if ((*pkt_head_ptr)->app_field > 0)
  {
    size = (uint16)((*pkt_head_ptr)->app_field);

    /* convert size in bits into one in bytes */
    size_read = (size + 7) >> 3;

    size_tmp = dsm_pullup(pkt_head_ptr, buf, size_read);

    /* The size pulled out from the dsm items chain is not the same as  */
    /* the recorded size in the app_field. Some thing is wrong. Give    */
    /* what we have read in bits, and report an ERROR.                  */
    if (size_tmp != size_read)
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"DSM read error in bit");
      /* size is counted in bits  */
      size = size_tmp * 8; 
    }
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"DSM App_field=0");
  }

  return (size);
}


/*===========================================================================

FUNCTION DSM_BIT_LENGTH_PACKET

DESCRIPTION
  Return the number of payload bits in the passed packet buffer chain.
  The bit length is stored in the app_field in the dsm_item_type structure
  when dsm_sdu_to_pkt_chain_bit() is used to create the packet.

DEPENDENCIES
  dsm_sdu_to_pkt_chain_bit() is used to created the packet, which stores
  the bit length in the app_field of the dsm_item_type structure.
  If dsm_sdu_to_pkt_chain_bit() is not used to create the packet, then
  the developer must ensure that the app_field is correctly set in order
  to use this function.


RETURN VALUE
  The number of bits used in the packet.

SIDE EFFECTS
  None
===========================================================================*/

word dsm_bit_length_packet
(
  dsm_item_type *item_ptr       /* pointer to head of packet  */
)
{
  uint16 cnt = 0;                 /* temporary counter variable */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while(item_ptr != NULL)
  {
    cnt      += (uint16)item_ptr->app_field;
    item_ptr  = item_ptr->pkt_ptr;
  }
  return( cnt);
}


/*===========================================================================

FUNCTION DSM_CREATE_EMPTY_PKT

DESCRIPTION            
  Create an empty DSM items chain by the required packet size.
  it is limited to use small dsm itme pool only.
  
  To use dsm_length_packet() to obtain the length of this empty buffer chain,
  the return value of the dsm_length_packet() is exactly the pkt_size, but 
  the allocated size might be bigger up to the size of the allocated DSM items.

DEPENDENCIES
  None.
  
RETURN VALUE
  Packet pointer w/ empty payload. It returns NULL if it fails to 
  allocate dsm items.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsm_create_empty_pkt
(
  word  pkt_size                    /* Packet size in bytes                */
)
{
  dsm_item_type *pkt_head_ptr = NULL;    /* ptr to last item in pkt chain  */
  dsm_item_type *last_item_ptr = NULL;   /* ptr to last item in pkt chain  */
  dsm_item_type *tmp_item_ptr = NULL;    /* tmp ptr for newly alloc item   */
  word     bytes_allocated = 0;          /* total number of bytes copied   */


  while (bytes_allocated < pkt_size)
  {

    /*---------------------------------------------------------------------
      Allocate a new dsm item from the small item pool.
    ---------------------------------------------------------------------*/
    if ((tmp_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)) == NULL)
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unable to allocate new item!");
      if (pkt_head_ptr != NULL)
      {
        /* Free the dsm items, if they are allocated before failed */
        dsm_free_packet(&pkt_head_ptr);
        pkt_head_ptr = NULL;
      }
      break;
    }

    /*---------------------------------------------------------------------
      Chain the item in as the last item in the packet chain.
    ---------------------------------------------------------------------*/
    /* This DSM item is the first one in the packet.  */
    if (pkt_head_ptr == NULL)
    {
      pkt_head_ptr = tmp_item_ptr;
      last_item_ptr = pkt_head_ptr;
    }
    /* Some DSM items are in the packet already.  */
    else
    {
      last_item_ptr->pkt_ptr = tmp_item_ptr;
      last_item_ptr = tmp_item_ptr;
    }
    last_item_ptr->used = MIN(last_item_ptr->size, (pkt_size - bytes_allocated));
    bytes_allocated += last_item_ptr->size;

  } /* while (bytes_copied < size) */

  return (pkt_head_ptr);
}


/*===========================================================================

FUNCTION DSM_SET_SDU_CNF

DESCRIPTION            
  Allow user to turn on/off of the SDU confirm flag in an SDU packet.
  
  The LSB (bit) of the app_field is dedicated to the CNF flag. 

DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsm_set_sdu_cnf
(
  dsm_item_type *sdu_pkt,                      /* Point to the SDU packet. */
  boolean        flag                          /* CNF flag.                */
)
{
  if (sdu_pkt != NULL)
  {
    /* set the SDU confirmation flag. */
    if (flag)
    {
      sdu_pkt->app_field |= (uint32)SDU_CNF_BIT;
    }
    /* reset the SDU confirmation flag. */
    else
    {
      sdu_pkt->app_field &= (uint32)SDU_CNF_MASK;
    }
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"set_sdu_cnf:empty SDU");
  }
}


/*===========================================================================

FUNCTION DSM_WRITE_SDU_CNF

DESCRIPTION            
  Write MUI (sequence number) in the app_field.
  This MUI uses the second byte of the app_field.

DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsm_write_sdu_mui
(
  dsm_item_type *sdu_pkt,                      /* Point to the SDU packet. */
  byte           sdu_mui               /* SDU MUI (i.e. SDU sequence number) */
)
{
  if (sdu_pkt != NULL)
  {
    /* reset the SDU sequence number part in app_field */
    sdu_pkt->app_field &= (uint32)SDU_MUI_BYTE_MASK;
    /* write the MUI filed into app_field */
    sdu_pkt->app_field |= ((((uint32)sdu_mui) << 8) & SDU_MUI_BYTE);
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"write_sdu_mui:empty SDU");
  }
}


/*===========================================================================

FUNCTION DSM_SET_CRC_STATUS_BIT

DESCRIPTION            
  Set CRC status bit.

DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 dsm_set_crc_status_bit 
(
  dsm_item_type **pkt_head_ptr    /* Address of ptr to head of packet     */
)
{
  uint16 status = TRUE;

  if (pkt_head_ptr && (*pkt_head_ptr))
  {
    (*pkt_head_ptr)->app_field |= CRC_STATUS_BIT;
  }
  else
  {
    status = FALSE;
  }

  return status;
}


/*===========================================================================

FUNCTION DSM_READ_CRC_STATUS_BIT

DESCRIPTION            
  Read CRC status bit.

DEPENDENCIES
  None.
  
RETURN VALUE
  1 for bit set, 0 for bit not set.

SIDE EFFECTS
  None
===========================================================================*/
uint16 dsm_read_crc_status_bit 
(
  dsm_item_type **pkt_head_ptr    /* Address of ptr to head of packet     */
)
{
  uint16 status_bit = 0;

  if (pkt_head_ptr && (*pkt_head_ptr))
  {
    if (((*pkt_head_ptr)->app_field & CRC_STATUS_BIT) == CRC_STATUS_BIT)
    {
      status_bit = 1;
    }
  }

  return status_bit;
}

/*===========================================================================

FUNCTION DSM_GET_SFN_FOR_DL_SDU

DESCRIPTION 
  Obtain the latest SFN of the SDU DSM chain.
  In MCCH for MBMS, the SFN is stroed in the app_ptr by L1.


DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsm_get_sfn_for_dl_sdu
(
  dsm_item_type *sdu_ptr    /* Address of ptr to SDU packet     */
)
{
  uint32 sfn = SFN_MOD_MASK;
  uint32 new_sfn = SFN_MOD_MASK;

  /* get the inital number for sfn */
  if (sdu_ptr != NULL)
  {
    if (sdu_ptr->pool_id != DSM_DUP_ITEM_POOL)
    {
      sfn = (uint32)sdu_ptr->app_ptr;
    }
    else
    {
        sfn = (uint32)sdu_ptr->dup_ptr->app_ptr;
    }
    /* Go to the next DSM item in the chain */
    sdu_ptr = sdu_ptr->pkt_ptr;
  }
  while (sdu_ptr != NULL)
  {
    if (sdu_ptr->pool_id != DSM_DUP_ITEM_POOL)
    {
      new_sfn = (uint32)sdu_ptr->app_ptr;
    }
    else
    {
        new_sfn = (uint32)sdu_ptr->dup_ptr->app_ptr;
    }
    if (DSMUTIL_SFN_GT(new_sfn, sfn))
    {
      sfn = new_sfn;
    }

    /* Go to the next DSM item in the chain */
    sdu_ptr = sdu_ptr->pkt_ptr;
  }

  return sfn;
}

#endif /* #if defined(FEATURE_DSM_LARGE_ITEMS) && defined(FEATURE_DSM_DUP_ITEMS)*/
