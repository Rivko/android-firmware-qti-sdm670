/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

Diag Legacy Service Mapping Layer Implementation for Diagnostics over STM

General Description
This routines in this file are used to send data over STM using the debugtrace
interfaces in LSM.
  
Copyright (c) 2014,2016-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/qurt/src/Diag_LSM_stm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/17   sp      Removed packed void * parameters 
06/01/16   nk      Fixed KW errors
11/14/14   vk      Multi-PD Optimization
07/06/14   ph      Added support for Diag over STM
===========================================================================*/

#include "comdef.h"
#include "stdlib.h"
#include "tracer.h"

#include "diagdiag_v.h"
#include "diagcmd.h"
#include "diagbuf_v.h"
#include "diagi_v.h"
#include "assert.h"

#include "Diag_LSMi.h"
#include "diag_shared_i.h" /* for definition of diag_data struct. */
#include "Diag_LSM_Qdi_wrapper.h"
#include "Diag_LSM_stm.h"
#include "osal.h"


#ifdef FEATURE_DIAG_STM
/* Safe guard to ensure file is not compiled if feature is not enabled */


/* Local variable declarations */
static boolean diag_stm_initialized = FALSE; // Indicates if diag tracer has been initialized
boolean diag_stm_enabled = FALSE; // Transmit data over STM

boolean diag_pthread_initialized = FALSE; // TLS key initialized for STM. Note: Must be done before diag_stm_enabled can be set to TRUE.
osal_tls_key_t diag_tracer_tls_key; // TLS key for diag tracer handle


/* Local #defines */
#define TRACER_PROTOCOL_DIAG 1 // Protocol ID value; not used by tracer interface.

/* Defines used for enabling Diag over STM */
#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) && !defined (DIAG_IMAGE_QDSP6_PROC)
/* The ARM9 modem and NOT QDSP6 modem */
#define DIAG_STM_PROC_MASK DIAG_STM_PROC_MASK_MODEM
#elif defined (DIAG_IMAGE_APPS_PROC) && !defined (DIAG_IMAGE_QDSP6_PROC)
/* Brew apps and WM apps */
#define DIAG_STM_PROC_MASK DIAG_STM_PROC_MASK_APPS
#elif defined(DIAG_IMAGE_QDSP6_PROC)
/* QDSP6 modem */
#define DIAG_STM_PROC_MASK DIAG_STM_PROC_MASK_ADSP
#elif defined(DIAG_IMAGE_RIVA_PROC)
/* RIVA */
#define DIAG_STM_PROC_MASK DIAG_STM_PROC_MASK_WCNSS
#endif

/*===========================================================================

FUNCTION DIAG_STM_INIT

DESCRIPTION
  Prepares Diag to be able to send traffic over STM. Note this does not enable
  Diag traffic over STM when called, it simply allows Diag to switch it on
  when needed.
 
  Specific init steps taken:
    1. Register any command handlers needed for enabling STM (if applicable)
    2. Create the TLS key used to store tracer handlers
 
  diag_stm_initialized will be set to TRUE only if all steps are successfully
  completed, especially the TLS key creation.
 
PARAMETERS
  None

DEPENDENCIES
  None

===========================================================================*/
void diag_stm_init(void)
{
  int rval;

  if(!diag_stm_initialized)
  {

    /* Create TLS key for storage of Diag tracer client handles.
       This key MUST be created before other Diag calls try to access
       the Diag tracer TLS information.
     */

    rval = osal_pthread_key_create(&diag_tracer_tls_key, NULL);

    if(rval == OSAL_SUCCESS) // Success
    {
      diag_stm_initialized = TRUE;
    }
  }
}

