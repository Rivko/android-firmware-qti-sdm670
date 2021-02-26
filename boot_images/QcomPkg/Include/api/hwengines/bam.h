#ifndef BAM_H_
#define BAM_H_

/**
  @file bam.h
  @brief Public interface include file for the BAM driver.

  This file contains definitions of constants, data structures, and
  interfaces that provide operational control to the BAM device.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/11/14   RL      64-bit support
08/13/13   RL      LPAE support
07/15/13   RL      Trigger pipe event ( system mode )
12/04/12   AN      Added error bus control register to result data structure
09/20/12   SS      Added Desc & Data FIFO info to core dump
05/17/12   SS      Added Core Dump Support
02/29/12   MK      Added BulkTransfer Support
02/08/12   NK      Added support for new descriptors and iovec flags
03/31/11   MK      Created

===============================================================================
                   Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/



#include "DALStdDef.h"

/** @addtogroup bam_constants
@{ */

#define BAM_MAX_PIPES 31 /**< Max pipes supported by the BAM driver (and the device) */

#define BAM_IRQ_INVALID 0 /**< IRQ is not available, or IRQ mode is not desired */

/** @}  - bam_constants */

/** @addtogroup bam_enum_api
@{ */

/** Various status codes returned by BAM APIs */
typedef enum
{
    BAM_SUCCESS          = 0x0, /**< Operation was a success */
    BAM_FAILED           = 0x1, /**< Operation failed        */
    BAM_INVALID_CONFIG   = 0x2, /**< Invalid Config provided */
    BAM_INVALID_BUFSIZE  = 0x3, /**< Invalid buffer size for dump */
    BAM_POLL_NO_RESULT   = 0x4, /**< No result was generated in a polling operation. */
    BAM_PIPE_DISABLED    = 0x5, /**< Pipes is disabled */
    BAM_PIPE_NOT_EMPTY   = 0x6, /**< Pipe not empty */
    BAM_NOT_ENOUGH_SPACE = 0x7  /**< Not enough space in descriptor fifo */

}bam_status_type;


/** Bit flag options for BAM connection.  The enums should
 * be OR'd together to create the option set for the BAM connection.
 */
typedef enum
{
   BAM_O_DESC_DONE         = 0x00000001,    /**< Descriptor processed indication from hardware */
   BAM_O_INACTIVE          = 0x00000002,    /**< Inactivity timeout indication from hardware */
   BAM_O_WAKEUP            = 0x00000004,    /**< Peripheral wake up indication from hardware */
   BAM_O_OUT_OF_DESC       = 0x00000008,    /**< Out of descriptors indication from hardware */
   BAM_O_ERROR             = 0x00000010,    /**< Error indication from hardware */
   BAM_O_EOT               = 0x00000020,    /**< End-of-transfer indication from hardware */
   BAM_O_RST_ERROR         = 0x00000040,  /**< Unsuccesfful pipe reset operation */
   BAM_O_HRESP_ERROR       = 0x00000080,  /**< Error response from AHB master */
   /* Options to enable hardware features */
   BAM_O_STREAMING         = 0x00010000,    /**< Enable streaming mode (no EOT) */

   /* Options to enable software features */
   BAM_O_POLL              = 0x001000000,   /**< Transfer operation should be polled */
   BAM_O_NO_Q              = 0x002000000,   /**< Disable queuing of transfer events for the connection end point */
   BAM_O_WAKEUP_IS_ONESHOT = 0x004000000,   /**< BAM_O_WAKEUP will be disabled after triggered */
   BAM_O_ACK_TRANSFERS     = 0x00800000,    /**< Client must read each descriptor from the FIFO using bam_pipe_getiovec() */
   BAM_O_AUTO_ENABLE       = 0x01000000,    /**< Connection is automatically enabled */
   BAM_O_ACK_LAST_ONLY     = 0x02000000,    /**< No acknowledgement for all processed descriptors.
                                                 Use this pipe option if the client does not want any notification
                                                 of EOT/INT bits set on all the newly *processed* descriptors.
                                                 The client will be notified of the last newly processed descriptor.
                                                 Clients using this option has to note the following:
                                                  a. In the object of type 'bam_result_type' which gets passed to
                                                     bam_pipe_poll or the argument of the pipe callback function,
                                                     only the fields 'event', 'iovec', 'cb_data' and 'xfer_cb_data' have valid values.
                                                  b. 'event','iovec' and 'xfer_cb_data' field values correspond to the
                                                     last processed descriptor.
                                                  c. an event 'BAM_EVENT_DESC_PROCESSED' is set for 'event'
                                               */ 

   BAM_O_PIPE_NO_INIT       = 0x04000000,    /**< Do not initlize/deinitialize pipe hw */  
   BAM_O_IS_32_BIT          = 0x7fffffff     /**< Force enum size to 32-bit */
} bam_options_type;

