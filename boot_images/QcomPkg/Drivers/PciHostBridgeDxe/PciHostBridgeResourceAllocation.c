/** @file
*  Implementation of the Pci Host Bridge Resource Allocation for Qualcomm PCIe Root Complex
*
*  Copyright (c) 2016, Qualcomm Technologies Inc. All rights reserved.
*  Copyright (c) 2011-2015, ARM Ltd. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
*  EDIT HISTORY
*
*  when      who      what, where, why
*  --------  -------  --------------------------------------------------------
*  07/28/16  tselvam  Included I/O port access support
*  07/08/16  tselvam  Derived from ARMJuno PciHostBridgeDxe for Qualcomm PCIe
**/

#include "PciHostBridge.h"

/**
  @fn PciHbRaNotifyPhase
  @brief These are the notifications from the PCI bus driver that it is about to enter a certain
         phase of the PCI enumeration process.

  This member function can be used to notify the host bridge driver to perform specific actions,
  including any chipset-specific initialization, so that the chipset is ready to enter the next phase.
  Eight notification points are defined at this time. See belows:
  EfiPciHostBridgeBeginEnumeration       Resets the host bridge PCI apertures and internal data
                                        structures. The PCI enumerator should issue this notification
                                        before starting a fresh enumeration process. Enumeration cannot
                                        be restarted after sending any other notification such as
                                        EfiPciHostBridgeBeginBusAllocation.
  EfiPciHostBridgeBeginBusAllocation     The bus allocation phase is about to begin. No specific action is
                                        required here. This notification can be used to perform any
                                        chipset-specific programming.
  EfiPciHostBridgeEndBusAllocation       The bus allocation and bus programming phase is complete. No
                                        specific action is required here. This notification can be used to
                                        perform any chipset-specific programming.
  EfiPciHostBridgeBeginResourceAllocation
                                        The resource allocation phase is about to begin. No specific
                                        action is required here. This notification can be used to perform
                                        any chipset-specific programming.
  EfiPciHostBridgeAllocateResources      Allocates resources per previously submitted requests for all the PCI
                                        root bridges. These resource settings are returned on the next call to
                                        GetProposedResources(). Before calling NotifyPhase() with a Phase of
                                        EfiPciHostBridgeAllocateResource, the PCI bus enumerator is responsible
                                        for gathering I/O and memory requests for
                                        all the PCI root bridges and submitting these requests using
                                        SubmitResources(). This function pads the resource amount
                                        to suit the root bridge hardware, takes care of dependencies between
                                        the PCI root bridges, and calls the Global Coherency Domain (GCD)
                                        with the allocation request. In the case of padding, the allocated range
                                        could be bigger than what was requested.
  EfiPciHostBridgeSetResources           Programs the host bridge hardware to decode previously allocated
                                        resources (proposed resources) for all the PCI root bridges. After the
                                        hardware is programmed, reassigning resources will not be supported.
                                        The bus settings are not affected.
  EfiPciHostBridgeFreeResources          Deallocates resources that were previously allocated for all the PCI
                                        root bridges and resets the I/O and memory apertures to their initial
                                        state. The bus settings are not affected. If the request to allocate
                                        resources fails, the PCI enumerator can use this notification to
                                        deallocate previous resources, adjust the requests, and retry
                                        allocation.
  EfiPciHostBridgeEndResourceAllocation  The resource allocation phase is completed. No specific action is
                                        required here. This notification can be used to perform any chipsetspecific
                                        programming.

  @param[in] This                The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
  @param[in] Phase               The phase during enumeration

  @retval EFI_INVALID_PARAMETER  This is NULL.
  @retval EFI_NOT_READY          This phase cannot be entered at this time. For example, this error
                                is valid for a Phase of EfiPciHostBridgeAllocateResources if
                                SubmitResources() has not been called for one or more
                                PCI root bridges before this call
  @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error. This error is valid
                                for a Phase of EfiPciHostBridgeSetResources.
  @retval EFI_INVALID_PARAMETER  Invalid phase parameter
  @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.
                                This error is valid for a Phase of EfiPciHostBridgeAllocateResources if the
                                previously submitted resource requests cannot be fulfilled or
                                were only partially fulfilled.
  @retval EFI_SUCCESS            The notification was accepted without any errors.

**/
EFI_STATUS
PciHbRaNotifyPhase (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE     Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE*   HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE*   RootBridgeInstance;
  EFI_PHYSICAL_ADDRESS        BaseAddress;
  UINT64                      AddrLen;
  UINTN                       BitsOfAlignment;
  LIST_ENTRY*                 RbList;
  UINT32                      Index;
  UINT64                      Alignment;
  ROOT_PORT_RES_TYPE          RootPortResourceType;
  EFI_STATUS                  Status;

  PCI_TRACE ("PciHbRaNotifyPhase()");

  if (This == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);

  switch (Phase)
  {
  case EfiPciHostBridgeBeginEnumeration:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeBeginEnumeration)");
    if (HostBridgeInstance->CanRestarted)
    {
      RbList = HostBridgeInstance->RbHead.ForwardLink;
      while (RbList != &HostBridgeInstance->RbHead)
      {
        RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY(RbList);
        for (Index = ResTypeIo; Index < ResTypeMax; Index++)
        {
          RootBridgeInstance->ResAlloc[Index].Base = 0;
          RootBridgeInstance->ResAlloc[Index].Length = 0;
          RootBridgeInstance->ResAlloc[Index].Alignment = 0;
          RootBridgeInstance->ResAlloc[Index].Status = ResNone;
        }
        RbList = RbList->ForwardLink;
      }
      HostBridgeInstance->ResourceSubmitted = FALSE;
      HostBridgeInstance->CanRestarted = TRUE;
    }
    else
    {
    	return EFI_NOT_READY;
    }
    break;

  case EfiPciHostBridgeBeginBusAllocation:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeBeginBusAllocation)");
    /* The bus allocation phase is about to begin */
    HostBridgeInstance->CanRestarted = FALSE;
    break;

  case EfiPciHostBridgeEndBusAllocation:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeEndBusAllocation)");
    /*
     * The bus allocation and bus programming phase is complete.
     * All the PCI-to-PCI bridges have been given and written back
     * a bus number range into their configuration
     */
    break;

  case EfiPciHostBridgeBeginResourceAllocation:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeBeginResourceAllocation)");
    // The resource allocation phase is about to begin.
    break;

  case EfiPciHostBridgeAllocateResources:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeAllocateResources)");
    /*
     * Allocates resources per previously submitted requests for all the
     *  PCI root bridges. The resources have been submitted to
     *  PciHbRaSubmitResources() before.
     */
    if (HostBridgeInstance->ResourceSubmitted)
    {
      RbList = HostBridgeInstance->RbHead.ForwardLink;
      while (RbList != &HostBridgeInstance->RbHead)
      {
        RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY(RbList);
        for(Index = ResTypeIo; Index <= ResTypePMem64; Index++)
        {
          if (RootBridgeInstance->ResAlloc[Index].Length != 0)
          {
            /* Get the number of '1' in Alignment */
            Alignment = RootBridgeInstance->ResAlloc[Index].Alignment;
            BitsOfAlignment = LowBitSet64 (Alignment + 1);
            AddrLen = RootBridgeInstance->ResAlloc[Index].Length;

            switch(Index)
            {
            case ResTypeIo:
              BaseAddress = ALIGN_VALUE(RootBridgeInstance->ResAperture[PciRbResTypeIo].End , Alignment + 1);
              RootPortResourceType = PciRbResTypeIo;
              break;
            case ResTypeMem32:
            case ResTypeMem64:
            case ResTypePMem32:
            case ResTypePMem64:
              BaseAddress = ALIGN_VALUE(RootBridgeInstance->ResAperture[PciRbResTypeMem].End, Alignment + 1);
              RootPortResourceType = PciRbResTypeMem;
              break;
            default:
              /* Just a placeholder, will never come to this case */
              break;
            }

            Status = gDS->AllocateMemorySpace (EfiGcdAllocateMaxAddressSearchTopDown,
                                               EfiGcdMemoryTypeMemoryMappedIo,
                                               MIN(31, BitsOfAlignment),
                                               AddrLen,
                                               &BaseAddress,
                                               HostBridgeInstance->ImageHandle,
                                               NULL);
            if (Status == EFI_SUCCESS)
            {
              RootBridgeInstance->ResAlloc[Index].Base =  (UINTN) SYS_ADDR_TO_PCI_ADDR (BaseAddress, RootBridgeInstance->ResAperture[RootPortResourceType]) ;
              RootBridgeInstance->ResAlloc[Index].Status = ResAllocated;
            }
            else
            {
              DEBUG ((EFI_D_ERROR, "Error: %a, Status = %d\n", Index?"AllocateMemorySpace":"AllocateIoSpace", Status));
              ASSERT (FALSE);
            }
          }
        }
        RbList = RbList->ForwardLink;
      }
      return Status;
    }
    else
      return EFI_NOT_READY;
    break;

  case EfiPciHostBridgeSetResources:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeSetResources)");
    /*
     * Programs the host bridge hardware to decode previously allocated
     * resources (proposed resources) for all the PCI root bridges.
     * The PCI bus driver will now program the resources
     */
    break;

  case EfiPciHostBridgeFreeResources:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeFreeResources)");
    /*
     * Deallocates resources that were previously allocated for all
     * the PCI root bridges and resets the I/O and memory apertures
     * to their initial state.
     */
    RbList = HostBridgeInstance->RbHead.ForwardLink;
    while (RbList != &HostBridgeInstance->RbHead)
    {
      RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY (RbList);
      for (Index = ResTypeIo; Index <= ResTypePMem64; Index++) {
        if (RootBridgeInstance->ResAlloc[Index].Status == ResAllocated)
        {
          AddrLen = RootBridgeInstance->ResAlloc[Index].Length;
          BaseAddress = RootBridgeInstance->ResAlloc[Index].Base;
          Status = gDS->FreeMemorySpace (BaseAddress, AddrLen);
          if (EFI_ERROR (Status))
          {
            PCI_TRACE ("EfiPciHostBridgeFreeResources(): ERROR: Fail to FreeMemorySpace.");
          }

          RootBridgeInstance->ResAlloc[Index].Base      = 0;
          RootBridgeInstance->ResAlloc[Index].Length    = 0;
          RootBridgeInstance->ResAlloc[Index].Status    = ResNone;
        }
      }

      RbList = RbList->ForwardLink;
    }

    HostBridgeInstance->ResourceSubmitted = FALSE;
    HostBridgeInstance->CanRestarted     = TRUE;
    return Status;
    break;

  case EfiPciHostBridgeEndResourceAllocation:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeEndResourceAllocation)");
    break;

  case EfiPciHostBridgeEndEnumeration:
    PCI_TRACE ("PciHbRaNotifyPhase(EfiPciHostBridgeEndEnumeration)");
    break;

  default:
    DEBUG ((EFI_D_INFO, "PciHbRaNotifyPhase(Phase:%d)\n", Phase));
    ASSERT (FALSE);
  }

  return EFI_SUCCESS;
}

