/** @file
  Implements the MpPark protocol

  Copyright (c) 2011-2015, Qualcomm Technologies,Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 01/20/15   rli     Update to read NumActiveCores from cfg file
 09/22/14   rli     Added support for PSCI_COMPLIANT feature
 08/15/14   sm      Switched to new SCM API
 06/30/14   rli     Updated to support AARCH64
 06/30/14   rli     Updated to support octacore BDS menu option
 06/26/14   rli     Removed SCTLR, ACTLR, DACR from MPSTART_ARGS
 05/30/14   sj      Using Uncached memory to hold params while making SCM Calls
 05/14/14   rli     Updated to use SCM protocol
 02/26/14   rli     Updated to use TzRuntimeLib for the TZ fast calls
 04/10/13   niting  Updated NumCpus to read from cfg file
 03/20/13   jz      Fixed ActiveCPUsUpdated()
 11/28/12   aus     Accumulate timeout failures over all cores and keep track of 
                    active number of CPUs. Increased timeout to 50ms
 04/26/12   yg      Pwr and Park Core1 during exit boot services
 01/31/12   niting  Enable cold/warm boot timer sync
 01/25/12   yg      Powerup Core before bringing out of reset
 11/23/11   jz      Memory map PCDs cleanup
 11/08/11   plc     Adding support for 4 cores in MPPark
 10/25/11   tp      Updates to support multi-processor power collapse
 10/07/11   vishalo Changed MpSharedArgs* to MpParkCode*
 09/22/11   yg      Clean cache before taking Core1 out of reset
 06/15/11   cwg     Initial version.

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/EFIScm.h> 
#include <Include/scm_sip_interface.h>

#include "com_dtypes.h"
#include "MpPark.h"
#include "MpParkLib.h"
#include "HALqgic.h"

//*************************************************************
//  Defines
//*************************************************************
 
#define IPIACK_TIMEOUT  50000 // Set to 50000us

/** ACPI-related globals **/
UINT32    NumCpus =  0;
BOOLEAN   PsciCompliant = FALSE;

volatile UINT32 *g_pNumActiveCPUs;  // pointer to uncached memory

/* This is helper function to call in TZ */
STATIC EFI_STATUS scm_tz (UINT32 id, UINT32 coreID, UINT32 Addr)
{
  STATIC QCOM_SCM_PROTOCOL     *pQcomScmProtocol                  = NULL;
  EFI_STATUS                   Status                             = EFI_SUCCESS;
  UINT64                       Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                       Results[SCM_MAX_NUM_RESULTS]       = {0};
  tz_set_boot_addr_t           *SysCallReq                        = (tz_set_boot_addr_t*)Parameters;
  tz_syscall_rsp_t             *SysCallRsp                        = (tz_syscall_rsp_t*)Results;
    
  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&pQcomScmProtocol);
  if( (Status != EFI_SUCCESS) || (pQcomScmProtocol == NULL) )
  {
      DEBUG(( EFI_D_ERROR, "LocateProtocol for SCM failed, Status = (0x%x)\r\n", Status));
      goto ErrorExit;
  }

  SysCallReq->boot_addr = Addr;
  if (coreID >= 4 )
  {
      SysCallReq->affl0_bmsk = 1 << (coreID%4);
      SysCallReq->affl1_bmsk = 0x2;
  }
  else
  {
      SysCallReq->affl0_bmsk = 1 << coreID;
      SysCallReq->affl1_bmsk = 0x1;
  }

  SysCallReq->affl2_bmsk = 0x1;
  SysCallReq->affl3_bmsk = 0x1;
  SysCallReq->flags      = (8 == sizeof(VOID*))? (id | TZ_BOOT_ADDR_AARCH64) : id;

  // Make the SCM call
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_SET_BOOT_ADDR_ID,
                                            TZ_SET_BOOT_ADDR_ID_PARAM_ID,
                                            Parameters,
                                            Results
                                            );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "ScmSipSysCall() failed, Status:%r\r\n", Status));
    goto ErrorExit;
  }
  if (SysCallRsp->status != 1)
  {
      DEBUG(( EFI_D_ERROR, "TZ_SET_BOOT_ADDR_ID failed, status=0x%x\r\n", SysCallRsp->status));
      goto ErrorExit;
  }

