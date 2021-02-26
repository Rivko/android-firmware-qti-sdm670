/*============================================================================
  micro_diagbuffer_uimage.c

  Public-facing/support micro_diagbuffer uImage island mode functions.

  Copyright (c) 2015-2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/micro_diagbuffer_uimage.c#2 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/17   nk      Qshrink 2 support for uimage diag messages
06/14/17   sp      Created new file to contain uImage micro diagbuffer front end API's
06/10/16   as      Fixed external references from Diag in island mode to enable whitelisting
02/18/16   as      Added check for integer overflow in micro_ULogInternal_FreeSpaceForNewMsg
02/11/16   as      Resolved integer overflow and definition of 'remainingSpace'
11/19/15   as      Resolved error checking of log name
10/28/15   is      Resolve issues with micro_log_submit_ext() and MICRO_MSG_*_EXT() APIs
08/16/15   vk      as part of micro diagbuffer drain changes moved #defines 
                   to allow them to be visible in other .c files                   
06/12/15   is      Add support to log Diag msgs, logs, and events

===========================================================================*/

#include "event_defs.h" /* For event_id_enum_type */
#include "log.h"        /* For log_code_type */
#include "qurt.h"       /* For qurt_island_get_status */
#include "micro_diagbuffer.h"
#include "micro_diagbuffer_i.h"
#include <stringl/stringl.h>  /* For memscpy */
#include "micro_diagbuf_mpd_type.h"

#ifdef DIAG_UIMAGE_MODE
extern micro_diagbuffer_handle uimage_diagbuffer;
extern micro_diagbuf_mpd_type* micro_diagmpd_buf;
#endif

#define MSG_V2_CHECK_IS_HASH(X) ((((uint32)(X)) & 0x03) != 0)

/**
 * Routines
 */

