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
#include "sns_mem_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_icm4x6xx_hal.h"
#include "sns_icm4x6xx_sensor_instance.h"
#include "sns_icm4x6xx_sensor.h"

#include "sns_dae.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_diag_service.h"

#ifndef ICM4X6XX_DISABLE_DAE
/*======================================================================================
  Helper Functions
  ======================================================================================*/
static void build_accel_static_config_request(
  icm4x6xx_state             *sensor_state,
  sns_dae_set_static_config  *config_req)
{
    icm4x6xx_dae_if_info* dae_if = &sensor_state->common.dae_if;

    if (YOKOHAMA == sensor_state->common.product_info.series) {
      dae_if->ag.nano_hal_vtable_name =
        (sensor_state->hardware_id == 0) ? "icm426xx_hal_table" : "icm426xx_hal_table2";
    } else if (HAVANA == sensor_state->common.product_info.series) {
      dae_if->ag.nano_hal_vtable_name =
        (sensor_state->hardware_id == 0) ? "icm406xx_hal_table" : "icm406xx_hal_table2";
    } else {
      dae_if->ag.nano_hal_vtable_name = "icm4x6xx_unknow";
    }

    sns_strlcpy(config_req->func_table_name,
              dae_if->ag.nano_hal_vtable_name,
              sizeof(config_req->func_table_name));

    config_req->interrupt              = SNS_DAE_INT_OP_MODE_IRQ;
    config_req->has_irq_config         = true;
    config_req->irq_config             = sensor_state->common.irq_config;
    config_req->has_s4s_config         = false;
    if (sensor_state->common.product_info.i3c_use_int1) {
      config_req->has_ibi_config     = false;
    } else {
      // TODO: Add IBI config here
    }
    sns_com_port_config* com_config = &sensor_state->common.com_port_info.com_config;
    config_req->ascp_config.bus_type          = (sns_async_com_port_bus_type)(com_config->bus_type);
    config_req->ascp_config.slave_control     = com_config->slave_control;
    config_req->ascp_config.reg_addr_type     = SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT;
    config_req->ascp_config.min_bus_speed_kHz = com_config->min_bus_speed_KHz;
    config_req->ascp_config.max_bus_speed_kHz = com_config->max_bus_speed_KHz;
    config_req->ascp_config.bus_instance      = com_config->bus_instance;

    config_req->has_accel_info         = true;
    config_req->accel_info.accel_range = ACC_RANGE_UNIT_G((ACC_RANGE_MAX - sensor_state->common.a_res_idx), sensor_state->common.product_info.hi_range);
    config_req->accel_info.axis_map_count = ARR_SIZE(config_req->accel_info.axis_map);
    for(uint32_t i = 0; i < config_req->accel_info.axis_map_count; i++) {
        config_req->accel_info.axis_map[i].ipaxis = sensor_state->common.axis_map[i].ipaxis;
        config_req->accel_info.axis_map[i].opaxis = sensor_state->common.axis_map[i].opaxis;
        config_req->accel_info.axis_map[i].invert = sensor_state->common.axis_map[i].invert;
    }
    //  TODO: Populate Additional Attributes
    // config_req->accel_info.accel_attr[0].value.has_sint = true;
    // config_req->accel_info.accel_attr[0].value.sint = rigid_body_type;
    // config_req->accel_info.accel_attr[0].attr_id = SNS_STD_SENSOR_ATTRID_RIGID_BODY;
    // config_req->accel_info.accel_attr_count = 1;
}

static void build_temp_static_config_request(
  icm4x6xx_state             *sensor_state,
  sns_dae_set_static_config *config_req)
{
    icm4x6xx_dae_if_info* dae_if = &sensor_state->common.dae_if;

    if (YOKOHAMA == sensor_state->common.product_info.series) {
        dae_if->temp.nano_hal_vtable_name =
            (sensor_state->hardware_id == 0) ? "icm426xx_temperature_hal_table" : "icm426xx_temperature_hal_table2";
    } else if (HAVANA == sensor_state->common.product_info.series) {
        dae_if->temp.nano_hal_vtable_name =
            (sensor_state->hardware_id == 0) ? "icm406xx_temperature_hal_table" : "icm406xx_temperature_hal_table2";
    } else {
        dae_if->temp.nano_hal_vtable_name = "icm406xx_temperature_unknow";
    }

    sns_strlcpy(config_req->func_table_name,
              dae_if->temp.nano_hal_vtable_name,
              sizeof(config_req->func_table_name));
    config_req->interrupt          = false;
    config_req->has_irq_config     = false;
    config_req->has_accel_info     = false;
    config_req->has_ibi_config     = false;
    sns_com_port_config* com_config = &sensor_state->common.com_port_info.com_config;
    config_req->ascp_config.bus_type          = (sns_async_com_port_bus_type)(com_config->bus_type);
    config_req->ascp_config.slave_control     = com_config->slave_control;
    config_req->ascp_config.reg_addr_type     = SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT;
    config_req->ascp_config.min_bus_speed_kHz = com_config->min_bus_speed_KHz;
    config_req->ascp_config.max_bus_speed_kHz = com_config->max_bus_speed_KHz;
    config_req->ascp_config.bus_instance      = com_config->bus_instance;
}

