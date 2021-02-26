/*==============================================================================

FILE:      uGPIOIntUser.c

DESCRIPTION
  This modules implements the API to utilize the Micro GPIO interrupt controller.
  This Micro GPIO Interrupt Controller module is strictly used in USER Protection
  Domain and in Micro image.The definitions in this module are implemented internally
  and should not be exposed for external client usage directly without using the
  accompanying uGPIOInt.h in the core/api/systemdrivers interface file for this module.

REFERENCES

       Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/src/user/uimage/uGPIOIntUser.c#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

// #include "uTimetick.h" so far now not sure about the logging plan.

#include <atomic_ops.h>
#include <qurt.h>
#include "qurt_int.h"
#include "qurt_event.h"
#include "qurt_anysignal.h"
#include "stringl.h"
#include "uGPIOIntUser.h"
#include "DDIGPIOMgr.h"
#include "pdcGpio.h"
#include "qurt_island.h"
#include "pdcTimestamp.h"
#include "GPIOIntQDI.h"

/*------------------------------------------------------------------------------
 GPIOInt Data Declarations.
------------------------------------------------------------------------------*/ 

/*
 * This is static uGPIOInt state data. It can be accessed for debugging 
 * uGPIO Interrupts to see what is the current registration state of the GPIO.
 */
extern uGPIOIntCntrlType uGPIOIntData;

/*------------------------------------------------------------------------------
 GPIOInt Macro Declarations.
------------------------------------------------------------------------------*/ 
#define UGPIOINT_PRIORITY 4

#define VALID_UGPIO(x) ((x) < MAX_NUMBER_OF_GPIOS)
#define VALID_UGPIO_IDX(x) (uGPIOIntData.aGPIOLUT[x] != UGPIOINT_NONE)
#define UGPIOINT_IDX(x) (uGPIOIntData.aGPIOLUT[x])
#define UGPIOINT_ALT_IDX(x) (uGPIOIntData.state[UGPIOINT_IDX(x)].nAltIdx)
#define UGPIOINT_VALID_ALT_IDX(x) (uGPIOIntData.state[UGPIOINT_IDX(x)].nAltIdx < MAX_NUMBER_OF_UGPIOS)

#define SET_UPPER_16(x) (((x) & 0xffff) << 16)
#define SET_LOWER_16(x)  ((x) & 0xffff)
#define GET_UPPER_16(x) (((x) >> 16) & 0xffff)
#define GET_LOWER_16(x)  ((x) & 0xffff)

/* 
 * For each interrupt handler thread a Default IST stack size is declared.
 */
#define GPIOINT_IST_STACK_SIZE  2048

/*------------------------------------------------------------------------------
Following functions are internal functions.
------------------------------------------------------------------------------*/

extern void uGPIOInt_ISTMain(void * ISTParam);
static int32 uGPIOInt_ConfigureIST(void);

/*==========================================================================

  FUNCTION      uGPIOInt_ConfigureIslandInterrupt

==========================================================================*/

void
uGPIOInt_ConfigureIslandInterrupt(uint32 nIndex )
{
  /* Call into qurt here to configure island interrupt */
  qurt_island_add_interrupt( uGPIOIntData.gsSpec, uGPIOIntData.state[nIndex].qurt_intr_id );
}

/*==========================================================================

  FUNCTION      uGPIOInt_DeconfigureIslandInterrupt

==========================================================================*/

void
uGPIOInt_DeconfigureIslandInterrupt(uint32 nIndex )
{
  /* Call into qurt here to deconfigure island interrupt */
  qurt_island_remove_interrupt( uGPIOIntData.gsSpec, uGPIOIntData.state[nIndex].qurt_intr_id );
}

/*==========================================================================

  FUNCTION      uGPIOInt_SetupFlags

==========================================================================*/

void
uGPIOInt_SetupFlags(uint32 nGPIO, uint32 nFlags)
{
  if ( nFlags & UGPIOINTF_DSR )
  {
    uGPIOIntData.state[UGPIOINT_IDX(nGPIO)].gpio_intr_flags |= UGPIOINTF_DSR;
  }
  if ( nFlags & UGPIOINTF_ISLAND )
  {
    uGPIOIntData.state[UGPIOINT_IDX(nGPIO)].gpio_intr_flags |= UGPIOINTF_ISLAND;
    uGPIOInt_ConfigureIslandInterrupt(UGPIOINT_IDX(nGPIO));
  }
}

