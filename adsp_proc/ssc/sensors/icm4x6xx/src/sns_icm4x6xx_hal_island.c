/*
*
* Copyright @ 2014-2019 InvenSense Inc.  All rights reserved.
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

/*Don't modify the order of this table, it's follow the table of icm4x6xx_sensor_odr*/
static const icm4x6xx_filter_delay_map filter_delay_map[] = {
    {
        .odr_reg_value = ODR_NOT_SUPPORTED,
        .filter_delay = 0,
    },
    {
        .odr_reg_value = ODR_32KHZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_1000 * 1000000),
    },
    {
        .odr_reg_value = ODR_16KHZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_1000 * 1000000),
    },
    {
        .odr_reg_value = ODR_8KHZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_1000 * 1000000),
    },
    {
        .odr_reg_value = ODR_4KHZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_1000 * 1000000),
    },
    {
        .odr_reg_value = ODR_2KHZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_1000 * 1000000),
    },
    {
        .odr_reg_value = ODR_1KHZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_1000 * 1000000),
    },
    {
        .odr_reg_value = ODR_200HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_200 * 1000000),
    },
    {
        .odr_reg_value = ODR_100HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_100 * 1000000),
    },
    {
        .odr_reg_value = ODR_50HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_50 * 1000000),
    },
    {
        .odr_reg_value = ODR_25HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_25 * 1000000),
    },
    {
        .odr_reg_value = ODR_12_5HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_25 * 1000000),
    },
    {
        .odr_reg_value = ODR_6_25HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_25 * 1000000),
    },
    {
        .odr_reg_value = ODR_3_125HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_25 * 1000000),
    },
    {
        .odr_reg_value = ODR_1_5625HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_25 * 1000000),
    },
    {
        .odr_reg_value = ODR_500HZ,
        .filter_delay = NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_500 * 1000000),
    },
};

/**
 * Read wrapper for Synch Com Port Service.
 *
 * @param[i] port_handle      port handle
 * @param[i] reg_addr         register address
 * @param[i] buffer           read buffer
 * @param[i] bytes            bytes to read
 * @param[o] xfer_bytes       bytes read
 *
 * @return sns_rc
 */
sns_rc icm4x6xx_com_read_wrapper(
       sns_sensor_instance *instance,
       uint32_t reg_addr,
       uint8_t *buffer,
       uint32_t bytes,
       uint32_t *xfer_bytes)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    sns_sync_com_port_service *scp_service = state->scp_service;
    sns_sync_com_port_handle *port_handle = state->com_port_info.port_handle;
    sns_port_vector port_vec;
    port_vec.buffer = buffer;
    port_vec.bytes = bytes;
    port_vec.is_write = false;
    port_vec.reg_addr = reg_addr;

    return scp_service->api->sns_scp_register_rw(port_handle,
                                                 &port_vec,
                                                 1,
                                                 false,
                                                 xfer_bytes);
}

/**
 * Write wrapper for Synch Com Port Service.
 *
 * @param[i] port_handle      port handle
 * @param[i] reg_addr         register address
 * @param[i] buffer           write buffer
 * @param[i] bytes            bytes to write
 * @param[o] xfer_bytes       bytes written
 * @param[i] save_write_time  true to save write transfer time.
 *
 * @return sns_rc
 */
sns_rc icm4x6xx_com_write_wrapper(
       sns_sensor_instance *instance,
       uint32_t reg_addr,
       uint8_t *buffer,
       uint32_t bytes,
       uint32_t *xfer_bytes,
       bool save_write_time)
{
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    sns_sync_com_port_service *scp_service = state->scp_service;
    sns_sync_com_port_handle *port_handle = state->com_port_info.port_handle;
    sns_port_vector port_vec;
    sns_rc rc = SNS_RC_SUCCESS;

    port_vec.buffer = buffer;
    port_vec.bytes = bytes;
    port_vec.is_write = true;
    port_vec.reg_addr = reg_addr;

#ifdef ICM4X6XX_USE_INT_TS
    rc = scp_service->api->sns_scp_register_rw(port_handle,
                                               &port_vec,
                                               1,
                                               save_write_time,
                                               xfer_bytes);
#else
    if (!save_write_time) {
        rc = scp_service->api->sns_scp_register_rw(port_handle,
                                                   &port_vec,
                                                   1,
                                                   save_write_time,
                                                   xfer_bytes);
    } else {
        sns_time ts0, ts1;
        ts0 = sns_get_system_time();
        rc = scp_service->api->sns_scp_register_rw(port_handle,
                                                   &port_vec,
                                                   1,
                                                   save_write_time,
                                                   xfer_bytes);
        ts1 = sns_get_system_time();

        ICM4X6XX_INST_PRINTF(HIGH, instance, "w dt %d", (uint32_t)(ts1 - ts0));
        state->strobe_master_cnt = ts1;
        if (((uint32_t)(ts1 - ts0)) < 5000)
            state->lpf_parameter = 10;
        else if ((((uint32_t)(ts1 - ts0)) > 19200))
            state->lpf_parameter = 500;
        else if ((((uint32_t)(ts1 - ts0)) > 15000))
            state->lpf_parameter = 100;
        else
            state->lpf_parameter = 30;
    }
#endif

    return rc;
}

/**
 * If mask = 0x0 or 0xFF, or if size > 1, write reg_value
 * directly to reg_addr. Else, read value at reg_addr and only
 * modify bits defined by mask.
 *
 * @param[i] port_handle      handle to synch COM port
 * @param[i] reg_addr         reg addr to modify
 * @param[i] reg_value        value to write to register
 * @param[i] size             number of bytes to write
 * @param[o]  xfer_bytes      number of bytes transfered
 * @param[i] save_write_time  save write time input
 * @param[i] mask             bit mask to update
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
int icm4x6xx_write_mask(
    sns_sensor_instance *instance,
    uint32_t reg_addr,
    uint8_t reg_value,
    uint32_t *xfer_bytes,
    bool save_write_time,
    uint8_t mask)
{
    uint8_t rw_buffer = 0;
    uint32_t rw_bytes = 0;
    int rc = 0;

    /* read current value from this register */
    rc += icm4x6xx_com_read_wrapper(instance,
                                   reg_addr,
                                   &rw_buffer,
                                   1,
                                   &rw_bytes);

    /* generate new value */
    rw_buffer = (rw_buffer & (~mask)) | (reg_value & mask);

    /* write new value to this register */
    rc += icm4x6xx_com_write_wrapper(instance,
                                     reg_addr,
                                     &rw_buffer,
                                     1,
                                     xfer_bytes,
                                     save_write_time);
    return rc;
}

