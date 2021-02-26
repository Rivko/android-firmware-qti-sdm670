/*
*
* Copyright @ 2014-2018 InvenSense Inc.  All rights reserved.
*
* INVENSENSE, INC. ("LICENSOR") SOFTWARE LICENSE AGREEMENT ("Agreement")
* BY DOWNLOADING, INSTALLING, COPYING OR OTHERWISE USING THE ENCLOSED SOFTWARE AND OTHER AVAILABLE MATERIALS ("LICENSED
* MATERIALS"), YOU ("LICENSEE") AGREE TO BE BOUND BY THE FOLLOWING TERMS AND CONDITIONS OF THIS AGREEMENT.  IF LICENSEE DOES NOT
* AGREE TO THE TERMS AND CONDITION OF THIS AGREEMENT, THEN DO NOT DOWNLOAD, INSTALL, COPY OR OTHERWISE USE THE ENCLOSED SOFTWARE
* AND MATERIALS.
*
* The Licensed Materials may include open source and proprietary software in both source code ("Source Code") and object code
* ("Object Code") versions, documentation and other materials.  Except as expressly indicated herein, all terms and conditions of
* this Agreement apply to all of the Licensed Materials.  In the event that any file contained within the Licensed Materials
* incorporates a different software license, such other license shall apply to such file.
* Except as expressly set forth herein, LICENSOR owns all of the Licensed Materials and makes them available to Licensee only
* under the terms and conditions set forth in this Agreement.
*
* 1. License:  Subject to the terms of this Agreement, LICENSOR hereby grants to Licensee a royalty-free, non-exclusive license to
* possess and to use the Licensed Materials in a secure location in accordance with the applicable terms herein.  Licensee may
* make back-up copies and install and use multiple copies of the Licensed Materials on a shared computer or concurrently on
* different computers, solely for Licensee's use within Licensee's Enterprise. "Enterprise" shall mean individual use by Licensee
* or any legal entity (such as a corporation or university) and the subsidiaries it owns by more than 50 percent.  The following
* terms apply to the specified type of Licensed Material:
*
* 1.1 Source Code:  Licensee shall have the right to use, copy, modify and prepare and have prepared derivative works of the
* Source Code for internal development and testing purposes only.  Licensee shall own any modification it makes directly to the
* Source Code that optimizes quality or performance with Licensee's product ("Modification(s)") subject to LICENSOR's ownership of
* the underlying Source Code and all intellectual property rights therein.
*
* 1.2 Object Code:  Licensee may distribute the Object Code (in object code form only) and compiled Source Code (in object code
* form only) with Modifications solely for execution or operation with applicable LICENSOR products for which Source Code and
* Object Code was designed and as incorporated in Licensee's software products. Licensee agrees not to disassemble, decompile or
* reverse engineer the Object Code.
*
* 2. Notices:  Licensee agrees to comply with and reproduce any and all copyright and other attribution notices/instructions of
* LICENSOR as included in the Licensed Materials in connection with Licensee's distribution rights pursuant to the Agreement.
*
* 3. Subcontractors:  Licensee may engage subcontractors to exercise Licensee's rights hereunder. Licensee is responsible to
* ensure that Licensee subcontractors comply with the terms and conditions of this Agreement.  Any act by a subcontractor that
* would be a breach of this Agreement by Licensee if Licensee performed the act will be deemed a breach of this Agreement by
* Licensee.
*
* 4. License Grant Back: Licensee hereby grants to LICENSOR and its Affiliates an exclusive, worldwide, irrevocable, perpetual,
* sublicenseable (through multiple tiers of sublicensees), royalty-free and fully paid-up right and license to the Modification(s)
* (in object code form) created by or on behalf of Licensee so that LICENSOR may copy, modify, create derivatives works thereof,
* to use, have used, import, make, have made, sell, offer to sell, sublicense (through multiple tiers of sublicensees), distribute
* (through multiple tiers of distributors) such derivative work(s) on a stand-alone basis or as incorporated into the Licensed
* Materials or other related technologies.  For the sake of clarity, LICENSOR is not prohibited or otherwise restricted from
* independently developing new features or functionality with respect to the Licensed Materials.
*
* 5. No Other License: No rights or licenses with respect to any proprietary information or patent, copyright, trade secret or
* other intellectual property right owned or controlled by LICENSOR are granted by LICENSOR to Licensee under this Agreement,
* expressly or by implication, except as expressly provided in this Agreement.
* 6. Intellectual Property Ownership: Except as expressly licensed to Licensee under this Agreement, LICENSOR reserves all right,
* title and interest, including but not limited to all intellectual property rights, in and to the Licensed Materials and any
* derivative work(s) made thereto.
*
* 7. Term of Agreement:  This Agreement is effective until (i) automatically terminated if Licensee fails to comply with any of
* the terms and conditions of this Agreement; or (ii) terminated by LICENSOR.  LICENSOR may terminate this Agreement (and with it,
* all of Licensee's right to the Licensed Materials) immediately upon written notice (which may include email) to Licensee, with
* or without cause; provided however, that sublicenses of Derivatives, to the extent validly granted to its customers prior to
* termination of this Agreement, shall survive such termination.
*
* 8. Confidential Information. "Confidential Information" means (i) the Licensed Materials; (ii) the structure, sequence and
* organization of the Licensed Materials and the concepts, methods of operations and ideas disclosed therein; and (iii) any trade
* secrets of LICENSOR or its affiliates or its or their suppliers relating to the Licensed Materials. Licensee will not disclose
* any Confidential Information to any third party (except subcontractors, as permitted herein) or use Confidential Information
* except as expressly permitted in this Agreement.  Licensee agrees to take all reasonable measures to protect Confidential
* Information and prevent its unauthorized disclosure, including measures at least as stringent as those measures Licensee takes
* to protect Licensee's own most sensitive confidential information.  Licensee agrees to restrict access to Confidential
* Information to Licensee employees and subcontractors who are under obligations to protect Confidential Information in accordance
* with this Agreement and who have a "need to know" the Confidential Information to exercise Licensee license rights in this
* Agreement.  All Confidential Information, and any documents and other tangible objects containing or representing Confidential
* Information, and all copies of Confidential Information, are and will remain the exclusive property of LICENSOR.
*
* 9. Defensive Suspension: If Licensee commences or participates in any legal proceeding against LICENSOR, then LICENSOR may, in
* its sole discretion, suspend or terminate all license grants and any other rights provided under this Agreement during the
* pendency of such legal proceedings.
*
* 10. No Support:  LICENSOR has no obligation to support or to continue providing or updating any of the Licensed Materials.
*
* 11. No Warranty:  THE LICENSED MATERIALS PROVIDED BY LICENSOR TO LICENSEE HEREUNDER ARE PROVIDED "AS IS."  LICENSOR DISCLAIMS
* ALL WARRANTIES, EXPRESS, IMPLIED OR STATUTORY, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*
* 12. Limitation of Liability: LICENSOR SHALL NOT BE LIABLE TO LICENSEE, LICENSEE'S CUSTOMERS, OR ANY OTHER PERSON OR ENTITY
* CLAIMING THROUGH OR UNDER LICENSEE FOR ANY LOSS OF PROFITS, INCOME, SAVINGS, OR ANY OTHER CONSEQUENTIAL, INCIDENTAL, SPECIAL,
* PUNITIVE, DIRECT OR INDIRECT DAMAGES (WHETHER IN AN ACTION IN CONTRACT, TORT OR BASED ON A WARRANTY), EVEN IF LICENSOR HAS BEEN
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF THE ESSENTIAL PURPOSE
* OF ANY LIMITED REMEDY.  IN NO EVENT SHALL LICENSOR'S AGGREGATE LIABILITY TO LICENSEE OR ANY OTHER PERSON OR ENTITY CLAIMING
* THROUGH OR UNDER LICENSEE EXCEED THE AMOUNT OF MONEY ACTUALLY PAID BY LICENSEE TO LICENSOR FOR THE LICENSED MATERIALS.
*
* 13. Applicable Law and Jurisdiction: This Agreement shall be deemed to have been made in, and shall be construed pursuant to,
* the laws of the State of California. The state and/or federal courts residing in Santa Clara County, California shall have
* exclusive jurisdiction over any dispute or claim arising out of this Agreement. The United Nations Convention on Contracts for
* the International Sale of Goods is specifically disclaimed.
*
* 14. Feedback: Licensee may, but is not obligated to, provide to LICENSOR any suggestions, comments and feedback regarding the
* Licensed Materials that are delivered by LICENSOR to Licensee under this Agreement (collectively, "Licensee Feedback").
* LICENSOR may use and include any Licensee Feedback that Licensee voluntarily provides to improve the Licensed Materials or other
* related LICENSOR technologies.  Accordingly, if Licensee provides Licensee Feedback, Licensee grants LICENSOR and its licensees
* a perpetual, irrevocable, worldwide, royalty-free, fully paid-up license grant to freely use, have used, sell, modify,
* reproduce, transmit, license, sublicense (through multiple tiers of sublicensees), distribute (through multiple tiers of
* distributors), and otherwise commercialize the Licensee Feedback in the Licensed Materials or other related technologies.
*
* 15. RESTRICTED RIGHTS NOTICE: Licensed Materials has been developed entirely at private expense and is commercial computer
* software provided with RESTRICTED RIGHTS. Use, duplication or disclosure by the U.S. Government or a U.S. Government
* subcontractor is subject to the restrictions set forth in the license agreement under which Licensed Materials was obtained
* pursuant to DFARS 227.7202-3(a) or as set forth in subparagraphs (c)(1) and (2) of the Commercial Computer Software - Restricted
* Rights clause at FAR 52.227-19, as applicable.
*
* 16. Miscellaneous: If any provision of this Agreement is inconsistent with, or cannot be fully enforced under, the law, such
* provision will be construed as limited to the extent necessary to be consistent with and fully enforceable under the law. This
* Agreement is the final, complete and exclusive agreement between the parties relating to the subject matter hereof, and
* supersedes all prior or contemporaneous understandings and agreements relating to such subject matter, whether oral or written.
* This Agreement is solely between LICENSOR and Licensee.  There are no third party beneficiaries, express or implied, to this
* Agreement. This Agreement may only be modified in writing signed by an authorized officer of LICENSOR.  Licensee agrees that it
* will not ship, transfer or export the Licensed Materials into any country, or use the Licensed Materials in any manner,
* prohibited by the United States Bureau of Industry and Security or any export laws, restrictions or regulations. This Agreement,
* and Licensee's rights and obligations herein, may not be assigned, subcontracted, delegated, or otherwise transferred by
* Licensee without LICENSOR's prior written consent, and any attempted assignment, subcontract, delegation, or transfer in
* violation of the foregoing will be null and void.   The terms of this Agreement shall be binding upon assignees.
*
*
*/