/** Events associated with a client event trigger */
typedef enum
{
   BAM_EVENT_INVALID = 0,

   BAM_EVENT_EOT,           /**<  End-of-transfer indication from hardware */
   BAM_EVENT_DESC_DONE,     /**<  Descriptor processed indication from hardware */
   BAM_EVENT_OUT_OF_DESC,   /**<  Out of descriptors indication from hardware */
   BAM_EVENT_WAKEUP,        /**<  Peripheral wake up indication from hardware */
   BAM_EVENT_FLOWOFF,       /**<  Graceful halt (idle) indication from hardware */
   BAM_EVENT_INACTIVE,      /**<  Inactivity timeout indication from hardware */
   BAM_EVENT_ERROR,         /**<  Error indication from hardware - Only at Top Level of BAM */
   BAM_EVENT_HRESP_ERROR,   /**<  Error response from AHB master */
   BAM_EVENT_RST_ERROR,     /**<  Unsuccessful pipe reset operation */
   BAM_EVENT_EMPTY,         /**<  Indication of empty buffer from hardware - Only at Top Level of BAM */
   BAM_EVENT_IN_PROGRESS,   /**<  Indicates that there is an active descriptor */
   BAM_EVENT_DESC_PROCESSED,/**<  New descriptor proccessed notification */
   BAM_EVENT_MAX,

   BAM_EVENT_IS_32_BIT = 0x7fffffff   /**< Force enum size to 32-bit */
} bam_event_type;

/** Flag bits supported by BAM hardware for the options field in bam_iovec_type */
typedef enum
{
   BAM_IOVEC_FLAG_INT       = 0x800,  /**<  Generate interrupt */
   BAM_IOVEC_FLAG_EOT       = 0x400,  /**<  Generate end-of-transfer indication */
   BAM_IOVEC_FLAG_EOB       = 0x200,  /**<  Generate end-of-block indication */
   BAM_IOVEC_FLAG_NWD       = 0x100,  /**<  Generate notify when done */
   BAM_IOVEC_FLAG_CMD       = 0x080,  /**<  Submit a command descriptor */
   BAM_IOVEC_FLAG_LCK       = 0x040,  /**<  Lock the bam to the pipe's group
                                             (will remain locked until desc
                                              with unlock bit is submitted by a
                                              pipe from the locking pipe's grp ) */
   BAM_IOVEC_FLAG_UNLCK     = 0x020,  /**<  Unlock the bam */
   BAM_IOVEC_FLAG_IMM_CMD   = 0x010,  /**<  Submit an immediate command descriptor */

   BAM_IOVEC_FLAG_NO_SUBMIT = 0x008,  /**<  Do not submit descriptor to hardware */
   BAM_IOVEC_FLAG_DEFAULT   = 0x001   /**<  Use driver default */
}bam_iovec_options_type;

/**  Option to indicate the Producer or Consumer direction of the pipe */
typedef enum
{
    BAM_DIR_PRODUCER = 0x1, /**< The pipe can be written (producer mode) */
    BAM_DIR_CONSUMER = 0x0  /**< The pipe can be read (consumer mode) */
}bam_dir_type;

/** Options to indicate the operating mode of the BAM */
typedef enum
{
  BAM_MODE_BAM2BAM = 0x0,  /**< The pipe interacts with another BAM */
  BAM_MODE_SYSTEM  = 0x1   /**< The pipe interacts with the system (software) */
}bam_mode_type;


/** Bitmasked options for BAM notifications via an IRQ */
typedef enum
{
    BAM_IRQ_HRESP_ERR_EN =  0x2,  /**< Illegal Memory access notification */
    BAM_IRQ_ERR_EN       =  0x4,  /**< Fatal Error in BAM due to failed pipe reset */
    BAM_IRQ_EMPTY_EN     =  0x8,  /**< BAM buffers are empty */
    BAM_IRQ_TIMER_EN     = 0x10   /**< BAM inactivity timer has reached its threshold */
}bam_irq_options_type;

