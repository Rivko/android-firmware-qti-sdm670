#ifndef __VCSBSP_H__
#define __VCSBSP_H__
/*
===========================================================================
*/
/**
  @file VCSBSP.h

  BSP interface definitions for the DAL VCS device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010-2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/src/VCSBSP.h#2 $
  $DateTime: 2017/08/07 11:42:50 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/22/14   lil     Created.
 
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "comdef.h"
#include "VCSDefs.h"
#include "HALvcs.h"
#include "HALvcsInternal.h"
#include "DDIChipInfo.h"
#include "voltage_level.h"
#include "npa_resource.h"
#include "npa_remote_resource.h"

/*=========================================================================
      Macros
==========================================================================*/


/*
 * Macros to define the index for each rail resource.
 */
#define VCS_NPA_RAIL_RES_IDX_RAIL          0

/*
 * Macro to define the number of resources for each rail.
 */
#define VCS_NPA_RAIL_NUM_RESOURCES  1

/*
 * Macros to define the index for each rail dependency.
 */
#define VCS_NPA_RAIL_DEP_IDX_REQ   0
#define VCS_NPA_RAIL_DEP_IDX_SUP   1

/*
 * Macro to define the number of dependencies for each rail.
 */
#define VCS_NPA_RAIL_NUM_DEPENDENCIES 2

/*
 * Helper macro to define BSP array fields.
 */
#define VCS_DEF_ARR_FIELD(field, data) \
  .p ## field = data, \
  .nNum ## field = ARR_SIZE(data)

/*
 * Helper macro to define name related entries for a given rail.
 */
#define VCS_DEF_RAIL_NAME(x) \
  .mNode.name = x, \
  .aResources[VCS_NPA_RAIL_RES_IDX_RAIL].name = x

/*
 * Helper macro to define dependency name related entries for a given rail.
 */
#define VCS_DEF_RAIL_DEP_NAME(x) \
  .aDependencies[VCS_NPA_RAIL_DEP_IDX_REQ].name = x, \
  .aDependencies[VCS_NPA_RAIL_DEP_IDX_REQ].client_type = NPA_CLIENT_REQUIRED, \
  .aDependencies[VCS_NPA_RAIL_DEP_IDX_SUP].name = x, \
  .aDependencies[VCS_NPA_RAIL_DEP_IDX_SUP].client_type = NPA_CLIENT_SUPPRESSIBLE, \
  .mNode.dependency_count = 2

/*
 * Helper macro to define configurations for a given rail.
 */
#define VCS_DEF_RAIL_CONFIGS(x) \
  .pConfigs = x, \
  .nNumConfigs = ARR_SIZE(x)

/*
 * Helper macro to define configurations for a given rail.
 */
#define VCS_DEF_RAIL_IMPL(impl, data) \
  .pControl = &VCS##impl##RailImplControl, \
  .pImpl = (void *)(data) \

/*
 * Helper macro to define attributes of rail resources.
 */
#define VCS_DEF_RAIL_ATTRIBUTES(x) \
  .aResources[VCS_NPA_RAIL_RES_IDX_RAIL].attributes = x
  
/*
 * Helper macro to define the remote PDs permitted access to this rail.
 */
#define VCS_DEF_RAIL_REMOTE_PD(x) \
  .eRemoteDomainId = x


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * VCSLogType
 *
 * Default configuration for the VCS log.
 *
 *  nLogSize        - Log size in bytes.  Must be a power of 2.
 */
typedef struct
{
  uint32 nLogSize;
} VCSLogType;


/*
 * VCSPropertyValueType
 *
 * Generic VCS property data.  Actual data type will depend on the property
 * and will not even necessarily be a pointer (i.e. could be straight uint32).
 */
typedef const void *VCSPropertyValueType;


/*
 * VCSHWVersionType;
 *
 * HW version information.
 *
 *  Min             - Minimum supported HW Major/Minor revision
 *  Max             - Maximum supported HW Major/Minor revision
 *                    '0xFF' indicates all versions.
 *  eChipInfoFamily - Chip this configuration is for. '0' indicates all chips.
 *  aeChipInfoId    - Array of chip id's that the configuration supports.
 *                    NULL indicates all chips.
 */
