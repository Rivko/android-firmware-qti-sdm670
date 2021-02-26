/**
  @file  gpi_i.c
  @brief GPI internal APIs.

*/
/*
===============================================================================

                               Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
06/15/17   ah      Sync to common GPI driver.
02/19/16   ah      File created.

===============================================================================
              Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include "gpi_utils.h"


/**
 * Issues a GPI command.
 *
 * @param[in]    gpii_id         GPI Interface ID
 * @param[in]    chan            Virtual Channel ID
 * @param[in]    cmd_type        Command type
 *
 * @return       GPI_RETURN_STATUS   Return status.
 */
GPI_RETURN_STATUS issue_cmd(gpii_info *gpii,
                            GPI_CHAN_TYPE chan,
                            GPI_CHAN_CMD cmd_type)
{
   uint32 cmd = 0;
   GPI_RETURN_STATUS status = GPI_STATUS_SUCCESS;
   GPI_CHAN_STATE curr_state = (chan == GPI_EVT_RING) ?
                            gpii->evt_ring_info.state :
                            gpii->chan_info[chan].state;
   uint32 cmd_opcode_shft = (chan == GPI_EVT_RING) ?
                            HWIO_EE_n_EV_CH_CMD_OPCODE_SHFT :
                            HWIO_EE_n_GSI_CH_CMD_OPCODE_SHFT;

   gpi_log(GPI_DEBUG, "issue_cmd() <-\n");
   gpi_log(GPI_DEBUG, "gpii ID: %d, chan: %d, cmd_type: %d\n",
           gpii->id, chan, cmd_type);

   if (chan == GPI_EVT_RING              &&
       cmd_type != GPI_CHAN_CMD_ALLOCATE &&
       cmd_type != GPI_CHAN_CMD_DE_ALLOC)
   {
      gpi_log(GPI_ERROR, "ERROR - command: %d not allowed for event rings\n", cmd_type);
      return GPI_STATUS_ERROR;
   }

   gpi_syncenter(&gpii->gpii_mutex);

   /* EEs are limited to a single outstanding CMD for both
      event channel/transfer channel */
   if (gpii->pending_cmd != GPI_CHAN_CMD_NONE)
   {
      gpi_log(GPI_ERROR, "ERROR - pending command: %d for GPII ID: %d\n",
              gpii->pending_cmd, gpii->id);
      gpi_syncleave(&gpii->gpii_mutex);
      return GPI_STATUS_ERROR;
   }

   /* Validate the requested state according to the chan state machine */
      switch (cmd_type)
      {
         case GPI_CHAN_CMD_ALLOCATE:
            if (curr_state != GPI_CHAN_DISABLED) 
            {
               gpi_log(GPI_ERROR, "ERROR - CMD_ALLOCATE, current state: %d\n",
                       curr_state);
               status = GPI_STATUS_ERROR;
            }
            break;
         case GPI_CHAN_CMD_START:
            if (curr_state != GPI_CHAN_ALLOCATED && 
                curr_state != GPI_CHAN_STOPPED &&
                curr_state != GPI_CHAN_STOP_IN_PROG)
            {
               gpi_log(GPI_ERROR, "ERROR - CMD_START, current state: %d\n",
                       curr_state);
               status = GPI_STATUS_ERROR;
            }
            break;
         case GPI_CHAN_CMD_STOP:
            if (curr_state == GPI_CHAN_DISABLED ||
                curr_state == GPI_CHAN_ALLOCATED)
            {
               gpi_log(GPI_ERROR, "ERROR - CMD_STOP, current state: %d\n",
                       curr_state);
               status = GPI_STATUS_ERROR;
            }
            break;
         case GPI_CHAN_CMD_RESET:
            /* Reset should not be called from Disabled or Allocated state,
             * this causes IPA endpoint to be reset before it was enabled.
             * After reset, channel should be reconfigured before using it again
             */
            if (curr_state != GPI_CHAN_STOPPED)
            {
               gpi_log(GPI_ERROR, "ERROR - CMD_RESET, current state: %d\n",
                       curr_state);
               status = GPI_STATUS_ERROR;
            }
            break;
         case GPI_CHAN_CMD_DE_ALLOC:
            if (curr_state != GPI_CHAN_ALLOCATED)
            {
               gpi_log(GPI_ERROR, "ERROR - CMD_DE_ALLOC, current state: %d\n",
                       curr_state);
               status = GPI_STATUS_ERROR;
            }
            break;
         case GPI_CHAN_CMD_DB_STOP:
            if (curr_state == GPI_CHAN_DISABLED  ||
                curr_state == GPI_CHAN_ALLOCATED ||
                chan == GPI_EVT_RING)
            {
               gpi_log(GPI_ERROR, "ERROR - CMD_DB_STOP, current state: %d\n",
                       curr_state);
               status = GPI_STATUS_ERROR;
            }
            break;

         case QUP_CMD_GSI_STOP_ON_EOT:
         case QUP_CMD_UART_SW_STALE:
         case QUP_CMD_UART_RFR_READY:
         case QUP_CMD_UART_RFR_NOT_READY:
         case QUP_CMD_UART_BREAK_START:
         case QUP_CMD_UART_BREAK_STOP:
            /* For UART commands we don't do any protocol specific checking... */
            break;

         default:
            gpi_log(GPI_ERROR, "ERROR - default cmd_type: %d\n", cmd_type);
            status = GPI_STATUS_ERROR;
      }

   if (status != GPI_STATUS_ERROR)
   {
      cmd = cmd_type << cmd_opcode_shft;
      gpii->pending_cmd = cmd_type;

      if (chan != GPI_EVT_RING)
      {
         cmd |= chan;
         HWIO_EE_n_GSI_CH_CMD_OUTI(gpii->gpi->qup[gpii->qup_type].gsi_pa, gpii->id, cmd);
         gpi_log(GPI_DEBUG, "Chan cmd issued\n");
      }
      else
      {
         HWIO_EE_n_EV_CH_CMD_OUTI(gpii->gpi->qup[gpii->qup_type].gsi_pa, gpii->id, cmd);
         gpi_log(GPI_DEBUG, "Evt ring cmd issued\n");
      }
   }

   gpi_syncleave(&gpii->gpii_mutex);

   gpi_log(GPI_DEBUG, "issue_cmd() ->\n");
   return status;
}

