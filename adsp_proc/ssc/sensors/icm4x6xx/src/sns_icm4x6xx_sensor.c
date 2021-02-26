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
#include "sns_attribute_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "sns_icm4x6xx_sensor.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_suid.pb.h"
#include "sns_registry.pb.h"

#define NAME                    "icm4x6xx"

#define CONFIG_ACCEL            ".accel.config"
#define CONFIG_GYRO             ".gyro.config"
#define CONFIG_TEMP             ".temp.config"
#define CONFIG_MD               ".md.config"
#ifdef ICM4X6XX_ENABLE_LOWG
#define CONFIG_FREEFALL         ".freefall.config"
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
#define CONFIG_HIGH_SHOCK       ".high_shock.config"
#endif
#define PLATFORM_CONFIG         "_platform.config"
#define PLATFORM_PLACEMENT      "_platform.placement"
#define PLATFORM_ORIENT         "_platform.orient"
#define PLATFORM_FAC_CAL_ACCEL  "_platform.accel.fac_cal"
#define PLATFORM_FAC_CAL_GYRO   "_platform.gyro.fac_cal"
#define PLATFORM_FAC_CAL_TEMP   "_platform.temp.fac_cal"
#define PLATFORM_CONFIG_MD      "_platform.md.config"

#define ICM4X6XX_GEN_GROUP(idx,group) NAME "_" #idx group

typedef struct pb_arg_reg_group_arg
{
  sns_sensor_instance* instance;
  const char*          name;
  icm4x6xx_sensor_type sensor;
  uint32_t version;
}pb_arg_reg_group_arg;

enum {
  REG_CONFIG_ACCEL,
  REG_CONFIG_GYRO,
  REG_CONFIG_TEMP,
  REG_CONFIG_MD,
#ifdef ICM4X6XX_ENABLE_LOWG
  REG_CONFIG_FREEFALL,
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
  REG_CONFIG_HIGH_SHOCK,
#endif
  REG_PLATFORM_CONFIG,
  REG_PLATFORM_PLACEMENT,
  REG_PLATFORM_ORIENT,
  REG_PLATFORM_FAC_CAL_ACCEL,
  REG_PLATFORM_FAC_CAL_GYRO,
  REG_PLATFORM_FAC_CAL_TEMP,
  REG_PLATFORM_CONFIG_MD,
  REG_MAX_CONFIGS,
};

static char icm4x6xx_reg_config[SENSOR_INST_CNT][REG_MAX_CONFIGS][40] = {
  {
    ICM4X6XX_GEN_GROUP(0, CONFIG_ACCEL),
    ICM4X6XX_GEN_GROUP(0, CONFIG_GYRO),
    ICM4X6XX_GEN_GROUP(0, CONFIG_TEMP),
    ICM4X6XX_GEN_GROUP(0, CONFIG_MD),
#ifdef ICM4X6XX_ENABLE_LOWG
    ICM4X6XX_GEN_GROUP(0, CONFIG_FREEFALL),
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
    ICM4X6XX_GEN_GROUP(0, CONFIG_HIGH_SHOCK),
#endif
    ICM4X6XX_GEN_GROUP(0, PLATFORM_CONFIG),
    ICM4X6XX_GEN_GROUP(0, PLATFORM_PLACEMENT),
    ICM4X6XX_GEN_GROUP(0, PLATFORM_ORIENT),
    ICM4X6XX_GEN_GROUP(0, PLATFORM_FAC_CAL_ACCEL),
    ICM4X6XX_GEN_GROUP(0, PLATFORM_FAC_CAL_GYRO),
    ICM4X6XX_GEN_GROUP(0, PLATFORM_FAC_CAL_TEMP),
    ICM4X6XX_GEN_GROUP(0, PLATFORM_CONFIG_MD),
  },
#ifdef ICM4X6XX_ENABLE_DUAL_SENSOR
  {
    ICM4X6XX_GEN_GROUP(1, CONFIG_ACCEL),
    ICM4X6XX_GEN_GROUP(1, CONFIG_GYRO),
    ICM4X6XX_GEN_GROUP(1, CONFIG_TEMP),
    ICM4X6XX_GEN_GROUP(1, CONFIG_MD),
#ifdef ICM4X6XX_ENABLE_LOWG
    ICM4X6XX_GEN_GROUP(0, CONFIG_FREEFALL),
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
    ICM4X6XX_GEN_GROUP(0, CONFIG_HIGH_SHOCK),
#endif
    ICM4X6XX_GEN_GROUP(1, PLATFORM_CONFIG),
    ICM4X6XX_GEN_GROUP(1, PLATFORM_PLACEMENT),
    ICM4X6XX_GEN_GROUP(1, PLATFORM_ORIENT),
    ICM4X6XX_GEN_GROUP(1, PLATFORM_FAC_CAL_ACCEL),
    ICM4X6XX_GEN_GROUP(1, PLATFORM_FAC_CAL_GYRO),
    ICM4X6XX_GEN_GROUP(1, PLATFORM_FAC_CAL_TEMP),
    ICM4X6XX_GEN_GROUP(1, PLATFORM_CONFIG_MD),
  }
#endif
};

void icm4x6xx_sensor_process_registry_event(sns_sensor *const this,
                                           sns_sensor_event *event)
{
  bool rv = true;
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
#ifdef ICM4X6XX_ENABLE_DUAL_SENSOR
  uint32_t reg_idx = state->registration_idx;
#else
  uint32_t reg_idx = 0;
#endif

  pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
      event->event_len);

  if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
  {
    sns_registry_read_event read_event = sns_registry_read_event_init_default;
    pb_buffer_arg group_name = {0,0};
    read_event.name.arg = &group_name;
    read_event.name.funcs.decode = pb_decode_string_cb;

    if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
    {
      ICM4X6XX_PRINTF(ERROR, this, "Error decoding registry event");
    }
    else
    {
      stream = pb_istream_from_buffer((void*)event->event, event->event_len);

      if(0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_CONFIG_ACCEL],
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_CONFIG_GYRO],
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_CONFIG_TEMP],
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_CONFIG_MD],
#ifdef ICM4X6XX_ENABLE_LOWG
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_CONFIG_FREEFALL],
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_CONFIG_HIGH_SHOCK],
#endif
                      group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_cfg,
              .parsed_buffer = &state->common.registry_cfg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_cfg_received = true;
          state->is_dri = state->common.registry_cfg.is_dri;
          state->hardware_id = state->common.registry_cfg.hw_id;
          state->resolution_idx = state->common.registry_cfg.res_idx;
          state->supports_sync_stream = state->common.registry_cfg.sync_stream;

          if (state->sensor == ICM4X6XX_ACCEL)
          {
            sns_sensor *sensor = NULL;
            state->common.a_res_idx = state->resolution_idx;
            for(sensor = this->cb->get_library_sensor(this, true);
              sensor != NULL;
              sensor = this->cb->get_library_sensor(this, false))
            {
              icm4x6xx_state *sensor_state = (icm4x6xx_state*)sensor->state->state;
              sensor_state->common.a_res_idx = state->common.a_res_idx;
              ICM4X6XX_PRINTF(HIGH, this, "(0x%x)Registry accel cfg received a res_idx %d",
                sensor_state->sensor, sensor_state->common.a_res_idx);
            }
          }
          else if (state->sensor == ICM4X6XX_GYRO)
          {
            sns_sensor *sensor = NULL;
            state->common.g_res_idx = state->resolution_idx;
            for(sensor = this->cb->get_library_sensor(this, true);
              sensor != NULL;
              sensor = this->cb->get_library_sensor(this, false))
            {
              icm4x6xx_state *sensor_state = (icm4x6xx_state*)sensor->state->state;
              sensor_state->common.g_res_idx = state->common.g_res_idx;
              ICM4X6XX_PRINTF(HIGH, this, "(0x%x)Registry gyro cfg received g res_idx %d",
                sensor_state->sensor, sensor_state->common.g_res_idx);
            }
          }

          ICM4X6XX_PRINTF(LOW, this, "(0x%x)Registry read event received "
                     "is_dri:%d, hardware_id:%d resolution_idx:%d, supports_sync_stream:%d",
                     state->sensor,
                     state->is_dri, (uint32_t)(state->hardware_id), state->resolution_idx,
                     state->supports_sync_stream);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_CONFIG],
                           group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_pf_cfg,
              .parsed_buffer = &state->common.registry_pf_cfg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_pf_cfg_received = true;

          state->common.com_port_info.com_config.bus_type = state->common.registry_pf_cfg.bus_type;
          state->common.com_port_info.com_config.bus_instance = state->common.registry_pf_cfg.bus_instance;
          state->common.com_port_info.com_config.slave_control = state->common.registry_pf_cfg.slave_config;
          state->common.com_port_info.i2c_address = state->common.registry_pf_cfg.slave_config;
