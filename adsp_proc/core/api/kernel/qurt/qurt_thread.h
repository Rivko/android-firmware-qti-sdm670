#ifndef QURT_THREAD_H
#define QURT_THREAD_H
/**
  @file qurt_thread.h 
  @brief  Prototypes of Thread API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



#include <string.h>
#include "qurt_pmu.h"


/*=============================================================================
												CONSTANTS AND MACROS
=============================================================================*/


/*
  Bitmask configuration is for selecting DSP hardware threads. 
  To select all the hardware threads, using QURT_THREAD_CFG_BITMASK_ALL 
  and the following: \n
  - For QDSP6 V2/V3, all 6 hardware threads are selected \n
  - For QDSP6 V3L, all 4 hardware threads are selected \n
  - For QDSP6 V4, all 3 hardware threads are selected
 */  


#define QURT_THREAD_CFG_BITMASK_HT0      0x00000001   /**< HTO. */
#define QURT_THREAD_CFG_BITMASK_HT1      0x00000002   /**< HT1. */
#define QURT_THREAD_CFG_BITMASK_HT2      0x00000004   /**< HT2. */ 
#define QURT_THREAD_CFG_BITMASK_HT3      0x00000008   /**< HT3. */
#define QURT_THREAD_CFG_BITMASK_HT4      0x00000010   /**< HT4. */
#define QURT_THREAD_CFG_BITMASK_HT5      0x00000020   /**< HT5. */
/** @cond RTOS_user_guide */
/** @addtogroup thread_macros
@{ */
/**   @xreflabel{sec:qurt_thread_cfg} */  

#define QURT_THREAD_CFG_BITMASK_ALL      0x000000ff   /**< Select all the hardware threads. */
/** @} */ /* end_addtogroup thread_macros */
/** @endcond */

#define QURT_THREAD_CFG_USE_RAM          0x00000000   /**< Use RAM. */
#define QURT_THREAD_CFG_USE_TCM          0x00000100   /**< Use TCM. */
/** @cond RTOS_user_guide */
/** @addtogroup thread_macros
@{ */
#define QURT_THREAD_BUS_PRIO_DISABLED    0   /**< Thread internal bus priority disabled */
#define QURT_THREAD_BUS_PRIO_ENABLED     1   /**< Thread internal bus priority enabled  */
/** @} */ /* end_addtogroup thread_macros */
/** @endcond */
/*
   Macros for QuRT thread attributes.   
 */

#define QURT_HTHREAD_L1I_PREFETCH      0x1     /**< Enables hardware L1 instruction cache prefetching. */
#define QURT_HTHREAD_L1D_PREFETCH      0x2     /**< Enables hardware L1 data cache prefetching. */
#define QURT_HTHREAD_L2I_PREFETCH      0x4     /**< Enables hardware L2 instruction cache prefetching. */
#define QURT_HTHREAD_L2D_PREFETCH      0x8     /**< Enables hardware L2 data cache prefetching. */
#define QURT_HTHREAD_DCFETCH           0x10    /**< Enables DC fetch to the provided virtual address. 
                                     DC fetch instructs the hardware that a data memory access is likely. 
                                     Instructions are dropped in the case of high bus utilization. */

#define QURT_THREAD_ATTR_CREATE_JOINABLE             1
#define QURT_THREAD_ATTR_CREATE_DETACHED             0


#define QURT_THREAD_ATTR_NAME_MAXLEN            16  /**< Maximum name length. */
#define QURT_THREAD_ATTR_TCB_PARTITION_RAM      0  /**< Creates threads in RAM/DDR. */
#define QURT_THREAD_ATTR_TCB_PARTITION_TCM      1  /**< Creates threads in TCM. */
/** @cond RTOS_user_guide */
/** @addtogroup thread_macros
@{ */
#define QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT  QURT_THREAD_ATTR_TCB_PARTITION_RAM  /**< Backward compatibility. */
#define QURT_THREAD_ATTR_PRIORITY_DEFAULT       255   /**< Priority.*/
#define QURT_THREAD_ATTR_ASID_DEFAULT           0  /**< ASID. */
#define QURT_THREAD_ATTR_AFFINITY_DEFAULT      (-1)  /**< Affinity. */
#define QURT_THREAD_ATTR_BUS_PRIO_DEFAULT       255  /**< Bus priority. */
#define QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT   (-2)  /**< Timetest ID. */
#define QURT_THREAD_ATTR_STID_DEFAULT 0
/** @} */ /* end_addtogroup thread_macros */
/** @endcond */
/*=============================================================================
												TYPEDEFS
=============================================================================*/
/** @addtogroup thread_types
@{ */
/** @cond RTOS_user_guide */
typedef enum {
    CCCC_PARTITION = 0,     /**< Use the CCCC page attribute bits to determine the main or auxiliary partition. */
    MAIN_PARTITION = 1,     /**< Use the main partition. */
    AUX_PARTITION = 2,      /**< Use the auxiliary partition. */
    MINIMUM_PARTITION = 3   /**< Use the minimum. Allocates the least amount of cache (no-allocate policy possible) for this thread. */
} qurt_cache_partition_t;

