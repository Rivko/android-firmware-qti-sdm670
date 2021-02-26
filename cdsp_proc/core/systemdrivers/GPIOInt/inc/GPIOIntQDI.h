#ifndef __GPIOIntQdi_H
#define __GPIOIntQdi_H
/*===========================================================================

  G P I O   I N T E R R U P T   C O N T R O L L E R  Q D I   W R A P P E R  

DESCRIPTION
  This modules contains the Qurt Driver Invocation (QDI) layer that interfaces
  with the driver in the kernel on behalf of the user clients in the user domain.

REFERENCES

       Copyright (c) 2012-2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/inc/GPIOIntQDI.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "qurt_qdi_constants.h"
#include "DDIGPIOInt.h"
/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/
#define GPIOINT_QDI_REGISTER_SUMMARY_TASK        (0+QDI_PRIVATE)
#define GPIOINT_QDI_RECEIVE_SUMMARY_SIGNAL       (1+QDI_PRIVATE)
#define GPIOINT_QDI_ACK_SUMMARY_SIGNAL           (2+QDI_PRIVATE)

#define GPIOINT_QDI_ACK_SIGNAL                    0x1
#define GPIOINT_QDI_INTERRUPT_SIGNAL              0x10
#define GPIOINT_QDI_TASK_START                    0x00008000
#define GPIOINT_QDI_TASK_STOP                     0x00010000  
#define GPIOINT_QDI_TRUE                          0x00010
#define GPIOINT_QDI_FALSE                         0x0               
#define GPIOINT_QDI_ERROR                         -1
#define GPIOINT_QDI_SUCCESS                       0

#define GPIOIntQdiName                            "/dev/GPIOIntQdi"

#define GPIOINTQDI_SUMMARY_SHIFT 16
#define GPIOINTQDI_SUMMARY_MASK  0xFFF

#define GPIOINT_QDI_SUMMARY_CLR_BMSK ( GPIOINT_QDI_INTERRUPT_SIGNAL | \
                                       ( GPIOINTQDI_SUMMARY_MASK <<   \
                                         GPIOINTQDI_SUMMARY_SHIFT ) )

#define GPIOINTQDI_SET_SUMMARY_SIG( nGPIO ) \
          ( ( ( nGPIO & GPIOINTQDI_SUMMARY_MASK ) \
              << GPIOINTQDI_SUMMARY_SHIFT ) \
            | GPIOINT_QDI_INTERRUPT_SIGNAL )

#define GPIOINTQDI_GET_SUMMARY_SIG( nSig ) \
          ( ( ( nSigs >> GPIOINTQDI_SUMMARY_SHIFT ) \
              & GPIOINTQDI_SUMMARY_MASK ) )

static __inline DALResult
GPIOInt_DisableGPIOInterrupt(DalDeviceHandle * _h, uint32 gpio)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, DisableInterrupt ), _h, gpio);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->DisableGPIOInterrupt(DALDEVICE_LOCAL_CLIENTID, _h, gpio);
}

static __inline DALResult
GPIOInt_EnableGPIOInterrupt(DalDeviceHandle * _h, uint32 gpio)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, EnableInterrupt ), _h, gpio);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->EnableGPIOInterrupt(DALDEVICE_LOCAL_CLIENTID, _h, gpio);
}

void GPIOIntQDI_Init(void);

int GPIOIntQDI_WaitOnAckSummarySignal( uint32 nPD );

int GPIOIntQDI_SetSummarySignal( uint32 nPD, uint32 nGPIO );

#endif /*___GPIOIntQdi_H*/