/** Options to configure timer control */
typedef enum
{
   BAM_TIMER_OP_CONFIG = 0, /**< Configure the timer */
   BAM_TIMER_OP_RESET,      /**< Reset the timer */
/* BAM_TIMER_OP_START,   Not supported by hardware yet */
/* BAM_TIMER_OP_STOP,    Not supported by hardware yet */
   BAM_TIMER_OP_READ,       /**< Read the timer */

   BAM_TIMER_OP_IS_32_BIT = 0x7fffffff  /* Force enum size to 32-bit */
} bam_timer_oper_type;

/** Operating modes for the inactivity timer */
typedef enum
{
   BAM_TIMER_MODE_ONESHOT   = 0,          /**< Timer is a one-shot timer */
/* BAM_TIMER_MODE_PERIODIC,    Not supported by hardware yet */
   BAM_TIMER_MODE_IS_32_BIT = 0x7fffffff  /**< Force enum size to 32-bit */
} bam_timer_mode_type;

/** Command Element Operation modes */
typedef enum
{
    BAM_CE_OP_WRITE = 0,  /**< Write to a register */
    BAM_CE_OP_READ  = 1   /**< Read from a register - Currently unsupported */
}bam_ce_oper_type;

/** 
 * option to indicate the bam core dump type
 */
typedef enum
{
    BAM_DUMP_TOP  = 0x1, /**< indicates starts of BAM dump */
    BAM_DUMP_PIPE = 0x2  /**< indicates starts of pipe dump */
}bam_dump_type;

/** 
 * callback behavior
 */
typedef enum
{
    BAM_INVOKE_CB_DEFAULT = 0,    /**< assume the default behavior when invoking callbacks */
    BAM_INVOKE_CB_IN_POLL = 1     /**< call the callbacks as part of a polling operation */
}bam_cb_mode_type;
/** @}  - bam_enum_api */

/** Specify the bam virtual address type */
typedef DALSYSMemAddr bam_vaddr;

/** Config structure to identify the BAM to be managed by the BAM driver */
typedef struct _bamconfig
{
    /* Mandatory fields to be supplied by the client */
  uint64 bam_pa;          /**< Physical Address of the BAM block */
  bam_vaddr bam_va;       /**< Virtual Address of the BAM block. If null BAM
                               driver will remap*/
  uint32 bam_irq;         /**< IRQ for the BAM block. If bam_irq == 0, BAM will
                               operate in polling mode */

  uint32 bam_irq_mask;    /**< IRQs to be enabled */
  uint32 sum_thresh;      /**< Descriptor Summing threshold - Max 32kB*/

  uint32 options;         /**< Reserved for future use */

  uint32 bam_mti_irq_pa;  /**< MTI interrupt generation address - Valid only
                               for BAM rev 1 and 2 hardware and when BAM is
                               managed on a remote processor.
                               @note This address must correspond to the MTI
                               associated with the "uIrq" IRQ enum specified
                               above */
} bam_config_type;

/** Config structure to initialize a BAM pipe */
typedef struct _PipeConfig
{
      uint32 options         /**< Bitmasked Options from BAM_O_* enum */;
      bam_dir_type dir;      /**< Direction of data flow in the Pipe */
      bam_mode_type mode;    /**< Operating mode of the Pipe */
      bam_vaddr desc_base_va;/**< Virtual address of descriptor fifo */
      uint64 desc_base_pa;   /**< Physical address of descriptor FIFO */
      uint32 desc_size;      /**< Size (bytes) of descriptor FIFO */
      uint16 evt_thresh;     /**< Event threshold to notify peripheral */

      /* Security features */
      uint32 lock_group;     /**< Group # to which the pipe belongs  */

       /* The following are only valid if mode is BAM2BAM */
      uint64 peer_base_pa;   /**< Peer BAM's physical base address */
      uint64 data_base_pa;   /**< Physical address of data FIFO */
      uint32 peer_pipe_num;  /**< Peer BAM's pipe to be connected to */
      bam_vaddr data_base_va;/**< Virtual address of data FIFO */
      uint32 data_size;      /**< Size (bytes) of data FIFO */

}bam_pipe_config_type;