#ifdef ICM4X6XX_ENABLE_I3C
          state->common.com_port_info.i3c_address = state->common.registry_pf_cfg.i3c_address;
#endif
          state->common.com_port_info.com_config.min_bus_speed_KHz = state->common.registry_pf_cfg.min_bus_speed_khz;
          state->common.com_port_info.com_config.max_bus_speed_KHz = state->common.registry_pf_cfg.max_bus_speed_khz;
          state->common.com_port_info.com_config.reg_addr_type = state->common.registry_pf_cfg.reg_addr_type;
          state->common.irq_config.interrupt_num = state->common.registry_pf_cfg.dri_irq_num;
          state->common.irq_config.interrupt_pull_type = state->common.registry_pf_cfg.irq_pull_type;
          state->common.irq_config.is_chip_pin = state->common.registry_pf_cfg.irq_is_chip_pin;
          state->common.irq_config.interrupt_drive_strength = state->common.registry_pf_cfg.irq_drive_strength;
          /* None DAE supports palse mode and latch mode, but select latch mode,
             DAE only supports palse mode, DAE/none DAE/Yoko/Hava are seperated by diff config files */
          state->common.irq_config.interrupt_trigger_type = state->common.registry_pf_cfg.irq_trigger_type;
          state->common.rail_config.num_of_rails = state->common.registry_pf_cfg.num_rail;
          state->common.registry_rail_on_state = state->common.registry_pf_cfg.rail_on_state;
          sns_strlcpy(state->common.rail_config.rails[0].name,
                      state->common.registry_pf_cfg.vddio_rail,
                      sizeof(state->common.rail_config.rails[0].name));
          sns_strlcpy(state->common.rail_config.rails[1].name,
                      state->common.registry_pf_cfg.vdd_rail,
                      sizeof(state->common.rail_config.rails[1].name));

#ifdef ICM4X6XX_ENABLE_I3C
          if (state->common.com_port_info.com_config.bus_type == SNS_BUS_I3C_SDR ||
              state->common.com_port_info.com_config.bus_type == SNS_BUS_I3C_HDR_DDR) {
              state->common.com_port_info.com_config.slave_control = state->common.com_port_info.i3c_address;
          }
#endif

          ICM4X6XX_PRINTF(LOW, this, "Registry read event received "
                     "bus_type:%d bus_instance:%d slave_control:%d "
                     "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d "
                     "i2c_address: %d i3c_address: %d",
                     state->common.com_port_info.com_config.bus_type,
                     state->common.com_port_info.com_config.bus_instance,
                     state->common.com_port_info.com_config.slave_control,
                     state->common.com_port_info.com_config.min_bus_speed_KHz,
                     state->common.com_port_info.com_config.max_bus_speed_KHz,
                     state->common.com_port_info.com_config.reg_addr_type,
                     state->common.com_port_info.i2c_address,
                     state->common.com_port_info.i3c_address);

          ICM4X6XX_PRINTF(LOW, this, "interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d "
                     "interrupt_drive_strength:%d interrupt_trigger_type:%d rigid body type:%d",
                     state->common.irq_config.interrupt_num,
                     state->common.irq_config.interrupt_pull_type,
                     state->common.irq_config.is_chip_pin,
                     state->common.irq_config.interrupt_drive_strength,
                     state->common.irq_config.interrupt_trigger_type,
                     state->common.registry_pf_cfg.rigid_body_type);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_PLACEMENT],
                           group_name.buf_len))
      {
        {
          uint8_t arr_index = 0;
          pb_float_arr_arg arr_arg = {
            .arr = state->common.placement,
            .arr_index = &arr_index,
            .arr_len = 12
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "placement",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &arr_arg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_placement_received = true;
          ICM4X6XX_PRINTF(LOW, this, "Registry read event for group registry_placement received "
                     "p[0]:%u p[6]:%u p[11]:%u", (uint32_t)state->common.placement[0],
                     (uint32_t)state->common.placement[6], (uint32_t)state->common.placement[11]);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_ORIENT],
                           group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "orient",
              .parse_func = sns_registry_parse_axis_orientation,
              .parsed_buffer = state->common.axis_map
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_orient_received = true;

          ICM4X6XX_PRINTF(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[0].ipaxis,
                 state->common.axis_map[0].opaxis, state->common.axis_map[0].invert);

          ICM4X6XX_PRINTF(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[1].ipaxis, state->common.axis_map[1].opaxis,
                 state->common.axis_map[1].invert);

          ICM4X6XX_PRINTF(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[2].ipaxis, state->common.axis_map[2].opaxis,
                 state->common.axis_map[2].invert);
        }
      }
      else if(0 == strncmp((char*)group_name.buf,
                           icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_ACCEL],
                           group_name.buf_len) ||
              0 == strncmp((char*)group_name.buf,
                         icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_GYRO],
                           group_name.buf_len) ||
              0 == strncmp((char*)group_name.buf,
                         icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_TEMP],
                         group_name.buf_len))
      {
        uint32_t fac_cal_version;
        {
          uint8_t bias_arr_index = 0, scale_arr_index = 0;
          pb_float_arr_arg bias_arr_arg = {
            .arr = state->fac_cal_bias,
            .arr_index = &bias_arr_index,
            .arr_len = TRIAXIS_NUM
          };

          pb_float_arr_arg scale_arr_arg = {
            .arr = state->fac_cal_scale,
            .arr_index = &scale_arr_index,
            .arr_len = TRIAXIS_NUM
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 3,
            .parse_info[0] = {
              .group_name = "bias",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &bias_arr_arg
            },
            .parse_info[1] = {
              .group_name = "scale",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &scale_arr_arg
            },
            .parse_info[2] = {
              .group_name = "corr_mat",
              .parse_func = sns_registry_parse_corr_matrix_3,
              .parsed_buffer = &state->fac_cal_corr_mat
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
          fac_cal_version = arg.version;
        }

        if(rv)
        {
          state->registry_fac_cal_received = true;
          state->fac_cal_version = fac_cal_version;
          if(state->fac_cal_scale[0] != 0.0)
          {
            state->fac_cal_corr_mat.e00 = state->fac_cal_scale[0];
            state->fac_cal_corr_mat.e11 = state->fac_cal_scale[1];
            state->fac_cal_corr_mat.e22 = state->fac_cal_scale[2];
          }
          ICM4X6XX_PRINTF(MED, this, "(%#X)Read bias from registry: %d/1000 %d/1000 %d/1000 ,ver:%d",
                    state->sensor,
                    (int32_t)(state->fac_cal_bias[TRIAXIS_X]*1000),
                    (int32_t)(state->fac_cal_bias[TRIAXIS_Y]*1000),
                    (int32_t)(state->fac_cal_bias[TRIAXIS_Z]*1000),
                    state->fac_cal_version);
        }
      }
      else if(0 == strncmp((char*)group_name.buf,
                           icm4x6xx_reg_config[reg_idx][REG_PLATFORM_CONFIG_MD],
                           group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "config",
              .parse_func = sns_registry_parse_md_cfg,
              .parsed_buffer = &state->md_config
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->registry_md_config_received = true;
          ICM4X6XX_PRINTF(HIGH, this, "MD config(disable:%d, threshold:%d/1000 m/s2)", state->md_config.disable, (int32_t)(state->md_config.thresh*1000));
        }
      }
      else
      {
        rv = false;
      }

      if(!rv)
      {
        ICM4X6XX_PRINTF(ERROR, this, "Error decoding registry group");
      }
    }
  }
  else
  {
    //ERROR lead to Acceptance test FAIL:  M-EF Total error/fatal messages
    ICM4X6XX_PRINTF(HIGH, this, "Received unsupported registry event msg id %u",
               event->message_id);
  }
}

