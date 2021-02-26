#ifndef __CLOCKDRIVER_H__
#define __CLOCKDRIVER_H__
/*
===========================================================================
*/
/**
  @file ClockDriver.h

  Internal header file for the clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010-2017 QUALCOMM Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/src/ClockDriver.h#3 $
  $DateTime: 2017/10/25 23:14:56 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"
#include "comdef.h"
#include "DALSys.h"
#ifdef CLOCK_ULOG_STUB
#include "ClockULogStub.h"
#else
  #include "ULogFront.h"
#endif
#include "ClockDefs.h"
#include "ClockBSP.h"
#ifdef CLOCK_NPA_STUB
#include "ClockNPAStub.h"
#else
  #include "npa.h"
#endif
#include "DDIClock.h"
#include "ClockEnv.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/

/**
 * Macro lock the driver mutex.
 */
#define DALCLOCK_LOCK(drvCtxt) DALSYS_SyncEnter(drvCtxt->hLock)


/**
 * Macro try-lock the driver mutex.
 */
#define DALCLOCK_TRYLOCK(drvCtxt) DALSYS_SyncTryEnter(drvCtxt->hLock)


/**
 * Macro to free the driver mutex.
 */
#define DALCLOCK_FREE(drvCtxt) DALSYS_SyncLeave(drvCtxt->hLock)


/**
 * Check if source is controlled by an NPA resource.
 */
#define CLOCK_SOURCE_NPA_CONTROLLED(src) \
  ((src)->szName && (src)->szName[0] == '/')


/**
 * NPA client name used for internal clock driver clients.
 */
#define CLOCK_NPA_CLIENT_NAME  "clock"


/**
 * Convert an NPA state to a frequency in Hz.
 */
#define NPA_TO_HZ(state) \
  (uint32)((state) == NPA_MAX_STATE ? (state) : (state) * 1000)


/*
 * Macros for checking, setting and clearing the unified flag bits in the
 * driver nodes.  Note these work on any structure containing an "nFlags"
 * field.
 */
#define CLOCK_FLAG_IS_SET(ptr, flag) (((ptr)->nFlags & CLOCK_FLAG_##flag) ? TRUE : FALSE)
#define CLOCK_FLAG_SET(ptr, flag)    ((ptr)->nFlags |= CLOCK_FLAG_##flag)
#define CLOCK_FLAG_CLEAR(ptr, flag)  ((ptr)->nFlags &= ~CLOCK_FLAG_##flag)


/*
 * Macros for checking, setting and clearing the global flag bits in the
 * driver context.  Note these work only on the "nGlobalFlags" field
 * within the driver context structure.
 */
#define CLOCK_GLOBAL_FLAG_IS_SET(flag) ((pDrvCtxt->pBSP->nFlags & CLOCK_GLOBAL_FLAG_##flag) ? TRUE : FALSE)
#define CLOCK_GLOBAL_FLAG_SET(flag)    (pDrvCtxt->pBSP->nFlags |= CLOCK_GLOBAL_FLAG_##flag)
#define CLOCK_GLOBAL_FLAG_CLEAR(flag)  (pDrvCtxt->pBSP->nFlags &= ~CLOCK_GLOBAL_FLAG_##flag)


/*
 * Values used for making NPA requests on clock sources.
 *  0:          Everything related to XO can be turned off.
 *  0xFFFFFFFF: Everything possible in the chain (crystal to pad) must be on.
 */
#define CLOCK_SOURCE_NOT_REQUIRED     0
#define CLOCK_SOURCE_REQUIRED         0xFFFFFFFF


/*
 * Default size of the BIST ULOG log buffer.
 *
 * NOTE: The log size should be specified in powers of 2.
 */
#define CLOCK_DEFAULT_LOG_SIZE        4096
#define CLOCK_BIST_DEFAULT_LOG_SIZE   65536


/*
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF


/*
 * Clock ID macros.
 */