/*==========================================================================

  FUNCTION      uGPIOInt_ClearFlags

==========================================================================*/

void
uGPIOInt_ClearFlags(uint32 nIndex )
{
  if ( uGPIOIntData.state[nIndex].gpio_intr_flags & UGPIOINTF_DSR )
  {
    uGPIOIntData.state[nIndex].gpio_intr_flags &= ~UGPIOINTF_DSR;
  }
  if ( uGPIOIntData.state[nIndex].gpio_intr_flags & UGPIOINTF_ISLAND )
  {
    uGPIOInt_DeconfigureIslandInterrupt(nIndex);
    uGPIOIntData.state[nIndex].gpio_intr_flags &= ~UGPIOINTF_ISLAND;
  }
}

/*==========================================================================

  FUNCTION      uGPIOInt_AssignAlternateIndex

==========================================================================*/

int32
uGPIOInt_AssignAlternateIndex(uint32 nGPIO, uint32 nIdx, uint32 nAltIntID)
{
  uint32 index;
  for(index =0;index < MAX_NUMBER_OF_UGPIOS; index++)
  {
    if (uGPIOIntData.state[index].nGPIO == UGPIOINT_NONE) 
    {
      /* Assign Alt Index in Source */
      uGPIOIntData.state[nIdx].nAltIdx = index;

      /* Copy data to new index */
      uGPIOIntData.state[index].nGPIO = (uint8)nGPIO;
      uGPIOIntData.state[index].nAltIdx = (uint8)nIdx;
      uGPIOIntData.state[index].gpio_intr_flags = uGPIOIntData.state[nIdx].gpio_intr_flags;
      uGPIOIntData.state[index].intr_trigger = uGPIOIntData.state[nIdx].intr_trigger;
      uGPIOIntData.state[index].isr = uGPIOIntData.state[nIdx].isr;
      uGPIOIntData.state[index].isr_param = uGPIOIntData.state[nIdx].isr_param;
      uGPIOIntData.state[index].qurt_intr_id = (uint16)nAltIntID;

      break;
    }   
  }
  if (index >= MAX_NUMBER_OF_UGPIOS) 
  {
    return UGPIOINT_ERROR;
  }
  else
  {
    return UGPIOINT_SUCCESS;
  }
}

/*==========================================================================

  FUNCTION      uGPIOInt_AssignInterruptIndex

  DESCRIPTION   See uGPIOIntUser.h

==========================================================================*/

int32
uGPIOInt_AssignInterruptIndex(uint32 nGPIO)
{
  uint32 index;

  if (uGPIOIntData.aGPIOLUT[nGPIO] == UGPIOINT_NONE) 
  {
    for(index =0;index < MAX_NUMBER_OF_UGPIOS; index++)
    {
      if (uGPIOIntData.state[index].nGPIO == UGPIOINT_NONE) 
      {
        uGPIOIntData.state[index].nGPIO = (uint8)nGPIO;
        uGPIOIntData.aGPIOLUT[nGPIO] = (uint8)index;
        break;
      }   
    }
    if (index >= MAX_NUMBER_OF_UGPIOS) 
    {
      return UGPIOINT_ERROR;
    }
    else
    {
      return UGPIOINT_SUCCESS;
    }
  }
  else
  {
    return UGPIOINT_SUCCESS;
  }
 
}

