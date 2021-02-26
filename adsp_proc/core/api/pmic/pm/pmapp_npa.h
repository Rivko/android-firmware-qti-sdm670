#ifndef __PMAPP_NPA_H__
#define __PMAPP_NPA_H__
/*===========================================================================


                  P M I C    N P A    H E A D E R    F I L E

DESCRIPTION
  This file contains prototype definitions to support interaction
  with the QUALCOMM Power Management ICs using NPA framework.

Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/api/pmic/pm/pmapp_npa.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/17   akm     New File
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        DEFINITIONS

===========================================================================*/

/*
 * Generic mode IDs that can be used for clients that only use ON / OFF or
 * ACTIVE / STANDBY states.
 */
#define PMIC_NPA_GROUP_ID_AUDIO             "/pmic/client/audio"
#define PMIC_NPA_GROUP_ID_AUDIO_RX          "/pmic/client/audio_rx"
#define PMIC_NPA_GROUP_ID_AUDIO_SPEAKER     "/pmic/client/audio_speaker"
#define PMIC_NPA_GROUP_ID_AUDIO2            "/pmic/client/audio2"
#define PMIC_NPA_GROUP_ID_CODEC             "/pmic/client/codec"
#define PMIC_NPA_GROUP_ID_CODEC_ANC         "/pmic/client/codec_anc"
#define PMIC_NPA_GROUP_ID_DIG_MIC           "/pmic/client/dig_mic"
#define PMIC_NPA_GROUP_ID_SENSOR_VDD        "/pmic/client/sensor_vdd"
#define PMIC_NPA_GROUP_ID_SENSOR_VDD_2      "/pmic/client/sensor_vdd_2"
#define PMIC_NPA_GROUP_ID_SENSOR_VDDIO      "/pmic/client/sensor_vddio"
#define PMIC_NPA_GROUP_ID_SENSOR_VDDIO_2    "/pmic/client/sensor_vddio_2"
#define PMIC_NPA_GROUP_ID_SLIMBUS           "/pmic/client/slimbus"
#define PMIC_NPA_GROUP_ID_TSCREEN           "/pmic/client/tscreen"
#define PMIC_NPA_GROUP_ID_TSCREEN1          "/pmic/client/tscreen1"
#define PMIC_NPA_GROUP_ID_TSCREEN2          "/pmic/client/tscreen2"
#define PMIC_NPA_GROUP_ID_TSCREEN3          "/pmic/client/tscreen3"
#define PMIC_NPA_GROUP_ID_XO                "/pmic/client/xo"
#define PMIC_NPA_GROUP_ID_RAIL_CX           "/pmic/client/rail_cx"
#define PMIC_NPA_GROUP_ID_PM_INIT           "/pmic/client/pm_init"

/*===========================================================================

                        ENUMERATION

===========================================================================*/
/**
Vote                                  PMIC XO  PMIC XO Buffer  SOC CXO PAD
PMIC_NPA_MODE_ID_CLK_CXO_XO_OFF       OFF      OFF             OFF
PMIC_NPA_MODE_ID_CLK_CXO_BUFFER_OFF    ON      OFF             OFF
PMIC_NPA_MODE_ID_CLK_SOC_CXO_PAD_OFF   ON       ON             OFF
PMIC_NPA_MODE_ID_CLK_CXO_XO_ON         ON       ON              ON
*/
enum
{
   PMIC_NPA_MODE_ID_CLK_CXO_XO_OFF      = 0,
   PMIC_NPA_MODE_ID_CLK_CXO_BUFFER_OFF  = 1,
   PMIC_NPA_MODE_ID_CLK_SOC_CXO_PAD_OFF = 2,
   PMIC_NPA_MODE_ID_CLK_CXO_XO_ON       = 3,
   PMIC_NPA_MODE_ID_XO_MAX              = 4,
};

enum
{
   PMIC_NPA_MODE_ID_AUDIO_STANDBY     = 0,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_LOW  = 1,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_MED  = 2,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_HIGH = 3,
   PMIC_NPA_MODE_ID_AUDIO_MAX         = 4,
};

enum
{
   PMIC_NPA_MODE_ID_CODEC_PATH_OFF = 0,
   PMIC_NPA_MODE_ID_CODEC_1PATH_ON = 1,
   PMIC_NPA_MODE_ID_CODEC_2PATH_ON = 2,
   PMIC_NPA_MODE_ID_CODEC_3PATH_ON = 3,
   PMIC_NPA_MODE_ID_CODEC_4PATH_ON = 4,
   PMIC_NPA_MODE_ID_CODEC_MAX      = 5,
};

enum
{
    PMIC_NPA_MODE_ID_GENERIC_OFF = 0,
    PMIC_NPA_MODE_ID_GENERIC_STANDBY = 1,
    PMIC_NPA_MODE_ID_GENERIC_ACTIVE = 2,
    PMIC_NPA_MODE_ID_GENERIC_LV = PMIC_NPA_MODE_ID_GENERIC_ACTIVE,
    PMIC_NPA_MODE_ID_GENERIC_MV = 3,
    PMIC_NPA_MODE_ID_GENERIC_HV = 4,
    PMIC_NPA_MODE_ID_GENERIC_MAX,
};

enum
{
   PMIC_NPA_MODE_ID_SENSOR_POWER_OFF = 0,
   PMIC_NPA_MODE_ID_SENSOR_LPM       = 1,
   PMIC_NPA_MODE_ID_SENSOR_POWER_ON  = 2,
   PMIC_NPA_MODE_ID_SENSOR_MAX       = 3,
};

enum
{
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_0   = 0,
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_1   = 1,
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_2   = 2,
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_3   = 3,
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_MAX = 4,
};

enum
{
    PMIC_NPA_MODE_ID_CORE_RAIL_LOW_MINUS,
};

#endif // PMAPP_NPA_H

