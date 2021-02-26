
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diag Interface

GENERAL DESCRIPTION
   Contains main implementation of Diagnostic Log Services.

EXTERNALIZED FUNCTIONS
   log_set_code
   log_set_length
   log_set_timestamp
   log_submit
   log_free
   log_get_length
   log_get_code
   log_status
   log_alloc
   log_commit
   log_shorten

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2007-2012, 2014,2017 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

                              Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/Diag_LSM_Log.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/29/17   sa      Send IOCTL for mask update after diagID is set.
06/14/17   sp      Add support for microULog debug capability
06/08/17   gn      Added support for diag services to uImage
04/24/17   sp      Added support for extended logs in userPD
04/09/17   sp      Added support for extended logs
27/01/17   nk      Fixed issue with log_submit api
11/14/14   jtm/vk  Multi-PD Optimizations 
07/06/14   ph      Added support for Diag over STM
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
07/05/12   sg      Changes to bringup Diag MultiPD
10/19/10   mad     Fixed log_shorten
04/09/10   sg      Merging DCM rtos and wm files
09/23/09   sg      Mainlined when MSM5000_IRAM_FWD is not defined
07/07/09   mad     Removed unwanted headers: queue.h,diagpkti.h, diagi.h,
                   diagpkt.h,assert.h,qw.h,log_codes.h,diagtune.h,string.h
01/27/09   mad     modified log_submit() to allocate memory only if log mask is
                   enabled.
09/30/08   mad     Changes to take out Component services, and use
                   filesystem calls to access windiag driver.
09/04/08   mad     Fixed bug in log_free(): to free the correct pointer
05/01/08   JV      Added support to update the copy of log_mask in this process
                   during initialization and also on mask change
02/11/08   JV      Changed the comparison of IDiagPkt_Send()'s return value from
                   AEE_FAILURE to AEE_SUCCESS
01/16/08   JV      Created stubs for log_on_demand_register and log_on_demand_unregister
                   as we do not have function pointer support for Diag 1.5A
11/20/07   JV      Created

===========================================================================*/


/* ==========================================================================
   Include Files
========================================================================== */
#include "diagdiag_v.h"           /* For diag_log_rsp_type */
#include "log.h"
#include "msg.h"                  /* For MSG_LOW */
#include "diagstub.h"             /* For ts_get */
#include "Diag_LSM_Log_i.h"       /* For LSM log init and deinit */
#include "Diag_LSMi.h"            /* For declaration of windiag driver HANDLE etc */
#include "diagbuf_mpd.h"          /* For mpd functions */
#include "diaglogi.h"             /* For Diag log_header_type and functions */
#include "diag_shared_i.h"        /* For definition diag datatypes. */
#include "log_arrays_i.h"         /* For log_last_item_tbl */
#include "Diag_LSM_Qdi_wrapper.h" /* For LSM QDI wrapper functions */
#include "Diag_LSM_stm.h"         /* For diag stm macros and functions */
#include "diag_diagIDi.h"
#include "diagi_v.h"              /* For diagbuf_header_type */

#include <stringl/stringl.h>      /* For memscpy */

/* The content of this table are computed in diag_lsm_log_init().  'uint16' is used
 * instead of 'int' to reduce data bus cycles to access an element of this
 * array.  The last entry is the offset to the byte following the array.  It
 * is used for range checking. */
extern uint16 log_mask_offset_tbl[LOG_EQUIP_ID_LAST+1];
extern byte log_composite_mask[LOG_MASK_SIZE];

/* 'log_mask' is the mask of bits used to represent the configuration of all
* log codes. '1' denotes the code being enabled, '0' denotes 
* disabled. 
*/

static byte log_mask[LOG_MASK_SIZE];
extern log_state_enum_type log_state;

void *log_commit_last = NULL; /* Many writers, 1 reader (DIAG) */