static sns_rc send_static_config_request(
  sns_data_stream           *stream,
  sns_dae_set_static_config *config_req)
{
  sns_rc rc = SNS_RC_FAILED;
  uint8_t encoded_msg[sns_dae_set_static_config_size];
  sns_request req = {
    .message_id  = SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG,
    .request     = encoded_msg,
    .request_len = 0
  };

  req.request_len = pb_encode_request(encoded_msg, sizeof(encoded_msg), config_req,
                                      sns_dae_set_static_config_fields, NULL);
  if(0 < req.request_len)
  {
    rc = stream->api->send_request(stream, &req);
  }
  return rc;
}

static void icm4x6xx_build_send_ag_static_config(sns_sensor *this)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    sns_service_manager *svc_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_svc = (sns_stream_service*)
                  svc_mgr->get_service(svc_mgr, SNS_STREAM_SERVICE);

    if(NULL == state->common.dae_if.ag.stream) {
      sns_sensor_uid dae_suid;
      if(sns_suid_lookup_get(&state->common.suid_lookup_data, "data_acquisition_engine", &dae_suid)) {
        stream_svc->api->create_sensor_stream(stream_svc, this, dae_suid, &state->common.dae_if.ag.stream);
      }
    }

    if(NULL != state->common.dae_if.ag.stream) {
      sns_dae_set_static_config config_req = sns_dae_set_static_config_init_default;
      if(state->common.dae_if.ag.state == PRE_INIT) {
        state->common.dae_if.ag.state = INIT_PENDING;
        build_accel_static_config_request(state, &config_req);
      }

      if(SNS_RC_SUCCESS != send_static_config_request(state->common.dae_if.ag.stream, &config_req)) {
        ICM4X6XX_PRINTF(LOW, this, "check_support:: static config fail");
        state->common.dae_if.ag.state = UNAVAILABLE;
        sns_sensor_util_remove_sensor_stream(this, &state->common.dae_if.ag.stream);
      }
    }
}

static void icm4x6xx_build_send_temp_static_config(sns_sensor *this)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    sns_service_manager *svc_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_svc = (sns_stream_service*)
                  svc_mgr->get_service(svc_mgr, SNS_STREAM_SERVICE);

    if(NULL == state->common.dae_if.temp.stream) {
      sns_sensor_uid dae_suid;
      if(sns_suid_lookup_get(&state->common.suid_lookup_data, "data_acquisition_engine", &dae_suid)) {
        stream_svc->api->create_sensor_stream(stream_svc, this, dae_suid, &state->common.dae_if.temp.stream);
      }
    }

    if(NULL != state->common.dae_if.temp.stream) {
      sns_dae_set_static_config config_req = sns_dae_set_static_config_init_default;
      if(state->common.dae_if.temp.state == PRE_INIT) {
        state->common.dae_if.temp.state = INIT_PENDING;
        build_temp_static_config_request(state, &config_req);
      }

      if(SNS_RC_SUCCESS != send_static_config_request(state->common.dae_if.temp.stream, &config_req)) {
        ICM4X6XX_PRINTF(LOW, this, "check_support:: static config fail");
        state->common.dae_if.temp.state = UNAVAILABLE;
        sns_sensor_util_remove_sensor_stream(this, &state->common.dae_if.temp.stream);
      }
    }
}

static bool stream_usable(icm4x6xx_dae_stream *dae_stream)
{
  return (NULL != dae_stream->stream && dae_stream->stream_usable);
}

/* ------------------------------------------------------------------------------------ */
static bool icm4x6xx_send_ag_config(
            sns_sensor_instance *this,
            icm4x6xx_dae_stream *dae_stream,
            icm4x6xx_instance_state *state)
{
  bool cmd_sent = false;
  uint8_t packet_size = 0;
  sns_accel_dynamic_info *accel_info;
  sns_dae_set_streaming_config config_req = sns_dae_set_streaming_config_init_default;
  uint8_t encoded_msg[sns_dae_set_streaming_config_size];
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG,
    .request      = encoded_msg
  };
  state->fifo_info.cur_max_wmk = state->fifo_info.max_requested_wmk;
  state->fifo_info.cur_max_flush_ticks = state->fifo_info.max_requested_flush_ticks;
  config_req.dae_watermark            = state->fifo_info.max_requested_wmk;
  config_req.has_data_age_limit_ticks = true;
  config_req.data_age_limit_ticks     = state->fifo_info.max_requested_flush_ticks;
  if (config_req.data_age_limit_ticks < (UINT64_MAX * 0.9)) {
    config_req.data_age_limit_ticks += config_req.data_age_limit_ticks / 10;
  }
  config_req.has_polling_config       = false;

  float acc_odr = state->accel_info.actual_publish ? state->accel_info.cur_odr : 0.0f;
  config_req.has_accel_info    = true;
  accel_info                   = &config_req.accel_info;
  accel_info->odr              = acc_odr;
  accel_info->num_initial_invalid_samples = state->accel_info.num_samples_to_discard;
  accel_info->offset_cal_count = 3;
  accel_info->offset_cal[0]    = state->accel_registry_cfg.fac_cal_bias[0];
  accel_info->offset_cal[1]    = state->accel_registry_cfg.fac_cal_bias[1];
  accel_info->offset_cal[2]    = state->accel_registry_cfg.fac_cal_bias[2];
  accel_info->scale_cal_count  = 3;
  accel_info->scale_cal[0]     = state->accel_registry_cfg.fac_cal_corr_mat.e00;
  accel_info->scale_cal[1]     = state->accel_registry_cfg.fac_cal_corr_mat.e11;
  accel_info->scale_cal[2]     = state->accel_registry_cfg.fac_cal_corr_mat.e22;

  icm4x6xx_get_packet_size(this, &packet_size);