/**
 * @brief read device ID.
 *
 * @param[out] id    point to the value of device ID.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_id(
    sns_sync_com_port_service *scp_service,
    sns_sync_com_port_handle *port_handle,
    uint8_t *id)
{
    uint32_t xfer_bytes;

    sns_port_vector port_vec;
    port_vec.buffer = id;
    port_vec.bytes = 1;
    port_vec.is_write = false;
    port_vec.reg_addr = REG_WHO_AM_I;

    return scp_service->api->sns_scp_register_rw(port_handle,
                                                 &port_vec,
                                                 1,
                                                 false,
                                                 &xfer_bytes);
}

/**
 * @brief Enable or Disable data ready interrupt.
 *
 * @param[in] enable    Identify enable data ready interrupt or not.
 *                      true: enable data ready interrupt
 *                      false: disable data ready interrupt
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_en_dri(
    sns_sensor_instance *instance,
    bool enable)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    uint32_t xfer_bytes;
    int rc = 0;

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
                                  enable ? BIT_DRI_IBI_EN : 0,
                                  &xfer_bytes,
                                  false,
                                  BIT_DRI_IBI_EN);

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
                                  enable ? DRI_INT2_EN_MASK : 0,
                                  &xfer_bytes,
                                  false,
                                  DRI_INT2_EN_MASK);
#else
        rc += icm4x6xx_write_mask(instance,
                                  REG_INT_SOURCE0,
                                  enable ? DRI_EN_MASK : 0,
                                  &xfer_bytes,
                                  false,
                                  DRI_EN_MASK);
#endif
    }

    return rc;
}

/**
 * @brief Enable or Disable fifo watermark interrupt.
 *
 * @param[in] enable    Identify enable fifo watermark interrupt or not.
 *                      true: enable fifo watermark interrupt
 *                      false: disable fifo watermark interrupt
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_en_wm_int(
    sns_sensor_instance *instance,
    bool enable)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    uint32_t xfer_bytes;
    int rc = 0;

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
                                  REG_INT_SOURCE8,
                                  enable ? BIT_WM_IBI_EN : 0,
                                  &xfer_bytes,
                                  false,
                                  BIT_WM_IBI_EN);

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
        rc +=  icm4x6xx_write_mask(instance,
                                   REG_INT_SOURCE3,
                                   enable ? WM_INT_INT2_EN_MASK : 0,
                                   &xfer_bytes,
                                   false,
                                   WM_INT_INT2_EN_MASK);
#else
        rc +=  icm4x6xx_write_mask(instance,
                                   REG_INT_SOURCE0,
                                   enable ? WM_INT_EN_MASK : 0,
                                   &xfer_bytes,
                                   false,
                                   WM_INT_EN_MASK);
#endif
    }

    return rc;
}

/**
 * @brief Enable or Disable fifo watermark interrupt.
 *
 * @param[in] enable    Identify enable fifo watermark interrupt or not.
 *                      true: enable fifo watermark interrupt
 *                      false: disable fifo watermark interrupt
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_is_wm_int_enabled(
    sns_sensor_instance *instance,
    bool *enabled)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    int rc = 0;
    uint8_t reg;
    uint32_t xfer_bytes;

#ifdef ICM4X6XX_ENABLE_I3C
    if (state->com_port_info.com_config.bus_type == SNS_BUS_I3C_SDR  && !state->product_info.i3c_use_int1) {
        uint8_t bank = 4;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
        rc += icm4x6xx_com_read_wrapper(instance,
                                        REG_INT_SOURCE8,
                                        &reg,
                                        1,
                                        &xfer_bytes);
        bank = 0;
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_BANK_SEL,
                                         &bank,
                                         1,
                                         &xfer_bytes,
                                         false);
        *enabled = reg & BIT_WM_IBI_EN;
    } else {
#else
    {
#endif

#ifdef ICM4X6XX_USE_INT2
        rc += icm4x6xx_com_read_wrapper(instance,
                                        REG_INT_SOURCE3,
                                        &reg,
                                        1,
                                        &xfer_bytes);
        *enabled = reg & WM_INT_INT2_EN_MASK;
#else
        rc += icm4x6xx_com_read_wrapper(instance,
                                        REG_INT_SOURCE0,
                                        &reg,
                                        1,
                                        &xfer_bytes);
        *enabled = reg & WM_INT_EN_MASK;
#endif
    }

    return rc;
}

/**
 * @brief Enable or Disable motion detect interrupt.
 *
 * @param[in] enable    Identify enable motion detect interrupt or not.
 *                      true: enable motion detect interrupt
 *                      false: disable motion detect interrupt
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_en_wom_int(
    sns_sensor_instance *instance,
    bool enable)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    uint32_t xfer_bytes;
    int rc = 0;

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
                                  REG_INT_SOURCE9,
                                  enable ? (BIT_WOM_Z_IBI_EN | BIT_WOM_Y_IBI_EN | BIT_WOM_X_IBI_EN) : 0,
                                  &xfer_bytes,
                                  false,
                                  BIT_WOM_Z_IBI_EN | BIT_WOM_Y_IBI_EN | BIT_WOM_X_IBI_EN);

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
                                  REG_INT_SOURCE4,
                                  enable ? WOM_INT2_EN_MASK : 0,
                                  &xfer_bytes,
                                  false,
                                  WOM_INT2_EN_MASK);
#else
        rc += icm4x6xx_write_mask(instance,
                                  REG_INT_SOURCE1,
                                  enable ? WOM_EN_MASK : 0,
                                  &xfer_bytes,
                                  false,
                                  WOM_EN_MASK);
#endif
    }

    return rc;
}

/**
 * @brief Enable or Disable smd interrupt.
 *
 * @param[in] enable    Identify enable smd interrupt or not.
 *                      true: enable smd interrupt
 *                      false: disable smd interrupt
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_en_smd_int(
    sns_sensor_instance *instance,
    bool enable)
{
#ifdef ICM4X6XX_ENABLE_I3C
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
#endif
    uint32_t xfer_bytes;
    int rc = 0;

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
                                  REG_INT_SOURCE9,
                                  enable ? BIT_SMD_IBI_EN : 0,
                                  &xfer_bytes,
                                  false,
                                  BIT_SMD_IBI_EN);

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
        rc += icm4x6xx_write_mask(instance,
                                  REG_INT_SOURCE1,
                                  enable ? SMD_EN_MASK : 0,
                                  &xfer_bytes,
                                  false,
                                  SMD_EN_MASK);
    }

    return rc;
}

/**
 * @brief Enable or Disable Accel.
 *
 * @param[in] enable    Identify enable accel or not.
 *                      true: enable accel
 *                      false: disable accel
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_set_accel_mode(
    sns_sensor_instance *instance,
    icm4x6xx_power_mode mode)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    int rc = 0;

    if (mode == state->accel_info.curr_mode)
        return rc;

    ICM4X6XX_INST_PRINTF(HIGH, instance, "acc pwr mode %d", mode);

    rc += icm4x6xx_write_mask(instance,
                              REG_PWR_MGMT_0,
                              mode,
                              &xfer_bytes,
                              false,
                              ACCEL_LNM_MASK);

    /* 9136 Havana needs 200us to sync fly changes */
    if (HAVANA == state->product_info.series)       //ACC/GYRO Power mode change time(us)
        sns_busy_wait(sns_convert_ns_to_ticks(200 * 1000));

    if (rc == SNS_RC_SUCCESS) {
        state->accel_info.is_enabled =
            (mode == ICM4X6XX_A_LPM || mode== ICM4X6XX_A_LNM) ? true : false;
        state->accel_info.curr_mode = mode;
    }

    return rc;
}

