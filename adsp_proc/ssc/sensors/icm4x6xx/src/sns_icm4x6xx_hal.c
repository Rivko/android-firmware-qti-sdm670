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

#include "sns_sync_com_port_service.h"
#include "sns_types.h"
#include "sns_icm4x6xx_hal.h"
#ifdef ICM4X6XX_ENABLE_DEBUG_I3C
#include "sns_mem_util.h"
#endif

#if defined ICM4X6XX_ENABLE_LOWG || defined ICM4X6XX_ENABLE_HIGHG
#if (defined ICM4X6XX_ONE_SHOT_WRITE_DL_SRAM_FW) || (defined ICM4X6XX_ONESHOT_ONE_VEC_DL_SRAM_FW) || (defined ICM4X6XX_SEVERAL_TIMES_WRITE_DL_STATIC_MEM)
static uint8_t dmp_code_buffer[3736] = {0}; // 3736 = sizeof(dmp_image) * 4 + 4 * 2
#elif defined ICM4X6XX_ONESHOT_MULTI_VEC_DL_SRAM_FW
static sns_port_vector dmp_code_port_vec[1868];
static uint8_t data_buff[1868] = {0};
#endif
static uint8_t dmp_image[] = {
    #include "sns_icm4x6xx_dmp_sram_img.h"
    };

