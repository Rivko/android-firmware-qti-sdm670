#ifndef QURT_TRACE_H
#define QURT_TRACE_H
/**
  @file qurt_trace.h 
  @brief  Prototypes of system call tracing helpers API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



/*=============================================================================
												GLOBAL VARIABLES
=============================================================================*/
/** @cond RTOS_user_guide_supplement */
/** @addtogroup etm_macros
@{ */
/* ETM trace types. */
#define QURT_ETM_TYPE_PC_ADDR                           (1<<0) /**< PC address.*/
#define QURT_ETM_TYPE_MEMORY_ADDR                       (1<<1) /**< Memory address. */
#define QURT_ETM_TYPE_TESTBUS                           (1<<2) /**< Test bus. */
#define QURT_ETM_TYPE_CYCLE_ACCURATE                    (1<<3) /**< Cycle accurate. */
#define QURT_ETM_TYPE_CYCLE_COARSE                      (1<<4) /**< Cycle coarse. */
#define QURT_ETM_TYPE_PC_AND_MEMORY_ADDR                (QURT_ETM_TYPE_PC_ADDR|QURT_ETM_TYPE_MEMORY_ADDR) /**< PC and memory address. */
#define QURT_ETM_TYPE_PC_ADDR_AND_TESTBUS               (QURT_ETM_TYPE_PC_ADDR|QURT_ETM_TYPE_TESTBUS) /**< PC address and test bus. */
#define QURT_ETM_TYPE_MEMORY_ADDR_AND_TESTBUS           (QURT_ETM_TYPE_MEMORY_ADDR|QURT_ETM_TYPE_TESTBUS) /**< Memory address and test bus.*/
#define QURT_ETM_TYPE_PC_AND_MEMORY_ADDR_AND_TESTBUS    (QURT_ETM_TYPE_PC_ADDR|QURT_ETM_TYPE_MEMORY_ADDR|QURT_ETM_TYPE_TESTBUS) /**< PC, memory address, and test bus. */

/* ETM routes. */
#define QURT_ETM_ROUTE_TO_QDSS      0 /**< ETM route to QDSS. */
#define QURT_ETM_ROUTE_TO_Q6ETB     1 /**< ETM route to Q6ETB. */

/* ETM filters. */
#define QURT_ETM_TRACE_FILTER_ALL_DEFAULT   0       /**< Filter all as default. */
#define QURT_ETM_TRACE_FILTER_HNUM0         (1<<0)  /**< Filter HNUM0. */    
#define QURT_ETM_TRACE_FILTER_HNUM1         (1<<1)  /**< Filter HNUM1. */     
#define QURT_ETM_TRACE_FILTER_HNUM2         (1<<2)  /**< Filter HNUM2. */     
#define QURT_ETM_TRACE_FILTER_HNUM3         (1<<3)  /**< Filter HNUM3. */  
#define QURT_ETM_TRACE_FILTER_ALL           (QURT_ETM_TRACE_FILTER_HNUM0 | QURT_ETM_TRACE_FILTER_HNUM1 | QURT_ETM_TRACE_FILTER_HNUM2 | QURT_ETM_TRACE_FILTER_HNUM3)
#define QURT_ETM_TRACE_FILTER_CLUSTER0      (1<<6)  /**< Filter Trace Cluster0 Address */  
#define QURT_ETM_TRACE_FILTER_CLUSTER1      (1<<7)  /**< Filter Trace Cluster1 Address */  

/* ETM Enable flags */
#define QURT_ETM_OFF                0  /**< ETM off. */
#define QURT_ETM_ON                 1  /**< ETM on. */

/* ETM setup return values */
#define QURT_ETM_SETUP_OK                   0 /**< ETM setup OK. */
#define QURT_ETM_SETUP_ERR                  1 /**< ETM setup error. */

/* ETM breakpoint types */
#define QURT_ETM_READWRITE_BRKPT            0 /**< ETM read/write breakpoint. */
#define QURT_ETM_READ_BRKPT                 1 /**< ETM read breakpoint. */
#define QURT_ETM_WRITE_BRKPT                2 /**< ETM write breakpoint. */
#define QURT_ETM_BRKPT_INVALIDATE           3 /**< Invalidate breakpoint. */

/* ATB status flags */
#define QURT_ATB_OFF                        0  /**< ATB off. */
#define QURT_ATB_ON                         1  /**< ATB on. */


/** @} */ /* end_addtogroup etm_macros */
/** @endcond */

extern char qurt_trace_use_pcycles;

/**STM trace information. */
typedef struct qurt_stm_trace_info {
   /** @cond */
   unsigned int stm_port_addr[6]; /*STM port address where trace data needs to be written to*/
   unsigned int thread_event_id; /*Event ID for context switches*/
   unsigned int interrupt_event_id; /*Event ID for interrupts */
   unsigned int marker; /* Marker value that needs to be written at the beginning of the trace */
   /** @endcond */
} qurt_stm_trace_info_t;