/**
 * @brief Enable or Disable Gyro.
 *
 * @param[in] enable    Identify enable gyro or not.
 *                      true: enable gyro
 *                      false: disable gyro
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_en_gyro(
    sns_sensor_instance *instance,
    bool enable)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    int rc = 0;

    if (state->gyro_info.is_enabled == enable)
        return rc;

    // but it seems even we do nothing for 9099,
    // it won't have any bad impact.
    rc += icm4x6xx_write_mask(instance,
                               REG_PWR_MGMT_0,
                               enable ? GYRO_LNM_MASK : 0,
                               &xfer_bytes,
                               false,
                               GYRO_LNM_MASK);

    /* 9136 Havana needs 200us to sync fly changes */
    if (HAVANA == state->product_info.series) {     //ACC/GYRO Power mode change time(us)
        sns_busy_wait(sns_convert_ns_to_ticks(200 * 1000));
    }

    if (rc == SNS_RC_SUCCESS)
        state->gyro_info.is_enabled = enable;

    return rc;
}

#ifdef ICM4X6XX_ENABLE_HIGHG
int icm4x6xx_en_high_shock(
    sns_sensor_instance *instance,
    bool enable)
{
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

    rc += icm4x6xx_write_mask(instance,
                              REG_APEX_CONFIG0,
                              enable ? BIT_HIGH_EN : 0,
                              &xfer_bytes,
                              false,
                              BIT_HIGH_EN | BIT_DMP_POWER_SAVE_EN);

    return rc;
}
#endif

