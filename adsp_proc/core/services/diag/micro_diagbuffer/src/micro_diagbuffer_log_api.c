/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Logging Services

General Description
This file contains the definitions to log apis for micro image. This APIs will be called by
clients when they want to generate a new log. It will check to see if uImage mode is active
or not when it is called. If active, the API will copy the log packet into the uImage diag_buffer
using the diag_buffer interface. A dedicated diag_buffer will be set aside for uImage logging.
This buffer will act as a circular buffer and will not be drained while in uImage mode. Upon exiting
uImage mode, Diag task will drain out the uImage diag_buffer as part of it’s regular draining process.

Copyright (c) 2015-2018 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                              Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/micro_diagbuffer_log_api.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/18   is      Add support for pd threshold buffering mode in island mode
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
11/02/16   ph      Added micro_log_set_timestamp API.
08/12/15   xy      Verified DIAG uimage APIs in island mode
06/15/15   xy      Added new DIAG APIs for uImage mode
05/12/15   xy      Added support for DIAG uImage APIs
05/12/15   xy      Created file
===========================================================================*/
#include "diaglogi.h" /*for LOG_MASK_SIZE*/
#include "diag_v.h" /*for DIAGBUF_VERSION_1*/
#include "diagdiag_v.h" /*for MAX_VALUE_UINT32 */
#include "msgcfg.h"
#include "log.h"
#include "micro_msg_diag_service.h"
#include "micro_diagbuffer.h"
#include "diag_micro_i.h" /*For log_mask_enabled()*/

#if defined(DIAG_UIMAGE_MODE)
#include "micro_diagbuf_mpd_type.h"
extern micro_diagbuf_mpd_type* micro_diagmpd_buf;
#endif

extern byte log_composite_mask[LOG_MASK_SIZE];
extern byte log_listener_mask[LOG_MASK_SIZE];

extern log_state_enum_type log_state;

uint32 micro_alloc_count_log = 0;

extern void * log_alloc_core (log_code_type code,  unsigned int length);
extern void * log_alloc_ext_core (log_code_type code, unsigned int length, unsigned int version, void * parameter);
extern void log_commit_core (void * ptr);
extern void log_free_core (void * ptr);
extern void log_shorten_core (void * ptr,  unsigned int length);
extern void log_set_timestamp_ex (void * log_hdr_ptr);
extern boolean log_submit_core (void * ptr);
extern boolean log_submit_ext_core (void * ptr, unsigned int version, void * parameter);



/*===========================================================================

FUNCTION LOG_ALLOC

DESCRIPTION
  Allocate a buffer of size 'length' for logging data. Supports version field
 
PARAMETERS 
  code                 ID of the log to be reported
  length               The specified length is the length of the entire log, 
                             	including the log header. 
=========================================================================*/

void * log_alloc (
  log_code_type code,
  unsigned int length
)
{
   void *return_ptr = NULL;
#if defined(DIAG_UIMAGE_MODE)  
	/*check if it is in micro image mode*/
	if (diag_island_mode_check())
	{
		return return_ptr;
	}
	else
#endif
	{
		/*if not in micro image mode, call regular API*/
		return_ptr = log_alloc_core (code, length);
	}
	
	  return return_ptr;
}/*log_alloc*/

/*===========================================================================

FUNCTION LOG_ALLOC_EXT

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
log_alloc_ext (log_code_type code, unsigned int length, unsigned int version, void * parameter)
{ 
    void *return_ptr = NULL;
#if defined(DIAG_UIMAGE_MODE)  
	/*check if it is in micro image mode*/
	if (diag_island_mode_check())
	{
		return return_ptr;
	}
	else
#endif
	{
		/*if not in micro image mode, call regular API*/
		return_ptr = log_alloc_ext_core (code, length,version,parameter);
	}
	
	  return return_ptr;
}

/*=========================================================================*/
void log_commit (void * ptr)
{    
#if defined(DIAG_UIMAGE_MODE)  
	/*check if it is in micro image mode*/
	if (diag_island_mode_check())
	{
		return ;
	}
	else
#endif
	{
		/*if not in micro image mode, call regular API*/
		log_commit_core (ptr);
	}
}

/*=========================================================================*/
void log_free (void * ptr)
{
#if defined(DIAG_UIMAGE_MODE)  
	/*check if it is in micro image mode*/
	if (diag_island_mode_check())
	{
		return ;
	}
	else
#endif
	{
		/*if not in micro image mode, call regular API*/
		log_commit_core (ptr);
	}
} /* log_free */

