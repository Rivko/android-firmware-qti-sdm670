#pragma once
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

#include "sns_icm4x6xx_sensor_instance.h"

#define REG_CHIP_CONFIG             0x11
    #define SOFT_RESET_MASK             0x01
#define REG_DRIVE_CONFIG            0x13
    #define SPI_SPEED_MASK              0x07
    #define I2C_SPEED_MASK              0x38
    #define I2C_SPEED_SHIFT             3
#define REG_INT_CONFIG              0x14
    #define INT1_ACTIVE_HIGH_MASK       0x01
    #define INT1_PUSH_PULL_MASK         0x02
    #define INT1_LATCHED_MODE_MASK      0x04
    #define INT2_ACTIVE_HIGH_MASK       0x08
    #define INT2_PUSH_PULL_MASK         0x10
    #define INT2_LATCHED_MODE_MASK      0x20
#define REG_FIFO_CONFIG             0x16
    #define BIT_FIFO_MODE_SHIFT (6)
    #define BIT_FIFO_MODE_CTRL_MASK ((0x03)<<BIT_FIFO_MODE_SHIFT)
    #define BIT_FIFO_MODE_CTRL_BYPASS ((0x00)<<BIT_FIFO_MODE_SHIFT)
    #define BIT_FIFO_MODE_CTRL_STREAM ((0x01)<<BIT_FIFO_MODE_SHIFT)
    #define BIT_FIFO_MODE_CTRL_SNAPSHOT ((0x02)<<BIT_FIFO_MODE_SHIFT)
#define REG_TEMP_DATA0              0x1d
#define REG_ACCEL_DATA_X0_UI        0x1F
#define REG_GYRO_DATA_X0_UI         0x25
#define REG_TMST_DATA               0x2b
#define REG_FIFO_BYTE_COUNT_L       0x2e
#define REG_INT_STATUS              0x2d
    #define RESET_DONE_MASK             0x10
    #define DRI_INT_MASK                0x08
    #define WM_INT_MASK                 0x04
    #define OVERFLOW_INT_MASK           0x02
#define REG_FIFO_DATA               0x30
#define REG_APEX_DATA3              0x34
    #define BIT_DMP_IDLE                0X04
#define REG_INT_STATUS2             0x37
    #define BIT_INT_STATUS_SMD          0x08
    #define BIT_INT_STATUS_WOM_Z        0x04
    #define BIT_INT_STATUS_WOM_Y        0x02
    #define BIT_INT_STATUS_WOM_X        0x01
/* Havana INT_STATUS2 config register */
#define REG_HAVANA_INT_STATUS2      0x59

#define REG_INT_STATUS3             0x38
    #define BIT_INT_DMP_POWER_SAVE_DET      0x40
    #define BIT_INT_STATUS3_STEP_DET        0x20
    #define BIT_INT_STATUS3_STEP_CNT_OVFL   0x10
    #define BIT_INT_STATUS3_TILT_DET        0x08
    #define BIT_INT_STATUS3_WAKE_DET        0x04
    #define BIT_INT_STATUS3_LOW_G_DET       0x04
    #define BIT_INT_STATUS3_SLEEP_DET       0x02
    #define BIT_INT_STATUS3_HIGH_G_DET      0x02
    #define BIT_INT_STATUS3_TAP_DET         0x01

#define REG_SIGNAL_PATH_RESET_REG   0x4B
    #define BIT_DMP_INIT_EN             0x40
    #define BIT_ABORT_AND_RESET         0x08
    #define BIT_TMST_STROBE             0x04
    #define BIT_FIFO_FLUSH              0x02
    #define BIT_TEMP_RST                0x01
#define REG_INTF_CONFIG0            0x4C
    #define FIFO_HOLD_LAST_DATA_EN      0x80
    #define RECORD_MODE_MASK            0x40
    #define FIFO_COUNT_BIG_ENDIAN_MASK  0x20
    #define SENSOR_DATA_BIG_ENDIAN_MASK 0x10
    #define UI_INTF_MASK                0x03
#define REG_INTF_CONFIG1            0x4D
    #define BIT_RTC_MODE_EN             0x04
