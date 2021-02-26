#ifndef SRS_TRUMEDIA_CALIB_H
#define SRS_TRUMEDIA_CALIB_H
/*==============================================================================
  @file srs_trumedia_calib.h
  @brief This file contains SRS_TRUMEDIA API
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

/** @h2xml_title1           {SRS TRUMEDIA}
    @h2xml_title_agile_rev  {SRS TRUMEDIA}
    @h2xml_title_date       {March 7, 2016} */

#include "mmdefs.h"
#define APPI_SRS_TRUMEDIA_MODULE_ID                   0x10005010
/** @h2xmlm_module       {"APPI_SRS_TRUMEDIA_MODULE_ID",
                          APPI_SRS_TRUMEDIA_MODULE_ID}

    @h2xmlm_displayName  {"AUDIO SRS TRUMEDIA"}
	@h2xmlm_description	 {ID of the SRS TruMedia module.\n

    This module supports the following parameter IDs:\n
    - #APPI_SRS_TRUMEDIA_PARAMS\n
    - #APPI_SRS_TRUMEDIA_WOWHD_PARAMS\n
    - #APPI_SRS_TRUMEDIA_CSHP_PARAMS\n
    - #APPI_SRS_TRUMEDIA_HPF_PARAMS\n
    - #APPI_SRS_TRUMEDIA_AEQ_PARAMS\n
    - #APPI_SRS_TRUMEDIA_HL_PARAMS\n
    - #APPI_SRS_TRUMEDIA_GEQ_PARAMS\n}

    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->
 */


#define APPI_SRS_TRUMEDIA_PARAMS                      0x10005011

/* The following are bit fields for the  Enable_Flags
 * field in srs_TruMedia_params_t structure defined below*/

/* Any SRS processing can occur. This bit must be set if any of the
    other enable macros are to be used.
 */
#define SRS_ENABLE_GLOBAL    0x00000001

/** SRS WOW HD processing stage is active. */
#define SRS_ENABLE_WOWHD     0x00000002

/** CS Headphone (CSHP) processing stage is active. */
#define SRS_ENABLE_CSHP      0x00000004

/** High Pass Filter stage is active. */
#define SRS_ENABLE_HPF       0x00000008

/** Adaptive Equalizer stage is active. */
#define SRS_ENABLE_AEQ       0x00000010

/** Hard Limiter stage is active. */
#define SRS_ENABLE_HL        0x00000020

/** Graphic Equalizer stage is active. @newpage */
#define SRS_ENABLE_GEQ       0x00000040

typedef struct srs_TruMedia_params_intf_t srs_TruMedia_params_intf_t;
/** @h2xmlp_parameter   {"APPI_SRS_TRUMEDIA_PARAMS", APPI_SRS_TRUMEDIA_PARAMS}
    @h2xmlp_description {ID of the SRS TruMedia enable parameters used by
    APPI_SRS_TRUMEDIA_MODULE_ID.\n The following are bit fields for the  Enable_Flags field in srs_TruMedia_params_t structure defined below \n Any SRS processing can occur. This bit must be set if any of the
    other enable macros are to be used.\n
SRS_ENABLE_GLOBAL    0x00000001 \n

(SRS WOW HD processing stage is active.)
SRS_ENABLE_WOWHD     0x00000002 \n

(CS Headphone (CSHP) processing stage is active.)
SRS_ENABLE_CSHP      0x00000004 \n

(High Pass Filter stage is active.)
SRS_ENABLE_HPF       0x00000008 \n

(Adaptive Equalizer stage is active. )
SRS_ENABLE_AEQ       0x00000010 \n

(Hard Limiter stage is active.)
SRS_ENABLE_HL        0x00000020 \n

(Graphic Equalizer stage is active.)
SRS_ENABLE_GEQ       0x00000040 \n }
	*/

#include "adsp_begin_pack.h"


/* Payload of the APPI_SRS_TRUMEDIA_PARAMS parameters used by the SRS
 TruMedia module.
 */
struct srs_TruMedia_params_intf_t {
   uint8_t Enable_Flags;
   /**< @h2xmle_description  {Bitfield of macros used to enable SRS TruMedia.\nThe values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled.\n The bit corresponding to SRS_ENABLE_GLOBAL must be set if any of the
        other bits in the bit field are to be set.\n}
        @h2xmle_rangeList    {"SRS_ENABLE_GLOBAL"=SRS_ENABLE_GLOBAL;
                              "SRS_ENABLE_WOWHD"=SRS_ENABLE_WOWHD;
							  "SRS_ENABLE_CSHP"=SRS_ENABLE_CSHP;
							  "SRS_ENABLE_HPF"=SRS_ENABLE_HPF;
							  "SRS_ENABLE_AEQ"=SRS_ENABLE_AEQ;
							  "SRS_ENABLE_HL"=SRS_ENABLE_HL;
							  "SRS_ENABLE_GEQ"=SRS_ENABLE_GEQ}
		@h2xmle_default	{SRS_ENABLE_GLOBAL}      */

