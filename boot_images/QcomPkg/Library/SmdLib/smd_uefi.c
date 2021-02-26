/*=========================================================================

                    Shared Memory Driver Simulator Source File

 Copyright (c)  2013-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/12/15   rli     Updated to use strncmp from LoaderUtilsLib
11/26/14   bm      Update APCS_IPC register address for honeybadger
03/25/14   bm      64bit support
01/13/13   bm      Fall back on PCD if HWIO query fails.
01/07/13   bm      Get APCS_IPC_INTERRUPT register address from HWIO
03/21/13   bm      Remove call to smd_uefi_start_isr_timer - no need to fake
                   trigger SmdIsr.
03/21/13   bm      Implement memcpy routine to ensure word aligned access.
03/07/13   bm      HWIO does not have KPSS base for 8x26 and 8x10, use known 
                   APPS_IPC address.
03/06/13   bm      Use libc functions exported by DalModLib.
03/06/13   bm      Use HWIO for APPS_IPC addr and other fixes.
12/18/12   bm      SMD OS/target file for UEFI boot environment.
===========================================================================*/

/*===========================================================================
                                INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "smd.h"
#include "smd_os.h"
#include "smd_internal.h"
#include "smd_target.h"
#include "smd_os_common.h"
#include "smd_main.h"
#include "smem.h"

#include <Library/UefiBootServicesTableLib.h>
#include "Protocol/EFIHWIO.h"

#define BYTES_PER_WORD 4
#define BITS_PER_BYTE  8

/*=========================================================================== 
                               TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
                           LOCAL DATA DECLARATIONS 
===========================================================================*/
/**
 * SMD context structure for each running task.
 *
 * Standalone SMD Lite is taskless, hence only one context 
 * exists for all active ports. 
 */
static smd_context_type smd_context;

/** Table defining interrupts for each channel in the target */
smd_interrupt_info_type smd_interrupt_table[SMEM_NUM_HOSTS];

/*===========================================================================
                          LOCAL FUNCTION DEFINITIONS
===========================================================================*/

static void* smd_uefi_get_ipc_register_from_pcd(void)
{
  /* Get SMEM info based on chipset family */
  if((PcdGet32(PcdSmemInformation) & 0xff) == 1/*badger - 8994*/)
  {
    return (void*)0xF900D008;
  } else if((PcdGet32(PcdSmemInformation) & 0xff) == 2/*bear 8916*/)
  {
    return (void*)0x0B011008;
  } else if((PcdGet32(PcdSmemInformation) & 0xff) == 3/*honeybadger 8996*/)
  {
    return (void*)0x09820010;
  } else if((PcdGet32(PcdSmemInformation) & 0xff) == 5/*nazgul*/)
  {
    return (void*)0x17911008;
  }

  ERR_FATAL("smd_uefi_get_ipc_register: Unsupported Chip family, PCD:0x%x",
      PcdGet32(PcdSmemInformation), 0, 0);
}

static void* smd_uefi_get_ipc_register(void)
{
  EFI_STATUS Status;
  EFI_HWIO_PROTOCOL *HWIOProtocol;
  EFI_GUID gEfiHwioProtocolGuid = EFI_HWIO_PROTOCOL_GUID ;
  UINT8* apcs_kpss_glb;

  Status = gBS->LocateProtocol(&gEfiHwioProtocolGuid, NULL,
    (void**)&HWIOProtocol);

  if ((EFI_SUCCESS != Status) || (HWIOProtocol == NULL))
  {
     return smd_uefi_get_ipc_register_from_pcd();
  }

  Status = HWIOProtocol->MapRegion(HWIOProtocol, "APCS_KPSS_GLB", 
      (UINT8**) &apcs_kpss_glb);

  if (EFI_SUCCESS != Status)
  {
    /* Try with a different register name, as 8994 has different name */
    Status = HWIOProtocol->MapRegion(HWIOProtocol, "APCS_ALIAS0_APSS_GLB",
        (UINT8**) &apcs_kpss_glb);

    if(EFI_SUCCESS != Status)
    {
      return smd_uefi_get_ipc_register_from_pcd();
    }
  }

  return (void*) (apcs_kpss_glb + 8); /* apcs_ipc_interrupt = apcs_kpss_glb + 8 */
}

/*===========================================================================
FUNCTION      SmdIsr
===========================================================================*/
/**
@brief        SMD interrupt service routine.

              This function will be called by the system when a remote
              processor sends an interrupt to SMSM. This function queues the
              deferred procedure call (DPC) for this interrupt.

@return       TRUE
*/
/*=========================================================================*/
void SmdIsr(void)
{
  smd_main(&smd_context);
}

