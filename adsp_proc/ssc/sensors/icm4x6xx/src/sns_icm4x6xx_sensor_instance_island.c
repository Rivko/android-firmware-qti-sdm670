/*
*
* Copyright @ 2014-2020 InvenSense Inc.  All rights reserved.
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
#include "sns_sensor_event.h"
#include "sns_event_service.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_types.h"
#include "sns_cal_util.h"
#include "sns_cal.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_interrupt.pb.h"
#include "sns_timer.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_sensor_util.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_icm4x6xx_hal.h"
#include "sns_icm4x6xx_sensor.h"
#include "sns_icm4x6xx_sensor_instance.h"

typedef struct log_sensor_state_raw_info {
    /* Pointer to diag service */
    sns_diag_service *diag;
    /* Pointer to sensor instance */
    sns_sensor_instance *instance;
    /* Pointer to sensor UID*/
    struct sns_sensor_uid *sensor_uid;
    /* Size of a single encoded sample */
    size_t encoded_sample_size;
    /* Pointer to log*/
    void *log;
} log_sensor_state_raw_info;

// Unencoded batch sample
typedef struct icm4x6xx_batch_sample {
    /* Batch Sample type as defined in sns_diag.pb.h */
    sns_diag_batch_sample_type sample_type;
    /* Timestamp of the sensor state data sample */
    sns_time timestamp;
    /*Raw sensor state data sample*/
    float sample[TRIAXIS_NUM];
    /* Data status.*/
    sns_std_sensor_sample_status status;
} icm4x6xx_batch_sample;

typedef struct icm4x6xx_batch_sample_one_axis {
    /* Batch Sample type as defined in sns_diag.pb.h */
    sns_diag_batch_sample_type sample_type;
    /* Timestamp of the sensor state data sample */
    sns_time timestamp;
    /*Raw sensor state data sample*/
    float sample[1];
    /* Data status.*/
    sns_std_sensor_sample_status status;
} icm4x6xx_batch_sample_one_axis;

static const icm4x6xx_odr_reg_map inv_reg_map[] = {
    {
        .odr = ICM4X6XX_ODR_0,
        .odr_reg_value = ODR_NOT_SUPPORTED,
        .a_discard_samples = 0,
        .g_discard_samples = 0,
    },
    {
        .odr = ICM4X6XX_ODR_1_5625,
        .odr_reg_value = ODR_1_5625HZ,
        .a_discard_samples = 1,
        .g_discard_samples = 1,
    },
    {
        .odr = ICM4X6XX_ODR_3_125,
        .odr_reg_value = ODR_3_125HZ,
        .a_discard_samples = 1,
        .g_discard_samples = 1,
    },
    {
        .odr = ICM4X6XX_ODR_6_25,
        .odr_reg_value = ODR_6_25HZ,
        .a_discard_samples = 1,
        .g_discard_samples = 1,
    },
    {
        .odr = ICM4X6XX_ODR_12_5,
        .odr_reg_value = ODR_12_5HZ,
        .a_discard_samples = 1,
        .g_discard_samples = 1,
    },
    {
        .odr = ICM4X6XX_ODR_25,
        .odr_reg_value = ODR_25HZ,
        .a_discard_samples = 1,
        .g_discard_samples = 2,
    },
    {
        .odr = ICM4X6XX_ODR_50,
        .odr_reg_value = ODR_50HZ,
        .a_discard_samples = 1,
        .g_discard_samples = 4,
    },
    {
        .odr = ICM4X6XX_ODR_100,
        .odr_reg_value = ODR_100HZ,
        .a_discard_samples = 1,
        .g_discard_samples = 8,
    },
    {
        .odr = ICM4X6XX_ODR_200,
        .odr_reg_value = ODR_200HZ,
        .a_discard_samples = 2,
        .g_discard_samples = 16,
    },
    {
        .odr = ICM4X6XX_ODR_500,
        .odr_reg_value = ODR_500HZ,
        .a_discard_samples = 5,
        .g_discard_samples = 30, // 60ms also engouh, for pass gyro-024
    },
    {
        .odr = ICM4X6XX_ODR_1000,
        .odr_reg_value = ODR_1KHZ,
        .a_discard_samples = 10,
        .g_discard_samples = 80,
    },
    {
        .odr = ICM4X6XX_ODR_2000,
        .odr_reg_value = ODR_2KHZ,
        .a_discard_samples = 20,
        .g_discard_samples = 160,
    },
};

/**
 * Encode Sensor State Log.Interrupt
 *
 * @param[i] log Pointer to log packet information
 * @param[i] log_size Size of log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of
 *                            the log
 * @param[o] encoded_log Pointer to location where encoded
 *                       log should be generated
 * @param[o] bytes_written Pointer to actual bytes written
 *       during encode
 *
 * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc icm4x6xx_encode_sensor_state_log_interrupt(
       void *log,
       size_t log_size,
       size_t encoded_log_size,
       void *encoded_log,
       size_t *bytes_written)
{
    UNUSED_VAR(log_size);
    sns_rc rc = SNS_RC_SUCCESS;

    if(NULL == encoded_log || NULL == log || NULL == bytes_written)
        return SNS_RC_FAILED;

    sns_diag_sensor_state_interrupt *sensor_state_interrupt =
                                (sns_diag_sensor_state_interrupt *)log;
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

    if(!pb_encode(&stream,
                  sns_diag_sensor_state_interrupt_fields,
                  sensor_state_interrupt))
        rc = SNS_RC_FAILED;

    if (SNS_RC_SUCCESS == rc)
        *bytes_written = stream.bytes_written;

    return rc;
}

/**
 * Encode log sensor state raw packet
 *
 * @param[i] log Pointer to log packet information
 * @param[i] log_size Size of log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of
 *                            the log
 * @param[o] encoded_log Pointer to location where encoded
 *                       log should be generated
 * @param[o] bytes_written Pointer to actual bytes written
 *       during encode
 *
 * @return sns_rc
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc icm4x6xx_encode_log_sensor_state_raw(
       void *log,
       size_t log_size,
       size_t encoded_log_size,
       void *encoded_log,
       size_t *bytes_written)
{
    sns_rc rc = SNS_RC_SUCCESS;
    uint32_t i = 0;
    size_t encoded_sample_size = 0;
    size_t parsed_log_size = 0;
    sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
    uint8_t arr_index = 0;
    float temp[TRIAXIS_NUM];
    pb_float_arr_arg arg = {
        .arr = (float *)temp,
        .arr_len = TRIAXIS_NUM,
        .arr_index = &arr_index
    };

    if (NULL == encoded_log || NULL == log || NULL ==bytes_written)
        return SNS_RC_FAILED;

    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg = &arg;

    if(!pb_get_encoded_size(&encoded_sample_size,
                            sns_diag_batch_sample_fields,
                            &batch_sample))
        return SNS_RC_FAILED;

    pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
    icm4x6xx_batch_sample *raw_sample = (icm4x6xx_batch_sample *)log;

    while (parsed_log_size < log_size &&
           (stream.bytes_written + encoded_sample_size) <= encoded_log_size &&
           i < (uint32_t)(-1)) {
        arr_index = 0;
        arg.arr = (float *)raw_sample[i].sample;

        batch_sample.sample_type = raw_sample[i].sample_type;
        batch_sample.status = raw_sample[i].status;
        batch_sample.timestamp = raw_sample[i].timestamp;

        if(!pb_encode_tag(&stream, PB_WT_STRING,
                          sns_diag_sensor_state_raw_sample_tag)) {
            rc = SNS_RC_FAILED;
            break;
        } else if(!pb_encode_delimited(&stream,
                                       sns_diag_batch_sample_fields,
                                       &batch_sample)) {
            rc = SNS_RC_FAILED;
            break;
        }

        parsed_log_size += sizeof(icm4x6xx_batch_sample);
        i++;
    }

    if (SNS_RC_SUCCESS == rc)
        *bytes_written = stream.bytes_written;

    return rc;
}

sns_rc icm4x6xx_encode_log_sensor_state_raw_one_axis(
       void *log,
       size_t log_size,
       size_t encoded_log_size,
       void *encoded_log,
       size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[1];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = 1,
    .arr_index = &arr_index};

  if(NULL == encoded_log || NULL == log || NULL == bytes_written)
  {
    return SNS_RC_FAILED;
  }

  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;

  if(!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields,
                          &batch_sample))
  {
    return SNS_RC_FAILED;
  }

  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
  icm4x6xx_batch_sample_one_axis *raw_sample = (icm4x6xx_batch_sample_one_axis *)log;

  while(parsed_log_size < log_size &&
        (stream.bytes_written + encoded_sample_size)<= encoded_log_size &&
        i < (uint32_t)(-1))
  {
    arr_index = 0;
    arg.arr = (float *)raw_sample[i].sample;

    batch_sample.sample_type = raw_sample[i].sample_type;
    batch_sample.status = raw_sample[i].status;
    batch_sample.timestamp = raw_sample[i].timestamp;

    if(!pb_encode_tag(&stream, PB_WT_STRING,
                      sns_diag_sensor_state_raw_sample_tag))
    {
      rc = SNS_RC_FAILED;
      break;
    }
    else if(!pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                                 &batch_sample))
    {
      rc = SNS_RC_FAILED;
      break;
    }

    parsed_log_size += sizeof(icm4x6xx_batch_sample_one_axis);
    i++;
  }

  if (SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }

  return rc;
}

sns_rc icm4x6xx_log_sensor_state_raw_three_axis(
       sns_sensor_instance *instance,
       icm4x6xx_sensor_type sensor_type,
       bool first_sample_in_batch,
       bool last_sample_in_batch,
       float *raw_data,
       sns_time timestamp,
       sns_std_sensor_sample_status status)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state *)instance->state->state;
    sns_diag_service* diag = state->diag_service;
    log_sensor_state_raw_info log_state_raw_info;
    sns_sensor_uid suid = (sensor_type == ICM4X6XX_ACCEL) ?
                state->accel_info.suid : state->gyro_info.suid;
    uint32_t max_log_size = 0;
    uint32_t log_size = sizeof(icm4x6xx_batch_sample);

    sns_memzero(&log_state_raw_info, sizeof(log_state_raw_info));
    log_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
    log_state_raw_info.diag = diag;
    log_state_raw_info.instance = instance;
    log_state_raw_info.sensor_uid = &suid;

    if (NULL == log_state_raw_info.diag)
        return SNS_RC_FAILED;

    // allocate memory for sensor state - raw sensor log packet
    max_log_size = log_state_raw_info.diag->api->get_max_log_size(
                                   log_state_raw_info.diag);

    if (log_size > max_log_size) {
        return SNS_RC_FAILED;
    }

    log_state_raw_info.log = log_state_raw_info.diag->api->alloc_log(
                                        log_state_raw_info.diag,
                                        log_state_raw_info.instance,
                                        log_state_raw_info.sensor_uid,
                                        log_size,
                                        SNS_DIAG_SENSOR_STATE_LOG_RAW);

    if (NULL == log_state_raw_info.log)
        return SNS_RC_FAILED;

    icm4x6xx_batch_sample *sample =
            (icm4x6xx_batch_sample *)log_state_raw_info.log;

    if (first_sample_in_batch && last_sample_in_batch) {
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;
    } else if (!first_sample_in_batch && !last_sample_in_batch) {
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
    } else if (first_sample_in_batch) {
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
    } else if (last_sample_in_batch) {
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_LAST;
    }

    sample->timestamp = timestamp;

    sns_memscpy(sample->sample, sizeof(sample->sample),
                raw_data, sizeof(sample->sample));
    sample->status = status;

    log_state_raw_info.diag->api->submit_log(
        log_state_raw_info.diag,
        log_state_raw_info.instance,
        log_state_raw_info.sensor_uid,
        log_size,
        log_state_raw_info.log,
        SNS_DIAG_SENSOR_STATE_LOG_RAW,
        log_state_raw_info.encoded_sample_size,
        icm4x6xx_encode_log_sensor_state_raw);

    return SNS_RC_SUCCESS;
}

sns_rc icm4x6xx_log_sensor_state_raw_one_axis(
       sns_sensor_instance *instance,
       bool first_sample_in_batch,
       bool last_sample_in_batch,
       float *raw_data,
       sns_time timestamp,
       sns_std_sensor_sample_status status)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state *)instance->state->state;
    sns_diag_service* diag = state->diag_service;
    log_sensor_state_raw_info log_state_raw_info;
    sns_sensor_uid suid = state->sensor_temp_info.suid;
    uint32_t max_log_size = 0;
    uint32_t log_size = sizeof(icm4x6xx_batch_sample_one_axis);

    sns_memzero(&log_state_raw_info, sizeof(log_state_raw_info));
    log_state_raw_info.encoded_sample_size = state->log_temp_raw_encoded_size;
    log_state_raw_info.diag = diag;
    log_state_raw_info.instance = instance;
    log_state_raw_info.sensor_uid = &suid;

    if (NULL == log_state_raw_info.diag)
        return SNS_RC_FAILED;

    // allocate memory for sensor state - raw sensor log packet
    max_log_size = log_state_raw_info.diag->api->get_max_log_size(
                                   log_state_raw_info.diag);

    if (log_size > max_log_size) {
        return SNS_RC_FAILED;
    }

    log_state_raw_info.log = log_state_raw_info.diag->api->alloc_log(
                                        log_state_raw_info.diag,
                                        log_state_raw_info.instance,
                                        log_state_raw_info.sensor_uid,
                                        log_size,
                                        SNS_DIAG_SENSOR_STATE_LOG_RAW);

    if (NULL == log_state_raw_info.log)
        return SNS_RC_FAILED;

    icm4x6xx_batch_sample_one_axis *sample =
            (icm4x6xx_batch_sample_one_axis *)log_state_raw_info.log;

    if (first_sample_in_batch && last_sample_in_batch) {
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;
    } else if (!first_sample_in_batch && !last_sample_in_batch) {
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
    } else if (first_sample_in_batch) {
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
    } else if (last_sample_in_batch) {
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_LAST;
    }

    sample->timestamp = timestamp;

    sns_memscpy(sample->sample, sizeof(sample->sample),
                raw_data, sizeof(sample->sample));
    sample->status = status;

    log_state_raw_info.diag->api->submit_log(
        log_state_raw_info.diag,
        log_state_raw_info.instance,
        log_state_raw_info.sensor_uid,
        log_size,
        log_state_raw_info.log,
        SNS_DIAG_SENSOR_STATE_LOG_RAW,
        log_state_raw_info.encoded_sample_size,
        icm4x6xx_encode_log_sensor_state_raw_one_axis);

    return SNS_RC_SUCCESS;
}

/**
 * Send Calibration event to client
 *
 * @param[i] instance        instance reference
 * @param[i] sensor          bit mask for sensors to handle
 * @return none
 */
void icm4x6xx_send_cal_event(sns_sensor_instance *const instance, icm4x6xx_sensor_type sensor_type)
{
  icm4x6xx_instance_state *state =
     (icm4x6xx_instance_state*)instance->state->state;

  sns_cal_event new_calibration_event = sns_cal_event_init_default;
  float bias_data[] = {0,0,0};
  float comp_matrix_data[] = {0,0,0,0,0,0,0,0,0};
  sns_icm4x6xx_registry_cfg *reg_cfg;
  sns_time cal_event_ts = sns_get_system_time();
  bool is_new_bias = false;

  if(sensor_type == ICM4X6XX_ACCEL) {
    reg_cfg = &state->accel_registry_cfg;
    if(0 == state->accel_info.last_cal_info.timestamp)
      state->accel_info.last_cal_info.timestamp = cal_event_ts;

    if((reg_cfg->fac_cal_bias[0] == state->accel_info.last_cal_info.bias[0]) &&
       (reg_cfg->fac_cal_bias[1] == state->accel_info.last_cal_info.bias[1]) &&
       (reg_cfg->fac_cal_bias[2] == state->accel_info.last_cal_info.bias[2])) {
        /* latch last cal event ts due to bias not change */
        cal_event_ts = state->accel_info.last_cal_info.timestamp;
    } else {
        /* update bias and ts to state struct when bias changed */
        sns_memscpy(state->accel_info.last_cal_info.bias, sizeof(state->accel_info.last_cal_info.bias),
                reg_cfg->fac_cal_bias, sizeof(reg_cfg->fac_cal_bias));
        state->accel_info.last_cal_info.timestamp = cal_event_ts;
        is_new_bias = true;
    }
  } else {
    reg_cfg = &state->gyro_registry_cfg;
    if(0 == state->gyro_info.last_cal_info.timestamp)
      state->gyro_info.last_cal_info.timestamp = cal_event_ts;

    if((reg_cfg->fac_cal_bias[0] == state->gyro_info.last_cal_info.bias[0]) &&
       (reg_cfg->fac_cal_bias[1] == state->gyro_info.last_cal_info.bias[1]) &&
       (reg_cfg->fac_cal_bias[2] == state->gyro_info.last_cal_info.bias[2])) {
        cal_event_ts = state->gyro_info.last_cal_info.timestamp;
    } else {
        sns_memscpy(state->gyro_info.last_cal_info.bias, sizeof(state->gyro_info.last_cal_info.bias),
                reg_cfg->fac_cal_bias, sizeof(reg_cfg->fac_cal_bias));
        state->gyro_info.last_cal_info.timestamp = cal_event_ts;
        is_new_bias = true;
    }
  }

  sns_memscpy(bias_data, sizeof(bias_data),
              reg_cfg->fac_cal_bias, sizeof(reg_cfg->fac_cal_bias));
  sns_memscpy(comp_matrix_data, sizeof(comp_matrix_data),
              reg_cfg->fac_cal_corr_mat.data, sizeof(reg_cfg->fac_cal_corr_mat.data));

  pb_buffer_arg buff_arg_bias = (pb_buffer_arg)
      { .buf = &bias_data, .buf_len = ARR_SIZE(bias_data) };
  pb_buffer_arg buff_arg_comp_matrix = (pb_buffer_arg)
      { .buf = &comp_matrix_data, .buf_len = ARR_SIZE(comp_matrix_data) };
  sns_sensor_uid *suid_current;

  //update suid
  if (sensor_type == ICM4X6XX_ACCEL)
    suid_current = &state->accel_info.suid;
  else
    suid_current = &state->gyro_info.suid;

  new_calibration_event.bias.funcs.encode = &pb_encode_float_arr_cb;
  new_calibration_event.bias.arg = &buff_arg_bias;
  new_calibration_event.comp_matrix.funcs.encode = &pb_encode_float_arr_cb;
  new_calibration_event.comp_matrix.arg = &buff_arg_comp_matrix;
  new_calibration_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  ICM4X6XX_INST_PRINTF(MED, instance, "(0x%x)Cal event, new %d, ts %x%08x", sensor_type, is_new_bias,
            (uint32_t)(cal_event_ts >> 32), (uint32_t)cal_event_ts);
  pb_send_event(instance,
                sns_cal_event_fields,
                &new_calibration_event,
                cal_event_ts,
                SNS_CAL_MSGID_SNS_CAL_EVENT,
                suid_current);
}

/**
 * Reset Calibration values
 *
 * @param[i] instance        instance reference
 * @param[i] sensor          bit mask for sensors to handle
 * @return none
 */
void icm4x6xx_reset_cal_data(sns_sensor_instance *const instance, icm4x6xx_sensor_type sensor_type)
{
  icm4x6xx_instance_state *state =
     (icm4x6xx_instance_state*)instance->state->state;
  float bias_data[] = {0,0,0};
  float comp_matrix_data[] = {1,0,0,0,1,0,0,0,1};
  sns_icm4x6xx_registry_cfg *reg_cfg;

  if(sensor_type == ICM4X6XX_ACCEL) {
    reg_cfg = &state->accel_registry_cfg;
  } else {
    reg_cfg = &state->gyro_registry_cfg;
  }

  sns_memscpy(reg_cfg->fac_cal_bias, sizeof(reg_cfg->fac_cal_bias),
              bias_data, sizeof(bias_data));
  sns_memscpy(reg_cfg->fac_cal_corr_mat.data, sizeof(reg_cfg->fac_cal_corr_mat.data),
              comp_matrix_data, sizeof(comp_matrix_data));
  reg_cfg->version++;
}

