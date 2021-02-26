/*
==============================================================================

FILE:         VCSBSP.c

DESCRIPTION:
  This file contains VCS bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/image/adsp/hw/sdm670/VCSBSP.c#3 $

==============================================================================
            Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/


#include "comdef.h"
#include "VCSBSP.h"

// Components used in this image
#include "VCSRPMhRailImpl.h"


/*=========================================================================
      Externs
==========================================================================*/


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Data Declarations
==========================================================================*/


/*
 *  VCSRailConfigs_MX
 *
 *  Set of rail configurations for MX.
 */
static VCSRailConfigType VCSRailConfigs_MX[] =
{
  {
    .eCornerMin  = RAIL_VOLTAGE_LEVEL_OFF,
    .eCornerMax  = RAIL_VOLTAGE_LEVEL_MAX,
    .eCornerInit = RAIL_VOLTAGE_LEVEL_NOM,
    .HWVersion   = { {0x0, 0x0}, {0xFF, 0xFF} },
  }
};


/*
 *  VCSRailConfigs_CX
 *
 *  Set of rail configurations for CX.
 */
static VCSRailConfigType VCSRailConfigs_CX[] =
{
  {
    .eCornerMin  = RAIL_VOLTAGE_LEVEL_OFF,
    .eCornerMax  = RAIL_VOLTAGE_LEVEL_MAX,
    .eCornerInit = RAIL_VOLTAGE_LEVEL_NOM,
    .HWVersion   = { {0x0, 0x0}, {0xFF, 0xFF} },
  }
};


/*
 *  VCSRailConfigs_SSCMX
 *
 *  Set of rail configurations for SSC MX.
 */
static VCSRailConfigType VCSRailConfigs_SSCMX[] =
{
  {
    .eCornerMin = RAIL_VOLTAGE_LEVEL_OFF,
    .eCornerMax = RAIL_VOLTAGE_LEVEL_MAX,
    .eCornerInit = RAIL_VOLTAGE_LEVEL_NOM,
    .HWVersion = { { 0x0, 0x0 },{ 0xFF, 0xFF } },
  }
};


/*
 *  VCSRailConfigs_SSCCX
 *
 *  Set of rail configurations for SSC CX.
 */
static VCSRailConfigType VCSRailConfigs_SSCCX[] =
{
  {
    .eCornerMin = RAIL_VOLTAGE_LEVEL_OFF,
    .eCornerMax = RAIL_VOLTAGE_LEVEL_MAX,
    .eCornerInit = RAIL_VOLTAGE_LEVEL_NOM,
    .HWVersion = { { 0x0, 0x0 },{ 0xFF, 0xFF } },
  }
};


/*
 * VCSRPMhRailImpl_MX
 */
static VCSRPMhRailImplType VCSRPMhRailImpl_MX =
{
  .szName = "mx.lvl",
  .eDRVId = RSC_DRV_AUDIO,
  .bBatchCommand = TRUE,
};


/*
 * VCSRPMhRailImpl_CX
 */
static VCSRPMhRailImplType VCSRPMhRailImpl_CX =
{
  .szName = "cx.lvl",
  .eDRVId = RSC_DRV_AUDIO,
  .bBatchCommand = TRUE,
};


/*
 * VCSRPMhRailImpl_SSCMX
 */
static VCSRPMhRailImplType VCSRPMhRailImpl_SSCMX =
{
  .szName = "lmx.lvl",
  .eDRVId = RSC_DRV_VIRTUAL_SENSORS,
  .bBatchCommand = TRUE,
};


/*
 * VCSRPMhRailImpl_SSCCX
 */
static VCSRPMhRailImplType VCSRPMhRailImpl_SSCCX =
{
  .szName = "lcx.lvl",
  .eDRVId = RSC_DRV_VIRTUAL_SENSORS,
  .bBatchCommand = TRUE,
};


/*
 * aRails
 *
 * Set of rail instances.
 */
static VCSRailNodeType aRails[] =
{
  /*-----------------------------------------------------------------------*/
  /* MX                                                                    */
  /*-----------------------------------------------------------------------*/
  {
    VCS_DEF_RAIL_NAME("/vcs/vdd_mx"),
    /* no dependency */
    VCS_DEF_RAIL_CONFIGS(VCSRailConfigs_MX),
    VCS_DEF_RAIL_ATTRIBUTES(NPA_RESOURCE_DEFAULT),
    VCS_DEF_RAIL_IMPL(RPMh, &VCSRPMhRailImpl_MX),
  },

  /*-----------------------------------------------------------------------*/
  /* CX                                                                    */
  /*-----------------------------------------------------------------------*/
  {
    VCS_DEF_RAIL_NAME("/vcs/vdd_cx"),
    VCS_DEF_RAIL_DEP_NAME("/vcs/vdd_mx"),
    VCS_DEF_RAIL_CONFIGS(VCSRailConfigs_CX),
    VCS_DEF_RAIL_ATTRIBUTES(NPA_RESOURCE_DEFAULT),
    VCS_DEF_RAIL_IMPL(RPMh, &VCSRPMhRailImpl_CX),
  },

  /*-----------------------------------------------------------------------*/
  /* SSC_MX                                                                */
  /*-----------------------------------------------------------------------*/
  {
    VCS_DEF_RAIL_NAME("/vcs/vdd_ssc_mx"),
    /* no dependency */
    VCS_DEF_RAIL_CONFIGS(VCSRailConfigs_SSCMX),
    VCS_DEF_RAIL_ATTRIBUTES(NPA_RESOURCE_DEFAULT),
    VCS_DEF_RAIL_IMPL(RPMh, &VCSRPMhRailImpl_SSCMX),
  },

  /*-----------------------------------------------------------------------*/
  /* SSC_CX                                                                */
  /*-----------------------------------------------------------------------*/
  {
    VCS_DEF_RAIL_NAME("/vcs/vdd_ssc_int"),
    VCS_DEF_RAIL_DEP_NAME("/vcs/vdd_ssc_mx"),
    VCS_DEF_RAIL_CONFIGS(VCSRailConfigs_SSCCX),
    VCS_DEF_RAIL_ATTRIBUTES(NPA_RESOURCE_DEFAULT),
    VCS_DEF_RAIL_IMPL(RPMh, &VCSRPMhRailImpl_SSCCX),
  },

};


/*
 * VCS Log Default Configuration.
 */
const VCSLogType VCS_LogDefaultConfig[] =
{
  {
    /* .nLogSize = */ 4096,
  }
};


static VCSBaseType aBases[] =
{
  /* empty */
};


/*
 *  VCSBSP
 *
 *  List and length of Rail and LDO configurations.
 */
const VCSBSPType VCSBSP =
{
  VCS_DEF_ARR_FIELD(Bases, aBases),
  VCS_DEF_ARR_FIELD(Rails, aRails),
};