static int icm4x6xx_load_dmp_sram_code(sns_sensor_instance *instance)
{
    int rc = SNS_RC_SUCCESS;
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    /* Custom SRAM image containing Tilt/HighG/LowG */
    const uint8_t *dmp_prog = dmp_image;
    uint32_t start_offset = 16;
    uint32_t size = sizeof(dmp_image)/sizeof(dmp_image[0]);

    ICM4X6XX_INST_PRINTF(LOW, instance, "dl freefall fw");

    /* use special SPI speed for freefall FW download */
    uint32_t config_min_bus_speed_KHz = state->com_port_info.com_config.min_bus_speed_KHz;
    uint32_t config_max_bus_speed_KHz = state->com_port_info.com_config.max_bus_speed_KHz;
    state->com_port_info.com_config.min_bus_speed_KHz = 3100;
    state->com_port_info.com_config.max_bus_speed_KHz = 3900;
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
    // deregister and register again, for updating SPI speed
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->com_port_info.port_handle = NULL;
    state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                              &state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

	/* DMP SRAM is updated by DMP DMA every time accel ODR triggers, hence
	 * first 7B of SRAM are not usable.
	 * Since start addresses are aligned on 32B binary, there is some loss
	 * here (minor however).
	 */
	if(size + start_offset > 1024U) {
        ICM4X6XX_INST_PRINTF(ERROR, instance, "incorrect sram code size");
		return -1;
    }

    uint8_t pwr_value = 0x0;
    uint32_t xfer_bytes;
    icm4x6xx_com_read_wrapper(instance, REG_PWR_MGMT_0, &pwr_value, 1, &xfer_bytes);
    if (pwr_value & ACCEL_LNM_MASK) {
  	    /* Writing DMP SRAM not supported when accel is not IDLE */
        ICM4X6XX_INST_PRINTF(ERROR, instance, "accel is running");
        return -1;
    }

    uint8_t data = pwr_value | ICM426XX_IDLE_MASK;
    icm4x6xx_com_write_wrapper(instance, REG_PWR_MGMT_0, &data, 1, &xfer_bytes, false);
    /* I also don't know why wait 200us here , sample code have that delay here, and no documents */
    sns_busy_wait(sns_convert_ns_to_ticks(200 * 1000));

    //icm4x6xx_reset_dmp(instance);

	data = BIT_DMP_MEM_ACCESS_EN;
	rc |= icm4x6xx_com_write_wrapper(instance, REG_SCAN0, &data, 1, &xfer_bytes, false);
	sns_busy_wait(sns_convert_ns_to_ticks(100 * 1000));

	/* Write new DMP program into SRAM */
    uint8_t dmp_sram_offset = 0x40;
    uint32_t i = 0, cur_idx = 0;

#ifdef ICM4X6XX_ONESHOT_SIMPLE_RW_DL_SRAM_FW
    uint32_t offset = start_offset;
    sns_sync_com_port_service *scp_service = state->scp_service;
    sns_sync_com_port_handle *port_handle = state->com_port_info.port_handle;

    while (i < sizeof(dmp_code_buffer) && offset < (size+start_offset) && cur_idx < size) {
        if ((0 == (offset % 256)) || (0 == i)) {
            dmp_code_buffer[i] = REG_MEM_BANK_SEL;
            dmp_code_buffer[i+1] = dmp_sram_offset + offset / 256;
            i += 2;
        }
        dmp_code_buffer[i + 0] = REG_MEM_START_ADDR;
        dmp_code_buffer[i + 1] = offset % 256;
        dmp_code_buffer[i + 2] = REG_MEM_R_W;
        dmp_code_buffer[i + 3] = dmp_prog[cur_idx];
        i += 4;
        cur_idx++;
        offset++;
    }

	if(cur_idx != size) {
		rc = -1;
		goto end_load_dmp_sram;
	}

    scp_service->api->sns_scp_simple_rw(port_handle, true, false, dmp_code_buffer, i, &xfer_bytes);
    ICM4X6XX_INST_PRINTF(MED, instance, "size %d i %d write %d",size, i, xfer_bytes);
#elif defined ICM4X6XX_ONESHOT_ONE_VEC_DL_SRAM_FW
    uint32_t offset = start_offset;
    sns_sync_com_port_service *scp_service = state->scp_service;
    sns_sync_com_port_handle *port_handle = state->com_port_info.port_handle;

    while (i < sizeof(dmp_code_buffer) && offset < (size+start_offset) && cur_idx < size) {
        if ((0 == (offset % 256)) || (0 == i)) {
            dmp_code_buffer[i] = REG_MEM_BANK_SEL;
            dmp_code_buffer[i+1] = dmp_sram_offset + offset / 256;
            i += 2;
        }
        dmp_code_buffer[i + 0] = REG_MEM_START_ADDR;
        dmp_code_buffer[i + 1] = offset % 256;
        dmp_code_buffer[i + 2] = REG_MEM_R_W;
        dmp_code_buffer[i + 3] = dmp_prog[cur_idx];
        i += 4;
        cur_idx++;
        offset++;
    }

	if(cur_idx != size) {
		rc = -1;
		goto end_load_dmp_sram;
	}

    sns_port_vector port_vec;

    port_vec.buffer = &(dmp_code_buffer[1]);
    port_vec.bytes = i - 1 ;
    port_vec.is_write = true;
    port_vec.reg_addr = dmp_code_buffer[0];

    rc += scp_service->api->sns_scp_register_rw(port_handle,
                                                &port_vec,
                                                1,
                                                false,
                                                &xfer_bytes);
    ICM4X6XX_INST_PRINTF(MED, instance, "size %d i %d write %d",size, i, xfer_bytes);
#elif defined ICM4X6XX_SEVERAL_TIMES_WRITE_DL_STACK_MEM
    uint32_t offset = start_offset;
    sns_sync_com_port_service *scp_service = state->scp_service;
    sns_sync_com_port_handle *port_handle = state->com_port_info.port_handle;
    uint16_t write_times = 0;

    #define NUM_BYTES_WRITE_FOR_ONESHOT (255) // must be odd number, 1023,255 has been test, takes about 20ms
    uint8_t dmp_code_buffer[NUM_BYTES_WRITE_FOR_ONESHOT+1];
    uint16_t dmp_code_buffer_idx = 0;

    while (dmp_code_buffer_idx < sizeof(dmp_code_buffer) && offset < (size+start_offset) && cur_idx < size) {
        if ((0 == (offset % 256)) || (0 == i)) {
            dmp_code_buffer[dmp_code_buffer_idx] = REG_MEM_BANK_SEL;
            dmp_code_buffer[dmp_code_buffer_idx+1] = dmp_sram_offset + offset / 256;
            i += 2;
            dmp_code_buffer_idx +=2;
        }
        dmp_code_buffer[dmp_code_buffer_idx + 0] = REG_MEM_START_ADDR;
        dmp_code_buffer[dmp_code_buffer_idx + 1] = offset % 256;
        dmp_code_buffer[dmp_code_buffer_idx + 2] = REG_MEM_R_W;
        dmp_code_buffer[dmp_code_buffer_idx + 3] = dmp_prog[cur_idx];
        i += 4;
        dmp_code_buffer_idx += 4;
        cur_idx++;
        offset++;

        if (dmp_code_buffer_idx >= (NUM_BYTES_WRITE_FOR_ONESHOT+1-2-4) || cur_idx == size) {
            sns_port_vector port_vec;
            port_vec.buffer = &(dmp_code_buffer[1]);
            port_vec.bytes = dmp_code_buffer_idx-1;
            port_vec.is_write = true;
            port_vec.reg_addr = dmp_code_buffer[0];

            rc += scp_service->api->sns_scp_register_rw(port_handle,
                                                        &port_vec,
                                                        1,
                                                        false,
                                                        &xfer_bytes);

            write_times++;
            //ICM4X6XX_INST_PRINTF(MED, instance, "size %d i %d write %d, write_times %d",size, i, xfer_bytes, write_times);
            dmp_code_buffer_idx = 0;
        }
    }

	if(cur_idx != size) {
		rc = -1;
		goto end_load_dmp_sram;
	}
#elif defined ICM4X6XX_SEVERAL_TIMES_WRITE_DL_STATIC_MEM
    uint32_t offset = start_offset;
    sns_sync_com_port_service *scp_service = state->scp_service;
    sns_sync_com_port_handle *port_handle = state->com_port_info.port_handle;
    uint16_t remain_bytes_to_w = 0;
    uint16_t bytes_already_w = 0;

    #define NUM_BYTES_WRITE_FOR_ONESHOT (255) // must be odd number, 1023,255 has been test

    while (i < sizeof(dmp_code_buffer) && offset < (size+start_offset) && cur_idx < size) {
        if ((0 == (offset % 256)) || (0 == i)) {
            dmp_code_buffer[i] = REG_MEM_BANK_SEL;
            dmp_code_buffer[i+1] = dmp_sram_offset + offset / 256;
            i += 2;
        }
        dmp_code_buffer[i + 0] = REG_MEM_START_ADDR;
        dmp_code_buffer[i + 1] = offset % 256;
        dmp_code_buffer[i + 2] = REG_MEM_R_W;
        dmp_code_buffer[i + 3] = dmp_prog[cur_idx];
        i += 4;
        cur_idx++;
        offset++;
    }

	if(cur_idx != size) {
		rc = -1;
		goto end_load_dmp_sram;
	}

    sns_port_vector port_vec;

    remain_bytes_to_w = i;

    while (remain_bytes_to_w > 0) {
        port_vec.buffer = &(dmp_code_buffer[bytes_already_w + 1]);
        port_vec.bytes = (remain_bytes_to_w > NUM_BYTES_WRITE_FOR_ONESHOT) ? NUM_BYTES_WRITE_FOR_ONESHOT : remain_bytes_to_w;
        port_vec.is_write = true;
        port_vec.reg_addr = dmp_code_buffer[bytes_already_w];

        bytes_already_w += port_vec.bytes + 1;
        if (i > bytes_already_w)
            remain_bytes_to_w = i - bytes_already_w;
        else
            remain_bytes_to_w = 0;

        //ICM4X6XX_INST_PRINTF(MED, instance, "bytes write %d, remaining %d", bytes_already_w, remain_bytes_to_w);

        rc += scp_service->api->sns_scp_register_rw(port_handle,
                                                    &port_vec,
                                                    1,
                                                    false,
                                                    &xfer_bytes);
        ICM4X6XX_INST_PRINTF(MED, instance, "size %d i %d write %d",size, i, xfer_bytes);
    }
#elif defined ICM4X6XX_ONESHOT_MULTI_VEC_DL_SRAM_FW
    uint32_t offset = start_offset;
    sns_sync_com_port_service *scp_service = state->scp_service;
    sns_sync_com_port_handle *port_handle = state->com_port_info.port_handle;

    while (i < ARR_SIZE(dmp_code_port_vec) && offset < (size+start_offset) && cur_idx < size) {
        if ((0 == (offset % 256)) || (0 == i)) {
            //dmp_code_buffer[i] = REG_MEM_BANK_SEL;
            //dmp_code_buffer[i+1] = dmp_sram_offset + offset / 256;
            data_buff[i] = dmp_sram_offset + offset / 256;
            dmp_code_port_vec[i].buffer = &(data_buff[i]);
            dmp_code_port_vec[i].bytes = 1;
            dmp_code_port_vec[i].is_write = true;
            dmp_code_port_vec[i].reg_addr = REG_MEM_BANK_SEL;
            i++;
        }
        //dmp_code_buffer[i + 0] = REG_MEM_START_ADDR;
        //dmp_code_buffer[i + 1] = offset % 256;
        data_buff[i] = offset % 256;
        dmp_code_port_vec[i].buffer = &(data_buff[i]);
        dmp_code_port_vec[i].bytes = 1;
        dmp_code_port_vec[i].is_write = true;
        dmp_code_port_vec[i].reg_addr = REG_MEM_START_ADDR;
        i++;

        //dmp_code_buffer[i + 2] = REG_MEM_R_W;
        //dmp_code_buffer[i + 3] = dmp_prog[cur_idx];
        data_buff[i] = dmp_prog[cur_idx];
        dmp_code_port_vec[i].buffer = &(data_buff[i]);
        dmp_code_port_vec[i].bytes = 1;
        dmp_code_port_vec[i].is_write = true;
        dmp_code_port_vec[i].reg_addr = REG_MEM_R_W;

        i++;
        cur_idx++;
        offset++;
    }

	if(cur_idx != size) {
		rc = -1;
		goto end_load_dmp_sram;
	}

    rc += scp_service->api->sns_scp_register_rw(port_handle,
                                                dmp_code_port_vec,
                                                i-1,
                                                false,
                                                &xfer_bytes);

    ICM4X6XX_INST_PRINTF(MED, instance, "size %d i %d write %d",size, i, xfer_bytes);
#else
	data = dmp_sram_offset;
	icm4x6xx_com_write_wrapper(instance, REG_MEM_BANK_SEL, &data, 1, &xfer_bytes, false);

	cur_idx = 0;
	for(i = start_offset; i < (size+start_offset); i++) {
		if(0 == (i % 256)) {
			/* Start filling new DMP SRAM bank */
			data = dmp_sram_offset + (i/256);
			rc |= icm4x6xx_com_write_wrapper(instance, REG_MEM_BANK_SEL, &data, 1, &xfer_bytes, false);
		}

		data = i % 256;
		rc |= icm4x6xx_com_write_wrapper(instance, REG_MEM_START_ADDR, &data, 1, &xfer_bytes, false);
		data = dmp_prog[cur_idx];
		rc |= icm4x6xx_com_write_wrapper(instance, REG_MEM_R_W, &data, 1, &xfer_bytes, false);
		cur_idx++;
	}

	if(cur_idx != size) {
		rc = -1;
		goto end_load_dmp_sram;
	}
#endif

#ifdef ICM4X6XX_VERIFY_SRAM_FW_DL
	/* Verify SRAM content is matching loaded DMP program */
	cur_idx = 0;
	i = start_offset;
	data = dmp_sram_offset;
	rc |= icm4x6xx_com_write_wrapper(instance, REG_MEM_BANK_SEL, &data, 1, &xfer_bytes, false);
	while(i < (size+start_offset)) {
		/* Handle MEM_BANK_SEL switch on 256 multiples */
		if(0 == (i % 256)) {
			data = dmp_sram_offset + (i/256);
			rc |= icm4x6xx_com_write_wrapper(instance, REG_MEM_BANK_SEL, &data, 1, &xfer_bytes, false);
		}
		/* Read bytes [0-255] of each bank which was written */
		data = i % 256;
		rc |= icm4x6xx_com_write_wrapper(instance, REG_MEM_START_ADDR, &data, 1, &xfer_bytes, false);
		rc |= icm4x6xx_com_read_wrapper(instance, REG_MEM_R_W, &data, 1, &xfer_bytes);
		/* Check SRAM content match byte-to-byte */
		if(data != dmp_prog[cur_idx]) {
			rc = -1;
            ICM4X6XX_INST_PRINTF(MED, instance, "freefall FW DL failed %d %d", cur_idx, i);
			goto end_load_dmp_sram;
		}

		i++;
		cur_idx++;
	}
#endif

	data = 0;
	rc |= icm4x6xx_com_write_wrapper(instance, REG_SCAN0, &data, 1, &xfer_bytes, false);
	sns_busy_wait(sns_convert_ns_to_ticks(100 * 1000));
	if(!rc) {
		// Succesful SRAM load, execution is possible
        ICM4X6XX_INST_PRINTF(MED, instance, "freefall FW DL successfully");
	}

	/* Update DMP program counter to start from SRAM */
    data = 3;
    icm4x6xx_com_write_wrapper(instance, REG_BANK_SEL, &data, 1, &xfer_bytes, false);
	data = 0x8;
	rc |= icm4x6xx_com_write_wrapper(instance, 0x71, &data, 1, &xfer_bytes, false);
    data = 0;
    icm4x6xx_com_write_wrapper(instance, REG_BANK_SEL, &data, 1, &xfer_bytes, false);

end_load_dmp_sram:
	rc |= icm4x6xx_com_write_wrapper(instance, REG_PWR_MGMT_0, &pwr_value, 1, &xfer_bytes, false);
    /* recover SPI speed */
    state->com_port_info.com_config.min_bus_speed_KHz = config_min_bus_speed_KHz;
    state->com_port_info.com_config.max_bus_speed_KHz = config_max_bus_speed_KHz;
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
    // deregister and register again, for updating SPI speed
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->com_port_info.port_handle = NULL;
    state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                              &state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

    ICM4X6XX_INST_PRINTF(ERROR, instance, "freefall fw dl %d", rc);

    return rc;
}
#endif

