#ifndef DSM_UTIL_H
#define DSM_UTIL_H

/*===========================================================================

                D A T A   S E R V I C E   M E M O R Y   P O O L

            F U N C T I O N  U T I L I T Y  H E A D E R    F I L E

DESCRIPTION
  This file contains types and declarations associated with function utillties 
  of the DMSS Data Service Memory pool and services for WCDMA.

Copyright (c) 2000, 2004, 2008 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath:  L:/src/asw/MSM5200/dsm/vcs/dsmutil.h_v   1.5   17 Jul 2001 10:35:16   kwolver  $ 
  $Header: //components/rel/core.qdsp6/2.1/api/memory/dsmutil.h#1 $  $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   ms      Updates to dsmutili_new_buffer_w_thresh - allocate from
                   specific DSM pool based on pool_size requested by DSM client
06/23/10   ms      Removed header file inclusion and moved event typedef to .c
06/07/10   ms      Added support to log EVENT_DSM_POOL_LEVEL
06/03/10   ms      Added new API - dsmutili_new_buffer_w_thresh
01/25/10   ar      removed dsm_create_dup_empty_pkt() api as its no longer
                   used
10/16/09   sm      Remove featurizations from header file
08/13/09   sm      Removed DSM_ITEM_SUBSYSTEM_DIAG
03/25/09   etv     Added DSM_ITEM_SUBSYSTEM_HDR
01/19/09   sm      Added MACRO DSM_FLEXIBLE_LARGE_ITEM_THRESHOLD 
08/28/08   ar      Added dsm_new_buffer_flexible_by_pool_id() under feature
                   FEATURE_DSM_DYNAMIC_POOL_SELECTION
07/11/08   sm      Replaced feature name FEATURE_DSM_DATA_PATH_OPT with 
                   FEATURE_DSM_DYNAMIC_POOL_SELECTION 
06/26/08   sm      1. Added dsm_new_buffer_flexible() to allocate DSM item
                      based on the size requested
                   2. Added DSM_ITEM_GET/SET_SIZE) to get/set "size" field of a 
                      given dsm item
                   3. Added dsm_new_packet_flexible() to allocate chain (if required)
                      of DSM item based on the size requested
                   4. Added dsm_new_buffer_explicit() to explicitly provide file and
                      line number from where DSM item needs to be allocated
03/21/07   ar      1.Added macro DSMI_ALIGN_PKT_SIZE_TO_64_BYTES in order to align
                     a given packet size to 64 byte boundary.
                   2.Added macro DSMI_MEM_THRESHOLD_TO_FREE_DSM to set the
                     memory threshold for freeing the DSM item.
                   3.Added macro DSMI_THRESHOLD_FOR_COMPLETE_ITEM_ALLOCATION
                     for setting the memory threshold for allocating a complete DSM item.
                   4.Added function prototype dsm_create_empty_packet() under feature
                     FEATURE_WCDMA_DL_DATA_PATH_OPT
03/14/08   ar      Changed code featurization from FEATURE_MBMS to FEATURE_MODEM_MBMS
02/07/08   vr      Support for FEATURE_MBMS. Added function prototype for 
                   dsm_get_sfn_for_dl_sdu().
09/17/04   ttl     Removed definition of FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
08/20/04   ttl     Added dsm_set_crc_status_bit() and dsm_read_crc_status_bit().
10/09/00   ttl     Creat.


===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "queue.h"      /* Queue declaration package            */
#include "dsm.h"        /* Data Service Memory pool definitions */

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/

/* MACRO to return the size of any dsm_ptr */
#define DSM_ITEM_GET_SIZE(dsm_ptr) ((dsm_ptr)->size)

/* MACRO to set the size of any dsm_ptr */
#define DSM_ITEM_SET_SIZE(dsm_ptr, new_size) ((dsm_ptr)->size = (new_size))

/* MACRO to return the used of any dsm_ptr */
#define DSM_ITEM_GET_USED(dsm_ptr) ((dsm_ptr)->used)