/**
 * <!-- micro_diagbuffer_debug_vmsg_ext -->
 *
 * @brief Writes printf data to the specified log
 * Example: micro_diagbuffer_debug_vmsg_ext(handle, 3, "test %d: %s", 1, "complete");
 *
 * @param log : Handle to the micro_diagbuffer 
 * @param const_type: constant pointer to the string of the msg
 * @param num_args : Number of word-sized parameters in the given va_list
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 * @param ap : A variable list of arguments to the printf
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result micro_diagbuffer_debug_vmsg_ext( micro_diagbuffer_handle log,
                                             const msg_v2_const_type* const_type,
                                             uint8 num_args,
                                             uint8 version,
                                             const void* param,
                                             va_list ap )
{
  uint32 total_size;
  uint32 bufsize_mask;
  uint8 param_size = 0;
  uint32 *dwordPtr=NULL;
  uint16 *wordPtr=NULL;
  uint8 *bytePtr=NULL;
  uint8 i;
  micro_diagbuffer_result ret = MICRO_DIAGBUFFER_SUCCESS;
  uint32 pad =0;
  uint32 isHash=0;
  uint32 msg_size=0;
  int filename_len = 0;
  int fmt_str_len = 0;
  char *str_buf = NULL;

  if ( !log || !(log->logStatus & MICRO_ULOG_STATUS_ENABLED) || num_args > 9 )
  {
    return MICRO_DIAGBUFFER_ERROR;
  }

  if (version == 0)
  {
    /* No parameters */
    param_size = 0;
  }
  else if (version == 1)
  {
    /* Param field is pointer to a uint32 ID */
    param_size = sizeof(uint32);
  }
  else
  {
    /* Unsupported version */
    return MICRO_DIAGBUFFER_ERROR;
  }
  
  bufsize_mask = MICRO_ULOG_BUFSIZE_MASK( log );
  isHash = MSG_V2_CHECK_IS_HASH(const_type->msg);

  if(isHash){

      /* format + length + TS + num_args + version + 2byts padding + msg_desc_type + hash_value + param + args */
	 total_size = 2 + 2  + MICRO_ULOG_TIMESTAMP_SIZE + sizeof(uint8) + sizeof(uint8) + sizeof(uint16) + sizeof(msg_desc_type) + sizeof(uint16) + param_size + (num_args * sizeof(uint32));
	 pad = MICRO_ULOG_PADCOUNT( total_size ) - total_size; //add padding at the end of the msg to round it to the next word boundary 
     if (pad >=sizeof(uint32)) 
     {
        return MICRO_DIAGBUFFER_ERROR;
     }
     if ( micro_ULogInternal_UsesFastWrite( (micro_ULogHandle)log ) )
     {
        uint32 size_idx;
        ret = (micro_diagbuffer_result)micro_ULogInternal_FastFmtSizeTS( (micro_ULogHandle)log, total_size, 
                                            MICRO_ULOG_SUBTYPE_DIAG_MSG_QSR2, &size_idx );
        if ( ret == MICRO_DIAGBUFFER_SUCCESS )
        {	 

			/* Num args */
            bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
            *bytePtr = (uint8)num_args;
            log->write += 1;

			/* Version */
            bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
            *bytePtr = (uint8)version;
            log->write += 1;

			/*reserved 2 bytes*/
            bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
            *bytePtr = 0;
            log->write += 1;

			bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
            *bytePtr = 0;
            log->write += 1;


            /* Arguments */
            for ( i = 0; i < num_args; i++ )
            {
               dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
               *dwordPtr = (uint32)va_arg( ap, uint32 );
               log->write += 4;
            }
			
            /* Param */
            if (version == 1)
            {
               dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
               *dwordPtr = *((uint32 *)param);
               log->write += 4;
            }
      
			/* Copy line number */
            wordPtr = (uint16 *)&(log->buffer[log->write & bufsize_mask]);
			*wordPtr = (uint16)(((msg_v2_const_type *)const_type)->desc.line);
			log->write += 2;

			/* Copy SSID */
            wordPtr = (uint16 *)&(log->buffer[log->write & bufsize_mask]);
			*wordPtr = (uint16)(((msg_v2_const_type *)const_type)->desc.ss_id);
			log->write += 2;

			/* Copy ss mask */
            dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
			*dwordPtr = (uint32)(((msg_v2_const_type *)const_type)->desc.ss_mask);
			log->write += 4;

			/* Copy hash value */
            dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
			*dwordPtr = (uint32)(((msg_v2_const_type *)const_type)->msg);
			log->write += 4;
			
            /*add padding at the end of the msg to round it to the next word boundary */
            log->write += pad;
			
            /* Update the message length */
            dwordPtr = (uint32 *)&(log->buffer[size_idx & bufsize_mask]);
            *dwordPtr |= ((total_size << MICRO_ULOG_LENGTH_SHIFT) & MICRO_ULOG_LENGTH_MASK);

            /* Release the lock, acquired within FastFmtSizeTS */
            MICRO_ULOG_UNLOCK_LOG( log );		
      }
   }
  else /* no fastwrite */
  {
    uint8 index = 0;
    uint64 timestamp;
    uint8 dataArray[68]; //subtype (4) + timestsamp (8)  + num_args (1) + version (1) +reserved(2)
                         // + msg_const_type (8) + hash (4) + param(4) + max args (4x9) = 58 bytes

	/* Message metadata */
    dwordPtr = (uint32 *)&(dataArray[index]);
    *dwordPtr++ = MICRO_ULOG_SUBTYPE_DIAG_MSG_QSR2;
    index += 4;
    
    /* Timestamp */
    timestamp = MICRO_ULOG_GETTIME64();
    *dwordPtr++ = (uint32)timestamp;
    *dwordPtr++ = (uint32)((uint64)timestamp >> 32);
    index += 8;

	/* Num args */
    bytePtr = (uint8 *)&(dataArray[index]);
    *bytePtr++ = (uint8)num_args;
    index += 1;

    /* Version */
    *bytePtr++ = (uint8)version;
    index += 1;
 
    /* Reserved 2 bytes */
    *bytePtr++ = (uint8)version;
    index += 1;
    *bytePtr++ = (uint8)version;
    index += 1;
	
    /* Arguments */
    dwordPtr = (uint32 *)&(dataArray[index]);
    for ( i = 0; i < num_args; i++ )
    {
      *dwordPtr++ = va_arg( ap, uint32 );
      index += 4;
    }
	
    /* Param */
    if (version == 1)
    {
      dwordPtr = (uint32 *)&(dataArray[index]);
      *dwordPtr++ = *((uint32*)param);
      index += 4;
    }
      
	/* Copy line number */
	wordPtr = (uint16 *)&(dataArray[index]);
	*wordPtr++ = (uint16)(((msg_v2_const_type *)const_type)->desc.line);
	index += 2;
			 
	/* Copy SSID */
	wordPtr = (uint16 *)&(dataArray[index]);
	*wordPtr++ = (uint16)(((msg_v2_const_type *)const_type)->desc.ss_id);
	index += 2;
			 
	/* Copy ss mask */
	dwordPtr = (uint32 *)&(dataArray[index]);
	*dwordPtr++ = (uint32)(((msg_v2_const_type *)const_type)->desc.ss_mask);
	index += 4;
		 
	/* Copy hash value */
	dwordPtr = (uint32 *)&(dataArray[index]);
	*dwordPtr++ = (uint32)(((msg_v2_const_type *)const_type)->msg);
	index += 4;

    ret = log->transportVTable->write( log, total_size,
                                       (char *)dataArray, 0, NULL );
  }

  return ret;
}
else{

    if(micro_diagmpd_buf->buffer_location== DIAG_ROOTPD){

     /* format + length + TS + msg_const_type*+ num_args + version + 2byts padding + param + args */
    total_size = 2 + 2  + MICRO_ULOG_TIMESTAMP_SIZE + sizeof(msg_const_type*) + sizeof(uint8) + sizeof(uint8) + sizeof(uint16)+ param_size + (num_args * sizeof(uint32));
	pad = MICRO_ULOG_PADCOUNT( total_size ) - total_size; //add padding at the end of the msg to round it to the next word boundary 
	if (pad >=sizeof(uint32)) 
	{
	  return MICRO_DIAGBUFFER_ERROR;
	}

	if ( micro_ULogInternal_UsesFastWrite( (micro_ULogHandle)log ) )
	{
	  uint32 size_idx;
	  ret = (micro_diagbuffer_result)micro_ULogInternal_FastFmtSizeTS( (micro_ULogHandle)log, total_size, 
										  MICRO_ULOG_SUBTYPE_DIAG_MSG, &size_idx );
	  if ( ret == MICRO_DIAGBUFFER_SUCCESS )
	  {
		/* Address of const msg_const_type */
		dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
		*dwordPtr = (uint32)const_type;
		log->write += 4;

		/* Num args */
		bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
		*bytePtr = (uint8)num_args;
		log->write += 1;
		
		/* Version */
		bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
		*bytePtr = (uint8)version;
		log->write += 1;
		
		/*reserved 2 bytes*/
		bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
		*bytePtr = 0;
		log->write += 1;
			  
		bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
		*bytePtr = 0;
		log->write += 1;
					
		/* Param */
		if (version == 1)
		{
		  dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
		  *dwordPtr = *((uint32 *)param);
		  log->write += 4;
		}
	
		/* Arguments */
		for ( i = 0; i < num_args; i++ )
		{
		  dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
		  *dwordPtr = (uint32)va_arg( ap, uint32 );
		  log->write += 4;
		}

		/*add padding at the end of the msg to round it to the next word boundary */
		log->write += pad;
		/* Update the message length */
		dwordPtr = (uint32 *)&(log->buffer[size_idx & bufsize_mask]);
		*dwordPtr |= ((total_size << MICRO_ULOG_LENGTH_SHIFT) & MICRO_ULOG_LENGTH_MASK);

		/* Release the lock, acquired within FastFmtSizeTS */
		MICRO_ULOG_UNLOCK_LOG( log );
	  }
	}
	else /* no fastwrite */
	{
	  uint8 index = 0;
	  uint64 timestamp;
	  uint8 dataArray[68]; //subtype (4) + timestsamp (8) + addr of msg_const_type (4) 
						   //+ num_args (1) + version (1) + param (4) + max args (4x9) = 58 bytes

	  /* Message metadata */
	  dwordPtr = (uint32 *)&(dataArray[index]);
	  *dwordPtr++ = MICRO_ULOG_SUBTYPE_DIAG_MSG;
	  index += 4;
	  
	  /* Timestamp */
	  timestamp = MICRO_ULOG_GETTIME64();
	  *dwordPtr++ = (uint32)timestamp;
	  *dwordPtr++ = (uint32)((uint64)timestamp >> 32);
	  index += 8;

	  /* Address of const msg_const_type */
	  *dwordPtr++ = (uint32)const_type;
	  index += 4;
	  
	  /* Num args */
	  bytePtr = (uint8 *)&(dataArray[index]);
	  *bytePtr++ = (uint8)num_args;
	  index += 1;
	  
	  /* Version */
	  *bytePtr++ = (uint8)version;
	  index += 1;

	  /* Reserved 2 bytes */
	  *bytePtr++ = (uint8)version;
	  index += 1;
	  *bytePtr++ = (uint8)version;
	  index += 1;
		   
	  /* Param */
	  if (version == 1)
	  {
		dwordPtr = (uint32 *)&(dataArray[index]);
		*dwordPtr++ = *((uint32*)param);
		index += 4;
	  }
		
	  /* Arguments */
	  dwordPtr = (uint32 *)&(dataArray[index]);
	  for ( i = 0; i < num_args; i++ )
	  {
		*dwordPtr++ = va_arg( ap, uint32 );
		index += 4;
	  }

	  ret = log->transportVTable->write( log, total_size,
										 (char *)dataArray, 0, NULL );
	}

	return ret;
 }
 else if(micro_diagmpd_buf->buffer_location== DIAG_USERPD){
 	
    char null_terminator = '\0';
	
	for (str_buf = (char *)const_type->msg ; *str_buf != ':'; str_buf++) 
		;
    filename_len = str_buf - (char *)(const_type)->msg;
	fmt_str_len = MIN(MSG_MAX_STRLEN, (int)strlen((char *)const_type->msg+ filename_len + 1));
	
	
    /* total number of bytes to be allocated, including dereferenced FileName and Format strings */
    msg_size = sizeof(msg_desc_type) + fmt_str_len + 1 + filename_len + 1 ;
	
	/* format + length + TS + num_args + version + 2byts padding + args + param + line no + SSID + ss mask + format string + file name */
	total_size = 2 + 2  + MICRO_ULOG_TIMESTAMP_SIZE + sizeof(uint8) + sizeof(uint8) + sizeof(uint16) + (num_args * sizeof(uint32)) + param_size + msg_size ;
	pad = MICRO_ULOG_PADCOUNT( total_size ) - total_size; //add padding at the end of the msg to round it to the next word boundary 
	if (pad >=sizeof(uint32)) 
	{
	  return MICRO_DIAGBUFFER_ERROR;
	}

	if ( micro_ULogInternal_UsesFastWrite( (micro_ULogHandle)log ) )
	{
	  uint32 size_idx;
	  ret = (micro_diagbuffer_result)micro_ULogInternal_FastFmtSizeTS( (micro_ULogHandle)log, total_size, 
	                                          MICRO_ULOG_SUBTYPE_DIAG_MSG_EXPANDED, &size_idx );
	  if ( ret == MICRO_DIAGBUFFER_SUCCESS )
	  {	
			/* Num args */
            bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
            *bytePtr = (uint8)num_args;
            log->write += 1;
			
			/* Version */
            bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
            *bytePtr = (uint8)version;
            log->write += 1;

			/*reserved 2 bytes*/
            bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
            *bytePtr = 0;
            log->write += 1;

			bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
            *bytePtr = 0;
            log->write += 1;
			
			/* Arguments */
			for ( i = 0; i < num_args; i++ )
			{
				dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
				*dwordPtr = (uint32)va_arg( ap, uint32 );
				log->write += 4;
			}

			 /* Param */
			 if (version == 1)
			 {
				 dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
				 *dwordPtr = *((uint32 *)param);
				 log->write += 4;
			 }
				
			 /* Copy line number */
            wordPtr = (uint16 *)&(log->buffer[log->write & bufsize_mask]);
			*wordPtr = (uint16)(((msg_v2_const_type *)const_type)->desc.line);
			log->write += 2;

			/* Copy SSID */
            wordPtr = (uint16 *)&(log->buffer[log->write & bufsize_mask]);
			*wordPtr = (uint16)(((msg_v2_const_type *)const_type)->desc.ss_id);
			log->write += 2;

			/* Copy ss mask */
            dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
			*dwordPtr = (uint32)(((msg_v2_const_type *)const_type)->desc.ss_mask);
			log->write += 4;

            /*The arguments are copied in qsr_msg_send* functions*/
            /*Copy the fmt string*/
			dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
            memscpy(((char *)(dwordPtr)), fmt_str_len, 
		            (const_type->msg + filename_len + 1), fmt_str_len);
           
            *((char *)(dwordPtr)+fmt_str_len) = null_terminator;
			log->write += fmt_str_len+1;

            /*Copy the file name*/
			dwordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
            memscpy(((char *)(dwordPtr)), filename_len, 
		            (const_type->msg), filename_len);
					
			/*Copy the null terminator*/
            *((char *)(dwordPtr) + filename_len) = null_terminator;
			
            log->write += filename_len+1;

			 /*add padding at the end of the msg to round it to the next word boundary */
			log->write += pad;
			

			/* Update the message length */
			dwordPtr = (uint32 *)&(log->buffer[size_idx & bufsize_mask]);
			*dwordPtr |= ((total_size << MICRO_ULOG_LENGTH_SHIFT) & MICRO_ULOG_LENGTH_MASK);
			
			/* Release the lock, acquired within FastFmtSizeTS */
			MICRO_ULOG_UNLOCK_LOG( log );	 
	  }
    }
	else /* no fastwrite */
	{
	  uint8 index = 0;
	  uint64 timestamp;
	  uint8 dataArray[204]; //subtype (4) + timestsamp (8) + num_args (1) + version (1) + reserved(2) + 
						   // + param (4) + max args (4x9) + MICRO_ULOG_MAX_MSG_SIZE (128) + maximum size of file name = 58 bytes 

	  /* Message metadata */
	  dwordPtr = (uint32 *)&(dataArray[index]);
	  *dwordPtr++ = MICRO_ULOG_SUBTYPE_DIAG_MSG_EXPANDED;
	  index += 4;
	  
	  /* Timestamp */
	  timestamp = MICRO_ULOG_GETTIME64();
	  *dwordPtr++ = (uint32)timestamp;
	  *dwordPtr++ = (uint32)((uint64)timestamp >> 32);
	  index += 8;
	  
	  /* Num args */
	  bytePtr = (uint8 *)&(dataArray[index]);
	  *bytePtr++ = (uint8)num_args;
	  index += 1;
	  
	  /* Version */
	  *bytePtr++ = (uint8)version;
	  index += 1;

	  /* Reserved 2 bytes */
	  *bytePtr++ = (uint8)version;
	  index += 1;
	  *bytePtr++ = (uint8)version;
	  index += 1;
		   
	  /* Param */
	  if (version == 1)
	  {
		dwordPtr = (uint32 *)&(dataArray[index]);
		*dwordPtr++ = *((uint32*)param);
		index += 4;
	  }
		
	  /* Arguments */
	  dwordPtr = (uint32 *)&(dataArray[index]);
	  for ( i = 0; i < num_args; i++ )
	  {
		*dwordPtr++ = va_arg( ap, uint32 );
		index += 4;
	  }
	  
	  /* Copy line number */
	  wordPtr = (uint16 *)&(dataArray[index]);
	  *wordPtr++ = (uint16)(((msg_v2_const_type *)const_type)->desc.line);
	  index += 2;
			   
	  /* Copy SSID */
	  wordPtr = (uint16 *)&(dataArray[index]);
	  *wordPtr++ = (uint16)(((msg_v2_const_type *)const_type)->desc.ss_id);
	  index += 2;
			   
	  /* Copy ss mask */
	  dwordPtr = (uint32 *)&(dataArray[index]);
	  *dwordPtr++ = (uint32)(((msg_v2_const_type *)const_type)->desc.ss_mask);
	  index += 4;
	  
      /*The arguments are copied in qsr_msg_send* functions*/
      /*Copy the fmt string*/
	  dwordPtr = (uint32 *)&(dataArray[index]);
      memscpy((void *)((char *)(dwordPtr)), fmt_str_len, 
		       (void *)(const_type->msg + filename_len + 1), fmt_str_len);
           
     *((char *)(dwordPtr)+fmt_str_len) = null_terminator;
     log->write += fmt_str_len+1;

     /*Copy the file name*/
	 dwordPtr = (uint32 *)&(dataArray[index]);
     memscpy((void *)((char *)(dwordPtr)), filename_len, 
		      (void *)(const_type->msg), filename_len);
	 
	 /*Copy the null terminator*/
     *((char *)(dwordPtr) + filename_len) = null_terminator;
			
      log->write += filename_len+1;	  
		   
	  ret = log->transportVTable->write( log, total_size,
										 (char *)dataArray, 0, NULL );
	}

 }
 return ret;
}
}

