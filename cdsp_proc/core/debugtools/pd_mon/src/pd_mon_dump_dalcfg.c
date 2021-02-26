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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/pd_mon/src/pd_mon_dump_dalcfg.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "DALSys.h"
#include "DALPropDef.h"

#include "pd_mon_dump.h"

/**
Initialize PD MON Restart Dal Config and check if PD dump is enabled.
@return
PD_DUMP_DISABLED - PD dump Disabled.
PD_DUMP_ENABLED - PD dump Enabled

*/

uint32_t pd_mon_dump_dalcfg_check_is_enabled(void)
{
  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

  if (DAL_SUCCESS == DALSYS_GetDALPropertyHandleStr("pd_mon_restart_dalcfg", pHandle))
  {
     // Check for devcfg PD dump enabled
     if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"pd_mon_dump_disable", 0, &propValue))
     {
        if(1 == propValue.Val.dwVal)
        {
           return PD_DUMP_DISABLED;
        }
     }
  }
  return PD_DUMP_ENABLED;
}
