#ifndef DALInterruptController_H
#define DALInterruptController_H
/*==============================================================================

FILE:      DalInterruptController.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "DDIInterruptController.h"

#define MAX_DAL_INTERRUPT_ID    512
#define DAL_INTERRUPT_TRIGGER_MASK 0x7
#define DAL_INTERRUPT_PRIORITY_MASK 0x000000ff
#define DAL_INTERRUPT_PRIORITY_SHFT 24
#define DAL_INTERRUPT_ISTSTACK_SHFT 4
#define DAL_INTERRUPT_ISTSTACK_MASK 0xFFFF

#define DAL_UINTERRUPT_CONTROLLER_SHFT 8
#define DAL_UINTERRUPT_CONTROLLER_MASK 0x1

/*------------------------------------------------------------------------------
Declaring a "InterruptController" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct InterruptControllerDrvCtxt InterruptControllerDrvCtxt;
typedef struct InterruptControllerDevCtxt InterruptControllerDevCtxt;
typedef struct InterruptControllerClientCtxt InterruptControllerClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "InterruptController" Vtable
------------------------------------------------------------------------------*/
typedef struct InterruptControllerDALVtbl InterruptControllerDALVtbl;
struct InterruptControllerDALVtbl
{
   int (*InterruptController_DriverInit)(InterruptControllerDrvCtxt *);
   int (*InterruptController_DriverDeInit)(InterruptControllerDrvCtxt *);
};

struct InterruptControllerDevCtxt
{
   //Base Members
   uint32   dwRefs;                                    
    DALDEVICEID DevId;                                  
    uint32   dwDevCtxtRefIdx; 
    InterruptControllerDrvCtxt  *pInterruptControllerDrvCtxt;                             
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32 Reserved[16];
    //InterruptController Dev state can be added by developers here
};

struct InterruptControllerDrvCtxt
{
   //Base Members
   InterruptControllerDALVtbl InterruptControllerDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
    uint32  dwRefs;
   InterruptControllerDevCtxt InterruptControllerDevCtxt[1];
    //InterruptController Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "InterruptController" Client Context
------------------------------------------------------------------------------*/
struct InterruptControllerClientCtxt
{
   //Base Members
   uint32  dwRefs;                     
    uint32  dwAccessMode;  
    void *pPortCtxt;
    InterruptControllerDevCtxt *pInterruptControllerDevCtxt;            
    DalInterruptControllerHandle DalInterruptControllerHandle; 
   //InterruptController Client state can be added by developers here

   // this driver only has one instance
   // so store globals here
   DALSYSSyncHandle hSyncIntrCtrlTbl;
   uint32 IntrCtrlTblNextSlot;
   void * IntrCtrlTbl[MAX_DAL_INTERRUPT_ID];
};

DALResult InterruptController_DriverInit(InterruptControllerDrvCtxt *);
DALResult InterruptController_DriverDeInit(InterruptControllerDrvCtxt *);
DALResult InterruptController_DeviceInit(InterruptControllerClientCtxt *);
DALResult InterruptController_DeviceDeInit(InterruptControllerClientCtxt *);
DALResult InterruptController_Reset(InterruptControllerClientCtxt *);
DALResult InterruptController_PowerEvent(InterruptControllerClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult InterruptController_Open(InterruptControllerClientCtxt *, uint32);
DALResult InterruptController_Close(InterruptControllerClientCtxt *);
DALResult InterruptController_Info(InterruptControllerClientCtxt *,DalDeviceInfo *, uint32);
DALResult InterruptController_InheritObjects(InterruptControllerClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to DalInterruptController interface
------------------------------------------------------------------------------*/
DALResult InterruptController_RegisterISR( InterruptControllerClientCtxt *, DALInterruptID , const DALISR , const DALISRCtx , uint32 );
DALResult  InterruptController_RegisterIST( InterruptControllerClientCtxt *,DALInterruptID , const DALISR , const DALISRCtx , uint32, char * ); 
DALResult InterruptController_RegisterEvent( InterruptControllerClientCtxt *, DALInterruptID , const DALSYSEventHandle , uint32 );
DALResult InterruptController_Unregister( InterruptControllerClientCtxt *, DALInterruptID );
DALResult InterruptController_InterruptDone( InterruptControllerClientCtxt *, DALInterruptID );
DALResult InterruptController_InterruptEnable( InterruptControllerClientCtxt *, DALInterruptID );
DALResult InterruptController_InterruptDisable( InterruptControllerClientCtxt *, DALInterruptID );
DALResult InterruptController_InterruptTrigger( InterruptControllerClientCtxt *, DALInterruptID );
DALResult InterruptController_InterruptClear( InterruptControllerClientCtxt *, DALInterruptID );
DALResult InterruptController_InterruptStatus( InterruptControllerClientCtxt *, DALInterruptID );
DALResult InterruptController_SetTrigger(InterruptControllerClientCtxt *, DALInterruptID ,uint32 );
DALResult InterruptController_IsInterruptPending(InterruptControllerClientCtxt *, DALInterruptID ,uint32*);
DALResult InterruptController_IsInterruptEnabled(InterruptControllerClientCtxt *, DALInterruptID,uint32*);
DALResult InterruptController_IsAnyInterruptPending(InterruptControllerClientCtxt *,uint32* bState);
DALResult InterruptController_MapWakeupInterrupt(InterruptControllerClientCtxt * , DALInterruptID ,uint32 );
DALResult InterruptController_Sleep(InterruptControllerClientCtxt * , InterruptControllerSleepType sleep);
DALResult InterruptController_Wakeup(InterruptControllerClientCtxt * , InterruptControllerSleepType sleep);
DALResult InterruptController_GetInterruptTrigger(InterruptControllerClientCtxt *, DALInterruptID ,uint32* );
DALResult InterruptController_LogState(InterruptControllerClientCtxt *,void *);
DALResult InterruptController_GetInterruptID(InterruptControllerClientCtxt  *pclientCtxt,const char* szIntrName,uint32* pnIntrID);
#endif /*DALInterruptController_H*/