   uint8_t Reserved;
   /**< @h2xmle_description  {Clients must set this field to 0.}
        @h2xmle_range    {0..0}
        @h2xmle_default	{0}                        */

   uint8_t I_0;
   /**< @h2xmle_description  {SRS licensing payload used to verify the SRS Licensing scheme with the
        aDSP module.}
		@h2xmle_default	{0} */

   uint8_t V_0;
   /**< @h2xmle_description  {SRS licensing payload.}
   @h2xmle_default	{0} */

   uint8_t I_1;
   /**< @h2xmle_description  {SRS licensing payload.}
	@h2xmle_default	{0} */

   uint8_t V_1;
   /**< @h2xmle_description  {SRS licensing payload.}
	@h2xmle_default	{0} */


   uint8_t I_2;
   /**< @h2xmle_description  {SRS licensing payload.}
	@h2xmle_default	{0} */

   uint8_t V_2;
   /**< @h2xmle_description  {SRS licensing payload.}
	@h2xmle_default	{0} */

}
#include "adsp_end_pack.h"
;


typedef struct srs_TruMedia_params_t srs_TruMedia_params_t;

#include "adsp_begin_pack.h"

/* Payload of the APPI_SRS_TRUMEDIA_PARAMS parameters used by the SRS
 TruMedia module.
 */
struct srs_TruMedia_params_t {
   uint8_t Enable_Flags;


   uint8_t Reserved;


   uint8_t I_0;


   uint8_t V_0;



   uint8_t I_1;


   uint8_t V_1;


   uint8_t I_2;


   uint8_t V_2;


   uint16_t IGain;

}
#include "adsp_end_pack.h"
;


/* ID of the WOW HD configuration parameters used by
 */
#define APPI_SRS_TRUMEDIA_WOWHD_PARAMS                0x10005012

/* The following are bit fields for the EnableFlags field in
 * srs_TruMedia_wowhd_params_t defined below*/

/** DTS limiter is enabled and is specific to the WOW HD feature. */
#define SRS_TRUMEDIA_WOWHD_DOLIMIT          0x0001

/** DTS SRS 3D is enabled and is specific to the WOW HD feature. */
#define SRS_TRUMEDIA_WOWHD_DOSRS3D          0x0002

/** DTS TruBass is enabled and is specific to the WOW HD feature. */
#define SRS_TRUMEDIA_WOWHD_DOTB             0x0004

/** DTS TruBass Split Analysis is enabled and is specific to the WOW HD
    feature. */
#define SRS_TRUMEDIA_WOWHD_DOSATB           0x0008

/** DTS Focus is enabled and is specific to the WOW HD feature. */
#define SRS_TRUMEDIA_WOWHD_DOFOCUS          0x0010

/** DTS Definition is enabled and is specific to the WOW HD feature. */
#define SRS_TRUMEDIA_WOWHD_DODEF            0x0020

/** TruBass is stereo when set and mono when not set. */
#define SRS_TRUMEDIA_WOWHD_TBMODE           0x0040

/** Headphone mode for SRS 3D is active. */
#define SRS_TRUMEDIA_WOWHD_SRSTYPE          0x0080

/** Bitmask to use when defining SRS_TRUMEDIA_WOWHD_SRSMODE values.*/
#define SRS_TRUMEDIA_WOWHD_SRSMODE_MASK     0x0F00

/** SRS 3D mode is SRSSrs3dMono. */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_MONO     0x0100

/** SRS 3D mode is SRSSrs3dSingleSpeaker. */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_SINGLE   0x0200

/** SRS 3D mode is SRSSrs3dStereo. */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_STEREO   0x0300

/** SRS 3D mode is SRSSrs3dExtreme. */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_XTREME   0x0400

/** DTS High Pass Filter is enabled and is specific to the WOW HDX feature. */
#define SRS_TRUMEDIA_WOWHDX_DOHPF           0x1000

/** DTS Crossover is enabled and is specific to the WOW HDX feature. */
#define SRS_TRUMEDIA_WOWHDX_DOXOVER         0x2000

/** DTS SRS WideSurround is enabled and is specific to the WOW HDX
    feature. @newpage */
#define SRS_TRUMEDIA_WOWHDX_DOWDSRD         0x4000

