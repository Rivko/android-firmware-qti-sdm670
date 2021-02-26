#ifndef PROCINFO_H
#define PROCINFO_H
/*
#============================================================================
#  Name:
#    procinfo.h 
#
#  Description:
#     Common header file for Process Information server
#
# Copyright (c) 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/

#include "stdlib.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum {
  PROCINFO_OEM_NAME_TYPE          = 0,   /* Only to be given to OEMs */
  PROCINFO_INTERNAL_NAME_TYPE,
  PROCINFO_DOMAIN_NAME_TYPE,
  PROCINFO_NAME_TYPES
} procinfo_nametype;

typedef enum {
  PROCINFO_OP_GET_NAME            = 0,
  PROCINFO_OP_SET_NAME,
  PROCINFO_OP_TYPES
} procinfo_optype;

typedef enum {
  PROCINFO_QDI_SUCCESS            = 0,
  PROCINFO_QDI_FAILURE            = -1,
  PROCINFO_QDI_INVALID_PARAMETERS = -2,
  PROCINFO_QDI_INVALID_OPERATION  = -3,
  PROCINFO_QDI_NOT_INITIALIZED    = -4
} PROCINFO_RESULT;

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
 *    PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *    Refer to the enum PROCINFO_RESULT for a list of possible results
 * =====================================================================  */
PROCINFO_RESULT procinfo_qdi_invoke_op(uint32_t optype, uint32_t ntype, char *buf, size_t buflen);

#if defined(__cplusplus)
}
#endif

#endif /* PROCINFO_H */