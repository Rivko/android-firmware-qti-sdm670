#ifndef __DALITLMM_H__
#define __DALITLMM_H__
/**
  @file DDITlmm.h 
  
  Public interface include file for accessing the TLMM DAL device.
  
  The DDITlmm.h file is the public API interface to the Top Level Mode Multiplexor 
  (TLMM) Device Access Library (DAL).
*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/DDITlmm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/16   dcf     Updated driver APIs - Removed obsolete APIs and functionalities.
07/05/10   sw      (Tech Pubs) edited Doxygen comments and markup; modified
                   Doxygen markup to meet Living Docs standards.
06/17/09   dcf     Fix for embedded output.
02/23/09   dcf     Added comments and versioning.
=============================================================================
Copyright (c) 2010-2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary and Confidential. 
===========================================================================*/



/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "com_dtypes.h"
#include "TlmmDefs.h"
#include "string.h"


/*==========================================================================
  Function pointer table for TLMM glue code.  This is part of the DAL 
  framework glue code.
==========================================================================*/
/** @cond */
typedef struct DalTlmm DalTlmm;
struct DalTlmm
{
   struct DalDevice DalDevice;
   DALResult (*ConfigGpio)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioSignalType gpio_config, DALGpioEnableType enable);
   DALResult (*ConfigGpioGroup)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioEnableType enable, DALGpioSignalType* gpio_group, uint32 size);
   DALResult (*GetGpioNumber)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioSignalType gpio_config, uint32 *  number);
   DALResult (*GpioIn)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioSignalType gpio_config, DALGpioValueType*  value);
   DALResult (*GpioOut)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioSignalType gpio_config, DALGpioValueType value);
   DALResult (*GpioOutGroup)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioSignalType* gpio_config, uint32 size, DALGpioValueType value);
   DALResult (*SetTlmmPort)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioPortType Port, uint32  value);
   DALResult (*GetCurrentConfig)(uint32 ClientHandle, DalDeviceHandle* _h, uint32 gpio_number, DALGpioSignalType* config);
   DALResult (*GetGpioStatus)(uint32 ClientHandle, DalDeviceHandle* _h, uint32 gpio_number, DALGpioStatusType* status);
   DALResult (*SetInactiveConfig)(uint32 ClientHandle, DalDeviceHandle* _h, uint32 gpio_number, DALGpioSignalType config);
   DALResult (*GetInactiveConfig)(uint32 ClientHandle,DalDeviceHandle *h, uint32 gpio_number, DALGpioSignalType* config);
   DALResult (*LockGpio)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioClientType client, uint32 gpio_number);
   DALResult (*UnlockGpio)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioClientType client, uint32 gpio_number);
   DALResult (*IsGpioLocked)(uint32 ClientHandle, DalDeviceHandle* _h, uint32 gpio_number, DALGpioLockType* lock);
   DALResult (*GetOutput)(uint32 ClientHandle, DalDeviceHandle* _h, uint32 gpio_number, DALGpioValueType* value);
   DALResult (*GetGpioId)(uint32 ClientHandle, DalDeviceHandle* _h, const char* gpio_str, uint32 nStrSize, DALGpioIdType* nGpioId);
   DALResult (*ConfigGpioId)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioIdType nGpioId, DalTlmm_GpioConfigIdType* tGpioSettings);
   DALResult (*ConfigGpioIdInactive)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioIdType nGpioId);
   DALResult (*GpioIdOut)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioIdType nGpioId, DALGpioValueType value);
   DALResult (*GpioIdIn)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioIdType nGpioId, DALGpioValueType *value);
   DALResult (*ReleaseGpioId)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioIdType nGpioId);
   DALResult (*SelectGpioIdMode)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioIdType nGpioId, DalGpioModeType eMode, DalTlmm_GpioConfigIdType* pUserSettings);
   DALResult (*GetGpioIdSettings)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioIdType nGpioId,  DalTlmm_GpioIdSettingsType* pGpioSettings);
   DALResult (*ConfigGpioIdModeIndexed)(uint32 ClientHandle, DalDeviceHandle* _h, DALGpioIdType nGpioId, uint32 nIndex);
};


typedef struct DalTlmmHandle DalTlmmHandle; 
struct DalTlmmHandle 
{
   uint32 dwDalHandleId;
   const DalTlmm * pVtbl;
   void * pClientCtxt;
   uint32 dwVtblen;
};