/*==========================================================================

   FUNCTION      uGPIOInt_SetQURTTriggerConfig

  DESCRIPTION   This function sets the trigger for the interrupt vector
                using the QURT OS apis.

  PARAMETERS    
  uint32 nInterrupt  : The interrupt vector number.
  uint32 nTrigger : The interrupt trigger type.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
static int32 uGPIOInt_SetQURTTriggerConfig
(
  
  uint32 nInterruptVector,
  uGPIOIntTriggerType eTrigger,
  uint32 gpio
)
{
  uint32 nDetectType,nPolarity;

  switch (eTrigger)
  {
    case UGPIOINT_TRIGGER_FALLING:
      nDetectType = 1;
      nPolarity = 1;
      break;
    case UGPIOINT_TRIGGER_RISING:
    case UGPIOINT_TRIGGER_DUAL_EDGE:
      nDetectType = 1;
      nPolarity = 0;
      break;
    case UGPIOINT_TRIGGER_HIGH:
      nDetectType = 0;
      nPolarity = 0;
      break;
    case UGPIOINT_TRIGGER_LOW:
      nDetectType = 0;
      nPolarity = 1;
      break;
    default:
      return UGPIOINT_ERROR;    
  }

  if (QURT_EOK != qurt_interrupt_set_config(nInterruptVector, nDetectType, nPolarity))
  {
    /* Throw an error. */
    return UGPIOINT_ERROR;
  } 

  return UGPIOINT_SUCCESS;

} /* SetQURTTriggerConfig */


/*==========================================================================

  FUNCTION      uGPIOInt_UnConfigureDirectConnectInterrupt

  DESCRIPTION   This function configures the given GPIO pin to the
                direct connect line on the main interrupt controller

==========================================================================*/
int32 
uGPIOInt_UnConfigureDirectConnectInterrupt
(
  uint32 gpio,
  boolean bSetTlmmRegs
) 
{
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].gpio_intr_flags &= ~UGPIOINTF_REGISTERED;
  return UGPIOINT_SUCCESS;

} /* END uGPIOInt_UnConfigureDirectConnectInterrupt */


/*==========================================================================

   FUNCTION      uGPIOInt_Init

   DESCRIPTION   See uGPIOIntUser.h

==========================================================================*/
int32 uGPIOInt_Init(void)
{
  uint32 index;
  int32 nResult;
  
  if(uGPIOIntData.uGPIOInt_Init != 1)
  {
    /* 
     * We get the mapping of all supported direct connect interrupts from DALGPIOInt.                                                                            .
     * This is done at power up.                                                                                             .
     */
    qurt_mutex_init(&uGPIOIntData.uGPIOIntLock);

    /*
     * Get Island Spec.
     */
    qurt_island_get_specid( &uGPIOIntData.gsSpec );
    
    /*
     * Initialize the uGPIO LUT table. this the full size of 200 GPIOs
     */
    for(index =0;index < MAX_NUMBER_OF_GPIOS; index++)
    {
      uGPIOIntData.aGPIOLUT[index] = UGPIOINT_NONE;  
    }

    /*
     * Initialize the uGPIO state table.
     * The interrupt configuration is fixed for the 10 configurations possible in hw for 10 direct connects.
     */
    for(index =0;index < MAX_NUMBER_OF_UGPIOS; index++)
    {
      uGPIOIntData.state[index].qurt_intr_id = UGPIOINT_INT_NONE;
      uGPIOIntData.state[index].gpio_intr_flags = 0;
      uGPIOIntData.state[index].nGPIO = UGPIOINT_NONE;  
      uGPIOIntData.state[index].nInterruptMask = (1<< index);
      uGPIOIntData.state[index].nAltIdx = 0xff;
      uGPIOIntData.nGPIOIntMask |= uGPIOIntData.state[index].nInterruptMask;
    }
        
    uGPIOIntData.nThreadID = 0;
    uGPIOIntData.nGPIOIntRegistrationMask = 0;

    /*
     * Spawn the IST here in init so it can wait for registration commands when the client registers.
     */
    nResult =  uGPIOInt_ConfigureIST();

    /* 
     * Acquire GPIO Manager
     */
    if( DalGPIOMgr_Attach( &uGPIOIntData.hGPIOMgr ) != DAL_SUCCESS )
    {
      return UGPIOINT_ERROR;
    }

    if (nResult == UGPIOINT_SUCCESS) 
    {
      uGPIOIntData.uGPIOInt_Init = 1;
    }
	
    if ( qurt_process_get_id()!= 0 )
    {
      uGPIOIntData.gpioint_qdi = qurt_qdi_open(GPIOIntQdiName,qurt_process_get_id());
      if(uGPIOIntData.gpioint_qdi < 0)
      {
        return UGPIOINT_ERROR;
      }
    }
    return nResult;
  }

  return UGPIOINT_SUCCESS;

} /* END uGPIOInt_Init */