#ifdef ICM4X6XX_ENABLE_LOWG
/**
 * @brief Enable or Disable Freefall.
 *
 * @param[in] enable    Identify enable freefall or not.
 *                      true: enable freefall
 *                      false: disable freefall
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_en_freefall(
    sns_sensor_instance *instance,
    bool enable)
{
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

    rc += icm4x6xx_write_mask(instance,
                              REG_APEX_CONFIG0,
#ifdef ICM4X6XX_HIGHG_LOWG_HIGH_PERFORMANCE
                              enable ? BIT_LOWG_EN | BIT_DMP_ODR_100HZ : 0,
#else
                              enable ? BIT_LOWG_EN | BIT_DMP_ODR_50HZ : 0,
#endif
                              &xfer_bytes,
                              false,
                              BIT_LOWG_EN | BIT_DMP_ODR_MASK | BIT_DMP_POWER_SAVE_EN);  //need to en WOM when en BIT_DMP_POWER_SAVE_EN

    return rc;
}
#endif

#if defined ICM4X6XX_ENABLE_LOWG || defined ICM4X6XX_ENABLE_HIGHG
int icm4x6xx_resume_dmp(
    sns_sensor_instance *instance)
{
    uint32_t xfer_bytes;
    uint8_t reg_value = 0;
    uint8_t retry_num = 100;
    int rc = SNS_RC_SUCCESS;

    rc += icm4x6xx_write_mask(instance,
                              REG_SIGNAL_PATH_RESET_REG,
                              BIT_DMP_INIT_EN,
                              &xfer_bytes,
                              false,
                              BIT_DMP_INIT_EN);

    sns_busy_wait(sns_convert_ns_to_ticks(1*1000*1000));

    do {
        rc += icm4x6xx_com_read_wrapper(instance,
                                        REG_APEX_DATA3,
                                        &reg_value,
                                        1,
                                        &xfer_bytes);
        if (reg_value & BIT_DMP_IDLE)
            break;

        rc += sns_busy_wait(sns_convert_ns_to_ticks(1*1000*1000));
    } while(retry_num--);

    ICM4X6XX_INST_PRINTF(MED, instance, "wait dmp idle %d", retry_num);

    return rc;
}
#endif

/**
 * @brief Convert ODR to register value
 *
 * @param[in] odr  the desired odr.
 * @param[out] odr_reg  the converted odr register value.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_odr_to_reg_val(
    float odr,
    icm4x6xx_sensor_odr *odr_reg)
{
    if (odr >= (uint16_t)ICM4X6XX_ODR_2000)
        *odr_reg = ODR_2KHZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_1000)
        *odr_reg = ODR_1KHZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_500)
        *odr_reg = ODR_500HZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_200)
        *odr_reg = ODR_200HZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_100)
        *odr_reg = ODR_100HZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_50)
        *odr_reg = ODR_50HZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_25)
        *odr_reg = ODR_25HZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_12_5)
        *odr_reg = ODR_12_5HZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_6_25)
        *odr_reg = ODR_6_25HZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_3_125)
        *odr_reg = ODR_3_125HZ;
    else if (odr >= (uint16_t)ICM4X6XX_ODR_1_5625)
        *odr_reg = ODR_1_5625HZ;
    else
        *odr_reg = ODR_12_5HZ;

    return 0;
}

/**
 * @brief Set Accel ODR
 *
 * @param[in] odr    the Accel ODR will be set.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_set_accel_odr(
    sns_sensor_instance *instance,
    float odr)
{
    int rc = 0;
    uint32_t xfer_bytes;
    icm4x6xx_sensor_odr odr_reg = ODR_NOT_SUPPORTED;
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;

    if (FLT_EQUAL(state->accel_info.cur_odr, odr))
        return rc;

    rc += icm4x6xx_odr_to_reg_val(odr, &odr_reg);
    rc += icm4x6xx_write_mask(instance,
                              REG_ACCEL_CONFIG0,
                              odr_reg,
                              &xfer_bytes,
                              false,
                              ACCEL_ODR_MASK);

    if (!rc)
        state->accel_info.cur_odr = odr;

    state->accel_info.filter_delay = (sns_time)filter_delay_map[odr_reg].filter_delay;

    if ((HAVANA != state->product_info.series) && FLT_EQUAL(state->accel_info.cur_odr, ICM4X6XX_ODR_1000))
        state->accel_info.filter_delay = (sns_time)NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW15_ODR_1000 * 1000000);

#ifdef ICM4X6XX_DEBUG_VERBOSE
    ICM4X6XX_INST_PRINTF(MED, instance, "set a odr(%d/1000), filter delay(%d us)",
        (uint32_t)((state->accel_info.cur_odr)*1000),
        (uint32_t)TICK2NS(state->accel_info.filter_delay));
#endif

    return rc;
}

/**
 * @brief Set Gyro and Accel ODR, in this setting
 *        Gyro and Accel always share same odr
 *
 * @param[in] odr    the Accel/Gyro ODR will be set.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_set_gyro_odr(
    sns_sensor_instance *instance,
    float odr)
{
    int rc = 0;
    uint32_t xfer_bytes;
    icm4x6xx_sensor_odr odr_reg = ODR_NOT_SUPPORTED;
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;

    if (FLT_EQUAL(state->gyro_info.cur_odr, odr))
        return rc;

    rc += icm4x6xx_odr_to_reg_val(odr, &odr_reg);
    rc += icm4x6xx_write_mask(instance,
                              REG_GYRO_CONFIG0,
                              odr_reg,
                              &xfer_bytes,
                              false,
                              GYRO_ODR_MASK);

    if (!rc)
        state->gyro_info.cur_odr = odr;

    state->gyro_info.filter_delay = (sns_time)filter_delay_map[odr_reg].filter_delay;

    if ((HAVANA != state->product_info.series) && FLT_EQUAL(state->gyro_info.cur_odr, ICM4X6XX_ODR_1000))
        state->gyro_info.filter_delay = (sns_time)NS2TICK(ICM4X6XX_FILTER_DELAY_3RD_BW15_ODR_1000 * 1000000);

#ifdef ICM4X6XX_DEBUG_VERBOSE
    ICM4X6XX_INST_PRINTF(MED, instance, "set g odr(%d/1000), filter delay(%d us)",
        (uint32_t)((state->gyro_info.cur_odr)*1000),
        (uint32_t)TICK2NS(state->gyro_info.filter_delay));
#endif

    return rc;
}

/**
 * @brief enable/disable  accel or gyro fifo
 *
 * @param[in] en_accel    Identify enable accel fifo or not.
 *                        true: enable accel fifo, write accel data into fifo
 *                        false: disable accel fifo
 * @param[in] en_gyro     Identify enable gyro fifo or not.
 *                        true: enable gyro fifo, write gyro data into fifo
 *                        false: disable gyro fifo
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_en_fifo(
    sns_sensor_instance *instance,
    bool en_accel,
    bool en_gyro)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)(instance->state->state);
    uint8_t bit_mask = 0;
    uint8_t reg = 0;
    uint32_t xfer_bytes;

    bit_mask = FIFO_ACCEL_EN_MASK |
               FIFO_GYRO_EN_MASK |
               FIFO_TEMP_EN_MASK |
               FIFO_TMST_FSYNC_EN_MASK |
               FIFO_HIRES_EN_MASK |
               FIFO_WM_GT_TH_MASK;

    reg = (en_accel ? FIFO_ACCEL_EN_MASK : 0) |
          (en_gyro ? FIFO_GYRO_EN_MASK : 0) |
          ((en_accel || en_gyro) ? FIFO_TEMP_EN_MASK : 0) |
          ((en_accel || en_gyro) ? FIFO_TMST_FSYNC_EN_MASK : 0) |
          (state->product_info.use_hi_res ? ((en_accel || en_gyro) ? FIFO_HIRES_EN_MASK : 0) : 0) |
          FIFO_WM_GT_TH_MASK;

    return icm4x6xx_write_mask(instance,
                               REG_FIFO_CONFIG_1,
                               reg,
                               &xfer_bytes,
                               false,
                               bit_mask);
}

/**
 * @brief choose fifo working mode
 *
 * @param[in] fifo_mode    fifo working mode
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_fifo_mode(
    sns_sensor_instance *instance,
    icm4x6xx_fifo_mode fifo_mode)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)(instance->state->state);
    uint32_t xfer_bytes;
    int rc = SNS_RC_SUCCESS;

    rc += icm4x6xx_write_mask(instance,
                              REG_FIFO_CONFIG,
                              fifo_mode << BIT_FIFO_MODE_SHIFT,
                              &xfer_bytes,
                              false,
                              BIT_FIFO_MODE_CTRL_MASK);
    state->fifo_info.fifo_mode = fifo_mode;

    return rc;
}

#ifndef ICM4X6XX_USE_INT_TS
/**
 * @brief enable 20-bit timestamp reading
 *
 * @param[in] enable   enable or disable 20-bit timestamp reading
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_enable_tmst_val(
    sns_sensor_instance *instance,
    bool enable)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_SIGNAL_PATH_RESET_REG,
                               enable ? BIT_TMST_STROBE : 0,
                               &xfer_bytes,
                               true,
                               BIT_TMST_STROBE);
}
#endif
#if 0
/**
 * @brief Time Stamp delta Enable
 *
 * @param[in] enable   enable or disable delta_tmst
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_enable_delta_tmst(
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

    return icm4x6xx_write_mask(instance,
                               tmst_config_reg,
                               enable ? (BIT_TMST_DELTA_EN | TMST_SREG_ON_EN | BIT_TMST_TO_REGS_EN | BIT_TMST_EN) : 0,
                               &xfer_bytes,
                               false,
                               BIT_TMST_DELTA_EN | TMST_SREG_ON_EN | BIT_TMST_TO_REGS_EN | BIT_TMST_EN);
}
#endif

/**
 * @brief config smd mode
 *
 * @param[in] smd_mode  set smd to this "smd_mode" mode
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_smd_mode(
    sns_sensor_instance *instance,
    icm4x6xx_smd_mode mode)
{
    uint32_t xfer_bytes;
    uint8_t bit_mask = BIT_WOM_AND_MODE | BIT_WOM_COMPARE_PRE | SMD_MODE_MASK;
    // BIT_WOM_COMPARE_PRE will cause constant speed moving wom not triggered
    uint8_t reg = mode | BIT_WOM_COMPARE_PRE;

    return icm4x6xx_write_mask(instance,
                               REG_SMD_CONFIG,
                               reg,
                               &xfer_bytes,
                               false,
                               bit_mask);
}

/**
 * @brief set motion detect threshold
 *
 * @param[in] threshold    set motion detect threshold to [threshold]mg
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_wom_threshold(
    sns_sensor_instance *instance,
    uint8_t threshold)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    int rc = 0;
    uint8_t reg_value = 0;
    uint8_t wom_thresh_bank = WOM_THR_BANK;
    uint8_t wom_x_thresh_reg = REG_WOM_X_THR;
    uint8_t wom_y_thresh_reg = REG_WOM_Y_THR;
    uint8_t wom_z_thresh_reg = REG_WOM_Z_THR;

    /* Choose suitable register address for different chip */
    if (HAVANA == state->product_info.series) {
        wom_thresh_bank = 0;
        wom_x_thresh_reg = REG_HAVANA_ACCEL_WOM_X_THR;
        wom_y_thresh_reg = REG_HAVANA_ACCEL_WOM_Y_THR;
        wom_z_thresh_reg = REG_HAVANA_ACCEL_WOM_Z_THR;
    }

    reg_value = wom_thresh_bank;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &wom_thresh_bank,
                                     1,
                                     &xfer_bytes,
                                     false);

    /* Convert to mg, 1LSB == 4mg*/
    reg_value = threshold >> 2;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     wom_x_thresh_reg,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);
    rc += icm4x6xx_com_write_wrapper(instance,
                                     wom_y_thresh_reg,
                                     &reg_value,
                                     1,
                                     &xfer_bytes,
                                     false);
    rc += icm4x6xx_com_write_wrapper(instance,
                                     wom_z_thresh_reg,
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

/**
 * @brief set accel bandwidth
 *
 * @param[in] order    choose accel bandwidth
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_accel_bandwidth(
    sns_sensor_instance *instance,
    icm4x6xx_bandwidth bw)
{
    uint32_t xfer_bytes;

    ICM4X6XX_INST_PRINTF(HIGH, instance, "acc bw %d", bw);

    return icm4x6xx_write_mask(instance,
                               REG_GYRO_ACCEL_CONFIG0,
                               bw << BIT_ACCEL_BW_SHIFT,
                               &xfer_bytes,
                               false,
                               BIT_ACCEL_BW_MASK);
}

/**
 * @brief set gyro bandwidth
 *
 * @param[in] order    choose gyro bandwidth
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_gyro_bandwidth(
    sns_sensor_instance *instance,
    icm4x6xx_bandwidth bw)
{
    uint32_t xfer_bytes;

    return icm4x6xx_write_mask(instance,
                               REG_GYRO_ACCEL_CONFIG0,
                               bw,
                               &xfer_bytes,
                               false,
                               BIT_GYRO_BW_MASK);
}

/**
 * @brief read interrupt status reg one.
 *
 * @param[out] status point to the value of
 *                    interrupt status reg one
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_int_status(
    sns_sensor_instance *instance,
    uint8_t *status)
{
    uint32_t xfer_bytes;

    return icm4x6xx_com_read_wrapper(instance,
                                     REG_INT_STATUS,
                                     status,
                                     1,
                                     &xfer_bytes);
}

/**
 * @brief read interrupt status reg two.
 *
 * @param[out] status point to the value of
 *                    interrupt status reg two
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_int_status2(
    sns_sensor_instance *instance,
    uint8_t *status)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    uint8_t int_status2_addr = REG_INT_STATUS2;

    if (HAVANA == state->product_info.series)
        int_status2_addr = REG_HAVANA_INT_STATUS2;

    return icm4x6xx_com_read_wrapper(instance,
                                     int_status2_addr,
                                     status,
                                     1,
                                     &xfer_bytes);
}

/**
 * @brief read interrupt status reg three.
 *
 * @param[out] status point to the value of
 *                    interrupt status reg three
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_int_status3(
    sns_sensor_instance *instance,
    uint8_t *status)
{
    uint32_t xfer_bytes;

    return icm4x6xx_com_read_wrapper(instance,
                                     REG_INT_STATUS3,
                                     status,
                                     1,
                                     &xfer_bytes);
}

int icm4x6xx_read_accel_stcode(
    sns_sensor_instance *instance,
    uint8_t accel_st_code[3])
{
    int rc = 0;
    uint32_t xfer_bytes;
    uint8_t bank = 0;

    bank = 2;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &bank,
                                     1,
                                     &xfer_bytes,
                                     false);

    rc += icm4x6xx_com_read_wrapper(instance,
                                    REG_XA_ST_DATA,
                                    accel_st_code,
                                    3,
                                    &xfer_bytes);

    bank = 0;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &bank,
                                     1,
                                     &xfer_bytes,
                                     false);

    return rc;
}

int icm4x6xx_read_gyro_stcode(
    sns_sensor_instance *instance,
    uint8_t gyro_st_code[3])
{
    int rc = 0;
    uint32_t xfer_bytes;
    uint8_t bank = 0;

    bank = 1;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &bank,
                                     1,
                                     &xfer_bytes,
                                     false);

    rc += icm4x6xx_com_read_wrapper(instance,
                                    REG_XG_ST_DATA,
                                    gyro_st_code,
                                    3,
                                    &xfer_bytes);

    bank = 0;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &bank,
                                     1,
                                     &xfer_bytes,
                                     false);

    return rc;
}

/**
 * @brief read fifo count.
 *
 * @param[out] count point to the value of
 *                   fifo count
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_fifo_count(
    sns_sensor_instance *instance,
    uint16_t *count)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    int rc = 0;
    uint32_t xfer_bytes;
    uint8_t buff[2];
    uint16_t max_count = 0;

    rc = icm4x6xx_com_read_wrapper(instance,
                                   REG_FIFO_BYTE_COUNT_L,
                                   buff,
                                   2,
                                   &xfer_bytes);

    *count = (uint16_t)(buff[0] << 8 | buff[1]);

    if (YOKOHAMA == state->product_info.series) {
        /* According DS 6.3 MAXIMUM FIFO STORAGE
         * the largest size FIFO size is 2080 bytes*/
        max_count = 2080;
    } else if (HAVANA == state->product_info.series) {
        /* According DS 6.3 MAXIMUM FIFO STORAGE
         * the largest size FIFO size is 1040 bytes*/
        max_count = 1040;
    } else {
        max_count = 1040;
    }

    if (*count > max_count) {
        ICM4X6XX_INST_PRINTF(ERROR, instance,
            "FF c %d", *count);
        *count = max_count;
    }

    return rc;
}

