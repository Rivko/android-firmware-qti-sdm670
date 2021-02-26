#ifndef __APR_API_H__
#define __APR_API_H__

/** @file apr_api.h
*/

/*===========================================================================
Copyright (c) 2009-2012 QUALCOMM Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - QUALCOMM Technologies, Inc.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/apr.common/2.0/api/apr_api.h#1 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
===========================================================================*/

#include "apr_comdef.h"

/*****************************************************************************
 * Core Routines                                                             *
 ****************************************************************************/

/** @ingroup apr_core_init_apr
  Initializes the APR infrastructure. 

  @proto
  @code
APR_EXTERNAL int32_t apr_init ( void );
  @endcode

  @detdesc
  This function is called once by each supported protection domain during
  system bringup or at runtime to initialize the APR infrastructure. The APR
  infrastructure must be initialized before any other APR APIs may be called.

  @return
  #APR_EOK when successful.

  @dependencies
  None.
  
  @par Code example
  @code
#include "apr_api.h"

int32_t rc;

rc = apr_init( );
if ( rc )
{
  printf( "Could not initialize the APR service.\n" );
}
  @endcode
*/
APR_EXTERNAL int32_t apr_init ( void );

/** @ingroup apr_core_deinit_apr
  Deinitializes the APR infrastructure.

  @proto
  @code
APR_EXTERNAL int32_t apr_deinit ( void );
  @endcode

  @detdesc
  This function is called once by each supported protection domain during
  system shutdown or at runtime to deinitialize the APR infrastructure. No
  APIs except for apr_init() may be called after the APR infrastructure has
  been deinitialized.
 
  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "apr_api.h"

int32_t rc;

rc = apr_deinit( );
if ( rc )
{
  printf( "Could not de-initialize the APR service.\n" );
}
  @endcode
*/
APR_EXTERNAL int32_t apr_deinit ( void );

/** @ingroup apr_core_apr_call
  Provides a single API entry point into the APR infrastructure.

  @proto
  @code
APR_EXTERNAL int32_t apr_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);
  @endcode

  @param[in]      cmd_id   Command ID to execute.
  @param[in,out]  params   Command parameters.
  @param[out]     size     Size of the command parameters in bytes.
 
  @detdesc
  This function allows the APR infrastructure to expand and deprecate runtime
  functions without breaking forward- and backward-compatible link bindings.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "apr_api.h"

int32_t rc;
char_t ver_buf[31];
uint32_t ver_actual_size;
apr_cmd_get_version_t args;

args.buffer = ver_buf;
args.buf_size = sizeof( ver_buf );
args.ret_size = &ver_actual_size;
rc = apr_call( APR_CMDID_GET_VERSION, &args, sizeof( args ) );

printf( "The local APR version is: %s", ver_buf );
  @endcode
*/
APR_EXTERNAL int32_t apr_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

/*****************************************************************************
 * Core Controls                                                             *
 ****************************************************************************/

/** @addtogroup apr_core_get_version
@{ */
/** Retrieves the local APR infrastructure version string.

  @proto
  @code
static inline int32_t __apr_cmd_get_version (
  char_t* buffer,
  uint32_t buf_size,
  uint32_t* ret_size
);
  @endcode

  @param[in] buffer     Buffer that receives the APR version string.
  @param[in] buf_size   Buffer size, including the NULL character.
  @param[out] ret_size  Number of types written, including the NULL character.

  @detdesc
  This function returns the version of the current APR infrastructure. The
  version allows the client caller to determine infrastructure
  incompatibilities at runtime. The client caller can set the buffer parameter
  to NULL to retrieve the actual buffer size required in the ret_size
  parameter.

  @dependencies
  None.
 
  @return
  #APR_EOK when successful.

  @par Code example 1
  @code
#include "apr_api_inline.h"

int32_t rc;
char_t ver_buf[31];
uint32_t ver_actual_size;

rc = __apr_cmd_get_version( ver_buf, sizeof( ver_buf ), &ver_actual_size );

printf( "The local APR version is: %s", ver_buf );
  @endcode  @newpage

  @par Code example 2
  @code
#include "apr_api.h"

int32_t rc;
char_t ver_buf[31];
uint32_t ver_actual_size;
apr_cmd_get_version_t args;

args.buffer = ver_buf;
args.buf_size = sizeof( ver_buf );
args.ret_size = &ver_actual_size;
rc = apr_call( APR_CMDID_GET_VERSION, &args, sizeof( args ) );

printf( "The local APR version is: %s", ver_buf );
  @endcode
*/
#define APR_CMDID_GET_VERSION ( 0x000110E6 )

typedef struct apr_cmd_get_version_t apr_cmd_get_version_t;

/** Payload of the #APR_CMDID_GET_VERSION command.
*/
struct apr_cmd_get_version_t
{
  char_t* buffer;     /**< Buffer that receives the APR version string. */
  uint32_t buf_size;  /**< Buffer size, including the NULL character. */
  uint32_t* ret_size; /**< Number of types written, including the NULL
                           character. */
};
/**@} */ /* end_addtogroup apr_core_get_version */

#endif /* __APR_API_H__ */
