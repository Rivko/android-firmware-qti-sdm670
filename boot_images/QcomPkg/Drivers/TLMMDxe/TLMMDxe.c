/** @file

  Copyright (c) 2011-2014, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

  TLMM 
**/
/*===========================================================================

             M A I N   D A L   T L M M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the DAL TLMM driver.  The
  API in this module MUST always remain target independent.


===========================================================================
             Copyright (c) 2010-2015 Qualcomm Technologies, Inc.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/14   sho     Use DAL configuration to register devices
05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
01/06/11   dcf     Created.
==============================================================================*/

/*==============================================================================

                       INCLUDE FILES FOR THIS MODULE

==============================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include <DALDeviceId.h>
#include <DalDevice.h>
#include <DDITlmm.h>
#include <Protocol/EFITlmm.h>


/*==============================================================================

                         DEFINES FOR THIS MODULE

==============================================================================*/


/*==============================================================================

                     EXTERNAL DEFINES FOR THIS MODULE

==============================================================================*/

extern DALResult TLMM_DalTlmm_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice);




/*==============================================================================

                      PROTOTYPES USED IN THIS MODULE

==============================================================================*/

EFI_STATUS
EFI_TLMM_ConfigGpio(IN UINT32 config, IN UINT32 enable);

EFI_STATUS
EFI_TLMM_ConfigGpioGroup(IN UINT32 enable, IN UINT32 *config_group, IN UINT32 size);

EFI_STATUS
EFI_TLMM_GpioIn(IN UINT32 config, OUT UINT32 *value);

EFI_STATUS
EFI_TLMM_GpioOut(IN UINT32 config, IN UINT32 value);

EFI_STATUS
EFI_TLMM_SetInactiveConfig(IN UINT32 gpio_number, IN UINT32 config);




/*==============================================================================

                     GLOBAL VARIABLES FOR THIS MODULE

==============================================================================*/


/*
 * DALTLMM interface handle.  This must be initialized before accessing any 
 * DALTLMM APIs. 
 */

DalDeviceHandle * tlmm_handle;



/*
 * For dynamic device loading the DALSYSModConfig structure is populated 
 * by this module. 
 */

static DALDEVICEID DalTlmm_DeviceId[1] = { DALDEVICEID_TLMM };
static DALREG_DriverInfo DALTLMM_DriverInfo = { TLMM_DalTlmm_Attach, 1, DalTlmm_DeviceId };
static DALREG_DriverInfo * DALDriverInfoArr[1] = { &DALTLMM_DriverInfo };

static DALSYSConfig DALSYSModConfig = 
   {
      {0, NULL},              // string device info
      {1, DALDriverInfoArr}   // numeric driver info
   };


/*
 * Function table for DAL wrapper APIs.
*/

static EFI_TLMM_PROTOCOL TLMMProtocol = 
{
  EFI_TLMM_PROTOCOL_REVISION,
  EFI_TLMM_ConfigGpio,
  EFI_TLMM_ConfigGpioGroup,
  EFI_TLMM_GpioIn,
  EFI_TLMM_GpioOut,
  EFI_TLMM_SetInactiveConfig
};



/*==============================================================================

                             API IMPLEMENTATION

==============================================================================*/


/*==============================================================================

  FUNCTION      EFI_TLMM_ConfigGpio

  DESCRIPTION   See EFI_TLMM_CONFIG_GPIO in Protocol/TLMM.h

==============================================================================*/