/* MACRO to set the used of any dsm_ptr */
#define DSM_ITEM_SET_USED(dsm_ptr, new_used) ((dsm_ptr)->used = (new_used))

/* MACRO to indicate threshold to in bytes to allocate from large pool
   in function dsmutili_new_buffer_flexible() whenever requested size
   is bigger than this threshold. Set this to 512 for both Rel7 and non Rel7 */
#define DSM_FLEXIBLE_LARGE_ITEM_THRESHOLD 512

/*===========================================================================

FUNCTION dsm_new_buffer_flexible

DESCRIPTION            
  If FEATURE_DSM_DYNAMIC_POOL_SELECTION is not defined
    this macro calls dsmi_new_buffer using pool_id
  else if FEATURE_DSM_DYNAMIC_POOL_SELECTION is defined
    Based on the dsm_size_requested, this function will identify the best
    pool (SMALL or LARGE) and allocate from that pool.

    This function will return a pointer to the allocated DSM item based on
    the size requested.
  
    NOTE: Size of the DSM item allocated can be smaller than requested if:
            1. requested size is bigger than LARGE POOL SIZE
            2. UE runs out of Best fit (and larger) pools and fall_back is 
               set to TRUE
  
    Caller of this function is expected to have necessary handling for the case
    indicated in above two points i.e. if allocated size is less than requested
    size.
    (DSM_ITEM_SIZE(dsm_ptr) can be used to get the size of the DSM item
     allocated)

DEPENDENCIES
  None.

PARAMETERS
  pool_id - DSM Pool from which new item should be allocated if 
            FEATURE_DSM_DYNAMIC_POOL_SELECTION is not enabled

  dsm_size_requested - requested size of the buffer

  fall_back - boolean to indicate whether this API should try to allocate 
              from SMALL DSM pool if it runs out of LARGE DSM pool

  sub_system_id - subsytem id of the caller (for future use)
    Currently following subsystems are supported:
    DSM_ITEM_SUBSYSTEM_WCDMA,
    DSM_ITEM_SUBSYSTEM_PS,
    DSM_ITEM_SUBSYSTEM_HSUSB,
    DSM_ITEM_SUBSYSTEM_GERAN,
    DSM_ITEM_SUBSYSTEM_SMD,
    DSM_ITEM_SUBSYSTEM_BT, <bluetooth>
    DSM_ITEM_SUBSYSTEM_RESERVED1,
    DSM_ITEM_SUBSYSTEM_RESERVED2,
    DSM_ITEM_SUBSYSTEM_RESERVED3,
    DSM_ITEM_SUBSYSTEM_RESERVED4,
    DSM_ITEM_SUBSYSTEM_LEGACY, // for legacy API calls
    DSM_ITEM_SUBSYSTEM_UNKNOWN, // use this if user subsystem is unknown or
                                // not defined

RETURN VALUE
  Packet pointer w/ empty payload. It returns NULL if it fails to 
  allocate dsm item.

SIDE EFFECTS
  Caller of this function is expected to have necessary handling for the case
  indicated in above two points i.e. if allocated size is less than requested
  size.
  (DSM_ITEM_SIZE(dsm_ptr) can be used to get the size of the DSM item
   allocated)
===========================================================================*/

#define dsm_new_buffer_flexible(pool_id, dsm_size_requested, fall_back, sub_system_id) \
  dsmutili_new_buffer_flexible(pool_id, dsm_size_requested, fall_back, __FILE__,__LINE__)

#define dsm_new_buffer_w_thresh(pool_id, pool_size, fall_back, dsm_threshold, sub_system_id) \
  dsmutili_new_buffer_w_thresh(pool_size, fall_back, dsm_threshold, __FILE__,__LINE__)