/**
 * <!-- _micro_diagbuffer_debug_msg -->
 *
 * @brief Writes a Diag F3 msg packet to the specified log
 * Example: _micro_diagbuffer_debug_msg(handle, 3, "test %d: 
 * %s", 1, "complete"); 
 *
 * @param log : Handle to the micro_diagbuffer 
 * @param const_type: constant pointer to the string of the msg
 * @param num_args : Number of word-sized parameters in the given va_list
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 * @param ap : A variable list of arguments to the printf
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result _micro_diagbuffer_debug_msg( micro_diagbuffer_handle log,
                                             const msg_const_type* const_type,
                                             uint8 num_args,
                                             uint8 version,
                                             const void* param,
                                             ... )
{
  va_list ap;
  micro_diagbuffer_result ret=0;

  va_start( ap, param );
  /* Narayana: Commented the below line as we need to duplicate the micro_diagbuffer_debug_vmsg_ext api for non QSR2 path) */
 // ret = micro_diagbuffer_debug_vmsg_ext( log, const_type, num_args, version, param, ap );
  va_end( ap );

  return ret;
}

micro_diagbuffer_result _micro_diagbuffer_v2_debug_msg( micro_diagbuffer_handle log,
                                             const msg_v2_const_type* const_type,
                                             uint8 num_args,
                                             uint8 version,
                                             const void* param,
                                             ... )
{
  va_list ap;
  micro_diagbuffer_result ret;

  va_start( ap, param );
  ret = micro_diagbuffer_debug_vmsg_ext( log, const_type, num_args, version, param, ap );
  va_end( ap );

  return ret;
}


