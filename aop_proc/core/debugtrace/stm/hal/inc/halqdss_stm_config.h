#ifndef HALQDSS_STM_CONFIG_H
#define HALQDSS_STM_CONFIG_H
/*=========================================================================

                       STM HAL CONFIGURE

GENERAL DESCRIPTION 
   Contains  functionalities to access the 4KB of memory mapped STM register set 
that are connected through the APB bus. The STM hal is used by the STM 
configuration driver in order to enable the generation of software instrumented 
trace.  Functionalites exposed by the APIs include initializtion, wrappers to  
configure/Control, read/write into hardware registers.

INITIALIZATION AND SEQUENCING REQUIREMENTS 
   STM HAL is used by the STM cofiguration driver only. The only pre-condition 
to its usage is that it be initialized through HAL_qdss_stm_Init().
   
    
  

      Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/* 
Function Name: HAL_qdss_stm_HalConfigInit
    
Input:
   nBaseAddr: Base address of the STM hardware block to be used for computing 
register offsets.
 
Description: 
   Invoked by the driver to initialize the STM HAL module. The top level client 
must provide nBaseAddr identifying the register base address to be used in 
computing offsets. The HAL module SHOULD NOT reference the hardware by using 
physical address. 
   Internally the function obtains the base address of the STM hardware block 
and saves it to compute the register offsets.This API does NOT configure the 
STM hardware. Configuration of the STM block is the responsibility of the client 
driver which uses the HAL.
 
Return: None 
 
*/ 
void HAL_qdss_stm_HalConfigInit(uint32 nBaseAddr);



/*
Function Name: HAL_qdss_stm_SetTimeStampFreq

Input: 
   nFreq: Integer value of the frequency value to be used in the FREQ/FREQ_TS 
STP packets that are generated during every sync period when TSEN is enabled. 
Writing to this register also causes frequency packets to be generated 
indicating the new frequency.
*/
extern void  HAL_qdss_stm_SetTimeStampFreq(uint32 nFreq);


  
/* 
Function Name: HAL_qdss_stm_AccessLock 
 
Description: 
   Allows write access to the STM 4KB memory mapped registers. Access to the STM 
harware block is required in order to configure the STM hardware.
 
Return: None 
*/ 
extern void  HAL_qdss_stm_AccessUnLock(void);

/* 
Function Name: HAL_qdss_stm_AccessUnlock 
 
Description: 
   Prevents write access to the STM 4KB memory mapped registers. Access to the 
STM harware block is required in order to configure the STM hardware.
 
Return: None 
*/ 
extern void  HAL_qdss_stm_AccessLock(void);



extern boolean HAL_qdss_stm_AuthStatusNSNID(void);

#endif
