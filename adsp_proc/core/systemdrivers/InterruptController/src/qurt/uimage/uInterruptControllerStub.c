#include "com_dtypes.h"
#include "uInterruptController.h"


int32 uInterruptController_UnRegister( uint32 nInterruptID)  
{
  return UINTERRUPT_SUCCESS;
}  
   
int32
uInterruptController_RegisterInterrupt( uint32 nInterruptID, uint32 trigger,
uIRQ isr,uIRQCtx param, uint32 nFlags)
{
  return UINTERRUPT_SUCCESS;
}
                
int32 uInterruptController_IsInterruptPending( uint32 nInterruptID, uint32 *  state)
{
  return UINTERRUPT_SUCCESS;
}                
                    
int32 uInterruptController_TriggerInterrupt(uint32 nInterruptID)
{
  return UINTERRUPT_SUCCESS;
}                                    
          
int32 uInterruptController_Init(void)
{
  return UINTERRUPT_SUCCESS;
}                                                               

int32
uInterruptController_Enable(uint32 nInterruptID, boolean bEnable)
{
  return UINTERRUPT_SUCCESS;
}