#define CLOCK_MAKE_ID(cat, idx)   (((cat) << 16) | (idx))
#define CLOCK_GET_ID_CATEGORY(id) (((id) & 0xFFFF0000) >> 16)
#define CLOCK_GET_ID_INDEX(id)    ((id) & 0xFFFF)
#define CLOCK_ID_INVALID          0


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * ClockIdCategoryType
 *
 * Clock ID categories.
 */
typedef enum
{
  CLOCK_ID_CATEGORY_NONE = 0,
  CLOCK_ID_CATEGORY_CLOCK,
  CLOCK_ID_CATEGORY_POWERDOMAIN,
  CLOCK_ID_CATEGORY_SOURCE,
  CLOCK_ID_CATEGORY_DEBUGMUX,
  CLOCK_ID_CATEGORY_DIVIDER,

  CLOCK_ID_CATEGORY_TOTAL,
} ClockIdCategoryType;


/**
 * Clock Reference Type for client contexts to keep track of the total number
 * of reference counts for each type of trackable resource.
 */
typedef struct
{
  uint32 nClockRefs;
  uint32 nPowerDomainRefs;
  uint32 nSourceRefs;

}ClockCtxtReferenceType;

/**
 *  ID structure to keep track of allocated client contexts.
 */
typedef struct
{
  uint32                 nPDID;               /**< The ID from the DAL framework associated with a particular PD */
  uint32                 nIdx;                /**< ClockClientCxts[] index associated with this client information */
  boolean                bInUse;              /**< Flag specifying if a client context is currently in use */
  ClockCtxtReferenceType TotalCtxtReferences; /**< The total resource reference count for this context */

}ClockCtxtInfoType;


/*
 * Driver, device and client contexts
 */
typedef struct ClockDALDrvCtxt    ClockDALDrvCtxt;
typedef struct ClockDALDevCtxt    ClockDALDevCtxt;
typedef struct ClockDALClientCtxt ClockDALClientCtxt;


/**
 * Private "Clock" Vtable
 */
typedef struct ClockDALVtbl ClockDALVtbl;
struct ClockDALVtbl
{
  DALResult (*Clock_DriverInit)(ClockDALDrvCtxt *);
  DALResult (*Clock_DriverDeInit)(ClockDALDrvCtxt *);
};


/**
 * Clock client context.
 */
struct ClockDALClientCtxt
{
  /*
   * Base members - do not modify!
   */
  uint32                  dwRefs;
  uint32                  dwAccessMode;
  void                   *pPortCtxt;
  struct ClockDALDevCtxt *pClockDevCtxt;
  DalClockHandle          DalClockHandle;
  ClockCtxtInfoType       CtxtInfo;
};


/**
 * Clock driver context.
 */
typedef struct
{
  ClockBSPType      *pBSP;                    /**< Main clock driver BSP data. */
  DALSYSSyncObj      LockObj;                 /**< Sync object storage */
  DALSYSSyncHandle   hLock;                   /**< Handle to our mutex */
  ULogHandle         hClockLog;               /**< Handle to our ULOG log */
  ULogHandle         hClockBISTLog;           /**< Handle to our BIST ULOG log */
  boolean            bBISTEnabled;            /**< Flag used to enable the BIST */
  uint32             nBISTLogSize;            /**< Size of the BIST ULOG log */
  boolean            bDCVSEnabled;            /**< DCVS state */
  void              *pImageCtxt;              /**< Image context */
  boolean            bSuspendVoltageRequest;  /**< Voltage suspension state */
} ClockDrvCtxt;


/**
 * Clock callback context type.
 */
typedef struct ClockCallbackCtxtType
{
  void          (*pCallback)(void *);  /**< callback function for the resource */
  void          *pCallbackContext;     /**< context passed to the callback */
  ClockDrvCtxt  *pDriverContext;       /**< clock driver context used for locking */
} ClockCallbackCtxtType;


/**
 * Clock device context.
 */
struct ClockDALDevCtxt
{
  /*
   * Base Members - do not modify!
   */
  uint32           dwRefs;
  DALDEVICEID      DevId;
  uint32           dwDevCtxtRefIdx;
  ClockDALDrvCtxt *pClockDrvCtxt;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32           Reserved[16];
};


