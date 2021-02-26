#ifndef DALTimer_H
#define DALTimer_H
/*==============================================================================

FILE:      DalTimer.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "DDITimer.h"

/*------------------------------------------------------------------------------
Declaring a "Timer" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct TimerDrvCtxt TimerDrvCtxt;
typedef struct TimerDevCtxt TimerDevCtxt;
typedef struct TimerClientCtxt TimerClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "Timer" Vtable
------------------------------------------------------------------------------*/
typedef struct TimerDALVtbl TimerDALVtbl;
struct TimerDALVtbl
{
	int (*Timer_DriverInit)(TimerDrvCtxt *);
	int (*Timer_DriverDeInit)(TimerDrvCtxt *);
};

struct TimerDevCtxt
{
	//Base Members
	uint32   dwRefs;                                    
    DALDEVICEID DevId;                                  
    uint32   dwDevCtxtRefIdx; 
    TimerDrvCtxt  *pTimerDrvCtxt;                             
	DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32 Reserved[16];
    //Timer Dev state can be added by developers here
};

struct TimerDrvCtxt
{
	//Base Members
	TimerDALVtbl TimerDALVtbl;
	uint32  dwNumDev;
	uint32  dwSizeDevCtxt;
	uint32  bInit;
    uint32  dwRefs;
	TimerDevCtxt TimerDevCtxt[1];
    //Timer Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "Timer" Client Context
------------------------------------------------------------------------------*/
struct TimerClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   TimerDevCtxt *pTimerDevCtxt;
   DalTimerHandle DalTimerHandle;
   //Timer Client state can be added by developers here
};

DALResult Timer_DriverInit(TimerDrvCtxt *);
DALResult Timer_DriverDeInit(TimerDrvCtxt *);
DALResult Timer_DeviceInit(TimerClientCtxt *);
DALResult Timer_DeviceDeInit(TimerClientCtxt *);
DALResult Timer_Reset(TimerClientCtxt *);
DALResult Timer_PowerEvent(TimerClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult Timer_Open(TimerClientCtxt *, uint32);
DALResult Timer_Close(TimerClientCtxt *);
DALResult Timer_Info(TimerClientCtxt *,DalDeviceInfo *, uint32);
DALResult Timer_InheritObjects(TimerClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to DalTimer interface
------------------------------------------------------------------------------*/
DALResult Timer_Register( TimerClientCtxt *, const DALSYSEventHandle , uint32 );
DALResult Timer_UnRegister( TimerClientCtxt *, const DALSYSEventHandle );
DALResult Timer_GetTimerCount( TimerClientCtxt *, void * , uint32 );

#endif /*DALTimer_H*/


