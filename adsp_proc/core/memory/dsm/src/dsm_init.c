/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M _ I N I T . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  dsm_init()
    Initialize the Data Services Memory pool unit.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1/memory/dsm/src/dsm_init.c#1 $

when        who    what, where, why
--------    ---    -------------------------------------------------------------
05/17/11    rj     Change DSMI_DS_LARGE_ITEM_CNT to new default value  
05/05/11    rj     Added ifndefs for pools counts and sizes 
03/22/11    rj     Removed HDR Pool
03/22/11    rj     Added ifndef for DSMI_DS_SMALL_ITEM_SIZ
12/23/10    ag     Add ifndefs for small and large item counts.   
10/07/10    ag     Avoid re-initialization of DSM pools.
05/24/10    akb    Introduced Feature FEATURE_A2_DUN_OPTIMIZED_DSM and wrapped around 
                   A2 DUN Small DSM counts for 9600/9200 builds
05/20/10    sch    Reduced Large and Small DSM counts to free up memory on ARM9
                   for 9k targets
2/11/10     kvk    Modified UMTS RLC thresholds inline with 42M data rates
12/04/09    sm     Modify large and small count to suffice LTE requirements and
                   done some clean up
10/09/08    kvk    Revert back to old DNE/FEW/MANY count for Large item based FC 
10/08/09    kvk    Moved the RLC DNE, FEW and MANY computation only for Q6
08/07/09    av     Enabling DUP based flow control algorithm in rlc & thresholds for it
08/05/09    kvk    Added support to compute RLC pool DNE, FEW and MANY thresholds
                   based on the target data rates
07/15/09    kvk    Increased DUP item count to handle the small IP frames/SDUs
07/07/09    kvk    Fixed compile error
07/07/09    kvk    Adjusted the RLC FC values as per review comments
07/01/09    av     Large DSM item based RLC FC changes
4/10/09     kvk    1)Reverted back the Small DSM item to 128
                   2)Increased A9 large DSM item count to 356 and 
                     reduced small item count to 1200 
1/09/08     kvk    Increased Small DSM item size to 256
04/09/09   bfc     Set up DSM pool callbacks for the SMD QMI bridge.
12/22/06    hrk    Added HDR specific initializations.
08/23/06    ptm    Remove oncrpc item pool.
07/24/06    hal    Added HDR message item pool
03/03/06    ptm    Added oncrpc dsm item pool.
10/10/94    jjw    Created file
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "dsm.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "assert.h"
#include "dsm_init.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
                         TARGET SPECIFIC ITEM SIZING
---------------------------------------------------------------------------*/

#ifndef DSMI_DS_SMALL_ITEM_SIZ
#define DSMI_DS_SMALL_ITEM_SIZ           128
#endif

  /*-------------------------------------------------------------------------
    Counts for various memory levels/marks for small items.

    DONT_EXCEED:    PPP starts dumping frames and the resequencing queue of
                    all the tcp connections is flushed
    LINK_LAYER_DNE: RLP resets
    TRANSPORT_FEW:  TCP stops adding out of order segments to resequencing
                    queue. Note that this is a count and not a memory level,
                    i.e., no callback is registered for this count.
    LINK_LAYER_FEW: RLP stops processing new frames. It entertains only
                    retransmissions.
    FEW:            The application is flow controlled while attempting 
                    socket writes, i.e., it gets EWOULDBLOCK.
    MANY:           The application is informed that it is no longer flow
                    controlled and socket writes succeed.
 
    Note: The counts corresponding to different memory levels should be in
          order: DONT_EXCEED <= LINK_LAYER_DNE <= LINK_LAYER_FEW <=
          FEW <= MANY. If one needs to change this order, the order of 
          levels should also be changed for dsm_mem_level_enum_type.
  -------------------------------------------------------------------------*/

#define DSMI_DS_SMALL_ITEM_DONT_EXCEED       3
#define DSMI_DS_SMALL_ITEM_LINK_LAYER_DNE    6
#define DSMI_DS_SMALL_ITEM_TRANSPORT_LAYER_FEW 25
#define DSMI_DS_SMALL_ITEM_LINK_LAYER_FEW    50

