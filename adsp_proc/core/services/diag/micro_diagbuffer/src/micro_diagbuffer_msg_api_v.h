/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Message Services

General Description
  Internal micro msg apis declarations are present in this file



Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                           Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/micro_diagbuffer_msg_api_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/17   is      Resolve crash with MSG macros in island mode
07/19/17   nk      Qshrink 2 support for uimage diag messages

============================================================================*/
#ifndef MICRO_DIAGBUFFER_MSG_API_V_H
#define MICRO_DIAGBUFFER_MSG_API_V_H
#include "micro_diagbuffer.h"
#define MSG_V2_CHECK_IS_HASH(X) ((((uint32)(X)) & 0x03) != 0)

void micro_msg_send_ext(const msg_const_type *const_blk, uint32 version, void *parameter);

void micro_msg_send_1_ext (const msg_const_type *const_blk, uint32 version,
                        void *parameter, uint32 xx_arg1);

void micro_msg_send_2_ext(const msg_const_type *const_blk, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2);

void micro_msg_send_3_ext (const msg_const_type *const_blk, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3);

void micro_msg_send_var_ext (const msg_const_type *const_blk, uint32 version, void *parameter, uint32 num_args, ...);



void micro_msg_v2_send_ext(const msg_v2_const_type *const_blk, uint32 version, void *parameter);

void micro_msg_v2_send_1_ext(const msg_v2_const_type *const_blk, uint32 version, void *parameter, uint32 xx_arg1);

void micro_msg_v2_send_2_ext(const msg_v2_const_type *const_blk, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2);
					   
void micro_msg_v2_send_3_ext (const msg_v2_const_type *const_blk, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3);

void micro_msg_v2_send_var_ext (const msg_v2_const_type *const_blk, uint32 version, void *parameter, uint32 num_args, ...);

/**
 * <!-- _micro_diagbuffer_v2_debug_msg -->
 *
 * @brief Writes a Diag F3 msg packet to the specified log
 * Example: _micro_diagbuffer_debug_msg(handle, 3, "test %d: 
 * %s", 1, "complete"); 
 *
 * @param log : Handle to the micro_diagbuffer 
 * @param const_type: constant pointer to the string of the msg
 * @param num_args : Number of word-sized parameters in the given va_list
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 * @param ap : A variable list of arguments to the printf
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
 micro_diagbuffer_result _micro_diagbuffer_v2_debug_msg( micro_diagbuffer_handle log,
                                             const msg_v2_const_type* const_type,
                                             uint8 num_args,
                                             uint8 version,
                                             const void* param,
                                             ... );

#endif /*  MICRO_DIAGBUFFER_MSG_API_V_H */
