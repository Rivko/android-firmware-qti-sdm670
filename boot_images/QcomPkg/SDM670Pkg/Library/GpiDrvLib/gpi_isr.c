/**
  @file  gpi_isr.c
  @brief GPI interrupt handler.
*/
/*
===============================================================================

                               Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/15/17   ah      Sync to common GPI driver
04/21/17   ah      Added memory barrier.
02/19/16   ah      File created.

===============================================================================
              Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include "gpi_utils.h"


static gpi_ctxt *glob_ctxt = NULL;


/**
 * @brief Complete the execution of remaining work if in polling mode,
 *        or defer to a task or worker thread if in ISR context.
 *
 * @param[in ]     gpii        GPII context
 * @param[in ]     handler     handling thread
 * @param[in ]     evt         Event
 */
void dispatch_worker(gpii_info *gpii, gpi_ring_elem *evt)
{
   if (evt != NULL)
   {
      if (TRUE != en_q(&gpii->gpi->gpi_thread.q, evt))
         return;
   }

   if (gpii->gpi->exec_mode == GPI_POLLING)
   {
      /* Complete the handling of this event */
      handle_gpi_evts(gpii->gpi);
   }
   else
   {
      /* Defer further execution to a worker thread/task */
      gpi_signal_worker(gpii->gpi);
   }
}

/**
 * @brief Handle channle control
 *
 * @param[in ]     gpii        GPII context
 */
void handle_chan_ctrl(gpii_info *gpii, uint32 chan, U_LONG gsi_pa)
{
   gpi_ring_elem evt;
   uint32 chan_state;

   evt.dword_2 = 0;
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GPII_ID, gpii->id);
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_CHID, chan);
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GSI_INST, gpii->qup_type);

   evt.ctrl = GPI_IRQ_CH_CTRL;

   chan_state = HWIO_EE_n_GSI_CH_k_CNTXT_0_INMI2(gsi_pa, gpii->id, chan,
                                                       HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_BMSK) >>
                                                       HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_SHFT;

         /* Save the new channel state for later handling by the helper */
         GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_CHAN_STATE, chan_state);

         dispatch_worker(gpii, &evt);
}

/**
 * @brief Handle event ring control
 *
 * @param[in ]     gpii        GPII context
 */
void handle_evt_ctrl(gpii_info *gpii, U_LONG gsi_pa)
{
   gpi_ring_elem evt;
   uint32 chan_state;

   evt.dword_2 = 0;
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GPII_ID, gpii->id);
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GSI_INST, gpii->qup_type);

   evt.ctrl = GPI_IRQ_EV_CTRL;

   chan_state = HWIO_EE_n_EV_CH_k_CNTXT_0_INMI2(gsi_pa, gpii->id, 0,
                                                HWIO_EE_n_EV_CH_k_CNTXT_0_CHSTATE_BMSK) >>
                                                HWIO_EE_n_EV_CH_k_CNTXT_0_CHSTATE_SHFT;

   /* Save new state for later handling by the helper */
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_CHAN_STATE, chan_state);

   dispatch_worker(gpii, &evt);
}

/**
 * @brief Handle global events
 *
 * @param[in ]     gpii        GPII context
 */
void handle_global(gpii_info *gpii, U_LONG gsi_pa)
{
   gpi_ring_elem evt;

   evt.dword_2 = 0;
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GPII_ID, gpii->id);
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GSI_INST, gpii->qup_type);
      /* Track the first error that occurred in this GPII, helper will handle it */
      GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_ERROR,
                      HWIO_EE_n_ERROR_LOG_INMI(gsi_pa, gpii->id,
                                               HWIO_EE_n_ERROR_LOG_ERROR_LOG_BMSK) >>
                      HWIO_EE_n_ERROR_LOG_ERROR_LOG_SHFT);
   evt.ctrl = GPI_IRQ_GLOB_EE;

      dispatch_worker(gpii, &evt);

      /* Clear it */
      HWIO_EE_n_ERROR_LOG_OUTI(gsi_pa, gpii->id, 0);
      HWIO_EE_n_ERROR_LOG_CLR_OUTI(gpii->gpi->qup[gpii->qup_type].gsi_pa, gpii->id, ~0u);
}

/**
 * @brief Handle IEOB completions
 *
 * @param[in ]     gpii        GPII context
 */
void handle_ieob(gpii_info *gpii)
{
   gpi_syncenter(&gpii->gpi->gpi_mutex);
   gpii->gpi->gsi_evt_ring_bmsk |= (1 << (get_gpii_idx(gpii->gpi, gpii->id, gpii->qup_type)));
   gpi_syncleave(&gpii->gpi->gpi_mutex);

   dispatch_worker(gpii, NULL);
}

/**
 * @brief Handle inter-EE channel control
 *
 * @param[in ]     gpii        GPII context
 */
