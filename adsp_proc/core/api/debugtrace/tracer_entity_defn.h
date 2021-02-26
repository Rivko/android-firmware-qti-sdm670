/**
  @file tracer_entity_defn.h
  @brief For specifying entities recognized by tracer. Will be built
  into an enum and reference tables.

  Obtain entity ids from tracer_entity.h. Do not include
  tracer_entity_defn.h in your files.

  Developer notes:
      Define the DEFINE_ENTITY(x,y,z) macro before each include
      of this file because any macro definition is undefined at
      the end of this file.
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.

      The Tracer.h_Debug_Tool_API_mainpage_mainpage.dox file contains the
      group/module descriptions that are displayed in the output PDF generated
      using Doxygen and LaTeX. To edit or update any of the group/module text
      in the PDF, edit the _mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*=============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.qdsp6/2.1/api/debugtrace/tracer_entity_defn.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  01/17/13   llg    (Tech Pubs) Edited/added Doxygen comments and markup.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Dummy definition for when auto included by deviceconfig properties.
#ifndef DEFINE_ENTITY
#define DEFINE_ENTITY(x,y,z) z
#endif

/** @cond */
/*
  OST base layer entity ids, unique per target system.

  DEFINE_ENTITY(name, value, comment)
      "name" is a maximum of 10 alphanumeric characters.
      "comment" must be properly delimited as a C-style comment.

  Reference entity id name is TRACER_ENTITY_<name>
 */
DEFINE_ENTITY(NONE,  0, /* Raw output stream */)
DEFINE_ENTITY(TDS,   1, /* Tracer output stream (w/ TDS header) */)
DEFINE_ENTITY(SWEVT, 2, /* Software event with parameters */)
// IDs 3-10 are reserved for TRACER use.

// All others are independent output streams (OST header and framing only)
DEFINE_ENTITY(ULOG, 11, /* Ulog */)
DEFINE_ENTITY(PROF, 12, /* PMU and TP profiling */)
DEFINE_ENTITY(DIAG, 13, /* Diag */)
DEFINE_ENTITY(FAST_RPC, 14, /* Fast RPC libs profiling */) 

// 200-239 Reserved for assignment to 3rd parties.
// 240 to 254 Reserved for OST use.
/** @endcond */

// Clear the previously defined macro to reset for subsequent use.
#undef DEFINE_ENTITY