#define DAL_TlmmDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTLMM_INTERFACE_VERSION,hDalDevice)

/** @endcond */



/*==========================================================================

                     APPLICATION PROGRAMMER'S INTERFACE

==========================================================================*/
/** 
  @addtogroup driver_apis
  @{
*/


/* ============================================================================
**  Function : DalTlmm_ConfigGpio
** ============================================================================*/
/**
  Configures a GPIO.
 
  This function configures a general purpose I/O or a hardware function 
  multiplexed on a particular GPIO pin. The configuration is based in part on 
  the value for the enable parameter.
  - If this parameter is set to DAL_TLMM_GPIO_ENABLE, the configuration 
    associated with the gpio_config parameter is programmed to hardware.
  - If DAL_TLMM_GPIO_DISABLE is specified, the GPIO number is extracted from 
    the gpio_config parameter and used to index an internal table to retrieve 
    the inactive (or low-power) configuration associated with the particular GPIO.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_config [in] -- Configuration to use whenn programming a GPIO.
  @param enable      [in] -- DAL_TLMM_GPIO_ENABLE or DAL_TLMM_GPIO_DISABLE.
    
  @return
  DAL_SUCCESS -- GPIO was successfully configured as requested.\n
  DAL_ERROR -- Either an invalid GPIO number in the configuration or ownership 
  cannot be obtained from a secure root.
    
  @dependencies
  None.

  @sideeffects
  Ownership for the specified GPIO can be changed, if allowed 
  and required. This may be necessary to provide visibility to 
  generic I/O on certain multiprocessor systems. 
    
  @sa
  DAL_GPIO_CFG, DALGpioSignalType
*/ 

static __inline DALResult
DalTlmm_ConfigGpio(DalDeviceHandle * _h, DALGpioSignalType gpio_config, DALGpioEnableType enable)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, ConfigGpio ),
        _h, gpio_config, enable);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->ConfigGpio(DALDEVICE_LOCAL_CLIENTID, _h, gpio_config, enable);
}


/* ============================================================================
**  Function : DalTlmm_ConfigGpioGroup
** ============================================================================*/
/**
  Configures a group of GPIOs.
 
  This function configures a group of GPIOs or a hardware function multiplexed 
  on a particular GPIO pin.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param enable      [in] -- DAL_TLMM_GPIO_ENABLE or DAL_TLMM_GPIO_DISABLE.
  @param *gpio_group [in] -- Pointer to an array of GPIO configurations.
  @param size        [in] -- Number of GPIO configurations in gpio_group.
    
  @return
  If 1, DAL_SUCCESS -- All configurations were valid and any remote requests 
                       were successful.\n
  DAL_ERROR -- Invalid configurations; unsuccessful requests.
    
  @dependencies
  None.

  @sa
  DalTlmm_ConfigGpio
*/ 

static __inline DALResult
DalTlmm_ConfigGpioGroup
(
  DalDeviceHandle * _h, 
  DALGpioEnableType  enable,
  DALGpioSignalType* gpio_group, 
  uint32             size
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return 
        hRemote->pVtbl->FCN_6(
          DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, ConfigGpioGroup ),
          _h, enable, gpio_group, size*(sizeof(uint32)));
   }
   return 
     ((DalTlmmHandle *)_h)->pVtbl->ConfigGpioGroup(DALDEVICE_LOCAL_CLIENTID, _h, enable, gpio_group, size);
}


/* ============================================================================
**  Function : DalTlmm_GetGpioNumber
** ============================================================================*/
/**
  Retrieves a GPIO number.
 
  Retrieves the GPIO number from a specified configuration. This function does 
  the same thing as the macro DAL_GPIO_NUMBER.
 
  @param *_h           [in] -- Handle to the TLMM DAL device.
  @param gpio_config   [in] -- Configuration from which to get the GPIO number.
  @param *gpio_number [out] -- GPIO number to return to the caller.
    
  @return
  Returns DAL_SUCCESS even if the GPIO number is invalid for the the current 
  target.
      
  @dependencies
  None.

  @sa
  DAL_GPIO_NUMBER
*/

