/** @file TLMMApp.c
   
  TLMM Test Application. 
   
  This application tests the TLMM DAL and TLMM DXE interface 
  mechanisms. 

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
07/30/11   sy      Adding TestInterface Library
 01/13/11   dcf     Created.

=============================================================================*/


#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <DDITlmm.h>
#include <DALDeviceId.h>
#include <DalDevice.h>
#include <DALSys.h>
#include <Protocol/EFITlmm.h>

/**
  Handle to the DAL TLMM Device context. 
 */
DalDeviceHandle * tlmm_handle = NULL;


/** 
  Handle to the TLMM DXE protocol API. 
 */
EFI_TLMM_PROTOCOL * TLMMProtocol;


/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.  This application 
  is presented to test the mechanism for which to manipulate 
  GPIO.  It should only be used in test scenarios as it affects 
  the state of the HW.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
TLMMAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  DALResult result = DAL_ERROR;
  EFI_STATUS status;

  TEST_START("TLMM");
  /* Create a DAL-formatted GPIO configuration for testing. */
  DALGpioSignalType config = 
    (DALGpioSignalType)DAL_GPIO_CFG(11, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA); //GPIO-11 is used for testing the app
  
  /* Initialize DALSYS and attach to the DAL TLMM driver. */
  DALSYS_InitMod(NULL); 
  result = DAL_DeviceAttach(DALDEVICEID_TLMM, &tlmm_handle);

  if((DAL_ERROR == result) || (tlmm_handle == NULL))
  {
    ASSERT(0);
  }

  /* Access the TLMM protocol. */
  status = gBS->LocateProtocol( &gEfiTLMMProtocolGuid, NULL, (void**)&TLMMProtocol);

  ASSERT_EFI_ERROR(status);

  /* Test the DAL TLMM driver interface. This application does not have a way to
     verify that the HW has been affected.  The purpose is to test the interface
     mechanism.  Only probing the physical device or examining the contents of
     HW registers can the actual functionality be fully verified. */

  /* Configure the GPIO. */
  result = DalTlmm_ConfigGpio(tlmm_handle, config, DAL_TLMM_GPIO_ENABLE);

  if(DAL_SUCCESS == result)
  {
    /* Drive the GPIO HIGH. */
    result = DalTlmm_GpioOut(tlmm_handle, config, DAL_GPIO_HIGH_VALUE);

    if(result == DAL_SUCCESS)
    {
      /* Drive the GPIO LOW */
      result = DalTlmm_GpioOut(tlmm_handle, config, DAL_GPIO_LOW_VALUE);
    }
    if(result != DAL_SUCCESS)
    {
      ASSERT(0);
    }
  }

  /* Test EFI wrapper interface. */
  if(TLMMProtocol)
  {
    TLMMProtocol->ConfigGpio(
     (UINT32)EFI_GPIO_CFG(11, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), 
     TLMM_GPIO_ENABLE);

    TLMMProtocol->GpioOut(
     (UINT32)EFI_GPIO_CFG(11, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), 
     GPIO_HIGH_VALUE);

    TLMMProtocol->GpioOut(
     (UINT32)EFI_GPIO_CFG(11, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), 
     GPIO_LOW_VALUE);
  }
  TestStatus("TLMM",EFI_SUCCESS);
  TEST_STOP("TLMM");
  return EFI_SUCCESS;

} /* TLMMAppMain */