#define REG_PWR_MGMT_0              0x4E
    #define ACCEL_LNM_MASK              0x03
    #define GYRO_LNM_MASK               0x0C
    #define ICM426XX_IDLE_MASK          0x10
#define REG_GYRO_CONFIG0            0x4F
    #define GYRO_FSR_MASK               0xE0
    #define GYRO_ODR_MASK               0x0F
    #define ACCEL_FSR_SHIFT             5
#define REG_ACCEL_CONFIG0           0x50
    #define ACCEL_FSR_MASK              0xE0
    #define ACCEL_ODR_MASK              0x0f
    #define GYRO_FSR_SHIFT              5
#define REG_GYRO_CONFIG1            0x51
    #define BIT_GYRO_AVG_FILT_8K_HZ     0x10
    #define BIT_GYRO_FILT_ORD_SHIFT     0x02
    #define BIT_GYRO_FILT_ORD_MASK      0x0C
#define REG_GYRO_ACCEL_CONFIG0      0x52
    #define BIT_GYRO_BW_MASK                0x0f
    #define BIT_ACCEL_BW_MASK               0xf0
    #define BIT_ACCEL_BW_SHIFT              0x04
#define REG_ACC_CONFIG1             0x53
    #define BIT_ACC_AVG_FILT_8K_HZ      0x01
    #define BIT_ACC_FILT_ORD_SHIFT      0x03
    #define BIT_ACC_FILT_ORD_MASK       0x18
#define REG_TMST_CONFIG_REG         0x54
    #define TMST_SREG_ON_EN             0x80
    #define BIT_TMST_TO_REGS_EN         0x10
    #define BIT_TMST_RESOL              0x08
    #define BIT_TMST_DELTA_EN           0x04
    #define BIT_TMST_FSYNC_EN           0x02
    #define BIT_TMST_EN                 0x01

#define REG_APEX_CONFIG0            0x56
    #define BIT_DMP_POWER_SAVE_EN       0x80
    #define BIT_LOWG_EN                 0x08
    #define BIT_HIGH_EN                 0x04
    #define BIT_DMP_ODR_25HZ            0x00
    #define BIT_DMP_ODR_50HZ            0x02
    #define BIT_DMP_ODR_100HZ           0x03
    #define BIT_DMP_ODR_MASK            0x03

/* Havana TMST config register */
#define REG_HAVANA_TMST_CONFIG_REG      0x5A

/* Havana WOM THRESH register */
#define REG_HAVANA_ACCEL_WOM_X_THR      0x54
#define REG_HAVANA_ACCEL_WOM_Y_THR      0x55
#define REG_HAVANA_ACCEL_WOM_Z_THR      0x56

#define REG_SMD_CONFIG              0x57
    #define BIT_WOM_AND_MODE            0x08
    #define BIT_WOM_COMPARE_PRE         0x04
    #define SMD_MODE_MASK               0x03
#define REG_FIFO_CONFIG_1           0x5f
    #define FIFO_RESUME_PARTIAL_RD_MASK 0x40
    #define FIFO_WM_GT_TH_MASK          0x20
    #define FIFO_HIRES_EN_MASK          0x10
    #define FIFO_TMST_FSYNC_EN_MASK     0x08
    #define FIFO_TEMP_EN_MASK           0x04
    #define FIFO_GYRO_EN_MASK           0x02
    #define FIFO_ACCEL_EN_MASK          0x01
#define REG_FIFO_WM_TH_L            0x60
#define REG_FIFO_WM_TH_H            0x61
#define REG_FSYNC_CONFIG            0x62
#define REG_INT_CONFIG0             0x63
    #define INT_FIFO_THS_CLR_SEL_MASK   0x0C
    #define INT_FIFO_THS_CLR_SEL_SHIFT  0x02
#define REG_INT_CONFIG1             0x64
    #define BIT_INT_ASY_RST_DIS_MASK    0x10
#define REG_INT_SOURCE0             0x65
    #define DRI_EN_MASK                 0x08
    #define WM_INT_EN_MASK              0x04
    #define FIFO_FULL_EN_MASK           0x02