static void icm4x6xx_sensor_send_registry_request(sns_sensor *const this,
                                                 char *reg_group_name)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  uint8_t buffer[100];
  int32_t encoded_len;
  sns_memset(buffer, 0, sizeof(buffer));
  sns_rc rc = SNS_RC_SUCCESS;

  sns_registry_read_req read_request;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = reg_group_name,
    .buf_len = (strlen(reg_group_name) + 1) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &read_request, sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
      .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    rc = state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
  }
}

void icm4x6xx_request_registry(sns_sensor *const this)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
#ifdef ICM4X6XX_ENABLE_DUAL_SENSOR
  uint32_t reg_idx = state->registration_idx;
#else
  uint32_t reg_idx = 0;
#endif

  // place a request to registry sensor

  if (state->reg_data_stream == NULL &&
     state->common.product_info.whoami == 0)
  {
    sns_sensor_uid suid;

    sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", &suid);
    stream_svc->api->create_sensor_stream(stream_svc,
        this, suid, &state->reg_data_stream);

    if(ICM4X6XX_ACCEL == state->sensor)
    {
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_CONFIG]);
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_PLACEMENT]);
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_ORIENT]);
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_CONFIG_ACCEL]);
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_ACCEL]);
    }
    else if(ICM4X6XX_GYRO == state->sensor)
    {
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_CONFIG_GYRO]);
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_GYRO]);
    }
    else if(ICM4X6XX_SENSOR_TEMP == state->sensor)
    {
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_CONFIG_TEMP]);
    }
    else if(ICM4X6XX_MOTION_DETECT == state->sensor)
    {
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_PLATFORM_CONFIG_MD]);
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_CONFIG_MD]);
    }
#ifdef ICM4X6XX_ENABLE_LOWG
    else if(ICM4X6XX_FREE_FALL == state->sensor)
    {
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_CONFIG_FREEFALL]);
    }
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
    else if(ICM4X6XX_HIGH_SHOCK == state->sensor)
    {
      icm4x6xx_sensor_send_registry_request(this, icm4x6xx_reg_config[reg_idx][REG_CONFIG_HIGH_SHOCK]);
    }
#endif

  }
}