ErrorExit:
    return Status;
}

/*
 *  This file has 3 parts (TODO: Split into seperate files)
 *
 *  Part 1: Code executed in UEFI Boot services context from UEFI on Core 0. This
 *          part is labeled (A) UEFI Executable code.
 *
 *  Part 2: Code executed during UEFI Boot services from other CPU Cores. Its the
 *          same UEFI code at the same address as Core 0 is executed by other cores.
 *          This part is labeled as (B) Other Cores running shared UEFI code. So this
 *          code is executed by other cores only once during MPPark.
 *
 *  Part 3: Code executed in Runtime from HLOS from all cores, code not related to UEFI.
 *          This part is labeled as (C) Runtime Executable code from all CPU Cores.
 *          This code is executed by the cores when ever they come out of power collapse.
 *
 *
 *  The design of the whole module goes like this:
 *
 *  Refer to the structure instance MPSTART_ARGS from MpPark.h
 *
 *      typedef struct
 *      {
 *          UINT64 ProcessorId;      // Processor ID
 *          UINT64 JumpAddress;      // Jump Address
 *          UINT32 OSReserved[508];
 *      
 *          UINT32 MailboxCode[500];
 *          UINT32 Reserved[10];
 *          UINT32 Callback;         // Cpu init C code
 *          UINT32 StackTop;         // STACK TOP
 *      
 *      } MPSTART_ARGS;
 *
 *   # MailboxCode : Contains the code that's executed by the CPU Core as follows:
 *                   * MailBox Code
 *                   * Stack
 *
 *   # JumpAddress : Updated by HLOS when it schedules anything for this core to execute
 *
 *   # Callback    : Points to the function MpParkPerCPUInit
 *
 *   # StackTop points to the stack located in MailBox Code region (end of that array)
 *
 *
 *  - There should be as many of the above structure instances as many CPU Cores + 1
 *  - First instances of the structure holds the Cold boot startup code with reset vectors
 *     This code is pointed to executed for ALL cores
 *  - Rest of the instances are per core basis, each core has its own instance so these
 *     cores can be scheduled independently and are set as warm boot handler for the core
 *
 *   (TODO: Add More comments)
 * */


/*==============================================================================
 *   Runtime Executable code from all CPU Cores.
 *   NOTE: this code is *NOT* executed in UEFI context.
 *
 *  ****************    START   **************************************
 *
 *=========================================================================== */

EFI_EVENT EfiExitBootServicesEvent = (EFI_EVENT)NULL;
extern EFI_GUID  gQcomTokenSpaceGuid;

EFI_STATUS MpParkProcessor ( VOID );

/* This callback is to notify that boot services is about to exit */
VOID
EFIAPI
ExitBootServicesCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
    if ( PsciCompliant )
    {
    /* When PSCI is enabled we just power up A57 rail*/
      PowerupCPUCore(4); // same action as for Core 4 (first core in A57)
      DEBUG ((EFI_D_WARN, "PSCI_COMPLIANT - MpPark skipped.\r\n"));
    }
    else
    {
      ASSERT (MpParkProcessor () == EFI_SUCCESS);
    }

}

