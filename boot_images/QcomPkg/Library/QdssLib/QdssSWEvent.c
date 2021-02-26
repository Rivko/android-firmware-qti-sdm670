/*=============================================================================

FILE:         QdssSWEvent.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "QdssSWEvent.h"
#include <Library/UefiBootServicesTableLib.h>     //gBS
#include <Protocol/EFIQdss.h>  


EFI_QDSS_PROTOCOL *gQdssProtocolPtr = NULL;


EFI_STATUS QdssSWEventInit(void) 
{
   EFI_STATUS Status = EFI_SUCCESS;
   
   if (NULL == gQdssProtocolPtr) {

      Status = gBS->LocateProtocol(&gEfiQdssProtocolGuid,
                                   NULL,
                                   (VOID **)&gQdssProtocolPtr
                                   );
   }
   return Status;
}

void QdssSWEvent(IN UINT32 SwEventIdNum)
{
   QdssSWEventInit(); //idempotent

   if (NULL != gQdssProtocolPtr) {
      gQdssProtocolPtr->TraceSwEvent(SwEventIdNum);
   }
}


void QdssSWEventData(IN UINT32 SwEventIdNum,
                     IN UINT32 NumberArgs,
                     ...)
{
   QdssSWEventInit();//idempotent

   if (NULL != gQdssProtocolPtr) {
      VA_LIST varg_list;

      VA_START(varg_list, NumberArgs);
      gQdssProtocolPtr->TraceSwEventValist(SwEventIdNum,
                                           NumberArgs,
                                           varg_list);
      VA_END(varg_list);
   }
}



