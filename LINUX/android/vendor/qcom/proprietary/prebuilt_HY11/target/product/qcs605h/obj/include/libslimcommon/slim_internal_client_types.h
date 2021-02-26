#ifndef SLIM_INTERNAL_CLIENT_TYPES_H
#define SLIM_INTERNAL_CLIENT_TYPES_H
/**
  @file
  @brief Data types for internal clients.

  SLIM internal API header containing the client types needed for internal
  SLIM API use.
  @ingroup slim_CoreClientApi
*/

/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/common/core/inc/slim_internal_client_types.h#1 $ */

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "slim_client_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  @addtogroup slim_CoreClientApi
  @{
*/
/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/

/**
@name Public Type Declarations
@{
*/

/*! Context structure for making asynchronous SLIM calls */
typedef struct
{
  slimClientHandleT           p_Handle;
  /**< SLIM client handle used for opening connection */
  slimNotifyCallbackFunctionT fn_Callback;
  /**< User-provided callback function */
  uint64_t                    t_CallbackData;
  /**< User-provided cookie */
} slimClientContextT;

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SLIM_INTERNAL_CLIENT_TYPES_H */