void handle_inter_ee_chan_ctrl(gpii_info *gpii, U_LONG gsi_pa, uint32 chan)
{
   gpi_ring_elem evt;

   evt.dword_2 = 0;
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GPII_ID, gpii->id);
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GSI_INST, gpii->qup_type);
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_CHID, chan);
   evt.ctrl = GPI_IRQ_INTER_EE_CH_CTRL;

            /* SW expected to figure the new channel state based on
               the control operation it performed on the channel */
            GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_CHAN_STATE,
                   HWIO_EE_n_GSI_CH_k_CNTXT_0_INMI2(gsi_pa, gpii->id, chan,
                                                             HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_BMSK) >>
                            HWIO_EE_n_GSI_CH_k_CNTXT_0_CHSTATE_SHFT);

            dispatch_worker(gpii, &evt);
}

/**
 * @brief Handle inter-EE event ring control
 *
 * @param[in ]     gpii        GPII context
 */
void handle_inter_ee_evt_ctrl(gpii_info *gpii, U_LONG gsi_pa)
{
   uint32 bmsk;
   gpi_ring_elem evt;

   evt.dword_2 = 0;
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GPII_ID, gpii->id);
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GSI_INST, gpii->qup_type);

   evt.ctrl = GPI_IRQ_INTER_EE_EV_CTRL;

   /* Figure which event channel state changed in which EE */
   bmsk = HWIO_INTER_EE_n_SRC_EV_CH_IRQ_INMI(gsi_pa, gpii->id,
                                             HWIO_INTER_EE_n_SRC_EV_CH_IRQ_EV_CH_BIT_MAP_BMSK) >>
                                             HWIO_INTER_EE_n_SRC_EV_CH_IRQ_EV_CH_BIT_MAP_SHFT;

   if (bmsk)
   {
      /* Figure the new channel state based on the control operation it performed,
         save new state for later handling by the helper */
      GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_CHAN_STATE,
                      HWIO_EE_n_EV_CH_k_CNTXT_0_INMI2(gsi_pa, gpii->id, 0,
                                                      HWIO_EE_n_EV_CH_k_CNTXT_0_CHSTATE_BMSK) >>
                      HWIO_EE_n_EV_CH_k_CNTXT_0_CHSTATE_SHFT);
      dispatch_worker(gpii, &evt);
   }
}

/**
 * @brief Handle general events
 *
 * @param[in ]     gpii        GPII context
 */
void handle_general(gpii_info *gpii, uint32 bmsk)
{
   gpi_ring_elem evt;

   evt.dword_2 = 0;
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GPII_ID, gpii->id);
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_GSI_INST, gpii->qup_type);

   evt.ctrl = GPI_IRQ_GENERAL;

   /* Save the interrupt type for later handling */
   GPI_WRITE_FIELD(evt.dword_2, LOCAL_EVT_INT_TYPE, bmsk);

   dispatch_worker(gpii, &evt);
}

/**
 * @brief Handle IRQ, gets the IRQ type and handles it
 *
 * @param[in ]     gpii        GPII context
 */