void icm4x6xx_send_config_event(sns_sensor_instance *const instance)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    bool hi_res = state->product_info.use_hi_res;
    bool hi_range = state->product_info.hi_range;

    sns_std_sensor_physical_config_event phy_sensor_config =
        sns_std_sensor_physical_config_event_init_default;

    char operating_mode_normal[] = ICM4X6XX_NORMAL;
    char operating_mode_lpm[] = ICM4X6XX_LPM;
    char operating_mode_hpf[] = ICM4X6XX_HIGH_PERF;
    pb_buffer_arg op_mode_args;

    /* If no sensors are enabled for streaming then don't send
     *  config event */
    if(!state->accel_info.is_enabled && !state->gyro_info.is_enabled)
        return;

    ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "send_config_event");

    op_mode_args.buf = &operating_mode_hpf[0];
    op_mode_args.buf_len = sizeof(operating_mode_hpf);

    phy_sensor_config.has_sample_rate = true;
#ifdef ICM4X6XX_1K_2_500HZ
    if (state->do_1K_2_500_dec && FLT_EQUAL(state->accel_info.cur_odr, ICM4X6XX_ODR_1000)) {
        phy_sensor_config.sample_rate = state->accel_info.is_enabled ?
                                         ICM4X6XX_ODR_500 : 0;
    } else
#endif
    {
        phy_sensor_config.sample_rate = state->accel_info.is_enabled ?
                                         state->accel_info.cur_odr : 0;
    }

    phy_sensor_config.has_water_mark = true;
#ifdef ICM4X6XX_1K_2_500HZ
    if (state->do_1K_2_500_dec && FLT_EQUAL(state->accel_info.cur_odr, ICM4X6XX_ODR_1000)) {
        phy_sensor_config.water_mark = state->fifo_info.cur_wmk / 2;
    } else
#endif
    {
        phy_sensor_config.water_mark = state->fifo_info.cur_wmk;
    }

    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = 240;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = ACC_RES_MG_LSB(state->accel_info.fsr, hi_res, hi_range);
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = -1 * ACC_RANGE_UNIT_G(state->accel_info.fsr, hi_range);
    phy_sensor_config.range[1] = ACC_RANGE_UNIT_G(state->accel_info.fsr, hi_range);
    phy_sensor_config.has_stream_is_synchronous = true;
    phy_sensor_config.stream_is_synchronous = false;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = true;
#ifndef ICM4X6XX_DISABLE_DAE
    /* For sensors that route data through the SDC/DAE sensor, the DAE watermark
     * should be set to the number of samples stored in SDC before waking up Q6. */
    if (state->dae_if.ag.stream_usable) {
        phy_sensor_config.has_DAE_watermark = true;
        phy_sensor_config.DAE_watermark =
            (state->fifo_info.max_requested_flush_ticks == 0) ?
            UINT32_MAX :
            state->fifo_info.cur_max_wmk;
    } else {
        phy_sensor_config.has_DAE_watermark = false;
        phy_sensor_config.DAE_watermark = 0;
    }
#else
    phy_sensor_config.has_DAE_watermark = false;
    phy_sensor_config.DAE_watermark = 0;
#endif

    if ((state->accel_info.actual_publish && state->accel_info.desire_publish) ||
         state->accel_info.gated_client_present) {
        ICM4X6XX_INST_PRINTF(LOW, instance, "g send_config_event, new:%d, sr %d:%d, wmk %d:%d", state->a_new_stream,
        (uint32_t)state->accel_info.last_sent_cfg.sample_rate, (uint32_t)phy_sensor_config.sample_rate,
        (uint32_t)state->accel_info.last_sent_cfg.wmk, (uint32_t)phy_sensor_config.water_mark);
        sns_time cfg_event_ts = sns_get_system_time();
        if((state->accel_info.last_sent_cfg.sample_rate == phy_sensor_config.sample_rate)
            && (state->accel_info.last_sent_cfg.wmk == phy_sensor_config.water_mark)) {
            /* latch ts for old stream and config not changed */
            cfg_event_ts = state->accel_info.last_sent_cfg.timestamp;
        }

        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      cfg_event_ts,
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->accel_info.suid);
        ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "a send_config_event, ts %x%08x",
            (uint32_t)(cfg_event_ts >> 32), (uint32_t)cfg_event_ts);

        state->accel_info.last_sent_cfg.timestamp = cfg_event_ts;
        state->accel_info.last_sent_cfg.wmk = phy_sensor_config.water_mark;
        state->accel_info.last_sent_cfg.sample_rate = phy_sensor_config.sample_rate;

        if (state->a_new_stream) {
            icm4x6xx_send_cal_event(instance, ICM4X6XX_ACCEL);
            state->a_new_stream = false;
        }
    }

    if(state->gyro_info.actual_publish && state->gyro_info.desire_publish) {
        // Override above values with gyro info
        phy_sensor_config.has_sample_rate = true;
#ifdef ICM4X6XX_1K_2_500HZ
            if (state->do_1K_2_500_dec && FLT_EQUAL(state->accel_info.cur_odr, ICM4X6XX_ODR_1000)) {
                phy_sensor_config.sample_rate = state->gyro_info.is_enabled ?
                                                 ICM4X6XX_ODR_500 : 0;
            } else
#endif
            {
                phy_sensor_config.sample_rate = state->gyro_info.is_enabled ?
                                                 state->gyro_info.cur_odr : 0;
            }

        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = 570;
        phy_sensor_config.has_resolution = true;
        phy_sensor_config.resolution = GYRO_RES_DPS(state->gyro_info.fsr, hi_res, hi_range);
        phy_sensor_config.range_count = 2;
        phy_sensor_config.range[0] = -1 * GYRO_RANGE_DPS(state->gyro_info.fsr, hi_range);
        phy_sensor_config.range[1] = GYRO_RANGE_DPS(state->gyro_info.fsr, hi_range);

        ICM4X6XX_INST_PRINTF(LOW, instance, "g send_config_event, new:%d, sr %d:%d, wmk %d:%d", state->g_new_stream,
        (uint32_t)state->gyro_info.last_sent_cfg.sample_rate, (uint32_t)phy_sensor_config.sample_rate,
        (uint32_t)state->gyro_info.last_sent_cfg.wmk, (uint32_t)phy_sensor_config.water_mark);
        sns_time cfg_event_ts = sns_get_system_time();
        if((state->gyro_info.last_sent_cfg.sample_rate == phy_sensor_config.sample_rate)
            && (state->gyro_info.last_sent_cfg.wmk == phy_sensor_config.water_mark)) {
            /* latch ts for old stream and config not changed */
            cfg_event_ts = state->gyro_info.last_sent_cfg.timestamp;
        }

        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      cfg_event_ts,
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->gyro_info.suid);
        ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "g send_config_event, ts %x%08x",
            (uint32_t)(cfg_event_ts >> 32), (uint32_t)cfg_event_ts);

        state->gyro_info.last_sent_cfg.timestamp = cfg_event_ts;
        state->gyro_info.last_sent_cfg.wmk = phy_sensor_config.water_mark;
        state->gyro_info.last_sent_cfg.sample_rate = phy_sensor_config.sample_rate;

        if (state->g_new_stream) {
            icm4x6xx_send_cal_event(instance, ICM4X6XX_GYRO);
            state->g_new_stream = false;
        }
    }

    if(state->sensor_temp_info.actual_publish) {
        // Override above values with sensor temperature info
        phy_sensor_config.sample_rate = state->sensor_temp_info.sampling_rate_hz; // TODO CHECK
        phy_sensor_config.has_water_mark = false;
        phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
        phy_sensor_config.operation_mode.arg = &op_mode_args;
        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = 240;
        phy_sensor_config.has_resolution = true;
        phy_sensor_config.resolution = ICM4X6XX_SENSOR_TEMPERATURE_RESOLUTION;
        phy_sensor_config.range_count = 2;
        phy_sensor_config.range[0] = ICM4X6XX_SENSOR_TEMPERATURE_RANGE_MIN;
        phy_sensor_config.range[1] = ICM4X6XX_SENSOR_TEMPERATURE_RANGE_MAX;
        phy_sensor_config.has_dri_enabled = true;
        phy_sensor_config.dri_enabled = false;
#ifndef ICM4X6XX_DISABLE_DAE
        if (state->dae_if.temp.stream_usable) {
            phy_sensor_config.has_DAE_watermark = true;
            phy_sensor_config.DAE_watermark =
                (state->sensor_temp_info.max_requested_flush_ticks == 0) ?
                UINT32_MAX :
                (state->sensor_temp_info.sampling_rate_hz / state->sensor_temp_info.report_rate_hz);
        } else {
            phy_sensor_config.has_DAE_watermark = false;
            phy_sensor_config.DAE_watermark = 0;
        }
#else
        phy_sensor_config.has_DAE_watermark = false;
        phy_sensor_config.DAE_watermark = 0;
#endif

        ICM4X6XX_INST_PRINTF(LOW, instance, "t send_config_event, sr %d:%d",
        state->sensor_temp_info.last_sent_cfg.sample_rate, phy_sensor_config.sample_rate);
        sns_time cfg_event_ts = sns_get_system_time();
        if(state->sensor_temp_info.last_sent_cfg.sample_rate == phy_sensor_config.sample_rate) {
            /* latch ts for old stream and config not changed */
            cfg_event_ts = state->sensor_temp_info.last_sent_cfg.timestamp;
        }
        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      cfg_event_ts,
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->sensor_temp_info.suid);
        state->sensor_temp_info.last_sent_cfg.timestamp = cfg_event_ts;
        state->sensor_temp_info.last_sent_cfg.sample_rate = phy_sensor_config.sample_rate;
    }

    if (state->md_info.md_client_present) {
        if (state->gyro_info.is_enabled) {
            op_mode_args.buf = &operating_mode_normal[0];
            op_mode_args.buf_len = sizeof(operating_mode_normal);
        } else {
            op_mode_args.buf = &operating_mode_lpm[0];
            op_mode_args.buf_len = sizeof(operating_mode_lpm);
        }

        phy_sensor_config.has_sample_rate = false;
        phy_sensor_config.has_water_mark = false;
        phy_sensor_config.range_count = 0;
        phy_sensor_config.has_resolution = false;
        phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
        phy_sensor_config.operation_mode.arg = &op_mode_args;
        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = 50; //50uA
        phy_sensor_config.has_stream_is_synchronous = false;
        phy_sensor_config.has_dri_enabled = false;
        phy_sensor_config.has_DAE_watermark = false;
        phy_sensor_config.has_sync_ts_anchor = false;

        ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "md send_config_event");

        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      sns_get_system_time(),
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->md_info.suid);

    }

#ifdef ICM4X6XX_ENABLE_LOWG
    if (state->freefall_info.free_fall_client_present && state->freefall_info.enable_free_fall) {
        op_mode_args.buf = &operating_mode_lpm[0];
        op_mode_args.buf_len = sizeof(operating_mode_lpm);

        phy_sensor_config.has_sample_rate = false;
        phy_sensor_config.has_water_mark = false;
        phy_sensor_config.range_count = 0;
        phy_sensor_config.has_resolution = false;
        phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
        phy_sensor_config.operation_mode.arg = &op_mode_args;
        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = 50;
        phy_sensor_config.has_stream_is_synchronous = false;
        phy_sensor_config.has_dri_enabled = false;
        phy_sensor_config.has_DAE_watermark = false;
        phy_sensor_config.has_sync_ts_anchor = false;

        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      sns_get_system_time(),
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->freefall_info.suid);

    }
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
    if (state->high_shock_info.high_shock_client_present && state->high_shock_info.enable_high_shock) {
        op_mode_args.buf = &operating_mode_lpm[0];
        op_mode_args.buf_len = sizeof(operating_mode_lpm);

        phy_sensor_config.has_sample_rate = false;
        phy_sensor_config.has_water_mark = false;
        phy_sensor_config.range_count = 0;
        phy_sensor_config.has_resolution = false;
        phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
        phy_sensor_config.operation_mode.arg = &op_mode_args;
        phy_sensor_config.has_active_current = true;
        phy_sensor_config.active_current = 50;
        phy_sensor_config.has_stream_is_synchronous = false;
        phy_sensor_config.has_dri_enabled = false;
        phy_sensor_config.has_DAE_watermark = false;
        phy_sensor_config.has_sync_ts_anchor = false;

        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      sns_get_system_time(),
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->high_shock_info.suid);

    }
#endif
}

void icm4x6xx_send_int_clear_msg(sns_sensor_instance *const this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_data_stream* data_stream = state->interrupt_data_stream;
    sns_request irq_msg = {
        .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_IS_CLEARED,
        .request    = NULL
    };

#ifdef ICM4X6XX_ENABLE_I3C
    if (state->com_port_info.com_config.bus_type == SNS_BUS_I3C_SDR &&
        !state->product_info.i3c_use_int1) {
        return;
    }
#endif

    if (state->irq_info.irq_config.interrupt_trigger_type == SNS_INTERRUPT_TRIGGER_TYPE_HIGH ||
        state->irq_info.irq_config.interrupt_trigger_type == SNS_INTERRUPT_TRIGGER_TYPE_LOW) {
        data_stream->api->send_request(data_stream, &irq_msg);
        ICM4X6XX_VERB_INST_PRINTF(MED, this, "send clear msg");
    } else {
        ICM4X6XX_VERB_INST_PRINTF(MED, this, "trigger type is edge, don't send clear msg");
    }
}

#ifdef ICM4X6XX_DISABLE_DAE
void icm4x6xx_send_ascp_read_req(sns_sensor_instance *const this, uint8_t reg, uint32_t len)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_port_vector async_read_msg;
    uint8_t buffer[100];
    uint32_t enc_len;

    async_read_msg.bytes = len;
    async_read_msg.reg_addr = reg;
    async_read_msg.is_write = false;
    async_read_msg.buffer = NULL;

    state->ascp_request_cnt++;
    if (state->ascp_request_cnt > 100)
        ICM4X6XX_INST_PRINTF(HIGH, this,
            "unhandled ascp cnt %d", state->ascp_request_cnt); // too much unhandled ascp request

    sns_ascp_create_encoded_vectors_buffer(&async_read_msg, 1, true, buffer, sizeof(buffer), &enc_len);
    // Send message to Async Com Port
    sns_request async_com_port_request = (sns_request) {
        .message_id = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW,
        .request_len = enc_len,
        .request = buffer
    };
    state->async_com_port_data_stream->api->send_request(
                  state->async_com_port_data_stream, &async_com_port_request);
}
#endif

sns_rc icm4x6xx_get_best_match_odr(
    icm4x6xx_instance_state *state,
    float desired_sample_rate,
    float *chosen_sample_rate,
    icm4x6xx_sensor_odr *chosen_reg_value,
    uint8_t *a_samples_to_discard,
    uint8_t *g_samples_to_discard)
{
    uint8_t idx;

    if(ICM4X6XX_ODR_2000 < desired_sample_rate)
        return SNS_RC_NOT_SUPPORTED;

    for(idx = 0; idx < ARR_SIZE(inv_reg_map); idx++) {
#ifndef ICM4X6XX_DISABLE_DAE
        /* For DAE, the minim ODR should be big than 20Hz */
        if (FLT_EQUAL(inv_reg_map[idx].odr, ICM4X6XX_ODR_12_5)) {
            continue;
        }
#endif

        /* some chip doesn't support 12.5HZ odr,
         * skip this odr this kind chip */
        if ((HAVANA == state->product_info.series) &&
            FLT_EQUAL(inv_reg_map[idx].odr, ICM4X6XX_ODR_12_5)) {
            continue;
        }

        /* some chip doesn't support 6.25HZ odr,
         * skip this odr this kind chip */
        if (FLT_EQUAL(inv_reg_map[idx].odr, ICM4X6XX_ODR_6_25)) {
            continue;
        }

        /* some chip doesn't support 3.125HZ odr,
         * skip this odr this kind chip */
        if (FLT_EQUAL(inv_reg_map[idx].odr, ICM4X6XX_ODR_3_125)) {
            continue;
        }

        /* some chip doesn't support 1.5625HZ odr,
         * skip this odr this kind chip */
        if (FLT_EQUAL(inv_reg_map[idx].odr, ICM4X6XX_ODR_1_5625)) {
            continue;
        }

        if(desired_sample_rate <= inv_reg_map[idx].odr)
            break;
    }

    if (idx >= ARR_SIZE(inv_reg_map))
        return SNS_RC_NOT_SUPPORTED;

#ifdef  ICM4X6XX_1K_2_500HZ
    /* Havana didn't have 500Hz, use 1K to 500 decimation */
    if ((HAVANA == state->product_info.series) && (idx == 9)) {
        idx = 10;
        state->do_1K_2_500_dec = true;
    } else {
        state->do_1K_2_500_dec = false;
    }
#endif

    *chosen_sample_rate = inv_reg_map[idx].odr;
    *chosen_reg_value = inv_reg_map[idx].odr_reg_value;
    *a_samples_to_discard = inv_reg_map[idx].a_discard_samples;
    *g_samples_to_discard = inv_reg_map[idx].g_discard_samples;

    /* For both Yokohama and Havana testAccelerometer_1hz */
    if (FLT_EQUAL(desired_sample_rate, 1.0))
        *a_samples_to_discard += 10;

    /* Only for Yokohama testAccelerometer_25hz */
    if ((YOKOHAMA == state->product_info.series) && FLT_EQUAL(desired_sample_rate, 25.0))
        *a_samples_to_discard += 1;

    /* Only for Yokohama testAccelerometer_15hz */
    if ((YOKOHAMA == state->product_info.series) && FLT_EQUAL(desired_sample_rate, 15.0))
        *a_samples_to_discard += 1;

    /* Only for Yokohama testAccelerometer_10hz */
    if ((YOKOHAMA == state->product_info.series) && FLT_EQUAL(desired_sample_rate, 10.0))
        *a_samples_to_discard += 3;

    /* Only for Yokohama testAccelerometer_5hz */
    if ((YOKOHAMA == state->product_info.series) && FLT_EQUAL(desired_sample_rate, 5.0))
        *a_samples_to_discard += 7;

    /* H_W_B 9470 */
    if (HAVANA == state->product_info.series)
        *a_samples_to_discard += 3;

    return SNS_RC_SUCCESS;
}

/**
 * dae wmk(max_requested_wmk) must be the multiple of sensor HW wmk
 */
static uint16_t icm4x6xx_get_best_watermark(
                sns_sensor_instance *this,
                uint16_t max_hw_wmk,
                uint32_t desired_wmk)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    uint32_t best_wmk = desired_wmk;
    uint32_t divide = 1;

    // desire_wmk shold be same with state->fifo_info.max_requested_wmk, checking FLUSH ONLY CASE
#ifdef ICM4X6XX_DISABLE_DAE
    if (max_hw_wmk <= desired_wmk)
        return max_hw_wmk;
    else
        return desired_wmk;
