/** @file
  QDSS Trace Funnel UEFI driver protocol interface.

  Copyright (c) 2015 Qualcomm Technologies, Inc. All rights reserved.


**/
/*=============================================================================
                              EDIT HISTORY

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/QdssDxe/QdssDxe.c#1 $
 $DateTime: 2017/09/18 09:26:13 $
 $Author: pwbldsvc $

 When     Who    What, where, why
 -------- ---    -----------------------------------------------------------
 05/01/15 lht    Created.

=============================================================================*/

/*==============================================================================

                       INCLUDE FILES FOR THIS MODULE

==============================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include "Protocol/EFIQdss.h"
#include "QdssLite.h"
#include "QdssSWEvent.h"
#include "tracer_event_ids.h"

int qdss_enable_debugtrace_flag = 0; // 0 = QDSS trace disabled on boot up.
                                     // 1 = QDSS trace enabled on boot up.

/*==============================================================================

                      PROTOTYPES USED IN THIS MODULE

==============================================================================*/
EFI_STATUS
EFI_QDSS_Init(void);

EFI_STATUS
EFI_QDSS_Enable_Trace(void);

EFI_STATUS
EFI_QDSS_Disable_Trace(void);

EFI_STATUS
EFI_QDSS_Enable_Sw_Event(IN UINT32 swEventIdNum);

EFI_STATUS
EFI_QDSS_Disable_Sw_Event(IN UINT32 swEventIdNum);

EFI_STATUS
EFI_QDSS_Trace_Sw_Event(IN UINT32 SwEventIdNum);

EFI_STATUS
EFI_QDSS_Trace_Sw_Event_Vargs(IN UINT32 SwEventIdNum, IN UINT32 NumberArgs, ...);

EFI_STATUS
EFI_QDSS_Trace_Sw_Event_Valist(IN UINT32 SwEventIdNum, IN UINT32 NumberArgs, VA_LIST varg_list);


/*==============================================================================

                     GLOBAL VARIABLES FOR THIS MODULE

==============================================================================*/

static EFI_QDSS_PROTOCOL QdssProtocol = {
  EFI_QDSS_PROTOCOL_REVISION,
  EFI_QDSS_Init,
  EFI_QDSS_Enable_Trace,
  EFI_QDSS_Disable_Trace,
  EFI_QDSS_Enable_Sw_Event,
  EFI_QDSS_Disable_Sw_Event,
  EFI_QDSS_Trace_Sw_Event,
  EFI_QDSS_Trace_Sw_Event_Vargs,
  EFI_QDSS_Trace_Sw_Event_Valist
};

/*==============================================================================

                             API IMPLEMENTATION

==============================================================================*/

// EFI_QDSS_PROTOCOL implementation
EFI_STATUS
EFI_QDSS_Init(void)
{
  if (0 == QdssInit()) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
EFI_QDSS_Enable_Trace(void)
{
  if (0 == QdssEnableTrace()) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
EFI_QDSS_Disable_Trace(void)
{
  if (0 == QdssDisableTrace()) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
EFI_QDSS_Enable_Sw_Event(IN UINT32 SwEventIdNum)
{
  switch (QdssEnableSwEvent(SwEventIdNum))
  {
    case 0:
        return EFI_SUCCESS;
    case 1:
        return EFI_INVALID_PARAMETER;
    default:
        return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
EFI_QDSS_Disable_Sw_Event(IN UINT32 SwEventIdNum)
{
  switch (QdssDisableSwEvent(SwEventIdNum))
  {
    case 0:
        return EFI_SUCCESS;
    case 1:
        return EFI_INVALID_PARAMETER;
    default:
        return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
EFI_QDSS_Trace_Sw_Event(IN UINT32 SwEventIdNum)
{
  if (0 == QdssTraceSwEvent(SwEventIdNum)) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
EFI_QDSS_Trace_Sw_Event_Vargs(IN UINT32 SwEventIdNum,
                             IN UINT32 NumberArgs,
                             ...)
{
  int retval;
  VA_LIST varg_list;

  VA_START(varg_list, NumberArgs);
  retval = QdssTraceSwEventData(SwEventIdNum, NumberArgs, varg_list);
  VA_END(varg_list);

  if (0 == retval) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}



EFI_STATUS
EFI_QDSS_Trace_Sw_Event_Valist(IN UINT32 SwEventIdNum,
                             IN UINT32 NumberArgs,
                               VA_LIST varg_list)  
{
  int retval;

  retval = QdssTraceSwEventData(SwEventIdNum, NumberArgs, varg_list);

  if (0 == retval) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}



/**
  QDSS DXE driver entry point.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
QdssDxeEntryPoint(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_HANDLE  handle = NULL;
  EFI_STATUS  status;
  UINT32              Attributes;
  UINT8 enable_qdss_data=0;
  UINTN enable_qdss_size=sizeof(UINT8);

  status = gRT->GetVariable (L"EnableQDSSOnBoot",
                             &gQcomTokenSpaceGuid,
                             &Attributes,
                             &enable_qdss_size,
                             &enable_qdss_data);

  if ((EFI_SUCCESS == status) && (0 != enable_qdss_data)) {
     qdss_enable_debugtrace_flag = 1;
     DEBUG(( EFI_D_WARN, "EnableQDSSOnBoot set\n"));
  }
  else {
     DEBUG(( EFI_D_WARN, "EnableQDSSOnBoot not set status=%x, data=%d\n",
             status,enable_qdss_data));
  }


  if (1 == qdss_enable_debugtrace_flag) {
    QdssInit();  // Setup QDSS HW and enable trace capability.
                 // If fails, QDSS SW tracing remains disabled.
  }




  // Publish the protocol
  status = gBS->InstallMultipleProtocolInterfaces(
                  &handle,
                  &gEfiQdssProtocolGuid, (VOID **) &QdssProtocol,
                  NULL, NULL,
                  NULL
                  );

  QDSS_SWEVENT(QDSS_INITIALIZED);

  return status;
} /* QdssDxeEntryPoint */


