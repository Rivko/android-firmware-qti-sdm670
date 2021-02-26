/*
#============================================================================
#  Name:
#    procinfo_qdi_client_user.c 
#
#  Description:
#    Process Information QDI Client Process Code that goes to user image 
#
# Copyright (c) 2016 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>

#include "comdef.h"               /* Definitions for byte, word, etc.     */
#include "err.h"
#include "tms_utils.h"
#include "tms_utils_msg.h"

#include "procinfo_qdi_client.h"  /* Include for user PD only */

int32_t procinfo_qdi_client_handle = -1;
static  procinfo_qdi_device_data_t procinfo_client_entry = {0};

/** =====================================================================
 * Function:
 *     procinfo_qdi_invoke_op
 *
 * Description:
 *     Invoke the QDI method to operate on the process information
 *
 * Parameters:
 *     optype           : Type of operation enum value (get/set name)
 *     ntype            : Value of name type from enum
 *     buf              : Source/destination buffer for name
 *     buflen           : Optional length of buffer
 *
 * Returns:
 *     PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *     Refer to the enum PROCINFO_RESULT for a list of possible results 
 * =====================================================================  */
DLL_API_GLOBAL PROCINFO_RESULT procinfo_qdi_invoke_op(uint32_t optype, uint32_t ntype, char *buf, size_t buflen)
{
  int ret = qurt_qdi_handle_invoke(procinfo_qdi_client_handle, PROCINFO_QDI_OP, optype, ntype, buf, buflen);

  /* Populate client internal copy */
  if (PROCINFO_QDI_SUCCESS == ret && PROCINFO_OP_SET_NAME == optype && PROCINFO_OEM_NAME_TYPE == ntype && buf != NULL)
  {
    /* Populate OEM name */
    strlcpy(procinfo_client_entry.oem_name, buf, PROCINFO_QDI_OEM_NAME_LEN);
  }
  
  return ret;
}

/** =====================================================================
 * Function:
 *     procinfo_client_get_name
 *
 * Description: 
 *     Get name for its own process domain
 *
 * Parameters:
 *     ntype            : Value of name type from enum
 *     dest             : Destination buffer to recieve domain name
 *     dest_len         : Length of user buffer
 *
 * Returns:
 *    PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *    Refer to the enum PROCINFO_RESULT for a list of possible results 
 * =====================================================================  */
PROCINFO_RESULT procinfo_client_get_name(uint32_t ntype, char *dest, size_t dest_len)
{
  int ret = PROCINFO_QDI_FAILURE;

  if (PROCINFO_NAME_TYPES < ntype || NULL == dest || 0 == dest_len)
  {
    ret = PROCINFO_QDI_INVALID_PARAMETERS;
    TMS_MSG_ERROR("Incorrect name type/NULL or empty dest buffer");
    return ret;
  }
  
  char *src_type = NULL;
  switch (ntype)
  {
  case PROCINFO_OEM_NAME_TYPE:
    {
      src_type = procinfo_client_entry.oem_name;
      break;
    }

  case PROCINFO_INTERNAL_NAME_TYPE:
    {
      src_type = procinfo_client_entry.internal_name;
      break;
    }

  case PROCINFO_DOMAIN_NAME_TYPE:
    {
      src_type = procinfo_client_entry.domain_name;
      break;
    }
  default:
    {
      TMS_MSG_ERROR("Wrong name type");
      ret = PROCINFO_QDI_NOT_INITIALIZED;
      return ret;
    }
  }

  if ('\0' == src_type[0])
  {
    if (PROCINFO_INTERNAL_NAME_TYPE == ntype)
    {
      /* Fallback to get name from qurt API */
      qurt_thread_context_get_pname(qurt_thread_get_id(), dest, dest_len);
      ret = PROCINFO_QDI_NOT_INITIALIZED;
    }
    /* else */
    /* Requested name not set*/
    return ret;
  }

  /* Local copy name to dest */ 
  if (0 != strlcpy(dest, src_type, dest_len))
  {
    ret = PROCINFO_QDI_SUCCESS;
  }
  else
  {
    TMS_MSG_ERROR("strlcpy failed");
  }

  return ret;
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_invoke_close
 *
 * Description:
 *     Closes the specified driver, releasing any resources associated with the open driver
 *
 * Parameters:
 *     handle : handle to be released
 *
 * Returns:
 *     PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *     Refer to the enum PROCINFO_RESULT for a list of possible results 
 * =====================================================================  */
PROCINFO_RESULT procinfo_qdi_invoke_close(void)
{  
  return qurt_qdi_close(procinfo_qdi_client_handle);
}

/** =====================================================================
 * Function:
 *     procinfo_qdi_client_init
 *
 * Description:
 *     Initialization function in user pd.
 *     Function to obtain a QDI handle given the qdi device name.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
DLL_API_GLOBAL void procinfo_qdi_client_init(void)
{  
  procinfo_qdi_client_handle = qurt_qdi_open(PROCINFO_DRIVER_NAME);
  
  if (0 <= procinfo_qdi_client_handle)
  {    
    /* Populate PID */
    procinfo_client_entry.pid = qurt_process_get_id();

    /* Get internal name from qurt API for local copy */
    qurt_thread_context_get_pname(qurt_thread_get_id(), procinfo_client_entry.internal_name, PROCINFO_QDI_INTERNAL_NAME_LEN);
  }
  else
  {
    ERR_FATAL("pd procinfo_qdi_clientinit failed", 0, 0, 0);
  }
}
