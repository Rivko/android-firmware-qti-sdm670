#ifndef DIAG_V_H
#define DIAG_V_H
/*==========================================================================

                     Diagnostic Task Header File

Description
  Global Data declarations of the diag_task.

Copyright (c) 1991-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/common/src/diag_v.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implemented seperate queue for UserPD events and enabled buffering
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
04/09/17   sp      Added support for extended logs 
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
05/12/16   ph      SMD references removal and code clean up.
06/18/15   sa      Added support for time sync feature.
07/06/14   ph      Added support for Diag over STM
07/16/14   xy      Update Diag delayed response command to take parameter  
04/10/14   sr      Support for peripheral buffering mode   
09/27/13   xy      Removed code under FEATURE_WINCE  
05/09/13   sr      Added support for diag mask change notification for 
                   registered users  
04/22/13   ph       Moved listeners allocation to system heap.
02/11/13   sr      Mainling Diag central routing  
01/03/13   sg      Command Req/Response Channel separation 
09/26/12   is      Support for preset masks
07/05/12   sg      Changes to bring up diag Multi PD
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap. 
03/10/12   sg      Fixed Klock work warning 
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
04/18/11   is      Update feature flag for SMD-Lite
03/04/11   is      Support for Diag over SMD-Lite
02/04/11   vs      Removed check for MASTER to not have RSP_SEND
11/26/10   vs      if a9 on 9k - have proc ID be modem_proc
11/22/10   hm      diag_v.h clean up. Moved the defines to respective custdiag.h files  
10/25/10   is      Diag feature flag cleanup
07/23/10   mad     Added prototype for diagpkt_check_validity_request_ondevice() 
07/12/10   mad     Changes for apps-only diag bring-up 
06/30/10   sg      diag_client_maskchange() is made static 
04/20/10   sg      Moved diag_client_maskchange() here form diag_shared_i.h
06/07/10   is      Replace external IMAGE_* feature flags with internal Diag 
                   versions
04/21/10   JV      Added prototypes for functions that manage the new heap for
                   events.
12/14/09   sg      Added diag_rx_rsp_notify()
07/14/09   vs      Added diag_set_drain_timer
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Added new internal feature to single out the Q6 on 8x50
                   DIAG_QDSP6_APPS_PROC
07/31/09   JV      Merged Q6 diag code back to mainline
07/28/09   as      Removed public API's from internal file.
07/17/09   mad     Featurized includion of customer.h for WM.
07/10/09   as      Decoupled feature FEATURE_DIAG_NON_STREAMING
05/07/09   vk      moved declerations into diag.h that were being referred from there
01/15/09   as      Fixed corner case in proc ID
12/12/08   vg      split public and private information diag.h and diag_v.h
10/03/08   vg      Update configuration to enable msm 8200, 2 modem processors
12/22/06   as      Moved proc ID macros to diag.h
12/05/06   as      Added signal for Diag drain timer
11/21/06   as      Moved DIAG internal features from diagi.h to diag.h
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
10/17/03   ph      For Rel A builds, redirect to new MC subsystem command.
09/23/03    gr     Function prototypes related to changes that enable more
                   efficient detection of the condition where diag is out of
                   heap space.
08/20/01   jal     Changes to support more Diag packets.  Support for powerup
                   online/offline state, service programming lock state.
04/06/01   lad     Cosmetic changes.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  This file now
                   contains an API.  No other information or coupling remains
                   except for featurized legacy code that impacts existing 
                   targets.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "comdef.h"

#include "customer.h"    


#include "qw.h"
#include "diag.h"
#include "diag_shared_i.h"


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
/* Diagnostics version (protocol revision).
*/
/* DIAG_DIAGVER                                                            
   Diagnostics version, used to ensure compatibility of the DM and the DMSS.
   1   Original
   2   Changes to status packet and verno packet. Removed verstr packet
   3   for release to factory. Sends RF mode in status packet
   4   Adds release directory to verno packet and dipswitch packets
   5   Many changes in DM.
   6   Added Vocoder PCM and PKT loopback, and Analog IS-55 tests
   7   New FER data in tagraph packet
   8   Streaming protocol enhancements and event reporting.
*/

#define DIAG_DIAGVER 8

/* -------------------------------------------------------------------------
** Diag Internal features
** ------------------------------------------------------------------------- */

/* Runtime Device Map port selection for diag */
#if defined (FEATURE_RUNTIME_DEVMAP) && !defined(DIAG_APPS_ONLY_BRINGUP)
#if defined (DIAG_MP)
  #if defined (DIAG_MP_MASTER)|| defined(FEATURE_DIAG_MODEM_ONLY)

    #define DIAG_RUNTIME_DEVMAP

  #endif
#else
   #define DIAG_RUNTIME_DEVMAP
#endif
#endif

#if defined(FEATURE_DATA_SERIALIZER) && defined(DIAG_RUNTIME_DEVMAP)
  #define FEATURE_DIAG_ON_DATA_PORT
#endif /* FEATURE_DATA_SERIALIZER */

/* Error cases */