typedef struct srs_TruMedia_wowhd_params_intf_t srs_TruMedia_wowhd_params_intf_t;
/** @h2xmlp_parameter   {"APPI_SRS_TRUMEDIA_WOWHD_PARAMS", APPI_SRS_TRUMEDIA_WOWHD_PARAMS}
    @h2xmlp_description {The following are bit fields for the EnableFlags field in srs_TruMedia_wowhd_params_t defined below\n
( DTS limiter is enabled and is specific to the WOW HD feature. )
SRS_TRUMEDIA_WOWHD_DOLIMIT-0x0001\n

( DTS SRS 3D is enabled and is specific to the WOW HD feature. )
SRS_TRUMEDIA_WOWHD_DOSRS3D-0x0002\n

( DTS TruBass is enabled and is specific to the WOW HD feature. )
SRS_TRUMEDIA_WOWHD_DOTB-0x0004\n

( DTS TruBass Split Analysis is enabled and is specific to the WOW HD
    feature. )
SRS_TRUMEDIA_WOWHD_DOSATB-0x0008\n

( DTS Focus is enabled and is specific to the WOW HD feature. )
SRS_TRUMEDIA_WOWHD_DOFOCUS-0x0010\n

( DTS Definition is enabled and is specific to the WOW HD feature. )
SRS_TRUMEDIA_WOWHD_DODEF-0x0020\n

( TruBass is stereo when set and mono when not set. )
SRS_TRUMEDIA_WOWHD_TBMODE-0x0040\n

( Headphone mode for SRS 3D is active. )
SRS_TRUMEDIA_WOWHD_SRSTYPE-0x0080\n

( Bitmask to use when defining SRS_TRUMEDIA_WOWHD_SRSMODE values.)
SRS_TRUMEDIA_WOWHD_SRSMODE_MASK-0x0F00\n

( SRS 3D mode is SRSSrs3dMono. )
SRS_TRUMEDIA_WOWHD_SRSMODE_MONO-0x0100\n

( SRS 3D mode is SRSSrs3dSingleSpeaker. )
SRS_TRUMEDIA_WOWHD_SRSMODE_SINGLE-0x0200\n

( SRS 3D mode is SRSSrs3dStereo. )
SRS_TRUMEDIA_WOWHD_SRSMODE_STEREO-0x0300\n

( SRS 3D mode is SRSSrs3dExtreme. )
SRS_TRUMEDIA_WOWHD_SRSMODE_XTREME-0x0400\n

( DTS High Pass Filter is enabled and is specific to the WOW HDX feature. )
SRS_TRUMEDIA_WOWHDX_DOHPF-0x1000\n

( DTS Crossover is enabled and is specific to the WOW HDX feature. )
SRS_TRUMEDIA_WOWHDX_DOXOVER-0x2000 \n

( DTS SRS WideSurround is enabled and is specific to the WOW HDX
    feature. )
SRS_TRUMEDIA_WOWHDX_DOWDSRD-0x4000 \n }
	*/
#include "adsp_begin_pack.h"

/* Payload of the APPI_SRS_TRUMEDIA_WOWHD_PARAMS used by the SRS TruMedia
 module.
 */
struct srs_TruMedia_wowhd_params_intf_t {
   uint32_t EnableFlags;
   /**< @h2xmle_description  {Bitfield of macros used to enable the SRS TruMedia WOW HD
        parameters.\nThe values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled.}
        @h2xmle_rangeList    {"SRS_TRUMEDIA_WOWHD_DOLIMIT"=SRS_TRUMEDIA_WOWHD_DOLIMIT;
                              "SRS_TRUMEDIA_WOWHD_DOSRS3D"=SRS_TRUMEDIA_WOWHD_DOSRS3D;
							  "SRS_TRUMEDIA_WOWHD_DOTB"=SRS_TRUMEDIA_WOWHD_DOTB;
							  "SRS_TRUMEDIA_WOWHD_DOSATB"=SRS_TRUMEDIA_WOWHD_DOSATB;
							  "SRS_TRUMEDIA_WOWHD_DOFOCUS"=SRS_TRUMEDIA_WOWHD_DOFOCUS;
							  "SRS_TRUMEDIA_WOWHD_DODEF"=SRS_TRUMEDIA_WOWHD_DODEF;
							  "SRS_TRUMEDIA_WOWHD_TBMODE"=SRS_TRUMEDIA_WOWHD_TBMODE;
							  "SRS_TRUMEDIA_WOWHD_SRSMODE_MASK"=SRS_TRUMEDIA_WOWHD_SRSMODE_MASK;
							  "SRS_TRUMEDIA_WOWHD_SRSMODE_MONO"=SRS_TRUMEDIA_WOWHD_SRSMODE_MONO;
							  "SRS_TRUMEDIA_WOWHD_SRSMODE_SINGLE"=SRS_TRUMEDIA_WOWHD_SRSMODE_SINGLE;
							  "SRS_TRUMEDIA_WOWHD_SRSMODE_STEREO"=SRS_TRUMEDIA_WOWHD_SRSMODE_STEREO;
							  "SRS_TRUMEDIA_WOWHD_SRSMODE_XTREME"=SRS_TRUMEDIA_WOWHD_SRSMODE_XTREME;
							  "SRS_TRUMEDIA_WOWHDX_DOHPF"=SRS_TRUMEDIA_WOWHDX_DOHPF;
							  "SRS_TRUMEDIA_WOWHDX_DOXOVER"=SRS_TRUMEDIA_WOWHDX_DOXOVER;
							  "SRS_TRUMEDIA_WOWHDX_DOWDSRD"=SRS_TRUMEDIA_WOWHDX_DOWDSRD}
		@h2xmle_default	{SRS_TRUMEDIA_WOWHD_DOLIMIT}      */