/**
 * Clock driver context.
 */
struct ClockDALDrvCtxt
{
  /*
   * Base members - do not modify!
   */
  ClockDALVtbl              ClockDALVtbl;
  uint32                    dwNumDev;
  uint32                    dwSizeDevCtxt;
  uint32                    bInit;
  uint32                    dwRefs;
  ClockDALDevCtxt           ClockDevCtxt[1];

  /*
   * Private driver context.
   */
  DalDeviceHandle          *phHWIO;
  ClockDrvCtxt              DrvCtxt;
};


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : Clock_DriverInit
** =========================================================================*/
/**
  Initialize the clock driver.

  This function initializes the clock driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverInit
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : ClockStub_DriverInit
** =========================================================================*/
/**
  Initialize the clock driver stub.

  This function initializes the clock driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult ClockStub_DriverInit
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_DriverDeInit
** =========================================================================*/
/**
  De-initialize the clock driver.

  This function shuts-down the clock driver.  It is not expected to ever
  be called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverDeInit
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : ClockStub_DriverDeInit
** =========================================================================*/
/**
  De-initialize the clock driver stub.

  This function shuts-down the clock driver.  It is not expected to ever
  be called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult ClockStub_DriverDeInit
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitBases
** =========================================================================*/
/**
  Initialize clock register bases.

  This function initializes register bases used by the clock driver. It maps
  each address used by the driver to its virtual representation.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitBases
(
  ClockDrvCtxt *pDrvCtxt
);



/* =========================================================================
**  Function : Clock_InitTarget
** =========================================================================*/
/**
  Initialize the target specific portion of the clock driver.

  This function initializes the portion of the clock driver that is specific
  to the image and chipset we are running on.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitTarget
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : ClockStub_InitTarget
** =========================================================================*/
/**
  Initialize the target specific portion of the clock driver.

  This function initializes the portion of the clock driver that is specific
  to the image and chipset we are running on.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult ClockStub_InitTarget
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitDCVS
** =========================================================================*/
/**
  Initialize the NPA DCVS nodes for this image in the clock driver.

  This function initializes the DCVS module for the current image.  This
  will define and create the required NPA nodes.  Nodes will be be
  active until DalClock_EnableDCVS has been called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitDCVS
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitNPA
** =========================================================================*/
/**
  Initialize the generic NPA resources.

  This function initializes the generic NPA resources that control domain
  frequencies.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitNPA
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_TestClock
** =========================================================================*/
/**
  Perform a non-destructive built-in self test on the specified clock node
  in the driver.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param ClockId [in] -- Clock ID of the clock to be tested.

  @return
  DAL_SUCCESS -- BIST was successful.
  DAL_ERROR -- BIST failed.

  @dependencies
  Core and image initialization must be complete prior to running the test.
*/

DALResult Clock_TestClock
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock
);


/* =========================================================================
**  Function : Clock_BIST
** =========================================================================*/
/**
  Perform a non-destructive built-in self test on all clock nodes in the
  driver.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- BIST was successful.
  DAL_ERROR -- BIST failed.

  @dependencies
  Core and image initialization must be complete prior to running the BIST.
*/

DALResult Clock_BIST
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_ImageBIST
** =========================================================================*/
/**
  Perform a non-destructive built-in self test on the image specific portion
  of the clock driver.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *bBISTPassed [in/out] -- Pointer to the main BIST passed boolean.
  @param *nFailedTests [in/out] -- Pointer to the main BIST value which stores
                                   number of failed tests.

  @return
  DAL_SUCCESS -- Image BIST was successful.
  DAL_ERROR -- Image BIST failed.

  @dependencies
  Core and image initialization must be complete prior to running the Image BIST.
*/

DALResult Clock_ImageBIST
(
  ClockDrvCtxt  *pDrvCtxt,
  boolean       *bBISTPassed,
  uint32        *nFailedTests
);