#endif

    if (max_hw_wmk >= desired_wmk) {
        if (state->fifo_info.max_requested_wmk > desired_wmk) {
            /* dae wmk must be the multiple of sensor HW wmk, except when dae wmk equal UINT32_MAX,
               fixed Acc-028/029/030/031/042/043/048/049/050/051/052 test error of acceptance v71.6*/
            if(UINT32_MAX != state->fifo_info.max_requested_wmk)
                state->fifo_info.max_requested_wmk = (state->fifo_info.max_requested_wmk / desired_wmk) * desired_wmk;
        }
        return desired_wmk;
    }

    /* now use max_requested_wmk to cal ideal device watermark */
    if (desired_wmk != state->fifo_info.max_requested_wmk)
        desired_wmk = state->fifo_info.max_requested_wmk;

    if ((desired_wmk % max_hw_wmk) == 0)
        return max_hw_wmk;
    else {
        divide = desired_wmk / max_hw_wmk + 1;
        best_wmk = desired_wmk / divide;
        while (desired_wmk % best_wmk != 0) {
            best_wmk -= 1; // max loop count is FIFO size
        }
    }

    return best_wmk;
}

void icm4x6xx_update_md_intr(
     sns_sensor_instance *const instance,
     bool enable,
     bool md_not_armed_event)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;

    icm4x6xx_en_wom_int(instance, enable);
    state->md_info.md_state.motion_detect_event_type =
        enable ? SNS_MOTION_DETECT_EVENT_TYPE_ENABLED : SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;

    if(enable || md_not_armed_event) {
        if (state->md_info.pre_md_state.motion_detect_event_type !=
            state->md_info.md_state.motion_detect_event_type) {
            pb_send_event(instance,
                          sns_motion_detect_event_fields,
                          &state->md_info.md_state,
                          sns_get_system_time(),
                          SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                          &state->md_info.suid);
            ICM4X6XX_INST_PRINTF(HIGH, instance, "md_armed=%d",
                                      state->md_info.md_state.motion_detect_event_type);
        }
    }
    state->md_info.pre_md_state.motion_detect_event_type =
        state->md_info.md_state.motion_detect_event_type;
}

#ifdef ICM4X6XX_ENABLE_LOWG
void icm4x6xx_update_free_fall_status(
     sns_sensor_instance *const instance,
     bool enable)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;

    state->freefall_info.free_fall_state.free_fall_event_type =
        enable ? SNS_FREE_FALL_EVENT_TYPE_ENABLED : SNS_FREE_FALL_EVENT_TYPE_DISABLED;

    pb_send_event(instance,
                  sns_free_fall_event_fields,
                  &state->freefall_info.free_fall_state,
                  sns_get_system_time(),
                  SNS_FREE_FALL_MSGID_SNS_FREE_FALL_EVENT,
                  &state->freefall_info.suid);
    ICM4X6XX_INST_PRINTF(HIGH, instance, "freefall_armed=%d",
                           state->freefall_info.free_fall_state.free_fall_event_type);
}
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
void icm4x6xx_update_high_shock_status(
     sns_sensor_instance *const instance,
     bool enable)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;

    state->high_shock_info.high_shock_state.high_shock_event_type =
        enable ? SNS_HIGH_SHOCK_EVENT_TYPE_ENABLED : SNS_HIGH_SHOCK_EVENT_TYPE_DISABLED;

    pb_send_event(instance,
                  sns_high_shock_event_fields,
                  &state->high_shock_info.high_shock_state,
                  sns_get_system_time(),
                  SNS_HIGH_SHOCK_MSGID_SNS_HIGH_SHOCK_EVENT,
                  &state->high_shock_info.suid);
    ICM4X6XX_INST_PRINTF(HIGH, instance, "highshock_armed=%d",
                           state->high_shock_info.high_shock_state.high_shock_event_type);
}
#endif

void icm4x6xx_convert_and_send_temp_sample(
     sns_sensor_instance *const instance,
     sns_time timestamp,
     int16_t temp_data)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    float temp_Celsius = (temp_data * ICM4X6XX_SENSOR_TEMPERATURE_RESOLUTION) + 25.0;
    sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

    if (HAVANA == state->product_info.series) {
        temp_Celsius = (temp_data * ICM4X6XX_SENSOR_TEMPERATURE_RES_HAVANA) + 25.0;
    }

    if (state->sensor_temp_info.actual_publish && state->sensor_temp_info.desire_publish) {
        ICM4X6XX_INST_PRINTF(MED, instance, "temp raw %d, temp %d/100",
                                  temp_data, (int32_t)(temp_Celsius * 100));

#ifdef  ICM4X6XX_TEMPERATURE_ONE_UNRELIABLE_SAMPLE
        if (state->sensor_temp_info.discard_samples) {
            status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
            state->sensor_temp_info.discard_samples--;
        }
#endif

        pb_send_sensor_stream_event(instance,
                                    &state->sensor_temp_info.suid,
                                    timestamp,
                                    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                    status,
                                    &temp_Celsius,
                                    1,
                                    state->encoded_sensor_temp_event_len);
        icm4x6xx_log_sensor_state_raw_one_axis(instance, true, true,
            &temp_Celsius, timestamp, status);
    }
}

/**
 * @brief check need flush fifo or not
 *
 * @param[in] this   point to sns_sensor_instance
 *
 * @return true, flush FIFO
 *         flase, don't flush FIFO
 */
bool icm4x6xx_need_flush_fifo(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    bool flush = false;

    state->odr_changed = false;
    state->a_off_to_on = false;
    state->g_off_to_on = false;

    if (!state->accel_info.is_enabled && !state->gyro_info.is_enabled)
        return false;

    if (state->fifo_info.curr_format != state->fifo_info.desire_format)
        flush = true;

    if (((uint16_t)(state->accel_info.req_match_odr) !=
        (uint16_t)(state->gyro_info.cur_odr))) { /* use gyro odr for sampling odr, since md odr(50Hz) may be different */
        state->odr_changed = true;
        flush = true;
    }

    if ((uint16_t)(state->gyro_info.req_match_odr) !=
        (uint16_t)(state->gyro_info.cur_odr)) {
        state->odr_changed = true;
        flush = true;
    }

    if (state->accel_info.desire_en && !state->accel_info.is_enabled) {
        state->a_off_to_on = true;
        flush = true;
    }

    if (state->gyro_info.desire_en && !state->gyro_info.is_enabled) {
        state->g_off_to_on = true;
        flush = true;
    }

#ifndef ICM4X6XX_DISABLE_DAE
    if (state->fifo_info.cur_max_wmk != state->fifo_info.max_requested_wmk ||
        state->fifo_info.cur_max_flush_ticks != state->fifo_info.max_requested_flush_ticks) {
        flush = true;
    }
#endif

    ICM4X6XX_INST_PRINTF(MED, this, "odr changed %d, a/g off to on %d %d, need flush %d",
        state->odr_changed, state->a_off_to_on, state->g_off_to_on, flush);

    return flush;
}

//wo = without odr change, one sensor enabled, the other sensor from off to on
void icm4x6xx_set_hw_wo_stop_fifo(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    /* Only do this @max chip odr is not changed */
    if (state->odr_changed) {
        return;
    }

    if (state->a_off_to_on) {
        icm4x6xx_set_accel_odr(this, state->accel_info.req_match_odr);
        icm4x6xx_set_accel_mode(this, ICM4X6XX_A_LNM);
    }

    if (state->g_off_to_on) {
        icm4x6xx_set_gyro_odr(this, state->gyro_info.req_match_odr);
        icm4x6xx_en_gyro(this, state->gyro_info.desire_en);
    }
}

void icm4x6xx_st_store_pre_state(sns_sensor_instance* this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    state->st_resume.desire_en_a = state->accel_info.desire_en;
    state->st_resume.desire_en_g = state->gyro_info.desire_en;
    state->st_resume.desire_en_a_fifo = state->accel_info.en_fifo;
    state->st_resume.desire_en_g_fifo = state->gyro_info.en_fifo;
    state->st_resume.desire_odr_reg = state->accel_info.desire_odr_reg;
    state->st_resume.req_match_odr = state->accel_info.req_match_odr;
    state->st_resume.watermark = state->fifo_info.desire_wmk;
    state->st_resume.max_requested_wmk = state->fifo_info.max_requested_wmk;
    state->st_resume.max_requested_flush_ticks = state->fifo_info.max_requested_flush_ticks;
    state->st_resume.desire_format = state->fifo_info.desire_format;
    state->st_resume.enable_md_int = state->md_info.enable_md_int;
    ICM4X6XX_INST_PRINTF(MED, this, "icm4x6xx_st_store_pre_state done");
}

void icm4x6xx_create_heart_beat_timer(sns_sensor_instance *this, sns_time timeout_ticks)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    size_t req_len;
    uint8_t buffer[50];
    req_payload.is_periodic = true;
    req_payload.start_time = sns_get_system_time();
    req_payload.timeout_period = timeout_ticks;

#ifndef ICM4X6XX_DISABLE_DAE
    if (state->fifo_info.max_requested_wmk == UINT32_MAX)
        return;
#endif

    ICM4X6XX_VERB_INST_PRINTF(MED, this, "creat HB timer");

    req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                    sns_timer_sensor_config_fields, NULL);
    if(req_len > 0) {
        if(state->heart_beat.timer_data_stream == NULL) {
            sns_service_manager *service_mgr = this->cb->get_service_manager(this);
            sns_stream_service *stream_mgr = (sns_stream_service*)
                    service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
            stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                  this,
                  state->timer_suid,
                  &state->heart_beat.timer_data_stream);
        }

        if(state->heart_beat.timer_data_stream == NULL) {
            ICM4X6XX_INST_PRINTF(ERROR, this, "E");
            return;
        } else {
            sns_request timer_req = {
                .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
                .request = buffer,
                .request_len = req_len
            };
            state->heart_beat.timer_data_stream->api->send_request(state->heart_beat.timer_data_stream, &timer_req);

#ifdef ICM4X6XX_DEBUG_VERBOSE
            ICM4X6XX_INST_PRINTF(LOW, this, "hb timer start time: %x%08x",
                         (uint32_t)(req_payload.start_time >> 32),
                         (uint32_t)req_payload.start_time);
            ICM4X6XX_INST_PRINTF(LOW, this, "hb timeout_period: %x%08x",
                         (uint32_t)(req_payload.timeout_period >> 32),
                         (uint32_t)req_payload.timeout_period);
#endif
        }
    } else {
        ICM4X6XX_INST_PRINTF(ERROR, this, "E");
    }
}

void icm4x6xx_create_en_md_timer(sns_sensor_instance *this, sns_time timeout_ticks)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    size_t req_len;
    uint8_t buffer[50];
    req_payload.is_periodic = false;
    req_payload.start_time = sns_get_system_time();
    req_payload.timeout_period = timeout_ticks;

    ICM4X6XX_INST_PRINTF(MED, this, "c md timer");
    req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                    sns_timer_sensor_config_fields, NULL);
    if(req_len > 0) {
        if(state->timer_en_md_stream == NULL) {
            sns_service_manager *service_mgr = this->cb->get_service_manager(this);
            sns_stream_service *stream_mgr = (sns_stream_service*)
                    service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
            stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                  this,
                  state->timer_suid,
                  &state->timer_en_md_stream);
        }

        if(state->timer_en_md_stream == NULL) {
            ICM4X6XX_INST_PRINTF(ERROR, this, "E");
            return;
        } else {
            sns_request timer_req = {
                .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
                .request = buffer,
                .request_len = req_len
            };
            state->timer_en_md_stream->api->send_request(state->timer_en_md_stream, &timer_req);
        }
    } else {
        ICM4X6XX_INST_PRINTF(ERROR, this, "E");
    }
}

int icm4x6xx_flush_fifo(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    int rc = 0;

    ICM4X6XX_INST_PRINTF(MED, this, "flushing ff");

#ifdef ICM4X6XX_DISABLE_DAE
    if (state->config_step == ICM4X6XX_CONFIG_FLUSHING_HW)
        rc += icm4x6xx_en_wm_int(this, false);
#endif

#ifndef ICM4X6XX_DISABLE_DAE
    state->flushing_fifo = true; // only used for DAE
    if (state->config_step == ICM4X6XX_CONFIG_FLUSHING_HW) {
        ICM4X6XX_INST_PRINTF(MED, this, "flush HW in process");
        return rc;
    }

    if (!icm4x6xx_dae_if_flush_hw(this)) {
    }
#else
    {
        icm4x6xx_handle_interrupt_event(this, true);
    }
#endif

    return rc;
}
// ensure format changed successed
int icm4x6xx_ff_format_check(sns_sensor_instance *this)
{
    int rc = 0;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
#ifndef ICM4X6XX_DISABLE_DAE
    icm4x6xx_dae_if_info *dae_if = &((icm4x6xx_instance_state*)this->state->state)->dae_if;

    /* Only change FIFO format @flush fifo */
    if (ICM4X6XX_CONFIG_FLUSHING_HW != state->config_step &&
        !icm4x6xx_dae_if_is_on_start_streaming(&dae_if->temp) &&
        !icm4x6xx_dae_if_is_on_start_streaming(&dae_if->ag))
        return rc;
#else    /* Only change FIFO format @flush fifo */
    if (ICM4X6XX_CONFIG_FLUSHING_HW != state->config_step)
        return rc;
#endif

    /* Checking fifo format changing successfully or not */
    if (state->fifo_info.curr_format != state->fifo_info.desire_format) {
        bool match = false;
        rc += icm4x6xx_is_fifo_format_match(this,
                                            state->fifo_info.desire_format,
                                            &match);
        if (!match) {
#if 0//def ICM4X6XX_DISABLE_DAE     //Fix crash issue
            state->fifo_info.cnt_flush_hw++;

            rc += icm4x6xx_flush_fifo(this);

            if (state->fifo_info.cnt_flush_hw > 3) {
                ICM4X6XX_INST_PRINTF(ERROR, this, "ff c failed"); // fifo format change failed
                state->fifo_info.cnt_flush_hw = 0;
                state->fifo_info.curr_format = state->fifo_info.desire_format;
                rc += icm4x6xx_reset_fifo(this);
            }
#else
            ICM4X6XX_INST_PRINTF(HIGH, this, "fifo format not match"); // fifo format change failed
            state->fifo_info.cnt_flush_hw = 0;
            state->fifo_info.curr_format = state->fifo_info.desire_format;
            rc += icm4x6xx_reset_fifo(this);
#endif
        } else {
            state->fifo_info.cnt_flush_hw = 0;
            state->fifo_info.curr_format = state->fifo_info.desire_format;
        }
    }

    return rc;
}

int icm4x6xx_desire_setting_check(sns_sensor_instance *this)
{
    int rc = 0;
    bool unexpected_setting = false;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    if (state->md_info.enable_md_int && state->accel_info.en_fifo) {
        unexpected_setting = true;
    } else if (state->md_info.enable_md_int && state->accel_info.desire_publish)  {
        unexpected_setting = true;
    }

    if (state->product_info.series != HAVANA &&
        state->gyro_info.en_fifo != state->gyro_info.desire_en)
        unexpected_setting = true;

    if (unexpected_setting)
        ICM4X6XX_INST_PRINTF(MED, this, "unexpected_setting");

    return rc;
}

bool icm4x6xx_need_to_reset_fifo(icm4x6xx_instance_state *state)
{
    if (ICM4X6XX_CONFIG_FLUSHING_HW != state->config_step)
        return false;

    if (HAVANA != state->product_info.series) {
        if (state->odr_changed)
            return true;
        else
            return false;
    } else { // TODO: else is Havana, add other produce
        if (state->odr_changed)
            return true;
        else if (!state->gyro_info.is_enabled && state->gyro_info.desire_en)
            return false; // Accel is on, desire enabling gyro, and odr is not change
        else if (state->gyro_info.is_enabled && !state->gyro_info.desire_en)
            return false; // desire disabling gyro, and odr is not change
        else if (!state->accel_info.is_enabled && state->accel_info.desire_en)
            return true; // gyro is on, enable accel, Accel has quick responce feature, so reset
        else if (state->accel_info.is_enabled && !state->accel_info.desire_en)
            return false; // desire disabling accel, Accel has quick responce feature
        else
            return false;
    }
}

icm4x6xx_power_mode icm4x6xx_choose_acc_power_mode(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    if (!state->accel_info.desire_en)
        return ICM4X6XX_A_OFF;

    if (HAVANA == state->product_info.series || state->accel_info.en_fifo)
        return ICM4X6XX_A_LNM;

#ifdef ICM4X6XX_ENABLE_LOWG
    if (state->freefall_info.enable_free_fall) {
#ifdef ICM4X6XX_HIGHG_LOWG_HIGH_PERFORMANCE
        return ICM4X6XX_A_LNM;
#else
        return ICM4X6XX_A_LPM;
#endif
    }
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
    if (state->high_shock_info.enable_high_shock) {
#ifdef ICM4X6XX_HIGHG_LOWG_HIGH_PERFORMANCE
        return ICM4X6XX_A_LNM;
#else
        return ICM4X6XX_A_LPM;
#endif
    }
#endif

    /* "G LNM + A LPM" combination is supported, but not listed in DS,
     * confirmed by MA/CHRE plaform and email */
    //if (state->gyro_info.desire_en)
    //    return ICM4X6XX_A_LNM;

    if (state->md_info.enable_md_int)
        return ICM4X6XX_A_LPM;

    /* Enable accel For temperature */
    if (state->accel_info.desire_en)
        return ICM4X6XX_A_LNM;

    ICM4X6XX_INST_PRINTF(HIGH, this, "acc off");

    return ICM4X6XX_A_OFF;
}

icm4x6xx_bandwidth icm4x6xx_choose_acc_bandwidth(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    if (HAVANA == state->product_info.series)
        return BW_ODR_DIV_2;

    if (state->accel_info.desire_mode == ICM4X6XX_A_LPM)
        return BW_ODR_DIV_20;

    if (state->accel_info.desire_mode == ICM4X6XX_A_LNM &&
        FLT_EQUAL(state->accel_info.cur_odr , ICM4X6XX_ODR_1000))
        return BW_LL_MAX_200_8X_ODR; // Yokohama low latency mode
    else
        return BW_ODR_DIV_2; // Yokohama LNM or OFF

    // hardware self-test bandwith is setting directly in responce function

}

void icm4x6xx_reconfig_hw(sns_sensor_instance *this)
{
    int rc = 0;
    sns_time hb_timeout = 0;
    icm4x6xx_power_mode acc_pwr_mode;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    //50Hz ODR for MD(enable MD when only MD client present)
    float req_acc_odr = FLT_EQUAL(0, state->accel_info.req_match_odr) ? ICM4X6XX_ODR_50 : state->accel_info.req_match_odr;
    bool reset_fifo = icm4x6xx_need_to_reset_fifo(state);

    ICM4X6XX_INST_PRINTF(LOW, this, "reconfig_hw enter with reset fifo %d", reset_fifo);

    state->odr_just_changed = false;

    if (reset_fifo) {
        rc += icm4x6xx_set_fifo_mode(this, BYPASS);
    }

    rc += icm4x6xx_ff_format_check(this);

    if (state->md_info.md_client_present &&
        !state->md_info.enable_md_int) {
        icm4x6xx_update_md_intr(this, false, true);
        icm4x6xx_set_md_config(this, false);
        sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_en_md_stream);
    } else if (!state->md_info.md_client_present &&
        (state->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_ENABLED ||
         state->md_info.md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_FIRED)) {
        icm4x6xx_update_md_intr(this, false, false);
        icm4x6xx_set_md_config(this, false);
        sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_en_md_stream);
    }

    rc += icm4x6xx_set_fifo_watermark(this);
    state->fifo_info.req_chg_ff_format = false;

    /* desire setting check */
    rc += icm4x6xx_desire_setting_check(this);

    acc_pwr_mode = icm4x6xx_choose_acc_power_mode(this);
    state->accel_info.desire_mode = acc_pwr_mode;

    /* Accel switch from LPM to LNM will cause MD fired */
    if (state->accel_info.curr_mode == ICM4X6XX_A_LPM &&
        acc_pwr_mode == ICM4X6XX_A_LNM) {
        state->md_info.ignore_md_fired_num = 3; // 3 comes from test result
        /* in case of desire ODR is 500Hz
         * it's better don't set 500Hz @Accel LPM */
        rc += icm4x6xx_set_accel_mode(this, ICM4X6XX_A_OFF);
    } else if (state->accel_info.curr_mode == ICM4X6XX_A_LNM &&
        acc_pwr_mode == ICM4X6XX_A_LPM) {
        /* in case of previous ODR is 500Hz
         * it's better don't set 500Hz @Accel LPM */
        req_acc_odr = ICM4X6XX_ODR_50;
        /* DO NOT change LNM/LPM directly, otherwise clk src will be disordered
         * durig aging/stress test, case like no fifo update issue */
        rc += icm4x6xx_set_accel_mode(this, ICM4X6XX_A_OFF);
    }

    /* Use 50Hz ODR for wom */
    if (acc_pwr_mode == ICM4X6XX_A_LPM)
        req_acc_odr = ICM4X6XX_ODR_50;

    // Only enable wm int @ irq is ready
    if ((state->accel_info.desire_publish ||
        state->gyro_info.desire_publish ||
        state->fac_test_in_progress) &&
        state->irq_info.irq_ready) {
        rc += icm4x6xx_en_wm_int(this, true);
    }