STATIC
VOID
SetupQgic(
  VOID 
  )
{
  HAL_qgic_BaseAddressType qgicBaseAddrs;
  
  /* Setup HAL QGIC */
  qgicBaseAddrs.nGICDBaseAddress = PcdGet32(PcdInterruptBaseAddress);
  qgicBaseAddrs.nGICCBaseAddress = PcdGet32(PcdQGICCBaseAddress);
  
  HAL_qgic_Init(&qgicBaseAddrs);
  
  /* Enable SGI's */
  HAL_qgic_Enable(0);
  HAL_qgic_Enable(1);
  HAL_qgic_Enable(2);
  HAL_qgic_Enable(3);
  HAL_qgic_Enable(4);
  HAL_qgic_Enable(5);
  HAL_qgic_Enable(6);
  HAL_qgic_Enable(7);
  HAL_qgic_Enable(8);
  HAL_qgic_Enable(9);
  HAL_qgic_Enable(10);
  HAL_qgic_Enable(11);
  HAL_qgic_Enable(12);
  HAL_qgic_Enable(13);
  HAL_qgic_Enable(14);
  HAL_qgic_Enable(15);
  
  /* Set priority mask to lowest to allow all interrupts */
  HAL_qgic_SetPriorityMask(HAL_QGIC_PRIORITY_LOWEST);
  
  /* Enable Interrupts */
  HAL_qgic_EnableInterrupts();
}

/*
 * Called from Aux CPU startup code
 */
VOID
MpParkPerCPUInit (
  UINT32 PageBase
  )
{
  MPSTART_ARGS* pMPStartArgs;
  UINTN MemoryBase = 0;
  MAILBOX Mailbox;

  Mailbox = (MAILBOX)(PageBase + (sizeof(MPSTART_ARGS)/2));

  ASSERT (GetMemoryInfo(DDR_SYSTEM_MEMORY, &MemoryBase, NULL) == EFI_SUCCESS);
  
  pMPStartArgs  = (MPSTART_ARGS*) (MemoryBase + PcdGet32(PcdMpParkCodeBaseOffset));

  SetupQgic();

  g_pNumActiveCPUs = (UINT32*)&pMPStartArgs->StackTop;
  (*g_pNumActiveCPUs)++;

  ArmDataSyncronizationBarrier();

  Mailbox();
}


/*==============================================================================
 *   Runtime Executable code from all CPU Cores.
 *   NOTE: this code is *NOT* executed in UEFI context.
 *
 *  ****************    END   **************************************
 *
 *=========================================================================== */

STATIC
EFI_STATUS
ActiveCPUsUpdated (
  UINTN  curNCpus
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN timeStalled = 0;
    
  /* Wait until secondary CPU updates active CPU count */
  while (curNCpus == *g_pNumActiveCPUs)
  {
      gBS->Stall(1);
      timeStalled += 1;

      if (timeStalled > IPIACK_TIMEOUT)
        return EFI_TIMEOUT;
  }

  return Status;
}