/** @endcond */
/** Thread ID type */
typedef unsigned int qurt_thread_t;

/** @cond RTOS_user_guide */
/** Thread attributes */
typedef struct _qurt_thread_attr {
    
    char name[QURT_THREAD_ATTR_NAME_MAXLEN]; /**< Thread name. */
    unsigned char tcb_partition;  /**< Indicates whether the thread TCB resides in RAM or
                                       on chip memory (in other words, TCM). */
    unsigned char  stid;          /**< SW thread ID used to configure "stid" register
                                       for profiling pusposes */
    unsigned short priority;      /**< Thread priority. */
    unsigned char  asid;          /**< Address space ID. */
    unsigned char  bus_priority;  /**< Internal bus priority. */
    unsigned short timetest_id;   /**< Timetest ID. */
    unsigned int   stack_size;    /**< Thread stack size. */
    void *stack_addr;             /**< Pointer to the stack address base, the range of the stack is
                                       (stack_addr, stack_addr+stack_size-1). */
    unsigned short detach_state;  /**< Detach state of the thread */

} qurt_thread_attr_t;

/** @cond RTOS_user_guide */
/** Dynamic TLS attributes */
typedef struct qurt_tls_info {
  unsigned int module_id;        /**< Module Id for loaded dynamic linked library */
  unsigned int tls_start;        /**< Start address of TLS data */
  unsigned int tls_data_end;     /**< End address of TLS RW data */
  unsigned int tls_end;          /**< End address of TLS data */
}qurt_tls_info;
/** @endcond */
/** @} */ /* end_addtogroup thread_types */

