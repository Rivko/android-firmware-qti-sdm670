#ifndef MIC2AMB_CALIB_H
#define MIC2AMB_CALIB_H
/*==============================================================================
  @file mic2amb_calib.h
  @brief This file contains MIC2AMB API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/ 
/*========================================================================
 Edit History

 when        who        what, where, why
 --------    ---     -------------------------------------------------------
 08/02/2018  snaginen     Initial .
 ========================================================================== */

#include "mmdefs.h"

/** @h2xml_title1           {Microphone Input to Ambisonic Conversion Module(MIC2AMB)}
    @h2xml_title_agile_rev  {MIC2AMB}
    @h2xml_title_date       {February 8th, 2018} */

/**
   @h2xmlx_xmlNumberFormat {int}
*/

#define MODULE_ID_MIC2AMB                                  0x00010949

/** 
    @h2xmlm_module       {"MODULE_ID_MIC2AMB",
                           MODULE_ID_MIC2AMB}
    @h2xmlm_displayName  {"MIC2AMB"}    
    @h2xmlm_description  {Microphone Input to Ambisonic Conversion Module.\n
     This module supports the following parameter IDs:\n
       - #PARAM_ID_MIC2AMB_ENABLE\n
       - #PARAM_ID_MIC2AMB_CONFIG\n
       - #AUDPROC_PARAM_ID_ENABLE\n 
       - #AUDPROC_PARAM_ID_LIB_VERSION\n }
    @h2xmlm_toolPolicy   {Calibration; RTC}
    @{                   <-- Start of the Module -->*/

/*  ID of the MIC2AMB Enable parameter used by MODULE_ID_MIC2AMB.*/
#define PARAM_ID_MIC2AMB_ENABLE                            0x00010950

/*  Structure for the enable parameter of MIC2AMB module. */
typedef struct mic2amb_param_id_enable_t mic2amb_param_id_enable_t;
/** @h2xmlp_parameter   {"PARAM_ID_MIC2AMB_ENABLE", PARAM_ID_MIC2AMB_ENABLE} 
    @h2xmlp_description {Enables the MIC2AMB module}
    @h2xmlp_toolPolicy  {Calibration; RTC} */

#include "adsp_begin_pack.h"

/*  Payload of the PARAM_ID_MIC2AMB_ENABLE parameter used by the
 MIC2AMB module.
 */
struct mic2amb_param_id_enable_t
{
  
  uint32_t enable_flag;
   /**< @h2xmle_description {Enable/Disable the MIC2AMB module}
        @h2xmle_rangeList   {"Disable"=0;
                             "Enable"=1}  
        @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

/*  ID of the MIC2AMB configuration parameter used by MODULE_ID_MIC2AMB.*/
#define PARAM_ID_MIC2AMB_CONFIG                            0x00010951

/*  Structure for the configuration parameters of MIC2AMB module. */
typedef struct mic2amb_param_id_config_t mic2amb_param_id_config_t;
/** @h2xmlp_parameter   {"PARAM_ID_MIC2AMB_CONFIG", PARAM_ID_MIC2AMB_CONFIG} 
    @h2xmlp_description {Configures the MIC2AMB module}
    @h2xmlp_toolPolicy  {Calibration; RTC} */

#include "adsp_begin_pack.h"

/*  Payload of the PARAM_ID_MIC2AMB_CONFIG parameter used by the
 MIC2AMB module.
 */
struct mic2amb_param_id_config_t
{
      
  uint32_t fft_size;
   /**< @h2xmle_description {Specifies the length of FFT}
        @h2xmle_rangeList   {"512" = 512}  
        @h2xmle_default     {512}  */

  uint32_t compensation_order;
   /**< @h2xmle_description {Specifies the compensation order}
        @h2xmle_range       {1..4}  
        @h2xmle_default     {4}  */

  uint32_t ambisonic_order;
   /**< @h2xmle_description {Specifies the ambisonic order of output data}
        @h2xmle_rangeList   {"MIC2AMB_AMBISONIC_ORDER_1ST"=1}  
        @h2xmle_default     {1}  */
        
  uint32_t norm_type;
   /**< @h2xmle_description {Specifies the normalization type of output data}
        @h2xmle_rangeList   {"MIC2AMB_NORM_TYPE_SN3D"=0}  
        @h2xmle_default     {0}  */

  uint32_t channel_order_type;
   /**< @h2xmle_description {Specifies the channel order type of output data}
        @h2xmle_rangeList   {"MIC2AMB_CHANNEL_ORDER_TYPE_ACN"=0}  
        @h2xmle_default     {0}  */        

  uint32_t noise_order;
  /**< @h2xmle_description  {Specifies the order of noise gain}
       @h2xmle_rangeList    {"1st-order"=1}
       @h2xmle_default      {1} */
      
  uint32_t dim_inp_ch;
  /**< @h2xmle_description  {Specifies number of microphone input channels in MIC2AMB processing}
       @h2xmle_rangeList    {"4" = 4}
       @h2xmle_default      {4} */

  int32_t noise_gain_buf[0];
  /**< @h2xmle_description  {Array of noise gain 2D buffer of dimension [x][y]
                             where x=(noise_order +1), y=(fft_size/2+1);
                             array contains linear gain of int32_t data(one word)in Q28,
                             Size of array is dynamic and it's equivalent to x*y words}
       @h2xmle_dataFormat  {Q28}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_variableArraySize {"(noise_order +1)*(fft_size/2+1)"}
       @h2xmle_elementType {rawData}
       @h2xmle_displayType {file} 
       @h2xmle_defaultFile {mic2amb_noise_gain_default.bin}  */  

  int32_t mixing_gain_buf[0];
  /**< @h2xmle_description  {Array of mixing gain matrix 3D buffer of dimension [x][y][z]
                            where x=compensation_order, y= (ambisonic_order+1)^2, z=dim_inp_ch;
                            Array contains linear gain of int32_t data(one word) in Q28,
                            Size of array is dynamic and it's equivalent to x*y*z words}
       @h2xmle_dataFormat  {Q28}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_variableArraySize {"compensation_order*(ambisonic_order+1)*(ambisonic_order+1)*dim_inp_ch"}
       @h2xmle_elementType {rawData}
       @h2xmle_displayType {file}
       @h2xmle_defaultFile {mic2amb_mixing_gain_default.bin}  */
       
  int32_t filter_coeffs_buf[0];
  /**< @h2xmle_description  {Array of filter coefficients 3D buffer of dimension [x][y][z]
                            where x=compensation_order, y= (ambisonic_order+1)^2, z=(fft_size/2+1);
                            Array contains complex filter coefficients of cint2x32(int64_t)in Q28,
                            Size of array is dynamic and it's equivalent to 2*x*y*z words}
       @h2xmle_dataFormat  {Q28}
       @h2xmle_range       {-2147483648..2147483647}
       @h2xmle_variableArraySize {"2*compensation_order*(ambisonic_order+1)*(ambisonic_order+1)*(fft_size/2+1)"}       
       @h2xmle_elementType {rawData}
       @h2xmle_displayType {file}
       @h2xmle_defaultFile {mic2amb_filter_coeffs_default.bin}   */
}
#include "adsp_end_pack.h"
;


/** @}                   <-- End of the Module -->*/

#endif