static __inline DALResult
DalTlmm_GetGpioNumber
(
  DalDeviceHandle   *_h, 
  DALGpioSignalType gpio_config, 
  uint32            *gpio_number
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, GetGpioNumber ), 
        _h, gpio_config, gpio_number);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->GetGpioNumber(DALDEVICE_LOCAL_CLIENTID, _h, 
                                                     (DALGpioSignalType)gpio_config, 
                                                     gpio_number);
}


/* ============================================================================
**  Function : DalTlmm_GpioIn
** ============================================================================*/
/**
  Reads a GPIO value.
 
  Reads the value of a GPIO that has been configured as an INPUT.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_config [in] -- Configuration from which to get the GPIO number.
  @param *value     [out] -- Value to be returned to the caller.
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.
    
  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_GpioIn
(
  DalDeviceHandle *_h, 
  DALGpioSignalType gpio_config, 
  DALGpioValueType*value
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, GpioIn ), 
        _h, gpio_config, (uint32*)value);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->GpioIn(DALDEVICE_LOCAL_CLIENTID, _h, gpio_config, value);
}


/* ============================================================================
**  Function : DalTlmm_GpioOut
** ============================================================================*/
/**
  Writes a GPIO value.
 
  Writes a value (high or low) to a GPIO that is configured as an OUTPUT, has a 
  function select set to 0, and has no pull. This type of configuration is known 
  as general purpose I/O or bit-banged I/O.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_config [in] -- Configuration from which to get the GPIO number.
  @param value       [in] -- Value to write (DAL_GPIO_VALUE_HIGH or 
                             DAL_GPIO_VALUE_LOW).
    
  @return
  DAL_SUCCESS -- GPIO is valid.\n
  DAL_ERROR -- Invalid GPIO.

  @dependencies
  None.
    
  @sa
  DALGpioValueType
*/

static __inline DALResult
DalTlmm_GpioOut
(
  DalDeviceHandle  *_h, 
  DALGpioSignalType gpio_config, 
  DALGpioValueType  value
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, GpioOut ), _h, gpio_config, value);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->GpioOut(DALDEVICE_LOCAL_CLIENTID, _h, gpio_config, value);
}


/* ============================================================================
**  Function : DalTlmm_GpioOutGroup
** ============================================================================*/
/**
  Writes an output value to a group of GPIOs.
 
  Writes a value (high or low) to an array of GPIOs that are configured as OUTPUT,
  have function selects set to 0, and have pull set to no-pull.
 
  @param *_h           [in] -- Handle to the TLMM DAL device.
  @param *gpio_group   [in] -- Array of GPIO configurations to which to write an
                               output value.
  @param size          [in] -- Number of elements in the gpio_group array.
  @param value         [in] -- Value to write (DAL_GPIO_VALUE_HIGH or
                               DAL_GPIO_VALUE_LOW).
    
  @return
  DAL_SUCCESS -- GPIO numbers are valid.\n
  DAL_ERROR -- Invalid numbers.

  @dependencies
  None.
    
  @sa
  DalTlmm_GpioOut, DALGpioValueType
*/

static __inline DALResult
DalTlmm_GpioOutGroup
(
  DalDeviceHandle   *_h, 
  DALGpioSignalType *gpio_group, 
  uint32            size, 
  DALGpioValueType  value
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, GpioOutGroup ), 
        _h, (uint32)gpio_group, value);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->GpioOutGroup(DALDEVICE_LOCAL_CLIENTID, _h, 
                                                      gpio_group, 
                                                      size, value);
}


/* ============================================================================
**  Function : DalTlmm_SetPort
** ============================================================================*/
/**
  Configures a TLMM hardware port.
 
  This function configures the TLMM hardware based on the DALGpioPortType value 
  passed in as a parameter. This function is used to configure non-GPIO TLMM 
  registers.
 
  @param *_h   [in] -- Handle to the TLMM DAL device.
  @param port  [in] -- DALGpioPortType enumeration associated with the hardware 
                       functionality needed.
  @param value [in] -- Enable/disable value (0 = Disable, 1 = Enable).
    
  @return
  DAL_SUCCESS -- Operation was successful.\n
  DAL_ERROR -- Operation was unsuccessful.

  @dependencies
  None.
    
  @sa
  DALGpioPortType
*/