/*===========================================================================

FUNCTION DIAG_STM_WRITE_PKT

DESCRIPTION
  Writes the specified packet to STM using the tracer APIs using the tracer
  handle that exists for this thread in TLS. If a handle does not exist, one
  is created and then stored using TLS.
 
PARAMETERS
  ptr      Pointer to data to be written
  length   Number of bytes to write from ptr
 
DEPENDENCIES
  diag_stm_initliazed is TRUE

===========================================================================*/
void diag_stm_write_pkt(void *ptr, uint32 length)
{
  tracer_return_enum_t rval;
  static tracer_options_t diag_tracer_options = TRACER_OPT_TIMESTAMP;
  static uint32 diag_tracer_reg_error_cnt = 0;
  static uint32 diag_tracer_write_error_cnt = 0;
  tracer_client_handle_t client_handle;
  boolean write_flag = TRUE;
  int ret;

  if((ptr != NULL) && diag_stm_initialized && diag_stm_enabled && (length > 0))
  {
    /* Check if tracer handle has been written into TLS */
    client_handle = osal_pthread_getspecific(diag_tracer_tls_key);
    if(client_handle == NULL)
    {
      write_flag = FALSE;
      /* No handle exists; need to create a handle for this thread */
      rval = tracer_register(&client_handle, TRACER_ENTITY_DIAG, TRACER_PROTOCOL_DIAG);
      if(rval == TRACER_RETURN_SUCCESS)
      {
        ret = osal_pthread_setspecific(diag_tracer_tls_key, (void*)client_handle);
        ASSERT(ret == OSAL_SUCCESS);
        write_flag = TRUE;
      }
      else
      {
        // tracer didn't intiialize -- why?
        // Don't send error MSG, as it may result in an recursive loop
        // Increment debug counter for tracer registration
        diag_tracer_reg_error_cnt++;
      }
    }
    /* Send packet to tracer */
    if(write_flag)
    {

      rval = tracer_data(client_handle, diag_tracer_options, length, ptr);

      if(rval != TRACER_RETURN_SUCCESS)
      {
        // If write fails, increment debug counter for write failures
        diag_tracer_write_error_cnt++;
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_STM_ALLOC

DESCRIPTION
  Allocates a buffer using malloc, intended to be transmitted using the
  tracer APIs and then freed. Packets allocated using this API are not intended to
  be processed by the diag task.
 
  This is a direct wrapper for malloc (no additional headers added, etc.)
  Intended use case is allocating memory for messages and event reports, where
  diag will be determining the exact size of the final packet.
 
PARAMETERS 
  length      Length of buffer to allocate
 
DEPENDENCIES
 
 
===========================================================================*/
void * diag_stm_alloc(uint32 length)
{
  void *ptr = NULL;
  if(length > 0)
  {
    ptr = malloc(length);
  }
  return (ptr);
}

/*===========================================================================

FUNCTION DIAG_STM_FREE

DESCRIPTION
  Frees a buffer allocated using diag_stm_alloc
 
PARAMETERS 
  ptr      Pointer to free
 
DEPENDENCIES

===========================================================================*/
void diag_stm_free(void *ptr)
{
  free(ptr);
}

/*===========================================================================

FUNCTION DIAG_STM_LSM_LOG_ALLOC

DESCRIPTION
  Allocates a log packet using malloc, intended to be transmitted using the
  tracer APIs and then freed. Logs allocated using this API are not intended to
  be processed by the diag task.
 
  For consistency in other log handling APIs, the method for allocating the logs
  and the headers used are consistent with the standard diagbuf_alloc function.
 
  The status member of the diagbuf header is set to DIAGBUF_STM_BUFFER_S to
  allow other Diag APIs to know where the buffer was allocated from.
 
PARAMETERS 
  length               The specified length is the length of the entire log, 
                       including the log header.  
DEPENDENCIES
 
WARNING 
  This function should not be called directly. Use macro
  diag_stm_log_alloc(x) instead.
 
===========================================================================*/
void * diag_stm_lsm_log_alloc(uint16 length)
{
  uint32 alloc_length;
  void * ptr = NULL; /* Pointer to return.        */
  diagbuf_header_type *pkt_hdr;

  if(length == 0)
  {
    /* Return NULL if an empty log packet is being requested */
    return (NULL);
  }

  alloc_length =
     sizeof(diagbuf_header_type) +  /* Diagbuf header    */
     (uint32)length;     /* Number of bytes requested buffer */

  ptr = malloc(alloc_length);

  if(ptr)
  {
    /* Set header field of buffer */
    pkt_hdr = ((diagbuf_header_type *)ptr);

    pkt_hdr->status = DIAGBUF_STM_BUFFER_S;
    /* Note: pkt_hdr->pad not needed for STM implementation, sicne this allocation is not
       going into diagbuf */
    pkt_hdr->length = length;

    /* Set 'ptr' to pkt_hdr +1 = data following pkt_hdr */
    ptr = pkt_hdr + 1;
  }

  return (ptr);
} /* diag_stm_lsm_log_alloc */

/*===========================================================================

FUNCTION DIAG_STM_LSM_LOG_FREE

DESCRIPTION
  Frees a buffer allocated using diag_stm_log_alloc (accounts for diagbuf header)
 
PARAMETERS 
  ptr      Pointer to free 
 
DEPENDENCIES
 
WARNING 
  This function should not be called directly. Use macro
  diag_stm_log_free(x) instead.
 
===========================================================================*/
void diag_stm_lsm_log_free(void *ptr)
{
  diagbuf_header_type *pkt_hdr;

  if(ptr)
  {
    pkt_hdr = ((diagbuf_header_type *)ptr) - 1;
    free(pkt_hdr);
  }
} /* diag_stm_lsm_log_free */

/*===========================================================================

FUNCTION DIAG_STM_UPDATE_MODE_LSM

DESCRIPTION
  Used to get the stm mode information from guest OS using the IOCTL
  for STM.
 
PARAMETERS
  None
 
DEPENDENCIES
 
RETURN 
  Returns 
    TRUE  Retrieved the STM mode information successfully.
    FALSE Unable to get the STM mode information.
===========================================================================*/
boolean diag_stm_update_mode_LSM(void)
{
	boolean status = TRUE;
	boolean stm_mode;
   if(diag_qdi_handle>=DIAG_FD_MIN_VALUE)
   {
      dword Len_Recvd = 0;
	  if(!(diag_lsm_ioctl(DIAG_IOCTL_GET_STM_MODE, NULL,0, &stm_mode,sizeof(diag_stm_enabled), &Len_Recvd)) || Len_Recvd != LOG_MASK_SIZE ) 
      {
		diag_stm_enabled = stm_mode;
        status = FALSE;
      }
   }
   else
   {
      status = FALSE;
   }
	return status;
}

#endif /* FEATURE_DIAG_STM */
