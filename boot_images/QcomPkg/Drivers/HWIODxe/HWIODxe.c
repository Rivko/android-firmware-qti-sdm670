/** @file HWIODxe.c
   
  This file implements HWIO EFI protocol interface.

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/16/14   sho     Use DAL configuration to register devices
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 03/24/11   gfr     Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include <Protocol/EFIHWIO.h>
#include "DALDeviceId.h"
#include "DDIHWIO.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Function Prototypes
==========================================================================*/

EFI_STATUS EFI_MapRegion(IN EFI_HWIO_PROTOCOL *This,
                         IN CONST CHAR8 *Base, OUT UINT8 **Address);
EFI_STATUS EFI_MapRegionByAddress(IN EFI_HWIO_PROTOCOL *This,
                         IN UINT8 *AddressPhys, OUT UINT8 **Address);
EFI_STATUS EFI_UnMapRegion(IN EFI_HWIO_PROTOCOL *This,
                         IN UINT8 *Address);


/*=========================================================================
      Data Declarations
==========================================================================*/

// HWIO Public Protocol 
static EFI_HWIO_PROTOCOL HWIOProtocol =
{
   EFI_HWIO_PROTOCOL_VERSION,
   EFI_MapRegion,
   EFI_MapRegionByAddress,
   EFI_UnMapRegion
};

static DalDeviceHandle *hHWIO = NULL;

/* 
 * Defined in DalHWIOFwk.c
 */
extern DALREG_DriverInfo DALHWIO_DriverInfo;

/* 
 * DALSYSModConfig needs to be created for WM and UEFI - dynamic 
 * loading of DAL drivers. This will not work for AMSS image (static loading 
 * via DAL based XML files at compile time) so the following can't go in 
 * DalHWIOFwk.c which is common across images 
 * 
 */ 
static DALREG_DriverInfo* DALDriverInfoArr[1] = { &DALHWIO_DriverInfo};
static DALSYSConfig DALSYSModConfig = 
   {
      {0, NULL},              // string device info
      {1, DALDriverInfoArr}   // numeric driver info
   };




/*=========================================================================
      Functions
==========================================================================*/


/**
  Maps a HWIO region into virtual memory.
 
  This function maps a HWIO region into virtual memory and returns a
  pointer to the base.
 
  @param This        [in] -- The EFI_HWIO_PROTOCOL instance.
  @param Base        [in] -- String name of the base to map, i.e. "CLK_CTL"
  @param Address     [out] -- Pointer to fill in with the virtual address.

  @return
  EFI_SUCCESS -- Memory was mapped successfully, and Address will contain
                 a pointer to the virtual address.\n
  EFI_PROTOCOL_ERROR -- Base was not valid for this target, or Address was NULL.
*/ 

EFI_STATUS
EFI_MapRegion(
   IN  EFI_HWIO_PROTOCOL *This,
   IN CONST CHAR8 *Base,
   OUT UINT8 **Address)
{
  DALResult eResult;

  eResult = DalHWIO_MapRegion(hHWIO, Base, Address);

  if (eResult == DAL_SUCCESS)
  {
    return EFI_SUCCESS;
  }
  else
  {
    return EFI_PROTOCOL_ERROR;
  }
}


/**
  Maps a HWIO region into virtual memory using the physical address.
 
  This function maps a HWIO region into virtual memory using the required
  physical address and returns a pointer to the base.
 
  @param This        [in] -- The EFI_HWIO_PROTOCOL instance.
  @param AddressPhys [in] -- Base physical address.
  @param Address     [out] -- Pointer to fill in with the virtual address.

  @return
  EFI_SUCCESS -- Memory was mapped successfully, and Address will contain
                 a pointer to the virtual address.\n
  EFI_PROTOCOL_ERROR -- Base was not valid for this target, or Address was NULL.
*/ 

EFI_STATUS
EFI_MapRegionByAddress(
   IN  EFI_HWIO_PROTOCOL *This,
   IN UINT8 *AddressPhys,
   OUT UINT8 **Address)
{
  DALResult eResult;

  eResult = DalHWIO_MapRegionByAddress(hHWIO, AddressPhys, Address);

  if (eResult == DAL_SUCCESS)
  {
    return EFI_SUCCESS;
  }
  else
  {
    return EFI_PROTOCOL_ERROR;
  }
}


/**
  Unmaps a HWIO region into virtual memory.
 
  This function unmaps a HWIO base region in virtual memory.  If other
  users have also mapped the region it will remain mapped until all users
  have called UnMapRegion.
 
  @param This        [in] -- The EFI_HWIO_PROTOCOL instance.
  @param Address     [in] -- Address of region to unmap.

  @return
  EFI_SUCCESS -- Memory was unmapped successfully.
  EFI_PROTOCOL_ERROR -- Address was not valid.
*/ 

EFI_STATUS
EFI_UnMapRegion(
   IN  EFI_HWIO_PROTOCOL *This,
   IN  UINT8 *Address)
{
  DALResult eResult;

  eResult = DalHWIO_UnMapRegion(hHWIO, Address);

  if (eResult == DAL_SUCCESS)
  {
    return EFI_SUCCESS;
  }
  else
  {
    return EFI_PROTOCOL_ERROR;
  }
}


/**
  HWIO DXE driver entry point. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI
HWIODxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_HANDLE  handle = NULL;
  EFI_STATUS  status;
  DALResult   eResult;

  /*
   * Init DALSys
   */
  DALSYS_InitMod(&DALSYSModConfig);

  /*
   * Attach to the HWIO DAL
   */
  eResult = DAL_DeviceAttach(DALDEVICEID_HWIO, &hHWIO);
  if(eResult != DAL_SUCCESS)
  {
    DALSYS_DeInitMod();
    return EFI_PROTOCOL_ERROR;
  }

  /*
   * Publish the HWIO Protocol
   */
  status = gBS->InstallMultipleProtocolInterfaces( 
                  &handle, 
                  &gEfiHwioProtocolGuid, 
                  (void **)&HWIOProtocol, NULL );

  /*
   * Detach and clean up if the install failed.
   */
  if (status != EFI_SUCCESS)
  {
    DAL_DeviceDetach(hHWIO);
    DALSYS_DeInitMod();
  }

  return status;
}


