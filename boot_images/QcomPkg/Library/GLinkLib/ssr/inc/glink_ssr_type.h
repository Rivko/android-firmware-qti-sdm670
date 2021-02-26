#ifndef GLINK_SSR_TYPE_H
#define GLINK_SSR_TYPE_H

/**
 * @file glink_ssr_type.h
 *
 * Declares the coomon types for the 
 * GLINK SSR component. 
 */

/** \defgroup glink_ssr GLINK SSR
 * \ingroup MPROC
 *
 * Provides types and macros for the GLINK SSR protocol 
 * Server. 
 *
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2014 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/14   rs     Initial revision
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "glink.h"

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/
#define GLINK_SSR_COMMAND_CLEANUP       ( 0 )
#define GLINK_SSR_RESPONSE_ACK          ( 1 )


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

#pragma pack(push, 1)

/** GLINK SSR command header type */
typedef struct 
    glink_ssr_command_header_struct glink_ssr_command_header_type;
struct glink_ssr_command_header_struct
{
  /** Version */
  uint32 version;

  /** Command */
  uint32 command;

  /** Sequence Number */
  uint32 sequence_number;

  /** Length of the channel name */
  uint32 name_len;

  /** Subsystem name */
  char   name[ GLINK_CH_NAME_LEN ];
};

/** GLINK SSR command header type */
typedef struct 
    glink_ssr_response_header_struct glink_ssr_response_header_type;
struct glink_ssr_response_header_struct
{
  /** Version */
  uint32 version;

  /** Response */
  uint32 response;

  /** Sequence Number */
  uint32 sequence_number;
};

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* GLINK_SSR_TYPE_H */