#ifdef ICM4X6XX_1K_2_500HZ
  if (state->do_1K_2_500_dec && FLT_EQUAL(state->accel_info.cur_odr, ICM4X6XX_ODR_1000)) {
      packet_size *= 2;
  }
#endif

  config_req.has_expected_get_data_bytes = true;
  config_req.expected_get_data_bytes = state->fifo_info.cur_wmk * packet_size;

  ICM4X6XX_INST_PRINTF(HIGH, this, "dae_wm=%u, data_age_limit_ticks=0x%x%08x, wm=%u, expected %d B",
                     config_req.dae_watermark, (uint32_t)(config_req.data_age_limit_ticks>>32),(uint32_t)(config_req.data_age_limit_ticks &0xFFFFFFFF),
                     state->fifo_info.cur_wmk, config_req.expected_get_data_bytes);

  if((req.request_len =
      pb_encode_request(encoded_msg,
                        sizeof(encoded_msg),
                        &config_req,
                        sns_dae_set_streaming_config_fields,
                        NULL)) > 0)
  {
    if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
    {
      dae_stream->state = STREAM_STARTING;
      cmd_sent = true;
    }
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool send_temp_config(
  icm4x6xx_dae_stream       *dae_stream,
  sns_sensor_instance      *this)
{
  bool cmd_sent = false;
  sns_time time_now = sns_get_system_time();
  icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
  icm4x6xx_sensor_temp_info *temp_info = &state->sensor_temp_info;
  uint32_t dae_wm = (uint32_t)(temp_info->sampling_rate_hz / temp_info->report_rate_hz);
  sns_dae_set_streaming_config config_req = sns_dae_set_streaming_config_init_zero;
  uint8_t encoded_msg[sns_dae_set_streaming_config_size];
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG,
    .request      = encoded_msg
  };
  config_req.dae_watermark = dae_wm;
  config_req.has_data_age_limit_ticks = true;
  config_req.data_age_limit_ticks = state->sensor_temp_info.max_requested_flush_ticks;
  if (config_req.data_age_limit_ticks < (UINT64_MAX * 0.9)) {
    config_req.data_age_limit_ticks += config_req.data_age_limit_ticks / 10;
  }
  config_req.has_polling_config = true;
  config_req.polling_config.polling_interval_ticks = temp_info->sampling_intvl;
  config_req.polling_config.polling_offset =
    (time_now + temp_info->sampling_intvl) / temp_info->sampling_intvl *
    temp_info->sampling_intvl;

  config_req.has_expected_get_data_bytes = true;
  config_req.expected_get_data_bytes = 2;


  ICM4X6XX_INST_PRINTF(MED, this,
                 "send_temp_config:: SR=%u RR=%u WM=%u now=%x%08x",
                 (uint32_t)temp_info->sampling_rate_hz,
                 (uint32_t)temp_info->report_rate_hz, dae_wm,
                 (uint32_t)(time_now >> 32), (uint32_t)time_now);
  ICM4X6XX_INST_PRINTF(MED, this,
                 "send_temp_config:: wm=%u age=%u invtl=%u offset=%x%08x",
                 config_req.dae_watermark, (uint32_t)config_req.data_age_limit_ticks,
                 (uint32_t)config_req.polling_config.polling_interval_ticks,
                 (uint32_t)(config_req.polling_config.polling_offset >> 32),
                 (uint32_t)config_req.polling_config.polling_offset);

  if((req.request_len =
      pb_encode_request(encoded_msg,
                        sizeof(encoded_msg),
                        &config_req,
                        sns_dae_set_streaming_config_fields,
                        NULL)) > 0)
  {
    if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
    {
      dae_stream->state = STREAM_STARTING;
      cmd_sent = true;
      temp_info->cur_sampling_rate_hz = temp_info->sampling_rate_hz;
    }
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool icm4x6xx_dae_flush_hw(icm4x6xx_dae_stream *dae_stream)
{
  bool cmd_sent = false;
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_FLUSH_HW,
    .request      = NULL,
    .request_len  = 0
  };

  if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
  {
    cmd_sent = true;
    dae_stream->flushing_hw = true;
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static bool icm4x6xx_dae_flush_samples(icm4x6xx_dae_stream *dae_stream)
{
  bool cmd_sent = false;
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_FLUSH_DATA_EVENTS,
    .request      = NULL,
    .request_len  = 0
  };

  if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
  {
    cmd_sent = true;
    dae_stream->flushing_data = true;
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
/* Stop response to DAE interrupt */
static bool icm4x6xx_dae_stop_streaming(icm4x6xx_dae_stream *dae_stream)
{
  bool cmd_sent = false;
  sns_request req = {
    .message_id   = SNS_DAE_MSGID_SNS_DAE_PAUSE_SAMPLING,
    .request      = NULL,
    .request_len  = 0
  };

  if(SNS_RC_SUCCESS == dae_stream->stream->api->send_request(dae_stream->stream, &req))
  {
    cmd_sent = true;
    dae_stream->state = STREAM_STOPPING;
  }
  return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
static void process_fifo_samples(
  sns_sensor_instance *this,
  sns_time            timestamp,
  const uint8_t       *buf,
  size_t              buf_len)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    ICM4X6XX_INST_PRINTF(LOW, this, "dae sensor TS: %x%08x",
                     (uint32_t)(timestamp >> 32),
                        (uint32_t)timestamp);

    icm4x6xx_process_fifo_data_buffer(this, buf, buf_len, timestamp);

    if(state->fac_test_in_progress) {
        /** All self-tests need to be handled in normal mode. */
        state->island_service->api->sensor_instance_island_exit(state->island_service, this);
        icm4x6xx_process_fac_test(this);
    }
}

/* ------------------------------------------------------------------------------------ */
static void process_temp_samples(sns_sensor_instance *this,
                                 sns_time            timestamp,
                                 const uint8_t       *buf,
                                 size_t              buf_len)
{
    if(buf_len == 2) {
        int16_t temp_raw = (int16_t)(((int16_t)buf[0]) << 8 | buf[1]);
        icm4x6xx_convert_and_send_temp_sample(this, timestamp, temp_raw);
    } else {
        ICM4X6XX_INST_PRINTF(ERROR, this,  "Unexpected data len %u from DAE sensor", buf_len);
    }
}


/* ------------------------------------------------------------------------------------ */
static void process_data_event(sns_sensor_instance *this,
                               icm4x6xx_dae_stream  *dae_stream,
                               pb_istream_t        *pbstream)
{
  pb_buffer_arg decode_arg;
  sns_dae_data_event data_event = sns_dae_data_event_init_default;
  data_event.sensor_data.funcs.decode = &pb_decode_string_cb;
  data_event.sensor_data.arg = &decode_arg;
  if(pb_decode(pbstream, sns_dae_data_event_fields, &data_event))
  {
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    icm4x6xx_dae_if_info* dae_if = &state->dae_if;
    if (dae_stream == &dae_if->ag) {
#if 0
        if (!state->flushing_fifo) {
          state->is_flush = false;
          state->interrupt_timestamp = data_event.timestamp;
        } else {
          state->is_flush = true;
          state->flushing_fifo = false;
        }
#endif
      state->dae_ts_type = data_event.timestamp_type;
      process_fifo_samples(
        this, data_event.timestamp, (uint8_t*)decode_arg.buf, decode_arg.buf_len);
    }
    else
    {
      process_temp_samples(
        this, data_event.timestamp, (uint8_t*)decode_arg.buf, decode_arg.buf_len);
    }
  }
}

/* ------------------------------------------------------------------------------------ */
static void process_interrupt_event(
  sns_sensor_instance *this,
  icm4x6xx_dae_stream *dae_stream,
  pb_istream_t        *pbstream)
{
  icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

  ICM4X6XX_INST_PRINTF(HIGH, this, "process_interrupt_event::stream=%x", dae_stream->stream);

  if(dae_stream == &state->dae_if.ag)
  {
    sns_dae_interrupt_event interrupt_event = sns_dae_interrupt_event_init_default;
    if(pb_decode(pbstream, sns_dae_interrupt_event_fields, &interrupt_event))
    {
      if (interrupt_event.registers.bytes[0] & (BIT_INT_STATUS_WOM_Z | BIT_INT_STATUS_WOM_Y | BIT_INT_STATUS_WOM_X)) {
        icm4x6xx_handle_md_interrupt(this, interrupt_event.timestamp);
      }

#ifdef ICM4X6XX_ENABLE_LOWG
      if (interrupt_event.registers.bytes[1] & BIT_INT_STATUS3_LOW_G_DET) {
        icm4x6xx_handle_freefall_interrupt(this, interrupt_event.timestamp);
      }
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
      if (interrupt_event.registers.bytes[1] & BIT_INT_STATUS3_HIGH_G_DET) {
        icm4x6xx_handle_high_shock_interrupt(this, interrupt_event.timestamp);
      }
#endif
    }
    else
    {
      ICM4X6XX_INST_PRINTF(ERROR, this, "Failed decoding interrupt event");
    }
  }
  else
  {
    ICM4X6XX_INST_PRINTF(ERROR, this, "unknown interrupt");
  }
}

/* ------------------------------------------------------------------------------------ */
static void icm4x6xx_dae_process_response(
            sns_sensor_instance *this,
            icm4x6xx_dae_stream *dae_stream,
            pb_istream_t        *pbstream)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    sns_dae_resp resp = sns_dae_resp_init_default;

    if(pb_decode(pbstream, sns_dae_resp_fields, &resp))
    {
        ICM4X6XX_INST_PRINTF(MED, this,
               "proc_resp:: msg=%u err=%u cfg_step=%u, state=%u, usable=%u",
               resp.msg_id, resp.err, state->config_step, dae_stream->state,
               dae_stream->stream_usable);
        switch(resp.msg_id)
        {
        case SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG:
            state->irq_info.irq_ready = true;
            dae_stream->stream_usable = true;
            //dae_stream->state = IDLE;
            if(SNS_STD_ERROR_NO_ERROR != resp.err)
            {
                /* DAE sensor does not have support for this driver */
                dae_stream->stream_usable = false;
                state->irq_info.irq_ready = false;
            }

            if (state->config_step == ICM4X6XX_CONFIG_UPDATING_HW &&
                dae_stream->stream_usable &&
                dae_stream == &state->dae_if.ag) {
                icm4x6xx_reconfig_hw(this);
            }
            break;
        case SNS_DAE_MSGID_SNS_DAE_S4S_DYNAMIC_CONFIG:
            break;
        case SNS_DAE_MSGID_SNS_DAE_SET_STREAMING_CONFIG:
            if(dae_stream->stream != NULL && dae_stream->state == STREAM_STARTING)
            {
                dae_stream->state = (SNS_STD_ERROR_NO_ERROR == resp.err) ? STREAMING : IDLE;
            }
            if(dae_stream->state == STREAMING && (dae_stream == &state->dae_if.ag))
            {
                #if 0
                if ((state->accel_info.desire_publish ||
                    state->gyro_info.desire_publish ||
                    state->fac_test_in_progress) &&
                    state->irq_info.irq_ready) {
                    icm4x6xx_en_wm_int(this, true);
                }
                #endif
                icm4x6xx_en_gyro(this, state->gyro_info.desire_en);
                icm4x6xx_set_accel_mode(this, state->accel_info.desire_mode);
                icm4x6xx_send_config_event(this);
            }
            break;
        case SNS_DAE_MSGID_SNS_DAE_FLUSH_HW:
            dae_stream->flushing_hw = false;
            if (dae_stream == &state->dae_if.ag) {
                icm4x6xx_dae_if_flush_samples(this);
                if(ICM4X6XX_CONFIG_FLUSHING_HW == state->config_step) {
                    //icm4x6xx_set_hw_wo_stop_fifo(this);
                    if (icm4x6xx_need_to_reset_fifo(state))
                        icm4x6xx_set_fifo_mode(this, BYPASS);
                    //icm4x6xx_reconfig_hw(this);
                } else {
                    if ((state->accel_info.desire_publish ||
                        state->gyro_info.desire_publish ||
                        state->fac_test_in_progress) &&
                        state->irq_info.irq_ready)
                        icm4x6xx_en_wm_int(this, true);
                }
            }
            break;
    case SNS_DAE_MSGID_SNS_DAE_FLUSH_DATA_EVENTS:
        dae_stream->flushing_data = false;
        if (state->fifo_flush_in_progress && (dae_stream == &state->dae_if.ag)) {
            state->fifo_flush_in_progress = false;
            if (state->accel_info.desire_publish &&
                state->accel_info.actual_publish) {
                icm4x6xx_send_fifo_flush_done(this, &state->accel_info.suid);
            }

            if (state->gyro_info.desire_publish &&
                state->gyro_info.actual_publish) {
                icm4x6xx_send_fifo_flush_done(this, &state->gyro_info.suid);
            }
        }

        if(ICM4X6XX_CONFIG_FLUSHING_HW == state->config_step) {
            icm4x6xx_reconfig_hw(this);
        }
      break;
    case SNS_DAE_MSGID_SNS_DAE_PAUSE_SAMPLING:
      if (dae_stream->state == STREAM_STOPPING)
        dae_stream->state = (SNS_STD_ERROR_NO_ERROR != resp.err) ? STREAMING : IDLE;

      if (SNS_STD_ERROR_NO_ERROR != resp.err)
        ICM4X6XX_INST_PRINTF(HIGH, this, "stop streaming failed");

      if (NULL != state->dae_if.temp.stream &&
          state->dae_if.temp.state != STREAM_STOPPING &&
          dae_stream == &state->dae_if.temp &&
          0 < state->sensor_temp_info.sampling_intvl) {
          //icm4x6xx_dae_flush_samples(&state->dae_if.temp);
          icm4x6xx_dae_if_start_temp_streaming(this);
      }

      break;
    case SNS_DAE_MSGID_SNS_DAE_PAUSE_S4S_SCHED:
      break;

    case SNS_DAE_MSGID_SNS_DAE_RESP:
    case SNS_DAE_MSGID_SNS_DAE_DATA_EVENT:
      break; /* unexpected */
    }
  }
}

/* ------------------------------------------------------------------------------------ */
static void icm4x6xx_process_events(sns_sensor_instance *this, icm4x6xx_dae_stream *dae_stream)
{
    sns_sensor_event *event;

    while(NULL != dae_stream->stream &&
        NULL != (event = dae_stream->stream->api->peek_input(dae_stream->stream)))
    {
        if (dae_stream->stream_usable)
        {
            pb_istream_t pbstream =
                        pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

            ICM4X6XX_VERB_INST_PRINTF(MED, this,  "process_events message_id %d", event->message_id);

            if (SNS_DAE_MSGID_SNS_DAE_DATA_EVENT == event->message_id)
            {
                process_data_event(this, dae_stream, &pbstream);
            }
            else if(SNS_DAE_MSGID_SNS_DAE_INTERRUPT_EVENT == event->message_id)
            {
                process_interrupt_event(this, dae_stream, &pbstream);
            }
            else if(SNS_DAE_MSGID_SNS_DAE_RESP == event->message_id)
            {
                icm4x6xx_dae_process_response(this, dae_stream, &pbstream);
            }
            else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == event->message_id)
            {
                ICM4X6XX_INST_PRINTF(MED, this,
                               "process_events:: ERROR_EVENT stream=%x state=%u",
                               dae_stream->stream, dae_stream->state);
                dae_stream->stream_usable = false;
                icm4x6xx_register_interrupt(this);
            }
            else
            {
                ICM4X6XX_INST_PRINTF(ERROR, this,  "Unexpected message id %u", event->message_id);
                /* TODO - report unexpected message */
            }
        }
        event = dae_stream->stream->api->get_next_input(dae_stream->stream);
    }
}

/*======================================================================================
  Public Functions
  ======================================================================================*/
bool icm4x6xx_dae_if_available(sns_sensor_instance *this)
{
    /* both streams must be availble */
    icm4x6xx_dae_if_info *dae_if = &((icm4x6xx_instance_state*)this->state->state)->dae_if;
  return (stream_usable(&dae_if->ag) && stream_usable(&dae_if->temp));
}

bool icm4x6xx_dae_if_is_ag_streaming(sns_sensor_instance *this)
{
  icm4x6xx_dae_if_info *dae_if = &((icm4x6xx_instance_state*)this->state->state)->dae_if;
  return (stream_usable(&dae_if->ag) &&
          (dae_if->ag.state == STREAMING || dae_if->ag.state == STREAM_STARTING));
}

bool icm4x6xx_dae_if_is_temp_streaming(sns_sensor_instance *this)
{
  icm4x6xx_dae_if_info *dae_if = &((icm4x6xx_instance_state*)this->state->state)->dae_if;
  return (stream_usable(&dae_if->temp) &&
          (dae_if->temp.state == STREAMING || dae_if->temp.state == STREAM_STARTING));
}

bool icm4x6xx_dae_if_is_on_start_streaming(icm4x6xx_dae_stream *dae_stream)
{
  return (NULL != dae_stream->stream && dae_stream->stream_usable &&
            dae_stream->state == STREAM_STARTING);
}

void icm4x6xx_dae_if_check_support(sns_sensor *this)
{
    icm4x6xx_build_send_ag_static_config(this);
    icm4x6xx_build_send_temp_static_config(this);
}

static void icm4x6xx_update_dae_state_sibling_sensors(sns_sensor *const this)
{
    sns_sensor *sensor = NULL;
    icm4x6xx_state *state;
    icm4x6xx_state *acc_state = (icm4x6xx_state*)this->state->state;

    for (sensor = this->cb->get_library_sensor(this, true);
         sensor != NULL;
         sensor = this->cb->get_library_sensor(this, false)) {
        state = (icm4x6xx_state*)sensor->state->state;

        if(state->sensor != ICM4X6XX_ACCEL) {
            sns_memscpy(&state->common, sizeof(state->common),
                      &acc_state->common, sizeof(acc_state->common));
        }
    }
}

static void icm4x6xx_dae_if_process_stream_events(
    sns_sensor *this,
    icm4x6xx_dae_if_state  *state,
    struct sns_data_stream **data_stream)
{
  sns_sensor_event *event;
  struct sns_data_stream *stream = *data_stream;

  if (NULL == stream || 0 == stream->api->get_input_cnt(stream)) {
    return;
  }

  while (NULL != (event = stream->api->peek_input(stream)))
  {
    pb_istream_t pbstream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

    if(SNS_DAE_MSGID_SNS_DAE_RESP == event->message_id)
    {
      sns_dae_resp resp = sns_dae_resp_init_default;
      if(pb_decode(&pbstream, sns_dae_resp_fields, &resp))
      {
        if(SNS_DAE_MSGID_SNS_DAE_SET_STATIC_CONFIG == resp.msg_id)
        {
          if(*state == INIT_PENDING)
          {
            *state = (SNS_STD_ERROR_NO_ERROR != resp.err) ? UNAVAILABLE : IDLE;
            if(UNAVAILABLE == *state)
            {
              ICM4X6XX_PRINTF(HIGH, this, "process_sensor_events:: dae unavailable");
            }
          }
        }
      }
    }
    else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == event->message_id)
    {
      *state = UNAVAILABLE;
    }

    event = stream->api->get_next_input(stream);
  }

  if(UNAVAILABLE == *state || IDLE == *state)
  {
    sns_sensor_util_remove_sensor_stream(this, data_stream);
    icm4x6xx_update_dae_state_sibling_sensors(this);
  }
}


void icm4x6xx_dae_if_process_sensor_events(sns_sensor *this)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;

  icm4x6xx_dae_if_process_stream_events(this, &(state->common.dae_if.ag.state), &(state->common.dae_if.ag.stream));
  icm4x6xx_dae_if_process_stream_events(this, &(state->common.dae_if.temp.state), &(state->common.dae_if.temp.stream));
}