/*===========================================================================

  FUNCTION      uGPIOInt_ConfigureIST

  DESCRIPTION   This function spawns a single IST thread for handling
                each GPIO interrupt.

  PARAMETERS
  Parameters : None. 

  DEPENDENCIES  None.

  RETURN VALUE  DALResult
  UGPIOINT_SUCCESS : If the IST was spawned successfully.
  UGPIOINT_ERROR : If the IST Spawning encountered an error.

  SIDE EFFECTS  None.


==========================================================================*/
static int32 uGPIOInt_ConfigureIST(void)
{
  int nResult;
  unsigned int StackSize;
  unsigned char * StackAddr;
 
  nResult = 0;

  /*
   * Set the IST name to UGPIOIST
   */
  snprintf(uGPIOIntData.aISTName, 
          (sizeof(unsigned char)*UGPIOIST_TASKNAME_SIZE), "UGPIOIST");

  /*
   *  Initialize any thread attribute objects needed by the interrupt service thread.
   */
  qurt_thread_attr_init (&uGPIOIntData.ThreadAttr);

  /*
   *  Initialize any thread attribute and signal objects needed by
   *  the interrupt service thread.
   */
  qurt_anysignal_init(&uGPIOIntData.ISTSignal);

  /*
   * 64 bit alignment of the stack ptr.
   */
  StackAddr = (unsigned char *)
              (((unsigned long)uGPIOIntData.aISTStack -1) & (~0x7)) + 0x8;

  StackSize = (unsigned int)( UGPIOINT_IST_STACK_SIZE - 
              ((unsigned long)StackAddr - 
              (unsigned long)uGPIOIntData.aISTStack) );

  /* 
   * To ensure that the entire stack (start to end) is 64-bit aligned
   * so the last odd bytes are dropped off.
   */
  StackSize &= ~(0x7);

  
  qurt_thread_attr_set_stack_size(&uGPIOIntData.ThreadAttr, 
                                  StackSize);
  qurt_thread_attr_set_stack_addr (&uGPIOIntData.ThreadAttr, 
                                   (void *)StackAddr);
  qurt_thread_attr_set_priority (&uGPIOIntData.ThreadAttr, 
                                (unsigned short)UGPIOINT_PRIORITY);

  qurt_thread_attr_set_tcb_partition(&uGPIOIntData.ThreadAttr,1); // This task should reside in TCM Memory for UGPIOInt Delivery.

  qurt_thread_attr_set_name(&uGPIOIntData.ThreadAttr, 
                            uGPIOIntData.aISTName);
   
  nResult = qurt_thread_create((qurt_thread_t *)&uGPIOIntData.nThreadID, 
                               &uGPIOIntData.ThreadAttr,
                               uGPIOInt_ISTMain,NULL);
   
  if (nResult == QURT_EFATAL) 
  {
    return UGPIOINT_ERROR;
  }

  return UGPIOINT_SUCCESS;

} /* END uGPIOInt_ConfigureIST */


/*==========================================================================

  FUNCTION      uGPIOInt_ConfigPDC

==========================================================================*/

