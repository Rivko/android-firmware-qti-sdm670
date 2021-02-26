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
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "sns_icm4x6xx_hal.h"
#include "sns_icm4x6xx_sensor.h"
#include "sns_icm4x6xx_sensor_instance.h"

#include "sns_async_com_port.pb.h"
#include "sns_interrupt.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_sync_com_port_service.h"

static void icm4x6xx_inst_cleanup(sns_sensor_instance *const this)
{
  icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
  uint8_t int_status;

  // TODO: Is this OK to do here? Introduced by DAE sensor, since we could be
  // waiting for response from DAE before writing to HW -- but we don't have that chance.
  if(NULL != state->com_port_info.port_handle) {
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
  }

  /* Normally, the device should be power down, it is unnecessary to do this.
   * But it's better to do reset here, for example:
   * some other sensor uses same power with A/G, so if that sensor is on,
   * even A/G is off, the device still has power */
  //icm4x6xx_reset_device(this);

  icm4x6xx_en_wom_int(this, false);
  icm4x6xx_en_wm_int(this, false);
  icm4x6xx_en_fifo_full_int(this, false);
  icm4x6xx_en_dri(this, false);

  icm4x6xx_set_accel_mode(this, ICM4X6XX_A_OFF);
  icm4x6xx_en_gyro(this, false);
  icm4x6xx_read_int_status(this, &int_status);
  icm4x6xx_read_int_status2(this, &int_status);
  icm4x6xx_send_int_clear_msg(this);

  if(NULL != state->com_port_info.port_handle) {
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
  }

  sns_sensor_util_remove_sensor_instance_stream(this, &state->interrupt_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->async_com_port_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->heart_beat.timer_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_en_md_stream);

  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->scp_service = NULL;
  }

#ifndef ICM4X6XX_DISABLE_DAE
  icm4x6xx_dae_if_deinit(this);
#endif
}

/** See sns_sensor_instance_api::init */
sns_rc icm4x6xx_inst_init(sns_sensor_instance *const this,
                        sns_sensor_state const *sstate)
{
    icm4x6xx_instance_state *state =
              (icm4x6xx_instance_state*)this->state->state;
    icm4x6xx_state *sensor_state =
              (icm4x6xx_state*)sstate->state;
    float data[3];
    float temp_data[1];
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    uint64_t buffer[10];
    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
    sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
    uint8_t arr_index = 0;
    float diag_temp[TRIAXIS_NUM];
    pb_float_arr_arg arg = {
            .arr = (float*)diag_temp,
            .arr_len = TRIAXIS_NUM,
            .arr_index = &arr_index
        };

    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg = &arg;
    sns_sensor_uid irq_suid, ascp_suid, dae_suid;

    #ifdef ICM4X6XX_ENABLE_DUAL_SENSOR
    //this would be stored when the state is initialized
    state->instance_idx = sensor_state->registration_idx;
    //TODO:
    //state->rigid_body_type = sensor_state->rigid_body_type;
    ICM4X6XX_INST_PRINTF(LOW, this, "icm4x6xx_inst_init enter hw_idx=%d, reg_idx=%d, instance_idx=%d",
            sensor_state->hardware_id, sensor_state->registration_idx, state->instance_idx);
    #else
    ICM4X6XX_INST_PRINTF(LOW, this, "icm4x6xx_inst_init enter");
    #endif

    state->scp_service = (sns_sync_com_port_service*)
              service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
    state->island_service = (sns_island_service*)
              service_mgr->get_service(service_mgr, SNS_ISLAND_SERVICE);

    /**---------Setup stream connections with dependent Sensors---------*/
    sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "interrupt", &irq_suid);
    sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "async_com_port", &ascp_suid);
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                   this,
                                                   irq_suid,
                                                   &state->interrupt_data_stream);

    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                   this,
                                                   ascp_suid,
                                                   &state->async_com_port_data_stream);
    /* Copy whoami */
    sns_memscpy(&state->product_info,
                sizeof(state->product_info),
                &sensor_state->common.product_info,
                sizeof(sensor_state->common.product_info));

    ICM4X6XX_INST_PRINTF(MED, this, "invn product %d, hi_res %d, hi_range %d",
                state->product_info.series,
                state->product_info.use_hi_res,
                state->product_info.hi_range);

    /** Initialize COM port to be used by the Instance */
    sns_memscpy(&state->com_port_info,
              sizeof(state->com_port_info),
              &sensor_state->common.com_port_info,
              sizeof(sensor_state->common.com_port_info));
    state->com_port_info.port_handle = NULL;

    state->accel_info.fsr = ACC_RANGE_MAX - sensor_state->common.a_res_idx;
    state->gyro_info.fsr = GYRO_RANGE_MAX - sensor_state->common.g_res_idx;

    ICM4X6XX_INST_PRINTF(MED, this, "a_res_idx, g_res_idx, a_fsr, g_fsr: %d %d %d %d",
                sensor_state->common.a_res_idx, sensor_state->common.g_res_idx,
                state->accel_info.fsr, state->gyro_info.fsr);

    state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                              &state->com_port_info.port_handle);

    if(NULL == state->interrupt_data_stream ||
       NULL == state->async_com_port_data_stream ||
       NULL == state->com_port_info.port_handle)
    {
        ICM4X6XX_INST_PRINTF(LOW, this, "icm4x6xx_inst_cleanup enter");
        icm4x6xx_inst_cleanup(this);
        return SNS_RC_FAILED;
    }

    /**----------- Copy all Sensor UIDs in instance state -------------*/
    sns_memscpy(&state->accel_info.suid,
              sizeof(state->accel_info.suid),
              &sensor_state->common.a_suid,
              sizeof(state->accel_info.suid));
    sns_memscpy(&state->gyro_info.suid,
              sizeof(state->gyro_info.suid),
              &sensor_state->common.g_suid,
              sizeof(state->gyro_info.suid));
    sns_memscpy(&state->md_info.suid,
              sizeof(state->md_info.suid),
              &sensor_state->common.md_suid,
              sizeof(state->md_info.suid));
    sns_memscpy(&state->sensor_temp_info.suid,
              sizeof(state->sensor_temp_info.suid),
              &sensor_state->common.st_suid,
              sizeof(state->sensor_temp_info.suid));