/* ------------------------------------------------------------------------------------ */
sns_rc icm4x6xx_dae_if_init(
    sns_sensor_instance     *const this,
    sns_stream_service      *stream_mgr,
    sns_sensor_uid          *dae_suid,
    icm4x6xx_state          *sensor_state)
{
    sns_rc rc = SNS_RC_NOT_AVAILABLE;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    icm4x6xx_dae_if_info* dae_if = &state->dae_if;

    dae_if->ag.state = sensor_state->common.dae_if.ag.state;
    dae_if->temp.state = sensor_state->common.dae_if.temp.state;

    if(IDLE == dae_if->ag.state) {
        stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                     this,
                                                     *dae_suid,
                                                     &dae_if->ag.stream);
        if(NULL != dae_if->ag.stream) {
          sns_dae_set_static_config config_req = sns_dae_set_static_config_init_default;
          build_accel_static_config_request(sensor_state, &config_req);
          rc = send_static_config_request(dae_if->ag.stream, &config_req);
        }
        else
        {
          ICM4X6XX_INST_PRINTF(ERROR, this, "NULL = dae_if->ag.stream");
        }
    }

    if(IDLE == dae_if->temp.state) {
        stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                     this,
                                                     *dae_suid,
                                                     &dae_if->temp.stream);
        if(NULL != dae_if->temp.stream) {
          sns_dae_set_static_config config_req = sns_dae_set_static_config_init_default;
          build_temp_static_config_request(sensor_state, &config_req);
          rc += send_static_config_request(dae_if->temp.stream, &config_req);
        }
    }

    if(SNS_RC_SUCCESS != rc)
    {
        icm4x6xx_dae_if_deinit(this);
    }
    else
    {
      dae_if->ag.stream_usable   = true;
      dae_if->temp.stream_usable = true;
    }
    ICM4X6XX_INST_PRINTF(HIGH, this,
                "dae_if_init: rc=%d, state(ag/t)=%u/%u, usable(ag/t)=%u/%u, ag flush_hw=%d",
                rc, dae_if->ag.state, dae_if->temp.state,
                dae_if->ag.stream_usable, dae_if->temp.stream_usable,
                dae_if->ag.flushing_hw);
    return rc;
}