/** Config strucure to get Bam core dump */ 
typedef struct _coredumpconfig
{
  void *user_buffer;       /**< Buffer for bam core dump */
  uint32 buf_size;         /**< Bam coredump buffer size */
  uint32 pipe_mask;        /**< Bam pipe mask */
  uint32 peer_pipe_mask;   /**< Mask of pipes whose peers need to be dumped */
  void *descfifo_buf;      /**< Descriptor fifo buffer, currently used only in Bam2Bam mode  */
  uint32 descfifo_size;    /**< Aggregate size of all pipe Desc buffer need to dump */
  void *datafifo_buf;      /**< Data fifo buffer, used only in Bam2Bam mode */
  uint32 datafifo_size;    /**< Aggregate size of all pipe Data fifo buffer need to dump */
  void  *option;           /**< future use */
  
} bam_coredump_config_type;

/** structure to describe the transfer packet. It corresponds to the native
 *  I/O vector (BAM descriptor) supported by BAM hardware
 */
typedef struct
{
  uint32 buf_pa;         /**< Buffer physical address */
  uint32 buf_size : 16;  /**< Buffer size in bytes */
  uint32 buf_pa_msb :4;  /**< MSB of a 36 bit buffer physical address */
  uint32 flags : 12;     /**< Flag bitmask (see bam_iovec_options_type) */
} bam_iovec_type;

/** structure to describe a command element. It corresponds to the native
 *  command element supported by BAM hardware
 */
typedef struct
{
  uint32 reg_addr : 24;  /**< Register physical address */
  uint32 command  : 8;   /**< Command  */
  uint32 data ;          /**< Data */
  uint32 mask;           /**< Mask */
  uint32 reserved;       /**< Reserved */
} bam_ce_type;


/** This data type is passed as a payload when triggering a callback event
 * object registered for a BAM or its pipe or via the poll .
 */
typedef struct _bam_result_type
{
   void *cb_data;         /**< user pointer registered via bam_pipe_init or bam_init */

   bam_event_type event;  /**< type of event */

   /** Data associated with the event */
   union
   {
      /** Data for EOT or Desc_Done for Pipe Interrupt */
      struct
      {
         /** The associated I/O vector
          * If the end point is a system-mode producer, the size will reflect
          * the actual number of bytes written to the buffer by the pipe.
          *
          * NOTE: If this I/O vector was part of a set submitted to
          * bam_pipe_transfer(), then the vector array itself will be
          * updated with all of the actual counts.
          */
          bam_iovec_type iovec;

          /** Number of bytes transferred
           * since last EOT (including this iovec)
           */
          uint32 accum_size_bytes;

         void *xfer_cb_data;      /**< user data registered with the transfer */
      } xfer;

      /** Data for Top Level Bam Interrupt - Valid only for BAM_EVENT_ERROR */
      struct
      {
         uint32 status;      /**< Masked status indicating error types */
         uint64 address;     /**< Address which caused the error */
         uint32 data;        /**< Data returned for Transfer that caused the error */
         uint32 bus_ctrls;   /**< Bus control information in case of data error */
      } error;
   } data;
   bam_cb_mode_type  cb_mode;     /**< caller indicates if the pipe callback should be invoked or the default behavior should be assumed */
} bam_result_type;


/** Opaque Pointer to a BAM Object - Can represent a BAM device or
 *  one of its pipes */
typedef void* bam_handle;

/**
 * User callback function to be called for async operations
 */
typedef void (*bam_callback_func_type)(bam_result_type bam_result);
/**
 * User callback type with payload info to be provided to the
 * user callback
 */
typedef struct
{
    bam_callback_func_type func; /**< Function to be called via callback */
    void *data;                  /**< User provided data to be supplied with the callback */
}bam_callback_type;

/** This struct defines a timer control operation parameters and is used as an
 * argument for the bam_pipe_timerctrl() function
 */
typedef struct
{
   bam_timer_oper_type op;   /**< Timer control operation */

   /* The following configuration parameters must be set when the timer
    * control operation is BAM_TIMER_OP_CONFIG.
    */
   bam_timer_mode_type mode; /**< Timer control operation - Valid only for
                                  BAM_TIMER_OP_CONFIG */
   uint32 timeout_ms;        /**< Inactivity timeout (msec) - Valid only for
                                  BAM_TIMER_OP_CONFIG */

} bam_timer_ctrl_type;

/**
 * Timer result structure to report back the timer status
 */
typedef struct
{
   uint32 curr_timer;  /**< Current timer value in the BAM HW */
} bam_timer_rslt_type;

/**
 * This struct provides Bam core dump buffer & max descriptor size info.
 * 
 */