   uint16_t Gain_Input;
   /**< @h2xmle_description  {Gain (in Q15 format) applied to the signal before processing.}
	@h2xmle_default	{0x4CCD}
	@h2xmle_dataFormat {Q15} */


   uint16_t Gain_Output;
   /**< @h2xmle_description  { Gain (in Q15 format) applied to the signal after processing. }
	@h2xmle_default	{0x7FFF}
	@h2xmle_dataFormat {Q15} */

   uint16_t Gain_Bypass;
   /**< @h2xmle_description  {Currently not used.}
	@h2xmle_default	{0x7FFF} */


   uint16_t Focus_Level;
   /**< @h2xmle_description  {Level of focus effect (in Q15 format).}
	@h2xmle_default	{0x3333}
	@h2xmle_dataFormat {Q15} */


   uint16_t Def_Level;
   /**< @h2xmle_description  {Level of definition effect (in Q15 format). }
	@h2xmle_default	{0x7FFF}
	@h2xmle_dataFormat {Q15} */


   uint16_t Center_Level;
   /**< @h2xmle_description  {SRS 3D Center control (in Q15 format).}
	@h2xmle_default	{0x4000}
	@h2xmle_dataFormat {Q15} */


   uint16_t Space_Level;
   /**< @h2xmle_description  {SRS 3D Space control (in Q15 format).}
	@h2xmle_default	{0x6666}
	@h2xmle_dataFormat {Q15} */


   uint16_t Reserved;
   /**< @h2xmle_description  {This field must be set to 0. }
	@h2xmle_default	{0} */


   uint32_t Limit_Level;
   /**< @h2xmle_description  {Level of the limiter (in Q30 format).}
	@h2xmle_default	{0x10000000}
	@h2xmle_dataFormat {Q30} */


   uint16_t TruBass_Level;
   /**< @h2xmle_description  {Level of the TruBass effect (in Q15 format). }
	@h2xmle_default	{0x7FFF}
	@h2xmle_dataFormat {Q15} */


   uint16_t TruBass_Compress;
   /**< @h2xmle_description  {Level of the TruBass compressor (in Q15 format).}
	@h2xmle_default	{0x2000}
	@h2xmle_dataFormat {Q15} */


   uint32_t TruBass_Coefs[16];
   /**< @h2xmle_description  {Array of TruBass coefficients. }
        @h2xmle_default  {0x0345EA38} */

}
#include "adsp_end_pack.h"
;



typedef struct srs_TruMedia_wowhd_params_t srs_TruMedia_wowhd_params_t;

/* Payload of the APPI_SRS_TRUMEDIA_WOWHD_PARAMS used by the SRS TruMedia
 module.
 */
struct srs_TruMedia_wowhd_params_t {
   uint32_t EnableFlags;


   uint16_t Gain_Input;


   uint16_t Gain_Output;


   uint16_t Gain_Bypass;



   uint16_t Focus_Level;



   uint16_t Def_Level;


   uint16_t Center_Level;



   uint16_t Space_Level;

   uint16_t Reserved;


   uint32_t Limit_Level;


   uint16_t TruBass_Level;


   uint16_t TruBass_Compress;


   uint32_t TruBass_Coefs[16];


   uint32_t Xhp_Coefs[16];


   uint32_t Xlp_Coefs[16];


   uint32_t Tbhp_Coefs[16];


   uint32_t Tbhp_order;


   uint16_t Wdsrd_Center_Gain;


   uint16_t Wdsrd_Spk_Sep_Level;

}
#include "adsp_end_pack.h"
;

/* ID of the CS Headphone configuration parameters used by
 */
#define APPI_SRS_TRUMEDIA_CSHP_PARAMS                 0x10005013

/* The following are bit fields for the EnableFlags field in
   srs_TruMedia_cshp_params_t defined below   */

/** Limiter is enabled. */
#define SRS_TRUMEDIA_CSHP_DOLIMIT       0x0001

/** Dialog enhancement is enabled. */
#define SRS_TRUMEDIA_CSHP_DODIALOG      0x0002

/** TruBass is enabled. */
#define SRS_TRUMEDIA_CSHP_DOTB          0x0004

/** TruBass Split Analysis is active. */
#define SRS_TRUMEDIA_CSHP_DOSATB        0x0008

/** Circle Surround (CS) decoder is enabled. */
#define SRS_TRUMEDIA_CSHP_DODECODE      0x0010

/** CS decoding is active when set. Passive matrix decoding is active when not
    set. */
#define SRS_TRUMEDIA_CSHP_DECODEMODE    0x0020

/** Definition is enabled. */
#define SRS_TRUMEDIA_CSHP_DODEF         0x0040

/** TruBass is stereo when set, and mono when not set. */
#define SRS_TRUMEDIA_CSHP_TBMODE        0x0080

