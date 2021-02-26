#ifndef DS1_CALIB_H
#define DS1_CALIB_H

/*
This file contains the structure definitions for the
commands related with Dolby Surround1 Audio Processing.
*/

/*===========================================================================
  Copyright (c) 2013,2016 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*========================================================================
Edit History
when       who     what, where, why
--------   ---     -------------------------------------------------------
01/25/13   bvodapal     API file for DS1AP
08/17/16   hbansal      Moved file to different location.
========================================================================== */

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @h2xml_title1           {AUDPROC_MODULE_ID_DS1AP}
    @h2xml_title_agile_rev  {AUDPROC_MODULE_ID_DS1AP}
    @h2xml_title_date       {March 17, 2017} */


#define AUDPROC_MODULE_ID_DS1AP                       0x00010775

/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_DS1AP",
                          AUDPROC_MODULE_ID_DS1AP}
    @h2xmlm_description  {ID of the AUDPROC_MODULE_ID_DS1AP module.\n

    This module supports the following parameter IDs:\n
        - #AUDPROC_PARAM_ID_DS1AP_DEON\n
        - #AUDPROC_PARAM_ID_DS1AP_DEA\n
	    - #AUDPROC_PARAM_ID_DS1AP_DED\n
		- #AUDPROC_PARAM_ID_DS1AP_DVLE\n
		- #AUDPROC_PARAM_ID_DS1AP_DVLA\n
		- #AUDPROC_PARAM_ID_DS1AP_DVLI\n
		- #AUDPROC_PARAM_ID_DS1AP_DVLO\n
		- #AUDPROC_PARAM_ID_DS1AP_DVME\n
		- #AUDPROC_PARAM_ID_DS1AP_DVMC\n
		- #AUDPROC_PARAM_ID_DS1AP_GENB\n
        - #AUDPROC_PARAM_ID_DS1AP_GEBF\n
		- #AUDPROC_PARAM_ID_DS1AP_GEBG\n
		- #AUDPROC_PARAM_ID_DS1AP_IENB\n
        - #AUDPROC_PARAM_ID_DS1AP_IEBF\n
		- #AUDPROC_PARAM_ID_DS1AP_IEBT\n
		- #AUDPROC_PARAM_ID_DS1AP_IEA\n
		- #AUDPROC_PARAM_ID_DS1AP_AOON\n
		- #AUDPROC_PARAM_ID_DS1AP_AOCC\n
		- #AUDPROC_PARAM_ID_DS1AP_AONB\n
		- #AUDPROC_PARAM_ID_DS1AP_AOBF\n
		- #AUDPROC_PARAM_ID_DS1AP_AOBG\n
		- #AUDPROC_PARAM_ID_DS1AP_ARNB\n
		- #AUDPROC_PARAM_ID_DS1AP_ARBF\n
		- #AUDPROC_PARAM_ID_DS1AP_ARBI\n
		- #AUDPROC_PARAM_ID_DS1AP_ARBL\n
		- #AUDPROC_PARAM_ID_DS1AP_ARBH\n
		- #AUDPROC_PARAM_ID_DS1AP_AROD\n
		- #AUDPROC_PARAM_ID_DS1AP_ARTP\n
		- #AUDPROC_PARAM_ID_DS1AP_BVER\n
		- #AUDPROC_PARAM_ID_DS1AP_BNDL\n
		- #AUDPROC_PARAM_ID_DS1AP_VER\n
		- #AUDPROC_PARAM_ID_DS1AP_INIT_ENDP\n
		- #AUDPROC_PARAM_ID_DS1AP_PREG\n
		- #AUDPROC_PARAM_ID_DS1AP_PSTG\n
		- #AUDPROC_PARAM_ID_DS1AP_VOL\n
		- #AUDPROC_PARAM_ID_DS1AP_PLMD\n
        - #AUDPROC_PARAM_ID_DS1AP_PLB\n
		- #AUDPROC_PARAM_ID_DS1AP_TEST\n
		- #AUDPROC_PARAM_ID_DS1AP_NGON\n
		- #AUDPROC_PARAM_ID_DS1AP_VNNB\n
        - #AUDPROC_PARAM_ID_DS1AP_VNBF\n
		- #AUDPROC_PARAM_ID_DS1AP_VNBG\n
		- #AUDPROC_PARAM_ID_DS1AP_VNBE\n
		- #AUDPROC_PARAM_ID_DS1AP_VCBG\n
		- #AUDPROC_PARAM_ID_DS1AP_VCBG\n
		- #AUDPROC_PARAM_ID_DS1AP_VEN\n
		- #AUDPROC_PARAM_ID_DS1AP_VCNB\n
		- #AUDPROC_PARAM_ID_DS1AP_VDHE\n
		- #AUDPROC_PARAM_ID_DS1AP_DHSB\n
		- #AUDPROC_PARAM_ID_DS1AP_DHRG\n
		- #AUDPROC_PARAM_ID_DS1AP_VSPE\n
		- #AUDPROC_PARAM_ID_DS1AP_DSSB\n
		- #AUDPROC_PARAM_ID_DS1AP_DSSA\n
		- #AUDPROC_PARAM_ID_DS1AP_DSSF\n
		- #AUDPROC_PARAM_ID_DS1AP_SCPE\n
		- #AUDPROC_PARAM_ID_DS1AP_VMON\n
		- #AUDPROC_PARAM_ID_DS1AP_VMB\n

    All parameter IDs are device independent.
	Dolby Surround1 Module License ID DOLBY_DS1_LICENSE_ID 0x00000001. This ID is used as an identifier for DS1 license via ADSP generic license mechanism.\n\n
	 This ID can be used to create custom topology. All the params must be defined
     under this module id if it is to be used as custom topology.\n

      Topology Recommendation: \n
      Guideline: The media format converter module (AUDPROC_MODULE_ID_MFC,
          0x10912) should be placed after the DS1AP module.\n
       Reason: The DS1AP module changes the number of channels in the  data.\n
          If it is disabled, the MFC module will convert the data to the correct
          number of channels.\n
       Mandatory/Recommended: Recommended \n}
    @h2xmlm_toolPolicy 				{NO_SUPPORT}
    @{                   <-- Start of the Module -->
*/
/* Dolby Surround1 Module License ID. This ID is used as an identifier for DS1 license via ADSP generic license mechanism.
 * Please refer AVCS_CMD_SET_LICENSE for more details
*/
#define DOLBY_DS1_LICENSE_ID                                 0x00000001

#define AUDPROC_MODULE_ID_DS1AP_DE   				          0x00010737

/* ID of the Dialog Enhancer(DE) Enable used by AUDPROC_MODULE_ID_DS1AP_DE.*/
#define AUDPROC_PARAM_ID_DS1AP_DEON   							0x00010738

/* Structure for the Dialog Enhancer(DE) Enable parameter for a DS1AP DE module. */
typedef struct audproc_param_id_ds1ap_deon_t audproc_param_id_ds1ap_deon_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DEON", AUDPROC_PARAM_ID_DS1AP_DEON}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_deon_t
{
    uint32_t                 de_enable;
       /**< @h2xmle_description  {Specifies the state of Dialog Enhancement(DE) module.\n }
        @h2xmle_default   {0}
	@h2xmle_rangeList {"Disable" = 0; "Enable" = 1} */

}
#include "adsp_end_pack.h"
;

/* ID of the Dialog Enhancer Amount used by AUDPROC_MODULE_ID_DS1AP_DE.*/
#define AUDPROC_PARAM_ID_DS1AP_DEA  				 			0x00010739

/* Structure for the Dialog Enhancer Amount parameter for a DS1AP DE module. */
typedef struct audproc_param_id_ds1ap_dea_t audproc_param_id_ds1ap_dea_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DEA", AUDPROC_PARAM_ID_DS1AP_DEA}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dea_t
{
    uint32_t                 de_amount;
     /**< @h2xmle_description  {Controls the amount of dialog boosting.\n
      Numerical Range:  [0,16] (which maps to [0.00,1.00] ) with 4 fractional bits.\n }
        @h2xmle_default   {0}
	@h2xmle_range {0..16} */
}
#include "adsp_end_pack.h"
;


/* ID of the Dialog Enhancer Ducking factor used by AUDPROC_MODULE_ID_DS1AP_DE.*/
#define AUDPROC_PARAM_ID_DS1AP_DED   							0x0001073A

/* Structure for the Dialog Enhancer Ducking parameter for a DS1AP DE module. */
typedef struct audproc_param_id_ds1ap_ded_t audproc_param_id_ds1ap_ded_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DED", AUDPROC_PARAM_ID_DS1AP_DED}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_ded_t
{
    uint32_t                 de_ducking;
     /**< @h2xmle_description  {Controls the degree of suppresion of channels that do not have dialog content.\n
      Numerical Range:  [0,16] (which maps to [0.00,1.00] ) with 4 fractional bits.\n }
        @h2xmle_default   {0}
	@h2xmle_range {0..16} */
}
#include "adsp_end_pack.h"
;


#define AUDPROC_MODULE_ID_DS1AP_DV   								0x0001073B

/* ID of the Dolby Volume Leveller Enable used by AUDPROC_MODULE_ID_DS1AP_DV. */
#define AUDPROC_PARAM_ID_DS1AP_DVLE  								0x0001073C

/* Structure for the Dolby Volume Leveller Enable parameter for a DS1AP DV module. */
typedef struct audproc_param_id_ds1ap_dvle_t audproc_param_id_ds1ap_dvle_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DVLE", AUDPROC_PARAM_ID_DS1AP_DVLE}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dvle_t
{
    uint32_t                 dvle_enable;
	/**< @h2xmle_description  {Specifies the state of Dolby Volume Leveler(DVLE) in the Dolby Volume module.\n }
        @h2xmle_default   {1}
	@h2xmle_rangeList {"Disable" = 0; "Enable" = 1} */

}
#include "adsp_end_pack.h"
;


/* ID of the Dolby Volume Leveller Amount used by AUDPROC_MODULE_ID_DS1AP_DV. */
#define AUDPROC_PARAM_ID_DS1AP_DVLA  								0x0001073D

/* Structure for the Dolby Volume Leveller Amount parameter for a DS1AP DV module. */
typedef struct audproc_param_id_ds1ap_dvla_t audproc_param_id_ds1ap_dvla_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DVLA", AUDPROC_PARAM_ID_DS1AP_DVLA}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dvla_t
{
    uint32_t                 dvle_amount;
	 /**< @h2xmle_description  {Defines how much the leveler can adjust the loudness to normalize different audio content.\n}
        @h2xmle_default   {7}
	 @h2xmle_range {0..10} */

}
#include "adsp_end_pack.h"
;


/* ID of the Dolby Volume Leveller Input Target used by AUDPROC_MODULE_ID_DS1AP_DV.*/
#define AUDPROC_PARAM_ID_DS1AP_DVLI  								0x0001073E

/* Structure for the Dolby Volume Leveller Input Target parameter for the DS1AP DV module. */
typedef struct audproc_param_id_ds1ap_dvli_t audproc_param_id_ds1ap_dvli_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DVLI", AUDPROC_PARAM_ID_DS1AP_DVLI}
    @h2xmlp_description { Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dvli_t
{
    int32_t                 dvle_in_target;
	 /**< @h2xmle_description  {Sets the target average loudness level. Dolby Volume will level the audio to the average loudness level specified by the Leveler Input Target.The level is specified according to a K loudness weighting.\n
     Default : -20.00 dB LKFS \n
	 Numerical Range:  [-640,0] (which means [-40.00,0.00] dB LKFS) with 4 fractional bits.\n }
     @h2xmle_default   {-20}
	@h2xmle_range {-640..0} */


}
#include "adsp_end_pack.h"
;

/* ID of the Dolby Volume Leveller Output Target used by AUDPROC_MODULE_ID_DS1AP_DV. */
#define AUDPROC_PARAM_ID_DS1AP_DVLO									0x0001073F

/* Structure for the Dolby Volume Leveller Target Output parameter for a DS1AP DV module. */
typedef struct audproc_param_id_ds1ap_dvlo_t audproc_param_id_ds1ap_dvlo_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DVLO", AUDPROC_PARAM_ID_DS1AP_DVLO}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dvlo_t
{
    int32_t                 dvle_out_target;
	 /**< @h2xmle_description  {The Leveler Output Target is set by a system designer in order to calibrate the system to the proper playback Sound Pressure Level[SPL].To compensate for the difference between mastering and playback levels, Dolby Volume applies an un-shaped digital gain of Leveler Output Target minus Leveler Input Target,so that zero digital and analog volume produces reference loudness levels.\n
     Default : -20.00 dB \n
	 Numerical Range:  [-640,0] (which means [-40.00,0.00] dB) with 4 fractional bits.\n }
     @h2xmle_default   {-20}
	 @h2xmle_range {-640..0} */


}
#include "adsp_end_pack.h"
;


/* ID of the Dolby Volume Modeller Enable used by AUDPROC_MODULE_ID_DS1AP_DV. */
#define AUDPROC_PARAM_ID_DS1AP_DVME  								0x00010740

/* Structure for the Dolby Volume Modeller Enable parameter for a DS1AP DV module. */
typedef struct audproc_param_id_ds1ap_dvme_t audproc_param_id_ds1ap_dvme_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DVME", AUDPROC_PARAM_ID_DS1AP_DVME}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dvme_t
{
    uint32_t                 dvm_enable;
	/**< @h2xmle_description  {Specifies the state of Dolby Volume Modeler (DVM) module.\n }
        @h2xmle_default   {0}
	@h2xmle_rangeList {"Disable" = 0; "Enable" = 1} */

}
#include "adsp_end_pack.h"
;

/* ID of the Dolby Volume Modeller Calibration used by AUDPROC_MODULE_ID_DS1AP_DV. */
#define AUDPROC_PARAM_ID_DS1AP_DVMC  								0x00010741


/* Structure for the Dolby Volume Modeller Calibration parameter for a DS1AP DV module. */
typedef struct audproc_param_id_ds1ap_dvmc_t audproc_param_id_ds1ap_dvmc_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DVMC", AUDPROC_PARAM_ID_DS1AP_DVMC}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dvmc_t
{
    int32_t                 dvm_calibration;
	 /**< @h2xmle_description  {The calibration offset provides the ability to fine-tune the actual playback-level after the Leveler Output Target has been set
		 by the system manufacturer. This input parameter allows end users to adjust for variables such as listening positions or differing
		 speaker sensitivities. For example, the end user of an A/V receiver (AVR) with Dolby Volume can recalibrate an AVR to optimize the
		 reference level for the user's actual listening position.\n
         Default : 0.00, 0.00, 0.00, 0.00, ... dB\n
	  Numerical Range:  [-320,320] (which means [-20.00,20.00] dB) with 4 fractional bits.\n }
        @h2xmle_default   {0}
	@h2xmle_range {-320..320} */

}
#include "adsp_end_pack.h"
;


#define AUDPROC_MODULE_ID_DS1AP_GEQ						0x00010747

/* ID of the Graphic Equalizer Enable used by AUDPROC_MODULE_ID_DS1AP_GEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_GEON   					0x00010748
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DS1AP 				0x0001033B
/* Structure for the Graphic Equalizer Enable parameter for a DS1AP GEQ module. */
typedef struct audproc_param_id_ds1ap_geon_t audproc_param_id_ds1ap_geon_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_GEON", AUDPROC_PARAM_ID_DS1AP_GEON}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_geon_t
{
    uint32_t                 geq_enable;
	/**< @h2xmle_description  {Specifies the state of Graphic Equalizer (GEQ) module.\n }
        @h2xmle_default   {0}
	@h2xmle_rangeList {"Disable" = 0; "Enable" = 1} */

}
#include "adsp_end_pack.h"
;


/* ID of the Graphic Equalizer Band Count used by AUDPROC_MODULE_ID_DS1AP_GEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_GENB   					0x00010749

/* Structure for the Graphic Equalizer Band Count parameter for a DS1AP GEQ module. */
typedef struct audproc_param_id_ds1ap_genb_t audproc_param_id_ds1ap_genb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_GENB", AUDPROC_PARAM_ID_DS1AP_GENB}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_genb_t
{
    uint32_t                 geq_band_count;
	 /**< @h2xmle_description  {Specifies the number of Frequency bands Graphic Equalizer operate in. Similar number of target band gains and center frequencies will need to be provided to the Intelligent Equalizer.\n
          Default : 0.00, 0.00, 0.00, 0.00, ... dB\n
	  Numerical Range:  [1,40] \n}
        @h2xmle_default   {10}
	@h2xmle_range {1..40} */

}
#include "adsp_end_pack.h"
;


/* ID of the Graphic Equalizer Band Center Frequencies used by AUDPROC_MODULE_ID_DS1AP_GEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_GEBF   					0x0001074A

/* Structure for the Graphic Equalizer Band Center Frequency parameter for a DS1AP GEQ module. */
typedef struct audproc_param_id_ds1ap_gebf_t audproc_param_id_ds1ap_gebf_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_GEBF", AUDPROC_PARAM_ID_DS1AP_GEBF}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_gebf_t
{
    uint32_t                 geq_band_freq[40];
	/**< @h2xmle_description  {Specifies the band center frequencies for the Graphic Equalizer.\n
          Default : 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ... Hertz\n
	  Numerical Range:  [20,20000] in hertz for each band.\n } */

}
#include "adsp_end_pack.h"
;


/* ID of the Graphic Equalizer Band Gains used by AUDPROC_MODULE_ID_DS1AP_GEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_GEBG   					0x0001074B

/* Structure for the  Graphic Equalizer Band Gains parameter for a DS1AP DV module. */
typedef struct audproc_param_id_ds1ap_gebg_t audproc_param_id_ds1ap_gebg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_GEBG", AUDPROC_PARAM_ID_DS1AP_GEBG}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_gebg_t
{
    int32_t                 geq_band_gain[40];
	 /**< @h2xmle_description  {Specifies the band gains for the Graphic Equalizer.\n
          Default : 0.00, 0.00, 0.00, 0.00, ... dB\n
	  Numerical Range:  [-576,576] (which means [-36.00,36.00] dB) with 4 fractional bits.\n }
        @h2xmle_default   {0}
	@h2xmle_range {-576..576} */

}
#include "adsp_end_pack.h"
;



#define AUDPROC_MODULE_ID_DS1AP_IEQ   0x00010742

/* ID of the Intelligent Equalizer Enable used by AUDPROC_MODULE_ID_DS1AP_IEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_IEON   									0x00010743

/* Structure for the Intelligent Equalizer Enable parameter for a DS1AP IEQ module. */
typedef struct audproc_param_id_ds1ap_ieon_t audproc_param_id_ds1ap_ieon_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_IEON", AUDPROC_PARAM_ID_DS1AP_IEON}
    @h2xmlp_description {  User Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_ieon_t
{
    uint32_t                 ieq_enable;
    /**< @h2xmle_description  {Specifies the state of Intelligent Equalizer (IEQ) module.\n }
        @h2xmle_default   {0}
	@h2xmle_rangeList {"Disable" = 0; "Enable" = 1} */

}
#include "adsp_end_pack.h"
;

/* ID of the Intelligent Equalizer Band Count used by AUDPROC_MODULE_ID_DS1AP_IEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_IENB   									0x00010744

/* Structure for the Intelligent Equalizer Band Count parameter for a DS1AP IEQ module. */
typedef struct audproc_param_id_ds1ap_ienb_t audproc_param_id_ds1ap_ienb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_IENB", AUDPROC_PARAM_ID_DS1AP_IENB}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_ienb_t
{
    uint32_t                 ieq_band_count;
     /**< @h2xmle_description  { Specifies the number of Frequency bands Intelligent Equalizer operate in. Similar number of target band gains and center frequencies will need to be provided to the Intelligent Equalizer.\n }
        @h2xmle_default   {10}
	@h2xmle_range {1..40} */

}
#include "adsp_end_pack.h"
;

/* ID of the Intelligent Equalizer Band Center frequencies used by AUDPROC_MODULE_ID_DS1AP_IEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_IEBF   								    0x00010745

/* Structure for the Intelligent Equalizer Band Center frequencies parameter for a DS1AP IEQ module. */
typedef struct audproc_param_id_ds1ap_iebf_t audproc_param_id_ds1ap_iebf_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_IEBF", AUDPROC_PARAM_ID_DS1AP_IEBF}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_iebf_t
{
    uint32_t                 ieq_band_freq[40];
    /**< @h2xmle_description  { Specifies the band center frequencies for the Intelligent Equalizer in 'hertz'.\n
          Default : 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ... Hertz. \n }
        @h2xmle_default   {10} */

}
#include "adsp_end_pack.h"
;



/** ID of the Intelligent Equalizer Band Targets used by AUDPROC_MODULE_ID_DS1AP_IEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_IEBT   									0x00010746

/* Structure for the Intelligent Equalizer Band Target parameter for a DS1AP IEQ module. */
typedef struct audproc_param_id_ds1ap_iebt_t audproc_param_id_ds1ap_iebt_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_IEBT", AUDPROC_PARAM_ID_DS1AP_IEBT}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_iebt_t
{
    int32_t                 ieq_band_target[40];
     /**< @h2xmle_description  {Specifies the band target levels for the Intelligent Equalizer.\n
          Default :  0.00, 0.00, 0.00, 0.00, ... dBFS. \n
	  Numerical Range: [-480,480] (which maps to [-30.00,30.00] dBFS) with 4 fractional bits.\n }
        @h2xmle_default   {10}
	@h2xmle_range {-480..480} */

}
#include "adsp_end_pack.h"
;

/** ID of the Intelligent Equalizer Amount used by AUDPROC_MODULE_ID_DS1AP_IEQ.*/
#define AUDPROC_PARAM_ID_DS1AP_IEA   									0x0001076A


/* Structure for the Intelligent Equalizer Amount parameter for a DS1AP IEQ module. */
typedef struct audproc_param_id_ds1ap_iea_t audproc_param_id_ds1ap_iea_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_IEA", AUDPROC_PARAM_ID_DS1AP_IEA}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_iea_t
{
    int32_t                 ieq_amount;
     /**< @h2xmle_description  {Specifies the strength of the Intelligent Equalizer effect to apply.\n
          Default : 0.63 \n
	  Numerical Range:  [0,16] (which means [0.00,1.00] ) with 4 fractional bits.\n }
        @h2xmle_default   {10}
	@h2xmle_range {0..16} */

}
#include "adsp_end_pack.h"
;

#define AUDPROC_MODULE_ID_DS1AP_AO  						0x00010758

/* ID of the Audio Optimizer Enable used by AUDPROC_MODULE_ID_DS1AP_AO. */
#define AUDPROC_PARAM_ID_DS1AP_AOON       					0x00010759

/* Structure for the Audio Optimizer Enable parameter for the Audio Optimizer module. */
typedef struct audproc_param_id_ds1ap_aoon_t audproc_param_id_ds1ap_aoon_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_AOON",AUDPROC_PARAM_ID_DS1AP_AOON}
    @h2xmlp_description {Factory Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_aoon_t
{
    uint32_t  ao_enable;
    /**< @h2xmle_description  { Specifies the state of the Audio Optimizer.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1;
			      "Auto( Enable if endpoint is set as internal speakers)" = 2}
        @h2xmle_default      {2}  */

}
#include "adsp_end_pack.h"
;


/** ID of the Audio Optimizer Channel Count used by AUDPROC_MODULE_ID_DS1AP_AO. */
#define AUDPROC_PARAM_ID_DS1AP_AOCC       					0x0001075A

/* Structure for the Audio Optimizer Channel Count parameter for the Audio Optimizer module. */
typedef struct audproc_param_id_ds1ap_aocc_t audproc_param_id_ds1ap_aocc_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_AOCC",AUDPROC_PARAM_ID_DS1AP_AOCC}
    @h2xmlp_description {Factory Configurable}  */


#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_aocc_t
{
    uint32_t  ao_ch_count;
    /**< @h2xmle_description  {Specifies the maximum number of channel data, which will be provided to the Audio Optimizer.}
        @h2xmle_range    {0,8}
        @h2xmle_default  {1}  */

}
#include "adsp_end_pack.h"
;


/** ID of the Audio Optimizer Band Count used by AUDPROC_MODULE_ID_DS1AP_AO.*/
#define AUDPROC_PARAM_ID_DS1AP_AONB        					0x0001075B

/* Structure for the Audio Optimizer Band Count parameter for the Audio Optimizer module. */
typedef struct audproc_param_id_ds1ap_aonb_t audproc_param_id_ds1ap_aonb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_AONB",AUDPROC_PARAM_ID_DS1AP_AONB}
    @h2xmlp_description {Factory Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_aonb_t
{
    uint32_t ao_band_count;
     /**< @h2xmle_description  {Specifies the number of frequency bands per channel, in which Audio Optimizer will be operating in.}
          @h2xmle_range    {1,40}
          @h2xmle_default  {10}  */

}
#include "adsp_end_pack.h"
;

/** ID of the Audio Optimizer Band Center frequencies used by AUDPROC_MODULE_ID_DS1AP_AO. */
#define AUDPROC_PARAM_ID_DS1AP_AOBF         				0x0001075C

/* Structure for the Audio Optimizer Band Center frequency parameter for Audio Optimizer module. */
typedef struct audproc_param_id_ds1ap_aobf_t audproc_param_id_ds1ap_aobf_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_AOBF",AUDPROC_PARAM_ID_DS1AP_AOBF}
    @h2xmlp_description {Factory Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_aobf_t
{
    uint32_t  ao_band_freq[40];
     /**< @h2xmle_description  { Specifies the center frequencies of each band of Audio Optimizer, in unit of'hertz'.}
          @h2xmle_range    {20,20000}
          @h2xmle_default  {64}  */
}
#include "adsp_end_pack.h"
;


/** ID of the Audio Optimizer Band Gains for each channel used by AUDPROC_MODULE_ID_DS1AP_AO. */
#define AUDPROC_PARAM_ID_DS1AP_AOBG        					0x0001075D

/* Structure for the Audio Optimizer Band Gains parameter of each channel for the Audio Optimizer module. */
typedef struct audproc_param_id_ds1ap_aobg_t audproc_param_id_ds1ap_aobg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_AOBG",AUDPROC_PARAM_ID_DS1AP_AOBG}
    @h2xmlp_description {Factory Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_aobg_t
{
    int32_t  ao_band_gain[329];
     /**< @h2xmle_description  {Specifies the gains for each band corresponding to each channel in the Audio Optimizer.\n
      NOTE: This data is stored channel by channel. Each data set is preceeded by a channel identifier for that particular set of gains.\n
      Numerical Range: [-480,480] (which maps to [-30.00,30.00] dB) \n
      Default : 2, 0.00, 0.00, 0.00 ... dB \n\n
      The data array will stop searching when either 'AUDPROC_PARAM_ID_DS1AP_AOCC' number of channels worth of data has been read or the
		 channel ID AK_CHAN_EMPTY is encountered. All other channels (if applicable) will be set to have a gain of 0 dB.
		 Enumerated values of channel identifiers as mentioned above are given below:\n
		 AK_CHAN_EMPTY    (0)\n
		 AK_CHAN_L        (2)\n
		 AK_CHAN_R        (3)\n
		 AK_CHAN_C        (4)\n
		 AK_CHAN_LFE      (5)\n
		 AK_CHAN_Ls       (7)\n
		 AK_CHAN_Rs       (8)\n
		 AK_CHAN_Lb       (9)\n
		 AK_CHAN_Rb      (10)\n}

      @h2xmle_range    {-480,480}   */

}
#include "adsp_end_pack.h"
;


#define AUDPROC_MODULE_ID_DS1AP_ARE                         0x0001075E

/** ID of the Audio Regulator Band Count used by AUDPROC_MODULE_ID_DS1AP_ARE. */
#define AUDPROC_PARAM_ID_DS1AP_ARNB         			    0x0001075F

/* Structure for the Audio Regulator Band Count parameter for the DS1AP Audio Regulator module. */
typedef struct audproc_param_id_ds1ap_arnb_t audproc_param_id_ds1ap_arnb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_ARNB", AUDPROC_PARAM_ID_DS1AP_ARNB}
    @h2xmlp_description {Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_arnb_t
{
    uint32_t ar_band_count;
     /**< @h2xmle_description  { Specifies the number of frequency bands which will be supplied to the Audio Regulator.\n
	 When this value changes, the Band Frequencies(AUDPROC_PARAM_ID_DS1AP_ARBF), Isolates(AUDPROC_PARAM_ID_DS1AP_ARBI),
	 the Higher Thresholds(AUDPROC_PARAM_ID_DS1AP_ARBH) and Lower Thresholds(AUDPROC_PARAM_ID_DS1AP_ARBL) must all be re-written.\n
         The Audio Regulator will not be permitted to enter speaker distortion limiting mode until valid data has been supplied\n.}

         @h2xmle_range    {1,40}
        @h2xmle_default      {10}  */
}
#include "adsp_end_pack.h"
;

/** ID of the Audio Regulator Band Center Frequencies used by AUDPROC_MODULE_ID_DS1AP_ARE. */
#define AUDPROC_PARAM_ID_DS1AP_ARBF          				0x00010760

/* Structure for the Audio Regulator Band Center Frequencies parameter for the DS1AP Audio Regulator module. */
typedef struct audproc_param_id_ds1ap_arbf_t audproc_param_id_ds1ap_arbf_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_ARBF" ,AUDPROC_PARAM_ID_DS1AP_ARBF}
    @h2xmlp_description {Factory Configurable.}  */


#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_arbf_t
{
    uint32_t  ar_band_freq[40];
    /**< @h2xmle_description  { An array of specifying the center frequencies of the bands the Audio Regulator will operate on.\n
	These values must increment from lowest to highest. Invalid data will cause the Audio Regulator to not be permitted to enter
	 speaker distortion limiting mode. The 'AUDPROC_PARAM_ID_DS1AP_ARBI', 'AUDPROC_PARAM_ID_DS1AP_ARBL' and 'AUDPROC_PARAM_ID_DS1AP_ARBH'
	 parameters must be updated after modifying this paramete\n\n.
	 Numerical Range in hertz for each band.\n
	 Default : 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ... Hertz\n} */
}
#include "adsp_end_pack.h"
;


/** ID of the Audio Regulator Band Isolate used by AUDPROC_MODULE_ID_DS1AP_ARE. */
#define AUDPROC_PARAM_ID_DS1AP_ARBI          				0x00010761

/* Structure for the Audio Regulator Band Isolate parameter for the DS1AP Audio Regulator module. */
typedef struct audproc_param_id_ds1ap_arbi_t audproc_param_id_ds1ap_arbi_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_ARBI", AUDPROC_PARAM_ID_DS1AP_ARBI}
    @h2xmlp_description {Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_arbi_t
{
    uint32_t  ar_band_islolates[40];
   /**< @h2xmle_description  {Specifies whether to operate bands in isolated or regulated mode. No Timbre Preservation will be applied to isolated bands.}
        @h2xmle_rangeList    {"Regulated"=0;
                              "Isolated"=1}
        @h2xmle_default      {0}  */

}
#include "adsp_end_pack.h"
;


/** ID of the Audio Regulator Band Thresholds Lower used by AUDPROC_MODULE_ID_DS1AP_ARE.*/
#define AUDPROC_PARAM_ID_DS1AP_ARBL           				0x00010762

/* Structure for the Audio Regulator Band Low Thresholds parameter for the DS1AP Audio Regulator module. */
typedef struct audproc_param_id_ds1ap_arbl_t audproc_param_id_ds1ap_arbl_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_ARBL", AUDPROC_PARAM_ID_DS1AP_ARBL}
    @h2xmlp_description {Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_arbl_t
{
    int32_t  ar_band_lt[40];
    /**< @h2xmle_description  {Specifies the low onset point of the compression curve.\n
      Numerical Range: [-2080,0] (which maps to[-130.00,0.00] dBFS) with 4 fractional bits.\n
      Default : -12.00, -12.00, -12.00, -12.00, -12.00, -12.00, -12.00, -12.00, -12.00 ...., -12.00 dBFS\n} */
}
#include "adsp_end_pack.h"
;

/** ID of the Audio Regulator Band Thresholds Higher used by AUDPROC_MODULE_ID_DS1AP_ARE. */
#define AUDPROC_PARAM_ID_DS1AP_ARBH       					0x00010763

/* Structure for the Audio Regulator Band High Thresholds  parameter for the DS1AP Audio Regulator module. */
typedef struct audproc_param_id_ds1ap_arbh_t audproc_param_id_ds1ap_arbh_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_ARBH", AUDPROC_PARAM_ID_DS1AP_ARBH}
    @h2xmlp_description {Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_arbh_t
{
    int32_t  ar_band_ht[40];
     /**< @h2xmle_description  {Specifies the higher limiting point of the compression curve.\n
      Numerical Range: [-2080,0] (which maps to [-130.00,0.00] dBFS) with 4 fractional bits.\n
      Default : 0.00, 0.00, 0.00, 0.00, 0.00 , 0.00, 0.00, 0.00,...., 0.00 dBFS\n} */

}
#include "adsp_end_pack.h"
;

/** ID of the Audio Regulator Overdrive used by AUDPROC_MODULE_ID_DS1AP_ARE. */
#define AUDPROC_PARAM_ID_DS1AP_AROD      					0x00010764

/* Structure for the Audio Regulator Overdrive parameter for the DS1AP Audio Regulator module. */
typedef struct audproc_param_id_ds1ap_arod_t audproc_param_id_ds1ap_arod_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_AROD", AUDPROC_PARAM_ID_DS1AP_AROD}
    @h2xmlp_description {Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_arod_t
{
    uint32_t   ar_overdrive;
   /**< @h2xmle_description  { Specifies the amount,the signal is allowed to push past the distortion thresholds.\n
    Numerical Range: [0,192] (which maps to[0.00,12.00] dB) with 4 fractional bits.\n}
        @h2xmle_range    {0,192}
        @h2xmle_default      {0}  */

}
#include "adsp_end_pack.h"
;

/** ID of the Audio Regulator Timbre Preservation used by AUDPROC_MODULE_ID_DS1AP_ARE.*/
#define AUDPROC_PARAM_ID_DS1AP_ARTP    						0x00010765

/* Structure for the Audio Regulator Timbre Preservation for a DS1AP Audio Regulator module. */
typedef struct audproc_param_id_ds1ap_artp_t audproc_param_id_ds1ap_artp_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_ARTP", AUDPROC_PARAM_ID_DS1AP_ARTP}
    @h2xmlp_description {Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_artp_t
{
    uint32_t               ar_tp_amount;
    /**< @h2xmle_description  { Controls the compromise between loudness and timbre preservation at high volume levels.\n
     Numerical Range: [0,16] (which maps to [0.00,1.00] ) with 4 fractional bits.\n}
      @h2xmle_default      {1}  */

}
#include "adsp_end_pack.h"
;



#define AUDPROC_MODULE_ID_DS1AP_BUNDLE	                    0x00010723

/** ID to get the Bundle Version of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_BUNDLE. */
#define AUDPROC_PARAM_ID_DS1AP_BVER  						0x00010724

/* Structure for the bundle version parameter for a DS1AP Bundle module. */
typedef struct audproc_param_id_ds1ap_bver_t audproc_param_id_ds1ap_bver_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_BVER", AUDPROC_PARAM_ID_DS1AP_BVER}
    @h2xmlp_description { READ_ONLY}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_bver_t
{
    int32_t                  bundle_version[5];
    /**< @h2xmle_description  {Identifies the version of the bundle as a combination of versions of all objects. This parameter is automatically calculated as the sum of the versions of all objects in the bundle and is intended as a way to uniquely identify the bundle, in conjunction with the AUDPROC_PARAM_ID_DS1AP_BNDL parameter.\n
      Default : 3,36,6,0,0\n }
     @h2xmle_range       {-32768..32767} */

}
#include "adsp_end_pack.h"
;



/** ID to get the Bundle ID of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_BUNDLE. */
#define AUDPROC_PARAM_ID_DS1AP_BNDL 						0x00010725

/* Structure for the bundle ID parameter for a DS1AP Bundle module. */
typedef struct audproc_param_id_ds1ap_bndl_t audproc_param_id_ds1ap_bndl_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_BNDL", AUDPROC_PARAM_ID_DS1AP_BNDL}
    @h2xmlp_description { READ_ONLY}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_bndl_t
{
    int32_t                  bundle_id[2];
    /**< @h2xmle_description {This Parameter is used get to Identify the bundle. This can be interpreted as a four character ASCII name identifying the Bundle ID.\n
     Default :  21316,49 \n}
    @h2xmle_range       {-32768..32767}  */

}
#include "adsp_end_pack.h"
;

/** ID to get the object version of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_BUNDLE. */
#define AUDPROC_PARAM_ID_DS1AP_VER 						    0x00010726

/* Structure to get the object version parameter for a DS1AP Bundle module. */
typedef struct audproc_param_id_ds1ap_ver_t audproc_param_id_ds1ap_ver_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VER", AUDPROC_PARAM_ID_DS1AP_VER}
    @h2xmlp_description { READ_ONLY}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_ver_t
{
    int32_t                  version[4];
    /**< @h2xmle_description {This Parameter is used get the Version number of an object.\n
     Default : 1,8,3,0 \n}
    @h2xmle_range       {-32768..32767}  */
}
#include "adsp_end_pack.h"
;


/** ID of the Endpoint of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_BUNDLE. */
#define AUDPROC_PARAM_ID_DS1AP_INIT_ENDP   					0x00010727

/* Structure for the endpoint parameter for a DS1AP Bundle module. */
typedef struct audproc_param_id_ds1ap_init_endp_t audproc_param_id_ds1ap_init_endp_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_INIT_ENDP", AUDPROC_PARAM_ID_DS1AP_INIT_ENDP}
    @h2xmlp_description {Factory Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_init_endp_t
{
    uint32_t                 endpoint;
    /**< @h2xmle_description  {Specifies the Endpoint of the signal chain .i.e., Device to which it is connected.}
        @h2xmle_rangeList    {"DS1_ENDP_INT_SPEAKERS"=0;
                              "DS1_ENDP_EXT_SPEAKERS"=1;
			      "DS1_ENDP_HEADPHONES"=2;
			      "DS1_ENDP_HDMI"=3;
			      "DS1_ENDP_SPDIF"=4;
			      "DS1_ENDP_DLNA"=5;
			      "DS1_ENDP_ANALOG"=6}
        @h2xmle_default      {0}  */


}
#include "adsp_end_pack.h"
;


/** ID of the Pregain applied to Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_BUNDLE. */
#define AUDPROC_PARAM_ID_DS1AP_PREG   						0x00010728

/* Structure for the bundle pregain parameter for a DS1AP Bundle module. */
typedef struct audproc_param_id_ds1ap_preg_t audproc_param_id_ds1ap_preg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_PREG", AUDPROC_PARAM_ID_DS1AP_PREG}
    @h2xmlp_description {Factory Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_preg_t
{
    int32_t                 bundle_pregain;
    /**< @h2xmle_description  { The amount of gain applied to the signal prior to entering the bundle. If the audio entering the DS1 bundle is    known to have been boosted or attenuated, this value should be set to reflect how much boosting has been applied.\n
    Numeric range  : [-2080,480] (which maps to [-130.00,30.00] dB)] with 4 fractional bits\n
    Default : 0.00 dB\n} */

}
#include "adsp_end_pack.h"
;



/** ID of the Postgain applied on Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_BUNDLE. */
#define AUDPROC_PARAM_ID_DS1AP_PSTG   						0x00010729

/* Structure for the bundle postgain parameter for a DS1AP Bundle module. */
typedef struct audproc_param_id_ds1ap_pstg_t audproc_param_id_ds1ap_pstg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_PSTG", AUDPROC_PARAM_ID_DS1AP_PSTG}
    @h2xmlp_description {Factory Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_pstg_t
{
    int32_t                 bundle_postgain;
    /**< @h2xmle_description  {The amount of gain which will be applied after leaving the bundle. For example: by an analog volume control.\n
                               Numeric range  : [-2080,480] (which maps to [-130.00,30.00] dB)] with 4 fractional bits\n		                                     Default : 0.00 dB\n} */

}
#include "adsp_end_pack.h"
;

/** ID of the Bundle volume applied on Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_BUNDLE. */
#define AUDPROC_PARAM_ID_DS1AP_VOL   						0x0001072A

/* Structure for the bundle volume parameter for a DS1AP Bundle module. */
typedef struct audproc_param_id_ds1ap_vol_t audproc_param_id_ds1ap_vol_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VOL", AUDPROC_PARAM_ID_DS1AP_VOL}
    @h2xmlp_description {Factory Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vol_t
{
    int32_t                 bundle_volume;
    /**< @h2xmle_description  {The amount of gain which the user would like to be applied to the signal.\n
                              Range:[-2080,480] (which maps to [-130.00,30.00] dB)] with 4 fractional bits.\n
		              Default : 0.00 dB\n} */

}
#include "adsp_end_pack.h"
;


#define AUDPROC_MODULE_ID_DS1AP_PLIM   					0x00010766

/* ID of the Peak Limiting and Protection Mode used by AUDPROC_MODULE_ID_DS1AP_PLIM.*/
#define AUDPROC_PARAM_ID_DS1AP_PLMD    					0x00010767

/* Structure for the Peak Limiting and Protection Mode parameter for the DS1AP Peak Limiter module. */
typedef struct audproc_param_id_ds1ap_plmd_t audproc_param_id_ds1ap_plmd_t;

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_plmd_t
{
    uint32_t               plim_mode;
     /**< @h2xmle_description  { Specifies or controls the state of the Peak Limiter and Audio Regulator\n
		 •	DS1_PLMD_DISABLE_ALL - Audio Regulator and Peak Limiter will be disabled.\n
		 •	DS1_PLMD_PEAK_ONLY - Peak Limiter will be enabled and Audio Regulator will be disabled.\n
		 •	DS1_PLMD_REGULATED_PEAK - Peak Limiter will be enabled and the Audio Regulator will be enabled in a 'peak limiting only' mode.\n
		 •	DS1_PLMD_REGULATED_DISTORTION - Peak Limiter will be enabled and the Audio Regulator will be enabled in 'speaker distortion limiting' mode.\n
		 •	DS1_PLMD_AUTO - The Peak Limiter and Audio Regulator are always enabled. The Audio Regulator will be operating in peak limiting only mode unless the endpoint is ‘internal speakers’, in which case the Audio Regulator will operate in speaker distortion limiting mode.\n
		NOTE:	The Audio Regulator will not be enabled if the Peak Limiter is disabled.\n}

	@h2xmle_rangeList {"DS1_PLMD_DISABLE_ALL" = 0;"DS1_PLMD_PEAK_ONLY" = 1; "DS1_PLMD_REGULATED_PEAK" = 2; "DS1_PLMD_REGULATED_DISTORTION" = 3; "DS1_PLMD_AUTO" = 4}

	@h2xmle_default {4} */

}
#include "adsp_end_pack.h"
;


/* ID of the Peak Limiter Boost used by AUDPROC_MODULE_ID_DS1AP_PLIM.*/
#define AUDPROC_PARAM_ID_DS1AP_PLB     					0x00010768

/* Structure for the Peak Limiter Boost parameter for the DS1AP Peak Limiter module. */
typedef struct audproc_param_id_ds1ap_plb_t audproc_param_id_ds1ap_plb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_PLB", AUDPROC_PARAM_ID_DS1AP_PLB}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_plb_t
{
    uint32_t               plim_boost;
     /**< @h2xmle_description  { Specifies the gain in dB by which the signal will be boosted. The limiter will apply this maximum gain if the resulting signal is less than the allowed peak level.\n Numerical Range: [0,192] (which means [0.00,12.00] dB) with 4 fractional bits.\n}
          @h2xmle_range  {0..192}
	  @h2xmle_default {0} */

}
#include "adsp_end_pack.h"
;


/* ID of the Peak Limiter Test Mode Enable used by AUDPROC_MODULE_ID_DS1AP_PLIM.*/
#define AUDPROC_PARAM_ID_DS1AP_TEST      				0x00010769


/* Structure for the Peak Limiter Test Mode Enable parameter for the DS1AP Peak limiter Module. */
typedef struct audproc_param_id_ds1ap_test_t audproc_param_id_ds1ap_test_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_TEST", AUDPROC_PARAM_ID_DS1AP_TEST}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_test_t
{
    uint32_t               plim_test_mode;
     /**< @h2xmle_description {Toggles test mode in the peak limiter.\n
          Supported Values:\n
          0 - Peak Limiter will behave normally\n
	  1 – Peak Limiter will be in test mode. In this mode, peak limiter performs a hard clip to limit the output signal.\n}
	  @h2xmle_default {0} */
}
#include "adsp_end_pack.h"
;


#define AUDPROC_PARAM_ID_DS1AP_MIXED_STEREO_ENABLE    0x108c7

typedef struct audproc_param_id_ds1ap_mixed_streo_enable_t audproc_param_id_ds1ap_mixed_streo_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_MIXED_STEREO_ENABLE", AUDPROC_PARAM_ID_DS1AP_MIXED_STEREO_ENABLE}
    @h2xmlp_description {  Factory Configurable.}  */

#include "adsp_begin_pack.h"
struct audproc_param_id_ds1ap_mixed_streo_enable_t
{
    uint32_t               enable;
    /**< @h2xmle_description  {Enables or disables the mixed stereo mode. The mixed stereo mode only takes effect when the output has L and R channels only. L and R channels will be unmodified.Any other value – L and R will be mixed to provide (L+R)/2 on both channels\n }
        @h2xmle_default   {0} */

}
#include "adsp_end_pack.h"
;

#define AUDPROC_MODULE_ID_DS1AP_NGS   						0x00010735


/* ID of the Next Generation Surround(NGS) Enable used by AUDPROC_MODULE_ID_DS1AP_NGS.*/
#define AUDPROC_PARAM_ID_DS1AP_NGON   						0x00010736

/* Structure for the Next Generation Surround(NGS) Enable parameter for a DS1AP NGS module. */
typedef struct audproc_param_id_ds1ap_ngon_t audproc_param_id_ds1ap_ngon_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_NGON", AUDPROC_PARAM_ID_DS1AP_NGON}
    @h2xmlp_description { User Configurable}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_ngon_t
{
    uint32_t                 ngs_enable;
    /**< @h2xmle_description  {Specifies the state of Next Generation Surround (NGS) module.\n }
        @h2xmle_default   {2}
	@h2xmle_rangeList {"Disable" = 0; "Enable" = 1; "Auto(Enable if either Dolby Headphone or Dolby Virtual Speaker is enabled)" = 2} */

}
#include "adsp_end_pack.h"
;



#define AUDPROC_MODULE_ID_DS1AP_VIS    							0x0001072B

/* ID to get the Visualizer Band Count of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS module. */
#define AUDPROC_PARAM_ID_DS1AP_VNNB   							0x0001072C

/* Structure for the Visualizer Band Count parameter for a DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_vnnb_t audproc_param_id_ds1ap_vnnb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VNNB", AUDPROC_PARAM_ID_DS1AP_VNNB}
    @h2xmlp_description {Read only}
    @h2xmlp_toolPolicy   {NO_SUPPORT} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vnnb_t
{
    uint32_t                  vis_band_count;
   /**< @h2xmle_description  {Returns the number of excitations and gains that can be natively exported from the signal chain.This value is dependent on the sample rate.It is always accessible regardless of whether the ‘AUDPROC_PARAM_ID_DS1AP_VEN’ parameter is on or off.\n }
         @h2xmle_default       {1}
         @h2xmle_range    {1..20} */

}
#include "adsp_end_pack.h"
;



/* ID to get the Visualizer Band Center Frequencies of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS module. */
#define AUDPROC_PARAM_ID_DS1AP_VNBF   							0x0001072D

/* Structure for the Visualizer Band Center Frequency parameter for a DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_vnbf_t audproc_param_id_ds1ap_vnbf_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VNBF", AUDPROC_PARAM_ID_DS1AP_VNBF}
    @h2xmlp_description {Read only}
    @h2xmlp_toolPolicy   {NO_SUPPORT} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vnbf_t
{
    uint32_t                  vis_band_freq[20];
     /**< @h2xmle_description  {Contains the center frequencies of the native visualizer bands.These center frequencies are dependent on the sample rate. They are always accessible regardless of whether the ‘AUDPROC_PARAM_ID_DS1AP_VEN’ parameter is on or off. The range of these centre frequencies is from  20 to 20000 Hertz and are unscaled.\n } */

}
#include "adsp_end_pack.h"
;


/* ID to get the Visualizer Band Gains of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS module. */
#define AUDPROC_PARAM_ID_DS1AP_VNBG   							0x0001072E

/* Structure for the Visualizer Band Gains parameter for a DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_vnbg_t audproc_param_id_ds1ap_vnbg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VNBG", AUDPROC_PARAM_ID_DS1AP_VNBG}
    @h2xmlp_description {Read only}
    @h2xmlp_toolPolicy   {NO_SUPPORT} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vnbg_t
{
    int32_t                  vis_band_gain[20];
     /**< @h2xmle_description  { The ‘AUDPROC_PARAM_ID_DS1AP_VEN’ parameter must be 'Enabled' to read this value. The gains are scaled by 16. i.e., 16 = 1 dB.\n }
         @h2xmle_default       {0}
         @h2xmle_range    {-32768..32767} */

}
#include "adsp_end_pack.h"
;


/* ID to get the Visualizer Band Excitations of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS module. */
#define AUDPROC_PARAM_ID_DS1AP_VNBE   							0x0001072F

/* Structure for the Visualizer Band Excitations parameter for a DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_vnbe_t audproc_param_id_ds1ap_vnbe_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VNBE", AUDPROC_PARAM_ID_DS1AP_VNBE}
    @h2xmlp_description {Read only}
    @h2xmlp_toolPolicy   {NO_SUPPORT} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vnbe_t
{
    int32_t                  vis_band_excitation[20];
     /**< @h2xmle_description  {Contains the excitation data for the native visualizer bands. The ‘AUDPROC_PARAM_ID_DS1AP_VEN’ parameter must be 'on' to read this value. The excitations are scaled by 16. i.e. 16 = 1 dB.\n }
         @h2xmle_default       {0}
         @h2xmle_range    {-32768..32767} */

}
#include "adsp_end_pack.h"
;


/* ID to get the Visualizer Custom Band Gains of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS Module.*/
#define AUDPROC_PARAM_ID_DS1AP_VCBG   							0x00010730

/* Structure for the Visualizer Custom Band Gains parameter for a DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_vcbg_t audproc_param_id_ds1ap_vcbg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VCBG", AUDPROC_PARAM_ID_DS1AP_VCBG}
    @h2xmlp_description {Read only}
    @h2xmlp_toolPolicy   {NO_SUPPORT} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vcbg_t
{
    int32_t                 vis_custom_band_gain[20];
     /**< @h2xmle_description  { Contains the visualizer gains for the custom visualizer bands. The AUDPROC_PARAM_ID_DS1AP_VEN parameter must be 'on' to read this value.\n}
    @h2xmle_default       {0}
    @h2xmle_range    {-32768..32767} */

}
#include "adsp_end_pack.h"
;


/* ID to get the Visualizer Custom Band Excitations of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS module. */
#define AUDPROC_PARAM_ID_DS1AP_VCBE   							0x00010731

/* Structure for the Visualizer Custom Band Excitations parameter for a DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_vcbe_t audproc_param_id_ds1ap_vcbe_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VCBE", AUDPROC_PARAM_ID_DS1AP_VCBE}
    @h2xmlp_description {Read only}
    @h2xmlp_toolPolicy   {NO_SUPPORT} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vcbe_t
{
    int32_t                 vis_custom_band_excitation[20];
    /**< @h2xmle_description  { Contains the excitation data for the custom visualizer bands. The AUDPROC_PARAM_ID_DS1AP_VEN parameter must be 'on' to read this value.\n}
    @h2xmle_default       {0}
    @h2xmle_range    {-32768..32767} */

}
#include "adsp_end_pack.h"
;


/* ID for the Visualizer Enable of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS Module. */
#define AUDPROC_PARAM_ID_DS1AP_VEN    							0x00010732

/* Structure for the Visualizer Enable parameter for the DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_ven_t audproc_param_id_ds1ap_ven_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VEN", AUDPROC_PARAM_ID_DS1AP_VEN}
    @h2xmlp_description { Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_ven_t
{
    int32_t               vis_enable;
   /**< @h2xmle_description  { Toggles support for visualization }
        @h2xmle_default   {0}
	@h2xmle_rangeList {"Disable" = 0; "Enable" = 1} */

}
#include "adsp_end_pack.h"
;


/* ID for the Visualizer Custom Band Count of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS Module. */
#define AUDPROC_PARAM_ID_DS1AP_VCNB     						0x00010733


/* Structure for the Visualizer Custom Band Count parameter for the DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_vcnb_t audproc_param_id_ds1ap_vcnb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VCNB", AUDPROC_PARAM_ID_DS1AP_VCNB}
    @h2xmlp_description { Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vcnb_t
{
    uint32_t               vis_custom_band_count;
      /**< @h2xmle_description  { The number of custom visualizer frequency bands to which the native bands will be mapped to. This parameter specifies the number of center frequencies which will be supplied by the user. After this parameter is changed, the AUDPROC_PARAM_ID_DS1AP_VCBF parameter must be updated. Changes to this parameter will not take effect until AUDPROC_PARAM_ID_DS1AP_VEN is 'on'.\n}
        @h2xmle_default   {10}
	@h2xmle_range {1..20} */

}
#include "adsp_end_pack.h"
;


/* ID for the Visualizer Custom Band Center Frequencies of Dolby Surround1 used by AUDPROC_MODULE_ID_DS1AP_VIS Module.*/
#define AUDPROC_PARAM_ID_DS1AP_VCBF      						0x00010734

/* Structure for the Visualizer Custom Band Center Frequencies parameter for the DS1AP Visualizer module. */
typedef struct audproc_param_id_ds1ap_vcbf_t audproc_param_id_ds1ap_vcbf_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VCBF", AUDPROC_PARAM_ID_DS1AP_VCBF}
    @h2xmlp_description { Factory Configurable.}  */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vcbf_t
{
    uint32_t               vis_custom_band_freq[20];
    /**< @h2xmle_description  { The center frequencies of each custom visualizer band which the native band data will be interpolated onto. Changes to this parameter will not take effect until AUDPROC_PARAM_ID_DS1AP_VEN is 'on'.\n  Default : 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 Hertz.\n} */

}
#include "adsp_end_pack.h"
;



#define AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER  						0x0001074C


/** ID of the Dolby Headphone(DH) Enable used by AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_DS1AP_VDHE    								0x0001074D

/* Structure for the Dolby Headphone(DH) Enable parameter for the DS1AP Dolby Virtualizer module. */
typedef struct audproc_param_id_ds1ap_vdhe_t audproc_param_id_ds1ap_vdhe_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VDHE", AUDPROC_PARAM_ID_DS1AP_VDHE}
    @h2xmlp_description {User Configurable} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vdhe_t
{
    uint32_t                 dh_enable;
	/**< @h2xmle_description  {Specifies the state of Dolby Headphone(DH).\n}
         @h2xmle_default       {0}
        @h2xmle_rangeList     {"Disable" = 0;
		                       "Enable" = 1;
							   "Auto(Enable if no other virtualizer is enabled and endpoint is headphones.)" = 2} */
}
#include "adsp_end_pack.h"
;

/** ID of the Dolby Headphone(DH) Surround Boost used by AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_DS1AP_DHSB    								0x0001074E

/* Structure for the Dolby Headphone(DH) Surround Boost parameter for the DS1AP Dolby Virtualizer module.*/
typedef struct audproc_param_id_ds1ap_dhsb_t audproc_param_id_ds1ap_dhsb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DHSB", AUDPROC_PARAM_ID_DS1AP_DHSB}
    @h2xmlp_description {User Configurable} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dhsb_t
{
    uint32_t                 dh_surr_boost;
	/**< @h2xmle_description  {Specifies the amount of boost applied to the surround channels when Dolby Headphone is enabled as the Virtualizer, in 'dB'.\n
	Numerical Range: [0,96] (which maps to [0.00,6.00] dB) with 4bits fractional value.\n}
         @h2xmle_default       {6}
         @h2xmle_range    {0..6} */


}
#include "adsp_end_pack.h"
;



/** ID of the Dolby Headphone(DH) Reverb Gain used by AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_DS1AP_DHRG									0x0001074F

/* Structure for the Dolby Headphone(DH) Reverb Gain parameter for the DS1AP Dolby Virtualizer module. */
typedef struct audproc_param_id_ds1ap_dhrg_t audproc_param_id_ds1ap_dhrg_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DHRG", AUDPROC_PARAM_ID_DS1AP_DHRG}
    @h2xmlp_description {User Configurable} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dhrg_t
{
    int32_t                 dh_reverb_gain;
	/**< @h2xmle_description  {Specifies the Amount of reverberation to add to the signal when Dolby Headphone is enabled as the virtualizer in 'dB'.\n
	Numerical Range: [-2080,192] (which maps to [-130.00,12.00] dB) with 4bits fractional value.\n}
         @h2xmle_default       {6}
         @h2xmle_range    {0..6} */


}
#include "adsp_end_pack.h"
;


/** ID of the Dolby Virtual Speaker(DVS) Enable used by AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_DS1AP_VSPE      							0x00010750

/* Structure for the Dolby Virtual Speaker(DVS) Enable parameter for the DS1AP Dolby Virtualizer module.*/
typedef struct audproc_param_id_ds1ap_vspe_t audproc_param_id_ds1ap_vspe_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VSPE", AUDPROC_PARAM_ID_DS1AP_VSPE}
    @h2xmlp_description {User Configurable} */


#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vspe_t
{
    uint32_t                 vsp_enable;
	/**< @h2xmle_description  {Specifies the state of Virtual Speaker(DVSQ).\n}
         @h2xmle_default       {0}
		 @h2xmle_rangeList      {"Disable"=0; "Enable"=1; "Auto(Enable if no other virtualizer is enabled and endpoint is internal speakers)" = 2} */

}
#include "adsp_end_pack.h"
;

/** ID of the Dolby Virtual Speaker(DVS) Surround Boost used by AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER.*/
#define AUDPROC_PARAM_ID_DS1AP_DSSB      							0x00010751

/* Structure for the Dolby Virtual Speaker(DVS) Surround Boost parameter for the DS1AP Dolby Virtualizer module. */
typedef struct audproc_param_id_ds1ap_dssb_t audproc_param_id_ds1ap_dssb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DSSB", AUDPROC_PARAM_ID_DS1AP_DSSB}
    @h2xmlp_description {User Configurable} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dssb_t
{
    uint32_t                 vsp_surr_boost;
	/**< @h2xmle_description  {Specifies the Amount of boost to the surround channels when Dolby Virtual Speaker is enabled as the virtualizer in 'dB'.\n
	Numerical Range: [0,96] (which maps to [0.00,6.00] dB) with 4bits fractional value.\n}
         @h2xmle_default       {6}
         @h2xmle_range    {0..6} */

}
#include "adsp_end_pack.h"
;

/* ID of the Dolby Virtual Speaker(DVS) Speaker Angle used by AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_DS1AP_DSSA        							0x00010752

/* Structure for the Dolby Virtual Speaker(DVS) Speaker Angle parameter for the DS1AP Dolby Virtualizer module. */
typedef struct audproc_param_id_ds1ap_dssa_t audproc_param_id_ds1ap_dssa_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DSSA", AUDPROC_PARAM_ID_DS1AP_DSSA}
    @h2xmlp_description {User Configurable} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dssa_t
{
    uint32_t                 vsp_spkr_angle;
	/**< @h2xmle_description  {Informs Dolby Virtual speaker module, the physical angular separations of the loudspeakers, in 'degrees'.\n}
         @h2xmle_default       {10}
         @h2xmle_range    {5..30} */

}
#include "adsp_end_pack.h"
;

/* ID of the Dolby Virtual Speaker(DVS) Start Frequency used by AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_DS1AP_DSSF      							0x00010753

/* Structure for the Dolby Virtual Speaker(DVS) Start Frequency parameter for the DS1AP Dolby Virtualizer module. */
typedef struct audproc_param_id_ds1ap_dssf_t audproc_param_id_ds1ap_dssf_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_DSSF", AUDPROC_PARAM_ID_DS1AP_DSSF}
    @h2xmlp_description {User Configurable} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_dssf_t
{
    uint32_t                 start_freq;
    /**< @h2xmle_description  {Specifies the frequency in 'hertz' below which, the signal will not get virtualized. This is useful for improving bass response.\n}
         @h2xmle_default       {20}
         @h2xmle_range    {20..20000} */

}
#include "adsp_end_pack.h"
;

/* ID of the Dolby Virtual Surround Compressor Enable used by AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER. */
#define AUDPROC_PARAM_ID_DS1AP_SCPE									0x00010754

/* Structure for the Dolby Virtual Surround Compressor Enable parameter for the DS1AP Dolby Virtualizer module. */
typedef struct audproc_param_id_ds1ap_scpe_t audproc_param_id_ds1ap_scpe_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_SCPE", AUDPROC_PARAM_ID_DS1AP_SCPE}
    @h2xmlp_description {Specifies the state of Virtual Surround Compressor.} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_scpe_t
{
    uint32_t                 scpe_enable;
	/**< @h2xmle_description  { Specifies the state of Virtual Surround Compressor.\n
		 Surround Compressor provides up to 6dB of boost to the surround channels. \n
		 The amount of boost is given by the Dolby Virtual Speaker Surround Boost parameter when
		 Dolby Virtual Speaker is enabled and the Dolby Headphone Surround Boost parameter otherwise.\n}
        @h2xmle_default       (2)
        @h2xmle_rangeList     {"Disable" = 0;
		                       "Enable" = 1;
							   "Auto(Enabled whenever Dolby virtual speaker or Dolby headphone are enabled)" = 2} */

}
#include "adsp_end_pack.h"
;


#define AUDPROC_MODULE_ID_DS1AP_VM  					0x00010755


/* ID of the Volume Maximizer Enable used by the AUDPROC_MODULE_ID_DS1AP_VM . */
#define AUDPROC_PARAM_ID_DS1AP_VMON        				0x00010756

/* Structure for the Volume Maximizer Enable parameter for the DS1AP Volume Maximizer module. */
typedef struct audproc_param_id_ds1ap_vmon_t audproc_param_id_ds1ap_vmon_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VMON", AUDPROC_PARAM_ID_DS1AP_VMON}
    @h2xmlp_description {User Configurable} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vmon_t
{
    uint32_t                 vm_enable;
	/**< @h2xmle_description  {Specifies the state of Volume Maximizer.\n}
         @h2xmle_default       {2}
        @h2xmle_rangeList     {"Disable" = 0;
		                       "Enable" = 1;
							   "Auto(Enabled if Dolby Volume Leveler is enabled)" = 2} */


}
#include "adsp_end_pack.h"
;

/* ID of the Volume Maximizer Boost used by the AUDPROC_MODULE_ID_DS1AP_VM. */
#define AUDPROC_PARAM_ID_DS1AP_VMB        				0x00010757

/* Structure for the Volume Maximizer Boost parameter for the DS1AP Volume Maximizer module. */
typedef struct audproc_param_id_ds1ap_vmb_t audproc_param_id_ds1ap_vmb_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DS1AP_VMB", AUDPROC_PARAM_ID_DS1AP_VMB}
    @h2xmlp_description {User Configurable} */

#include "adsp_begin_pack.h"

struct audproc_param_id_ds1ap_vmb_t
{
    uint32_t                 vm_boost;
/**< @h2xmle_description  {Specifies the amount of signal boost to be applied by the Volume Maximizer, in 'dB'.\n
	Numerical Range: [0,240] (which maps to [0.00,15.00] dB) with 4 bits fractional value.\n}
         @h2xmle_default       {9}
         @h2xmle_range    {0..15} */

}
#include "adsp_end_pack.h"
;

#ifdef __cplusplus
}
#endif /*__cplusplus*/
/** @}                   <-- End of the Module -->*/
#endif // #ifdef DS1_CALIB_H