/*=============================================================================
												FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_thread_attr_init
  Initializes the structure used to set the thread attributes when a thread is created.
  After an attribute structure is initialized, the individual attributes in the structure can be
  explicitly set using the thread attribute operations.

  The default attribute values set by the initialize operation are the following: \n
  - Name -- Null string \n
  - TCB partition -- #QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT
  - Affinity -- #QURT_THREAD_ATTR_AFFINITY_DEFAULT \n
  - Priority -- #QURT_THREAD_ATTR_PRIORITY_DEFAULT \n
  - ASID -- #QURT_THREAD_ATTR_ASID_DEFAULT \n   
  - Bus priority -- #QURT_THREAD_ATTR_BUS_PRIO_DEFAULT \n
  - Timetest ID -- #QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT \n
  - stack_size -- 0
  - stack_addr -- 0

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Pointer to the thread attribute structure.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_init (qurt_thread_attr_t *attr)
{

    attr->name[0] = 0;
    attr->tcb_partition = QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT;
    attr->priority = QURT_THREAD_ATTR_PRIORITY_DEFAULT;
    attr->asid = QURT_THREAD_ATTR_ASID_DEFAULT;
    attr->bus_priority = QURT_THREAD_ATTR_BUS_PRIO_DEFAULT;
    attr->timetest_id = QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT;
    attr->stack_size = 0;
    attr->stack_addr = 0;
    attr->detach_state = QURT_THREAD_ATTR_CREATE_DETACHED;
    attr->stid = QURT_THREAD_ATTR_STID_DEFAULT;
}

/**@ingroup func_qurt_thread_attr_set_name
  Sets the thread name attribute.\n
  This function specifies the name to be used by a thread.
  Thread names are used to identify a thread during debugging or profiling. \n
  @note1hang Thread names differ from the kernel-generated thread identifiers used to
  specify threads in the API thread operations.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] name     Pointer to the character string containing the thread name.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_name (qurt_thread_attr_t *attr, char *name)
{
    strlcpy (attr->name, name, QURT_THREAD_ATTR_NAME_MAXLEN);
    attr->name[QURT_THREAD_ATTR_NAME_MAXLEN - 1] = 0;
}


/**@ingroup func_qurt_thread_attr_set_tcb_partition
  Sets the thread TCB partition attribute.
  Specifies the memory type where a thread control block (TCB) of a thread is allocated.
  TCBs can be allocated in RAM or TCM/LPM.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr  Pointer to the thread attribute structure.
  @param[in] tcb_partition TCB partition. Values:\n
                     0 - TCB resides in RAM \n
                     1 - TCB resides in TCM/LCM

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_tcb_partition (qurt_thread_attr_t *attr, unsigned char tcb_partition)
{
    attr->tcb_partition = tcb_partition;
}

/**@ingroup func_qurt_thread_attr_set_priority
  Sets the thread priority to be assigned to a thread.
  Thread priorities are specified as numeric values in the range 1 to 255, with 1 representing
  the highest priority.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] priority Thread priority.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_priority (qurt_thread_attr_t *attr, unsigned short priority)
{
    attr->priority = priority;
}

/**@ingroup qurt_thread_attr_set_detachstate
  Sets the thread detach state with which thread is created.
  Thread detach state is either joinable or detached, specified by following values-
  QURT_THREAD_ATTR_CREATE_JOINABLE            
  QURT_THREAD_ATTR_CREATE_DETACHED     

  By default all qurt threads are created detached.       

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] detachstate Thread detach state.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_detachstate (qurt_thread_attr_t *attr, unsigned short detachstate)
{	
    if(detachstate == QURT_THREAD_ATTR_CREATE_JOINABLE  || detachstate == QURT_THREAD_ATTR_CREATE_DETACHED){
		attr->detach_state = detachstate;
	}
}

/*RTOS_user_guide */
/**@ingroup func_qurt_thread_attr_set_timetest_id
  Sets the thread timetest attribute.\n
  Specifies the timetest identifier to be used by a thread.

  Timetest identifiers are used to identify a thread during debugging or profiling. \n
  @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
             specify threads in the API thread operations.

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr   Pointer to the thread attribute structure.
  @param[in] timetest_id Timetest identifier value.

  @return
  None.

  @dependencies
  None.
  */
static inline void qurt_thread_attr_set_timetest_id (qurt_thread_attr_t *attr, unsigned short timetest_id)
{
    attr->timetest_id = timetest_id;
}

/**@ingroup func_qurt_thread_attr_set_stack_size
  @xreflabel{sec:set_stack_size}
  Sets the thread stack size attribute.\n
  Specifies the size of the memory area to be used for a call stack of a thread.

  The thread stack address (Section @xref{sec:set_stack_addr}) and stack size specify the memory area used as a
  call stack for the thread. The user is responsible for allocating the memory area used for
  the stack.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] stack_size Size (in bytes) of the thread stack.

  @return
  None.

  @dependencies
  None.
*/

static inline void qurt_thread_attr_set_stack_size (qurt_thread_attr_t *attr, unsigned int stack_size)
{
    attr->stack_size = stack_size;
}

/**@ingroup func_qurt_thread_attr_set_stack_addr
  @xreflabel{sec:set_stack_addr}
  Sets the thread stack address attribute. \n
  Specifies the base address of the memory area to be used for a call stack of a thread.

  stack_addr must contain an address value that is 8-byte aligned.

  The thread stack address and stack size (Section @xref{sec:set_stack_size}) specify the memory area used as a
  call stack for the thread. \n
  @note1hang The user is responsible for allocating the memory area used for the thread
             stack. The memory area must be large enough to contain the stack that is
             created by the thread.

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] stack_addr  Pointer to the 8-byte aligned address of the thread stack.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_stack_addr (qurt_thread_attr_t *attr, void *stack_addr)
{
    attr->stack_addr = stack_addr;
}

/**@ingroup func_qurt_thread_attr_set_bus_priority
   Sets the internal bus priority state in the Hexagon core for this software thread attribute. 
   Memory requests generated by the thread with bus priority enabled are
   given priority over requests generated by the thread with bus priority disabled. 
   The default value of bus priority is disabled.

   @note1hang Sets the internal bus priority for Hexagon processor version V60 or greater. 
              The priority is not propagated to the bus fabric.
  
   @datatypes
   #qurt_thread_attr_t

   @param[in] attr Pointer to the thread attribute structure.

   @param[in] bus_priority Enabling flag. Values: \n 
         - #QURT_THREAD_BUS_PRIO_DISABLED \n
         - #QURT_THREAD_BUS_PRIO_ENABLED @tablebulletend

   @return
   None

   @dependencies
   None.
*/
static inline void qurt_thread_attr_set_bus_priority ( qurt_thread_attr_t *attr, unsigned short bus_priority)
{
    attr->bus_priority = bus_priority;
}