typedef struct srs_TruMedia_cshp_params_t srs_TruMedia_cshp_params_t;
/** @h2xmlp_parameter   {"APPI_SRS_TRUMEDIA_CSHP_PARAMS", APPI_SRS_TRUMEDIA_CSHP_PARAMS}
    @h2xmlp_description {The following are bit fields for the EnableFlags field in srs_TruMedia_cshp_params_t defined below\n
( Limiter is enabled. )
SRS_TRUMEDIA_CSHP_DOLIMIT       0x0001\n

( Dialog enhancement is enabled. )
SRS_TRUMEDIA_CSHP_DODIALOG      0x0002\n

( TruBass is enabled. )
SRS_TRUMEDIA_CSHP_DOTB          0x0004\n

( TruBass Split Analysis is active. )
SRS_TRUMEDIA_CSHP_DOSATB        0x0008\n

( Circle Surround (CS) decoder is enabled. )
SRS_TRUMEDIA_CSHP_DODECODE      0x0010\n

( CS decoding is active when set. Passive matrix decoding is active when not
    set. )
SRS_TRUMEDIA_CSHP_DECODEMODE    0x0020\n

( Definition is enabled. )
SRS_TRUMEDIA_CSHP_DODEF         0x0040\n

( TruBass is stereo when set, and mono when not set. )
	SRS_TRUMEDIA_CSHP_TBMODE        0x0080 \n} */
#include "adsp_begin_pack.h"

/* Payload of the APPI_SRS_TRUMEDIA_CSHP_PARAMS parameters used by
 the SRS TruMedia module.
 */
struct srs_TruMedia_cshp_params_t {
   uint32_t EnableFlags;
   /**< @h2xmle_description  {Bitfield of macros used to specify how SRS TruMedia CSHP is
        enabled.\nThe values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled.}
        @h2xmle_rangeList    {"SRS_TRUMEDIA_CSHP_DOLIMIT"=SRS_TRUMEDIA_CSHP_DOLIMIT;
                              "SRS_TRUMEDIA_CSHP_DODIALOG"=SRS_TRUMEDIA_CSHP_DODIALOG;
							  "SRS_TRUMEDIA_CSHP_DOTB"=SRS_TRUMEDIA_CSHP_DOTB;
							  "SRS_TRUMEDIA_CSHP_DOSATB"=SRS_TRUMEDIA_CSHP_DOSATB;
							  "SRS_TRUMEDIA_CSHP_DODECODE"=SRS_TRUMEDIA_CSHP_DODECODE;
							  "SRS_TRUMEDIA_CSHP_DECODEMODE"=SRS_TRUMEDIA_CSHP_DECODEMODE;
							  "SRS_TRUMEDIA_CSHP_DODEF"=SRS_TRUMEDIA_CSHP_DODEF;
							  "SRS_TRUMEDIA_CSHP_TBMODE"=SRS_TRUMEDIA_CSHP_TBMODE
							  }
		@h2xmle_default	{SRS_TRUMEDIA_CSHP_DOLIMIT}      */

   uint16_t Gain_Input;
   /**< @h2xmle_description  {Specifies the gain (in Q15 format) applied to the signal before
        processing.}
	@h2xmle_default	{0x4CCD}
	@h2xmle_dataFormat {Q15} */


   uint16_t Gain_Output;
   /**< @h2xmle_description  { Specifies the gain (in Q15 format) applied to the signal after
        processing.}
	@h2xmle_default	{0x7FFF}
	@h2xmle_dataFormat {Q15} */

   uint16_t Gain_Bypass;
   /**< @h2xmle_description  {Currently not used.}
	@h2xmle_default      {0x7FFF} */


   uint16_t Dialog_Level;
   /**< @h2xmle_description  {Specifies the level of dialog enhancement (in Q15 format). }
	@h2xmle_default	{0x3333}
	@h2xmle_dataFormat {Q15} */


   uint16_t Def_Level;
   /**< @h2xmle_description  {Specifies the level of definition effect (in Q15 format).}
	@h2xmle_default	{0x7FFF}
	@h2xmle_dataFormat {Q15} */


   uint16_t Reserved;
   /**< @h2xmle_description  {This field must be set to 0. }
	@h2xmle_default	{0}
	*/


   uint32_t Limit_Level;
   /**< @h2xmle_description  {Specifies the limiter's limit (in Q30 format).}
	@h2xmle_default	{0x10000000}
	@h2xmle_dataFormat {Q30} */


   uint16_t TruBass_Level;
   /**< @h2xmle_description  {Specifies the level of TruBass effect (in Q15 format).}
	@h2xmle_default	{0x7FFF}
	@h2xmle_dataFormat {Q15} */


   uint16_t TruBass_Compress;
   /**< @h2xmle_description  {Specifies the TruBass compressor's level (in Q15 format). }
	@h2xmle_default	{0x2000}
	@h2xmle_dataFormat {Q15} */


   uint32_t TruBass_Coefs[16];
   /**< @h2xmle_description  {TruBass coefficients. }
        @h2xmle_default	{0x0345EA38} */
}
#include "adsp_end_pack.h"
;