static bool icm4x6xx_encode_registry_group_cb(struct pb_ostream_s *stream,
                                            struct pb_field_s const *field,
                                            void *const *arg)
{
    pb_arg_reg_group_arg* pb_arg = (pb_arg_reg_group_arg*)*arg;
    icm4x6xx_instance_state *state =
            (icm4x6xx_instance_state*)pb_arg->instance->state->state;

    if(0 == strncmp(pb_arg->name,"bias",strlen("bias")))
    {
        char const *names[] = {"x", "y", "z"};

        for (int i = 0; i < ARR_SIZE(names); i++)
        {
            pb_buffer_arg name_data = (pb_buffer_arg)
                { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
            sns_registry_data_item pb_item = sns_registry_data_item_init_default;

            pb_item.name.funcs.encode = &pb_encode_string_cb;
            pb_item.name.arg = &name_data;
            pb_item.has_flt = true;
            pb_item.has_version = true;

            if(pb_arg->sensor == ICM4X6XX_ACCEL)
            {
                pb_item.flt = state->accel_registry_cfg.fac_cal_bias[i];
                pb_item.version = state->accel_registry_cfg.version;
            }
            else
            {
                pb_item.flt = state->gyro_registry_cfg.fac_cal_bias[i];
                pb_item.version = state->gyro_registry_cfg.version;
            }

            if(!pb_encode_tag_for_field(stream, field))
                return false;

            if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
            {
                ICM4X6XX_INST_PRINTF(ERROR, pb_arg->instance, "Error encoding sns_registry_data_item_fields");
                return false;
            }
        }
    }
    else if (0 == strncmp(pb_arg->name,"corr_mat",strlen("corr_mat")))
    {
        char const *names[] = {"0_0", "0_1", "0_2",
                            "1_0", "1_1", "1_2",
                            "2_0", "2_1", "2_2",};

        for (int i = 0; i < ARR_SIZE(names); i++)
        {
            pb_buffer_arg name_data = (pb_buffer_arg)
                    { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
            sns_registry_data_item pb_item = sns_registry_data_item_init_default;

            pb_item.name.funcs.encode = &pb_encode_string_cb;
            pb_item.name.arg = &name_data;
            pb_item.has_flt = true;
            pb_item.has_version = true;
            if(pb_arg->sensor == ICM4X6XX_ACCEL)
            {
                pb_item.flt = state->accel_registry_cfg.fac_cal_corr_mat.data[i];
                pb_item.version = state->accel_registry_cfg.version;
            }
            else
            {
                pb_item.flt = state->gyro_registry_cfg.fac_cal_corr_mat.data[i];
                pb_item.version = state->gyro_registry_cfg.version;
            }

            if(!pb_encode_tag_for_field(stream, field))
                return false;

            if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
            {
                ICM4X6XX_INST_PRINTF(ERROR, pb_arg->instance, "Error encoding sns_registry_data_item_fields");
                return false;
            }
        }
    }
    return true;
}

static bool icm4x6xx_encode_registry_cb(struct pb_ostream_s *stream,
                                        struct pb_field_s const *field,
                                        void *const *arg)
{
    pb_arg_reg_group_arg *reg_arg = (pb_arg_reg_group_arg*)*arg;
    sns_sensor_instance *instance = reg_arg->instance;
    char const *names[] = {"bias", "corr_mat"};

    for(int i = 0; i < ARR_SIZE(names); i++)
    {
        pb_buffer_arg name_data = (pb_buffer_arg)
            { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
                sns_registry_data_item pb_item = sns_registry_data_item_init_default;
                pb_arg_reg_group_arg pb_arg= (pb_arg_reg_group_arg){
                .name = NULL,.instance = instance, .sensor = reg_arg->sensor
            };

        pb_item.has_version = true;
        pb_item.version = reg_arg->version;
        pb_item.name.arg = &name_data;
        pb_item.name.funcs.encode = &pb_encode_string_cb;

        if(0==strncmp(names[i],"bias",name_data.buf_len))
        {
            pb_arg.name = names[i];
            pb_item.has_subgroup = true;
            pb_item.subgroup.items.funcs.encode = &icm4x6xx_encode_registry_group_cb;
            pb_item.subgroup.items.arg = &pb_arg;

        }
        else if(0==strncmp(names[i],"corr_mat",name_data.buf_len))
        {
            pb_arg.name = names[i];
            pb_item.has_subgroup = true;
            pb_item.subgroup.items.funcs.encode = &icm4x6xx_encode_registry_group_cb;
            pb_item.subgroup.items.arg = &pb_arg;
        }
        if(!pb_encode_tag_for_field(stream, field))
        {
            ICM4X6XX_INST_PRINTF(ERROR, instance,"Error encoding corr_mat");

            return false;
        }

        if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
        {
            ICM4X6XX_INST_PRINTF(ERROR, instance,"Error encoding sns_registry_data_item_fields");
            return false;
        }
    }

    return true;
}

void icm4x6xx_update_registry(sns_sensor *const this,
        sns_sensor_instance *const instance, icm4x6xx_sensor_type sensor)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  pb_arg_reg_group_arg arg = {.instance = instance };
  icm4x6xx_instance_state *inst_state =
            (icm4x6xx_instance_state*)instance->state->state;
#ifdef ICM4X6XX_ENABLE_DUAL_SENSOR
  uint32_t reg_idx = state->registration_idx;
#else
  uint32_t reg_idx = 0;
#endif

  uint8_t buffer[1000];
  int32_t encoded_len;
  pb_buffer_arg name_data;
  sns_registry_write_req write_req = sns_registry_write_req_init_default;

  ICM4X6XX_PRINTF(LOW, this, "reg_idx %d", reg_idx);

  if(sensor == ICM4X6XX_ACCEL)
  {
    name_data = (pb_buffer_arg)
          { .buf = icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_ACCEL],
            .buf_len = strlen(icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_ACCEL]) + 1 };
    arg.sensor = ICM4X6XX_ACCEL;
    arg.version = inst_state->accel_registry_cfg.version;
  }
  else if(sensor == ICM4X6XX_GYRO)
  {
    name_data = (pb_buffer_arg)
          { .buf = icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_GYRO],
            .buf_len = strlen(icm4x6xx_reg_config[reg_idx][REG_PLATFORM_FAC_CAL_GYRO]) + 1 };
    arg.sensor = ICM4X6XX_GYRO;
    arg.version = inst_state->accel_registry_cfg.version;
  }
  else
  {
    ICM4X6XX_PRINTF(ERROR, this, "Unsupported sensor %d", sensor);
    return;
  }

  write_req.name.funcs.encode = &pb_encode_string_cb;
  write_req.name.arg = &name_data;
  write_req.data.items.funcs.encode = &icm4x6xx_encode_registry_cb;
  write_req.data.items.arg = &arg;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &write_req, sns_registry_write_req_fields, NULL);
  if(0 < encoded_len)
  {
    if(NULL == state->reg_data_stream)
    {
      sns_service_manager *smgr = this->cb->get_service_manager(this);
      sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
      sns_sensor_uid suid;

      sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", &suid);
      stream_svc->api->create_sensor_stream(stream_svc, this, suid,
          &state->reg_data_stream);
    }

    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
          .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
    state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
  }
}

void icm4x6xx_update_sensor_state(sns_sensor *const this,
        sns_sensor_instance *const instance)
{
  icm4x6xx_state *sensor_state;
  icm4x6xx_instance_state *inst_state = (icm4x6xx_instance_state*)instance->state->state;
  sns_sensor *sensor = NULL;

  for(sensor = this->cb->get_library_sensor(this, true);
      sensor != NULL;
      sensor = this->cb->get_library_sensor(this, false))
  {
    sensor_state = (icm4x6xx_state*)sensor->state->state;

    if(sensor_state->sensor == ICM4X6XX_ACCEL
       && sensor_state->fac_cal_version < inst_state->accel_registry_cfg.version)
    {
      sns_memscpy(&sensor_state->fac_cal_bias, sizeof(sensor_state->fac_cal_bias),
                  &inst_state->accel_registry_cfg.fac_cal_bias[0],
                  sizeof(inst_state->accel_registry_cfg.fac_cal_bias));

      sns_memscpy(&sensor_state->fac_cal_corr_mat, sizeof(sensor_state->fac_cal_corr_mat),
                  &inst_state->accel_registry_cfg.fac_cal_corr_mat,
                  sizeof(inst_state->accel_registry_cfg.fac_cal_corr_mat));
      sensor_state->fac_cal_version = inst_state->accel_registry_cfg.version;
    }
    else if(sensor_state->sensor == ICM4X6XX_GYRO
            && sensor_state->fac_cal_version < inst_state->gyro_registry_cfg.version)
    {
      sns_memscpy(&sensor_state->fac_cal_bias, sizeof(sensor_state->fac_cal_bias),
                  &inst_state->gyro_registry_cfg.fac_cal_bias[0],
                  sizeof(inst_state->gyro_registry_cfg.fac_cal_bias));

      sns_memscpy(&sensor_state->fac_cal_corr_mat, sizeof(sensor_state->fac_cal_corr_mat),
                  &inst_state->gyro_registry_cfg.fac_cal_corr_mat,
                  sizeof(inst_state->gyro_registry_cfg.fac_cal_corr_mat));
      sensor_state->fac_cal_version = inst_state->gyro_registry_cfg.version;
    }
    else if(sensor_state->sensor == ICM4X6XX_SENSOR_TEMP
            && sensor_state->fac_cal_version < inst_state->sensor_temp_registry_cfg.version)
    {
      sns_memscpy(&sensor_state->fac_cal_bias, sizeof(sensor_state->fac_cal_bias),
                  &inst_state->sensor_temp_registry_cfg.fac_cal_bias[0],
                  sizeof(inst_state->sensor_temp_registry_cfg.fac_cal_bias));

      sns_memscpy(&sensor_state->fac_cal_corr_mat, sizeof(sensor_state->fac_cal_corr_mat),
                  &inst_state->sensor_temp_registry_cfg.fac_cal_corr_mat,
                  sizeof(inst_state->sensor_temp_registry_cfg.fac_cal_corr_mat));
      sensor_state->fac_cal_version = inst_state->sensor_temp_registry_cfg.version;
    }
  }
}