static inline void qurt_thread_attr_enable_stid ( qurt_thread_attr_t *attr, char enable_stid)
{
    if (enable_stid) {
        attr->stid = enable_stid;
    }
    else
    {
        attr->stid = QURT_THREAD_ATTR_STID_DEFAULT;
    }
}

/**@ingroup func_qurt_thread_get_name
  Gets the thread name of current thread.\n
  Returns the thread name of the current thread. 
  Thread names are assigned to threads as thread attributes (Section @xref{sec:threads}). They are used to
  identify a thread during debugging or profiling.

  @param[out] name Pointer to a character string, which specifies the address where the returned thread name is stored.
  @param[in] max_len Maximum length of the character string that can be returned.

  @return
  None.

  @dependencies
  None.
*/
void qurt_thread_get_name (char *name, unsigned char max_len);

/**@ingroup func_qurt_thread_create
  @xreflabel{hdr:qurt_thread_create}
  Creates a thread with the specified attributes, and makes it executable.

  @note1hang This function fails (with an error result) if the set of hardware threads
             specified in the thread attributes is invalid for the target processor version.

  @datatypes
  #qurt_thread_t \n
  #qurt_thread_attr_t
  
  @param[out]  thread_id    Returns a pointer to the thread identifier if the thread was 
                             successfully created.
  @param[in]   attr 	    Pointer to the initialized thread attribute structure that specifies 
                             the attributes of the created thread.
  @param[in]   entrypoint   C function pointer, which specifies the main function of a thread.
  @param[in]   arg  	     Pointer to a thread-specific argument structure
  
   
  @return 
  #QURT_EOK -- Thread created. \n
  #QURT_EFAILED -- Thread not created. 

  @dependencies
  None.
 */
int qurt_thread_create (qurt_thread_t *thread_id, qurt_thread_attr_t *attr, void (*entrypoint) (void *), void *arg);

/*
   Stops the current thread, frees the kernel TCB, and yields to the next highest ready thread. 
  
   @return
   void 

   @dependencies
   None.
 */
void qurt_thread_stop(void);


/**@ingroup func_qurt_thread_resume
   Resumes the execution of a suspended thread.
  
   @param[in]  thread_id Thread identifier.

   @return 
   #QURT_EOK -- Thread successfully resumed. \n
   #QURT_EFATAL -- Resume operation failed.

   @dependencies
   None.
 */
int qurt_thread_resume(unsigned int thread_id);

/**@ingroup func_qurt_thread_get_id
   Gets the identifier of the current thread.\n
   Returns the thread identifier for the current thread.
     
   @return 
   Thread identifier -- Identifier of the current thread. 

   @dependencies
   None.
 */
qurt_thread_t qurt_thread_get_id (void);


/**@ingroup func_qurt_thread_get_l2cache_partition
   Returns the current value of the L2 cache partition assigned to the caller thread.\n
     
   @return 
   Value of the data type #qurt_cache_partition_t.

   @dependencies
   None.
 */
qurt_cache_partition_t qurt_thread_get_l2cache_partition (void);