#ifdef ICM4X6XX_ENABLE_LOWG
    sns_memscpy(&state->freefall_info.suid,
            sizeof(state->freefall_info.suid),
            &sensor_state->common.free_fall_suid,
            sizeof(state->freefall_info.suid));
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
    sns_memscpy(&state->high_shock_info.suid,
            sizeof(state->high_shock_info.suid),
            &sensor_state->common.high_shock_suid,
            sizeof(state->high_shock_info.suid));
#endif

    sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "timer", &state->timer_suid);

    /**-------------------------Init FIFO State-------------------------*/
    state->fifo_info.record_mode = false;
    state->fifo_info.curr_format = ICM4X6XX_FORMAT_EMPTY;
    state->fifo_info.cnt_flush_hw = 0;
    state->fifo_info.cur_wmk = 0;
    state->fifo_info.desire_wmk = 0;
    state->fifo_info.max_requested_wmk = 0;

    state->is_first_sample = true;
    state->odr_just_changed = false;
    state->pre_publish_ts = 0;
    state->ascp_request_cnt = 0;
    state->before_re_hw_intnl_cnt = 0;
    state->after_re_hw_intnl_cnt = 0;
    state->odr_changed = false;
    state->a_off_to_on = false;
    state->g_off_to_on = false;
    state->is_flush = false;
    state->interrupt_timestamp = 0;
#ifdef  ICM4X6XX_1K_2_500HZ
    state->do_1K_2_500_dec = false;
#endif

    /**-------------------------Init Accel State-------------------------*/
    state->accel_info.desire_en = false;
    state->accel_info.is_enabled = false;
    state->accel_info.curr_mode = ICM4X6XX_A_OFF;
    state->accel_info.cur_odr = 0.0;
    state->accel_info.req_match_odr = 0.0;
    state->accel_info.bw = BW_ODR_DIV_2;
    state->accel_info.desire_publish = false;
    state->accel_info.actual_publish = false;
    state->accel_info.num_samples_to_discard = 0;

    state->md_info.ignore_md_fired_num = 0;

    /**-------------------------Init Gyro State-------------------------*/
    state->gyro_info.desire_en = false;
    state->gyro_info.is_enabled = false;
    state->gyro_info.cur_odr = 0.0;
    state->gyro_info.req_match_odr = 0.0;
    state->gyro_info.desire_publish = false;
    state->gyro_info.actual_publish = false;
    state->gyro_info.num_samples_to_discard = 0;

    /**-------------------------Init Heart Beat State-------------------------*/
    state->heart_beat.timer_data_stream = NULL;
    state->heart_beat.heart_attack_cnt = 0;

    state->md_info.pre_md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;

#ifdef ICM4X6XX_ENABLE_LOWG
    state->freefall_info.enable_free_fall = false;
    state->freefall_info.free_fall_state.free_fall_event_type = SNS_FREE_FALL_EVENT_TYPE_DISABLED;
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
    state->high_shock_info.enable_high_shock = false;
    state->high_shock_info.high_shock_state.high_shock_event_type = SNS_HIGH_SHOCK_EVENT_TYPE_DISABLED;
#endif

    state->a_new_stream = false;
    state->g_new_stream = false;
    state->fifo_flush_in_progress = false;

    state->sensor_temp_info.desire_publish = false;
    state->sensor_temp_info.actual_publish = false;
    state->sensor_temp_info.cur_sampling_rate_hz = 0.0f;
#ifdef  ICM4X6XX_TEMPERATURE_ONE_UNRELIABLE_SAMPLE
    state->sensor_temp_info.discard_samples = 1;
#endif

    state->timer_data_stream = NULL;
    state->timer_en_md_stream = NULL;

    state->a_reported_sample = 0;
    state->g_reported_sample = 0;

#ifndef ICM4X6XX_DISABLE_DAE
    state->flushing_fifo = false;