/*===========================================================================

FUNCTION dsm_new_buffer_flexible_by_pool_id

DESCRIPTION            
  If FEATURE_DSM_DYNAMIC_POOL_SELECTION is not defined
    this macro calls dsmi_new_buffer using pool_id
  else in case the FEATURE_DSM_DYNAMIC_POOL_SELECTION is defined
    If the requested item pool_id is DSM_DS_SMALL_ITEM_SIZ or 
    DSM_DS_LARGE_ITEM_POOL, 
      it uses the dsmutili_new_buffer_flexible() to allocate a new dsm item. 
    Else 
      it uses the dsmi_new_buffer() to get the requested item.
    
    This function will return a pointer to the allocated DSM item based on
    the size requested.
  
    NOTE: Size of the DSM item allocated can be smaller than requested if:
            1. requested size is bigger than LARGE POOL SIZE
            2. UE runs out of Best fit (and larger) pools and fall_back is 
               set to TRUE
  
    Caller of this function is expected to have necessary handling for the case
    indicated in above two points i.e. if allocated size is less than requested
    size.
    (DSM_ITEM_SIZE(dsm_ptr) can be used to get the size of the DSM item
     allocated)

DEPENDENCIES
  None.

PARAMETERS
  pool_id - DSM Pool from which new item should be allocated

  dsm_size_requested - requested size of the buffer

  fall_back - boolean to indicate whether this API should try to allocate 
              from SMALL DSM pool if it runs out of LARGE DSM pool

  sub_system_id - subsytem id of the caller (for future use)
    Currently following subsystems are supported:
    DSM_ITEM_SUBSYSTEM_WCDMA,
    DSM_ITEM_SUBSYSTEM_PS,
    DSM_ITEM_SUBSYSTEM_HSUSB,
    DSM_ITEM_SUBSYSTEM_GERAN,
    DSM_ITEM_SUBSYSTEM_SMD,
    DSM_ITEM_SUBSYSTEM_BT, <bluetooth>
    DSM_ITEM_SUBSYSTEM_RESERVED1,
    DSM_ITEM_SUBSYSTEM_RESERVED2,
    DSM_ITEM_SUBSYSTEM_RESERVED3,
    DSM_ITEM_SUBSYSTEM_RESERVED4,
    DSM_ITEM_SUBSYSTEM_LEGACY, // for legacy API calls
    DSM_ITEM_SUBSYSTEM_UNKNOWN, // use this if user subsystem is unknown or
                                // not defined

RETURN VALUE
  Packet pointer w/ empty payload. It returns NULL if it fails to 
  allocate dsm item.

SIDE EFFECTS
  Caller of this function is expected to have necessary handling for the case
  indicated in above two points i.e. if allocated size is less than requested
  size.
  (DSM_ITEM_SIZE(dsm_ptr) can be used to get the size of the DSM item
   allocated)
===========================================================================*/

#define dsm_new_buffer_flexible_by_pool_id(pool_id, dsm_size_requested, fall_back, sub_system_id) \
  dsmutili_new_buffer_flexible_by_pool_id(pool_id, dsm_size_requested, fall_back, __FILE__,__LINE__)