typedef struct
{
  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Min;

  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Max;

  DalChipInfoFamilyType    eChipInfoFamily;
  const DalChipInfoIdType *aeChipInfoId;

} VCSHWVersionType;


/*
 * VCSBase
 *
 * Structure for a register base mapping.
 *
 * nPhysAddr  - Physical address to be mapped.
 * nSize      - Size of region to be mapped.
 * pnVirtAddr - Mapped virtual address.
 */
typedef struct VCSBase
{
  uint32  nPhysAddr;
  uint32  nSize;
  uint32 *pnVirtAddr;
} VCSBaseType;


/*
 * VCSRailConfigType
 *
 * BSP Data structure for describing a rail configuration.
 *
 *  eCornerMin  - The minimum corner at which the rail is allowed to operate
 *  eCornerMax  - The max corner at which the rail is allowed to operate
 *  eCornerInit - The initial corner of rail at boot-up
 *  HWVersion   - Supported hardware version info
 */
typedef struct VCSRailConfigType
{
  rail_voltage_level eCornerMin;
  rail_voltage_level eCornerMax;
  rail_voltage_level eCornerInit;
  VCSHWVersionType   HWVersion;
} VCSRailConfigType;


typedef struct VCSRailNodeType VCSRailNodeType;

/*
 * VCSRailControlType
 *
 * BSP data structure for defining rail control functions.
 *
 *  fpInit      - Pointer to funciton for initializing rail.
 *  fpSetCorner - Pointer to function for setting corner on rail.
 *  fpMapCorner - Pointer to function for mapping a requested corner to a supported corner on the rail.
 */
typedef struct VCSRailControlType
{
  DALResult (*fpInit)     (VCSRailNodeType *pRail);
  DALResult (*fpSetCorner)(npa_client_handle hClient, rail_voltage_level eCorner);
  DALResult (*fpMapCorner)(VCSRailNodeType *pRail, rail_voltage_level *eCorner);
} VCSRailControlType;


/*
 * VCS rail node type.
 *
 *  pControl         - Pointer to control structure for this rail impl.
 *  pImpl            - Pointer to implementation type config data.
 *  eCorner          - Current corner.
 *  mNode            - NPA node.
 *  aResource        - NPA resources for this node.
 *  aDependency      - NPA dependencies for this node.
 *  eRemoteDomainId  - Identifiers of remote domains.
 *  pConfigs         - Pointer to array of rail configurations.
 *  nNumConfigs      - Length of pConfigs array.
 *  pActiveConfig    - Pointer to detected rail corner configuration.
 *  eCornerNAS       - NAS rail corner (OFF if none).
 *  nNASRequestCount - Number of NAS requests while entering sleep.
 */
typedef struct VCSRailNodeType
{
  // Rail type implementation
  VCSRailControlType      *pControl;
  void                    *pImpl;

  // State
  rail_voltage_level       eCorner;

  // NPA stuff
  npa_node_definition      mNode;
  npa_resource_definition  aResources[VCS_NPA_RAIL_NUM_RESOURCES];
  npa_node_dependency      aDependencies[VCS_NPA_RAIL_NUM_DEPENDENCIES];
  npa_remote_domain_id     eRemoteDomainId;

  // BSP data
  VCSRailConfigType       *pConfigs;
  uint32                   nNumConfigs;
  VCSRailConfigType       *pActiveConfig;

  // NAS support
  rail_voltage_level       eCornerNAS;
  uint32                   nNASRequestCount;

} VCSRailNodeType;


/*
 * VCSBSPType
 *
 * BSP Data structure for list of the rail configurations.
 *
 *  pBases    - Pointer to array of bases.
 *  nNumBases - Length of array pBases.
 *  pRails    - Pointer to array of rails.
 *  nNumRails - Length of array pRails.
 */
typedef struct VCSBSPType
{
  VCSBaseType     *pBases;
  uint32           nNumBases;
  VCSRailNodeType *pRails;
  uint32           nNumRails;
} VCSBSPType;


#endif  /* __VCSBSP_H__ */
