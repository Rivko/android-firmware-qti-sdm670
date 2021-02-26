/*==============================================================================

FILE:      Dal<driver>.c

DESCRIPTION: This file implements a sample DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DAL<driver>.h"

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult 
<driver>_DriverInit(<driver>DrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
<driver>_DriverDeInit(<driver>DrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
------------------------------------------------------------------------------*/
DALResult 
<driver>_DeviceInit(<driver>ClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
<driver>_DeviceDeInit(<driver>ClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
<driver>_PowerEvent(<driver>ClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
<driver>_Open(<driver>ClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
<driver>_Close(<driver>ClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
<driver>_Info(<driver>ClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in <interface> Interface. 
------------------------------------------------------------------------------*/

<dal_user_interface_function_definition>