/* To easily add a new product,
 * the whoami value should be only used in
 * icm4x6xx_check_product_info function,
 * and it's better whoami should only be used in that switch case
 */
#define ICM40600_WHOAMI_VALUE           0x99
#define ICM40602_WHOAMI_VALUE           0x31
#define ICM40604_WHOAMI_VALUE           0x32
#define ICM40605_WHOAMI_VALUE           0x33
#define ICM40607_WHOAMI_VALUE           0x38
#define ICM42600_WHOAMI_VALUE           0x40
#define ICM42602_WHOAMI_VALUE           0x41
#define ICM42605_WHOAMI_VALUE           0x42
#define ICM42606_WHOAMI_VALUE           0x43
#define ICM42622_WHOAMI_VALUE           0x46
#define ICM42686_WHOAMI_VALUE           0x44
#define ICM42688_WHOAMI_VALUE           0x47

static bool icm4x6xx_check_product_info(sns_sensor *this, uint8_t whoami)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    bool hw_is_present = false;

    ICM4X6XX_PRINTF(MED, this, "WHOAMI 0x%x", whoami);

    state->common.product_info.series = UNKOWN;
    state->common.product_info.i3c_supported = false;
#ifdef ICM4X6XX_ENABLE_I3C
    state->common.product_info.i3c_use_int1 = ICM4X6XX_I3C_USE_INT1;
#endif
    state->common.product_info.use_hi_res = false;
    state->common.product_info.hi_range = false;

    switch (whoami) {
    case ICM40600_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = HAVANA;
        break;
    case ICM40602_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = HAVANA;
        break;
    case ICM40604_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = HAVANA;
        break;
    case ICM40605_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = HAVANA;
        break;
    case ICM40607_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = YOKOHAMA;
        break;
    case ICM42600_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = YOKOHAMA;
        break;
    case ICM42602_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = YOKOHAMA;
        break;
    case ICM42605_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = YOKOHAMA;
        break;
    case ICM42606_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = YOKOHAMA;
        break;
    case ICM42622_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.series = YOKOHAMA;
        break;
    case ICM42686_WHOAMI_VALUE:
        hw_is_present = true;
        state->common.product_info.use_hi_res = true;
        state->common.product_info.hi_range = true;
        state->common.product_info.series = YOKOHAMA;
        break;
    case ICM42688_WHOAMI_VALUE:
        hw_is_present = true;
        if (4 == state->common.a_res_idx && 7 == state->common.g_res_idx)
        {
            //High resolution (i.e. 20-bits data format) is applicable only when accel FSR is +/-16G and gyro FSR is +/-2000dps
            state->common.product_info.use_hi_res = true;
        }
        else
        {
            state->common.product_info.use_hi_res = false;
        }
        ICM4X6XX_PRINTF(MED, this, "use_hi_res(%d), a_res_idx(%d),g_res_idx(%d)",
                        state->common.product_info.use_hi_res,
                        state->common.a_res_idx,
                        state->common.g_res_idx);
        state->common.product_info.series = YOKOHAMA;
        break;
    default:
        hw_is_present = false;
        break;
    }

    if (!hw_is_present) {
        return hw_is_present;
    }

    if (YOKOHAMA == state->common.product_info.series) {
        /* 40607 not support I3C according to FAE training at Dec/11/2018 */
        if (ICM40607_WHOAMI_VALUE == whoami)
            state->common.product_info.i3c_supported = false;
        else
            state->common.product_info.i3c_supported = true;
        state->common.product_info.fifo_size =
            ICM4X6XX_YOKOHAMA_MAX_FIFO * FIFO_USE_PROPORTION;
    } else if (HAVANA == state->common.product_info.series) {
        state->common.product_info.i3c_supported = false;
        state->common.product_info.fifo_size =
            ICM4X6XX_HAVANA_MAX_FIFO * FIFO_HAVANA_PROPORTION;
    } else {
        state->common.product_info.i3c_supported = false;
        state->common.product_info.fifo_size = 0;
        ICM4X6XX_PRINTF(MED, this, "unkown product");
    }

    state->common.product_info.whoami = whoami;

    ICM4X6XX_PRINTF(MED, this, "invn product %d, hi_res %d, hi_range %d",
                state->common.product_info.series,
                state->common.product_info.use_hi_res,
                state->common.product_info.hi_range);

    return hw_is_present;
}

bool icm4x6xx_discover_hw(sns_sensor *const this)
{
    uint8_t whoami = 0;
    bool hw_is_present = false;
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    sns_rc rv = SNS_RC_SUCCESS;

#ifdef ICM4X6XX_ENABLE_I3C
    rv = icm4x6xx_enter_i3c_mode(NULL, &state->common.com_port_info, state->scp_service);
    if (rv != SNS_RC_SUCCESS)
        ICM4X6XX_PRINTF(MED, this, "enter i3c mode failed");
#endif

    /**-------------------Read and Confirm WHO-AM-I------------------------*/
    rv = icm4x6xx_read_id(state->scp_service,
                          state->common.com_port_info.port_handle,
                          &whoami);

    hw_is_present = (rv == SNS_RC_SUCCESS) ?
                    icm4x6xx_check_product_info(this, whoami) : false;

    /**------------------Power Down and Close COM Port--------------------*/
    state->scp_service->api->sns_scp_update_bus_power(
                                              state->common.com_port_info.port_handle,
                                              false);

    state->scp_service->api->sns_scp_close(state->common.com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->common.com_port_info.port_handle);

    /**----------------------Turn Power Rail OFF--------------------------*/
    state->common.rail_config.rail_vote = SNS_RAIL_OFF;
    state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                           this,
                                                           &state->common.rail_config,
                                                           NULL);

    return hw_is_present;
}