/**
 * @brief read temperature data from sensor register.
 *
 * @param[out] count point to temperature data
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_temp_data(
    sns_sensor_instance *instance,
    int16_t *temp_raw)
{
    int rc = 0;
    uint8_t buff[2];
    uint32_t xfer_bytes;

    rc = icm4x6xx_com_read_wrapper(instance,
                                   REG_TEMP_DATA0,
                                   buff,
                                   sizeof(buff),
                                   &xfer_bytes);

    *temp_raw = (int16_t)(((int16_t)buff[0]) << 8 | buff[1]);

    return rc;
}

/**
 * @brief Read Accel LSB
 *
 * @param[out] accel_raw point to the data of accel X/Y/Z LSB.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_accel_data(
    sns_sensor_instance *instance,
    int16_t accel_raw[3])
{
    int rc = 0;
    uint8_t buff[6] = {0};
    uint32_t xfer_bytes;

    rc = icm4x6xx_com_read_wrapper(instance,
                                   REG_ACCEL_DATA_X0_UI,
                                   buff,
                                   sizeof(buff),
                                   &xfer_bytes);

    accel_raw[0] = (int16_t)(((int16_t)buff[0]) << 8 | buff[1]);
    accel_raw[1] = (int16_t)(((int16_t)buff[2]) << 8 | buff[3]);
    accel_raw[2] = (int16_t)(((int16_t)buff[4]) << 8 | buff[5]);

    return rc;
}

/**
 * @brief Read Gyro LSB
 *
 * @param[out] gyro_raw    the data of gyro X/Y/Z LSB.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_gyro_data(
    sns_sensor_instance *instance,
    int16_t gyro_raw[3])
{
    int rc = 0;
    uint8_t buff[6] = {0};
    uint32_t xfer_bytes;

    rc = icm4x6xx_com_read_wrapper(instance,
                                   REG_GYRO_DATA_X0_UI,
                                   buff,
                                   sizeof(buff),
                                   &xfer_bytes);

    gyro_raw[0] = (int16_t)(((int16_t)buff[0]) << 8 | buff[1]);
    gyro_raw[1] = (int16_t)(((int16_t)buff[2]) << 8 | buff[3]);
    gyro_raw[2] = (int16_t)(((int16_t)buff[4]) << 8 | buff[5]);

    return rc;
}

#ifndef ICM4X6XX_USE_INT_TS
/**
 * @brief Read TMST LSB
 *
 * @param[out] tmst, store the TMST value.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_tmst_data(
    sns_sensor_instance *instance,
    uint16_t *tmst)
{
    int rc = 0;
    uint8_t buff[2] = {0};
    uint32_t xfer_bytes;

    rc = icm4x6xx_com_read_wrapper(instance,
                                   REG_TMST_DATA,
                                   buff,
                                   sizeof(buff),
                                   &xfer_bytes);

    *tmst = (uint16_t)(buff[0] << 8 | buff[1]);

    return rc;
}

int icm4x6xx_read_tmst_val(
    sns_sensor_instance *instance,
    uint32_t *tmst_reg)
{
    int rc = 0;
    uint8_t buff[3] = {0};
    uint32_t xfer_bytes;
    uint8_t bank;

    bank = 1;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &bank,
                                     1,
                                     &xfer_bytes,
                                     false);

    rc += icm4x6xx_com_read_wrapper(instance,
                                   REG_TMSTVAL0,
                                   buff,
                                   sizeof(buff),
                                   &xfer_bytes);

    bank = 0;
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_BANK_SEL,
                                     &bank,
                                     1,
                                     &xfer_bytes,
                                     false);

    *tmst_reg = (uint32_t)((buff[2] & 0x0f) << 16 |
                           buff[1] << 8 | buff[0]);

    return rc;
}
#endif

/**
 * @brief Read fifo data
 *
 * @param[out] buf point to fifo data.
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_read_fifo_buf(
    sns_sensor_instance *instance,
    uint8_t *buf,
    uint32_t len)
{
    uint32_t xfer_bytes;

    return icm4x6xx_com_read_wrapper(instance,
                                     REG_FIFO_DATA,
                                     buf,
                                     len,
                                     &xfer_bytes);
}

/**
 * @brief Enable or Disable accel self test.
 *
 * @param[in] enable    Identify enable self test or not.
 *                      true: enable self test
 *                      false: disable self test
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_enable_accel_self_test(
    sns_sensor_instance *instance,
    bool enable)
{
    uint32_t xfer_bytes;
    uint8_t bit_mask = BIT_ST_REGULATOR_EN |
                       BIT_ACCEL_Z_ST_EN |
                       BIT_ACCEL_Y_ST_EN |
                       BIT_ACCEL_X_ST_EN;

    return icm4x6xx_write_mask(instance,
                               REG_SELF_TEST_CONFIG,
                               enable ? bit_mask : 0,
                               &xfer_bytes,
                               false,
                               bit_mask);
}

/**
 * @brief Enable or Disable gyro self test.
 *
 * @param[in] enable    Identify enable self test or not.
 *                      true: enable self test
 *                      false: disable self test
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_enable_gyro_self_test(
    sns_sensor_instance *instance,
    bool enable)
{
    uint32_t xfer_bytes;
    uint8_t bit_mask = BIT_ST_REGULATOR_EN |
                       BIT_GYRO_Z_ST_EN |
                       BIT_GYRO_Y_ST_EN |
                       BIT_GYRO_X_ST_EN;

    return icm4x6xx_write_mask(instance,
                               REG_SELF_TEST_CONFIG,
                               enable ? bit_mask : 0,
                               &xfer_bytes,
                               false,
                               bit_mask);
}

/**
 * @brief Read fifo data
 *
 * @param[in] instance, point to sensor instance
 *            enable,   enable motion detect or not
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
void icm4x6xx_set_md_config(
     sns_sensor_instance *instance,
     bool enable)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)(instance->state->state);
    /* convert threshold to mg */
    uint16_t threshold = state->md_info.md_config.thresh * 1000 / G;

    ICM4X6XX_INST_PRINTF(MED, instance, "MD thrhd %d", threshold);

    /* Limit max threshold value */
    if (threshold > 1024)
        threshold = 1024;

    /* Limit min threshold value */
    if (threshold == 0)
        threshold = 4;

    /* update wom threshold */
    if (enable)
        icm4x6xx_set_wom_threshold(instance, threshold);

    /* ODR for WOM already set in re_hw */
    //if (enable) {
    //    icm4x6xx_set_accel_odr(instance, ICM4X6XX_ODR_50);
    //}

    /* Choose SMD mode
     * Only disable smd here, we enable smd in timer,
     * due to MD always fired */
    if (!enable)
        icm4x6xx_set_smd_mode(instance, OFF_MODE);
}