/* ID of the high pass filter configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.
 */
#define APPI_SRS_TRUMEDIA_HPF_PARAMS                  0x10005014

typedef struct srs_TruMedia_hpf_params_t srs_TruMedia_hpf_params_t;
/** @h2xmlp_parameter   {"APPI_SRS_TRUMEDIA_HPF_PARAMS", APPI_SRS_TRUMEDIA_HPF_PARAMS}
    @h2xmlp_description {ID of the high pass filter configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.\n Payload of the APPI_SRS_TRUMEDIA_HPF_PARAMS parameters used by the
 SRS TruMedia module.} */
#include "adsp_begin_pack.h"

/* Payload of the APPI_SRS_TRUMEDIA_HPF_PARAMS parameters used by the
 SRS TruMedia module.
 */
struct srs_TruMedia_hpf_params_t {
   uint32_t Order;
   /**< @h2xmle_description  {Order of the filter.}
	@h2xmle_default	{0x04}   */

   uint32_t Coefs[26];
   /**< @h2xmle_description  {High pass filter coefficients.}
        @h2xmle_default      {0x02}*/
}
#include "adsp_end_pack.h"
;


/* ID of the adaptive equalizer (AEQ) configuration parameters used by
     #APPI_SRS_TRUMEDIA_MODULE_ID.
 */
#define APPI_SRS_TRUMEDIA_AEQ_PARAMS                  0x10005015

/* The following are bit fields for the EnableFlags field in
 * srs_TruMedia_aeq_params_t defined below*/

/** Left/right link channel is enabled. Regardless of the right channel
    values, the left channel values are copied to the right channel. */
#define SRS_TRUMEDIA_AEQ_LR_LINK  0x00000001

/** Left channel is enabled (Do Left == Enable left). */
#define SRS_TRUMEDIA_AEQ_DOL      0x00000002

/** Right channel is enabled (Do Right == Enable right). */
#define SRS_TRUMEDIA_AEQ_DOR      0x00000004

/** Left channel (band 0) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB0      0x00000010

/** Left channel (band 1) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB1      0x00000020

/** Left channel (band 2) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB2      0x00000040

/** Left channel (band 3) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB3      0x00000080

/** Left channel (band 4) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB4      0x00000100

/** Left channel (band 5) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB5      0x00000200

/** Left channel (band 6) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB6      0x00000400

/** Left channel (band 7) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB7      0x00000800

/** Left channel (band 8) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB8      0x00001000

/** Left channel (band 9) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB9      0x00002000

/** Left channel (band 10) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB10     0x00004000

/** Left channel (band 11) is enabled. */
#define SRS_TRUMEDIA_AEQ_LB11     0x00008000

/** Right channel (band 0) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB0      0x00010000

/** Right channel (band 1) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB1      0x00020000

/** Right channel (band 2) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB2      0x00040000

/** Right channel (band 3) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB3      0x00080000

/** Right channel (band 4) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB4      0x00100000

/** Right channel (band 5) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB5      0x00200000

/** Right channel (band 6) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB6      0x00400000

/** Right channel (band 7) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB7      0x00800000

/** Right channel (band 8) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB8      0x01000000

/** Right channel (band 9) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB9      0x02000000

/** Right channel (band 10) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB10     0x04000000

/** Right channel (band 11) is enabled. */
#define SRS_TRUMEDIA_AEQ_RB11     0x08000000

/** Dolby FIR filter is enabled. */
#define SRS_TRUMEDIA_AEQ_DOFIR      0x10000000

/** Select between inbuilt (the table in the code) or a coefficient read from
    a file. */
#define SRS_TRUMEDIA_AEQ_FIR_SELECT 0x20000000