#if defined(IMAGE_QDSP6_PROC) || defined (WIN32)
  /* If feature FEATURE_DSM_DYNAMIC_POOL_SELECTION is defined then
      - SMALL item in MODEM PROC is needed only for GSM/GPRS/EDGE, WCDMA R99 DL,
        1x.
      - SMALL items are NOT needed for HSPA
      - SMD on uplink is expected to allocate DSM item selectively
      - WCDMA L1 DL for HSPA is expected to allocate DSM item selectively
      - WCDMA RLC is not expected to allocate any DSM item on UL path */
  #ifndef DSMI_DS_SMALL_ITEM_CNT
   #define DSMI_DS_SMALL_ITEM_CNT         3072
  #endif

  #define DSMI_DS_SMALL_ITEM_FEW_MARK        20
  #define DSMI_DS_SMALL_ITEM_SIO_FEW_MARK    25
  #define DSMI_DS_SMALL_ITEM_MANY_MARK       170
  #define DSMI_DS_SMALL_ITEM_SIO_MANY_MARK   175
  #define DSMI_DS_SMALL_ITEM_RLC_FEW_MARK    375
  #define DSMI_DS_SMALL_ITEM_RLC_MANY_MARK   550

  #define DSMI_DS_SMALL_ITEM_HDR_FEW_MARK    85
  #define DSMI_DS_SMALL_ITEM_HDR_MANY_MARK   110

  /*---------------------------------------------------------------------------
   Size, Count and count for different memory marks/levels for LARGE items. 
    The significants of the counts DONT_EXCEED, TRANSPORT_FEW, FEW and MANY
    is same as described above for small items.

   Sizing for Large Memory Pool items. Header size defines enough space for
   worst case TCP/IP/PPP header.  This should be:

   Default Max MSS + TCP Header Size + MAX TCP Options Size + IP Header Size
   + MAX IP Options Size + MAX PPP Header Size + PPP Checksum Size.

   The large item count needs to be increased if maximum-size SSL records
   are processed.
  ---------------------------------------------------------------------------*/
  #ifndef DSMI_DS_LARGE_ITEM_SIZ
    #define DSMI_DS_LARGE_ITEM_SIZ               1536
  #endif

  #ifndef DSMI_DS_LARGE_ITEM_CNT
    #define DSMI_DS_LARGE_ITEM_CNT               2000 
  #endif

  #define DSMI_DS_LARGE_ITEM_DONT_EXCEED       1  
  #define DSMI_DS_LARGE_ITEM_TRANSPORT_FEW     3
  #define DSMI_DS_LARGE_ITEM_LINK_LAYER_FEW    7
  #define DSMI_DS_LARGE_ITEM_FEW_MARK          7
  #define DSMI_DS_LARGE_ITEM_SIO_FEW_MARK      7
  #define DSMI_DS_LARGE_ITEM_SMD_FEW_MARK      10
  #define DSMI_DS_LARGE_ITEM_MANY_MARK         11
  #define DSMI_DS_LARGE_ITEM_SIO_MANY_MARK     11
  #define DSMI_DS_LARGE_ITEM_SMD_MANY_MARK     50
  /*Initializing DSM level for RLC large item FC*/
  #define DSMI_DS_LARGE_ITEM_LINK_LAYER_DNE    24
  #define DSMI_DS_LARGE_ITEM_RLC_FEW_MARK      210
  #define DSMI_DS_LARGE_ITEM_RLC_MANY_MARK     400

  uint32 dsmi_ds_large_item_rlc_dne_mark = DSMI_DS_LARGE_ITEM_LINK_LAYER_DNE;
  uint32 dsmi_ds_large_item_rlc_few_mark = DSMI_DS_LARGE_ITEM_RLC_FEW_MARK;
  uint32 dsmi_ds_large_item_rlc_many_mark = DSMI_DS_LARGE_ITEM_RLC_MANY_MARK;

  /*Define RLC params needed to compute to large item pool thresholds for Large item FC*/
  uint32 rlc_rtt       = 80;     // in the units of ms
  uint32 max_data_rate = 42000;  // in the units of Kbps

  /*---------------------------------------------------------------------------
   Size, Count, Few, many and do not exceed counts for DUP items 
   ---------------------------------------------------------------------------*/
  #ifndef DSMI_DUP_ITEM_CNT
    #define DSMI_DUP_ITEM_CNT                   8000
  #endif
  #define DSMI_DUP_ITEM_DONT_EXCEED           5
  #define DSMI_DUP_ITEM_FEW_MARK              550
  #define DSMI_DUP_ITEM_MANY_MARK             600
  #define DSMI_DUP_ITEM_RLC_FEW_MARK          4913   
  #define DSMI_DUP_ITEM_RLC_MANY_MARK         5740

  
  

