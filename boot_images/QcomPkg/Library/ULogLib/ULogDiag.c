/*============================================================================
@file ULogDiag.c

ULog Diagnostic Interface to support reading and controlling ULog logs
via DIAG commands.

Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "ULogBuildFeatures.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ULog.h"
#ifndef DEFEATURE_ULOG_DIAG
#include <stddef.h>
#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "DALSys.h"
#include "ULogDiagFormat.h"
#include "ULogFront.h"


typedef struct external_ulog_handle
{
  struct external_ulog_handle * next;
  ULogHandle internalHandle;
  ULogHandle externalHandle;
} external_ulog_handle;

static external_ulog_handle * ulogDiagExtHeadPtr = NULL;

typedef struct ulog_diag_plugin_handle
{
  struct ulog_diag_plugin_handle * next;
  uint32 plugin_id; 
  int (*plugin_fcn) (uint32 msg_to_plugin);
} ulog_diag_plugin_handle;

static ulog_diag_plugin_handle * ulogDiagPluginHeadPtr = NULL;


/**
 * <!-- GetExternalHandle -->
 *
 * @brief Given an internal handle, create a new handle if we haven't created one already.
 *
 * @return an external handle that we can share with diag and use to validate 
 * future diag requests.
 */
static ULogHandle GetExternalHandle(ULogHandle handle)
{
  external_ulog_handle * idxHandle = ulogDiagExtHeadPtr;
  external_ulog_handle * prevHandle;
  external_ulog_handle * newHandle;

  if (ulogDiagExtHeadPtr == NULL)   //no preexisting list of handles, start one
  {
    DALSYS_Malloc( sizeof(external_ulog_handle), (void **)&newHandle );
    if (newHandle != NULL)
    {
      newHandle->next = NULL;
      newHandle->internalHandle = handle;
      newHandle->externalHandle = (ULogHandle)1;
      ulogDiagExtHeadPtr = newHandle;
      return(newHandle->externalHandle);
    }
  }
  else                      //a list exists, search for a match, create a new handle if match not found
  {
    prevHandle = ulogDiagExtHeadPtr;
    while (idxHandle != NULL)
    {
      if (idxHandle->internalHandle == handle)
      {
        // We found a matching handle.
        return(idxHandle->externalHandle);
      }

      prevHandle = idxHandle;
      idxHandle = idxHandle->next;
    }

    //no matching handle found, create one
	DALSYS_Malloc( sizeof(external_ulog_handle), (void **)&newHandle );
	if (newHandle != NULL)
    {
      newHandle->next = NULL;
      newHandle->internalHandle = handle;
      newHandle->externalHandle = (ULogHandle)((int)prevHandle->externalHandle+1);
      prevHandle->next = newHandle;
      return(newHandle->externalHandle);
    }
  }

  return(NULL);
}


/**
 * <!-- GetInternalHandle -->
 * 
 * @brief Given an external handle, find the corresponding internal handle and return it.
 *
 * @return An internal handle or NULL if the handle was not found. 
 */
static ULogHandle GetInternalHandle(ULogHandle handle)
{
  external_ulog_handle * idxHandle = ulogDiagExtHeadPtr;

  if (handle != NULL)
  {
    while (idxHandle != NULL)
    {
      if (idxHandle->externalHandle == handle)
      {
        return(idxHandle->internalHandle);
      }
      idxHandle = idxHandle->next;
    }
  }
  return(NULL);
}


/**
 * <!-- ULogDiag_RunPlugin -->
 *
 * @brief See if we have a registered plugin for the requested id, run it. 
 *
 * Plugins should mostly return DAL_SUCCESS, or DAL_ERROR.
 * For more complicated return values, values 10000 to 20000 
 * should remain unused by normal ULog return values and 
 * can be returned. The PC application uses ULOG_ERR_NOTCONNECTED
 * to indicate an ID has no plugin attached to it. 							
 *
 * @return the return value, usually DAL_SUCCESS, or ULOG_ERR_NOTCONNECTED
 */