/**
 * @brief reset fifo
 *
 * @param[in] instance point to sensor instance
 *
 * @return 0 if success,
 *         non-zero value if failed.
 */
int icm4x6xx_reset_fifo(
    sns_sensor_instance *instance)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint32_t xfer_bytes;
    int rc = 0;

    if (HAVANA == state->product_info.series) {
        uint8_t fifo_config1;
        uint8_t burst_read[3];
        /* H_W_B 9052: FIFO-count mis-match due to FIFO-flush is not getting cleared if there is no serial interface activity
         * The cleanest way to issue a FIFO_FLUSH should be as follows:
         * - Stop DMA data feeding to FIFO by setting fifo_accel_en, fifo_gyro_en and fifo_hires_en to 0 in FIFO_CONFIG1 register;
         * - Set the FIFO_FLUSH bit;
         * - Wait for 100us;
         * - Make a read burst of 3 bytes;
         * - Enable back DMA data generation to FIFO by setting fifo_accel_en, fifo_gyro_en and fifo_hires_en back to 1;
         */
        /* Store fifo setting */
        rc += icm4x6xx_com_read_wrapper(instance,
                                        REG_FIFO_CONFIG_1,
                                        &fifo_config1,
                                        1,
                                        &xfer_bytes);
        /* Stop DMA data feeding */
        rc += icm4x6xx_write_mask(instance,
                                  REG_FIFO_CONFIG_1,
                                  0,
                                  &xfer_bytes,
                                  false,
                                  FIFO_HIRES_EN_MASK | FIFO_GYRO_EN_MASK | FIFO_ACCEL_EN_MASK);
        /* Set FIFO FLUSH bit */
        rc += icm4x6xx_write_mask(instance,
                                  REG_SIGNAL_PATH_RESET_REG,
                                  BIT_FIFO_FLUSH,
                                  &xfer_bytes,
                                  false,
                                  BIT_FIFO_FLUSH);
        /* Wait 100us */
        rc += sns_busy_wait(sns_convert_ns_to_ticks(100 * 1000));
        /* Make a read burst of 3 bytes */
        rc += icm4x6xx_read_fifo_buf(instance, burst_read, 3);
        /* Recovery fifo setting */
        rc += icm4x6xx_com_write_wrapper(instance,
                                         REG_FIFO_CONFIG_1,
                                         &fifo_config1,
                                         1,
                                         &xfer_bytes,
                                         false);
    } else {
        rc += icm4x6xx_write_mask(instance,
                                  REG_SIGNAL_PATH_RESET_REG,
                                  BIT_FIFO_FLUSH,
                                  &xfer_bytes,
                                  false,
                                  BIT_FIFO_FLUSH);
    }

    return rc;
}