#else /*IMAGE_QDSP6_PROC*/
  /*---------------------------------------------------------------------------
    These settings track 6280 but are smaller due to memory constraints.
  ---------------------------------------------------------------------------*/
  #ifdef FEATURE_A2_DUN_OPTIMIZED_DSM
    #ifndef DSMI_DS_SMALL_ITEM_CNT
      #define DSMI_DS_SMALL_ITEM_CNT             50
    #endif 
    #define DSMI_DS_SMALL_ITEM_FEW_MARK        6
    #define DSMI_DS_SMALL_ITEM_SIO_FEW_MARK    10
    #define DSMI_DS_SMALL_ITEM_MANY_MARK       30
    #define DSMI_DS_SMALL_ITEM_SIO_MANY_MARK   31
    #define DSMI_DS_SMALL_ITEM_RLC_FEW_MARK    20
    #define DSMI_DS_SMALL_ITEM_RLC_MANY_MARK   24
    #define DSMI_DS_SMALL_ITEM_HDR_FEW_MARK    6
    #define DSMI_DS_SMALL_ITEM_HDR_MANY_MARK   10
  #else
    #ifndef DSMI_DS_SMALL_ITEM_CNT
      #define DSMI_DS_SMALL_ITEM_CNT             1200
    #endif
    #define DSMI_DS_SMALL_ITEM_FEW_MARK        105
    #define DSMI_DS_SMALL_ITEM_SIO_FEW_MARK    110
    #define DSMI_DS_SMALL_ITEM_MANY_MARK       700
    #define DSMI_DS_SMALL_ITEM_SIO_MANY_MARK   705
    #define DSMI_DS_SMALL_ITEM_RLC_FEW_MARK    1500
    #define DSMI_DS_SMALL_ITEM_RLC_MANY_MARK   2200
    #define DSMI_DS_SMALL_ITEM_HDR_FEW_MARK    85
    #define DSMI_DS_SMALL_ITEM_HDR_MANY_MARK   110
  #endif /* FEATURE_A2_DUN_OPTIMIZED_DSM */

  #ifdef FEATURE_DSM_LARGE_ITEMS
  /*---------------------------------------------------------------------------
    Size, Count and count for different memory marks/levels for LARGE items. 
    The significants of the counts DONT_EXCEED, TRANSPORT_FEW, FEW and MANY
    is same as described above for small items.
  
    Sizing for Large Memory Pool items. Header size defines enough space for
    worst case TCP/IP/PPP header.  This should be:
    Default Max MSS + TCP Header Size + MAX TCP Options Size + IP Header Size
    + MAX IP Options Size + MAX PPP Header Size + PPP Checksum Size.

    The large item count needs to be increased if maximum-size SSL records
    are processed.
  ---------------------------------------------------------------------------*/
    #ifndef DSMI_DS_LARGE_ITEM_SIZ
      #define DSMI_DS_LARGE_ITEM_SIZ               1536
    #endif

    #ifdef FEATURE_A2_DUN_OPTIMIZED_DSM
      #ifndef DSMI_DS_LARGE_ITEM_CNT
        #define DSMI_DS_LARGE_ITEM_CNT               60
      #endif 
    #else
      #ifndef DSMI_DS_LARGE_ITEM_CNT
        #define DSMI_DS_LARGE_ITEM_CNT               356
      #endif
    #endif /* FEATURE_A2_DUN_OPTIMIZED_DSM */
    #define DSMI_DS_LARGE_ITEM_DONT_EXCEED       1  
    #define DSMI_DS_LARGE_ITEM_TRANSPORT_FEW     3
    #define DSMI_DS_LARGE_ITEM_LINK_LAYER_FEW    7
    #define DSMI_DS_LARGE_ITEM_FEW_MARK          7
    #define DSMI_DS_LARGE_ITEM_SIO_FEW_MARK      7
    #define DSMI_DS_LARGE_ITEM_SMD_FEW_MARK      10
    #define DSMI_DS_LARGE_ITEM_MANY_MARK         11
    #define DSMI_DS_LARGE_ITEM_SIO_MANY_MARK     11
    #define DSMI_DS_LARGE_ITEM_SMD_MANY_MARK     11
    #define DSMI_DS_LARGE_ITEM_LINK_LAYER_DNE    10
    #ifdef FEATURE_A2_DUN_OPTIMIZED_DSM
      #define DSMI_DS_LARGE_ITEM_RLC_FEW_MARK      10
      #define DSMI_DS_LARGE_ITEM_RLC_MANY_MARK     20
    #else
      #define DSMI_DS_LARGE_ITEM_RLC_FEW_MARK      70
      #define DSMI_DS_LARGE_ITEM_RLC_MANY_MARK     140      
    #endif /* FEATURE_A2_DUN_OPTIMIZED_DSM */
    uint32  dsmi_ds_large_item_rlc_few_mark = DSMI_DS_LARGE_ITEM_RLC_FEW_MARK;
    uint32  dsmi_ds_large_item_rlc_dne_mark = DSMI_DS_LARGE_ITEM_LINK_LAYER_DNE;
    uint32 dsmi_ds_large_item_rlc_many_mark = DSMI_DS_LARGE_ITEM_RLC_MANY_MARK;
  #endif /* FEATURE_DSM_LARGE_ITEMS */

  #ifdef FEATURE_DSM_DUP_ITEMS
  /*---------------------------------------------------------------------------
    Size, Count, Few, many and do not exceed counts for DUP items 
  ---------------------------------------------------------------------------*/
    #ifndef DSMI_DUP_ITEM_CNT
      #define DSMI_DUP_ITEM_CNT                   1600
    #endif

    #define DSMI_DUP_ITEM_DONT_EXCEED           5
    #define DSMI_DUP_ITEM_FEW_MARK              550
    #define DSMI_DUP_ITEM_MANY_MARK             600
    #define DSMI_DUP_ITEM_RLC_FEW_MARK          360   
    #define DSMI_DUP_ITEM_RLC_MANY_MARK         600   
  
  #endif /* FEATURE_DSMI_DUP_ITEMS */
 