#ifdef GPI_MT
/**
 * @brief Initializes GPI synchronization objects
 *
 * @param[in]    gpi                   Pointer to GPI context.
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS init_gpi_sync(gpi_ctxt *gpi)
{
   uint32 i;

   gpi_log(GPI_DEBUG, "init_gpi_sync() <-\n");

   if (GPI_STATUS_SUCCESS != gpi_syncinit(&gpi->gpi_mutex))
      return GPI_STATUS_ERROR;

   for (i = 0; i < gpi->num_gpii; i++)
   {
      if (GPI_STATUS_SUCCESS != gpi_syncinit(&gpi->gpii[i].gpii_mutex))
         return GPI_STATUS_ERROR;
   }

   gpi_log(GPI_DEBUG, "init_gpi_sync() ->\n");
   return GPI_STATUS_SUCCESS;
}

/**
 * @brief Initializes GPI threads
 *
 * @param[in]    gpi                   Pointer to GPI context.
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS init_threads(gpi_ctxt *gpi)
{
   gpi_log(GPI_DEBUG, "init_threads() <-\n");

   gpi->gpi_thread.priority = GPI_TASK_PRI;
   gpi->gpi_thread.state = GPI_THREAD_INITIAL;
   gpi_eventInit(&gpi->gpi_thread.thread.hEvent);
   reset_evt_q(&gpi->gpi_thread.q);
   if (GPI_STATUS_SUCCESS != gpi_syncinit(&gpi->gpi_thread.q.lock))
   {
      gpi_log(GPI_ERROR, "ERROR - Thread gpi_syncinit() failed!\n");
      return GPI_STATUS_ERROR;
   }

   gpi_log(GPI_DEBUG, "init_threads() ->\n");
   return GPI_STATUS_SUCCESS;
}
#endif

/**
 * @brief Set GPII specific parameters all GPI rings
 *
 * @param[in]       gpii           Pointer to GPII context.
 * @param[in]       params         Client submitted parameters
 * @param[in]       chan_type      Channel type
 *  
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS set_gpii_chan(gpii_info *gpii,
                                gpi_iface_params *params,
                                GPI_CHAN_TYPE chan_type)
{
   gpi_log(GPI_DEBUG, "set_gpii_chan() <-\n");

   gpii->chan_info[chan_type].state = GPI_CHAN_DISABLED;
   gpii->chan_info[chan_type].base_pa = params->chan[chan_type].ring_base_pa;
   gpii->chan_info[chan_type].base_va = params->chan[chan_type].ring_base_va;
   gpii->chan_info[chan_type].num_elem = params->chan[chan_type].tre_ring_size;
   gpii->chan_info[chan_type].local_rp = gpii->chan_info[chan_type].base_pa;

   gpi_log(GPI_DEBUG, "set_gpii_chan() ->\n");
   return GPI_STATUS_SUCCESS;
}

/**
 * @brief Set GPII specific parameters all GPI rings
 *
 * @param[in]       gpii               Pointer to GPII context.
 *  
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS set_gpii_evt_chan(gpii_info *gpii)
{
   gpi_log(GPI_DEBUG, "set_gpii_evt_chan() <-\n");

   gpii->evt_ring_info.base_va = (U_LONG)&gpii->evt_ring_elem[0];
   gpii->evt_ring_info.base_pa = (U_LONG)gpi_va_to_pa((void *)gpii->evt_ring_info.base_va);
   gpii->evt_ring_info.local_rp = gpii->evt_ring_info.base_pa;
   gpii->evt_ring_info.state = GPI_CHAN_DISABLED;
   gpii->evt_ring_info.num_elem = NUM_GPI_EVT_RING_ELEM;

   gpi_log(GPI_DEBUG, "set_gpii_evt_chan() ->\n");
   return GPI_STATUS_SUCCESS;
}

/**
 * @brief Set GPII specific parameters all GPI rings
 *
 * @param[in,out ]     gpi_ctxt           GPI context
 * @param[in]          gpii              Pointer to GPII context.
 * @param[in,out]      params            Client submitted parameters
 */