/**
 * <!-- _micro_diagbuffer_event -->
 * 
 * @brief Writes a Diag event packet to the specified log
 *
 * @param log : Handle to the micro_diagbuffer
 * @param event_id : Event ID to log, as defined in event_id_enum_type
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result _micro_diagbuffer_event( micro_diagbuffer_handle log,
                                                event_id_enum_type event_id,
                                                uint8 version,
                                                const void* param )
{
  uint32 total_size;
  uint8 param_size = 0;
  uint32 * wordPtr;
  uint16 * dblPtr;
  uint8 * bytePtr;
  uint32 bufsize_mask;
  micro_diagbuffer_result ret = MICRO_DIAGBUFFER_SUCCESS;
  uint32 pad = 0;

  if ( !log || !(log->logStatus & MICRO_ULOG_STATUS_ENABLED) )
  {
    return MICRO_DIAGBUFFER_ERROR;
  }

  if (version == 0)
  {
    /* No parameters */
    param_size = 0;
  }
  else if (version == 1)
  {
    /* Param field is pointer to a uint32 ID */
    param_size = sizeof(uint32);
  }
  else
  {
    /* Unsupported version */
    return MICRO_DIAGBUFFER_ERROR;
  }
  
  bufsize_mask = MICRO_ULOG_BUFSIZE_MASK( log );

  /* fmt + len + TS + event_id + version + reserved 1 byte + param */
  /* sizeof(event_id_enum_type) is 4 bytes, we want to have only event_id field size which is 2 bytes */
  total_size = 2 + 2 + MICRO_ULOG_TIMESTAMP_SIZE + sizeof(event_id_enum_type) + sizeof(uint8) + sizeof(uint8)+ param_size;

  pad = MICRO_ULOG_PADCOUNT( total_size ) - total_size; //add padding at the end of the msg to round it to the next word boundary 
  /*pad should not be greater than 4*/
  if (pad >=sizeof(uint32)) 
  {
    return MICRO_DIAGBUFFER_ERROR;
  }

  if ( micro_ULogInternal_UsesFastWrite( (micro_ULogHandle)log ) )
  {
    uint32 size_idx;
    ret = (micro_diagbuffer_result)micro_ULogInternal_FastFmtSizeTS( (micro_ULogHandle)log, total_size,
                                                                      MICRO_ULOG_SUBTYPE_DIAG_EVENT, &size_idx );
    if ( ret == MICRO_DIAGBUFFER_SUCCESS )
    {
      /* event id */
      dblPtr = (uint16 *)&(log->buffer[log->write & bufsize_mask]);
      *dblPtr = (uint16)event_id;
      log->write += 2;
      
      /* Version */
      bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
      *bytePtr = (uint8)version;
      log->write += 1;
  
      /* Reserved 1 byte */
      bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
      *bytePtr = 0;
      log->write += 1;
          
      /* Param */
      if (version == 1)
      {
        wordPtr = (uint32 *)&(log->buffer[(log->write & bufsize_mask)]);
        *wordPtr = *((uint32 *)param);
        log->write += 4;
      }

      /*add padding at the end of the msg to round it to the next word boundary */
      log->write += pad;

      /* Update the message length */
      wordPtr = (uint32 *)&(log->buffer[size_idx & bufsize_mask]);
      *wordPtr |= ((total_size << MICRO_ULOG_LENGTH_SHIFT) & MICRO_ULOG_LENGTH_MASK);
      /* Release the lock, acquired within FastFmtSizeTS */
      MICRO_ULOG_UNLOCK_LOG( log );
    }
  }
  else
  {
    uint8 index = 0;
    uint64 timestamp;
    uint8 dataArray[60]; //subtype (4) + timestsamp (8) + event_id (2) + version (1) + param (4) = 19 bytes
    
    /* Message metadata */
    wordPtr = (uint32 *)&(dataArray[index]);
    *wordPtr++ = MICRO_ULOG_SUBTYPE_DIAG_EVENT;
    index += 4;
    
    /* Timestamp */
    timestamp = MICRO_ULOG_GETTIME64();
    *wordPtr++ = (uint32)timestamp;
    *wordPtr++ = (uint32)((uint64)timestamp >> 32);
    index += 8;

    /* event id */
    dblPtr = (uint16 *)&(dataArray[index]);
    *dblPtr++ = (uint16)event_id;
    index += 2;
      
    /* Version */
    bytePtr = (uint8 *)&(dataArray[index]);
    *bytePtr++ = (uint8)version;
    index += 1;

    /* Reserved 1 byte */
    bytePtr = (uint8 *)&(dataArray[index]);
    *bytePtr++ = 0;
    index += 1;  
        
    /* Param */
    if (version == 1)
    {
      wordPtr = (uint32 *)&(dataArray[index]);
      *wordPtr++ = *((uint32*)param);
      index += 4;
    }

    ret = log->transportVTable->write( log, total_size,
                                       (char *)dataArray, 0, NULL );
  }
  return ret;
}


