/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/src/AFEClientHandler.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFECodecHandler.h"
#include "AFEDevCommon.h"

/*==========================================================================
  Function Prototypes
========================================================================== */
ADSPResult afe_cdc_dev_cfg_read_int_status(uint16 int_idx, uint16_t *int_status);
ADSPResult afe_cdc_dev_cfg_write_int_mask(uint16_t int_idx, uint16_t is_it_masking);
ADSPResult afe_cdc_dev_cfg_write_int_clear(uint16_t is_it_clear);

void afe_cdc_ist_cb(void *ist_arg);
extern qurt_elite_interrupt_ist_t codec_int_ist;

#define CDC_IST_STACK_SIZE_BYTES 1024

/*==========================================================================
  Function implementations
========================================================================== */

void afe_cdc_ist_cb(void *ist_arg)
{
   // Send signal to Codec Interrupt Service
   qurt_elite_signal_send(cdc_handler_global_ptr->ist_signal_ptr);
}

ADSPResult afe_svc_cdc_int_register_handler(elite_msg_any_t *pMsg)
{
   ADSPResult           result = ADSP_EOK;
   cdc_client_info_t    *client_info_ptr = NULL;
   client_info_list_t   *cdc_client_info_list = NULL;
   uint16_t             client_handle = 0, int_idx = 0x0;
   char_t               thread_name[16];
   uint32_t             gpio_num;

   elite_msg_custom_afe_cdc_int_info_t  *pPayload = NULL;

   //This condition will become TRUE only when  WCDxxxx codec is not connected to MSM.
   //If WCDxxxx is connected to MSM then only LA/WP drivers will send AFE_PARAM_ID_CDC_REG_CFG_INIT during boot up, otherwise they wont send
   //codec related register addresses. We are skipping the register/deregister of codec interrupts when internal codec alone is present.
   if (FALSE == cdc_handler_global_ptr->is_cdc_reg_cfg_init_received)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Not registering with codec inteerupts, codec reg init not received: %d",\
               cdc_handler_global_ptr->is_cdc_reg_cfg_init_received);
      return ADSP_EOK;

   }
   //Register for codec GPIO interrupt if not done so already
   //This is done for the first client registration for codec interrupt
   if (!cdc_handler_global_ptr->is_codec_int_registered)
   {
      /* Get the codec interrupt GPIO type */
      lpasshwio_gpio_type_t    cdc_intr_gpio_type = afe_get_cdc_intr_gpio_type();

      if (GPIO_TYPE_UNSUPPORTED == cdc_intr_gpio_type)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Invalid codec interrupt GPIO type: 0x%lx", cdc_intr_gpio_type);
         goto _bail_out;
      }

      /* Save the codec interrupt GPIO type */
      cdc_handler_global_ptr->cdc_gpio_int_prop.gpio_type = cdc_intr_gpio_type;


      /* Check if CDC intr GPIO type is default
         (i.e. mapped to LPASS via direct connect lines) */
      if (GPIO_TYPE_DEFAULT == cdc_intr_gpio_type)
      {
         /* Populate the interrupt trigger type */
         cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.trigger_type = (uint32_t)GPIOINT_TRIGGER_RISING;

         /* Configure GPIO pin */
         if (ADSP_FAILED(result = afe_device_gpio_init("AUD_CODEC_INT_GPIO_MAP",
                                                       &cdc_handler_global_ptr->cdc_gpio_int_prop,
                                                       AFE_GPIO_NUM_INVALID)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Failed to configure the GPIO for codec hardware interrupt, result: %d", result);

            /* Clear any partial init's */
            afe_device_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop);

            goto _bail_out;
         }

         /* Save the l2vic#, used for qurt_elite_interrupt_register() */
         cdc_handler_global_ptr->intr_id = cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.l2vic_int_vector;
      }
      else if (GPIO_TYPE_PDC == cdc_intr_gpio_type) /* Codec GPIO intr is mapped via LPASS PDC */
      {
         /* Get the chip level GPIO num for codec interrupt from dev config */
         if (AFE_GPIO_NUM_INVALID == (gpio_num = afe_get_cdc_intr_gpio_num()))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Codec Interrupt GPIO num is invalid");
            goto _bail_out;
         }

         /* Save the GPIO num */
         cdc_handler_global_ptr->cdc_gpio_int_prop.n_gpio = gpio_num;

         /* Configure GPIO pin */
         if (ADSP_EOK != (result = afe_device_pdc_gpio_init(&cdc_handler_global_ptr->cdc_gpio_int_prop)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to configure GPIO pin: #lu", gpio_num);

            /* Clear up any partial init's */
            afe_device_pdc_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop);

            goto _bail_out;
         }
      }
      else if (GPIO_TYPE_LPI == cdc_intr_gpio_type) /* Codec intr GPIO is mapped via LPASS LPI TLMM */
      {
         /* We are reusing same structure "cdc_gpio_int_prop" to store GPIO number and L2VIC
          * MSM GPIO and LPI GPIO are mutually exclusive for routing cdc interrupt,
          * during registration we will have either MSM TLMM info or LPI GPIO info in this structure
          * For TLMM, GPIO will get updated as part of querying string in afe_device_gpio_init */

         /* Get the LPI GPIO num for codec interrupt */
         if (AFE_GPIO_NUM_INVALID == (gpio_num = afe_get_cdc_intr_gpio_num()))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Codec Interrupt LPI GPIO num is invalid");
            goto _bail_out;
         }

         /* Save the GPIO num */
         cdc_handler_global_ptr->cdc_gpio_int_prop.n_gpio = gpio_num;

         /* Configure the LPI GPIO pin */
         if (ADSP_FAILED(result = afe_device_lpi_gpio_init(&cdc_handler_global_ptr->cdc_gpio_int_prop)))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Failed to configure the GPIO[%lu] for codec interrupt: %d", gpio_num, result);

            /* Clear up any partial init's */
            afe_device_lpi_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop);

            goto _bail_out;
         }
      }

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Registering IST for Codec Interrupt");

      /* Get the codec interrupt IRQ type */
      lpasshwio_irq_type_t    cdc_irq_type = afe_get_cdc_intr_type();

      if (IRQ_TYPE_UNSUPPORTED == cdc_irq_type)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid codec interrupt type: 0x%lx", cdc_irq_type);
         goto _bail_out;
      }

      /* Save the codec interrupt type */
      cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.irq_type = cdc_irq_type;

      /* If interrupt is not gated by any h/w block */
      if (IRQ_TYPE_DEFAULT == cdc_irq_type)
      {
         qurt_elite_strl_cpy(thread_name, "CodecIntIST", sizeof(thread_name));

         if (ADSP_FAILED(result = qurt_elite_interrupt_register(&codec_int_ist,
                                                                cdc_handler_global_ptr->intr_id,
                                                                afe_cdc_ist_cb, NULL,
                                                                thread_name,
                                                                CDC_IST_STACK_SIZE_BYTES)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Failed to register IST with Qurt for the codec hardware interrupt: %d", result);

            goto _bail_out;
         }
      }
      else if (IRQ_TYPE_PDC == cdc_irq_type) /* Codec INT is PDC gated */
      {
         /* Populate the interrupt callback function pointer */
         cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.intr_cb = afe_cdc_ist_cb;

         /* Populate the interrupt trigger type */
         cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.trigger_type = (uint32_t)GPIOINT_TRIGGER_RISING;

         /* Configure GPIO interrupt */
         if (ADSP_EOK != (result = afe_device_pdc_gpio_intr_reg(&cdc_handler_global_ptr->cdc_gpio_int_prop)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to configure PDC GPIO intr, result: %d", result);

            /* Clear up any partial init's */
            afe_device_pdc_gpio_intr_dereg(&cdc_handler_global_ptr->cdc_gpio_int_prop);

            goto _bail_out;
         }
      }
      else if (IRQ_TYPE_PMU == cdc_irq_type) /* Codec INT is PMU gated */
      {
         /* Configure the LPI GPIO interrupt */

         /* Populate the callback function pointer */
         cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.intr_cb = afe_cdc_ist_cb;

         /* Populate the interrupt trigger type */
         cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.trigger_type = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;

         /* Interrupt string, used when l2vic num is not known explicitly
            Interrupt string name is picked from the IPCAT */
         cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.p_intr_l2vic_string = (char *)("bi_px_ssc_23");

         if (ADSP_FAILED(result = afe_device_gated_intr_register(&cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Failed to configure the GPIO for codec hardware interrupt, result: %d", result);

            /* Clear up any partial init's */
            afe_device_gated_intr_deregister(&cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop);

            goto _bail_out;
         }
      }
      cdc_handler_global_ptr->is_codec_int_registered = TRUE;
   } /*End of if (!cdc_handler_global_ptr->is_codec_int_registered) */

   // get client payload
   pPayload = (elite_msg_custom_afe_cdc_int_info_t *)(pMsg->pPayload);

   // access client specific information
   client_info_ptr = (cdc_client_info_t *)&pPayload->client_info;

   // validate the requested interrupt id
   if ((!IS_VALID_CDC_INT_INDEX(client_info_ptr->int_index)) || (0 != client_info_ptr->client_handle))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to register the client, int-id: 0x%x, client_handle: 0x%x", client_info_ptr->int_index, client_info_ptr->client_handle);
      result = ADSP_EFAILED;
      goto _bail_out;
   }

   // store the int id
   int_idx = client_info_ptr->int_index;

   // get a handle to internal client's list for this interrupt
   cdc_client_info_list = &(cdc_handler_global_ptr->client_info_list[int_idx]);

   // go through client list nodes and find the free node to register the client
   for (client_handle = 0; client_handle < AFE_CDC_INT_MAX_CLIENTS; client_handle++)
   {
      // look for the free node
      if (cdc_client_info_list->client_is_used[client_handle] == FALSE)
      {
         // assign absolute client handle
         client_info_ptr->client_handle = CLIENT_HANDLE_CONVERT_REL_TO_ABS(client_handle, int_idx);

         // mark the node as used
         cdc_client_info_list->client_is_used[client_handle] = TRUE;

         // copy the client info for book keeping
         cdc_client_info_list->client_info[client_handle] = pPayload->client_info;

         // updated num clients for book keeping
         (cdc_client_info_list->num_clients)++;

         // Un-Mask the interrupt if there is atleast one client for this interrupt
         if (1 == cdc_client_info_list->num_clients)
         {
            cdc_handler_global_ptr->sw_int_mask |= (1 << int_idx);

#ifdef CDC_DEV_SLIMBUS
            if (ADSP_EOK != (result = afe_slimbus_core_driver_open(cdc_handler_global_ptr->sb_cfg.p_sb_driver)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
               return ADSP_EFAILED;
            }
#endif // CDC_DEV_SLIMBUS

            afe_cdc_dev_cfg_write_int_mask(int_idx, FALSE);

#ifdef CDC_DEV_SLIMBUS
            afe_slimbus_core_driver_close(cdc_handler_global_ptr->sb_cfg.p_sb_driver);
#endif // CDC_DEV_SLIMBUS
         }
         break;
      }
   }

   if (AFE_CDC_INT_MAX_CLIENTS == client_handle)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Client requests are out of max supported: 0x%x, 0x%x", client_info_ptr->int_index, cdc_client_info_list->num_clients);
      result = ADSP_EUNSUPPORTED;
      goto _bail_out;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "CDC Client registration is successful: handle: 0x%x, int: 0x%x, mask: 0x%lx", client_info_ptr->client_handle, client_info_ptr->int_index, cdc_handler_global_ptr->sw_int_mask);

_bail_out:

   /* Interrupt registration failure are handled inline.
      Clean up any GPIO related config if interrupt registration
      has failed */
   if ((ADSP_EOK != result) &&
       (TRUE == cdc_handler_global_ptr->cdc_gpio_int_prop.is_gpio_configured))
   {
      switch (cdc_handler_global_ptr->cdc_gpio_int_prop.gpio_type)
      {
         case GPIO_TYPE_DEFAULT:
         {
            /* De-init the default type GPIO */
            afe_device_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop);
            break;
         }
         case GPIO_TYPE_PDC:
         {
            /* De-init the PDC type GPIO */
            afe_device_pdc_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop);
            break;
         }
         case GPIO_TYPE_LPI:
         {
            /* De-init the LPI type GPIO  */
            afe_device_lpi_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop);
            break;
         }
         default:
            break;
      }
   }

   elite_svc_send_ack(pMsg, result);
   return result;
}