EFI_STATUS
EFI_TLMM_ConfigGpio(IN UINT32 config, IN UINT32 enable)
{
  DALResult result = DAL_ERROR;
  DALGpioEnableType dal_enable = DAL_TLMM_GPIO_DISABLE;

  if(tlmm_handle)
  {
    if(enable != 0)
    {
      dal_enable = DAL_TLMM_GPIO_ENABLE;
    }
    result = DalTlmm_ConfigGpio(tlmm_handle, config, dal_enable);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_ConfigGpio */



/*==============================================================================

  FUNCTION      EFI_TLMM_ConfigGpioGroup

  DESCRIPTION   See EFI_TLMM_CONFIG_GPIO_GROUP in Protocol/TLMM.h

==============================================================================*/ 

EFI_STATUS
EFI_TLMM_ConfigGpioGroup(IN UINT32 enable, IN UINT32 *config_group, IN UINT32 size)
{
  DALResult result = DAL_ERROR;
  DALGpioEnableType dal_enable = DAL_TLMM_GPIO_DISABLE;

  if(tlmm_handle)
  {
    if(enable != 0)
    {
      dal_enable = DAL_TLMM_GPIO_ENABLE;
    }
    result = DalTlmm_ConfigGpioGroup(tlmm_handle, dal_enable, (uint32*)config_group, size);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_ConfigGpioGroup */



/*==============================================================================

  FUNCTION      EFI_TLMM_GpioIn

  DESCRIPTION   See EFI_TLMM_GPIO_IN in Protocol/TLMM.h

==============================================================================*/ 

EFI_STATUS
EFI_TLMM_GpioIn(IN UINT32 config, OUT UINT32 *value)
{
  DALResult result = DAL_ERROR;
  DALGpioValueType dal_value;

  if(tlmm_handle)
  {
    result = DalTlmm_GpioIn(tlmm_handle, config, &dal_value);

    if(dal_value == DAL_GPIO_LOW_VALUE)
    {
      *value = 0;
    }
    else
    {
      *value = 1;
    }
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_GpioIn */



/*==============================================================================

  FUNCTION      EFI_TLMM_GpioOut

  DESCRIPTION   See EFI_TLMM_GPIO_OUT in Protocol/TLMM.h

==============================================================================*/ 

EFI_STATUS
EFI_TLMM_GpioOut(IN UINT32 config, IN UINT32 value)
{
  DALResult result = DAL_ERROR;
  DALGpioValueType dal_value = DAL_GPIO_LOW_VALUE;
  if(tlmm_handle)
  {
    if( value != 0)
    {
      dal_value = DAL_GPIO_HIGH_VALUE;
    }
    result = DalTlmm_GpioOut(tlmm_handle, config, dal_value);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_GpioOut */



/*==============================================================================

  FUNCTION      EFI_TLMM_SetInactiveConfig

  DESCRIPTION   See EFI_TLMM_SET_INACTIVE_CONFIG in Protocol/TLMM.h

==============================================================================*/ 

EFI_STATUS
EFI_TLMM_SetInactiveConfig(IN UINT32 gpio_number, IN UINT32 config)
{
  DALResult result = DAL_ERROR;
  if(tlmm_handle)
  {
    result = DalTlmm_SetInactiveConfig(tlmm_handle, gpio_number, config);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_SetInactiveConfig */



/*==============================================================================

  FUNCTION      DALTLMMEntryPoint

  DESCRIPTION   EFI DAL-Wrapper Entry point for TLMM/GPIO access.

==============================================================================*/ 

EFI_STATUS
EFIAPI
TLMMEntryPoint (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_HANDLE  handle = NULL;
  EFI_STATUS  status;
  DALResult result = DAL_ERROR;

  DALSYS_InitMod(&DALSYSModConfig);

  result = DAL_DeviceAttach(DALDEVICEID_TLMM, &tlmm_handle);

  if(DAL_SUCCESS == result)
  {
    status = gBS->InstallMultipleProtocolInterfaces(&handle, &gEfiTLMMProtocolGuid, 
                                                    (void **)&TLMMProtocol, NULL, NULL, NULL );
    ASSERT_EFI_ERROR(status);
    DalDevice_Open(tlmm_handle, DAL_OPEN_SHARED);
  }
  else
  {
    DAL_DeviceDetach(handle);
    DALSYS_DeInitMod();
    status = EFI_UNSUPPORTED;
  }

  return status;

} /* DALTLMMEntryPoint */
