/**
  @file bam.c
  @brief
  This file contains the implementation of the interfaces that provide operational
  control to the BAM device.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.qdsp6/2.1/hwengines/bam/stub/bam_stub.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/26/12   SS      Created

===============================================================================
                   Copyright (c) 2012 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/



#include "bam.h"


/* This is done to shut linker error */
uint32 (*bamcore_pipe_tzprotect)(uint32 base, uint32 pipenum, uint32 vmid_index) = 0;

// Pipe memory parition table (version 0)
// --------------------------------------
// table header:
// word  0: 0x00434D50
// word  1: [31:24] 0x0
//          [23: 0] number of partiton elements
//
// partition elements list follows
// table header with no gaps
//
// partition element:
// word 0: producer pipe event register address
// word 1: consumer pipe event register address
// word 0: descriptors FIFO address
// word 0: data FIFO address address
// word 0: [31:16] data FIFO size
//         [15: 0] descriptors FIFO size
// --------------------------------------


/* BAM ISR processor */
void bam_isr(void *ctxt)
{
	return;
}


/**
 * @brief Inits the security configuration of the BAM
 *
 * Inits the security configuration of the BAM. This can be directly called if
 * and only if the execution environment is trustzone and does NOT intend to use
 * the BAM. This API ensures that no resources are allocated to the BAM and only
 * configures the security detail of that BAM
 *
 * @param bam_cfg - Pointer to the BAM config
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_secure_init(bam_config_type *bam_cfg)
{
    return BAM_FAILED;
}

/**
 * @brief Initialize BAM driver .
 * Initializes the BAM driver data structures and pipe memory,
 *
 * @return Status of the operation
 *
 * @note This API is not thread-safe
*/

bam_status_type bam_drv_init(void)
{
	return BAM_FAILED;
}

/**
 * @brief Initialize BAM driver .
 * Deinitializes the BAM driver data structures,
 *
 * @return Status of the operation
 *
 * @note This API is not thread-safe
*/

bam_status_type bam_drv_deinit(void)
{
    return BAM_FAILED;
}

/**
 * @brief Initialize BAM and configures it.
 * Initializes the BAM. Allocates a handle to the BAM,
 * which the client is expected to use for future calls to the driver.
 *
 * @param *handle - Pointer to be allocated for the BAM handle
 * @param bam_cfg  - Configuration data for BAM. This is peripheral specific
 * @param bam_cb - Callback to get any top level BAM events
 *
 * @return A handle to the BAM Device if successful
 *
*/

bam_handle bam_init(bam_config_type *bam_cfg, bam_callback_type *bam_cb)
{
   return (bam_handle)NULL;
}

/**
 * @brief Uninits the BAM if required & release the IRQ if allocated. Finally
 *        releases any allocations if made by the driver
 * Uninitializes the BAM HW if mentioned and frees up resources that are
 * allocated by the driver (IRQ, memory etc). This should be the last API to
 * be called, and any pipe disable must happen before this API is called.
 *
 * @param *handle - Pointer to the BAM handle
 * @param disable_bam  - Flag to indicate if the BAM HW must be disabled
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_deinit(bam_handle handle, uint8 disable_bam)
{
    return BAM_FAILED;
}

/**
 * @brief Simply resets the BAM hw and restores the config.
 *
 * Resets the BAM hw and restores the config provided via bam_init API. This
 * can be used to re-initialize a bam after the core was reset
 *
 * @param *handle - Pointer to the BAM handle
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_reset(bam_handle handle)
{
    return BAM_FAILED;
}


/**
 * @brief Enables/Disables the IRQ mode of the BAM Top Level
 *
 * Enables/Disables the IRQ mode of the BAM Top Level.
 *
 * @param *handle - Pointer to the BAM handle
 * @param irq_en  - Enable (1)/ Disable (0) the irq mode
 * @param irq_mask - IRQs to be enabled for the BAM
 *
 * @return Status of the operation
 *
 * @note This API only sets the IRQ mode for the BAM and NOT the individual pipes
*/
bam_status_type bam_setirqmode(bam_handle bamhandle, uint32 irq_en,uint32 irq_mask)
{
   return BAM_FAILED;
}

/**
 * @brief Initializes pipe pipe_num in the BAM based on the config provided.
 *
 * Enables pipe pipe_num in the BAM based on the pipe_cfg provided. Returns
 * a handle to the pipe allocated to the client to be used for future pipe
 * operations
 *
 * @param *bamhandle - Pointer to the handle to the BAM from which the pipe
 *                     is to be allocated
 * @param pipe_num - Pipe to be allocated in the bam.
 * @param bam_cfg  - Configuration data for the pipe. This is connection specific
 * @param pipe_cb  - Callback to be notified of Pipe level events
 *
 * @return A handle to the pipe allocated successfully
 *
*/

bam_handle bam_pipe_init(bam_handle bamhandle, uint32 pipe_num, bam_pipe_config_type *pipe_cfg, bam_callback_type *pipe_cb)
{
    return (bam_handle)NULL;
}

/**
 * @brief Uninitializes the pipe.
 *
 * Uninitializes a pipe and disables it. No further operations
 * can be performed on the pipe, and the handle is invalidated.
 *
 * @param *pipehandle - handle to the pipe to be uninitialized
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_pipe_deinit(bam_handle pipehandle)
{
    return BAM_FAILED;
}

/**
 * @brief Enables a pipe.
 *
 * Enables a pipe for transfers
 *
 * @param *pipehandle - handle to the pipe to be enabled
 *
 * @return Status of the operation
 *
 * @dependencies bam_pipe_init must have succeeded
 *
*/
bam_status_type bam_pipe_enable(bam_handle pipehandle)
{
    return  BAM_FAILED;
}