#define REG_INT_SOURCE1             0x66
    #define WOM_EN_MASK                 0x07
    #define SMD_EN_MASK                 0x08
#define REG_INT_SOURCE3             0x68
    #define DRI_INT2_EN_MASK            0x08
    #define WM_INT_INT2_EN_MASK         0x04
    #define FIFO_FULL_INT2_EN_MASK      0x02
#define REG_INT_SOURCE4             0x69
    #define WOM_INT2_EN_MASK            0x07
    #define SMD_INT2_EN_MASK            0x08
#define REG_SELF_TEST_CONFIG    0x70
    #define BIT_ST_REGULATOR_EN         0x40
    #define BIT_ACCEL_Z_ST_EN           0x20
    #define BIT_ACCEL_Y_ST_EN           0x10
    #define BIT_ACCEL_X_ST_EN           0x08
    #define BIT_GYRO_Z_ST_EN            0x04
    #define BIT_GYRO_Y_ST_EN            0x02
    #define BIT_GYRO_X_ST_EN            0x01
#define REG_SCAN0              0x71
    #define BIT_DMP_MEM_ACCESS_EN		0x08
    #define BIT_MEM_OTP_ACCESS_EN		0x04
    #define BIT_FIFO_MEM_RD_SYS			0x02
    #define BIT_FIFO_MEM_WR_SER			0x01
#define REG_MEM_BANK_SEL       0x72
#define REG_MEM_START_ADDR     0x73
#define REG_MEM_R_W            0x74
#define REG_WHO_AM_I                0x75
#define REG_BANK_SEL                0x76

/* Bank 1 */
#define REG_XG_ST_DATA              0x5F
#define REG_YG_ST_DATA              0x60
#define REG_ZG_ST_DATA              0x61

#define REG_TMSTVAL0                0x62
#define REG_TMSTVAL1                0x63
#define REG_TMSTVAL2                0x64
#define REG_INTF_CONFIG5            0x7B
    #define BIT_PIN9_FUNC_INT2		    0x00
    #define BIT_PIN9_FUNC_FSYNC		    0x02
    #define BIT_PIN9_FUNC_CLKIN		    0x04
    #define BIT_PIN9_FUNC_MASK		    0x06
#define REG_INTF_CONFIG6            0x7C
    #define I3C_SDR_EN                  0x01
    #define I3C_DDR_EN                  0x02

/* Bank 2 */
#define REG_XA_ST_DATA              0x3B
#define REG_YA_ST_DATA              0x3C
#define REG_ZA_ST_DATA              0x3D

/* Bank 4 */
#define WOM_THR_BANK            4
#define REG_APEX_CONFIG4        0x43
#define REG_APEX_CONFIG5        0x44
#define REG_APEX_CONFIG6        0x45
#define REG_WOM_X_THR           0x4A
#define REG_WOM_Y_THR           0x4B
#define REG_WOM_Z_THR           0x4C
#define REG_INT_SOURCE6         0x4D
    #define BIT_DMP_POWER_SAVE_INT1_EN  0x40
    #define BIT_STEP_DET_INT1_EN        0x20
    #define BIT_STEP_CNT_OVFL_INT1_EN   0x10
    #define BIT_TILT_DET_INT1_EN        0x8
    #define BIT_WAKE_DET_INT1_EN        0x4
    #define BIT_LOWG_DET_INT1_EN        0x4
    #define BIT_SLEEP_DET_INT1_EN       0x2
    #define BIT_HIGHG_DET_INT1_EN       0x2
    #define BIT_TAP_DET_INT1_EN         0x1
#define REG_INT_SOURCE7         0x4E
    #define BIT_DMP_POWER_SAVE_INT2_EN  0x40
    #define BIT_STEP_DET_INT2_EN        0x20
    #define BIT_STEP_CNT_OVFL_INT2_EN   0x10
    #define BIT_TILT_DET_INT2_EN        0x8
    #define BIT_WAKE_DET_INT2_EN        0x4 //Not mentioned in Excel Reg Map
    #define BIT_LOWG_DET_INT2_EN        0x4 //From Excel Reg Map
    #define BIT_HIGHG_DET_INT2_EN       0x2
    #define BIT_TAP_DET_INT2_EN         0x1