#endif

    state->encoded_imu_event_len = pb_get_encoded_size_sensor_stream_event(data, 3);
    state->encoded_sensor_temp_event_len = pb_get_encoded_size_sensor_stream_event(temp_data, 1);

    state->diag_service =  (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

    state->scp_service =  (sns_sync_com_port_service*)
      service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

    /** Initialize IRQ info to be used by the Instance */
    state->irq_info.irq_config = sensor_state->common.irq_config;
    state->irq_info.irq_ready = false;
    state->irq_info.irq_registered = false;
    ICM4X6XX_INST_PRINTF(HIGH, this, "irq_trigger_type=%d", state->irq_info.irq_config.interrupt_trigger_type);

    state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

    icm4x6xx_reset_device(this);
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
    #if 0 // this cause crash in SLPI 254
    // deregister and register again, for updating SPI speed
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                              &state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
    #endif

    /** Configure the Async Com Port */
    {
        sns_data_stream* data_stream = state->async_com_port_data_stream;
        sns_com_port_config* com_config = &state->com_port_info.com_config;
        uint8_t pb_encode_buffer[100];
        sns_request async_com_port_request =
            {
              .message_id  = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG,
              .request     = &pb_encode_buffer
            };

        state->ascp_config.bus_type          = (sns_async_com_port_bus_type)(com_config->bus_type);
        state->ascp_config.slave_control     = com_config->slave_control;
        state->ascp_config.reg_addr_type     = SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT;
        state->ascp_config.min_bus_speed_kHz = com_config->min_bus_speed_KHz;
        state->ascp_config.max_bus_speed_kHz = com_config->max_bus_speed_KHz;
        state->ascp_config.bus_instance      = com_config->bus_instance;

        ICM4X6XX_INST_PRINTF(MED, this, "bus type(%d, 0:I2C/1:SPI/3:I3C), MAX bus speed: %d KHz",
            state->ascp_config.bus_type, com_config->max_bus_speed_KHz);

        async_com_port_request.request_len =
                  pb_encode_request(pb_encode_buffer,
                                    sizeof(pb_encode_buffer),
                                    &state->ascp_config,
                                    sns_async_com_port_config_fields,
                                    NULL);
        data_stream->api->send_request(data_stream, &async_com_port_request);
    }

    /** Copy down axis conversion settings */
    sns_memscpy(state->axis_map,  sizeof(sensor_state->common.axis_map),
              sensor_state->common.axis_map, sizeof(sensor_state->common.axis_map));

    /** Initialize factory calibration */
    state->accel_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
    state->accel_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
    state->accel_registry_cfg.fac_cal_corr_mat.e22 = 1.0;
    state->gyro_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
    state->gyro_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
    state->gyro_registry_cfg.fac_cal_corr_mat.e22 = 1.0;
    state->sensor_temp_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
    state->sensor_temp_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
    state->sensor_temp_registry_cfg.fac_cal_corr_mat.e22 = 1.0;

    /** Copy down MD configuration */
    sns_memscpy(&state->md_info.md_config, sizeof(state->md_info.md_config),
              &sensor_state->md_config, sizeof(sensor_state->md_config));

    /** Determine sizes of encoded logs */
    sns_diag_sensor_state_interrupt sensor_state_interrupt =
                     sns_diag_sensor_state_interrupt_init_default;
    pb_get_encoded_size(&state->log_interrupt_encoded_size,
                      sns_diag_sensor_state_interrupt_fields,
                      &sensor_state_interrupt);

    /** Determine size of sns_diag_sensor_state_raw as defined in
     *  sns_diag.proto
     *  sns_diag_sensor_state_raw is a repeated array of samples of
     *  type sns_diag_batch sample. The following determines the
     *  size of sns_diag_sensor_state_raw with a single batch
     *  sample */
    if(pb_encode_tag(&stream, PB_WT_STRING,
                    sns_diag_sensor_state_raw_sample_tag))
    {
        if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                                   &batch_sample))
        {
            state->log_raw_encoded_size = stream.bytes_written;
        }
    }
    //get the temp sensor log encoded size
    arg.arr_len = 1;
    if(pb_encode_tag(&stream, PB_WT_STRING, sns_diag_sensor_state_raw_sample_tag))
    {
      if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields, &batch_sample))
      {
        state->log_temp_raw_encoded_size = stream.bytes_written;
      }
    }

    sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "data_acquisition_engine", &dae_suid);

#ifdef ICM4X6XX_DISABLE_DAE
    ICM4X6XX_INST_PRINTF(LOW, this, "invn dae disabled");
    icm4x6xx_register_interrupt(this);
#else
    icm4x6xx_dae_if_init(this, stream_mgr, &dae_suid, sensor_state);
#endif

    return SNS_RC_SUCCESS;
}

// QC: Removed sensor_state parameter
sns_rc icm4x6xx_inst_deinit(sns_sensor_instance *const this)
{
  ICM4X6XX_INST_PRINTF(LOW, this, "icm4x6xx_inst_deinit inst_cleanup\n");

  icm4x6xx_inst_cleanup(this);

  return SNS_RC_SUCCESS;
}

/**
 * Sends a self-test completion event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 * @param[i] result    pass/fail result
 * @param[i] type      test type
 * @param[i] err_code  driver specific error code
 *
 * @return none
 */
static void icm4x6xx_send_self_test_event(sns_sensor_instance *instance,
                                        sns_sensor_uid *uid, bool test_result,
                                        sns_physical_sensor_test_type type,
                                        icm4x6xx_test_err_code err_code)
{
  uint8_t data[1] = {(uint8_t)err_code};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
     sns_physical_sensor_test_event_init_default;

  test_event.test_passed = test_result;
  test_event.test_type = type;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
}