#ifdef ICM4X6XX_ENABLE_LOWG
    if (state->freefall_info.enable_free_fall) {
        ICM4X6XX_INST_PRINTF(MED, this, "en freefall");
        rc += icm4x6xx_resume_dmp(this);
        rc += icm4x6xx_en_freefall(this, true);
        icm4x6xx_update_free_fall_status(this, true);
    } else  if (!state->freefall_info.free_fall_client_present &&
        (state->freefall_info.free_fall_state.free_fall_event_type == SNS_FREE_FALL_EVENT_TYPE_ENABLED ||
         state->freefall_info.free_fall_state.free_fall_event_type == SNS_FREE_FALL_EVENT_TYPE_FIRED)) {
        icm4x6xx_update_free_fall_status(this, false);
        rc += icm4x6xx_en_freefall(this, false);
    } else  if (state->freefall_info.free_fall_client_present && !state->freefall_info.enable_free_fall &&
        state->freefall_info.free_fall_state.free_fall_event_type == SNS_FREE_FALL_EVENT_TYPE_ENABLED) {
        rc += icm4x6xx_en_freefall(this, false);
        icm4x6xx_update_free_fall_status(this, false);
    }
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
    if (state->high_shock_info.enable_high_shock) {
        ICM4X6XX_INST_PRINTF(MED, this, "en highshock");
        rc += icm4x6xx_resume_dmp(this);
        rc += icm4x6xx_en_high_shock(this, true);
        icm4x6xx_update_high_shock_status(this, true);
    } else  if (!state->high_shock_info.high_shock_client_present &&
        (state->high_shock_info.high_shock_state.high_shock_event_type == SNS_HIGH_SHOCK_EVENT_TYPE_ENABLED ||
         state->high_shock_info.high_shock_state.high_shock_event_type == SNS_HIGH_SHOCK_EVENT_TYPE_FIRED)) {
        icm4x6xx_update_high_shock_status(this, false);
        rc += icm4x6xx_en_high_shock(this, false);
    } else  if (state->high_shock_info.high_shock_client_present && !state->high_shock_info.enable_high_shock &&
        state->high_shock_info.high_shock_state.high_shock_event_type == SNS_HIGH_SHOCK_EVENT_TYPE_ENABLED) {
        rc += icm4x6xx_en_high_shock(this, false);
        icm4x6xx_update_high_shock_status(this, false);
    }
#endif

    rc += icm4x6xx_set_accel_odr(this, req_acc_odr);
    rc += icm4x6xx_set_gyro_odr(this, state->gyro_info.req_match_odr);

    if (HAVANA != state->product_info.series) {
        icm4x6xx_bandwidth accel_bandwith = icm4x6xx_choose_acc_bandwidth(this);
        rc += icm4x6xx_set_accel_bandwidth(this, accel_bandwith);
    }

    if ((HAVANA != state->product_info.series) && FLT_EQUAL(state->gyro_info.req_match_odr, ICM4X6XX_ODR_1000)) {
        /* For SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES */
        rc += icm4x6xx_set_gyro_bandwidth(this, BW_LL_MAX_200_8X_ODR);
    }

#ifdef ICM4X6XX_DISABLE_DAE
    rc += icm4x6xx_en_gyro(this, state->gyro_info.desire_en);
    rc += icm4x6xx_set_accel_mode(this, acc_pwr_mode);
#else
    if (!state->gyro_info.desire_en)
        rc += icm4x6xx_en_gyro(this, state->gyro_info.desire_en);
    if (ICM4X6XX_A_OFF == acc_pwr_mode)
        rc += icm4x6xx_set_accel_mode(this, acc_pwr_mode);
#endif

    /* state->fifo_info.fifo_mode == BYPASS is sanity check
     * For following case in Havana, this is not the best way, but this is simplest
     * and least side effect way.
     * a. gated accel is present, gyro is on(ODR 1000)
     * b. a new gyro request is in, nothing change, driver send 602 MSG to DAE
     * c. MD fired, driver converts gated accel to non-gated, do DAE_FLUSH_HW
     * d. set fifo mode to BYPASS (!state->accel_info.is_enabled && state->accel_info.desire_en)
     * e. DAE 602 response back to driver
     * f. (!state->accel_info.is_enabled && state->accel_info.desire_en) was not matched now.
     * g. and then reset_fifo in re_hw is false, so fifo mode will be always bypass to cause no data
     * this is still a good way, it is not a good idea to change config_step to 602 respense
     */
    if (reset_fifo || state->fifo_info.fifo_mode == BYPASS) {
        state->odr_just_changed = true;
        icm4x6xx_set_fifo_mode(this, STREAM);
        icm4x6xx_send_int_clear_msg(this);
        state->is_first_sample = true;
        state->pre_publish_ts = 0;
        if (HAVANA == state->product_info.series) {
            // Jira SWHAVANA-112
            int16_t data_temp[3] = {0};
            icm4x6xx_read_accel_data(this, data_temp);
        }
    }

    state->is_first_sample |= state->reset_first_sample;
    state->reset_first_sample = false;

    if (rc == SNS_RC_SUCCESS) {
        state->accel_info.actual_publish = state->accel_info.desire_publish;
        state->gyro_info.actual_publish = state->gyro_info.desire_publish;
#ifdef ICM4X6XX_ENABLE_RTC_MODE
        state->sampling_intvl = ((uint16_t)state->gyro_info.cur_odr) == 0 ? 0 :
                                sns_convert_ns_to_ticks(1000000000 / (state->gyro_info.cur_odr  * ICM4X6XX_ODR_ADJUST_RATE));
#else
        state->sampling_intvl = ((uint16_t)state->gyro_info.cur_odr) == 0 ? 0 :
                                sns_convert_ns_to_ticks(1000000000 / state->gyro_info.cur_odr);
#endif
    }

    ICM4X6XX_INST_PRINTF(HIGH, this, "a/g desire/actual pub %d %d/%d %d, irq rdy %d, 1st_sample %d, rc %d",
                         state->accel_info.desire_publish,
                         state->gyro_info.desire_publish,
                         state->accel_info.actual_publish,
                         state->gyro_info.actual_publish,
                         state->irq_info.irq_ready,
                         state->is_first_sample,
                         rc);

#ifdef ICM4X6XX_DISABLE_DAE
    // Enable timer in case of sensor temp clients
    if (state->sensor_temp_info.desire_publish) {
#else
    if (state->sensor_temp_info.desire_publish &&
        !icm4x6xx_dae_if_available(this)) {
#endif
        icm4x6xx_set_polling_config(this);
    } else {
        state->sensor_temp_info.timer_is_active = false;
        state->sensor_temp_info.cur_sampling_rate_hz = 0.0f;
        sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
    }
    state->sensor_temp_info.actual_publish = state->sensor_temp_info.desire_publish;

#ifndef ICM4X6XX_DISABLE_DAE
    if (!state->sensor_temp_info.actual_publish ||
        !FLT_EQUAL(state->sensor_temp_info.sampling_rate_hz, state->sensor_temp_info.cur_sampling_rate_hz))
        icm4x6xx_dae_if_stop_temp_streaming(this);

    if (state->accel_info.actual_publish ||
        state->gyro_info.actual_publish ||
        state->sensor_temp_info.actual_publish ||
        state->md_info.md_client_present ||
#ifdef ICM4X6XX_ENABLE_LOWG
        state->freefall_info.enable_free_fall ||
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
        state->high_shock_info.enable_high_shock ||
#endif
        state->fac_test_in_progress)
        icm4x6xx_dae_if_start_streaming(this);
    else
        icm4x6xx_dae_if_stop_streaming(this);
#endif

    //icm4x6xx_dump_reg(this);

#ifdef ICM4X6XX_DISABLE_DAE
    hb_timeout = state->sampling_intvl * ICM4X6XX_HB_ODR_COUNT * state->fifo_info.cur_wmk;
#else
    hb_timeout = state->sampling_intvl * ICM4X6XX_HB_ODR_COUNT *
            (icm4x6xx_dae_if_available(this) ? state->fifo_info.max_requested_wmk : state->fifo_info.cur_wmk);
#endif
    state->heart_beat.timeout_period = SNS_MAX(HB_TIMEOUT_TICKS, hb_timeout);

    if (state->accel_info.actual_publish || state->gyro_info.actual_publish || state->fac_test_in_progress) {
        state->heart_beat.heart_attack_cnt = 0;
        icm4x6xx_create_heart_beat_timer(this, state->heart_beat.timeout_period);
    } else {
        sns_sensor_util_remove_sensor_instance_stream(this, &state->heart_beat.timer_data_stream);
        uint8_t int_status = 0;
        icm4x6xx_read_int_status2(this, &int_status);
        icm4x6xx_read_int_status(this, &int_status);
        icm4x6xx_send_int_clear_msg(this);
    }

    state->config_step = ICM4X6XX_CONFIG_IDLE; /* done with reconfig */

    if (state->md_info.enable_md_int) {
        icm4x6xx_set_md_config(this, true);
        icm4x6xx_create_en_md_timer(this, sns_convert_ns_to_ticks(ACC_START_UP_TIME));
    }

    icm4x6xx_send_config_event(this);
}

/**
 * Changes all gated accel requests to non-gated accel requests.
 *
 * @param instance   Reference to the instance
 *
 * @return None
 */
static void icm4x6xx_convert_accel_gated_req_to_non_gated(
            sns_sensor_instance *const instance)
{
    sns_request *request;
    bool req_converted_to_non_gated = false;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state *)instance->state->state;
    sns_sensor_uid *suid = &state->accel_info.suid;

    /** Parse through existing requests and change gated accel
     *  requests to non-gated accel requests. */
    for(request = (sns_request *)instance->cb->get_client_request(instance, suid, true);
        NULL != request;
        request = (sns_request *)instance->cb->get_client_request(instance, suid, false))
    {
        if (request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG) {
            request->message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
            req_converted_to_non_gated = true;
        }
    }

    /** Send an event to gated stream clients that the request is
     *  now treated as non-gated */
    if(req_converted_to_non_gated) {
        sns_service_manager *mgr = instance->cb->get_service_manager(instance);
        sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
        sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);

        if(NULL != event) {
            event->message_id = SNS_STD_EVENT_GATED_SENSOR_MSGID_GATED_REQ_CONVERTED_TO_NON_GATED;
            event->event_len = 0;
            event->timestamp = sns_get_system_time();
            e_service->api->publish_event(e_service, instance, event, suid);
        }
    }
}

/**
 * Extract a gyro sample from a segment of the fifo buffer and generate an
 * event.
 *
 * @param[i] fifo_buf        The segment of fifo buffer that has the gyro sample
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current icm4x6xx sensor instance
 * @param[i] event_service      Event service for sending out the gyro sample
 * @param[i] state              The state of the icm4x6xx sensor instance
 */
static void icm4x6xx_handle_gyro_sample(
            sns_sensor_instance *const instance,
            const uint8_t fifo_buf[6],
            bool is_hi_res,
            int8_t hi_res[3],
            sns_time timestamp,
            bool is_first_one,
            bool is_last_one)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state *)instance->state->state;
    int16_t raw_lsb_16b[TRIAXIS_NUM] = {0};
    int32_t raw_lsb_32b[TRIAXIS_NUM] = {0};
    float ipdata[TRIAXIS_NUM] = {0}, opdata_raw[TRIAXIS_NUM] = {0};
    bool hi_range = state->product_info.hi_range;
    uint8_t i = 0;

    if (!state->gyro_info.is_enabled)
        return;

    raw_lsb_16b[TRIAXIS_X] = (int16_t)(((int16_t)(fifo_buf[0] << 8)) | fifo_buf[1]);
    raw_lsb_16b[TRIAXIS_Y] = (int16_t)(((int16_t)(fifo_buf[2] << 8)) | fifo_buf[3]);
    raw_lsb_16b[TRIAXIS_Z] = (int16_t)(((int16_t)(fifo_buf[4] << 8)) | fifo_buf[5]);

    if (is_hi_res) {
        raw_lsb_32b[TRIAXIS_X] = (((int32_t)raw_lsb_16b[TRIAXIS_X] << 4)) | hi_res[TRIAXIS_X];
        raw_lsb_32b[TRIAXIS_Y] = (((int32_t)raw_lsb_16b[TRIAXIS_Y] << 4)) | hi_res[TRIAXIS_Y];
        raw_lsb_32b[TRIAXIS_Z] = (((int32_t)raw_lsb_16b[TRIAXIS_Z] << 4)) | hi_res[TRIAXIS_Z];
    } else {
        raw_lsb_32b[TRIAXIS_X] = (int32_t)raw_lsb_16b[TRIAXIS_X];
        raw_lsb_32b[TRIAXIS_Y] = (int32_t)raw_lsb_16b[TRIAXIS_Y];
        raw_lsb_32b[TRIAXIS_Z] = (int32_t)raw_lsb_16b[TRIAXIS_Z];
    }

    /** Compensate filter delay */
    timestamp -= state->gyro_info.filter_delay;

    ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "Gyro LSB: %d %d %d ",
                         raw_lsb_32b[TRIAXIS_X], raw_lsb_32b[TRIAXIS_Y], raw_lsb_32b[TRIAXIS_Z]);
    if (is_hi_res != state->product_info.use_hi_res) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "hi_res bit mismatch hi_res setting %d:%d",
            is_hi_res, state->product_info.use_hi_res);
    }

    ipdata[TRIAXIS_X] = raw_lsb_32b[TRIAXIS_X] * GYRO_RES_RPS(state->gyro_info.fsr, is_hi_res, hi_range);
    ipdata[TRIAXIS_Y] = raw_lsb_32b[TRIAXIS_Y] * GYRO_RES_RPS(state->gyro_info.fsr, is_hi_res, hi_range);
    ipdata[TRIAXIS_Z] = raw_lsb_32b[TRIAXIS_Z] * GYRO_RES_RPS(state->gyro_info.fsr, is_hi_res, hi_range);

    // axis conversion
    for(i = 0; i < TRIAXIS_NUM; i++) {
        opdata_raw[state->axis_map[i].opaxis] =
            (state->axis_map[i].invert ? -1.0 : 1.0) * ipdata[state->axis_map[i].ipaxis];
    }

    if(state->fac_test_in_progress && state->fac_test_sensor == ICM4X6XX_GYRO) {
        state->fac_test_info.num_samples++;

        // Discard first 8 samples for the 100Hz ODR
        for(i = 0; state->fac_test_info.num_samples > FACTORY_CAL_DISCARD_G && i < TRIAXIS_NUM; i++) {
            state->fac_test_info.sample_sum[i] += opdata_raw[i];
            state->fac_test_info.sample_square_sum[i] += opdata_raw[i] * opdata_raw[i];
        }
    }

    // factory calibration
    vector3 opdata_cal = sns_apply_calibration_correction_3(
                            make_vector3_from_array(opdata_raw),
                            make_vector3_from_array(state->gyro_registry_cfg.fac_cal_bias),
                            state->gyro_registry_cfg.fac_cal_corr_mat);

    if (!state->fac_test_in_progress &&
        (state->gyro_info.desire_publish && state->gyro_info.actual_publish)) {
        sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

        if (state->gyro_info.num_samples_to_discard != 0) {
            status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
            state->gyro_info.num_samples_to_discard--;

            ICM4X6XX_INST_PRINTF(LOW, instance, "G discard %d ", state->gyro_info.num_samples_to_discard);
        }

        pb_send_sensor_stream_event(instance,
                                    &state->gyro_info.suid,
                                    timestamp,
                                    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                    status,
                                    opdata_cal.data,
                                    ARR_SIZE(opdata_cal.data),
                                    state->encoded_imu_event_len);

        state->g_reported_sample++;
        ICM4X6XX_VERB_INST_PRINTF(HIGH, instance, "g_reported_sample %d, TS: %x%08x, filter delay: %d",
                             state->g_reported_sample,
                             (uint32_t)(timestamp >> 32), (uint32_t)timestamp,
                             (uint32_t)tick2us(state->gyro_info.filter_delay));

        icm4x6xx_log_sensor_state_raw_three_axis(instance, ICM4X6XX_GYRO, is_first_one,
                is_last_one, opdata_raw, timestamp, status);
    }
}

/**
 * Extract a accel sample from a segment of the fifo buffer and generate an
 * event.
 *
 * @param[i] fifo_buf        The segment of fifo buffer that has the accel sample
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current icm4x6xx sensor instance
 * @param[i] event_service      Event service for sending out the gyro sample
 * @param[i] state              The state of the icm4x6xx sensor instance
 */