/* =========================================================================
**  Function : Clock_FindClockConfig
** =========================================================================*/
/**
  Finds a particular clock configuration in the BSP.

  This function finds a particular clock configuration in the BSP based on
  the specified frequency and match criteria.

  @param *pDomain[in] -- Pointer to a clock domain.
  @param nFreqHz [in] -- Frequency in Hz
  @param eMatch [in] -- Match criteria
  @param **pMatchingConfig [out] -- Matching configuration

  @return
  DAL_SUCCESS -- A matching configuration was found.
  DAL_ERROR -- A matching configuration was not found.

  @dependencies
  None.
*/

DALResult Clock_FindClockConfig
(
  ClockDomainNodeType  *pDomain,
  uint32                nFreqHz,
  ClockFrequencyType    eMatch,
  ClockMuxConfigType  **pMatchingConfig
);


/* =========================================================================
**  Function : Clock_SetClockConfig
** =========================================================================*/
/**
  Sets a clock configuration.

  This function configures a clock domain to a particular configuration.
  It is used internally only.

  @param *pDrvCtxt [in] -- Driver context.
  @param *pClockDomain [in] -- Clock domain to configure.
  @param *pNewConfig [in] -- New configuration to use.

  @return
  DAL_SUCCESS -- The domain was successfully configured.
  DAL_ERROR -- The domain configuration failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_SetClockConfig
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pDomain,
  ClockMuxConfigType  *pNewConfig
);


/* =========================================================================
**  Function : Clock_InitSources
** =========================================================================*/
/**
  Initialize clock sources.

  This function allocates memory for the clock source nodes and initializes
  them based on the BSP/XML data.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitSources
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_EnableSourceInternal
** =========================================================================*/
/**
  Requests to enable a source.

  This function enables a source, generally as a result of a call to
  Clock_EnableClockDomain().  CPU based source requests are tracked
  independently of non-CPU based as the former will be automatically
  released when the CPU goes to sleep.
  A reference count is maintained so callers must ensure that all Enables
  and Disables are matched.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param bSuppressibleRequest [in] -- TRUE if this is a source request is for a suppressible clock.

  @return
  DAL_SUCCESS -- The source was successfully enabled.
  DAL_ERROR -- The source was not enabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_EnableSourceInternal
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bSuppressibleRequest
);


/* =========================================================================
**  Function : Clock_DisableSourceInternal
** =========================================================================*/
/**
  Disables a source.

  This function disables a source, generally as a result of a call to
  Clock_DisableClockDomain().
  A reference count is maintained so callers must ensure that all Enables
  and Disables are matched.

  @param *pDrvCtxt [in]             -- Handle to the DAL driver context.
  @param *pSource [in]              -- Pointer to a source node.
  @param  bSuppressibleRequest [in] -- TRUE if this is a source request is for a suppressible clock.
  @param  bDoNotDisable [in]        -- TRUE if this is a source request that should not disable the source.

  @return
  DAL_SUCCESS -- The source was successfully disabled.
  DAL_ERROR -- The source was not disabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_DisableSourceInternal
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bSuppressibleRequest,
  boolean              bDoNotDisable
);


/* =========================================================================
**  Function : Clock_FindSourceConfig
** =========================================================================*/
/**
  Finds a particular source configuration in the BSP.

  This function finds a particular source configuration in the BSP based on
  the specified frequency and match criteria.

  @param *pSource [in] -- Pointer to a source node.
  @param nFreqHz [in] -- Frequency in Hz
  @param eMatch [in] -- Match criteria
  @param **pMatchingConfig [out] -- Matching configuration

  @return
  DAL_SUCCESS -- A matching configuration was found.
  DAL_ERROR -- A matching configuration was not found.

  @dependencies
  None.
*/
DALResult Clock_FindSourceConfig
(
  ClockSourceNodeType        *pSource,
  uint32                      nFreqHz,
  ClockFrequencyType          eMatch,
  const ClockSourceFreqConfigType **pMatchingConfig
);