typedef struct
{
  uint32 num_pipes;               /**< Number of pipe supported by Bam instance*/
  uint32 bam_header_buffer_size;  /**< buffer size for core dump header */
  uint32 bam_buffer_size;         /**< Required buffer size for the bam dump*/
  uint32 bam_pipe_buffer_size;    /**< Required buffer size for a bam pipe dump*/
  uint32 bam_max_desc_size;       /**< max transfer size support per descriptor */    
}bam_info_type;


/** @}  - bam_struct_api */


/** @addtogroup bam_func_api
@{ */

/**
 * Command Descriptor Helper Macros
 */

/** BAM_FILL_CD
 * Fills a command descriptor
 * @param[in, out] CD_ADDR  command descriptor address
 * @param[in]      CE_BASE  command element base address
 * @param[in]      NUM      number of command elements
 * @param[in]      FLAGS    flags to specify (see  bam_iovec_options_type). CD flag is automatically set 
 */ 
#define BAM_FILL_CD(CD_ADDR, CE_BASE,NUM, FLAGS) (*(bam_iovec_type*)(CD_ADDR)).buf_pa=(uint32)(CE_BASE);                           \
                                                 (*(bam_iovec_type*)(CD_ADDR)).buf_pa_msb=(uint32)(CE_BASE>>32); \
                                                 (*(bam_iovec_type*)(CD_ADDR)).buf_size=(uint32)((NUM)*sizeof(bam_ce_type)); \
                                                 (*(bam_iovec_type*)(CD_ADDR)).flags=(uint32)(FLAGS)|BAM_IOVEC_FLAG_CMD


/**
 * @brief fills a command element
 *
 * Fills a command element (should be of bam_ce_type size)
 *
 * @param[in]      ce_base   address of the command element base
 * @param[in]      index     command element index
 * @param[in]      reg       register address to program
 * @param[in]      cmd       command (see bam_ce_oper_type)
 * @param[in]      data      data to program to the register
 * @param[in]      mask      mask to use                      
 */
void bam_fill_ce(bam_vaddr ce_base, uint32 index ,uint32 reg, uint32 cmd, uint64 data, uint32 mask);

#define BAM_FILL_CE(CE_BASE, INDEX ,REG, CMD, DATA, MASK)\
     bam_fill_ce((CE_BASE),INDEX,REG,CMD,DATA,MASK)


/**
 * @brief Bam get info
 *
 * This API will provide BAM, pipe & Descriptor required buffer size info for bam core dump.
 *
 * @note
 * Core dump buffer size should be calculated as per below info
 * In Bam2Bam mode:
 * BAM dump buffer size: (bam_buffer_size  + (bam_buffer_size * Nums of different instance of peer bam))
 * BAM pipe dump buffer size: ((bam_pipe_buffer_size * nums of pipe) + (bam_pipe_buffer_size * nums of peer pipe)).
 * In system mode:
 * BAM dump buffer size: bam_buffer_size
 * BAM pipe dump buffer size: bam_pipe_buffer_size * nums of pipe 
 *   
 * Core dump Buf size :BAM dump buffer size + BAM pipe dump buffer size + bam_dumpheader_size 
 *
 * @param[in] bamhandle         bam handle
 * @param[out] info             bam dump buffer size info
 *
 * @return                      status of the operation.
*/
bam_status_type bam_get_info(bam_handle bamhandle, bam_info_type *info);

/**
 * @brief Initialize BAM and configures it.
 * Initializes the BAM driver and the BAM. Allocates a handle to the BAM,
 * which the client is expected to use for future calls to the driver.
 *
 * @param[in] *bam_cfg   Configuration data for BAM. This is peripheral specific
 * @param[in] *bam_cb    Callback to get any top level BAM events
 *
 * @return A handle to the BAM Device if successful
 *
*/
bam_handle bam_init( bam_config_type *bam_cfg, bam_callback_type *bam_cb);

