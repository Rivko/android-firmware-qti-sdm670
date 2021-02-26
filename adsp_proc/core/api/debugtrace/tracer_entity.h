#ifndef TRACER_ENTITY_H
#define TRACER_ENTITY_H
/**
  @file tracer_entity.h
  @brief Defined entity identifiers for encapsulation of output stream and
         used by OST base protocol.
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.

      The Tracer.h_Debug_Tool_API_mainpage_mainpage.dox file contains the
      group/module descriptions that are displayed in the output PDF generated
      using Doxygen and LaTeX. To edit or update any of the group/module text
      in the PDF, edit the _mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*=============================================================================
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.qdsp6/2.1/api/debugtrace/tracer_entity.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/12/13   llg    (Tech Pubs) Edited/added Doxygen comments and markup.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/** @ingroup tracer_entity_h_constants
  OST base layer entity values are unique per target system.
*/
#undef DEFINE_ENTITY
#define DEFINE_ENTITY(txt, val, cmt) TRACER_ENTITY_##txt=val, cmt
/** @ingroup tracer_entity_h_data_types
  Enumeration for the Tracer OST entity ID.
*/
typedef enum
{
   #include "tracer_entity_defn.h"

   // 200-239 Reserved for assignment to 3rd parties.

   TRACER_ENTITY_MGMT     = 240, /**< OST system level manager. */
   // 241 to 254 Reserved for future OST use.
   TRACER_ENTITY_ALL      = 255  /**< Broadcast to all entities. */
} tracer_ost_entity_id_enum_t;

/** @ingroup tracer_entity_h_constants
  8-bit entity ID field, maximum number of values.
*/
#define TRACER_ENTITY_MAX   256

/** @ingroup tracer_entity_h_data_types
  Tracer OST entity ID.
*/
typedef uint8 tracer_ost_entity_id_t;

/*-------------------------------------------------------------------------*/
/** @addtogroup tracer_entity_h_constants
@{ */
/**  @name OST base layer protocol version
  OST base layer protocol version values are unique per entity.
  This is the current protocol version for each defined entity.
*/
/** @{ */
/** Not applicable, unknown, or undefined. */
#define TRACER_PROTOCOL_NONE     0
/** Tracer data stream version. */
#define TRACER_PROTOCOL_TDS      1
/** Software event with parameters version. */
#define TRACER_PROTOCOL_SWEVT    1
/** @} */ /* end_name_group OST base layer protocol version */
/** @} */ /* end_addtogroup tracer_entity_h_constants */

/** @ingroup tracer_entity_h_data_types
  Tracer OST protocol ID.
*/
typedef uint8 tracer_ost_protocol_id_t;

/** @addtogroup tracer_entity_h_constants
@{ */
/** Default entity. */
#define TRACER_ENTITY_DEFAULT    TRACER_ENTITY_TDS
/** Default version. */
#define TRACER_PROTOCOL_DEFAULT  TRACER_PROTOCOL_TDS
/** @} */ /* end_addtogroup tracer_entity_h_constants */


#endif /* #ifndef TRACER_ENTITY_H */


