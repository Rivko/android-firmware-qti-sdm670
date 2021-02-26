/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  F3 Trace DAL Device Configuration Initialization
                
GENERAL DESCRIPTION
  Contains handlers to support F3 message trace retrieval from the EFS.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be called after any initialization routines with higher precedence
  (eg post-compilation) such as NV.

Copyright (c) 2012-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULEW
$Header: //components/rel/core.qdsp6/2.1/services/diag/f3_trace/src/diag_f3_trace_devcfg.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
01/25/13   rh      Transferred file ownership and location to Diag 
07/27/12   mcg     Initial file creation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msg.h"
#include "diag_f3_tracei.h"
#include "diag_f3_trace_devcfg.h"
#include "DALPropDef.h"
#include "DALSys.h"

 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/



/*===========================================================================

FUNCTION diag_f3_trace_devcfg_init

DESCRIPTION
  Initializes f3 trace configuration based on devcfg settings
  -note that diag_f3_trace can be configured through other mechanisms (eg NV)
   which may have higher precedence. 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void diag_f3_trace_devcfg_init(void)
{
  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

  DALSYS_GetDALPropertyHandleStr("diag_f3_trace",pHandle);

  uint32 fdi_control, fdi_detail;

  // Check for diag_f3_trace_control
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"diag_f3_trace_control", 0, &propValue))
  {
    //abort if disabled
    if(0 == (propValue.Val.dwVal & DIAG_F3_TRACE_ENABLE))
    {
      //disable configuration
      diag_f3_trace_init_configs (0, 0);
      return;
    }
    else
    {
      fdi_control = propValue.Val.dwVal;
    }
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Could not read diag_f3_trace_control devcfg");
    return;
  }

  // diag_f3_trace enabled, read diag_f3_trace_detail
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"diag_f3_trace_detail", 0, &propValue))
  {
    fdi_detail = propValue.Val.dwVal;
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Could not read diag_f3_trace_detail devcfg");
    return;
  }

  //good to initiate main f3 trace module
  diag_f3_trace_init_configs (fdi_control, fdi_detail);
}


