#pragma once
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

#include "sns_icm4x6xx_build_config.h"
#include <stdint.h>
#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_sensor_instance.h"
#include "sns_time.h"
#include "sns_printf.h"

#include "sns_sensor_uid.h"
#include "sns_sensor_util.h"

#include "sns_async_com_port.pb.h"
#include "sns_diag_service.h"
#include "sns_interrupt.pb.h"
#include "sns_island_service.h"
#include "sns_icm4x6xx_dae_if.h"
#include "sns_motion_detect.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"

#include "sns_math_util.h"
#include "sns_registry_util.h"

#ifdef ICM4X6XX_ENABLE_LOWG
#include "sns_lowg.pb.h"
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
#include "sns_highg.pb.h"
#endif

/** Forward Declaration of Instance API */
sns_sensor_instance_api icm4x6xx_sensor_instance_api;

#define ODR_1K_2_500_COMPARE_INTVL (28800) /* 28800 = 19200 * 1.5 */

#define HB_TIMEOUT_TICKS    (19200 * 100) //100ms

/* CTS(JitterVerification.java) allowed jitter is +/-2% */
#define CTS_JITTER_PCNT (0.98f)

#define INTVL_TOLERANCE_FACTOR (1.05f)
#define LPF_PARAMETER_N 25
#define MAX_SCP_SAMPLES 20

#define ACC_START_UP_TIME (10 * 1000 * 1000) //10ms, unit is ns

#define ICM4X6XX_HB_ODR_COUNT 3

#define FLT_EPSILON                 (1.0)
#define FLT_EQUAL(a, b)             (fabs((a) - (b)) < FLT_EPSILON)

#define DEG2RAD(dps)                ((dps) * PI / 180.0f)
#define SENSOR_DATA_BITS(HI_RES)    ((HI_RES) ? 20 : 16) /* internal use 19 bits, but it is 20 bits data */
#define ACC_MIN_RANGE(HI_RANGE)  ((HI_RANGE) ? 2 : 1)
#define GYRO_MIN_RANGE(HI_RANGE)  ((HI_RANGE) ? 31.25f : 15.625f)
#define ACC_RANGE_UNIT_G(FSR_SEL, HI_RANGE)      (ACC_MIN_RANGE(HI_RANGE) << (ACC_RANGE_MAX - (FSR_SEL)))
#define ACC_RANGE_M2_PER_S(FSR_SEL, HI_RANGE)     (ACC_RANGE_UNIT_G((FSR_SEL), (HI_RANGE)) * G)
#define ACC_RES_M2_PER_S(FSR_SEL, HI_RES, HI_RANGE)   (ACC_RANGE_M2_PER_S((FSR_SEL), (HI_RANGE)) / (float)(1 << (SENSOR_DATA_BITS(HI_RES) -1)))
#define ACC_RES_MG_LSB(FSR_SEL, HI_RES, HI_RANGE)     (ACC_RANGE_UNIT_G((FSR_SEL), (HI_RANGE)) * 1000.0f / (float)(1 << (SENSOR_DATA_BITS(HI_RES) -1)))
#define GYRO_RANGE_DPS(FSR_SEL, HI_RANGE)     (GYRO_MIN_RANGE(HI_RANGE) * (1 << (GYRO_RANGE_MAX - (FSR_SEL))))
#define GYRO_RANGE_RPS(FSR_SEL, HI_RANGE)     (DEG2RAD(GYRO_RANGE_DPS((FSR_SEL), (HI_RANGE))))
#define GYRO_RES_RPS(FSR_SEL, HI_RES, HI_RANGE)       (GYRO_RANGE_RPS((FSR_SEL), (HI_RANGE)) / (float)(1 << (SENSOR_DATA_BITS(HI_RES) -1)))
#define GYRO_RES_DPS(FSR_SEL, HI_RES, HI_RANGE)       (GYRO_RANGE_DPS((FSR_SEL), (HI_RANGE)) / (float)(1 << (SENSOR_DATA_BITS(HI_RES) -1)))

#define NS2TICK(ns) ((ns) * 19.2f / 1000) //19.2MHz tick frequency
#define TICK2NS(tick) ((tick) / 19.2f) //19.2MHz tick frequency