int ULogDiag_RunPlugin(uint32 selected_plugin_id, uint32 msg_to_plugin)
{
  ulog_diag_plugin_handle * idxHandle = ulogDiagPluginHeadPtr;

  if (ulogDiagPluginHeadPtr == NULL)   
  {
    return ULOG_ERR_NOTCONNECTED;
  }

  while (idxHandle != NULL)
  {
    if (idxHandle->plugin_id == selected_plugin_id)  
    {
      // found the id, run the plugin function
      return idxHandle->plugin_fcn(msg_to_plugin);
    }
    idxHandle = idxHandle->next;
  }

  return ULOG_ERR_NOTCONNECTED;
}


/**
 * <!-- ULogDiagAddPlugin -->
 *
 * @brief Setup and enable a ULog Diag plugin.
 * 
 * When the PC based ULogDiagPluginLauncher.exe is run, the plugin with a 
 * matching plugin_id will be run with a 32 bit int passed as an argument 
 * to the plugin. These plugins can be used to create useful analysis tools.
 * For example: when the PC ULogDiagPluginLauncher is run:
 *     ULogDiagPluginLauncher.exe COM12 Modem 1 0
 * The plugin that the NPA team provides as 
 *     ULogDiagAddPlugin(npa_generate_dump_log, ULOG_DIAG_PLUGIN_ID_NPADUMP); 
 * will create a new ULog called "NPA Dump Log" that can be read out with
 * the ULogDiagReader tool.  
 *
 * Plugins should mostly return DAL_SUCCESS, or DAL_ERROR.
 * For more complicated return values, values 10000 to 20000 
 * should remain unused by normal ULog return values and 
 * can be returned. The PC application uses ULOG_ERR_NOTCONNECTED
 * to indicate an ID has no plugin attached to it. 							
 *
 * @param new_pluginfcn : A function pointer to a plugin.  The plugin accepts 
 *                        a uint32 as an input and returns an integer.
 * @param new_plugin_id : A uint32 id that the plugin will be registered with.  
 *
 * @return DAL_SUCCESS if the plugin is registered successfully, an error code if it isn't. 
 */
ULogResult ULogDiagAddPlugin(int(*new_pluginfcn)(uint32), uint32 new_plugin_id){
  ulog_diag_plugin_handle * idxHandle = ulogDiagPluginHeadPtr;
  ulog_diag_plugin_handle * prevHandle;
  ulog_diag_plugin_handle * newHandle;
  
  if (new_pluginfcn==NULL) 
  {
    return ULOG_ERR_INVALIDPARAMETER;
  }

  if (ulogDiagPluginHeadPtr == NULL)   //no preexisting list of handles, start one
  {
    DALSYS_Malloc(sizeof(ulog_diag_plugin_handle), (void **)&newHandle);
    if (newHandle != NULL)
    {
      newHandle->next = NULL;
      newHandle->plugin_id = new_plugin_id;
      newHandle->plugin_fcn = new_pluginfcn;
      ulogDiagPluginHeadPtr = newHandle;
      return(DAL_SUCCESS);
    }
  }
  else                      //a list exists, search for a match, create a new handle if match not found
  {
    prevHandle = ulogDiagPluginHeadPtr;
    while (idxHandle != NULL)
    {
      if (idxHandle->plugin_id == new_plugin_id)  
      {
        // The id is already taken.  Return an error.
        return ULOG_ERR_ALREADYCONNECTED;
      }

      prevHandle = idxHandle;
      idxHandle = idxHandle->next;
    }

    //no matching handle found, create one
    DALSYS_Malloc(sizeof(ulog_diag_plugin_handle), (void **)&newHandle);
    if (newHandle != NULL)
    {
      newHandle->next = NULL;
      newHandle->plugin_id = new_plugin_id;
      newHandle->plugin_fcn = new_pluginfcn;
      prevHandle->next = newHandle;
      return DAL_SUCCESS;
    }
  }
   return DAL_SUCCESS;
}


