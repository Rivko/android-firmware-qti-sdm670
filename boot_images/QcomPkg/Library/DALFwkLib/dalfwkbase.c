/*==============================================================================
FILE:  DALdwkbase.c

SERVICES:  DAL Framework Base functions

GENERAL DESCRIPTION:

        Copyright (c) 2007,2010,2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
               All Rights Reserved.
            Qualcomm Confidential and Proprietary
==============================================================================*/
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "DALStdDef.h"
#include "DALFramework.h"
#include "DALSys.h"
#include "DALSysCmn.h"
#include <Library/BaseLib.h>

/*------------------------------------------------------------------------------
Functions local to this file
------------------------------------------------------------------------------*/
static void DALFW_InitDrvCtxt(DALDrvCtxt *pdalDrvCtxt);
static void DALFW_DeInitDrvCtxt(DALDrvCtxt *pdalDrvCtxt);
static void DALFW_DeInitDevCtxt(DALDevCtxt *pdalDrvCtxt);
static void DALFW_InitDevCtxt(DALDevCtxt * pdalDevCtxt, DALDrvCtxt *pdalDrvCtxt,
                       uint32 dwdevRefIdx, DALDEVICEID devId);
static void DALFW_InitClientCtxt(DALClientCtxt *pdalClientCtxt,
                          DALDevCtxt *pdalDevCtxt, uint32 bisMgr);

#define DALFW_GET_DEV_CTXT(pDrvCtxt,devIdx) \
   ((DALDevCtxt *)(((byte *)(pDrvCtxt->DALDevCtxt))+(pDrvCtxt->dwSizeDevCtxt*devIdx)))

static void DALFW_InitDrvCtxt(DALDrvCtxt *pdalDrvCtxt)
{
    if(!pdalDrvCtxt->bInit)
   {
      uint32 dwdevIdx = 0;
      pdalDrvCtxt->dwRefs = 0;
      pdalDrvCtxt->bInit  = 1;
      //Mark all DAL Device context's as availaible. Dev Id of ZERO (0) is not
      //allowed in our system

      for(dwdevIdx=0;dwdevIdx<pdalDrvCtxt->dwNumDev;dwdevIdx++)
      {
         DALDevCtxt *pDevCtxt = DALFW_GET_DEV_CTXT(pdalDrvCtxt,dwdevIdx);
         pDevCtxt->DevId = 0;
      }
      //Call the driver initialization function
      if(pdalDrvCtxt->DALDrvVtbl.DAL_DriverInit)
      {
         pdalDrvCtxt->DALDrvVtbl.DAL_DriverInit(pdalDrvCtxt);
      }
   }
}
static void DALFW_DeInitDrvCtxt(DALDrvCtxt *pdalDrvCtxt)
{
    if(pdalDrvCtxt->bInit)
   {
      pdalDrvCtxt->dwRefs = 0;
      pdalDrvCtxt->bInit  = 0;
      //call the driver de-initialization function
      if(pdalDrvCtxt->DALDrvVtbl.DAL_DriverDeInit)
      {
         pdalDrvCtxt->DALDrvVtbl.DAL_DriverDeInit(pdalDrvCtxt);
      }
   }
}
static void DALFW_InitDevCtxt(DALDevCtxt * pdalDevCtxt, DALDrvCtxt *pdalDrvCtxt,
                       uint32 dwdevRefIdx, DALDEVICEID devId)
{
    pdalDevCtxt->dwRefs = 0;
    pdalDevCtxt->DevId = devId;
    pdalDevCtxt->pDALDrvCtxt = pdalDrvCtxt;
    pdalDevCtxt->dwDevCtxtRefIdx = dwdevRefIdx;
}

static void DALFW_DeInitDevCtxt(DALDevCtxt *pdalDevCtxt)
{
   pdalDevCtxt->dwRefs = 0;
   pdalDevCtxt->DevId  = 0;
   pdalDevCtxt->pDALDrvCtxt = NULL;
   pdalDevCtxt->dwDevCtxtRefIdx = 0;
   pdalDevCtxt->strDeviceName = NULL;
}

static void DALFW_InitClientCtxt(DALClientCtxt *pdalClientCtxt,
                          DALDevCtxt *pdalDevCtxt, uint32 bisMgr)
{
    //Client has not been initialized
    if(NULL == pdalClientCtxt->pDALDevCtxt)
    {
        pdalClientCtxt->dwRefs = 0;
        pdalClientCtxt->dwAccessMode = 0;
        pdalClientCtxt->pDALDevCtxt = pdalDevCtxt;
    }
}