/**
  @fn PciHbRaGetNextRootBridge
  @brief Return the device handle of the next PCI root bridge that is associated with this Host Bridge.

  This function is called multiple times to retrieve the device handles of all the PCI root bridges that
  are associated with this PCI host bridge. Each PCI host bridge is associated with one or more PCI
  root bridges. On each call, the handle that was returned by the previous call is passed into the
  interface, and on output the interface returns the device handle of the next PCI root bridge. The
  caller can use the handle to obtain the instance of the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL
  for that root bridge. When there are no more PCI root bridges to report, the interface returns
  EFI_NOT_FOUND. A PCI enumerator must enumerate the PCI root bridges in the order that they
  are returned by this function.
  For D945 implementation, there is only one root bridge in PCI host bridge.

  @param[in]       This              The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
  @param[in, out]  RootBridgeHandle  Returns the device handle of the next PCI root bridge.

  @retval EFI_SUCCESS            If parameter RootBridgeHandle = NULL, then return the first Rootbridge handle of the
                                specific Host bridge and return EFI_SUCCESS.
  @retval EFI_INVALID_PARAMETER  This is NULL.
  @retval EFI_NOT_FOUND          Can not find the any more root bridge in specific host bridge.
  @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not an EFI_HANDLE that was
                                returned on a previous call to GetNextRootBridge().
**/
EFI_STATUS
PciHbRaGetNextRootBridge (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL* This,
  IN OUT EFI_HANDLE*                                   RootBridgeHandle)
{
  BOOLEAN                   NoRootBridge = TRUE;
  LIST_ENTRY*               RbList;
  PCI_HOST_BRIDGE_INSTANCE* HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE* RootBridgeInstance;

  if (This == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  RbList = HostBridgeInstance->RbHead.ForwardLink;

  while (RbList != &HostBridgeInstance->RbHead)
  {
    NoRootBridge = FALSE;
    RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY (RbList);
    if (*RootBridgeHandle == NULL)
    {
      /*
       * Return the first Root Bridge Handle of the Host Bridge
       */
      *RootBridgeHandle = RootBridgeInstance->Handle;
      return EFI_SUCCESS;
    }
    else
    {
      if (*RootBridgeHandle == RootBridgeInstance->Handle)
      {
        /*
         * Get next if we do have one
         */
        RbList = RbList->ForwardLink;
        if (RbList != &HostBridgeInstance->RbHead)
        {
          RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY (RbList);
          *RootBridgeHandle = RootBridgeInstance->Handle;
          return EFI_SUCCESS;
        }
        else
        {
          return EFI_NOT_FOUND;
        }
      }
    }
    RbList = RbList->ForwardLink;
  }

  if (NoRootBridge)
  {
    return EFI_NOT_FOUND;
  }
  else
  {
    return EFI_INVALID_PARAMETER;
  }
}

/**
  @fn PciHbRaGetAllocAttributes
  @brief Returns the allocation attributes of a PCI root bridge.

  The function returns the allocation attributes of a specific PCI root bridge. The attributes can vary
  from one PCI root bridge to another. These attributes are different from the decode-related
  attributes that are returned by the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.GetAttributes() member function. The
  RootBridgeHandle parameter is used to specify the instance of the PCI root bridge. The device
  handles of all the root bridges that are associated with this host bridge must be obtained by calling
  GetNextRootBridge(). The attributes are static in the sense that they do not change during or
  after the enumeration process. The hardware may provide mechanisms to change the attributes on
  the fly, but such changes must be completed before EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL is
  installed. The permitted values of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ATTRIBUTES are defined in
  "Related Definitions" below. The caller uses these attributes to combine multiple resource requests.
  For example, if the flag EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM is set, the PCI bus enumerator needs to
  include requests for the prefetchable memory in the nonprefetchable memory pool and not request any
  prefetchable memory.
    Attribute                                 Description
  ------------------------------------         ----------------------------------------------------------------------
  EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM         If this bit is set, then the PCI root bridge does not support separate
                                              windows for nonprefetchable and prefetchable memory. A PCI bus
                                              driver needs to include requests for prefetchable memory in the
                                              nonprefetchable memory pool.

  EFI_PCI_HOST_BRIDGE_MEM64_DECODE             If this bit is set, then the PCI root bridge supports 64-bit memory
                                              windows. If this bit is not set, the PCI bus driver needs to include
                                              requests for a 64-bit memory address in the corresponding 32-bit
                                              memory pool.

  @param[in]   This               The instance pointer of EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
  @param[in]   RootBridgeHandle   The device handle of the PCI root bridge in which the caller is interested. Type
                                 EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.0 Specification.
  @param[out]  Attributes         The pointer to attribte of root bridge, it is output parameter

  @retval EFI_INVALID_PARAMETER   This is NULL.
  @retval EFI_INVALID_PARAMETER   Attribute pointer is NULL
  @retval EFI_INVALID_PARAMETER   RootBridgehandle is invalid.
  @retval EFI_SUCCESS             Success to get attribute of interested root bridge.
**/
EFI_STATUS
PciHbRaGetAllocAttributes (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                        RootBridgeHandle,
  OUT UINT64                                           *Attributes
  )
{
  PCI_HOST_BRIDGE_INSTANCE* HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE* RootBridgeInstance;
  LIST_ENTRY*               RbList;

  PCI_TRACE ("PciHbRaGetAllocAttributes()");

  if (This == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if(Attributes == NULL)
  {
    PCI_TRACE ("PciHbRaGetAllocAttributes(): NULL Attribute pointer");
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  RbList = HostBridgeInstance->RbHead.ForwardLink;

  while(RbList != &HostBridgeInstance->RbHead)
  {
    RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY(RbList);
    if (RootBridgeInstance->Handle == RootBridgeHandle)
    {
      *Attributes = RootBridgeInstance->MemAllocAttributes;
      return EFI_SUCCESS;
    }

    RbList = RbList->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  @fn PciHbRaStartBusEnumeration
  @brief Sets up the specified PCI root bridge for the bus enumeration process.

  This member function sets up the root bridge for bus enumeration and returns the PCI bus range
  over which the search should be performed in ACPI 2.0 resource descriptor format.

  @param[in]   This              The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in]   RootBridgeHandle  The PCI Root Bridge to be set up.
  @param[out]  Configuration     Pointer to the pointer to the PCI bus resource descriptor.

  @retval EFI_INVALID_PARAMETER Invalid Root bridge's handle
  @retval EFI_INVALID_PARAMETER  This is NULL.
  @retval EFI_INVALID_PARAMETER Configuration is NULL
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate ACPI resource descriptor tag.
  @retval EFI_SUCCESS           Sucess to allocate ACPI resource descriptor.
**/
EFI_STATUS
PciHbRaStartBusEnumeration (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                        RootBridgeHandle,
  OUT VOID                                            **Configuration
  )
{
  VOID    *Buffer;
  UINT8   *Ptr;
  PCI_HOST_BRIDGE_INSTANCE* HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE* RootBridgeInstance;
  LIST_ENTRY*               RbList;

  PCI_TRACE ("PciHbRaStartBusEnumeration()");

  if (This == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (Configuration == NULL)
  {
    PCI_TRACE ("PciHbRaStartBusEnumeration(): NULL Configuration pointer");
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  RbList = HostBridgeInstance->RbHead.ForwardLink;

  while (RbList != & HostBridgeInstance->RbHead)
  {
    RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY(RbList);
    if (RootBridgeHandle == RootBridgeInstance->Handle)
    {
      Buffer = AllocateZeroPool (sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Ptr = (UINT8 *)Buffer;

      /*
       * Fill an ACPI descriptor table with the Bus Number Range.
       * This information will be used by the PCI Bus driver
       * to set bus numbers to PCI-to-PCI bridge.
       */
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->Desc = ACPI_ADDRESS_SPACE_DESCRIPTOR;
      /* 'Len' field implies the length of the struct EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR after this field 'Len' */
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->Len  = sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) - 3;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->ResType = ACPI_ADDRESS_SPACE_TYPE_BUS;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->GenFlag = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->SpecificFlag = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrSpaceGranularity = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrRangeMin = RootBridgeInstance->BusStart;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrRangeMax = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrTranslationOffset = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrLen = RootBridgeInstance->BusLength;

      Ptr = Ptr + sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Ptr)->Desc = ACPI_END_TAG_DESCRIPTOR;
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Ptr)->Checksum = 0x0;

      *Configuration = Buffer;
      return EFI_SUCCESS;
    }

    RbList = RbList->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  @fn PciHbRaSetBusNumbers
  @brief Programs the PCI root bridge hardware so that it decodes the specified PCI bus range.

  This member function programs the specified PCI root bridge to decode the bus range that is
  specified by the input parameter Configuration.
  The bus range information is specified in terms of the ACPI 2.0 resource descriptor format.

  @param[in] This              The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  The PCI Root Bridge whose bus range is to be programmed
  @param[in] Configuration     The pointer to the PCI bus resource descriptor

  @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
  @retval EFI_INVALID_PARAMETER  This is NULL.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_INVALID_PARAMETER  Configuration does not point to a valid ACPI 2.0 resource descriptor.
  @retval EFI_INVALID_PARAMETER  Configuration does not include a valid ACPI 2.0 bus resource descriptor.
  @retval EFI_INVALID_PARAMETER  Configuration includes valid ACPI 2.0 resource descriptors other than
                                bus descriptors.
  @retval EFI_SUCCESS            The bus range for the PCI root bridge was programmed.
**/
EFI_STATUS
PciHbRaSetBusNumbers (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN EFI_HANDLE                                         RootBridgeHandle,
  IN VOID                                              *Configuration
  )
{
  PCI_HOST_BRIDGE_INSTANCE*   HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE*   RootBridgeInstance;
  LIST_ENTRY*                 RbList;
  UINT8*                      Ptr;
  UINTN                       BusStart;
  UINTN                       BusEnd;
  UINTN                       BusLen;

  PCI_TRACE ("PciHbRaSetBusNumbers()");

  if (This == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (Configuration == NULL)
  {
    PCI_TRACE ("PciHbRaSetBusNumbers(): NULL Configuration pointer");
    return EFI_INVALID_PARAMETER;
  }

  Ptr = Configuration;
  if (*Ptr != ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  /*
   * Check if the passed ACPI descriptor table define a Bus Number Range
   */
  if (((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->ResType != ACPI_ADDRESS_SPACE_TYPE_BUS) {
    return EFI_INVALID_PARAMETER;
  }

  /*
   * Check if the Configuration only passed one ACPI Descriptor (+ End Descriptor)
   */
  if (*((UINT8*)(Ptr + sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR))) != ACPI_END_TAG_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  RbList = HostBridgeInstance->RbHead.ForwardLink;

  while (RbList != &HostBridgeInstance->RbHead)
  {
    RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY(RbList);
    if (RootBridgeInstance->Handle == RootBridgeHandle)
    {
      BusStart = (UINTN)((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrRangeMin;
      BusLen = (UINTN)((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr)->AddrLen;
      BusEnd = BusStart + BusLen - 1;

      ASSERT (BusStart <= BusEnd);
      ASSERT ((BusStart >= RootBridgeInstance->BusStart) && (BusLen <= RootBridgeInstance->BusLength));

      RootBridgeInstance->BusStart  = BusStart;
      RootBridgeInstance->BusLength = BusLen;

      return EFI_SUCCESS;
    }
    RbList = RbList->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  @fn PciHbRaSubmitResources
  @brief Submits the I/O and memory resource requirements for the specified PCI root bridge.

  This function is used to submit all the I/O and memory resources that are required by the specified
  PCI root bridge. The input parameter Configuration is used to specify the following:
  - The various types of resources that are required
  - The associated lengths in terms of ACPI 2.0 resource descriptor format

  @param[in] This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
  @param[in] RootBridgeHandle  The PCI root bridge whose I/O and memory resource requirements are being submitted.
  @param[in] Configuration     The pointer to the PCI I/O and PCI memory resource descriptor.

  @retval EFI_SUCCESS            The I/O and memory resource requests for a PCI root bridge were accepted.
  @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
  @retval EFI_INVALID_PARAMETER  This is NULL.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_INVALID_PARAMETER  Configuration does not point to a valid ACPI 2.0 resource descriptor.
  @retval EFI_INVALID_PARAMETER  Configuration includes requests for one or more resource types that are
                                not supported by this PCI root bridge. This error will happen if the caller
                                did not combine resources according to Attributes that were returned by
                                GetAllocAttributes().
  @retval EFI_INVALID_PARAMETER  Address Range Maximum" is invalid.
  @retval EFI_INVALID_PARAMETER  "Address Range Length" is invalid for this PCI root bridge.
  @retval EFI_INVALID_PARAMETER  "Address Space Granularity" is invalid for this PCI root bridge.
**/
EFI_STATUS
PciHbRaSubmitResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN EFI_HANDLE                                         RootBridgeHandle,
  IN VOID                                              *Configuration
  )
{
  PCI_HOST_BRIDGE_INSTANCE*           HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE*           RootBridgeInstance;
  LIST_ENTRY*                         RbList;
  UINT8*                              Ptr;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR*  Desc;
  PCI_RESOURCE_TYPE                   ResType;

  PCI_TRACE ("PciHbRaSubmitResources()");

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);

  if (This == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (Configuration == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  /*
   * Check if the ACPI Descriptor tables is conformed
   */
  Ptr = (UINT8 *)Configuration;
  while (*Ptr == ACPI_ADDRESS_SPACE_DESCRIPTOR)
  {
    Ptr += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) ;
  }

  if (*Ptr != ACPI_END_TAG_DESCRIPTOR)
  {
    return EFI_INVALID_PARAMETER;
  }

  RbList = HostBridgeInstance->RbHead.ForwardLink;
  Ptr = (UINT8 *)Configuration;

  while(RbList != &HostBridgeInstance->RbHead)
  {
    RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY(RbList);
    if (RootBridgeHandle == RootBridgeInstance->Handle)
    {
      /* While the entry is an ACPI Descriptor Table */
      while ( *Ptr == ACPI_ADDRESS_SPACE_DESCRIPTOR)
      {
        Desc = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr;

        if (Desc->AddrLen == 0)
        {
          HostBridgeInstance->ResourceSubmitted = TRUE;
          continue;
        }

        /* Check if the description is valid */
        if (Desc->AddrLen > 0xffffffff)
        {
          return EFI_INVALID_PARAMETER;
        }

        if ((Desc->AddrRangeMax >= 0xffffffff) ||
            (Desc->AddrRangeMax != (GetPowerOfTwo64 (Desc->AddrRangeMax + 1) - 1)))
        {
          return EFI_INVALID_PARAMETER;
        }

        switch (Desc->ResType)
        {
        case ACPI_ADDRESS_SPACE_TYPE_MEM:
          /* Check invalid Address Space Granularity */
          if ((Desc->AddrSpaceGranularity != 32) && (Desc->AddrSpaceGranularity != 64))
          {
            return EFI_INVALID_PARAMETER;
          }

          /* check the memory resource request is supported by PCI root bridge */
          if ( (RootBridgeInstance->MemAllocAttributes == EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM) &&
               (Desc->SpecificFlag == ACPI_SPECFLAG_PREFETCHABLE) )
          {
            return EFI_INVALID_PARAMETER;
          }

          if (Desc->AddrSpaceGranularity == 32)
          {
            if (Desc->SpecificFlag == ACPI_SPECFLAG_PREFETCHABLE)
            {
              ResType = ResTypePMem32;
            }
            else
            {
              ResType = ResTypeMem32;
            }
          }
          else
          {
            if (Desc->SpecificFlag == ACPI_SPECFLAG_PREFETCHABLE)
            {
              ResType = ResTypePMem64;
            }
            else
            {
              ResType = ResTypeMem64;
            }
          }
          RootBridgeInstance->ResAlloc[ResType].Length = Desc->AddrLen;
          RootBridgeInstance->ResAlloc[ResType].Alignment = Desc->AddrRangeMax;
          RootBridgeInstance->ResAlloc[ResType].Base = Desc->AddrRangeMin;
          RootBridgeInstance->ResAlloc[ResType].Status = ResSubmitted;
          HostBridgeInstance->ResourceSubmitted = TRUE;
          break;
        case ACPI_ADDRESS_SPACE_TYPE_IO:
          RootBridgeInstance->ResAlloc[ResTypeIo].Length = Desc->AddrLen;
          RootBridgeInstance->ResAlloc[ResTypeIo].Alignment = Desc->AddrRangeMax;
          RootBridgeInstance->ResAlloc[ResTypeIo].Base = Desc->AddrRangeMin;
          RootBridgeInstance->ResAlloc[ResTypeIo].Status = ResSubmitted;
          HostBridgeInstance->ResourceSubmitted = TRUE;
          break;
        default:
          /*
           * Could be the case Desc->ResType == ACPI_ADDRESS_SPACE_TYPE_BUS
           */
          ASSERT (FALSE);
          break;
        }
        Ptr += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
      }
      return EFI_SUCCESS;
    }
    RbList = RbList->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  @fn PciHbRaGetProposedResources
  @brief Returns the proposed resource settings for the specified PCI root bridge.

  This member function returns the proposed resource settings for the specified PCI root bridge. The
  proposed resource settings are prepared when NotifyPhase() is called with a Phase of
  EfiPciHostBridgeAllocateResources. The output parameter Configuration
  specifies the following:
  - The various types of resources, excluding bus resources, that are allocated
  - The associated lengths in terms of ACPI 2.0 resource descriptor format

  @param[in]  This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
  @param[in]  RootBridgeHandle  The PCI root bridge handle. Type EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.0 Specification.
  @param[out] Configuration     The pointer to the pointer to the PCI I/O and memory resource descriptor.

  @retval EFI_SUCCESS            The requested parameters were returned.
  @retval EFI_INVALID_PARAMETER  This is NULL.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_INVALID_PARAMETER  RootBridgeHandle is not a valid root bridge handle.
  @retval EFI_DEVICE_ERROR       Programming failed due to a hardware error.
  @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.
**/
EFI_STATUS
PciHbRaGetProposedResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                        RootBridgeHandle,
  OUT VOID                                            **Configuration
  )
{
  PCI_HOST_BRIDGE_INSTANCE            *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE            *RootBridgeInstance;
  UINT32                              i;
  UINT32                              ResAllocCount;
  VOID                                *Buffer;
  UINT8                               *Ptr;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR   *Desc;
  LIST_ENTRY                          *RbList;

  PCI_TRACE ("PciHbRaGetProposedResources()");

  if (This == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (Configuration == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  RbList = HostBridgeInstance->RbHead.ForwardLink;
  while(RbList != &HostBridgeInstance->RbHead)
  {
    RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY(RbList);
    if (RootBridgeInstance->Handle == RootBridgeHandle)
    {
      /*
       * Count the number of Resource Allocated
       * for this Root Bridge
       */
      ResAllocCount = 0;
      for (i = 0; i < ResTypeMax; i++) {
        if (RootBridgeInstance->ResAlloc[i].Length != 0)  ResAllocCount++;
      }

      if (ResAllocCount ==  0) {
        return EFI_INVALID_PARAMETER;
      }

      Buffer = AllocateZeroPool (ResAllocCount * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Ptr = Buffer;
      for (i = ResTypeIo; i < ResTypeMax; i++) {
        if (RootBridgeInstance->ResAlloc[i].Length != 0) {    // Base != 0 if the resource has been allocated
          Desc = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)Ptr;

          Desc->Desc = ACPI_ADDRESS_SPACE_DESCRIPTOR;
          Desc->Len  = 0x2B;
          Desc->GenFlag = 0;
          Desc->AddrRangeMax = 0;

          switch (i) {
          case ResTypeIo:
            Desc->ResType = ACPI_ADDRESS_SPACE_TYPE_IO;
            Desc->SpecificFlag = 0;
            Desc->AddrSpaceGranularity = 0;
            break;
          case ResTypeMem32:
            Desc->ResType = ACPI_ADDRESS_SPACE_TYPE_MEM;
            Desc->SpecificFlag = 0;
            Desc->AddrSpaceGranularity = 32;
            break;
          case ResTypePMem32:
            Desc->ResType = ACPI_ADDRESS_SPACE_TYPE_MEM;
            Desc->SpecificFlag = ACPI_SPECFLAG_PREFETCHABLE;
            Desc->AddrSpaceGranularity = 32;
            break;
          case ResTypeMem64:
            Desc->ResType = ACPI_ADDRESS_SPACE_TYPE_MEM;
            Desc->SpecificFlag = 0;
            Desc->AddrSpaceGranularity = 64;
            break;
          case ResTypePMem64:
            Desc->ResType = ACPI_ADDRESS_SPACE_TYPE_MEM;
            Desc->SpecificFlag = ACPI_SPECFLAG_PREFETCHABLE;
            Desc->AddrSpaceGranularity = 64;
            break;
          }
          Desc->AddrRangeMin = RootBridgeInstance->ResAlloc[i].Base;
          Desc->AddrTranslationOffset = (RootBridgeInstance->ResAlloc[i].Base != 0) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
          Desc->AddrLen = RootBridgeInstance->ResAlloc[i].Length;
          Ptr += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
        }
      }
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Ptr)->Desc = ACPI_END_TAG_DESCRIPTOR;
      ((EFI_ACPI_END_TAG_DESCRIPTOR *)Ptr)->Checksum = 0x0;
      *Configuration = Buffer;
      return EFI_SUCCESS;
    }
    RbList = RbList->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**
  @fn PciHbRaPreprocessController
  @brief Provides the hooks from the PCI bus driver to every PCI controller (device/function) at various
         stages of the PCI enumeration process that allow the host bridge driver to preinitialize individual
         PCI controllers before enumeration.

  This function is called during the PCI enumeration process. No specific action is expected from this
  member function. It allows the host bridge driver to preinitialize individual PCI controllers before
  enumeration.

  @param This              Pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance.
  @param RootBridgeHandle  The associated PCI root bridge handle. Type EFI_HANDLE is defined in
                          InstallProtocolInterface() in the UEFI 2.0 Specification.
  @param PciAddress        The address of the PCI device on the PCI bus. This address can be passed to the
                          EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL member functions to access the PCI
                          configuration space of the device. See Table 12-1 in the UEFI 2.0 Specification for
                          the definition of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS.
  @param Phase             The phase of the PCI device enumeration.

  @retval EFI_SUCCESS              The requested parameters were returned.
  @retval EFI_INVALID_PARAMETER    This is NULL.
  @retval EFI_INVALID_PARAMETER    RootBridgeHandle is not a valid root bridge handle.
  @retval EFI_INVALID_PARAMETER    Phase is not a valid phase that is defined in
                                  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE.
**/
EFI_STATUS
PciHbRaPreprocessController (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                        RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS       PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE      Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE*   HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE*   RootBridgeInstance;
  LIST_ENTRY*                 RbList;

  PCI_TRACE ("PciHbRaPreprocessController()");

  if (This == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  RbList = HostBridgeInstance->RbHead.ForwardLink;

  /*
   * Enumerate the root bridges in this host bridge
   */
  while (RbList != &HostBridgeInstance->RbHead) {
    RootBridgeInstance = ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY (RbList);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      break;
    }
    RbList = RbList->ForwardLink;
  }

  if (RbList == &HostBridgeInstance->RbHead) {
    return EFI_INVALID_PARAMETER;
  }

  if ((UINT32)Phase > EfiPciBeforeResourceCollection) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}
