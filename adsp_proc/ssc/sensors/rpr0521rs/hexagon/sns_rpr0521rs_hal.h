//==============================================================================
// MODULE        : sns_rpr0521rs_hal.h
// FUNCTION      : Hardware Access Layer functions
// AUTHOR        : Masafumi Seike
// MAKING DATE   : Nov/16/2017
// MODIFIED      : Ryosuke Yamamoto
// MODIFIED DATE : Dec/4/2018
// REMARKS       :
// COPYRIGHT     : Copyright (C) 2017 - 2018 - ROHM CO.,LTD.
//               : Redistribution and use in source and binary forms, with or
//               : without modification, are permitted provided that the
//               : following conditions are met:
//               : 1. Redistributions of source code must retain the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer.
//               : 2. Redistributions in binary form must reproduce the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer in the documentation and/or other materials
//               : provided with the distribution.
//               : 3. Neither the name of ROHM CO.,LTD. nor the names of its
//               : contributors may be used to endorse or promote products
//               : derived from this software without specific prior written
//               : permission.
//               : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//               : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//               : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//               : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//               : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//               : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//               : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//               : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//               : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//               : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//               : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//               : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//               : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//               : SUCH DAMAGE.
//==============================================================================
#ifndef SNS_RPR0521RS_HAL_H_
#define SNS_RPR0521RS_HAL_H_ (1)

// Setting parameter of raw odr to register setting
typedef enum {
    RPR0521RS_ODR_ALS_0_PS_0 = 0x00,
    RPR0521RS_ODR_ALS_0_PS_10,
    RPR0521RS_ODR_ALS_0_PS_40,
    RPR0521RS_ODR_ALS_0_PS_100,
    RPR0521RS_ODR_ALS_0_PS_400,
    RPR0521RS_ODR_ALS_100_PS_50,
    RPR0521RS_ODR_ALS_100_PS_100,
    RPR0521RS_ODR_ALS_100_PS_400,
    RPR0521RS_ODR_ALS_400_PS_50,
    RPR0521RS_ODR_ALS_400_PS_100,
    RPR0521RS_RESERVED1,
    RPR0521RS_RESERVED2,
    RPR0521RS_ODR_ALS_50_PS_50,
    RPR0521RS_NON_SET,
} RPR0521RS_SET_ODR;

typedef struct {
    uint16_t high;
    uint16_t low;
} THRESHOLD;

typedef struct {
    uint16_t data0;
    uint16_t data1;
} ALS_DATA;

typedef enum {
    RPR0521RS_ALS_REG_GAIN_1X    = 0,  //   1x AGAIN */ 
    RPR0521RS_ALS_REG_GAIN_2X,         //   2x AGAIN */
    RPR0521RS_ALS_REG_GAIN_64X,        //  64x AGAIN */
    RPR0521RS_ALS_REG_GAIN_128X,       // 128x AGAIN */
    RPR0521RS_ALS_REG_GAIN_INVALID
} RPR0521RS_ALS_REG_GAIN;

// RPR0521 Register Addresses define
#define RPR0521RS_REG_SYSTEMCTL  (0x40U)
#define RPR0521RS_REG_MODECTL    (0x41U)
#define RPR0521RS_REG_ALSPSCTL   (0x42U)
#define RPR0521RS_REG_PSCTL      (0x43U)
#define RPR0521RS_REG_ALS_DATA   (0x46U)
#define RPR0521RS_REG_PS_DATA    (0x44U)
#define RPR0521RS_REG_INTERRUPT  (0x4AU)
#define RPR0521RS_REG_PS_TH_L    (0x4BU)
#define RPR0521RS_REG_PS_TH_M    (0x4CU)
#define RPR0521RS_REG_PS_TL_L    (0x4DU)
#define RPR0521RS_REG_PS_TL_M    (0x4EU)
#define RPR0521RS_REG_ALS_TH_L   (0x4FU)
#define RPR0521RS_REG_ALS_TH_M   (0x50U)
#define RPR0521RS_REG_ALS_TL_L   (0x51U)
#define RPR0521RS_REG_ALS_TL_M   (0x52U)
#define RPR0521RS_REG_ID         (0x92U)

