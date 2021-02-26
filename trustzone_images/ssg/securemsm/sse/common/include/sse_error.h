/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef _SSE_ERROR_H
#define _SSE_ERROR_H

#define SSE_ERROR_COMPONENT_MASK  0xFFFF0000
#define SSE_ERROR_CODE_MASK       0x0000FFFF

#define SSE_ERROR_CREATE(component,code)  (((component & 0x0000FFFF) << 16) | (code & 0x0000FFFF))

#define SSE_OK                            0x00000000

#define E_SSE_NOT_IMPLEMENTED             0x00000001
#define E_SSE_UNKNOWN                     0x00000002
#define E_SSE_UNKNOWN_FUNCTION            0x00000004
#define E_SSE_INVALID_INPUT               0x00000005    /**< Input data is invalid. */
#define E_SSE_INVALID_RANGE               0x00000006    /**< If address/pointer. */
#define E_SSE_BUFFER_TOO_SMALL            0x00000007    /**< A buffer is too small. */
#define E_SSE_INVALID_TIMEOUT             0x00000008    /**< The chosen timeout value was invalid. */
#define E_SSE_TIMEOUT                     0x00000009    /**< Timeout expired. */
#define E_SSE_NULL_POINTER                0x0000000a    /**< Null pointer. */
#define E_SSE_INVALID_PARAMETER           0x0000000b    /**< Invalid parameter. */
#define E_SSE_PANIC                       0x0000000c    /**< Panic condition, restart TrustLet */
#define E_SSE_MESSAGING                   0x0000000d    /**< Error in the messaging mechanism */

#endif //_SSE_ERROR_H

