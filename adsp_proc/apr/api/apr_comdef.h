#ifndef __APR_COMDEF_H__
#define __APR_COMDEF_H__

/** @file apr_comdef.h
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

  $Header: //components/rel/apr.common/2.0/api/apr_comdef.h#1 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
                   (Merged Doxygen comments from Oct. 2010, and updated new
                   comments).
===========================================================================*/

#include "mmdefs.h"

/** @addtogroup apr_core_constants_macros
@{ */

/****************************************************************************/
/** @name APR Core Export Macros
@{ */
/****************************************************************************/

#ifdef __cplusplus
  #define APR_INTERNAL extern "C"
    /**< Export macro that indicates an internal function, which is not
         intended for public use. */
#else
  #define APR_INTERNAL extern
    /**< Export macro that indicates an internal function, which is not
         intended for public use. */
#endif /* __cplusplus */


#ifdef __cplusplus
  #define APR_EXTERNAL extern "C"
    /**< Export macro that indicates an external function, which is intended
         for public use. */
#else
  #define APR_EXTERNAL extern
    /**< Export macro that indicates an external function, which is intended
         for public use. */
#endif /* __cplusplus */

/** @} */ /* end_name APR Core Export Macros */

/****************************************************************************/
/** @name APR Core Value Definitions
@{ */
/****************************************************************************/

#define APR_UNDEFINED_ID_V ( ( uint32_t ) 0xFFFFFFFF )
  /**< Undefined value for where a valid GUID is expected. */

#define APR_NULL_V ( 0 )
  /**< NULL handle is defined to be 0. */

/** @} */ /* end_name APR Core Value Definitions */

/****************************************************************************/
/** @name APR Core Bitmask Macros
@{ */
/****************************************************************************/

#define APR_GET_BITMASK( mask, shift, value ) \
  ( ( ( value ) & ( mask ) ) >> ( shift ) )
  /**< Gets the value of a field, including the specified mask and shift. */

#define APR_SET_BITMASK( mask, shift, value ) \
  ( ( ( value ) << ( shift ) ) & ( mask ) )
  /**< Sets a value in a field with a specified mask and shift. */

#define APR_GET_FIELD( field, value ) \
  APR_GET_BITMASK( ( field##_MASK ), ( field##_SHFT ), ( value ) )
  /**< Gets the value of a field. */

#define APR_SET_FIELD( field, value ) \
  APR_SET_BITMASK( ( field##_MASK ), ( field##_SHFT ), ( value ) )
  /**< Sets a value in a field. */

/** @} */ /* end_name APR Core Bitmask Macros */

/****************************************************************************/
/** @name APR Core Accessor Macros
@{ */
/****************************************************************************/

#define APR_PTR_END_OF( base_ptr, offset ) \
  ( ( ( uint8_t* ) base_ptr ) + ( offset ) )
  /**< Returns an 8-bit aligned pointer to a base address pointer plus an
       offset, in bytes. */

/** @} */ /* end_name APR Core Accessor Macros */
/** @} */ /* end_addtogroup apr_core_constants_macros */

/****************************************************************************/
/* Compiler-specific Macros
 */
/****************************************************************************/

#ifndef inline
  #if defined( __ARMCC_VERSION )
    #define inline __inline
  #elif defined( _MSC_VER )
    #define inline __inline
  #endif
#endif /* inline */

#endif /* __APR_COMDEF_H__ */