/* External Function Implementations */
/*===========================================================================

FUNCTION LOG_ALLOC_CORE

DESCRIPTION
  This function allocates a buffer of size 'length' for logging data.  The
  specified length is the length of the entire log, including the log
  header.  This operation is inteneded only for logs that do not require
  data accumulation.

  !!! The header is filled in automatically by this routine.

PARAMETERS
  log_code_type code   - log code
  unsigned int length  - size of log to alloc

DEPENDENCIES:
   Diag log service must be initialized.
   log_commit() or log_free() must be called ASAP after this call.

RETURN VALUE
  A pointer to the allocated buffer is returned on success.
  If the log code is disabled or there is not enough space, NULL is returned.

SIDE EFFECTS
  Since this allocation is made from a shared resource pool, log_commit()
  or log_free() must be called as soon as possible and in a timely fashion.

  If you need to log accumulated data, store the accumulated data in your
  own memory space and use log_submit() to log the data.
===========================================================================*/
PACKED void* log_alloc_core(
   log_code_type code,
   unsigned int length
   )
{
   diag_log_rsp_type *plog_pkt_ptr; /* Pointer to packet being created */
   log_header_type *phdr_ptr = NULL;
   void *return_ptr = NULL;
#ifdef FEATURE_DIAG_STM
   boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */
#endif

   if (diag_qdi_handle < DIAG_FD_MIN_VALUE)
   {
      return (NULL);
   }
   if (length <= sizeof(log_header_type))
   {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Alloc invalid length %d", length);
   }
   else if (log_status(code))
   {
#ifdef FEATURE_DIAG_STM
      /* Check is STM enabled */
      if (stm_enabled)
      {
         plog_pkt_ptr = (diag_log_rsp_type *)diag_stm_log_alloc(length + LOG_DIAGPKT_OFFSET);
      }
      else
      /* For any other case, we allocate from diagbuf as the packet is needed by something other than just STM */
#endif
      {
         plog_pkt_ptr = (diag_log_rsp_type *)diagbuf_mpd_alloc(0, FPOS(diag_log_rsp_type, log) + length, 0, DIAGBUF_ALLOC_REQUEST_LOG, 0, DIAG_EXT_API_VERSION_0);
      }

      if (plog_pkt_ptr != NULL)
      {
         /* Fill in top of packet. i.e. diag_log_rsp_type*/
         plog_pkt_ptr->cmd_code = DIAG_LOG_F;
         plog_pkt_ptr->more = 0;
         plog_pkt_ptr->len = (uint16)length;

         /* Fill in log header*/
         phdr_ptr = (log_header_type *)&(plog_pkt_ptr->log);
         phdr_ptr->len = (uint16)length;
         phdr_ptr->code = code;
         time_get(phdr_ptr->ts);

         return_ptr = (void *)&(plog_pkt_ptr->log);
      }
      else
      {
#ifdef VINDBG
         printf("\n dropped log 0X%x", code);
#endif
         /* Dropped a log. */
         MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Dropped log 0x%x", code);
      }

   } /* if valid and enabled */

   return (return_ptr);

} /* log_alloc */

/*===========================================================================

FUNCTION LOG_ALLOC_EXT_CORE

DESCRIPTION
  Allocate a buffer of size 'length' for logging data. Supports version field

PARAMETERS
  code                 ID of the log to be reported
  length               The specified length is the length of the entire log,
                             	including the log header.
  version              Specifies the version - currently, there are two versions
                          0 - same as log_alloc() (for backwards compatibility)
                          1 - The outgoing log packet header will have support to
                              include ID field to differentiate the log pkts
                              coming from different instances of the processor.
 *parameter           Based on the version passed in, this pointer is interpreted
                      as follows-
                        version       pointer
                         0             NULL
                         1             uint32 * . The uint32 value this pointer points to,
                                       is the ID of the instance generating the
                                       log packet
=========================================================================*/