ADSPResult afe_svc_cdc_int_deregister_handler(elite_msg_any_t *pMsg)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_afe_cdc_int_info_t  *pPayload = NULL;
   cdc_client_info_t  *client_info_ptr = NULL;
   client_info_list_t *cdc_client_info_list = NULL;
   uint16_t client_handle = 0, int_idx = 0x0;


   //This condition will become TRUE only when  WCDxxxx codec is not connected to MSM.
   //If WCDxxxx is connected to MSM then only LA/WP drivers will send AFE_PARAM_ID_CDC_REG_CFG_INIT during boot up, otherwise they wont send
   //codec related register addresses. We are skipping the register/deregister of codec interrupts when internal codec alone is present.
   if (FALSE == cdc_handler_global_ptr->is_cdc_reg_cfg_init_received)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "codec reg init not received: %d",\
               cdc_handler_global_ptr->is_cdc_reg_cfg_init_received);
      return ADSP_EOK;
   }

   // get client payload
   pPayload = (elite_msg_custom_afe_cdc_int_info_t *)(pMsg->pPayload);

   // get client specific information
   client_info_ptr = (cdc_client_info_t *)&pPayload->client_info;

   // get interrupt info
   int_idx = client_info_ptr->int_index;

   // validity check on the interrupt id
   if (!IS_VALID_CDC_INT_INDEX(int_idx))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Interrupt info is out of range: 0x%x", int_idx);
      result = ADSP_EUNSUPPORTED;
      goto _bail_out;
   }

   // get handle to client's list for this interrupt
   cdc_client_info_list = &(cdc_handler_global_ptr->client_info_list[int_idx]);

   // TBD: error check for client handle w.r.t int idx.

   // claculate the relative client_handle for this interrupt
   client_handle = CLIENT_HANDLE_CONVERT_ABS_TO_REL(client_info_ptr->client_handle, int_idx);

   // validity check on the client handle
   if (client_handle < AFE_CDC_INT_MAX_CLIENTS)
   {
      // mark the node as un-used
      cdc_client_info_list->client_is_used[client_handle] = FALSE;

      // ignore any pending signals or acks from client
      cdc_client_info_list->client_signaled[client_handle] = FALSE;

      // reset client speicific internal node
      memset(&cdc_client_info_list->client_info[client_handle], 0, sizeof(cdc_client_info_list->client_info[client_handle]));

      // reduce the clients counter
      (cdc_client_info_list->num_clients)--;

      // Mask the interrupt if there is no single client for that
      if (0 == cdc_client_info_list->num_clients)
      {
         cdc_client_info_list->postpone_int_unmask = 0;

         cdc_handler_global_ptr->sw_int_mask &= ~(1 << int_idx);

#ifdef CDC_DEV_SLIMBUS
         if (ADSP_EOK != (result = afe_slimbus_core_driver_open(cdc_handler_global_ptr->sb_cfg.p_sb_driver)))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
            return ADSP_EFAILED;
         }
#endif // CDC_DEV_SLIMBUS

         afe_cdc_dev_cfg_write_int_mask(int_idx, TRUE);

#ifdef CDC_DEV_SLIMBUS
         afe_slimbus_core_driver_close(cdc_handler_global_ptr->sb_cfg.p_sb_driver);
#endif // CDC_DEV_SLIMBUS

         //Now de-register the codec interrupt if clients for all interrupts are de-registered
         if ((cdc_handler_global_ptr->is_codec_int_registered) && (0 == cdc_handler_global_ptr->sw_int_mask))
         {
            /* Get the codec interrupt IRQ type */
            lpasshwio_irq_type_t    cdc_irq_type = cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop.irq_type;

            if (IRQ_TYPE_DEFAULT == cdc_irq_type)
            {
               // De-register codec hw interrupt with kernel
               if (ADSP_FAILED(result = qurt_elite_interrupt_unregister(&codec_int_ist)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in interrupt deregistration: %d", result);
               }
            }
            else if (IRQ_TYPE_PDC == cdc_irq_type) /* Codec INT is PDC gated */
            {
               /* De-register GPIO interrupt */
               if (ADSP_EOK != (result = afe_device_pdc_gpio_intr_dereg(&cdc_handler_global_ptr->cdc_gpio_int_prop)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                        "Failed to deregister the CDC GPIO interrupt, result = %d", result);
               }
            }
            else if (IRQ_TYPE_PMU == cdc_irq_type) /* Codec INT is PMU gated */
            {
               /* De-register GPIO interrupt */
               if (ADSP_EOK != (result = afe_device_gated_intr_deregister(&cdc_handler_global_ptr->cdc_gpio_int_prop.intr_prop)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deregister the GPIO interrupt, result = %d", result);
               }
            }


            /* Get the codec interrupt GPIO type */
            lpasshwio_gpio_type_t    cdc_int_gpio_type = cdc_handler_global_ptr->cdc_gpio_int_prop.gpio_type;

            if (GPIO_TYPE_DEFAULT == cdc_int_gpio_type)
            {
               /* De-register Default type GPIO */
               if (ADSP_EOK != (result = afe_device_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                        "Failed to deregister the CDC Defalult GPIO, result = %d", result);
               }
            }
            else if (GPIO_TYPE_PDC == cdc_int_gpio_type) /* Codec INT GPIO is PDC type */
            {
               /* De-register PDC type GPIO */
               if (ADSP_EOK != (result = afe_device_pdc_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                        "Failed to deregister the CDC PDC GPIO, result = %d", result);
               }
            }
            else if (GPIO_TYPE_LPI == cdc_int_gpio_type) /* Codec INT GPIO is LPI type */
            {
               /* De-register LPI type GPIO */
               if (ADSP_EOK != (result = afe_device_lpi_gpio_deinit(&cdc_handler_global_ptr->cdc_gpio_int_prop)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                        "Failed to deregister the CDC LPI GPIO, result = %d", result);
               }
            }

            cdc_handler_global_ptr->is_codec_int_registered = FALSE;
         }
      }

      client_info_ptr->client_handle = 0x0;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Client requests are out of max supported: int: 0x%x, num_clients: 0x%x", int_idx, cdc_client_info_list->num_clients);
      result = ADSP_EUNSUPPORTED;
      goto _bail_out;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "CDC Client De-registration request: handle: 0x%x, int: 0x%x, mask: 0x%lx", client_info_ptr->client_handle, int_idx, cdc_handler_global_ptr->sw_int_mask);

_bail_out:
   elite_svc_send_ack(pMsg, result);
   return result;
}

