/*
#============================================================================
#  Name:
#    procinfo_qdi_client_root.c 
#
#  Description:
#    Process Information QDI Client Process Code that goes to root image 
#
# Copyright (c) 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
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
  return qurt_qdi_handle_invoke(procinfo_qdi_client_handle, PROCINFO_QDI_OP, optype, ntype, buf, buflen);
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
  return qurt_qdi_handle_invoke(procinfo_qdi_client_handle, PROCINFO_QDI_OP, PROCINFO_OP_GET_NAME, ntype, dest, dest_len);
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
 *     Initialization function in root pd.
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
  
  if (0 > procinfo_qdi_client_handle)
  {
    ERR_FATAL("pd procinfo_qdi_clientinit failed", 0, 0, 0);
  }
}