#endif/*IMAGE_QDSP6_PROC*/

/*--------------------------------------------------------------------------
  Defining the static array that stores the small items.
--------------------------------------------------------------------------*/
#define DSMI_DS_SMALL_ITEM_ARRAY_SIZ (DSMI_DS_SMALL_ITEM_CNT * \
   (DSMI_DS_SMALL_ITEM_SIZ + DSM_ITEM_HEADER_SIZE + 31))
static uint32  dsm_ds_small_item_array[DSMI_DS_SMALL_ITEM_ARRAY_SIZ/4];
dsm_pool_mgmt_table_type dsm_ds_small_item_pool;

/*--------------------------------------------------------------------------
  Defining the static array that stores the large items.
--------------------------------------------------------------------------*/
#ifdef FEATURE_DSM_LARGE_ITEMS
#define DSMI_DS_LARGE_ITEM_ARRAY_SIZ (DSMI_DS_LARGE_ITEM_CNT * \
   (DSMI_DS_LARGE_ITEM_SIZ + DSM_ITEM_HEADER_SIZE + 31))
static uint32  dsm_ds_large_item_array[DSMI_DS_LARGE_ITEM_ARRAY_SIZ/4];
dsm_pool_mgmt_table_type dsm_ds_large_item_pool;
#endif /* #ifdef FEATURE_DSM_LARGE_ITEMS */

