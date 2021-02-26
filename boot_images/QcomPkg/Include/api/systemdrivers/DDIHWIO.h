#ifndef DDIHWIO_H
#define DDIHWIO_H
/*
===========================================================================
*/
/**
  @file DDIHWIO.h

  Public interface include file for accessing the HWIO DAL device driver.

  The DDIHWIO.h file is the public API interface to the HW I/O (HWIO)
  Device Access Library (DAL).
*/
/*
  ====================================================================

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/


/*
 * DAL system header.
 */
#include "DALSys.h"

/*
 * HAL HWIO base pointer declaration macros:
 *   HWIO_BASE_PTR(base)
 *   DECLARE_HWIO_BASE_PTR(base)
 */
#include "HALhwio.h"



/*=========================================================================
      Type Definitions
==========================================================================*/

/**
  @addtogroup macros
  @{
*/

/**
 * Macro that defines the interface version.
 */
#define DALHWIO_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/**
  @}
*/


/**
  @addtogroup functions
  @{
*/

/* ============================================================================
**  Function Macro : DALHWIO_MAP_REGION
** ============================================================================*/
/**
  Standard HWIO memory mapping macro.

  This macro is the recommended method to map a normal HWIO region for
  use by HWIO macros (HWIO_IN, HWIO_OUT, etc).

  @param pHandle [in] -- Handle to the HWIO DAL.
  @param base    [in] -- Name of the base to map, i.e. CLK_CTL.  Will be
                         used to generate the string name of the base as
                         well as the name of the pointer to fill in.

  @sa
  DalHWIO_MapRegion
*/