static __inline DALResult
DalTlmm_SetPort
(
  DalDeviceHandle *_h, 
  DALGpioPortType port,
  uint32          value
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, SetTlmmPort ), 
        _h, port, value);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->SetTlmmPort(DALDEVICE_LOCAL_CLIENTID, _h, port, value);
}


/* ============================================================================
**  Function : DalTlmm_GetCurrentConfig
** ============================================================================*/
/**
  Retrieves the current configuration of a GPIO.
 
  This function retrieves the currently programmed configuration for a 
  particular GPIO.
 
  @param *_h           [in] -- Handle to the TLMM DAL device.
  @param gpio_number   [in] -- GPIO number for which to retrieve the configuration.
  @param *gpio_config [out] -- Current configuration.
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.

  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_GetCurrentConfig
(
  DalDeviceHandle  *_h, 
  uint32           gpio_number, 
  DALGpioSignalType *gpio_config
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, GetCurrentConfig ), 
        _h, gpio_number, (uint32*)gpio_config);
   }
   return 
     ((DalTlmmHandle *)_h)->pVtbl->GetCurrentConfig(DALDEVICE_LOCAL_CLIENTID, _h, gpio_number, gpio_config);
}


/* ============================================================================
**  Function : DalTlmm_GetGpioStatus
** ============================================================================*/
/**
  Retrieves the current status of a GPIO.
 
  This function retrieves the current status of a GPIO: whether it is configured 
  to an active state or an inactive state.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_number [in] -- GPIO number for which to retrieve the configuration.
  @param *status    [out] -- State of the GPIO: either DAL_GPIO_ACTIVE or
                             DAL_GPIO_INACTIVE.
 
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.
    
  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_GetGpioStatus
(
  DalDeviceHandle   *_h, 
  uint32            gpio_number, 
  DALGpioStatusType *status
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, GetGpioStatus ), 
        _h, gpio_number, (uint32*)status);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->GetGpioStatus(DALDEVICE_LOCAL_CLIENTID, _h, gpio_number, status);
}
#define DalTlmm_GetPinStatus DalTlmm_GetGpioStatus


/* ============================================================================
**  Function : DalTlmm_SetInactiveConfig
** ============================================================================*/
/**
  Sets the inactive configuration of a GPIO.
 
  This function is used to modify the inactive or low-power configuration of a 
  GPIO. It modifies the configuration that is programmed when 
  DAL_TLMM_GPIO_DISABLE is passed in to the configuration functions.

  This function was originally provided for power debugging, but it can be used 
  elsewhere as necessary. It does not change the current configuration of a GPIO.
 
  @param *_h          [in] -- Handle to the TLMM DAL device.
  @param gpio_number  [in] -- GPIO number for which to retrieve the configuration.
  @param gpio_config [out] -- Configuration to use to replace the low-power
                              configuration in the table.
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.

  @dependencies
  None.
    
  @sa
  DalTlmm_GetInactiveConfig
*/

static __inline DALResult
DalTlmm_SetInactiveConfig
(
  DalDeviceHandle   *_h, 
  uint32            gpio_number, 
  DALGpioSignalType gpio_config
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, SetInactiveConfig ), 
        _h, gpio_number, gpio_config);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->SetInactiveConfig(DALDEVICE_LOCAL_CLIENTID, _h, 
                                                           gpio_number, 
                                                           gpio_config);
}


/* ============================================================================
**  Function : DalTlmm_GetInactiveConfig
** ============================================================================*/
/**
  Retrieves the inactive configuration of a GPIO.
 
  This function retrieves the currently set low-power (inactive) configuration 
  for a particular GPIO.
 
  @param *_h           [in] -- Handle to the TLMM DAL device.
  @param gpio_number   [in] -- GPIO number for which to retrieve the configuration
  @param *gpio_config [out] -- Currently set low-power configuration.
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.

  @dependencies
  None.
    
  @sa
  DalTlmm_SetInactiveConfig
*/

static __inline DALResult
DalTlmm_GetInactiveConfig
(
  DalDeviceHandle  *_h, 
  uint32           gpio_number, 
  DALGpioSignalType *gpio_config
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, GetInactiveConfig ), 
        _h, gpio_number, (uint32*)gpio_config);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->GetInactiveConfig(DALDEVICE_LOCAL_CLIENTID, _h, 
                                                           gpio_number, 
                                                           gpio_config);
}


