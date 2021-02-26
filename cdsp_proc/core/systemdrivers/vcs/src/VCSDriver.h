#ifndef __VCSDRIVER_H__
#define __VCSDRIVER_H__
/*
===========================================================================
*/
/**
  @file VCSDriver.h 
  
  Internal header file for the VCS device driver.
*/
/*  
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/src/VCSDriver.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


#include "com_dtypes.h"
#include "comdef.h"
#include "DALSys.h"
#include "ULogFront.h"
#include "VCSDefs.h"
#include "VCSBSP.h"
#include "npa.h"
#include "voltage_level.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/

/**
 * Macros for checking, setting and clearing the global flag bits in the
 * driver context.  Note these work only on the "nGlobalFlags" field
 * within the driver context structure.
 */
#define VCS_GLOBAL_FLAG_IS_SET(flag) ((pDrvCtxt->nGlobalFlags & VCS_GLOBAL_FLAG_##flag) ? TRUE : FALSE)
#define VCS_GLOBAL_FLAG_SET(flag)    (pDrvCtxt->nGlobalFlags |= VCS_GLOBAL_FLAG_##flag)
#define VCS_GLOBAL_FLAG_CLEAR(flag)  (pDrvCtxt->nGlobalFlags &= ~VCS_GLOBAL_FLAG_##flag)

/*
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define VCS_ENUM_32BITS(name) VCS_##name##_32BITS = 0x7FFFFFFF

/*
 * Global flag bits
 *  STUB_API - Stubs out entire API (DAL and NPA nodes expected).
 */
#define VCS_GLOBAL_FLAG_STUB_API 0x00000001

/*
 * Flags used to enable/disable various features.
 */
#define VCS_FLAG_DISABLED_BY_USER                       (0x1UL << 0)
#define VCS_FLAG_DISABLED_BY_EFS                        (0x1UL << 1)
#define VCS_FLAG_DISABLED_BY_BSP                        (0x1UL << 2)
#define VCS_FLAG_DISABLED_BY_SWCLIENT                   (0x1UL << 3)
#define VCS_FLAG_DISABLED_BY_CPR                        (0x1UL << 4)
//#define VCS_FLAG_DISABLED__x                          (0x1UL << 5)
#define VCS_FLAG_DISABLED_HW_LOGIC                      (0x1UL << 31)

/*
 * Helper macros to convert between data types.
 */
#define VCS_CLIENT_TO_RAIL(c) \
  (VCSRailNodeType *)((c)->resource->node->data)

#define VCS_RESOURCE_TO_RAIL(r) \
  (VCSRailNodeType *)((r)->definition->data)

/*
 * Helper macros to get the name of the rail.
 */
#define VCS_RAIL_NAME(r) \
  (r)->aResources[VCS_NPA_RAIL_RES_IDX_RAIL].name
#define VCS_RESOURCE_NAME(r) \
  VCS_RAIL_NAME((VCS_RESOURCE_TO_RAIL(r)))

/*
 * Helper macros to get the dependency name of the rail.
 */
#define VCS_RAIL_DEP_NAME(r) \
  (r)->aDependencies[VCS_NPA_RAIL_DEP_IDX_REQ].name
#define VCS_RESOURCE_DEP_NAME(r) \
  VCS_RAIL_DEP_NAME((VCS_RESOURCE_TO_RAIL(r)))

/*
 * Helper macros to get the handle of the rail resources.
 */
#define VCS_RAIL_HANDLE(r) \
  (r)->aResources[VCS_NPA_RAIL_RES_IDX_RAIL].handle

/*
 * Helper macros to get the min corner of the rail.
 */
#define VCS_RAIL_MIN(r) \
  (r)->pActiveConfig != NULL ? \
    (r)->pActiveConfig->eCornerMin : \
    RAIL_VOLTAGE_LEVEL_OFF

#define VCS_RESOURCE_MIN(r) \
  VCS_RAIL_MIN((VCS_RESOURCE_TO_RAIL(r)))

/*
 * Helper macros to get the max corner of the rail.
 */
#define VCS_RAIL_MAX(r) \
  (r)->pActiveConfig != NULL ? \
    (r)->pActiveConfig->eCornerMax : \
    RAIL_VOLTAGE_LEVEL_MAX