/*=============================================================================
												GLOBAL FUNCTIONS
=============================================================================*/


/**@ingroup func_qurt_trace_get_marker
  Gets the kernel trace marker.\n
  Returns the current value of the kernel trace marker.
  The marker consists of a hardware thread identifier and an index into the kernel trace
  buffer. The trace buffer records various kernel events.

  @note1hang This function is used with qurt_trace_changed() to
             determine whether certain kernel events occurred in a block of code.

  @return
  Integer -- Kernel trace marker.

  @dependencies
  None.
*/
unsigned int qurt_trace_get_marker(void);

/**@ingroup func_qurt_trace_changed  
  Determines whether specific kernel events have occurred. \n
  Returns a value indicating whether the specified kernel events have been recorded in the
  kernel trace buffer since the specified kernel trace marker was obtained.

  The prev_trace_marker parameter specifies a kernel trace marker that was obtained by calling 
  qurt_trace_get_marker().
  For more information on the mask value, refer to the description of the trace_mask element in 
  @xhyperref{80VB41992,80-VB419-92}. \n

  @note1hang This function is used with qurt_trace_get_marker to determine whether
             certain kernel events occurred in a block of code.\n
  @note1cont This function cannot determine whether a specific kernel event type has
             occurred unless that event type has been enabled in the trace_mask element
             of the system configuration file. \n
  @note1cont QuRT supports the recording of interrupt and context switch events only (such as
             a trace_mask value of 0x3).

  @param[in] prev_trace_marker Previous kernel trace marker.
  @param[in] trace_mask        Mask value indicating the kernel events to check for.

  @returns
  1 -- Kernel events of the specified type have occurred since the
       specified trace marker was obtained.\n
  0 -- No kernel events of the specified type have occurred since the
       specified trace marker was obtained.

  @dependencies
  None.
*/
int qurt_trace_changed(unsigned int prev_trace_marker, unsigned int trace_mask);

/*=============================================================================
												CONSTANTS AND MACROS
=============================================================================*/
/** @addtogroup function_tracing_macro
@{ */
#ifndef QURT_DEBUG 
#define QURT_TRACE(str, ...) __VA_ARGS__
  /**< Function tracing is implemented with a debug macro (QURT_TRACE), which
       optionally generates printf statements both before and after every function call that is
       passed as a macro argument. 

       For example, the following macro call in the source code:
       @code
       QURT_TRACE(myfunc, my_func(33))
       
       @endcode
       generates the following debug output:
       @code
       myfile:nnn: my_func >>> calling my_func(33)
       myfile:nnn: my_func >>> returned my_func(33)
       @endcode
       The debug output includes the source file and line number of the function call, along with
       the text of the call itself.

       The debug output is generated using the library function qurt_printf.
       The symbol QURT_DEBUG controls generation of the debug output. If this symbol is
       not defined, function tracing is not generated.\n
       @note1hang The debug macro is accessed through the QuRT API header file. 
        */
