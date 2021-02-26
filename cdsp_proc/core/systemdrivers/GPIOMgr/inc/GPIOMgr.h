#ifndef DALGPIOMgr_H
#define DALGPIOMgr_H
/*==============================================================================

FILE:      GPIOMgr.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright © 2015-2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "DDIGPIOMgr.h"

/*------------------------------------------------------------------------------
Declaring a "GPIOMgr" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct GPIOMgrDrvCtxt GPIOMgrDrvCtxt;
typedef struct GPIOMgrDevCtxt GPIOMgrDevCtxt;
typedef struct GPIOMgrClientCtxt GPIOMgrClientCtxt;

/*------------------------------------------------------------------------------
Defining Internal Structures
------------------------------------------------------------------------------*/

typedef struct
{
  uint8  nPD; /* LockGPIO */
  uint8  nDirConnID; /* GetDirConn */
  uint8  nTargetProc; /* GetDirConn */
  uint8  nFlags; 
}
GPIOMgr_StateType;

/*------------------------------------------------------------------------------
Declaring a private "GPIOMgr" Vtable
------------------------------------------------------------------------------*/
typedef struct GPIOMgrDALVtbl GPIOMgrDALVtbl;
struct GPIOMgrDALVtbl
{
  int (*GPIOMgr_DriverInit)(GPIOMgrDrvCtxt *);
  int (*GPIOMgr_DriverDeInit)(GPIOMgrDrvCtxt *);
};

struct GPIOMgrDevCtxt
{
  uint32   dwRefs;                                    
  DALDEVICEID DevId;                                  
  uint32   dwDevCtxtRefIdx; 
  GPIOMgrDrvCtxt  *pGPIOMgrDrvCtxt;                             
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];
  DALSYS_SYNC_OBJECT(hSyncObj);
  DALSYSSyncHandle hMutex;
  uint32 nMaxGpios;
  GPIOMgr_StateType *pState;
  GPIOMgrConfigMapType *pDirConnMap;
  GPIOMgrConfigPDType *pPDMap;
  uint32 nDirConnMapSize;
  uint32 nPDMapSize;
};

struct GPIOMgrDrvCtxt
{
  GPIOMgrDALVtbl  GPIOMgrDALVtbl;
  uint32          dwNumDev;
  uint32          dwSizeDevCtxt;
  uint32          bInit;
  uint32          dwRefs;
  GPIOMgrDevCtxt  GPIOMgrDevCtxt[1];
};

/*------------------------------------------------------------------------------
Declaring a "GPIOMgr" Client Context
------------------------------------------------------------------------------*/

struct GPIOMgrClientCtxt
{
  uint32          dwRefs;                     
  uint32          dwAccessMode;  
  void            *pPortCtxt;
  GPIOMgrDevCtxt  *pGPIOMgrDevCtxt;            
  GPIOMgrIFHandle GPIOMgrIFHandle; 
};



DALResult GPIOMgr_DriverInit(GPIOMgrDrvCtxt *);
DALResult GPIOMgr_DriverDeInit(GPIOMgrDrvCtxt *);

DALResult GPIOMgr_LockGPIO(uint32, DalDeviceHandle*, uint32, uint32);
DALResult GPIOMgr_ReleaseGPIO(uint32, DalDeviceHandle*, uint32);
DALResult GPIOMgr_GetGPIOCurrentPD(uint32, DalDeviceHandle*, uint32, uint32*);
DALResult GPIOMgr_GetDirectConnectGPIO(uint32, DalDeviceHandle*, uint32, uint32*);
DALResult GPIOMgr_ReleaseDirectConnectGPIO(uint32, DalDeviceHandle*, uint32, uint32);
DALResult GPIOMgr_GetDirectConnectInterruptID(uint32, DalDeviceHandle*, uint32, uint32*);

/*------------------------------------------------------------------------------
Functions specific to GPIOMgrIF interface
------------------------------------------------------------------------------*/
//<dal_user_interface_function_declaration>

#endif /*DALGPIOMgr_H*/