/* Driver runs factory calibration @100Hz, driver will discard some start-up samples */
#define FACTORY_CAL_DISCARD_A   (5) //Accel 9470
#define FACTORY_CAL_DISCARD_G   (8)
#define FACTORY_CAL_SAMPLES     (60)

/** Supported opertating modes */
#define ICM4X6XX_NORMAL       "NORMAL"
#define ICM4X6XX_LPM          "LPM"
#define ICM4X6XX_HIGH_PERF    "HIGH_PERF"

#define ICM4X6XX_HAVANA_MAX_FIFO_SIZE (1040)
#define ICM4X6XX_YOKOHAMA_MAX_FIFO_SIZE (2080)

/**
 * ICM4X6XX ODR (Hz) definitions
 */
#define ICM4X6XX_ODR_0                 0.0
#define ICM4X6XX_ODR_1_5625            1.5625
#define ICM4X6XX_ODR_3_125             3.125
#define ICM4X6XX_ODR_6_25              6.25
#define ICM4X6XX_ODR_12_5              12.5
#define ICM4X6XX_ODR_25                25.0
#define ICM4X6XX_ODR_50                50.0
#define ICM4X6XX_ODR_100               100.0
#define ICM4X6XX_ODR_200               200.0
#define ICM4X6XX_ODR_500               500.0
#define ICM4X6XX_ODR_1000              1000.0
#define ICM4X6XX_ODR_2000              2000.0

#define ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_25                31.8f // a/g filter delay:31.8ms
#define ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_50                15.9f // a/g filter delay:15.9ms
#define ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_100               8.0f  // a/g filter delay:8.0ms
#define ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_200               4.0f  // a/g filter delay:4.0ms
#define ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_500               1.6f  // a/g filter delay:1.6ms
#define ICM4X6XX_FILTER_DELAY_3RD_BW2_ODR_1000              0.8f  // a/g filter delay:0.8ms
#define ICM4X6XX_FILTER_DELAY_3RD_BW15_ODR_1000             0.2f  // a/g filter delay:0.2ms

typedef enum icm4x6xx_sensor_odr {
    ODR_NOT_SUPPORTED   = 0,
    ODR_32KHZ   = 1,
    ODR_16KHZ   = 2,
    ODR_8KHZ    = 3,
    ODR_4KHZ    = 4,
    ODR_2KHZ    = 5,
    ODR_1KHZ    = 6,
    ODR_200HZ   = 7,
    ODR_100HZ   = 8,
    ODR_50HZ    = 9,
    ODR_25HZ    = 10,
    ODR_12_5HZ  = 11,
    ODR_6_25HZ  = 12,
    ODR_3_125HZ  = 13,
    ODR_1_5625HZ  = 14,
    ODR_500HZ = 15,
} icm4x6xx_sensor_odr;

typedef enum icm4x6xx_accel_fsr {
    ACC_RANGE_16G = 0,
    ACC_RANGE_8G = 1,
    ACC_RANGE_4G = 2,
    ACC_RANGE_2G = 3,
    ACC_RANGE_1G = 4,
    ACC_RANGE_MAX = ACC_RANGE_1G
} icm4x6xx_accel_fsr;

typedef enum icm4x6xx_gyro_fsr {
    GYRO_RANGE_2000DPS = 0,
    GYRO_RANGE_1000DPS = 1,
    GYRO_RANGE_500DPS = 2,
    GYRO_RANGE_250DPS = 3,
    GYRO_RANGE_125DPS = 4,
    GYRO_RANGE_62_5DPS = 5,
    GYRO_RANGE_31_25DPS = 6,
    GYRO_RANGE_15_625DPS = 7,
    GYRO_RANGE_MAX = GYRO_RANGE_15_625DPS
} icm4x6xx_gyro_fsr;

/* fifo work mode */
typedef enum icm4x6xx_fifo_mode {
    BYPASS,
    STREAM,
    SNAPSHOT,
    FIFO_MODE_MASK
} icm4x6xx_fifo_mode;

typedef enum icm4x6xx_spi_speed {
    SPI_5_MHZ = 3,
    SPI_17_MHZ = 5,
} icm4x6xx_spi_speed;

typedef enum icm4x6xx_ui_intf {
    AUTO_INTF,
    I2C_INTF = 2,
    SPI_INTF = 3,
} icm4x6xx_ui_intf;

typedef enum icm4x6xx_smd_mode {
    OFF_MODE,
    OLD_MODE,
    SHORT_1S,
    SHORT_3S
} icm4x6xx_smd_mode;