static void icm4x6xx_handle_accel_sample(
            sns_sensor_instance *const instance,
            const uint8_t fifo_buf[6],
            bool is_hi_res,
            int8_t hi_res[3],
            sns_time timestamp,
            bool is_first_one,
            bool is_last_one)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state *)instance->state->state;
    int16_t raw_lsb_16b[TRIAXIS_NUM] = {0};
    int32_t raw_lsb_32b[TRIAXIS_NUM] = {0};
    float ipdata[TRIAXIS_NUM] = {0}, opdata_raw[TRIAXIS_NUM] = {0};
    bool hi_range = state->product_info.hi_range;
    uint8_t i = 0;

    if (!state->accel_info.is_enabled)
        return;

    raw_lsb_16b[TRIAXIS_X] = (int16_t)(((int16_t)(fifo_buf[0] << 8)) | fifo_buf[1]);
    raw_lsb_16b[TRIAXIS_Y] = (int16_t)(((int16_t)(fifo_buf[2] << 8)) | fifo_buf[3]);
    raw_lsb_16b[TRIAXIS_Z] = (int16_t)(((int16_t)(fifo_buf[4] << 8)) | fifo_buf[5]);

    if (is_hi_res) {
        raw_lsb_32b[TRIAXIS_X] = (((int32_t)raw_lsb_16b[TRIAXIS_X] << 4)) | hi_res[TRIAXIS_X];
        raw_lsb_32b[TRIAXIS_Y] = (((int32_t)raw_lsb_16b[TRIAXIS_Y] << 4)) | hi_res[TRIAXIS_Y];
        raw_lsb_32b[TRIAXIS_Z] = (((int32_t)raw_lsb_16b[TRIAXIS_Z] << 4)) | hi_res[TRIAXIS_Z];
    } else {
        raw_lsb_32b[TRIAXIS_X] = (int32_t)raw_lsb_16b[TRIAXIS_X];
        raw_lsb_32b[TRIAXIS_Y] = (int32_t)raw_lsb_16b[TRIAXIS_Y];
        raw_lsb_32b[TRIAXIS_Z] = (int32_t)raw_lsb_16b[TRIAXIS_Z];
    }

    /** Compensate filter delay */
    timestamp -= state->accel_info.filter_delay;

    ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "Accel LSB: %d %d %d TS: %x%08x, filter delay: %d, res=%d/10^6",
                         raw_lsb_32b[TRIAXIS_X], raw_lsb_32b[TRIAXIS_Y], raw_lsb_32b[TRIAXIS_Z],
                         (uint32_t)(timestamp >> 32), (uint32_t)timestamp,
                         (uint32_t)tick2us(state->accel_info.filter_delay),
                         (uint32_t)(ACC_RES_M2_PER_S(state->accel_info.fsr, is_hi_res, hi_range) * 1000 * 1000));
    if (is_hi_res != state->product_info.use_hi_res) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "hi_res bit mismatch hi_res setting %d:%d",
            is_hi_res, state->product_info.use_hi_res);
    }

    ipdata[TRIAXIS_X] = raw_lsb_32b[TRIAXIS_X] * ACC_RES_M2_PER_S(state->accel_info.fsr, is_hi_res, hi_range);
    ipdata[TRIAXIS_Y] = raw_lsb_32b[TRIAXIS_Y] * ACC_RES_M2_PER_S(state->accel_info.fsr, is_hi_res, hi_range);
    ipdata[TRIAXIS_Z] = raw_lsb_32b[TRIAXIS_Z] * ACC_RES_M2_PER_S(state->accel_info.fsr, is_hi_res, hi_range);

    // axis conversion
    for(i = 0; i < TRIAXIS_NUM; i++) {
        opdata_raw[state->axis_map[i].opaxis] = (state->axis_map[i].invert ? -1.0 : 1.0) *
                    ipdata[state->axis_map[i].ipaxis];
    }

    if(state->fac_test_in_progress && state->fac_test_sensor == ICM4X6XX_ACCEL) {
        state->fac_test_info.num_samples++;

        // Discard first 5 samples for the 100Hz ODR (havana 9470 3 samples)
        for(i = 0; state->fac_test_info.num_samples > FACTORY_CAL_DISCARD_A && i < TRIAXIS_NUM; i++) {
            if(i == (TRIAXIS_NUM - 1)) {
                float temp_data = opdata_raw[i] - G;
                state->fac_test_info.sample_sum[i] += temp_data;
                state->fac_test_info.sample_square_sum[i] += temp_data * temp_data;
            } else {
                state->fac_test_info.sample_sum[i] += opdata_raw[i];
                state->fac_test_info.sample_square_sum[i] += opdata_raw[i] * opdata_raw[i];
            }
        }
    }

    // factory calibration
    vector3 opdata_cal = sns_apply_calibration_correction_3(
                            make_vector3_from_array(opdata_raw),
                            make_vector3_from_array(state->accel_registry_cfg.fac_cal_bias),
                            state->accel_registry_cfg.fac_cal_corr_mat);

    if (!state->fac_test_in_progress &&
        ((state->accel_info.desire_publish && state->accel_info.actual_publish) ||
        (state->accel_info.gated_client_present && !state->md_info.enable_md_int))) {
        sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

        if(state->accel_info.num_samples_to_discard != 0) {
            status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
            ICM4X6XX_INST_PRINTF(LOW, instance, "A discard %d ", state->accel_info.num_samples_to_discard);
            state->accel_info.num_samples_to_discard--;
        }

        pb_send_sensor_stream_event(instance,
                                    &state->accel_info.suid,
                                    timestamp,
                                    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                    status,
                                    opdata_cal.data,
                                    ARR_SIZE(opdata_cal.data),
                                    state->encoded_imu_event_len);

        state->a_reported_sample++;
        ICM4X6XX_INST_PRINTF(HIGH, instance, "a_reported_sample %d, TS: %x%08x, %d",
                             state->a_reported_sample,
                             (uint32_t)(timestamp >> 32), (uint32_t)timestamp,
                             status);

        icm4x6xx_log_sensor_state_raw_three_axis(instance, ICM4X6XX_ACCEL, is_first_one,
                is_last_one, opdata_raw, timestamp, status);
    }
}

void icm4x6xx_send_fifo_flush_done(
     sns_sensor_instance *const instance,
     sns_sensor_uid *suid)
{
    sns_service_manager *mgr = instance->cb->get_service_manager(instance);
    sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
    sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);

    ICM4X6XX_INST_PRINTF(HIGH, instance, "flush req done"); // send flush done

    if(NULL != event) {
        event->message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
        event->event_len = 0;
        event->timestamp = sns_get_system_time();

        e_service->api->publish_event(e_service, instance, event, suid);
    }
}

static uint32_t icm4x6xx_cal_valid_fifo_len(
                sns_sensor_instance *instance,
                const uint8_t *buf,
                size_t buf_len,
                uint16_t *cnt)
{
    icm4x6xx_fifo_header_t header;
    uint32_t valid_buf_len = 0;

    /* Calculator valid FIFO buf length */
    while (valid_buf_len < buf_len) {
        header.head_byte = buf[valid_buf_len];
        if (header.head_byte == 0xff ||
            header.head_byte == 0x80 ||
            header.head_byte == 0x00) {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "ff emtpy"); //fifo emtpy
            break;
        }
        valid_buf_len++;
        if(header.bits.accel_bit) {
            valid_buf_len += 6;
        }

        if(header.bits.gyro_bit) {
            valid_buf_len += 6;
        }

        valid_buf_len += header.bits.twentybits_bit ? 2 : 1; // temperature bytes

        if (header.bits.timestamp_bit & 0x02) {
            valid_buf_len += 2;
        }

        if (header.bits.twentybits_bit) {
            valid_buf_len += 3;
        }
        (*cnt)++;
    }

    return valid_buf_len;
}
// get matched_cnt of INT TS
static void icm4x6xx_match_ts_with_fifo_cnt(
       sns_sensor_instance *instance,
       uint16_t packet_cnt,
       sns_time timestamp,
       uint16_t *matched_cnt)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state *)instance->state->state;
    int32_t cur_intvl = 0;
    //int32_t next_intvl = 0;
    uint32_t cur_intvl_dt = 0;
    //uint32_t next_intvl_dt = 0;
    uint32_t min_intvl_dt = (uint32_t)(state->sampling_intvl);
    uint16_t cnt = state->fifo_info.cur_wmk;
    uint16_t min_intvl_cnt = cnt;

    if (packet_cnt == state->fifo_info.cur_wmk) {   //match, conside it as WM INT ts
        *matched_cnt = packet_cnt;
        return;
    }

#ifndef ICM4X6XX_DISABLE_DAE
    if (state->is_flush || state->dae_ts_type == sns_dae_timestamp_type_SNS_DAE_TIMESTAMP_TYPE_SYSTEM_TIME) {
#else
    if (state->is_flush) {
#endif
        *matched_cnt = packet_cnt;
        return;
    }

    if (state->is_first_sample) {
        if (!state->odr_just_changed) {
            *matched_cnt = state->fifo_info.cur_wmk;    //first reliable ts from INT ts
            return;
        } else {
            state->odr_just_changed = false;
        }
    }

    if (packet_cnt < state->fifo_info.cur_wmk) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "ff is full"); //abnormal case, like overflow, overflow reason unknown
        *matched_cnt = packet_cnt;
        return;
    }

    ICM4X6XX_INST_PRINTF(LOW, instance, "wm %d pcnt %d", cnt, packet_cnt);

    while (cnt <= packet_cnt) {
        cur_intvl = (timestamp - state->pre_record_ts) / cnt;
        //next_intvl = (timestamp - state->pre_record_ts) / (cnt + 1);
        cur_intvl_dt = abs(cur_intvl - (int32_t)(state->sampling_intvl));
        //next_intvl_dt = abs(next_intvl - (uint32_t)(state->sampling_intvl));
        ///* find the min dt, large wmk will be a risk */
        if (cur_intvl_dt < min_intvl_dt) {
            min_intvl_cnt = cnt;
            min_intvl_dt = cur_intvl_dt;
        }

        cnt++;

        ICM4X6XX_INST_PRINTF(LOW, instance, "cur intvl/intvl_dt %d/%d", cur_intvl, cur_intvl_dt);
    }
    *matched_cnt = min_intvl_cnt;
}

static void icm4x6xx_cal_intvl_with_lpf(
            sns_sensor_instance *instance,
            uint16_t packet_cnt,
            sns_time timestamp,
            sns_time *meas_intvl)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state *)instance->state->state;
    sns_time cal_end = 0;
    double estimated_intvl = 0;
    double measured_interval = 0;

#ifndef ICM4X6XX_DISABLE_DAE
    /* data_age_limit_ticks = state->fifo_info.cur_max_flush_ticks * 1.1 */
    if (state->fifo_info.flush_only &&
        timestamp - state->pre_record_ts > (state->fifo_info.cur_max_flush_ticks * 1.1f)) { //cur_max_flush_ticks = flush period, lost sample condition
        state->is_first_sample = true;
    }
#endif

    if (state->is_first_sample) {
        if ((timestamp - state->sampling_intvl * (packet_cnt - 1)) > state->pre_record_ts)
            state->pre_record_ts = timestamp - state->sampling_intvl * packet_cnt;  //state->sampling_intvl: theoretical ts interval
        state->pre_meas_intvl = state->sampling_intvl;
        state->is_first_sample = false;
        state->odr_just_changed= false;
    }

    measured_interval = (timestamp - state->pre_record_ts) / packet_cnt;
    estimated_intvl = state->pre_meas_intvl;

//#ifdef ICM4X6XX_DEBUG_VERBOSE
    ICM4X6XX_INST_PRINTF(MED, instance, "pre_record_ts: %x%08x, pre_meas_intvl: %d",
                         (uint32_t)(state->pre_record_ts >> 32), (uint32_t)state->pre_record_ts,
                         (uint32_t)tick2us(estimated_intvl));
//#endif

    /**********************************************************************
    ** Use low pass filter to smooth interval jitter.
    ** LPF: Y(n) = (1-a)*Y(n-1) + (a*X(n))) = Y(n-1) - (a*(Y(n-1)-X(n)))
    ** a = 1 / LPF_PARAMETER_N;
    **********************************************************************/
    estimated_intvl = estimated_intvl - (estimated_intvl - measured_interval) / state->lpf_parameter;

    if (estimated_intvl > state->sampling_intvl * INTVL_TOLERANCE_FACTOR) {
        estimated_intvl = state->sampling_intvl;
    }

    cal_end = state->pre_record_ts + (estimated_intvl) * packet_cnt;

    /* We don't trust following DAE TS on
     * (measured_interval < state->sampling_intvl * CTS_JITTER_PCNT)
     */
    if (cal_end > timestamp && (measured_interval >= state->sampling_intvl * CTS_JITTER_PCNT)) {    //cal_end > timestamp due to bigger est intvl
        *meas_intvl = measured_interval;
    } else if (cal_end > timestamp && (measured_interval < state->sampling_intvl * CTS_JITTER_PCNT)) {
        /* A litter big than interval * CTS_JITTER_PCNT, 100 is tick value, change to ms by divide 19200 */
        *meas_intvl = state->sampling_intvl * CTS_JITTER_PCNT + 100;
    } else {
        *meas_intvl = estimated_intvl;
    }

    ICM4X6XX_INST_PRINTF(MED, instance, "meas_intvl %d est_intvl %d ",
                         (uint32_t)tick2us(measured_interval), (uint32_t)tick2us(*meas_intvl));
}

void icm4x6xx_cal_sample_intvl_with_int_ts(
     sns_sensor_instance *instance,
     uint16_t packet_cnt,
     sns_time timestamp,
     sns_time *meas_intvl)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state *)instance->state->state;
    uint16_t matched_cnt = packet_cnt;
// state->sampling_intvl, never change except new ODR
    if (state->is_first_sample && timestamp <= state->pre_record_ts) {
        state->pre_record_ts = timestamp - state->sampling_intvl * packet_cnt;
        state->pre_meas_intvl = state->sampling_intvl;
        state->is_first_sample = false;
        ICM4X6XX_INST_PRINTF(MED, instance, "warning: first sample ts < pre_record_ts");
    }

    if (timestamp <= state->pre_record_ts) {
        *meas_intvl = state->sampling_intvl * CTS_JITTER_PCNT;
        ICM4X6XX_INST_PRINTF(MED, instance, "warning: ts < pre_record_ts");
        return;
    }

    icm4x6xx_match_ts_with_fifo_cnt(instance, packet_cnt, timestamp, &matched_cnt);

    state->lpf_parameter = LPF_PARAMETER_N;

#ifndef ICM4X6XX_DISABLE_DAE
    if (state->dae_ts_type == sns_dae_timestamp_type_SNS_DAE_TIMESTAMP_TYPE_SYSTEM_TIME) {
        state->lpf_parameter = 300;
    } else if (state->dae_ts_type == sns_dae_timestamp_type_SNS_DAE_TIMESTAMP_TYPE_HW_IRQ) {
        state->lpf_parameter = LPF_PARAMETER_N;
    }
#else
    if (state->is_flush) {
        if (packet_cnt < 10)
            state->lpf_parameter = 300;
        else if (packet_cnt < 100)
            state->lpf_parameter = 50;
        else
            state->lpf_parameter = 25;
    }
#endif
    ICM4X6XX_INST_PRINTF(MED, instance, "packet/match cnt: %d %d, LPF: %d",
                            packet_cnt, matched_cnt, state->lpf_parameter);

    icm4x6xx_cal_intvl_with_lpf(instance, matched_cnt, timestamp, meas_intvl);
}

#ifndef ICM4X6XX_USE_INT_TS
static void icm4x6xx_cal_ts_end_with_intnl_cnt(
            sns_sensor_instance *instance,
            uint16_t last_rec_cnt_16b,
            sns_time *last_rec_ts)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state *)instance->state->state;
    sns_time intl_cnt_strobe_ts = 0;
    uint32_t intl_cnt_20b = 0;
    uint32_t last_rec_cnt_20b = 0;
    uint32_t last_rec_cnt_b16_b19 = 0;
    uint32_t delta_cnt = 0;

#ifndef ICM4X6XX_READ_TMST_IN_INT
    int rc = 0;
    rc += icm4x6xx_enable_tmst_val(instance, true);
    intl_cnt_strobe_ts = state->strobe_master_cnt;
    rc += icm4x6xx_read_tmst_val(instance, &intl_cnt_20b);
    if (rc) {
        ICM4X6XX_INST_PRINTF(ERROR, instance, "read intnl count failed %d", rc);
    }
#else
    intl_cnt_strobe_ts = state->r_fifo_cnt_master_cnt;
    intl_cnt_20b = state->slave_cnt;
#endif

    if ((intl_cnt_20b & 0x0000ffff) < last_rec_cnt_16b) {
        last_rec_cnt_b16_b19 = ((intl_cnt_20b & 0x000f0000) == 0) ? 0x000f0000 :
                               (intl_cnt_20b & 0x000f0000) - 0x00010000;
    } else {
        last_rec_cnt_b16_b19 = intl_cnt_20b & 0x000f0000;
    }

    last_rec_cnt_20b = last_rec_cnt_b16_b19 | last_rec_cnt_16b;
    if (last_rec_cnt_20b > intl_cnt_20b) {
        /* icm4x6xx TMST 20 bits register overflow */
        ICM4X6XX_INST_PRINTF(LOW, instance, "intnl cnt reset %d", intl_cnt_20b);
        delta_cnt = 0x000fffff - last_rec_cnt_20b + 1 + intl_cnt_20b;
    } else {
        delta_cnt = intl_cnt_20b - last_rec_cnt_20b;
    }

    *last_rec_ts = intl_cnt_strobe_ts - sns_convert_ns_to_ticks(delta_cnt * 1000);

#ifdef ICM4X6XX_DEBUG_VERBOSE
    ICM4X6XX_INST_PRINTF(LOW, instance, "intl_cnt_20b, last_rec_cnt_16b: %#x %#x",
                         intl_cnt_20b, last_rec_cnt_16b);
    ICM4X6XX_INST_PRINTF(LOW, instance, "last_rec_cnt: %#x %#x %#x ",
                         last_rec_cnt_20b, last_rec_cnt_b16_b19, last_rec_cnt_16b);
    ICM4X6XX_INST_PRINTF(MED, instance, "delta_cnt: %d ", delta_cnt);
    ICM4X6XX_INST_PRINTF(LOW, instance, "intl_cnt_strobe_ts: %x%08x",
                         (uint32_t)(intl_cnt_strobe_ts >> 32), (uint32_t)intl_cnt_strobe_ts);
    ICM4X6XX_INST_PRINTF(LOW, instance, "last_rec_ts: %x%08x",
                         (uint32_t)(*last_rec_ts >> 32), (uint32_t)*last_rec_ts);
    ICM4X6XX_INST_PRINTF(LOW, instance, "intnl cnt: %d", intl_cnt_20b);
#endif
}

/**
 * @brief Calculate sample interval according device
 *          internal count
 *
 * @param[in] instance, point to sensor instance
 * @param[in] last_rec_cnt_16b, 16 bits last record count in fifo
 * @param[in] packet_cnt, total packet count
 * @param[out] last_rec_ts, last record system TS
 * @param[out] meas_intvl, sample interval
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
void icm4x6xx_cal_sample_intvl_with_intnl_cnt(
     sns_sensor_instance *instance,
     uint16_t last_rec_cnt_16b,
     uint16_t packet_cnt,
     sns_time *last_rec_ts,
     sns_time *meas_intvl)
{
    icm4x6xx_cal_ts_end_with_intnl_cnt(instance, last_rec_cnt_16b, last_rec_ts);
    icm4x6xx_cal_intvl_with_lpf(instance, packet_cnt, *last_rec_ts, meas_intvl);
}
#endif

void icm4x6xx_process_fifo_data_buffer(
     sns_sensor_instance *instance,
     const uint8_t       *buf,
     size_t              buf_len,
     sns_time            timestamp)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state *)instance->state->state;
    icm4x6xx_fifo_header_t header;
    uint32_t idx = 0;
    uint32_t valid_buf_len = 0;
    uint8_t packet_size = 0;
    uint16_t packet_cnt = 0;
    uint16_t total_cnt = 0;
    uint16_t pre_rec_cnt_16b = 0;
    sns_time last_rec_ts = 0;
    sns_time meas_intvl = 0;
#ifdef ICM4X6XX_ODR_CHANGING_SAMPLE_MISSING_SAMPLE_GAP
    // TODO remove this
    sns_time pre_record_intvl = state->pre_meas_intvl;
    sns_time pre_odr_last_record_ts = state->pre_record_ts;
    bool added_one_more_sample = true;

    if (state->odr_just_changed) {
        added_one_more_sample = false;
    }
#endif
    sns_time sample_ts = 0;
    bool report_sample = false;
    bool skipped_one_sample = false;
    int8_t accel_high_res[3];
    int8_t gyro_high_res[3];
    uint8_t accel_raw[6];
    uint8_t gyro_raw[6];
    bool report_accel = false;
    bool report_gyro = false;


    /* Get packet size */
    icm4x6xx_get_packet_size(instance, &packet_size);
    if (packet_size == 0) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "icm4x6xx_get_packet_size zero");
        goto CLEAN_STATUS;
    }

    /* Calculator valid buf_len */
    valid_buf_len = icm4x6xx_cal_valid_fifo_len(instance, buf, buf_len, &packet_cnt);
    ICM4X6XX_INST_PRINTF(LOW, instance, "valid buf_len/packet_cnt %d %d", valid_buf_len, packet_cnt);
    if (valid_buf_len == 0 || packet_cnt == 0) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "no valid senor data");
        goto CLEAN_STATUS;
    }

    if (packet_cnt != valid_buf_len / packet_size) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "mixed packet error");
    }

    total_cnt = packet_cnt;

    icm4x6xx_create_heart_beat_timer(instance, state->heart_beat.timeout_period);