#ifdef ICM4X6XX_DISABLE_DAE
/**
 * @brief config max spi speed
 *
 * @param[in] instance, point to sensor instance
 *       [in] spi_speed, the specific spi speed
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_config_spi_speed(
           sns_sensor_instance *instance,
           icm4x6xx_spi_speed spi_speed)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

    if (state->com_port_info.com_config.bus_type != SNS_BUS_SPI) {
        return rc;
    }

    rc = icm4x6xx_write_mask(instance,
                             REG_DRIVE_CONFIG,
                             spi_speed,
                             &xfer_bytes,
                             false,
                             SPI_SPEED_MASK);

    if (spi_speed == SPI_17_MHZ) {
        state->com_port_info.com_config.max_bus_speed_KHz = 17000;
        state->com_port_info.com_config.min_bus_speed_KHz = 15000;
    } else if (spi_speed == SPI_5_MHZ) {
        state->com_port_info.com_config.max_bus_speed_KHz = 5000;
    }

    return rc;
}
#endif

#ifdef ICM4X6XX_USE_INT2
/**
 * @brief Enable or Disable int2 push pull mode.
 *
 * @param[in] enable    Identify enable int2 push pull mode or not.
 *                      true: enable int2 push pull mode
 *                      false: disable int2 push pull mode
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_en_int2_push_pull(
           sns_sensor_instance *instance,
           bool enable)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_INT_CONFIG,
                               enable ? INT2_PUSH_PULL_MASK : 0,
                               &xfer_bytes,
                               false,
                               INT2_PUSH_PULL_MASK);
}
#else
/**
 * @brief Enable or Disable int1 push pull mode.
 *
 * @param[in] enable    Identify enable int1 push pull mode or not.
 *                      true: enable int1 push pull mode
 *                      false: disable int1 push pull mode
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_en_int1_push_pull(
           sns_sensor_instance *instance,
           bool enable)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_INT_CONFIG,
                               enable ? INT1_PUSH_PULL_MASK : 0,
                               &xfer_bytes,
                               false,
                               INT1_PUSH_PULL_MASK);
}
#endif

//#ifdef ICM4X6XX_DISABLE_DAE   //DAE also need int config
#ifdef ICM4X6XX_USE_INT2
/**
 * @brief Enable or Disable int2 latched mode.
 *
 * @param[in] enable    Identify enable int2 latched mode or not.
 *                      true: enable int2 latched mode
 *                      false: disable int2 latched mode, use pulsed mode
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_en_int2_latched_mode(
           sns_sensor_instance *instance,
           bool enable)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_INT_CONFIG,
                               enable ? INT2_LATCHED_MODE_MASK : 0,
                               &xfer_bytes,
                               false,
                               INT2_LATCHED_MODE_MASK);
}
#else
/**
 * @brief Enable or Disable int1 latched mode.
 *
 * @param[in] enable    Identify enable int1 latched mode or not.
 *                      true: enable int1 latched mode
 *                      false: disable int1 latched mode, use pulsed mode
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_en_int1_latched_mode(
           sns_sensor_instance *instance,
           bool enable)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_INT_CONFIG,
                               enable ? INT1_LATCHED_MODE_MASK : 0,
                               &xfer_bytes,
                               false,
                               INT1_LATCHED_MODE_MASK);
}

/**
* @brief Config int1 polarity.
*
* @param[in] enable    Identify enable int1 latched mode or not.
*                   true: enable int1 active high
*                   false: enable int1 active low
*
* @return 0 if success,
*         non-zero value if failed.
*/
static int icm4x6xx_config_int1_polarity(
           sns_sensor_instance *instance,
           icm4x6xx_int_polarity polarity)
{
   uint32_t xfer_bytes;

   return icm4x6xx_write_mask(instance,
                              REG_INT_CONFIG,
                              (ICM4X6XX_INT_ACTIVE_HIGH == polarity) ? INT1_ACTIVE_HIGH_MASK : 0,
                              &xfer_bytes,
                              false,
                              INT1_ACTIVE_HIGH_MASK);
}