/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_event_send
===========================================================================*/
/**
@brief        Send an event to the specified processor.

@param[in]    info   the port information pointer

@return       None

@note         May be called at any IRQL
*/
/*=========================================================================*/
void smd_event_send(smd_stream_info_type *info)
{
  /* make sure memory is consistent before sending interrupt */
  SMD_MEMORY_BARRIER();

  /* interrupt the other side for receive (read data I sent) */
  smd_send_interrupt(info->channel_type);
} /* smd_event_send */

/*===========================================================================
  FUNCTION  smd_send_interrupt
===========================================================================*/
/**
Send an interrupt to the target processor in the specified channel. 

@param[in] channel         Channel between the source and target processors

@return
NA.

@dependencies
None.

@sideeffects
Interrupts the target processor.

@note       May be called at any IRQL
*/
/*=========================================================================*/
void smd_send_interrupt( smd_channel_type channel )
{
  smd_interrupt_info_type *intr;

  intr = smd_get_interrupt_info( channel );

  /* If the virtual address has been mapped, write the value directly */
  if( intr && intr->irq_out != SMD_NULL_OUT_INTERRUPT )
  {
    *(intr->irq_out) = intr->irq_out_mask;    
  }
  else
  {
    /* This is probably an error */
  }
}

/*===========================================================================
FUNCTION      smd_acquire_open_list_lock
===========================================================================*/
/**
Locks SMD Lite open ports list critical section.

This function protects the standalone SMD Lite open ports list, accessed from
both client's and ISR context.

@param[in] context      Owner of the SMD Lite open ports list

@return       None

@note         Must be called at IRQL <= DISPATCH_LEVEL
*/
/*=========================================================================*/
void smd_acquire_open_list_lock(smd_context_type *context)
{
  /* UEFI is single threaded, no locks needed */
#if 0  
  DALSYS_SyncEnter( context->os.open_list_cs );
#endif  
}

/*===========================================================================
FUNCTION      smd_release_open_list_lock
===========================================================================*/
/**
Unlocks SMD Lite open ports list critical section.

This function protects the standalone SMD Lite open ports list, accessed from
both client's and ISR context.

@param[in] context      Owner of the SMD Lite open ports list

@return       None

@note         Must be called at IRQL = DISPATCH_LEVEL
*/
/*=========================================================================*/
void smd_release_open_list_lock(smd_context_type *context)
{
  /* UEFI is single threaded, no locks needed */
#if 0  
  DALSYS_SyncLeave( context->os.open_list_cs );
#endif  
}

/*===========================================================================
FUNCTION      smd_acquire_cs_lock
===========================================================================*/
/**
@brief        lock a critical section

@param[in]    cs - pointer the the critical section

@return       None.

@note         Must be called at IRQL <= DISPATCH_LEVEL
*/
/*=========================================================================*/

void smd_acquire_cs_lock(smd_cs_type *cs)
{
  /* UEFI is single threaded, no locks needed */
#if 0  
  DALSYSSyncHandle *cs_ptr = ( DALSYSSyncHandle * ) cs;

  DALSYS_SyncLeave(cs_ptr);
#endif  
}

/*===========================================================================
FUNCTION      smd_release_cs_lock
===========================================================================*/
/**
@brief        unlock a critical section

@param[in]    cs - pointer the the critical section

@return       None.

@note         Must be called at IRQL = DISPATCH_LEVEL
*/
/*=========================================================================*/
void smd_release_cs_lock(smd_cs_type *cs)
{
  /* UEFI is single threaded, no locks needed */
#if 0  
  DALSYSSyncHandle *cs_ptr = ( DALSYSSyncHandle * ) cs;

  DALSYS_SyncLeave(cs_ptr);
#endif
}

/*===========================================================================
FUNCTION      smd_cs_create
===========================================================================*/
/**
@brief        Initializes a Critical Section

@return       The critical section.

@note         Must be called at IRQL <= DISPATCH_LEVEL
*/
/*=========================================================================*/
void *smd_cs_create(void)
{
  /* UEFI is single threaded, no locks needed */
#if 0  
  DALSYSSyncHandle  cs = NULL;

  /* Create the new critical section.  On RIVA, using this lock from interrupt 
   * context will cause a crash, so smd_cs_event_create() must be used 
   * instead. */
  if( DAL_SUCCESS != DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE,
                                        &cs, 
                                        NULL ) )
  {
    ERR_FATAL("smd_cs_create: Critical section creation failed", 0, 0, 0);
  }

  return ((void *)cs);
