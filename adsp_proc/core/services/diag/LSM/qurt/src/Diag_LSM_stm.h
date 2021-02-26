#ifndef DIAG_LSM_STM_H
#define DIAG_LSM_STM_H



/*==========================================================================

                            Diagnostics over STM

Description
  Internal type and function definitions used by Diag for sending data
  over STM via the debugtrace interfaces.

Copyright (c) 2014, 2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                          Edit History

      $Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/qurt/src/Diag_LSM_stm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/17   sp      Added support for extended logs 
03/03/17   sp      Removed packed void * parameters 
11/14/14   vk      Multi-PD Optimization
07/06/14   ph      Added support for Diag over STM

===========================================================================*/

#ifdef FEATURE_DIAG_STM

 /* Flag Indicating if data should be transmitted using STM */
extern boolean diag_stm_enabled;

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
void diag_stm_init(void);

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
void diag_stm_write_pkt(void *ptr, uint32 length);

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
void * diag_stm_alloc(uint32 length);

/*===========================================================================

FUNCTION DIAG_STM_FREE

DESCRIPTION
  Frees a buffer allocated using diag_stm_alloc
 
PARAMETERS 
  ptr      Pointer to free
 
DEPENDENCIES

===========================================================================*/
void diag_stm_free(void *);

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
  code                 ID of the event to be reported
  length               The specified length is the length of the entire log,
                       including the log header.
DEPENDENCIES

===========================================================================*/
void * diag_stm_lsm_log_alloc(uint16 length);

/*===========================================================================

FUNCTION DIAG_STM_LSM_LOG_FREE

DESCRIPTION
  Frees a buffer allocated using diag_stm_lsm_log_alloc
  (accounts for diagbuf header)

PARAMETERS
  ptr      Pointer to free

DEPENDENCIES

===========================================================================*/
void diag_stm_lsm_log_free(void *ptr); 

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
boolean diag_stm_update_mode_LSM(void);

/* Wrappers for MSG and EVENT allocations */
#define diag_stm_msg_alloc(x) diag_stm_alloc(x)
#define diag_stm_event_alloc(x) diag_stm_alloc(x)
#define diag_stm_log_alloc(x) diag_stm_lsm_log_alloc(x)
#define diag_stm_msg_free(x) diag_stm_free(x)
#define diag_stm_event_free(x) diag_stm_free(x)
#define diag_stm_log_free(x) diag_stm_lsm_log_free(x)

#else

/* If FEATURE_DIAG_STM is not defined, hard code enabled flag to FALSE */
#define diag_stm_enabled FALSE

#endif


#endif
