////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxpipeline.h
/// @brief CHX pipeline class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXPIPELINE_H
#define CHXPIPELINE_H

#include <assert.h>

#include "chi.h"
#include "chioverride.h"
#include "camxcdktypes.h"
#include "chxextensionmodule.h"

/// Constants
static const UINT64 InvalidPipeline = 0x0;
static const UINT   SensorNodeId    = 0;
static const UINT   TorchNodeId     = 10;


/// @brief Define the pipeline types
enum PipelineType
{
    Invalid             = 0,
    RealtimePreview     = 1,
    RealtimeZSL         = 2,
    OfflineZSLYUV       = 3,
    OfflineZSLJPEG      = 4,
    Default             = 5,
    MFNRPrefilter       = 6,
    MFNRBlend           = 7,
    MFNRPostfilter      = 8,
    MFNRScale           = 9,
    OfflineYUVJPEG      = 10,
    OfflineCustom       = 11,
    OfflinePreview      = 12,
    MaxPipelineTypes    = 13,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Pipeline class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Pipeline
{
public:
    static Pipeline* Create(
        UINT32                                cameraId,
        PipelineType                          type);

    /// Destroy
    VOID Destroy();

    /// Set the output buffers
    VOID SetOutputBuffers(
        UINT                     numOutputs,
        CHIPORTBUFFERDESCRIPTOR* pOutputs);

    /// Set the input buffers
    VOID SetInputBuffers(
        UINT                     numInputs,
        CHIPORTBUFFERDESCRIPTOR* pInputs);

    /// Set the preview stream
    CDKResult SetPreviewStream(
        CHISTREAM* pStream);

    /// Set the RDI stream
    CDKResult SetRDIStream(
        CHISTREAM* pStream);

    /// Set the snapshot stream
    CDKResult SetSnapshotStream(
        CHISTREAM* pStream);

    /// Set the blend output stream
    CDKResult SetOutputStream(
        CHISTREAM* pStream);

    /// Set the input stream
    CDKResult SetInputStream(
        CHISTREAM* pStream);

    /// Set pipeline activate flag
    VOID SetPipelineActivateFlag()
    {
        m_pipelineActivated = TRUE;
    }

    /// Set pipeline deactivate
    VOID SetPipelineDeactivate()
    {
        m_pipelineActivated = FALSE;
    }

    /// Get the pipeline handle
    CHIPIPELINEDESCRIPTOR GetPipelineHandle() const
    {
        return m_hPipelineHandle;
    }

    /// Return the input buffer descriptor
    CHIPORTBUFFERDESCRIPTOR* GetInputBufferDescriptors()
    {
        return ((FALSE == IsRealTime()) ? &m_pipelineInputBuffer[0] : NULL);
    }

    /// Set the pipeline node/port info
    VOID SetPipelineNodePorts(
        const CHIPIPELINECREATEDESCRIPTOR* pCreateDesc)
    {
        m_pipelineDescriptor = *pCreateDesc;
    }

    /// Set the pipeline resource policy
    VOID SetPipelineResourcePolicy(
        CHIRESOURCEPOLICY policy)
    {
        m_resourcePolicy = policy;
    }

    /// Get the pipeline resource policy
    CHIRESOURCEPOLICY GetPipelineResourcePolicy() const
    {
        return m_resourcePolicy;
    }

    /// Set the defer finalize pipeline flag
    VOID SetDeferFinalizeFlag(BOOL isDeferFinalizeNeeded)
    {
        m_isDeferFinalizeNeeded = isDeferFinalizeNeeded;
    }

    /// Get the defer finalize pipeline flag
    BOOL GetDeferFinalizeFlag() const
    {
        return m_isDeferFinalizeNeeded;
    }
    /// Get the input buffer requirements for this pipeline
    const CHIPIPELINEINPUTOPTIONS* GetInputOptions() const
    {
        return &m_pipelineInputOptions[0];
    }

    /// Is pipeline active
    BOOL IsPipelineActive() const
    {
        return m_pipelineActivated;
    }

    /// Is pipeline real time
    BOOL IsRealTime() const
    {
        return m_pipelineDescriptor.isRealTime;
    }

    /// Check if the pipeline is offline
    BOOL IsOffline()
    {
        return ((TRUE == IsRealTime()) ? FALSE : TRUE);
    }

    /// Create the pipeline descriptor
    CDKResult CreateDescriptor();

    /// Get the pipeline information
    const CHIPIPELINEINFO GetPipelineInfo() const;

    /// Get the sensor mode index
    CHISENSORMODEINFO* GetSensorModeInfo()
    {
        return m_pSelectedSensorMode;
    }

    /// Get CameraId of the pipeline
    UINT32 GetCameraId()
    {
        return m_cameraId;
    }

private:
    Pipeline() = default;
    ~Pipeline();

    /// Does the pipeline have sensor node
    static BOOL HasSensorNode(
        const ChiPipelineCreateDescriptor* pCreateDesc);

    /// Initialize the pipeline object
    CDKResult Initialize(
        UINT32                                cameraId,
        PipelineType                          type);

    /// Create the real time preview pipeline
    CDKResult CreateRealTimePreviewPipeline();

    /// Create the real time pipeline descriptor
    VOID SetupRealtimePreviewPipelineDescriptor();

    /// Finalize the real time pipeline
    CDKResult FinalizeRealTimeZSLPipeline();

    /// Create the real time ZSL pipeline descriptor used to create the pipeline
    VOID SetupRealTimeZSLPipelineDescriptor();

    /// Finalize the Offline ZSL pipeline
    CDKResult FinalizeOfflineZSLPipeline();

    /// Setup the offline ZSL JPEG pipeline descriptor
    VOID SetupOfflineZSLJPEGPipelineDescriptor();

    /// Setup the offline ZSL YUV pipeline descriptor
    VOID SetupOfflineZSLYUVPipelineDescriptor();

    /// Setup the offline Prefilter pipeline descriptor
    VOID SetupMFNRPrefilterPipelineDescriptor();

    /// Setup the offline Blend pipeline descriptor
    VOID SetupMFNRBlendPipelineDescriptor();

    /// Setup the offline Scale pipeline descriptor
    VOID SetupMFNRScalePipelineDescriptor();

    /// Setup the offline ZSL pipeline descriptor
    VOID SetupMFNRPostFilterPipelineDescriptor();

    // Do not implement the copy constructor or assignment operator
    Pipeline(const Pipeline& rPipeline) = delete;
    Pipeline& operator= (const Pipeline& rPipeline) = delete;

    PipelineType                m_type;                            ///< Pipeline type
    CHIPIPELINEDESCRIPTOR       m_hPipelineHandle;                 ///< Pipeline handle
    /// @todo Better way would be to not have fixed max size but as per need only
    CHIPIPELINECREATEDESCRIPTOR m_pipelineDescriptor;              ///< Pipeline descriptor
    CHINODE                     m_nodes[20];                       ///< List of nodes in the pipeline
    CHINODELINK                 m_links[20];                       ///< Links in the pipeline
    CHIINPUTPORTDESCRIPTOR      m_inputPorts[20];                  ///< Input ports
    CHIOUTPUTPORTDESCRIPTOR     m_outputPorts[20];                 ///< Output ports
    CHILINKNODEDESCRIPTOR       m_linkNodeDescriptors[20];         ///< Link node descriptors
    CHIPORTBUFFERDESCRIPTOR     m_pipelineOutputBuffer[16];        ///< Pipeline Output buffer descriptor
    UINT32                      m_numOutputBuffers;                ///< Number of output buffers of the pipeline
    CHIPORTBUFFERDESCRIPTOR     m_pipelineInputBuffer[16];         ///< Pipeline input buffers
    UINT32                      m_numInputBuffers;                 ///< Number of input buffers
    CHIPIPELINEINPUTOPTIONS     m_pipelineInputOptions[16];        ///< Input buffer requirements of the pipeline
    CHIPIPELINEINFO             m_pipelineInfo;                    ///< Pipeline input, output info
    UINT32                      m_cameraId;                        ///< Camera Id
    CHISENSORMODEINFO*          m_pSelectedSensorMode;             ///< Selected sensor mode for this pipeline
    BOOL                        m_pipelineActivated;               ///< flag to indicate if pipeline is active
    UINT32                      m_statsSkipPattern;                ///< 3A frame skip pattern for this pipeline
    UINT                        m_enableFOVC;                      ///< Enable/disable FOVC for this pipeline
    CHIRESOURCEPOLICY           m_resourcePolicy;                  ///< Resource Policy
    BOOL                        m_isDeferFinalizeNeeded;           ///< Flag to indicate if pipeline need to defer
                                                                   ///  finalize
};

#endif // CHXPIPELINE_H