/* Filter order */
typedef enum icm4x6xx_filter_order {
    FIRST_ORDER,    // 1st order
    SEC_ORDER,      // 2nd order
    THIRD_ORDER,    // 3rd order
    ORDER_MASK        // N/A
} icm4x6xx_filter_order;

/* sensor bandwidth */
typedef enum icm4x6xx_bandwidth {
    BW_ODR_DIV_2 = 0,
    BW_ODR_DIV_4 = 1,
    BW_ODR_DIV_5 = 2,
    BW_ODR_DIV_8 = 3,
    BW_ODR_DIV_10 = 4,
    BW_ODR_DIV_16 = 5,
    BW_ODR_DIV_20 = 6,
    BW_ODR_DIV_40 = 7,
    BW_LL_MAX_200_8X_ODR = 15
} icm4x6xx_bandwidth;

/* fifo format*/
typedef enum icm4x6xx_fifo_format {
    ICM4X6XX_FORMAT_EMPTY,
    ICM4X6XX_FORMAT_ACCEL_8_BYTES,
    ICM4X6XX_FORMAT_GYRO_8_BYTES,
    ICM4X6XX_FORMAT_16_BYTES,
    ICM4X6XX_FORMAT_20_BYTES,
    ICM4X6XX_FORMAT_SPECIAL,
    ICM4X6XX_FORMAT_UNKNOWN
} icm4x6xx_fifo_format;

/* Interrupt Line Clear Option (latched mode) */
typedef enum {
    CLEAR_ON_STATUS_BIT_R_0,
    CLEAR_ON_STATUS_BIT_R_1,
    CLEAR_ON_FIFO_1BYTE_R,
    CLEAR_ON_BOTH,
} icm4x6xx_int_line_clr_op;

#ifdef ICM4X6XX_ENABLE_LOWG

#define BIT_APEX_CONFIG5_LOWG_TIME_TH_POS   0
#define BIT_APEX_CONFIG5_LOWG_TIME_TH_MASK (0x07<<BIT_APEX_CONFIG5_LOWG_TIME_TH_POS)