#include <string.h>
#include "sns_icm4x6xx_oem_test.h"
#include "sns_attribute_util.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_sync_com_port_service.h"
#include "sns_types.h"

#include "sns_icm4x6xx_hal.h"
#include "sns_icm4x6xx_sensor.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_motion_detect.pb.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_registry.pb.h"
#include "sns_cal.pb.h"

#ifdef ICM4X6XX_OEM_TEST
#include "sns_physical_sensor_oem_config.pb.h"
#endif

static sns_sensor_uid const* icm4x6xx_get_sensor_uid(sns_sensor const *const this)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;

    return &state->my_suid;
}

void icm4x6xx_start_power_rail_timer(sns_sensor *const this,
                                           sns_time timeout_ticks,
                                           icm4x6xx_power_rail_pending_state pwr_rail_pend_state)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[20];

  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = false;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = timeout_ticks;

  if(NULL == state->timer_stream)
  {
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
    sns_sensor_uid suid;

    sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", &suid);
    stream_svc->api->create_sensor_stream(stream_svc, this, suid, &state->timer_stream);
  }

  req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                              sns_timer_sensor_config_fields, NULL);
  if(req_len > 0 && NULL != state->timer_stream)
  {
    sns_request timer_req =
        {
            .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
            .request = buffer, .request_len = req_len
        };
    state->timer_stream->api->send_request(state->timer_stream, &timer_req);
    state->power_rail_pend_state = pwr_rail_pend_state;
  }
  else
  {
    ICM4X6XX_PRINTF(ERROR, this, "E");
  }
}
/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void icm4x6xx_set_self_test_inst_config(sns_sensor *this,
                              sns_sensor_instance *instance)
{
    sns_request config;

    config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
    config.request_len = 0;
    config.request = NULL;

    this->instance_api->set_client_config(instance, &config);
}

/** See sns_icm4x6xx_sensor.h*/
sns_rc icm4x6xx_sensor_notify_event(sns_sensor *const this)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    sns_rc rv = SNS_RC_SUCCESS;
    sns_sensor_event *event;

    state->island_service->api->sensor_island_exit(state->island_service, this);

    if(!sns_suid_lookup_complete(&state->common.suid_lookup_data)) {
        state->island_service->api->sensor_island_exit(state->island_service, this);
        sns_suid_lookup_handle(this, &state->common.suid_lookup_data);

        if(sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", NULL))
            icm4x6xx_request_registry(this);

        if(sns_suid_lookup_complete(&state->common.suid_lookup_data))
            sns_suid_lookup_deinit(this, &state->common.suid_lookup_data);
    }

    /**----------------------Handle a Timer Sensor event.-------------------*/
    if(NULL != state->timer_stream)
    {
        event = state->timer_stream->api->peek_input(state->timer_stream);
        while(NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                         event->event_len);
            sns_timer_sensor_event timer_event;

            if(SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
            {
                if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
                {
                    if(state->power_rail_pend_state == ICM4X6XX_POWER_RAIL_PENDING_INIT)
                    {
                        /** Initial HW discovery is OK to run in normal mode. */
                        state->island_service->api->sensor_island_exit(state->island_service, this);

                        state->common.hw_is_present = icm4x6xx_discover_hw(this);

                        if(state->common.hw_is_present)
                        {
                            icm4x6xx_update_sibling_sensors(this);
#ifndef ICM4X6XX_DISABLE_DAE
                            icm4x6xx_dae_if_check_support(this);
#endif
                        }
                        else
                        {
                            rv = SNS_RC_INVALID_LIBRARY_STATE;
                            ICM4X6XX_PRINTF(MED, this, "ICM4X6XX HW absent");
                        }
                        state->power_rail_pend_state = ICM4X6XX_POWER_RAIL_PENDING_NONE;
                    }
                    else if(state->power_rail_pend_state == ICM4X6XX_POWER_RAIL_PENDING_SET_CLIENT_REQ)
                    {
                        sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
                        if(NULL != instance)
                        {
                            icm4x6xx_instance_state *inst_state =
                                (icm4x6xx_instance_state*) instance->state->state;
                            inst_state->instance_is_ready_to_configure = true;

                            //inst_state->scp_service->api->sns_scp_update_bus_power(inst_state->com_port_info.port_handle, true);
                            //icm4x6xx_reset_device(instance);
                            //inst_state->scp_service->api->sns_scp_update_bus_power(inst_state->com_port_info.port_handle, false);

                            icm4x6xx_reval_instance_config(this, instance, state->sensor);
                            if(inst_state->new_self_test_request)
                            {
                                icm4x6xx_set_self_test_inst_config(this, instance);
                            }
                            state->power_rail_pend_state = ICM4X6XX_POWER_RAIL_PENDING_NONE;
                        }
                    }
                }
                else
                {
                    ICM4X6XX_PRINTF(ERROR, this, "E");
                }
            }
            event = state->timer_stream->api->get_next_input(state->timer_stream);
        }
        /** Free up timer stream if not needed anymore */
        if(state->power_rail_pend_state == ICM4X6XX_POWER_RAIL_PENDING_NONE)
        {
            sns_sensor_util_remove_sensor_stream(this, &state->timer_stream);
        }
    }

#ifndef ICM4X6XX_DISABLE_DAE
    icm4x6xx_dae_if_process_sensor_events(this);
#endif

    if(NULL != state->reg_data_stream)
    {
        event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
        while(NULL != event)
        {
            /** All registry events can be handled in normal mode. */
            state->island_service->api->sensor_island_exit(state->island_service, this);
            icm4x6xx_sensor_process_registry_event(this, event);

            event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
        }
    }

  if(state->common.registry_pf_cfg_received && state->common.registry_cfg_received &&
     state->common.registry_orient_received && state->common.registry_placement_received)
  {
    /** Initial HW detection sequence is OK to run in normal mode. */
    state->island_service->api->sensor_island_exit(state->island_service, this);
    if(sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL))
    {
      icm4x6xx_start_hw_detect_sequence(this);
    }
  }

    return rv;
}

static void  icm4x6xx_send_flush_config(sns_sensor *const this,
                                       sns_sensor_instance *instance)
{
    sns_request config;

    ICM4X6XX_PRINTF(MED, this, "flush req in");

    config.message_id = SNS_STD_MSGID_SNS_STD_FLUSH_REQ;
    config.request_len = 0;
    config.request = NULL;

    this->instance_api->set_client_config(instance, &config);
}