void icm4x6xx_publish_ag_commom_attrs(sns_sensor *const this)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;

    if (state->sensor != ICM4X6XX_ACCEL && state->sensor != ICM4X6XX_GYRO) {
        return;
    }

    if (state->common.product_info.series == HAVANA) {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
        values[0].has_flt = true;
        values[0].flt = ICM4X6XX_ODR_25;
        values[1].has_flt = true;
        values[1].flt = ICM4X6XX_ODR_50;
        values[2].has_flt = true;
        values[2].flt = ICM4X6XX_ODR_100;
        values[3].has_flt = true;
        values[3].flt = ICM4X6XX_ODR_200;
#ifdef ICM4X6XX_1K_2_500HZ
        values[4].has_flt = true;
        values[4].flt = ICM4X6XX_ODR_500;
#else
        values[4].has_flt = true;
        values[4].flt = ICM4X6XX_ODR_1000;
#endif
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES,
          values, ARR_SIZE(values), false);
    } else {
#ifdef ICM4X6XX_ENABLE_RTC_MODE
        float odr_adjust_rate = ICM4X6XX_ODR_ADJUST_RATE;
#else
        float odr_adjust_rate = 1.0f;
#endif

#ifdef ICM4X6XX_DISABLE_DAE
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
          SNS_ATTR, SNS_ATTR/*, SNS_ATTR*/};
        values[0].has_flt = true;
        values[0].flt = ICM4X6XX_ODR_12_5 * odr_adjust_rate;
        values[1].has_flt = true;
        values[1].flt = ICM4X6XX_ODR_25 * odr_adjust_rate;
        values[2].has_flt = true;
        values[2].flt = ICM4X6XX_ODR_50 * odr_adjust_rate;
        values[3].has_flt = true;
        values[3].flt = ICM4X6XX_ODR_100 * odr_adjust_rate;
        values[4].has_flt = true;
        values[4].flt = ICM4X6XX_ODR_200 * odr_adjust_rate;
        values[5].has_flt = true;
        values[5].flt = ICM4X6XX_ODR_500 * odr_adjust_rate;
        //values[6].has_flt = true;
        //values[6].flt = ICM4X6XX_ODR_1000;
#else
        /* For DAE, the minim ODR should be big than 20Hz */
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
          SNS_ATTR};
        values[0].has_flt = true;
        values[0].flt = ICM4X6XX_ODR_25 * odr_adjust_rate;
        values[1].has_flt = true;
        values[1].flt = ICM4X6XX_ODR_50 * odr_adjust_rate;
        values[2].has_flt = true;
        values[2].flt = ICM4X6XX_ODR_100 * odr_adjust_rate;
        values[3].has_flt = true;
        values[3].flt = ICM4X6XX_ODR_200 * odr_adjust_rate;
        values[4].has_flt = true;
        values[4].flt = ICM4X6XX_ODR_500 * odr_adjust_rate;
#endif
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES,
            values, ARR_SIZE(values), false);
    }

    {
         sns_std_attr_value_data values[] = {SNS_ATTR};
         values[0].has_flt = true;
         values[0].flt = ICM4X6XX_ODR_1000;

         sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES,
             values, ARR_SIZE(values), false);
    }

    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint = state->common.product_info.fifo_size; // samples
        sns_publish_attribute(
            this, SNS_STD_SENSOR_ATTRID_FIFO_SIZE, &value, 1, true);
    }
}

void icm4x6xx_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

static sns_rc icm4x6xx_register_power_rail(sns_sensor *const this)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  sns_service_manager *smgr = this->cb->get_service_manager(this);
  sns_rc rv = SNS_RC_SUCCESS;

  state->common.rail_config.rail_vote = SNS_RAIL_OFF;

  if(NULL == state->pwr_rail_service)
  {
    state->pwr_rail_service =
     (sns_pwr_rail_service*)smgr->get_service(smgr, SNS_POWER_RAIL_SERVICE);

    state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                           &state->common.rail_config);
  }

  if(NULL == state->pwr_rail_service)
  {
    rv = SNS_RC_FAILED;
  }
  return rv;
}

static void icm4x6xx_publish_a_range_res(sns_sensor *const this)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    bool hi_res = state->common.product_info.use_hi_res;
    bool hi_range = state->common.product_info.hi_range;

    if (state->sensor != ICM4X6XX_ACCEL) {
        return;
    }

    {
      sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};

      sns_std_attr_value_data range0[] = {SNS_ATTR, SNS_ATTR};
      range0[0].has_flt = true;
      range0[0].flt = -1 * ACC_RANGE_UNIT_G(ACC_RANGE_1G, hi_range);
      range0[1].has_flt = true;
      range0[1].flt = ACC_RANGE_UNIT_G(ACC_RANGE_1G, hi_range);
      values[0].has_subtype = true;
      values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[0].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range0, .buf_len = ARR_SIZE(range0) });

      sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
      range1[0].has_flt = true;
      range1[0].flt = -1 * ACC_RANGE_UNIT_G(ACC_RANGE_2G, hi_range);
      range1[1].has_flt = true;
      range1[1].flt = ACC_RANGE_UNIT_G(ACC_RANGE_2G, hi_range);
      values[1].has_subtype = true;
      values[1].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[1].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });

      sns_std_attr_value_data range2[] = {SNS_ATTR, SNS_ATTR};
      range2[0].has_flt = true;
      range2[0].flt = -1 * ACC_RANGE_UNIT_G(ACC_RANGE_4G, hi_range);
      range2[1].has_flt = true;
      range2[1].flt = ACC_RANGE_UNIT_G(ACC_RANGE_4G, hi_range);
      values[2].has_subtype = true;
      values[2].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[2].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range2, .buf_len = ARR_SIZE(range2) });

      sns_std_attr_value_data range3[] = {SNS_ATTR, SNS_ATTR};
      range3[0].has_flt = true;
      range3[0].flt = -1 * ACC_RANGE_UNIT_G(ACC_RANGE_8G, hi_range);;
      range3[1].has_flt = true;
      range3[1].flt = ACC_RANGE_UNIT_G(ACC_RANGE_8G, hi_range);
      values[3].has_subtype = true;
      values[3].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[3].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range3, .buf_len = ARR_SIZE(range3) });

      sns_std_attr_value_data range4[] = {SNS_ATTR, SNS_ATTR};
      range4[0].has_flt = true;
      range4[0].flt = -1 * ACC_RANGE_UNIT_G(ACC_RANGE_16G, hi_range);
      range4[1].has_flt = true;
      range4[1].flt = ACC_RANGE_UNIT_G(ACC_RANGE_16G, hi_range);;
      values[4].has_subtype = true;
      values[4].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[4].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range4, .buf_len = ARR_SIZE(range4) });
      sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
          values, ARR_SIZE(values), false);
    }

    {
      /* Resolution attribute value for accel is in mg/LSB unit */
      sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
      values[0].has_flt = true;
      values[0].flt = ACC_RES_MG_LSB(ACC_RANGE_1G, hi_res, hi_range);
      values[1].has_flt = true;
      values[1].flt = ACC_RES_MG_LSB(ACC_RANGE_2G, hi_res, hi_range);
      values[2].has_flt = true;
      values[2].flt = ACC_RES_MG_LSB(ACC_RANGE_4G, hi_res, hi_range);
      values[3].has_flt = true;
      values[3].flt = ACC_RES_MG_LSB(ACC_RANGE_8G, hi_res, hi_range);
      values[4].has_flt = true;
      values[4].flt = ACC_RES_MG_LSB(ACC_RANGE_16G, hi_res, hi_range);
      sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS,
          values, ARR_SIZE(values), false);
    }

    /** Only accel and gyro use registry information for selected range. */
    {
      sns_std_attr_value_data values[] = {SNS_ATTR};
      sns_std_attr_value_data rangeMinMax[] = {SNS_ATTR, SNS_ATTR};
      rangeMinMax[0].has_flt = true;
      rangeMinMax[0].flt = -1 * ACC_RANGE_UNIT_G(ACC_RANGE_MAX - state->resolution_idx, hi_range);
      rangeMinMax[1].has_flt = true;
      rangeMinMax[1].flt = ACC_RANGE_UNIT_G(ACC_RANGE_MAX - state->resolution_idx, hi_range);
      values[0].has_subtype = true;
      values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[0].subtype.values.arg =
        &((pb_buffer_arg){ .buf = rangeMinMax, .buf_len = ARR_SIZE(rangeMinMax) });
      sns_publish_attribute(
          this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE, &values[0], ARR_SIZE(values), true);
    }

    {
      /** Only accel and gyro use registry information for selected resolution. */
      sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.has_flt = true;
      value.flt = ACC_RES_MG_LSB(ACC_RANGE_MAX - state->resolution_idx, hi_res, hi_range);
      sns_publish_attribute(
          this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION, &value, 1, false);
    }
}

