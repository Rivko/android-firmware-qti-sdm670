/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostics over STM

General Description
This routines in this file are used to send data over STM using the debugtrace
interfaces.
  
Copyright (c) 2013-14,2016-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diag_stm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/17   sp      Added support for extended logs in rootPD
03/03/17   sp      Removed packed void * parameters
06/01/16   nk      Fixed KW errors.
07/06/14   ph      Added support for Diag over STM
09/20/13   tbg     File creation
===========================================================================*/

#include "comdef.h"
#include "stdlib.h"
#include "tracer.h"

#include "diagdiag_v.h"
#include "diagcmd.h"
#include "diagbuf_v.h"
#include "diagi_v.h"
#include "assert.h"

#include "diag_stm.h"
#include "osal.h"
#include "ULog.h"

#include <stringl/stringl.h>  /* For memscpy */

#ifdef FEATURE_DIAG_STM
/* Safe guard to ensure file is not compiled if feature is not enabled */


/* Local variable declarations */
static boolean diag_stm_initialized = FALSE; // Indicates if diag tracer has been initialized
boolean diag_stm_enabled = FALSE; // Transmit data over STM

boolean diag_pthread_initialized = FALSE; // TLS key initialized for STM. Note: Must be done before diag_stm_enabled can be set to TRUE.
osal_tls_key_t diag_tracer_tls_key; // TLS key for diag tracer handle


   /*  How extended packets are handled when allocated by Diag STM.
       chain field in diagbuf header is updated with DIAGBUF_SECOND_PKT_CHAIN indicating it is
       a chained packet.We allocate the buffer once and reuse it without having to copy the entire
       packet to a new buffer.

       Initial allocation:
       ----------------------------------------------------------------------------------------
       | Extended Header (V1 or V2)  |  Diagbuf_hdr (4 bytes) | Log hdr and log  (N bytes)|
       ----------------------------------------------------------------------------------------

       Prior to sending to STM, Move the diagbuf_hdr to the start of buffer and extended header next to it.
       -----------------------------------------------------------------------------------------
       | Diagbuf_hdr (4 bytes) |      Extended Header (V1 or V2)    | Log hdr and log  (N bytes)|
       -----------------------------------------------------------------------------------------
       We need to ensure to trim off the unnecessary pad to the start of the buffer i.e. Ulog name length
       for the V2 extended logs. Now we send the data from the extended header part with
       appropriate length.
       */



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

#ifdef FEATURE_DIAG_STM
extern void diag_stm_mode_change_notify(int Svc);
#endif
/* Local function prototypes */
#if (defined(DIAG_MP_MASTER)) || (!defined(DIAG_MP_MASTER) && !defined(DIAG_MP))
void* diag_stm_configure_request(void *, uint16);

/* Diag STM Command */
static const diagpkt_user_table_entry_type diag_subsys_tbl_stm[] =
{
  {DIAGDIAG_STM_CONTROL, DIAGDIAG_STM_CONTROL, diag_stm_configure_request}
};

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
#if (defined(DIAG_MP_MASTER)) || (!defined(DIAG_MP_MASTER) && !defined(DIAG_MP))
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_MODEM_PROC, DIAG_SUBSYS_DIAG_SERV, diag_subsys_tbl_stm);
#endif

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

