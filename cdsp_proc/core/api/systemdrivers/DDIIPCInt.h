#ifndef DDIIPCINT_H
#define DDIIPCINT_H
/*
===========================================================================
*/
/**
  @file DDIIPCInt.h 
  
  Public interface include file for the inter-processor interrupt driver.
  
  This file contains interface definitions for triggering interrupts from
  one processor to another.
*/
/*  
  ====================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/DDIIPCInt.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALSys.h"


/** 
  @addtogroup apis
  @{ 
*/ 

/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Define the interface version.
 */
#define DALIPCINT_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)


/**
 * The processor identifier.
 *
 * This type represents a processor in the system.
 * Note: Any new ids must be added to the end, and the numbers must match
 * the IPCInt HAL for HAL_ipcint_ProcessorType.
 */
typedef enum
{
  DALIPCINT_PROC_NULL       = 0, /**< No processor. */
  DALIPCINT_PROC_MCPU       = 1, /**< The modem CPU. */ 
  DALIPCINT_PROC_MDSPSW     = 2, /**< Modem software DSP. */
  DALIPCINT_PROC_MDSPFW     = 3, /**< Modem firmware DSP. */
  DALIPCINT_PROC_ACPU       = 4, /**< Applications processor (core 0). */
  DALIPCINT_PROC_ACPU1      = 5, /**< Applications processor (core 1). */
  DALIPCINT_PROC_ADSP       = 6, /**< Application DSP. */
  DALIPCINT_PROC_SPS        = 7, /**< Sensors processor. */
  DALIPCINT_PROC_RPM        = 8, /**< RPM */
  DALIPCINT_PROC_RIVA       = 9, /**< Riva processor. */
  DALIPCINT_PROC_WCN        = DALIPCINT_PROC_RIVA, /**< Riva processor = WCN processor . */
  DALIPCINT_PROC_GSS        = 10,/**< GSS processor. */
  DALIPCINT_PROC_LPASS      = 11,/**< LPASS processor. */
  DALIPCINT_PROC_TZ         = 12, /**< Trust Zone (core 0). */
  DALIPCINT_PROC_SPSS       = 13, /**< Secure Processor */
  DALIPCINT_PROC_CDSP       = 14, /**< Turing (Compute) DSP */
  DALIPCINT_PROC_GFX        = 15, /**< Graphics */
  DALIPCINT_PROC_TITAN      = 16, /**< Titan Camera */
  DALIPCINT_PROC_SDC        = 17, /**< Sensors Context Hub (SDC) */
  /** @cond */
  DALIPCINT_NUM_PROCS       = 18,
  DALIPCINT_PROC_32BITS     = 0x7FFFFFF
  /** @endcond */

} DalIPCIntProcessorType;


/**
 * The set of possible inter-processor interrupts.
 *
 * This type represents which inter-processor interrupt should be sent.
 * There are ten general purpose interrupts, and a few special use ones.
 */
typedef enum
{
  DALIPCINT_GP_0           = 0,  /**< General purpose interrupt #0 */ 
  DALIPCINT_GP_1           = 1,  /**< General purpose interrupt #1 */
  DALIPCINT_GP_2           = 2,  /**< General purpose interrupt #2 */
  DALIPCINT_GP_3           = 3,  /**< General purpose interrupt #3 */
  DALIPCINT_GP_4           = 4,  /**< General purpose interrupt #4 */
  DALIPCINT_GP_5           = 5,  /**< General purpose interrupt #5 */
  DALIPCINT_GP_6           = 6,  /**< General purpose interrupt #6 */
  DALIPCINT_GP_7           = 7,  /**< General purpose interrupt #7 */
  DALIPCINT_GP_8           = 8,  /**< General purpose interrupt #8 */
  DALIPCINT_GP_9           = 9,  /**< General purpose interrupt #9 */
  DALIPCINT_GP_LOW         = DALIPCINT_GP_0,  /**< Low priority irq. */
  DALIPCINT_GP_MED         = DALIPCINT_GP_1,  /**< Medium priority irq. */
  DALIPCINT_GP_HIGH        = DALIPCINT_GP_2,  /**< High priority irq. */
  DALIPCINT_SOFTRESET      = 10,  /**< Soft reset interrupt. */
  DALIPCINT_WAKEUP         = 11,  /**< Wakeup interrupt */

  /** @cond */
  DALIPCINT_NUM_INTS       = 12,
  DALIPCINT_INT_32BITS     = 0x7FFFFFF
  /** @endcond */

} DalIPCIntInterruptType;



/*=========================================================================
      Interface Definitions
==========================================================================*/

/** @cond */

typedef struct DalIPCInt DalIPCInt;
struct DalIPCInt
{
  struct DalDevice DalDevice;
  DALResult (*Trigger)(uint32 ClientId, DalDeviceHandle * _h,  DalIPCIntProcessorType  eTarget,  DalIPCIntInterruptType  eInterrupt);
  DALResult (*TriggerBySource)(uint32 ClientId, DalDeviceHandle * _h,  DalIPCIntProcessorType  eSource,  DalIPCIntProcessorType  eTarget,  DalIPCIntInterruptType  eInterrupt);
  DALResult (*IsSupported)(uint32 ClientId, DalDeviceHandle * _h,  DalIPCIntProcessorType  eTarget,  DalIPCIntInterruptType  eInterrupt);
  DALResult (*IsSupportedBySource)(uint32 ClientId, DalDeviceHandle * _h,  DalIPCIntProcessorType  eSource,  DalIPCIntProcessorType  eTarget,  DalIPCIntInterruptType  eInterrupt);
};