ADSPResult afe_svc_cdc_int_ack_handler(elite_msg_any_t *pMsg)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_afe_cdc_int_info_t  *pPayload = NULL;
   cdc_client_info_t  *client_info_ptr = NULL;
   client_info_list_t *cdc_client_info_list = NULL;
   uint16_t client_handle = 0, pending_acks = 0, int_idx = 0x0;

   // get client payload ptr
   pPayload = (elite_msg_custom_afe_cdc_int_info_t *)(pMsg->pPayload);

   // get client info from payload
   client_info_ptr = (cdc_client_info_t *)&pPayload->client_info;

   // get interrupt info
   int_idx = client_info_ptr->int_index;

   // error check on interrupt validity
   if (!IS_VALID_CDC_INT_INDEX(int_idx))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Interrupt info is out of range: 0x%x", int_idx);
      result = ADSP_EUNSUPPORTED;
      goto _bail_out;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Received Int Ack, client-id: 0x%x, int_idx: 0x%x", client_info_ptr->client_handle, int_idx);

   // get list of clients to this interrupt
   cdc_client_info_list = &(cdc_handler_global_ptr->client_info_list[int_idx]);

   // cacluate the relative client handle index for this client
   client_handle = CLIENT_HANDLE_CONVERT_ABS_TO_REL(client_info_ptr->client_handle, int_idx);

   // update book keeping to indicate the recived ack from client
   if (TRUE == cdc_client_info_list->client_is_used[client_handle])
   {
      client_info_ptr = &cdc_client_info_list->client_info[client_handle];

      // update book keeping that interrupt info is signaled to client
      cdc_client_info_list->client_signaled[client_handle] = FALSE;
   }

   // make sure that we received all acks from all clients on this interrupt before un-masking
   // TBD: we can use mask for tracking client pending acks: optimization instead of loop
   for (client_handle = 0; client_handle < AFE_CDC_INT_MAX_CLIENTS; client_handle++)
   {
      if (TRUE == cdc_client_info_list->client_is_used[client_handle])
      {
         pending_acks += (cdc_client_info_list->client_signaled[client_handle]);
      }
   }

   // if all clients send the ack, then unmask the interupt
   if ((0 == pending_acks) && (TRUE == cdc_client_info_list->postpone_int_unmask))
   {
#ifdef CDC_DEV_SLIMBUS
      if (ADSP_EOK != (result = afe_slimbus_core_driver_open(cdc_handler_global_ptr->sb_cfg.p_sb_driver)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
         return ADSP_EFAILED;
      }
#endif // CDC_DEV_SLIMBUS

      // clear the interrupt
      afe_cdc_dev_cfg_write_int_clear(int_idx);
      afe_cdc_dev_cfg_write_int_mask(int_idx, FALSE);

#ifdef CDC_DEV_SLIMBUS
      afe_slimbus_core_driver_close(cdc_handler_global_ptr->sb_cfg.p_sb_driver);
#endif // CDC_DEV_SLIMBUS

      cdc_client_info_list->postpone_int_unmask = FALSE;
   }

_bail_out:
   elite_msg_return_payload_buffer(pMsg);
   return result;
}

