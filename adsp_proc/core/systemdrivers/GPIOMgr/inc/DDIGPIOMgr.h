#ifndef __DDIGPIOMgr_H__
#define __DDIGPIOMgr_H__
/*===========================================================================*/

              /*   D A L   G P I O   M A N A G E R    */

/**
  @file DDIGPIOMgr.h 
  @brief This file contains the API specification for the Dal GPIO Manager device 
  driver. The definitions in this module are used externally by the clients of the GPIO 
  interrupt controller.
*/

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the mainpage file. To change 
         any of the the file/group text for the PDF, edit this 
         file, or contact Tech Pubs.
*/

/*===========================================================================
       Copyright (c) 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOMgr/inc/DDIGPIOMgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
5/21/2015  cpaulo  First draft created. 
===========================================================================*/
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DDIGPIOInt.h"
#include "GPIOMgrInterface.h"

/**
  Defines the interface version for GPIO Interrupt Driver.
 */
#define GPIOMGR_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/** @cond 
*/ /* Internal GPIOMgr function pointer structure to be skipped by Documentation */
typedef struct GPIOMgrIF GPIOMgrIF;
struct GPIOMgrIF
{
   struct DalDevice DalDevice;
   DALResult (*GPIOMgr_LockGPIO)(uint32 ClientHandle, DalDeviceHandle *_h , uint32 nGPIO, uint32 nPD);
   DALResult (*GPIOMgr_ReleaseGPIO)(uint32 ClientHandle, DalDeviceHandle *_h , uint32 nGPIO);
   DALResult (*GPIOMgr_GetGPIOCurrentPD)(uint32 ClientHandle, DalDeviceHandle *_h , uint32 nGPIO, uint32* pnPD);
   DALResult (*GPIOMgr_GetDirectConnectGPIO)(uint32 ClientHandle, DalDeviceHandle *_h , uint32 nGPIO, uint32* pDirConnID);
   DALResult (*GPIOMgr_ReleaseDirectConnectGPIO)(uint32 ClientHandle, DalDeviceHandle *_h , uint32 nGPIO, uint32 nDirConnID);
   DALResult (*GPIOMgr_GetDirectConnectInterruptID)(uint32 ClientHandle, DalDeviceHandle *_h , uint32 nGPIO, uint32* pInterruptID);
};

typedef struct GPIOMgrIFHandle GPIOMgrIFHandle; 
struct GPIOMgrIFHandle 
{
   uint32 dwDalHandleId;
   const GPIOMgrIF * pVtbl;
   void * pClientCtxt;
   uint32 dwVtblen;
};

#define GPIOMgrDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,GPIOMgr_INTERFACE_VERSION,hDalDevice)

/** @endcond */

/* =========================================================================
**  Function : GPIOMgr_LockGPIO
** =========================================================================*/
/**
  Locks a given GPIO.

  This function will attempt to lock a given GPIO to a given PD.

  @param[in] _h         The client context pointer of the GPIOMgr DAL.
  @param[in] nGPIO      The GPIO pin number to be locked. 
  @param[in] nPD        The PD which the GPIO is to be locked. 
  @return
  DAL_SUCCESS -- is returned if the GPIO is successfully locked. 
  DAL_ERROR -- is returned if the GPIO is in use by another 
  client. 
  @dependencies
  None.
*/

static __inline DALResult
DalGPIOMgr_LockGPIO(DalDeviceHandle * _h, uint32 nGPIO, uint32 nPD)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(DALVTBLIDX(((GPIOMgrIFHandle *)_h)->pVtbl, GPIOMgr_LockGPIO ), _h, nGPIO, nPD);
   }
   return ((GPIOMgrIFHandle *)_h)->pVtbl->GPIOMgr_LockGPIO(DALDEVICE_LOCAL_CLIENTID, _h, nGPIO, nPD);
}


/* =========================================================================
**  Function : GPIOMgr_ReleaseGPIO
** =========================================================================*/
/**
  Unlocks a given GPIO.

  This function will attempt to unlock a given GPIO from a given PD.

  @param[in] _h         The client context pointer of the GPIOMgr DAL.
  @param[in] nGPIO      The GPIO pin number to be unlocked. 
  @return
  DAL_SUCCESS -- is returned if the GPIO is successfully unlocked. 
  DAL_ERROR -- is returned if the GPIO is not locked prior to call. 
  @dependencies
  None.
*/

static __inline DALResult
DalGPIOMgr_ReleaseGPIO(DalDeviceHandle * _h, uint32 nGPIO)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((GPIOMgrIFHandle *)_h)->pVtbl, GPIOMgr_ReleaseGPIO ), _h, nGPIO);
   }
   return ((GPIOMgrIFHandle *)_h)->pVtbl->GPIOMgr_ReleaseGPIO(DALDEVICE_LOCAL_CLIENTID, _h, nGPIO);
}


/* =========================================================================
**  Function : GPIOMgr_GetGPIOCurrentPD
** =========================================================================*/
/**
  Gets the current PD that the GPIO is locked to.

  @param[in]  _h         The client context pointer of the GPIOMgr DAL.
  @param[in]  nGPIO      The GPIO pin number to fetch the current PD. 
  @param[out] pnPD       The PD that the GPIO is registered to. 
  @return
  DAL_SUCCESS -- is returned if PD successfully returned. 
  DAL_ERROR -- is returned if the GPIO is not currently in use. 
  @dependencies
  None.
*/

