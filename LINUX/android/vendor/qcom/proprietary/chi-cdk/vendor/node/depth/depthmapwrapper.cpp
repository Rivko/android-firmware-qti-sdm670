/**********************************************************************
*  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include "depthmapwrapper.h"
// Include depth map library header file

struct DepthMapWrapper::DepthWrapperData {
  // Declare Depth map object.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthMapWrapper::DepthMapWrapper
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DepthMapWrapper::DepthMapWrapper(
    int32_t width,
    int32_t height,
    int32_t dm_width,
    int32_t dm_height,
    uint64_t* exposure_time,
    int32_t* sensitivity)
{
    (void)width;
    (void)height;
    (void)dm_width;
    (void)dm_height;

    pDepthData = new DepthWrapperData;
    if (pDepthData)
    {
       // Instantiate depth library object
    }
    // Conversion library is responsible for setting the initial exposure values
    *exposure_time = 13000000;
    *sensitivity = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthMapWrapper::processFrame
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DepthMapWrapper::processFrame(
    const int8_t* in_image,
    WrapperDepthCamConfig *wrapper_depth_map_config,
    uint32_t num_outputs,
    WrapperDepthOutput outputs[]) {

    (void)in_image;
    bool rc = true;

    if (!pDepthData || !wrapper_depth_map_config || (0 == num_outputs)
        || !outputs)
    {
        return false;
    }

    for (uint32_t i = 0; i < num_outputs; ++i)
    {
        switch(outputs[i].format)
        {
            case WRAPPER_CAM_FORMAT_IR:
                if (!outputs[i].out_depth_image || !outputs[i].out_image_length) {
                    rc = false;
                    break;
                }
                // Call relevant depth  API
            break;
            case WRAPPER_CAM_FORMAT_DEPTH16:
                if (!outputs[i].out_depth_image || !outputs[i].out_image_length) {
                    rc = false;
                    break;
                }
                // Call relevant depth API
            break;
            case WRAPPER_CAM_FORMAT_DEPTH_POINT_CLOUD:
                if (!outputs[i].out_pcloud_image || !outputs[i].count_pt_cloud) {
                    rc = false;
                    break;
                }
                // Call relevant depth API
            break;
            default:
               rc = false;
               break;
        }
    }
    // Example of setting auto exposure value to 15.000555 ms
    wrapper_depth_map_config->in_out_depth_map_ae_val = 15000555;
    return rc;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthMapWrapper::~DepthMapWrapper
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DepthMapWrapper::~DepthMapWrapper()
{
    if (pDepthData)
    {
        //delete Depth map object
        delete pDepthData;
        pDepthData = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthMapWrapperCreate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DepthMapWrapper* DepthMapWrapperCreate(
    int32_t width,
    int32_t height,
    int32_t dm_width,
    int32_t dm_height,
    uint64_t* exposure_time,
    int32_t* sensitivity)
{
    return new DepthMapWrapper(width,
        height,
        dm_width,
        dm_height,
        exposure_time,
        sensitivity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthMapWrapperProcessFrame
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DepthMapWrapperProcessFrame(DepthMapWrapper* pDepthMapWrapper,
    const int8_t *in_image,
    WrapperDepthCamConfig *wrapper_depth_map_config,
    uint32_t num_outputs,
    WrapperDepthOutput outputs[])
{
    return pDepthMapWrapper->processFrame(in_image,
        wrapper_depth_map_config,
        num_outputs,
        outputs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthMapWrapperDestroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DepthMapWrapperDestroy(DepthMapWrapper* pDepthMapWrapper)
{
    if (NULL != pDepthMapWrapper)
    {
        delete pDepthMapWrapper;
    }
}
