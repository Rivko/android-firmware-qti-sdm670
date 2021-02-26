#ifndef QMI_IDL_LIB_TARGET_H
#define QMI_IDL_LIB_TARGET_H
/*===========================================================================

                    Q M I _ I D L _ L I B _ T A R G E T . H

  QuRT OS Specific APIs for encode/decode library.
  
  ---------------------------------------------------------------------------
  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/
#include <setjmp.h>
#include <string.h>
#include "stringl.h"

typedef struct {
  jmp_buf jb;
  int err;
  int v1;
  int v2;
  int v3;
} qmi_idl_lib_exception_type;

#define QMI_IDL_LIB_DEBUG_PRINT(err)

#define ERROR_LABEL __idl_exception

#define QMI_IDL_LIB_TRY(exc) if(!((exc)->err= setjmp((exc)->jb))) 

#define QMI_IDL_LIB_CATCH(exc) else

#define QMI_IDL_HANDLE_ERROR(exc, err_val, value1, value2, value3 ) \
  do { \
    (exc)->v1 = value1; \
    (exc)->v2 = value2; \
    (exc)->v3 = value3; \
    longjmp((exc)->jb, err_val); \
  } while(0)

#define QMI_IDL_LIB_GET_ERROR(exc) ((exc)->err)

#define QMI_IDL_MEM_COPY(dst,dst_size,src,copy_len) memscpy(dst,dst_size,src,copy_len)

#ifdef DLL_API_GLOBAL
#define ENCDEC_EXPORT DLL_API_GLOBAL
#else
#define ENCDEC_EXPORT
#endif

#endif