#endif
//#endif

#ifndef ICM4X6XX_USE_INT_TS
/**
 * @brief enable timestamp register
 *
 * @param[in] enable   enable or disable timestamp register
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_enable_tmst(
    sns_sensor_instance *instance,
    bool enable)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    uint8_t tmst_config_reg = REG_TMST_CONFIG_REG;

    /* Choose suitable TMST register address for
     * different chip */
    if (HAVANA == state->product_info.series)
        tmst_config_reg = REG_HAVANA_TMST_CONFIG_REG;

    // TODO: CHECK THIS for which is needed for fifo format change

    return icm4x6xx_write_mask(instance,
                               tmst_config_reg,
                               enable ? TMST_SREG_ON_EN | BIT_TMST_TO_REGS_EN | BIT_TMST_EN : 0,
                               &xfer_bytes,
                               false,
                               TMST_SREG_ON_EN | BIT_TMST_TO_REGS_EN | BIT_TMST_EN);
}
#endif

/**
 * @brief Enable or Disable fifo count and sensor data big endian mode.
 *
 * @param[in] enable    Identify enable big endian or not.
 *                      true: enable fifo count and sensor data big endian mode
 *                      false: disable fifo count and sensor data big endian mode
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_en_big_endian_mode(
           sns_sensor_instance *instance,
           bool enable)
{
    uint32_t xfer_bytes;
    uint8_t bit_mask = FIFO_COUNT_BIG_ENDIAN_MASK | SENSOR_DATA_BIG_ENDIAN_MASK;
    uint8_t reg = enable ? bit_mask : 0;

    return icm4x6xx_write_mask(instance,
                               REG_INTF_CONFIG0,
                               reg,
                               &xfer_bytes,
                               false,
                               bit_mask);
}

/**
 * @brief Config Accel FSR
 *
 * @param[in] fsr    the FSR of Accel to be set.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_set_accel_fsr(
    sns_sensor_instance *instance,
    icm4x6xx_accel_fsr fsr)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)(instance->state->state);
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

    rc += icm4x6xx_write_mask(instance,
                              REG_ACCEL_CONFIG0,
                              fsr << ACCEL_FSR_SHIFT,
                              &xfer_bytes,
                              false,
                              ACCEL_FSR_MASK);
    state->accel_info.fsr = fsr;

    return rc;
}

/**
 * @brief Config Gyro FSR
 *
 * @param[in] fsr    the FSR of Gyro to be set.
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_gyro_fsr(
    sns_sensor_instance *instance,
    icm4x6xx_gyro_fsr fsr)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)(instance->state->state);
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

    rc += icm4x6xx_write_mask(instance,
                              REG_GYRO_CONFIG0,
                              fsr << GYRO_FSR_SHIFT,
                              &xfer_bytes,
                              false,
                              GYRO_FSR_MASK);
    state->gyro_info.fsr = fsr;

    return rc;
}

/**
 * @brief config fsync
 *
 * @param[in] data, bit [4:6]: FSYNC_UI_SEL
 *                      [2]: FSYNC_AUX1_FLAG_CLEAR_SEL
 *                      [1]: FSYNC_UI_FLAG_CLEAR_SEL
 *                      [0]: FSYNC_POLARITY
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
static int icm4x6xx_config_fsync(
           sns_sensor_instance *instance,
           uint8_t data)
{
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

    rc += icm4x6xx_write_mask(instance,
                              REG_FSYNC_CONFIG,
                              data,
                              &xfer_bytes,
                              false,
                              0x70);

    return rc;
}

/**
 * @brief set accel filter order
 *
 * @param[in] order    choose accel filter order
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_accel_filter_order(
    sns_sensor_instance *instance,
    icm4x6xx_filter_order order)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_ACC_CONFIG1,
                               order << BIT_ACC_FILT_ORD_SHIFT,
                               &xfer_bytes,
                               false,
                               BIT_ACC_FILT_ORD_MASK);
}

/**
 * @brief set gyro filter order
 *
 * @param[in] order    choose gyro filter order
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_gyro_filter_order(
    sns_sensor_instance *instance,
    icm4x6xx_filter_order order)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_GYRO_CONFIG1,
                               order << BIT_GYRO_FILT_ORD_SHIFT,
                               &xfer_bytes,
                               false,
                               BIT_GYRO_FILT_ORD_MASK);
}

#ifdef ICM4X6XX_ENABLE_HIGHG
static int icm4x6xx_en_high_shock_int(
           sns_sensor_instance *instance,
           bool enable)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

#ifdef ICM4X6XX_ENABLE_I3C
    if (state->com_port_info.com_config.bus_type == SNS_BUS_I3C_SDR && !state->product_info.i3c_use_int1) {
        uint8_t bank = 4;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
        rc += icm4x6xx_write_mask(instance,
                                  REG_INT_SOURCE10,
                                  enable ? BIT_HIGHG_DET_IBI_EN : 0,
                                  &xfer_bytes,
                                  false,
                                  BIT_HIGHG_DET_IBI_EN);

        bank = 0;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
    } else {
#else
    {
#endif
        uint8_t bank = 4;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);

#ifdef ICM4X6XX_USE_INT2
        rc +=  icm4x6xx_write_mask(instance,
                                   REG_INT_SOURCE7,
                                   enable ? BIT_HIGHG_DET_INT2_EN : 0,
                                   &xfer_bytes,
                                   false,
                                   BIT_HIGHG_DET_INT2_EN);
#else
        rc +=  icm4x6xx_write_mask(instance,
                                   REG_INT_SOURCE6,
                                   enable ? BIT_HIGHG_DET_INT1_EN : 0,
                                   &xfer_bytes,
                                   false,
                                   BIT_HIGHG_DET_INT1_EN);
#endif
        bank = 0;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
    }

    return rc;
}

static int icm4x6xx_config_highg_parameter(sns_sensor_instance *instance)
{
    //icm4x6xx_instance_state *state =
      //  (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;
    uint8_t reg_value = 0;

    reg_value = 4;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);

    reg_value = ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2844MG | ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_20MS;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_APEX_CONFIG6,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);

    reg_value = 0;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);

    return rc;
}
#endif

#ifdef ICM4X6XX_ENABLE_LOWG
/**
 * @brief Enable or Disable fifo freefall interrupt.
 *
 * @param[in] enable    Identify enable fifo freefall interrupt or not.
 *                      true: enable fifo freefall interrupt
 *                      false: disable fifo freefall interrupt
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_en_freefall_int(
           sns_sensor_instance *instance,
           bool enable)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

#ifdef ICM4X6XX_ENABLE_I3C
    if (state->com_port_info.com_config.bus_type == SNS_BUS_I3C_SDR && !state->product_info.i3c_use_int1) {
        uint8_t bank = 4;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
        rc += icm4x6xx_write_mask(instance,
                                  REG_INT_SOURCE10,
                                  enable ? BIT_LOWG_DET_IBI_EN : 0,
                                  &xfer_bytes,
                                  false,
                                  BIT_LOWG_DET_IBI_EN);

        bank = 0;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
    } else {
#else
    {
#endif
        uint8_t bank = 4;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
#ifdef ICM4X6XX_USE_INT2
        rc +=  icm4x6xx_write_mask(instance,
                                   REG_INT_SOURCE7,
                                   enable ? BIT_LOWG_DET_INT2_EN : 0,
                                   &xfer_bytes,
                                   false,
                                   BIT_LOWG_DET_INT2_EN);
#else
        rc +=  icm4x6xx_write_mask(instance,
                                   REG_INT_SOURCE6,
                                   enable ? BIT_LOWG_DET_INT1_EN : 0,
                                   &xfer_bytes,
                                   false,
                                   BIT_LOWG_DET_INT1_EN);
#endif
        bank = 0;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
    }

    return rc;
}

static int icm4x6xx_config_lowg_parameter(sns_sensor_instance *instance)
{
    //icm4x6xx_instance_state *state =
      //  (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;
    uint8_t reg_value = 0;

    reg_value = 4;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);

    reg_value = ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_156MG;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_APEX_CONFIG4,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);

    reg_value = ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_125MG | ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_20MS;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_APEX_CONFIG5,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);

    reg_value = 0;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);

    return rc;
}
#endif

/**
 * @brief set fifo watermark, wm_th should be calculated by current fifo format
 *
 * @param[in] wm_th    FIFO watermark, user should calculate it
 *                      with current fifo packet format
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
static int icm4x6xx_enable_record_mode(
           sns_sensor_instance *instance,
           bool enable)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)(instance->state->state);
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

    rc += icm4x6xx_write_mask(instance,
                              REG_INTF_CONFIG0,
                              enable ? RECORD_MODE_MASK : 0,
                              &xfer_bytes,
                              false,
                              RECORD_MODE_MASK);
    state->fifo_info.record_mode = enable;

    return rc;
}

/**
 * @brief enable or disable Asynchronous reset for Interrupt
 *
 * @param[in] instance    point to sensor instance
 * @param[in] enable      flag to identify enable or disable
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
static int icm4x6xx_enable_int_async_reset(
           sns_sensor_instance *instance,
           bool enable)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_INT_CONFIG1,
                               !enable ? BIT_INT_ASY_RST_DIS_MASK : 0,
                               &xfer_bytes,
                               false,
                               BIT_INT_ASY_RST_DIS_MASK);
}

/**
 * @brief Enable or Disable fifo overflow interrupt.
 *
 * @param[in] enable    Identify enable fifo overflow interrupt or not.
 *                      true: enable fifo overflow interrupt
 *                      false: disable fifo overflow interrupt
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_en_fifo_full_int(
    sns_sensor_instance *instance,
    bool enable)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

#ifdef ICM4X6XX_ENABLE_I3C
    if (state->com_port_info.com_config.bus_type == SNS_BUS_I3C_SDR  && !state->product_info.i3c_use_int1) {
        uint8_t bank = 4;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
        rc += icm4x6xx_write_mask(instance,
                                  REG_INT_SOURCE8,
                                  enable ? BIT_FIFO_FULL_IBI_EN : 0,
                                  &xfer_bytes,
                                  false,
                                  BIT_FIFO_FULL_IBI_EN);

        bank = 0;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
    } else {
#else
    {
#endif

#ifdef ICM4X6XX_USE_INT2
        rc += icm4x6xx_write_mask(instance,
                                  REG_INT_SOURCE3,
                                  enable ? FIFO_FULL_INT2_EN_MASK : 0,
                                  &xfer_bytes,
                                  false,
                                  FIFO_FULL_INT2_EN_MASK);
#else
        rc += icm4x6xx_write_mask(instance,
                                  REG_INT_SOURCE0,
                                  enable ? FIFO_FULL_EN_MASK : 0,
                                  &xfer_bytes,
                                  false,
                                  FIFO_FULL_EN_MASK);
#endif
    }

    return rc;
}

/**
 * @brief config ui interface
 *
 * @param[in] instance, point to sensor instance
 *       [in] intf, the specific interface to be used
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_config_ui_intf(
           sns_sensor_instance *instance,
           icm4x6xx_ui_intf intf)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_INTF_CONFIG0,
                               intf,
                               &xfer_bytes,
                               false,
                               UI_INTF_MASK);
}

static int icm4x6xx_en_fifo_hold_last_data(
           sns_sensor_instance *instance,
           bool enable)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_INTF_CONFIG0,
                               enable ? FIFO_HOLD_LAST_DATA_EN : 0,
                               &xfer_bytes,
                               false,
                               FIFO_HOLD_LAST_DATA_EN);
}

/**
 * @brief Enable glitch filter and change slew rate for I2C
 *
 * @param[in] instance, point to sensor instance
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_i2c_config(
           sns_sensor_instance *instance)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    uint8_t reg = 0;
    int rc = SNS_RC_SUCCESS;

    if (HAVANA == state->product_info.series) {
        return rc;
    }

    /* change I2C slew rate and enable glitch filter for I2C,
     * according Hema's mail 2018/05/02, 2:47,
     * and also Deepa's mail 2018/04/05, 12:10
     * Updated this setting, according Hema's mail 2018/05/03, 7:58,
     * and also ds ICM-42605-M_datasheet_internal_release_050218.pdf 12.3 */

    /* Enable glitch filter */
    reg = 1;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &reg,
                                     1,
                                     &xfer_bytes,
                                     false);
    rc += icm4x6xx_write_mask(instance,
                              REG_INTF_CONFIG6,
                              0,
                              &xfer_bytes,
                              false,
                              I3C_SDR_EN | I3C_DDR_EN);
    reg = 0;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &reg,
                                     1,
                                     &xfer_bytes,
                                     false);

    /* Change slew rate */
    rc += icm4x6xx_write_mask(instance,
                              REG_DRIVE_CONFIG,
                              (1 << I2C_SPEED_SHIFT) | 1,
                              &xfer_bytes,
                              false,
                              SPI_SPEED_MASK | I2C_SPEED_MASK);

    return rc;
}