#ifdef FEATURE_DIAG_STRESS_PROFILE
uint64 diag_tracer_pcycles_total = 0;
uint32 diag_tracer_byte_total = 0;
#endif /* FEATURE_DIAG_STRESS_PROFILE */

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
void diag_stm_write_pkt(void* ptr, uint32 length)
{
  tracer_return_enum_t rval;
  static tracer_options_t diag_tracer_options = TRACER_OPT_TIMESTAMP;
  static uint32 diag_tracer_reg_error_cnt = 0;
  static uint32 diag_tracer_write_error_cnt = 0;
  tracer_client_handle_t client_handle;
  boolean write_flag = TRUE;
  int return_val = 0;

  if(( ptr != NULL) && diag_stm_initialized && diag_stm_enabled && (length > 0))
  {
    /* Check if tracer handle has been written into TLS */
    client_handle = osal_pthread_getspecific(diag_tracer_tls_key);
    if(client_handle == NULL)
    {
      write_flag = FALSE;
      /* No handle exists; need to create a handle for this thread */
      rval = tracer_register(&client_handle, TRACER_ENTITY_DIAG, TRACER_PROTOCOL_DIAG);
      if (rval == TRACER_RETURN_SUCCESS)
      {
        return_val = osal_pthread_setspecific(diag_tracer_tls_key, (void*)client_handle);
        ASSERT(return_val == OSAL_SUCCESS);
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
#ifdef FEATURE_DIAG_STRESS_PROFILE
      uint64 pcycle_end, pcycle_start;
      pcycle_start = qurt_get_core_pcycles();
#endif /* FEATURE_DIAG_STRESS_PROFILE */

      rval = tracer_data(client_handle, diag_tracer_options, length, ptr);

      if (rval != TRACER_RETURN_SUCCESS)
      {
        // If write fails, increment debug counter for write failures
        diag_tracer_write_error_cnt++;
      }

#ifdef FEATURE_DIAG_STRESS_PROFILE
      pcycle_end = qurt_get_core_pcycles();
      diag_tracer_pcycles_total += pcycle_end-pcycle_start;
      diag_tracer_byte_total += length;
#endif /* FEATURE_DIAG_STRESS_PROFILE */
    }
  }
}

/*===========================================================================

FUNCTION DIAG_STM_CONTROL_MODE

DESCRIPTION
  Used to enable or disable Diag over STM. Function will only allow it to be
  enabled if diag_stm_initialized is TRUE.
 
PARAMETERS
  mode_info    parameter indicating enable/disable of Diag over STM
               (DIAG_STM_ENABLE or DIAG_STM_DISABLE)
 
DEPENDENCIES
 
RETURN 
  Returns the current state of Diag over STM
    DIAG_STM_DISABLE  Diag STM is currently disabled
    DIAG_STM_ENABLE   Diag STM is currently enabled

===========================================================================*/
uint8 diag_stm_control_mode(uint8 mode_info)
{
  uint8 rval = DIAG_STM_DISABLE;

  if(diag_stm_initialized)
  {
    if (mode_info == DIAG_STM_ENABLE)
    {
      diag_stm_enabled = TRUE;
      rval = DIAG_STM_ENABLE;
#ifdef FEATURE_DIAG_STM
	  diag_stm_mode_change_notify(DIAG_STM_MODE_CHANGE);
#endif
    }
    else if (mode_info == DIAG_STM_DISABLE)
    {
      diag_stm_enabled = FALSE;
      rval = DIAG_STM_DISABLE;
#ifdef FEATURE_DIAG_STM
	  diag_stm_mode_change_notify(DIAG_STM_MODE_CHANGE);
#endif
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid paramater receieved in diag_control_stm_mode");
      if (diag_stm_enabled)
      {
        rval = DIAG_STM_ENABLE;
      }
    }
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_stm_control_mode called but Diag STM is not initialized");
  }
  return rval;
}

#if (defined(DIAG_MP_MASTER)) || (!defined(DIAG_MP_MASTER) && !defined(DIAG_MP))
/*===========================================================================

FUNCTION DIAG_STM_CONFIGURE_REQUEST

DESCRIPTION
  Packet handler for Diag over STM configuration command. For use in modem
  only builds where there is no apps processor.
 
PARAMETERS
  req_pkt     Incoming command packet
  pkt_len     Length of incoming packet
 
DEPENDENCIES
 
RETURN 
  Returns response packet

===========================================================================*/
void * diag_stm_configure_request(
  void * req_pkt,
  uint16 pkt_len
)
{
  diagpkt_subsys_cmd_code_type cmd_code;
  diag_stm_control_rsp_type *rsp = NULL;
  diag_stm_control_req_type *req = (diag_stm_control_req_type *)req_pkt;
  boolean valid_cmd = TRUE;

  if (pkt_len != sizeof (diag_stm_control_req_type) || req_pkt == NULL)
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);
  rsp = (diag_stm_control_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
           cmd_code, sizeof(diag_stm_control_rsp_type));
  
  if(rsp != NULL)
  {	
    if((req->stm_control > DIAG_STM_ENABLE) || (req->version != DIAG_STM_VER_1))
    {
      /* Mark response field invalid if either parameter is out of range */
      valid_cmd = FALSE;
    }

    rsp->proc_mask = req->proc_mask;
    rsp->version = req->version;
    rsp->proc_mask = req->proc_mask;
    /* In this mode, only DIAG_STM_PROC_MASK is supported */
    rsp->is_support_mask = DIAG_STM_PROC_MASK;

    if(!(req->proc_mask & DIAG_STM_PROC_MASK))
    {
      /* This configuration only supports the default processor */
      /* Any other value will return an invalid flag in the response */
      valid_cmd = FALSE;
    }

    if (valid_cmd)
    {
      rsp->stm_status = diag_stm_control_mode(req->stm_control);
    }
    else
    {
      rsp->stm_status = DIAG_STM_DISABLE;
    }

    rsp->valid_cmd = valid_cmd;
  }
  
  return rsp;
} /* diag_stm_configure_request */
#endif

