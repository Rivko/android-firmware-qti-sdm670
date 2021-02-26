/*===========================================================================
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  =========================================================================== */
#ifndef DTS_HPX_CALIB_H
#define DTS_HPX_CALIB_H

/**
   @file dts_hpx_calib.h

   @brief This file contains the api definitions for dts headphone X
   premix and postmix modules
*/

/*===========================================================================
Edit History
when         who       what, where, why
----------   ---       ------------------------------------------------------
2013/10/22   kgodara   DTS eagle module and static topo ids
2016/08/17   hbansal   Moved file to different location.
2017/03/27   pag	   Added h2xml comments.
============================================================================= */

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/* DTS postmix COPP Topology ID. The client can route multiple ASM sessions
    to this COPP with Matrix operating in Native mode.
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DTS_HPX                                               0X00010347


/*
 * DTS postmix module id. Configuration of DTS postmix module must be done
 * by dts configurator and workspace test tool and should not be attempted
 * by anything else.
 *
 *     Topology Recommendation:
 *      Guideline: The media format converter module (AUDPROC_MODULE_ID_MFC,
 *         0x10912) should be placed after the HPX postmix module.
 *      Reason: The HPX postmix module converts data to 48 kHz stereo. If
 *         the end device is not 48 kHz stereo, the MFC module is needed to
 *         convert it to the device format.
 *      Mandatory/Recommended: Recommended
*/
#define AUDPROC_MODULE_ID_DTS_HPX_POSTMIX                                                   0X0001077B
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_DTS_HPX_POSTMIX",
                          AUDPROC_MODULE_ID_DTS_HPX_POSTMIX}
    @h2xmlm_description  {DTS postmix module id. Configuration of DTS postmix module must be done by dts configurator and workspace test tool and should not be attempted by anything else.\n
    Topology Recommendation:
Guideline: The media format converter module (AUDPROC_MODULE_ID_MFC,0x10912) should be placed after the HPX postmix module.\n
Reason: The HPX postmix module converts data to 48 kHz stereo. If the end device is not 48 kHz stereo, the MFC module is needed to convert it to the device format.\n
Mandatory/Recommended: Recommended\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/** @}                   <-- End of the Module -->      */

/* DTS premix popp topology id. This topology id can be used to open popp for
    premix module of dts headphone X.
    This topology has following modules in it
     - Resampler                   (#AUDPROC_MODULE_ID_RESAMPLER)
     - DTS_HPX_premix              (#AUDPROC_MODULE_ID_DTS_HPX_PREMIX)
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_DTS_HPX_PREMIX                                          0X00010DED


/*
 * DTS premix module id. Configuration of DTS premix module must be done
 * by dts configurator and workspace test tool and should not be attempted
 * by anything else.
*/
#define AUDPROC_MODULE_ID_DTS_HPX_PREMIX                                                    0X0001077C
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_DTS_HPX_PREMIX",
                          AUDPROC_MODULE_ID_DTS_HPX_PREMIX}
    @h2xmlm_description  {DTS premix module id.\n Configuration of DTS premix module must be done by dts configurator and workspace test tool and should not be attempted by anything else.\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/** @}                   <-- End of the Module -->      */


/*
 * Library Description strings for DTS HPX library used by DTS Eagle
 * Driver to show description of current eagle library in ADSP. This
 * api will be called by ADSP Core services.
 */
#define AUDPROC_PARAM_ID_DTSHPX_DTS_PARAM_LIBRARY_DESCRIPTION_STRING                        0X1001514C
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DTSHPX_DTS_PARAM_LIBRARY_DESCRIPTION_STRING",AUDPROC_PARAM_ID_DTSHPX_DTS_PARAM_LIBRARY_DESCRIPTION_STRING}
@h2xmlp_description	 {Library Description strings for DTS HPX library used by DTS Eagle Driver to show description of current eagle library in ADSP. This api will be called by ADSP Core services.}
*/
/*
 * Library version strings for DTS HPX library used by DTS Eagle
 * Driver to show version number of current eagle library in ADSP. This
 * api will be called by ADSP Core services.
 */
#define AUDPROC_PARAM_ID_DTSHPX_DTS_PARAM_LIBRARY_VERSION_STRING                            0X1001514D
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DTSHPX_DTS_PARAM_LIBRARY_VERSION_STRING",AUDPROC_PARAM_ID_DTSHPX_DTS_PARAM_LIBRARY_VERSION_STRING}
@h2xmlp_description	 {Library version strings for DTS HPX library used by DTS Eagle Driver to show version number of current eagle library in ADSP. This api will be called by ADSP Core services.}
*/

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* DTS_HPX_CALIB_H */