/**
 * Returns decoded request message for type
 * sns_std_sensor_config.
 *
 * @param[in] in_request   Request as sotred in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool icm4x6xx_get_decoded_imu_request(sns_sensor const *this, sns_request const *in_request,
                                            sns_std_request *decoded_request,
                                            sns_std_sensor_config *decoded_payload)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg =
      { .decoded_struct = decoded_payload,
        .fields = sns_std_sensor_config_fields };
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
  stream = pb_istream_from_buffer(in_request->request,
                                  in_request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    ICM4X6XX_PRINTF(ERROR, this, "E");
    return false;
  }
  return true;
}

/**
 * Decodes self test requests.
 *
 * @param[i] this              Sensor reference
 * @param[i] request           Encoded input request
 * @param[o] decoded_request   Decoded standard request
 * @param[o] test_config       decoded self test request
 *
 * @return bool True if decoding is successfull else false.
 */
static bool icm4x6xx_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
                                                  sns_std_request *decoded_request,
                                                  sns_physical_sensor_test_config *test_config)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg =
      { .decoded_struct = test_config,
        .fields = sns_physical_sensor_test_config_fields };
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
  stream = pb_istream_from_buffer(request->request,
                                  request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    ICM4X6XX_PRINTF(ERROR, this, "E");
    return false;
  }
  return true;
}

/**
 * Parses through all starndard streaming requests and deduces
 * best HW config for the inertial sensor type.
 *
 * @param[i] this             Sensor reference
 * @param[i] instance         Instance reference
 * @param[i] sensor_type      sensor type
 * @param[o] chosen_sample_rate   chosen sample rate in Hz
 * @param[o] chosen_report_rate   chosen report rate in Hz
 * @param[o] chosen_flush_period_ticks   chosen flush period in ticks
 * @param[o] passive_ngated_client_present  True if passive non-gated
 *       client is present.
 * @param[o] non_gated_sensor_client_present  True if non-gated
 *       client is present.
 * @param[o] gated_sensor_client_present  Ture if gated client is
 *       present.
 */
static void icm4x6xx_get_imu_config(sns_sensor *this,
            sns_sensor_instance *instance,
            icm4x6xx_sensor_type sensor_type,
            float *chosen_sample_rate,
            float *chosen_report_rate,
            float *chosen_dev_wakeup_sdc_rate,
            sns_time *chosen_flush_period_ticks,
            bool *passive_ngated_client_present,
            bool *non_gated_sensor_client_present,
            bool *gated_sensor_client_present,
            bool *max_batch,
            bool *flush_only)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    sns_sensor_uid *suid = NULL;
    sns_request const *request;
    bool is_max_batch = true;
    bool is_flush_only = true;

    ICM4X6XX_VERB_PRINTF(MED, this, "(0x%x)get imu config", sensor_type);

    if (sensor_type == ICM4X6XX_ACCEL) {
        suid = &state->common.a_suid;
    } else if (sensor_type == ICM4X6XX_GYRO) {
        suid = &state->common.g_suid;
    }

  *chosen_report_rate = 0.0f;
  *chosen_sample_rate = 0.0f;
  *passive_ngated_client_present = false;
  *non_gated_sensor_client_present = false;
  *chosen_flush_period_ticks = 0;
  if(gated_sensor_client_present)
  {
    *gated_sensor_client_present = false;
  }

    /** Parse through existing requests and get fastest sample
     *  rate and report rate requests. */
    for(request = instance->cb->get_client_request(instance, suid, true);
        NULL != request;
        request = instance->cb->get_client_request(instance, suid, false))
    {
        sns_std_request decoded_request;
        sns_std_sensor_config decoded_payload = {0};

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
       ||
       request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(icm4x6xx_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
      {
        float report_rate = 0.0f;
        float wakeup_sdc_rate = 0.0f;
        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                                       decoded_payload.sample_rate);
        //has_batching %u has_flush_period %d batch_period %d flush_period %d
        ICM4X6XX_PRINTF(MED, this, "(%d)hb %u hfp %d bp %d fp %d sr %d",
                        sensor_type,
                    decoded_request.has_batching, decoded_request.batching.has_flush_period,
                            decoded_request.batching.batch_period, decoded_request.batching.flush_period,
                        (uint32_t)(decoded_payload.sample_rate * 100));

        if(decoded_request.has_batching)
        {
          is_max_batch &= (decoded_request.batching.has_max_batch && decoded_request.batching.max_batch);
          is_flush_only &= (decoded_request.batching.has_flush_only && decoded_request.batching.flush_only);

          if (decoded_request.batching.has_flush_period)
          {
            /* Not sure why SEE framework set flush_period to 0, filter this */
            uint32 temp_period =
                     (decoded_request.batching.flush_period >= decoded_request.batching.batch_period) ?
                     decoded_request.batching.flush_period:
                     decoded_request.batching.batch_period;
            *chosen_flush_period_ticks =
              SNS_MAX(*chosen_flush_period_ticks,
                      sns_convert_ns_to_ticks((uint64_t)(temp_period)*1000ULL));
          }
          else
          {
            *chosen_flush_period_ticks =
              SNS_MAX(*chosen_flush_period_ticks,
                      sns_convert_ns_to_ticks((uint64_t)(decoded_request.batching.batch_period)*1000ULL));
          }
        }
        else
        {
          *chosen_flush_period_ticks = UINT64_MAX;
          is_max_batch = false;
          is_flush_only = false;
        }

        if(decoded_request.has_batching && decoded_request.batching.batch_period > 0) {
            report_rate = (1000000.0f / (float)decoded_request.batching.batch_period);
        }
        else
        {
          report_rate = decoded_payload.sample_rate;
        }

        wakeup_sdc_rate = report_rate;
        *chosen_dev_wakeup_sdc_rate = SNS_MAX(*chosen_dev_wakeup_sdc_rate, wakeup_sdc_rate);

        if (decoded_request.has_batching &&
            decoded_request.batching.has_flush_only &&
            decoded_request.batching.flush_only) {
            report_rate = (1.0f / UINT32_MAX);
        }

        *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);

        if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
#ifdef ICM4X6XX_PASSIVE_SENSOR_SUPPORT
            if(decoded_request.has_is_passive && decoded_request.is_passive)
            {
              *passive_ngated_client_present = true;
            }
            else
#endif
            *non_gated_sensor_client_present = true;
        }
        else
        {
          if(gated_sensor_client_present)
          {
            *gated_sensor_client_present = true;
          }
        }
      }
    }
  }

  /** If there is a client for the sensor and
   *  if max_batch is set in all requests then choose the largest WM. */
  if (is_max_batch) {
    if (*non_gated_sensor_client_present ||
       (gated_sensor_client_present && *gated_sensor_client_present)) {
      *chosen_report_rate = (1.0f / UINT32_MAX);
    }
  }

  *max_batch = is_max_batch;
  *flush_only = is_flush_only;
  ICM4X6XX_PRINTF(MED, this, "flush_only %u max_batch %d",
                is_flush_only, is_max_batch);
}

/**
 * Determines motion detect config.
 *
 * @param[i] this   Sensor reference
 * @param[i] instance  Sensor Instance reference
 * @param[o] chosen_md_enable  True if MD should be enabled
 * @param[o] md_client_present True if there is an MD client
 *       present.
 *
 * @return none
 */
static void icm4x6xx_get_motion_detect_config(sns_sensor *this,
                                             sns_sensor_instance *instance,
                                             bool *chosen_md_enable,
                                             bool *md_client_present)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    sns_sensor_uid *suid = &state->common.md_suid;
    icm4x6xx_instance_state *inst_state = (icm4x6xx_instance_state*)instance->state->state;
    sns_request const *request;

    *chosen_md_enable = false;
    *md_client_present = false;

    for(request = instance->cb->get_client_request(instance, suid, true);
        NULL != request;
        request = instance->cb->get_client_request(instance, suid, false))
    {
        if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
        {
            // Enable MD interrupt when:
            // 1. There is a new request and MD is in MDF/MDD state OR
            // 2. There is an existing request and MD is in MDE/MDD state
            // Introduced for power measurement testing,
            // Disable md interrupt using registry setting and send Disable event to md client,
            // if disable flag is true and client is present
            if (!state->md_config.disable) {
                *chosen_md_enable = ((inst_state->md_info.md_new_req &&
                                    (inst_state->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_FIRED
                                    || inst_state->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_DISABLED))
                                    ||
                                    (inst_state->md_info.md_client_present &&
                                    (inst_state->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_ENABLED
                                    || inst_state->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_DISABLED)));
            }

            *md_client_present = true;

            // Consumed new request
            inst_state->md_info.md_new_req = false;
            return;
        }
    }
}