/*===========================================================================

FUNCTION dsm_new_packet_flexible

DESCRIPTION            
  If FEATURE_DSM_DYNAMIC_POOL_SELECTION is not defined
    Calls dsmutili_new_packet using pool_id and dsm_size_requested
    to allocate chain of DSM items for dsm_size_requested.
  else if FEATURE_DSM_DYNAMIC_POOL_SELECTION is defined
    Based on the dsm_size_requested, function will identify the best pool
    (SMALL or LARGE) for dsm_size_requested<and remaining_size after first
    DSM item allocation> and allocates from that pool.

    This function will return a pointer to the DSM item chain based on
    the size requested.
  
    Total Size of the DSM chain will be equal or greater than requested size.

DEPENDENCIES
    None

PARAMETERS
  pool_id - DSM Pool from which new item should be allocated if 
            FEATURE_DSM_DYNAMIC_POOL_SELECTION is not enabled

  dsm_size_requested - requested size of the buffer

  sub_system_id - subsytem id of the caller (for future use)
    Currently following subsystems are supported:
    DSM_ITEM_SUBSYSTEM_WCDMA,
    DSM_ITEM_SUBSYSTEM_PS,
    DSM_ITEM_SUBSYSTEM_HSUSB,
    DSM_ITEM_SUBSYSTEM_GERAN,
    DSM_ITEM_SUBSYSTEM_SMD,
    DSM_ITEM_SUBSYSTEM_BT, <bluetooth>
    DSM_ITEM_SUBSYSTEM_RESERVED1,
    DSM_ITEM_SUBSYSTEM_RESERVED2,
    DSM_ITEM_SUBSYSTEM_RESERVED3,
    DSM_ITEM_SUBSYSTEM_RESERVED4,
    DSM_ITEM_SUBSYSTEM_LEGACY, // for legacy API calls
    DSM_ITEM_SUBSYSTEM_UNKNOWN, // use this if user subsystem is unknown or
                                // not defined

RETURN VALUE
  Chain of DSM pointers w/ empty payload such that total size of chain is 
  equal to or greater than dsm_size_requested. It returns NULL if it fails 
  to allocate requested size.

SIDE EFFECTS
  Caller of this function is expected to have necessary handling whenever this
  function returns chain of DSM items.

===========================================================================*/
#define dsm_new_packet_flexible(pool_id, dsm_size_requested, sub_system_id) \
  dsmutili_new_packet_flexible(pool_id, dsm_size_requested, __FILE__,__LINE__)

/*===========================================================================
FUNCTION DSM_NEW_BUFFER_EXPLICIT()

DESCRIPTION
  This function will return a pointer to the next free item from the free
  queue. Pool id is passed in as parameter. 
  This function explicitly takes file and line pointer as the argument.

DEPENDENCIES
  None.

PARAMETERS
  pool_id - Pool from which new item should be allocated. 
  file_ptr - file pointer of the file from which this macro is called
  line - line from which this macro is called

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_new_buffer_explicit(pool_id, file_ptr, line) \
  dsmi_new_buffer(pool_id, file_ptr, line)

/*---------------------------------------------------------------------------
  This MACRO computes the 128 byte aligned size of a given packet size
---------------------------------------------------------------------------*/

#define DSMI_ALIGN_PKT_SIZE_TO_64_BYTES(pkt_size) ((((pkt_size) + 63)>>6)<<6)

/* This MACRO represents the remaining memory threshold of a DSM item. 
  If there is lesser memory available in the DSM item than the threshold then
  free the DSM item. Note: Always set its value greater than or equal to packet
  alignment size in dsm_create_empty_pkt(). For ex: for a 128 byte aligned 
  packet DSMI_MEM_THRESHOLD_TO_FREE_DSM can take values greater than
  or equal to 128.*/
#define DSMI_MEM_THRESHOLD_TO_FREE_DSM      128

/* This MACRO represents the minimum packet size in order to allocate a 
  complete DSM item */
#define DSMI_THRESHOLD_FOR_COMPLETE_ITEM_ALLOCATION     1024


/*===========================================================================

                      ENUMERATIONS

===========================================================================*/
/* Subsystem Id to be used in dsm_new_buffer_flexible() */
typedef enum
{
  DSM_ITEM_SUBSYSTEM_WCDMA,
  DSM_ITEM_SUBSYSTEM_PS,
  DSM_ITEM_SUBSYSTEM_HSUSB,
  DSM_ITEM_SUBSYSTEM_GERAN,
  DSM_ITEM_SUBSYSTEM_SMD,
  DSM_ITEM_SUBSYSTEM_BT,
  DSM_ITEM_SUBSYSTEM_HDR,
  DSM_ITEM_SUBSYSTEM_RESERVED1,
  DSM_ITEM_SUBSYSTEM_RESERVED2,
  DSM_ITEM_SUBSYSTEM_RESERVED3,
  DSM_ITEM_SUBSYSTEM_LEGACY = 30, /* for legacy API calls */
  DSM_ITEM_SUBSYSTEM_UNKNOWN = 31, /* use this if user subsystem is unknown or
                                 not defined */
  DSM_ITEM_SUBSYSTEM_MAX = 32,
  DSM_ITEM_SUBSYSTEM_INVALID = 0xFFFF
} dsm_item_subsystem_id_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSMUTILI_NEW_BUFFER_FLEXIBLE()