/* ============================================================================
**  Function : DalTlmm_LockGpio
** ============================================================================*/
/**
  Provides a rudimentary locking mechanism for a GPIO.
 
  This function is used to provide a rudimentary locking mechanism for a GPIO. 
  Its purpose is for debugging only (mainly power). Once locked, a GPIO is 
  bypassed for sleep and active configuration until it is unlocked.

  Use this function only for debugging purposes.
 
  @param *_h          [in] -- Handle to the TLMM DAL device.
  @param client       [in] -- Client requesting the GPIO lock.
  @param gpio_number [out] -- GPIO number on which to lock configurations.
    
  @return
  DAL_SUCCESS -- GPIO number and client are valid.\n
  DAL_ERROR -- Invalid number and client.

  @dependencies
  None.
    
  @sa
  DalTlmm_UnlockGpio, DalTlmm_IsGpioLocked
*/

static __inline DALResult
DalTlmm_LockGpio
(
  DalDeviceHandle  *_h, 
  DALGpioClientType client, 
  uint32           gpio_number
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, LockGpio ), 
        _h, client, gpio_number);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->LockGpio(DALDEVICE_LOCAL_CLIENTID, _h, client, gpio_number);
}


/* ============================================================================
**  Function : DalTlmm_UnlockGpio
** ============================================================================*/
/**
  Provides a rudimentary unlocking mechanism for a GPIO.
 
  This function unlocks a GPIO that had been previously locked. If a GPIO is not 
  locked, calling this function does nothing.

  Use this function only for debugging purposes.

  @param *_h          [in] -- Handle to the TLMM DAL device.
  @param client       [in] -- Client requesting the GPIO unlock.
  @param gpio_number [out] -- GPIO number to unlock configurations on.
        
  @return
  DAL_SUCCESS -- GPIO number and client are valid.\n
  DAL_ERROR -- Invalid number and client.

  @dependencies
  None.

  @sa
  DalTlmm_LockGpio, DalTlmm_IsGpioLocked
*/

static __inline DALResult
DalTlmm_UnlockGpio
(
  DalDeviceHandle  *_h, 
  DALGpioClientType client, 
  uint32           gpio_number
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, UnlockGpio ), 
        _h, client, gpio_number);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->UnlockGpio(DALDEVICE_LOCAL_CLIENTID, _h, client, gpio_number);
}


/* ============================================================================
**  Function : DalTlmm_IsGpioLocked
** ============================================================================*/
/**
  Returns the lock status of a GPIO.
 
  This function retrieves the current lock status of a GPIO.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_number [in] -- GPIO number for which locking information is to be 
                             retrieved.
  @param *lock      [out] -- Lock status of the GPIO (DAL_GPIO_UNLOCKED or
                             DAL_GPIO_LOCKED).
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.

  @dependencies
  None.
    
  @sa
  DalTlmm_LockGpio, DalTlmm_UnlockGpio
*/

static __inline DALResult
DalTlmm_IsGpioLocked(DalDeviceHandle * _h, uint32  gpio_number, DALGpioLockType* lock)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(
        DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, IsGpioLocked ), 
        _h, gpio_number, (uint32*)lock);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->IsGpioLocked(DALDEVICE_LOCAL_CLIENTID, _h, gpio_number, lock);
}


/* ============================================================================
**  Function : DalTlmm_GetOutput
** ============================================================================*/
/**
  Retrieves the output value of a GPIO.
 
  This function retrieves the current output value of a GPIO that has been 
  configured as a generic I/O with output enabled.
 
  @param *_h         [in] -- Handle to the TLMM DAL device.
  @param gpio_number [in] -- GPIO number to use to obtain the output value.
  @param *value     [out] -- Current value of the output GPIO (DAL_GPIO_VALUE_LOW 
                             or DAL_GPIO_VALUE_HIGH).
    
  @return
  DAL_SUCCESS -- GPIO number is valid.\n
  DAL_ERROR -- Invalid number.
    
  @dependencies
  None.
*/

static __inline DALResult
DalTlmm_GetOutput(DalDeviceHandle * _h, uint32 gpio_number, DALGpioValueType* value)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalTlmmHandle *)_h)->pVtbl, GetOutput ), _h, gpio_number, (uint32*)value);
   }
   return ((DalTlmmHandle *)_h)->pVtbl->GetOutput(DALDEVICE_LOCAL_CLIENTID, _h, gpio_number, value);
}