/**
 * @brief Inits the security configuration of the BAM
 *
 * Inits the security configuration of the BAM. This can be directly called if
 * and only if the execution environment is trustzone and does NOT intend to use
 * the BAM. This API ensures that no resources are allocated to the BAM and only
 * configures the security detail of that BAM
 *
 * @param[in] *bam_cfg  Pointer to the BAM config
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_secure_init(bam_config_type *bam_cfg);


/**
 * @brief Uninits the BAM if required & release the IRQ if allocated. Finally
 *        releases any allocations if made by the driver
 * Uninitializes the BAM HW if mentioned and frees up resources that are
 * allocated by the driver (IRQ, memory etc). This should be the last API to
 * be called, and any pipe disable must happen before this API is called.
 *
 * @param[in] handle       Pointer to the BAM handle
 * @param[in] disable_bam   Flag to indicate if the BAM HW must be disabled
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_deinit(bam_handle handle, uint8 disable_bam);


/**
 * @brief Simply resets the BAM hw and restores the config.
 *
 * Resets the BAM hw and restores the config provided via bam_init API. This
 * can be used to re-initialize a bam after the core was reset
 *
 * @param[in] handle  Pointer to the BAM handle
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_reset(bam_handle handle);

/**
 * @brief Enables/Disables the IRQ mode of the BAM Top Level
 *
 * Enables/Disables the IRQ mode of the BAM Top Level.
 *
 * @param[in] bamhandle  Pointer to the BAM handle
 * @param[in] irq_en     Enable (1)/ Disable (0) the irq mode
 * @param[in] irq_mask   IRQs to be enabled for the BAM
 *
 * @return Status of the operation
 *
 * @note This API only sets the IRQ mode for the BAM and NOT the individual pipes
*/
bam_status_type bam_setirqmode(bam_handle bamhandle, uint32 irq_en,uint32 irq_mask);

/**
 * @brief Check if a pipe is operating in interrupt or polling mode
 *
 *
 * @param *handle  - handle to the pipe
 * @param *irq_en  - irq mode enabled ( 1 ) or disabled ( 0 )
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_pipe_isirqmode(bam_handle pipehandle, uint32* irq_en);

/**
 * @brief Initializes pipe pipe_num in the BAM based on the config provided.
 *
 * Enables pipe pipe_num in the BAM based on the pipe_cfg provided. Returns
 * a handle to the pipe allocated to the client to be used for future pipe
 * operations
 *
 * @param[in] bamhandle   Pointer to the handle to the BAM from which the pipe
 *                        is to be allocated
 * @param[in] pipe_num    Pipe to be allocated in the bam.
 * @param[in] *pipe_cfg   Configuration data for the pipe. This is connection specific
 * @param[in] *pipe_cb    Callback to be notified of Pipe level events
 *
 * @return A handle to the pipe allocated successfully
 *
*/

bam_handle bam_pipe_init(bam_handle bamhandle, uint32 pipe_num, bam_pipe_config_type *pipe_cfg, bam_callback_type *pipe_cb);

/**
 * @brief Uninitializes the pipe.
 *
 * Uninitializes a pipe and disables it. No further operations
 * can be performed on the pipe, and the handle is invalidated.
 *
 * @param[in] pipehandle  handle to the pipe to be uninitialized
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_pipe_deinit(bam_handle pipehandle);

/**
 * @brief Enables a pipe.
 *
 * Enables a pipe for transfers
 *
 * @param[in] pipehandle handle to the pipe to be enabled
 *
 * @return Status of the operation
 *
 * @dependencies bam_pipe_init must have succeeded
 *
*/
bam_status_type bam_pipe_enable(bam_handle pipehandle);

/**
 * @brief Disables a pipe.
 *
 * Disables a pipe preventing further transfers
 *
 * @param[in] pipehandle handle to the pipe to be disabled
 *
 * @return Status of the operation
 *
 * @dependencies bam_pipe_init must have succeeded
 *
*/

bam_status_type bam_pipe_disable(bam_handle pipehandle);

/**
 * @brief Enables/Disables the IRQ mode of a pipe.
 *
 * Enables/Disables the IRQ mode of a pipe.
 *
 * @note
 * Note that only events specified in parameter pipe_cfg_options are
 * considered when acknowledging descriptors.When setting a pipe to
 * interrupt mode, the callback is invoked only for events specified
 * in pipe_cfg_options. When setting a pipe to polling mode, only descriptors
 * which have the flags specified in pipe_cfg_options are acknowledged.
 *
 * @param[in] pipehandle     handle to the pipe
 * @param[in] irq_en         Enable (1)/ Disable (0) the irq mode
 * @param[in] pipe_cfg_opts  Events considered when acknowledging descriptors.
 *
 * @return Status of the operation
 *
 * @note This API only sets the IRQ mode for the individual pipes and NOT the BAM.
*/

bam_status_type bam_pipe_setirqmode(bam_handle pipehandle, uint32 irq_en, uint32 pipe_cfg_opts);