static void icm4x6xx_publish_g_range_res(sns_sensor *const this)
{
    icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
    bool hi_res = state->common.product_info.use_hi_res;
    bool hi_range = state->common.product_info.hi_range;

    if (state->sensor != ICM4X6XX_GYRO) {
        return;
    }

    {
      sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};

      sns_std_attr_value_data range0[] = {SNS_ATTR, SNS_ATTR};
      range0[0].has_flt = true;
      range0[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_15_625DPS, hi_range);
      range0[1].has_flt = true;
      range0[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_15_625DPS, hi_range);
      values[0].has_subtype = true;
      values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[0].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range0, .buf_len = ARR_SIZE(range0) });

      sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
      range1[0].has_flt = true;
      range1[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_31_25DPS, hi_range);
      range1[1].has_flt = true;
      range1[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_31_25DPS, hi_range);
      values[1].has_subtype = true;
      values[1].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[1].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });


      sns_std_attr_value_data range2[] = {SNS_ATTR, SNS_ATTR};
      range2[0].has_flt = true;
      range2[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_62_5DPS, hi_range);
      range2[1].has_flt = true;
      range2[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_62_5DPS, hi_range);
      values[2].has_subtype = true;
      values[2].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[2].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range2, .buf_len = ARR_SIZE(range2) });

      sns_std_attr_value_data range3[] = {SNS_ATTR, SNS_ATTR};
      range3[0].has_flt = true;
      range3[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_125DPS, hi_range);
      range3[1].has_flt = true;
      range3[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_125DPS, hi_range);
      values[3].has_subtype = true;
      values[3].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[3].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range3, .buf_len = ARR_SIZE(range3) });

      sns_std_attr_value_data range4[] = {SNS_ATTR, SNS_ATTR};
      range4[0].has_flt = true;
      range4[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_250DPS, hi_range);
      range4[1].has_flt = true;
      range4[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_250DPS, hi_range);
      values[4].has_subtype = true;
      values[4].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[4].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range4, .buf_len = ARR_SIZE(range4) });
      sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
          values, ARR_SIZE(values), false);

      sns_std_attr_value_data range5[] = {SNS_ATTR, SNS_ATTR};
      range5[0].has_flt = true;
      range5[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_500DPS, hi_range);
      range5[1].has_flt = true;
      range5[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_500DPS, hi_range);
      values[5].has_subtype = true;
      values[5].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[5].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range5, .buf_len = ARR_SIZE(range5) });
      sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
          values, ARR_SIZE(values), false);

      sns_std_attr_value_data range6[] = {SNS_ATTR, SNS_ATTR};
      range6[0].has_flt = true;
      range6[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_1000DPS, hi_range);
      range6[1].has_flt = true;
      range6[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_1000DPS, hi_range);
      values[6].has_subtype = true;
      values[6].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[6].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range6, .buf_len = ARR_SIZE(range6) });
      sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
          values, ARR_SIZE(values), false);

      sns_std_attr_value_data range7[] = {SNS_ATTR, SNS_ATTR};
      range7[0].has_flt = true;
      range7[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_2000DPS, hi_range);
      range7[1].has_flt = true;
      range7[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_2000DPS, hi_range);
      values[7].has_subtype = true;
      values[7].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[7].subtype.values.arg =
        &((pb_buffer_arg){ .buf = range7, .buf_len = ARR_SIZE(range7) });
      sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
          values, ARR_SIZE(values), false);
    }

    {
      sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
      values[0].has_flt = true;
      values[0].flt = GYRO_RES_DPS(GYRO_RANGE_15_625DPS, hi_res, hi_range);
      values[1].has_flt = true;
      values[1].flt = GYRO_RES_DPS(GYRO_RANGE_31_25DPS, hi_res, hi_range);
      values[2].has_flt = true;
      values[2].flt = GYRO_RES_DPS(GYRO_RANGE_62_5DPS, hi_res, hi_range);
      values[3].has_flt = true;
      values[3].flt = GYRO_RES_DPS(GYRO_RANGE_125DPS, hi_res, hi_range);
      values[4].has_flt = true;
      values[4].flt = GYRO_RES_DPS(GYRO_RANGE_250DPS, hi_res, hi_range);
      values[5].has_flt = true;
      values[5].flt = GYRO_RES_DPS(GYRO_RANGE_500DPS, hi_res, hi_range);
      values[6].has_flt = true;
      values[6].flt = GYRO_RES_DPS(GYRO_RANGE_1000DPS, hi_res, hi_range);
      values[7].has_flt = true;
      values[7].flt = GYRO_RES_DPS(GYRO_RANGE_2000DPS, hi_res, hi_range);
      sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS,
          values, ARR_SIZE(values), false);
    }

    /** Only accel and gyro use registry information for selected range. */
    {
      sns_std_attr_value_data values[] = {SNS_ATTR};
      sns_std_attr_value_data rangeMinMax[] = {SNS_ATTR, SNS_ATTR};
      rangeMinMax[0].has_flt = true;
      rangeMinMax[0].flt = -1 * GYRO_RANGE_DPS(GYRO_RANGE_MAX - state->resolution_idx, hi_range);
      rangeMinMax[1].has_flt = true;
      rangeMinMax[1].flt = GYRO_RANGE_DPS(GYRO_RANGE_MAX - state->resolution_idx, hi_range);
      values[0].has_subtype = true;
      values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
      values[0].subtype.values.arg =
        &((pb_buffer_arg){ .buf = rangeMinMax, .buf_len = ARR_SIZE(rangeMinMax) });
      sns_publish_attribute(
          this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE, &values[0], ARR_SIZE(values), true);
    }

    {
      /** Only accel and gyro use registry information for selected resolution. */
      sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.has_flt = true;
      value.flt = GYRO_RES_DPS(GYRO_RANGE_MAX - state->resolution_idx, hi_res, hi_range);
      sns_publish_attribute(
          this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION, &value, 1, false);
    }

}

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void
icm4x6xx_publish_registry_attributes(sns_sensor *const this)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->is_dri;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->supports_sync_stream;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->hardware_id;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
      SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    for(uint8_t i = 0; i < 12; i++)
    {
      values[i].has_flt = true;
      values[i].flt = state->common.placement[i];
    }
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->common.registry_pf_cfg.rigid_body_type;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->common.registry_pf_cfg.rigid_body_type;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }
}