/*===========================================================================

FUNCTION DIAG_STM_LOG_EXT_HANDLER

Function for 'extended' log packets which need the extended header populated
prior to transmission.

In the case a packet is allocated from diagbuf (and not using STM malloc),
An addition copy will be required to assemble the full packet so that it
can be passes to the tracer APIs.

===========================================================================*/
void diag_stm_log_ext_handler(void* pkt_ptr)
{
   uint8* copy_ptr;
   uint8* send_ptr;
  /* pkt_ptr points to the head of the diagbuf_header */
  diagbuf_header_type *diagbuf_hdr2 = (diagbuf_header_type*)pkt_ptr;

  ASSERT(diagbuf_hdr2->version >= DIAGBUF_VERSION_1);

  if(diagbuf_hdr2->version == DIAGBUF_VERSION_1 )
  {
    /* Need to check if this allocated from STM or from diagbuf.*/
    if(diagbuf_hdr2->status == DIAGBUF_STM_BUFFER_S)
    {
      /* STM Buffer allocation, so memory for packet already exists.
        Check with the comments at the top of the file for details.
      */
      diag_cmd_ext_v1_type* ext_pkt_hdr = (diag_cmd_ext_v1_type*)((char*)diagbuf_hdr2 - sizeof(diag_cmd_ext_v1_type));
      diagbuf_header_type orig_diagbuf_hdr;
      diag_cmd_ext_v1_type orig_v1_header;
      uint32 length;

      /* Backup original header details */
      memscpy(&orig_diagbuf_hdr,sizeof(diagbuf_header_type),
                diagbuf_hdr2,sizeof(diagbuf_header_type));

      memscpy(&orig_v1_header,sizeof(diag_cmd_ext_v1_type),
                ext_pkt_hdr,sizeof(diag_cmd_ext_v1_type));

      /* Read original length in pkt_header */
      length = orig_diagbuf_hdr.length ;

      /* Format the data properly i.e. diagbuf header followed by extended header */

      copy_ptr= (uint8 *)ext_pkt_hdr;

      memscpy(copy_ptr, sizeof(diagbuf_header_type),
                &orig_diagbuf_hdr, sizeof(diagbuf_header_type));

      copy_ptr+=sizeof(diagbuf_header_type);

      memscpy((void*)copy_ptr,sizeof(diag_cmd_ext_v1_type),
                (void *)&orig_v1_header,sizeof(diag_cmd_ext_v1_type));

      /* Calculate new length, including the extended header */
      length += sizeof(diag_cmd_ext_v1_type);

      diag_stm_write_pkt(copy_ptr,length);

      /* Need to restore original header so calling function can correctly ID this as an STM allocation */
      memscpy(&diagbuf_hdr2,sizeof(diagbuf_header_type),
            &orig_diagbuf_hdr,sizeof(diagbuf_header_type));

    }
    else
    {
      /* diagbuf buffer. Full packet needs to be populated in a new buffer which can be
       passed to the tracer APIs. */
      void* full_ext_log_pkt;
      diag_cmd_ext_v1_type *ext_hdr;
      uint32 full_length = 0;
      uint8* cpy_ptr = NULL;
      diagbuf_header_type *diagbuf_hdr1;


      /* Calculate length of full packet, and allocate needed memory */
      full_length = diagbuf_hdr2->length + sizeof(diag_cmd_ext_v1_type);
      full_ext_log_pkt = diag_stm_alloc(full_length);

      /* Packet is in two segments currently; Need to reassemble into a single buffer */

      if(full_ext_log_pkt)
      {
        diagbuf_hdr1 = diagbuf_hdr2 - diagbuf_ver_len[DIAGBUF_VERSION_1-1];
        ext_hdr = (diag_cmd_ext_v1_type*)diagbuf_hdr1;

        diagbuf_hdr1 -= 1;

        ext_hdr->cmd_code = DIAG_CMD_EXT_F;
        cpy_ptr = (uint8 *)full_ext_log_pkt;
        memscpy((void*)cpy_ptr,full_length,(void *)ext_hdr,sizeof(diag_cmd_ext_v1_type));
        cpy_ptr += sizeof(diag_cmd_ext_v1_type);

        memscpy((void*)cpy_ptr,full_length-sizeof(diag_cmd_ext_v1_type),
            (void*)(diagbuf_hdr2+1),diagbuf_hdr2->length);

        diag_stm_write_pkt(full_ext_log_pkt,full_length);
        diag_stm_free(full_ext_log_pkt);
      }
    }
  }
  else if(diagbuf_hdr2->version == DIAGBUF_VERSION_2 )
  {
    /* Need to check if this allocated from STM or from diagbuf.*/
    if(diagbuf_hdr2->status == DIAGBUF_STM_BUFFER_S)
    {
      /* STM Buffer allocation, so memory for packet already exists.
         Check at the top of the file for details.
      */
      diag_cmd_ext_v2_type* ext_pkt_hdr = (diag_cmd_ext_v2_type*)((char*)diagbuf_hdr2 - sizeof(diag_cmd_ext_v2_type));
      diagbuf_header_type orig_diagbuf_hdr;
      diag_cmd_ext_v2_type orig_v2_header;
      uint32 ulog_offset_len;
      uint32 pad;
      uint32 length;
      uint32 name_length;

      /* Backup original header details */
      memscpy(&orig_diagbuf_hdr,sizeof(diagbuf_header_type),
                diagbuf_hdr2,sizeof(diagbuf_header_type));

      memscpy(&orig_v2_header,sizeof(diag_cmd_ext_v2_type),
                ext_pkt_hdr,sizeof(diag_cmd_ext_v2_type));

      ulog_offset_len= FPOS(diag_cmd_ext_v2_type, ulog_name);
      name_length= orig_v2_header.name_len;

      /* Read original length in pkt_header */
      length = orig_diagbuf_hdr.length ;

      /* Format the data properly i.e. diagbuf header followed by extended header */
      copy_ptr= (uint8 *)ext_pkt_hdr;

      memscpy(copy_ptr, sizeof(diagbuf_header_type),
                &orig_diagbuf_hdr, sizeof(diagbuf_header_type));

      copy_ptr+=sizeof(diagbuf_header_type);

      pad= sizeof(diag_cmd_ext_v2_type) - (name_length + ulog_offset_len );

      copy_ptr+= pad;

      /*take copy of the copy_ptr since the packet need to be sent from this address*/
      send_ptr= copy_ptr;

      memscpy((void *)copy_ptr,ulog_offset_len,
                (void *)&orig_v2_header, ulog_offset_len);

      copy_ptr+= ulog_offset_len;

      memscpy((void *)copy_ptr,ULOG_MAX_NAME_SIZE,
                (void *)&(orig_v2_header.ulog_name[0]), name_length);

      copy_ptr+= name_length;

      /* Calculate new length, including the extended header */
      length += name_length + ulog_offset_len;

      diag_stm_write_pkt(send_ptr,length);

      /* Need to restore original header so calling function can correctly ID this as an STM allocation */
      memscpy(diagbuf_hdr2,sizeof(diagbuf_header_type),
                &orig_diagbuf_hdr,sizeof(diagbuf_header_type));

    }
    else
    {
      /* diagbuf buffer. Full packet needs to be populated in a new buffer which can be
         passed to the tracer APIs. */
      void* full_ext_log_pkt;
      diag_cmd_ext_v2_type *ext_hdr;
      uint32 full_length = 0;
      uint8* cpy_ptr = NULL;
      diagbuf_header_type *diagbuf_hdr1;

      /* Calculate length of full packet, and allocate needed memory */
      full_length = diagbuf_hdr2->length + sizeof(diag_cmd_ext_v2_type);
      full_ext_log_pkt = diag_stm_alloc(full_length);

      /* Packet is in two segments currently; Need to reassemble into a single buffer */

      if(full_ext_log_pkt)
      {
        diagbuf_hdr1 = diagbuf_hdr2 - diagbuf_ver_len[DIAGBUF_VERSION_2-1];
        ext_hdr = (diag_cmd_ext_v2_type*)diagbuf_hdr1;

        diagbuf_hdr1 -= 1;

        ext_hdr->cmd_code = DIAG_CMD_EXT_F;
        cpy_ptr = (uint8 *)full_ext_log_pkt;
        memscpy((void*)cpy_ptr,full_length,(void *)ext_hdr,sizeof(diag_cmd_ext_v2_type));
        cpy_ptr += sizeof(diag_cmd_ext_v2_type);

        memscpy((void*)cpy_ptr,full_length-sizeof(diag_cmd_ext_v2_type),
                    (void*)(diagbuf_hdr2+1),diagbuf_hdr2->length);

        diag_stm_write_pkt(full_ext_log_pkt,full_length);
        diag_stm_free(full_ext_log_pkt);
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_STM_LOG_ALLOC

DESCRIPTION
  Allocates a log packet using malloc, intended to be transmitted using the
  tracer APIs and then freed. Logs allocated using this API are not intended to
  be processed by the diag task.

  For consistency in other log handling APIs, the method for allocating the logs
  and the headers used are consistent with the standard diagbuf_alloc function.

  The status member of the diagbuf header is set to DIAGBUF_STM_BUFFER_S to
  allow other Diag APIs to know where the buffer was allocated from.

PARAMETERS
  code                 ID of the log to be reported
  length               The specified length is the length of the entire log,
                       including the log header.
  version              Extended log version number.
    					 0 (DIAGBUF_VERSION_0) indicates regular log packet
    					 1 (DIAGBUF_VERSION_1) indicates log packet with extended header (v1)
                         2 (DIAGBUF_VERSION_2) indicates log packet with extended header (v2)
  parameter            Parameter associated with extended packets
DEPENDENCIES

===========================================================================*/
void * diag_stm_log_alloc (uint8 code, uint16 length, uint8 version, void *parameter)
{
  uint32 alloc_length;
  void * ptr = NULL; /* Pointer to return.        */
  diagbuf_header_type *pkt_hdr;

  char temp_buf[ULOG_MAX_NAME_SIZE];
  uint32 name_len=0;

  if(length==0)
  {
    /* Return NULL if an empty log packet is being requested */
    return NULL;
  }

  alloc_length =
    sizeof(diagbuf_header_type) +  /* Diagbuf header    */
      (uint32)length;     /* Number of bytes requested buffer */

  // CRC field not needed
  //     sizeof(uint16)    +   /* CRC -- can we drop this for STM? */

  if(version >DIAGBUF_MAX_VERSION_NUM)
  {
    return NULL;
  }

  switch(version)
  {
    case DIAGBUF_VERSION_0:
    case DIAGBUF_VERSION_1:
    case DIAGBUF_VERSION_2:
    {
      /* Allocate addition length needed for header. See comments at top of file for more details
         on how this is used. */
      if ((version >DIAGBUF_VERSION_0) && (parameter == NULL))
      {
        /* No parameter passed with extended packet */
        return NULL;
      }

      if(version == DIAGBUF_VERSION_1)
      {
        alloc_length += sizeof(diag_cmd_ext_v1_type);
      }
      else if(version == DIAGBUF_VERSION_2)
      {
        alloc_length += sizeof(diag_cmd_ext_v2_type);

        if(ULogCore_NameGet( parameter, &temp_buf[0]) == DAL_SUCCESS)
        {
          /*Match found, get length*/
          name_len= strlen(temp_buf);

          if(name_len > ULOG_MAX_NAME_SIZE )
          {
            ERR_FATAL("Invalid ULog ", 0, 0, 0);
          }
        }
        else
        {
          return NULL;
        }
      }

  ptr = malloc(alloc_length);

  if (ptr)
  {
    /* Set header field of buffer */
    pkt_hdr = ((diagbuf_header_type *) ptr);

        if (version == DIAGBUF_VERSION_0)
        {
          pkt_hdr->length = length;
          pkt_hdr->chain = DIAGBUF_NO_CHAIN;
        }
        else if (version == DIAGBUF_VERSION_1)
        {
          diag_cmd_ext_v1_type* ext_pkt_hdr = (diag_cmd_ext_v1_type*)pkt_hdr;

          /*Populate the header fields*/
          ext_pkt_hdr->cmd_code= DIAG_CMD_EXT_F;
          ext_pkt_hdr->version= DIAGBUF_VERSION_1;
          ext_pkt_hdr->proc_id=DIAG_MY_PROC_ID;
          ext_pkt_hdr->id = *((uint32*)parameter);

          pkt_hdr= (diagbuf_header_type *)((byte*)ext_pkt_hdr + sizeof(diag_cmd_ext_v1_type));

          pkt_hdr->chain = DIAGBUF_SECOND_PKT_CHAIN;
          pkt_hdr->length = length;
        }
        else
        {
          diag_cmd_ext_v2_type* ext_pkt_hdr = (diag_cmd_ext_v2_type*)pkt_hdr;

          /*Populate the header fields*/
          ext_pkt_hdr->cmd_code= DIAG_CMD_EXT_F;
          ext_pkt_hdr->version= DIAGBUF_VERSION_2;
          ext_pkt_hdr->proc_id=DIAG_MY_PROC_ID;
          /*Do we need to + 1 for NULL termination ? */
          ext_pkt_hdr->name_len = name_len;
          ext_pkt_hdr->time_offset_type = 0xFF;
          ext_pkt_hdr->time_offset = 0;

          /*Copy the ULOG name*/
          strlcpy(ext_pkt_hdr->ulog_name, temp_buf, name_len );

          pkt_hdr= (diagbuf_header_type *)((byte*)ext_pkt_hdr + sizeof(diag_cmd_ext_v2_type));
          pkt_hdr->chain = DIAGBUF_SECOND_PKT_CHAIN;
          pkt_hdr->length = length;
        }

    pkt_hdr->status = DIAGBUF_STM_BUFFER_S;
        /* Note: pkt_hdr->pad not needed for STM implementation, since this allocation is not
       going into diagbuf */
        pkt_hdr->version = version;

    /* Set 'ptr' to pkt_hdr +1 = data following pkt_hdr */
    ptr = pkt_hdr + 1;

    diagpkt_set_cmd_code (ptr, code);
  }
      break;
    }
    default:
      break;
  }

  return ptr;
} /* diag_stm_log_alloc */


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
  return ptr;
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

FUNCTION DIAG_STM_LOG_FREE

DESCRIPTION
  Frees a buffer allocated using diag_stm_log_alloc (accounts for diagbuf header)

PARAMETERS
  ptr      Pointer to free

DEPENDENCIES

===========================================================================*/
void diag_stm_log_free(void *ptr)
{
  diagbuf_header_type *pkt_hdr;

  if(ptr)
  {
    pkt_hdr = ((diagbuf_header_type *) ptr) - 1;

    /* Check if this is an extended header */
    if (pkt_hdr->chain == DIAGBUF_SECOND_PKT_CHAIN)
    {
      char* head_ptr = (char*)pkt_hdr;

      /* Only version 1 supported currently */
      ASSERT( (pkt_hdr->version = DIAGBUF_VERSION_1) || (pkt_hdr->version = DIAGBUF_VERSION_2));

      if(pkt_hdr->version == DIAGBUF_VERSION_1)
      {
        head_ptr -= sizeof(diag_cmd_ext_v1_type);
      }
      else if (pkt_hdr->version == DIAGBUF_VERSION_2)
      {
        head_ptr -= sizeof(diag_cmd_ext_v1_type);
      }

      free(head_ptr);
    }
    else
    {
      free(pkt_hdr);
    }
  }
}

/*===========================================================================

FUNCTION DIAG_STM_GET_MODE_DCM 

DESCRIPTION
  Populates the output buffer with the STM mode information and returns TRUE
  if it is able to write the mode.
  
 
PARAMETERS 
  out_buf_ptr      	Pointer to the buffer where STM mode is to be written.
  outbuf_max_bytes 	Maximum bytes that can be written.
  pbytes_written	Pointer to hold the info of number of bytes written.
 
DEPENDENCIES
	None

===========================================================================*/
boolean diag_stm_get_mode_dcm(byte* out_buf_ptr, unsigned long outbuf_max_bytes, unsigned long* pbytes_written) 
{
	boolean success = FALSE;
	if( (out_buf_ptr) && (outbuf_max_bytes == sizeof(diag_stm_enabled) ) && (pbytes_written))
	{
		*((byte*)out_buf_ptr) = diag_stm_enabled;
		*pbytes_written = sizeof(diag_stm_enabled);
		success = TRUE;		
	}
	else
	{
		MSG(MSG_SSID_DIAG,MSG_LEGACY_ERROR, "diag_stm.c error:LSM request for STM failed \n");
	}
	return success;
}

#endif /* FEATURE_DIAG_STM */