/**
 * @brief Get current packet size, according current FIFO format
 *
 * @param[in] instance, point to sensor instance
 * @param[in] size, store currect packet size
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_get_packet_size(
    sns_sensor_instance *instance,
    uint8_t *size)
{
    icm4x6xx_instance_state *state =
        (icm4x6xx_instance_state*)instance->state->state;
    uint8_t packet_size = 0;
    int rc = 0;

    if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_20_BYTES)
        packet_size = 20;
    else if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_16_BYTES)
        packet_size = 16;
    else if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_ACCEL_8_BYTES ||
             state->fifo_info.curr_format == ICM4X6XX_FORMAT_GYRO_8_BYTES)
        packet_size = 8;
    else if (state->fifo_info.curr_format == ICM4X6XX_FORMAT_EMPTY) {
        ICM4X6XX_INST_PRINTF(HIGH, instance, "fifo disabled");
        packet_size = 0;
    } else {
        ICM4X6XX_INST_PRINTF(ERROR, instance, "incorrect ff format");
        rc = SNS_RC_FAILED;
    }

    *size = packet_size;

    return rc;
}

/**
 * @brief set fifo watermark, wm_th should be calculated by current fifo format
 *
 * @param[in] wm_th    FIFO watermark, user should calculate it
 *                      with current fifo packet format
 *
 * @return 0 if success
 *         non-zero value if failed.
 */