#define VCS_RESOURCE_MAX(r) \
  VCS_RAIL_MAX((VCS_RESOURCE_TO_RAIL(r)))

/*
 * Helper macros to get the init corner of the rail.
 */
#define VCS_RAIL_INIT(r) \
  (r)->pActiveConfig != NULL ? \
    (r)->pActiveConfig->eCornerInit : \
    RAIL_VOLTAGE_LEVEL_NOM

#define VCS_RESOURCE_INIT(r) \
  VCS_RAIL_INIT((VCS_RESOURCE_TO_RAIL(r)))

/*
 * Helper macro to get rail resource definitions.
 */
#define VCS_RAIL_RES_DEF_RAIL(r) \
  (&((r)->aResources[VCS_NPA_RAIL_RES_IDX_RAIL]))

/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * VCS driver context.
 *
 *  pBSP         - Main VCS BSP data.
 *  hVCSLog      - ULOG handle.
 *  nGlobalFlags - Global flags bit-field.
 *  pImageCtxt   - Pointer to the image context.
 */
typedef struct VCSDrvCtxt
{
  VCSBSPType *pBSP;
  ULogHandle  hVCSLog;
  uint32      nGlobalFlags;
  void       *pImageCtxt;
} VCSDrvCtxt;


/*=========================================================================
      Function Definitions
==========================================================================*/


/*=========================================================================
**  Function : VCS_GetDrvCtxt
** =========================================================================*/
/**
  Returns driver context.

  This function is used to get the driver context when called from the lite
  API call path.


  @return
  Pointer to VCS driver context.

  @dependencies
  None.
*/

VCSDrvCtxt *VCS_GetDrvCtxt(void);


/* =========================================================================
**  Function : VCS_DriverInit
** =========================================================================*/
/**
  Initialize the VCS driver.

  This function initializes the VCS driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCS_DriverInit
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCSStub_DriverInit
** =========================================================================*/
/**
  Initialize the VCS driver stub.

  This function initializes the VCS driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.

  @dependencies
  None.
*/

DALResult VCSStub_DriverInit
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCS_DriverDeInit
** =========================================================================*/
/**
  De-initialize the VCS driver.

  This function shuts-down the VCS driver.  It is not expected to ever
  be called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- De-init was successful.
  DAL_ERROR -- De-init failed.

  @dependencies
  None.
*/

DALResult VCS_DriverDeInit
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCSStub_DriverDeInit
** =========================================================================*/
/**
  De-initialize the VCS driver stub.

  This function shuts-down the VCS driver.  It is not expected to ever
  be called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- De-init was successful.

  @dependencies
  None.
*/

DALResult VCSStub_DriverDeInit
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCS_InitBases
** =========================================================================*/
/**
  Initialize VCS register bases.

  This function initializes register bases used by the VCS driver. It maps
  each address used by the driver to its virtual representation.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCS_InitBases
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCS_InitTarget
** =========================================================================*/
/**
  Initialize the target specific portion of the VCS driver.

  This function initializes the portion of the VCS driver that is specific
  to the image and chipset we are running on.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCS_InitTarget
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCSStub_InitTarget
** =========================================================================*/
/**
  Initialize the target specific portion of the VCS driver.

  This function initializes the portion of the VCS driver that is specific
  to the image and chipset we are running on.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCSStub_InitTarget
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCS_InitRails
** =========================================================================*/
/**
  Initialize the rail data structures.

  This function initializes the data structures used to control voltage
  rails.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCS_InitRails
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCS_GetPropertyValue
** =========================================================================*/
/**
  Retrieve a VCS driver property from the environment store.

  This function retrieves the given VCS property from the relevant
  storage location (may be DAL properties DB or another source).

  @param *szName [in] -- Name of the property to look up
  @param *pValue [out] -- Location to store the result

  @return
  DAL_SUCCESS -- Property was found and retrieved
  DAL_ERROR -- Otherwise.

  @dependencies
  None.
*/

DALResult VCS_GetPropertyValue
(
  const char            *szName,
  VCSPropertyValueType  *pValue
);


/*=========================================================================
**  Function : VCS_IsBSPSupported
** =========================================================================*/
/**

  This function validates the specified BSP config is supported on the
  executing target chip and version.

  @param pHWVersion [in] - HW Version

  @return
  TRUE - HW version is supported.
  FALSE - HW version is not supported.

  @dependencies
  None.
*/

