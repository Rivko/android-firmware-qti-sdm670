#ifndef VOL_CTRL_PARAM_CALIB_H
#define VOL_CTRL_PARAM_CALIB_H
/*==============================================================================
  @file vol_ctrl_param_calib.h
  @brief This file contains parameters of VOL_CTRL and VOL_CTRL_INSTANCE_2 API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/21/17   pag     Added h2xml comments.
 ========================================================================== */

/** @h2xml_title1           {Parameter of Volume Control}
    @h2xml_title_agile_rev  {Parameter of Volume Control}
    @h2xml_title_date       {February 9, 2017} */

#include  "mmdefs.h"


/*==============================================================================
   Constants
==============================================================================*/


/* ID of the Master Gain parameter used by AUDPROC_MODULE_ID_VOL_CTRL. */
#define AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN                       0x00010BFF

/* Structure for the master gain parameter for a volume control module. */
typedef struct audproc_volume_ctrl_master_gain_t audproc_volume_ctrl_master_gain_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN", AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN}
    @h2xmlp_description {Specifies the linear master gain}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN parameter used
 by the Volume Control module.
 */
struct audproc_volume_ctrl_master_gain_t
{
   uint16_t master_gain;
   /**< @h2xmle_description  { Specifies linear master gain.\n}
        @h2xmle_dataFormat    {Q13}   */

   uint16_t reserved;
   /**< @h2xmle_description  { Clients must set this field to 0.\n}
        @h2xmle_default       (0)
        @h2xmle_rangeList     {"0" = 0} */
}
#include "adsp_end_pack.h"
;

/** ID of the mute Configuration parameter used by AUDPROC_MODULE_ID_VOL_CTRL. */
#define AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG                       0x00010C01

/* Structure for the mute configuration parameter for a
 volume control module. */
typedef struct audproc_volume_ctrl_mute_config_t audproc_volume_ctrl_mute_config_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG", AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG}
    @h2xmlp_description {Configures the volume control module}
    @h2xmlp_toolPolicy  {NO_SUPPORT}*/


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG parameter used
 by the Volume Control module.
 */
struct audproc_volume_ctrl_mute_config_t
{
   uint32_t mute_flag;
    /**< @h2xmle_description  { Specifies whether mute is enabled. \n}
         @h2xmle_rangeList   {"Disable"= 0;
                              "Enable"=1}  */
}
#include "adsp_end_pack.h"
;

/* ID of the Soft Stepping Volume parameters used by AUDPROC_MODULE_ID_VOL_CTRL. */
#define AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS               0x00010C29

/** Supported parameters for a soft stepping linear ramping curve. */
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR                               0

/** Exponential ramping curve. */
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP                                  1

/** Logarithmic ramping curve.  */
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG                                  2

/** Fractional exponent ramping curve. @newpage */
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_FRAC_EXP                             3

/* Structure for holding soft stepping volume parameters. */
typedef struct audproc_soft_step_volume_params_t audproc_soft_step_volume_params_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS", AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS}
    @h2xmlp_description {Specifies Soft Stepping Volume parameters.\n
	Supported parameters for a soft stepping linear ramping curve\n
	AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR - 0 \n\n
	Exponential ramping curve\n
	AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP    - 1 \n\n
	Logarithmic ramping curve\n
	AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG    - 2 \n\n
	Fractional exponent ramping curve\n
	AUDPROC_PARAM_SVC_RAMPINGCURVE_FRAC_EXP - 3 \n\n } */

#include "adsp_begin_pack.h"

struct audproc_soft_step_volume_params_t
{
   uint32_t period;
   /**< @h2xmle_description  { Specifies period in milliseconds.\n }
         @h2xmle_range   {0..15000}
	 @h2xmle_default {40} */

   uint32_t step;
    /**< @h2xmle_description  { Specifies step in microseconds.\n}
         @h2xmle_range   {0..15000000}
	 @h2xmle_default {1000} */

   uint32_t ramping_curve;
   /**< @h2xmle_description  { Specifies ramping curve type.\n
   Supported Values:\n\n
   Supported parameters for a soft stepping linear ramping curve - AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR\n \n
   Exponential ramping curve - AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP \n\n
   Logarithmic ramping curve -  AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG \n \n
   Fractional exponent ramping curve - AUDPROC_PARAM_SVC_RAMPINGCURVE_FRAC_EXP \n  }

   @h2xmle_rangeList   {"AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR" = 0;
                              "AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP" = 1;
                              "AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG" = 2;
                              "AUDPROC_PARAM_SVC_RAMPINGCURVE_FRAC_EXP" = 3}
   @h2xmle_default  {1}   */

}
#include "adsp_end_pack.h"
;


/* ID of the Soft Pause parameters used by #AUDPROC_MODULE_ID_VOL_CTRL. */
#define AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS                      0x00010D6A

/* Structure for holding soft pause parameters. */

typedef struct audproc_soft_pause_params_t audproc_soft_pause_params_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS", AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS}
    @h2xmlp_description {Specifies soft pause parameters.\n}
	@h2xmlp_toolPolicy   {NO_SUPPORT} */

#include "adsp_begin_pack.h"

struct audproc_soft_pause_params_t
{
	uint32_t enable_flag;
	/**< @h2xmle_description  { Specifies whether soft pause is enabled.\n }
         @h2xmle_rangeList   {"Disable"= 0;
                              "Enable"=1}  */


     uint32_t period;
	 /**< @h2xmle_description  { Specifies period in milliseconds.\n }
         @h2xmle_range   {0..15000}  */


