#ifndef __VCSLDORAILIMPL_H__
#define __VCSLDORAILIMPL_H__
/*
===========================================================================
*/
/**
  @file VCSLDORailImpl.h

  Interface definitions for the LDO rail impl type.
*/
/*
  ====================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/src/RailImpls/VCSLDORailImpl.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "VCSBSP.h"
#include "HALvcs.h"
#include "cpr_defs.h"
#include "npa.h"
#include "voltage_level.h"


/*=========================================================================
      Extern data
==========================================================================*/

extern VCSRailControlType VCSLDORailImplControl;


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * VCSLDOCornerFuseType
 *
 * BSP data structure for describing a corner that the LDO supports as well
 * as how to interpret the fuse info to determine whether a corner should be
 * used on a specific target instance.
 *
 *  eCorner - Corner identifier.
 *  nAddr   - Address of fuse register.
 *  nMask   - Bitmask that specifies enable field in fuse register.
 */
typedef struct VCSLDOCornerFuseType
{
  rail_voltage_level eCorner;
  uint32             nAddr;
  uint32             nMask;
} VCSLDOCornerFuseType;


/*
 * VCSLDOModeType
 * 
 *  Types of modes supported by LDO.
 */
typedef enum VCSLDOModeType
{
  VCS_LDO_MODE_STANDALONE  = 0,
  VCS_LDO_MODE_WITH_COPROC = 1,
  
  VCS_LDO_MODE_MAX_NUM     = 2
} VCSLDOModeType;


/*
 * VCSLDOConfigType
 *
 * BSP data structure for describing the LDO configuration.
 *
 *  eMode      - Mode of this configuration.
 *  bEnable    - Enable switch for this LDO.
 *  eCornerMin - Min corner supported by the LDO.
 *  eCornerMax - Max corner supported by the LDO.
 *  HALConfig  - HAL configuration of LDO.
 *  HWVersion  - HW version this config applies towards.
 */
typedef struct VCSLDOConfigType
{
  VCSLDOModeType        eMode;
  boolean               bEnable;
  rail_voltage_level    eCornerMin;
  rail_voltage_level    eCornerMax;
  HAL_vcs_LDOConfigType HALConfig;
  VCSHWVersionType      HWVersion;
} VCSLDOConfigType;


/*
 * VCSLDORailImplType.
 *
 *  HALdesc          - HAL ldo structure.
 *  eDomainId        - CPR domain identifier.
 *  szNameCPR        - String name of CPR node for closed loop update events.
 *  pConfigs         - Pointer to array of LDO configurations.
 *  nNumConfigs      - Number of LDO configurations.
 *  pCornerFuses     - Fuse enablement data for supported corners.
 *  nNumCornerFuses  - Length of array pCornerFuses.
 *  eMode            - Mode identifier.
 *  bIsEnabled       - Boolean indicating whether the LDO is enabled.
 *  nVoltageUV       - Voltage in uV.
 *  nNumRestrictions - Number of restrictions on the LDO. The LDO will be
 *                     disabled if this number is greater than 0.
 *  nDisable         - LDO disable flag.
 *  pActiveConfig    - Pointer to detected LDO corner configuration.
 *  aCorners         - Array of corner items.
 *  nNumCorners      - Number of entries populated in the aCorners list.
 *  hEventCPR        - NPA event to receive intra-corner voltage change recommendations.
 */
typedef struct VCSLDORailImplType
{
  HAL_vcs_LDODescType   HALdesc;
  cpr_domain_id         eDomainId;
  const char           *szNameCPR;
  VCSLDOConfigType     *pConfigs;
  uint32                nNumConfigs;
  VCSLDOCornerFuseType *pCornerFuses;
  uint32                nNumCornerFuses;

  VCSLDOModeType        eMode;
  boolean               bIsEnabled;
  uint32                nVoltageUV;
  uint32                nNumRestrictions;
  uint32                nDisable;
  VCSLDOConfigType     *pActiveConfig;
  rail_voltage_level    aCorners[RAIL_VOLTAGE_LEVEL_NUM_LEVELS];
  uint32                nNumCorners;
  npa_event_handle      hEventCPR;
} VCSLDORailImplType;


/*=========================================================================
      Prototypes
==========================================================================*/

/* =========================================================================
**  Function : VCS_LDORailImplSetMode
** =========================================================================*/
/**
  Attempts to reconfigure the LDO to operate under the specified mode.
  It will first disable the LDO via an added restriction so that the LDO can
  be reconfigured while in the disabled state.  The restriction is subsequently
  removed and the LDO is re-enabled if possible under the new configuration.
  
  Note: The caller must take steps to lock down the rail node mutex prior
        to invoking this interface.

  @param *pRail [in] -- Pointer rail node.
  @param  eMode [in] -- Mode identifier.

  @return
  DAL_SUCCESS -- Mode was set successfully.
  DAL_ERROR -- Mode wasn't set.

  @dependencies
  None.
*/

extern DALResult VCS_LDORailImplSetMode
(
  VCSRailNodeType *pRail,
  VCSLDOModeType   eMode
);


/* =========================================================================
**  Function : VCS_LDORailImplAddRestriction
** =========================================================================*/
/**
  Restrict the usage of a given LDO. This may be required for various
  reasons such as thermal mitigation. We keep a count of the number of
  restrictions so that we only re-allow LDO usage after all clients have
  lifted their restrictions. Any restriction on the LDO disables its
  usage completely.

  @param pRail [in] Pointer to rail node.

  @return
  DAL_SUCCESS -- Successfully restricted the usage of the requested LDO. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid rail node passed in.

  @dependencies
  None.
*/

DALResult VCS_LDORailImplAddRestriction
(
  VCSRailNodeType *pRail
);


/* =========================================================================
**  Function : VCS_LDORailImplRemoveRestriction
** =========================================================================*/
/**
  Remove a restriction on the usage of a given LDO. If the LDO has no more
  restrictions on it, we re-allow its usage.

  @param pRail [in] Pointer to rail node.

  @return
  DAL_SUCCESS -- Successfully removed the restriction on requested LDO. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid rail node passed in.

  @dependencies
  None.
*/

DALResult VCS_LDORailImplRemoveRestriction
(
  VCSRailNodeType *pRail
);


#endif  /* __VCSLDORAILIMPL_H__ */ 

