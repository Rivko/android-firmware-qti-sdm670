////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodedefog.h
/// @brief Chi node for defog
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXCHINODEDefog_H
#define CAMXCHINODEDefog_H


#include "chinode.h"
#include "camxchinodeutil.h"
#include "chistatsproperty.h"
#include "chiispstatsdefs.h"
#include "chiiqmoduledefines.h"
#include "defog_internal.h"

static const UINT32 MaxRequests   = 64; ///< Max Requests


// NOWHINE FILE NC004c: Things outside the Camx namespace should be prefixed with Camx/CSL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Chi node structure for Chi interface.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChiDefogNode
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialization required to create a node
    ///
    /// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult Initialize(
        CHINODECREATEINFO* pCreateInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// QueryBufferInfo
    ///
    /// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
    ///
    /// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult QueryBufferInfo(
        CHINODEQUERYBUFFERINFO* pQueryBufferInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetBufferInfo
    ///
    /// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
    ///
    /// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult SetBufferInfo(
        CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ProcessRequest
    ///
    /// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
    ///
    /// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ProcessRequest(
        CHINODEPROCESSREQUESTINFO* pProcessRequestInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ReadMetaData
    ///
    /// @brief  Get Metadata
    ///
    /// @param  tag   Input Tag to be read from FrameWork Meta
    ///
    /// @return CDKResultSuccess or error code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID* ReadMetaData(UINT64 frameNum, UINT32 tag,const CHAR* tagName, INT32 = 1);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMetaChanged
    ///
    /// @brief  Get vendor tag base
    ///
    /// @return CDKResultSuccess or error code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL IsMetaChanged(UINT32 *data1, UINT32 *data2, UINT32 mask);
    BOOL IsMetaChanged(BOOL   *data1, BOOL   *data2, UINT32 mask);
    BOOL IsMetaChanged(FLOAT  *data1, FLOAT  *data2, UINT32 mask);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetVendorTagBase
    ///
    /// @brief  Get vendor tag base
    ///
    /// @return CDKResultSuccess or error code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult GetVendorTagBases();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckDependency
    ///
    /// @brief  Check if the dependencies are satisfied. Set the dependency if the dependency is not met.
    ///
    /// @param  pProcessRequestInfo   Pointer to a structure that defines the information required for processing a request.
    ///
    /// @return TRUE if dependency is satisfied, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL CheckDependency(
        CHINODEPROCESSREQUESTINFO* pProcessRequestInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsRealTime
    ///
    /// @brief  Method to query if node is part of realtime pipeline
    ///
    /// @return TRUE or FALSE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL IsRealTime() const
    {
        return m_nodeFlags.isRealTime;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ImgAlgoShdrAECStatsReadyCallback
    ///
    /// @brief  Callback function from Chinode where Stats vendor tags are filled
    ///
    /// @param  stats stats.
    /// @param  user_data user_data.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //static VOID ImgAlgoShdrAECStatsReadyCallback(
    //    SHDRSTATSINFO *stats,
    //    VOID * user_data);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiDefogNode
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiDefogNode();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~ChiDefogNode
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~ChiDefogNode();

    BOOL SetDeFogConfigParams(UINT64 frameNum);


private:
    ChiDefogNode(const ChiDefogNode&) = delete;               ///< Disallow the copy constructor
    ChiDefogNode& operator=(const ChiDefogNode&) = delete;    ///< Disallow assignment operator

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LoadLib
    ///
    /// @brief  Load the algo lib and map function pointers
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult LoadLib();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetVendorTagAddr
    ///
    /// @brief  Get Vendor Tag Location
    ///
    /// @return Addr of tag if success or 0.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 GetVendorTagAddr(const CHAR* pTagname, const CHAR* = "org.quic.camera.defog");


    CHIHANDLE           m_hChiSession;  ///< The Chi session handle
    UINT32              m_nodeId;       ///< The node's Id
    UINT32              m_nodeCaps;     ///< The selected node caps
    CHINODEFLAGS        m_nodeFlags;    ///< Node flags
    UINT64              m_processedFrame;   ///< The count for processed frame

    ///< Typedefs for Defog interface functions

    /******************************************************/

    /** DefogInitialize
    *    @lib_handle: defog library handle
    *    @params: configuration params for defog
    *
    * Initializes defog library
    *
    * return: CDKResultSuccess on success, CDKResultEFailed when fails
    **/
    typedef StatsAlgorithmHandle(*DEFOGLIBINIT) ();

    /** DefogGetParams
    *    @caps: query capabilities data
    *
    * defog lib fills it's capability and requirements
    *
    * return: CDKResultSuccess on success, CDKResultEFailed when fails
    **/
    typedef UINT32(*DEFOGGETPARAMS) (StatsAlgorithmHandle handler, DefogConfigParams *pDefogConfigParams);

    /** DefogSetParams
    *    @caps: query capabilities data
    *
    * defog lib fills it's capability and requirements
    *
    * return: CDKResultSuccess on success, CDKResultEFailed when fails
    **/
    typedef UINT32(*DEFOGSETPARAMS) (StatsAlgorithmHandle handler, DefogConfigParams *pDefogConfigParams);

    /** DefogProcess
    *    @handle: shdr library handle
    *    @buffer: buffer descriptor
    *    @input: true if buffer is input and false if buffer is output
    *
    * Maps buffer
    *
    * return: CDKResultSuccess on success, CDKResultEFailed when fails
    **/
    typedef UINT32(*DEFOGPROCESS) (StatsAlgorithmHandle handler, DefogData *pDefogData);

    /** DefogUninitialize
    *    @handle: shdr lib handle
    *
    * Deinitializes shdr module
    *
    * return: nothing
    **/
    typedef UINT32(*DEFOGUNINITIALIZE) (StatsAlgorithmHandle handler);

    /**** Metadata Pool ***/
    typedef enum MetaPool
    {
        InputPool  = 1,
        ResultPool = 2
    } MetadataPoolType;

    struct PrevDefogConfigParams
    {
        UINT32 param_update_mask;
        BOOL   defog_en;                    // 0: disable defog, 1: enable defog                                       (default: 1)
        BOOL   ce_en;                       // 0: disable CE(contrast enahancement), 1: enable CE                      (default: 0)
        UINT32 speed;                       // [0:10], convergence speed, 0: slowest, 10: fastest                      (default: 10)
        UINT32 convergence_mode;            // 1: serial mode, 2: parallel mode                                        (default: 1)
        BOOL   sd_en;                       // 0: disable SD(scene detector), 1: enable SD                             (default: 1)
        BOOL   sd_2a_en;                    // 0: SD without 2A info, 1: SD with 2A info                               (default: 1)
        UINT32 algo;                        // 0: Gamma Defog, 1: CV Defog, 2: Manual Gamma Defog, 3: Manual CV Defog  (default: 0)
        FLOAT  strength;                    // [0.0:100.0], 0.0: no defog effect, 100.0: strongest defog effect        (default: 1.0)
        UINT32 lp_algo_decision_mode;       // 0: min bin decision, 1: avg bin decision, 2: max bin decision           (default: 0)
        FLOAT  lp_algo_color_comp_gain;     // 0.0: disable color compensation, 1.0: 1x color compensation gain        (default: 1.0)
        BOOL   abc_en;                      // 0: disable ABC(adaptive brightness compensation), 1: enable ABC         (default: 1)
        BOOL   acc_en;                      // 0: disable ACC(advanced contrast control), 1: enable ACC                (default: 1)
        UINT32 defog_dark_thres;            // [1:256]  (default: 20): the two parameters are for defog reprocess. if defog status is settled and dark/bright bins are
        UINT32 defog_bright_thres;          // [1:256]  (default: 80):   within the range of defog_bright_thres and defog_dark_thres, it will keep the previous defog settings.
        UINT32 dark_limit;                  // [0:255]: limit max internal defog strength for dark regions             (default: 255)
        UINT32 bright_limit;                // [0:255]: limit max internal defog strength for bright regions           (default: 0)
        UINT32 dark_preserve;               // [0:255]: move dark bin to preserve dark details                         (default: 10)
        UINT32 bright_preserve;             // [0:255]: move bright bin to preserve bright details                     (default: 50)
        FLOAT  abc_str;                     // [0.0:4.0]: strength for adaptive brightness compensation                (default: 2.0)
        FLOAT  acc_dark_str;                // [0.0:4.0]: ACC strength for dark region                                 (default: 2.0)
        FLOAT  acc_bright_str;              // [0.0:4.0]: ACC strength for bright region                               (default: 0.5)
        BOOL    guc_en;                     // 0: disable GUC(Gain-up control), enable GUC                             (default: 1)
        BOOL    dcc_en;                     // 0: disable DCC(dynamic contrast control), 1: enable DCC                 (default: 1)
        FLOAT   guc_str;                    // [0.0:5.0]: GUC strength on CE mode                                      (default: 1.0)
        FLOAT   dcc_dark_str;               // [0.0:4.0]: DCC strength for dark region on CE mode                      (default: 1.0)
        FLOAT   dcc_bright_str;             // [0.0:4.0]: DCC strength for bright region on CE mode                    (default: 1.0)
        FOG_SCENE_DETECTION_PARAMS defog_trig_params;   // Define DNR/Lux/CCT triggers for fog scene detection         (default settings are as below)
        CE_SCENE_DETECTION_PARAMS ce_trig_params;       // Define Gain triggers for ce scene detection                 (default settings are as below)
       };
    /********************** Defog trigger default settings ****************************
      dnr_trigger[FogSceneDetectionDNRTriggerModeFlatScene].start         = 0.0f;
      dnr_trigger[FogSceneDetectionDNRTriggerModeFlatScene].end           = 1.0f;
      dnr_trigger[FogSceneDetectionDNRTriggerModeFogScene].start          = 1.5f;
      dnr_trigger[FogSceneDetectionDNRTriggerModeFogScene].end            = 4.0f;
      dnr_trigger[FogSceneDetectionDNRTriggerModeNormalScene].start       = 4.5f;
      dnr_trigger[FogSceneDetectionDNRTriggerModeNormalScene].end         = 8.0f;
      dnr_trigger[FogSceneDetectionDNRTriggerModeFlatScene].p             = 0;
      dnr_trigger[FogSceneDetectionDNRTriggerModeFogScene].p              = 100;
      dnr_trigger[FogSceneDetectionDNRTriggerModeNormalScene].p           = 0;
      lux_trigger[FogSceneDetectionLuxTriggerModeDayLight].start          = 0.0f;
      lux_trigger[FogSceneDetectionLuxTriggerModeDayLight].end            = 50.0f;
      lux_trigger[FogSceneDetectionLuxTriggerModeNormalLight].start       = 80.0f;
      lux_trigger[FogSceneDetectionLuxTriggerModeNormalLight].end         = 450.0f;
      lux_trigger[FogSceneDetectionLuxTriggerModeLowLight].start          = 510.0f;
      lux_trigger[FogSceneDetectionLuxTriggerModeLowLight].end            = 900.0f;
      lux_trigger[FogSceneDetectionLuxTriggerModeDayLight].p              = 0;
      lux_trigger[FogSceneDetectionLuxTriggerModeNormalLight].p           = 100;
      lux_trigger[FogSceneDetectionLuxTriggerModeLowLight].p              = 0;
      cct_trigger[FogSceneDetectionCCTTriggerModeLow].start               = 0.0f;
      cct_trigger[FogSceneDetectionCCTTriggerModeLow].end                 = 2000.0f;
      cct_trigger[FogSceneDetectionCCTTriggerModeIndoorAndOutdoor].start  = 2300.0f;
      cct_trigger[FogSceneDetectionCCTTriggerModeIndoorAndOutdoor].end    = 4500.0f;
      cct_trigger[FogSceneDetectionCCTTriggerModeOutdoorAndFog].start     = 5000.0f;
      cct_trigger[FogSceneDetectionCCTTriggerModeOutdoorAndFog].end       = 8500.0f;
      cct_trigger[FogSceneDetectionCCTTriggerModeHigh].start              = 10000.0f;
      cct_trigger[FogSceneDetectionCCTTriggerModeHigh].end                = 20000.0f;
      cct_trigger[FogSceneDetectionCCTTriggerModeLow].p                   = 0;
      cct_trigger[FogSceneDetectionCCTTriggerModeIndoorAndOutdoor].p      = 0;
      cct_trigger[FogSceneDetectionCCTTriggerModeOutdoorAndFog].p         = 100;
      cct_trigger[FogSceneDetectionCCTTriggerModeHigh].p                  = 0;
      drc_trigger[FogSceneDetectionDRCTriggerModeLowDRCGain].start        = 0.0f;
      drc_trigger[FogSceneDetectionDRCTriggerModeLowDRCGain].end          = 1.0f;
      drc_trigger[FogSceneDetectionDRCTriggerModeHighDRCGain].start       = 2.0f;
      drc_trigger[FogSceneDetectionDRCTriggerModeHighDRCGain].end         = 64.0f;
      drc_trigger[FogSceneDetectionDRCTriggerModeLowDRCGain].p            = 100;
      drc_trigger[FogSceneDetectionDRCTriggerModeHighDRCGain].p           = 0;
      hdr_trigger[FogSceneDetectionHDRTriggerModeLowHDRRatio].start       = 0.0f;
      hdr_trigger[FogSceneDetectionHDRTriggerModeLowHDRRatio].end         = 1.0f;
      hdr_trigger[FogSceneDetectionHDRTriggerModeHighHDRRatio].start      = 2.0f;
      hdr_trigger[FogSceneDetectionHDRTriggerModeHighHDRRatio].end        = 64.0f;
      hdr_trigger[FogSceneDetectionHDRTriggerModeLowHDRRatio].p           = 100;
      hdr_trigger[FogSceneDetectionHDRTriggerModeHighHDRRatio].p          = 0;
    **********************************************************************************/
    /********************** CE trigger default settings ****************************
      gain_trigger[CESceneDetectionGainTriggerModeLowGain].start          = 1.0f;
      gain_trigger[CESceneDetectionGainTriggerModeLowGain].end            = 2.0f;
      gain_trigger[CESceneDetectionGainTriggerModeNormalGain].start       = 4.0f;
      gain_trigger[CESceneDetectionGainTriggerModeNormalGain].end         = 8.0f;
      gain_trigger[CESceneDetectionGainTriggerModeHighGain].start         = 16.0f;
      gain_trigger[CESceneDetectionGainTriggerModeHighGain].end           = 4096.0f;
      gain_trigger[CESceneDetectionGainTriggerModeLowGain].p              = 100;
      gain_trigger[CESceneDetectionGainTriggerModeNormalGain].p           = 80;
      gain_trigger[CESceneDetectionGainTriggerModeHighGain].p             = 0;
    **********************************************************************************/

    CHILIBRARYHANDLE         m_hDeFogLib;                               ///< Handle for Shdr library.
    DEFOGLIBINIT             m_defog_lib_init;                          ///< Function pointer for eis2_initialize
    DEFOGGETPARAMS           m_defog_lib_getparams;                     ///< Function pointer
    DEFOGSETPARAMS           m_defog_lib_setparams;                     ///< Function pointer
    DEFOGPROCESS             m_defog_lib_process;                       ///< Function pointer
    DEFOGUNINITIALIZE        m_defog_lib_deinit;                        ///< Function pointer
    //SHDRLIBPROCESS          m_shdr_lib_process;                       ///< Function pointer
    StatsAlgorithmHandle     m_handle;
    DefogData*               m_p_DefogConfigData;
    DefogInternalData*       m_p_DefogInternalData;
    DefogConfigParams        m_DefogParams;                             ///< p_shdr_ctx
    PrevDefogConfigParams    m_PrevDefogParams;
    UINT32                  m_AECDataVendorTagBase;
    UINT32                  m_AWBDataVendorTagBase;
    UINT32                  m_Gamma16VendorTagBase;
    UINT32                  m_Gamma15VendorTagBase;
    UINT32                  m_ANRDataVendorTagBase;
    UINT32                  m_LTMDataVendorTagBase;
    UINT32                  m_ASFDataVendorTagBase;
    UINT32                  m_CVDataVendorTagBase;
    UINT32                  m_IHISTDataVendorTagBase;
    UINT32                  m_SHDRIHISTDataVendorTagBase;
    UINT32                  m_SHDRSettledVendorTagBase;
    UINT32                  m_SensorModeInfoTagBase;
    UINT32                  m_DefogEnableVendorTagBase;
    UINT32                  m_DefogAlgoVendorTagBase;
    UINT32                  m_DefogAlgoEnableCETagBase;
    UINT32                  m_DefogAlgoEnableConvTagBase;
    UINT32                  m_DefogAlgoGainUpControlBase;
    UINT32                  m_DefogAlgoDccControlBase;
    UINT32                  m_DefogAlgoGucStrengthBase;
    UINT32                  m_DefogAlgoDccDarkStrengthBase;
    UINT32                  m_DefogAlgoDccBrightStrengthBase;
    UINT32                  m_DefogAlgoceSceneDetectionBase;
    UINT32                  m_DefogStrengthVendorTagBase;
    UINT32                  m_DefogSpeedVendorTagBase;
    UINT32                  m_DefogDecisionModeVendorTagBase;
    UINT32                  m_DefogColorCompGainVendorTagBase;
    UINT32                  m_DefogAdaptiveBrightnessCompEnableVendorTagBase;
    UINT32                  m_DefogAdvanceContractControlEnableVendorTagBase;
    UINT32                  m_DefogAutoFogSceneDetectionEnableVendorTagBase;
    UINT32                  m_DefogAutoFogSceneDetectionWith2AEnableVendorTagBase;
    UINT32                  m_DefogDarkThresVendorTagBase;
    UINT32                  m_DefogBrightThresVendorTagBase;
    UINT32                  m_DefogAdaptiveBrightnessCompStrVendorTagBase;
    UINT32                  m_DefogMaxAdvanceContractControlDarkStrengthVendorTagBase;
    UINT32                  m_DefogMaxAdvanceContractControlBrightStrengthVendorTagBase;
    UINT32                  m_DefogDarkLimitVendorTagBase;
    UINT32                  m_DefogBrightLimitVendorTagBase;
    UINT32                  m_DefogDarkPreserveVendorTagBase;
    UINT32                  m_DefogBrightPreserveVendorTagBase;
    UINT32                  m_DefogTriggerParamVendorTagBase;
    UINT32                  m_DefogisSettledVendorTagBase;
    UINT32                  m_DefogAlgoFogSceneProbVendorTagBase;
    UINT32                  m_DefogGammaStrengthVendorTagBase;
    BOOL                    isSHDRSupported;

    UINT32                  m_frameCountVendorTagBase;
    UINT32                  m_ADRCinfoVendorTagBase;
    UINT32                  m_sensorModeInfoVendorTagBase;

public:
    /*static void ShdrCompletionCallback(void *handle,
        SHDRINPUTPARAM *input_param, int32_t output_fd,
        SHDROUTPUTPARAM *buf_3a, void *user_data);*/
    UINT32 frameNum[MaxRequests];
    CHINODEIMAGEFORMAT  m_format;       ///< The selected format
};
#endif // CAMXCHINODEDefog_H