#ifdef ICM4X6XX_ENABLE_LOWG
static void icm4x6xx_get_free_fall_config(
            sns_sensor_instance *instance,
            bool *freefall_client_present,
            bool *en_free_fall)
{
  icm4x6xx_instance_state *inst_state =
     (icm4x6xx_instance_state*)instance->state->state;
  sns_sensor_uid *suid = &inst_state->freefall_info.suid;
  sns_request const *request;

  *freefall_client_present = false;
  *en_free_fall = false;

  for(request = instance->cb->get_client_request(instance, suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, suid, false))
  {
    if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG) {
        *en_free_fall = ((inst_state->freefall_info.free_fall_new_req &&
                         (inst_state->freefall_info.free_fall_state.free_fall_event_type == SNS_FREE_FALL_EVENT_TYPE_FIRED ||
                          inst_state->freefall_info.free_fall_state.free_fall_event_type == SNS_FREE_FALL_EVENT_TYPE_DISABLED)) ||
                         (inst_state->freefall_info.free_fall_client_present &&
                         (inst_state->freefall_info.free_fall_state.free_fall_event_type == SNS_FREE_FALL_EVENT_TYPE_ENABLED ||
                          inst_state->freefall_info.free_fall_state.free_fall_event_type == SNS_FREE_FALL_EVENT_TYPE_DISABLED)));

      *freefall_client_present = true;
      inst_state->freefall_info.free_fall_new_req = false;
      return;
    }
  }
}
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
static void icm4x6xx_get_high_shock_config(
            sns_sensor_instance *instance,
            bool *high_shock_client_present,
            bool *en_high_shock)
{
  icm4x6xx_instance_state *inst_state =
     (icm4x6xx_instance_state*)instance->state->state;
  sns_sensor_uid *suid = &inst_state->high_shock_info.suid;
  sns_request const *request;

  *high_shock_client_present = false;
  *en_high_shock = false;

  for(request = instance->cb->get_client_request(instance, suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, suid, false))
  {
    if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG) {
        *en_high_shock = ((inst_state->high_shock_info.high_shock_new_req &&
                         (inst_state->high_shock_info.high_shock_state.high_shock_event_type == SNS_HIGH_SHOCK_EVENT_TYPE_FIRED ||
                          inst_state->high_shock_info.high_shock_state.high_shock_event_type == SNS_HIGH_SHOCK_EVENT_TYPE_DISABLED)) ||
                         (inst_state->high_shock_info.high_shock_client_present &&
                         (inst_state->high_shock_info.high_shock_state.high_shock_event_type == SNS_HIGH_SHOCK_EVENT_TYPE_ENABLED ||
                          inst_state->high_shock_info.high_shock_state.high_shock_event_type == SNS_HIGH_SHOCK_EVENT_TYPE_DISABLED)));

      *high_shock_client_present = true;
      inst_state->high_shock_info.high_shock_new_req = false;
      return;
    }
  }
}
#endif

/**
 * Parses through standard streaming requests for the sensor
 * temperature dataype and deduces best HW config.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Intance reference
 * @param[o] chosen_sample_rate  chosen sample rate in Hz
 * @param[o] chosen_report_rate  chosen report rate in Hz
 * @param[o] sensor_temp_client_present  True if there is a
 *       sensor temp client present
 * @param[o] sensor_temp_passive_client_present  True if there
 *       is a passive sensor temp client present
 *
 * @return none
 */
static void icm4x6xx_get_sensor_temp_config(sns_sensor *this,
            sns_sensor_instance *instance,
            float *chosen_sample_rate,
            float *chosen_report_rate,
            sns_time *chosen_flush_period_ticks,
            bool *sensor_temp_client_present,
            bool *sensor_temp_passive_client_present,
            bool *max_batch,
            bool *flush_only)
{
  icm4x6xx_state *state =
     (icm4x6xx_state*)this->state->state;
  icm4x6xx_instance_state *inst_state =
     (icm4x6xx_instance_state*)instance->state->state;
  sns_sensor_uid *suid = &state->common.st_suid;
  sns_request const *request;
  bool is_max_batch = true;
  bool is_flush_only = true;

  *chosen_report_rate = 0.0f;
  *chosen_sample_rate = 0.0f;
  *sensor_temp_client_present = false;
  *sensor_temp_passive_client_present = false;
  *chosen_flush_period_ticks = 0;

    /** Parse through existing requests and get fastest sample
   *  rate and report rate requests. */
  for(request = instance->cb->get_client_request(instance, suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(icm4x6xx_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
      {
        float report_rate = 0.0f;
        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                                       decoded_payload.sample_rate);

        if(decoded_request.has_batching
           &&
           decoded_request.batching.batch_period > 0)
        {
          report_rate = (1000000.0 / (float)decoded_request.batching.batch_period);
        }
        else
        {
          report_rate = decoded_payload.sample_rate;
        }

        if(decoded_request.has_batching)
        {
          is_max_batch &= (decoded_request.batching.has_max_batch && decoded_request.batching.max_batch);
          is_flush_only &= (decoded_request.batching.flush_only);

          if(decoded_request.batching.has_flush_period)
          {
            *chosen_flush_period_ticks =
              SNS_MAX(*chosen_flush_period_ticks,
                      sns_convert_ns_to_ticks((uint64_t)(decoded_request.batching.flush_period)*1000ULL));
          }
          else
          {
            *chosen_flush_period_ticks =
              SNS_MAX(*chosen_flush_period_ticks,
                      sns_convert_ns_to_ticks((uint64_t)(decoded_request.batching.batch_period)*1000ULL));
          }
        }
        else
        {
          *chosen_flush_period_ticks = UINT64_MAX;
          is_max_batch = false;
          is_flush_only = false;
        }

        *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);

#ifdef ICM4X6XX_PASSIVE_SENSOR_SUPPORT
        if(decoded_request.has_is_passive && decoded_request.is_passive)
        {
          *sensor_temp_passive_client_present = true;
        }
        else
#endif
        *sensor_temp_client_present = true;
      }
    }
  }

  if(*sensor_temp_client_present &&
     (is_max_batch || is_flush_only))
    {
      *chosen_report_rate = (1.0f / UINT32_MAX);
    }

  *max_batch = is_max_batch;
  *flush_only = is_flush_only;

  inst_state->sensor_temp_info.report_rate_hz  = *chosen_report_rate;
  inst_state->sensor_temp_info.sampling_rate_hz = *chosen_sample_rate;
}

/**
 * Set standard streaming config for the instance.
 *
 * @param[i] this        Sensor reference
 * @param[i] instance    Sensor Instance reference
 * @param[i] chosen_report_rate   chosen report rate in Hz
 * @param[i] chosen_sample_rate   chosen sample rate in Hz
 * @param[i] chosen_flush_period_ticks chosen flush period in tick
 * @param[i] registry_cfg Sensor specific registry configuration
 * @param[i] message_id           input message ID
 *
 * @return none
 */
static void icm4x6xx_set_inst_config(sns_sensor *this,
                                    sns_sensor_instance *instance,
                                    float chosen_report_rate,
                                    float chosen_wakeup_sdc_rate,
                                    float chosen_sample_rate,
                                    sns_time chosen_flush_period_ticks,
                                    sns_icm4x6xx_registry_cfg registry_cfg,
                                    uint32_t message_id)
{
    sns_icm4x6xx_req new_client_config;
    sns_request config;

    new_client_config.desired_report_rate = chosen_report_rate;
    new_client_config.desired_wakeup_sdc_rate  = chosen_wakeup_sdc_rate;
    new_client_config.desired_sample_rate = chosen_sample_rate;
    new_client_config.desired_flush_ticks = chosen_flush_period_ticks;
    new_client_config.registry_cfg = registry_cfg;

    config.message_id = message_id;
    config.request_len = sizeof(sns_icm4x6xx_req);
    config.request = &new_client_config;

    this->instance_api->set_client_config(instance, &config);
}

/**
 * Turns power rails off
 *
 * @paramp[i] this   Sensor reference
 *
 * @return none
 */
static void icm4x6xx_turn_rails_off(sns_sensor *this)
{
  sns_sensor *sensor;

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    icm4x6xx_state *sensor_state = (icm4x6xx_state*)sensor->state->state;
    if(sensor_state->common.rail_config.rail_vote != SNS_RAIL_OFF)
    {
      sensor_state->common.rail_config.rail_vote = SNS_RAIL_OFF;
      sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(sensor_state->pwr_rail_service,
                                                                      sensor,
                                                                      &sensor_state->common.rail_config,
                                                                      NULL);
    }
  }
}

/**
 * Copy sensor specfic registry info into instance state (Ex: md_config)
 *
 * @param[i] this        Sensor reference
 * @param[i] inst_state  Sensor Instance state reference
 *
 * @return None
 */