GPI_RETURN_STATUS set_gpii_params(gpi_ctxt *gpi, gpii_info *gpii, gpi_iface_params *params)
{
   gpi_log(GPI_DEBUG, "set_gpii_params() <-\n");

   params->gpi_handle = (void *)gpii;
   gpii->qup_type = params->qup_type;
   gpii->int_modt_val = params->int_modt_val;
   gpii->int_modc_val = params->int_modc_val;
   gpii->irq_mode = params->irq_mode;
   gpii->cb = params->cb;
   gpii->user_data = params->user_data;

   /* Channel ctxt */
   if (GPI_STATUS_SUCCESS != set_gpii_chan(gpii, params, GPI_INBOUND_CHAN))
   {
      gpi_log(GPI_ERROR, "ERROR - Failed to set gpii ID: %d chan %d!\n",
              gpii->id, GPI_INBOUND_CHAN);
      return GPI_STATUS_ERROR;
   }
   if (GPI_STATUS_SUCCESS != set_gpii_chan(gpii, params, GPI_OUTBOUND_CHAN))
   {
      gpi_log(GPI_ERROR, "ERROR - Failed to set gpii ID: %d chan %d!\n",
              gpii->id, GPI_OUTBOUND_CHAN);
      return GPI_STATUS_ERROR;
   }

   /* Evt ring ctxt */
   if (GPI_STATUS_SUCCESS != set_gpii_evt_chan(gpii))
   {
      gpi_log(GPI_ERROR, "ERROR - Failed to set gpii ID:%d evt chan!\n", gpii->id);
      return GPI_STATUS_ERROR;
   }

   gpi_log(GPI_DEBUG, "set_gpii_params() ->\n");
   return GPI_STATUS_SUCCESS;
}