#ifdef ICM4X6XX_ENABLE_RTC_MODE
int icm4x6xx_enable_rtc_mode(
    sns_sensor_instance *instance,
    bool enable)
{
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;
    uint8_t data = 0;

    data = 1;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &data,
                                     1,
                                     &xfer_bytes,
                                     false);
#ifdef ICM4X6XX_USE_INT2 //Should not run here!
    rc += icm4x6xx_write_mask(instance,
                              REG_INTF_CONFIG5,
                              enable ? BIT_PIN9_FUNC_INT2 : 0,
                              &xfer_bytes,
                              false,
                              BIT_PIN9_FUNC_MASK);

#else
    rc += icm4x6xx_write_mask(instance,
                              REG_INTF_CONFIG5,
                              enable ? BIT_PIN9_FUNC_CLKIN : 0,
                              &xfer_bytes,
                              false,
                              BIT_PIN9_FUNC_MASK);
#endif
    data = 0;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &data,
                                     1,
                                     &xfer_bytes,
                                     false);
    rc += icm4x6xx_write_mask(instance,
                              REG_INTF_CONFIG1,
                              BIT_RTC_MODE_EN,
                              &xfer_bytes,
                              false,
                              BIT_RTC_MODE_EN);

    return rc;
}
#endif

#ifdef ICM4X6XX_ENABLE_I3C
/**
 * @brief change slew rate for I3C
 *
 * @param[in] instance, point to sensor instance
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_i3c_config(
           sns_sensor_instance *instance)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    uint8_t i3c_spi_slew_rate = 5;//To accommodate lower clock rate we have increased the slew rate from 3 to 5
    int rc = SNS_RC_SUCCESS;

    if (!state->product_info.i3c_supported) {
        return rc;
    }

    /* change slew rate for I3C,
     * according Hema's mail 2018/05/02, 2:47,
     * and also Deepa's mail 2018/04/05, 12:10
     * Updated this setting, according Hema's mail 2018/05/03, 7:58,
     * and also ds ICM-42605-M_datasheet_internal_release_050218.pdf 12.3 */

    /* SPI_SLEW_RATE:
     *    5, IBI is used for interrupt assertion in I3C mode
     *    3, INT1/INT2 pins are used for interrupt assertion in I3C mode
     * following setting is using IBI for interrupt assertion in I3C mode */

    rc += icm4x6xx_write_mask(instance,
                              REG_DRIVE_CONFIG,
                              (0 << I2C_SPEED_SHIFT) | i3c_spi_slew_rate,
                              &xfer_bytes,
                              false,
                              SPI_SPEED_MASK | I2C_SPEED_MASK);

    return rc;
}