#define REG_INT_SOURCE8         0x4F
    #define BIT_DRI_IBI_EN              0x08
    #define BIT_WM_IBI_EN               0x04
    #define BIT_FIFO_FULL_IBI_EN        0x02
#define REG_INT_SOURCE9         0x50
    #define BIT_I3C_PROTOCOL_ERROR_IBI_EN 0x80
    #define BIT_SMD_IBI_EN                0x10
    #define BIT_WOM_Z_IBI_EN              0x08
    #define BIT_WOM_Y_IBI_EN              0x04
    #define BIT_WOM_X_IBI_EN              0x02
#define REG_INT_SOURCE10         0x51
    #define BIT_STEP_DET_IBI_EN        0x20
    #define BIT_STEP_CNT_OVFL_IBI_EN   0x10
    #define BIT_TILT_DET_IBI_EN        0x8
    #define BIT_WAKE_DET_IBI_EN        0x4
    #define BIT_LOWG_DET_IBI_EN        0x4
    #define BIT_SLEEP_DET_IBI_EN       0x2
    #define BIT_HIGHG_DET_IBI_EN       0x2
    #define BIT_TAP_DET_IBI_EN         0x1

/* Motion detect configuration */
#define ICM4X6XX_MD_THRESH          (1.0f)             // m/s2

#define ICM4X6XX_SW_RESET_CHECK_TIMES (10)

#define FIFO_HEADER_EMPTY_BIT    (0x80)
#define FIFO_HEADER_A_BIT        (0x40)
#define FIFO_HEADER_G_BIT        (0x20)
#define FIFO_HEADER_20_BIT        (0x10)

/********* Function Declarations *********/
int icm4x6xx_set_accel_fsr(
    sns_sensor_instance *instance,
    icm4x6xx_accel_fsr fsr);
int icm4x6xx_set_accel_bandwidth(
    sns_sensor_instance *instance,
    icm4x6xx_bandwidth bw);
int icm4x6xx_set_accel_filter_order(
    sns_sensor_instance *instance,
    icm4x6xx_filter_order order);
int icm4x6xx_set_gyro_filter_order(
    sns_sensor_instance *instance,
    icm4x6xx_filter_order order);
int icm4x6xx_read_accel_data(
    sns_sensor_instance *instance,
    int16_t accel_raw[3]);
int icm4x6xx_read_gyro_data(
    sns_sensor_instance *instance,
    int16_t gyro_raw[3]);
int icm4x6xx_set_gyro_fsr(
    sns_sensor_instance *instance,
    icm4x6xx_gyro_fsr fsr);
int icm4x6xx_enable_accel_self_test(
    sns_sensor_instance *instance,
    bool enable);
int icm4x6xx_enable_gyro_self_test(
    sns_sensor_instance *instance,
    bool enable);
int icm4x6xx_en_wm_int(
    sns_sensor_instance *instance,
    bool enable);
int icm4x6xx_en_dri(
    sns_sensor_instance *instance,
    bool enable);
int icm4x6xx_is_wm_int_enabled(
    sns_sensor_instance *instance,
    bool *enabled);
int icm4x6xx_en_wom_int(
    sns_sensor_instance *instance,
    bool enable);
int icm4x6xx_set_accel_mode(
    sns_sensor_instance *instance,
    icm4x6xx_power_mode mode);
int icm4x6xx_en_gyro(
    sns_sensor_instance *instance,
    bool enable);
#ifdef ICM4X6XX_ENABLE_LOWG
int icm4x6xx_en_freefall(
    sns_sensor_instance *instance,
    bool enable);
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
int icm4x6xx_en_high_shock(
    sns_sensor_instance *instance,
    bool enable);
#endif
#if defined ICM4X6XX_ENABLE_LOWG || defined ICM4X6XX_ENABLE_HIGHG
int icm4x6xx_resume_dmp(
    sns_sensor_instance *instance);
#endif
int icm4x6xx_set_accel_odr(
    sns_sensor_instance *instance,
    float odr);
int icm4x6xx_set_gyro_odr(
    sns_sensor_instance *instance,
    float odr);