int icm4x6xx_self_test_accel_response(sns_sensor_instance *instance)
{
    int32_t lsb[3] = {0,};
    int32_t lsb_st[3] = {0,};
    int16_t raw[3] = {0,};
    uint8_t accel_st_code[3] = {0};
    uint32_t st_response = 0;
    int rc = 0;

    #define ACC_SELFTEST_SAMPLES_NUMBER 200

    rc += icm4x6xx_read_accel_stcode(instance, accel_st_code);

    rc += icm4x6xx_set_accel_fsr(instance, ACC_RANGE_2G);
    rc += icm4x6xx_set_accel_bandwidth(instance, BW_ODR_DIV_10);
    rc += icm4x6xx_set_accel_filter_order(instance, THIRD_ORDER);
    rc += icm4x6xx_set_accel_odr(instance, ICM4X6XX_ODR_1000);
    rc += icm4x6xx_set_accel_mode(instance, ICM4X6XX_A_LNM);
    rc += icm4x6xx_set_accel_bandwidth(instance, BW_ODR_DIV_10);

    // Delay 100ms
    rc += sns_busy_wait(sns_convert_ns_to_ticks(100*1000*1000));

    // Read Accel raw data 20 samples every 1ms
    for(int i = 0 ; i < ACC_SELFTEST_SAMPLES_NUMBER ; i++) {
        rc += icm4x6xx_read_accel_data(instance, raw);
        for(int j = 0 ; j < 3 ; j++) {
            lsb[j] += raw[j];
        }
        rc += sns_busy_wait(sns_convert_ns_to_ticks(1 * 1000 * 1000));
    }

    // Average Data
    for(int i = 0 ; i < 3 ; i++) {
        lsb[i] = lsb[i] / ACC_SELFTEST_SAMPLES_NUMBER;
    }

    rc += icm4x6xx_enable_accel_self_test(instance, true);

    // Delay 100ms
    rc += sns_busy_wait(sns_convert_ns_to_ticks(200*1000*1000));

    // Read Accel raw data 20 samples every 1ms in self-test mode
    for(int i = 0 ; i < ACC_SELFTEST_SAMPLES_NUMBER ; i++) {
        rc += icm4x6xx_read_accel_data(instance, raw);
        for(int j = 0 ; j < 3 ; j++) {
            lsb_st[j] += raw[j];
        }
        rc += sns_busy_wait(sns_convert_ns_to_ticks(1 * 1000 * 1000));
    }

    rc += icm4x6xx_enable_accel_self_test(instance, false);
    rc += icm4x6xx_set_accel_mode(instance, ICM4X6XX_A_OFF);

    // Average Data
    for(int i = 0 ; i < 3 ; i++) {
        lsb_st[i] = lsb_st[i] / ACC_SELFTEST_SAMPLES_NUMBER;
    }

    uint32_t st_otp[3] = {0};

    ICM4X6XX_INST_PRINTF(MED, instance, "accel st_code %d %d %d", accel_st_code[0], accel_st_code[1], accel_st_code[2]);

    /* According AN-000149, ST_OTP = (2620/ 2^(3-FS)) * 1.01^(ST_code-1) (lsb) */
    for (int i = 0; i < 3; i++) {
        st_otp[i] = (2620/ pow(2, 3 - 3)) * powf(1.01, (accel_st_code[i]-1));
    }

    ICM4X6XX_INST_PRINTF(MED, instance, "accel st_otp %d %d %d", st_otp[0], st_otp[1], st_otp[2]);

    /* Check selftest result according AN-000149 */
    for (int i = 0; i < 3; i++) {
        st_response = abs(lsb[i] - lsb_st[i]);
        ICM4X6XX_INST_PRINTF(MED, instance, "accel st_response  %d", st_response);
        if (st_otp[i] != 0) {
            float ratio = ((float)st_response) / ((float)st_otp[i]);
            if ((ratio >= 1.5f) || (ratio <= 0.5f)) {
                ICM4X6XX_INST_PRINTF(HIGH, instance, "accel st test fail0");
                rc += SNS_RC_FAILED;
            }
        } else {
                /* compare 50mg, 1200mg to LSB according AN-000149
                * convert 50msg, 1200mg to LSB with fsr +/-2g
                * For Havana according AN-000134, 225mg and 675 mg,
                * but we can we same value as Yokohama to make more margin */
            if ((st_response < ((50 * 32768 / 2) / 1000)) ||
                (st_response > ((1200 * 32768 / 2) / 1000))) {
                ICM4X6XX_INST_PRINTF(HIGH, instance, "accel st test fail1");
                rc += SNS_RC_FAILED;
            }
        }
    }

    // Delay 200 ms
    rc += sns_busy_wait(sns_convert_ns_to_ticks(100*1000*1000));

    return rc;
}