/*--------------------------------------------------------------------------
  Defining the static array that stores the DUP items.
--------------------------------------------------------------------------*/
#ifdef FEATURE_DSM_DUP_ITEMS

#define DSMI_DUP_ITEM_ARRAY_SIZ (DSMI_DUP_ITEM_CNT * \
   (0 + DSM_ITEM_HEADER_SIZE + 31))
static uint32 dsm_dup_item_array[DSMI_DUP_ITEM_ARRAY_SIZ/4];
dsm_pool_mgmt_table_type dsm_dup_item_pool;

#endif /* FEATURE_DSM_DUP_ITEMS */


/*---------------------------------------------------------------------------
  Flag to indicate if pools are initialized 
---------------------------------------------------------------------------*/
static boolean dsm_standard_pools_initialized = FALSE;

#if defined(IMAGE_QDSP6_PROC) && defined(FEATURE_DSM_LARGE_ITEMS)
static void dsm_rlc_calc_dsm_pool_count_triggers(void);

/*===========================================================================
FUNCTION dsm_rlc_calc_dsm_pool_count_triggers
 
 DESCRIPTION
   During the DSM Initialization, these values are calculated based on the highest supported data 
   rates by the target and configured number and size of the DSM items
 
 DEPENDENCIES
   It assumes average RTT for the traffic model. This needs to be updated based on target for the 
   highest data rate (which is lowest RTT)
===========================================================================*/

static void dsm_rlc_calc_dsm_pool_count_triggers()
{
  uint32 num_items_req_per_sec; 

  /* 
   * As per the design, following are the Pool Counts --
   *    DSM_MEM_LEVEL_LINK_LAYER_DNE = 4ms worth data
   *    DSM_MEM_LEVEL_RLC_MANY = 1 complete RTT
   *    DSM_MEM_LEVEL_RLC_FEW = 1/2 RTT + 10ms
   */
  
  num_items_req_per_sec = ((max_data_rate* 1000)>>3)/DSMI_DS_LARGE_ITEM_SIZ;
  dsmi_ds_large_item_rlc_dne_mark = 1+ ((4 * num_items_req_per_sec)/1000);
  dsmi_ds_large_item_rlc_few_mark = 2+ (((4 + rlc_rtt/2 + 10) * num_items_req_per_sec)/1000);
  dsmi_ds_large_item_rlc_many_mark =3+ (((4 + rlc_rtt/2 + 10 + rlc_rtt) * num_items_req_per_sec)/1000);

}
#endif/*IMAGE_QDSP6_PROC*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSM_INIT()