/**
 * Initializes GPII HW registers and interrupt.
 *
 * @param[in]    gpii                  Pointer to GPII context.
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpii_init(gpii_info *gpii)
{
   U_LONG gsi_pa = gpii->gpi->qup[gpii->qup_type].gsi_pa;

   if (gpii->initialized == TRUE)
   {
      gpi_log(GPI_DEBUG, "gpii ID: %d already initialized, returning\n", gpii->id);
      return GPI_STATUS_SUCCESS;
   }

   if (((HWIO_EE_n_GSI_STATUS_INMI(gsi_pa, gpii->id, 
                                   HWIO_EE_n_GSI_STATUS_ENABLED_BMSK)) >> HWIO_EE_n_GSI_STATUS_ENABLED_SHFT) != 0)
   {
      gpii->gpi->gpii_hw_ready_bmsk |= 1 << gpii->id;

      HWIO_EE_n_CNTXT_TYPE_IRQ_MSK_OUTI(gsi_pa, gpii->id, ~0x0u);
      HWIO_EE_n_CNTXT_SRC_IEOB_IRQ_MSK_OUTI(gsi_pa, gpii->id, ~0x0u);
      HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_MSK_OUTI(gsi_pa, gpii->id, ~0x0u);
      HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_MSK_OUTI(gsi_pa, gpii->id, ~0x0u);
      HWIO_EE_n_CNTXT_GLOB_IRQ_EN_OUTI(gsi_pa, gpii->id, ~0x0u);
      HWIO_EE_n_CNTXT_GSI_IRQ_EN_OUTI(gsi_pa, gpii->id, ~0x0u);

      /* Set interrupt type */
      HWIO_EE_n_CNTXT_INTSET_OUTMI(gsi_pa, gpii->id, 
                                   HWIO_EE_n_CNTXT_INTSET_INTYPE_BMSK, GPI_INT_IRQ << HWIO_EE_n_CNTXT_INTSET_INTYPE_SHFT);

      if (gpii->irq_mode && !gpii->gpi->irq_muxing) 
      {
         if (GPI_STATUS_SUCCESS != register_irq(gpii))
         {
            gpi_log(GPI_ERROR, "ERROR - gpii register_irq() failed , exiting\n");
            return GPI_STATUS_ERROR;
         }
      }

      HWIO_EE_n_CNTXT_MSI_BASE_LSB_OUTI(gsi_pa, gpii->id, 0);
      HWIO_EE_n_CNTXT_MSI_BASE_MSB_OUTI(gsi_pa, gpii->id, 0);
      HWIO_EE_n_CNTXT_INT_VEC_OUTI(gsi_pa, gpii->id, 0);
      HWIO_EE_n_CNTXT_SCRATCH_0_OUTI(gsi_pa, gpii->id, 0);
      HWIO_EE_n_CNTXT_SCRATCH_1_OUTI(gsi_pa, gpii->id, 0);
      HWIO_EE_n_ERROR_LOG_OUTI(gsi_pa, gpii->id, 0x0);
      HWIO_EE_n_ERROR_LOG_CLR_OUTI(gsi_pa, gpii->id, ~0x0u);

      /* Done by manager EE only */
      HWIO_GSI_MANAGER_EE_QOS_n_OUTMI(gsi_pa, gpii->id,
                                      HWIO_GSI_MANAGER_EE_QOS_n_MAX_EV_ALLOC_BMSK,
                                      NUM_EVT_RINGS_PER_GPII << HWIO_GSI_MANAGER_EE_QOS_n_MAX_EV_ALLOC_SHFT);
      HWIO_GSI_MANAGER_EE_QOS_n_OUTMI(gsi_pa, gpii->id,
                                      HWIO_GSI_MANAGER_EE_QOS_n_MAX_CH_ALLOC_BMSK,
                                      NUM_RINGS_PER_GPII << HWIO_GSI_MANAGER_EE_QOS_n_MAX_CH_ALLOC_SHFT);
      HWIO_GSI_MANAGER_EE_QOS_n_OUTMI(gsi_pa, gpii->id,
                                      HWIO_GSI_MANAGER_EE_QOS_n_EE_PRIO_BMSK,
                                      1 << HWIO_GSI_MANAGER_EE_QOS_n_EE_PRIO_SHFT);

    HWIO_EE_n_CNTXT_SRC_GSI_CH_IRQ_CLR_OUTI(gsi_pa, gpii->id, ~0x0u);
    HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_CLR_OUTI(gsi_pa, gpii->id, ~0x0u);
   }
   else
   {
      gpi_log(GPI_ERROR, "ERROR - gpii ID: %d HW not ready\n", gpii->id);
      return GPI_STATUS_ERROR;
   }

   return GPI_STATUS_SUCCESS;
}


