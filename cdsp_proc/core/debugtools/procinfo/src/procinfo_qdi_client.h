#ifndef PROCINFO_QDI_CLIENT_H
#define PROCINFO_QDI_CLIENT_H
/*
#============================================================================
#  Name:
#    procinfo_qdi_client.c 
#
#  Description:
#    Process Information QDI Client Code for user pd images only
#
# Copyright (c) 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdlib.h"
#include "procinfo.h"
#include "procinfo_qdi.h"
#include "tms_dll_api.h"

#if defined(__cplusplus)
extern "C"
{
#endif

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
 *     PROCINFO_QDI_SUCCESS(0) or PROCINFO_QDI_FAILURE(-1)
 *     Refer to the enum PROCINFO_RESULT for a list of possible results 
 * =====================================================================  */
PROCINFO_RESULT procinfo_qdi_invoke_close(void);

/** =====================================================================
 * Function:
 *     procinfo_qdi_client_init
 *
 * Description:
 *     Initialization function in user/root pd.
 *     Function to obtain a QDI handle given the qdi device name.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void procinfo_qdi_client_init(void);

#if defined(__cplusplus)
}
#endif

#endif