int32
uGPIOInt_ConfigPDC
(
  uint32                    gpio, 
  uGPIOIntTriggerType       trigger,
  boolean                   bEnable,
  uint32                    nFlags,
  uint32*                   pnInt
)
{
  pdc_trigger_config       sConfig;
  uint32                   nIntId    = 0;
  uint32                   nAltIntId = 0;
  
  sConfig.drv_num = PDC_DRV0;

  /*
   * Determine the PDC interrupt configuration
   */
  switch (trigger)
  {

    case UGPIOINT_TRIGGER_DUAL_EDGE:
      sConfig.trig_type = TRIGGER_DUAL_EDGE;
      break;

    case UGPIOINT_TRIGGER_HIGH:
      sConfig.trig_type = TRIGGER_LEVEL_HIGH;
      break;

    case UGPIOINT_TRIGGER_LOW:
      sConfig.trig_type = TRIGGER_LEVEL_LOW;
      break;

    case UGPIOINT_TRIGGER_RISING:
      sConfig.trig_type = TRIGGER_RISING_EDGE;
      break;

    case UGPIOINT_TRIGGER_FALLING:
      sConfig.trig_type = TRIGGER_FALLING_EDGE;
      break;

    default:
      sConfig.trig_type = TRIGGER_LEVEL_HIGH;
      break;
  }

  /*
   * Inform PDC that the GPIO interrupt is enabled
   */
  if( PDC_SUCCESS == pdcGpio_config(gpio, &sConfig, &nIntId, &nAltIntId) )
  {
    *pnInt = SET_LOWER_16(nIntId) | SET_UPPER_16(nAltIntId);
    
    if ( bEnable )
    {
      pdcGpio_enable(gpio);
    }

    /*
     * Enable timestamping unit for this interrupt
     */
    #ifdef UGPIOTIMESTAMP_EN
    if ( nFlags & UGPIOINTF_TIMESTAMP_EN )
    {
      if( pdcTimestamp_enable(nIntId, TRUE) == PDC_TS_SUCCESS )
      {
        uGPIOIntData.state[UGPIOINT_IDX(gpio)].gpio_intr_flags |= UGPIOINTF_TIMESTAMP_EN;

        if ( nAltIntId != 0 )
        {
          pdcTimestamp_enable(nAltIntId, TRUE);
        }
      }
    }
    #endif /*UGPIOTIMESTAMP_EN*/

    return UGPIOINT_SUCCESS;
  }
  return UGPIOINT_ERROR;
} /* END GPIOIntr_ConfigMPMWakeup */