#if defined (DIAG_MP) && !defined(FEATURE_SMD) && !defined(FEATURE_SMD_LITE)
  #error "Error: FEATURE_SMD or FEATURE_SMD_LITE is required for DIAG_MP" 
#endif 

#if defined (FEATURE_DIAG_MODEM_ONLY) &&  \
    !(defined (DIAG_MP) || defined (DIAG_IMAGE_MODEM_PROC))
  #error "Error: Invalid configuration for DIAG MP"
#endif 


#if !defined (DIAG_MP_MASTER)
 #if defined (DIAG_REQ_FWD) 
   #error "Error: DIAG_REQ_FWD defined on Slave"
 #endif
 #if defined(DIAG_MP) && defined (DIAG_FRAMING)
   #error "Error: DIAG_FRAMING defined on Slave"
 #endif
#endif

#ifdef DIAG_UIMAGE_MODE
#define UIMAGE_DIAGBUFFER_SIZE 16384
#endif

#define UIMAGE_DIAGBUFFER_SIZE_FINAL MICRO_ULOG_MEM_NEEDED(UIMAGE_DIAGBUFFER_SIZE)

#define DIAG_FTM_SWITCH_VAL 2

/* -------------------------------------------------------------------------
** Function Definitions
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif




/*These define the bit postion in diag_mask_notify 
  for mask change notification*/
#define DIAG_EVENT_MASK_NOTIFY_BIT 1
#define DIAG_LOG_MASK_NOTIFY_BIT   2
#define DIAG_MSG_MASK_NOTIFY_BIT   4

/* Maximum length userPD name can have */
#define DIAG_MAX_PD_NAME_SIZE   80

/* Enumerates the list of all available userPD's */
typedef enum
{
  DIAG_PD_NONE,
  DIAG_PD_MIN,
  DIAG_PD_AUDIO = DIAG_PD_MIN,
  DIAG_PD_WLAN,
  DIAG_PD_SENSOR,
  DIAG_PD_ROOT,
  DIAG_PD_MAX = DIAG_PD_ROOT,
} diag_pd_type;

typedef struct {
  char pd[DIAG_MAX_PD_NAME_SIZE];
  diag_pd_type pd_type;
}diag_pd_tbl;

/* This table needs to be updated for all userPD's supported */
static const diag_pd_tbl diag_pd_table[] = 
{
  {"audio_pd", DIAG_PD_AUDIO},
  {"wlan_pd", DIAG_PD_WLAN},
  {"sensor_pd", DIAG_PD_SENSOR},
  {"root_pd", DIAG_PD_ROOT}
};


/*Defines the maximum version number we support currently for diagbuf 
  pkts*/
typedef enum
{
    DIAGBUF_VERSION_0  = 0,
    DIAGBUF_VERSION_1 = 1,
    DIAGBUF_VERSION_2 = 2,
    DIAGBUF_MAX_VERSION_NUM = DIAGBUF_VERSION_2
}diagbuf_pkt_version; 

/* ==================================================================
FUNCTION DIAG_MALLOC

DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
  void *diag_malloc (unsigned long num_bytes);

/* ==================================================================
FUNCTION DIAG_EVENT_MALLOC_SEC

DESCRIPTION
   Dynamically allocates a specified number of bytes from diag secondary
   event heap and returns a   pointer to this chunk of memory.

===================================================================== */
  void *diag_event_malloc_sec (unsigned long num_bytes);

  int diag_event_alloc_check_sec (unsigned int num_allocs, uint32 *alloc_sizes);

/* ==================================================================
FUNCTION DIAG_EVENT_FREE_SEC

DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc from diag_event_sec_heap.
===================================================================== */
  void diag_event_free_sec (void *mem_ptr);

  



/*===========================================================================
FUNCTION DIAG_SUSPEND_TASK

DESCRIPTION
  This procedure suspends the DIAG task for 'duration' milliseconds.  This
  routine has no impact on the flow of diagnostic data other than 
  suspending the DIAG task.
   
===========================================================================*/
  void diag_suspend (unsigned int duration);

/* ==================================================================
FUNCTION DIAG_BLOCK

DESCRIPTION
   Blocks the DIAG task until diag_release() is called.  The DIAG 
   task will be suspended completely until diag_release() is called.
===================================================================== */
  void diag_block (void);

/* ==================================================================
FUNCTION DIAG_LISTENERS_FREE

DESCRIPTION
   This function frees the listener 
===================================================================== */
  void diag_listeners_free(void *mem_ptr);

/* ==================================================================
FUNCTION DIAG_LISTENERS_MALLOC

DESCRIPTION
   This function allocates a listener
===================================================================== */
  void * diag_listeners_malloc (unsigned long num_bytes);


/* ==================================================================
FUNCTION DIAG_RELEASE

DESCRIPTION
   Blocks the DIAG task until diag_release() is called.  The DIAG 
   task will be suspended completely until diag_release() is called.
===================================================================== */
  void diag_release (void);

/*===========================================================================
FUNCTION DIAG_TX_NOTIFY

DESCRIPTION
  This routine notifies DIAG that data is ready to be sent without causing
  a context switch immediately.
  
  This eliminates concern that INTLOCK's may be violated from a context 
  switch.
  
RETURN VALUE
  None.

===========================================================================*/
  void diag_tx_notify (void);