ADSPResult afe_cdc_int_inform_registered_clients(uint16_t int_idx)
{
   ADSPResult result = ADSP_EOK;
   uint32_t nSize;
   elite_msg_any_t  msg;
   cdc_client_info_t *client_info_ptr = NULL;
   client_info_list_t *cdc_client_info_list = NULL;
   uint16_t client_handle = 0;
   elite_msg_custom_afe_cdc_int_info_t   *pPayload = NULL;

   // get the handle to clients list for this interrupt
   cdc_client_info_list = &cdc_handler_global_ptr->client_info_list[int_idx];

   // initialize the unmasking flag
   cdc_client_info_list->postpone_int_unmask = FALSE;

   // traverse through all clients that are conencted to this interrupt
   for (client_handle = 0; client_handle < AFE_CDC_INT_MAX_CLIENTS; client_handle++)
   {
      if (TRUE == cdc_client_info_list->client_is_used[client_handle])
      {
         // get client handle
         client_info_ptr = &cdc_client_info_list->client_info[client_handle];

         // inform the registered client with interrupt information, queue is higher prioriy than signal registration
         if (NULL != client_info_ptr->client_q_ptr)
         {
            // Send custom message to connected clients to indicate interrupt info
            nSize = sizeof(elite_msg_custom_afe_cdc_int_info_t);
            if (ADSP_EOK == (result = elite_msg_create_msg(&msg, &nSize, ELITE_CUSTOM_MSG, NULL, 0, ADSP_EOK)))
            {
               pPayload = (elite_msg_custom_afe_cdc_int_info_t *)msg.pPayload;
               pPayload->sec_op_code = ELITEMSG_CUSTOM_CDC_INT_CLIENT_SIG;


               memscpy(&pPayload->client_info, sizeof(cdc_client_info_t), client_info_ptr, sizeof(cdc_client_info_t));

               // push custom msg
               if (ADSP_EOK != (result = qurt_elite_queue_push_back(client_info_ptr->client_q_ptr, (uint64_t *)&msg)))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to send interrupt info to client, client-id: 0x%x, result: %d", client_info_ptr->client_handle, result);
                  elite_msg_return_payload_buffer(&msg);
               }
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to send interrupt info to client, client-id: 0x%x, result: %d", client_info_ptr->client_handle, result);
            }
         }
         else if (NULL != client_info_ptr->client_signal_ptr)
         {
            // if clients connected with signal only, then send the signal to indicate the interrupt
            qurt_elite_signal_send(client_info_ptr->client_signal_ptr);
         }

         // update book keeping that interrupt info is signaled to client
         if (TRUE == cdc_client_info_list->client_info[client_handle].enable_int_ack_ctrl)
         {
            cdc_client_info_list->client_signaled[client_handle] = TRUE;
            cdc_client_info_list->postpone_int_unmask = TRUE;
         }
      }
   }

   // un-mask the interrupt if no one is requesting the postponement
   if (FALSE == cdc_client_info_list->postpone_int_unmask)
   {
      // clear the interrupt
      afe_cdc_dev_cfg_write_int_clear(int_idx);
      afe_cdc_dev_cfg_write_int_mask(int_idx, FALSE);
   }

   return result;
}

