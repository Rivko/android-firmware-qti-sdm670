
#ifndef QURT_CYCLES_H
#define QURT_CYCLES_H 1
/**
  @file qurt_cond.h
  @brief  Prototypes of Kernel pcycle API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

	/*=====================================================================
	 Functions
	======================================================================*/
	 
/*======================================================================*/

/**@ingroup func_qurt_profile_reset_idle_pcycles
  @xreflabel{hdr:qurt_profile_reset_idle_pcycles}
  Sets the per-hardware-thread idle cycle counts to zero. 

  @return 
  None. 
		 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_profile_reset_idle_pcycles (void);
	 
/*======================================================================*/
/**@ingroup func_qurt_profile_get_thread_pcycles
  @xreflabel{hdr:qurt_profile_get_thread_pcycles}
  Gets the count of the running processor cycles for the current thread.\n
  Returns the current running processor cycle count for the current QuRT thread.

  @return 
  Integer -- Running processor cycle count for current thread.
		 
  @dependencies 
  None.
*/
/* ======================================================================*/
unsigned long long int qurt_profile_get_thread_pcycles(void);

/*======================================================================*/
/**@ingroup func_qurt_profile_get_thread_tcycles
  Gets the count of running thread cycles for the current thread.\n
  Returns the current running thread cycle count for the current QuRT thread.
	
  @return
  Integer -- Running thread cycle count for current thread.
		 
  @dependencies
  None.
*/
/* ======================================================================*/
unsigned long long int qurt_profile_get_thread_tcycles(void);
	
/*======================================================================*/
/**@ingroup func_qurt_get_core_pcycles
  @xreflabel{hdr:qurt_get_core_pcycles}
  Gets the count of core processor cycles executed.\n
  Returns the current number of running processor cycles executed since the Hexagon
  processor was last reset.

  This value is based on the hardware core clock, which varies in speed according to the
  processor clock frequency.

  @note1hang Because the hardware core clock stops running when the processor shuts
             down (due to all of the hardware threads being idle), the cycle values returned
             by this operation should be treated as relative rather than absolute.

  @note1cont Thread cycle counts are valid only in the V4 Hexagon processor version.

  @return 
  Integer -- Current count of core processor cycles.
		 
  @dependencies
  None.
*/
/* ======================================================================*/
unsigned long long int qurt_get_core_pcycles(void);

/*======================================================================*/
/**@ingroup func_qurt_profile_get_idle_pcycles
  Gets the counts of idle processor cycles. \n
  Returns the current idle processor cycle counts for all hardware threads.

  This operation accepts a pointer to a user-defined array, and writes to the array the current
  idle cycle count for each hardware thread.

  Each count value represents the number of processor cycles that have elapsed on the
  corresponding hardware thread while that thread has been in Wait mode.\n

  @note1hang This operation does not return the idle cycles that occur when the Hexagon
             processor shuts down (due to all of the hardware threads being idle). 
	
  @param[out] pcycles  User array [0..MAX_HW_THREADS-1] where the function 
                       stores the current idle cycle count values.

  @return
  None.
		 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_profile_get_idle_pcycles (unsigned long long *pcycles);

/*======================================================================*/
/**@ingroup func_qurt_profile_get_threadid_pcycles
  Gets the counts of the running processor cycles for the specified QuRT thread.\n
  Returns the current per-hardware-thread running cycle counts for the specified QuRT
  thread.

  Each count value represents the number of processor cycles that have elapsed on the
  corresponding hardware thread while that thread has been scheduled for the specified
  QuRT thread.

  @param[in]   thread_id  Thread identifier.
  @param[out]  pcycles    Pointer to a user array [0..MAX_HW_THREADS-1] where the function
                          stores the current running cycle count values.
	
  @return 				
  None. 
		 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_profile_get_threadid_pcycles (int thread_id, unsigned long long  *pcycles);

/*======================================================================*/
/**@ingroup func_qurt_profile_reset_threadid_pcycles
  @xreflabel{hdr:qurt_profile_reset_threadid_pcycles}
  Sets the per-hardware-thread running cycle counts to zero for the specified QuRT thread.

  @param[in]  thread_id Thread identifier.
	
  @return 
  None. 
		 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_profile_reset_threadid_pcycles (int thread_id);

/*======================================================================*/
/**@ingroup func_qurt_profile_enable
  @xreflabel{hdr:qurt_profile_enable}
  Enables profiling.\n
  Enables or disables cycle counting of the running and idle processor cycles.
  Profiling is disabled by default. \n

  @note1hang Enabling profiling does not automatically reset the cycle counts -- this must be
             done explicitly by calling the reset operations before starting cycle counting.
	
  @param[in] enable  Profiling. Values: \n
                     - 0 -- Disable profiling \n
                     - 1 -- Enable profiling @tablebulletend
	
  @return 
  None. 
		 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_profile_enable (int enable);


#endif