#ifndef ICM4X6XX_USE_INT_TS
    uint16_t last_rec_cnt_16b = 0;
    /* Get latest record device TS */
    last_rec_cnt_16b = (uint16_t)(buf[valid_buf_len-2] << 8 | buf[valid_buf_len-1]);

    UNUSED_VAR(timestamp);
    /* Get sample interval TS, last record TS, last record device tick */
    icm4x6xx_cal_sample_intvl_with_intnl_cnt(instance, last_rec_cnt_16b, packet_cnt,
                                                &last_rec_ts, &meas_intvl);
#else
    UNUSED_VAR(last_rec_ts);
    icm4x6xx_cal_sample_intvl_with_int_ts(instance, packet_cnt, timestamp, &meas_intvl);
#endif
    sample_ts = state->pre_record_ts;

    idx = 0;
    pre_rec_cnt_16b = state->pre_rec_cnt_16b;
    while (idx < valid_buf_len) {
        ICM4X6XX_VERB_INST_PRINTF(MED, instance, "dts0 %d, sample_ts: %x%08x, packet_cnt %d, final meas_intvl %d",
                             (uint32_t)pre_rec_cnt_16b,
                             (uint32_t)(sample_ts >> 32), (uint32_t)sample_ts,
                             (uint32_t)packet_cnt,
                             (uint32_t)tick2us(meas_intvl));
        sample_ts += meas_intvl;

        if (sample_ts > sns_get_system_time()) {    ///* bigger than system ts, QC maybe throw it */
            sample_ts = sns_get_system_time();
            ICM4X6XX_INST_PRINTF(HIGH, instance, "adjust to ts now");
        }

        report_accel = false;   ///* added for high resolution */
        report_gyro = false;    ///* added for high resolution */

        header.head_byte = buf[idx];
        ICM4X6XX_VERB_INST_PRINTF(HIGH, instance, "header: %#x", header.head_byte);
        if (header.head_byte == 0xff ||
            header.head_byte == 0x80 ||
            header.head_byte == 0x00) {
            ICM4X6XX_INST_PRINTF(ERROR, instance, "ff empty");
            break;
        }

        idx++; /* skip fifo header */

        if (HAVANA != state->product_info.series) {
            report_sample = true;
        } else {
#ifdef ICM4X6XX_1K_2_500HZ
            if (state->do_1K_2_500_dec && FLT_EQUAL(state->accel_info.cur_odr, ICM4X6XX_ODR_1000)) {    ///* throw which one, or keep which one */
                if (state->pre_publish_ts == 0 && total_cnt >= 2) {
                    report_sample = false;              ///* throw this one */
                } else if (state->pre_publish_ts == 0 && total_cnt == 1) {
                    report_sample = true;               ///* keep this one */
                    ICM4X6XX_INST_PRINTF(HIGH, instance, "it is not better to come here");
                } else if (state->pre_publish_ts == 0 && total_cnt == 0) {
                    report_sample = false;
                    ICM4X6XX_INST_PRINTF(ERROR, instance, "code shouldn't come here");
                } else if ((state->pre_publish_ts != 0) &&
                           ((sample_ts - state->pre_publish_ts) >= ODR_1K_2_500_COMPARE_INTVL)) {   //1k=1ms, 1.5ms is the judge condition
                    report_sample = true;
                } else if ((state->pre_publish_ts != 0) &&
                           ((sample_ts - state->pre_publish_ts) < ODR_1K_2_500_COMPARE_INTVL)) {
                    report_sample = false;
                }
            } else {
                report_sample = true;
            }
#else
            report_sample = true;
#endif
        }

        if (report_sample || skipped_one_sample) {
            state->pre_publish_ts = sample_ts;
            if (header.bits.accel_bit) {
                memcpy(accel_raw, &buf[idx], 6);
                report_accel = true;
                idx += 6; /* skip accel data */
            }

            if(header.bits.gyro_bit) {
                memcpy(gyro_raw, &buf[idx], 6);
                report_gyro = true;
                idx += 6; /* skip gyro data */
            }
            skipped_one_sample = false;
        } else {
            skipped_one_sample = true;
            if (header.bits.accel_bit) {
                if(state->accel_info.num_samples_to_discard != 0) {
                    ICM4X6XX_INST_PRINTF(LOW, instance, "A dis %d",
                            state->accel_info.num_samples_to_discard);
                    state->accel_info.num_samples_to_discard--;
                }
                idx += 6;
            }

            if(header.bits.gyro_bit) {
                if (state->gyro_info.num_samples_to_discard != 0) {
                    ICM4X6XX_INST_PRINTF(LOW, instance, "G dis %d",
                            state->gyro_info.num_samples_to_discard);
                    state->gyro_info.num_samples_to_discard--;
                }
                idx += 6;
            }
        }

        idx += header.bits.twentybits_bit ? 2 : 1; /* skip temperature data */

        /* handle and skip timestamp data */
        if (header.bits.timestamp_bit & 0x02) {
            pre_rec_cnt_16b = (uint16_t)(buf[idx] << 8 | buf[idx+1]);
            ICM4X6XX_VERB_INST_PRINTF(MED, instance, "dts1 %d ",
                                 (uint32_t)pre_rec_cnt_16b);
            idx += 2;
        }

        if (header.bits.twentybits_bit) {
            /* accel high resolution value, depends on accel_bit set or not*/
            accel_high_res[0] = (buf[0+idx] >> 4) & 0x0F;
            accel_high_res[1] = (buf[1+idx] >> 4) & 0x0F;
            accel_high_res[2] = (buf[2+idx] >> 4) & 0x0F;
            /* gyro high resolution value, depends on gyro_bit set or not*/
            gyro_high_res[0] = buf[0+idx] & 0x0F;
            gyro_high_res[1] = buf[1+idx] & 0x0F;
            gyro_high_res[2] = buf[2+idx] & 0x0F;
            idx += 3;
            ICM4X6XX_VERB_INST_PRINTF(MED, instance, "A/G high res mode");
        }

        bool is_first_one = (total_cnt == packet_cnt) ? true : false;
        bool is_last_one = (packet_cnt == 1) ? true : false;
        if (report_accel) {
#ifdef ICM4X6XX_ODR_CHANGING_SAMPLE_MISSING_SAMPLE_GAP  //for ACC-042
            if (!added_one_more_sample &&
                pre_record_intvl < 288000 &&        //pre_record_intvl intvl before change ODR, 288000=15ms
                (abs((int32_t)(meas_intvl - pre_record_intvl)) > 38400) &&
                (sample_ts - pre_odr_last_record_ts > (pre_record_intvl + meas_intvl))) {
                uint8_t max_samples = 0;
                uint8_t add_n_samples = (sample_ts - pre_odr_last_record_ts - pre_record_intvl) / meas_intvl;
                ICM4X6XX_INST_PRINTF(HIGH, instance, "add one a sample, remove this");
                while (add_n_samples &&  max_samples < 5) {
                    icm4x6xx_handle_accel_sample(instance, accel_raw, header.bits.twentybits_bit,
                        accel_high_res, sample_ts - meas_intvl * add_n_samples, true, true);
                    add_n_samples--;
                    max_samples++;
                }
            }
            added_one_more_sample = true;       //added_one_more_sample not seperate ACC/GYRO
#endif

            if (state->fifo_info.is_drm) {
				/* only the last sample will be report to fw during DRM, and use INT ts */
				if (is_last_one) {
                    icm4x6xx_handle_accel_sample(instance, accel_raw, header.bits.twentybits_bit,
                        accel_high_res, timestamp, is_first_one, is_last_one);
				} else {
                    ICM4X6XX_INST_PRINTF(HIGH, instance, "discard a sample %d:%d",
                        state->fifo_info.is_drm, is_last_one);
				}
            } else {
                icm4x6xx_handle_accel_sample(instance, accel_raw, header.bits.twentybits_bit,
                	accel_high_res, sample_ts, is_first_one, is_last_one);
            }
        }

        if (report_gyro) {
#ifdef ICM4X6XX_ODR_CHANGING_SAMPLE_MISSING_SAMPLE_GAP
            if (!added_one_more_sample &&
                pre_record_intvl < 288000 &&
                (abs((int32_t)(meas_intvl - pre_record_intvl)) > 38400) &&
                (sample_ts - pre_odr_last_record_ts > (pre_record_intvl + meas_intvl))) {
                uint8_t max_samples = 0;
                uint8_t add_n_samples = (sample_ts - pre_odr_last_record_ts - pre_record_intvl) / meas_intvl;
                ICM4X6XX_INST_PRINTF(HIGH, instance, "add one g sample, remove this");
                while (add_n_samples &&  max_samples < 5) { //max 5 samples
                    icm4x6xx_handle_gyro_sample(instance, gyro_raw, header.bits.twentybits_bit,
                        accel_high_res, sample_ts - meas_intvl * add_n_samples, true, true);
                    add_n_samples--;
                    max_samples++;
                }
            }
            added_one_more_sample = true;
#endif

            if (state->fifo_info.is_drm) {
				/* only the last sample will be report to fw during DRM, and use INT ts */
				if (is_last_one) {
					icm4x6xx_handle_gyro_sample(instance, gyro_raw, header.bits.twentybits_bit,
						gyro_high_res, timestamp, is_first_one, is_last_one);
			    } else {
					ICM4X6XX_INST_PRINTF(HIGH, instance, "discard g sample %d:%d",
						state->fifo_info.is_drm, is_last_one);
			    }
            } else {
				icm4x6xx_handle_gyro_sample(instance, gyro_raw, header.bits.twentybits_bit,
					gyro_high_res, sample_ts, is_first_one, is_last_one);
            }
        }

        if (packet_cnt == 0)
            break;
        packet_cnt--;
    }

    ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "last sample_ts: %x%08x, last_rec_ts-1111: %x%08x",
                         (uint32_t)(sample_ts >> 32), (uint32_t)sample_ts,
                         (uint32_t)(last_rec_ts >> 32), (uint32_t)last_rec_ts);

    state->pre_record_ts = sample_ts;
    state->pre_rec_cnt_16b = pre_rec_cnt_16b;

    //if ((state->sampling_intvl * 0.98 < meas_intvl) &&
      //  (state->sampling_intvl * 1.02 > meas_intvl))
        state->pre_meas_intvl = meas_intvl;
    //else
      //  ICM4X6XX_INST_PRINTF(HIGH, instance, "don't set new intvl"); // don't set new intvl

CLEAN_STATUS:
#ifdef ICM4X6XX_DISABLE_DAE
    {
        uint8_t int_status;
        icm4x6xx_read_int_status(instance, &int_status);
    }
#endif

#ifndef ICM4X6XX_DISABLE_DAE
    if(!icm4x6xx_dae_if_available(instance))
        icm4x6xx_send_int_clear_msg(instance);
#endif
}

void icm4x6xx_start_sensor_temp_polling_timer(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    uint8_t buffer[50];
    sns_rc rc = SNS_RC_SUCCESS;
    sns_request timer_req = {
        .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
        .request    = buffer
    };

    if(NULL == state->timer_data_stream) {
        sns_service_manager *smgr = this->cb->get_service_manager(this);
        sns_stream_service *strm_svc =
            (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
        rc = strm_svc->api->create_sensor_instance_stream(strm_svc, this,
            state->timer_suid, &state->timer_data_stream);
    }

    if (rc != SNS_RC_SUCCESS ||
        NULL == state->timer_data_stream) {
        ICM4X6XX_INST_PRINTF(ERROR, this, "E %d", rc);
        return;
    }

    req_payload.is_periodic = true;
    req_payload.start_time = sns_get_system_time();
    req_payload.timeout_period = state->sensor_temp_info.sampling_intvl;

    ICM4X6XX_INST_PRINTF(HIGH, this, "temp timer intvl: %x%08x",
                     (uint32_t)(req_payload.timeout_period >> 32),
                        (uint32_t)req_payload.timeout_period);

    ICM4X6XX_INST_PRINTF(HIGH, this, "start timer for temp");
    timer_req.request_len = pb_encode_request(buffer,
                                              sizeof(buffer),
                                              &req_payload,
                                              sns_timer_sensor_config_fields,
                                              NULL);
    if(timer_req.request_len > 0) {
        state->timer_data_stream->api->send_request(state->timer_data_stream, &timer_req);
        state->sensor_temp_info.cur_sampling_rate_hz = state->sensor_temp_info.sampling_rate_hz;
        state->sensor_temp_info.timer_is_active = true;
    }
}

/**
 * Updates temp sensor polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */
void icm4x6xx_set_polling_config(sns_sensor_instance *const this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    if(state->sensor_temp_info.sampling_intvl > 0) {
        //if(!state->sensor_temp_info.timer_is_active) // TODO: check does one data_stream means one client, and one stream config again means reconfig
        icm4x6xx_start_sensor_temp_polling_timer(this);
    } else if(state->sensor_temp_info.timer_is_active) {
        state->sensor_temp_info.timer_is_active = false;
        state->sensor_temp_info.cur_sampling_rate_hz = 0.0f;
        sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
    }
}

void icm4x6xx_handle_sensor_temp_sample(
    sns_sensor_instance *const instance,
    uint64_t timeout_ticks)
{
    int16_t temp_raw = 0;

    icm4x6xx_read_temp_data(instance, &temp_raw);
    icm4x6xx_convert_and_send_temp_sample(instance, timeout_ticks, temp_raw);
}

void icm4x6xx_register_interrupt(sns_sensor_instance *this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

#ifdef ICM4X6XX_ENABLE_I3C
    if (state->com_port_info.com_config.bus_type == SNS_BUS_I3C_SDR &&
        !state->product_info.i3c_use_int1) {
        return;
    }
#endif

    if(!state->irq_info.irq_registered) {
        sns_data_stream* data_stream = state->interrupt_data_stream;
        uint8_t buffer[20];
        sns_request irq_req = {
            .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ,
            .request    = buffer
        };

        ICM4X6XX_INST_PRINTF(HIGH, this, "reg int pin");
        sns_memset(buffer, 0, sizeof(buffer));
        irq_req.request_len = pb_encode_request(buffer,
                                                sizeof(buffer),
                                                &state->irq_info.irq_config,
                                                sns_interrupt_req_fields,
                                                NULL);
        if(irq_req.request_len > 0) {
            data_stream->api->send_request(data_stream, &irq_req);
            state->irq_info.irq_registered = true;
        }
    }
}

#ifdef ICM4X6XX_ENABLE_LOWG
void icm4x6xx_handle_freefall_interrupt(
     sns_sensor_instance *const instance,
     sns_time irq_timestamp)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    sns_diag_service* diag = state->diag_service;

    state->freefall_info.free_fall_state.free_fall_event_type = SNS_FREE_FALL_EVENT_TYPE_FIRED;
    pb_send_event(instance,
                  sns_free_fall_event_fields,
                  &state->freefall_info.free_fall_state,
                  irq_timestamp,
                  SNS_FREE_FALL_MSGID_SNS_FREE_FALL_EVENT,
                  &state->freefall_info.suid);

    ICM4X6XX_INST_PRINTF(HIGH, instance, "freefall fired");

    icm4x6xx_en_freefall(instance, false);

    state->freefall_info.enable_free_fall = false;

    // Sensor State HW Interrupt Log
    sns_diag_sensor_state_interrupt *log =
        (sns_diag_sensor_state_interrupt *)diag->api->alloc_log(
            diag,
            instance,
            &state->freefall_info.suid,
            sizeof(sns_diag_sensor_state_interrupt),
            SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT);

    if(NULL != log) {
        log->interrupt = SNS_DIAG_INTERRUPT_FREE_FALL;
        log->timestamp = irq_timestamp;
        diag->api->submit_log(diag,
                              instance,
                              &state->freefall_info.suid,
                              sizeof(sns_diag_sensor_state_interrupt),
                              log,
                              SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT,
                              state->log_interrupt_encoded_size,
                              icm4x6xx_encode_sensor_state_log_interrupt);
    }
}
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
void icm4x6xx_handle_high_shock_interrupt(
     sns_sensor_instance *const instance,
     sns_time irq_timestamp)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    sns_diag_service* diag = state->diag_service;

    state->high_shock_info.high_shock_state.high_shock_event_type = SNS_HIGH_SHOCK_EVENT_TYPE_FIRED;
    pb_send_event(instance,
                  sns_high_shock_event_fields,
                  &state->high_shock_info.high_shock_state,
                  irq_timestamp,
                  SNS_HIGH_SHOCK_MSGID_SNS_HIGH_SHOCK_EVENT,
                  &state->high_shock_info.suid);

    ICM4X6XX_INST_PRINTF(HIGH, instance, "highshock fired");

    icm4x6xx_en_high_shock(instance, false);

    state->high_shock_info.enable_high_shock = false;

    // Sensor State HW Interrupt Log
    sns_diag_sensor_state_interrupt *log =
        (sns_diag_sensor_state_interrupt *)diag->api->alloc_log(
            diag,
            instance,
            &state->high_shock_info.suid,
            sizeof(sns_diag_sensor_state_interrupt),
            SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT);

    if(NULL != log) {
        log->interrupt = SNS_DIAG_INTERRUPT_SHOCK;
        log->timestamp = irq_timestamp;
        diag->api->submit_log(diag,
                              instance,
                              &state->high_shock_info.suid,
                              sizeof(sns_diag_sensor_state_interrupt),
                              log,
                              SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT,
                              state->log_interrupt_encoded_size,
                              icm4x6xx_encode_sensor_state_log_interrupt);
    }
}
#endif

