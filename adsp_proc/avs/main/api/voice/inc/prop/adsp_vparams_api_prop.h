#ifndef __ADSP_VPARAMS_API_PROP_H__
#define __ADSP_VPARAMS_API_PROP_H__

/**
  @file adsp_vparams_api_prop.h
  @brief This file contains module and parameter IDs for calibration purposes.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Voice_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2010, 2012-2013, 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/16   HB       Renaming of file.
07/21/16   sw      (Tech Pubs) Merged Doxygen comments from AVS.2.7; edited
                   Doxygen comments for AVS.2.8.
02/25/13   sw      (Tech Pubs) Updated Doxygen for 2.0 internal draft.
02/27/12   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/11/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
===========================================================================*/
#include "aig_calib.h"
#include "avcrve_calib.h"
#include "beamr_calib.h"
#include "dmecns_calib.h"
#include "drc_calib.h"
#include "dtmf_detection_calib.h"
#include "enc_cng_calib.h"
#include "fir_calib.h"
#include "fluencev5_broadside_calib.h"
#include "fluence_prov2_calib.h"
#include "fns_calib.h"
#include "fv5ecns_dm_calib.h"
#include "fv5ecns_sm_calib.h"
#include "iir_calib.h"
#include "mbdrc_calib.h"
#include "smecns_calib.h"
#include "smecns_v2_calib.h"
#include "st_calib.h"
#include "vpecns_calib.h"
#include "wv_calib.h"
#include "wv_v2_calib.h"
#include "pbe_calib.h"



#define VOICE_PARAM_VP3                    (0x00010E03)


/** Reads the size of the VP3 data.

    This parameter is a multiple of 4 bytes, which is defined by the voice
    processor Tx to maintain parameter alignment.

    @vertspace{6} @inputtable{Cal_ID_0x00010E1A.tex} @newpage
*/
#define VOICE_PARAM_VP3_SIZE               (0x00010E1A)

/** Echo canceller Rx algorithm
  Fixed size for this parameter:
  - Version 0 -- 20 bytes

  @vertspace{6} @inputtable{Cal_ID_0x00010E0D.tex} @newpage
  */
#define VOICE_PARAM_ECRX                   (0x00010E0D)

/** Sound Focus algorithm for ECNS.

    The parameters listed in this structure correspond to how a user
    configures the sound focus application. This parameter is read/write
    (SET_PARAM and GET_PARAM commands are supported).

    This parameter ID applies only to #VOICE_MODULE_FLUENCE_PROV2 and
    #VOICE_MODULE_FLUENCEV5_BROADSIDE. This parameter ID can only be set or
    gotten (GET_PARAM) on topologies that contain these modules.

    More than one topology supports this parameter, and the client might not
    know the ECNS module ID. Therefore, the generic module ID,
    #VOICEPROC_MODULE_TX, supports this parameter. The VPM ensures
    that the payload is forwarded to the ECNS module in the chain.

    Upon a query (GET_PARAM), the data returned is consistent with what the
    algorithm is using internally.

    Maximum array size for this parameter:
    - Version 0 -- 28 bytes

    @vertspace{6} @inputtable{Cal_ID_0x00010E37.tex} @newpage
*/
#define VOICE_PARAM_FLUENCE_SOUNDFOCUS (0x00010E37)

/** Source tracking algorithm for ECNS.

    Based on the sector configuration the user sets, Fluence populates this
    structure and makes it available once every 20 ms. This parameter is
    read only (only the GET_PARAM commands are supported).

    This parameter ID applies only to #VOICE_MODULE_FLUENCE_PROV2 and
    #VOICE_MODULE_FLUENCEV5_BROADSIDE. This parameter ID can only be gotten
    (GET_PARAM) on topologies that contain these modules.

    More than one topology supports this parameter, and the client might not
    know the ECNS module ID. Therefore, the generic module ID,
    #VOICEPROC_MODULE_TX, supports this parameter. The VPM ensures
    that the payload is forwarded to the ECNS module in the chain.

    The maximum APR packet size allowed varies across chipsets based on the
    available memory. The client must consider the chipset and packet size to
    choose either the out-of-band or in-band form of transfer for this
    parameter.

    Maximum array size for this parameter:
    - Version 0 -- 376 bytes

    @vertspace{6} @inputtable{Cal_ID_0x00010E38.tex} @newpage
*/
#define VOICE_PARAM_FLUENCE_SOURCETRACKING (0x00010E38)


/* ======================================================================*/
/** @cond *//* Custom modules are not published in customer docs */
/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_SM_ECNS_1 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_sm_ecns_1.so.1
    get static properties function name: capi_v2_custom_sm_ecns_1_get_static_properties
    init function name                 : capi_v2_custom_sm_ecns_1_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_SM_ECNS_1         (0x10027050)

/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_SM_ECNS_2 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_sm_ecns_2.so.1
    get static properties function name: capi_v2_custom_sm_ecns_2_get_static_properties
    init function name                 : capi_v2_custom_sm_ecns_2_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_SM_ECNS_2         (0x10027051)

/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_SM_ECNS_3 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_sm_ecns_3.so.1
    get static properties function name: capi_v2_custom_sm_ecns_3_get_static_properties
    init function name                 : capi_v2_custom_sm_ecns_3_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_SM_ECNS_3         (0x10027052)

/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_DM_ECNS_1 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_dm_ecns_1.so.1
    get static properties function name: capi_v2_custom_dm_ecns_1_get_static_properties
    init function name                 : capi_v2_custom_dm_ecns_1_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_DM_ECNS_1         (0x10027053)

/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_DM_ECNS_2 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_dm_ecns_2.so.1
    get static properties function name: capi_v2_custom_dm_ecns_2_get_static_properties
    init function name                 : capi_v2_custom_dm_ecns_2_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_DM_ECNS_2         (0x10027054)

/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_DM_ECNS_3 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_dm_ecns_3.so.1
    get static properties function name: capi_v2_custom_dm_ecns_3_get_static_properties
    init function name                 : capi_v2_custom_dm_ecns_3_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_DM_ECNS_3         (0x10027055)

/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_QM_ECNS_1 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_qm_ecns_1.so.1
    get static properties function name: capi_v2_custom_qm_ecns_1_get_static_properties
    init function name                 : capi_v2_custom_qm_ecns_1_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_QM_ECNS_1         (0x10027056)

/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_QM_ECNS_2 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_qm_ecns_2.so.1
    get static properties function name: capi_v2_custom_qm_ecns_2_get_static_properties
    init function name                 : capi_v2_custom_qm_ecns_2_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_QM_ECNS_2         (0x10027057)

/* Custom ECNS module ID corresponding to VPM_TX_CUSTOM_QM_ECNS_3 topology.
    Name of the shared object and entry functions are predefined and same
    need to be used for implementation.

    shared object name                 : custom_qm_ecns_3.so.1
    get static properties function name: capi_v2_custom_qm_ecns_3_get_static_properties
    init function name                 : capi_v2_custom_qm_ecns_3_init

    This module supports custom parameter IDs.
*/
#define VOICE_MODULE_CUSTOM_QM_ECNS_3         (0x10027058)
/** @endcond *//* custom modules */
/* ======================================================================*/


/** @} */ /* end_addtogroup cal_module_ids */


#endif /* __ADSP_VPARAMS_API_PROP_H__ */