typedef struct DalIPCIntHandle DalIPCIntHandle; 
struct DalIPCIntHandle 
{
  uint32 dwDalHandleId;
  const DalIPCInt * pVtbl;
  void * pClientCtxt;
  uint32 dwVtblen;
};

#define DAL_IPCIntDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALIPCINT_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/*=========================================================================
      Interface Declarations
==========================================================================*/

/* ============================================================================
**  Function : DalIPCInt_Trigger
** ============================================================================*/
/**
  Trigger an interrupt to a different processor.

  This function is used to trigger an interrupt to a different processor.

  @param _h         [in] -- DAL device handle.
  @param eTarget    [in] -- The processor to trigger on.
  @param eInterrupt [in] -- The interrupt to send.

  @return
  DAL_SUCCESS -- Will return DAL_SUCCESS even if the interrupt could
  not be triggered.  To verify support DalIPCInt_IsSupported should be
  used. 
 
  @dependencies
  None.
*/ 

static __inline DALResult DalIPCInt_Trigger
(
  DalDeviceHandle        *_h,
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(
        DALVTBLIDX(((DalIPCIntHandle *)_h)->pVtbl, Trigger ),
        _h, eTarget, eInterrupt);
   }
   return ((DalIPCIntHandle *)_h)->pVtbl->Trigger(DALDEVICE_LOCAL_CLIENTID, _h, eTarget, eInterrupt);
}


/* ============================================================================
**  Function : DalIPCInt_TriggerBySource
** ============================================================================*/
/**
  Trigger an interrupt to a processor from an arbitrary processor.

  This function is used to trigger an interrupt to a processor using a
  potentially different source processor from the currently executing one.
  The source processor may be different from the current processor although
  generally this will not work except in certain testing scenarios.

  @param _h         [in] -- DAL device handle.
  @param eSource    [in] -- The processor to trigger from.
  @param eTarget    [in] -- The processor to trigger on.
  @param eInterrupt [in] -- The interrupt to send.

  @return
  DAL_SUCCESS -- Will return DAL_SUCCESS even if the interrupt could
  not be triggered.  To verify support DalIPCInt_IsSupported should be
  used. 
 
  @dependencies
  None.
*/ 

static __inline DALResult DalIPCInt_TriggerBySource
(
  DalDeviceHandle        *_h,
  DalIPCIntProcessorType  eSource,
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_3(
        DALVTBLIDX(((DalIPCIntHandle *)_h)->pVtbl, TriggerBySource ),
        _h, eSource, eTarget, eInterrupt);
   }
   return ((DalIPCIntHandle *)_h)->pVtbl->TriggerBySource(DALDEVICE_LOCAL_CLIENTID,
     _h, eSource, eTarget, eInterrupt);
}


/* ============================================================================
**  Function : DalIPCInt_IsSupported
** ============================================================================*/
/**
  This function returns if the given interrupt is supported to be triggered
  on the given target processor.

  @param _h         [in] -- DAL device handle.
  @param eTarget    [in] -- The target processor to check.
  @param eInterrupt [in] -- The interrupt to check.

  @return
  DAL_SUCCESS -- Interrupt is supported.\n
  DAL_ERROR -- Interrupt is not supported.
 
  @dependencies
  None.
*/ 

static __inline DALResult DalIPCInt_IsSupported
(
  DalDeviceHandle        *_h,
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt)
{
  if(DALISREMOTEHANDLE(_h))
  {
     DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
     return hRemote->pVtbl->FCN_1(
       DALVTBLIDX(((DalIPCIntHandle *)_h)->pVtbl, IsSupported ),
       _h, eTarget, eInterrupt);
  }
   return ((DalIPCIntHandle *)_h)->pVtbl->IsSupported(DALDEVICE_LOCAL_CLIENTID,
     _h, eTarget, eInterrupt);
}


/* ============================================================================
**  Function : DalIPCInt_IsSupportedBySource
** ============================================================================*/
/**
  This function returns if the given interrupt is supported to be triggered
  on the given target processor from the given source processor.
  
  Note this may return DAL_ERROR if the source is different from the current
  processor but return DAL_SUCCESS if run on the same processor as the
  given source.

  @param _h         [in] -- DAL device handle.
  @param eSource    [in] -- The source processor to check.
  @param eTarget    [in] -- The target processor to check.
  @param eInterrupt [in] -- The interrupt to check.

  @return
  DAL_SUCCESS -- Interrupt is supported.\n
  DAL_ERROR -- Interrupt is not supported.
 
  @dependencies
  None.
*/ 

static __inline DALResult DalIPCInt_IsSupportedBySource
(
  DalDeviceHandle        *_h,
  DalIPCIntProcessorType  eSource,
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
)
{
  if(DALISREMOTEHANDLE(_h))
  {
     DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
     return hRemote->pVtbl->FCN_3(
       DALVTBLIDX(((DalIPCIntHandle *)_h)->pVtbl, IsSupportedBySource ),
       _h, eSource, eTarget, eInterrupt);
  }
   return ((DalIPCIntHandle *)_h)->pVtbl->IsSupportedBySource(DALDEVICE_LOCAL_CLIENTID,
     _h, eSource, eTarget, eInterrupt);
}

/** 
  @}
*/
void IPCInt_Init(void);
#endif /* !DDIIPCINT_H */