/* =========================================================================
**  Function : Clock_ConfigSource
** =========================================================================*/
/**
  Configures a source.

  This function configures a source, generally a PLL.  This can only be
  done if there are no users of the source and we own the PLL.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param *pConfig [in] -- New configuration.

  @return
  DAL_SUCCESS -- The source was successfully configures.
  DAL_ERROR -- The source was not configured, either due to invalid
               parameter or (more likely) the source was in use.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_ConfigSource
(
  ClockDrvCtxt               *pDrvCtxt,
  ClockSourceNodeType        *pSource,
  const ClockSourceFreqConfigType  *pConfig
);


/* =========================================================================
**  Function : Clock_DetectSourceConfig
** =========================================================================*/
/**
  Detect a source configuration.

  This function detects a source configuration by reading the HW state
  and matching it with the BSP data. It will set the various bookkeeping
  fields to match the detected configuration.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.

  @return
  DAL_SUCCESS -- The source configuration was successfully detected.
  DAL_ERROR -- The source configuration was not found.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_DetectSourceConfig
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource
);

/* =========================================================================
**  Function : Clock_InitSource
** =========================================================================*/
/**
  Configures a source.

  This function initializes a source, generally a PLL, from the current BSP
  data.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param  eSource  [in] -- Which source to initialize.
  @param  nFreqHz  [in] -- Optional frequency to init this source to.
                           The first source frequency config is used if
                           this argument is 0.

  @return
  DAL_SUCCESS -- The source was successfully initialized.
  DAL_ERROR -- The source was not initialized, generally because the source
               has no valid BSP data.

  @dependencies
  The clock mutex should be locked prior to calling this function or
  we should be a single execution thread environment (like boot).
*/

DALResult Clock_InitSource
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockSourceNodeType *pSource,
  uint32              nFreqHz
);


/* =========================================================================
**  Function : Clock_GetPropertyValue
** =========================================================================*/
/**
  Retrieve a clock driver property from the environment store.

  This function retrieves the given clock property from the relevant
  storage location (may be DAL properties DB or another source).

  @param *szName [in] -- Name of the property to look up
  @param *pValue [out] -- Location to store the result

  @return
  DAL_SUCCESS if property was found and retrieved, DAL_ERROR otherwise.

  @dependencies
  None.
*/

DALResult Clock_GetPropertyValue
(
  const char              *szName,
  ClockPropertyValueType  *pValue
);


/* =========================================================================
**  Function : Clock_MapHWIORegion
** =========================================================================*/
/**
  Map a HWIO region into memory for the clock driver.

  This function maps a physical HWIO region into memory for the clock driver
  using environment appropriate APIs.  If the mapping is not available
  the virtual address will be set to the provided physical address.

  @param nPhysAddr [in] -- Physical address.
  @param nSizeBytes [in] -- Size of the region in bytes.
  @param *pnVirtAddr [out] -- Location to store the mapped address.

  @return
  DAL_SUCCESS if mapping was ok, otherwise error code based on failure.

  @dependencies
  None.
*/

DALResult Clock_MapHWIORegion
(
  uintnt  nPhysAddr,
  uint32  nSizeBytes,
  uintnt *pnVirtAddr
);


/* =========================================================================
**  Function : Clock_MapAddress
** =========================================================================*/
/**
  Map a single address to its virtual representation.

  This function maps a single address to its virtual representation.

  @param *pDrvCtxt [in]   -- Handle to the DAL driver context.
  @param *pnAddr [in/out] -- Pointer to physical address to map. Updated
                             to mapped virtual address.

  @return
  DAL_SUCCESS if mapping was ok, otherwise error code based on failure.

  @dependencies
  None.
*/

DALResult Clock_MapAddress
(
  ClockDrvCtxt *pDrvCtxt,
  uintnt       *pnAddr
);