/**
 * <!-- _micro_diagbuffer_event_payload -->
 * 
 * @brief Writes a Diag event with payload packet to the specified log
 *
 * @param log : Handle to the micro_diagbuffer.
 * @param event_id : Event ID to log, as defined in event_id_enum_type.
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 * @param length : Length of the associated  payload.
 * @param payload : Pointer to the payload structure. Payload should be published.
                    Please refer to 80-V6196-1 for details.
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result _micro_diagbuffer_event_payload( micro_diagbuffer_handle log,
                                                event_id_enum_type event_id,
                                                uint8 version,
                                                const void* param,
                                                uint8 length,
                                                const void* payload )
{
  uint32 total_size;
  uint8 param_size = 0;
  uint32 * wordPtr;
  uint16 * dblPtr;
  uint8 * bytePtr;
  uint32 bufsize_mask;
  micro_diagbuffer_result ret = MICRO_DIAGBUFFER_SUCCESS;
  uint32 pad =0;

  if ( !log || !(log->logStatus & MICRO_ULOG_STATUS_ENABLED) || (length == 0) || !payload )
  {
    return MICRO_DIAGBUFFER_ERROR;
  }
  
  if (version == 0)
  {
    /* No parameters */
    param_size = 0;
  }
  else if (version == 1)
  {
    /* Param field is pointer to a uint32 ID */
    param_size = sizeof(uint32);
  }
  else
  {
    /* Unsupported version */
    return MICRO_DIAGBUFFER_ERROR;
  }
  
  bufsize_mask = MICRO_ULOG_BUFSIZE_MASK( log );

  /* format + length + TS + event_id + version + reserved 1 byte (Pad) + param  + payload length */
  total_size = 2 + 2 + MICRO_ULOG_TIMESTAMP_SIZE + sizeof(event_id_enum_type) + sizeof(uint8) + sizeof(uint8) + param_size + length;
  pad = MICRO_ULOG_PADCOUNT( total_size ) - total_size; //add padding at the end of the msg to round it to the next word boundary 
  /*pad should not be greater than 4*/
  if (pad >=sizeof(uint32)) 
  {
    return MICRO_DIAGBUFFER_ERROR;
  }

  if( total_size > MICRO_ULOG_MAX_MSG_SIZE )
  {
    return MICRO_DIAGBUFFER_ERROR;
  }
  
  if ( micro_ULogInternal_UsesFastWrite( (micro_ULogHandle)log ) )
  {
    uint32 size_idx;
    ret = (micro_diagbuffer_result)micro_ULogInternal_FastFmtSizeTS( (micro_ULogHandle)log, total_size,
                                                                      MICRO_ULOG_SUBTYPE_DIAG_EVENT_PAYLOAD, &size_idx );
    if ( ret == MICRO_DIAGBUFFER_SUCCESS )
    {
      /* event id */
      dblPtr = (uint16 *)&(log->buffer[log->write & bufsize_mask]);
      *dblPtr = (uint16)event_id;
      log->write += 2;
      
      /* Version */
      bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
      *bytePtr = (uint8)version;
      log->write += 1;
      
      /* reserved byte */
      bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
      *bytePtr = 0;
      log->write += 1;
            
      /* Param */
      if (version == 1)
      {
        wordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
        *wordPtr = *((uint32 *)param);
        log->write += 4;
      }

      /* Payload */
      bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
      if ((((log->write) & (bufsize_mask)) + length) <= (log->bufSize)) 
      {
        memscpy( bytePtr, log->bufSize-(log->write & bufsize_mask), payload, length );
        log->write += length;
      }
      else /* this is wrap around case, where some portion has been written at the end of */
      {    /* the buffer and remaining is being written to in the beginning */
        uint32 first_part_length = log->bufSize - (log->write & bufsize_mask);
        uint32 second_part_length = length - first_part_length;
        memscpy( bytePtr, (log->bufSize - (log->write & bufsize_mask)), payload, first_part_length );
        log->write += first_part_length;
        bytePtr =  (uint8 *)&(log->buffer[log->write & bufsize_mask]);
        memscpy( bytePtr, (log->readWriter & bufsize_mask), ((byte *)payload + first_part_length), second_part_length );
        log->write += second_part_length;
      }
 
      /*add padding at the end of the msg to round it to the next word boundary */
      log->write += pad;
                 
      /* Update the message length */
      wordPtr = (uint32 *)&(log->buffer[size_idx & bufsize_mask]);
      *wordPtr |= ((total_size << MICRO_ULOG_LENGTH_SHIFT) & MICRO_ULOG_LENGTH_MASK);
	  
      /* Release the lock, acquired within FastFmtSizeTS */
      MICRO_ULOG_UNLOCK_LOG( log );
    }
  }
  else
  {
    uint8 index = 0;
    uint64 timestamp;
    uint8 dataArray[MICRO_ULOG_MAX_MSG_SIZE];
           
    /* Message metadata */
    wordPtr = (uint32 *)&(dataArray[index]);
    *wordPtr++ = MICRO_ULOG_SUBTYPE_DIAG_EVENT_PAYLOAD;
    index += 4;
    
    /* Timestamp */
    timestamp = MICRO_ULOG_GETTIME64();
    *wordPtr++ = (uint32)timestamp;
    *wordPtr++ = (uint32)((uint64)timestamp >> 32);
    index += 8;
    
    /* event id */
    dblPtr = (uint16 *)&(dataArray[index]);
    *dblPtr++ = (uint16)event_id;
    index += 2;
      
    /* Version */
    bytePtr = (uint8 *)&(dataArray[index]);
    *bytePtr++ = (uint8)version;
    index += 1;
 
    /* reserved 1 byte */
    bytePtr = (uint8 *)&(dataArray[index]);
    *bytePtr++ = 0;
    index += 1;
      
    /* Param */
    if (version == 1)
    {
      wordPtr = (uint32 *)&(dataArray[index]);
      *wordPtr++ = *((uint32*)param);
      index += 4;
    }

    /* Payload */
    bytePtr = (uint8 *)&(dataArray[index]);
    memscpy( bytePtr, MICRO_ULOG_MAX_MSG_SIZE-index, payload, length );
      
    ret = log->transportVTable->write( log, total_size,
                                       (char *)dataArray, 0, NULL );
  }
  return ret;
}