void icm4x6xx_handle_md_interrupt(
     sns_sensor_instance *const instance,
     sns_time irq_timestamp)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    sns_diag_service* diag = state->diag_service;

    if (state->md_info.ignore_md_fired_num > 0) {
        ICM4X6XX_INST_PRINTF(MED, instance, "ignore this fired MD");
        state->md_info.ignore_md_fired_num--;
        return;
    }

    state->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_FIRED;
    pb_send_event(instance,
                  sns_motion_detect_event_fields,
                  &state->md_info.md_state,
                  irq_timestamp,
                  SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                  &state->md_info.suid);
    ICM4X6XX_INST_PRINTF(HIGH, instance, "MD fired");

    icm4x6xx_convert_accel_gated_req_to_non_gated(instance);

    // Sensor State HW Interrupt Log
    sns_diag_sensor_state_interrupt *log =
        (sns_diag_sensor_state_interrupt *)diag->api->alloc_log(
            diag,
            instance,
            &state->md_info.suid,
            sizeof(sns_diag_sensor_state_interrupt),
            SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT);

    if(NULL != log) {
        log->interrupt = SNS_DIAG_INTERRUPT_MOTION;
        log->timestamp = irq_timestamp;
        diag->api->submit_log(diag,
                              instance,
                              &state->md_info.suid,
                              sizeof(sns_diag_sensor_state_interrupt),
                              log,
                              SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT,
                              state->log_interrupt_encoded_size,
                              icm4x6xx_encode_sensor_state_log_interrupt);
    }

    if (state->md_info.md_state.motion_detect_event_type
        == SNS_MOTION_DETECT_EVENT_TYPE_FIRED) {
        state->md_info.enable_md_int = false;
        icm4x6xx_update_md_intr(instance, false, false);
        icm4x6xx_set_md_config(instance, false);
        state->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_FIRED;

        if(state->accel_info.gated_client_present) {
            state->accel_info.en_fifo = true;
#ifndef ICM4X6XX_USE_INT_TS
            state->gyro_info.en_fifo = true;
#endif

#ifdef ICM4X6XX_DISABLE_DAE
            if (state->product_info.series == HAVANA)
                state->gyro_info.en_fifo = true;
#endif

            bool en_g_fifo = state->gyro_info.en_fifo;
            bool en_a_fifo = state->accel_info.en_fifo;

            state->accel_info.actual_publish = true;
            state->accel_info.desire_publish = true;

            state->accel_info.desire_mode = icm4x6xx_choose_acc_power_mode(instance);

            if (state->product_info.use_hi_res && (en_a_fifo || en_g_fifo)) {
                state->fifo_info.desire_format = ICM4X6XX_FORMAT_20_BYTES;
            } else if (en_a_fifo && en_g_fifo) {
                state->fifo_info.desire_format = ICM4X6XX_FORMAT_16_BYTES;
            } else if (en_a_fifo) {
                state->fifo_info.desire_format = ICM4X6XX_FORMAT_ACCEL_8_BYTES;
            } else if (en_g_fifo) {
                state->fifo_info.desire_format = ICM4X6XX_FORMAT_GYRO_8_BYTES;
            } else {
                state->fifo_info.desire_format = ICM4X6XX_FORMAT_EMPTY;
                ICM4X6XX_INST_PRINTF(MED, instance, "A/G ff disable");
            }

            if (state->fifo_info.curr_format != state->fifo_info.desire_format) {
                state->fifo_info.req_chg_ff_format = true;
            }

            //if (state->accel_info.curr_mode == ICM4X6XX_A_LPM) {
                icm4x6xx_set_accel_mode(instance, ICM4X6XX_A_OFF);
            //}

            /* Gated accel ODR may be changed by MD config, so re-set */
            icm4x6xx_set_accel_odr(instance, state->accel_info.req_match_odr);

            icm4x6xx_en_fifo(instance, en_a_fifo, en_g_fifo);

            if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_EMPTY) {
                state->fifo_info.curr_format = state->fifo_info.desire_format;
                state->is_first_sample = true;
            }

            if (state->fifo_info.curr_format == state->fifo_info.desire_format) {
                /* set fifo watermark */
                icm4x6xx_set_fifo_watermark(instance);
                state->fifo_info.req_chg_ff_format = false;

                if ((state->accel_info.desire_publish ||
                    state->gyro_info.desire_publish ||
                    state->fac_test_in_progress) &&
                    state->irq_info.irq_ready)
                    icm4x6xx_en_wm_int(instance, true);

#ifndef ICM4X6XX_DISABLE_DAE
                icm4x6xx_dae_if_start_streaming(instance);
#else
                icm4x6xx_set_accel_mode(instance, ICM4X6XX_A_LNM);
#endif
            }

            if (ICM4X6XX_CONFIG_IDLE == state->config_step &&
                state->fifo_info.curr_format != state->fifo_info.desire_format) {
                state->config_step = ICM4X6XX_CONFIG_FLUSHING_HW;
#ifndef ICM4X6XX_DISABLE_DAE
                icm4x6xx_dae_if_flush_hw(instance);
#else
                icm4x6xx_flush_fifo(instance);
#endif
            } else {
                icm4x6xx_send_config_event(instance);
            }
        }
    }
}

#ifdef ICM4X6XX_DISABLE_DAE
void icm4x6xx_handle_interrupt_event(sns_sensor_instance *const instance, bool flush)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    uint8_t packet_size = 0;
    uint16_t fifo_count = 0;
    uint16_t bytes_to_read = 0;
    uint8_t int_status = 0;
    sns_time timestamp = state->interrupt_timestamp;

    if (state->ascp_request_cnt && !flush) {
        icm4x6xx_read_int_status(instance, &int_status);
        //icm4x6xx_send_int_clear_msg(instance);
        ICM4X6XX_INST_PRINTF(HIGH, instance, "ascping f %d", flush);
        return;
    } else if (state->ascp_request_cnt) {
        ICM4X6XX_INST_PRINTF(LOW, instance, "ascping f %d", flush);
        return;
    }

    icm4x6xx_get_packet_size(instance, &packet_size);
    if (packet_size == 0) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "ps 0");
    }

    icm4x6xx_read_fifo_count(instance, &fifo_count);
    if (flush && fifo_count > 0) {
        timestamp = sns_get_system_time();
        state->flush_ts = timestamp;
        ICM4X6XX_VERB_INST_PRINTF(MED, instance, "flush TS: %x%08x", (uint32_t)(timestamp >> 32), (uint32_t)timestamp);
    }

    state->is_flush = flush;

    ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "flush %d", state->is_flush);

#ifndef ICM4X6XX_USE_INT_TS
    if ((packet_size != 0) && (fifo_count != 0)) {
#ifdef ICM4X6XX_READ_TMST_IN_INT
        icm4x6xx_enable_tmst_val(instance, true);

#if 0
        // TODO: it seems in 845, this API can't work well.
        rc += state->scp_service->api->sns_scp_get_write_time(
                    state->com_port_info.port_handle, &intl_cnt_strobe_ts);
#else
        state->r_fifo_cnt_master_cnt = state->strobe_master_cnt;
#endif
        icm4x6xx_read_tmst_val(instance, &state->slave_cnt);
#endif
    }
#endif

    if (state->fifo_info.record_mode)
        bytes_to_read = fifo_count * packet_size;
    else
        bytes_to_read = fifo_count;

    ICM4X6XX_INST_PRINTF(LOW, instance, "flush %d ff cnt: %d", flush, fifo_count);

    if(bytes_to_read > 0) {
        if (bytes_to_read <= (packet_size * MAX_SCP_SAMPLES)) {
            uint8_t buf[bytes_to_read];
            icm4x6xx_read_fifo_buf(instance, buf, bytes_to_read);
            icm4x6xx_process_fifo_data_buffer(instance, buf, bytes_to_read, timestamp);

            if (state->fifo_flush_in_progress) {
                state->fifo_flush_in_progress = false;
                //icm4x6xx_en_wm_int(instance, true);

                if (state->accel_info.desire_publish &&
                    state->accel_info.actual_publish) {
                    icm4x6xx_send_fifo_flush_done(instance, &state->accel_info.suid);
                }

                if (state->gyro_info.desire_publish &&
                    state->gyro_info.actual_publish) {
                    icm4x6xx_send_fifo_flush_done(instance, &state->gyro_info.suid);
                }
            }

            if (ICM4X6XX_CONFIG_FLUSHING_HW == state->config_step &&
                state->ascp_request_cnt == 0) {
                icm4x6xx_reconfig_hw(instance);
            }

            if(state->fac_test_in_progress) {
                /** All self-tests need to be handled in normal mode. */
                state->island_service->api->sensor_instance_island_exit(state->island_service, instance);
                icm4x6xx_process_fac_test(instance);
            }
        } else {
            icm4x6xx_send_ascp_read_req(instance, REG_FIFO_DATA, bytes_to_read);
        }
    } else {
        // reset FIFO state if there are no samples to flush
        if (state->fifo_flush_in_progress && state->ascp_request_cnt == 0) {
            state->fifo_flush_in_progress = false;
            //icm4x6xx_en_wm_int(instance, true);

            if (state->accel_info.desire_publish &&
                state->accel_info.actual_publish) {
                icm4x6xx_send_fifo_flush_done(instance, &state->accel_info.suid);
            }

            if (state->gyro_info.desire_publish &&
                state->gyro_info.actual_publish) {
                icm4x6xx_send_fifo_flush_done(instance, &state->gyro_info.suid);
            }
        }

        if (ICM4X6XX_CONFIG_FLUSHING_HW == state->config_step &&
            state->ascp_request_cnt == 0) {
            icm4x6xx_reconfig_hw(instance);
        }

        if (!flush) {
            icm4x6xx_read_int_status(instance, &int_status);
            //icm4x6xx_send_int_clear_msg(instance);
        }
    }
}

static void icm4x6xx_process_ascp_vector(sns_port_vector *vector, void *user_arg)
{
    sns_sensor_instance *instance = (sns_sensor_instance *)user_arg;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;

    if(REG_FIFO_DATA == vector->reg_addr) {
        icm4x6xx_process_fifo_data_buffer(instance,
                                          vector->buffer,
                                          vector->bytes,
                                          state->is_flush ? state->flush_ts : state->interrupt_timestamp);
    } else {
        ICM4X6XX_INST_PRINTF(ERROR, instance, "E");
    }
}
#endif

static sns_rc icm4x6xx_validate_sensor_temp_odr(icm4x6xx_instance_state *state)
{
    sns_rc rc = SNS_RC_SUCCESS;

    if(state->sensor_temp_info.sampling_rate_hz <= 0.0f) {
        state->sensor_temp_info.sampling_intvl = 0;
        rc = SNS_RC_NOT_SUPPORTED;
    } else if(state->sensor_temp_info.sampling_rate_hz <= ICM4X6XX_SENSOR_TEMP_ODR_1) {
        state->sensor_temp_info.sampling_rate_hz = ICM4X6XX_SENSOR_TEMP_ODR_1;
    } else if (state->sensor_temp_info.sampling_rate_hz > ICM4X6XX_SENSOR_TEMP_ODR_1 &&
               state->sensor_temp_info.sampling_rate_hz <= ICM4X6XX_SENSOR_TEMP_ODR_5) {
        state->sensor_temp_info.sampling_rate_hz = ICM4X6XX_SENSOR_TEMP_ODR_5;
    } else {
        state->sensor_temp_info.sampling_intvl = 0;
        rc = SNS_RC_NOT_SUPPORTED;
    }

    if (rc == SNS_RC_SUCCESS) {
        state->sensor_temp_info.sampling_intvl =
          sns_convert_ns_to_ticks(1000000000.0 / state->sensor_temp_info.sampling_rate_hz);
    }

    return rc;
}

#ifdef ICM4X6XX_DISABLE_DAE
static void icm4x6xx_process_async_com_port_data_steam(sns_sensor_instance *const this)
{
    icm4x6xx_instance_state *state =
                    (icm4x6xx_instance_state*)this->state->state;
    sns_sensor_event *event;
    bool send_clr_msg = false;

    if (NULL == state->async_com_port_data_stream) {
        ICM4X6XX_INST_PRINTF(ERROR, this, "E");
        return;
    }

    event = state->async_com_port_data_stream->api->peek_input(state->async_com_port_data_stream);
    while(NULL != event) {
        if(event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR) {
            ICM4X6XX_INST_PRINTF(ERROR, this, "E %d",
                            event->message_id);
        } else if(event->message_id == SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW) {
            ICM4X6XX_VERB_INST_PRINTF(LOW, this, "received ascp event");

            pb_istream_t stream = pb_istream_from_buffer((uint8_t *)event->event, event->event_len);
            sns_ascp_for_each_vector_do(&stream, icm4x6xx_process_ascp_vector, (void *)this);

            if (state->ascp_request_cnt == 0) {
                ICM4X6XX_INST_PRINTF(ERROR, this, "strange ascp cnt");
            } else {
                state->ascp_request_cnt--;
            }

            send_clr_msg = true;

            if(state->fifo_flush_in_progress && state->ascp_request_cnt == 0) {
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

            if (ICM4X6XX_CONFIG_FLUSHING_HW == state->config_step &&
                state->ascp_request_cnt == 0) {
                icm4x6xx_reconfig_hw(this);
            }
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, this, "unkown msg id %d", event->message_id);
        }

        if(state->fac_test_in_progress) {
            /** All self-tests can be handled in normal mode. */
            state->island_service->api->sensor_instance_island_exit(state->island_service, this);
            icm4x6xx_process_fac_test(this);
        }

        event = state->async_com_port_data_stream->api->get_next_input(state->async_com_port_data_stream);
    }

    if (send_clr_msg)
        icm4x6xx_send_int_clear_msg(this);
}

static void icm4x6xx_process_interrupt_data_stream(sns_sensor_instance *const this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
    sns_sensor_event *event;
    bool send_clr_msg = false;

    if(NULL == state->interrupt_data_stream) {
        //int data stream is not created!
        ICM4X6XX_INST_PRINTF(ERROR, this, "E");
        return;
    }

    event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
    while(NULL != event) {//level trigger mode, avoid infinite loop
        if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT) {
            state->irq_info.irq_ready = true;
            ICM4X6XX_INST_PRINTF(HIGH, this, "received int reg event");

            if(state->md_info.enable_md_int)
                icm4x6xx_update_md_intr(this, true, false);

            if (state->accel_info.actual_publish ||
                state->gyro_info.actual_publish ||
                state->fac_test_in_progress)
                icm4x6xx_en_wm_int(this, true);
        } else if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT) {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                 event->event_len);
            send_clr_msg = true;

#ifdef INT_TS_SAME_WORKAROUND
            if (pb_decode(&stream, sns_interrupt_event_fields, &irq_event) &&
				(state->fifo_info.is_drm ||
                ((irq_event.timestamp > state->flush_ts) &&
                (irq_event.timestamp > state->pre_record_ts) &&
                ((irq_event.timestamp - state->interrupt_timestamp) > sns_convert_ns_to_ticks(100*1000))))) {
#else
            if (pb_decode(&stream, sns_interrupt_event_fields, &irq_event)) {
#endif

                ICM4X6XX_VERB_INST_PRINTF(LOW, this, "received interrupt event");

                if (state->md_info.enable_md_int) {
                    uint8_t int_status2 = 0;
                    icm4x6xx_read_int_status2(this, &int_status2);
                    if (int_status2 & (BIT_INT_STATUS_WOM_X | BIT_INT_STATUS_WOM_Y | BIT_INT_STATUS_WOM_Z) ||
                        int_status2 & BIT_INT_STATUS_SMD) {
                        /* 1. Handle MD interrupt: Send MD fired event to client.
                         * 2. Disable MD.
                         * 3. Start Gated Accel FIFO stream with desired config. */
                        icm4x6xx_handle_md_interrupt(this, irq_event.timestamp);
                        event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
                        continue;
                    }
                }

#if defined ICM4X6XX_ENABLE_LOWG || defined ICM4X6XX_ENABLE_HIGHG
                uint8_t int_status3 = 0;
                icm4x6xx_read_int_status3(this, &int_status3);
#endif

#ifdef ICM4X6XX_ENABLE_LOWG
                if (int_status3 & BIT_INT_STATUS3_LOW_G_DET) {
                    icm4x6xx_handle_freefall_interrupt(this, irq_event.timestamp);
                }
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
                if (int_status3 & BIT_INT_STATUS3_HIGH_G_DET) {
                    icm4x6xx_handle_high_shock_interrupt(this, irq_event.timestamp);
                }
#endif

                if (state->ascp_request_cnt == 0) {
                    state->interrupt_timestamp = irq_event.timestamp;
                    //uint8_t int_status = 0;
                    //icm4x6xx_read_int_status(this, &int_status);
                    //if (int_status & (WM_INT_MASK | OVERFLOW_INT_MASK))
                    {
                        ICM4X6XX_INST_PRINTF(MED, this, "int TS: %x%08x",
                                        (uint32_t)(state->interrupt_timestamp >> 32),
                                            (uint32_t)state->interrupt_timestamp);
                        icm4x6xx_handle_interrupt_event(this, false);
                    }
                } else {
                    ICM4X6XX_INST_PRINTF(HIGH, this, "ascping %d", state->ascp_request_cnt);
                }
            } else {
                uint8_t data = 0;
                icm4x6xx_read_int_status2(this, &data);
                icm4x6xx_read_int_status(this, &data);
#ifdef ICM4X6XX_DEBUG_VERBOSE
                ICM4X6XX_INST_PRINTF(LOW, this, "event decode fail or bad event ts");
                ICM4X6XX_INST_PRINTF(LOW, this, "pre int TS: %x%08x",
                                 (uint32_t)(state->interrupt_timestamp >> 32),
                                    (uint32_t)state->interrupt_timestamp);
                ICM4X6XX_INST_PRINTF(LOW, this, "current int TS: %x%08x",
                                 (uint32_t)(irq_event.timestamp >> 32),
                                    (uint32_t)irq_event.timestamp);
                ICM4X6XX_INST_PRINTF(LOW, this, "flush_ts: %x%08x",
                                 (uint32_t)(state->flush_ts >> 32),
                                    (uint32_t)state->flush_ts);
                ICM4X6XX_INST_PRINTF(LOW, this, "pre_record_ts: %x%08x",
                                 (uint32_t)(state->pre_record_ts >> 32),
                                    (uint32_t)state->pre_record_ts);
#endif
            }
        } else {
            ICM4X6XX_VERB_INST_PRINTF(HIGH, this, "unkown msg id %d", event->message_id);
        }

        event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
    }

    if (send_clr_msg && (state->ascp_request_cnt == 0))
        icm4x6xx_send_int_clear_msg(this);
}

static void icm4x6xx_process_sensor_temperature_data_stream(sns_sensor_instance *const this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_sensor_event *event;

    if(NULL == state->timer_data_stream) {
        ICM4X6XX_VERB_INST_PRINTF(MED, this, "sensor temperature data stream is not created!");
        return;
    }

    event = state->timer_data_stream->api->peek_input(state->timer_data_stream);
    while(NULL != event) {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                 event->event_len);
        if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT) {
            sns_timer_sensor_event timer_event;
            if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event)) {
                //received temperature timer event
                ICM4X6XX_VERB_INST_PRINTF(LOW, this, "receive t timer event");
                if (state->sensor_temp_info.desire_publish &&
                    state->sensor_temp_info.actual_publish &&
                    state->sensor_temp_info.timer_is_active &&
                    state->sensor_temp_info.sampling_intvl > 0) {
                    icm4x6xx_handle_sensor_temp_sample(this, timer_event.timeout_time);
                } else {
                    ICM4X6XX_VERB_INST_PRINTF(LOW, this, "remove t timer stream");
                    sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
                    state->sensor_temp_info.timer_is_active = false;
                    break;
                }
            }
        } else if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT) {
            /** TODO: decode and qse timer_reg_event*/
            ICM4X6XX_VERB_INST_PRINTF(LOW, this, "TIMER_SENSOR_REG_EVENT");
        } else {
            ICM4X6XX_VERB_INST_PRINTF(HIGH, this, "unkown msg id %d", event->message_id);
        }
        event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
    }
}
#endif

static void icm4x6xx_process_heart_beat_data_stream(sns_sensor_instance *const this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_sensor_event *event;

    if(NULL == state->heart_beat.timer_data_stream) {
        ICM4X6XX_VERB_INST_PRINTF(MED, this, "heart beat data stream is not created!");
        return;
    }

    event = state->heart_beat.timer_data_stream->api->peek_input(state->heart_beat.timer_data_stream);
    while(NULL != event) {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                 event->event_len);
        if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT) {
            sns_timer_sensor_event timer_event;
            if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event)) {
                ICM4X6XX_INST_PRINTF(LOW, this, "receive hb timer event"); // receive hb timer event
                if (state->accel_info.actual_publish || state->gyro_info.actual_publish) {
                    /* force soft reset when no sample in 3*ICM4X6XX_HB_ODR_COUNT odr */
                    if(state->heart_beat.heart_attack_cnt >= 2) {
                        state->heart_beat.heart_attack_cnt = 0;
                        icm4x6xx_st_store_pre_state(this);
                        /** reset device and st recover should be handled in normal mode. */
                        state->island_service->api->sensor_instance_island_exit(state->island_service, this);
                        icm4x6xx_reset_device(this);
                        icm4x6xx_st_recover_pre_state(this);
                        icm4x6xx_en_fifo(this, state->accel_info.en_fifo, state->gyro_info.en_fifo);
                        if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_EMPTY)
                            state->fifo_info.curr_format = state->fifo_info.desire_format;
                        icm4x6xx_reconfig_hw(this);
                    } else if(state->heart_beat.heart_attack_cnt >= 1) {
                        /* reset fifo if still not work */
                        icm4x6xx_reset_fifo(this);
                    } else {
                        uint8_t int_status, int_status2, int_status3;
                        icm4x6xx_read_int_status3(this, &int_status3);
                        icm4x6xx_read_int_status2(this, &int_status2);
                        icm4x6xx_read_int_status(this, &int_status);
                        icm4x6xx_send_int_clear_msg(this);
                        ICM4X6XX_INST_PRINTF(LOW, this, "int regs status 0x%x 0x%x 0x%x", int_status, int_status2, int_status3);
                    }

                    state->heart_beat.heart_attack_cnt++;
                } else {
                    ICM4X6XX_INST_PRINTF(HIGH, this, "no publish sensor"); // no publish sensor
                    sns_sensor_util_remove_sensor_instance_stream(this, &state->heart_beat.timer_data_stream);
                    break;
                }
                icm4x6xx_dump_reg(this);
            }
        } else if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT) {
            ICM4X6XX_VERB_INST_PRINTF(LOW, this, "hb timer reg");
        } else {
            ICM4X6XX_VERB_INST_PRINTF(LOW, this, "hb unkown msg id %d", event->message_id);
        }
        event = state->heart_beat.timer_data_stream->api->get_next_input(state->heart_beat.timer_data_stream);
    }
}

