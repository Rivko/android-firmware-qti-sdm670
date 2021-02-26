#ifndef DDITPDA_H
#define DDITPDA_H

//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.qdsp6/2.1/debugtrace/tpda/dal/inc/DDITPDA.h#1 $
//
//========================================================================
//             Copyright (c) 2015 QUALCOMM Technologies Inc.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================
#include "qdss_tpda.h"
#include "DalDevice.h"

#define DALTPDA_STRING_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)


#define DAL_TPDA_SUCCESS          0x0
#define DAL_TPDA_ERROR            -1
#define DAL_TPDA_INVALID_BLOCK    0x2
#define DAL_TPDA_INCORRECT_OPTION 0x3


#define TPDA_NAME_LEN 32

                     
typedef struct DalTPDA DalTPDA;
struct DalTPDA
{
   struct DalDevice DalDevice;
   DALResult (*TPDA_EnableDisable)(uint32 ClientHandle, DalDeviceHandle *phandle,const char *pTPDAStr,const char *pTPDMStr,DALBOOL bEnable);
   DALResult (*TPDA_SetParam)(uint32 ClientHandle, DalDeviceHandle* h,const char *pTPDAStr,uint8 param_id,uint32 val);
   DALResult (*TPDA_GetParam)(uint32 ClientHandle, DalDeviceHandle* h,const char *pTPDAStr,uint8 param_id,uint32 val);

};

typedef struct DalTPDAHandle DalTPDAHandle; 
struct DalTPDAHandle 
{
   uint32 dwDalHandleId;
   const DalTPDA *pVtbl;
   void * pClientCtxt;
};


/* ============================================================================
**  Function : DAL_TPDADeviceAttach
** ============================================================================*/
/**
  Attached to a TPDA device
 
  @param DevId        [in] -- Must be "DALDEVICEID_TPDA"
  @param hDalDevice   [out] -- Pointer to a device handle.
 
  @return
  DAL_SUCCESS -- Successful. Other APIs can be invoked after this.
  DAL_ERROR -- Failure. 
    
  @dependencies
  None
*/


#define DAL_TPDADeviceAttach(DevId,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL,DevId,DALTPDA_STRING_INTERFACE_VERSION,hDalDevice)


/* ============================================================================
**  Function : DAL_TPDA_EnableDisable
** ============================================================================*/
/**
  Enables or Disables a TPDM component.
  This function enables or disables a TPDM component and the TPDA component 
  that it is connected to.
 
  @param *phandle    [in] -- Handle to the TPDA DAL device.
  @param *pTPDAStr   [in] -- Name of TPDA.
  @param *pTPDMStr    [in] -- TPDM Name associated with TPDA whose port needs 
  to be enabled.
  @param bEnable     [in] -- TRUE(1)/FALSE(0) enables/disables the TPDA port.
 
  @return
  DAL_SUCCESS -- TPDA port enablement is successful. 
  DAL_ERROR -- Invalid TPDA parameters or incorrect device state.
    
  @dependencies
  Call DAL_TPDMDeviceAttach prior to calling this function.
*/


static __inline DALResult DAL_TPDA_EnableDisable(DalDeviceHandle *phandle,const char *pTPDAStr,const char *pTPDMStr,DALBOOL bEnable)
{
   return ((DalTPDAHandle *)phandle)->pVtbl->TPDA_EnableDisable(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDAStr,pTPDMStr,bEnable);
}

/* ============================================================================
**  Function : DAL_TPDA_SetParam
** ============================================================================*/
/**
  Controls TPDM components.
  This function configures TPDM components per interface inputs.
 
  @param *phandle    [in] -- Handle to the TPDA DAL device.
  @param *pTPDAStr   [in] -- Name of TPDA.
  @param param_id    [in] -- Integer that specifies control function to be 
  invoked.
  @param val         [in] -- parameter for control function.
 
  @return
  DAL_SUCCESS -- TPDA Enablement is successful. 
  DAL_ERROR -- Invalid TPDA parameters or incorrect device state.
    
  @dependencies
  Call DAL_TPDADeviceAttach prior to calling this function.
*/

static __inline DALResult
DAL_TPDA_SetParam(DalDeviceHandle *phandle, const char *pTPDAStr,uint8 param_id,uint32 val)
{
   return ((DalTPDAHandle *)phandle)->pVtbl->TPDA_SetParam(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDAStr,param_id,val);
}

#endif