/* =========================================================================
**  Function : Clock_InitVoltage
** =========================================================================*/
/**
  Initialize voltage tracking subsystem.

  This function prepares the voltage tracking module for managing voltage
  requests.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitVoltage
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_VoltageRequest
** =========================================================================*/
/**
  Make a voltage regulator request.

  This function is used internally for a clock domain to make a voltage
  regulator level request.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pCurrentRequest [in/out] -- Pointer to currently requested voltage config.
                                      Updated to pNewRequest.
  @param *pNewRequest [in] -- New voltage requirement.

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR -- Request failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_VoltageRequest
(
  ClockRailType         *pRail,
  ClockVRegRequestType  *pCurrentRequest,
  ClockVRegRequestType  *pNewRequest
);


/* =========================================================================
**  Function : Clock_VoltageSuspend
** =========================================================================*/
/**
  Make a request to suspend voltage requests.

  This function is used internally to suspend voltage requests.

  @param *pDrvCtxt [in]       -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR   -- Request failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_VoltageSuspend
(
  ClockRailType         *pRail
);


/* =========================================================================
**  Function : Clock_VoltageResume
** =========================================================================*/
/**
  Make a request to resume voltage requests.

  This function is used internally to resume voltage requests.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param bUpdate [in] -- Flag to update voltage request immediately upon resume.

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR   -- Request failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_VoltageResume
(
  ClockRailType         *pRail,
  boolean        bUpdate
);


/*=========================================================================
**  Function : Clock_IsBSPSupported
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

boolean Clock_IsBSPSupported
(
  const ClockHWVersionType *pHWVersion
);


/*=========================================================================
**  Function : Clock_GetDrvCtxt
** =========================================================================*/
/**
  Returns driver context.

  This function is used to get the driver context when called from the lite
  API call path.  The mutex shared with the lite API is taken.


  @return
  Pointer to clock driver context.

  @dependencies
  None.
*/

ClockDrvCtxt *Clock_GetDrvCtxt
(
  void
);


/*=========================================================================
**  Function : Clock_ReleaseDrvCtxt
** =========================================================================*/
/**
  This function is used to release the driver context when called from the lite
  API call path.  The mutex shared with the lite API is released.

  @return
  None.

  @dependencies
  None.
*/

void Clock_ReleaseDrvCtxt
(
  void
);


/*=========================================================================
**  Function : Clock_TransferReferenceCount
** =========================================================================*/
/**
  This function is used to transfer the reference count of lite API enable
  requests to the DAL driver.

  @return
  DAL_SUCCESS -- Successfully transferred reference count.
  DAL_ERROR_INTERNAL -- Failed to successfully enable clocks via DAL API.

  @dependencies
  None.
*/

DALResult Clock_TransferReferenceCount
(
  void
);


/*=========================================================================
**  Function : Clock_GetDrvCtxtNoLock
** =========================================================================*/
/**

  API to retrieve the static driver context without causing a mutex lock
  in the process.

  @dependencies
  None.
*/

ClockDrvCtxt *Clock_GetDrvCtxtNoLock
(
  void
);


/*=========================================================================
**  Function : Clock_GetInitialClientCtxt
** =========================================================================*/
/**

  Internal clock API to retrieve the first (root) client context for use
  in APIs that need a client context but have only a driver context.

  @dependencies
  None.
*/

ClockDALClientCtxt *Clock_GetInitialClientCtxt
(
  void
);


/*=========================================================================
**  Function : Clock_GetMappedSource
** =========================================================================*/
/**
  Map mux select value to internal source node.

  @return
  DAL_SUCCESS -- Mapped source was found.
  DAL_ERROR -- No mapped source found.

  @dependencies
  None.
*/

DALResult Clock_GetMappedSource
(
  const ClockSourceMapType   *aSourceMap,
  uint32                      nMuxSel,
  const ClockSourceMapType  **pMatchingSourceMap
);


/*=========================================================================
**  Function : Clock_GetRequiredCorner
** =========================================================================*/
/**
  Get required corner for frequency.

  @return
  DAL_SUCCESS -- Required corner found.
  DAL_ERROR -- No suitable corner found.

  @dependencies
  None.
*/

DALResult Clock_GetRequiredCorner
(
  const ClockFMaxType *aFMax,
  uint32               nFreqHz,
  rail_voltage_level  *peVRegLevel
);


