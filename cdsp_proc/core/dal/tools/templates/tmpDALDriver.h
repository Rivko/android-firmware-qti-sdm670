#ifndef DAL<driver>_H
#define DAL<driver>_H
/*==============================================================================

FILE:      Dal<driver>.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "<interface_header_file>"

/*------------------------------------------------------------------------------
Declaring a "<driver>" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct <driver>DrvCtxt <driver>DrvCtxt;
typedef struct <driver>DevCtxt <driver>DevCtxt;
typedef struct <driver>ClientCtxt <driver>ClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "<driver>" Vtable
------------------------------------------------------------------------------*/
typedef struct <driver>DALVtbl <driver>DALVtbl;
struct <driver>DALVtbl
{
	int (*<driver>_DriverInit)(<driver>DrvCtxt *);
	int (*<driver>_DriverDeInit)(<driver>DrvCtxt *);
};

struct <driver>DevCtxt
{
	//Base Members
	uint32   dwRefs;                                    
    DALDEVICEID DevId;                                  
    uint32   dwDevCtxtRefIdx; 
    <driver>DrvCtxt  *p<driver>DrvCtxt;                             
	DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32 Reserved[16];
    //<driver> Dev state can be added by developers here
};

struct <driver>DrvCtxt
{
	//Base Members
	<driver>DALVtbl <driver>DALVtbl;
	uint32  dwNumDev;
	uint32  dwSizeDevCtxt;
	uint32  bInit;
    uint32  dwRefs;
	<driver>DevCtxt <driver>DevCtxt[<driver_num_devices>];
    //<driver> Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "<driver>" Client Context
------------------------------------------------------------------------------*/
struct <driver>ClientCtxt
{
	//Base Members
	uint32  dwRefs;                     
    uint32  dwAccessMode;  
    void *pPortCtxt;
    <driver>DevCtxt *p<driver>DevCtxt;            
    <interface>Handle <interface>Handle; 
	//<driver> Client state can be added by developers here

};

DALResult <driver>_DriverInit(<driver>DrvCtxt *);
DALResult <driver>_DriverDeInit(<driver>DrvCtxt *);
DALResult <driver>_DeviceInit(<driver>ClientCtxt *);
DALResult <driver>_DeviceDeInit(<driver>ClientCtxt *);
DALResult <driver>_Reset(<driver>ClientCtxt *);
DALResult <driver>_PowerEvent(<driver>ClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult <driver>_Open(<driver>ClientCtxt *, uint32);
DALResult <driver>_Close(<driver>ClientCtxt *);
DALResult <driver>_Info(<driver>ClientCtxt *,DalDeviceInfo *, uint32);
DALResult <driver>_InheritObjects(<driver>ClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to <interface> interface
------------------------------------------------------------------------------*/
<dal_user_interface_function_declaration>

#endif /*DAL<driver>_H*/