DESCRIPTION
  This function will initialize the Data Service Memory Pool. It should be
  called once upon system startup. All the memory items are initialized and
  put onto their respective free queues.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsm_init(void)
{
  /* Initialize the pools only if not done previously */
 if (dsm_standard_pools_initialized == FALSE)
 {
    (void)dsm_init_pool(DSM_DS_SMALL_ITEM_POOL, 
                        (uint8*)dsm_ds_small_item_array,
                        DSMI_DS_SMALL_ITEM_ARRAY_SIZ,
                        DSMI_DS_SMALL_ITEM_SIZ);


    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_DNE,
                            DSMI_DS_SMALL_ITEM_DONT_EXCEED);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_LINK_LAYER_DNE,
                            DSMI_DS_SMALL_ITEM_LINK_LAYER_DNE);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_TRANSPORT_LAYER_FEW,
                            DSMI_DS_SMALL_ITEM_TRANSPORT_LAYER_FEW);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_LINK_LAYER_FEW,
                            DSMI_DS_SMALL_ITEM_LINK_LAYER_FEW);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,                          
                            DSM_MEM_LEVEL_HDR_FEW,
                            DSMI_DS_SMALL_ITEM_HDR_FEW_MARK);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_RLC_FEW,
                            DSMI_DS_SMALL_ITEM_RLC_FEW_MARK);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_FEW,
                            DSMI_DS_SMALL_ITEM_FEW_MARK);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_SIO_FEW,
                            DSMI_DS_SMALL_ITEM_SIO_FEW_MARK);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_HDR_MANY,
                            DSMI_DS_SMALL_ITEM_HDR_MANY_MARK);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_RLC_MANY,
                            DSMI_DS_SMALL_ITEM_RLC_MANY_MARK);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_MANY,
                            DSMI_DS_SMALL_ITEM_MANY_MARK);
    dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                            DSM_MEM_LEVEL_SIO_MANY,
                            DSMI_DS_SMALL_ITEM_SIO_MANY_MARK);


    #ifdef FEATURE_DSM_LARGE_ITEMS
    (void)dsm_init_pool(DSM_DS_LARGE_ITEM_POOL, 
                        (uint8*)dsm_ds_large_item_array,
                        DSMI_DS_LARGE_ITEM_ARRAY_SIZ,
                        DSMI_DS_LARGE_ITEM_SIZ);

    #ifdef IMAGE_QDSP6_PROC
    /*Compute the RLC thresholds*/
    dsm_rlc_calc_dsm_pool_count_triggers();
    #endif/*IMAGE_QDSP6_PROC*/

    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_DNE,
                            DSMI_DS_LARGE_ITEM_DONT_EXCEED);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_LINK_LAYER_DNE,
                            dsmi_ds_large_item_rlc_dne_mark);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_TRANSPORT_LAYER_FEW,
                            DSMI_DS_LARGE_ITEM_TRANSPORT_FEW);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_LINK_LAYER_FEW,
                            DSMI_DS_LARGE_ITEM_LINK_LAYER_FEW);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_FEW,
                            DSMI_DS_LARGE_ITEM_FEW_MARK);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_SIO_FEW,
                            DSMI_DS_LARGE_ITEM_SIO_FEW_MARK);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_SMD_FEW,
                            DSMI_DS_LARGE_ITEM_SMD_FEW_MARK);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_MANY,
                            DSMI_DS_LARGE_ITEM_MANY_MARK);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_SIO_MANY,
                            DSMI_DS_LARGE_ITEM_SIO_MANY_MARK);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_SMD_MANY,
                            DSMI_DS_LARGE_ITEM_SMD_MANY_MARK);
    /* Dsm event definition for RLC large item based flow control*/
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_RLC_FEW,
                            dsmi_ds_large_item_rlc_few_mark);
    dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
                            DSM_MEM_LEVEL_RLC_MANY,
                            dsmi_ds_large_item_rlc_many_mark);
 
    #endif /* FEATURE_DSM_LARGE_ITEMS */

    #ifdef FEATURE_DSM_DUP_ITEMS
    (void)dsm_init_pool(DSM_DUP_ITEM_POOL,
                        (uint8*)dsm_dup_item_array,
                        DSMI_DUP_ITEM_ARRAY_SIZ,
                        0);

    dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                            DSM_MEM_LEVEL_FEW,
                            DSMI_DUP_ITEM_FEW_MARK);


    dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                            DSM_MEM_LEVEL_MANY,
                            DSMI_DUP_ITEM_MANY_MARK);

    dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                            DSM_MEM_LEVEL_RLC_FEW,
                            DSMI_DUP_ITEM_RLC_FEW_MARK);


    dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                            DSM_MEM_LEVEL_RLC_MANY,
                            DSMI_DUP_ITEM_RLC_MANY_MARK);

    #endif /* FEATURE_DSM_DUP_ITEMS */

    /* Set the flag to TRUE to avoid re-initialization */
    dsm_standard_pools_initialized = TRUE;
  }
  else
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsm_init being attempted again when pools are already inited");
  }

} /* dsm_init() */