int icm4x6xx_self_test_gyro_response(sns_sensor_instance *instance)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    int32_t lsb[3] = {0,};
    int32_t lsb_st[3] = {0,};
    uint8_t gyro_st_code[3] = {0};
    int16_t raw[3] = {0,};
    uint32_t st_response = 0;
    int rc = 0;

    #define GYRO_SELFTEST_SAMPLES_NUMBER 200

    rc += icm4x6xx_read_gyro_stcode(instance, gyro_st_code);
    rc += icm4x6xx_set_gyro_fsr(instance, GYRO_RANGE_250DPS);
    rc += icm4x6xx_set_gyro_bandwidth(instance, BW_ODR_DIV_10);
    rc += icm4x6xx_set_gyro_odr(instance, ICM4X6XX_ODR_1000);
    rc += icm4x6xx_set_gyro_filter_order(instance, THIRD_ORDER);
    rc += icm4x6xx_en_gyro(instance, true);

    // Delay 100ms
    rc += sns_busy_wait(sns_convert_ns_to_ticks(100*1000*1000));

    // Read Gyro raw data 20 samples every 1ms
    for(int i = 0 ; i < GYRO_SELFTEST_SAMPLES_NUMBER ; i++) {
        rc += icm4x6xx_read_gyro_data(instance, raw);
        for(int j = 0 ; j < 3 ; j++) {
            lsb[j] += raw[j];
        }
        rc += sns_busy_wait(sns_convert_ns_to_ticks(1 * 1000 * 1000));
    }

    // Average Data
    for(int i = 0 ; i < 3 ; i++) {
        lsb[i] = lsb[i] / GYRO_SELFTEST_SAMPLES_NUMBER;
    }

    rc += icm4x6xx_enable_gyro_self_test(instance, true);

    // Delay 200ms
    rc += sns_busy_wait(sns_convert_ns_to_ticks(100*1000*1000));

    // Read Gyro raw data 20 samples every 1ms in self-test mode
    for(int i = 0 ; i < GYRO_SELFTEST_SAMPLES_NUMBER ; i++) {
        rc += icm4x6xx_read_gyro_data(instance, raw);
        for(int j = 0 ; j < 3 ; j++) {
            lsb_st[j] += raw[j];
        }
        rc += sns_busy_wait(sns_convert_ns_to_ticks(1 * 1000 * 1000));
    }

    rc += icm4x6xx_enable_gyro_self_test(instance, false);
    rc += icm4x6xx_en_gyro(instance, false);
    // Average Data
    for(int i = 0 ; i < 3 ; i++) {
        lsb_st[i] = lsb_st[i] / GYRO_SELFTEST_SAMPLES_NUMBER;
    }

    uint32_t st_otp[3] = {0};

    ICM4X6XX_INST_PRINTF(MED, instance, "gyro st_code %d %d %d", gyro_st_code[0], gyro_st_code[1], gyro_st_code[2]);

    /* According AN-000149, ST_OTP = (2620/ 2^(3-FS)) * 1.01^(ST_code-1) (lsb) */
    for (int i = 0; i < 3; i++) {
        st_otp[i] = (2620/ pow(2, 3 - 3)) * powf(1.01, (gyro_st_code[i]-1));
    }

    ICM4X6XX_INST_PRINTF(MED, instance, "gyro st_otp %d %d %d", st_otp[0], st_otp[1], st_otp[2]);

    /* Check selftest result according AN-000149 */
    for (int i = 0; i < 3; i++) {
        st_response = abs(lsb[i] - lsb_st[i]);
        ICM4X6XX_INST_PRINTF(MED, instance, "gyro st_response  %d", st_response);
        if (st_otp[i] != 0) {
            float ratio = ((float)st_response) / ((float)st_otp[i]);
            if ((ratio <= 0.5f) || ((ratio >= 1.5f) && (state->product_info.series == YOKOHAMA))) {
                ICM4X6XX_INST_PRINTF(HIGH, instance, "gyro st test fail0");
                rc += SNS_RC_FAILED;
            }
        } else {
            /* compare 60dps with LSB according AN-000149
                * convert 60dps to LSB with fsr +/-250dps */
            if (st_response < (60 * 32768 / 250)) {
                ICM4X6XX_INST_PRINTF(HIGH, instance, "gyro st test fail1");
                rc += SNS_RC_FAILED;
            }
        }
    }

    // Delay 200 ms
    rc += sns_busy_wait(sns_convert_ns_to_ticks(100*1000*1000));

    return rc;
}

void icm4x6xx_st_recover_pre_state(sns_sensor_instance* instance)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    float req_match_odr = 0.0;
    icm4x6xx_sensor_odr chosen_odr_reg_value = ODR_NOT_SUPPORTED;

    state->accel_info.desire_en = state->st_resume.desire_en_a;
    state->accel_info.en_fifo = state->st_resume.desire_en_a_fifo;
    state->gyro_info.desire_en = state->st_resume.desire_en_g;
    state->gyro_info.en_fifo = state->st_resume.desire_en_g_fifo;
    state->fifo_info.desire_wmk = state->st_resume.watermark;
    state->accel_info.req_match_odr = state->st_resume.req_match_odr;
    state->gyro_info.req_match_odr = state->st_resume.req_match_odr;
    state->accel_info.desire_odr_reg = state->st_resume.desire_odr_reg;
    state->gyro_info.desire_odr_reg = state->st_resume.desire_odr_reg;
    state->fifo_info.desire_format = state->st_resume.desire_format;
    state->fifo_info.max_requested_flush_ticks = state->st_resume.max_requested_flush_ticks;
    state->fifo_info.max_requested_wmk = state->st_resume.watermark;
    state->md_info.enable_md_int = state->st_resume.enable_md_int;

    icm4x6xx_get_best_match_odr(state,
                                state->accel_info.req_match_odr,
                                &req_match_odr,
                                &chosen_odr_reg_value,
                                &state->accel_info.num_samples_to_discard,
                                &state->gyro_info.num_samples_to_discard);
    ICM4X6XX_INST_PRINTF(MED, instance, "recover odr %d %d",
                    (uint32_t)req_match_odr, chosen_odr_reg_value);

    state->accel_info.req_match_odr = req_match_odr;
    state->gyro_info.req_match_odr = req_match_odr;
    state->accel_info.desire_odr_reg = chosen_odr_reg_value;
    state->gyro_info.desire_odr_reg = chosen_odr_reg_value;

    //state->accel_info.num_samples_to_discard = state->st_resume.a_samples_to_discard;
    //state->gyro_info.num_samples_to_discard = state->st_resume.g_samples_to_discard;
    ICM4X6XX_INST_PRINTF(HIGH, instance, "re-en st a discard %d", state->accel_info.num_samples_to_discard);
    ICM4X6XX_INST_PRINTF(HIGH, instance, "re-en st g discard %d", state->gyro_info.num_samples_to_discard);
    ICM4X6XX_INST_PRINTF(MED, instance, "icm4x6xx_st_recover_pre_state done");
}