/*==========================================================================

   FUNCTION      uGPIOInt_RegisterInterrupt

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32
uGPIOInt_RegisterInterrupt( uint32 gpio,uGPIOIntTriggerType trigger,
                            uGPIOINTISR isr,uGPIOINTISRCtx param, uint32 nFlags)
{
  uint32 nInt = 0, nSignalMask = 0, sigs = 0;
  qurt_anysignal_t sAckSig;

  if (!VALID_UGPIO(gpio))
  {
    return UGPIOINT_ERROR;
  }
  if (trigger > UGPIOINT_TRIGGER_DUAL_EDGE)
  {
    return UGPIOINT_ERROR;
  }

  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);

  if ( DalGPIOMgr_LockGPIO( uGPIOIntData.hGPIOMgr, gpio, qurt_process_get_id() ) != DAL_SUCCESS )
  {
    DalGPIOMgr_ReleaseGPIO( uGPIOIntData.hGPIOMgr, gpio );
    qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
    return UGPIOINT_ERROR;
  }

  if ( UGPIOINT_SUCCESS != uGPIOInt_AssignInterruptIndex(gpio) )
  {
    DalGPIOMgr_ReleaseGPIO( uGPIOIntData.hGPIOMgr, gpio );
    qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
    return UGPIOINT_ERROR;
  }

  if ( UGPIOINT_SUCCESS != uGPIOInt_ConfigPDC(gpio, trigger, TRUE, nFlags, &nInt) )
  {
    DalGPIOMgr_ReleaseGPIO( uGPIOIntData.hGPIOMgr, gpio );
    qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
    return UGPIOINT_ERROR;
  }

  /* Update qurt interrupt ID */
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id = GET_LOWER_16(nInt);

  /* Configure flags based on what's supported */
  uGPIOInt_SetupFlags(gpio, nFlags);

  if ( UGPIOINT_SUCCESS != uGPIOInt_SetQURTTriggerConfig((uint32)uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id,trigger,gpio) )
  {
    DalGPIOMgr_ReleaseGPIO( uGPIOIntData.hGPIOMgr, gpio );
    qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
    return UGPIOINT_ERROR;
  }

  if((uGPIOIntData.state[UGPIOINT_IDX(gpio)].isr != NULL) &&
     (uGPIOIntData.state[UGPIOINT_IDX(gpio)].isr != isr))
  {
    /*
     * Restore interrupts and return an error.
     */
    DalGPIOMgr_ReleaseGPIO( uGPIOIntData.hGPIOMgr, gpio );
    qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
    return UGPIOINT_ERROR;
  }

  uGPIOIntData.state[UGPIOINT_IDX(gpio)].isr = isr;
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].isr_param = param; 

  /*
   * Update static GPIOInt map with Trigger 
   */ 
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].intr_trigger = (uint8)trigger;
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].gpio_intr_flags |= UGPIOINTF_REGISTERED;

  /*
   * If this is a dual edge triggered interrupt, we need to configure a second line
   */
  if ( trigger == UGPIOINT_TRIGGER_DUAL_EDGE )
  {
    if ( UGPIOINT_SUCCESS != uGPIOInt_AssignAlternateIndex(gpio, UGPIOINT_IDX(gpio), GET_UPPER_16(nInt)) ||
         UGPIOINT_SUCCESS != uGPIOInt_SetQURTTriggerConfig((uint32)uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].qurt_intr_id,trigger,gpio) )
    {
      uGPIOIntData.state[UGPIOINT_IDX(gpio)].isr = isr;
      uGPIOIntData.state[UGPIOINT_IDX(gpio)].isr_param = param; 
      uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id = UGPIOINT_INT_NONE;
      uGPIOIntData.state[UGPIOINT_IDX(gpio)].gpio_intr_flags = 0;
      uGPIOIntData.state[UGPIOINT_IDX(gpio)].nGPIO = UGPIOINT_NONE;  
      uGPIOIntData.state[UGPIOINT_IDX(gpio)].nAltIdx = 0xff;
      DalGPIOMgr_ReleaseGPIO( uGPIOIntData.hGPIOMgr, gpio );
      qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
      return UGPIOINT_ERROR;
    }
    if ( UGPIOINT_SUCCESS != uGPIOInt_SetQURTTriggerConfig((uint32)uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id,trigger,gpio) )
    {
      DalGPIOMgr_ReleaseGPIO( uGPIOIntData.hGPIOMgr, gpio );
      qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
      return UGPIOINT_ERROR;
    }
  }

  /*
   * Set registration mask
   */
  uGPIOIntData.nGPIOIntRegistrationMask |= uGPIOIntData.state[UGPIOINT_IDX(gpio)].nInterruptMask;
  nSignalMask = uGPIOIntData.state[UGPIOINT_IDX(gpio)].nInterruptMask;

  /*
   * Create Ack sig to get feedback, and assign it to the gpio entry
   */
  qurt_anysignal_init(&sAckSig);
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].pAckSig = &sAckSig;

  /*
   * Set registration mask for dual edge falling portion
   */
  if ( trigger == UGPIOINT_TRIGGER_DUAL_EDGE && UGPIOINT_VALID_ALT_IDX(gpio) )
  {
    uGPIOIntData.nGPIOIntRegistrationMask |= uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].nInterruptMask;
    nSignalMask |= uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].nInterruptMask;
    uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].pAckSig = &sAckSig;
  }

  /* 
   * An IST thread is already created.
   * It will only be restarted on a re registration.
   */
  qurt_anysignal_set(&uGPIOIntData.ISTSignal, nSignalMask);
  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  /*
   * Wait for feedback from the IST, no need to clear the signal from the structure 
   */
  while (nSignalMask != 0)
  {
    sigs = qurt_anysignal_wait(&sAckSig, nSignalMask);
    qurt_anysignal_clear(&sAckSig, sigs);
    nSignalMask &= ~sigs;
  }

  qurt_anysignal_destroy(&sAckSig);

  return UGPIOINT_SUCCESS;

} /* END uGPIOInt_RegisterInterrupt */