void *
log_alloc_ext_core(log_code_type code, unsigned int length, unsigned int version, void * parameter)
{

   /* Variable Init */
   diag_log_rsp_type * log_pkt_ptr = NULL;
   log_header_type * hdr_ptr = NULL;
   void * log_entry_ptr = NULL;

   /* QDI Handle sanity check */
   if (diag_qdi_handle < DIAG_FD_MIN_VALUE)
      return NULL;

   /* Sanity checks */
   if (((version > DIAG_EXT_API_VERSION_0) && (parameter == NULL)) || (version > DIAG_EXT_API_VERSION_MAX))
  {
      return FALSE;
  }

   /* Length check */
   if (length <= sizeof(log_header_type))
  {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Alloc invalid length %d", length);
  }
   else if ((length + LOG_DIAGPKT_OFFSET) > DIAG_MAX_TX_PKT_SIZ)
  {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempt to allocate %d but Max Length of a log packet supported is %d", (length + LOG_DIAGPKT_OFFSET), DIAG_MAX_TX_PKT_SIZ);
   }
   else if (log_status(code))
   {
      /* Allocate enough for the entire log response packet, not just the log entry */
      log_pkt_ptr = (diag_log_rsp_type *) diagbuf_mpd_alloc_chain(DIAG_LOG_F, FPOS(diag_log_rsp_type, log) + length, 0, DIAGBUF_ALLOC_REQUEST_LOG, 1, version, parameter);

      if (log_pkt_ptr != NULL)
      {
         /* Fill in the log response (record) packet */
         diagbuf_mpd_write_stream_id (log_pkt_ptr, DIAG_STREAM_1);
         log_pkt_ptr->more = 0;
         log_pkt_ptr->cmd_code = DIAG_LOG_F;
         log_pkt_ptr->len = (uint16) length;

         /* Fill in the log header */
         hdr_ptr = (log_header_type *) &(log_pkt_ptr->log);
         hdr_ptr->len = (uint16) length;
         hdr_ptr->code = code;

         /* Fill in the timestamp */
         log_set_timestamp(hdr_ptr);

         /* The header pointer is the same as the log entry pointer */
         log_entry_ptr = (void *) hdr_ptr;
      }
      else
      {
        /* Dropped a log */
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Dropped log 0x%x", code);
      }
   }

   return log_entry_ptr;

} /* end log_alloc_ext */


/*===========================================================================

FUNCTION LOG_SHORTEN_CORE

DESCRIPTION
  This function shortens the length of a previously allocated logging buffer in
  legacy code. This is used when the size of the record is not known at allocation
  time.Now that diagbuf is not used in the LSM layer and we just use memory from
  a pre-allocated pool, calling log_shorten, does not free the excess memory, it just
  updates the length field.

PARAMETERS
   void * ptr  - pointer to the log pkt
  unsigned int length - length of the log pkt

DEPENDENCIES
   Diag log service must be initialized.
   This must be called prior to log_commit().

NOTE
   log_shorten_ext is defined in 'log.h' as a macro wrapper for log_shorten

RETURN VALUE
   None

===========================================================================*/
void log_shorten_core(
   PACKED void *ptr,
   unsigned int length
   )
{
   byte *pdiag_pkt = (byte *)ptr; /* use byte* for pointer arithmetic */
   diag_log_rsp_type *pdiag_log;

   if (ptr)
   {
      pdiag_pkt -= LOG_DIAGPKT_OFFSET;
      pdiag_log = (diag_log_rsp_type *)pdiag_pkt;

      if (length < pdiag_log->len)
      {
         diagbuf_header_type *pkt_hdr;
         pkt_hdr = ((diagbuf_header_type *)pdiag_pkt - 1);
         if (pkt_hdr->status != DIAGBUF_STM_BUFFER_S)
         {
            // Do not call diagbuf_mpd_shorten if allocated from STM heap
            diagbuf_mpd_shorten((void *)pdiag_log, length + LOG_DIAGPKT_OFFSET);
         }
         else
         {
           /* Ensure STM pkt headers are updated correctly with the new length */
           /* Should work for all Diag STM log allocations (including ext packets)
           so long as they contain a single diagbuf_header_type reference */
           pkt_hdr->length = length + LOG_DIAGPKT_OFFSET;
         }

         /* Set the log packet length to the new length */
         pdiag_log->len = (uint16)length;

         /* log_set_length takes the log itself, not the log packet */
         log_set_length(ptr, length);
      }
   }
} /* log_shorten */