/**@ingroup func_qurt_thread_set_timetest_id
   Sets the timetest identifier of the current thread.
   Timetest identifiers are used to identify a thread during debugging or profiling.\n
   @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
              specify threads in the API thread operations.

   @param[in]  tid  Timetest identifier.

   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_timetest_id (unsigned short tid);

/**@ingroup func_qurt_thread_set_cache_partition
   Sets the cache partition for the current thread. This function uses the type qurt_cache_partition_t to select the cache partition 
            of the current thread for the L1 I cache, L1 D cache, and L2 cache.
  
   @datatypes
   #qurt_cache_partition_t 

   @param[in] l1_icache L1 I cache partition.
   @param[in] l1_dcache L1 D cache partition.
   @param[in] l2_cache L2 cache partition.
    
   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_cache_partition(qurt_cache_partition_t l1_icache, qurt_cache_partition_t l1_dcache, qurt_cache_partition_t l2_cache);


#if 0
/**
   Sets the prefetch value in the SSR.
  
   @param   settings	The least significant 5 bits are used to set the prefetchability of the calling
                        thread. See @xhyperref{Q5,[Q5]} for hardware meanings.
    
   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_prefetch(unsigned int settings);
#endif

/**@ingroup func_qurt_thread_set_coprocessor
   Sets the coprocessor configuration.

   This operation performs per-thread configuration of the coprocessor. It is used during debugging or profiling.

   @note1hang The coproc_id value is stored in the XA field of the Hexagon system-level SSR. 
  
   @param[in]  enable    Coprocessor state. Values:\n
                         - 0 -- Disable  \n
                         - 1 -- Enable  @tablebulletend

   @param[in]  coproc_id   Coprocessor identifier value. This value is coprocessor-specific; for more information 
                           see the coprocessor programming manual.
    
   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_coprocessor(unsigned int enable, unsigned int coproc_id);

/**@ingroup func_qurt_thread_get_timetest_id
   Gets the timetest identifier of the current thread.\n
   Returns the timetest identifier of the current thread.\n
   Timetest identifiers are used to identify a thread during debugging or profiling. \n
   @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
              specify threads in the API thread operations.

   @return 
   Integer -- Timetest identifier. 

   @dependencies
   None.
 */
unsigned short qurt_thread_get_timetest_id (void);

/**@ingroup func_qurt_thread_exit
   @xreflabel{sec:qurt_thread_exit}
   Stops the current thread and awakens any threads joined to it, then destroys the stopped
   thread.

   Any thread that has been suspended on the current thread (by performing a thread join 
   -- Section @xref{sec:thread_join}) is awakened and passed a user-defined status value 
   indicating the status of the stopped thread.\n

   @note1hang Exit must be called in the context of the thread to be stopped.
  
   @param[in]   status User-defined thread exit status value.

   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_exit(int status);

/**@ingroup func_qurt_thread_join
   @xreflabel{sec:thread_join}
   Waits for a specified thread to finish.
   The specified thread should be another thread within the same process.
   The caller thread is suspended until the specified thread exits. When this happens, the
   caller thread is awakened. \n
   @note1hang If the specified thread has already exited, this function returns immediately
              with the result value #QURT_ENOTHREAD. \n
   @note1cont Two threads cannot call qurt_thread_join to wait for the same thread to finish.
              If this happens, QuRT generates an exception (see Section @xref{sec:exceptionHandling}).
  
   @param[in]   tid     Thread identifier.
   @param[out]  status  Destination variable for thread exit status. Returns an application-defined 
                        value indicating the termination status of the specified thread. 
  
   @return  
   #QURT_ENOTHREAD -- Thread has already exited. \n
   #QURT_EOK -- Thread successfully joined with valid status value. 

   @dependencies
   None.
 */
int qurt_thread_join(unsigned int tid, int *status);

/**@ingroup qurt_thread_detach
   @xreflabel{sec:thread_detach}
   Detaches a joinable thread.
   The specified thread should be another thread within the same process and should be created as a joinable thread.
   Only joinable threads can be detached.
   If a joinable thread is detached, it will finish it's execution and exit.
  
   @param[in]   tid     Thread identifier.
   
   @return  
   #QURT_ENOTHREAD -- Thread specifed by tid does not exist.. \n
   #QURT_EOK -- Thread successfully detached.

   @dependencies
   None.
 */
int qurt_thread_detach(unsigned int tid);

/**@ingroup func_qurt_thread_get_anysignal
   Gets the signal of the current thread.
   Returns the RTOS-assigned signal of the current thread.

   QuRT assigns every thread a signal to support communication between threads.
   
   @return  
   Signal object address -- Any-signal object assigned to the current thread.

   @dependencies
   None.
  
 */
unsigned int qurt_thread_get_anysignal(void);

