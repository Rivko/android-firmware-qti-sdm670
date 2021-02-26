#ifndef QURT_PMU_H
#define QURT_PMU_H
/**
  @file qurt_pmu.h 
  @brief  Prototypes of pipe interface API.  
	 A pipe or message queue will block if too full (send) or empty (receive).
	 Unless a nonblocking option is used, all datagrams are 64 bits.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_pmu_set
  Sets the value of the specified PMU register.

  @note1hang Setting PMUEVTCFG automatically clears the PMU registers PMUCNT0
             through PMUCNT3.
 
  @param[in] reg_id   PMU register. Values: \n
              - #QURT_PMUCNT0  \n
              - #QURT_PMUCNT1  \n
              - #QURT_PMUCNT2  \n
              - #QURT_PMUCNT3  \n
              - #QURT_PMUCFG  \n
              - #QURT_PMUEVTCFG  \n
              - #QURT_PMUCNT4  \n
              - #QURT_PMUCNT5  \n
              - #QURT_PMUCNT6  \n
              - #QURT_PMUCNT7  \n
              - #QURT_PMUEVTCFG1  @tablebulletend             
 
  @param[in] reg_value  Register value.
 
  @return
  None.
   
  @dependencies
  None.
 */
void qurt_pmu_set (int reg_id, unsigned int reg_value);
 
/**@ingroup func_qurt_pmu_get
  Gets the PMU register.\n
  Returns the current value of the specified PMU register.

  @param[in] reg_id   PMU register. Values: \n
               - #QURT_PMUCNT0  \n
               - #QURT_PMUCNT1  \n
               - #QURT_PMUCNT2 \n
               - #QURT_PMUCNT3  \n
               - #QURT_PMUCFG   \n
               - #QURT_PMUEVTCFG  \n
               - #QURT_PMUCNT4  \n
               - #QURT_PMUCNT5  \n
               - #QURT_PMUCNT6  \n
               - #QURT_PMUCNT7  \n
               - #QURT_PMUEVTCFG1  @tablebulletend           
 
  @return
   Integer -- Current value of the specified PMU register.

  @dependencies
  None.
 */
unsigned int  qurt_pmu_get (int reg_id);
 
/**@ingroup func_qurt_pmu_enable
  Enables or disables the Hexagon processor performance monitor unit (PMU).
  Profiling is disabled by default. \n
  @note1hang Enabling profiling does not automatically reset the count registers -- this must
            be done explicitly before starting event counting.
 
  @param[in] enable Performance monitor. Values: \n
                    - 0 -- Disable performance monitor \n
                    - 1 -- Enable performance monitor @tablebulletend
 
  @return 
  None.

  @dependencies
  None.
 */
void qurt_pmu_enable (int enable);

#endif /* QURT_PMU_H */