EFI_STATUS
MpParkProcessor ( VOID )
{
  EFI_STATUS              Status = EFI_INVALID_PARAMETER;
  MPSTART_ARGS*           pMPStartArgs;
  UINT32                  ResetVector;
  UINTN                   curCPUs, failCPUs = 0;
  UINTN                   MemoryBase = 0;
  BOOLEAN                 Flag = FALSE;
  UINTN                   VarSize;

  Status = GetConfigValue ("NumActiveCores", &NumCpus);
  if ((Status != EFI_SUCCESS) || (NumCpus == 0))
  {
    Status = GetConfigValue ("NumCpus", &NumCpus);
    if ((Status != EFI_SUCCESS) || (NumCpus == 0))
    {
      DEBUG ((EFI_D_WARN, "NumCpus not found in uefiplat.cfg. Defaulting to 1.\r\n"));
      // Default to 1
      NumCpus = 1;
    }
  }

 
  /* Check if EnableOctacore global variable set */
  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"EnableOctacore", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);
  if ((Status == EFI_SUCCESS) && Flag )
  {
    NumCpus = 8;
    DEBUG ((EFI_D_INFO, "Octacore mode is enabled. NumCpus = %d.\r\n", NumCpus));
  }
    
  Status = GetMemoryInfo(DDR_SYSTEM_MEMORY, &MemoryBase, NULL);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }
  
  /* Parked Page 0 - Cold startup code / aligned to 1MB boundary */
  pMPStartArgs = (MPSTART_ARGS*) (MemoryBase + PcdGet32(PcdMpParkCodeBaseOffset));
  ZeroMem(pMPStartArgs, sizeof(MPSTART_ARGS));

  /* relocate mailbox code  */
  CopyMem(pMPStartArgs->MailboxCode, StartAuxiliaryCPU, (UINTN)StartAuxiliaryCPU_END - 
                                                  (UINTN)StartAuxiliaryCPU);
  ResetVector = (UINT32)((UINTN)pMPStartArgs->MailboxCode);

  for(curCPUs = 1; curCPUs < NumCpus; curCPUs++)
  {
    /* set TZ cold boot vector */
    scm_tz(TZ_BOOT_ADDR_COLD, curCPUs, ResetVector);
  }

  /* use uncached memory for shared variables  */
  g_pNumActiveCPUs = (UINT32*)&pMPStartArgs->StackTop;

  *g_pNumActiveCPUs = 1;
  curCPUs = *g_pNumActiveCPUs;


  /* 
   * Parked Page 1 - CPU0
   */
  pMPStartArgs++;
  ZeroMem(pMPStartArgs, sizeof(MPSTART_ARGS));

  /* Default values per parking protocol */
  pMPStartArgs->ProcessorId = 0xFFFFFFFFFFFFFFFF;
  pMPStartArgs->JumpAddress   = 0x0000000000000000;

  /* relocate mailbox code */
  CopyMem(pMPStartArgs->MailboxCode, MpParkMailbox, (UINTN)MpParkMailbox_END - 
                                                    (UINTN)MpParkMailbox);

  /* set TZ CPU0 warm reset vector */
  ResetVector = (UINT32)((UINTN)pMPStartArgs->MailboxCode);
  scm_tz(TZ_BOOT_ADDR_WARM, (curCPUs - 1), ResetVector);

  /* 
   * Parked Pages 2-Number of CPUs (Auxillary CPUs)
   */
  while(curCPUs < NumCpus) {
    if (PowerupCPUCore(curCPUs) != EFI_SUCCESS)
    {
      DEBUG(( EFI_D_ERROR, "Powering up core %d failed.\r\n", curCPUs));
      curCPUs++;
      failCPUs++;
      continue;
    }
    pMPStartArgs++;
    ZeroMem(pMPStartArgs, sizeof(MPSTART_ARGS));

    /* Default values per parking protocol */
    pMPStartArgs->ProcessorId = 0xFFFFFFFFFFFFFFFF;
    pMPStartArgs->JumpAddress   = 0x0000000000000000;

    /* relocate mailbox code */
    CopyMem(pMPStartArgs->MailboxCode, MpParkMailbox, (UINTN)MpParkMailbox_END - 
                                                      (UINTN)MpParkMailbox);

    /* set TZ warm reset vector for this auxillary CPU */
    ResetVector = (UINT32)((UINTN)pMPStartArgs->MailboxCode);;
    scm_tz(TZ_BOOT_ADDR_WARM, curCPUs, ResetVector);

    pMPStartArgs->StackTop   = (UINTN)&pMPStartArgs->MailboxCode[500-sizeof(UINTN)/4];
    pMPStartArgs->Callback   = (UINTN)MpParkPerCPUInit;
    ArmDataMemoryBarrier();

    /* This auxillary Core will invalidate cache on initialization/reset */
    ArmCleanDataCache();

    /* release this auxillary CPU from reset */
    CPU_Start(curCPUs);

    /* wait for g_numActiveCPUs to increment */
    if (ActiveCPUsUpdated(curCPUs-failCPUs) == EFI_TIMEOUT) {
      Status = EFI_TIMEOUT;
      failCPUs++;
    }
    
    curCPUs++;
  }
  
  return Status;
}

EFI_STATUS
MpParkInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTablef
  )
{
    if (PowerupCPUCore(0) != EFI_SUCCESS)
    {
      DEBUG(( EFI_D_ERROR, "Chipset-specific initialization failed.\r\n"));
    }

  return gBS->CreateEventEx ( EVT_NOTIFY_SIGNAL,
                              TPL_CALLBACK,
                              ExitBootServicesCallBack,
                              NULL,
                              &gEfiEventExitBootServicesGuid,
                              &EfiExitBootServicesEvent);

}
