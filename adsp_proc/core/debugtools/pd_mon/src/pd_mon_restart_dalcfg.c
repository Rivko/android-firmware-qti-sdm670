/** vi: tw=128 ts=3 sw=3 et
@file pd_dump.c
@brief This file contains the API details for the Protection Domain Monitor, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/pd_mon/src/pd_mon_restart_dalcfg.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#include "DALSys.h"
#include "DALPropDef.h"

#include "pd_mon_restart.h"

/**
Initialize PD MON Restart Dal Config and check if PD restart is enabled.
@return
0 - PD restart Disabled.
1 - PD restart Enabled

*/

uint32_t pd_mon_restart_dalcfg_check(void)
{
  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

  if (DAL_SUCCESS == DALSYS_GetDALPropertyHandleStr("pd_mon_restart_dalcfg", pHandle))
  {
     // Check for devcfg PD restart enabled
     if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"pd_mon_restart_enable", 0, &propValue))
     {
        if(1 == propValue.Val.dwVal)
        {
           return 1;
        }
     }
  }
  return 0;
}
