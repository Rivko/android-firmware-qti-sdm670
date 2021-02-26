#ifndef DDITPDM_H
#define DDITPDM_H

//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tpdm/dal/inc/DDITPDM.h#1 $
//
//========================================================================
//             Copyright (c) 2011 QUALCOMM Technologies Incorporated.
//                    All Rights Reserved.
//                    QUALCOMM Proprietary
//========================================================================

#include "qdss_tpdm.h"
#include "DalDevice.h"

#define DALTPDM_STRING_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)


#define DAL_TPDM_SUCCESS          0x0
#define DAL_TPDM_ERROR            -1
#define DAL_TPDM_INVALID_BLOCK    0x2
#define DAL_TPDM_INCORRECT_OPTION 0x3

#define TPDM_NAME_LEN 32





                     
typedef struct DalTPDM DalTPDM;
struct DalTPDM
{
   struct DalDevice DalDevice;
   DALResult (*TPDM_EnableDisable)(uint32 ClientHandle, DalDeviceHandle* h,const char *pTPDMStr,uint32 TPDMTypeReq, DALBOOL bEnable);
   DALResult (*TPDM_Control)(uint32 ClientHandle, DalDeviceHandle* h,const char *pTPDMStr,uint8 param_id,uint32 val);
   DALResult (*TPDM_GetCompSupported)(uint32 ClientHandle, DalDeviceHandle *phandle,const char *pTPDMStr, uint32 *pVal);
   DALResult (*TPDM_GetTPDA)(uint32 ClientHandle, DalDeviceHandle *phandle,const char *pTPDMStr,const char **pTPDAVal);

   DALResult (*TPDM_Get_GPR)(uint32 ClientHandle, DalDeviceHandle *phandle,const char *pTPDMStr,uint32 GPRIndex,uint32 *pval);
   DALResult (*TPDM_Set_GPR)(uint32 ClientHandle, DalDeviceHandle *phandle,const char *pTPDMStr,uint32 GPRIndex,uint32 val);

   DALResult (*TPDM_DSB_SetEDCMR)(uint32 ClientHandle, DalDeviceHandle *phandle, const char *pTPDMStr,
                                  uint8 start, uint8 end, uint8 state);

   DALResult (*TPDM_DSB_GetParam)(uint32 ClientHandle, DalDeviceHandle* h,const char *pTPDMStr,uint8 param_id,uint32 *pval);
   DALResult (*TPDM_DSB_SetParam)(uint32 ClientHandle, DalDeviceHandle* h,const char *pTPDMStr,uint8 param_id,uint32 val);

   DALResult (*TPDM_CMB_GetParam)(uint32 ClientHandle, DalDeviceHandle* h,const char *pTPDMStr,uint8 param_id,uint32 *pval);
   DALResult (*TPDM_CMB_SetParam)(uint32 ClientHandle, DalDeviceHandle* h,const char *pTPDMStr,uint8 param_id,uint32 val);

};

typedef struct DalTPDMHandle DalTPDMHandle; 
struct DalTPDMHandle 
{
   uint32 dwDalHandleId;
   const DalTPDM *pVtbl;
   void * pClientCtxt;
};



/* ============================================================================
**  Function : DAL_TPDMDeviceAttach
** ============================================================================*/
/**
  Attached to a TPDM device
 
  @param DevId        [in] -- Must be "DALDEVICEID_LPA_TPDM"
  @param hDalDevice   [out] -- Pointer to a device handle.
 
  @return
  DAL_SUCCESS -- Successful. Other APIs can be invoked after this.
  DAL_ERROR -- Failure. 
    
  @dependencies
  None
*/


#define DAL_TPDMDeviceAttach(DevId,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL,DevId,DALTPDM_STRING_INTERFACE_VERSION,hDalDevice)



/* ============================================================================
**  Function : DAL_TPDM_EnableDisable
** ============================================================================*/
/**
  Enables or Disables a TPDM component.
  This function enables or disables a TPDM component and the TPDA component 
  that it is connected to.
 
  @param *phandle    [in] -- Handle to the TPDM DAL device.
  @param *pTPDMStr   [in] -- Name of TPDM.
  @param TPDMTypeReq [in] -- bit mask that reflects sub-types of TPDM that 
  needs to be enabled.
  @param bEnable     [in] -- TRUE(1)/FALSE(0) enables/disables the TPDM.
 
  @return
  DAL_SUCCESS -- TPDM Enablement is successful. 
  DAL_ERROR -- Invalid TPDM parameters or incorrect device state.
    
  @dependencies
  Call DAL_TPDMDeviceAttach prior to calling this function.
*/


static __inline DALResult
DAL_TPDM_EnableDisable(DalDeviceHandle *phandle,const char *pTPDMStr,uint32 TPDMTypeReq,DALBOOL bEnable)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_EnableDisable(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,TPDMTypeReq,bEnable);
}



