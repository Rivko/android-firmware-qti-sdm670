#ifndef __GPI_UTILS_H__
#define __GPI_UTILS_H__

/**
 * @file  gpi_utils.h
 * @brief Provides utility function definitions.
 */
/*
===============================================================================

Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
02/19/16   ah      File created.

===============================================================================
              Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include "gpi_i.h"


/**
 * @brief Resets a GPI event queue.
 *
 * @param[in ]        q             GPI work queues
 */
void reset_evt_q(gpi_work_q *q);

/**
 * @brief Returns the number of free ring elements.
 *
 * @param[in ]
 * @param[out]
 */
uint32 nbr_avail_ring_elem(gpii_info *gpii, uint8 chan, U_LONG base,
                           U_LONG wp, U_LONG rp, U_LONG size);

/**
 * @brief Queue an element to a Work queue.
 *
 * @param[in]        Queue             Work queue
 * @param[in]        elem              Work queue element
 */
boolean en_q(gpi_work_q *q, gpi_ring_elem *elem);

/**
 * @brief Retrieves a work queue element.
 *
 * @param[in ]        Queue             Work queue
 * @param[in,out ]    elem              Work queue element
 */
boolean de_q(gpi_work_q *q, gpi_ring_elem * elem);

/**
 * @brief Resets a GPI event queue.
 *
 * @param[in ]        q             GPI work queues
 */
void reset_evt_q(gpi_work_q *q);

/**
 * @brief Handles all GPI events
 *
 * @param[in ]        gpi           GPI context
 */
void handle_gpi_evts(gpi_ctxt *gpi);

/**
 * @brief Registers GPI interrupt handler
 *
 * @param[in ]        gpii           GPII instance
 */
GPI_RETURN_STATUS register_irq(gpii_info *gpii);

/**
 * Installs ISRs for all muxed IRQ's, one IRQ per QUPv3.
 * Called in case of muxed IRQ's only.
 *
 * @param[in ]        gpi           GPI context
 */
GPI_RETURN_STATUS register_irq_all(gpi_ctxt *gpi);

/**
 * @brief Spawn GPI Threads.
 *
 * @param[in,out ]     gpi_ctxt           GPI context
 */
#ifdef GPI_MT
GPI_RETURN_STATUS spawn_threads(gpi_ctxt *gpi);
#else
#define spawn_threads(...)  GPI_STATUS_SUCCESS
#endif

/**
 * @brief Initializes GPI synchronization objects
 *
 * @param[in]    gpi                   Pointer to GPI context.
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
#ifdef GPI_MT
GPI_RETURN_STATUS init_gpi_sync(gpi_ctxt *gpi);
#else
#define init_gpi_sync(...)  GPI_STATUS_SUCCESS
#endif

/**
 * @brief Initializes GPI threads
 *
 * @param[in]    gpi                   Pointer to GPI context.
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
#ifdef GPI_MT
GPI_RETURN_STATUS init_threads(gpi_ctxt *gpi);
#else
#define init_threads(...)  GPI_STATUS_SUCCESS
#endif

/**
 * @brief Stop GPI threads
 *
 * @param[in,out ]     gpi_ctxt        GPI context
 * @param[in ]         terminate       Terminate threads if TRUE, otherwise stops them
 * @param[in ]         wait            Wait time in msec
 *
 * @return       GPI_RETURN_STATUS     Return status.
 *
 */
#ifdef GPI_MT
GPI_RETURN_STATUS stop_thread(gpi_ctxt *gpi, boolean terminate, uint32 wait);
#else
#define stop_threads(...)   GPI_STATUS_SUCCESS
#endif

/**
 * Registers a GPI task.
 *
 * @param    None
 *
 * @return       None.
 */
#ifdef GPI_ST
void gpi_reg_task(gpi_ctxt *gpi);
#else
#define gpi_reg_task(...)
#endif

/** Reads a channel ctxt rp
 */
GPI_CHAN_STATE get_chan_state(gpii_info *gpii, uint8 chan);

/**
 * @brief Signal a worker thread or task
 *
 * @param[in,out ]     gpi_ctxt           GPI context
 *
 * @return       None.
 *
 */
void gpi_signal_worker(gpi_ctxt *gpi);

/** Reads a ring ctxt pointer
 */
U_LONG get_ring_ptr(gpii_info *gpii, uint8 chan,
                      GPI_RING_PTR_TYPE ptr_type);

/**
 * @brief GPI interrupt handler
 *
 * @param[in ]
 */
void gpi_isr(void *gpii_ctxt);

/**
 * @brief Initializes a specific channel registers.
 *
 * @param[in ]        gpii           gpii context
 * @param[in ]        chan_type      inbound/outbound channel type
 */
void init_chan_regs(gpii_info *gpii, GPI_CHAN_TYPE chan_type);

/**
 * @brief Initializes a specific event ring registers.
 *
 * @param[in ]        gpii           gpii context
 */
void init_evt_ring_regs(gpii_info *gpii);

/**
 * @brief Given a GPII ID return the GPII index if it exists
 *
 * @param[in,out ]     gpi_ctxt           GPI context
 *
 * @return       index if a GPII with the same ID exists, 0xFF otherwise.
 *
 */
uint8 get_gpii_idx(gpi_ctxt *gpi, uint8 gpii_id, QUP_BLOCK_TYPE qup_type);

#endif  /* __GPI_UTILS_H__ */