/*===========================================================================

FUNCTION LOG_COMMIT_CORE

DESCRIPTION
  This function commits a log buffer allocated by log_alloc().  Calling this
  function tells the logging service that the user is finished with the
  allocated buffer.

PARAMETERS
   void * ptr  - pointer to the log pkt to commit.

DEPENDENCIES
   Diag log service must be initialized.
   This must be called using a pointer returned from to log_alloc().

SIDE EFFECTS
  Since this allocation is made from a shared resource pool, this must be
  called as soon as possible after a log_alloc call.  This operation is not
  intended for logs that take considerable amounts of time ( > 0.01 sec ).

NOTE
  log_commit_ext is defined in 'log.h' as a macro wrapper for log_commit

RETURN VALUE
  None

===========================================================================*/
void log_commit_core(PACKED void *ptr)
{

   /* Variable Init */
   diagbuf_header_type * pkt_hdr = NULL;
   diag_log_rsp_type * log_record_ptr = NULL;
   void *ptr_first = NULL; // Pointer to the first packet incase of chaining

   if (ptr)
   {
      /* Set last-commited log pointer to current log entry */
      log_commit_last = ptr;

      /* Access the diagbuf header for the current log entry */
      log_record_ptr = (diag_log_rsp_type *) ((byte *)ptr - LOG_DIAGPKT_OFFSET);
      pkt_hdr = (diagbuf_header_type *) ((byte *)log_record_ptr - sizeof(diagbuf_header_type));

      if (pkt_hdr == NULL)
      {
         return;
      }

      /* Version-specific operations */
      switch (pkt_hdr->version)
      {
         case DIAGBUF_VERSION_1:
         case DIAGBUF_VERSION_2:
         {
            if (pkt_hdr->chain == DIAGBUF_SECOND_PKT_CHAIN)
            {
               /*
                  Commit the second packet first in case diagbuf_drain() is
                  called in between the two commits in this function; this
                  is to prevent the scenario where the first packet is
                  committed and the second one is not.
               */
               diagbuf_mpd_commit(log_record_ptr);

               /*
                  Use pointer arithmetic to move from the second packet to the first.
                  Commit the first packet (NOTE: First packet is not a response type,
                  it is a diag_cmd_ext_vN_type + DEAD + pad).
               */
               ptr_first = (void *)(pkt_hdr - diagbuf_mpd_ver_len[pkt_hdr->version-1]);
               diagbuf_mpd_commit(ptr_first);
            }
            break;
         }

         case DIAGBUF_VERSION_0:
         {
#ifdef FEATURE_DIAG_STM
            boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

            if (stm_enabled)
            {
              log_header_type *phdr_ptr = NULL;

              /* Set pointer to beginning of diag pkt, not the log */
              phdr_ptr = (log_header_type *)ptr;
              diag_stm_write_pkt(log_record_ptr, LOG_DIAGPKT_OFFSET + phdr_ptr->len);
              if (pkt_hdr->status == DIAGBUF_STM_BUFFER_S)
              {
                diag_stm_log_free((void *)log_record_ptr);
              }
            }
            else
#endif /* FEATURE_DIAG_STM */
            {
              diagbuf_mpd_commit(log_record_ptr);
              break;
            }
         }
         default:
         {
            /* Don't do anything for unsupported versions */
            break;
         }
      }
   }
   
   return;

} /* end log_commit */

/*===========================================================================

FUNCTION LOG_FREE_CORE

DESCRIPTION
  This function frees the buffer in pre-allocated memory.

DEPENDENCIES:
  Diag log service must be initialized.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void log_free_core(PACKED void *ptr)
{
   if (ptr)
   {
      byte *pkt_ptr = (byte *)ptr;
#ifdef FEATURE_DIAG_STM
      diagbuf_header_type *pkt_hdr;    /* Store the header in local variable     */
#endif

      pkt_ptr -= LOG_DIAGPKT_OFFSET;