ADSPResult afe_svc_cdc_int_process_handler()
{
   ADSPResult result = ADSP_EOK;
   uint16_t int_idx = 0;
   uint16_t int_status = 0;
   uint32_t int_rec_check = FALSE; //Flag for recursive check for interrupt status

   // traverse through all interrupts
   do
   {
      int_rec_check = FALSE;

      for (int_idx = 0; int_idx <= (uint16_t)AFE_CDC_INT_INDEX_END; int_idx++)
      {
         // check the interrupt status for every registered interrupt
         if ((((cdc_handler_global_ptr->sw_int_mask) & (1 << int_idx))) && (FALSE == (cdc_handler_global_ptr->client_info_list[int_idx].postpone_int_unmask)))
         {
            int_status = FALSE;

#ifdef CDC_DEV_SLIMBUS
            if (ADSP_EOK != (result = afe_slimbus_core_driver_open(cdc_handler_global_ptr->sb_cfg.p_sb_driver)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
               return ADSP_EFAILED;
            }
#endif // CDC_DEV_SLIMBUS

            result = afe_cdc_dev_cfg_read_int_status(int_idx, &int_status);

            if ((ADSP_EOK == result) && (TRUE == int_status))
            {
               // mask the interrupt to ensure that further interrupts will not be generated until acknowledge
               afe_cdc_dev_cfg_write_int_mask(int_idx, TRUE);

               // clear the interrupt
               afe_cdc_dev_cfg_write_int_clear(int_idx);

               // inform this interrupt to all registered clients
               afe_cdc_int_inform_registered_clients(int_idx);

               // Need to check any pending interrupt when the interrupt is masked after informing the clients
               // This is to service the interrupt(s) raised during processing of current interrupts
               if (FALSE == cdc_handler_global_ptr->client_info_list[int_idx].postpone_int_unmask)
               {
                  int_rec_check = TRUE;
               }

            }
#ifdef CDC_DEV_SLIMBUS
            afe_slimbus_core_driver_close(cdc_handler_global_ptr->sb_cfg.p_sb_driver);
#endif // CDC_DEV_SLIMBUS
         }
      }
   }while (int_rec_check);

   return result;
}
