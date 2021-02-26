/*==============================================================================
                           DALQDIDef.h

GENERAL DESCRIPTION
    This file provides defines common macros function declarations
    for QDI based DAL Remoting.

        Copyright © 2007-2012, 2016 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

    $Header: //components/rel/core.qdsp6/2.1.c4/dal/framework/inc/DALQdiDefs.h#1 $
 
when       who     description
--------   ---     ---------------------------------------------------------- 
05/15/2012 aa      Initial release for ADSP2.0 MPQuRT QURTOS/USERPD
==============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#ifndef _DALQDIDEFS_H
#define _DALQDIDEFS_H

#include "DALSysTypes.h"

#define DALDEVICE_QDI_HANDLE_ID         0x25E025E0
#define DALREMOTE_MSG_DDIFCN            0x00 + 128
#define DALREMOTE_MSG_ATTACH            0x01 + 128
#define DALREMOTE_MSG_DETACH            0x02 + 128
#define DALREMOTE_MSG_ATTACH_EX         0x03 + 128
#define DALREMOTE_MSG_STRING_ATTACH     0x04 + 128
#define DALREMOTE_MSG_STRING_ATTACH_EX  0x05 + 128
#define DALREMOTE_MSG_CREATE_PROXY      0x06 + 128
#define DALREMOTE_MSG_REMOVE_PROXY      0x07 + 128                           

#define DALISREMOTEDEVICE(h)  ((h->dwDalHandleId) & DALDEVICE_QDI_HANDLE_ID)

/**
  @brief Establish communication link with the desired device on a remote
         process.  

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device

  @param DeviceId : Desired Device Id
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult 
_DoRemoteDeviceAttach(DALDEVICEID devId, DalDeviceHandle **phdalDevice);

/**
  @brief Establish communication link with the desired device on a remote
         process.

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device.
  
  @param pszArg   : User Argument
         DeviceId : Desired Device Id
         ClientVersion: Version from the Interface Header file for this interface
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult 
_DoRemoteDeviceAttachEx(const char *pszArg, DALDEVICEID devId, 
                        DALInterfaceVersion ClientVersion,
                        DalDeviceHandle **phdalDevice);

/**
  @brief Establish communication link with the desired device on a remote
         process.  

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device

  @param pszDevName : Desired Device Id (String Device ID)
         phDevice   : Return parameter, contains DAL Device Handle, if return
                      code is DAL_SUCCESS
  @see DALSYS_GetEnv
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult 
_DoRemoteStringDeviceAttach(const char *pszDevName, DalDeviceHandle **phDalDevice);

/**
  @brief Establish communication link with the desired device on a remote
         process.

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device.
  
  @param pszArg   : User Argument
         pszDevName : Desired Device Id (String Device ID)
         ClientVersion: Version from the Interface Header file for this interface
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult 
_DoRemoteStringDeviceAttachEx(const char *pszArg,
                              const char *pszDevName,
                              DALInterfaceVersion ClientVersion,
                              DalDeviceHandle **phDalDevice);

/**
  @brief Relinquish communication link with the desired device on remote 
         process

  User(s)/client(s) of DAL drivers must use this API to release the
  desired device. Upon successful invocation of the API the Client will have
  successful relinquished communication link with the desired device

  @param hDevice : DAL Device Handle

  @see DALSYS_DeviceAttach
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/ 
                 
DALResult 
_DoRemoteDetach(DalDeviceHandle * hDalDevice);

/*
* Add a proxy for given dal device handle.
*/
DALResult 
_DoRemoteCreateProxy(DalDeviceHandle *hdalDevice);

/*
* Remove proxy for given dal device handle.
*/
DALResult 
_DoRemoteReleaseProxy(DalDeviceHandle *hdalDevice);

/*
 * Initialize remoting support
 */
DALResult DALRemoteInit(void);

/*
 * De-Initialize remoting support
 */
DALResult DALRemoteDeinit(void);

#endif