/**
 * <!-- _micro_diagbuffer_log_submit -->
 * 
 * @brief Writes a Diag log packet to the specified log
 *
 * @param log : Handle to the micro_diagbuffer.
 * @param log_code : Log code for the Diag log packet.
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 * @param length : Length of the associated  payload.
 * @param log_data : Pointer to the log packet data.
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result _micro_diagbuffer_log_submit( micro_diagbuffer_handle log,
                                                log_code_type log_code,
                                                uint8 version,
                                                const void* param,
                                                uint8 length,
                                                const void* log_data )
{
  uint32 total_size;
  uint8 param_size = 0;
  uint32 * wordPtr;
  uint16 * dblPtr;
  uint8 * bytePtr;
  uint32 bufsize_mask;
  micro_diagbuffer_result ret = MICRO_DIAGBUFFER_SUCCESS;
  uint32 pad = 0;
  uint8 * log_data_payload = NULL;
  uint8 log_payload_length = 0;

      
  if ( !log || !(log->logStatus & MICRO_ULOG_STATUS_ENABLED) || (length == 0) || !log_data )
  {
    return MICRO_DIAGBUFFER_ERROR;
  }
  
  if (version == 0)
  {
    /* No parameters */
    param_size = 0;
  }
  else if (version == 1)
  {
    /* Param field is pointer to a uint32 ID */
    param_size = sizeof(uint32);
  }
  else
  {
    /* Unsupported version */
    return MICRO_DIAGBUFFER_ERROR;
  }
  
  bufsize_mask = MICRO_ULOG_BUFSIZE_MASK( log );

  log_data_payload = ((uint8*)log_data)+sizeof(log_hdr_type);
  log_payload_length = length - sizeof(log_hdr_type);

  /* format + length + TS + log_code + version + reserved 1 byte (pad)+ param + log_payload_length */
  total_size = 2 + 2 + MICRO_ULOG_TIMESTAMP_SIZE + sizeof(log_code_type) + sizeof(uint8) + sizeof(uint8) + param_size + log_payload_length;
  pad = MICRO_ULOG_PADCOUNT( total_size ) - total_size; //add padding at the end of the msg to round it to the next word boundary 

  /*pad should not be greater than 4*/
  if (pad >=sizeof(uint32)) 
  {
    return MICRO_DIAGBUFFER_ERROR;
  }

  if( total_size > MICRO_ULOG_MAX_MSG_SIZE )
  {
    return MICRO_DIAGBUFFER_ERROR;
  }
  
  if ( micro_ULogInternal_UsesFastWrite( (micro_ULogHandle)log ) )
  {
    uint32 size_idx;
    ret = (micro_diagbuffer_result)micro_ULogInternal_FastFmtSizeTS( (micro_ULogHandle)log, total_size,
                                                                      MICRO_ULOG_SUBTYPE_DIAG_LOG, &size_idx );
    if ( ret == MICRO_DIAGBUFFER_SUCCESS )
    {
      /* log code */
      dblPtr = (uint16 *)&(log->buffer[log->write & bufsize_mask]);
      *dblPtr = (uint16)log_code;
      log->write += 2;
      
      /* Version */
      bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
      *bytePtr = (uint8)version;
      log->write += 1;
  
      /* Reserved 1 byte */
      bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);
      *bytePtr = 0;
      log->write += 1;
          
      /* Param */
      if (version == 1)
      {
        wordPtr = (uint32 *)&(log->buffer[log->write & bufsize_mask]);
        *wordPtr = *((uint32 *)param);
        log->write += 4;
      }

      /* Log data */
      bytePtr = (uint8 *)&(log->buffer[log->write & bufsize_mask]);

      if ((((log->write) & (bufsize_mask)) + log_payload_length) <= (log->bufSize)) 
      {
        /*if the packet can fit in the buffer*/ 
        memscpy( bytePtr, log->bufSize-(log->write & bufsize_mask), log_data_payload, log_payload_length );
        log->write += log_payload_length;
      }
      else
      { 
        /*wrap around case*/
        uint32 first_part_length = log->bufSize - (log->write & bufsize_mask);
        uint32 second_part_length = log_payload_length - first_part_length;
        memscpy( bytePtr, (log->bufSize - (log->write & bufsize_mask)), log_data_payload, first_part_length );
        log->write += first_part_length;
        bytePtr =  (uint8 *)&(log->buffer[log->write & bufsize_mask]);
        memscpy( bytePtr, (log->readWriter & bufsize_mask), ((byte *)log_data_payload + first_part_length), second_part_length );
        log->write += second_part_length;
      }

      /*add padding at the end of the msg to round it to the next word boundary */
      log->write += pad;
            
      /* Update the message length */
      wordPtr = (uint32 *)&(log->buffer[size_idx & bufsize_mask]);
      *wordPtr |= ((total_size << MICRO_ULOG_LENGTH_SHIFT) & MICRO_ULOG_LENGTH_MASK);
	  
      /* Release the lock, acquired within FastFmtSizeTS */
      MICRO_ULOG_UNLOCK_LOG( log );

    }
  }
  else
  {
    uint8 index = 0;
    uint64 timestamp;
    uint8 dataArray[MICRO_ULOG_MAX_MSG_SIZE];
           
    /* Message metadata */
    wordPtr = (uint32 *)&(dataArray[index]);
    *wordPtr++ = MICRO_ULOG_SUBTYPE_DIAG_LOG;
    index += 4;
    
    /* Timestamp */
    timestamp = MICRO_ULOG_GETTIME64();
    *wordPtr++ = (uint32)timestamp;
    *wordPtr++ = (uint32)((uint64)timestamp >> 32);
    index += 8;
    
    /* log code */
    dblPtr = (uint16 *)&(dataArray[index]);
    *dblPtr++ = (uint16)log_code;
    index += 2;
      
    /* Version */
    bytePtr = (uint8 *)&(dataArray[index]);
    *bytePtr++ = (uint8)version;
    index += 1;
 
    /* Reserved 1 byte */
    bytePtr = (uint8 *)&(dataArray[index]);
    *bytePtr++ = 0;
    index += 1;
         
    /* Param */
    if (version == 1)
    {
      wordPtr = (uint32 *)&(dataArray[index]);
      *wordPtr++ = *((uint32*)param);
      index += 4;
    }

    /* Log data */
    bytePtr = (uint8 *)&(dataArray[index]);
    memscpy( bytePtr, MICRO_ULOG_MAX_MSG_SIZE-index, log_data_payload, log_payload_length );
      
    ret = log->transportVTable->write( log, total_size,
                                       (char *)dataArray, 0, NULL );
  }
  return ret;
}