DESCRIPTION
  This function will return a pointer to the allocated DSM item based on
  the size requested.

  Size of the DSM item allocated can be smaller than requested if:
    1. requested size is bigger than LARGE POOL SIZE
    2. UE runs out of Best fit (and larger) pools and fall_back is set to TRUE

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
  dsm_item_type *: pointer to allocated DSM item

SIDE EFFECTS
  Caller of this function is expected to make sure whether the requested size is
  indeed allocated or not. 
  DSM_ITEM_SIZE() can be used to get the size of the DSM item allocated.
===========================================================================*/
dsm_item_type *dsmutili_new_buffer_flexible
(
  dsm_mempool_id_type pool_id,
  uint32 dsm_size_requested,
  boolean fall_back,
  const char * file,
  uint32 line
);

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
);

/*===========================================================================
FUNCTION DSMUTILI_NEW_BUFFER_FLEXIBLE_BY_POOL_ID()

DESCRIPTION
  This function will return a pointer to the allocated DSM item. 
  If the requested item pool_id is DSM_DS_SMALL_ITEM_SIZ or 
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
);

/*===========================================================================

FUNCTION dsm_new_packet_flexible

DESCRIPTION            
  If FEATURE_DSM_DYNAMIC_POOL_SELECTION is not defined
    this macro calls dsmutili_new_packet using pool_id and dsm_size_requested.
  else if FEATURE_DSM_DYNAMIC_POOL_SELECTION is defined
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
);
  
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
);


/*===========================================================================

FUNCTION DSM_SDU_TO_PKT_CHAIN_BYTE
DESCRIPTION
  Allocates dsm items packet for SDU in WCDMA and reads the data from 
  the provided buffer for size bytes.

DEPENDENCIES
  None.

RETURN VALUE
  The allocated packet chain pointer. If failed, it'll return NULL pointer.

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dsm_sdu_to_pkt_chain_byte
(
  void   *buf,                    /* Ptr. to bytes to copy                */
  word    size                    /* size in bytes of pushdown            */
);


/*===========================================================================

FUNCTION DSM_SDU_TO_PKT_CHAIN_BIT

DESCRIPTION            
  Allocates dsm items packet for SDU in WCDMA and reads the data from 
  the provided buffer for size bits.
  
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
  word    size                    /* size in bytes of pushdown            */
);


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
  None.

===========================================================================*/
uint16 dsm_pkt_to_sdu_chain_byte
(
  dsm_item_type **pkt_head_ptr,    /* Address of ptr to head of packet     */
  void          *buf               /* Ptr. to bytes to copy                */
);


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
  None
===========================================================================*/
word dsm_pkt_to_sdu_chain_bit
(
  dsm_item_type **pkt_head_ptr,    /* Address of ptr to head of packet     */
  void          *buf               /* Ptr. to bytes to copy                */
);

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
);

/*===========================================================================

FUNCTION DSM_CREATE_EMPTY_PKT

DESCRIPTION            
  Create an empty DSM item chain according to the required packet size.
  This is limited to use small dsm itme pool only.

DEPENDENCIES
  None.
  
RETURN VALUE
  Packet pointer w/ empty payload. It'll return NULL if it fails to 
  allocate dsm items.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsm_create_empty_pkt
(
  word  pkt_size                    /* Packet size in bytes                */
);

/*===========================================================================

FUNCTION DSM_SET_SDU_CNF

DESCRIPTION            
  Allow user to turn on/off of the SDU confirm flag in a SDU packet.
  
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
);


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
);

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
);

/*===========================================================================

FUNCTION DSM_READ_CRC_STATUS_BIT

DESCRIPTION            
  Read CRC status bit.

DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 dsm_read_crc_status_bit 
(
  dsm_item_type **pkt_head_ptr    /* Address of ptr to head of packet     */
);

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
);
#endif /* DSM_UNIT_H */