sns_rc icm4x6xx_enter_i3c_mode(
              sns_sensor_instance *const instance,
              icm4x6xx_com_port_info *com_port,
              sns_sync_com_port_service *scp_service)
{
    sns_sync_com_port_handle *i2c_port_handle = NULL;
    sns_com_port_config       i2c_com_config = com_port->com_config;
    sns_rc                    rv = SNS_RC_SUCCESS;
    uint32_t                  xfer_bytes;
    uint8_t                   buffer[6];

    if (com_port->com_config.bus_type != SNS_BUS_I3C_SDR &&
        com_port->com_config.bus_type != SNS_BUS_I3C_HDR_DDR) {
        return SNS_RC_SUCCESS;
    }

    if (NULL != instance) {
        ICM4X6XX_INST_PRINTF(LOW, instance, "enter i3c mode");
    }

    i2c_com_config.slave_control = com_port->i2c_address;
    rv = scp_service->api->sns_scp_register_com_port(&i2c_com_config, &i2c_port_handle);
    if (rv != SNS_RC_SUCCESS) {
        return SNS_RC_FAILED;
    }
    rv = scp_service->api->sns_scp_open(i2c_port_handle);
    if (rv != SNS_RC_SUCCESS) {
        return SNS_RC_FAILED;
    }

    /*******DAA*********/   //dynamic address assign
    buffer[0] = (com_port->i3c_address & 0xFF) << 1;    //7 bits address
    rv = scp_service->api->sns_scp_issue_ccc(i2c_port_handle,
                                             SNS_SYNC_COM_PORT_CCC_SETDASA, //static address for I3C
                                             buffer, 1, &xfer_bytes);
    scp_service->api->sns_scp_close(i2c_port_handle);
    scp_service->api->sns_scp_deregister_com_port(&i2c_port_handle);
    if (NULL != instance && rv == SNS_RC_SUCCESS) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "I3C addr: 0x%x", ((uint32_t)buffer[0])>>1);
    }

    if (NULL == instance)
        return rv;

    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint16_t max_fifo_size = ICM4X6XX_YOKOHAMA_MAX_FIFO_SIZE; // only Yokohama has I3C now

    /* Set max read size to the size fo the FIFO */
    buffer[0] = (uint8_t)((max_fifo_size >> 8) & 0xFF);
    buffer[1] = (uint8_t)(max_fifo_size & 0xFF);
    buffer[2] = 0; // 0 is unlimited value, but Yokohama ignores and drops the IBI payload size
    rv = scp_service->api->sns_scp_issue_ccc(com_port->port_handle,
                                             SNS_SYNC_COM_PORT_CCC_SETMRL,
                                             buffer, 3, &xfer_bytes);
    if (rv != SNS_RC_SUCCESS) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "SETMRL failed");
    }

    if (state->product_info.i3c_use_int1) { //i3c_use_int1 was read from macro
        /********disable IBI *****/
        buffer[0] = 0x01;
        rv = scp_service->api->sns_scp_issue_ccc(com_port->port_handle,
                                                 SNS_SYNC_COM_PORT_CCC_DISEC,
                                                 buffer, 1, &xfer_bytes);
        if (rv == SNS_RC_SUCCESS) {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "IBI disabled");
        } else {
            ICM4X6XX_INST_PRINTF(HIGH, instance, "disable IBI failed");
        }
    }

#ifdef ICM4X6XX_ENABLE_DEBUG_I3C
  /**-------------------Debug -- read all CCC info------------------------*/
  sns_memset(buffer, 0, sizeof(buffer));
  rv = scp_service->api->
    sns_scp_issue_ccc( com_port->port_handle,
                       SNS_SYNC_COM_PORT_CCC_GETMWL,
                       buffer, 2, &xfer_bytes );
  if( rv == SNS_RC_SUCCESS ) {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(LOW, instance, "max write length:0x%02x%02x", buffer[0], buffer[1]);
    }
  } else {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(ERROR, instance, "Get max write length failed!");
    }
  }
  rv = scp_service->api->
    sns_scp_issue_ccc( com_port->port_handle,
                       SNS_SYNC_COM_PORT_CCC_SETMWL,
                       buffer, 2, &xfer_bytes );
  if( rv != SNS_RC_SUCCESS ) {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(ERROR, instance, "Set max write length failed!");
    }
  }

#if 0   //TODO
/** FAIL:  M-EF Total error/fatal messages = 8 */
  sns_memset(buffer, 0, sizeof(buffer));
  rv = scp_service->api->
    sns_scp_issue_ccc( com_port->port_handle,
                       SNS_SYNC_COM_PORT_CCC_GETMRL,
                       buffer, 3, &xfer_bytes );
  if( rv == SNS_RC_SUCCESS ) {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(LOW, instance, "max read length:0x%02x%02x%02x",
                         buffer[0], buffer[1], buffer[2]);
    }
  } else {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(ERROR, instance, "Get max read length failed!");
    }
  }
#endif

  sns_memset(buffer, 0, sizeof(buffer));
  rv = scp_service->api->
    sns_scp_issue_ccc( com_port->port_handle,
                       SNS_SYNC_COM_PORT_CCC_GETPID,
                       buffer, 6, &xfer_bytes );
  if( rv == SNS_RC_SUCCESS ) {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(LOW, instance, "PID:0x%02x%02x:%02x%02x:%02x%02x",
                      buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
    }
  } else {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(ERROR, instance, "Get PID failed!");
    }
  }

  sns_memset(buffer, 0, sizeof(buffer));
  rv = scp_service->api->
    sns_scp_issue_ccc( com_port->port_handle,
                       SNS_SYNC_COM_PORT_CCC_GETBCR,
                       buffer, 1, &xfer_bytes );
  if( rv == SNS_RC_SUCCESS ) {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(LOW, instance, "bus charactaristics register:0x%x", buffer[0]);
    }
  } else {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(ERROR, instance, "Get BCR failed!");
    }
  }

  sns_memset(buffer, 0, sizeof(buffer));
  rv = scp_service->api->
    sns_scp_issue_ccc( com_port->port_handle,
                       SNS_SYNC_COM_PORT_CCC_GETDCR,
                       buffer, 1, &xfer_bytes );
  if( rv == SNS_RC_SUCCESS ) {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(LOW, instance, "device charactaristics register:0x%x", buffer[0]);
    }
  } else {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(ERROR, instance, "Get DCR failed!");
    }
  }

  sns_memset(buffer, 0, sizeof(buffer));
  rv = scp_service->api->
    sns_scp_issue_ccc( com_port->port_handle,
                       SNS_SYNC_COM_PORT_CCC_GETSTATUS,
                       buffer, 2, &xfer_bytes );
  if( rv == SNS_RC_SUCCESS ) {
    uint32_t status_reg =  buffer[0] | buffer[1] << 8;
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(LOW, instance, "status register:0x%x", status_reg);
    }
  } else {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(ERROR, instance, "Get status failed!");
    }
  }

  sns_memset(buffer, 0, sizeof(buffer));
  rv = scp_service->api->
    sns_scp_issue_ccc( com_port->port_handle,
                       SNS_SYNC_COM_PORT_CCC_GETMXDS,
                       buffer, 2, &xfer_bytes );
  if( rv == SNS_RC_SUCCESS && xfer_bytes == 2) {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(LOW, instance, "MXDS :0x%02x%02x", buffer[0], buffer[1]);
    }
  } else {
    if(NULL != instance)
    {
      ICM4X6XX_INST_PRINTF(ERROR, instance, "Get MXDS failed! rv:%d xfer_bytes:%d", rv, xfer_bytes);
    }
  }
