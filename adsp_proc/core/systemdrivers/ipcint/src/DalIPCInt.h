#ifndef DALIPCInt_H
#define DALIPCInt_H
/*==============================================================================

FILE:      DalIPCInt.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright (c) 2008-2016 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "DDIIPCInt.h"

/*------------------------------------------------------------------------------
Declaring a "IPCInt" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct IPCIntDrvCtxt IPCIntDrvCtxt;
typedef struct IPCIntDevCtxt IPCIntDevCtxt;
typedef struct IPCIntClientCtxt IPCIntClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "IPCInt" Vtable
------------------------------------------------------------------------------*/
typedef struct IPCIntDALVtbl IPCIntDALVtbl;
struct IPCIntDALVtbl
{
	int (*IPCInt_DriverInit)(IPCIntDrvCtxt *);
	int (*IPCInt_DriverDeInit)(IPCIntDrvCtxt *);
};

typedef struct
{
  uint32 nBaseAddress;
  uint32 nBaseAddressOffset;
  uint16 nBitOffset;
  uint16 nNumBits;
}
IPCIntConfigType;

typedef struct
{
  DalIPCIntProcessorType eProcessor;
  IPCIntConfigType*      nMap;
}
IPCIntCntrlType;

struct IPCIntDevCtxt
{
	//Base Members
	uint32   dwRefs;                                    
    DALDEVICEID DevId;                                  
    uint32   dwDevCtxtRefIdx; 
    IPCIntDrvCtxt  *pIPCIntDrvCtxt;                             
    DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32 Reserved[16];
    //IPCInt Dev state can be added by developers here
    IPCIntCntrlType ipcint_cntrl;    
    DALSYS_SYNC_OBJECT(IPCIntSyncObject);
    DALSYSSyncHandle  IPCInt_synchronization;
};

struct IPCIntDrvCtxt
{
	//Base Members
	IPCIntDALVtbl IPCIntDALVtbl;
	uint32  dwNumDev;
	uint32  dwSizeDevCtxt;
	uint32  bInit;
    uint32  dwRefs;
	IPCIntDevCtxt IPCIntDevCtxt[1];
    //IPCInt Drv state can be added by developers here
    DalIPCIntProcessorType eSourceProcessor; 
};

/*------------------------------------------------------------------------------
Declaring a "IPCInt" Client Context
------------------------------------------------------------------------------*/
struct IPCIntClientCtxt
{
	//Base Members
	uint32  dwRefs;                     
    uint32  dwAccessMode;  
    void *pPortCtxt;
    IPCIntDevCtxt *pIPCIntDevCtxt;           
    DalIPCIntHandle DalIPCIntHandle; 
	//IPCInt Client state can be added by developers here

};

#define IPC_NULL_BASE         0x0
#define IPC_NULL_BASE_OFFSET  0x0


/*------------------------------------------------------------------------------
Functions generic to DAL interface
------------------------------------------------------------------------------*/
DALResult IPCInt_DriverInit(IPCIntDrvCtxt *);
DALResult IPCInt_DriverDeInit(IPCIntDrvCtxt *);
DALResult IPCInt_DeviceInit(IPCIntClientCtxt *);
DALResult IPCInt_DeviceDeInit(IPCIntClientCtxt *);
DALResult IPCInt_Reset(IPCIntClientCtxt *);
DALResult IPCInt_PowerEvent(IPCIntClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult IPCInt_Open(IPCIntClientCtxt *, uint32);
DALResult IPCInt_Close(IPCIntClientCtxt *);
DALResult IPCInt_Info(IPCIntClientCtxt *,DalDeviceInfo *, uint32);
DALResult IPCInt_InheritObjects(IPCIntClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to DalIPCInt interface
------------------------------------------------------------------------------*/
DALResult IPCInt_Trigger( IPCIntClientCtxt *,  DalIPCIntProcessorType ,  DalIPCIntInterruptType );
DALResult IPCInt_TriggerBySource( IPCIntClientCtxt *,  DalIPCIntProcessorType ,  DalIPCIntProcessorType ,  DalIPCIntInterruptType );
DALResult IPCInt_IsSupported( IPCIntClientCtxt *,  DalIPCIntProcessorType ,  DalIPCIntInterruptType );
DALResult IPCInt_IsSupportedBySource( IPCIntClientCtxt *,  DalIPCIntProcessorType ,  DalIPCIntProcessorType ,  DalIPCIntInterruptType );

/*------------------------------------------------------------------------------
Functions required for platform configuration
------------------------------------------------------------------------------*/
void IPCInt_GetConfigMap( IPCIntConfigType **ppMap );
void IPCInt_InitConfig( void );

#endif /*DALIPCInt_H*/

