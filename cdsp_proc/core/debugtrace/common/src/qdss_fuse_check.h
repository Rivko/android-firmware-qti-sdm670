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
   setting for invasive debugging overrides settings for non-invasive debugging
   regardless of the actual setting for non-invasive fuse.

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
 $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/common/src/qdss_fuse_check.h#1 $
==========================================================================*/

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Bit masks for trace access modes.
typedef enum {
   QDSS_TRACE_ACCESS_MODE_NONE           = 0x00,
      // Trace access is not possible in any mode.
   QDSS_TRACE_ACCESS_MODE_NONSECURE_MODE = 0x01,
      // Trace access is possible in non-secure mode only.
   QDSS_TRACE_ACCESS_MODE_SECURE_MODE    = 0x02,
      // Trace access is possible in secure mode only.
   QDSS_TRACE_ACCESS_MODE_ALL            = 0x03,
      //Trace access is possible under both secure and non-secure mode.
   QDSS_TRACE_ACCESS_MODE_8BITS          = 0x7F
      // Trace access is 8bits mode.
} qdss_trace_access_mode_enum_t;


#define QDSS_TRACE_ACCESS_DENIED   0  // Fuse(s) blown, do not permit trace access
#define QDSS_TRACE_ACCESS_ALLOWED  1  // Fuse(s) intact, trace access is permitted

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Description:
   Checks the efuse associated with QDSS and returns whether
   trace access is possible or not regardless of the security mode of
   debugging (NS=0/1). The function checks the following OEM fuses:
   DAP_SPNIDEN_DISABLE, DAP_SPIDEN_DISABLE,
   DAP_NIDEN_DISABLE, DAP_DBGEN_DISABLE,
   DAP_DEVICEEN_DISABLE, ALL_DEBUG_DISABLE (these 2 not for RPM image)

Return:  QDSS_TRACE_ACCESS_DENIED
         QDSS_TRACE_ACCESS_ALLOWED
*/
int qdss_fuse_trace_access(void);

