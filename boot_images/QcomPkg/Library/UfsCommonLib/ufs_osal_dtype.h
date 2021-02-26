#ifndef _UFS_OSAL_DTYPE_H_
#define _UFS_OSAL_DTYPE_H_

/**
@file    UFS_OSAL_Dtype.h
@brief   Universal Flash Storage (UFS) OS adaption layer data type file

This file contains the definitions of the data type used

 Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


when        who   what, where, why
----------  ---   ----------------------------------------------------------- 
09-17-2014  rh    Initial version

=============================================================================*/


/**
 * UEFI redefine types
 */
typedef UINT8                    uint8_t;
typedef UINT16                   uint16_t;
typedef UINT32                   uint32_t;
typedef UINT64                   uint64_t;
typedef INT8                     int8_t;
typedef INT16                    int16_t;
typedef INT32                    int32_t;
typedef INT64                    int64_t;
typedef INTN                     intptr_t;
//typedef UINTN                    uintptr_t;
//typedef UINTN                    size_t;
typedef INTN                     ptrdiff_t;

#ifndef MAX_INT8
#define MAX_INT8                 127
#endif

#ifndef MAX_INT16
#define MAX_INT16                32767
#endif

#ifndef MAX_INT32
#define MAX_INT32                2147483647
#endif

#ifndef MAX_INT64
#define MAX_INT64                9223372036854775807ll
#endif

#ifndef MAX_UINT8
#define MAX_UINT8                255
#endif

#ifndef MAX_UINT16
#define MAX_UINT16               65535
#endif

#ifndef MAX_UINT32
#define MAX_UINT32               4294967295u
#endif

#ifndef MAX_UINT64
#define MAX_UINT64               18446744073709551615ull
#endif


#endif
