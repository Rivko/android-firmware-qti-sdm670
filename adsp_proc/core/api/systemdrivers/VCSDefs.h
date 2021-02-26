#ifndef VCSDEFS_H
#define VCSDEFS_H
/*
===========================================================================
*/
/**
  @file VCSDefs.h
  @brief Public definitions include file for accessing the VCS device driver.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The VCS_Driver_API_mainpage.dox file contains all file/group
      descriptions that are in the output PDF generated using Doxygen and
      LaTeX. To edit or update any of the file/group text in the PDF, edit the
      VCS_Driver_mainpage.dox file or contact Tech Pubs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/VCSDefs.h#2 $
  $DateTime: 2017/08/07 11:42:50 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h" /* for "uint32" */
#include "npa.h"
#include "voltage_level.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/** @addtogroup vcs_functions
@{ */

/*
 * VCS NPA Client type
 */
#define VCS_NPA_CLIENT_ACTIVE_FLOOR  NPA_CLIENT_CUSTOM1

/*
 * VCS NPA Rail Events.
 */
typedef enum
{
  VCS_NPA_RAIL_EVENT_PRE_CHANGE   = NPA_TRIGGER_CUSTOM_EVENT1,
  VCS_NPA_RAIL_EVENT_POST_CHANGE  = NPA_TRIGGER_CUSTOM_EVENT2,
  VCS_NPA_RAIL_EVENT_LIMIT_MAX    = NPA_TRIGGER_CUSTOM_EVENT3,
} VCSNPARailEventType;


/*
 * VCS NPA Rail Event Data.
 *
 * All fields are either:
 *   Non-zero when the state change is known prior to dispatching the event
 *   against the resource being monitored.
 *   For example: eCorner pre vs. post is always known.
 *
 *   Zero when the state change is unknown prior to dispatching the event
 *   against the resource being monitored.
 *   For example: nVoltageUV pre vs. post is not always known because the
 *   final voltage comes from CPR which is only known after the pre event has
 *   been dispatched. Another example is the voltage on CX/MX when changing
 *   corners as only RPM knows the actual voltage on those rails.
 */
typedef struct
{
  boolean bIsNAS ;            // Boolean indicates whether this change is NAS vs. CAS (immediate).

  struct
  {
    rail_voltage_level eCorner;    // Voltage corner before the change event completes.
                                   // Empty for limit max events.
    uint32             nVoltageUV; // Voltage in uV before the change event completes.
  } PreChange;
  struct
  {
    rail_voltage_level eCorner;    // Voltage corner after the change event completes.
                                   // New max corner for limit max events.
    uint32             nVoltageUV; // Voltage in uV after the change event completes.
  } PostChange;
} VCSNPARailEventDataType;


/*
 * NPA Resources Exported by VCS
 *
 * General resource naming scheme:
 *   RAIL
 *     Resource name: "/vcs/vdd_{railname}"
 *     Request units: rail_voltage_level enum values.
 *     Client types supported:
 *       - NPA_CLIENT_REQUIRED
 *       - NPA_CLIENT_SUPPRESSIBLE
 *       - VCS_NPA_CLIENT_ACTIVE_FLOOR
 */


/** @} */ /* end_addtogroup vcs_functions */

/** @addtogroup npa_functions
@{ */
/** @name NPA Query Requests
@{ */

/**
  NPA query ID for the number of voltage levels.
  This query returns the number of voltage levels for the resource
  based on the target and configured limitations.
 */
#define VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES                                      \
  (NPA_QUERY_RESERVED_END + 1)

/**
  NPA query ID for the current voltage in uV.
  This query returns the current voltage in uV
  based on the target and configured limitations.
 */
#define VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_UV                               \
  (VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES + 1)

/**
  NPA query ID for the current voltage in HW format.
  This query returns the current voltage in HW format 
  based on the target and configured limitations.
 */
#define VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_HW                               \
  (VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_UV + 1)

/**
  NPA query ID for the current voltage level enum value.
  This query returns the voltage level enum for the resource
  based on the target and configured limitations.
  (See voltage_level.h for enum definition).
 */
#define VCS_NPA_QUERY_VOLTAGE_CURRENT_LEVEL_ID                                 \
  (VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_HW + 1)

/**
  NPA query ID for the voltage level id. This query gets the enum id
  for the requested level using the enum from VCSCornerType defined above.
  The actual voltage level is to be added to this ID (i.e., to query voltage
  level id for level 2, pass VCS_NPA_QUERY_VOLTAGE_LEVEL_ID + 2).
  This interfaces uses 0 as the first index.
  (See voltage_level.h for enum definition).
 */
#define VCS_NPA_QUERY_VOLTAGE_LEVEL_ID                                         \
  (VCS_NPA_QUERY_VOLTAGE_CURRENT_LEVEL_ID + 1)

/**
  Marker to indicate boundary for query ID's.  Used for bounds logic in the
  query driver function.
 */
#define VCS_NPA_QUERY_END_MARKER                                               \
  (VCS_NPA_QUERY_VOLTAGE_LEVEL_ID + 0x20)


/** @} */ /* end_name_group NPA Query Requests */
/** @} */ /* end_addtogroup npa_functions */


#endif /* !VCSDEFS_H */