/*==========================================================================

   FUNCTION      uGPIOInt_DeregisterInterrupt

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32
uGPIOInt_DeregisterInterrupt(uint32  gpio)
{
  int nStatus;

  /*
   * if the GPIO is not configured by UGPIO module then we will
   * not have any valid index etc. Hence its better to abandon
   * any invalid accesses from  this point.
   */
  if ((!VALID_UGPIO(gpio))||(!VALID_UGPIO_IDX(gpio)))
  {
    return UGPIOINT_ERROR;
  }

  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);

  DalGPIOMgr_ReleaseGPIO( uGPIOIntData.hGPIOMgr, gpio );
  
  /*
   * Clear out the handler and remove the event. Here the direct connect 
   * interrupt handler is just saved for error checking purposes as it is 
   * largely handled by the Main GPIO interrupt controller.
   */

  if ( (uGPIOIntTriggerType)uGPIOIntData.state[UGPIOINT_IDX(gpio)].intr_trigger == UGPIOINT_TRIGGER_DUAL_EDGE && 
       UGPIOINT_VALID_ALT_IDX(gpio) )
  {
    /* 
     * Deregister IST from QURT so it can stop waiting for an interrupt.
     * Do this before clearing out the structure to assure that we don't 
     * access bad data. 
     */
    nStatus = qurt_interrupt_deregister(uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].qurt_intr_id); 

    uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].isr = NULL;
    uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].isr_param = 0;
    uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].intr_trigger = 0;

    uGPIOInt_ClearFlags(UGPIOINT_ALT_IDX(gpio));

    /*
     * Error trying to deregister dual edge interrupt!
     */
    if (QURT_EOK != nStatus) 
    { 
      qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock);
      return UGPIOINT_ERROR;
    } 

    /*
     * Clear out falling edge of dual edge interrupt in timestamp unit
     */
    #ifdef UGPIOTIMESTAMP_EN
    if ( uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].gpio_intr_flags & UGPIOINTF_TIMESTAMP_EN )
    {
      pdcTimestamp_enable(uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].qurt_intr_id, FALSE);
    }
    #endif /*UGPIOTIMESTAMP_EN*/

    uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].qurt_intr_id = UGPIOINT_INT_NONE;
    uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].nGPIO = UGPIOINT_NONE;

    /* Clear registration flag */
    uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].gpio_intr_flags &= ~UGPIOINTF_REGISTERED;

    /* Clear index */
    uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].nAltIdx = 0xff;
  }

  /* 
   * Deregister IST from QURT so it can stop waiting for an interrupt. 
   * Do this before clearing out the structure to assure that we don't 
   * access bad data. 
   */
  nStatus = qurt_interrupt_deregister(uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id); 

  uGPIOIntData.state[UGPIOINT_IDX(gpio)].isr = NULL;
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].isr_param = 0;
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].intr_trigger = 0;
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].nAltIdx = 0xff;

  uGPIOInt_ClearFlags(UGPIOINT_IDX(gpio));

  /*
   * Clear out interrupt in timestamp unit
   */
  #ifdef UGPIOTIMESTAMP_EN
  if ( uGPIOIntData.state[UGPIOINT_IDX(gpio)].gpio_intr_flags & UGPIOINTF_TIMESTAMP_EN )
  {
    uGPIOIntData.state[UGPIOINT_IDX(gpio)].gpio_intr_flags &= ~UGPIOINTF_TIMESTAMP_EN;
    pdcTimestamp_enable(uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id, FALSE);
  }
  #endif /*UGPIOTIMESTAMP_EN*/

  /*
   * This will deconfigure at PDC and disable
   */
  pdcGpio_unconfig(gpio);

  uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id = UGPIOINT_INT_NONE;

  if (QURT_EOK != nStatus) 
  { 
    qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock);
    return UGPIOINT_ERROR;
  } 

  /* Clear registration flag */
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].gpio_intr_flags &= ~UGPIOINTF_REGISTERED;
  uGPIOIntData.state[UGPIOINT_IDX(gpio)].nGPIO = UGPIOINT_NONE;
  uGPIOIntData.aGPIOLUT[gpio] = UGPIOINT_NONE;

  /*
   * Restore interrupts
   */
  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  return UGPIOINT_SUCCESS;

} /* END uGPIOInt_DeregisterInterrupt */


/*=============================================================================
  FUNCTION      uGPIOInt_EnableInterruptEx
=============================================================================*/
int32 
uGPIOInt_EnableInterruptEx
(
  
  uint32  gpio,
  boolean bEnable
)
{
  int32 nRetVal;
  if ((!VALID_UGPIO(gpio))||(!VALID_UGPIO_IDX(gpio)))
  {
    return UGPIOINT_ERROR;
  }

  if ( bEnable == TRUE )
  {
    nRetVal = uGPIOInt_EnableInterrupt(gpio);
    pdcGpio_enable(gpio);
  }
  else
  {
    nRetVal = uGPIOInt_DisableInterrupt(gpio);
    pdcGpio_disable(gpio);
  }

  return nRetVal;
}