boolean VCS_IsBSPSupported
(
  VCSHWVersionType *pHWVersion
);


/* =========================================================================
**  Function : VCS_MapHWIORegion
** =========================================================================*/
/**
  Map a HWIO region into memory for the VCS driver.

  This function maps a physical HWIO region into memory for the VCS driver
  using environment appropriate APIs.  If the mapping is not available
  the virtual address will be set to the provided physical address.

  @param nPhysAddr [in] -- Physical address.
  @param nSizeBytes [in] -- Size of the region in bytes.
  @param *pnVirtAddr [out] -- Location to store the mapped address.

  @return
  DAL_SUCCESS -- Mapping was OK
  DAL_ERROR -- Error code based on failure.

  @dependencies
  None.
*/

DALResult VCS_MapHWIORegion
(
  uint32  nPhysAddr,
  uint32  nSizeBytes,
  uint32 *pnVirtAddr
);


/* =========================================================================
**  Function : VCS_MapAddress
** =========================================================================*/
/**
  Map a address into memory for the VCS driver.

  This function maps a physical HWIO region into memory for the VCS driver
  using environment appropriate APIs.  If the mapping is not available
  the virtual address will be set to the provided physical address.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pnAddr   [in] -- Pointer to memory address.

  @return
  DAL_SUCCESS -- Mapping was OK
  DAL_ERROR -- Error code based on failure.

  @dependencies
  None.
*/

DALResult VCS_MapAddress
(
  VCSDrvCtxt *pDrvCtxt,
  uint32     *pnAddr
);


/* =========================================================================
**  Function : VCS_InitNPA
** =========================================================================*/
/**
  Create all NPA resources for each rail.

  This function creates an NPA resource based on BSP data.

  @param pDrvCtxt[in] -- Pointer to the driver context.

  @return
  DAL_SUCCESS -- Sucessfully created NPA resources.
  DAL_ERROR -- Failed to create NPA resources.

  @dependencies
  None.
*/

DALResult VCS_InitNPA
(
  VCSDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : VCS_NPASatisfyIncreasingDependency
** =========================================================================*/
/**
  This function checks client type aggregation and increases the dependency
  if the new request state has increased from the previous request state.

  @param hClient [in] -- Client handle.

  @return
  None.

  @dependencies
  None.
*/

void VCS_NPASatisfyIncreasingDependency
(
  npa_client_handle hClient
);


/* =========================================================================
**  Function : VCS_NPASatisfyDecreasingDependency
** =========================================================================*/
/**
  This function checks client type aggregation and decreases the dependency
  if the new request state has decreased from the previous request state.

  @param hClient [in] -- Client handle.

  @return
  None.

  @dependencies
  None.
*/

void VCS_NPASatisfyDecreasingDependency
(
  npa_client_handle hClient
);


/* =========================================================================
**  Function : VCS_NPASatisfyChangingDependency
** =========================================================================*/
/**
  This function checks client type aggregation and updates the dependency
  if the new request state has changed from the previous request state.

  @param hClient [in] -- Client handle.

  @return
  None.

  @dependencies
  None.
*/

void VCS_NPASatisfyChangingDependency
(
  npa_client_handle hClient
);


/* =========================================================================
**  Function : VCS_GetRailNode
** =========================================================================*/
/**
  Looks up a rail node based on string name.

  This function takes the name of a rail and returns the associated handle.
  The string name of the rail can be found in the VCS BSP.

  @param szName [in]  String name of the LDO.
  @param pRail  [out] Pointer to rail node pointer to be filled in.

  @return
  DAL_SUCCESS -- Rail was found and the handle was filled in. \n
  DAL_ERROR_NOT_FOUND -- Rail was not found on this target. \n
  DAL_ERROR_INVALID_PARAMETER -- Pointer to rail name or pointer is null.

  @dependencies
  None.
*/

DALResult VCS_GetRailNode(const char *szName,  VCSRailNodeType **pRail);

void VCS_LoadNV(void);

DALResult VCS_EnableHVX(VCSDrvCtxt *pDrvCtxt) __attribute__ ((weak));
DALResult VCS_DisableHVX(VCSDrvCtxt *pDrvCtxt) __attribute__ ((weak));

#endif /* !__VCS_DRIVER_H__ */