typedef struct srs_TruMedia_aeq_params_intf_t srs_TruMedia_aeq_params_intf_t;
/** @h2xmlp_parameter   {"APPI_SRS_TRUMEDIA_AEQ_PARAMS", APPI_SRS_TRUMEDIA_AEQ_PARAMS}
    @h2xmlp_description {The following are bit fields for the EnableFlags field in srs_TruMedia_aeq_params_t defined below\n
	( Left/right link channel is enabled. Regardless of the right channel
    values, the left channel values are copied to the right channel. )
 SRS_TRUMEDIA_AEQ_LR_LINK  0x00000001\n

( Left channel is enabled (Do Left == Enable left). )
 SRS_TRUMEDIA_AEQ_DOL      0x00000002\n

( Right channel is enabled (Do Right == Enable right). )
 SRS_TRUMEDIA_AEQ_DOR      0x00000004\n

( Left channel (band 0) is enabled. )
 SRS_TRUMEDIA_AEQ_LB0      0x00000010\n

( Left channel (band 1) is enabled. )
 SRS_TRUMEDIA_AEQ_LB1      0x00000020\n

( Left channel (band 2) is enabled. )
 SRS_TRUMEDIA_AEQ_LB2      0x00000040\n

( Left channel (band 3) is enabled. )
 SRS_TRUMEDIA_AEQ_LB3      0x00000080\n

( Left channel (band 4) is enabled. )
 SRS_TRUMEDIA_AEQ_LB4      0x00000100\n

( Left channel (band 5) is enabled. )
 SRS_TRUMEDIA_AEQ_LB5      0x00000200\n

( Left channel (band 6) is enabled. )
 SRS_TRUMEDIA_AEQ_LB6      0x00000400\n

( Left channel (band 7) is enabled. )
 SRS_TRUMEDIA_AEQ_LB7      0x00000800\n

( Left channel (band 8) is enabled. )
 SRS_TRUMEDIA_AEQ_LB8      0x00001000\n

( Left channel (band 9) is enabled. )
 SRS_TRUMEDIA_AEQ_LB9      0x00002000\n

( Left channel (band 10) is enabled. )
 SRS_TRUMEDIA_AEQ_LB10     0x00004000\n

( Left channel (band 11) is enabled. )
 SRS_TRUMEDIA_AEQ_LB11     0x00008000\n

( Right channel (band 0) is enabled. )
 SRS_TRUMEDIA_AEQ_RB0      0x00010000\n

( Right channel (band 1) is enabled. )
 SRS_TRUMEDIA_AEQ_RB1      0x00020000\n

( Right channel (band 2) is enabled. )
 SRS_TRUMEDIA_AEQ_RB2      0x00040000\n

( Right channel (band 3) is enabled. )
 SRS_TRUMEDIA_AEQ_RB3      0x00080000\n

( Right channel (band 4) is enabled. )
 SRS_TRUMEDIA_AEQ_RB4      0x00100000\n

( Right channel (band 5) is enabled. )
 SRS_TRUMEDIA_AEQ_RB5      0x00200000\n

( Right channel (band 6) is enabled. )
 SRS_TRUMEDIA_AEQ_RB6      0x00400000\n

( Right channel (band 7) is enabled. )
 SRS_TRUMEDIA_AEQ_RB7      0x00800000\n

( Right channel (band 8) is enabled. )
 SRS_TRUMEDIA_AEQ_RB8      0x01000000\n

( Right channel (band 9) is enabled. )
 SRS_TRUMEDIA_AEQ_RB9      0x02000000\n

( Right channel (band 10) is enabled. )
 SRS_TRUMEDIA_AEQ_RB10     0x04000000\n

( Right channel (band 11) is enabled. )
 SRS_TRUMEDIA_AEQ_RB11     0x08000000\n

( Dolby FIR filter is enabled. )
 SRS_TRUMEDIA_AEQ_DOFIR      0x10000000\n

( Select between inbuilt (the table in the code) or a coefficient read from
    a file. )
 SRS_TRUMEDIA_AEQ_FIR_SELECT 0x20000000\n } */

#include "adsp_begin_pack.h"

/* Payload of the APPI_SRS_TRUMEDIA_AEQ_PARAMS parameters used by the
 SRS TruMedia module.
 */
struct srs_TruMedia_aeq_params_intf_t {
   uint32_t EnableFlags;
   /**< @h2xmle_description  {Bitfield of macros used to enable the SRS TruMedia AEQ parameters.\nThe values are a bitwise OR of any of these macros. When a bit that
        corresponds to a particular macro is set, the processing block
        associated with that macro is enabled.}
        @h2xmle_rangeList    {"SRS_TRUMEDIA_AEQ_LR_LINK"=SRS_TRUMEDIA_AEQ_LR_LINK;
                              "SRS_TRUMEDIA_AEQ_DOL"=SRS_TRUMEDIA_AEQ_DOL;
							  "SRS_TRUMEDIA_AEQ_DOR"=SRS_TRUMEDIA_AEQ_DOR;
							  "SRS_TRUMEDIA_AEQ_LB0 through"=SRS_TRUMEDIA_AEQ_LB0;
							  "SRS_TRUMEDIA_AEQ_LB11"=SRS_TRUMEDIA_AEQ_LB11;
							  "SRS_TRUMEDIA_AEQ_RB0"=SRS_TRUMEDIA_AEQ_RB0;
							  "SRS_TRUMEDIA_AEQ_RB11"=SRS_TRUMEDIA_AEQ_RB11;
							  "SRS_TRUMEDIA_AEQ_DOFIR"=SRS_TRUMEDIA_AEQ_DOFIR;
							  "SRS_TRUMEDIA_AEQ_FIR_SELECT"=SRS_TRUMEDIA_AEQ_FIR_SELECT
							  }
		@h2xmle_default	{SRS_TRUMEDIA_AEQ_LR_LINK}      */

   uint16_t Gain_Input;
   /**< @h2xmle_description  {Specifies the gain (in Q12 format) applied to the signal before
        processing.}
	@h2xmle_default	{0x1000}
	@h2xmle_dataFormat {Q12} */


   uint16_t Gain_Output;
   /**< @h2xmle_description  {Specifies the gain (in Q12 format) applied to the signal after
        processing.}
	@h2xmle_default	{0x1000}
	@h2xmle_dataFormat {Q12} */