void icm4x6xx_start_factory_test(sns_sensor_instance* instance, icm4x6xx_sensor_type sensor)
{
  icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;

  icm4x6xx_st_store_pre_state(instance);

  state->fac_test_info.num_samples = 0;
  state->fac_test_info.sample_square_sum[0] = 0;
  state->fac_test_info.sample_square_sum[1] = 0;
  state->fac_test_info.sample_square_sum[2] = 0;
  state->fac_test_info.sample_sum[0] = 0;
  state->fac_test_info.sample_sum[1] = 0;
  state->fac_test_info.sample_sum[2] = 0;
  state->fac_test_in_progress = true;
  state->fac_test_sensor = sensor;
  state->fac_test_info.at_rest = true;

  if(sensor == ICM4X6XX_ACCEL)
  {
    state->fac_test_info.bias_thresholds[0] = 190 * G / 1000; // 190 mg
    state->fac_test_info.bias_thresholds[1] = 190 * G / 1000; // 190 mg
    state->fac_test_info.bias_thresholds[2] = 190 * G / 1000; // 190 mg
    state->fac_test_info.variance_threshold = 0.060025; // (25mg*G)^2
    state->accel_info.desire_en = true;
  }
  else if(sensor == ICM4X6XX_GYRO)
  {
    state->fac_test_info.bias_thresholds[0] = 10 * PI / 180; //10 dps
    state->fac_test_info.bias_thresholds[1] = 10 * PI / 180; //10 dps
    state->fac_test_info.bias_thresholds[2] = 10 * PI / 180; //10 dps
    state->fac_test_info.variance_threshold = 0.000195; // (0.8dps*PI/180)^2
    state->gyro_info.desire_en = true;
  }
  else
  {
    ICM4X6XX_INST_PRINTF(ERROR, instance, "Unknown sensor %d", sensor);
  }

  ICM4X6XX_INST_PRINTF(MED, instance, "icm4x6xx_start_factory_test() sensor %d", sensor);

  state->gyro_info.en_fifo = true;
  state->accel_info.en_fifo = true;
  state->accel_info.desire_en = true;
  state->gyro_info.desire_en = true;
  state->fifo_info.desire_wmk = 1;
  state->fifo_info.max_requested_wmk = 1;
  state->accel_info.req_match_odr = ICM4X6XX_ODR_100;
  state->gyro_info.req_match_odr = ICM4X6XX_ODR_100;
  state->accel_info.desire_odr_reg = ODR_100HZ;
  state->gyro_info.desire_odr_reg = ODR_100HZ;
  state->fifo_info.desire_format = ICM4X6XX_FORMAT_16_BYTES;
  state->fifo_info.max_requested_flush_ticks = UINT64_MAX;

  if (state->fifo_info.curr_format != ICM4X6XX_FORMAT_EMPTY) {
    ICM4X6XX_INST_PRINTF(HIGH, instance, "warning, sensor is in streaming now!");
  } else {
    state->fifo_info.curr_format = state->fifo_info.desire_format;
  }

  icm4x6xx_en_fifo(instance, state->accel_info.en_fifo, state->gyro_info.en_fifo);
  if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_EMPTY)
      state->fifo_info.curr_format = state->fifo_info.desire_format;

#ifndef ICM4X6XX_DISABLE_DAE
  if ((ICM4X6XX_CONFIG_IDLE == state->config_step) &&
    icm4x6xx_dae_if_is_ag_streaming(instance)) {
    state->config_step = ICM4X6XX_CONFIG_FLUSHING_HW;
    icm4x6xx_dae_if_flush_hw(instance);
  } else if (ICM4X6XX_CONFIG_IDLE == state->config_step) {
#else
  if (ICM4X6XX_CONFIG_IDLE == state->config_step) {
#endif
    icm4x6xx_reconfig_hw(instance);
  } else {
    ICM4X6XX_INST_PRINTF(HIGH, instance, "confit step is not idle");
  }
}

