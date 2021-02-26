#ifndef GLINK_SSR_CONFIG_H
#define GLINK_SSR_CONFIG_H

/**
 * @file glink_ssr_config.h
 *
 * Declares the coomon functions and macros for the 
 * GLINK SSR component. 
 */

/** \defgroup glink_ssr GLINK SSR
 * \ingroup MPROC
 *
 * Provides functions, types and macros for the GLINK SSR 
 * component for subsystems.
 *
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2014 - 2015, 2017 Qualcomm Technologies, Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/glink/ssr/inc/glink_ssr_config.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     Establish settings interface in core/settings directory
07/01/15   bc      Remove unnecessary header
10/22/14   rs      Initial revision
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "glink.h"
#include "glink_ssr_settings.h"
#include "glink_internal.h"

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC VARIABLE DECLARATIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  glink_ssr_config_get
  ===========================================================================*/
/**
  This function initializes and returns configuration.

  @param  None.

  @return   Configuration.
*/
/*=========================================================================*/
const glink_ssr_config_type *glink_ssr_config_get(void);

#endif   /* GLINK_SSR_CONFIG_H */