static void icm4x6xx_update_suid_to_accel_sensors(sns_sensor *const this)
{
    sns_sensor *sensor = NULL;
    icm4x6xx_state *state;
    icm4x6xx_state *acc_state = (icm4x6xx_state*)this->state->state;

    acc_state->common.a_suid = acc_state->my_suid;

    for (sensor = this->cb->get_library_sensor(this, true);
         sensor != NULL;
         sensor = this->cb->get_library_sensor(this, false)) {
        state = (icm4x6xx_state*)sensor->state->state;

        if (state->sensor == ICM4X6XX_GYRO) {
            acc_state->common.g_suid = state->my_suid;
        } else if (state->sensor == ICM4X6XX_MOTION_DETECT) {
            acc_state->common.md_suid = state->my_suid;
        } else if (state->sensor == ICM4X6XX_SENSOR_TEMP) {
            acc_state->common.st_suid = state->my_suid;
#ifdef ICM4X6XX_ENABLE_LOWG
        } else if (state->sensor == ICM4X6XX_FREE_FALL) {
            acc_state->common.free_fall_suid = state->my_suid;
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
        } else if (state->sensor == ICM4X6XX_HIGH_SHOCK) {
            acc_state->common.high_shock_suid = state->my_suid;
#endif
        }
    }
}

void icm4x6xx_update_sibling_sensors(sns_sensor *const this)
{
    sns_sensor *sensor = NULL;
    icm4x6xx_state *state;
    icm4x6xx_state *acc_state = (icm4x6xx_state*)this->state->state;

    icm4x6xx_update_suid_to_accel_sensors(this);

    for (sensor = this->cb->get_library_sensor(this, true);
         sensor != NULL;
         sensor = this->cb->get_library_sensor(this, false)) {
        state = (icm4x6xx_state*)sensor->state->state;

        if (state->sensor != ICM4X6XX_ACCEL) {
            sns_memscpy(&state->common, sizeof(state->common),
                      &acc_state->common, sizeof(acc_state->common));
            icm4x6xx_register_power_rail(sensor);
        }

        /** Moving registry based attribute publish here. */
        icm4x6xx_publish_registry_attributes(sensor);
        icm4x6xx_publish_a_range_res(sensor);
        icm4x6xx_publish_g_range_res(sensor);
        icm4x6xx_publish_ag_commom_attrs(sensor);
        icm4x6xx_publish_available(sensor);
        /** More clean up. */
        sns_sensor_util_remove_sensor_stream(sensor, &state->reg_data_stream);
    }
}

void icm4x6xx_start_hw_detect_sequence(sns_sensor *const this)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;

  state->common.registry_pf_cfg_received = false;

  /**-----------------Register and Open COM Port-------------------------*/
  if(NULL == state->common.com_port_info.port_handle)
  {
    rv = state->scp_service->api->sns_scp_register_com_port(
       &state->common.com_port_info.com_config,
       &state->common.com_port_info.port_handle);

    if(rv == SNS_RC_SUCCESS)
    {
      rv = state->scp_service->api->sns_scp_open(state->common.com_port_info.port_handle);
    }
    else
    {
      ICM4X6XX_PRINTF(ERROR, this, "sns_scp_register_com_port fail");
    }
  }
  if (NULL != state->pwr_rail_service)
  {
    ICM4X6XX_PRINTF(ERROR, this, "pwr_rail_service not NULL");
  }
  if (rv != SNS_RC_SUCCESS)
  {
    ICM4X6XX_PRINTF(ERROR, this, "sns_scp_open fail");
  }
  /**---------------------Register Power Rails --------------------------*/
  if(sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL)
     && NULL == state->pwr_rail_service
     && rv == SNS_RC_SUCCESS)
  {
    rv = icm4x6xx_register_power_rail(this);

    /**---------------------Turn Power Rails ON----------------------------*/
    state->common.rail_config.rail_vote = state->common.registry_rail_on_state;

    if(rv == SNS_RC_SUCCESS && NULL != state->pwr_rail_service)
    {
      rv = state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                               this,
                                                               &state->common.rail_config,
                                                               NULL);
    }

    /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
    if(rv == SNS_RC_SUCCESS)
    {
      icm4x6xx_start_power_rail_timer(this,
                                     sns_convert_ns_to_ticks(ICM4X6XX_OFF_TO_IDLE_MS * 1000 * 1000),
                                     ICM4X6XX_POWER_RAIL_PENDING_INIT);
    }
  }
}

void icm4x6xx_common_init(sns_sensor *const this)
{
  icm4x6xx_state *state = (icm4x6xx_state*)this->state->state;
  uint8_t i = 0;

  struct sns_service_manager *smgr= this->cb->get_service_manager(this);
  state->diag_service = (sns_diag_service *)
    smgr->get_service(smgr, SNS_DIAG_SERVICE);
  state->scp_service =
     (sns_sync_com_port_service *)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);
  state->island_service =
     (sns_island_service *)smgr->get_service(smgr, SNS_ISLAND_SERVICE);

  state->sensor_client_present = false;
  state->new_stream = false;

  if(state->sensor == ICM4X6XX_ACCEL
     || state->sensor == ICM4X6XX_GYRO)
  {
    // initialize axis conversion settings
    for(i = 0; i < TRIAXIS_NUM; i++)
    {
      state->common.axis_map[i].opaxis = i;
      state->common.axis_map[i].ipaxis = i;
      state->common.axis_map[i].invert = false;
    }
  }

  // initialize fac cal correction matrix to identity
  state->fac_cal_corr_mat.e00 = 1.0;
  state->fac_cal_corr_mat.e11 = 1.0;
  state->fac_cal_corr_mat.e22 = 1.0;

  SNS_SUID_LOOKUP_INIT(state->common.suid_lookup_data, NULL);

  if(state->sensor == ICM4X6XX_ACCEL)
  {
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "data_acquisition_engine");
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "interrupt");
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "async_com_port");
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "timer");
  }
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "registry");
}
