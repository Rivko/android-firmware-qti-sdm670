#ifndef SYS_M_DRIVER_INTERNAL_H
#define SYS_M_DRIVER_INTERNAL_H
/** vi: tw=128 ts=3 sw=3 et
@file sys_m_driver_internal.h
@brief This file contains the API details for the Protection Domain Accessors, API 1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 - 17 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/sys_m/src/sys_m_driver_internal.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

/**
INTERNAL: Common Module Sizing Information; Do not directly reference as a client user.
This file does not capture any client user API information; a client must use the published
interfaces only.
*/

#include "stdarg.h"
#include "stdlib.h"
#include "stringl/stringl.h"

#include "qurt.h"
#include "qurt_qdi_driver.h"
#include "sys_m_sizes.h"

#include "tms_utils.h"

#if defined(__cplusplus)
extern "C"
{
#endif


typedef enum
{
   SYS_M_DEVICE_STATUS_SUCCESS = 0,
   SYS_M_DEVICE_STATUS_ERROR = -1

} SYS_M_DEVICE_STATUS;

typedef int SYS_M_DEVICE_HANDLE;

typedef enum
{
   SYS_M_DEVICE_PID_0      = 0,
   SYS_M_DEVICE_PID_1      = 1,
   SYS_M_DEVICE_PID_2      = 2,
   SYS_M_DEVICE_PID_3      = 3,
   SYS_M_DEVICE_PID_4      = 4,
   SYS_M_DEVICE_PID_5      = 5,
   SYS_M_DEVICE_PID_6      = 6,
   SYS_M_DEVICE_PID_7      = 7,
   SYS_M_DEVICE_PID_8      = 8,
   SYS_M_DEVICE_PID_9      = 9,
   SYS_M_DEVICE_PID_10     = 10,
   SYS_M_DEVICE_PID_11     = 11,
   SYS_M_DEVICE_PID_12     = 12,
   SYS_M_DEVICE_PID_13     = 13,
   SYS_M_DEVICE_PID_14     = 14,
   SYS_M_DEVICE_PID_15     = 15,
   SYS_M_DEVICE_PID_16     = 16,
   SYS_M_DEVICE_PID_17     = 17,
   SYS_M_DEVICE_PID_18     = 18,
   SYS_M_DEVICE_PID_19     = 19,
   SYS_M_DEVICE_PID_20     = 20,
   SYS_M_DEVICE_PID_21     = 21,
   SYS_M_DEVICE_PID_22     = 22,
   SYS_M_DEVICE_PID_23     = 23,
   SYS_M_DEVICE_PID_24     = 24,
   SYS_M_DEVICE_PID_25     = 25,
   SYS_M_DEVICE_PID_26     = 26,
   SYS_M_DEVICE_PID_27     = 27,
   SYS_M_DEVICE_PID_28     = 28,
   SYS_M_DEVICE_PID_29     = 29,
   SYS_M_DEVICE_PID_30     = 30,
   SYS_M_DEVICE_PID_31     = 31,

   SYS_M_DEVICE_PID_NA     = 32,
   SYS_M_DEVICE_PID_MAX    = SYS_M_DEVICE_PID_NA

} SYS_M_DEVICE_PID;

// Device: LISTEN

#define SYS_M_LISTEN_WORKER_NAME                "smlworker"

#define SYS_M_LISTEN_DEVICE_PATH                "sys_m_listen/"                  /**< Device Prefix */

typedef enum
{
   SYS_M_LISTEN_METHOD_MIN = QDI_PRIVATE,
   SYS_M_LISTEN_METHOD_PING,                                                     /**< Ping Method */
   SYS_M_LISTEN_METHOD_READ,                                                     /**< API Method */
   SYS_M_LISTEN_METHOD_ACK,                                                      /**< API Method */
   SYS_M_LISTEN_METHOD_MAX

} SYS_M_LISTEN_METHOD;

/**
Internal, Listen Initialize
@param[in]     char*                         Initializer Name
@return        SYS_M_DEVICE_HANDLE           Operation Success Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
SYS_M_DEVICE_HANDLE sys_m_listen_init(char const* name);

/**
Internal, Listen Destroy
@param[in]     SYS_M_DEVICE_HANDLE           Opaque Handle
@return        SYS_M_DEVICE_STATUS           Operation Success Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
SYS_M_DEVICE_STATUS sys_m_listen_destroy(SYS_M_DEVICE_HANDLE handle);

/**
Internal, Listen Read
@param[in]     SYS_M_DEVICE_HANDLE           Opaque Handle
@param[in]     char*                         Output Buffer
@param[in]     size_t                        Output Buffer Size
@return        SYS_M_DEVICE_STATUS           Operation Success Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
SYS_M_DEVICE_STATUS sys_m_listen_read(SYS_M_DEVICE_HANDLE handle, char* out_p, size_t out_sz);

/**
Internal, Listen ACK
@param[in]     SYS_M_DEVICE_HANDLE           Opaque Handle
@return        SYS_M_DEVICE_STATUS           Operation Success Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
SYS_M_DEVICE_STATUS sys_m_listen_ack(SYS_M_DEVICE_HANDLE handle);

/**
Internal, Root Driver Initialize
@param[in]     NONE                          None
@return        NONE                          None
*/
void sys_m_listen_root_init(void);

/**
Internal, Root Driver Terminate
@param[in]     NONE                          None
@return        NONE                          None
*/
void sys_m_listen_root_term(void);

/**
Internal, Root Driver Initialize
@param[in]     NONE                          None
@return        NONE                          None
*/
void sys_m_listen_user_init(void);

/**
Internal, User Driver Terminate
@param[in]     NONE                          None
@return        NONE                          None
*/
void sys_m_listen_user_term(void);

#if defined(__cplusplus)
}
#endif

#endif