/*===========================================================================
FUNCTION DIAG_RX_NOTIFY

DESCRIPTION
  This callback routine is called when data has been received and is ready
  be read by the diag task.
  
===========================================================================*/
  void diag_rx_notify (void);

/*===========================================================================
FUNCTION DIAG_RX_RSP_NOTIFY

DESCRIPTION
  This function notifies DIAG that RX response data is ready to be sent .

===========================================================================*/
  void diag_rx_rsp_notify(void);

/*===========================================================================
FUNCTION DIAG_RX_DELAY_RSP_NOTIFY

DESCRIPTION
  This routine sets the DIAG_RX_DELAY_RSP_SIG 
  
===========================================================================*/
void diag_rx_delay_rsp_notify( void );

/*===========================================================================
FUNCTION DIAG_SET_DRAIN_TIMER

DESCRIPTION
  This function sets the drain timer.

===========================================================================*/
  void diag_set_drain_timer(void);

/*===========================================================================
FUNCTION DIAG_CLIENT_MASKCHANGE

DESCRIPTION
  Notifies all diag clients (Diag_LSMs) of mask change.
============================================================================*/
boolean diag_client_maskchange(int Svc);

//ToDo: Move to diagpkti_v.h
boolean diagpkt_check_validity_request_ondevice (void * buf);

void diagpkt_fwd_cmd_reg( void );


/*===========================================================================
FUNCTION DIAG_SET_TX_MODE_STREAM (internal)

DESCRIPTION
  Sets the Tx mode. 
  
PARAMETERS
  mode - DIAG_TX_MODE_STREAMING         0
       - DIAG_TX_MODE_BUFFERED_THRESH   1
       - DIAG_TX_MODE_BUFFERED_CIR      2
	   
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  diag_id   - diag_id of the entity (Peripheral/PD).
  
RETURN VALUE
  The current Tx mode.
  
===========================================================================*/ 
  uint8 diag_set_tx_mode_stream( uint8 mode, uint8 stream_id, uint8 diag_id );
  
  
/*===========================================================================
FUNCTION DIAG_GET_TX_MODE_STREAM (internal)

DESCRIPTION
  Gets the current Tx mode. 
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  diag_id   - diag_id of the entity (Peripheral/PD).
  
RETURN VALUE
  The current Tx mode.
  
===========================================================================*/  
  uint8 diag_get_tx_mode_stream( uint8 stream_id, uint8 diag_id );
  
  
/*===========================================================================
FUNCTION DIAG_SET_TX_PARAM_WM_STREAM (internal)

DESCRIPTION
  Sets the Tx high and low watermark parameter. In buffered threshold mode, 
  when the number of buffered bytes has reached the high wm level, we start
  to drain until we reach the low wm level.
  
  High wm level cannot be less then low wm level. High wm level should
  also not be greater than the level when we enable flow control to 
  stop the traffic from the peripherals.
  
  In buffered threshold and buffered circular modes, we stop draining when 
  the number of bytes in the buffered watermark queue has reached the low
  watermark level.
  
PARAMETERS
  high_per_value - High watermark value (as a percentage of total buffering pool size).
  low_per_value - Low watermark value (as a percentage of total buffering pool size).
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
   diag_id   - diag_id of the entity (Peripheral/PD).
			  
RETURN VALUE
  TRUE - The high/low wm values were updated successfully.
  FALSE - The high/low watermark values were NOT updated successfully.
  
===========================================================================*/  
  boolean diag_set_tx_param_wm_stream( uint8 high_per_value, uint8 low_per_value, 
                                            uint8 stream_id, uint8 diag_id );
  
/*===========================================================================
FUNCTION DIAG_HEALTH_RESET_DROPPED_CNT_STREAM (internal)

DESCRIPTION
  Resets buffering mode health counter statistics per stream id.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
				  
RETURN VALUE
  None
  
===========================================================================*/
  void diag_health_reset_dropped_cnt_stream( uint8 stream_id );
  
/*===========================================================================
FUNCTION DIAG_DRAIN_TX_BUFFER_STREAM (internal)

DESCRIPTION
  Drains the Tx buffer immediately. If in buffered mode, it will drain until
  Tx low watermark threshold is reached.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  diag_id   - diag_id of the entity (Peripheral/PD).
  
RETURN VALUE
  None
  
===========================================================================*/  
  void diag_drain_tx_buffer_stream( uint8 stream_id, uint8 diag_id );

/*===========================================================================
FUNCTION   diag_get_pd_name

DESCRIPTION
    This function retrieves the PD type for a given process name obtained by
    service registry. On Successful call, current userPD [AUDIO/WLAN/SENSOR]
    is returned else DIAG_PD_NONE is returned.

DEPENDENCIES
    TMS API call should give the correct domain information "mdm/modem/audio_pd"

RETURN VALUE
    NONE

SIDE EFFECTS
    NONE
===========================================================================*/
diag_pd_type diag_get_pd_name(char* name );

#ifdef __cplusplus
}
#endif
#endif /*DIAG_V_H*/
