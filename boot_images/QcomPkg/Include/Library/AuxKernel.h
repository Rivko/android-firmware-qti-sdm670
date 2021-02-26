/** @file

  The Auxiliary Kernel data structures and APIs

  Copyright (c) 2016, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/26/16   ai      Created
=============================================================================*/
#ifndef __AUXKERNEL_H__
#define __AUXKERNEL_H__

#include <Uefi.h>

#undef ALIGN
#if defined (__GNUC__)
  #define ALIGN(n)  __attribute__((aligned(n)))
#else
  #define ALIGN(n)  __align(n)
#endif


#define AUXKERNEL_SIGNATURE       SIGNATURE_32 ('N', 'C', 'L', 'S')     /* Signature */
#define AUXKERNEL_VERSION_1_0     0x00010000                          /* Version 1.0 */
#define AUXKERNEL_MESSAGE_SIZE    1024             /* Size of BSP/AP message buffers */


/* Prototype for functions launched by AuxKernel when using QC's MP services protocol */
typedef EFI_STATUS (* EFI_AUXKERNEL_PROCEDURE) (UINTN, UINTN);

typedef struct _AUXKERNEL AUXKERNEL;
typedef struct{
  UINT32    Signature;   /* Signature */ 
  UINT32    Version;     /* Version */ 
  UINT32    Length;      /* Length of Header+Data */
  UINT32    Reserved;    /* For alignment */
} AUXKERNEL_HEADER;

/* Core Settings - Cache, MMU, etc.*/
typedef struct {
  VOID*       TTBR0;    
  UINTN       MAIR;
  UINTN       TCR;
} AUX_CORE_CONFIG_INFO;

/* I/O management between BSP and AP */
typedef struct {
  /* Msg Buffers need to be aligned since they contain pointers */
  UINT8       MsgToBspBuffer[AUXKERNEL_MESSAGE_SIZE]      ALIGN(16); /* AP msg */
  UINT8       MsgFromBspBuffer[AUXKERNEL_MESSAGE_SIZE]    ALIGN(16); /* BSP response */
  BOOLEAN     MsgToBspFlag;       /* AP has msg for BSP */
  BOOLEAN     MsgToBspAckFlag;    /* BSP has read msg from AP */
  BOOLEAN     MsgFromBspFlag;     /* BSP has response for AP */
  BOOLEAN     MsgFromBspAckFlag;  /* AP has read response from BSP */
} AUX_IO_INFO;

/* Aux kernel state machine */
typedef enum {
  AUXKERNEL_STATE_IDLE            = 0,
  AUXKERNEL_STATE_TASK_ENQUEUED   = 1,
  AUXKERNEL_STATE_ACTIVE          = 2,
  AUXKERNEL_STATE_32_BIT_MAX      = 0x7FFFFFF
} AUXKERNEL_STATE;

/* Structure used to control APs and facilitate MP */
struct _AUXKERNEL {
  AUXKERNEL_HEADER          Header;      /* Headers */
  UINTN                     ApCpuNumber; /* Core Number */
  UINTN                     ApStackBase; /* Stack Base */
  AUXKERNEL_STATE           State;       /* Aux Kernel state machine */ 
  AUX_CORE_CONFIG_INFO      CoreInfo;    /* Core settings */
  AUX_IO_INFO               IoInfo;      /* I/O config */
  AUXKERNEL                 **Ap_gAuxKernel; /* User code gAuxKernel */
  EFI_AUXKERNEL_PROCEDURE   MpUserEntryFn;   /* Starting point of user code */
  UINTN                     MpUserEntryArg;  /* Arg1 - typically ImageHandle */
  UINTN                     MpUserEntryArg2; /* Arg2 - typically SystemTable */
  UINTN                     RegisterContext[36];  /* Space to backup register context with SetJump, if crash/assert happens, the stored context can be restored to continue */
};


/**
  Initializes auxiliary core.
  Executes on auxiliary core.

  @param[in] context           Pointer to AUXKERNEL with information
                               pertaining to initializing this auxiliary core
**/
VOID
_AuxCPUEntryPoint(
  UINTN context
  );

#endif /* __AUXKERNEL_H__ */