/*=========================================================================
**  Function : Clock_SetThreadPriority
** =========================================================================*/
/**
  Set priority of current thread.

  @param nPriority[in]      - Thread priority to set.
  @param nPrevPriority[out] - Original priority before setting new one.

  @dependencies
  None.
*/

void Clock_SetThreadPriority
(
  uint32  nPriority,
  uint32 *nPrevPriority
);


/*=========================================================================
**  Function : Clock_AssignNPAState
** =========================================================================*/
/**
  Invoke an NPA driver function and assign the resource state.

  This function calls an NPA node driver function with nRequest, and
  assigns the resource state to the return value. This function bypasses
  the NPA aggregation layer and can be used to override the current request
  state of a node.

  @param *pResource [in] -- Pointer to NPA resource to assign.
  @param nRequest [in] -- New request state.

  @return
  npa_resource_state -- Active state of the resource.

  @dependencies
  The NPA node pointed to by pResource must have already been created.
*/

npa_resource_state Clock_AssignNPAState
(
  npa_resource        *pResource,
  npa_resource_state   nRequest
);


/*=========================================================================
**  Function : Clock_RestoreNPAState
** =========================================================================*/
/**
  Invoke an NPA driver function and assign the current request state.

  This function calls an NPA node driver function with the current aggregated
  request state, and assigns the resource state to the return value.

  @param *pResource [in] -- Pointer to NPA resource to assign.

  @return
  npa_resource_state -- Active state of the resource.

  @dependencies
  The NPA node pointed to by pResource must have already been created.
*/

npa_resource_state Clock_RestoreNPAState
(
  npa_resource *pResource
);


/*=========================================================================
**  Function : Clock_GetClockNode
** =========================================================================*/
/**
  Return pointer to clock node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Clock identifier to look up.

  @return
  ClockNodeType* if found; NULL otherwise.
*/

ClockNodeType *Clock_GetClockNode
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nId
);


/*=========================================================================
**  Function : Clock_GetSourceNode
** =========================================================================*/
/**
  Return pointer to source node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Source identifier to look up.

  @return
  ClockSourceNodeType* if found; NULL otherwise.
*/

ClockSourceNodeType *Clock_GetSourceNode
(
  ClockDrvCtxt  *pDrvCtxt,
  SourceIdType   nId
);


/*=========================================================================
**  Function : Clock_GetPowerDomainNode
** =========================================================================*/
/**
  Return pointer to power domain node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Power domain identifier to look up.

  @return
  ClockPowerDomainNodeType* if found; NULL otherwise.
*/

ClockPowerDomainNodeType *Clock_GetPowerDomainNode
(
  ClockDrvCtxt            *pDrvCtxt,
  ClockPowerDomainIdType   nId
);


/*=========================================================================
**  Function : Clock_GetDebugMuxNode
** =========================================================================*/
/**
  Return pointer to debug mux node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Debug mux identifier to look up.

  @return
  ClockDebugMuxType* if found; NULL otherwise.
*/

ClockDebugMuxType *Clock_GetDebugMuxNode
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDebugMuxIdType  nId
);


/*=========================================================================
**  Function : Clock_GetDividerNode
** =========================================================================*/
/**
  Return pointer to clock node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Divider identifier to look up.

  @return
  ClockDividerNodeType* if found; NULL otherwise.
*/

ClockDividerNodeType *Clock_GetDividerNode
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDividerIdType   nId
);


/*=========================================================================
      Function Definitions From DDIClock.h
==========================================================================*/