#endif
    return rv;
}
#endif

#ifdef ICM4X6XX_DISABLE_AUX_PADS
static sns_rc icm4x6xx_disable_aux_pins(sns_sensor_instance *instance)
{
	//icm4x6xx_instance_state *state =
	//    (icm4x6xx_instance_state*)instance->state->state;
	uint32_t xfer_bytes = 0;
	int rc = SNS_RC_SUCCESS;
	uint8_t reg_value = 0;

	reg_value = 0x02;//bank 2
	rc += icm4x6xx_com_write_wrapper(instance,
									 REG_BANK_SEL,
									 &reg_value,
									 1,
									 &xfer_bytes,
									 false);

	reg_value = 0x01;
	rc += icm4x6xx_com_write_wrapper(instance,
									 0x70,
									 &reg_value,
									 1,
									 &xfer_bytes,
									 false);

	reg_value = 0x01;
	rc += icm4x6xx_com_write_wrapper(instance,
									 0x71,
									 &reg_value,
									 1,
									 &xfer_bytes,
									 false);

	reg_value = 0x01;
	rc += icm4x6xx_com_write_wrapper(instance,
									 0x72,
									 &reg_value,
									 1,
									 &xfer_bytes,
									 false);

	reg_value = 0x01;
	rc += icm4x6xx_com_write_wrapper(instance,
									 0x73,
									 &reg_value,
									 1,
									 &xfer_bytes,
									 false);

	reg_value = 0x00;//bank 0
	rc += icm4x6xx_com_write_wrapper(instance,
									 REG_BANK_SEL,
									 &reg_value,
									 1,
									 &xfer_bytes,
									 false);

	return rc;
}
#endif

/**
 * @brief check soft reset done or not.
 *
 * @Usage a. call yokohama_soft_reset first
 *        b. delay 1ms
 *        c. repeatedly call this function
 *                      until reset is done
 *
 * @param[out] is_done    Identify soft reset done or not.
 *                        true: soft reset has been done
 *                        false: soft reset has not been
 *                                    done or not started
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_check_soft_reset_done(
           sns_sensor_instance *instance,
           bool *is_done)
{
    int rc = SNS_RC_SUCCESS;
    uint8_t reg = 0x0;
    uint32_t xfer_bytes;

    rc = icm4x6xx_com_read_wrapper(instance,
                                   REG_INT_STATUS,
                                   &reg,
                                   1,
                                   &xfer_bytes);
    if (reg & RESET_DONE_MASK)
        *is_done = true;
    else
        *is_done = false;

    return rc;
}

/**
 * @brief do device soft reset.
 *
 * @Usage a. call this function first
 *        b. delay 1ms
 *        c. repeatedly call yokohama_check_soft_reset_done
 *                      until reset is done
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
static int icm4x6xx_soft_reset(
           sns_sensor_instance *instance)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_CHIP_CONFIG,
                               SOFT_RESET_MASK,
                               &xfer_bytes,
                               false,
                               SOFT_RESET_MASK);
}

static int icm4x6xx_device_sw_reset(
           sns_sensor_instance *instance)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    int rc = SNS_RC_SUCCESS;
    uint8_t retry_num = ICM4X6XX_SW_RESET_CHECK_TIMES;  /* TODO: check with hardware team the min/max reset time */
    bool reset_done = false;

    rc += icm4x6xx_soft_reset(instance);

    do {
        rc += sns_busy_wait(sns_convert_ns_to_ticks(1*1000*1000));  //1ms is enough for Hava/Yoko to reset done
#ifdef ICM4X6XX_ENABLE_I3C
        rc += icm4x6xx_enter_i3c_mode(instance, &state->com_port_info, state->scp_service);
        if (rc != SNS_RC_SUCCESS)
            ICM4X6XX_INST_PRINTF(MED, instance, "enter i3c mode failed");
#endif

        rc += icm4x6xx_check_soft_reset_done(instance, &reset_done);
        if (reset_done == true)
            break;
    } while(retry_num--);

    ICM4X6XX_INST_PRINTF(LOW, instance, "check reset done %d times", ICM4X6XX_SW_RESET_CHECK_TIMES - retry_num + 1);

    return rc;
}

static int icm4x6xx_reset_default_variable(
           sns_sensor_instance *instance)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    int rc = SNS_RC_SUCCESS;

    state->accel_info.cur_odr = 0.0;
    state->gyro_info.cur_odr = 0.0;
    state->accel_info.curr_mode = ICM4X6XX_A_OFF;
    state->accel_info.is_enabled = false;
    state->gyro_info.is_enabled = false;
    state->fifo_info.cur_wmk = 0;
    state->fifo_info.curr_format = ICM4X6XX_FORMAT_EMPTY;
    state->is_first_sample = true;

    return rc;
}

static int icm4x6xx_device_set_default_state(
           sns_sensor_instance *instance)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    int rc = SNS_RC_SUCCESS;

    /* DS Section 8.1, when using SPI, driver should disable I2C */
    if (state->com_port_info.com_config.bus_type == SNS_BUS_SPI)
        rc += icm4x6xx_config_ui_intf(instance, SPI_INTF);

    /* Need to change slew rate for I2C/I3C and enable glitch filter for I2C,
     * according Hema's mail 2018/05/02, 2:47,
     * and also Deepa's mail 2018/04/05, 12:10,
     * last update, @ Hema's mail 2018/05/03, 7:58,
     * and also ds ICM-42605-M_datasheet_internal_release_050218.pdf 12.3 */
    if (state->com_port_info.com_config.bus_type == SNS_BUS_I2C)
        rc += icm4x6xx_i2c_config(instance);
#ifdef ICM4X6XX_ENABLE_I3C
    else if (state->com_port_info.com_config.bus_type == SNS_BUS_I3C_SDR)
        rc += icm4x6xx_i3c_config(instance);
#endif
    /* It is unnecessary to change slew rate for SPI */
    /*
    else if (state->com_port_info.com_config.bus_type == SNS_BUS_SPI) {
    }*/

#ifdef ICM4X6XX_DISABLE_DAE
    /* Set Havana to Max SPI speed */
    if (HAVANA == state->product_info.series) {
        rc += icm4x6xx_config_spi_speed(instance, SPI_17_MHZ);
    }
#endif

