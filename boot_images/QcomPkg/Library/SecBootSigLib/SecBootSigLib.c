/** @file
                    SecBootSigLib.c

  SecBootSigLib driver is responsible for providing signal interface to measure
  or authentication.

  This file is platform independent, it needs to work with platform(target)
  dependent driver part.

  Copyright (c) 2013, 2015 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
--------   ---      ----------------------------------------------------------
07/13/15   al       Changing TPL_NOTIFY to TPL_CALLBACK
12/10/13   sm       Updated print statements
05/15/12   shl      Added support for OEM FW update
12/16/11   shl      Added MorPpi feature 
11/28/11   shl      Separate BdsPlatform from library lingking.
11/24/11   shl      Added measure boot support 
08/18/11   shl      Initial version

=============================================================================*/
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

// event to signal
EFI_EVENT  SecBootRegEvent1;
EFI_EVENT  MeasureBootRegEvent1;
EFI_EVENT  MeasureBootStartEvent1;
EFI_EVENT  MeasureReturnFromEfiAppFromBootOptionEvent1;
EFI_EVENT  MorPpiEvent1;
EFI_EVENT  OemFwUpdateEvent1;

//Event Guid
EFI_GUID gQcomSecBootRegEventGuid = { 0x1d2d1996, 0xc860, 0x4640, { 0xb7, 0xa0, 0xe2, 0xe3, 0x30, 0x9e, 0x70, 0x87 } };
EFI_GUID gQcomMeasureBootRegEventGuid = { 0x317b2dc1, 0x898,  0x4897, { 0xb1, 0x4f, 0x4e, 0xae, 0xc2, 0x82, 0xf7, 0x7f } };
EFI_GUID gQcomMeasureBootStartEventGuid = { 0x73164d70, 0xd19b, 0x48cc, { 0xa8, 0x71, 0x5e, 0x48, 0x5d, 0xa8, 0x99, 0x8a } };
EFI_GUID gQcomMeasureReturnFromEfiAppFromBootOptionEventGuid = { 0xce19f251, 0x6f1d, 0x4ffa, { 0x8d, 0xf5, 0xb9, 0xbf, 0xc0, 0x9, 0x9d, 0x92 } };
EFI_GUID gQcomMorPpiEventGuid = { 0xccf447c0, 0x910b, 0x49e0, { 0xab, 0xcc, 0xdf, 0x3a, 0xa, 0x95, 0xff, 0x70 } };
EFI_GUID gQcomOemFwUpdateEventGuid = { 0xccd86915, 0xccf7, 0x4b9a, { 0xaf, 0x7b, 0x41, 0x94, 0xa8, 0xc9, 0xad, 0x44 } };

/**
  This is called the SecBootRegEvent is signaled. This is dummy function
  and real callback is in SecurityDxe.c which can access the security
  framework.

  @param  Event      The event which is signaled.
  @param  *Context   A pointer to VOID type.
  
  @retval Status     RegisterSecBootVerifyPeImage() return value.

**/
VOID EFIAPI SecBootRegEventHandler1(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  return;
}

VOID EFIAPI MeasureBootRegEventHandler1(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  return;
}

VOID EFIAPI MeasureBootStartEventHandler1(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  return;
}

VOID EFIAPI MeasureReturnFromEfiAppFromBootOptionEventHandler1(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  return;
}

VOID EFIAPI MorPpiEventHandler1(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  return;
}

VOID EFIAPI OemFwUpdateEventHandler1(
  IN EFI_EVENT Event, 
  IN VOID *Context
)
{
  return;
}


/**
  Installs Security Architectural Protocol.

  @param  VOID         
  
  @retval Status   CreateEvent() return value.

**/
EFI_STATUS
EFIAPI
SetSecBootRegSignal( VOID )
{
  EFI_STATUS  Status;
 
  // create an event, when this envent is signaled, a verification funcion will be registered.
  Status = gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                                TPL_CALLBACK,
                                SecBootRegEventHandler1,
                                NULL,
                               &gQcomSecBootRegEventGuid,
                               &SecBootRegEvent1);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Create SecBootRegSignal failed, Status = (0x%p)\r\n", Status));
	goto ErrorExit;
  }

  // set signal immediately
  Status = gBS->SignalEvent( SecBootRegEvent1 );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Signal SecBootRegEvent failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // close event
  Status = gBS->CloseEvent( SecBootRegEvent1);