/** See sns_icm4x6xx_hal.h */
void icm4x6xx_run_self_test(sns_sensor_instance *instance)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    int rc = 0;
    uint8_t whoami = 0;
    bool who_am_i_success = false;

    ICM4X6XX_INST_PRINTF(MED, instance, "icm4x6xx_run_self_test");

    rc = icm4x6xx_read_id(state->scp_service,
                          state->com_port_info.port_handle,
                          &whoami);
    if (rc == SNS_RC_SUCCESS &&
        (whoami == state->product_info.whoami)) {
        ICM4X6XX_INST_PRINTF(MED, instance, "self test invn sensor present");
        who_am_i_success = true;
    }

    if(state->accel_info.test_info.test_client_present) {
        ICM4X6XX_INST_PRINTF(MED, instance, "self-test test_type = %d", state->accel_info.test_info.test_type);
        if (state->accel_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM) {
            icm4x6xx_send_self_test_event(instance, &state->accel_info.suid,
            who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, ICM4X6XX_TEST_NO_ERROR);
        } else if (state->accel_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY) {
            // Handle factory test. The driver may choose to reject any new
            // streaming/self-test requests when factory test is in progress.
            if (who_am_i_success)
                icm4x6xx_start_factory_test(instance, ICM4X6XX_ACCEL);
            else {
                ICM4X6XX_INST_PRINTF(MED, instance, "no dev exist");
                icm4x6xx_send_self_test_event(instance, &state->accel_info.suid,
                    false, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, rc);
            }

        } else if(state->accel_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_HW) {
            ICM4X6XX_INST_PRINTF(LOW, instance, "do Accel Self-Test HW");
            sns_sensor_util_remove_sensor_instance_stream(instance, &state->timer_data_stream);
            sns_sensor_util_remove_sensor_instance_stream(instance, &state->heart_beat.timer_data_stream);
            icm4x6xx_st_store_pre_state(instance);
#ifndef ICM4X6XX_DISABLE_DAE
            icm4x6xx_dae_if_stop_streaming(instance);
#endif
            rc = icm4x6xx_self_test_accel_response(instance);
            icm4x6xx_send_self_test_event(instance, &state->accel_info.suid,
                                    !rc, SNS_PHYSICAL_SENSOR_TEST_TYPE_HW, rc);
            icm4x6xx_reset_device(instance);
            icm4x6xx_st_recover_pre_state(instance);
            icm4x6xx_en_fifo(instance, state->accel_info.en_fifo, state->gyro_info.en_fifo);
            if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_EMPTY)
                state->fifo_info.curr_format = state->fifo_info.desire_format;
            icm4x6xx_reconfig_hw(instance);
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "unsupported test type for this sensor");
        }
        state->accel_info.test_info.test_client_present = false;
    }

    if(state->gyro_info.test_info.test_client_present) {
        if(state->gyro_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM) {
             icm4x6xx_send_self_test_event(instance, &state->gyro_info.suid,
                   who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, ICM4X6XX_TEST_NO_ERROR);
        } else if(state->gyro_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY) {
            ICM4X6XX_INST_PRINTF(MED, instance, "icm4x6xx_run_self_test GYRO SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY.");
            // Handle factory test. The driver may choose to reject any new
            // streaming/self-test requests when factory test is in progress.
            if (who_am_i_success)
                icm4x6xx_start_factory_test(instance, ICM4X6XX_GYRO);
            else {
                ICM4X6XX_INST_PRINTF(MED, instance, "no dev exist");
                icm4x6xx_send_self_test_event(instance, &state->gyro_info.suid,
                    false, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, rc);
            }
        } else if(state->gyro_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_HW) {
            ICM4X6XX_INST_PRINTF(LOW, instance, "do Gyro Self-Test HW");
            sns_sensor_util_remove_sensor_instance_stream(instance, &state->timer_data_stream);
            sns_sensor_util_remove_sensor_instance_stream(instance, &state->heart_beat.timer_data_stream);
            icm4x6xx_st_store_pre_state(instance);
#ifndef ICM4X6XX_DISABLE_DAE
            icm4x6xx_dae_if_stop_streaming(instance);
#endif
            rc = icm4x6xx_self_test_gyro_response(instance);
            icm4x6xx_send_self_test_event(instance, &state->gyro_info.suid,
                                    !rc, SNS_PHYSICAL_SENSOR_TEST_TYPE_HW, rc);
            icm4x6xx_reset_device(instance);
            icm4x6xx_st_recover_pre_state(instance);
            icm4x6xx_en_fifo(instance, state->accel_info.en_fifo, state->gyro_info.en_fifo);
            if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_EMPTY)
                state->fifo_info.curr_format = state->fifo_info.desire_format;
            icm4x6xx_reconfig_hw(instance);
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "unsupported test type for this sensor");
        }
        state->gyro_info.test_info.test_client_present = false;
    }

    if(state->md_info.test_info.test_client_present) {
        if(state->md_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM) {
            icm4x6xx_send_self_test_event(instance, &state->md_info.suid,
                who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, ICM4X6XX_TEST_NO_ERROR);
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "unsupported test type for this sensor");
        }
        state->md_info.test_info.test_client_present = false;
    }

#ifdef ICM4X6XX_ENABLE_LOWG
    if(state->freefall_info.test_info.test_client_present) {
        if(state->freefall_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM) {
            icm4x6xx_send_self_test_event(instance, &state->freefall_info.suid,
                who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, ICM4X6XX_TEST_NO_ERROR);
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "unsupported test type for this sensor");
        }
        state->freefall_info.test_info.test_client_present = false;
    }
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
    if(state->high_shock_info.test_info.test_client_present) {
        if(state->high_shock_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM) {
            icm4x6xx_send_self_test_event(instance, &state->high_shock_info.suid,
                who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, ICM4X6XX_TEST_NO_ERROR);
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "unsupported test type for this sensor");
        }
        state->high_shock_info.test_info.test_client_present = false;
    }
#endif

    if(state->sensor_temp_info.test_info.test_client_present) {
        if(state->sensor_temp_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM) {
            icm4x6xx_send_self_test_event(instance, &state->sensor_temp_info.suid,
                who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, ICM4X6XX_TEST_NO_ERROR);
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "unsupported test type for this sensor");
        }
        state->sensor_temp_info.test_info.test_client_present = false;
    }
}