/**
 * Main function to initialize GPI common data structure, interrupts,
 * threads, and synchronization objects.
 *
 * @param[in]    gpi                   Pointer to GPI context.
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_common_init(gpi_ctxt *gpi)
{
   uint32 i;

   gpi_log(GPI_DEBUG, "gpi_init() <-\n");

   if (!gpi->initialized)
   {
      /* Start GPI basic initialization */
      if (gpi->exec_mode == GPI_THREAD)
      {
         if (GPI_STATUS_SUCCESS != init_gpi_sync(gpi))
         {
            gpi_log(GPI_ERROR, "ERROR - init_gpi_sync() failed , exiting\n");
            return GPI_STATUS_ERROR;
         }

         if (GPI_STATUS_SUCCESS != init_threads(gpi))
         {
            gpi_log(GPI_ERROR, "ERROR - init_threads() failed , exiting\n");
            return GPI_STATUS_ERROR;
         }

         if (GPI_STATUS_SUCCESS != spawn_threads(gpi))
         {
            gpi_log(GPI_ERROR, "ERROR - spawn_threads() failed , exiting\n");
            return GPI_STATUS_ERROR;
         }
      }
      else
      {
         /* Initialize thread queue only, to be used by task/polling */
         reset_evt_q(&gpi->gpi_thread.q);
      }

      if (gpi->irq_muxing) 
      {
         if (GPI_STATUS_SUCCESS != register_irq_all(gpi))
         {
            gpi_log(GPI_ERROR, "ERROR - register_irq_all() failed , exiting\n");
            return GPI_STATUS_ERROR;
         }
#ifdef GPI_MODEM
         HWIO_TCSR_TOP_QUPV3_0_MSS_INT_SEL_OUT_0_OUT(0x001FFF00); /* Allow GSI bits of QUPv3 instance 0 through */
         HWIO_TCSR_TOP_QUPV3_1_MSS_INT_SEL_OUT_0_OUT(0x0);        /* Block all bits of QUPv3 instance 1         */
         HWIO_TCSR_TOP_QUPV3_0_MSS_INT_SEL_OUT_1_OUT(0x0);        /* Block all bits of QUPv3 instance 0         */
         HWIO_TCSR_TOP_QUPV3_1_MSS_INT_SEL_OUT_1_OUT(0x001FFF00); /* Allow GSI bits of QUPv3 instance 1 through */
#elif GPI_LPASS
         HWIO_TCSR_TOP_QUPV3_0_LPASS_INT_SEL_OUT_0_OUT(0x001FFF00); /* Allow GSI bits of QUPv3 instance 0 through */
         HWIO_TCSR_TOP_QUPV3_1_LPASS_INT_SEL_OUT_0_OUT(0x0);        /* Block all bits of QUPv3 instance 1         */
         HWIO_TCSR_TOP_QUPV3_0_LPASS_INT_SEL_OUT_1_OUT(0x0);        /* Block all bits of QUPv3 instance 0         */
         HWIO_TCSR_TOP_QUPV3_1_LPASS_INT_SEL_OUT_1_OUT(0x001FFF00); /* Allow GSI bits of QUPv3 instance 1 through */
#endif
      }

#ifdef GPI_ISLAND_MODE_ENABLE
   gpi_ring_elem  *pram;
   uint32 size = 0;
   /* Get memory from PRAM */
   pram = (gpi_ring_elem  *)gpi_acquire_partition(&size);
   if (pram == NULL || (size < (NUM_GPI_EVT_RING_ELEM * sizeof(gpi_ring_elem))))
   {
      gpi_log(GPI_ERROR, "ERROR - partition allocation failed!\n");
      return GPI_STATUS_ERROR;
   }
#endif

      for (i = 0; i < NUM_GPII; i++)
      {
         gpi->gpii[i].gpi = gpi;
         gpi->gpii[i].pending_cmd = GPI_CHAN_CMD_NONE;
         gpi->gpii[i].irq_mode = TRUE;
         gpi->gpii[i].initialized = FALSE;
#ifdef GPI_ISLAND_MODE_ENABLE
      gpi->gpii[i].evt_ring_elem = pram + (i * NUM_GPI_EVT_RING_ELEM);
#endif
      }

      gpi->initialized = TRUE;
      gpi_log(GPI_DEBUG, "gpi_init() <-\n");
      return GPI_STATUS_SUCCESS;
   }
   else
   {
      gpi_log(GPI_WARNING, "gpi already initialized!!\n");
      return GPI_STATUS_SUCCESS;
   }
}

