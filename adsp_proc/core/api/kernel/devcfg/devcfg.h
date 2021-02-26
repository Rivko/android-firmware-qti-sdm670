#ifndef DEVCFG_H
#define DEVCFG_H
/*==============================================================================
@file DALSys.h

                  DAL System lib API

        Copyright (c) 2016 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Technologies Confidential and Proprietary
=============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "devcfgTypes.h"
#include "string.h"
#ifdef DEVCFG_STANDALONE
#include "stubs.h"
#endif

#define DALSYS_PROP_TYPE_STR_PTR           0x10
#define DALSYS_PROP_TYPE_BYTE_PTR          0x11
#define DALSYS_PROP_TYPE_UINT32            0x02
#define DALSYS_PROP_TYPE_UINT32_PTR        0x12


//DAL Property attribute types
#define DALPROP_ATTR_TYPE_STRING_PTR          0x00000011
#define DALPROP_ATTR_TYPE_UINT32              0x00000002
#define DALPROP_ATTR_TYPE_BYTE_SEQ            0x00000008
#define DALPROP_ATTR_TYPE_BYTE_SEQ_PTR        0x00000018
#define DALPROP_ATTR_TYPE_UINT32_SEQ_PTR      0x00000014
#define DALPROP_ATTR_TYPE_STRUCT_PTR          0x00000012
//DAL Property end marker
#define DALPROP_ATTR_PROP_END                 0xFF00FF00

//DAL Property data types
#define DALPROP_DATA_TYPE_STRING              0x00000001
#define DALPROP_DATA_TYPE_BYTE_SEQ            0x00000002
#define DALPROP_DATA_TYPE_UINT32_SEQ          0x00000003
#define DALPROP_DATA_TYPE_STRUCT_INC_FILE     0x00000004

//Mask to seperate ID type and value
#define DALPROP_ATTR_TYPE_ID_VALUE_MASK       0xFFC00000
#define DALPROP_ATTR_TYPE_ID_ATTR_MASK        0x003FFFFF

/*------------------------------------------------------------------------------
* Function declaration and documentation
*-----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
  @brief Get the DAL Properties handle

  User(s)/client(s) of DAL drivers must use this API to get the DAL Properties
  Handle

  @param DeviceId   : Desired Device Id
       phDALProps : Pointer to DALPropertyHandle, only valid if return code is
                 DAL_SUCCESS
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/

DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId, DALSYSPropertyHandle hDALProps);

/**
  @brief Get the DAL Properties handle

  User(s)/client(s) of DAL drivers must use this API to get the DAL Properties
  Handle

  @param DeviceId   : Desired Device Id
         phDALProps : Pointer to DALPropertyHandle, only valid if return code is
                      DAL_SUCCESS
         prop_info_type : enum type denoting existence of property for SOC/PLATFORM
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_GetDALPropertyHandleEx(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps, 
                              DEVCFG_PROP_INFO_TYPE prop_info_type);

/**
  @brief Get the DAL Properties handle

  User(s)/client(s) of DAL drivers must use this API to get the DAL Properties
  Handle

  @param pszDevName : Desired Device String Identifier
         phDALProps : Pointer to DALPropertyHandle, only valid if return code is
                      DAL_SUCCESS
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps);

/**
  @brief Get the DAL Properties handle

  User(s)/client(s) of DAL drivers must use this API to get the DAL Properties
  Handle

  @param pszDevName : Desired Device String Identifier
         phDALProps : Pointer to DALPropertyHandle, only valid if return code is
                      DAL_SUCCESS
         prop_info_type : enum type denoting existence of property for SOC/PLATFORM
*
* @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_GetDALPropertyHandleStrEx(const char *pszDevName, DALSYSPropertyHandle hDALProps, 
                                 DEVCFG_PROP_INFO_TYPE prop_info_type);

/*
  @brief Get the Property Value. User must pass a ptr to the PropVariable.
  The User must also initialize the name field, this API will populate the
  name and value. The value is a "union" and must be used depending upon
  the appropriate type.

  @param hDALProps   : DAL Propery Handle
       pszName     : Prop Name ( if searching for uint32 name, this MUST be
                                 set to NULL)
        dwId        : Prop Id   ( integer prop name)
        pDALProp    : Ptr to the Property variable

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult
DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                  uint32 dwId,
                   DALSYSPropertyVar *pDALPropVar);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* DEVCFG_H */