/* ============================================================================
**  Function : DAL_TPDM_Control
** ============================================================================*/
/**
  Controls TPDM components.
  This function configures TPDM components per interface inputs.
 
  @param *phandle    [in] -- Handle to the TPDM DAL device.
  @param *pTPDMStr   [in] -- Name of TPDM.
  @param param_id    [in] -- Integer that specifies control function to be 
  invoked.
  @param val         [in] -- parameter for control function.
 
  @return
  DAL_SUCCESS -- TPDM Enablement is successful. 
  DAL_ERROR -- Invalid TPDM parameters or incorrect device state.
    
  @dependencies
  Call DAL_TPDMDeviceAttach prior to calling this function.
*/


static __inline DALResult
DAL_TPDM_Control(DalDeviceHandle *phandle, const char *pTPDMStr,uint8 
param_id,uint32 val)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_Control(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,param_id,val);
}

/* ============================================================================
**  Function : DAL_TPDM_GetCompSupported
** ============================================================================*/
/**
  Retrieve TPDM component types supported by a given TPDM.

 
  @param *phandle    [in] -- Handle to the TPDM DAL device.
  @param *pTPDMStr   [in] -- Name of TPDM.
  @param *pVal       [out] -- output integer that will be reflect the bit mask 
  of various TPDMs supported as specified in eTPDMTypes
 
  @return
  DAL_SUCCESS -- successful.
  DAL_ERROR -- Invalid TPDM parameters or incorrect device state.
    
  @dependencies
  Call DAL_TPDMDeviceAttach prior to calling this function.
*/

static __inline DALResult DAL_TPDM_GetCompSupported(DalDeviceHandle *phandle,const char *pTPDMStr, uint32 *pVal)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_GetCompSupported(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,pVal);
}

/* ============================================================================
**  Function : DAL_TPDM_GetTPDA
** ============================================================================*/
/**
  Retrieve TPDA name that a given TPDM is connected to.
 
  @param *phandle    [in] -- Handle to the TPDM DAL device.
  @param *pTPDMStr   [in] -- Name of TPDM.
  @param **pTPDAVal  [out] -- Name of TPDA.
 
  @return
  DAL_SUCCESS -- successful.
  DAL_ERROR -- Invalid TPDM parameters or incorrect device state.
    
  @dependencies
  Call DAL_TPDMDeviceAttach prior to calling this function.
*/

static __inline DALResult DAL_TPDM_GetTPDA(DalDeviceHandle *phandle,const char 
*pTPDMStr,const char **pTPDAVal)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_GetTPDA(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,pTPDAVal);
   
}

static __inline DALResult DAL_TPDM_Get_GPR(DalDeviceHandle *phandle,const char *pTPDMStr,
               uint32 GPRIndex,uint32 *pval)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_Get_GPR(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,GPRIndex,pval);
}



/* ============================================================================
**  Function : DAL_TPDM_Set_GPR
** ============================================================================*/
/**
  Write to GPR registers by specifying an register index/offset.
 
  @param *phandle    [in] -- Handle to the TPDM DAL device.
  @param *pTPDMStr   [in] -- Name of TPDM.
  @param GPRIndex    [in] -- Integer GPR register offset between 0-159.
  @param val         [in] -- register 32-bit value to be written.
 
  @return
  DAL_SUCCESS -- register write successful
  DAL_ERROR -- Invalid TPDM parameters or incorrect device state.
    
  @dependencies
  Call DAL_TPDMDeviceAttach prior to calling this function.
*/


static __inline DALResult DAL_TPDM_Set_GPR(DalDeviceHandle *phandle,const char *pTPDMStr,
               uint32 GPRIndex,uint32 val)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_Set_GPR(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,GPRIndex,val);
}


static __inline DALResult
DAL_TPDM_DSB_SetEDCMR(DalDeviceHandle *phandle, const char *pTPDMStr,
                                  uint8 start, uint8 end, uint8 state)
{
  return ((DalTPDMHandle*)phandle)->pVtbl->TPDM_DSB_SetEDCMR(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,start,end, state);
}


static __inline DALResult
DAL_TPDM_DSB_GetParam(DalDeviceHandle *phandle, const char *pTPDMStr,uint8 param_id,uint32 *pval)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_DSB_GetParam(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,param_id,pval);
}



static __inline DALResult
DAL_TPDM_DSB_SetParam(DalDeviceHandle *phandle, const char *pTPDMStr,uint8 param_id,uint32 val)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_DSB_SetParam(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,param_id,val);
}

static __inline DALResult
DAL_TPDM_CMB_GetParam(DalDeviceHandle *phandle, const char *pTPDMStr,uint8 param_id,uint32 *pval)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_CMB_GetParam(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,param_id,pval);
}

static __inline DALResult
DAL_TPDM_CMB_SetParam(DalDeviceHandle *phandle, const char *pTPDMStr,uint8 param_id,uint32 val)
{
   return ((DalTPDMHandle 
   *)phandle)->pVtbl->TPDM_CMB_SetParam(DALDEVICE_LOCAL_CLIENTID, phandle,pTPDMStr,param_id,val);
}




#endif