static void icm4x6xx_save_sensor_info_into_inst(
            sns_sensor *const this, icm4x6xx_instance_state *inst_state)
{
  sns_sensor *sensor = NULL;
  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    icm4x6xx_state *sensor_state = (icm4x6xx_state*)sensor->state->state;
    float *fac_cal_bias = NULL;
    matrix3 *fac_cal_corr_mat = NULL;

    /** Save factory bias from registry config into instance state */
    if (ICM4X6XX_ACCEL == sensor_state->sensor &&
        sensor_state->fac_cal_version >= inst_state->accel_registry_cfg.version)
    {
      fac_cal_bias = inst_state->accel_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &inst_state->accel_registry_cfg.fac_cal_corr_mat;
      inst_state->accel_registry_cfg.version = sensor_state->fac_cal_version;
    } else if (ICM4X6XX_GYRO == sensor_state->sensor &&
               sensor_state->fac_cal_version >= inst_state->gyro_registry_cfg.version)
    {
      fac_cal_bias = inst_state->gyro_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &inst_state->gyro_registry_cfg.fac_cal_corr_mat;
      inst_state->gyro_registry_cfg.version = sensor_state->fac_cal_version;
    }
    else if (ICM4X6XX_SENSOR_TEMP == sensor_state->sensor
      && sensor_state->fac_cal_version >= inst_state->sensor_temp_registry_cfg.version)
    {
      fac_cal_bias = inst_state->sensor_temp_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &inst_state->sensor_temp_registry_cfg.fac_cal_corr_mat;
      inst_state->sensor_temp_registry_cfg.version = sensor_state->fac_cal_version;
    }

    if (NULL != fac_cal_bias && NULL != fac_cal_corr_mat)
    {
      sns_memscpy(fac_cal_bias, sizeof(sensor_state->fac_cal_bias),
                  sensor_state->fac_cal_bias, sizeof(sensor_state->fac_cal_bias));

      sns_memscpy(fac_cal_corr_mat, sizeof(sensor_state->fac_cal_corr_mat),
                  &sensor_state->fac_cal_corr_mat,
                  sizeof(sensor_state->fac_cal_corr_mat));
    }

    /*Save md sensor registry config into instance state */
    if(ICM4X6XX_MOTION_DETECT == sensor_state->sensor)
    {
      sns_memscpy(&inst_state->md_info.md_config, sizeof(inst_state->md_info.md_config),
        &sensor_state->md_config, sizeof(sensor_state->md_config));
    }
  }
}

/**
 * Decodes a physical sensor self test request and updates
 * instance state with request info.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 * @param[i] new_request Encoded request
 *
 * @return True if request is valid else false
 */
static bool icm4x6xx_extract_self_test_info(sns_sensor *this,
                              sns_sensor_instance *instance,
                              struct sns_request const *new_request)
{
  sns_std_request decoded_request;
  sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  icm4x6xx_instance_state *inst_state = (icm4x6xx_instance_state*)instance->state->state;
  icm4x6xx_self_test_info *self_test_info;

  if(state->sensor == ICM4X6XX_ACCEL)
  {
    self_test_info = &inst_state->accel_info.test_info;
  }
  else if(state->sensor == ICM4X6XX_GYRO)
  {
    self_test_info = &inst_state->gyro_info.test_info;
  }
  else if(state->sensor == ICM4X6XX_MOTION_DETECT)
  {
    self_test_info = &inst_state->md_info.test_info;
  }
  else if(state->sensor == ICM4X6XX_SENSOR_TEMP)
  {
    self_test_info = &inst_state->sensor_temp_info.test_info;
  }
#ifdef ICM4X6XX_ENABLE_LOWG
  else if(state->sensor == ICM4X6XX_FREE_FALL)
  {
    self_test_info = &inst_state->freefall_info.test_info;
  }
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
  else if(state->sensor == ICM4X6XX_HIGH_SHOCK)
  {
    self_test_info = &inst_state->high_shock_info.test_info;
  }
#endif
  else
  {
    return false;
  }