/* ------------------------------------------------------------------------------------ */
void icm4x6xx_dae_if_deinit(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    sns_sensor_util_remove_sensor_instance_stream(this, &state->dae_if.ag.stream);
    sns_sensor_util_remove_sensor_instance_stream(this, &state->dae_if.temp.stream);
    state->dae_if.ag.flushing_hw = false;
    state->dae_if.ag.flushing_data = false;
    state->dae_if.ag.state = PRE_INIT;
    state->dae_if.temp.flushing_hw = false;
    state->dae_if.temp.flushing_data = false;
    state->dae_if.temp.state = PRE_INIT;

    ICM4X6XX_INST_PRINTF(HIGH, this, "dae_if_deinit");
}

/* ------------------------------------------------------------------------------------ */
bool icm4x6xx_dae_if_stop_temp_streaming(sns_sensor_instance *this)
{
    bool cmd_sent = false;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    icm4x6xx_dae_if_info    *dae_if = &state->dae_if;

    if (stream_usable(&state->dae_if.temp) &&
        (dae_if->temp.state == STREAMING || dae_if->temp.state == STREAM_STARTING)){
        ICM4X6XX_INST_PRINTF(MED, this,
                       "stop_streaming:: Temp stream=%x", &dae_if->temp.stream);
        cmd_sent |= icm4x6xx_dae_stop_streaming(&dae_if->temp);
        state->sensor_temp_info.cur_sampling_rate_hz = 0.0f;
    }
    return cmd_sent;
}