void icm4x6xx_process_fac_test(sns_sensor_instance *instance)
{
  int i;
  bool test_pass = true;
  float fac_cal_bias[TRIAXIS_NUM];

  icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
  sns_sensor_uid *uid = &state->accel_info.suid;
  sns_icm4x6xx_registry_cfg *registry_cfg = &state->accel_registry_cfg;
  icm4x6xx_test_err_code err_code = ICM4X6XX_TEST_NO_ERROR;

  ICM4X6XX_INST_PRINTF(MED, instance, "(0x%x)icm4x6xx_process_fac_test, num_samples %d",
            state->fac_test_sensor, state->fac_test_info.num_samples);

  if(state->fac_test_sensor == ICM4X6XX_GYRO)
  {
    registry_cfg = &state->gyro_registry_cfg;
    uid = &state->gyro_info.suid;
  }

  if(state->fac_test_info.num_samples >= FACTORY_CAL_SAMPLES)
  {
    ICM4X6XX_INST_PRINTF(MED, instance, "Sample Sums: %d %d %d",
                    (int32_t)state->fac_test_info.sample_sum[0],
                    (int32_t)state->fac_test_info.sample_sum[1],
                    (int32_t)state->fac_test_info.sample_sum[2]);
    ICM4X6XX_INST_PRINTF(MED, instance, "Sample square sums: %d %d %d",
                    (int32_t)state->fac_test_info.sample_square_sum[0],
                    (int32_t)state->fac_test_info.sample_square_sum[1],
                    (int32_t)state->fac_test_info.sample_square_sum[2]);

    state->fac_test_in_progress = false;

    if (state->fac_test_sensor == ICM4X6XX_GYRO)
        state->fac_test_info.num_samples -= FACTORY_CAL_DISCARD_G; // subtract discarded number of samples
    else if (state->fac_test_sensor == ICM4X6XX_ACCEL)
        state->fac_test_info.num_samples -= FACTORY_CAL_DISCARD_A;
    else
        ICM4X6XX_INST_PRINTF(ERROR, instance, "unkown test sensor");

    /** use variable to buffer the bias data */
    fac_cal_bias[0] =
       state->fac_test_info.sample_sum[0] / state->fac_test_info.num_samples;
    fac_cal_bias[1] =
       state->fac_test_info.sample_sum[1] / state->fac_test_info.num_samples;
    fac_cal_bias[2] =
       state->fac_test_info.sample_sum[2] / state->fac_test_info.num_samples;

    for(i = 0; i < TRIAXIS_NUM; i++)
    {
      float varT = (state->fac_test_info.sample_sum[i]) * (state->fac_test_info.sample_sum[i]);

      state->fac_test_info.variance[i] = (state->fac_test_info.sample_square_sum[i]
                                          - (varT / state->fac_test_info.num_samples)) / state->fac_test_info.num_samples;

      // Check variance to determine whether device is stationary
      if(state->fac_test_info.variance[i] > state->fac_test_info.variance_threshold)
      {
        // device is not stationary
        state->fac_test_info.at_rest = false;
        test_pass = false;
        err_code = ICM4X6XX_FAC_TEST_DEV_NOT_STATIONARY;
        ICM4X6XX_INST_PRINTF(ERROR, instance, "FAILED device not stationary var[%u]=%d/1000000 %d/1000000",
                            i, (int32_t)(state->fac_test_info.variance[i] * 1000000),
                            (uint32_t)(state->fac_test_info.variance_threshold * 1000000));
        break;
      }

      // Check biases are in defined limits
      if(fabsf(fac_cal_bias[i]) > state->fac_test_info.bias_thresholds[i])
      {
        test_pass = false;
        err_code = ICM4X6XX_FAC_TEST_HIGH_BIAS;
        ICM4X6XX_INST_PRINTF(ERROR, instance, "FAILED axis(%u) bias very large", i);
        break;
      }

      // Check for zero variance
      if(state->fac_test_info.variance[i] == 0.0)
      {
         test_pass = false;
         err_code = ICM4X6XX_FAC_TEST_ZERO_VARIANCE;
         ICM4X6XX_INST_PRINTF(ERROR, instance, "FAILED axis(%u) zero variance", i);
         break;
      }
    }

    if (test_pass)
    {
      /** update the new bias data only when calibration pass */
      registry_cfg->fac_cal_bias[0] = fac_cal_bias[0];
      registry_cfg->fac_cal_bias[1] = fac_cal_bias[1];
      registry_cfg->fac_cal_bias[2] = fac_cal_bias[2];
      registry_cfg->version++;
      ICM4X6XX_INST_PRINTF(MED, instance, "(%#X)fac cal pass, new bias(ver:%d): %d/1000, %d/1000, %d/1000",
                    state->fac_test_sensor,
                    registry_cfg->version,
                    (int32_t)(registry_cfg->fac_cal_bias[0]*1000),
                    (int32_t)(registry_cfg->fac_cal_bias[1]*1000),
                    (int32_t)(registry_cfg->fac_cal_bias[2]*1000));
    }
    /** update_fac_cal_in_registry if test is successful.*/
    state->update_fac_cal_in_registry = test_pass;

    icm4x6xx_send_cal_event(instance, state->fac_test_sensor);
    icm4x6xx_send_self_test_event(instance, uid,
        test_pass, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, err_code);

    icm4x6xx_st_recover_pre_state(instance);

    icm4x6xx_en_fifo(instance, state->accel_info.en_fifo, state->gyro_info.en_fifo);
    icm4x6xx_reset_fifo(instance);

#ifndef ICM4X6XX_DISABLE_DAE
    if ((ICM4X6XX_CONFIG_IDLE == state->config_step) &&
        icm4x6xx_dae_if_is_ag_streaming(instance) &&
        (state->accel_info.desire_en || state->gyro_info.desire_en)) {
        state->config_step = ICM4X6XX_CONFIG_FLUSHING_HW;
        icm4x6xx_dae_if_flush_hw(instance);
    } else if (ICM4X6XX_CONFIG_IDLE == state->config_step) {
#else
    if (ICM4X6XX_CONFIG_IDLE == state->config_step) {
#endif
      icm4x6xx_reconfig_hw(instance);
    } else {
      ICM4X6XX_INST_PRINTF(HIGH, instance, "confit step is not idle");
    }
  }
}