/* low */
typedef enum
{
  ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_20MS  = (0  << BIT_APEX_CONFIG5_LOWG_TIME_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_40MS  = (1  << BIT_APEX_CONFIG5_LOWG_TIME_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_60MS  = (2  << BIT_APEX_CONFIG5_LOWG_TIME_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_80MS  = (3  << BIT_APEX_CONFIG5_LOWG_TIME_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_100MS = (4  << BIT_APEX_CONFIG5_LOWG_TIME_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_120MS = (5  << BIT_APEX_CONFIG5_LOWG_TIME_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_140MS = (6  << BIT_APEX_CONFIG5_LOWG_TIME_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_160MS = (7  << BIT_APEX_CONFIG5_LOWG_TIME_TH_POS)
} ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_t;

#define BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS   3
#define BIT_APEX_CONFIG5_LOWG_PEAK_TH_MASK (0x1F<<BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS)

typedef enum
{
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_31MG   = (0x00 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_63MG   = (0x01 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_94MG   = (0x02 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_125MG  = (0x03 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_156MG  = (0x04 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_188MG  = (0x05 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_219MG  = (0x06 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_250MG  = (0x07 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_281MG  = (0x08 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_313MG  = (0x09 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_344MG  = (0x0A << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_375MG  = (0x0B << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_406MG  = (0x0C << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_438MG  = (0x0D << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_469MG  = (0x0E << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_500MG  = (0x0F << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_531MG  = (0x10 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_563MG  = (0x11 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_594MG  = (0x12 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_625MG  = (0x13 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_656MG  = (0x14 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_688MG  = (0x15 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_719MG  = (0x16 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_750MG  = (0x17 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_781MG  = (0x18 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_813MG  = (0x19 << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_844MG  = (0x1A << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_875MG  = (0x1B << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_906MG  = (0x1C << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_938MG  = (0x1D << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_969MG  = (0x1E << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_1000MG = (0x1F << BIT_APEX_CONFIG5_LOWG_PEAK_TH_POS)
} ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_t;

#define BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS   3
#define BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_MASK (0x07<<BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS)

typedef enum
{
  ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_31MG  = (0  << BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS),
  ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_63MG  = (1  << BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS),
  ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_94MG  = (2  << BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS),
  ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_125MG = (3  << BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS),
  ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_156MG = (4  << BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS),
  ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_188MG = (5  << BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS),
  ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_219MG = (6  << BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS),
  ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_250MG = (7  << BIT_APEX_CONFIG4_LOWG_PEAK_TH_HYST_POS)
} ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_t;
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
#define BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS   3
#define BIT_APEX_CONFIG6_HIGHG_PEAK_TH_MASK (0x1F<<BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS)

typedef enum
{
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2000MG = (0x00 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2031MG = (0x01 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2063MG = (0x02 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2094MG = (0x03 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2125MG = (0x04 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2156MG = (0x05 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2188MG = (0x06 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2219MG = (0x07 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2250MG = (0x08 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2281MG = (0x09 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2313MG = (0x0A << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2344MG = (0x0B << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2375MG = (0x0C << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2406MG = (0x0D << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2438MG = (0x0E << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2469MG = (0x0F << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2500MG = (0x10 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2531MG = (0x11 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2563MG = (0x12 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2594MG = (0x13 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2625MG = (0x14 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2656MG = (0x15 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2688MG = (0x16 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2719MG = (0x17 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2750MG = (0x18 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2781MG = (0x19 << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2813MG = (0x1A << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2844MG = (0x1B << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2875MG = (0x1C << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2906MG = (0x1D << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2938MG = (0x1E << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS),
  ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_2969MG = (0x1F << BIT_APEX_CONFIG6_HIGHG_PEAK_TH_POS)
} ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_t;

#define BIT_APEX_CONFIG6_HIGHG_TIME_TH_POS   0
#define BIT_APEX_CONFIG6_HIGHG_TIME_TH_MASK  0x07

typedef enum
{
  ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_20MS  = 0x0,
  ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_40MS  = 0x1,
  ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_60MS  = 0x2,
  ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_80MS  = 0x3,
  ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_100MS = 0x4,
  ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_120MS = 0x5,
  ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_140MS = 0x6,
  ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_160MS = 0x7
} ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_t;
#endif

typedef float icm4x6xx_gyro_sstvt;

typedef struct icm4x6xx_com_port_info {
    sns_com_port_config         com_config;
    sns_sync_com_port_handle    *port_handle;
    uint8_t                     i2c_address;
    uint8_t                     i3c_address;
} icm4x6xx_com_port_info;

/* Range attribute */
typedef struct range_attr {
  float min;
  float max;
} range_attr;

typedef enum
{
  ICM4X6XX_ACCEL         = 0x1,
  ICM4X6XX_GYRO          = 0x2,
  ICM4X6XX_MOTION_DETECT  = 0x4,
  ICM4X6XX_SENSOR_TEMP    = 0x8,
#ifdef ICM4X6XX_ENABLE_LOWG
  ICM4X6XX_FREE_FALL      = 0x10,
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
  ICM4X6XX_HIGH_SHOCK      = 0x20,
#endif
} icm4x6xx_sensor_type;

typedef union icm4x6xx_fifo_header_t {
    unsigned char head_byte;
    struct {
         unsigned char g_odr_change_bit : 1;
         unsigned char a_odr_change_bit : 1;
         unsigned char timestamp_bit : 2;
         unsigned char twentybits_bit : 1;
         unsigned char gyro_bit : 1;
         unsigned char accel_bit : 1;
         unsigned char msg_bit : 1;
     } bits;
} icm4x6xx_fifo_header_t;


typedef enum icm4x6xx_config_step {
  ICM4X6XX_CONFIG_IDLE,            /** not configuring */
  ICM4X6XX_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
  ICM4X6XX_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
  ICM4X6XX_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
  ICM4X6XX_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} icm4x6xx_config_step;

typedef enum {
    ICM4X6XX_TEST_NO_ERROR,
    ICM4X6XX_FAC_TEST_HIGH_BIAS,
    ICM4X6XX_FAC_TEST_DEV_NOT_STATIONARY,
    ICM4X6XX_FAC_TEST_ZERO_VARIANCE
} icm4x6xx_test_err_code;

/* chip series */
typedef enum chip_series {
    HAVANA,
    YOKOHAMA,
    UNKOWN
} icm4x6xx_product_series;

typedef struct icm4x6xx_product_info {
  uint8_t whoami;
  uint16_t fifo_size;
  bool i3c_supported;
  bool i3c_use_int1;
  icm4x6xx_product_series series;
  bool use_hi_res;
  bool hi_range;
} icm4x6xx_product_info;

typedef struct icm4x6xx_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool test_client_present;
} icm4x6xx_self_test_info;

/* icm4x6xx power mode */
typedef enum icm4x6xx_power_mode {
    ICM4X6XX_A_OFF,
    ICM4X6XX_A_OFF_1,
    ICM4X6XX_A_LPM,
    ICM4X6XX_A_LNM
} icm4x6xx_power_mode;

typedef enum
{
    ICM4X6XX_INT_ACTIVE_LOW,
    ICM4X6XX_INT_ACTIVE_HIGH
} icm4x6xx_int_polarity;

typedef struct icm4x6xx_cal_evt_info
{
  float bias[TRIAXIS_NUM];
  sns_time  timestamp;
} icm4x6xx_cal_evt_info;

typedef struct icm4x6xx_cfg_evt_info
{
  float     sample_rate;
  uint32_t  wmk;
  sns_time  timestamp;
#ifndef ICM4X6XX_DISABLE_DAE
  uint32_t  dae_watermark;
#endif
} icm4x6xx_cfg_evt_info;

typedef struct icm4x6xx_factory_test_info {
    int32_t num_samples;             /** number of samples acquired */
    float variance_threshold;        /** stationary detect variance threshold */
    float variance[TRIAXIS_NUM];          /** variance */
    float sample_square_sum[TRIAXIS_NUM]; /** sum of square of sample data */
    float sample_sum[TRIAXIS_NUM];        /** sum of sample data */
    float bias_thresholds[TRIAXIS_NUM];
    bool at_rest;
} icm4x6xx_factory_test_info;

/** HW FIFO information */
typedef struct icm4x6xx_fifo_info
{
  /** Determines which Sensor data to publish. Uses
   *  icm4x6xx_sensor_type as bit mask. */
  //uint8_t publish_sensors;

  /** record mode or not */
  bool record_mode;

  /** Direct Report Mode or not */
  bool is_drm;

  icm4x6xx_fifo_mode fifo_mode;
  /* fifo packet format */
  icm4x6xx_fifo_format curr_format;
  icm4x6xx_fifo_format desire_format;
  bool req_chg_ff_format;
  /* the number of flush HW for fifo format changing */
  uint8_t cnt_flush_hw;

  /** FIFO watermark levels for accel and gyro*/
  uint16_t cur_wmk;
  uint16_t desire_wmk;

  /** max requested FIFO watermark levels; possibly larger than max HW FIFO */
  uint32_t max_requested_wmk;
  uint32_t cur_max_wmk;

  /** max flush ticks*/
  uint64_t max_requested_flush_ticks;
  uint64_t cur_max_flush_ticks;

  bool flush_only;
  bool max_batch;
} icm4x6xx_fifo_info;

typedef struct icm4x6xx_accel_info
{
  float cur_odr;
  float req_match_odr;
  icm4x6xx_sensor_odr desire_odr_reg;
  icm4x6xx_accel_fsr fsr;
  icm4x6xx_bandwidth bw;
  sns_sensor_uid          suid;
  uint8_t                 num_samples_to_discard;
  bool                    gated_client_present;
  /* motion_detect or temperature will also
   * set this flag to true, but en_fifo will be false */
  bool desire_en;
  bool is_enabled;
  icm4x6xx_power_mode desire_mode;
  icm4x6xx_power_mode curr_mode;
  bool en_fifo;
  bool desire_publish;
  bool actual_publish;
  icm4x6xx_self_test_info  test_info;
  icm4x6xx_cfg_evt_info    last_sent_cfg;
  icm4x6xx_cal_evt_info    last_cal_info;
  /** Filter delay(tick) */
  sns_time filter_delay;
} icm4x6xx_accel_info;

typedef struct icm4x6xx_gyro_info
{
  /* if gyro is enabled, we will always set gyro data to fifo */
  bool desire_en;
  bool is_enabled;
  bool en_fifo;
  bool desire_publish;
  bool actual_publish;
  float        cur_odr;
  float        req_match_odr;
  icm4x6xx_sensor_odr desire_odr_reg;
  icm4x6xx_gyro_fsr           fsr;
  sns_sensor_uid          suid;
  uint8_t                 num_samples_to_discard;
  icm4x6xx_self_test_info  test_info;
  icm4x6xx_cfg_evt_info    last_sent_cfg;
  icm4x6xx_cal_evt_info    last_cal_info;
  /** Filter delay(tick) */
  sns_time filter_delay;
} icm4x6xx_gyro_info;

typedef struct icm4x6xx_motion_detect_info
{
  sns_sensor_uid          suid;
  bool                    enable_md_int;
  bool                    md_client_present;
  bool                    md_new_req;
  uint8_t ignore_md_fired_num;
  sns_motion_detect_event md_state;
  sns_motion_detect_event pre_md_state;
  icm4x6xx_self_test_info  test_info;
  sns_registry_md_cfg     md_config;
} icm4x6xx_motion_detect_info;

#ifdef ICM4X6XX_ENABLE_LOWG
typedef struct icm4x6xx_free_fall_info
{
  sns_sensor_uid          suid;
  bool                    enable_free_fall;
  bool                    free_fall_client_present;
  bool                    free_fall_new_req;

  sns_free_fall_event free_fall_state;
  icm4x6xx_self_test_info  test_info;
} icm4x6xx_free_fall_info;
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
typedef struct icm4x6xx_high_shock_info
{
  sns_sensor_uid          suid;
  bool                    enable_high_shock;
  bool                    high_shock_client_present;
  bool                    high_shock_new_req;

  sns_high_shock_event high_shock_state;
  icm4x6xx_self_test_info  test_info;
} icm4x6xx_high_shock_info;
#endif

typedef struct icm4x6xx_heart_beat
{
  uint32_t heart_attack_cnt;/** Count how many heart attack occurs */
  sns_time last_sample_ts;/** The last sample timestamp sent out from driver */
  sns_time start_time;/** Timestamp in tick at which heart beat timer should start */
  sns_time timeout_period;/** Unit: tick */
  sns_data_stream *timer_data_stream;
} icm4x6xx_heart_beat;
typedef struct icm4x6xx_sensor_temp_info
{
  sns_sensor_uid          suid;
  bool desire_en;
  bool is_enabled;
  bool desire_publish;
  bool actual_publish;
  bool                    timer_is_active;
  float                   report_rate_hz;
  float                   sampling_rate_hz;
  float                   cur_sampling_rate_hz;
  sns_time                sampling_intvl;
  icm4x6xx_self_test_info  test_info;
  uint64_t                max_requested_flush_ticks;
#ifdef  ICM4X6XX_TEMPERATURE_ONE_UNRELIABLE_SAMPLE
  uint8_t  discard_samples;
#endif
  icm4x6xx_cfg_evt_info last_sent_cfg;
} icm4x6xx_sensor_temp_info;

typedef struct icm4x6xx_irq_info
{
  sns_interrupt_req       irq_config;
  bool                    irq_ready;
  bool                    irq_registered;
} icm4x6xx_irq_info;

typedef struct icm4x6xx_async_com_port_info
{
  uint32_t                port_handle;
}icm4x6xx_async_com_port_info;

typedef struct sns_icm4x6xx_registry_cfg
{
  icm4x6xx_sensor_type sensor_type;
  matrix3             fac_cal_corr_mat;
  float               fac_cal_bias[3];
  uint32_t            version;
}sns_icm4x6xx_registry_cfg;

typedef struct icm4x6xx_st_resume {
    bool desire_en_a;
    bool desire_en_g;
    bool desire_en_a_fifo;
    bool desire_en_g_fifo;
    float req_match_odr;
    icm4x6xx_sensor_odr desire_odr_reg;
    uint16_t watermark;
    uint32_t max_requested_wmk;
    uint64_t max_requested_flush_ticks;
    icm4x6xx_fifo_format desire_format;
    bool enable_md_int;
    uint8_t a_samples_to_discard;
    uint8_t g_samples_to_discard;
} icm4x6xx_st_resume;

/** Private state. */
typedef struct icm4x6xx_instance_state
{
  /* Flush reason */
  bool odr_changed;
  bool a_off_to_on;
  bool g_off_to_on;

  //set is_first_sample or not
  bool reset_first_sample;

#ifdef  ICM4X6XX_1K_2_500HZ
  bool do_1K_2_500_dec;
#endif

  bool i3c_use_int1;

  uint16_t pre_rec_cnt_16b;

  bool a_new_stream;
  bool g_new_stream;

  icm4x6xx_product_info product_info;

  /** fifo details*/
  icm4x6xx_fifo_info       fifo_info;

  /** accel HW config details*/
  icm4x6xx_accel_info      accel_info;

  /** gyro HW config details*/
  icm4x6xx_gyro_info       gyro_info;

  /** motion detect info */
  icm4x6xx_motion_detect_info md_info;

#ifdef ICM4X6XX_ENABLE_LOWG
  /** free fall info */
  icm4x6xx_free_fall_info freefall_info;
#endif

#ifdef ICM4X6XX_ENABLE_HIGHG
  /** high shock info */
  icm4x6xx_high_shock_info high_shock_info;
#endif

  icm4x6xx_heart_beat heart_beat;
  /** Sensor Temperature config details. */
  icm4x6xx_sensor_temp_info sensor_temp_info;

  /** Interrupt dependency info. */
  icm4x6xx_irq_info        irq_info;

  icm4x6xx_st_resume st_resume;   //self test

  /** COM port info */
  icm4x6xx_com_port_info   com_port_info;

  /**--------Async Com Port--------*/
  sns_async_com_port_config  ascp_config;

#ifndef ICM4X6XX_DISABLE_DAE
  /**--------DAE interface---------*/
  icm4x6xx_dae_if_info       dae_if;
#endif

  icm4x6xx_config_step       config_step;

  sns_time interrupt_timestamp;
  sns_time flush_ts;
  sns_time sampling_intvl;
  sns_time pre_record_ts;
  sns_time pre_publish_ts;
  sns_time pre_meas_intvl;
  sns_time strobe_master_cnt;
  sns_time r_fifo_cnt_master_cnt;
  bool odr_just_changed;
  uint16_t lpf_parameter;
  uint32_t slave_cnt;
  bool is_first_sample;
  uint32_t before_re_hw_intnl_cnt;
  uint32_t after_re_hw_intnl_cnt;
  bool is_flush;

//#ifdef ICM4X6XX_DEBUG_VERBOSE
  uint32_t a_reported_sample;
  uint32_t g_reported_sample;
//#endif

  /** Data streams from dependentcies. */
  sns_sensor_uid       timer_suid;
  sns_data_stream      *interrupt_data_stream;
  sns_data_stream      *timer_data_stream;
  sns_data_stream      *timer_en_md_stream;
  sns_data_stream      *async_com_port_data_stream;

  /**----------Axis Conversion----------*/
  triaxis_conversion axis_map[TRIAXIS_NUM];

  /**----------Sensor specific registry configuration----------*/
  sns_icm4x6xx_registry_cfg accel_registry_cfg;
  sns_icm4x6xx_registry_cfg gyro_registry_cfg;
  sns_icm4x6xx_registry_cfg sensor_temp_registry_cfg;

  sns_diag_service *diag_service;
  sns_sync_com_port_service * scp_service;
  sns_island_service *island_service;

#ifndef ICM4X6XX_DISABLE_DAE
  bool flushing_fifo;
  sns_dae_timestamp_type dae_ts_type;
#endif
  bool instance_is_ready_to_configure;
  bool fifo_flush_in_progress;
  bool new_self_test_request;
  bool fac_test_in_progress;
  icm4x6xx_factory_test_info fac_test_info;
  icm4x6xx_sensor_type fac_test_sensor;

  uint8_t ascp_request_cnt;

  bool update_fac_cal_in_registry;

  size_t log_interrupt_encoded_size;
  size_t log_raw_encoded_size;
  size_t log_temp_raw_encoded_size;
  size_t encoded_imu_event_len;
  size_t encoded_sensor_temp_event_len;

#ifdef ICM4X6XX_ENABLE_DUAL_SENSOR
  uint8_t instance_idx;
#endif
} icm4x6xx_instance_state;

typedef struct icm4x6xx_filter_delay_map
{
  enum icm4x6xx_sensor_odr  odr_reg_value;
  float                     filter_delay;
} icm4x6xx_filter_delay_map;

typedef struct icm4x6xx_odr_reg_map
{
  enum icm4x6xx_sensor_odr odr_reg_value;
  float              odr;
  uint8_t            a_discard_samples;
  uint8_t            g_discard_samples;
} icm4x6xx_odr_reg_map;

typedef struct sns_icm4x6xx_req
{
  float desired_sample_rate;
  float desired_report_rate;
  float desired_wakeup_sdc_rate;
  uint64_t  desired_flush_ticks;
  sns_icm4x6xx_registry_cfg  registry_cfg;
} sns_icm4x6xx_req;

sns_rc icm4x6xx_get_best_match_odr(
       icm4x6xx_instance_state *state,
       float desired_sample_rate,
       float *chosen_sample_rate,
       icm4x6xx_sensor_odr *chosen_reg_value,
       uint8_t *a_samples_to_discard,
       uint8_t *g_samples_to_discard);

sns_rc icm4x6xx_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);

sns_rc icm4x6xx_inst_deinit(sns_sensor_instance *const this);

/**
 * Executes requested self-tests.
 *
 * @param instance   reference to the instace
 *
 * @return none
 */
void icm4x6xx_run_self_test(sns_sensor_instance *instance);
/**
 * Processes a fifo buffer and extracts accel and gyro samples from the buffer
 * and generates events for each sample.
 *
 * @param[i] instance     Sensor instance
 * @param[i] gyro_enabled Whether gyro is enabled
 * @param[i] first_ts     Timestamp of first sample in fifo
 * @param[i] interval     Sampling interval in time ticks
 * @param[i] fifo         Buffer containing samples read from HW FIFO
 * @param[i] num_bytes    Number of bytes in fifo buffer
 */
void icm4x6xx_process_fifo_data_buffer(
                    sns_sensor_instance *instance,
                    const uint8_t       *buf,
                    size_t              buf_len,
                    sns_time            timestamp);
void icm4x6xx_register_interrupt(sns_sensor_instance *this);
/**
 * Handles MD interrupt:
 *   1. Sends MD fired event.
 *   2. Starts Motion Accel Stream.
 *
 * @param[i] instance        Instance reference
 * @param[i] irq_timestamp   MD interrupt timestamp
 *
 * @return none
 */
void icm4x6xx_handle_md_interrupt(
    sns_sensor_instance *const instance,
    sns_time irq_timestamp);
#ifdef ICM4X6XX_ENABLE_LOWG
void icm4x6xx_handle_freefall_interrupt(
     sns_sensor_instance *const instance,
     sns_time irq_timestamp);
#endif
#ifdef ICM4X6XX_ENABLE_HIGHG
void icm4x6xx_handle_high_shock_interrupt(
     sns_sensor_instance *const instance,
     sns_time irq_timestamp);
#endif
void icm4x6xx_handle_interrupt_event(sns_sensor_instance *const instance, bool flush);
/**
 * Notification message to the interrupt Sensor that a level triggered interrupt
 * has been serviced and cleared so that it can be re-enabled.
 */
void icm4x6xx_send_int_clear_msg(sns_sensor_instance *const this);
void icm4x6xx_convert_and_send_temp_sample(
                        sns_sensor_instance *const instance,
                        sns_time timestamp,
                        int16_t temp_data);
void icm4x6xx_set_polling_config(sns_sensor_instance *const this);
void icm4x6xx_reconfig_hw(sns_sensor_instance *this);
void icm4x6xx_send_fifo_flush_done(
            sns_sensor_instance *const instance,
            sns_sensor_uid *suid);
void icm4x6xx_send_config_event(sns_sensor_instance *const instance);
/**
 * Processes results for factory test for accel and gyro.
 *
 * @param[i] instance   reference to instance
 */
void icm4x6xx_process_fac_test(sns_sensor_instance *instance);
void icm4x6xx_send_cal_event(
     sns_sensor_instance *const instance, icm4x6xx_sensor_type sensor_type);
void icm4x6xx_reset_cal_data(
     sns_sensor_instance *const instance, icm4x6xx_sensor_type sensor_type);
void icm4x6xx_st_store_pre_state(sns_sensor_instance* instance);
void icm4x6xx_st_recover_pre_state(sns_sensor_instance* instance);
bool icm4x6xx_need_to_reset_fifo(icm4x6xx_instance_state *state);
int icm4x6xx_flush_fifo(sns_sensor_instance *this);
void icm4x6xx_set_hw_wo_stop_fifo(sns_sensor_instance *this);