#ifdef ICM4X6XX_ENABLE_I3C
    if (state->com_port_info.com_config.bus_type != SNS_BUS_I3C_SDR ||
        state->product_info.i3c_use_int1) {
#else
    {
#endif

#ifdef ICM4X6XX_USE_INT2
        /* Enable push pull for int2 */
        rc += icm4x6xx_en_int2_push_pull(instance, true);

#else
        ICM4X6XX_VERB_INST_PRINTF(HIGH, instance, "irq_pull_type=%d", state->irq_info.irq_config.interrupt_pull_type);
        /* Enable push pull for int1 */
        if ((SNS_INTERRUPT_PULL_TYPE_PULL_DOWN == state->irq_info.irq_config.interrupt_pull_type)||
            (SNS_INTERRUPT_PULL_TYPE_PULL_UP == state->irq_info.irq_config.interrupt_pull_type))
            rc += icm4x6xx_en_int1_push_pull(instance, false);
        else //if (PULL_TYPE_NO_PULL/PULL_TYPE_KEEPER)
            rc += icm4x6xx_en_int1_push_pull(instance, true);
#endif

//#ifdef ICM4X6XX_DISABLE_DAE   //DAE also need int config
#ifdef ICM4X6XX_USE_INT2
        /* Enable int2 latched mode */
        rc += icm4x6xx_en_int2_latched_mode(instance, true);
#else
        ICM4X6XX_VERB_INST_PRINTF(HIGH, instance, "irq_trigger_type=%d", state->irq_info.irq_config.interrupt_trigger_type);
        /* pulse mode is more suitable here, but beware, we use latched mode for edge trigger before. */
        if ((SNS_INTERRUPT_TRIGGER_TYPE_RISING    == state->irq_info.irq_config.interrupt_trigger_type)||
            (SNS_INTERRUPT_TRIGGER_TYPE_FALLING   == state->irq_info.irq_config.interrupt_trigger_type)||
            (SNS_INTERRUPT_TRIGGER_TYPE_DUAL_EDGE == state->irq_info.irq_config.interrupt_trigger_type))
            rc += icm4x6xx_en_int1_latched_mode(instance, false);
        else //if (TRIGGER_TYPE_HIGH/TRIGGER_TYPE_LOW)
            rc += icm4x6xx_en_int1_latched_mode(instance, true);

        /* Config int1 interrupt polarity */
        if ((SNS_INTERRUPT_TRIGGER_TYPE_RISING    == state->irq_info.irq_config.interrupt_trigger_type)||
            (SNS_INTERRUPT_TRIGGER_TYPE_HIGH      == state->irq_info.irq_config.interrupt_trigger_type)||
            (SNS_INTERRUPT_TRIGGER_TYPE_DUAL_EDGE == state->irq_info.irq_config.interrupt_trigger_type))
            rc += icm4x6xx_config_int1_polarity(instance, ICM4X6XX_INT_ACTIVE_HIGH);
        else //if (TRIGGER_TYPE_FALLING/TRIGGER_TYPE_LOW)
            rc += icm4x6xx_config_int1_polarity(instance, ICM4X6XX_INT_ACTIVE_LOW);
#endif /*ICM4X6XX_USE_INT2*/
//#endif

        /* decrease system loading */
        //rc += icm4x6xx_sel_int_line_clr_op(instance, CLEAR_ON_STATUS_BIT_R_0);
    }

#ifdef ICM4X6XX_ENABLE_RTC_MODE
    /* enable RTC mode */
    rc += icm4x6xx_enable_rtc_mode(instance, true);
#endif

    /* Enable Timestamp field contains the measurement of time
     * since the last occurrence of ODR.*/
    //rc += icm4x6xx_enable_delta_tmst(instance, true);

#ifndef ICM4X6XX_USE_INT_TS
    /* Enable timestamp register */
    rc += icm4x6xx_enable_tmst(instance, true);
#endif

    /* Choose big endian mode for fifo count and sensor data
     * default mode for FPGA and chip may be different,
     * so we choose one mode here for both */
    rc += icm4x6xx_en_big_endian_mode(instance, true);

    /* enable fifo hold last data */
    rc += icm4x6xx_en_fifo_hold_last_data(instance, true);

    /* do not tag fsync flag for temperature resolution */
    rc += icm4x6xx_config_fsync(instance, 0);

    /* Choose Accel FSR */
    rc += icm4x6xx_set_accel_fsr(instance, state->accel_info.fsr);

    /* Choose Gyro FSR */
    rc += icm4x6xx_set_gyro_fsr(instance, state->gyro_info.fsr);

    /* Choose Accel filter order */
    rc += icm4x6xx_set_accel_filter_order(instance, THIRD_ORDER); // TODO, USE 3rd order filter

    /* Choose Gyro filter order */
    rc += icm4x6xx_set_gyro_filter_order(instance, THIRD_ORDER); // TODO, USE 3rd order filter

    /* Choose Accel bandwidth */
    rc += icm4x6xx_set_accel_bandwidth(instance, BW_ODR_DIV_2);

    /* Choose Gyro bandwidth */
    rc += icm4x6xx_set_gyro_bandwidth(instance, BW_ODR_DIV_2);

    /* Enable fifo */
    rc += icm4x6xx_set_fifo_mode(instance, STREAM);

    /* Enable fifo overflow interrupt */
    rc += icm4x6xx_en_fifo_full_int(instance, true);

#ifdef ICM4X6XX_ENABLE_LOWG
    /* Enable freefall interrupt */
    rc += icm4x6xx_en_freefall_int(instance, true);

    /* config lowg parameter */
    rc += icm4x6xx_config_lowg_parameter(instance);
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
    /* Enable high shock interrupt */
    rc += icm4x6xx_en_high_shock_int(instance, true);

    /* Config highg parameter*/
    rc += icm4x6xx_config_highg_parameter(instance);
#endif

    // TODO: Enable wm on IBI here, since we didn't need irq reg ready now

    /* Choose fifo count record or byte mode */
    // TODO: do test with both byte and record mode
    /* H_W_B 9450 pls don't use record mode for havana */
    rc += icm4x6xx_enable_record_mode(instance, false);

    if (HAVANA == state->product_info.series) {
        /* disable interrupt async reset in Havana */
        rc += icm4x6xx_enable_int_async_reset(instance, false);
    } else {
        /* The field int_asy_rst_disable must be 0 for Yokohama */
        rc += icm4x6xx_enable_int_async_reset(instance, true);
    }

#ifdef ICM4X6XX_DISABLE_AUX_PADS
	rc += icm4x6xx_disable_aux_pins(instance);
#endif
    return rc;
}

int icm4x6xx_reset_device(
    sns_sensor_instance *instance)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    int rc = SNS_RC_SUCCESS;

    ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "reset device enter");

#ifdef ICM4X6XX_ENABLE_I3C
    rc = icm4x6xx_enter_i3c_mode(instance, &state->com_port_info, state->scp_service);
    if (rc != SNS_RC_SUCCESS)
        ICM4X6XX_PRINTF(MED, instance, "enter i3c mode failed");
#endif

    rc += icm4x6xx_device_sw_reset(instance);
#if defined ICM4X6XX_ENABLE_LOWG || defined ICM4X6XX_ENABLE_HIGHG
    rc += icm4x6xx_load_dmp_sram_code(instance);
#endif
    rc += icm4x6xx_reset_default_variable(instance);
    rc += icm4x6xx_device_set_default_state(instance);

    ICM4X6XX_VERB_INST_PRINTF(LOW, instance, "reset device exit");

    return rc;
}
