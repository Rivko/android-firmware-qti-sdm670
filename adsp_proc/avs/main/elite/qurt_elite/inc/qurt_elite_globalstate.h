/*========================================================================
Copyright (c) 2010, 2013-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_globalstate.h

@brief This file contains the global state structure for the qurt_elite environment.
This state includes system-wide information such as the number
of active threads and malloc counters.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_globalstate_intro
The global state utilities include a global state structure and the following
functions for the QuRT_Elite environment. These utilities include system-wide
information such as the number of active threads and memory allocation
counters.
 - qurt_elite_globalstate_init()
 - qurt_elite_globalstate_deinit()
 - lpa_init()
 - lpa_deinit()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_globalstate.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/24/15    sw     (Tech Pubs) Edited Doxygen comments for 8996.
05/11/15    sw     (Tech Pubs) Edited Doxygen comments for 8952.
09/05/14    sw     (Tech Pubs) Merged Doxygen comments from 2.4; edited
                   Doxygen comments/markup for 2.6.
03/25/13    sw     (Tech Pubs) Edited Doxygen comments/markup for 2.0.
02/04/10    mwc      Created file.
03/09/10    brs      Edited for doxygen-to-latex process.
========================================================================== */

#ifndef QURT_ELITE_GLOBALSTATE_H
#define QURT_ELITE_GLOBALSTATE_H

/*=======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite_atomic.h"
#include "qurt_elite_thread.h"
#include "qurt_elite_mutex.h"
#include "qurt_elite_queue.h"
#include "qurt_elite_memorymap.h"
#include "qurt_elite_memory.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_globalstate
@{ */

/** Number of Audio Stream Manager (ASM) queues. User session IDs are from 1
    to 15. Session ID 0 is reserved.
*/
#define ASM_NUM_QUEUES (15+1)

/** Maximum length of the thread name, including the NULL terminator.
*/
#define QURT_ELITE_THREAD_NAME_MAX_LENGTH (16)

/* Forward declaration. */
typedef struct qurt_elite_globalstate_t qurt_elite_globalstate_t;

/* Forward declaration. */
typedef struct qurt_elite_thread_list_t qurt_elite_thread_list_t;

/* Forward declaration. */
typedef struct qurt_elite_memorymap_node_t qurt_elite_memorymap_node_t;

/* Forward declaration. */
typedef struct qurt_elite_memorymap_client_t qurt_elite_memorymap_client_t;

/** Valid thread states.
*/
typedef enum {
   QURT_ELITE_THREAD_NONE  =    0,  /**< No thread. */
   QURT_ELITE_THREAD_LAUNCHING  ,   /**< Thread is launching. */
   QURT_ELITE_THREAD_RUNNING    ,   /**< Thread is running. */
   QURT_ELITE_THREAD_EXITING        /**< Thread is exiting. */
} QURT_ELITE_THREAD_STATES;

/** Node of a thread in the array of threads in the global state structure.
*/
struct qurt_elite_thread_list_t {
   qurt_elite_thread_t     tid;      /**< Thread ID. */
   int                nExitStatus;   /**< Thread exit status. */
   char*              pStack;        /**< Thread stack buffer. */
   int                nStackSize;    /**< Thread stack size. */
   char               name[QURT_ELITE_THREAD_NAME_MAX_LENGTH];
                                     /**< Thread name. */
   int                (*pfStartRoutine)(void *);
                                     /**< Function pointer to the thread entry
                                          function. */
   void               *arg;          /**< Pointer to the thread arguments. */
   int                nThreadState;  /**< Thread state. */
   bool_t             fPreAllocatedStack;
                                     /**< Indicates whether the user assigned
                                          the thread stack. */

};

/** Physical and virtual address information about a memory segment.
 */
typedef struct {
    uint64_t phy_addr;   /**< Starting physical address. */
    uint32_t virt_addr;  /**< Starting virtual address. */
    uint32_t size;       /**< Size in bytes. @newpagetable */
} qurt_elite_mem_seg_t;

/** Memory usage statistics obtained during a test case run.
*/
typedef struct {
   qurt_elite_mutex_t   lock;
   /**< Mutex lock for this data structure. */

   uint32_t             num_mallocs;
   /**< Total number of memory allocations up to the current point in the
        test. */

   uint32_t             num_frees;
   /**< Total number of times memory is freed to the current point in the
        test. */

   uint32_t             curr_heap;
   /**< Current heap usage at the current point in the test. */

   uint32_t             peak_heap;
   /**< Peak heap usage up to the current point in the test.*/

} qurt_elite_mem_stats_t;

//***************************************************************************
// Global State
//***************************************************************************

/** Global structure used to track resources, such as threads and queues. This
  structure is intended for use in such tasks as debugging and checking for
  leaks.
*/
struct qurt_elite_globalstate_t
{

   qurt_elite_mem_stats_t avs_stats[QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS +1];
   /**< Heap statistics for Audio-Voice Subsystem (AVS) threads.

        This number comprises one default heap plus the
        #QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS non-default heap. */

   qurt_elite_mem_stats_t non_avs_stats;
   /**< Heap statistics for non-AVS threads. */

   volatile int32_t        nSimulatedMallocFailCount;
   /**< If the failure count > 0, counts memory allocations down to zero, and
        then simulates out-of-memory. This count is used for testing. */