/**
 * @brief Perform a single DMA transfer via the BAM pipe
 *
 * @details
 *
 * This function submits a DMA transfer request consisting of a single buffer
 * for a BAM pipe associated with a peripheral-to/from-memory
 * connection.
 *
 * The data buffer must be DMA ready.  The client is responsible for insuring
 * physically contiguous memory, cache maintenance, and memory barrier.
 *
 * The client must not modify the data buffer until the completion indication is
 * received.
 *
 * This function cannot be used if transfer queuing is disabled (see option
 * BAM_O_NO_Q).  The client must set the BAM_O_EOT option to receive a callback
 * event trigger when the transfer is complete.  The BAM driver will insure the
 * appropriate flags in the I/O vectors are set to generate the completion
 * indication.
 *
 * The return value from this function may indicate that an error occurred.
 * Possible causes include invalid arguments.
 *
 * @param[in] pipe_handle  Handle to the pipe
 * @param[in] buf_pa       Physical address of buffer to transfer
 * @param[in] buf_size     Size in bytes of buffer to transfer
 * @param[in] xfer_opts    OR'd I/O vector option flags (see \ref bam_iovec_options_type )
 * @param[in] *user_data    User pointer that will be returned to user as part of
 *                         event payload
 *
 *
 * @return Status of the operation
 *
 */
bam_status_type bam_pipe_transfer(bam_handle pipehandle, uint64 buf_pa, uint16 buf_size, uint16 xfer_opts, void *user_data);

/**
 * @brief Perform a DMA transfer via the BAM pipe
 *
 * @details
 *
 * This function submits multiple DMA transfer request consisting of a contiguous buffer
 * for a BAM pipe associated with a peripheral-to/from-memory
 * connection.
 *
 * The data buffer must be DMA ready.  The client is responsible for insuring
 * physically contiguous memory, cache maintenance, and memory barrier.
 *
 * The client must not modify the data buffer until the completion indication is
 * received.
 *
 * This function cannot be used if transfer will result in EOT trigger. That is, if
 * the buffers can be partially filled for a complete transfer, then this function
 * CANNOT be used. In that case, bam_pipe_transfer must be used and the transfer must
 * be split in 64k chunks.
 *
 * The return value from this function may indicate that an error occurred.
 * Possible causes include invalid arguments.
 *
 * @param[in] pipe_handle  Handle to the pipe
 * @param[in] buf_pa       Physical address of buffer to transfer
 * @param[in] buf_size     Size in bytes of buffer to transfer
 * @param[in] xfer_opts    OR'd I/O vector option flags (see \ref bam_iovec_options_type )
 * @param[in] *user_data   User pointer that will be returned to user as part of
 *                         event payload
 *
 *
 * @return Status of the operation
 *        BAM_NOT_ENOUGH_SPACE - not enough space in descriptor fifo
 *        BAM_SUCCESS          - operation successfull
 *        BAM_FAILED           - operation failed
 *
 */
bam_status_type bam_pipe_bulktransfer(bam_handle pipehandle, uint64 buf_pa, uint32 buf_size, uint16 xfer_opts, void *user_data);

/**
 * @brief Gets the number of free transfer entries for a BAM Pipe
 *
 * This function returns the number of free transfer entries
 * for a BAM Pipe
 *
 * @param[in] pipe_handle  Pointer to client's handle to the BAM Pipe
 *
 * @return Number of free entries. < 0 indicates an error
 *
 */
uint32 bam_pipe_getfreecount(bam_handle pipehandle);

/**
 * @brief Determine whether a BAM Pipe's FIFO is empty
 *
 * This function returns the empty state of a BAM Pipe
 *
 * In bam2bam pipes, this api will sample the sates of both
 * producer and consumer pipes at the instance it was called
 * and makes a decision whether a pipes is empty or not. It's important
 * to call this api multiple times to make sure the bam2bam connection 
 * is truely empty.
 *
 * The producer periperals should be stopped from producing data before
 * calling this api.
 *
 * @param[in] pipe_handle  Pointer to client's handle to the BAM Pipe
 *
 * @return 1 - Empty 0 - Not Empty
 *
 * @note - In bam2bam pipes, this api will sample the sates of both
 *         producer and consumer pipes at the instance it was called
 *         and concludes whether a pipes is empty or not. 
 */
uint32 bam_pipe_isempty(bam_handle pipehandle);

/**
 * @brief Triggers a descriptor event on behalf of peer BAM
 *
 * This function triggers a descriptor event on behalf of peer BAM
 * in BAM2BAM configuration to clear producer pipe state
 *
 * @param pipehandle - Pointer to client's handle to the BAM Pipe
 *
 * @return Status of the operation
 *
 */