static __inline DALResult
DalGPIOMgr_GetGPIOCurrentPD(DalDeviceHandle * _h, uint32 nGPIO, uint32* pnPD)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(DALVTBLIDX(((GPIOMgrIFHandle *)_h)->pVtbl, GPIOMgr_GetGPIOCurrentPD ), _h, nGPIO, pnPD);
   }
   return ((GPIOMgrIFHandle *)_h)->pVtbl->GPIOMgr_GetGPIOCurrentPD(DALDEVICE_LOCAL_CLIENTID, _h, nGPIO, pnPD);
}


/* =========================================================================
**  Function : GPIOMgr_GetDirectConnectGPIO
** =========================================================================*/
/**
  Gets a free direct connect GPIO for the GPIO given.

  @param[in]  _h          The client context pointer of the GPIOMgr DAL.
  @param[in]  nGPIO       The GPIO pin number to attach to the direct connect. 
  @param[out] pDirConnID  The direct connect index assigned to the GPIO. 
  @return
  DAL_SUCCESS -- is returned if a direct connect was assigned. 
  DAL_ERROR -- is returned if there are no more direct connects. 
  @dependencies
  GPIO must be locked.
*/

static __inline DALResult
DalGPIOMgr_GetDirectConnectGPIO(DalDeviceHandle * _h, uint32 nGPIO, uint32* pDirConnID)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(DALVTBLIDX(((GPIOMgrIFHandle *)_h)->pVtbl, GPIOMgr_GetDirectConnectGPIO ), _h, nGPIO, pDirConnID);
   }
   return ((GPIOMgrIFHandle *)_h)->pVtbl->GPIOMgr_GetDirectConnectGPIO(DALDEVICE_LOCAL_CLIENTID, _h, nGPIO, pDirConnID);
}


/* =========================================================================
**  Function : GPIOMgr_ReleaseDirectConnectGPIO
** =========================================================================*/
/**
  Releases a direct connect GPIO for the GPIO given.  This GPIO will now
  be added back to the free direct connect pool.

  @param[in] _h         The client context pointer of the GPIOMgr DAL.
  @param[in] nGPIO      The GPIO currently assigned to the direct connect. 
  @param[in] nDirConnID The direct connect ID to be released. 
  @return
  DAL_SUCCESS -- is returned if GPIO successfully released from direct connect. 
  DAL_ERROR -- is returned if the GPIO was never in use by direct connect. 
  @dependencies
  Direct connect must be already assigned to this GPIO.
*/

static __inline DALResult
DalGPIOMgr_ReleaseDirectConnectGPIO(DalDeviceHandle * _h, uint32 nGPIO, uint32 nDirConnID)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(DALVTBLIDX(((GPIOMgrIFHandle *)_h)->pVtbl, GPIOMgr_ReleaseDirectConnectGPIO ), _h, nGPIO, nDirConnID);
   }
   return ((GPIOMgrIFHandle *)_h)->pVtbl->GPIOMgr_ReleaseDirectConnectGPIO(DALDEVICE_LOCAL_CLIENTID, _h, nGPIO, nDirConnID);
}


/* =========================================================================
**  Function : GPIOMgr_GetDirectConnectInterruptID
** =========================================================================*/
/**
  Gets the interrupt ID of a direct connect interrupt that is locked to a GPIO.

  @param[in]  _h         The client context pointer of the GPIOMgr DAL.
  @param[in]  nGPIO      The GPIO currently assigned to the direct connect. 
  @param[out] pDirConnID The direct connect interrupt ID. 
  @return
  DAL_SUCCESS -- is returned if direct connect interrupt ID successfully returned. 
  DAL_ERROR -- is returned if the GPIO is not assigned a direct connect ID. 
  @dependencies
  None.
*/

static __inline DALResult
DalGPIOMgr_GetDirectConnectInterruptID(DalDeviceHandle * _h, uint32 nGPIO, uint32* pInterruptID)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_2(DALVTBLIDX(((GPIOMgrIFHandle *)_h)->pVtbl, GPIOMgr_GetDirectConnectInterruptID ), _h, nGPIO, pInterruptID);
   }
   return ((GPIOMgrIFHandle *)_h)->pVtbl->GPIOMgr_GetDirectConnectInterruptID(DALDEVICE_LOCAL_CLIENTID, _h, nGPIO, pInterruptID);
}


/* =========================================================================
**  Function : GPIOMgr_Attach
** =========================================================================*/
/**
  Attaches to the GPIOMgr DAL.

  @param[in] _h  A pointer to a Dal handle.
  @return
  DAL_SUCCESS -- Successfully attached. 
  DAL_ERROR -- Failed to attach. 
  @dependencies
  None.
*/

static __inline DALResult
DalGPIOMgr_Attach(DalDeviceHandle ** _h)
{
  return DAL_StringDeviceAttach( "GPIOManager", _h );
}


#endif  /*__DDIGPIOMgr_H__*/
