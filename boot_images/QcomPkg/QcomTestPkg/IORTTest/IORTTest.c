/** @file IORTTest.c

Tests for IORT

  Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TestInterface.h>
#include <Library/IORT.h>

#include "com_dtypes.h"

void print_id_mapping_info(char* DevName, uint32 nMappingNum, IORT_IDMapping* pMapping)
{
    AsciiPrint("ID Mapping Info for \"%a\" Mapping %d\n", DevName, nMappingNum);
    AsciiPrint("* InputBase 0x%x\n", pMapping->InputBase);
    AsciiPrint("* NumIDs %d\n", pMapping->NumIDs);
    AsciiPrint("* OutputBase 0x%x\n", pMapping->OutputBase);
    AsciiPrint("* OutputSMMUAddr 0x%x\n", pMapping->OutputSMMUBaseAddr);
    AsciiPrint("* Flags 0x%x\n", pMapping->Flags);
}

void print_named_component_info(char* DevName, IORT_NamedComponentInfo* pDevInfo)
{
    AsciiPrint("Device Info for \"%a\"\n", DevName);
    AsciiPrint("* NumIDMappings %d\n", pDevInfo->NumIDMappings);
    AsciiPrint("* DeviceFlags 0x%x\n", pDevInfo->Flags);
    AsciiPrint("* CacheCoherentAttr 0x%x\n", pDevInfo->CacheCoherentAttr);
    AsciiPrint("* AllocationHints 0x%x\n", pDevInfo->AllocationHints);
    AsciiPrint("* MemoryAccessFlags 0x%x\n", pDevInfo->MemoryAccessFlags);
    AsciiPrint("* MemSizeLimit %d\n", pDevInfo->MemSizeLimit);
}

void print_interrupt_info(char* IntName, IORT_InterruptInfo* pIntInfo) {
    AsciiPrint("Interrupt Info for \"%a\"\n", IntName);
    AsciiPrint("* Vector %d\n", pIntInfo->Vector);
    AsciiPrint("* Flags 0x%x\n", pIntInfo->Flags);
}

void print_smmuv1v2_info(uint64 Addr, IORT_SMMUv1v2Info* pSMMUInfo)
{
    AsciiPrint("SMMU Info for \"0x%016x\"\n", Addr);
    AsciiPrint("* NumIDMappings %d\n", pSMMUInfo->NumIDMappings);
    AsciiPrint("* BaseAddr 0x%016x\n", pSMMUInfo->BaseAddr);
    AsciiPrint("* Span 0x%016x\n", pSMMUInfo->Span);
    AsciiPrint("* Model %d\n", pSMMUInfo->Model);
    AsciiPrint("* Flags 0x%x\n", pSMMUInfo->Flags);
    AsciiPrint("* Number of Context Interrupts %d\n", pSMMUInfo->NumCtxtIrpts);
    AsciiPrint("* Number of PMU Interrupts %d\n", pSMMUInfo->NumPMUIrpts);
    print_interrupt_info("NSgIrpt", &pSMMUInfo->NSgIrpt);
    print_interrupt_info("NSgCfgIrpt", &pSMMUInfo->NSgCfgIrpt);
}

EFI_STATUS IORTTest_GetSMMUInfo(uint64 nSMMUInstanceID)
{
    EFI_STATUS Status = EFI_SUCCESS;
    IxErrnoType retval;
    IORT_SMMUv1v2Info Info;

    retval = IORT_GetSMMUv1v2Info(nSMMUInstanceID, &Info);
    if (E_SUCCESS == retval) {
        IORT_InterruptInfo* pContextInts;
        IORT_InterruptInfo* pPMUInts;
        print_smmuv1v2_info(nSMMUInstanceID, &Info);
        pContextInts = (IORT_InterruptInfo*)AllocatePool(sizeof(IORT_InterruptInfo)*Info.NumCtxtIrpts);
        pPMUInts = (IORT_InterruptInfo*)AllocatePool(sizeof(IORT_InterruptInfo)*Info.NumPMUIrpts);
        if(pContextInts) {
            int i = 0;
            retval = IORT_GetInterruptInfo(nSMMUInstanceID,
                                           IORT_SMMUv1v2_INTTYPE_CONTEXT,
                                           0,
                                           Info.NumCtxtIrpts,
                                           pContextInts);
            if (E_SUCCESS == retval) {
                for (i=0; i < Info.NumCtxtIrpts; i++) {
                    print_interrupt_info("Context Interrupt", 
                                         pContextInts + i);
                }                               
            } else {
                AsciiPrint("IORT_GetInterruptInfo(0x%x, CONTEXT, 0, %d) returned.\n",
                           nSMMUInstanceID, Info.NumCtxtIrpts, retval);
                Status = EFI_INVALID_PARAMETER;
            }
            FreePool(pContextInts);
        } else {
            AsciiPrint("Unable to allocate memory for Context Interrupts.\n");
            Status = EFI_INVALID_PARAMETER;
        }
        if(pPMUInts) {
            int i = 0;
            retval = IORT_GetInterruptInfo(nSMMUInstanceID,
                                           IORT_SMMUv1v2_INTTYPE_PMU,
                                           0,
                                           Info.NumPMUIrpts,
                                           pPMUInts);
            if (E_SUCCESS == retval) {
                for (i=0; i < Info.NumPMUIrpts; i++) {
                    print_interrupt_info("PMU Interrupt", 
                                         pPMUInts + i);
               }                               
            } else {
                AsciiPrint("IORT_GetInterruptInfo(0x%x, PMU, 0, %d) returned.\n",
                           nSMMUInstanceID, Info.NumPMUIrpts, retval);
                Status = EFI_INVALID_PARAMETER;
            }
            FreePool(pPMUInts);
        } else {
            AsciiPrint("Unable to allocate memory for PMU Interrupts.\n");
            Status = EFI_INVALID_PARAMETER;
        }                       
    } else {
        AsciiPrint("IORT_GetSMMUv1v2Info(0x%x) returned %d\n", nSMMUInstanceID, retval);
        Status = EFI_INVALID_PARAMETER;
    }
    return Status;
}

EFI_STATUS IORTTest_GetSMMUv1v2Instances()
{
    EFI_STATUS Status = EFI_SUCCESS;
    uint32 nSMMUInstances;
    uint64* pSMMUInstances;
    IxErrnoType retval;
    
    retval = IORT_GetSMMUv1v2Instances(&nSMMUInstances, NULL);
    if(E_SUCCESS == retval) {
        pSMMUInstances = (uint64*)AllocatePool(nSMMUInstances * sizeof(uint64));
        if (pSMMUInstances) {
            retval = IORT_GetSMMUv1v2Instances(&nSMMUInstances, pSMMUInstances);
            if (E_SUCCESS == retval) {
                uint64* pSMMUInstancePtr = pSMMUInstances;
                AsciiPrint("Found %d SMMUs\n", nSMMUInstances);
                while(nSMMUInstances-- && (EFI_SUCCESS == Status)) {
                   Status = IORTTest_GetSMMUInfo(*pSMMUInstances);
                   if (EFI_SUCCESS != Status) {
                       AsciiPrint("IORTTest_GetSMMUInfo(0x%x) failed", *pSMMUInstances);
                   }
                   pSMMUInstancePtr++;
                }
            } else {
                AsciiPrint("IORT_GetSMMUv1v2Instances(nNum, pInstances) returned %d\n", retval);
                Status = EFI_INVALID_PARAMETER;
            }
            FreePool(pSMMUInstances);
        } else {
            AsciiPrint("Unable to allocate memory for SMMU Instances\n");
            Status = EFI_INVALID_PARAMETER;
        }
          
    } else {
        AsciiPrint("IORT_GetSMMUv1v2Instances(&nNum, NULL) returned %d\n", retval);
        Status = EFI_INVALID_PARAMETER;
    }
    AsciiPrint("IORTTest_GetSMMUv1v2Instances() completed\n");
    return Status;
}

EFI_STATUS IORTTest_WalkDevInfo(char* pszDevName)
{
    EFI_STATUS Status = EFI_SUCCESS;
    IxErrnoType retval;
    IORT_NamedComponentInfo Info;
    
    retval = IORT_GetNamedComponentInfo(pszDevName, &Info);
    if (E_SUCCESS == retval) {
        uint32 numIDMappings = Info.NumIDMappings;
        IORT_IDMapping IDMapping;
        print_named_component_info(pszDevName, &Info);
        while (numIDMappings && (EFI_SUCCESS == Status)) {
            retval = IORT_GetDeviceSMMUIDMappings(pszDevName, Info.NumIDMappings - numIDMappings, 1, &IDMapping);
            if (E_SUCCESS == retval) {
                print_id_mapping_info(pszDevName, Info.NumIDMappings-numIDMappings, &IDMapping);
                Status = IORTTest_GetSMMUInfo(IDMapping.OutputSMMUBaseAddr);
            } else {
                AsciiPrint("IORT_GetDeviceSMMUIDMappings(%a, %d, 1) returned %d", pszDevName, Info.NumIDMappings - numIDMappings, retval);
                Status = EFI_INVALID_PARAMETER;
                break;
            }
            numIDMappings--;
        }
    } else {
        AsciiPrint("IORT_GetNamedComponentInfo(\"&a\") returned %d\n", pszDevName, retval);
        Status = EFI_INVALID_PARAMETER;
    }
    AsciiPrint("IORTTest_GetUSBDevInfo completed\n");
    return Status;
}

/**
The user Entry Point for Application. The user code starts with this function
as the real entry point for the application.

@param[in] ImageHandle    The firmware allocated handle for the EFI image.  
@param[in] SystemTable    A pointer to the EFI System Table.

@retval EFI_SUCCESS       The entry point is executed successfully.
@retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
IORTTestMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_STATUS OverallStatus = EFI_SUCCESS;
    
    TEST_START("IORTTest");

    Status = IORTTest_GetSMMUv1v2Instances();
    TestStatus("IORTTest_GetSMMUv1v2Instances",Status);
    if(EFI_SUCCESS != Status) {
        OverallStatus = EFI_INVALID_PARAMETER;
    }
    Status = IORTTest_WalkDevInfo("\\_SB_.USB3-0");
    TestStatus("IORTTest_WalkDevInfo(\"\\_SB_.USB3-0\")",Status);
    if(EFI_SUCCESS != Status) {
        OverallStatus = EFI_INVALID_PARAMETER;
    }
    Status = IORTTest_WalkDevInfo("\\_SB_.UFS0");
    TestStatus("IORTTest_WalkDevInfo(\"\\_SB_.UFS0\")",Status);
    if(EFI_SUCCESS != Status) {
        OverallStatus = EFI_INVALID_PARAMETER;
    }
    Status = IORTTest_WalkDevInfo("\\_SB_.SDC4");
    TestStatus("IORTTest_WalkDevInfo(\"\\_SB_.SDC4\")",Status);
    if(EFI_SUCCESS != Status) {
        OverallStatus = EFI_INVALID_PARAMETER;
    }
    Status = IORTTest_WalkDevInfo("\\_SB_.SDC2");
    TestStatus("IORTTest_WalkDevInfo(\"\\_SB_.SDC2\")",Status);
    if(EFI_SUCCESS != Status) {
        OverallStatus = EFI_INVALID_PARAMETER;
    }
    Status = IORTTest_WalkDevInfo("\\_SB_.SDC1");
    TestStatus("IORTTest_WalkDevInfo(\"\\_SB_.SDC1\")",Status);
    if(EFI_SUCCESS != Status) {
        OverallStatus = EFI_INVALID_PARAMETER;
    }
    TEST_STOP("IORTTest");
    return OverallStatus;
}