void handle_irq(gpii_info *gpii)
{
   volatile GPI_IRQ_TYPE irq_type = 0;
   volatile uint32 bmsk = 0;
   uint32 i;
   U_LONG gsi_pa = gpii->gpi->qup[gpii->qup_type].gsi_pa;

   /* Get the IRQ type */
   irq_type = (GPI_IRQ_TYPE)HWIO_EE_n_CNTXT_TYPE_IRQ_INI(gsi_pa, gpii->id);

   /* Channel control interrupt */
   if (irq_type & GPI_IRQ_CH_CTRL)
   {
      /* Get a Bit map of channels whose state has changed */
      bmsk = (HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_INMI(gsi_pa, gpii->id,
                                                  HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_GSI_CH_BIT_MAP_BMSK)) >>
             HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_GSI_CH_BIT_MAP_SHFT;

      for (i = 0; i < NUM_RINGS_PER_GPII; i++)
      {
         if (bmsk & (1 << i))
         {
            /* Clear corresponding bit */
            HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_OUTI(gsi_pa, gpii->id, (1 << i));
            gpi_memorybarrier();
            handle_chan_ctrl(gpii, i, gsi_pa);
            break;
         }
      }
   }

   /* Event control interrupt */
   if (irq_type & GPI_IRQ_EV_CTRL)
   {
      bmsk = HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_INMI(gsi_pa, gpii->id, HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_EV_CH_BIT_MAP_BMSK);
      if (bmsk & 1)
      {
         /* Clear corresponding bit */
         HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_OUTI(gsi_pa, gpii->id, 1);
         gpi_memorybarrier();
         handle_evt_ctrl(gpii, gsi_pa);
      }
   }

   /* Global interrupt */
   if (irq_type & GPI_IRQ_GLOB_EE)
   {
      /* Figure out what kind of global interrupt occurred */
      bmsk = HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_INMI(gsi_pa, gpii->id,
                                                HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ERROR_INT_BMSK) >>
             HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ERROR_INT_SHFT;

      /* Currently only error interrupt is supported */
      if (bmsk & HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ERROR_INT_BMSK)
      {
         /* Clear corresponding bit */
         HWIO_EE_n_CNTXT_GLOB_IRQ_CLR_OUTI(gsi_pa, gpii->id, 1);
         gpi_memorybarrier();
         handle_global(gpii, gsi_pa);
      }
   }

   /* Event interrupts, associated with specific event element */
   if (irq_type & GPI_IRQ_IEOB)
   {
      bmsk = HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_INMI(gsi_pa, gpii->id,
                                               HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_EV_CH_BIT_MAP_BMSK) >>
             HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_EV_CH_BIT_MAP_SHFT;

      if (bmsk & 1)
      {
         HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_OUTI(gsi_pa, gpii->id,
                                               HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_CLR_EV_CH_BIT_MAP_BMSK);
         gpi_memorybarrier();
         handle_ieob(gpii);
      }
   }

   /* External channel control interrupt */
   if (irq_type & GPI_IRQ_INTER_EE_CH_CTRL)
   {
      /* Figure which channel state changed in which EE. */
      bmsk = HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_INMI(gsi_pa, gpii->id,
                                                 HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_GSI_CH_BIT_MAP_BMSK) >>
             HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_GSI_CH_BIT_MAP_SHFT;

      if (bmsk)
      {
         /* In this case there is no additional channel state information */
         for (i = 0; i < NUM_RINGS_PER_GPII; i++)
         {
            if (bmsk & (1 << i))
            {
               /* Clear */
               HWIO_INTER_EE_n_SRC_GSI_CH_IRQ_CLR_OUTI(gsi_pa, gpii->id, (1 << i));
               gpi_memorybarrier();
               handle_inter_ee_chan_ctrl(gpii, gsi_pa, i);
               break;
            }
         }
      }
   }

   /* External evnet control interrupt */
   if (irq_type & GPI_IRQ_INTER_EE_EV_CTRL)
   {
      bmsk = HWIO_INTER_EE_n_SRC_EV_CH_IRQ_INI(gsi_pa, gpii->id);

      if (bmsk)
      {
         /* Clear */
         HWIO_INTER_EE_n_SRC_EV_CH_IRQ_CLR_OUTI(gsi_pa, gpii->id, 1);
         gpi_memorybarrier();
         handle_inter_ee_evt_ctrl(gpii, gsi_pa);
      }
   }

   /* General interrupt */
   if (irq_type & GPI_IRQ_GENERAL)
   {
      bmsk = HWIO_EE_n_CNTXT_GSI_IRQ_STTS_INI(gsi_pa, gpii->id);

      if (bmsk)
      {
         /* Clear */
         HWIO_EE_n_CNTXT_GSI_IRQ_CLR_OUTI(gsi_pa, gpii->id, HWIO_EE_n_CNTXT_GSI_IRQ_CLR_RMSK);
         gpi_memorybarrier();
         handle_general(gpii, bmsk);
      }
   }
}

/**
 * @brief GPI interrupt handler
 *
 * @param[in ]
 */
void gpi_isr(void *ctxt)
{
   uint8           i;
   QUP_BLOCK_TYPE  qup_type = MAX_NUM_QUP_BLOCKS;

   if (glob_ctxt != NULL)
   {
      qup_type = (QUP_BLOCK_TYPE)ctxt;

      if (qup_type >= QUP_0 && qup_type < MAX_NUM_QUP_BLOCKS)
      {
      /* Investigate all GPII's belonging to this QUP */
      for (i = 0; i < glob_ctxt->num_gpii; i++)
      {
         if (glob_ctxt->gpii[i].qup_type == qup_type &&
             glob_ctxt->gpii[i].registered == TRUE)
         {
            handle_irq(&glob_ctxt->gpii[i]);
         }
      }
   }
   }
   else
   {
      handle_irq((gpii_info *)ctxt);
   }
}

/**
 * @brief Registers GPI interrupt handler
 *
 * @param[in ]        gpii           GPII instance
 */
GPI_RETURN_STATUS register_irq(gpii_info *gpii)
{
   /* Register IRQ handler */
   return gpi_isrinstall(gpii->irq, gpi_isr, (void *)gpii, gpii->gpi->hGpiDALInterrupt);
}

/**
 * Installs ISRs for all muxed IRQ's, one IRQ per QUPv3.
 * Called in case of muxed IRQ's only.
 *
 * @param[in ]        gpi           GPI context
 * 
 * @return   None.
 */
GPI_RETURN_STATUS register_irq_all(gpi_ctxt *gpi)
{
   uint8 i;

   glob_ctxt = gpi;

   for (i = 0; i < MAX_NUM_QUP_BLOCKS; i++)
   {
      if (gpi->qup[i].active)
      {
         if (GPI_STATUS_SUCCESS != gpi_isrinstall(gpi->qup[i].qup_irq, gpi_isr, 
                                                  (void *)gpi->qup[i].type, gpi->hGpiDALInterrupt))
         {
            return GPI_STATUS_ERROR;
         }
      }
   }

   return GPI_STATUS_SUCCESS;
}