ErrorExit:
  return Status;

}


EFI_STATUS
EFIAPI
SetMeasureBootRegSignal( VOID )
{
  EFI_STATUS  Status;
 
  // create an event, when this envent is signaled, a verification funcion will be registered.
  Status = gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                                TPL_CALLBACK,
                                MeasureBootRegEventHandler1,
                                NULL,
                               &gQcomMeasureBootRegEventGuid,
                               &MeasureBootRegEvent1);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Create MeasureBootRegSignal failed, Status = (0x%p)\r\n", Status));
	goto ErrorExit;
  }

  // set signal immediately
  Status = gBS->SignalEvent( MeasureBootRegEvent1 );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Signal MeasureBootRegEvent failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // close event
  Status = gBS->CloseEvent( MeasureBootRegEvent1);

ErrorExit:
  return Status;

}


EFI_STATUS
EFIAPI
SetMeasureBootStartSignal( VOID )
{
  EFI_STATUS  Status;
 
  // create an event, when this envent is signaled, a verification funcion will be registered.
  Status = gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                                TPL_CALLBACK,
                                MeasureBootStartEventHandler1,
                                NULL,
                               &gQcomMeasureBootStartEventGuid,
                               &MeasureBootStartEvent1);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Create MeasureBootStartSignal failed, Status = (0x%p)\r\n", Status));
	goto ErrorExit;
  }

  // set signal immediately
  Status = gBS->SignalEvent( MeasureBootStartEvent1 );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Signal MeasureBootStartEvent failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // close event
  Status = gBS->CloseEvent( MeasureBootStartEvent1);

ErrorExit:
  return Status;

}

EFI_STATUS
EFIAPI
SetMeasureReturnFromEfiAppFromBootOptionSignal( VOID )
{
  EFI_STATUS  Status;
 
  // create an event, when this envent is signaled, a verification funcion will be registered.
  Status = gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                                TPL_CALLBACK,
                                MeasureReturnFromEfiAppFromBootOptionEventHandler1,
                                NULL,
                               &gQcomMeasureReturnFromEfiAppFromBootOptionEventGuid,
                               &MeasureReturnFromEfiAppFromBootOptionEvent1);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Create MeasureReturnFromEfiAppFromBootOptionSignal failed, Status = (0x%p)\r\n", Status));
	goto ErrorExit;
  }

  // set signal immediately
  Status = gBS->SignalEvent( MeasureReturnFromEfiAppFromBootOptionEvent1 );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Signal MeasureReturnFromEfiAppFromBootOptionEvent failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // close event
  Status = gBS->CloseEvent( MeasureReturnFromEfiAppFromBootOptionEvent1);

ErrorExit:
  return Status;

}


EFI_STATUS
EFIAPI
SetMorPpiSignal( VOID )
{
  EFI_STATUS  Status;
 
  // create an event, when this envent is signaled, a verification funcion will be registered.
  Status = gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                                TPL_CALLBACK,
                                MorPpiEventHandler1,
                                NULL,
                               &gQcomMorPpiEventGuid,
                               &MorPpiEvent1);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Create PpiSignal failed, Status = (0x%p)\r\n", Status));
	goto ErrorExit;
  }

  // set signal immediately
  Status = gBS->SignalEvent( MorPpiEvent1 );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Signal MorPpiEvent failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // close event
  Status = gBS->CloseEvent( MorPpiEvent1);

ErrorExit:
  return Status;

}

EFI_STATUS
EFIAPI
SetOemFwUpdateSignal( VOID )
{
  EFI_STATUS  Status;
 
  // create an event, when this envent is signaled, a verification funcion will be registered.
  Status = gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                                TPL_CALLBACK,
                                OemFwUpdateEventHandler1,
                                NULL,
                               &gQcomOemFwUpdateEventGuid,
                               &OemFwUpdateEvent1);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Create OemFwUpdateSignal failed, Status = (0x%p)\r\n", Status));
	goto ErrorExit;
  }

  // set signal immediately
  Status = gBS->SignalEvent(OemFwUpdateEvent1 );
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "Signal OemFwUpdateEvent failed, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // close event
  Status = gBS->CloseEvent( OemFwUpdateEvent1);

ErrorExit:
  return Status;

}