/**@ingroup func_qurt_thread_get_priority 
   Gets the priority of the specified thread. \n 
   Returns the thread priority of the specified thread.\n
   Thread priorities are specified as numeric values in a range as large as 0 through 255, with lower
   values representing higher priorities. 0 represents the highest possible thread priority. \n
   @note1hang QuRT can be configured to have different priority ranges.

   @datatypes
   #qurt_thread_t
  
   @param[in]  threadid	   Thread identifier.	

   @return
   -1 -- Invalid thread identifier. \n
   0 through 255 -- Thread priority value.

   @dependencies
   None.
 */
int qurt_thread_get_priority (qurt_thread_t threadid);

/**@ingroup func_qurt_thread_set_priority
   Sets the priority of the specified thread.\n
   Thread priorities are specified as numeric values in a range as large as 0 through 255, with lower
   values representing higher priorities. 0 represents the highest possible thread priority.

   @note1hang QuRT can be configured to have different priority ranges. For more
              information see Section @xref{sec:AppDev}.
   
   @datatypes
   #qurt_thread_t

   @param[in] threadid	    Thread identifier.	
   @param[in] newprio 	    New thread priority value.

   @return
   0 -- Priority successfully set. \n
   -1 -- Invalid thread identifier. \n 
   
   @dependencies
   None.
 */
int qurt_thread_set_priority (qurt_thread_t threadid, unsigned short newprio);

/*
   Gets the QuRT API version.
 
  @return
  Qurt API version.

  @dependencies
  None.
 */
unsigned int qurt_api_version(void);

/**@ingroup func_qurt_thread_attr_get
  Gets the attributes of the specified thread.\n

  @datatypes
  #qurt_thread_t \n
  #qurt_thread_attr_t

  @param[in]  thread_id	    Thread identifier.
  @param[out] attr 	    Pointer to the destination structure for thread attributes.
  
  @return
  #QURT_EOK -- Success. \n
  #QURT_EINVALID -- Invalid argument.

  @dependencies
  None.
 */
int qurt_thread_attr_get (qurt_thread_t thread_id, qurt_thread_attr_t *attr);

/**@ingroup func_qurt_system_stm_thread_attr_get
  Gets the attributes of the specified thread.\n
  This function assumes that Q6 is in single thread mode.\n
  
  @note1hang1 This function is to be called only when Q6 has encountered an exception and 
             has entered single thread mode. For all other purposes use qurt_thread_attr_get()
             to retrieve the thread attributes.

  @note2 User process threads have two different thread attribute structures. Only the currently
         executing thread attributes are returned.

  @datatypes
  #qurt_thread_t \n
  #qurt_thread_attr_t

  @param[in]  thread_id	    Thread identifier.
  
  @return
   Thread attributes -- Pointer to thread attributes of the specified thread.
   NULL -- If the Thread identifier is invalid.

  @dependencies
  None.
 */
qurt_thread_attr_t * qurt_system_stm_thread_attr_get (qurt_thread_t thread_id);

/**@ingroup qurt_thread_get_tls_base
  Gets the base address of thread local storage(TLS) of a dynamically loaded module
  for the current thread .\n
  
  @datatypes
  #qurt_tls_info \n

  @param[in]  info	   thread tls info for a module
  
  @return
   Pointer to TLS object for dynamically loaded module.
   NULL -- If the tls info is invalid.

  @dependencies
  None.
 */
void * qurt_thread_get_tls_base(qurt_tls_info* info);

/**@ingroup func_qurt_thread_pktcount_get
  Gets the PKTCOUNT of a specified thread.\n

  @datatypes
  #qurt_thread_t \n

  @param[in]  thread_id	    Thread identifier.
  
  @return
   PKTCOUNT

  @dependencies
  None.
 */

long long int qurt_thread_pktcount_get (qurt_thread_t thread_id);

/**@ingroup func_qurt_thread_pktcount_set
  sets the PKTCOUNT for the current qurt thread.\n


  @datatypes
  #long long int\n

  
  @return
   value pktcount has been set to
  @dependencies
  None.
 */

long long int qurt_thread_pktcount_set (long long int);

/* request pager wakeup, arg indicates reason */
int qurt_thread_wake_pager(unsigned arg);

/**@ingroup func_qurt_thread_stid_enable
  Enable stid profiling, return id to use for reading\n

  @datatypes
  #qurt_thread_t \n

  @param[in]  thread_id	    Thread identifier.
  
  @return
   stid

  @dependencies
  None.
 */

char qurt_thread_stid_get(qurt_thread_t thread_id);

#endif /* QURT_THREAD_H */