   uint16_t Gain_Bypass;
   /**< @h2xmle_description  {Currently not used. }
	@h2xmle_default   	{0x1000}  */


   uint16_t Reserved;
   /**< @h2xmle_description  {This field must be set to 0.}
	@h2xmle_default	{0}
	*/


   uint32_t L_Coefs[26];
   /**< @h2xmle_description  {Left-channel AEQ coefficients (6*(nBands)+2) -- DO NOT MODIFY.}
        @h2xmle_default                   {0x02} */


   uint32_t R_Coefs[26];
   /**< @h2xmle_description  {Right-channel AEQ coefficients (6*(nBands)+2) -- DO NOT MODIFY.}
         @h2xmle_default                   {0x02} */



}
#include "adsp_end_pack.h"
;


typedef struct srs_TruMedia_aeq_params_t srs_TruMedia_aeq_params_t;


#include "adsp_begin_pack.h"

/* Payload of the APPI_SRS_TRUMEDIA_AEQ_PARAMS parameters used by the
 SRS TruMedia module.
 */
struct srs_TruMedia_aeq_params_t {
   uint32_t EnableFlags;

   uint16_t Gain_Input;


   uint16_t Gain_Output;


   uint16_t Gain_Bypass;


   uint16_t Reserved;

   uint32_t L_Coefs[74];


   uint32_t R_Coefs[74];


   uint16_t FIR_Coefs[2048];


}
#include "adsp_end_pack.h"
;


/* ID of the hard limiter configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.
 */
#define APPI_SRS_TRUMEDIA_HL_PARAMS                   0x10005016

typedef struct srs_TruMedia_hl_params_t srs_TruMedia_hl_params_t;
/** @h2xmlp_parameter   {"APPI_SRS_TRUMEDIA_HL_PARAMS", APPI_SRS_TRUMEDIA_HL_PARAMS}
    @h2xmlp_description {Payload of the APPI_SRS_TRUMEDIA_HL_PARAMS parameters used by the
 SRS TruMedia module.} */
#include "adsp_begin_pack.h"


/* Payload of the APPI_SRS_TRUMEDIA_HL_PARAMS parameters used by the
 SRS TruMedia module.
 */
struct srs_TruMedia_hl_params_t {
   uint16_t Gain_Input;
   /**< @h2xmle_description  {Specifies the gain (in Q13 format) applied to the signal before
        processing.}
	@h2xmle_default	{0x2000}
	@h2xmle_dataFormat {Q13} */


   uint16_t Gain_Output;
   /**< @h2xmle_description  {Specifies the gain (in Q13 format) applied to the signal after
        processing.}
	@h2xmle_default	{0x2000}
	@h2xmle_dataFormat {Q13} */


   uint16_t Gain_Bypass;
   /**< @h2xmle_description  {Currently not used.}
	@h2xmle_default	{0x7FFF} */


   uint16_t Reserved_1;
   /**< @h2xmle_description  {This field must be set to 0.}
	@h2xmle_default	{0} */


   int32_t Delay_Len;
   /**< @h2xmle_description  { Number of samples in the delay buffer.}
	@h2xmle_default	{0x16} */


   uint32_t Boost;
   /**< @h2xmle_description  {Specifies the gain (in Q23 format) applied to the signal before
        limiting.}
	@h2xmle_default	{0x01400000}
	@h2xmle_dataFormat {Q23} */

   uint16_t Limit;
   /**< @h2xmle_description  {Specifies the limiter's limit (in Q15 format).}
	@h2xmle_default	{0x7FFF}
	@h2xmle_dataFormat {Q15}*/


   uint16_t Reserved_2;
   /**< @h2xmle_description  {This field must be set to 0. }
	@h2xmle_default	{0}
	*/


   uint32_t DecaySmooth;
   /**< @h2xmle_description  {When nonzero, decay smoothing is enabled. }
	@h2xmle_default	{0x01} */

}
#include "adsp_end_pack.h"
;


/* ID of the Graphic Equalizer configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.

    Do not fill this structure directly. An SRS Library at the operating system
    level generates this structure.

 */
#define APPI_SRS_TRUMEDIA_GEQ_PARAMS                  0x10005017

typedef struct srs_TruMedia_geq_params_t srs_TruMedia_geq_params_t;
/** @h2xmlp_parameter   {"APPI_SRS_TRUMEDIA_GEQ_PARAMS", APPI_SRS_TRUMEDIA_GEQ_PARAMS}
    @h2xmlp_description {ID of the Graphic Equalizer configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.\nDo not fill this structure directly. An SRS Library at the operating system
    level generates this structure.}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"


struct srs_TruMedia_geq_params_t{

   int16_t UserGains[10];
   /**< @h2xmle_description  {Gain for the left channel -- DO NOT MODIFY.} */
}
#include "adsp_end_pack.h"
;
/** @}                   <-- End of the Module -->*/


#endif