int icm4x6xx_en_fifo(
    sns_sensor_instance *instance,
    bool en_accel,
    bool en_gyro);
int icm4x6xx_set_fifo_mode(
    sns_sensor_instance *instance,
    icm4x6xx_fifo_mode fifo_mode);
int icm4x6xx_read_accel_stcode(
    sns_sensor_instance *instance,
    uint8_t accel_st_code[3]);
int icm4x6xx_read_gyro_stcode(
    sns_sensor_instance *instance,
    uint8_t gyro_st_code[3]);
int icm4x6xx_read_int_status(
    sns_sensor_instance *instance,
    uint8_t *status);
int icm4x6xx_read_int_status2(
    sns_sensor_instance *instance,
    uint8_t *status);
int icm4x6xx_read_fifo_count(
    sns_sensor_instance *instance,
    uint16_t *count);
int icm4x6xx_read_temp_data(
    sns_sensor_instance *instance,
    int16_t *temp_raw);
int icm4x6xx_read_fifo_buf(
    sns_sensor_instance *instance,
    uint8_t *buf,
    uint32_t len);
void icm4x6xx_set_md_config(
     sns_sensor_instance *instance,
     bool enable);
int icm4x6xx_set_smd_mode(
    sns_sensor_instance *instance,
    icm4x6xx_smd_mode mode);
#ifndef ICM4X6XX_USE_INT_TS
int icm4x6xx_read_tmst_data(
    sns_sensor_instance *instance,
    uint16_t *tmst);
int icm4x6xx_read_tmst_val(
    sns_sensor_instance *instance,
    uint32_t *tmst_reg);
int icm4x6xx_enable_tmst_val(
    sns_sensor_instance *instance,
    bool enable);
#endif
int icm4x6xx_reset_fifo(
    sns_sensor_instance *instance);
int icm4x6xx_get_packet_size(
    sns_sensor_instance *instance,
    uint8_t *size);
int icm4x6xx_set_fifo_watermark(
    sns_sensor_instance *instance);
int icm4x6xx_is_fifo_format_match(
    sns_sensor_instance *instance,
    icm4x6xx_fifo_format format,
    bool *match);
#ifdef ICM4X6XX_ENABLE_I3C
sns_rc icm4x6xx_enter_i3c_mode(
       sns_sensor_instance *const instance,
       icm4x6xx_com_port_info *com_port,
       sns_sync_com_port_service *scp_service);
#endif
int icm4x6xx_reset_device(
    sns_sensor_instance *instance);
int icm4x6xx_read_id(
    sns_sync_com_port_service *scp_service,
    sns_sync_com_port_handle *port_handle,
    uint8_t *id);
int icm4x6xx_en_fifo_full_int(
    sns_sensor_instance *instance,
    bool enable);
int icm4x6xx_dump_reg(
    sns_sensor_instance *instance);
#ifdef ICM4X6XX_DEBUG_DUMP_FIFO
int icm4x6xx_dump_fifo(
    sns_sensor_instance *instance,
    const uint8_t       *buf,
    size_t              buf_len);
#endif
int icm4x6xx_set_accel_bandwidth(
    sns_sensor_instance *instance,
    icm4x6xx_bandwidth bw);
int icm4x6xx_set_gyro_bandwidth(
    sns_sensor_instance *instance,
    icm4x6xx_bandwidth bw);
sns_rc icm4x6xx_com_read_wrapper(
       sns_sensor_instance *instance,
       uint32_t reg_addr,
       uint8_t *buffer,
       uint32_t bytes,
       uint32_t *xfer_bytes);
sns_rc icm4x6xx_com_write_wrapper(
       sns_sensor_instance *instance,
       uint32_t reg_addr,
       uint8_t *buffer,
       uint32_t bytes,
       uint32_t *xfer_bytes,
       bool save_write_time);
int icm4x6xx_write_mask(
    sns_sensor_instance *instance,
    uint32_t reg_addr,
    uint8_t reg_value,
    uint32_t *xfer_bytes,
    bool save_write_time,
    uint8_t mask);
int icm4x6xx_read_int_status3(
    sns_sensor_instance *instance,
    uint8_t *status);