#ifdef FEATURE_DIAG_STM

      //Check status to see if this is an STM only buffer. If so, use STM free function instead
      pkt_hdr = ((diagbuf_header_type *)pkt_ptr - 1);

      if (pkt_hdr->status == DIAGBUF_STM_BUFFER_S)
      {
         diag_stm_log_free(pkt_ptr);
      }
      else
#endif
      {
         diagbuf_mpd_shorten(pkt_ptr, 0);
      }
   } 
} /* log_free */

/*===========================================================================

FUNCTION LOG_SUBMIT_CORE

DESCRIPTION
  This function is called to log an accumlated log entry. If logging is
  enabled for the entry by the external device, then this function essentially
  does the folliwng:
  log = log_alloc ();
  memscpy (log, length, ptr, log->len);
  log_commit (log);

PARAMETERS
  void *ptr - pointer to log pkt

DEPENDENCIES
   Diag log service must be initialized.

RETURN VALUE
  Boolean indicating success.

SIDE EFFECTS
  None.
===========================================================================*/
boolean log_submit_core( PACKED void *ptr )
{
   boolean bReturnVal = FALSE;
   diag_log_rsp_type *plog_pkt_ptr = NULL;

#ifdef FEATURE_DIAG_STM
   boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */
#endif

   /* The header is common to all logs, and is always at the beginning of the
          * packet. */
   log_header_type *plog_ptr = (log_header_type *)ptr;
   if (plog_ptr && (diag_qdi_handle >= DIAG_FD_MIN_VALUE))
   {
      /* Local vars to avoid use of misaligned variables */
      log_code_type code = plog_ptr->code;
      unsigned int length = plog_ptr->len;
      if ((length > sizeof(log_header_type)) && log_status(code))
      {
#ifdef FEATURE_DIAG_STM
         /* Check is STM enabled and that DIAG_STREAM_STM is the *only* stream */
         if (stm_enabled)
         {
            /* In this use case, we detect if *only* STM is enabled (i.e. no listeners) and then allocate
            as an STM only packet.*/
            plog_pkt_ptr = (diag_log_rsp_type *)diag_stm_log_alloc(length + LOG_DIAGPKT_OFFSET);
         }
         else
         /* For any other case, we allocate from diagbuf as the packet is needed by something other than just STM */
#endif
         {
            plog_pkt_ptr = (diag_log_rsp_type *)diagbuf_mpd_alloc(0, FPOS(diag_log_rsp_type, log) + length, 0, DIAGBUF_ALLOC_REQUEST_LOG, 0, DIAG_EXT_API_VERSION_0);
         }
         if (plog_pkt_ptr != NULL)
         {
            plog_pkt_ptr->cmd_code = DIAG_LOG_F;
            plog_pkt_ptr->more = 0;
            plog_pkt_ptr->len = (uint16)length;
            memscpy(&plog_pkt_ptr->log, length, (void *)ptr, length);

            log_commit(&plog_pkt_ptr->log); //log_commit() takes care of STM as well.
            bReturnVal = TRUE;
         } /* if (plog_pkt_ptr != NULL) */
         else
         {
            /* Dropped a log */
            MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Dropped log 0x%x", code);
         }
      } /* if ((length > sizeof(log_header_type)) && log_status (code)) */
   } /* if (plog_ptr && ghWinDiag) */
   return (bReturnVal);
} /* log_submit */

/*=========================================================================*/