#else
  return (void*)0xdeadbeef;
#endif  
}/* smd_cs_create */

/*===========================================================================
FUNCTION      smd_cs_destroy
===========================================================================*/
/**
@brief        Destroys a Critical Section

@param[in]    Pointer to the critical section to destroy

@return       None

@note         Must be called at IRQL <= DISPATCH_LEVEL
*/
/*=========================================================================*/
void smd_cs_destroy( void *cs )
{
#if 0  
  DALSYS_DestroyObject( ( DALSYSSyncHandle ) cs );
#endif  
}/* smd_cs_destroy */

/*===========================================================================
FUNCTION      smd_assign_context
===========================================================================*/
/**
@brief        Assigns an SMD context to the provided SMD port, based on the
              requested priority.

              This function must be called after populating the info structure
              from the API function that opens the SMD port, before calling
              smd_event_send_smd() to notify the particular task responsible
              for handling the port.

@param[in,out]  info    Pointer to the SMD info structure on which to assign
                        the SMD context. This function will populate
                        info->context with the appropriate task context.

@param[in]    priority  The priority requested by the client. If SMD is
                        implemented using multiple tasks with different OS
                        priorities for ports of different priorities, this
                        function will use this parameter to assign the context
                        to the appropriate task.

@return       None

@note         May be called at any IRQL
*/
/*=========================================================================*/
void smd_assign_context( smd_info_type * info, smd_priority_type priority )
{
  UNREFERENCED_PARAMETER(priority);
  info->context = &smd_context;
}

/*===========================================================================
FUNCTION      smd_context_get_default
===========================================================================*/
/**
@brief        Returns a pointer to the default SMD context.

              This function is used by smd_cmd() to get the default SMD
              context used for handling loopback servers.

@return       Default SMD context

@note         May be called at any IRQL
*/
/*=========================================================================*/
smd_context_type * smd_context_get_default(void)
{
  return &smd_context;
}

/*===========================================================================
FUNCTION      smd_cmd_broadcast
===========================================================================*/
/**
@brief        Send an SMD command to all SMD tasks

              This function iterates through the list of SMD tasks and calls
              smd_cmd_context() to send the command to each task.

              This function is implemented by the OS-specific code, which
              alone has visibility into the true number of tasks and which
              ports they correspond to.

@param[in]    cmd_ptr    Pointer of the command buffer to send

@return       None

@note         Must be called at IRQL <= DISPATCH_LEVEL
*/
/*=========================================================================*/
void smd_cmd_broadcast( smd_cmd_type *cmd_ptr )
{
  smd_cmd_context(cmd_ptr, &smd_context);
}

/*===========================================================================
FUNCTION      smd_init_critical_sections
===========================================================================*/
/**
  Sets up critical sections used in Standalone SMD Lite

  @return
  None.
 
  @dependencies
  None.

  @sideeffects
  Initializes critical sections.
*/
/*=========================================================================*/
static void smd_init_critical_sections( void )
{
  /* UEFI is single threaded, no locks needed */
#if 0  
  smd_context_type *context;

  /* Initialize critical section for SMD Lite open ports list */
  context = smd_context_get_default();

  if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION,
                                       &context->os.open_list_cs, 
                                       NULL) )
  {
    ERR_FATAL("smd_init_critical_sections: SMDL open list cs creation failed",
              0, 0, 0);
  }
#endif  
}

/*===========================================================================
FUNCTION      smd_target_init_interrupts

DESCRIPTION   This function initializes the interprocessor interrupts for this
              target.

ARGUMENTS     this_host - source of the interrupts

RETURN VALUE  void
===========================================================================*/
void smd_target_init_interrupts( smem_host_type this_host )
{
  uint32 i;

  UNREFERENCED_PARAMETER(this_host);

  /* Clear out the interrupt table */
  for( i = 0; i < SMEM_NUM_HOSTS; i++ )
  {
    smd_interrupt_table[i].to = SMEM_INVALID_HOST;
    smd_interrupt_table[i].irq_out = SMD_NULL_OUT_INTERRUPT;
    smd_interrupt_table[i].irq_out_mask = 0;
    smd_interrupt_table[i].irq_in = SMD_NULL_IN_INTERRUPT;
  }

  /* Set the Apps->RPM out interrupt */
  smd_interrupt_table[SMEM_RPM].to = SMEM_RPM;
  /* get the apcs_ipc register address from hwio */
  //smd_interrupt_table[SMEM_RPM].irq_out = SMD_APSS_RPM_IPC_OUT_PHY;
  smd_interrupt_table[SMEM_RPM].irq_out = (smd_os_intr_type)smd_uefi_get_ipc_register();
  
  smd_interrupt_table[SMEM_RPM].irq_out_mask = SMD_APPS_RPM_IPC_OUT_MASK;
  smd_interrupt_table[SMEM_RPM].irq_in = SMD_NULL_IN_INTERRUPT;
  
}

