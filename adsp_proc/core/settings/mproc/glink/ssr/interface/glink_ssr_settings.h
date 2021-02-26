#ifndef GLINK_SSR_SETTINGS_H
#define GLINK_SSR_SETTINGS_H

/**
 * @file glink_ssr_settings.h
 *
 * Defines the structure definitions used in G-Link SSR settings
 * 
 */

/** \defgroup glink_ssr GLINK SSR
 * \ingroup MPROC
 *
 * Provides types for the GLINK SSR Settings
 *
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2017 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/settings/mproc/glink/ssr/interface/glink_ssr_settings.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/17   jg      Initial revision; Establish Settings Interface
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "DALStdDef.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
typedef struct _glink_ssr_config_type
{
  const char *host;             /* Local host name  */
  const char *master;           /* Master host name */
  const char *channel;          /* SSR channel name */
  const char *transport;        /* Transport name   */
} glink_ssr_config_type;

#endif   /* GLINK_SSR_SETTINGS_H */