  if(icm4x6xx_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
  {
    self_test_info->test_type = test_config.test_type;
    self_test_info->test_client_present = true;
    return true;
  }
  else
  {
    return false;
  }
}

/** See sns_icm4x6xx_sensor.h */
void icm4x6xx_reval_instance_config(sns_sensor *this,
                              sns_sensor_instance *instance,
                              icm4x6xx_sensor_type sensor_type)
{
    /**
     * 1. Get best non-gated and gated Accel Config.
     * 2. Get best Gyro Config.
     * 3. Get Motion Detect Config.
     * 4. Get best Sensor Temperature Config.
     * 5. Decide best Instance Config based on above outputs.
     */
    float chosen_sample_rate = 0.0f;
    float chosen_report_rate = 0.0f;
    float chosen_wakeup_sdc_rate = 0.0f;
    sns_time chosen_flush_period_ticks = 0;
    float sample_rate = 0.0f;
    float report_rate = 0.0f;
    float wakeup_sdc_rate = 0.0f;
    sns_time flush_period_ticks = 0;

    bool a_passive_ngated_client_present;
    bool g_passive_ngated_client_present;
    bool a_sensor_client_present_non_gated;
    bool a_sensor_client_present_gated;
    bool g_sensor_client_present;
    bool md_sensor_client_present;
    bool sensor_temp_client_present;
    bool sensor_temp_passive_client_present;
    bool a_max_batch = false;
    bool a_flush_only = false;
    bool g_max_batch = false;
    bool g_flush_only = false;
    bool t_max_batch = false;
    bool t_flush_only = false;

    icm4x6xx_instance_state *inst_state =
        (icm4x6xx_instance_state*)instance->state->state;
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;

    sns_icm4x6xx_registry_cfg registry_cfg = {.sensor_type = sensor_type};

    icm4x6xx_get_imu_config(this, instance, ICM4X6XX_ACCEL, &chosen_sample_rate,
            &chosen_report_rate, &chosen_wakeup_sdc_rate, &chosen_flush_period_ticks,
            &a_passive_ngated_client_present,
            &a_sensor_client_present_non_gated, &a_sensor_client_present_gated,
            &a_max_batch, &a_flush_only);

    inst_state->accel_info.gated_client_present = a_sensor_client_present_gated;

    ICM4X6XX_PRINTF(MED, this, "A: rr %u/100 sr %u/100 flush_period %u gated client %u",
             (uint32_t)(chosen_report_rate*100), (uint32_t)(chosen_sample_rate*100),
             (uint32_t)chosen_flush_period_ticks, a_sensor_client_present_gated);

    icm4x6xx_get_imu_config(this, instance, ICM4X6XX_GYRO, &sample_rate,
            &report_rate, &wakeup_sdc_rate, &flush_period_ticks,
            &g_passive_ngated_client_present,
            &g_sensor_client_present, NULL,
            &g_max_batch, &g_flush_only);

    ICM4X6XX_PRINTF(MED, this, "G: rr %u/100 sr %u/100 flush_period %u",
             (uint32_t)(report_rate*100), (uint32_t)(sample_rate*100),
             (uint32_t)flush_period_ticks);

    // From sns_std.proto regarding is_passive:
    // Actively enabling one sensor shall not lead to enabling of another
    // sensor having only passive requests.
    if(a_sensor_client_present_non_gated || a_sensor_client_present_gated || g_sensor_client_present)
    {
        chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
        chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
        chosen_wakeup_sdc_rate = SNS_MAX(chosen_wakeup_sdc_rate, wakeup_sdc_rate);
        chosen_flush_period_ticks = SNS_MAX(chosen_flush_period_ticks, flush_period_ticks);
    }

    icm4x6xx_get_motion_detect_config(this, instance,
            &inst_state->md_info.enable_md_int,
            &md_sensor_client_present);

    inst_state->md_info.md_client_present = md_sensor_client_present;

#ifdef ICM4X6XX_ENABLE_LOWG
    bool freefall_client_present = false;
    icm4x6xx_get_free_fall_config(instance, &freefall_client_present,
                                   &inst_state->freefall_info.enable_free_fall);

    inst_state->freefall_info.free_fall_client_present = freefall_client_present;
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
    bool high_shock_client_present = false;
    icm4x6xx_get_high_shock_config(instance, &high_shock_client_present,
                                   &inst_state->high_shock_info.enable_high_shock);

    inst_state->high_shock_info.high_shock_client_present = high_shock_client_present;
#endif

    if(a_sensor_client_present_non_gated)
    {
        inst_state->md_info.enable_md_int = false;
    }

    inst_state->fifo_info.flush_only = false;
    inst_state->fifo_info.max_batch = false;

    if ((a_sensor_client_present_non_gated || a_sensor_client_present_gated) &&
        g_sensor_client_present) {
        chosen_flush_period_ticks = (a_max_batch && g_max_batch) ? UINT64_MAX : chosen_flush_period_ticks;
        inst_state->fifo_info.max_requested_wmk = (a_flush_only && g_flush_only) ? UINT32_MAX :
                inst_state->fifo_info.max_requested_wmk;
        inst_state->fifo_info.flush_only = a_flush_only && g_flush_only;
        inst_state->fifo_info.max_batch = a_max_batch && g_max_batch;
    } else if ((a_sensor_client_present_non_gated || a_sensor_client_present_gated)) {
        chosen_flush_period_ticks = a_max_batch ? UINT64_MAX : chosen_flush_period_ticks;
        inst_state->fifo_info.max_requested_wmk = a_flush_only ? UINT32_MAX :
                inst_state->fifo_info.max_requested_wmk;
        inst_state->fifo_info.flush_only = a_flush_only;
        inst_state->fifo_info.max_batch = a_max_batch;
    } else if (g_sensor_client_present) {
        chosen_flush_period_ticks = g_max_batch ? UINT64_MAX : chosen_flush_period_ticks;
        inst_state->fifo_info.max_requested_wmk = g_flush_only ? UINT32_MAX :
                inst_state->fifo_info.max_requested_wmk;
        inst_state->fifo_info.flush_only = g_flush_only;
        inst_state->fifo_info.max_batch = g_max_batch;
    }

    icm4x6xx_get_sensor_temp_config(this, instance, &sample_rate, &report_rate,
               &flush_period_ticks,
               &sensor_temp_client_present,
               &sensor_temp_passive_client_present,
               &t_max_batch, &t_flush_only);
    inst_state->sensor_temp_info.max_requested_flush_ticks = flush_period_ticks;

    if (t_max_batch/* || t_flush_only*/)
      inst_state->sensor_temp_info.max_requested_flush_ticks = UINT64_MAX;

    ICM4X6XX_PRINTF(MED, this, "temp: rr %u sr %u flush_period %u",
             (uint32_t)report_rate, (uint32_t)sample_rate,
             (uint32_t)flush_period_ticks);

    /** Start with a clean slate */
    inst_state->accel_info.desire_en = false;
    inst_state->gyro_info.desire_en = false;
    inst_state->accel_info.en_fifo = false;
    inst_state->gyro_info.en_fifo = false;
    inst_state->accel_info.desire_publish = false;
    inst_state->gyro_info.desire_publish = false;
    inst_state->sensor_temp_info.desire_publish = false;
    inst_state->a_new_stream = false;
    inst_state->g_new_stream = false;

    if(a_sensor_client_present_non_gated)
    {
        inst_state->accel_info.desire_en = true;
        inst_state->accel_info.en_fifo = true;
        inst_state->accel_info.desire_publish = true;
        inst_state->a_new_stream = state->new_stream & true;
    }

    ICM4X6XX_PRINTF(MED, this, "a/g passive %d/%d, a ngatd/gatd %d/%d, g client %d, md client/en %d/%d",
                a_passive_ngated_client_present, g_passive_ngated_client_present,
                a_sensor_client_present_non_gated, a_sensor_client_present_gated, g_sensor_client_present,
                inst_state->md_info.md_client_present, inst_state->md_info.enable_md_int);

#ifdef ICM4X6XX_ENABLE_LOWG
    ICM4X6XX_PRINTF(MED, this, "freefall: client %d en %d",
                inst_state->freefall_info.free_fall_client_present,
                inst_state->freefall_info.enable_free_fall);
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
    ICM4X6XX_PRINTF(MED, this, "high_shock: client %d en %d",
                inst_state->high_shock_info.high_shock_client_present,
                inst_state->high_shock_info.enable_high_shock);
#endif

    if(a_sensor_client_present_gated)
    {
        inst_state->accel_info.desire_en = true;
        /* Acceptance test tool v74.1 Acc-411/421/431 */
        inst_state->a_new_stream = state->new_stream & true;
        if(!inst_state->md_info.enable_md_int)
        {
            inst_state->accel_info.en_fifo = true;
            inst_state->accel_info.desire_publish = true;
        }
    }

    if(g_sensor_client_present)
    {
        if (HAVANA == state->common.product_info.series) /* Gyro ODR regularity would be interrupted by Accel enable */
            inst_state->accel_info.desire_en = true;
        inst_state->gyro_info.desire_en = true;
        inst_state->gyro_info.en_fifo = true;
        inst_state->gyro_info.desire_publish = true;
        inst_state->g_new_stream = state->new_stream & true;
    }

#ifdef ICM4X6XX_ENABLE_LOWG
    if (inst_state->freefall_info.enable_free_fall) {
#ifdef ICM4X6XX_HIGHG_LOWG_HIGH_PERFORMANCE
        chosen_sample_rate = SNS_MAX(chosen_sample_rate, ICM4X6XX_ODR_1000);
#else
        chosen_sample_rate = SNS_MAX(chosen_sample_rate, ICM4X6XX_ODR_200);
#endif
        inst_state->accel_info.desire_en = true;
    }
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
    if (inst_state->high_shock_info.enable_high_shock) {
#ifdef ICM4X6XX_HIGHG_LOWG_HIGH_PERFORMANCE
        chosen_sample_rate = SNS_MAX(chosen_sample_rate, ICM4X6XX_ODR_1000);
#else
        chosen_sample_rate = SNS_MAX(chosen_sample_rate, ICM4X6XX_ODR_200);
#endif
        inst_state->accel_info.desire_en = true;
    }
#endif

    if(md_sensor_client_present)
    {
        inst_state->accel_info.desire_en = true;
    }

    if(sensor_temp_client_present)
    {
        //need to en acc sensor for temp sensor to work, check DS
        inst_state->accel_info.desire_en = true;
        inst_state->sensor_temp_info.desire_publish = true;
    }

    if(ICM4X6XX_ACCEL == sensor_type || ICM4X6XX_GYRO == sensor_type ||
        ICM4X6XX_SENSOR_TEMP == sensor_type)
    {
        sns_memscpy(registry_cfg.fac_cal_bias, sizeof(registry_cfg.fac_cal_bias),
                state->fac_cal_bias, sizeof(state->fac_cal_bias));

        sns_memscpy(&registry_cfg.fac_cal_corr_mat, sizeof(registry_cfg.fac_cal_corr_mat),
                &state->fac_cal_corr_mat, sizeof(state->fac_cal_corr_mat));
        registry_cfg.version = state->fac_cal_version;
    }

#ifndef ICM4X6XX_USE_INT_TS
    /* Use always 16 bytes fifo format, so we can use tmst in FIFO */
    if (inst_state->gyro_info.en_fifo || inst_state->accel_info.en_fifo) {
        inst_state->gyro_info.en_fifo = true;
        inst_state->accel_info.en_fifo = true;
    }
#endif

#ifdef ICM4X6XX_DISABLE_DAE
    if ((inst_state->product_info.series == HAVANA) &&
        (inst_state->gyro_info.en_fifo || inst_state->accel_info.en_fifo)) {
        inst_state->gyro_info.en_fifo = true;
        inst_state->accel_info.en_fifo = true;
    }
#endif

    icm4x6xx_set_inst_config(this,
                             instance,
                             chosen_report_rate,
                             chosen_wakeup_sdc_rate,
                             chosen_sample_rate,
                             chosen_flush_period_ticks,
                             registry_cfg,
                             SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
}

#ifdef ICM4X6XX_DEBUG_VERBOSE
static void icm4x6xx_debug_all_request(
            sns_sensor *const this,
            struct sns_request const *exist_request,
            struct sns_request const *new_request,
            bool remove)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};
    sns_std_request decoded_request_new;
    sns_std_sensor_config decoded_payload_new = {0};

    state->island_service->api->sensor_island_exit(state->island_service, this);

    ICM4X6XX_PRINTF(HIGH, this, "(0x%x)set_client_req: req=%d/%d rm=%u",
             state->sensor, exist_request != NULL ? exist_request->message_id : -1,
             new_request != NULL ? new_request->message_id : -1, remove);

