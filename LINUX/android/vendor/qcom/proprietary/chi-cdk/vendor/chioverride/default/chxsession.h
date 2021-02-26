////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxsession.h
/// @brief CHX session class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXSESSION_H
#define CHXSESSION_H

#include <assert.h>

#include "chi.h"
#include "chioverride.h"
#include "camxcdktypes.h"
#include "chxextensionmodule.h"
#include "chxincs.h"
#include "chxpipeline.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Session
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Session
{
public:

    /// Create an instance of the class
    static Session* Create(
        Pipeline**    ppPipelines,
        UINT32        numPipelines,
        ChiCallBacks* pCallbacks,
        VOID*         pPrivateData);

    /// Destroy Session object
    VOID Destroy(
        BOOL isForced);

    /// Get pipeline handle
    CHIPIPELINEDESCRIPTOR GetPipelineHandle(UINT index = 0) const
    {
        if (index < m_numPipelines)
        {
            return m_pPipelines[index]->GetPipelineHandle();
        }
        else
        {
            return NULL;
        }
    }

    /// Get session handle
    CHIHANDLE GetSessionHandle() const
    {
        return m_hSession;
    }

    /// Finalize the session creation
    CDKResult Finalize();
    /// Get the pipeline index from camera index
    UINT32 GetPipelineIndex(UINT32 cameraId);
    /// Get the Camera index from pipeline index
    UINT32 GetCameraId(UINT32 pipelineIndex);

    /// Set pipeline Activate flag
    VOID SetPipelineActivateFlag(UINT index = 0) const
    {
        if (index < m_numPipelines)
        {
            m_pPipelines[index]->SetPipelineActivateFlag();
        }
        else
        {
            CHX_LOG_INFO("Invalid Pipeline index");
        }
    }

    /// Set pipeline Deactivate
    VOID SetPipelineDeactivate(UINT index = 0) const
    {
        if (index < m_numPipelines)
        {
            m_pPipelines[index]->SetPipelineDeactivate();
        }
        else
        {
            CHX_LOG_INFO("Invalid Pipeline index");
        }
    }

    /// Is pipeline active
    BOOL IsPipelineActive(UINT index = 0) const
    {
        if (index < m_numPipelines)
        {
            return m_pPipelines[index]->IsPipelineActive();
        }
        else
        {
            CHX_LOG_INFO("Invalid Pipeline index");
        }

        return FALSE;
    }

    /// Get the sensor mode index
    CHISENSORMODEINFO* GetSensorModeInfo(UINT index = 0) const
    {
        if (index < m_numPipelines)
        {
            return m_pPipelines[index]->GetSensorModeInfo();
        }
        else
        {
            return NULL;
        }
    }

private:

    /// Initialize the session object
    CDKResult Initialize(
        Pipeline**      ppPipelines,
        UINT32          numPipelines,
        ChiCallBacks*   pChiCallbacks,
        VOID*           pPrivateData,
        CHISESSIONFLAGS flags = { { 0 } });         ///< Note the default value.

    /// Default Constructor
    Session() = default;
    /// Destructor
    ~Session();

    // Do not implement the copy constructor or assignment operator
    Session(const Session& rSession) = delete;
    Session& operator= (const Session& rSession) = delete;

    CHIHANDLE           m_hSession;                                     ///< Session handle
    Pipeline*           m_pPipelines[PipelineType::MaxPipelineTypes];   ///< Pipeline handles
    UINT                m_numPipelines;                                 ///< Number of pipelines
    CHIPIPELINEINFO*    m_pPipelineInfo;                                ///< Pipeline info
};

#endif // CHXSESSION_H