/**
 * <!-- ULogDiagReadAndFormat -->
 * 
 * @brief Read and format messages until the output buffer is full enough.
 *
 * @return A pointer to response packet.
 */
static PACKED void * ULogDiagReadAndFormat(ulogdiag_cmd_type_read * readCmd)
{
  ulogdiag_cmd_rsp_type_read * readRsp;
  ULogHandle localHandle;

  // If we are outputting in binary format, then the data is read
  // directly to the output buffer.
  // If we are outputting in ascii format, then the data is read to
  // a temporary buffer, and then formatted to the output buffer.
  readRsp = (ulogdiag_cmd_rsp_type_read *) diagpkt_subsys_alloc (DIAG_SUBSYS_ULOG,
                                    (uint16) readCmd->header.subsysCmdCode, 
                                    sizeof(ulogdiag_cmd_rsp_type_read));
  if(readRsp == NULL)
  {
    return (readRsp);
  }

  localHandle = GetInternalHandle(readCmd->handle);
  if (localHandle == NULL)
  {
    return(NULL);
  }
  
  if (readCmd->outputFormat == ULOGDIAG_FORMAT_BINARY)
  {
    readRsp->outputFormat = readCmd->outputFormat;
    if(ULOG_ERR_OVERRUN == 
    ULogCore_Read (localHandle,  
                   sizeof(readRsp->response),  
                   readRsp->response,  
		      &readRsp->outputLength))
    {
	readRsp->outputFormat = ULOGDIAG_FORMAT_BINARY_OVERRUN_DETECTED;
    }
  }
  else
  {
    char * stringStart = (char *)readRsp->response;
    uint32 outputMsgIndex = 0;
    uint32 msgConsumed;
    uint32 oneMsgBuffer[256];
    uint32 readOutputLength = 0;
    uint32 readFlags;
    unsigned baseOffsetBytes = offsetof(ulogdiag_cmd_rsp_type_read, response);

    // Null terminate the output message.
    readRsp->response[0] = 0;
    readRsp->outputLength = 0;

    ULogCore_Query(localHandle,
                   ULOG_VALUE_LOG_READFLAGS,
                   &readFlags);

    if (readFlags & ULOG_RD_FLAG_REWIND)
    {
      // We just had a log rewind.  We must send back a zero length
      // message.
      diagpkt_shorten (readRsp, (baseOffsetBytes + 1)); 
      return(readRsp);
    }

    // While the log is has messages in it, or the output buffer
    // is not full, continue reading from the log
    while (1)
    {
      readRsp->outputFormat = ULOGDIAG_FORMAT_ASCII;
      if(ULOG_ERR_OVERRUN == 
      ULogCore_ReadEx (localHandle,  
                       sizeof(oneMsgBuffer),  
                       (char *)oneMsgBuffer,  
                       &readOutputLength,
			  1))
      {
	readRsp->outputFormat	= ULOGDIAG_FORMAT_ASCII_OVERRUN_DETECTED;
      }

        // Read one message from the log
      if (readOutputLength > 4)
      {

        // Convert it and put it in the output buffer.
        ULogCore_MsgFormat (localHandle,
                            (char *)&oneMsgBuffer[1],  // Skip the status
                            &stringStart[outputMsgIndex], 
                            sizeof(readRsp->response) - outputMsgIndex, 
                            &msgConsumed);

        // Now how full is our output buffer
        outputMsgIndex = strlen(stringStart);

        readRsp->outputLength = outputMsgIndex;

        // If we have filled enough of the output buffer,
        // call it good.  Resize the buffer, and return.
        if ((sizeof(readRsp->response) - outputMsgIndex) < 255)
        {
          // Shorten the message to the size of the actual output.
          //
          diagpkt_shorten (readRsp, (baseOffsetBytes + outputMsgIndex + 1));
          return(readRsp);
        }
      }
      else
      {
        // The log has no data.  This may cause the rewind flag to be set, so
        // let's make sure it is cleared.
        if (readRsp->outputLength == 0)
        {
          ULogCore_Query(localHandle, ULOG_VALUE_LOG_READFLAGS, &readFlags);
        }

        // Shorten the message to the size of the actual output.
        diagpkt_shorten (readRsp, (baseOffsetBytes + outputMsgIndex + 1));
        return(readRsp);
      }
    }
  }
  return(readRsp);
}