bool icm4x6xx_dae_if_start_temp_streaming(sns_sensor_instance *this)
{
    bool cmd_sent = false;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    icm4x6xx_dae_if_info    *dae_if = &state->dae_if;

    if (stream_usable(&dae_if->temp) &&
        dae_if->temp.state != STREAMING &&
        dae_if->temp.state != STREAM_STOPPING &&
        0 < state->sensor_temp_info.sampling_intvl) {
      ICM4X6XX_INST_PRINTF(MED, this,
                     "start_streaming:: Temp stream=%x", &dae_if->temp.stream);
      cmd_sent |= send_temp_config(&dae_if->temp, this);
    }
    return cmd_sent;
}


/* ------------------------------------------------------------------------------------ */
bool icm4x6xx_dae_if_stop_streaming(sns_sensor_instance *this)
{
    bool cmd_sent = false;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    icm4x6xx_dae_if_info    *dae_if = &state->dae_if;

  if(stream_usable(&state->dae_if.ag) &&
    (dae_if->ag.state == STREAMING || dae_if->ag.state == STREAM_STARTING))
    {
        ICM4X6XX_INST_PRINTF(MED, this,
                       "stop_streaming:: AG stream=%x", &dae_if->ag.stream);
        cmd_sent |= icm4x6xx_dae_stop_streaming(&dae_if->ag);
    }

    cmd_sent |= icm4x6xx_dae_if_stop_temp_streaming(this);

    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
/* Start response to DAE interrupt */
bool icm4x6xx_dae_if_start_streaming(sns_sensor_instance *this)
{
    bool cmd_sent = false;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    icm4x6xx_dae_if_info    *dae_if = &state->dae_if;

    ICM4X6XX_INST_PRINTF(HIGH, this,
               "dae state =%d", dae_if->ag.state);

    if (stream_usable(&dae_if->ag) && dae_if->ag.state >= IDLE &&
        (state->accel_info.desire_en || state->gyro_info.desire_en))
    {
        ICM4X6XX_INST_PRINTF(MED, this,
                   "start_streaming:: AG stream=%x", &dae_if->ag.stream);
        cmd_sent |= icm4x6xx_send_ag_config(this, &dae_if->ag, state);
    }

    cmd_sent |= icm4x6xx_dae_if_start_temp_streaming(this);

    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool icm4x6xx_dae_if_flush_hw(sns_sensor_instance *this)
{
    icm4x6xx_dae_if_info *dae_if = &((icm4x6xx_instance_state*)this->state->state)->dae_if;
    bool cmd_sent = dae_if->ag.flushing_hw;

    icm4x6xx_en_wm_int(this, false);

    if(stream_usable(&dae_if->ag) && !dae_if->ag.flushing_hw && dae_if->ag.state >= IDLE)
    {
        cmd_sent |= icm4x6xx_dae_flush_hw(&dae_if->ag);
        ICM4X6XX_INST_PRINTF(MED, this,
                       "flush_hw:: AG stream=%x flushing=%u",
                       &dae_if->ag.stream, dae_if->ag.flushing_hw);
    }
    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
bool icm4x6xx_dae_if_flush_samples(sns_sensor_instance *this)
{
    icm4x6xx_dae_if_info *dae_if = &((icm4x6xx_instance_state*)this->state->state)->dae_if;
    bool cmd_sent = dae_if->ag.flushing_data | dae_if->temp.flushing_data;

    if(stream_usable(&dae_if->ag) && dae_if->ag.state >= IDLE)
    {
        if(!dae_if->ag.flushing_data)
        {
            ICM4X6XX_INST_PRINTF(MED, this, "AG stream=%x", &dae_if->ag.stream);
            icm4x6xx_dae_flush_samples(&dae_if->ag);
        }
        cmd_sent |= dae_if->ag.flushing_data;
        ICM4X6XX_INST_PRINTF(MED, this,
                       "flush_samples:: AG stream=%x flushing=%u",
                       &dae_if->ag.stream, dae_if->ag.flushing_data);
    }

    return cmd_sent;
}

/* ------------------------------------------------------------------------------------ */
void icm4x6xx_dae_if_process_events(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    icm4x6xx_process_events(this, &state->dae_if.ag);
    icm4x6xx_process_events(this, &state->dae_if.temp);

    if(!stream_usable(&state->dae_if.ag) || !stream_usable(&state->dae_if.temp))
    {
        icm4x6xx_dae_if_deinit(this);
    }
}
#endif