int icm4x6xx_set_fifo_watermark(
    sns_sensor_instance *instance)
{
    int rc = 0;
    icm4x6xx_instance_state *state = (icm4x6xx_instance_state*)instance->state->state;
    uint16_t desire_wmk = state->fifo_info.desire_wmk;
    uint8_t buff[2];
    uint8_t packet_size = 0;
    uint32_t xfer_bytes;
    bool wm_int_enabled = false;

    if (desire_wmk == state->fifo_info.cur_wmk &&
        !state->fifo_info.req_chg_ff_format)
        return rc;

    rc += icm4x6xx_get_packet_size(instance, &packet_size);
    if (packet_size == 0)
        return rc;

    /* In case of FIFO format changing fail */
    if (state->fifo_info.curr_format != state->fifo_info.desire_format) {
        desire_wmk = ((desire_wmk * packet_size) > state->product_info.fifo_size * 8) ?
                     (state->product_info.fifo_size * 8 /packet_size) : desire_wmk;
    }

    if (state->fifo_info.record_mode)
        packet_size = 1;

    rc += icm4x6xx_is_wm_int_enabled(instance, &wm_int_enabled);
    rc += icm4x6xx_en_wm_int(instance, false);

    buff[0] = desire_wmk * packet_size & 0x00ff;
    buff[1] = (desire_wmk * packet_size & 0xff00) >> 8;

    /* burst write is not supported */
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_FIFO_WM_TH_L,
                                     &buff[0],
                                     1,
                                     &xfer_bytes,
                                     false);
    rc += icm4x6xx_com_write_wrapper(instance,
                                     REG_FIFO_WM_TH_H,
                                     &buff[1],
                                     1,
                                     &xfer_bytes,
                                     false);

    rc += icm4x6xx_en_wm_int(instance, wm_int_enabled);

    if (rc == SNS_RC_SUCCESS)
        state->fifo_info.cur_wmk = desire_wmk;

    return rc;
}

int icm4x6xx_is_fifo_format_match(
    sns_sensor_instance *instance,
    icm4x6xx_fifo_format format,
    bool *match)
{
    int rc = SNS_RC_SUCCESS;
    uint8_t fifo_header = 0;
    uint8_t value_20bit = FIFO_HEADER_A_BIT | FIFO_HEADER_G_BIT | FIFO_HEADER_20_BIT;
    uint8_t value_16bit = FIFO_HEADER_A_BIT | FIFO_HEADER_G_BIT;
    icm4x6xx_fifo_format cur_format;

    rc += icm4x6xx_read_fifo_buf(instance, &fifo_header, 1);

    ICM4X6XX_INST_PRINTF(MED,instance, "ff header %#x", fifo_header);

    if (fifo_header & FIFO_HEADER_EMPTY_BIT)
        cur_format = ICM4X6XX_FORMAT_EMPTY;
    else if ((fifo_header & value_20bit) == value_20bit)
        cur_format = ICM4X6XX_FORMAT_20_BYTES;
    else if ((fifo_header & value_16bit) == value_16bit)
        cur_format = ICM4X6XX_FORMAT_16_BYTES;
    else if (fifo_header & FIFO_HEADER_A_BIT)
        cur_format = ICM4X6XX_FORMAT_ACCEL_8_BYTES;
    else if (fifo_header & FIFO_HEADER_G_BIT)
        cur_format = ICM4X6XX_FORMAT_GYRO_8_BYTES;
    else {
        cur_format = ICM4X6XX_FORMAT_UNKNOWN;
        ICM4X6XX_INST_PRINTF(ERROR, instance, "unkown header 0x%x", fifo_header);
    }

    if (cur_format == format || cur_format == ICM4X6XX_FORMAT_EMPTY)
        *match = true;
    else
        *match = false;

    return rc;
}

#ifdef ICM4X6XX_DEBUG_DUMP_FIFO
int icm4x6xx_dump_fifo(
    sns_sensor_instance *instance,
    const uint8_t       *buf,
    size_t              buf_len)
{
    int i = 0;
    for(i = 0; i < buf_len; i++) {
        ICM4X6XX_VERB_INST_PRINTF(MED, instance, "dump_fifo[%d] 0x%x", i, buf[i]);
    }

    return 0;
}
#endif /* ICM4X6XX_DEBUG_DUMP_FIFO */

int icm4x6xx_dump_reg(
    sns_sensor_instance *instance)
{
    uint32_t xfer_bytes;
    uint8_t value;
    int rc = SNS_RC_SUCCESS;
    uint8_t i = 0;
    uint8_t reg_map[] = {
        REG_INT_CONFIG,
        REG_FIFO_CONFIG,
        REG_ACCEL_DATA_X0_UI,
        REG_ACCEL_DATA_X0_UI+1,
        REG_ACCEL_DATA_X0_UI+2,
        REG_ACCEL_DATA_X0_UI+3,
        REG_ACCEL_DATA_X0_UI+4,
        REG_ACCEL_DATA_X0_UI+5,
        REG_INT_STATUS,
        REG_FIFO_BYTE_COUNT_L,
        REG_FIFO_BYTE_COUNT_L+1,
        REG_FIFO_DATA,
        REG_APEX_DATA3,
        REG_INT_STATUS2,
        REG_INT_STATUS3,
        REG_SIGNAL_PATH_RESET_REG,
        REG_INTF_CONFIG0,
        REG_INTF_CONFIG1,
        REG_PWR_MGMT_0,
        REG_GYRO_CONFIG0,
        REG_ACCEL_CONFIG0,
        REG_SMD_CONFIG,
        REG_FIFO_CONFIG_1,
        REG_FIFO_WM_TH_L,
        REG_FIFO_WM_TH_H,
        REG_INT_CONFIG0,
        REG_INT_CONFIG1,
        REG_INT_SOURCE0,
        REG_INT_SOURCE1,
        REG_INT_SOURCE3,
        REG_INT_SOURCE4,
        REG_WHO_AM_I
    };
    uint16_t n = sizeof(reg_map)/sizeof(reg_map[0]);

    for(i=0; i<n;i++) {
        rc += icm4x6xx_com_read_wrapper(instance,
                                        reg_map[i],
                                        &value,
                                        1,
                                        &xfer_bytes);

        ICM4X6XX_INST_PRINTF(LOW, instance, "reg[0x%x] 0x%x", reg_map[i], value);
    }

    return rc;
}