/*===========================================================================
FUNCTION      smd_target_init
===========================================================================*/
/**
@brief        Performs target specific initialization for SMD.

@return       None
*/
/*=========================================================================*/
void smd_target_init(void)
{
  /* Init critical sections */
  smd_init_critical_sections();

  /* Populate SMD interrupt table */
  smd_target_init_interrupts(SMD_THIS_HOST);
}

/*===========================================================================
FUNCTION      smd_uefi_timer_evt
===========================================================================*/
/**
@brief        Callback triggered by UEFI framework when associated timer
              expires. Trigger SmdIsr on timer expiry.

@param[in]    Event   Event associated with the timer
@param[in]    Context Callback data

@return       None

@note         None
*/
/*=========================================================================*/
void smd_uefi_timer_evt(EFI_EVENT Event, void *Context)
{
  SmdIsr();
}

/*===========================================================================
FUNCTION      smd_uefi_start_isr_timer
===========================================================================*/
/**
@brief        Start a recurring timer to trigger SMD Isr on expiry

@param[in]    None

@return       None

@note         None
*/
/*=========================================================================*/
void  smd_uefi_start_isr_timer(void)
{
  EFI_EVENT timerEvent;
  EFI_STATUS status;

  status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL,
                            TPL_CALLBACK,       /* Task priority level */
                            smd_uefi_timer_evt, /* callback function */
                            NULL,               /* cb data */
                            &timerEvent         /* event returned by fn */
                           );
  ASSERT(status == EFI_SUCCESS);
  
  status = gBS->SetTimer(timerEvent,
                         TimerPeriodic,
                         100000 /* delay in units of 100ns */
                        );
  ASSERT(status == EFI_SUCCESS);
}

/*===========================================================================
FUNCTION      smd_initialize_static
===========================================================================*/
/**
@brief        Start a timer to fire to simulate RPM->UEFI interrupt processing

@param[in]    None

@return       None

@note         None
*/
/*=========================================================================*/
void smd_initialize_static( void )
{
  smd_target_init();

  memset(&smd_context, 0, sizeof(smd_context));

  (void) q_init( &smd_context.cmd_queue );

  smem_init();

  smd_init();

  /* Start a timer to fire to simulate RPM->UEFI interrupt processing */
  //smd_uefi_start_isr_timer();

  return;
}