DALResult 
DALFW_AttachToStringDevice(const char *pszDeviceName,  DALDrvCtxt *pdalDrvCtxt, DALClientCtxt *pClientCtxt)
{
   uint32 dwdevIdx = 0;
   uint32 bisClientMgr = 0;
   uint32 dwFreeSlotIdx = pdalDrvCtxt->dwNumDev;
   DALDevCtxt *pDevCtxt  = NULL;
   
   //Initialize driver context only once.
   DALFW_InitDrvCtxt(pdalDrvCtxt);
 
   //Search if we have reserved a slot for this device?
   for(dwdevIdx=0;dwdevIdx<pdalDrvCtxt->dwNumDev;dwdevIdx++)
   {
     DALDevCtxt *pTmpDevCtxt = DALFW_GET_DEV_CTXT(pdalDrvCtxt,dwdevIdx);
      if(!pTmpDevCtxt->strDeviceName) 
        dwFreeSlotIdx = dwdevIdx;
      else if(AsciiStrLen(pszDeviceName) == AsciiStrLen(pTmpDevCtxt->strDeviceName))
      {
         if(AsciiStrnCmp(pszDeviceName, pTmpDevCtxt->strDeviceName, AsciiStrLen(pszDeviceName))== 0)
         {
            pDevCtxt = pTmpDevCtxt; 
            dwFreeSlotIdx = pdalDrvCtxt->dwNumDev;
            break;
         }
      }      
   }
   //If you reach this point either a match existed or a new slot needed
   if(dwFreeSlotIdx != pdalDrvCtxt->dwNumDev)
   {
      //Initialize Device context here
     pDevCtxt = DALFW_GET_DEV_CTXT(pdalDrvCtxt,dwFreeSlotIdx);
     DALFW_InitDevCtxt(pDevCtxt,pdalDrvCtxt,dwFreeSlotIdx,0);
     pDevCtxt->strDeviceName = pszDeviceName; //Initialize the device name
     bisClientMgr = 1;
   }
   //Initialize client context
   if(!pClientCtxt->pDALDevCtxt)
    {
        DALFW_InitClientCtxt(pClientCtxt,pDevCtxt,bisClientMgr);
    }
    
   return pDevCtxt?DAL_SUCCESS:DAL_ERROR; 
}

DALResult DALFW_AttachToDevice(DALDEVICEID devId, DALDrvCtxt *pdalDrvCtxt,
                           DALClientCtxt *pdalClientCtxt)
{
   uint32 bisClientMgr = 0;
   uint32 dwdevIdx = 0;
   uint32 dwcurDevIdx = 0;
   uint32 dwcount = 0;

   typedef enum DevAttachState
   {
      DEV_EXISTS,
      DEV_NEW,
      DEV_NO_SLOT
   }DevAttachState;
   DevAttachState devAttachState = DEV_NO_SLOT;

   //Init the driver ctxt, this function is no-op if already inited
   DALFW_InitDrvCtxt(pdalDrvCtxt);

   //Search if we have reserved a slot for this device?
   for(dwdevIdx=(pdalDrvCtxt->dwNumDev-1),dwcount=0;
      dwcount<pdalDrvCtxt->dwNumDev;dwdevIdx--,dwcount++)
   {
      DALDevCtxt *pDevCtxt = DALFW_GET_DEV_CTXT(pdalDrvCtxt,dwdevIdx);
      if(devId == pDevCtxt->DevId )
      {
         //this device id exists
         devAttachState = DEV_EXISTS;
         dwcurDevIdx = dwdevIdx;
         break;
      }
      //get first free slot
      if(0 == pDevCtxt->DevId)
      {
         devAttachState = DEV_NEW;
         dwcurDevIdx = dwdevIdx;
      }
   }

    switch (devAttachState)
   {
      case DEV_EXISTS:
         bisClientMgr = 0;
         break;

      case DEV_NEW:
         DALFW_InitDevCtxt(DALFW_GET_DEV_CTXT(pdalDrvCtxt,dwcurDevIdx),
                         pdalDrvCtxt,dwcurDevIdx,devId);
         bisClientMgr = 1;
         break;

      default:
         return DAL_ERROR;
   }

   //Initialize Client context
   DALFW_InitClientCtxt(pdalClientCtxt,DALFW_GET_DEV_CTXT(pdalDrvCtxt,dwcurDevIdx),
                   bisClientMgr);

   return DAL_SUCCESS;
}

void DALFW_MarkDeviceStatic(DALDevCtxt *pdalDevCtxt)
{
   // increment the device ref count
   ++(pdalDevCtxt->dwRefs);

   // increment the driver ref count
   ++(pdalDevCtxt->pDALDrvCtxt->dwRefs);
}

uint32 DALFW_AddRef(DALClientCtxt *pdalClientCtxt)
{
    //Increment the device ref count
    ++(pdalClientCtxt->pDALDevCtxt->dwRefs);

    //Increment the driver ref count
    ++(pdalClientCtxt->pDALDevCtxt->pDALDrvCtxt->dwRefs);

    //Increment the client connection count
    return ++(pdalClientCtxt->dwRefs);

}

uint32 DALFW_Release(DALClientCtxt *pdalClientCtxt)
{
   DALDrvCtxt *pdalDrvCtxt = pdalClientCtxt->pDALDevCtxt->pDALDrvCtxt;
   uint32 uret;

   if(0 == pdalClientCtxt->dwRefs)
      return 0;

   //Decrement driver ref count
   if(0 == --(pdalDrvCtxt->dwRefs))
   {
      //DeInit
      DALFW_DeInitDrvCtxt(pdalDrvCtxt);
    }
   //Decrement device ref count
   if(0 == --(pdalClientCtxt->pDALDevCtxt->dwRefs))
    {
        //mark this device as no longer in use
      DALFW_DeInitDevCtxt(pdalClientCtxt->pDALDevCtxt);
    }
    //decrement the client ref count
    uret = --(pdalClientCtxt->dwRefs);
    if(0 == uret)
      pdalClientCtxt->pDALDevCtxt = NULL;
    return uret;
}