/*=========================================================================*/
void log_shorten (
  void * ptr,
  unsigned int length
)
{
#if defined(DIAG_UIMAGE_MODE)  
	/*check if it is in micro image mode*/
	if (diag_island_mode_check())
	{
		return ;
	}
	else
#endif
	{
		/*if not in micro image mode, call regular API*/
		log_shorten_core (ptr,length);
	}  
} /* log_shorten */

/*===========================================================================
FUNCTION LOG_SUBMIT

DESCRIPTION
   Allocate a buffer of size 'length' for logging data. And copy the data from
   the ptr passed into the buffer.  This API will be called by clients when they 
   want to generate a new log. It will check to see if uImage mode is active or 
   not when it is called. If active, the API will copy the log packet into the 
   uImage diag_buffer using the diag_buffer. interface. A dedicated diag_buffer 
   will be set aside for uImage logging. This buffer will act as a circular
   buffer and will not be drained while in uImage mode. Upon exiting uImage mode,
   Diag task will drain out the uImage diag_buffer as part of it’s regular draining process.

PARAMETERS
  code                 ID of the log to be reported
  length               The specified length is the length of the entire log,
                       including the log header.
  ptr                  Pointer to the log packet.
=========================================================================*/
boolean log_submit (void * ptr)
{
    boolean return_val = FALSE;
#if defined(DIAG_UIMAGE_MODE)  
	/*check if it is in micro image mode*/
	if (diag_island_mode_check())
	{
        /* micro_diagbuffer_drain_check() will be called in log_submit_ext(), so 
           no need to call again here. */
		return_val=log_submit_ext(ptr, 0,NULL);
		return return_val;
	}
	else
#endif
	{
		/*if not in micro image mode, call regular API*/
		return_val=log_submit_core (ptr);
	}
    return return_val;
    
} /* log_submit */


/*===========================================================================
FUNCTION LOG_SUBMIT_EXT

DESCRIPTION
   Allocate a buffer of size 'length' for logging data. And copy the data from
   the ptr passed into the buffer. Supports version field  This API will be called
   by clients when they want to generate a new log. It will check to see if uImage
   mode is active or not when it is called. If active, the API will copy the log
   packet into the uImage diag_buffer using the diag_buffer. interface. A dedicated
   diag_buffer will be set aside for uImage logging. This buffer will act as a circular
   buffer and will not be drained while in uImage mode. Upon exiting uImage mode,
   Diag task will drain out the uImage diag_buffer as part of it’s regular draining process.

PARAMETERS
  code                 ID of the log to be reported
  length               The specified length is the length of the entire log,
                       including the log header.
  ptr                  Pointer to the log packet.
  version              Specifies the version - currently, there are two versions
                       0 - same as log_submit() if not running in uImage mode. In uImage mode,
                           the buffering/queueing behavior is slightly different
                       1 - The outgoing log packet header will have support to
                           include ID field to differentiate the log pkts
                           coming from different instances of the processor.
                           Same as log_submit_ext() if not running in uImage mode. In uImage mode,
                           the buffering/queueing behavior is slightly different
 *parameter           Based on the version passed in, this pointer is interpreted
                      as follows-
                        version       pointer
                         0             NULL
                         1             uint32 * . The uint32 value this pointer points to,
                                       is the ID of the instance generating the
                                       log packet
=========================================================================*/
boolean log_submit_ext (void * ptr, unsigned int version, void * parameter)
{

  boolean bReturnVal = FALSE;
  log_header_type *log_ptr = (log_header_type *)ptr;
  log_code_type code;
  unsigned int length;
  
#if defined(DIAG_UIMAGE_MODE)
  micro_diagbuffer_result ret_value = MICRO_DIAGBUFFER_ERROR;
#endif

  if ((ptr == NULL) || ((version == DIAGBUF_VERSION_1) && (parameter == NULL)) || (version > DIAGBUF_VERSION_1))
  {
    return FALSE;
  }

  code = log_ptr->code;
  length = log_ptr->len;
  
#if defined(DIAG_UIMAGE_MODE)  
/*check if it is in micro image mode*/
  if (diag_island_mode_check())
  {
    if (!log_status(code))
    {
      /*if log is not enabled, return*/
      return FALSE;
    }
    /*copy the log into diag_buffer*/
	if(micro_diagmpd_buf)
	{
	  ret_value =  MICRO_DIAGBUFFER_LOG(micro_diagmpd_buf->handle, code, version, parameter, length, ptr);
	}
	else
	{
	  ret_value = MICRO_DIAGBUFFER_ERR_INITINCOMPLETE;
	}
	
    bReturnVal = MICRO_DIAGBUFFER_GET_RESULT(ret_value);
	
	if(bReturnVal==TRUE)
	{
	  if ( micro_alloc_count_log == MAX_VALUE_UINT32 )
	  {
	    MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_log has reached maximum value.");
	  }
	  else
	  {
	    micro_alloc_count_log++;
	  }
      
      /* After successful allocation, check if we've reached threshold for draining. */
      micro_diagbuffer_drain_check();
	}
  }
  else
#endif
  {
    /*if not in micro image mode, call regular API*/
    bReturnVal = log_submit_ext_core (ptr, version, parameter);
  }

  return bReturnVal;
  
} /*log_submit_ext*/