     uint32_t step;
	 /**< @h2xmle_description  { Specifies step in milliseconds.\n }
         @h2xmle_range   {0..15000}  */


     uint32_t ramping_curve;
	 /**< @h2xmle_description  { Specifies ramping curve.\n }

	 @h2xmle_rangeList   {"AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR" = 0;
                              "AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP" = 1;
                              "AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG" = 2;
                              "AUDPROC_PARAM_SVC_RAMPINGCURVE_FRAC_EXP" = 3}
	 @h2xmle_default  {1}   */

  }

  #include "adsp_end_pack.h"
 ;


/* ID of the Multichannel Volume Control parameters used by #AUDPROC_MODULE_ID_VOL_CTRL. */

#define AUDPROC_PARAM_ID_MULTICHANNEL_GAIN                          0x00010713

#define VOLUME_CONTROL_MAX_CHANNELS                       8

/* Structure for holding one channel type - gain pair. */
typedef struct audproc_volume_ctrl_channel_type_gain_pair_t audproc_volume_ctrl_channel_type_gain_pair_t;


#include "adsp_begin_pack.h"

struct audproc_volume_ctrl_channel_type_gain_pair_t
{
   uint8_t channel_type;
   /**< @h2xmle_description  { Channel type for which the gain setting is to be applied.\n }  */


   uint8_t reserved1;
   /**< @h2xmle_description  {Clients must set this field to 0.\n }  */


   uint8_t reserved2;
   /**< @h2xmle_description  {Clients must set this field to 0.\n }  */

   uint8_t reserved3;
   /**< @h2xmle_description  {Clients must set this field to 0.\n }  */

   uint32_t gain;
   /**< @h2xmle_description  {Gain value for this channel.\n }
         @h2xmle_dataFormat {Q28} */

}
#include "adsp_end_pack.h"
;

/* Structure for the multichannel gain command */
typedef struct audproc_volume_ctrl_multichannel_gain_t audproc_volume_ctrl_multichannel_gain_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MULTICHANNEL_GAIN",AUDPROC_PARAM_ID_MULTICHANNEL_GAIN}
    @h2xmlp_description { VOLUME_CONTROL_MAX_CHANNELS - 8. \n Payload of the AUDPROC_PARAM_ID_MULTICHANNEL_GAIN channel type/gain
    pairs used by the Volume Control module.\n
    This structure immediately follows the
    audproc_volume_ctrl_multichannel_gain_t structure.}
     @h2xmlp_toolPolicy   {NO_SUPPORT}  */

#include "adsp_begin_pack.h"

struct audproc_volume_ctrl_multichannel_gain_t
{
   uint32_t num_channels;
   /**< @h2xmle_description  {Number of channels for which gain values are provided. Any channels
        present in the data for which gain is not provided are set to unity
        gain.\n }

		@h2xmle_range   {1..8}
        @h2xmle_default  {1}		*/



   audproc_volume_ctrl_channel_type_gain_pair_t gain_data[VOLUME_CONTROL_MAX_CHANNELS];
   /**< @h2xmle_description  {Array of channel type/gain pairs.\n
		Supported values :- see the payload of the structure.\n} */


}
#include "adsp_end_pack.h"
;

/* ID of the Multichannel Mute Configuration parameters used by #AUDPROC_MODULE_ID_VOL_CTRL. */
#define AUDPROC_PARAM_ID_MULTICHANNEL_MUTE                          0x00010714

/* Structure for holding one channel type - mute pair. */
typedef struct audproc_volume_ctrl_channel_type_mute_pair_t audproc_volume_ctrl_channel_type_mute_pair_t;



#include "adsp_begin_pack.h"

struct audproc_volume_ctrl_channel_type_mute_pair_t
{
   uint8_t channel_type;
   /**< @h2xmle_description  { Channel type for which the gain setting is to be applied.\n }  */

   uint8_t reserved1;
   /**< @h2xmle_description  {Clients must set this field to 0.\n }  */

   uint8_t reserved2;
   /**< @h2xmle_description  {Clients must set this field to 0.\n }  */

   uint8_t reserved3;
   /**< @h2xmle_description  {Clients must set this field to 0.\n }  */

   uint32_t mute;
   /**< @h2xmle_description  { Specifies whether this channel is set to mute. \n}
         @h2xmle_rangeList   {"Disable"= 0;
                              "Enable"=1}  */

}
#include "adsp_end_pack.h"
;


/* Structure for the multichannel mute command */
typedef struct audproc_volume_ctrl_multichannel_mute_t audproc_volume_ctrl_multichannel_mute_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MULTICHANNEL_MUTE", AUDPROC_PARAM_ID_MULTICHANNEL_MUTE}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_MULTICHANNEL_MUTE channel type/mute
 setting pairs used by the Volume Control module.\n
 This structure immediately follows the audproc_volume_ctrl_multichannel_mute_t structure.}
     @h2xmlp_toolPolicy   {NO_SUPPORT}  */

#include "adsp_begin_pack.h"


struct audproc_volume_ctrl_multichannel_mute_t
{
   uint32_t num_channels;
   /**< @h2xmle_description  {Number of channels for which mute configuration is provided. Any
        channels present in the data for which mute configuration is not
        provided are set to unmute.\n }

		@h2xmle_range   {1..8}
        @h2xmle_default  {1}		*/



   audproc_volume_ctrl_channel_type_mute_pair_t mute_data[VOLUME_CONTROL_MAX_CHANNELS];
   /**< @h2xmle_description  {Array of channel type/mute setting pairs.\n
		Supported values :- see the payload of the structure.\n} */

}
#include "adsp_end_pack.h"
;

#endif