    if (remove) {
        if (exist_request != NULL &&
            (exist_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG ||
            exist_request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)) {
            if (icm4x6xx_get_decoded_imu_request(this, exist_request, &decoded_request, &decoded_payload)) {
                ICM4X6XX_PRINTF(HIGH, this, "(0x%x)req debug removing batch: %u period %d, sr %d/100, flush: %d period %d",
                            state->sensor,
                            decoded_request.has_batching, decoded_request.batching.batch_period,
                            (uint32_t)(decoded_payload.sample_rate * 100),
                            decoded_request.batching.has_flush_period, decoded_request.batching.flush_period);
            }
        }

        if (new_request != NULL) {
            ICM4X6XX_PRINTF(ERROR, this, "req debug strange input parameter!!!");
        }
    } else {
        if (NULL != exist_request && NULL != new_request &&
            (new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG ||
             new_request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)) {
            if (icm4x6xx_get_decoded_imu_request(this, exist_request, &decoded_request, &decoded_payload)) {
                ICM4X6XX_PRINTF(HIGH, this, "(0x%x)req debug updating removing batch: %u period %d, sr %d/100, flush: %d period %d",
                            state->sensor,
                            decoded_request.has_batching, decoded_request.batching.batch_period,
                            (uint32_t)(decoded_payload.sample_rate * 100),
                            decoded_request.batching.has_flush_period, decoded_request.batching.flush_period);
            }
            if (icm4x6xx_get_decoded_imu_request(this, new_request, &decoded_request_new, &decoded_payload_new)) {
                ICM4X6XX_PRINTF(HIGH, this, "(0x%x)req debug updating request new batch: %u period %d, sr %d/100, flush: %d period %d",
                            state->sensor,
                            decoded_request_new.has_batching, decoded_request_new.batching.batch_period,
                            (uint32_t)(decoded_payload_new.sample_rate * 100),
                            decoded_request_new.batching.has_flush_period, decoded_request_new.batching.flush_period);
            }
        } else if (NULL != new_request &&
            (new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG ||
             new_request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)) {
            if (icm4x6xx_get_decoded_imu_request(this, new_request, &decoded_request_new, &decoded_payload_new)) {
                ICM4X6XX_PRINTF(HIGH, this, "(0x%x)req debug request new batch: %u period %d, sr %d/100, flush: %d period %d",
                            state->sensor,
                            decoded_request_new.has_batching, decoded_request_new.batching.batch_period,
                            (uint32_t)(decoded_payload_new.sample_rate * 100),
                            decoded_request_new.batching.has_flush_period, decoded_request_new.batching.flush_period);
            }

        } else if (NULL != exist_request && NULL == new_request) {
            ICM4X6XX_PRINTF(ERROR, this, "req debug strange input parameter!!!");
        }
    }
}
#endif

/** See sns_icm4x6xx_sensor.h */
struct sns_sensor_instance* icm4x6xx_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove)
{
    sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    sns_time on_timestamp;
    sns_time delta;
    bool reval_config = false;

#ifndef ICM4X6XX_DEBUG_VERBOSE
    ICM4X6XX_PRINTF(MED, this, "(0x%x)set_client_req 0x%X(%d) 0x%X(%d) rm=%d",
                    state->sensor,
                    exist_request,
                    exist_request == NULL ? 0 : exist_request->message_id,
                    new_request,
                    new_request == NULL ? 0 : new_request->message_id,
                    remove);
#else
    icm4x6xx_debug_all_request(this, exist_request, new_request, remove);
#endif

