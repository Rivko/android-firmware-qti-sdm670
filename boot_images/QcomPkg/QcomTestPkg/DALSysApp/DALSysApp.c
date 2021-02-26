/** @file DALSysApp.c

  Test application for DALSYS

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/08/14   ap      Fix cast errors for 64 bit
 07/30/11   sy      Adding TestInterface Library
 01/27/11   sho     Add history

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "DALFramework.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DALGlbCtxt.h"
#include <Library/TestInterface.h>


void * callBackTest(void *funcCtxt, uint32 dwparam, void *pPayload, 
      uint32 dwPayloadSize)
{
   ASSERT_EFI_ERROR((DALSYSMemAddr)funcCtxt == 0xdead0001);
   ASSERT_EFI_ERROR(dwparam  == 0xdead0002);
   ASSERT_EFI_ERROR(dwPayloadSize == 0xdead0003);
   *((int *)(pPayload)) = 0xdeaddead;
   return NULL;
}

EFI_STATUS
EFIAPI
DALSysAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
   DALResult ret;
   DALSYSEventHandle hEvent;
   DALSYSEventObj eventObj;
   DALSYSMemHandle hMem;
   DALSYSMemObj memObj;
   DALSYSMemInfo memInfo;
   void *buffer;
   unsigned int payload = 0;
   int *pContextInt1;
   int *pContextInt2;

   TEST_START("DALSysAppMain");
   DALSYS_InitMod(NULL);

   // Event Testing
   ret = DALSYS_EventCreate( DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hEvent, 
         &eventObj );
   ASSERT_EFI_ERROR(ret);

   ret = DALSYS_SetupCallbackEvent( hEvent, callBackTest, (void *)0xdead0001 );
   ASSERT_EFI_ERROR(ret);

   ret = DALSYS_EventCtrlEx( hEvent, DALSYS_EVENT_CTRL_TRIGGER, 0xdead0002, 
         &payload, 0xdead0003);
   ASSERT_EFI_ERROR(ret);
   ASSERT_EFI_ERROR(payload == 0xdeaddead);

   ret = DALSYS_MemRegionAlloc( 0, DALSYS_MEM_ADDR_NOT_SPECIFIED, 
                                DALSYS_MEM_ADDR_NOT_SPECIFIED, 50, &hMem, NULL );
   ASSERT_EFI_ERROR(ret);

   ret = DALSYS_MemRegionAlloc( 0, DALSYS_MEM_ADDR_NOT_SPECIFIED, 
                                DALSYS_MEM_ADDR_NOT_SPECIFIED, 50, &hMem, &memObj );
   ASSERT_EFI_ERROR(ret);
   ASSERT_EFI_ERROR(hMem == &memObj);

   ret = DALSYS_MemInfo(hMem, &memInfo);
   ASSERT_EFI_ERROR(ret);
   ASSERT_EFI_ERROR(memInfo.dwLen == 50);

   ret = DALSYS_MemRegionAlloc( DALSYS_MEM_PROPS_PHYS_PREALLOC, 0x12345678, 
         0x12345678, 50, &hMem, &memObj );
   ASSERT_EFI_ERROR(ret);
   ASSERT_EFI_ERROR(hMem == &memObj);

   // Cache Commands
   ret = DALSYS_CacheCommand(DALSYS_CACHE_CMD_INVALIDATE, 
         memInfo.VirtualAddr, 50);
   ASSERT_EFI_ERROR(ret);

   ret = DALSYS_CacheCommand(DALSYS_CACHE_CMD_CLEAN, memInfo.VirtualAddr, 50);
   ASSERT_EFI_ERROR(ret);

   ret = DALSYS_CacheCommand(DALSYS_CACHE_CMD_FLUSH, memInfo.VirtualAddr, 50);
   ASSERT_EFI_ERROR(ret);

   // dynamic memory
   ret = DALSYS_Malloc(100, &buffer);
   ASSERT_EFI_ERROR(ret);
   ASSERT_EFI_ERROR(NULL != buffer);

   ret = DALSYS_Free( buffer );
   ASSERT_EFI_ERROR(ret);
   

   // Busy Wait
   DALSYS_LogEvent(DALDEVICEID_TIMER, DALSYS_LOGEVENT_WARNING, 
                         "Wait for the next msg in 1 sec");
   DALSYS_BusyWait(1000000); // 1 sec
   DALSYS_LogEvent(DALDEVICEID_TIMER, DALSYS_LOGEVENT_WARNING, 
                         "MSG");

   // Global Context
   ret = DALGLBCTXT_Init();
   ASSERT_EFI_ERROR(ret);

   ret = DALGLBCTXT_AllocCtxt("Test Context", 100, DALGLBCTXT_LOCK_TYPE_NONE, 
         (void **)&pContextInt1);
   ASSERT_EFI_ERROR(ret);

   ret = DALGLBCTXT_FindCtxt("Test Context", (void **)&pContextInt2 );
   ASSERT_EFI_ERROR(ret);
   ASSERT_EFI_ERROR(*pContextInt1 == *pContextInt2);

   TestStatus("DALSysAppMain",EFI_SUCCESS);
   TEST_STOP("DALSysAppMain");
   return DAL_SUCCESS;
}
