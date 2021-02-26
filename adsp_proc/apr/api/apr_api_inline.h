#ifndef __APR_API_INLINE_H__
#define __APR_API_INLINE_H__

/** @file apr_api_inline.h
*/

/*===========================================================================
Copyright (c) 2010-2012 QUALCOMM Technologies, Inc.
All Rights Reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/apr.common/2.0/api/apr_api_inline.h#1 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Removed Doxygen comments because of revised
                   organization. Comments are in the #defines.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
                   (Merged Doxygen comments from Oct. 2010, and updated new
                   comments).
===========================================================================*/

#include "apr_comdef.h"
#include "apr_api.h"

/*
 * Inline APIs allows forward compatible linking.
 */
/*****************************************************************************
 * Inline Core Controls                                                      *
 ****************************************************************************/

static inline int32_t __apr_cmd_get_version (
  char_t* buffer,
  uint32_t buf_size,
  uint32_t* ret_size
)
{
  apr_cmd_get_version_t args;

  args.buffer = buffer;
  args.buf_size = buf_size;
  args.ret_size = ret_size;

  return apr_call( APR_CMDID_GET_VERSION, &args, sizeof( args ) );
}

#endif /* __APR_API_INLINE_H__ */