/*===========================================================================
FUNCTION      smd_32b_algn_memcpy

DESCRIPTION   This function copys data from a source buffer to a destination
              buffer always making word aligned access. It may try to read
              and write data outside the src/dest range to ensure 4byte access.

ARGUMENTS     dest - pointer to destination buffer

              src - pointer to source buffer

              size - number of bytes to copy

RETURN VALUE  dest is returned (to mirror lib c memcpy)
===========================================================================*/
void *smd_32b_algn_memcpy
(
 void *dest,
 void *src,
 unsigned int size
){
  char *c_src = (char *)src;
  char *c_dest = (char *)dest;
  uint32 *i_src;
  uint32 *i_dest;
  register uint32 word_src;
  register uint32 word_src2;
  register uint32 word_dest;

  uint64 src_offset;
  uint64 dest_offset;

  while(size)
  {  
    /* read a word from the src and destination */
    i_src = (uint32*) ((size_t)c_src & (size_t)0xfffffffffffffffc);
    word_src = *i_src;
    src_offset = (size_t)c_src & 0x3;

    i_dest = (uint32*) ((size_t)c_dest & (size_t)0xfffffffffffffffc);
    word_dest = *i_dest;
    dest_offset = (size_t)c_dest & 0x3;

    if(dest_offset >= src_offset)
    {
      /* shift the word_src_be to align with dest */
      word_src <<= (dest_offset - src_offset)*8;
    } else
    {
      /* shift the word_src_be to align with dest */
      word_src >>= (src_offset - dest_offset)*8;
      if(size > (BYTES_PER_WORD - src_offset))
	  {
        /* read next bytes into word_src */
        word_src2 = *(i_src+1);
        switch(src_offset - dest_offset)
        {
          case 1:
            word_src = (word_src & 0x00ffffff) | ( (word_src2 << (3*BITS_PER_BYTE)) & 0xff000000);
            break;
          case 2:
            word_src = (word_src & 0x0000ffff) | ( (word_src2 << (2*BITS_PER_BYTE)) & 0xffff0000);
            break;
          case 3:
            word_src = (word_src & 0x000000ff) | ( (word_src2 << (BITS_PER_BYTE)) & 0xffffff00);
            break;
        }
	  }
    }

    
    /* make the final word to be copied into dest by orring relevent word values */
    switch(BYTES_PER_WORD - dest_offset)
    {
      case 1:
        /* 1 valid byte to copy into dest */
        word_dest = (word_src & 0xff000000) | (word_dest & 0x00ffffff);
        /* write the value into memory */
        *(i_dest) = word_dest;
        size--;
        c_src++;
        c_dest++;
        break;
      case 2:
        /* 2 valid bytes to copy into dest */
        if(size > 1)
        {
          word_dest = (word_src & 0xffff0000) | (word_dest & 0x0000ffff);
          /* write the value into memory */
          *(i_dest) = word_dest;
          size -= 2;
          c_src += 2;
          c_dest += 2;
        } else {
          /* need to copy just a single byte */
          word_dest = (word_src & 0x00ff0000) | (word_dest & 0xff00ffff);
          /* write the value into memory */
          *(i_dest) = word_dest;
          size --;
          c_src ++;
          c_dest ++;
        }
        break;
      case 3:
        /* 3 valid bytes to copy */
        if(size > 2)
        {
          /* copy 3 bytes */
          word_dest = (word_src & 0xffffff00) | (word_dest & 0x000000ff);
          /* write the value into memory */
          *(i_dest) = word_dest;
          size -= 3;
          c_src += 3;
          c_dest += 3;
        } else if(size > 1){
          /* need to copy 2 bytes */
          word_dest = (word_src & 0x00ffff00) | (word_dest & 0xff0000ff);
          /* write the value into memory */

          *(i_dest) = word_dest;
          size -= 2;
          c_src += 2;
          c_dest += 2;
        } else {
          /* need to copy just a single byte */
          word_dest = (word_src & 0x0000ff00) | (word_dest & 0xffff00ff);
          /* write the value into memory */
          *(i_dest) = word_dest;
          size --;
          c_src ++;
          c_dest ++;
        }
        break;
      case 4:
        /* 4 valid bytes to copy */
        if(size > 3)
        {
          /* copy 4 bytes */
          /* write the value into memory */
          *(i_dest) = word_src;
          size -= 4;
          c_src += 4;
          c_dest += 4;
        } else if(size > 2){
          /* need to copy 3 bytes */
          word_dest = (word_src & 0x00ffffff) | (word_dest & 0xff000000);
          /* write the value into memory */
          *(i_dest) = word_dest;
          size -= 3;
          c_src += 3;
          c_dest += 3;
        } else if(size > 1){
          /* need to copy 2 bytes */
           word_dest = (word_src & 0x0000ffff) | (word_dest & 0xffff0000);
          /* write the value into memory */
          *(i_dest) = word_dest;
          size -= 2;
          c_src += 2;
          c_dest += 2;
        } else {
          /* need to copy just a single byte */
          word_dest = (word_src & 0x000000ff) | (word_dest & 0xffffff00);
          /* write the value into memory */
          *(i_dest) = word_dest;
          size --;
          c_src ++;
          c_dest ++;
        }
        break;     
    }//end switch
  }// end while
  return dest;
}/* smd_32b_algn_memcpy */


/*=============================================================================
 Standard C library functions needed in SMD
 Equivalent functions are found in UEFI
=============================================================================*/
#if 0
int strncmp(const char *str1, const char *str2, size_t num)
{
  return AsciiStrnCmp( str1, str2, num );
}

size_t strlen(const char *str)
{
  return AsciiStrLen( str );
}

int strcmp(const char *str1, const char *str2)
{
  return AsciiStrCmp( str1, str2 );
}

void memcpy(void * pDest, const void * pSrc, uint32 iLen)
{
  CopyMem(pDest, pSrc, (UINTN)iLen);
}

void memset(void * pDest, int iVal, uint32 iLen)
{
  SetMem( pDest, (UINTN)iLen, (UINT8)iVal );
}
#endif
