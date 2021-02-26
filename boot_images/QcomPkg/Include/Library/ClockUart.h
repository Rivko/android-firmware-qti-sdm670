#ifndef __CLOCKUART_H__
#define __CLOCKUART_H__
/*==================================================================================================

FILE: clockUart.h

DESCRIPTION: UEFI driver for UARTDM/UARTBAM Clock initialzation
          Copyright (c) 2017 Qualcomm Technologies Inc, All Rights Reserved
              Qualcomm Technologies Inc Proprietary and Confidential.

==================================================================================================*/
/*==================================================================================================
                                        TYPES DEFINITION
==================================================================================================*/
/**
@brief List of QUPv3 clocks that the clock driver supports.
*/
typedef enum
{
   CLK_QUPV3_WRAP0_S0 = 0,
   CLK_QUPV3_WRAP0_S1,
   CLK_QUPV3_WRAP0_S2,
   CLK_QUPV3_WRAP0_S3,
   CLK_QUPV3_WRAP0_S4,
   CLK_QUPV3_WRAP0_S5,
   CLK_QUPV3_WRAP0_S6,
   CLK_QUPV3_WRAP0_S7,
   CLK_QUPV3_WRAP1_S0,
   CLK_QUPV3_WRAP1_S1,
   CLK_QUPV3_WRAP1_S2,
   CLK_QUPV3_WRAP1_S3,
   CLK_QUPV3_WRAP1_S4,
   CLK_QUPV3_WRAP1_S5,
   CLK_QUPV3_WRAP1_S6,
   CLK_QUPV3_WRAP1_S7,
} Clock_qupv3_instance_type;


/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
/*==================================================================================================

FUNCTION: configure_clocks

DESCRIPTION:
 This function performs all necessary clock configuration for UART enablement.
 Allow to set frequency for an instance.

PARAMETERS:
   qupv3_instance:  qupv3 wrap uart instance
   nFrequencyHz:    Frequency(Hz) will be set for uart instances.

RETURN:
   None

RESTRIC:
   This funtion should be called only one time to init uart Clocks

==================================================================================================*/
void configure_clocks(Clock_qupv3_instance_type qupv3_instance, UINT32 nFrequencyHz);

#endif  // __CLOCKUART_H__
