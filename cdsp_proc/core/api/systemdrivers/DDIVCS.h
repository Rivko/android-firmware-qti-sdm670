#ifndef DDIVCS_H
#define DDIVCS_H
/*
===========================================================================
*/
/**
  @file DDIVCS.h
  @brief Public interface include file for accessing the VCS device driver.

  The DDIVCS.h file is the public API interface to the VCS 
  Device Access Library (DAL).
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The VCS_API_mainpage.dox file contains all file/group descriptions that 
      are in the output PDF generated using Doxygen and LaTeX. To edit or 
      update any of the file/group text in the PDF, edit the 
      VCS_API_mainpage.dox file or contact Tech Pubs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/DDIVCS.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALSys.h"
#include "VCSDefs.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/**
  Defines the interface version.
 */
#define DALVCS_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)


/*=========================================================================
      Interface Definitions
==========================================================================*/

/** @cond 
*/

typedef struct DalVCS DalVCS;
struct DalVCS
{
   struct DalDevice DalDevice;
   DALResult (*EnableHVX)(uint32 ClientHandle, DalDeviceHandle *_h);
   DALResult (*DisableHVX)(uint32 ClientHandle, DalDeviceHandle *_h);
};

typedef struct DalVCSHandle DalVCSHandle;
struct DalVCSHandle
{
   uint32 dwDalHandleId;
   const DalVCS * pVtbl;
   void * pClientCtxt;
};

#define DAL_VCSDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALVCS_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/*=========================================================================
      Interface Declarations
==========================================================================*/


/** @addtogroup hvx_functions
@{ */

/* =========================================================================
**  Function : DalVCS_EnableHVX
** =========================================================================*/
/**
  Power up the HVX coprocessor.
  This function latency could take longer during use cases where the eLDO
  must be temporarily put into BHS mode during HVX power up.

  @param _h   [in] Handle to the DAL device.

  @return
  DAL_SUCCESS -- Successfully enabled HVX.
  DAL_ERROR -- Failed to enable HVX.

  @dependencies
  None.
*/

static __inline DALResult
DalVCS_EnableHVX(DalDeviceHandle * _h)
{
   return ((DalVCSHandle *)_h)->pVtbl->EnableHVX(DALDEVICE_LOCAL_CLIENTID,  _h);
}


/* =========================================================================
**  Function : DalVCS_DisableHVX
** =========================================================================*/
/**
  Power down the HVX coprocessor.
  This function latency could take longer during use cases where the eLDO
  must be temporarily put into BHS mode during HVX power down.

  @param _h   [in] Handle to the DAL device.

  @return
  DAL_SUCCESS -- Successfully disabled HVX.
  DAL_ERROR -- Failed to disable HVX.

  @dependencies
  None.
*/

static __inline DALResult
DalVCS_DisableHVX(DalDeviceHandle * _h)
{
   return ((DalVCSHandle *)_h)->pVtbl->DisableHVX(DALDEVICE_LOCAL_CLIENTID,  _h);
}

/** @} */ /* end_addtogroup hvx_functions */


#endif /* !DDIVCS_H */