DALResult Clock_GetClockId(ClockDrvCtxt *pDrvCtxt, const char *szClock,  ClockIdType *pnId);
DALResult Clock_EnableClock(ClockDALClientCtxt *pClientCtxt, ClockNodeType *pClock);
DALResult Clock_DisableClock(ClockDALClientCtxt *pClientCtxt, ClockNodeType *pClock);
DALResult Clock_SetClockFrequency(ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock, uint32 nFreq, ClockFrequencyType eMatch, uint32 *pnResultFreq);
DALResult Clock_SetClockDivider(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, uint32 nDivider);
DALResult Clock_InvertClock(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, boolean  bInvert);
DALResult Clock_ResetClock(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockResetType eReset);
DALResult Clock_ResetClock_STM( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  ClockResetType  eReset);
DALResult Clock_ResetClockBranch(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockResetType eReset);
DALResult Clock_ConfigClock(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockConfigType eConfig);
DALResult Clock_IsClockEnabled(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, boolean *pbIsOn);
DALResult Clock_IsClockOn(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, boolean *pbIsOn);
DALResult Clock_GetClockFrequency(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, uint32 *pnFrequencyHz);
DALResult Clock_SelectExternalSource(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, uint32 nFreqHz, uint32 nSource, uint32 nDiv2x, uint32 nM, uint32 nN, uint32 n2D);
DALResult Clock_CalcClockFrequency(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, uint32 *pnFrequencyHz);
DALResult Clock_LogState(ClockDrvCtxt *pDrvCtxt, ULogHandle hULog, uint32 nFlags);
DALResult Clock_GetFrequencyPlan ( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockFreqPlanType **aFreqPlan, uint32 *nSize );
DALResult Clock_GetClockInfo ( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockInfoType *pClockInfo );

DALResult Clock_GetDebugMuxId(ClockDrvCtxt *pDrvCtxt, const char *szDebugMux, ClockDebugMuxIdType *pnId);
DALResult Clock_CalcFrequency(ClockDrvCtxt *pDrvCtxt, ClockDebugMuxType *pDebugMux, uint32 nMuxSel, uint32 *pnFrequencyHz);

DALResult Clock_GetSourceId(ClockDrvCtxt *pDrvCtxt, const char *szSource, SourceIdType *pnId);
DALResult Clock_EnableSource(ClockDALClientCtxt *pClientCtxt, ClockSourceNodeType *pSource);
DALResult Clock_DisableSource(ClockDALClientCtxt *pClientCtxt, ClockSourceNodeType *pSource);
DALResult Clock_SetSourceFrequency(ClockDrvCtxt *pDrvCtxt, ClockSourceNodeType *pSource, uint32 nFreq, ClockFrequencyType eMatch, uint32 *pnResultFreq);
DALResult Clock_GetSourceFrequency(ClockDrvCtxt *pDrvCtxt, ClockSourceNodeType *pSource, uint32 *pnFrequencyHz);
DALResult Clock_EnableDFS(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock);
DALResult Clock_IsDFSEnabled(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, boolean *pbIsEnabled);

DALResult Clock_GetPowerDomainId(ClockDrvCtxt *pDrvCtxt, const char *szPowerDomain, ClockPowerDomainIdType *pnPowerDomainId);
DALResult Clock_EnablePowerDomain(ClockDALClientCtxt *pClientCtxt, ClockPowerDomainNodeType *pPowerDomain);
DALResult Clock_DisablePowerDomain(ClockDALClientCtxt *pClientCtxt, ClockPowerDomainNodeType *pPowerDomain);

DALResult Clock_GetDividerId(ClockDrvCtxt *pDrvCtxt, const char *szDivider,  ClockDividerIdType *pnId);
DALResult Clock_SetDivider(ClockDrvCtxt *pDrvCtxt, ClockDividerNodeType *pDivider, uint32 nDivider);

DALResult Clock_EnableDCVS(ClockDrvCtxt *pDrvCtxt);
DALResult Clock_LoadNV(ClockDrvCtxt *pDrvCtxt);
DALResult Clock_SetCPUMaxFrequencyAtCurrentVoltage( ClockDrvCtxt *pDrvCtxt, boolean bRequired, uint32 *pnResultFreqHz);
DALResult Clock_ProcessorSleep(ClockDrvCtxt *pDrvCtxt, ClockSleepModeType eMode, uint32 nFlags);
DALResult Clock_ProcessorRestore(ClockDrvCtxt *pDrvCtxt, ClockSleepModeType eMode, uint32 nFlags);

#endif /* !__CLOCK_DRIVER_H__ */