bam_status_type bam_pipe_trigger_event(bam_handle pipehandle);

/**
 * @brief Simulates  a zero length transfer( ZLT ) with EOT event from peer pipe in bam2bam mode
 *
 * This function proxies a ZLT EOT event from the peer pipe, so as to flush
 * bam pipe fifos.
 *
 * This api is only valid for pipes configured as:
 *    a. bam2bam
 *    b. consumer type
 *
 * The following pre-conditions apply for this api to have the required effect
 *    a. pipe is quiet. The caller of this api should guarantee that
 *       peer pipe will not generate new write events.
 *
 * @param pipehandle - Pointer to client's handle to the BAM Pipe
 *
 * @return Status of the operation
 *
 */
bam_status_type bam_pipe_trigger_zlt_event(bam_handle pipehandle);

/**
 * @brief Triggers a descriptor event on behalf of system producer
 *
 * This function generates descriptors and trigger descriptor events
 * on behalf of a peer system producer to clear producer pipe state.
 *
 * @param pipehandle - Pointer to client's handle to the BAM Pipe
 *
 * @return Status of the operation
 *                BAM_FAILED        - operation failed
 *                BAM_SUCCESS       - operation successfull
 *                BAM_PIPE_DISABLED - pipe is disabled
 *
 */
bam_status_type bam_pipe_trigger_event_sys(bam_handle pipehandle);

/**
 * @brief Polls for a result
 *
 * This function polls the pipe for a result. As a side-effect,
 * the callback registered via bam_pipe_init might be called
 * if a result is pending. Upon an error detected, the callback
 * registered via bam_init will get called. This function by
 * itself will not return any result if a callback was provided.
 *
 * @param[in]  pipe_handle  Pointer to client's handle to the BAM Pipe
 * @param[out] *result      Pointer to the structure that can hold the result.
 *                          If a callback was provided at pipe init, then
 *                          the callback will be invoked with the result and
 *                          this param will be set to null/invalid.
 *
 * @return Status of the operation.
 *              BAM_FAILED         - operation failed.
 *              BAM_SUCCESS        - operation successfull.
 *              BAM_POLL_NO_RESULT - valid result was not generated 
 * @note The Pipe should be operating in polling mode
 *
 */
bam_status_type bam_pipe_poll(bam_handle pipehandle, bam_result_type *result);

/**
 * @brief Gets the current descriptor in FIFO
 *
 * This function return information about the descriptor currently being 
 * processed in descriptor FIFO. if there is no pending descriptor 
 * then result->event is set to BAM_EVENT_INVALID
 *
 * @param[in]  pipe_handle  Pointer to client's handle to the BAM Pipe
 * @param[out] *result      Pointer to the structure that can hold the result.
 *
 * @return Status of the operation.
 *
 * @note the actual descriptor being processed may change anytime 
 *       (even before this function returns)
 *
 */
bam_status_type bam_pipe_get_current_descriptor_info(bam_handle pipehandle, bam_result_type *result);

/**
 * @brief Perform timer control
 *
 * This function performs timer control operations.
 *
 * @param[in]  pipe_handle  Pointer to client's handle to the BAM Pipe
 * @param[in]  *timer_ctrl  Pointer to timer control operation parameters
 * @param[out] *timer_rslt  Pointer to buffer for timer operation result.
 *    This argument can be NULL if no result is expected for the operation.
 *    If non-NULL, the current timer value will always provided.
 *
 * @return Status of the operation.
 *
 */
bam_status_type bam_pipe_timerctrl(bam_handle pipehandle, bam_timer_ctrl_type *timer_ctrl, bam_timer_rslt_type *timer_rslt);

/**
 * @brief bam_core_dump
 *
 * This function will provide Bam & bam pipe register dump. Client can use this API to get the dump at any time.
 * Before callinfg to this API, user has to call bam_get_info() API to get required dump buffer size.
 *
 * @param bamhandle[in]      Pointer to client's handle to BAM instance.
 * @param coredump_cfg[in]   Pointer to bam core dump config structure
 * 
 *
 * @return                   Status of the operation.
 *
 * @note  Make sure BAM is intialized before calling to this API.
 */
bam_status_type bam_core_dump(bam_handle bamhandle, bam_coredump_config_type *coredump_cfg);

/** @}  - bam_func_api */

#endif /* BAM_H_ */