/**
 * <!-- ulogdiag_handler -->
 *
 * @brief Performs the command specified in the request packet.
 *
 * @param req : Pointer to request packet
 * @param pkt_len : Length of request packet
 *
 * @return a pointer to response packet.
 */
static PACKED void * ulogdiag_handler( PACKED void *req,  
                                       word pkt_len )
{
  ulogdiag_cmd_rsp_type_version * versionRsp;

  ulogdiag_cmd_type_connect * connectCmd;
  ulogdiag_cmd_rsp_type_connect * connectRsp;

  ulogdiag_cmd_type_allocate * allocateCmd;
  ulogdiag_cmd_rsp_type_allocate * allocateRsp;

  ulogdiag_cmd_type_enable * enableCmd;
  ulogdiag_cmd_rsp_type_enable * enableRsp;

  ulogdiag_cmd_type_list * listCmd;
  ulogdiag_cmd_rsp_type_list * listRsp;

  ulogdiag_cmd_type_read * readCmd;

  ulogdiag_cmd_type_header_read * headerReadCmd;
  ulogdiag_cmd_rsp_type_header_read * headerReadRsp;

  ulogdiag_cmd_type_disable * disableCmd;
  ulogdiag_cmd_rsp_type_disable * disableRsp;

  ulogdiag_cmd_type_settransport * settransportCmd;
  ulogdiag_cmd_rsp_type_settransport *settransportRsp;

  ulogdiag_cmd_type_plugin * pluginCmd;
  ulogdiag_cmd_rsp_type_plugin_cmd * pluginRsp;

  ulogdiag_msg_header * hdr = (ulogdiag_msg_header *)req;
  unsigned baseOffsetBytes;
  ULogHandle tempLog;
  PACKED void * rsp  = NULL;

  ULOGDIAG_CommandsType command_code = ULOGDIAG_CONNECT;

  if (req)
  {
    // Read the command code, and limit it to one byte.
    command_code = (ULOGDIAG_CommandsType) (hdr->subsysCmdCode & 0xFF);

    /*  Verify that the command code is valid */
    {
      switch (command_code)
      {
        case ULOGDIAG_VERSION:
          versionRsp = (ulogdiag_cmd_rsp_type_version *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                           (uint16) hdr->subsysCmdCode, 
                                           sizeof(ulogdiag_cmd_rsp_type_version));
          if (versionRsp == NULL)
          {
            return(NULL);
          }
          versionRsp->majorVersion = 1;
          versionRsp->minorVersion = 0;

          rsp = (void *)versionRsp;
          break;

        case ULOGDIAG_SETTRANSPORT_RAM:
          settransportCmd = (ulogdiag_cmd_type_settransport *)req;
          settransportRsp = (ulogdiag_cmd_rsp_type_settransport *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                           (uint16) hdr->subsysCmdCode, 
                                           sizeof(ulogdiag_cmd_rsp_type_settransport));

          if(NULL == settransportRsp )
          {
            return(NULL);
          }
          settransportRsp->result = ULogCore_SetTransportToRAM(GetInternalHandle(settransportCmd->handle));
          rsp = (void *)settransportRsp;
        break;
		

        case ULOGDIAG_SETTRANSPORT_STM:
          settransportCmd = (ulogdiag_cmd_type_settransport *)req;
          settransportRsp = (ulogdiag_cmd_rsp_type_settransport *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_settransport));

          if(NULL == settransportRsp )
          {
            return(NULL);
          }
          settransportRsp->result = ULogCore_SetTransportToStm(GetInternalHandle(settransportCmd->handle),
                                                               settransportCmd->protocol_num);
          rsp = (void *)settransportRsp;
        break;

        case ULOGDIAG_SETTRANSPORT_STMASCII:
          settransportCmd = (ulogdiag_cmd_type_settransport *)req;
          settransportRsp = (ulogdiag_cmd_rsp_type_settransport *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_settransport));

          if(NULL == settransportRsp )
          {
            return(NULL);
          }
          settransportRsp->result = ULogCore_SetTransportToStmAscii(GetInternalHandle(settransportCmd->handle),
                                                                    settransportCmd->protocol_num);
          rsp = (void *)settransportRsp;
        break;


        case ULOGDIAG_CONNECT:
          connectCmd = (ulogdiag_cmd_type_connect *)req;
          connectRsp = (ulogdiag_cmd_rsp_type_connect *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_connect));
          if (connectRsp == NULL)
          {
             return(NULL);
          }

          // Try to find the matching log.
          tempLog = ULogCore_HandleGet((char*)connectCmd->logName);

          if (tempLog!=NULL){
            // Found the log, get the external handle for it. 
            connectRsp->handle = GetExternalHandle(tempLog);

            if (connectRsp->handle!=NULL){

              // Make sure the ULOG_STATUS_LOG_SW_READ_STARTED hasn't been set so that this 
              // new read session doesn't immediately send an ULOG_ERROR_OVERRUN. 
              ULogCore_ReadSessionComplete(tempLog);

              // Return success, we found the log and have an external handle for them to use. 
              connectRsp->result = DAL_SUCCESS;                 
            }
            else
            {
              // This should be almost impossible unless a malloc getting an external handle fails. 
              connectRsp->result = ULOG_ERR_MISC;
            }
          }
          else
          {
            // Didn't find the log.  Return the correct error code. 
            connectRsp->handle = NULL;
            connectRsp->result = ULOG_ERR_NAMENOTFOUND;
          }
          rsp = (void *)connectRsp;
          break;

        case ULOGDIAG_ALLOCATE:
          allocateCmd = (ulogdiag_cmd_type_allocate *)req;
          allocateRsp = (ulogdiag_cmd_rsp_type_allocate *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_allocate));
          if (allocateRsp == NULL)
          {
            return(NULL);
          }
          allocateRsp->result = ULogCore_Allocate(GetInternalHandle(allocateCmd->handle),
                                                  allocateCmd->bufferSize);
          rsp = (void *)allocateRsp;
          break;

        case ULOGDIAG_ENABLE:
          enableCmd = (ulogdiag_cmd_type_enable *)req;
          enableRsp = (ulogdiag_cmd_rsp_type_enable *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_enable));
          if (enableRsp == NULL)
          {
            return(NULL);
          }
          enableRsp->result = ULogCore_Enable(GetInternalHandle(enableCmd->handle));
          rsp = (void *)enableRsp;
          break;

        case ULOGDIAG_LIST:
          listCmd = (ulogdiag_cmd_type_list *)req;
          listRsp = (ulogdiag_cmd_rsp_type_list *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_list));
          if (listRsp == NULL)
          {
            return(NULL);
          }
          baseOffsetBytes = offsetof(ulogdiag_cmd_rsp_type_list, nameArray);	
          listRsp->result = ULogCore_ListEx(listCmd->logListReadOffset,
                                            &listRsp->registeredCount,  
                                            sizeof(listRsp->nameArray),  
                                            &listRsp->namesReadCount,  
                                            listRsp->nameArray);
          listRsp->offsetCount = listCmd->logListReadOffset;
          // Shorten this allocation based on the number of logs read
          diagpkt_shorten(listRsp, (baseOffsetBytes + 24 * listRsp->namesReadCount));

          rsp = (void *)listRsp;
          break;

        case ULOGDIAG_READ_FORMAT:
          readCmd = (ulogdiag_cmd_type_read *)req;
          rsp = ULogDiagReadAndFormat(readCmd);
          break;

        case ULOGDIAG_HEADER_READ:
          headerReadCmd = (ulogdiag_cmd_type_header_read *)req;
          headerReadRsp = (ulogdiag_cmd_rsp_type_header_read *)diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_header_read));
          if (headerReadRsp == NULL)
          {
            return(NULL);
          }
          headerReadRsp->result = ULogCore_HeaderRead(GetInternalHandle(headerReadCmd->handle),  
                                                       headerReadCmd->headerReadOffset,
                                                       headerReadRsp->headerData,
                                                       sizeof(headerReadRsp->headerData),
                                                       &headerReadRsp->headerLength);
          rsp = (void *)headerReadRsp;
          break;

        case ULOGDIAG_DISABLE:
          disableCmd = (ulogdiag_cmd_type_disable *)req;
          disableRsp = (ulogdiag_cmd_rsp_type_disable *)diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_disable));
          if (disableRsp == NULL)
          {
            return(NULL);
          }
          disableRsp->result = ULogCore_Disable(GetInternalHandle(disableCmd->handle));
          rsp = (void *)disableRsp;
          break;

        case ULOGDIAG_PLUGIN: 
          pluginCmd = (ulogdiag_cmd_type_plugin *) req;
          pluginRsp = (ulogdiag_cmd_rsp_type_plugin_cmd *) diagpkt_subsys_alloc(DIAG_SUBSYS_ULOG,
                                            (uint16) hdr->subsysCmdCode, 
                                            sizeof(ulogdiag_cmd_rsp_type_plugin_cmd));
          if (pluginRsp == NULL)
          {
            return(NULL);
          }
          pluginRsp->result = ULogDiag_RunPlugin(pluginCmd->plugin_id, pluginCmd->msg_to_plugin);
          rsp = (void *)pluginRsp;
		  break;

        default:
          rsp = diagpkt_err_rsp(DIAG_BAD_PARM_F, req, pkt_len);
          break;

      }

      if (rsp)
      {
        /*  Send out our packet */
        diagpkt_commit(rsp);
        rsp = NULL;
      }
    }
  }

  return (rsp);
}