#else
#define QURT_TRACE(str, ...) \
	do { \
		qurt_printf("%s:%d: %s: >>> calling %s\n",__FILE__,__LINE__,str,#__VA_ARGS__); \
		__VA_ARGS__; \
		qurt_printf("%s:%d: %s: <<< %s returned\n",__FILE__,__LINE__,str,#__VA_ARGS__); \
	} while (0);
#endif
/** @} */ /* end_addtogroup function_tracing_macro */


/**@ingroup func_qurt_etm_set_config
  Sets the configuration for the ETM. This operation specifies the types of program information 
  to trace, the destination of the trace information, and various filters to limit the amount 
  of trace information generated. This function must be called before enabling the ETM, 
  or after stopping the ETM.  

  @returns
    #QURT_ETM_SETUP_OK -- Success. \n
    #QURT_ETM_SETUP_ERR -- Failure.

  @dependencies
  None.
*/
unsigned int qurt_etm_set_config(unsigned int type, unsigned int route, unsigned int filter);


/**@ingroup func_qurt_etm_enable
  Enables or disables the ETM trace. ETM is disabled by default, and must be configured before it is enabled.

  The default ETM configuration after system bootup is of type #QURT_ETM_TYPE_PC_ADDR, route #QURT_ETM_ROUTE_TO_QDSS, 
  and filter #QURT_ETM_TRACE_FILTER_ALL. These values are applied to qurt_etm_enable() if qurt_etm_set_config() is 
  not called first.
  
  @param[in] enable_flag Values: \n
                         #QURT_ETM_ON \n
						 #QURT_ETM_OFF  
      
  @returns
    #QURT_ETM_SETUP_OK -- Success. \n
    #QURT_ETM_SETUP_ERR -- Failure

  @dependencies
  None.
*/
unsigned int qurt_etm_enable(unsigned int enable_flag);


/**@ingroup func_qurt_etm_testbus_set_config
  Sets configuration data for the Hexagon ETM test bus.

  @note1hang The ETM test bus design and configuration can be changed per Hexagon chipset.

  @param[in] cfg_data    Configuration data.

  @returns
  #QURT_ETM_SETUP_OK -- Success. \n
  #QURT_ETM_SETUP_ERR -- Failure.

  @dependencies
  None.
*/
unsigned int qurt_etm_testbus_set_config(unsigned int cfg_data);


/**@ingroup func_qurt_etm_set_breakpoint
  Sets a breakpoint at the specified memory access.
  When the breakpoint is triggered, the ETM unit generates an ETM interrupt. 
  The breakpoint can be configured to apply only to read, write, or read/write accesses,
  or when the access involves a specific data value. 
 
  @note1hang The ETM interrupt handler raises an NMI exception after receiving the ETM interrupt.

  @note1hang The maximum number of breakpoints supported is limited by the ETM hardware.

  @param[in] type  Memory access type. Values: \n
           #QURT_ETM_READWRITE_BRKPT  \n
           #QURT_ETM_READ_BRKPT    \n
           #QURT_ETM_WRITE_BRKPT   \n
           #QURT_ETM_BRKPT_INVALIDATE 
  @param[in] address   ETM breakpoint address
  @param[in] data      Memory compare value. If the data value stored at the specified memory 
                       address matches the compare value, the breakpoint is triggered.
                       For every bit in the mask parameter that is set to 1, the corresponding
                       bit in the compare value must be set to 0. 
  @param[in] mask      Memory compare mask. A 32-bit mask value that limits the memory compare 
                       operation to the specified bits. For every bit in the mask value that is 
					   set to 1, the corresponding bit in the compare value parameter must be set 
					   to 0, and the bit is ignored during the memory compare operation. 
                       When the mask value is set to 0xffffffff, no memory compare operation
                       is performed as part of the breakpoint test.

   @returns
   #QURT_ETM_SETUP_OK -- Success. \n
   #QURT_ETM_SETUP_ERR -- Failure

   @dependencies
   None.
*/
 
unsigned int qurt_etm_set_breakpoint(unsigned int type, unsigned int address, unsigned int data, unsigned int mask);  
 

/**@ingroup func_qurt_etm_set_breakarea
  Sets a breakpoint for the specified memory area. When the breakpoint is triggered, 
  the ETM unit generates an ETM interrupt. The breakpoint can be configured to apply 
  only to read, write, or read/write accesses, or when a specific number of accesses 
  has been performed to the memory area. 

  @note1hang The ETM interrupt handler raises an NMI exception after receiving the ETM interrupt.

  @note1hang The maximum number of area breakpoints supported is limited by the ETM hardware.

  @param[in] type  Memory access type. Values: \n
           #QURT_ETM_READWRITE_BRKPT  \n
           #QURT_ETM_READ_BRKPT  \n
           #QURT_ETM_WRITE_BRKPT \n  
           #QURT_ETM_BRKPT_INVALIDATE 
  @param[in] start_address   Start address of breakpoint memory area.
  @param[in] end_address     End address of breakpoint memory area.
  @param[in] count           Number of memory accesses performed to memory area before the breakpoint is triggered. 

  @returns
  #QURT_ETM_SETUP_OK -- Success. \n
  #QURT_ETM_SETUP_ERR -- Failure

  @dependencies
  None.
*/
 
unsigned int qurt_etm_set_breakarea(unsigned int type, unsigned int start_address, unsigned int end_address, unsigned int count);


/**@ingroup func_qurt_etm_set_atb
  Set ATB bus state, to notify Qurt that the ATB bus is actively enabled or disabled.
  Qurt will perform the corresponding actions at low power management.
  
  @param[in] flag Values: \n
                         #QURT_ATB_ON \n
						 #QURT_ATB_OFF  
      
  @returns
    #QURT_ETM_SETUP_OK  -- Success. \n
    #QURT_ETM_SETUP_ERR -- Failure

  @dependencies
  None.
*/
unsigned int qurt_etm_set_atb(unsigned int flag);


/**@ingroup func_qurt_stm_trace_set_config
  Setup a STM port for tracing events.\n

  @datatypes
  #qurt_stm_trace_info_t \n

  @param[in]  stm_config_info Pointer to the STM trace information used to set up the trace
              in the kernel.
			  The strucure needs to have the following:
			  - One port address per HW thread
			  - Event ID for context switches
			  - Event ID for interrupt tracing
			  - Header/Marker to identify the beginning of the trace.

  @return
  QURT_EOK -- Success. \n
  QURT_EINVALID -- Failure. This could be because the passed port address is not in the page table.

  @dependencies
  None.
 */
unsigned int qurt_stm_trace_set_config(qurt_stm_trace_info_t *stm_config_info);

#endif /* QURT_TRACE_H */