// RPR0521 Register Value define
#define RPR0521RS_SW_RESET          (1U << 7)
#define RPR0521RS_INT_RESET         (1U << 6)

#define RPR0521RS_ALS_EN            (1U << 7)
#define RPR0521RS_PS_EN             (1U << 6)
#define RPR0521RS_PS_PULSE_200US    (0U << 5)
#define RPR0521RS_PS_OP_NORMAL_MODE (0U << 4)

#define RPR0521RS_ALS_DATA0_GAIN_1X (0U << 4)
#define RPR0521RS_ALS_DATA1_GAIN_1X (0U << 2)
#define RPR0521RS_LEDCURRENT_100MA  (2U << 0)

#define RPR0521RS_PS_GAIN_1X        (0U << 4)
#define RPR0521RS_PS_PERSISTENCE    (1U << 0)

#define RPR0521RS_INT_TRIG_PS       (1U << 0)
#define RPR0521RS_INT_TRIG_ALS      (1U << 1)
#define RPR0521RS_INT_LATCH_KEEP    (0U << 2)
#define RPR0521RS_INT_ASSERT_LOW    (0U << 3)
#define RPR0521RS_INT_MODE_OOUTSIDE (2U << 4)

#define RPR0521RS_PS_INT_OCCUR      (1U << 7)
#define RPR0521RS_ALS_INT_OCCUR     (1U << 6)

// Fixed initial parameters
#define RPR0521RS_INIT_MODECTL      (RPR0521RS_PS_PULSE_200US | RPR0521RS_PS_OP_NORMAL_MODE | RPR0521RS_ODR_ALS_100_PS_100)
#define RPR0521RS_INIT_ALSPSCTL     (RPR0521RS_LEDCURRENT_100MA)
#define RPR0521RS_INIT_PSCTL        (RPR0521RS_PS_GAIN_1X | RPR0521RS_PS_PERSISTENCE)
#define RPR0521RS_INIT_INTRRUPT     (RPR0521RS_INT_LATCH_KEEP | RPR0521RS_INT_ASSERT_LOW |RPR0521RS_INT_MODE_OOUTSIDE)
#define RPR0521RS_INIT_ALS_THH      (0x0000U)
#define RPR0521RS_INIT_ALS_THL      (0xFFFFU)
#define RPR0521RS_INIT_PS_THH       (0x0000U)
#define RPR0521RS_INIT_PS_THL       (0x0FFFU)

#define RPR0521RS_WHOAMI_VALUE      (0x0AU)
#define RPR0521RS_PARTID_MASK       (0x3FU)
#define RPR0521RS_POWER_CLR         (0x3FU)
#define RPR0521RS_MEASER_ODR_CLR    (0xF0U)

#define RPR0521RS_PS_FAR            (0U)
#define RPR0521RS_PS_NEAR           (1U)
#define RPR0521RS_PS_KEEP           (2U)

#define RPR0521RS_PS_ODR_MS_50      ( 50U)
#define RPR0521RS_PS_ODR_MS_100     (100U)

#define RPR0521RS_ALS_ODR_MS_100    (100U)
#define RPR0521RS_ALS_ODR_MS_400    (400U)

#define RPR0521RS_ALS_TH_PERCENT    (10U)

#define RPR0521RS_HBT_POLLING       (5U)
#define RPR0521RS_HBT_DRI           (20U)
#define RPR0521RS_HBT_FIRED_COUNT   (3U) // less than 255

#define RPR0521RS_ALS_MAX_VALUE     (0xFFFFU)
#define RPR0521RS_PS_MAX_VALUE      (0x0FFFU)
#define RPR0521RS_INT_TRG_CLR       (0xFCU)
#define RPR0521RS_AMB_IR_MASK       (0xC0U)

#endif // SNS_RPR0521RS_HAL_H_