/* ============================================================================
**  Function : DalTlmm_GetGpioId
** ============================================================================*/
/**
  Retrieves a GPIO identifier corresponding to an input string.
 
  This API retrieves a GPIO ID corresponding to a GPIO string name.  The string
  name must match exactly the expected name for the HW functionality.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *pszGpio[in] -- The name of the GPIO string to search for.
  @param *pnGpioId[out] -- The identification pertaining to the above string.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
 
  @sa
  DalTlmm_ConfigGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult
DalTlmm_GetGpioId(DalDeviceHandle * _h, const char* pszGpio, DALGpioIdType* pnGpioId)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GetGpioId(DALDEVICE_LOCAL_CLIENTID, _h, pszGpio, strlen(pszGpio)+1, pnGpioId);
}


/* ============================================================================
**  Function : DalTlmm_ConfigGpioId
** ============================================================================*/
/**
  Configures a GPIO based on identifier and user settings.
 
  This API configures a GPIO based on a GPIO Identifier and user configuration
  settings.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *nGpioId[in] -- The GPIO Identifier to configure.
  @param *pUserSettings[in] -- The user settings to apply to the GPIO for the
                               identifier in parameter 2.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
  
  @sa
  DalTlmm_GetGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult
DalTlmm_ConfigGpioId(DalDeviceHandle * _h, DALGpioIdType nGpioId, DalTlmm_GpioConfigIdType* pUserSettings)
{
   return ((DalTlmmHandle *)_h)->pVtbl->ConfigGpioId(DALDEVICE_LOCAL_CLIENTID, _h, nGpioId, pUserSettings);
}


/* ============================================================================
**  Function : DalTlmm_ConfigGpioIdInactive
** ============================================================================*/
/**
  Configures a GPIO to a predefined low-power state.
 
  This API configures a GPIO based on a GPIO Identifier to a predetermined
  low-power configuration defined for a particular platform.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *nGpioId[in] -- The GPIO Identifier to configure.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
  
  @sa
  DalTlmm_GetGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult
DalTlmm_ConfigGpioIdInactive(DalDeviceHandle * _h, DALGpioIdType nGpioId)
{
   return ((DalTlmmHandle *)_h)->pVtbl->ConfigGpioIdInactive(DALDEVICE_LOCAL_CLIENTID, _h, nGpioId);
}


/* ============================================================================
**  Function : DalTlmm_GpioIdOut
** ============================================================================*/
/**
  Outputs the state of a GPIO configure for output.
 
  This API configures a GPIO based on a GPIO Identifier to a predetermined
  low-power configuration defined for a particular platform.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *nGpioId[in] -- The GPIO Identifier to configure.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
  
  @sa
  DalTlmm_GetGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult
DalTlmm_GpioIdOut(DalDeviceHandle * _h, DALGpioIdType nGpioId, DALGpioValueType eValue)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GpioIdOut(DALDEVICE_LOCAL_CLIENTID, _h, nGpioId, eValue);
}


/* ============================================================================
**  Function : DalTlmm_GpioIdIn
** ============================================================================*/
/**
  Reads the state of the GPIO
 
  This API reads the input value of a GPIO signal.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *nGpioId[in] -- The GPIO Identifier to configure.
    
  @return
  DAL_SUCCESS -- The GPIO ID was found.\n
  DAL_ERROR -- The GPIO ID was not found.
    
  @dependencies
  None.
  
  @sa
  DalTlmm_GetGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult
DalTlmm_GpioIdIn(DalDeviceHandle * _h, DALGpioIdType nGpioId, DALGpioValueType *eValue)
{
   return ((DalTlmmHandle *)_h)->pVtbl->GpioIdIn(DALDEVICE_LOCAL_CLIENTID, _h, nGpioId, eValue);
}


/* ============================================================================
**  Function : DalTlmm_ReleaseGpioId
** ============================================================================*/
/**
  Releases an auto-lock on a GPIO.
 
  This API releases a lock which is automatically applied when the GPIO ID is
  retrieved.  The lock can only be released by the owner of GPIO ID.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *nGpioId[in] -- The GPIO Identifier to release the lock for.
    
  @return
  DAL_SUCCESS -- The lock was released.
  DAL_ERROR -- The GPIO ID is not allowed to unlock this GPIO.
    
  @dependencies
  None.
  
  @sa
  DalTlmm_GetGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult 
DalTlmm_ReleaseGpioId( DalDeviceHandle * _h, DALGpioIdType nGpioId) 
{
   return ((DalTlmmHandle *)_h)->pVtbl->ReleaseGpioId(DALDEVICE_LOCAL_CLIENTID, _h, nGpioId);
}


/* ============================================================================
**  Function : DalTlmm_SelectGpioIdMode
** ============================================================================*/
/**
  Allows the owner of a GPIO ID to toggle between IO mode and functional mode.
 
  This is an optional API to allow a GPIO owner who uses the GPIO ID to switch between
  the functional mode (function select for a particular platform) and IO mode.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *nGpioId[in] -- The GPIO Identifier to operate on.
  @param eMode[in] -- Specified whether to use IO mode or functional
                      mode.  TRUE specifies to use IO mode, FALSE specifies
                      to use functional mode.  Note, in cases where the
                      functional mode is also the IO mode, no change
                      will take place.
  @param pUserSettings -- Optional user settings to apply when switching mode.  A
                          null value will keep the current settings.
    
  @return
  DAL_SUCCESS -- The specified operation completed successfully.
  DAL_ERROR -- The GPIO ID does not have permission to change the functional
               mode of this GPIO.
    
  @dependencies
  None.
  
  @sa
  DalTlmm_GetGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult 
DalTlmm_SelectGpioIdMode( DalDeviceHandle * _h, DALGpioIdType nGpioId, DalGpioModeType eMode, DalTlmm_GpioConfigIdType* pUserSettings) 
{
   return ((DalTlmmHandle *)_h)->pVtbl->SelectGpioIdMode(DALDEVICE_LOCAL_CLIENTID, _h, nGpioId, eMode, pUserSettings);
}


/* ============================================================================
**  Function : DalTlmm_GetGpioIdSettings
** ============================================================================*/
/**
  Reads the hardware and returns the curreng GPIO configuration.
 
  This API releases a lock which is automatically applied when the GPIO ID is
  retrieved.  The lock can only be released by the owner of GPIO ID.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *nGpioId[in] -- The GPIO Identifier to retrieve the configuration for.
  @param *pGpioSettings[inout] -- The settings container to store the information in.
    
  @return
  DAL_SUCCESS -- The configuration was retrieved.
  DAL_ERROR -- The GPIO ID is not invalid.
    
  @dependencies
  None.
  
  @sa
  DalTlmm_GetGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult 
DalTlmm_GetGpioIdSettings( DalDeviceHandle * _h, DALGpioIdType nGpioId, DalTlmm_GpioIdSettingsType* pGpioSettings) 
{
   return ((DalTlmmHandle *)_h)->pVtbl->GetGpioIdSettings(DALDEVICE_LOCAL_CLIENTID, _h, nGpioId, pGpioSettings);
}


/* ============================================================================
**  Function : DalTlmm_ConfigGpioIdModeIndexed
** ============================================================================*/
/**
  Allows the owner of a GPIO ID to select a mode of configuration based on a 
  pre-defined list of configurations from the device configuration.
 
  This API allows a GPIO owner who uses the GPIO ID to switch between several
  pre-defined configurations based on the supplied index parameter.
 
  @param *_h [in] -- Handle to the TLMM DAL device.
  @param *nGpioId[in] -- The GPIO Identifier to operate on.
  @param nIndex[in] -- Index to the pre-defined table for the configuration required.
    
  @return
  DAL_SUCCESS -- The specified operation completed successfully.
  DAL_ERROR -- The GPIO ID does not have permission to change the functional
               mode of this GPIO or the supplied index is out of bounds.
    
  @dependencies
  None.
  
  @sa
  DalTlmm_GetGpioId, DalTlmm_GpioConfigIdType
*/

static __inline DALResult 
DalTlmm_ConfigGpioIdModeIndexed( DalDeviceHandle * _h, DALGpioIdType nGpioId, uint32 nIndex ) 
{
   return ((DalTlmmHandle *)_h)->pVtbl->ConfigGpioIdModeIndexed(DALDEVICE_LOCAL_CLIENTID, _h, nGpioId, nIndex);
}


/** 
  @}
*/ /* end_group driver_apis */



#endif