    if(remove)
    {
        if(NULL != instance)
        {
            if (exist_request->message_id ==
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG) {
                state->new_stream = false;
            }

            if (exist_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG ||
                exist_request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG) {
                state->new_stream = true;
            }

            instance->cb->remove_client_request(instance, exist_request);
            /* Assumption: The FW will call deinit() on the instance before destroying it.
             Putting all HW resources (sensor HW, COM port, power rail)in
             low power state happens in Instance deinit().*/

            icm4x6xx_reval_instance_config(this, instance, state->sensor);
        }
    }
    else
    {
        // 1. If new request then:
        //     a. Power ON rails.
        //     b. Power ON COM port - Instance must handle COM port power.
        //     c. Create new instance.
        //     d. Re-evaluate existing requests and choose appropriate instance config.
        //     e. set_client_config for this instance.
        //     f. Add new_request to list of requests handled by the Instance.
        //     g. Power OFF COM port if not needed- Instance must handle COM port power.
        //     h. Return the Instance.
        // 2. If there is an Instance already present:
        //     a. Add new_request to list of requests handled by the Instance.
        //     b. Remove exist_request from list of requests handled by the Instance.
        //     c. Re-evaluate existing requests and choose appropriate Instance config.
        //     d. set_client_config for the Instance if not the same as current config.
        //     e. publish the updated config.
        //     f. Return the Instance.
        // 3.  If "flush" request:
        //     a. Perform flush on the instance.
        //     b. Return NULL.

        if(NULL == instance)
        {
            ICM4X6XX_PRINTF(MED, this, "inst is not created");
            if(state->sensor == ICM4X6XX_GYRO)
            {
                state->common.rail_config.rail_vote = SNS_RAIL_ON_NPM;
            }
            else
            {
                state->common.rail_config.rail_vote = state->common.registry_rail_on_state;
            }

            state->pwr_rail_service->api->sns_vote_power_rail_update(
                                            state->pwr_rail_service,
                                            this,
                                            &state->common.rail_config,
                                            &on_timestamp);

            delta = sns_get_system_time() - on_timestamp;

            // Use on_timestamp to determine correct Timer value.
            if(delta < sns_convert_ns_to_ticks(ICM4X6XX_OFF_TO_IDLE_MS*1000*1000))
            {
                icm4x6xx_start_power_rail_timer(this,
                                        sns_convert_ns_to_ticks(ICM4X6XX_OFF_TO_IDLE_MS*1000*1000) - delta,
                                        ICM4X6XX_POWER_RAIL_PENDING_SET_CLIENT_REQ);
            }
            else
            {
                // rail is already ON
                ICM4X6XX_PRINTF(HIGH, this, "rail alrdy ON"); //rail is already on
                /* Somehow, power rail is already on, but instance is not created,
                 * we give some time for register INT GPIO before enabling accel sensor,
                 * due to:
                 *  a. it seems Yokohama also has accel quick responce feature
                 *  b. if we don't start timer here, after enabling accel, irq reg is still not ready,
                       but there is one accel sample, and when next accel sample is ready, the INT event
                       comes into driver, it will cause match INT with FIFO count incorrectly.
                 * So we start timer here to make driver more robust */
                icm4x6xx_start_power_rail_timer(this,
                                        sns_convert_ns_to_ticks(ICM4X6XX_OFF_TO_IDLE_MS*1000*1000),
                                        ICM4X6XX_POWER_RAIL_PENDING_SET_CLIENT_REQ);

                //reval_config = true;
            }

            /** create_instance() calls init() for the Sensor Instance */
            instance = this->cb->create_instance(this,
                                            sizeof(icm4x6xx_instance_state));

            icm4x6xx_instance_state *inst_state =
                (icm4x6xx_instance_state*)instance->state->state;

            /* Save MD INFO to instance state */
            icm4x6xx_save_sensor_info_into_inst(this, inst_state);

            /* If rail is already ON then flag instance OK to configure */
            if(reval_config)
                inst_state->instance_is_ready_to_configure = true;
        }
        else
        {
            icm4x6xx_instance_state *inst_state =
                (icm4x6xx_instance_state*)instance->state->state;
            ICM4X6XX_PRINTF(MED, this, "inst created, new req msg_id=%d, hw_idx=%d",
                new_request->message_id, state->hardware_id);

            if (inst_state->new_self_test_request) {
                //self test is in progress, reject this request, defined in sns_physical_sensor_test.proto
                ICM4X6XX_PRINTF(HIGH, this, "self testing, reject %d",
                                        new_request->message_id);
                return instance;
            }

            if(NULL != exist_request && NULL != new_request
                && new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
            {
                if ((state->sensor == ICM4X6XX_ACCEL && inst_state->accel_info.is_enabled) ||
                    (state->sensor == ICM4X6XX_GYRO && inst_state->gyro_info.is_enabled)) {
                    icm4x6xx_send_flush_config(this, instance);
                    /** Do not update instance client request list at this point
                    because FIFO flush is a transitory request for an on-going
                    stream request. */
                    return instance;
                } else {
                    /** There aren't any FIFO sensors enabled to support flush.
                    *  Send flush complete event anyway. */
                    icm4x6xx_send_fifo_flush_done(instance, &state->my_suid);
                    return instance;
                }
            }
            else
            {
                reval_config = true;

                /** An existing client is changing request*/
                if((NULL != exist_request) && (NULL != new_request))
                {
                    instance->cb->remove_client_request(instance, exist_request);
                }
                /** A new client sent new_request*/
                else if(NULL != new_request)
                {
                    // No-op. new_request will be added to requests list below.
                }
            }
        }

        /** Add the new request to list of client_requests.*/
        if(NULL != instance)
        {
            icm4x6xx_instance_state *inst_state =
                (icm4x6xx_instance_state*)instance->state->state;

            if(NULL != new_request)
            {
                instance->cb->add_client_request(instance, new_request);

                if (new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG
                    && state->sensor == ICM4X6XX_MOTION_DETECT) {
                    if (inst_state->md_info.enable_md_int) {
                        reval_config = false;
                        //there is exsisting md client already present, just send event
                        icm4x6xx_send_config_event(instance);
                        pb_send_event(instance,
                                      sns_motion_detect_event_fields,
                                      &inst_state->md_info.md_state,
                                      sns_get_system_time(),
                                      SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                                      &inst_state->md_info.suid);
                        ICM4X6XX_PRINTF(LOW,this,"md is in, just send event");
                    } else {
                        inst_state->md_info.md_new_req = true;
                    }
                }

#ifdef ICM4X6XX_ENABLE_LOWG
                if (new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG
                    && state->sensor == ICM4X6XX_FREE_FALL) {
                    if (inst_state->freefall_info.enable_free_fall) {
                        reval_config = false;
                        //there is exsisting freefall client already present, just send event
                        icm4x6xx_send_config_event(instance);
                        pb_send_event(instance,
                                      sns_free_fall_event_fields,
                                      &inst_state->freefall_info.free_fall_state,
                                      sns_get_system_time(),
                                      SNS_FREE_FALL_MSGID_SNS_FREE_FALL_EVENT,
                                      &inst_state->freefall_info.suid);
                        ICM4X6XX_PRINTF(LOW,this,"freefall is in, just send event");
                    } else {
                        inst_state->freefall_info.free_fall_new_req = true;
                    }
                }
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
                if (new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG
                    && state->sensor == ICM4X6XX_HIGH_SHOCK) {
                    if (inst_state->high_shock_info.enable_high_shock) {
                        reval_config = false;
                        //there is exsisting high_shock client already present, just send event
                        icm4x6xx_send_config_event(instance);
                        pb_send_event(instance,
                                      sns_high_shock_event_fields,
                                      &inst_state->high_shock_info.high_shock_state,
                                      sns_get_system_time(),
                                      SNS_HIGH_SHOCK_MSGID_SNS_HIGH_SHOCK_EVENT,
                                      &inst_state->high_shock_info.suid);
                        ICM4X6XX_PRINTF(LOW,this,"high_shock is in, just send event");
                    } else {
                        inst_state->high_shock_info.high_shock_new_req = true;
                    }
                }
#endif

                if (new_request->message_id == SNS_CAL_MSGID_SNS_CAL_RESET) {
                    ICM4X6XX_PRINTF(LOW,this,"Req reset cal data");
                    state->island_service->api->sensor_island_exit(state->island_service, this);
                    icm4x6xx_reset_cal_data(instance,state->sensor);
                    icm4x6xx_update_sensor_state(this, instance);
                    icm4x6xx_update_registry(this, instance, ICM4X6XX_ACCEL);
                    icm4x6xx_update_registry(this, instance, ICM4X6XX_GYRO);
                    //need to send cal event, from QC's request after acceptance test tool v71.6
                    icm4x6xx_send_cal_event(instance, state->sensor);
                }

                if (new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG ||
                    new_request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG) {
                    state->new_stream = true;
                }

#ifdef ICM4X6XX_OEM_TEST
                if (new_request->message_id ==
                    SNS_PHYSICAL_SENSOR_OEM_CONFIG_MSGID_SNS_PHYSICAL_SENSOR_OEM_CONFIG) {
                    state->island_service->api->sensor_island_exit(state->island_service, this);
                    icm4x6xx_handle_oem_request(this, instance, new_request);
                }
#endif

                if(new_request->message_id ==
                    SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
                {
                    if(icm4x6xx_extract_self_test_info(this, instance, new_request))
                    {
                        inst_state->new_self_test_request = true;
                    }
                    state->new_stream = false;
                }
            }

            if(reval_config && inst_state->instance_is_ready_to_configure)
            {
                icm4x6xx_reval_instance_config(this, instance, state->sensor);
                if(inst_state->new_self_test_request)
                {
                    icm4x6xx_set_self_test_inst_config(this, instance);
                }
            }
        }
    }

#ifdef ICM4X6XX_W_BIAS_TO_REG_IMMEDIATELY
  {
    icm4x6xx_instance_state *inst_state =
            (icm4x6xx_instance_state*)instance->state->state;
    if(inst_state->update_fac_cal_in_registry)
    {
        /** Registry write can be handled in normal mode. */
        ICM4X6XX_PRINTF(MED, this, "W to REG"); // write to REG
        state->island_service->api->sensor_island_exit(state->island_service, this);
        /** Update factory cal data in the registry. */
        icm4x6xx_update_registry(this, instance, ICM4X6XX_ACCEL);
        icm4x6xx_update_registry(this, instance, ICM4X6XX_GYRO);
        icm4x6xx_update_sensor_state(this, instance);
        inst_state->update_fac_cal_in_registry = false;
    }
  }
#endif

    // QC: Sensors are required to call remove_instance when clientless
    if (NULL != instance &&
        NULL == instance->cb->get_client_request(instance,
            &state->common.md_suid, true) &&
        NULL == instance->cb->get_client_request(instance,
            &state->common.a_suid, true) &&
        NULL == instance->cb->get_client_request(instance,
            &state->common.g_suid, true) &&
        NULL == instance->cb->get_client_request(instance,
            &state->common.st_suid, true)
#ifdef ICM4X6XX_ENABLE_LOWG
        && NULL == instance->cb->get_client_request(instance,
            &state->common.free_fall_suid, true)
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
        && NULL == instance->cb->get_client_request(instance,
            &state->common.high_shock_suid, true)
#endif
       )
    {
#ifndef ICM4X6XX_W_BIAS_TO_REG_IMMEDIATELY
        icm4x6xx_instance_state *inst_state =
               (icm4x6xx_instance_state*)instance->state->state;
        if(inst_state->update_fac_cal_in_registry)
        {
            /** Registry write can be handled in normal mode. */
            ICM4X6XX_PRINTF(MED, this, "W to REG"); // write to REG
            state->island_service->api->sensor_island_exit(state->island_service, this);
            /** Update factory cal data in the registry. */
            icm4x6xx_update_registry(this, instance, ICM4X6XX_ACCEL);
            icm4x6xx_update_registry(this, instance, ICM4X6XX_GYRO);
            icm4x6xx_update_sensor_state(this, instance);
            inst_state->update_fac_cal_in_registry = false;
        }
#endif
        ICM4X6XX_PRINTF(MED, this, "(0x%x)no present clients, rm instance(0x%x), hw_id=%d", state->sensor, instance, state->hardware_id);
        this->cb->remove_instance(instance);
        icm4x6xx_turn_rails_off(this);
    }

    ICM4X6XX_VERB_PRINTF(MED, this, "set client req exit");

    return instance;
}

sns_sensor_api icm4x6xx_accel_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &icm4x6xx_accel_init,
  .deinit             = &icm4x6xx_accel_deinit,
  .get_sensor_uid     = &icm4x6xx_get_sensor_uid,
  .set_client_request = &icm4x6xx_set_client_request,
  .notify_event       = &icm4x6xx_sensor_notify_event,
};

sns_sensor_api icm4x6xx_gyro_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &icm4x6xx_gyro_init,
  .deinit             = &icm4x6xx_gyro_deinit,
  .get_sensor_uid     = &icm4x6xx_get_sensor_uid,
  .set_client_request = &icm4x6xx_set_client_request,
  .notify_event       = &icm4x6xx_sensor_notify_event,
};

sns_sensor_api icm4x6xx_motion_detect_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &icm4x6xx_motion_detect_init,
  .deinit             = &icm4x6xx_motion_detect_deinit,
  .get_sensor_uid     = &icm4x6xx_get_sensor_uid,
  .set_client_request = &icm4x6xx_set_client_request,
  .notify_event       = &icm4x6xx_sensor_notify_event,
};

#ifdef ICM4X6XX_ENABLE_LOWG
sns_sensor_api icm4x6xx_free_fall_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &icm4x6xx_free_fall_init,
  .deinit             = &icm4x6xx_free_fall_deinit,
  .get_sensor_uid     = &icm4x6xx_get_sensor_uid,
  .set_client_request = &icm4x6xx_set_client_request,
  .notify_event       = &icm4x6xx_sensor_notify_event,
};
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
sns_sensor_api icm4x6xx_high_shock_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &icm4x6xx_high_shock_init,
  .deinit             = &icm4x6xx_high_shock_deinit,
  .get_sensor_uid     = &icm4x6xx_get_sensor_uid,
  .set_client_request = &icm4x6xx_set_client_request,
  .notify_event       = &icm4x6xx_sensor_notify_event,
};
#endif

sns_sensor_api icm4x6xx_sensor_temp_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &icm4x6xx_sensor_temp_init,
  .deinit             = &icm4x6xx_sensor_temp_deinit,
  .get_sensor_uid     = &icm4x6xx_get_sensor_uid,
  .set_client_request = &icm4x6xx_set_client_request,
  .notify_event       = &icm4x6xx_sensor_notify_event,
};

