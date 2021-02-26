/*=============================================================================

                 err_qdi_dalcfg.c

GENERAL DESCRIPTION
   Implements err QDI dalcfg to fetch some configuration parameters.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2015 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_qdi_dalcfg.c#1 $ 
$DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/21/15   din     File created to support Error on User PD.

=============================================================================*/


/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/

#include "DALSys.h"
#include "DALPropDef.h"

/*=============================================================================

FUNCTION ERR_QDI_DALCFG_GET_PD_TIMEOUT

DESCRIPTION
  Initializes the error qdi layer in root pd with runtime variables. 
  Gets the PD timeout timer in Milliseconds value for PD callback and PD exit. 

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
void err_qdi_dalcfg_get_pd_timeout
( 
  uint32 * pd_timeout_cb, 
  uint32 * pd_timeout_exit
)
{
  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

  DALSYS_GetDALPropertyHandleStr("err_qdi_dalcfg",pHandle);

  /*  Get PD timeout for PD CBs */
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,
                                             "pd_timeout_cb_msec", 0, 
                                             &propValue))
  {
     *pd_timeout_cb = propValue.Val.dwVal;
  }

  /*  Get PD timeout for PD Exits */
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,
                                             "pd_timeout_exit_msec", 0, 
                                             &propValue))
  {
    *pd_timeout_exit = propValue.Val.dwVal;
  }

  
} /* err_qdi_dalcfg_get_pd_timeout */

/*=============================================================================

FUNCTION ERR_QDI_DALCFG_GET_ERR_QDI_PDR_RECORDS

DESCRIPTION
  Initializes the error qdi log layer in root pd with runtime variables. 
  Gets the thrashing threshold in seconds for PDRs and Number of PDRS to 
  be logged.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None.

=============================================================================*/
void err_qdi_dalcfg_get_err_qdi_pdr_records
( 
  uint32 * threshold_timeout,
  uint32  * num_pdr_records  
)
{
  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

  DALSYS_GetDALPropertyHandleStr("err_qdi_dalcfg",pHandle);

  
  /*  Get Threshold timeouts for PDRs */
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,
                                             "threshold_timeout_sec", 0, 
                                             &propValue))
  {
    *threshold_timeout = propValue.Val.dwVal;
  }

  /*  Get Number of PDRs to be logged */
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,
                                             "num_pdr_records", 0, &propValue))
  {
    *num_pdr_records = propValue.Val.dwVal;
  }

} /* err_qdi_dalcfg_get_err_qdi_pdr_records */