#define DALHWIO_MAP_REGION(pHandle, base) \
  DalHWIO_MapRegion(pHandle, #base, &HWIO_BASE_PTR(base))


/* ============================================================================
**  Function Macro : DALHWIO_UNMAP_REGION
** ============================================================================*/
/**
  Standard HWIO memory un-mapping macro.

  This macro is the recommended method to unmap a normal HWIO region used
  by HWIO macros.  This macro will handle HWIO base region pointers that were
  weakly declared and not present in the final image (which will happen for
  a base pointer declared in AMSS for a base that does not exist on the
  target).

  Note on implementation: The use of a redirection function DerefWeakPointer
  in this macro is required to avoid compilation warnings about a
  conditional comparison being a constant (since the address of the pointer
  will always be fixed for a given build).  The check is required to handle
  weakly declared pointers (used in AMSS) that may not be present in the
  final image - this actually results in ppAddress being NULL (since the
  address of a weakly declared object will be NULL if never fully declared).

  @param pHandle [in] -- Handle to the HWIO DAL.
  @param base    [in] -- Name of the base to unmap, i.e. CLK_CTL.  Will be
                         used to generate the string name of the base as
                         well as the name of the pointer to fill in.

  @sa
  DalHWIO_UnMapRegion
*/

/** @cond */

static __inline uint8 *DalHWIO_DerefWeakPointer(uint8 **ppAddress)
{
  return (ppAddress != NULL ? *ppAddress : NULL);
}

/** @endcond */

#define DALHWIO_UNMAP_REGION(pHandle, base)  \
  DalHWIO_UnMapRegion(pHandle, DalHWIO_DerefWeakPointer(&HWIO_BASE_PTR(base)))


/*=========================================================================
      Interface Definitions - Internally used
==========================================================================*/

/** @cond */

typedef struct DalHWIO DalHWIO;
struct DalHWIO
{
   struct DalDevice DalDevice;
   DALResult (*MapRegion)(DalDeviceHandle * _h, const char * szBase, uint8 **ppAddress);
   DALResult (*UnMapRegion)(DalDeviceHandle * _h, uint8 *pVirtAddress);
   DALResult (*MapRegionByAddress)(DalDeviceHandle * _h, uint8 *pPhysAddress, uint8 **ppAddress);
   DALResult (*MapAllRegions)(DalDeviceHandle * _h);
};

typedef struct DalHWIOHandle DalHWIOHandle;
struct DalHWIOHandle
{
   uint32         dwDalHandleId;
   const DalHWIO *pVtbl;
   void          *pClientCtxt;
};

#define DAL_HWIODeviceAttach(DevId,hDalDevice) \
        DAL_DeviceAttachEx(NULL,DevId,DALHWIO_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/*=========================================================================
      Interface Declarations
==========================================================================*/


/* ============================================================================
**  Function : DalHWIO_MapRegion
** ============================================================================*/
/**
  Maps a HWIO region into virtual memory.

  This function maps a HWIO region into virtual memory and returns a
  pointer to the base.
  For mapping a standard HWIO region to the standar base pointer (i.e. for
  use by the HWIO_* access macros in HALhwio.h) it is recommended to use
  the DALHWIO_MAP_REGION macro.

  @param *_h         [in] -- Handle to the HWIO DAL device.
  @param szBase      [in] -- String name of the base to map, i.e. "CLK_CTL".
                             This string corresponds to a BASE definition
                             in the address file.  To map a MODULE definition
                             prefix the string with a "+", i.e. "+TLMM".
  @param **ppAddress [out] -- Pointer to fill in with the virtual address.

  @return
  DAL_SUCCESS -- Memory was mapped successfully, and ppAddress will contain
                 a pointer to the virtual address.\n
  DAL_ERROR -- Base was not valid for this target, or ppAddress was NULL.

  @dependencies
  None.

  @sa
  DALHWIO_MAP_REGION
*/

static __inline DALResult DalHWIO_MapRegion
(
  DalDeviceHandle *_h,
  const char      *szBase,
  uint8          **ppAddress
)
{
  return ((DalHWIOHandle *)_h)->pVtbl->MapRegion( _h, szBase, ppAddress);
}


/* ============================================================================
**  Function : DalHWIO_MapRegionByAddress
** ============================================================================*/
/**
  Maps a HWIO region into virtual memory using the physical address.

  This function maps a HWIO region into virtual memory using the required
  physical address and returns a pointer to the base.

  @param *_h           [in] -- Handle to the HWIO DAL device.
  @param *pPhysAddress [in] -- Physical address of the base to map.
  @param **ppAddress   [out] -- Pointer to fill in with the virtual address.

  @return
  DAL_SUCCESS -- Memory was mapped successfully, and ppAddress will contain
                 a pointer to the virtual address.\n
  DAL_ERROR -- Base was not valid for this target, or ppAddress was NULL.

  @dependencies
  None.

  @sa
  DalHWIO_MapRegion
*/

static __inline DALResult DalHWIO_MapRegionByAddress
(
  DalDeviceHandle *_h,
  uint8           *pPhysAddress,
  uint8          **ppAddress
)
{
  return
    ((DalHWIOHandle *)_h)->pVtbl->MapRegionByAddress(
      _h, pPhysAddress, ppAddress);
}


/* ============================================================================
**  Function : DalHWIO_UnMapRegion
** ============================================================================*/
/**
  Unmaps a HWIO region into virtual memory.

  This function unmaps a HWIO base region in virtual memory.  If other
  users have also mapped the region it will remain mapped until all users
  have called UnMapRegion.
  For unmapping a standard HWIO region to the standar base pointer (i.e. for
  use by the HWIO_* access macros in HALhwio.h) it is recommended to use
  the DALHWIO_UNMAP_REGION macro.  This macro will handle the case of a
  weakly-linked base pointer that does not really exist.

  @param *_h           [in] -- Handle to the HWIO DAL device.
  @param *pVirtAddress [in] -- Pointer to the virtual address to un-map.

  @return
  DAL_SUCCESS -- Memory was unmapped successfully.\n
  DAL_ERROR -- Address was not valid.

  @dependencies
  None.

  @sa
  DALHWIO_UNMAP_REGION
*/

static __inline DALResult DalHWIO_UnMapRegion
(
  DalDeviceHandle *_h,
  uint8           *pVirtAddress
)
{
  return ((DalHWIOHandle *)_h)->pVtbl->UnMapRegion( _h, pVirtAddress);
}


/* ============================================================================
**  Function : DalHWIO_MapAllRegions
** ============================================================================*/
/**
  Maps all HWIO regions into virtual memory.

  This function maps all HWIO regions into virtual memory.  It is only
  used in environments that support fixed virtual->physical HWIO mappings
  but require OS calls at boot to create them.
  This function should only be called once by the system loader, prior
  to any use of HWIO regions.

  @param *_h           [in] -- Handle to the HWIO DAL device.

  @return
  DAL_SUCCESS -- All memory regions were mapped successfully.
  DAL_ERROR -- At least one memory mapping failed.

  @dependencies
  None.

  @sa
  DalHWIO_MapRegion
*/

static __inline DALResult DalHWIO_MapAllRegions
(
  DalDeviceHandle *_h
)
{
  return ((DalHWIOHandle *)_h)->pVtbl->MapAllRegions(_h);
}


/**
  @}
*/ /* end_group Functions */

#endif /* DDIHWIO_H */

