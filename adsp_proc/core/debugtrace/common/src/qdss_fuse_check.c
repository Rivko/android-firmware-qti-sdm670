/*=========================================================================
                       QDSS e-fuse check

GENERAL DESCRIPTION
   The interface to check efuses associated with the QDSS for accessing its
   trace functionality is implemented. The fuses that are checked include
   DBGEN, SPIDEN, NIDEN and SPNIDEN. The functionality controlled by the fuses
   are as follows. DBGEN controls non-secure invasive debugging, SPIDEN controls
   secured invasive debugging, NIDEN controls non-secure non-invasive debugging
   and SPNIDEN controls secure non-invasive debugging.

   According to the ARM Coresight architecture, in all cases, the fuse
   setting for invasive debugging overrides settings for non-invasive debugging.


EXTERNALIZED FUNCTIONS
   qdss_fuse_trace_access

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Requires System FPB clock to access the efuse registers.

==========================================================================*/
/*=============================================================================
  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*==========================================================================
 $Header: //components/rel/core.qdsp6/2.1/debugtrace/common/src/qdss_fuse_check.c#1 $
==========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "qdss_fuse_check.h"
#include "halhwio_qdss.h"
#include "qdss_fuse_regs.h"

/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/

/*
Input: None

Description:
   Returns the accessibility of of QDSS peripheral for in non-secure mode for
   non-invasive debugging such as use of STM for software instrumented tracing,
   ETM and PFT. The function checks the fuse configuration of DBGEN and NIDEN.
   If DBGEN is enabled, non-invasive debugging is also enabled regardless of the
   configuration of NIDEN.

Return:
   1 - Trace is permitted in non-secure mode.
   0 - Trace is not permitted in non-secure mode.
*/

int qdss_fuse_trace_access_non_secure(void)
{
   unsigned int uNIDEN_DISABLE;
   unsigned int uDBGGEN_DISABLE;

   uNIDEN_DISABLE = HWIO_INF(OEM_CONFIG1, DAP_NIDEN_DISABLE);
   uDBGGEN_DISABLE = HWIO_INF(OEM_CONFIG1, DAP_DBGEN_DISABLE);
   return ((!uDBGGEN_DISABLE) || (!uNIDEN_DISABLE)) ? 1 : 0;
}


/*
Input: None

Description:
   Returns the accessibility of of QDSS peripheral in secure mode for
   non-invasive debugging such as use of STM for software instrumented tracing,
   ETM and PFT. The function checks the fuse configuration of SPIDEN and SPNIDEN.
   If SPIDEN is enabled, non-invasive debugging is also enabled regardless of the
   configuration of SPNIDEN.

Return:
   1 - Trace is permitted in secure mode.
   0 - Trace is not permitted in secure mode.
*/

int qdss_fuse_trace_access_secure(void)
{
   unsigned int uSPIDEN_DISABLE;
   unsigned int uSPNIDEN_DISABLE;

   uSPNIDEN_DISABLE = HWIO_INF(OEM_CONFIG1, DAP_SPNIDEN_DISABLE);
   uSPIDEN_DISABLE  = HWIO_INF(OEM_CONFIG1, DAP_SPIDEN_DISABLE);
   return ((!uSPIDEN_DISABLE) || (!uSPNIDEN_DISABLE)) ? 1 : 0;
}

/*---------------------------------------------------------------------------
Description:
   The specific security mode in which trace hardware is accessible is
   returned by this function. Accesses are guarded by efuse settings.

Return: QDSS_TRACE_ACCESS_MODE_...
        See qdss_trace_access_mode_enum.
*/
qdss_trace_access_mode_enum_t qdss_fuse_trace_access_mode(void)
{
   int val = 0;
   val = (qdss_fuse_trace_access_secure() << 1) |
          qdss_fuse_trace_access_non_secure();
   return (qdss_trace_access_mode_enum_t) val;
}

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
int qdss_fuse_trace_access(void)
{
   if (QDSS_TRACE_ACCESS_MODE_ALL == qdss_fuse_trace_access_mode())
   {
      return QDSS_TRACE_ACCESS_ALLOWED;
   }
   else
   {
      return QDSS_TRACE_ACCESS_DENIED;
   }
}