/*===========================================================================

FUNCTION LOG_SET_LENGTH

DESCRIPTION
  This function sets the length field in the given log record.

PARAMETERS
  *ptr          ptr points to the log whose length field is being set
  length        length to be set in the log header
=========================================================================*/
void log_set_length (void * ptr, unsigned int length)
{
  if (ptr == NULL)
  {
    return;
  }
  /* All log packets are required to start with 'log_header_type'. */
  ((log_header_type *) ptr)->len = (uint16) length;

} /* log_set_length */


/*===========================================================================
FUNCTION LOG_SET_CODE

DESCRIPTION
  This function sets the log code

PARAMETERS
  *ptr         ptr points to the log whose logcode field is being set
  code         length to be set in the log header
=========================================================================*/
void log_set_code (void * ptr, log_code_type code)
{
  if (ptr == NULL)
  {
    return;
  }
  /* All log packets are required to start with 'log_header_type'. */
  ((log_header_type *) ptr)->code = code;

} /* log_set_code */

/*===========================================================================
FUNCTION LOG_GET_LENGTH

DESCRIPTION
   This function returns the length field in the given log record.

PARAMETERS
  *ptr         ptr points to the log whose length field is being queried.

RETURN VALUE
  unsigned     the length field in the given log record.
=========================================================================*/
unsigned int log_get_length(void * ptr)
{
  log_header_type *log = (log_header_type *) ptr;
  unsigned int length = 0;

  if (log)
  {
    length = log->len;
  }

  return length;
} /* log_get_length */

/*===========================================================================
FUNCTION LOG_GET_CODE

DESCRIPTION
   This function returns the log code of the given log record.

PARAMETERS
  *ptr         ptr points to the log whose code field is being queried.

RETURN VALUE
  log_code_type  log_code
=========================================================================*/
log_code_type log_get_code (void * ptr)
{
  log_header_type *log = (log_header_type *) ptr;
  log_code_type code = 0;

  if (log)
  {
    code = (log_code_type) log->code;
  }

  return code;
} /* log_get_code */

/*===========================================================================
FUNCTION LOG_GET_STATUS

DESCRIPTION
   This function returns whether a particular code is enabled for logging.

PARAMETERS
  code     Specifies the code.

RETURN VALUE
  TRUE   if log mask is enabled
  FALSE  if log mask is disabled
=========================================================================*/
boolean
log_status (log_code_type code)
{
  unsigned int id, item;
  boolean status = FALSE;
  id = LOG_GET_EQUIP_ID (code);
  item = LOG_GET_ITEM_NUM (code);
  
   /* If we're in active mode, check log_state first */
  if( diag_island_mode_check() == FALSE ) 
  {
    if(log_state == LOG_NORMAL_STATE)
    {
       status = log_mask_enabled(log_composite_mask, id, item);
    }
    else if(log_state == LOG_FLUSH_STATE)
    {
       status = log_mask_enabled(log_listener_mask, id, item);
    }
  }
  else{
     /* check if log mask for uimag mode is enabled*/
     status = log_mask_enabled(log_composite_mask, id, item);
  }
  return status;
} /* log_status */

/*===========================================================================
FUNCTION LOG_SET_TIMESTAMP

DESCRIPTION
   This function sets the timestamp for the given log pointer passed when diag is 
	 not in island mode. In island mode, this is just a stub and doesn't set 
	 timestamp. Timestamp update is taken care in micro_log_submit_ext() API and 
	 client need not set the timestamp field explicitly. 

PARAMETERS
  ptr     Pointer to the log packet.

RETURN VALUE
  None
 
=========================================================================*/
void log_set_timestamp ( void * ptr)
{
  if( !diag_island_mode_check())
  {
    log_set_timestamp_ex(ptr);
  }
  return;
}