   qurt_elite_atomic_word_t      nMsgQs;
   /**< Counter of queues to help generate unique names. */

   qurt_elite_atomic_word_t      nMemRegions;
   /**< Counter of the number of memory regions in a system. */

   qurt_elite_mutex_t            mutex;
   /**< Mutex for thread safety of this structure. */

   qurt_elite_thread_list_t      aThreadContextBuf[QURT_ELITE_MAX_SW_THREADS];
   /**< Linked list of threads in the system. */

   qurt_elite_memorymap_client_t *pMemoryMapClientList;
   /**< Linked list of memory map clients in the system. */

   volatile uint32_t        bEnableQLogging;
   /**< Logs the commands going into queues and coming out of queues. */

   qurt_elite_queue_t            *pAdmStatSvcCmdQ;
   /**< Pointer to the Audio Device Manager (ADM) static service command
        queue. */

   qurt_elite_queue_t *pAfeStatSvcCmdQ;
   /**< Pointer to the Audio Front End (AFE) static service command queue. */

   qurt_elite_queue_t *pVoiceTimerCmdQ;
   /**< Pointer to the ASM static service session command queues.

        i@sscr{th} element @rarrow i@sscr{th} session @rarrow
        (i-1)@sscr{th} ASM internal session ID */

   qurt_elite_queue_t *pAsmSessionCmdQ[ASM_NUM_QUEUES];
   /**< Specifies whether all static services are up and ready. */

   qurt_elite_queue_t *pAvcsCmdQ;
   /**< Pointer to the command queue of the Audio-Voice Core Service (AVCS). */

   qurt_elite_queue_t *pVPMSvcCmdQ;
   /**< Pointer to the Voice Processing Manager (VPM) static service command
        queue. */

   volatile uint32_t uSvcUpStatus;
   /**< Specifies whether the aDSP static services are up and ready. */

   uint64_t lpm_virt_addr;
   /**< Base virtual address of the low-power memory (LPM). */

   uint32_t lpm_size;
   /**< Size of the LPM in bytes. */

   uint32_t lpm_heapid;
   /**< Heap ID of the LPM. */

   bool_t is_lpm_cached;
   /**< Specifies whether the LPM is cached.

        @values
        - TRUE -- Cached
        - FALSE -- Uncached @tablebulletend */

   qurt_elite_mem_seg_t lpm_afe_dma_buf;
   /**< AFE DMA buffers are placed in LPM. */

   qurt_elite_mem_seg_t lpm_afe_working_buf;
   /**< AFE working buffer is placed in LPM for low power. */
   
   volatile uint32_t size_lpa_heap_from_ddr;
   /**< Heap size of low-power audio (LPA). */

    uint32_t *ddr_lpa_heap;
    /**< Address in the heap where LPA dynamic buffers are allocated.
         @newpagetable */

};


/** Linked list of memory regions.
 */
struct qurt_elite_memorymap_node_t
{

    qurt_mem_pool_t     MemPool;
    /**< Memory pool from which the memory region is created. */

    uint16_t        unNumContPhysReg;
    /**< Number of physical memory regions in this node. */

    int16_t         ref_count;
    /**< Reference count that the client can increment to lock this memory map
         handle.

         Unmapping can only be performed if ref_count reaches zero.

         The client must decrement ref_count when it does not use this memory
         map handle. */

    uint32_t        mapping_mode;
    /**< Specifies whether the mapping is physical or virtual, or if it is a
         physical offset. */

    qurt_elite_memorymap_node_t *pNext;
    /**< Pointer to the next node in the linked list.

         @tblsubhd{If unNumContPhysReg is greater than 1}
         In addition to the number of qurt_elite_memorymap_region_record_t
         structures following this structure, one more ContPhysReg follows to
         represent the master region for all ContPhysRegs (called the <i>
         virtual memory region</i> in QuRT).

         When freeing the regions, all of the following space is also freed. */
};


/** Maintains a linked list of clients registered with qurt_elite_memorymap.
 */
struct qurt_elite_memorymap_client_t
{

    qurt_elite_memorymap_node_t   *pMemMapListNode;
    /**< List of memory map nodes for this client.*/

    qurt_elite_mutex_t             mClientMutex;
    /**< Mutex to access the list. */

    qurt_elite_memorymap_client_t *pNext;
    /**< Next entry in the client list. @newpagetable */

};


/** Instance of the global state structure. */
extern qurt_elite_globalstate_t qurt_elite_globalstate;


/**
  Initializes the global state structure. This function must be
  called once at bootup.

  @return
  None.

  @dependencies
  None.
*/
void qurt_elite_globalstate_init(void);

/**
  De-initializes the global state structure. This function must be
  called before exiting to do a final cleanup.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_globalstate_deinit(void);

/**
  Initializes LPA. This function must be called before using LPA.

  @return
  ADSP_EOK -- Initialization is successful.
  @par
  ADSP_EFAILED -- Otherwise.
  @par
  For more information, refer to @xrefcond{Q1,80-NF774-4,80-NA610-4}.

  @dependencies
  None.
*/
ADSPResult lpa_init(void);

/**
  De-initializes LPA.

  @return
  None.

  @dependencies
  Must be called after all usage of LPA ends.
*/
void lpa_deinit(void);

/** @} */ /* end_addtogroup qurt_elite_globalstate */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_GLOBALSTATE_H
