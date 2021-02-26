/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * adsppm_types.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef ADSPPM_TYPES_H_
#define ADSPPM_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


#ifndef NULL
#define NULL  0
#endif

#ifndef _BOOLEAN_DEFINED
typedef unsigned char       boolean; //!< Boolean value type
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef unsigned long int   uint32;  //!< Unsigned 32 bit value
#define _UINT32_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef signed long int     int32;   //!< Signed 32 bit value
#define _INT32_DEFINED
#endif

#ifndef _UINT64_DEFINED
typedef unsigned long long  uint64;  //!< Unsigned 64 bit value
#define _UINT64_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef signed long long    int64;   //!< Signed 64 bit value
#define _INT64_DEFINED
#endif

#ifndef _BYTE_DEFINED
typedef unsigned char       byte;    //!< Unsigned 8  bit value type
#define _BYTE_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef signed char         int8;    //!< Signed 8  bit value
#define _INT8_DEFINED
#endif

#ifndef _BOOL32_DEFINED
typedef unsigned long int   bool32;  //!< unsigned long boolean value
#define _BOOL32_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef unsigned char       uint8;   //!< Unsigned 8  bit value
#define _UINT8_DEFINED
#endif


#ifdef __cplusplus
}
#endif //__cplusplus


#endif /* ADSPPM_TYPES_H_ */