/** Map the diag functions to the proper commands */
static const diagpkt_user_table_entry_type ulogdiag_diag_tbl[] = 
{
  {ULOGDIAG_VERSION | ULOGDIAG_PROCESSOR_ID, 
   ULOGDIAG_MAX_CMD | ULOGDIAG_PROCESSOR_ID, 
   ulogdiag_handler}
};


/**
 * <!-- ULogDiag_Init -->
 * 
 * @brief Register with Diag to have our ulogdiag_handler called.
 */
void ULogDiag_Init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_ULOG, ulogdiag_diag_tbl);
}


/**
 * <!-- ULogBackend_Init -->
 *
 * @brief The diag functionality is setup when this is called. 
 *
 * Called early in a system's boot to initialize any available ULog backend features. 
 */
void ULogBackend_Init(void)
{
  ULogDiag_Init();
}


#else
/**
 * <!-- ULogBackend_Init -->
 *
 * Do nothing if Diag is not enabled. 
 */
void ULogBackend_Init(void)
{
}


/**
 * <!-- ULogDiag_AddPlugin -->
 *
 * @return ULOG_ERR_NOTSUPPORTED if Diag is not enabled. 
 */
ULogResult ULogDiag_AddPlugin( int(*name)(uint32), uint32 ulogdiag_plugin_id )
{
   return ULOG_ERR_NOTSUPPORTED;
}
#endif /* DEFEATURE_ULOG_DIAG */

#ifdef __cplusplus
}
#endif