/**
 * @brief Disables a pipe.
 *
 * Disables a pipe preventing further transfers
 *
 * @param *pipehandle - handle to the pipe to be disabled
 *
 * @return Status of the operation
 *
 * @dependencies bam_pipe_init must have succeeded
 *
*/

bam_status_type bam_pipe_disable(bam_handle pipehandle)
{
    return BAM_FAILED;
}


/**
 * @brief Enables/Disables the IRQ mode of a pipe.
 *
 * Enables/Disables the IRQ mode of a pipe.
 *
 * @param *handle - handle to the pipe
 * @param irq_en  - Enable (1)/ Disable (0) the irq mode
 * @param pipe_cfg_opts - IRQs to be enabled for the BAM. See bam_options_type
 *
 * @return Status of the operation
 *
 * @note This API only sets the IRQ mode for the individual pipes and NOT the BAM.
*/
bam_status_type bam_pipe_setirqmode(bam_handle pipehandle, uint32 irq_en, uint32 pipe_cfg_opts)
{
    return BAM_FAILED;
}

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
 * @param pipe_handle - Handle to the pipe
 *
 * @param buf_pa - Physical address of buffer to transfer
 *
 * @param buf_size - Size in bytes of buffer to transfer
 *
 * @param xfer_opts - OR'd I/O vector option flags (see BAM_IOVEC_FLAG defines)
 *
 * @param user_data - User pointer that will be returned to user as part of
 *    event payload
 *
 *
 * @return Status of the operation
 *
 */
bam_status_type bam_pipe_transfer(bam_handle pipehandle, uint32 buf_pa, uint16 buf_size, uint16 xfer_opts, void *user_data)
{
    return BAM_FAILED;
}

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
 * @param pipe_handle - Handle to the pipe
 *
 * @param buf_pa - Physical address of buffer to transfer
 *
 * @param buf_size - Size in bytes of buffer to transfer
 *
 * @param xfer_opts - OR'd I/O vector option flags (see BAM_IOVEC_FLAG defines)
 *
 * @param user_data - User pointer that will be returned to user as part of
 *    event payload
 *
 *
 * @return Status of the operation
 *
 */
bam_status_type bam_pipe_bulktransfer(bam_handle pipehandle, uint32 buf_pa, uint32 buf_size, uint16 xfer_opts, void *user_data)
{
   return BAM_FAILED;
}

/**
 * @brief Gets the number of free transfer entries for a BAM Pipe
 *
 * This function returns the number of free transfer entries
 * for a BAM Pipe
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 *
 * @return Number of free entries. < 0 indicates an error
 *
 */
uint32 bam_pipe_getfreecount(bam_handle pipehandle)
{
    return 0;

}

/**
 * @brief Determine whether a BAM Pipe's FIFO is empty
 *
 * This function returns the empty state of a BAM Pipe
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 *
 * @return 1 - Empty 0 - Not Empty
 *
 */
uint32 bam_pipe_isempty(bam_handle pipehandle)
{
     return 0;
}

/**
 * @brief Polls for a result
 *
 * This function polls the pipe for a result. As a side-effect,
 * the callback registered via bam_pipe_init might be called
 * if a result is pending. Upon an error detected, the callback
 * registered via bam_init will get called. This function by
 * itself will not return any result if a callback was provided.
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 * @param[out] result - Pointer to the structure that can hold the result.
 *                      If a callback was provided at pipe init, then
 *                      the callback will be invoked with the result and
 *                      this param will be set to null/invalid.
 *
 * @return Status of the operation.
 *
 * @note The Pipe should be operating in polling mode
 *
 */
bam_status_type bam_pipe_poll(bam_handle pipehandle, bam_result_type *result)
{
    return BAM_FAILED;
}

/**
 * @brief Perform timer control
 *
 * This function performs timer control operations.
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 * @param timer_ctrl - Pointer to timer control operation parameters
 * @param [out] timer_rslt - Pointer to buffer for timer operation result.
 *    This argument can be NULL if no result is expected for the operation.
 *    If non-NULL, the current timer value will always provided.
 *
 * @return Status of the operation.
 *
 */
bam_status_type bam_pipe_timerctrl(bam_handle pipehandle, bam_timer_ctrl_type *timer_ctrl, bam_timer_rslt_type *timer_rslt)
{
   return BAM_FAILED;
}

/**
 * @brief bam_core_dump
 *
 * This function get the Bam & pipe register dump after error condition.
 *
 * @param bamhandle   	- Pointer to client's handle to BAM.
 * @param coredump_cfg    - Pointer to bam core dump Config structure.
 * 
 * @return 				- None.
 *
 * Note: Before calling this API, use API bam_get_info() to find out required core dump buffer size.
 *
 */
bam_status_type bam_core_dump(bam_handle bamhandle, bam_coredump_config_type *coredump_cfg)
{
	return BAM_FAILED;
}

/**
  * @brief Bam get info
  *
  * This API will provide pipe info for given bam instance.
  *
  * @param bamhandle	bam handle
  * @param info			to get bam dump buffer
  *
  *
  */
bam_status_type bam_get_info(bam_handle bamhandle, bam_info_type *info)
{

   return BAM_FAILED;
}