boolean
log_submit_ext_core(void * ptr, unsigned int version, void * parameter)
{

   /* Variable Init */
   diag_log_rsp_type * log_pkt_ptr = NULL;
   log_header_type * log_ptr = (log_header_type *) ptr;
   log_code_type code;
   unsigned int length;
   boolean bReturnVal = FALSE;

   /* QDI Handle sanity check */
   if (diag_qdi_handle < DIAG_FD_MIN_VALUE)
      return FALSE;

   /* Sanity checks */
   if ((log_ptr == NULL) || ((version > DIAG_EXT_API_VERSION_0) && (parameter == NULL)) || (version > DIAG_EXT_API_VERSION_MAX))
   {
      return FALSE;
   }

   /* Fetch values from the log header */
   code = log_ptr->code;
   length = log_ptr->len;

   /* Length check */
   if (length <= sizeof(log_header_type))
   {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Alloc invalid length %d", length);
   }
   else if ((length + LOG_DIAGPKT_OFFSET) > DIAG_MAX_TX_PKT_SIZ)
   {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempt to allocate %d but Max Length of a log packet supported is %d", (length + LOG_DIAGPKT_OFFSET), DIAG_MAX_TX_PKT_SIZ);
   }
   else if (log_status(code))
   {
      /* Allocate enough for the entire log response packet, not just the log entry */
      log_pkt_ptr = (diag_log_rsp_type *) diagbuf_mpd_alloc_chain(DIAG_LOG_F, FPOS(diag_log_rsp_type, log) + length, 0, DIAGBUF_ALLOC_REQUEST_LOG, 1, version, parameter);

      if (log_pkt_ptr != NULL)
      {
         /* Fill in the log response (record) packet */
         diagbuf_mpd_write_stream_id (log_pkt_ptr, DIAG_STREAM_1);

         log_pkt_ptr->more = 0;
         log_pkt_ptr->cmd_code = DIAG_LOG_F;
         log_pkt_ptr->len = (uint16) length;

         /* Copy the log into the struct */
         memscpy(&log_pkt_ptr->log, length, (void *)ptr, length);

         /* Commit log to buffer */
         log_commit(&log_pkt_ptr->log);
         bReturnVal = TRUE;
      }
      else
      {
         /* Dropped a log */
         MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Dropped log 0x%x", code);
      }
   }

   return bReturnVal;

} /* end log_submit_ext */

/*===========================================================================

FUNCTION LOG_SET_TIMESTAMP_EX

DESCRIPTION
  This function captures the system time and stores it in the given log record.

DEPENDENCIES
   Diag log service must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void log_set_timestamp_ex(PACKED void *plog_hdr_ptr)
{
   if (plog_hdr_ptr)
   {
      time_get(((log_header_type *)plog_hdr_ptr)->ts);
   }
} /* log_set_timestamp_ex */

/*===========================================================================

FUNCTION DIAG_LSM_LOG_INIT

DESCRIPTION
 Initializes the log service. No masks will be updated here.
 Mask update is done after diagID has been set.

RETURN VALUE
 boolean indicating success

SIDE EFFECTS
 None.
===========================================================================*/
boolean Diag_LSM_Log_Init(void)
{
   boolean status = TRUE;
   int i = 0;

   log_mask_offset_tbl[0] = 0;

   for (i = 0; i < LOG_EQUIP_ID_LAST; i++)
   {
      log_mask_offset_tbl[i + 1] = 1 + log_mask_offset_tbl[i] +
         LOG_MASK_ARRAY_INDEX(log_last_item_tbl[i]);
   }
   log_state = LOG_NORMAL_STATE;
   return (status);
} /* Diag_LSM_Log_Init */

/*===========================================================================

FUNCTION    Diag_LSM_Log_DeInit

DESCRIPTION
  De-Initialize the Diag Log service.

DEPENDENCIES


RETURN VALUE
   boolean indicating success


SIDE EFFECTS


===========================================================================*/
boolean Diag_LSM_Log_DeInit(void)
{
   return (TRUE);
}     /* Diag_LSM_Log_DeInit */

/*===========================================================================

FUNCTION    log_update_mask

DESCRIPTION
  Update the log mask.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean log_update_mask(void)
{
   boolean status = TRUE;
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      dword maskLen_Recvd = 0;

      if (!(diag_lsm_ioctl(DIAG_IOCTL_GETLOGMASK, NULL, 0, log_mask, sizeof(log_mask), &maskLen_Recvd)) || maskLen_Recvd != LOG_MASK_SIZE)
      {
         status = FALSE;
      }
   }
   else
   {
      status = FALSE;
   }
   /* Update log_composite_mask when DIAG_IOCTL_GETLOGMASK is successful */
   if(status == TRUE)
   {
	  memscpy(log_composite_mask, sizeof(log_composite_mask),log_mask, sizeof(log_mask));
   }
   	
   return (status);
} /* log_update_mask */