static void icm4x6xx_process_en_md_stream(sns_sensor_instance *const this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_sensor_event *event;

    if (NULL == state->timer_en_md_stream) {
        ICM4X6XX_VERB_INST_PRINTF(MED, this, "en_md null"); // en_md is not created
        return;
    }

    event = state->timer_en_md_stream->api->peek_input(state->timer_en_md_stream);
    while(NULL != event) {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                 event->event_len);
        if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT) {
            sns_timer_sensor_event timer_event;
            if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event)) {
                ICM4X6XX_VERB_INST_PRINTF(LOW, this, "receive md timer event");
                if (state->md_info.enable_md_int) {
                    icm4x6xx_set_smd_mode(this, OLD_MODE);
                    if (state->irq_info.irq_ready)
                        icm4x6xx_update_md_intr(this, true, false);
                } else {
                    ICM4X6XX_VERB_INST_PRINTF(LOW, this, "remove md stream");
                    sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_en_md_stream);
                    break;
                }
            }
        } else if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT) {
            ICM4X6XX_VERB_INST_PRINTF(LOW, this, "en_md timer reg");
        } else {
            ICM4X6XX_VERB_INST_PRINTF(LOW, this, "en_md unkown msg id %d", event->message_id);
        }
        event = state->timer_en_md_stream->api->get_next_input(state->timer_en_md_stream);
    }
}

static void icm4x6xx_publish_error(
            sns_sensor_instance *const this,
            icm4x6xx_sensor_type sensor_type)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_sensor_uid *suid = &state->accel_info.suid;

    if (ICM4X6XX_ACCEL == sensor_type) {
        suid = &state->accel_info.suid;
    } else if (ICM4X6XX_GYRO == sensor_type) {
        suid = &state->gyro_info.suid;
    } else if (ICM4X6XX_MOTION_DETECT == sensor_type) {
        suid = &state->md_info.suid;
    } else if (ICM4X6XX_SENSOR_TEMP == sensor_type) {
        suid = &state->sensor_temp_info.suid;
#ifdef ICM4X6XX_ENABLE_LOWG
    } else if (ICM4X6XX_FREE_FALL == sensor_type) {
        suid = &state->freefall_info.suid;
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
    } else if (ICM4X6XX_HIGH_SHOCK == sensor_type) {
        suid = &state->high_shock_info.suid;
#endif
    } else {
        return;
    }

    /* 0Hz and above MAX ODR test case need to send sns_std_error_event */
    sns_std_error_event error_event;
    error_event.error = SNS_STD_ERROR_INVALID_VALUE;

    pb_send_event(this,
                  sns_std_error_event_fields,
                  &error_event,
                  sns_get_system_time(),
                  SNS_STD_MSGID_SNS_STD_ERROR_EVENT,
                  suid);
}

static sns_rc icm4x6xx_inst_notify_event(sns_sensor_instance *const this)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;

    ICM4X6XX_VERB_INST_PRINTF(LOW, this, "inst notify event enter");

    // Turn COM port ON
    state->scp_service->api->sns_scp_update_bus_power(
        state->com_port_info.port_handle, true);

    icm4x6xx_process_en_md_stream(this);
#ifndef ICM4X6XX_DISABLE_DAE
    icm4x6xx_dae_if_process_events(this);
#else
    icm4x6xx_process_async_com_port_data_steam(this);
    icm4x6xx_process_interrupt_data_stream(this);
    icm4x6xx_process_sensor_temperature_data_stream(this);
#endif
    icm4x6xx_process_heart_beat_data_stream(this);

    // Turn COM port OFF
    state->scp_service->api->sns_scp_update_bus_power(
        state->com_port_info.port_handle, false);

    ICM4X6XX_VERB_INST_PRINTF(LOW, this, "inst notify event exit");

    return SNS_RC_SUCCESS;
}

/** See sns_sensor_instance_api::set_client_config */
static sns_rc icm4x6xx_inst_set_client_config(
              sns_sensor_instance *const this,
              sns_request const *client_request)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)this->state->state;
    sns_sync_com_port_service *scp_service = state->scp_service;
    float desired_sample_rate = 0.0;
    float desired_report_rate = 0.0;
    float desired_wakeup_sdc_rate = 0.0;
    float req_match_odr = 0.0;
    icm4x6xx_sensor_odr chosen_odr_reg_value = ODR_NOT_SUPPORTED;
    uint32_t desired_wmk = 0;
    sns_rc rv = SNS_RC_SUCCESS;
    uint8_t a_samples_to_discard = 0;
    uint8_t g_samples_to_discard = 0;
    sns_icm4x6xx_req *payload = (sns_icm4x6xx_req*)client_request->request;
    float *fac_cal_bias = NULL;
    matrix3 *fac_cal_corr_mat = NULL;

    //current driver version
    ICM4X6XX_INST_PRINTF(MED, this, "drv version 0x%04x",
                      ICM4X6XX_DRIVER_VERSION);

    // Turn COM port ON
    scp_service->api->sns_scp_update_bus_power(
              state->com_port_info.port_handle,
              true);

#ifdef ICM4X6XX_DISABLE_DAE
    icm4x6xx_register_interrupt(this);
#else
    if(!icm4x6xx_dae_if_available(this))
        icm4x6xx_register_interrupt(this);
#endif

    if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG) {
        // 1. Extract sample, report rates from client_request.
        // 2. Configure sensor HW.
        // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
        // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
        // 5. Save the current config information like type, sample_rate, report_rate, etc.
        bool en_a_fifo = state->accel_info.en_fifo;
        bool en_g_fifo = state->gyro_info.en_fifo;
        uint16_t max_wmk = state->product_info.fifo_size / SNS_MAX(1, ((en_a_fifo ? 1 : 0) + (en_g_fifo ? 1 : 0)));
        ICM4X6XX_INST_PRINTF(MED, this, "a/g desire en fifo %d %d, max_wmk %d",
                          en_a_fifo, en_g_fifo, max_wmk);

        desired_sample_rate = payload->desired_sample_rate;
        desired_report_rate = payload->desired_report_rate;
        desired_wakeup_sdc_rate = payload->desired_wakeup_sdc_rate;
        ICM4X6XX_INST_PRINTF(MED, this, "desire sample/report/wakeup_sdc rate %d %d %d",
                             (uint32_t)desired_sample_rate, (uint32_t)desired_report_rate,
                             (uint32_t)desired_wakeup_sdc_rate);

        if(desired_report_rate > desired_sample_rate) {
            // bad request. Return error or default report_rate to sample_rate
            desired_report_rate = desired_sample_rate;
        }

        state->reset_first_sample = false;

        rv = icm4x6xx_get_best_match_odr(state,
                                         desired_sample_rate,
                                         &req_match_odr,
                                         &chosen_odr_reg_value,
                                         &a_samples_to_discard,
                                         &g_samples_to_discard);
        ICM4X6XX_INST_PRINTF(MED, this, "best match odr %d(%dth)",
                        (uint32_t)req_match_odr, chosen_odr_reg_value);

        /**
         * accel or gyro error handle:
         *   1) above Max ODR: ODR match return error
         *   2) 0hz: a/g resample rate is 0 or MD test, MD doesn't have 0hz test
         *   ERROR log level  lead to acceptance test FAIL:  M-EF Total error/fatal messages
         */
        if((rv != SNS_RC_SUCCESS ||
           (FLT_EQUAL(0, desired_sample_rate) && !state->md_info.md_client_present))) {
            /* sensor_temp depends on accel too */
            if (state->accel_info.desire_en && !state->sensor_temp_info.desire_publish)
            {
                state->accel_info.desire_en = false;
                state->accel_info.desire_publish = false;
                icm4x6xx_publish_error(this, ICM4X6XX_ACCEL);
                ICM4X6XX_INST_PRINTF(HIGH, this, "Unsupported ODR rv=%d", rv);
            }
            if (state->gyro_info.desire_en)
            {
                state->gyro_info.desire_en = false;
                state->gyro_info.desire_publish = false;
                icm4x6xx_publish_error(this, ICM4X6XX_GYRO);
                ICM4X6XX_INST_PRINTF(HIGH, this, "Unsupported ODR rv=%d", rv);
            }
        }

        if(state->sensor_temp_info.desire_publish) {
            rv = icm4x6xx_validate_sensor_temp_odr(state);
            if(rv != SNS_RC_SUCCESS) {
                //sensor_temp ODR match error, TODO: error handle
                ICM4X6XX_INST_PRINTF(HIGH, this, "Unsupported ODR rv=%d", rv);
                state->sensor_temp_info.desire_publish = false;
                icm4x6xx_publish_error(this, ICM4X6XX_SENSOR_TEMP);
            }
        } else {
            state->sensor_temp_info.sampling_intvl = 0;
            state->sensor_temp_info.sampling_rate_hz = 0.0f;
        }

        // motion detect stream removing
        if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_EMPTY &&
            state->accel_info.is_enabled &&
            !state->accel_info.actual_publish) {
            icm4x6xx_set_accel_mode(this, ICM4X6XX_A_OFF);
        }

        if ((state->accel_info.is_enabled == false &&
            state->accel_info.desire_en == true) ||
            state->accel_info.curr_mode == ICM4X6XX_A_LPM) {
            state->accel_info.num_samples_to_discard = a_samples_to_discard;
            state->st_resume.a_samples_to_discard = a_samples_to_discard;
            ICM4X6XX_INST_PRINTF(HIGH, this, "a discard %d", a_samples_to_discard);
        }

        if (state->gyro_info.is_enabled == false && state->gyro_info.desire_en == true) {
            state->gyro_info.num_samples_to_discard = g_samples_to_discard;
            state->st_resume.g_samples_to_discard = g_samples_to_discard;
            ICM4X6XX_INST_PRINTF(HIGH, this, "g discard %d", g_samples_to_discard);
        }

        if (desired_report_rate != 0.0f)
            desired_wmk = (uint32_t)(req_match_odr / desired_wakeup_sdc_rate);
        else
            desired_wmk = 1;

        state->fifo_info.max_requested_wmk =
            (desired_report_rate > 0.0f) ? (uint32_t)(req_match_odr / desired_report_rate) : UINT32_MAX;

#ifdef ICM4X6XX_1K_2_500HZ
        if (state->do_1K_2_500_dec) {
            if (FLT_EQUAL(req_match_odr, ICM4X6XX_ODR_1000) && (desired_wmk == 1)) {
                desired_wmk = 2;
                ICM4X6XX_INST_PRINTF(LOW, this, "set wmk to 2"); // set wmk to 2
            } else if (FLT_EQUAL(req_match_odr, 1000.0) && (desired_wmk % 2)) {
                desired_wmk -= 1;
                ICM4X6XX_INST_PRINTF(HIGH, this, "adjust wmk to %u", desired_wmk);
            }

            if (FLT_EQUAL(req_match_odr, ICM4X6XX_ODR_1000)) {
                if (state->fifo_info.max_requested_wmk==1) {
                    state->fifo_info.max_requested_wmk = 2;
                } else if (state->fifo_info.max_requested_wmk % 2) {
                    state->fifo_info.max_requested_wmk -= 1;
                }
            }
        }
#endif

        desired_wmk = icm4x6xx_get_best_watermark(this, max_wmk, desired_wmk);

        /* Force set wm=1 to Workaround of DRI for DRM(VR) */
        if (FLT_EQUAL(req_match_odr, ICM4X6XX_ODR_1000)) {
            ICM4X6XX_INST_PRINTF(MED, this, "is_drm:%d wm:%u", state->fifo_info.is_drm, desired_wmk);
            state->fifo_info.is_drm = true;
            desired_wmk = 1;
        } else {
            state->fifo_info.is_drm = false;
        }

        ICM4X6XX_INST_PRINTF(MED, this, "desire/dae wmk %u %u", desired_wmk, state->fifo_info.max_requested_wmk);

        state->fifo_info.desire_wmk = desired_wmk;
        state->accel_info.req_match_odr = req_match_odr;
        state->gyro_info.req_match_odr = req_match_odr;
        state->accel_info.desire_odr_reg = chosen_odr_reg_value;
        state->gyro_info.desire_odr_reg = chosen_odr_reg_value;

        state->fifo_info.max_requested_flush_ticks = payload->desired_flush_ticks;

        if (state->product_info.use_hi_res && (en_a_fifo || en_g_fifo)) {
            state->fifo_info.desire_format = ICM4X6XX_FORMAT_20_BYTES;
        } else if (en_a_fifo && en_g_fifo) {
            state->fifo_info.desire_format = ICM4X6XX_FORMAT_16_BYTES;
        } else if (en_a_fifo) {
            state->fifo_info.desire_format = ICM4X6XX_FORMAT_ACCEL_8_BYTES;
        } else if (en_g_fifo) {
            state->fifo_info.desire_format = ICM4X6XX_FORMAT_GYRO_8_BYTES;
        } else {
            state->fifo_info.desire_format = ICM4X6XX_FORMAT_EMPTY;
            ICM4X6XX_INST_PRINTF(MED, this, "A/G fifo disable");
        }

        ICM4X6XX_INST_PRINTF(MED, this, "desire a/g %d %d, fifo format %d",
                             state->accel_info.desire_en,
                             state->gyro_info.desire_en,
                             state->fifo_info.desire_format);

        if (state->fac_test_in_progress) {
            icm4x6xx_st_store_pre_state(this);
            ICM4X6XX_INST_PRINTF(MED, this, "fac_tsting");
            goto EXIT;
        }

        if (state->fifo_info.curr_format != state->fifo_info.desire_format ||
            state->config_step != ICM4X6XX_CONFIG_IDLE) {
            /* Program the new FIFO configuration
             * this configuration will take effect, when fifo is empty
             * This step configiuration is not protected with async config
             * but we will checking desire format changing successfully or not */
            rv += icm4x6xx_en_fifo(this, en_a_fifo, en_g_fifo);
            state->fifo_info.req_chg_ff_format = true;
        }

        if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_EMPTY) {
            state->fifo_info.curr_format = state->fifo_info.desire_format;
            state->reset_first_sample = true;    // fifo not enable is the begin of first sample
        }

#ifndef ICM4X6XX_DISABLE_DAE
        //state->reset_first_sample |= state->fifo_info.flush_only;

        if (state->dae_if.ag.state == INIT_PENDING || state->dae_if.temp.state == INIT_PENDING || !state->irq_info.irq_ready) {
            ICM4X6XX_INST_PRINTF(HIGH, this, "waiting DAE init done");
            state->config_step = ICM4X6XX_CONFIG_UPDATING_HW;
        }

        if (!state->accel_info.desire_en && !state->gyro_info.desire_en) {
            icm4x6xx_dae_if_stop_streaming(this);
        }
#endif

        if (icm4x6xx_need_flush_fifo(this) && state->config_step == ICM4X6XX_CONFIG_IDLE) {  //format changed
            state->config_step = ICM4X6XX_CONFIG_FLUSHING_HW;
            state->reset_first_sample |= state->odr_changed;
            icm4x6xx_set_hw_wo_stop_fifo(this);
            icm4x6xx_send_config_event(this);
#ifdef ICM4X6XX_DISABLE_DAE
            icm4x6xx_flush_fifo(this);
#else
            //icm4x6xx_en_wm_int(this, false);
            icm4x6xx_dae_if_flush_hw(this);
#endif
        } else if (state->config_step == ICM4X6XX_CONFIG_IDLE) {  //format not changed
            icm4x6xx_reconfig_hw(this);
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, this, "config_step %d", state->config_step);
            icm4x6xx_send_config_event(this);
        }

        // update registry configuration
        if((ICM4X6XX_ACCEL == payload->registry_cfg.sensor_type)
            && payload->registry_cfg.version >= state->accel_registry_cfg.version)
        {
            fac_cal_bias = state->accel_registry_cfg.fac_cal_bias;
            fac_cal_corr_mat = &state->accel_registry_cfg.fac_cal_corr_mat;
            state->accel_registry_cfg.version = payload->registry_cfg.version;
        }
        else if((ICM4X6XX_GYRO == payload->registry_cfg.sensor_type)
            && payload->registry_cfg.version >= state->gyro_registry_cfg.version)
        {
            fac_cal_bias = state->gyro_registry_cfg.fac_cal_bias;
            fac_cal_corr_mat = &state->gyro_registry_cfg.fac_cal_corr_mat;
            state->gyro_registry_cfg.version = payload->registry_cfg.version;
        }
        else if((ICM4X6XX_SENSOR_TEMP == payload->registry_cfg.sensor_type)
            && payload->registry_cfg.version >= state->sensor_temp_registry_cfg.version)
        {
            fac_cal_bias = state->sensor_temp_registry_cfg.fac_cal_bias;
            fac_cal_corr_mat = &state->sensor_temp_registry_cfg.fac_cal_corr_mat;
            state->sensor_temp_registry_cfg.version = payload->registry_cfg.version;
        }

        if(NULL!= fac_cal_bias && NULL != fac_cal_corr_mat)
        {
            sns_memscpy(fac_cal_bias, sizeof(payload->registry_cfg.fac_cal_bias),
                      payload->registry_cfg.fac_cal_bias,
                      sizeof(payload->registry_cfg.fac_cal_bias));

            sns_memscpy(fac_cal_corr_mat, sizeof(payload->registry_cfg.fac_cal_corr_mat),
                      &payload->registry_cfg.fac_cal_corr_mat,
                      sizeof(payload->registry_cfg.fac_cal_corr_mat));
        }
    }
    else if(client_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
    {
        if (!state->fifo_flush_in_progress) {
            state->fifo_flush_in_progress = true;
            icm4x6xx_flush_fifo(this);
        }
    }
    else if(client_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
    {
        /** All self-tests can be handled in normal mode. */
        state->island_service->api->sensor_instance_island_exit(state->island_service, this);
        icm4x6xx_run_self_test(this);
        state->new_self_test_request = false;
    }

EXIT:
    // Turn COM port OFF
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);

    return SNS_RC_SUCCESS;
}

/** Public Data Definitions. */
sns_sensor_instance_api icm4x6xx_sensor_instance_api =
{
    .struct_len        = sizeof(sns_sensor_instance_api),
    .init              = &icm4x6xx_inst_init,
    .deinit            = &icm4x6xx_inst_deinit,
    .set_client_config = &icm4x6xx_inst_set_client_config,
    .notify_event      = &icm4x6xx_inst_notify_event
};
