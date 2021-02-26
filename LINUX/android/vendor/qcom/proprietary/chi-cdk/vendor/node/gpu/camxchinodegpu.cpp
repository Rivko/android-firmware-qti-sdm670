////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodegpu.cpp
/// @brief Chi node for GPU functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE PR008: Error: - Can't include library (bracketed) includes after quoted includes

#include <CL/cl_ext_qcom.h>
#include <system/camera_metadata.h>
#include "camxchinodegpu.h"
#include "camxutils.h"

#undef LOG_TAG
#define LOG_TAG "CHIGPU"

/// @todo (CAMX-2282) Create CDK Utils and OS Utils that can be used from CDK/Chi nodes

#if defined (_LINUX)

const CHAR* pDefaultOpenCLLibraryName = "libOpenCL";

#elif defined (_WIN32)

const CHAR* pDefaultOpenCLLibraryName = "OpenCL";

#else
#error Unsupported target defined
#endif // defined(_LINUX) || defined(_WIN32)

// =============================================================================================================================
// OpenCL Stuff
// =============================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::GetInstance
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GPUOpenCL* GPUOpenCL::GetInstance()
{
    static GPUOpenCL s_gpuOpenCL;

    if (s_gpuOpenCL.m_initStatus == CLInitInvalid)
    {
        s_gpuOpenCL.Initialize();
    }

    return &s_gpuOpenCL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::GPUOpenCL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GPUOpenCL::GPUOpenCL()
    : m_initStatus(CLInitInvalid)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::~GPUOpenCL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GPUOpenCL::~GPUOpenCL()
{
    Uninitialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::InitializeFuncPtrs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::InitializeFuncPtrs()
{
    CDKResult   result          = CDKResultSuccess;
    INT         numCharWritten  = 0;
    CHAR        libFilePath[FILENAME_MAX];

    numCharWritten = ChiNodeUtils::SNPrintF(libFilePath,
                                            FILENAME_MAX,
                                            "%s%s%s.%s",
                                            VendorLibPath,
                                            PathSeparator,
                                            pDefaultOpenCLLibraryName,
                                            SharedLibraryExtension);

    m_hOpenCLLib = ChiNodeUtils::LibMapFullName(libFilePath);

    if (NULL == m_hOpenCLLib)
    {
        result = CDKResultEUnableToLoad;
    }
    else
    {
        m_pfnCLGetPlatformIDs       = reinterpret_cast<PFNCLGETPLATFORMIDS>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clGetPlatformIDs"));
        m_pfnCLGetDeviceIDs         = reinterpret_cast<PFNCLGETDEVICEIDS>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clGetDeviceIDs"));
        m_pfnCLCreateContext        = reinterpret_cast<PFNCLCREATECONTEXT>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clCreateContext"));
        m_pfnCLReleaseContext       = reinterpret_cast<PFNCLRELEASECONTEXT>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clReleaseContext"));

        m_pfnCLCreateCommandQueue   = reinterpret_cast<PFNCLCREATECOMMANDQUEUE>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clCreateCommandQueue"));
        m_pfnCLReleaseCommandQueue  = reinterpret_cast<PFNCLRELEASECOMMANDQUEUE>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clReleaseCommandQueue"));

        m_pfnCLCreateImage2D        = reinterpret_cast<PFNCLCREATEIMAGE2D>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clCreateImage2D"));
        m_pfnCLCreateImage          = reinterpret_cast<PFNCLCREATEIMAGE>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clCreateImage"));
        m_pfnCLReleaseMemObject     = reinterpret_cast<PFNCLRELEASEMEMOBJECT>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clReleaseMemObject"));

        m_pfnCLCreateProgramWithSource = reinterpret_cast<PFNCLCREATEPROGRAMWITHSOURCE>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clCreateProgramWithSource"));

        m_pfnCLBuildProgram         = reinterpret_cast<PFNCLBUILDPROGRAM>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clBuildProgram"));
        m_pfnCLReleaseProgram       = reinterpret_cast<PFNCLRELEASEPROGRAM>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clReleaseProgram"));

        m_pfnCLGetProgramBuildInfo  = reinterpret_cast<PFNCLGETPROGRAMBUILDINFO>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clGetProgramBuildInfo"));

        m_pfnCLCreateKernel         = reinterpret_cast<PFNCLCREATEKERNEL>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clCreateKernel"));
        m_pfnCLReleaseKernel        = reinterpret_cast<PFNRELEASEKERNEL>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clReleaseKernel"));
        m_pfnCLSetKernelArg         = reinterpret_cast<PFNCLSETKERNELARG>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clSetKernelArg"));

        m_pfnCLEnqueueNDRangeKernel = reinterpret_cast<PFNCLENQUEUENDRANGEKERNEL>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clEnqueueNDRangeKernel"));

        m_pfnCLFlush                = reinterpret_cast<PFNCLFLUSH>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clFlush"));
        m_pfnCLFinish               = reinterpret_cast<PFNCLFINISH>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clFinish"));
        m_pfnCLGetDeviceInfo        = reinterpret_cast<PFNCLGETDEVICEINFO>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clGetDeviceInfo"));
        m_pfnCLCreateBuffer         = reinterpret_cast<PFNCLCREATEBUFFER>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clCreateBuffer"));
        m_pfnCLCreateSampler        = reinterpret_cast<PFNCLCREATESAMPLER>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clCreateSampler"));
        m_pfnCLReleaseSampler       = reinterpret_cast<PFNCLRELEASESAMPLER>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clReleaseSampler"));

        m_pfnCLEnqueueCopyBuffer = reinterpret_cast<PFNCLENQUEUECOPYBUFFER>(
                                          ChiNodeUtils::LibGetAddr(m_hOpenCLLib, "clEnqueueCopyBuffer"));
    }

    if ((NULL == m_pfnCLGetPlatformIDs)             ||
        (NULL == m_pfnCLGetDeviceIDs)               ||
        (NULL == m_pfnCLCreateContext)              ||
        (NULL == m_pfnCLReleaseContext)             ||
        (NULL == m_pfnCLCreateCommandQueue)         ||
        (NULL == m_pfnCLReleaseCommandQueue)        ||
        (NULL == m_pfnCLCreateImage)                ||
        (NULL == m_pfnCLCreateImage2D)              ||
        (NULL == m_pfnCLReleaseMemObject)           ||
        (NULL == m_pfnCLCreateProgramWithSource)    ||
        (NULL == m_pfnCLBuildProgram)               ||
        (NULL == m_pfnCLReleaseProgram)             ||
        (NULL == m_pfnCLGetProgramBuildInfo)        ||
        (NULL == m_pfnCLCreateKernel)               ||
        (NULL == m_pfnCLReleaseKernel)              ||
        (NULL == m_pfnCLSetKernelArg)               ||
        (NULL == m_pfnCLEnqueueNDRangeKernel)       ||
        (NULL == m_pfnCLFlush)                      ||
        (NULL == m_pfnCLFinish)                     ||
        (NULL == m_pfnCLGetDeviceInfo)              ||
        (NULL == m_pfnCLCreateBuffer)               ||
        (NULL == m_pfnCLCreateSampler)              ||
        (NULL == m_pfnCLReleaseSampler))
    {
        LOG_ERROR(CamxLogGroupChi,
                  "Error Initializing one or more function pointers in Library: %s%s.%s",
                  libFilePath,
                  pDefaultOpenCLLibraryName,
                  SharedLibraryExtension);
        result = CDKResultENoSuch;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::Initialize()
{

    m_initStatus = CLInitRunning;

    CDKResult       result          = CDKResultSuccess;
    cl_platform_id  platform        = NULL;
    cl_int          error           = CL_SUCCESS;
    cl_uint         numPlatforms    = 0;

    result = InitializeFuncPtrs();

    if (CDKResultSuccess == result)
    {
        // Get the platform ID
        error = m_pfnCLGetPlatformIDs(1, &platform, &numPlatforms);

        if (CL_SUCCESS != error)
        {
            LOG_ERROR(CamxLogGroupChi, "Error getting the platform ID: %d", error);
            result = CDKResultEFailed;
        }

        // Get the requested device
        if (CDKResultSuccess == result)
        {
            error = m_pfnCLGetDeviceIDs(platform,  CL_DEVICE_TYPE_GPU, 1, &m_device, NULL);
            if (CL_SUCCESS != error)
            {
                LOG_ERROR(CamxLogGroupChi, "Error getting the requested Device: %d", error);
                result = CDKResultEFailed;
            }
        }

        if (CDKResultSuccess == result)
        {
            m_context = m_pfnCLCreateContext( NULL, 1, &m_device, NULL, NULL, &error);

            if ((NULL == m_context) || (CL_SUCCESS != error))
            {
                LOG_ERROR(CamxLogGroupChi, "Error creating an OpenCL context: %d", error);
                result = CDKResultEUnsupported;
            }
        }

        if (CDKResultSuccess == result)
        {
            m_queue = m_pfnCLCreateCommandQueue(m_context, m_device, 0, &error);

            if ((NULL == m_queue) || (CL_SUCCESS != error))
            {
                LOG_ERROR(CamxLogGroupChi, "Error creating the OpenCL Command Queue: %d", error);
                result = CDKResultEUnsupported;
            }
        }
    }

    if (CDKResultSuccess == result)
    {
        result = InitializeKernel();
    }

    if (CDKResultSuccess == result)
    {
        result = InitializeResources();
    }

    if (CDKResultSuccess == result)
    {
        m_pOpenCLMutex = CamX::Mutex::Create("OpenCLObject");

        if (NULL == m_pOpenCLMutex)
        {
            result = CDKResultENoMemory;
        }
    }

    if (CDKResultSuccess == result)
    {
        m_initStatus = CLInitDone;
    }
    else
    {
        m_initStatus = CLInitInvalid;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::InitializeResources
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::InitializeResources()
{
    cl_weight_image_desc_qcom weightDesc;
    cl_image_format           weightFmt;
    cl_half                   ds4Weights[2][8];
    cl_mem_ion_host_ptr       ionMem                 = { { 0 } };
    cl_image_format           input_1x1_image_format = { CL_RGBA, CL_UNSIGNED_INT32 };
    cl_image_desc             image_desc             = { 0 };
    CDKResult                 result                 = CDKResultSuccess;
    cl_int                    error                  = CL_FALSE;

    weightFmt.image_channel_data_type = CL_HALF_FLOAT;
    weightFmt.image_channel_order     = CL_R;

    memset(&weightDesc, 0, sizeof(cl_weight_image_desc_qcom));
    weightDesc.image_desc.image_type = CL_MEM_OBJECT_WEIGHT_IMAGE_QCOM;

    weightDesc.image_desc.image_width      = 8;
    weightDesc.image_desc.image_height     = 8;
    weightDesc.image_desc.image_array_size = 1;
    // Specify separable filter, default (flags=0) is 2D convolution filter
    weightDesc.weight_desc.flags           = CL_WEIGHT_IMAGE_SEPARABLE_QCOM;
    weightDesc.weight_desc.center_coord_x  = 3;
    weightDesc.weight_desc.center_coord_y  = 3;

    // Initialize weights
    static float ds4WeightsFloat[] = {
        (125.f / 1024.f),
        (91.f / 1024.f),
        (144.f / 1024.f),
        (152.f / 1024.f),
        (152.f / 1024.f),
        (144.f / 1024.f),
        (91.f / 1024.f),
        (125.f / 1024.f)
    };

    for (int i = 0; i < 8; i++)
    {
        ds4Weights[0][i] = (cl_half)ChiNodeUtils::FloatToHalf(ds4WeightsFloat[i]);
        ds4Weights[1][i] = (cl_half)ChiNodeUtils::FloatToHalf(ds4WeightsFloat[i]);
    }

    m_ds4WeightsImage = m_pfnCLCreateImage(m_context,
                                           CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                           &weightFmt,
                                           (cl_image_desc*)&weightDesc,
                                           (void*)ds4Weights,
                                           &error);

    if (CL_SUCCESS != error)
    {
        LOG_ERROR(CamxLogGroupChi, "Failed to create ds4 weights image with error %d", error);
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        // Create sampler
        m_ds4Sampler = m_pfnCLCreateSampler(m_context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, &error);

        if (CL_SUCCESS != error)
        {
            LOG_ERROR(CamxLogGroupChi, "Failed to create ds4 sampler with error %d", error);
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::InitializeKernel
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::InitializeKernel()
{
    CDKResult   result  = CDKResultSuccess;
    cl_int      err     = CL_FALSE;

    m_program = m_pfnCLCreateProgramWithSource(m_context, 1, reinterpret_cast<const CHAR**>(&m_pProgramSource), NULL, &err);

    if (CL_SUCCESS != err)
    {
        LOG_ERROR(CamxLogGroupChi, "clCreateProgramWithSource failed: error: %d", err);
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        err = m_pfnCLBuildProgram(m_program, 1, &m_device, NULL, NULL, NULL);

        if (CL_SUCCESS != err)
        {
            CHAR log[512] = "\0";
            m_pfnCLGetProgramBuildInfo(m_program, m_device, CL_PROGRAM_BUILD_LOG, sizeof(log), log, NULL);
            LOG_ERROR(CamxLogGroupChi, "clBuildProgram failed: error: %d, Log: %s", err, log);
            result = CDKResultEFailed;
        }
    }

    if (CDKResultSuccess == result)
    {
        // Create the copy kernel
        m_copyImageKernel = m_pfnCLCreateKernel(m_program, "copyImage", &err);

        if (CL_SUCCESS != err)
        {
            if (NULL != m_copyImageKernel)
            {
                m_pfnCLReleaseKernel(m_copyImageKernel);
                m_copyImageKernel = NULL;
            }

            LOG_ERROR(CamxLogGroupChi, "clCreateKernel for the CopyImage Kernel failed: error: %d", err);
            result = CDKResultEFailed;
        }
    }

    if (CDKResultSuccess == result)
    {
        // Create the rotation kernel
        m_rotateImageKernel = m_pfnCLCreateKernel(m_program, "rotateImage", &err);

        if (CL_SUCCESS != err)
        {
            if (NULL != m_rotateImageKernel)
            {
                m_pfnCLReleaseKernel(m_rotateImageKernel);
                m_rotateImageKernel = NULL;
            }

            LOG_ERROR(CamxLogGroupChi, "clCreateKernel for the RotateImage Kernel failed: error: %d", err);
            result = CDKResultEFailed;
        }
    }


    if (CDKResultSuccess == result)
    {
        // Create the flip kernel
        m_flipImageKernel = m_pfnCLCreateKernel(m_program, "flipImage", &err);


        if (CL_SUCCESS != err)
        {
            if (NULL != m_flipImageKernel)
            {
                m_pfnCLReleaseKernel(m_flipImageKernel);
                m_flipImageKernel = NULL;
            }

            LOG_ERROR(CamxLogGroupChi, "clCreateKernel for the FlipImage Kernel failed: error: %d", err);
            result = CDKResultEFailed;
        }
    }

    if (CDKResultSuccess == result)
    {
        // Create the ds4 kernel
        m_ds4SinglePlaneKernel = m_pfnCLCreateKernel(m_program, "ds4_single_plane", &err);

        if (CL_SUCCESS != err)
        {
            if (NULL != m_ds4SinglePlaneKernel)
            {
                m_pfnCLReleaseKernel(m_ds4SinglePlaneKernel);
                m_ds4SinglePlaneKernel = NULL;
            }

            LOG_ERROR(CamxLogGroupChi, "clCreateKernel for the DS4SinglePlane Kernel failed: error: %d", err);
            result = CDKResultEFailed;
        }
    }


    if (CDKResultSuccess == result)
    {
        // Create the box filter kernel
        m_boxFilterSinglePlaneKernel = m_pfnCLCreateKernel(m_program, "boxfilter_single_plane", &err);

        if (CL_SUCCESS != err)
        {
            if (NULL != m_boxFilterSinglePlaneKernel)
            {
                m_pfnCLReleaseKernel(m_boxFilterSinglePlaneKernel);
                m_boxFilterSinglePlaneKernel = NULL;
            }

            LOG_ERROR(CamxLogGroupChi, "clCreateKernel for the BoxFilterSinglePlane Kernel failed: error: %d", err);
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::CreateSeparateYUVImagePlanes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::CreateSeparateYUVImagePlanes(
    CHINODEBUFFERHANDLE hBuffer,
    UINT                imageIndex,
    cl_mem_flags        memoryAccessFlags,
    cl_mem*             pYUVImage,
    cl_mem*             pYImage,
    cl_mem*             pUVImage)
{
    CDKResult   result = CDKResultSuccess;
    cl_int      err = CL_SUCCESS;

    LOG_INFO(CamxLogGroupChi,
             " Create Separate Image input params"
             " yuvFormat[0].height       = %d"
             " yuvFormat[0].width        = %d"
             " yuvFormat[0].planeStride  = %d"
             " yuvFormat[0].sliceHeight  = %d"
             " yuvFormat[1].height       = %d"
             " yuvFormat[1].width        = %d"
             " yuvFormat[1].planeStride  = %d"
             " yuvFormat[1].sliceHeight  = %d",
             hBuffer->format.formatParams.yuvFormat[0].height,
             hBuffer->format.formatParams.yuvFormat[0].width,
             hBuffer->format.formatParams.yuvFormat[0].planeStride,
             hBuffer->format.formatParams.yuvFormat[0].sliceHeight,
             hBuffer->format.formatParams.yuvFormat[1].height,
             hBuffer->format.formatParams.yuvFormat[1].width,
             hBuffer->format.formatParams.yuvFormat[1].planeStride,
             hBuffer->format.formatParams.yuvFormat[1].sliceHeight);

    cl_image_desc       imageDesc = {0};
    cl_mem_ion_host_ptr ionMem    = {{0}};

    ionMem.ext_host_ptr.allocation_type     = CL_MEM_ION_HOST_PTR_QCOM;
    ionMem.ext_host_ptr.host_cache_policy   = CL_MEM_HOST_UNCACHED_QCOM;
    ionMem.ion_filedesc                     = hBuffer->pImageList[imageIndex].fd[0];
    ionMem.ion_hostptr                      = hBuffer->pImageList[imageIndex].pAddr[0];

    /// @note Passing in actual height and using image_slice_pitch should be the right way; we need to double check with CL
    ///       team
    imageDesc.image_type           = CL_MEM_OBJECT_IMAGE2D;
    imageDesc.image_width          = hBuffer->format.width;
    imageDesc.image_height         = hBuffer->format.height;
    imageDesc.image_row_pitch      = 0;
    imageDesc.image_slice_pitch    = 0;

    /// @todo (CAMX-2286) Improvements to GPU Node Support: Add support for UBWC, TP10 etc
    cl_image_format   imageFormat     = { CL_QCOM_NV12,       CL_UNORM_INT8 };
    cl_image_format   imageYFormat    = { CL_QCOM_NV12_Y,     CL_UNORM_INT8 };
    cl_image_format   imageUVFormat   = { CL_QCOM_NV12_UV,    CL_UNORM_INT8 };

    switch (hBuffer->format.format)
    {
        case P010:
            imageFormat     = { CL_QCOM_P010,       CL_QCOM_UNORM_INT10 };
            imageYFormat    = { CL_QCOM_P010_Y,     CL_QCOM_UNORM_INT10 };
            imageUVFormat   = { CL_QCOM_P010_UV,    CL_QCOM_UNORM_INT10 };
            break;
        default:
            // Unexpected format, default to NV12
            LOG_ERROR(CamxLogGroupChi, "CHINODEGPU: Unexpected format of %u, defaulting to NV12", hBuffer->format.format);
        case YUV420NV12:
            imageFormat     = { CL_QCOM_NV12,       CL_UNORM_INT8 };
            imageYFormat    = { CL_QCOM_NV12_Y,     CL_UNORM_INT8 };
            imageUVFormat   = { CL_QCOM_NV12_UV,    CL_UNORM_INT8 };
            break;
    }

    *pYUVImage = m_pfnCLCreateImage(m_context,
                                    (CL_MEM_EXT_HOST_PTR_QCOM | CL_MEM_READ_ONLY),
                                    &imageFormat,
                                    &imageDesc,
                                    reinterpret_cast<VOID*>(&ionMem),
                                    &err);

    if ((CL_SUCCESS == err) && (NULL != *pYUVImage))
    {
        imageDesc.image_type           = CL_MEM_OBJECT_IMAGE2D;
        imageDesc.image_width          = hBuffer->format.width;
        imageDesc.image_height         = hBuffer->format.height;
        imageDesc.image_array_size     = 1;
        imageDesc.image_row_pitch      = 0;
        imageDesc.image_slice_pitch    = 0;
        imageDesc.mem_object           = (cl_mem)(*pYUVImage);

        *pYImage    = m_pfnCLCreateImage(m_context, memoryAccessFlags, &imageYFormat, &imageDesc, NULL, &err);

        if (CL_SUCCESS == err)
        {
            *pUVImage   = m_pfnCLCreateImage(m_context, memoryAccessFlags, &imageUVFormat, &imageDesc, NULL, &err);
        }
    }

    if ((NULL == *pYUVImage) || (NULL == *pYImage) || (NULL == *pUVImage))
    {
        LOG_ERROR(CamxLogGroupChi, "clCreateImage failed: error: %d", err);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::ReleaseSeparateYUVImagePlanes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::ReleaseSeparateYUVImagePlanes(
    cl_mem yuvImage,
    cl_mem yImage,
    cl_mem uvImage)
{
    CDKResult result = CDKResultSuccess;

    if (NULL != yImage)
    {
        m_pfnCLReleaseMemObject(yImage);
        yImage = NULL;
    }

    if (NULL != uvImage)
    {
        m_pfnCLReleaseMemObject(uvImage);
        uvImage = NULL;
    }

    if (NULL != yuvImage)
    {
        m_pfnCLReleaseMemObject(yuvImage);
        yuvImage = NULL;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::ExecuteCopyImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::ExecuteCopyImage(
    cl_mem  dst,
    cl_mem  src,
    UINT32  width,
    UINT32  height)
{
    CDKResult   result          = CDKResultSuccess;
    cl_int      err             = CL_SUCCESS;
    size_t      globalSize[2]   = {width, height};

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_copyImageKernel, 0, sizeof(cl_mem), &dst);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_copyImageKernel, 1, sizeof(cl_mem), &src);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLEnqueueNDRangeKernel(m_queue, m_copyImageKernel, 2, NULL, &globalSize[0], NULL, 0, NULL, NULL);
    }

    if (CL_SUCCESS != err)
    {
        LOG_ERROR(CamxLogGroupChi, "ExecuteCopyImage failed: error: %d", err);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::ExecuteRotateImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::ExecuteRotateImage(
    cl_mem          dst,
    cl_mem          src,
    UINT32          dstWidth,
    UINT32          dstHeight,
    UINT32          srcWidth,
    UINT32          srcHeight,
    RotationAngle   rotation)
{
    CDKResult   result        = CDKResultSuccess;
    cl_int      err           = CL_SUCCESS;
    size_t      globalSize[2] = { dstWidth, dstHeight };
    FLOAT       sinTheta;
    FLOAT       cosTheta;

    switch (rotation)
    {
    case RotationAngle::Rotate0Degrees:
        sinTheta = 0.0f;
        cosTheta = 1.0f;
        break;
    case RotationAngle::Rotate90Degrees:
        sinTheta = 1.0f;
        cosTheta = 0.0f;
        break;
    case RotationAngle::Rotate180Degrees:
        sinTheta = 0.0f;
        cosTheta = -1.0f;
        break;
    case RotationAngle::Rotate270Degrees:
        sinTheta = -1.0f;
        cosTheta = 0.0f;
        break;
    default:
        LOG_ERROR(CamxLogGroupChi, "Unknown rotation angle %d", rotation);
        err = CL_INVALID_VALUE;
        break;
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_rotateImageKernel, 0, sizeof(cl_mem), &dst);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_rotateImageKernel, 1, sizeof(cl_mem), &src);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_rotateImageKernel, 2, sizeof(UINT32), &dstWidth);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_rotateImageKernel, 3, sizeof(UINT32), &dstHeight);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_rotateImageKernel, 4, sizeof(UINT32), &srcWidth);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_rotateImageKernel, 5, sizeof(UINT32), &srcHeight);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_rotateImageKernel, 6, sizeof(FLOAT), &sinTheta);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_rotateImageKernel, 7, sizeof(FLOAT), &cosTheta);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLEnqueueNDRangeKernel(m_queue, m_rotateImageKernel, 2, NULL, &globalSize[0], NULL, 0, NULL, NULL);
    }

    if (CL_SUCCESS != err)
    {
        LOG_ERROR(CamxLogGroupChi, "ExecuteRotateImage failed: error: %d", err);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::ExecuteFlipImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::ExecuteFlipImage(
    cl_mem        dst,
    cl_mem        src,
    UINT32        width,
    UINT32        height,
    UINT32        inStride,
    UINT32        outStride,
    UINT32        inUVOffset,
    UINT32        outUVOffset,
    FlipDirection direction)
{
    CDKResult   result          = CDKResultSuccess;
    cl_int      err             = CL_SUCCESS;
    size_t      globalSize[2]   = {width, height};

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 0, sizeof(cl_mem), &dst);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 1, sizeof(cl_mem), &src);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 2, sizeof(UINT32), &width);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 3, sizeof(UINT32), &height);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 4, sizeof(UINT32), &inStride);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 5, sizeof(UINT32), &outStride);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 6, sizeof(UINT32), &inUVOffset);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 7, sizeof(UINT32), &outUVOffset);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_flipImageKernel, 8, sizeof(UINT32), &direction);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLEnqueueNDRangeKernel(m_queue, m_flipImageKernel, 2, NULL, &globalSize[0], NULL, 0, NULL, NULL);
    }

    if (CL_SUCCESS != err)
    {
        LOG_ERROR(CamxLogGroupChi, "ExecuteFlipImage failed: error: %d", err);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::CopyImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::CopyBuffer(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput)
{
    CDKResult result = CDKResultSuccess;

    cl_int   err = CL_SUCCESS;

    LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::CopyBuffer : NOT TESTED");

    for (UINT i = 0; i < hOutput->imageCount; i++)
    {
        cl_mem  dstBuffer = NULL;
        cl_mem  srcBuffer = NULL;

        UINT32  width           = hInput->format.width;
        UINT32  height          = hInput->format.height;
        UINT32  inStride        = hInput->format.formatParams.yuvFormat[0].planeStride;
        UINT32  outStride       = hOutput->format.formatParams.yuvFormat[0].planeStride;
        UINT32  inHeightStride  = hInput->format.formatParams.yuvFormat[0].sliceHeight;
        UINT32  outHeightStride = hInput->format.formatParams.yuvFormat[0].sliceHeight;

        UINT32 totalSize = 0;
        for (UINT32 j = 0; j < hOutput->numberOfPlanes; j++)
        {
            totalSize += hOutput->planeSize[j];
        }

        // Create 1x1 Buffer for input on top of the original ion allocation
        cl_mem_ion_host_ptr ionMem              = { {0} };
        ionMem.ext_host_ptr.allocation_type     = CL_MEM_ION_HOST_PTR_QCOM;
        ionMem.ext_host_ptr.host_cache_policy   = CL_MEM_HOST_UNCACHED_QCOM;
        ionMem.ion_filedesc                     = hInput->pImageList[i].fd[0];
        ionMem.ion_hostptr                      = hInput->pImageList[i].pAddr[0];

        srcBuffer = m_pfnCLCreateBuffer(m_context,
                                        (CL_MEM_READ_WRITE | CL_MEM_EXT_HOST_PTR_QCOM),
                                        totalSize,
                                        reinterpret_cast<VOID*>(&ionMem),
                                        &err);

        if (CL_SUCCESS == err)
        {
            // Create 1x1 Buffer for output on top of the original ion allocation
            ionMem.ext_host_ptr.allocation_type     = CL_MEM_ION_HOST_PTR_QCOM;
            ionMem.ext_host_ptr.host_cache_policy   = CL_MEM_HOST_UNCACHED_QCOM;
            ionMem.ion_filedesc                     = hOutput->pImageList[i].fd[0];
            ionMem.ion_hostptr                      = hOutput->pImageList[i].pAddr[0];

            dstBuffer = m_pfnCLCreateBuffer(m_context,
                                            (CL_MEM_READ_WRITE | CL_MEM_EXT_HOST_PTR_QCOM),
                                            totalSize,
                                            reinterpret_cast<VOID*>(&ionMem),
                                            &err);
        }

        if (CL_SUCCESS == err)
        {
             err = m_pfnCLEnqueueCopyBuffer(m_queue, srcBuffer, dstBuffer, 0, 0, totalSize, 0, NULL, NULL);
        }

        if (CL_SUCCESS == err)
        {
            err = m_pfnCLFinish(m_queue);
        }

        if (NULL != srcBuffer)
        {
            m_pfnCLReleaseMemObject(srcBuffer);
            srcBuffer = NULL;
        }

        if (NULL != dstBuffer)
        {
            m_pfnCLReleaseMemObject(dstBuffer);
            dstBuffer = NULL;
        }

        if (CL_SUCCESS != err)
        {
            result = CDKResultEFailed;
            break;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::CopyImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::CopyImage(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput)
{
    CDKResult   result  = CDKResultSuccess;
    cl_int      err     = CL_SUCCESS;

    /// @note This code only works for Linear NV12 at the moment.
    // CAMX_ASSERT(hInput->formatParams.format.format == YUV420NV12);
    // CAMX_ASSERT(hInput->numberOfPlanes == 2);
    for (UINT i = 0; i < hOutput->imageCount; i++)
    {
        cl_mem  dstYUVImage = NULL;
        cl_mem  srcYUVImage = NULL;
        cl_mem  dstYImage   = NULL;
        cl_mem  srcYImage   = NULL;
        cl_mem  dstUVImage  = NULL;
        cl_mem  srcUVImage  = NULL;
        UINT32  width   = hInput->format.width;
        UINT32  height  = hInput->format.height;
        // UINT32  sliceHeightY  = hInput->format.formatParams.yuvFormat[0].sliceHeight;
        // UINT32  sliceHeightUV = hInput->format.formatParams.yuvFormat[1].sliceHeight;

        /// @todo (CAMX-2286) Improvements to GPU Node Support: Need to avoid calling Create and Release
        ///                   ION buffers per-process request
        result = CreateSeparateYUVImagePlanes(hOutput, i, CL_MEM_WRITE_ONLY, &dstYUVImage, &dstYImage, &dstUVImage);

        if (CDKResultSuccess == result)
        {
            result = CreateSeparateYUVImagePlanes(hInput, i, CL_MEM_READ_ONLY, &srcYUVImage, &srcYImage, &srcUVImage);
        }

        m_pOpenCLMutex->Lock();
        if (CDKResultSuccess == result)
        {
           if ((NULL != dstYImage) && (NULL != srcYImage))
           {
               result = ExecuteCopyImage(dstYImage, srcYImage, width, height);
           }

           if ((NULL != dstUVImage) && (NULL != srcUVImage))
           {
               UINT32 uvPlaneWidth = (hInput->format.format == P010) ? width : (width >> 1);
               result = ExecuteCopyImage(dstUVImage, srcUVImage, uvPlaneWidth, height >> 1);
           }
        }

        /// @todo (CAMX-2286) Improvements to GPU Node Support: We shouldn't be calling clFinish here. Instead, we need to use
        ///                   events and spawn a thread to wait on those events, and then signal the buffer to unblock the
        ///                   next node in the pipeline.
        if (CDKResultSuccess == result)
        {
            err = m_pfnCLFinish(m_queue);
        }

        if (CL_SUCCESS != err)
        {
            result = CDKResultEFailed;
        }
        m_pOpenCLMutex->Unlock();

        ReleaseSeparateYUVImagePlanes(srcYUVImage, srcYImage, srcUVImage);
        ReleaseSeparateYUVImagePlanes(dstYUVImage, dstYImage, dstUVImage);
    }

    if (CDKResultSuccess != result)
    {
        LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::CopyImage failed: error: %d", result);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::RotateImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::RotateImage(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput,
    RotationAngle       targetRotation)
{
    CDKResult   result = CDKResultSuccess;
    cl_int      err = CL_SUCCESS;

    /// @note This code only works for Linear NV12 at the moment.
    // CAMX_ASSERT(hInput->formatParams.format.format == YUV420NV12);
    // CAMX_ASSERT(hInput->numberOfPlanes == 2);
    for (UINT i = 0; i < hOutput->imageCount; i++)
    {
        cl_mem  dstYUVImage = NULL;
        cl_mem  srcYUVImage = NULL;
        cl_mem  dstYImage   = NULL;
        cl_mem  srcYImage   = NULL;
        cl_mem  dstUVImage  = NULL;
        cl_mem  srcUVImage  = NULL;
        UINT32  dstWidth    = 0;
        UINT32  dstHeight   = 0;
        UINT32  srcWidth    = hInput->format.width;
        UINT32  srcHeight   = hInput->format.height;

        // If rotated, output buffer rotation needs to have width and height swapped
        if ((RotationAngle::Rotate90Degrees == targetRotation) ||
            (RotationAngle::Rotate270Degrees == targetRotation))
        {
            dstWidth = srcHeight;
            dstHeight = srcWidth;
        }
        else
        {
            dstWidth = srcWidth;
            dstHeight = srcHeight;
        }

        /// @todo (CAMX-2286) Improvements to GPU Node Support: Need to avoid calling Create and Release
        ///                   ION buffers per-process request
        result = CreateSeparateYUVImagePlanes(hOutput, i, CL_MEM_WRITE_ONLY, &dstYUVImage, &dstYImage, &dstUVImage);

        if (CDKResultSuccess == result)
        {
            result = CreateSeparateYUVImagePlanes(hInput, i, CL_MEM_READ_ONLY, &srcYUVImage, &srcYImage, &srcUVImage);
        }

        m_pOpenCLMutex->Lock();
        if (CDKResultSuccess == result)
        {
            if ((NULL != dstYImage) && (NULL != srcYImage))
            {
                result = ExecuteRotateImage(dstYImage,
                                            srcYImage,
                                            dstWidth,
                                            dstHeight,
                                            srcWidth,
                                            srcHeight,
                                            targetRotation);
            }
        }

        if (CDKResultSuccess == result)
        {
            if ((NULL != dstUVImage) && (NULL != srcUVImage))
            {
                result = ExecuteRotateImage(dstUVImage,
                                            srcUVImage,
                                            (dstWidth >> 1),
                                            (dstHeight >> 1),
                                            (srcWidth >> 1),
                                            (srcHeight >> 1),
                                            targetRotation);
            }
        }

        /// @todo (CAMX-2286) Improvements to GPU Node Support: We shouldn't be calling clFinish here. Instead, we need to use
        ///                   events and spawn a thread to wait on those events, and then signal the buffer to unblock the
        ///                   next node in the pipeline.
        if (CDKResultSuccess == result)
        {
            err = m_pfnCLFinish(m_queue);
        }
        m_pOpenCLMutex->Unlock();

        if (CL_SUCCESS != err)
        {
            result = CDKResultEFailed;
        }

        ReleaseSeparateYUVImagePlanes(srcYUVImage, srcYImage, srcUVImage);
        ReleaseSeparateYUVImagePlanes(dstYUVImage, dstYImage, dstUVImage);
    }

    if (CDKResultSuccess != result)
    {
        LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::CopyImage failed: error: %d", result);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::FlipImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::FlipImage(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput,
    FlipDirection       direction)
{
    CDKResult   result  = CDKResultSuccess;
    cl_int      err     = CL_SUCCESS;

    /// @note This code only works for Linear NV12 at the moment.
    // CAMX_ASSERT(hInput->formatParams.format.format == YUV420NV12);
    // CAMX_ASSERT(hInput->numberOfPlanes == 2);
    for (UINT i = 0; i < hOutput->imageCount; i++)
    {
        cl_mem  dstYBuffer  = 0;
        cl_mem  srcYBuffer  = 0;
        cl_mem  dstUVBuffer = 0;
        cl_mem  srcUVBuffer = 0;
        UINT32  width       = hInput->format.width;
        UINT32  height      = hInput->format.height;
        UINT32  inStride    = hInput->format.formatParams.yuvFormat[0].planeStride;
        UINT32  outStride   = hOutput->format.formatParams.yuvFormat[0].planeStride;
        UINT32  inUVOffset  = 0;
        UINT32  outUVOffset = 0;

        LOG_ERROR(CamxLogGroupChi, "Input width: %d, inStride is %d, output stride is %d ", width, inStride, outStride);


        cl_mem_ion_host_ptr ionmemDstY              = {{0}};
        ionmemDstY.ext_host_ptr.allocation_type     = CL_MEM_ION_HOST_PTR_QCOM;
        ionmemDstY.ext_host_ptr.host_cache_policy   = CL_MEM_HOST_UNCACHED_QCOM;
        ionmemDstY.ion_filedesc                     = hOutput->pImageList[i].fd[0];
        ionmemDstY.ion_hostptr                      = hOutput->pImageList[i].pAddr[0];

        /// @todo (CAMX-2286) Improvements to GPU Node Support: Need to avoid calling Create and Release
        ///                   ION buffers per-process request
        dstYBuffer = m_pfnCLCreateBuffer(m_context,
                                         (CL_MEM_USE_HOST_PTR | CL_MEM_EXT_HOST_PTR_QCOM),
                                         (inStride * height),
                                         reinterpret_cast<VOID*>(&ionmemDstY),
                                         &err);
        if (CL_SUCCESS != err)
        {
            LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::FlipImage failed to create clBuffer dstY: error: %d", err);
        }

        cl_mem_ion_host_ptr ionmemDstUV              = {{0}};
        ionmemDstUV.ext_host_ptr.allocation_type     = CL_MEM_ION_HOST_PTR_QCOM;
        ionmemDstUV.ext_host_ptr.host_cache_policy   = CL_MEM_HOST_UNCACHED_QCOM;
        ionmemDstUV.ion_filedesc                     = hOutput->pImageList[i].fd[1];
        ionmemDstUV.ion_hostptr                      = hOutput->pImageList[i].pAddr[1];

        dstUVBuffer = m_pfnCLCreateBuffer(m_context,
                                          (CL_MEM_USE_HOST_PTR | CL_MEM_EXT_HOST_PTR_QCOM),
                                          (inStride * height / 2),
                                          reinterpret_cast<VOID*>(&ionmemDstUV),
                                          &err);
        if (CL_SUCCESS != err)
        {
            LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::FlipImage failed to create clBuffer dstUV: error: %d", err);
        }

        cl_mem_ion_host_ptr ionmemSrcY              = {{0}};
        ionmemSrcY.ext_host_ptr.allocation_type     = CL_MEM_ION_HOST_PTR_QCOM;
        ionmemSrcY.ext_host_ptr.host_cache_policy   = CL_MEM_HOST_UNCACHED_QCOM;
        ionmemSrcY.ion_filedesc                     = hInput->pImageList[i].fd[0];
        ionmemSrcY.ion_hostptr                      = hInput->pImageList[i].pAddr[0];

        srcYBuffer = m_pfnCLCreateBuffer(m_context,
                                         (CL_MEM_USE_HOST_PTR | CL_MEM_EXT_HOST_PTR_QCOM),
                                         (inStride * height),
                                         reinterpret_cast<VOID*>(&ionmemSrcY),
                                         &err);
        if (CL_SUCCESS != err)
        {
            LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::FlipImage failed to create clBuffer srcY: error: %d", err);
        }

        cl_mem_ion_host_ptr ionmemSrcUV              = {{0}};
        ionmemSrcUV.ext_host_ptr.allocation_type     = CL_MEM_ION_HOST_PTR_QCOM;
        ionmemSrcUV.ext_host_ptr.host_cache_policy   = CL_MEM_HOST_UNCACHED_QCOM;
        ionmemSrcUV.ion_filedesc                     = hInput->pImageList[i].fd[1];
        ionmemSrcUV.ion_hostptr                      = hInput->pImageList[i].pAddr[1];

        srcUVBuffer = m_pfnCLCreateBuffer(m_context,
                                          (CL_MEM_USE_HOST_PTR | CL_MEM_EXT_HOST_PTR_QCOM),
                                          (inStride * height / 2),
                                          reinterpret_cast<VOID*>(&ionmemSrcUV),
                                          &err);
        if (CL_SUCCESS != err)
        {
            LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::FlipImage failed to create clBuffer srcUV: error: %d", err);
            result  = CDKResultEFailed;
        }

        m_pOpenCLMutex->Lock();
        if (CDKResultSuccess == result)
        {
            if ((NULL != dstYBuffer) && (NULL != srcYBuffer))
            {
                result = ExecuteFlipImage(dstYBuffer,
                                        srcYBuffer,
                                        width,
                                        height,
                                        inStride,
                                        outStride,
                                        0,
                                        0,
                                        direction);
            }
        }

        if (CDKResultSuccess == result)
        {
            if ((NULL != dstUVBuffer) && (NULL != srcUVBuffer))
            {
                inUVOffset  = hInput->pImageList[i].pAddr[1] - hInput->pImageList[i].pAddr[0];
                outUVOffset = hOutput->pImageList[i].pAddr[1] - hOutput->pImageList[i].pAddr[0];
                result      = ExecuteFlipImage(dstUVBuffer,
                                            srcUVBuffer,
                                            (width >> 1),
                                            (height >> 1),
                                            inStride,
                                            outStride,
                                            inUVOffset,
                                            outUVOffset,
                                            direction);
            }
        }

        /// @todo (CAMX-2286) Improvements to GPU Node Support: We shouldn't be calling clFinish here. Instead, we need to use
        ///                   events and spawn a thread to wait on those events, and then signal the buffer to unblock the
        ///                   next node in the pipeline.
        if (CDKResultSuccess == result)
        {
            err = m_pfnCLFinish(m_queue);
        }
        m_pOpenCLMutex->Unlock();

        if (CL_SUCCESS != err)
        {
            result = CDKResultEFailed;
        }

        if (0 != dstYBuffer)
        {
            m_pfnCLReleaseMemObject(dstYBuffer);
        }
        if (0 != dstUVBuffer)
        {
            m_pfnCLReleaseMemObject(dstUVBuffer);
        }
        if (0 != srcYBuffer)
        {
            m_pfnCLReleaseMemObject(srcYBuffer);
        }
        if (0 != srcUVBuffer)
        {
            m_pfnCLReleaseMemObject(srcUVBuffer);
        }
    }

    if (CDKResultSuccess != result)
    {
        LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::FlipImage failed: error: %d", result);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::DownscaleImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::DownscaleBy4Image(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput)
{
    CDKResult   result = CDKResultSuccess;
    cl_int      err = CL_SUCCESS;

    /// @note This code only works for Linear NV12 at the moment.
    // CAMX_ASSERT(hInput->formatParams.format.format == YUV420NV12);
    // CAMX_ASSERT(hInput->numberOfPlanes == 2);
    for (UINT i = 0; i < hOutput->imageCount; i++)
    {
        cl_mem  dstYUVImage;
        cl_mem  srcYUVImage;
        cl_mem  dstYImage;
        cl_mem  srcYImage;
        cl_mem  dstUVImage;
        cl_mem  srcUVImage;
        UINT32  srcWidth  = hInput->format.width;
        UINT32  srcHeight = hInput->format.height;
        UINT32  dstWidth  = ChiNodeUtils::DivideAndCeil(srcWidth, 4) * 2;
        UINT32  dstHeight = ChiNodeUtils::DivideAndCeil(srcHeight, 4) * 2;

        // 20.0f is roughly large value not to use srcYMax in CL Kernel
        FLOAT   srcYMax   = hInput->format.height + 20.f;

        // Slice height being passed in the format is bogus, calculating manually until that is fixed. (this was due to a packing issue)
        UINT32  srcSliceHeightY  = hInput->format.formatParams.yuvFormat[0].sliceHeight; // ChiNodeUtils::AlignGeneric32(srcHeight, 32);
        UINT32  srcSliceHeightUV = hInput->format.formatParams.yuvFormat[1].sliceHeight; // ChiNodeUtils::AlignGeneric32((srcHeight / 2), 16);

        UINT32  dstSliceHeightY  = hOutput->format.formatParams.yuvFormat[0].sliceHeight;
        UINT32  dstSliceHeightUV = hOutput->format.formatParams.yuvFormat[1].sliceHeight;

        /// @todo (CAMX-2286) Improvements to GPU Node Support: Need to avoid calling Create and Release
        ///                   ION buffers per-process request
        result = CreateSeparateYUVImagePlanes(hOutput, i, CL_MEM_WRITE_ONLY, &dstYUVImage, &dstYImage, &dstUVImage);
        // CAMX_ASSERT(CDKResultSuccess == result);
        result = CreateSeparateYUVImagePlanes(hInput, i, CL_MEM_READ_ONLY, &srcYUVImage, &srcYImage, &srcUVImage);
        // CAMX_ASSERT(CDKResultSuccess == result);

        m_pOpenCLMutex->Lock();

        if ((CDKResultSuccess == result) && (NULL != dstYImage) && (NULL != srcYImage))
        {
            result = ExecuteDownscaleBy4SinglePlane(dstYImage,
                                                    srcYImage,
                                                    dstWidth,
                                                    dstHeight,
                                                    srcYMax);
            // CAMX_ASSERT(CDKResultSuccess == result);
        }

        if ((CDKResultSuccess == result) && (NULL != dstUVImage) && (NULL != srcUVImage))
        {
            result = ExecuteBoxFilterSinglePlane(dstUVImage,
                                                 srcUVImage,
                                                 4.0f,
                                                 4.0f,
                                                 (dstWidth  >> 1),
                                                 (dstHeight >> 1),
                                                 srcYMax);
            // CAMX_ASSERT(CDKResultSuccess == result);
        }

        /// @todo (CAMX-2286) Improvements to GPU Node Support: We shouldn't be calling clFinish here. Instead, we need to use
        ///                   events and spawn a thread to wait on those events, and then signal the buffer to unblock the
        ///                   next node in the pipeline.
        if (CDKResultSuccess == result)
        {
            err = m_pfnCLFinish(m_queue);
        }

        m_pOpenCLMutex->Unlock();

        ReleaseSeparateYUVImagePlanes(srcYUVImage, srcYImage, srcUVImage);
        ReleaseSeparateYUVImagePlanes(dstYUVImage, dstYImage, dstUVImage);

        if (CL_SUCCESS != err)
        {
            result = CDKResultEFailed;
        }

        if (CDKResultSuccess != result)
        {
            // Don't continue trying to execute if anything has failed.
            break;
        }
    }

    if (CDKResultSuccess != result)
    {
        LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::DownscaleBy4Image failed: result: %d, error: %d", result, err);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::ExecuteDownscaleBy4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::ExecuteDownscaleBy4SinglePlane(
    cl_mem          dst,
    cl_mem          src,
    UINT32          dstWidth,
    UINT32          dstHeight,
    FLOAT           srcYMax)
{
    CDKResult   result        = CDKResultSuccess;
    cl_int      err           = CL_SUCCESS;
    UINT        argIdx        = 0;
    // Set the global work size to the 4x1 scale
    size_t      globalSize[2] = { dstWidth, dstHeight };

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_ds4SinglePlaneKernel, argIdx++, sizeof(cl_mem), &src);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_ds4SinglePlaneKernel, argIdx++, sizeof(cl_mem), &dst);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_ds4SinglePlaneKernel, argIdx++, sizeof(cl_mem), &m_ds4WeightsImage);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_ds4SinglePlaneKernel, argIdx++, sizeof(cl_sampler), &m_ds4Sampler);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_ds4SinglePlaneKernel, argIdx++, sizeof(FLOAT), &srcYMax);
    }

    if (CL_SUCCESS != err)
    {
        LOG_ERROR(CamxLogGroupChi, "ExecuteDownscaleBy4SinglePlane failed setting arg %u with err %d", argIdx, err);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLEnqueueNDRangeKernel(m_queue, m_ds4SinglePlaneKernel, 2, NULL, &globalSize[0], NULL, 0, NULL, NULL);
    }

    if (CL_SUCCESS != err)
    {
        LOG_ERROR(CamxLogGroupChi, "ExecuteDownscaleBy4SinglePlane failed: error: %d", err);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::ExecuteBoxFilterSinglePlane
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::ExecuteBoxFilterSinglePlane(
    cl_mem          dst,
    cl_mem          src,
    FLOAT           scaleX,
    FLOAT           scaleY,
    UINT32          dstWidth,
    UINT32          dstHeight,
    FLOAT           srcYMax)
{
    CDKResult   result        = CDKResultSuccess;
    cl_int      err           = CL_SUCCESS;
    UINT        argIdx        = 0;
    // Set the global work size to the 4x1 scale
    size_t      globalSize[2] = { dstWidth, dstHeight };

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_boxFilterSinglePlaneKernel, argIdx++, sizeof(cl_mem), &src);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_boxFilterSinglePlaneKernel, argIdx++, sizeof(cl_mem), &dst);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_boxFilterSinglePlaneKernel, argIdx++, sizeof(cl_sampler), &m_ds4Sampler);
    }

    if (CL_SUCCESS == err)
    {
        cl_box_filter_size_qcom scaleFactors;

        scaleFactors.box_filter_width  = scaleX;
        scaleFactors.box_filter_height = scaleY;

        err = m_pfnCLSetKernelArg(m_boxFilterSinglePlaneKernel, argIdx++, sizeof(cl_box_filter_size_qcom), &scaleFactors);
    }

    if (CL_SUCCESS == err)
    {
        cl_float2 unpackedScales;

        unpackedScales.s[0] = scaleX;
        unpackedScales.s[1] = scaleY;

        err = m_pfnCLSetKernelArg(m_boxFilterSinglePlaneKernel, argIdx++, sizeof(cl_float2), &unpackedScales);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLSetKernelArg(m_boxFilterSinglePlaneKernel, argIdx++, sizeof(FLOAT), &srcYMax);
    }

    if (CL_SUCCESS != err)
    {
        LOG_ERROR(CamxLogGroupChi, "ExecuteBoxFilterSinglePlane failed setting arg %u with err %d", argIdx, err);
    }

    if (CL_SUCCESS == err)
    {
        err = m_pfnCLEnqueueNDRangeKernel(m_queue, m_boxFilterSinglePlaneKernel, 2, NULL, &globalSize[0], NULL, 0, NULL, NULL);
    }

    if (CL_SUCCESS != err)
    {
        LOG_ERROR(CamxLogGroupChi, "ExecuteBoxFilterSinglePlane failed: error: %d", err);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GPUOpenCL::Uninitialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GPUOpenCL::Uninitialize()
{
    CDKResult result = CDKResultSuccess;

    cl_int err = CL_SUCCESS;

    if (NULL != m_queue)
    {
        err = m_pfnCLReleaseCommandQueue(m_queue);
        LOG_ERROR(CamxLogGroupChi, "GPUOpenCL ReleaseCommandQueue error -0 means success-: %d", err);
        m_queue = NULL;
    }

    if (NULL != m_context)
    {
        err = m_pfnCLReleaseContext(m_context);
        LOG_ERROR(CamxLogGroupChi, "GPUOpenCL ReleaseContext error -0 means success-: %d", err);
        m_context = NULL;
    }

    if (NULL != m_copyImageKernel)
    {
        m_pfnCLReleaseKernel(m_copyImageKernel);
        m_copyImageKernel = NULL;
    }

    if (NULL != m_rotateImageKernel)
    {
        m_pfnCLReleaseKernel(m_rotateImageKernel);
        m_rotateImageKernel = NULL;
    }

    if (NULL != m_flipImageKernel)
    {
        m_pfnCLReleaseKernel(m_flipImageKernel);
        m_flipImageKernel = NULL;
    }

    if (NULL != m_ds4SinglePlaneKernel)
    {
        m_pfnCLReleaseKernel(m_ds4SinglePlaneKernel);
        m_ds4SinglePlaneKernel = NULL;
    }

    if (NULL != m_boxFilterSinglePlaneKernel)
    {
        m_pfnCLReleaseKernel(m_boxFilterSinglePlaneKernel);
        m_boxFilterSinglePlaneKernel = NULL;
    }

    if (NULL != m_ds4WeightsImage)
    {
        m_pfnCLReleaseMemObject(m_ds4WeightsImage);
        m_ds4WeightsImage = NULL;
    }

    if (NULL != m_ds4Sampler)
    {
        m_pfnCLReleaseSampler(m_ds4Sampler);
        m_ds4Sampler = NULL;
    }

    if (NULL != m_program)
    {
        m_pfnCLReleaseProgram(m_program);
        m_program = NULL;
    }

    if (NULL != m_hOpenCLLib)
    {
        result = ChiNodeUtils::LibUnmap(m_hOpenCLLib);
        m_hOpenCLLib = 0;
    }

    if (CDKResultSuccess != result)
    {
        LOG_ERROR(CamxLogGroupChi, "GPUOpenCL::Uninitialize Failed to unmap lib: %d", result);
    }

    m_initStatus = CLInitInvalid;

    return result;
}

// =============================================================================================================================
// END OpenCL Stuff
// =============================================================================================================================

// NOWHINE FILE CP040: Keyword new not allowed. Use the CAMX_NEW/CAMX_DELETE functions insteads

ChiNodeInterface g_ChiNodeInterface;    ///< The instance save the CAMX Chi interface
UINT32           g_vendorTagBase = 0;   ///< Chi assigned runtime vendor tag base for the node

/// @todo (CAMX-1854) Need to get the major / minor verion from Chi
static const UINT32 ChiNodeMajorVersion = 0;    ///< The major version of Chi interface
static const UINT32 ChiNodeMinorVersion = 0;    ///< The minor version of Chi interface

static const CHAR   GpuNodeSectionName[]         = "com.qti.node.gpu";  ///< The section name for node

static const UINT32 GpuNodeTagBase               = 0;                   ///< Tag base
static const UINT32 GpuNodeTagSupportedFeature   = GpuNodeTagBase + 0;  ///< Tag for supported features
static const UINT32 GpuNodeTagCurrentMode        = GpuNodeTagBase + 1;  ///< Tag to indicated current operation mode
static const UINT32 GpuNodeTagProcessedFrameNum  = GpuNodeTagBase + 2;  ///< Tag to show processed frame's count
static const UINT32 GpuNodeTagFrameDimension     = GpuNodeTagBase + 3;  ///< Tag to show frame dimensions

static const UINT32 GpuNodeNumTags               = 4;

// Reminder to change the constant and array entries below.
CHI_STATIC_ASSERT(4 == GpuNodeNumTags);

///< Supported vendor tag list, it shall align with the definition in g_VendorTagSectionGpuNode
static const UINT32 g_VendorTagList[] =
{
    GpuNodeTagSupportedFeature,
    GpuNodeTagCurrentMode,
    GpuNodeTagProcessedFrameNum,
    GpuNodeTagFrameDimension,
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagSectionGpuNode[GpuNodeNumTags] =
{
    { "SupportedFeature",     TYPE_INT32, 1 },
    { "CurrentMode",          TYPE_BYTE,  1 },
    { "ProcessedFrameNumber", TYPE_INT64, 1 },
    { "FrameDimension",       TYPE_INT32, 2 },
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagGpuNodeSection[] =
{
    {
        GpuNodeSectionName,  0,
        sizeof(g_VendorTagSectionGpuNode) / sizeof(g_VendorTagSectionGpuNode[0]), g_VendorTagSectionGpuNode,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoGpuNode[] =
{
    {
        &g_VendorTagGpuNodeSection[0],
        sizeof(g_VendorTagGpuNodeSection) / sizeof(g_VendorTagGpuNodeSection[0])
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodeGetCaps
///
/// @brief  Implementation of PFNNODEGETCAPS defined in chinode.h
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GpuNodeGetCaps(
    CHINODECAPSINFO* pCapsInfo)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pCapsInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pCapsInfo is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (sizeof(CHINODECAPSINFO) <= pCapsInfo->size)
        {
            pCapsInfo->nodeCapsMask = ChiNodeCapsScale | ChiNodeCapsGpuMemcpy |
                                      ChiNodeCapsGPURotate | ChiNodeCapsGPUDownscale | ChiNodeCapsGPUFlip;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODECAPSINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag  Pointer to a structure that defines the run-time Chi assigned vendor tag base.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GpuNodeQueryVendorTag(
    CHIQUERYVENDORTAG* pQueryVendorTag)
{
    CDKResult result = CDKResultSuccess;
    if (NULL == pQueryVendorTag)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalide argument: pQueryVendorTag is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (pQueryVendorTag->size >= sizeof(CHIQUERYVENDORTAG))
        {
            pQueryVendorTag->pVendorTagInfo = g_VendorTagInfoGpuNode;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "pQueryVendorTag is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodeCreate
///
/// @brief  GPU Node Implementation of PFNNODECREATE defined in chinode.h
///
/// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GpuNodeCreate(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult   result  = CDKResultSuccess;
    ChiGPUNode* pNode   = NULL;

    if (NULL == pCreateInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pTagTypeInfo is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (sizeof(CHINODECREATEINFO) > pCreateInfo->size)
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODECREATEINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    if (CDKResultSuccess == result)
    {
        pNode = new ChiGPUNode;
        if (NULL == pNode)
        {
            result = CDKResultENoMemory;
        }
    }

    if (CDKResultSuccess == result)
    {
        result = pNode->Initialize(pCreateInfo);
    }

    if (CDKResultSuccess == result)
    {
        pCreateInfo->phNodeSession = reinterpret_cast<CHIHANDLE*>(pNode);
    }

    if (CDKResultSuccess != result)
    {
        if (NULL != pNode)
        {
            delete pNode;
            pNode = NULL;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodeDestroy
///
/// @brief  Implementation of PFNNODEDESTROY defined in chinode.h
///
/// @param  pDestroyInfo    Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GpuNodeDestroy(
    CHINODEDESTROYINFO* pDestroyInfo)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pDestroyInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pDestroyInfo is NULL");
    }
    if ((CDKResultSuccess == result) && (NULL == pDestroyInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pDestroyInfo->hNodeSession is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (sizeof(CHINODEDESTROYINFO) <= pDestroyInfo->size)
        {
            ChiGPUNode* pNode = static_cast<ChiGPUNode*>(pDestroyInfo->hNodeSession);
            // Guaranteed to be not null at this point (playing safe due threating in future)
            if (NULL != pNode)
            {
                delete pNode;
                pNode = NULL;
            }

            pDestroyInfo->hNodeSession  = NULL;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEDESTROYINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodeQueryBufferInfo
///
/// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
///
/// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GpuNodeQueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pQueryBufferInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid pQueryBufferInfo argument");
    }

    if ((CDKResultSuccess == result) && (NULL == pQueryBufferInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid pQueryBufferInfo->hNodeSessionn argument");
    }

    if (CDKResultSuccess == result)
    {
        if (sizeof(CHINODEQUERYBUFFERINFO) <= pQueryBufferInfo->size)
        {
            ChiGPUNode* pNode   = static_cast<ChiGPUNode*>(pQueryBufferInfo->hNodeSession);
            result              = pNode->QueryBufferInfo(pQueryBufferInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEQUERYBUFFERINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodeSetBufferInfo
///
/// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GpuNodeSetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pSetBufferInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid pSetBufferInfo argument");
    }

    if ((CDKResultSuccess == result) && (NULL == pSetBufferInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid pSetBufferInfo->hNodeSession argument");
    }

    if (CDKResultSuccess == result)
    {
        if (sizeof(CHINODESETBUFFERPROPERTIESINFO) <= pSetBufferInfo->size)
        {
            ChiGPUNode* pNode   = static_cast<ChiGPUNode*>(pSetBufferInfo->hNodeSession);
            result              = pNode->SetBufferInfo(pSetBufferInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODESETBUFFERPROPERTIESINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodeProcRequest
///
/// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult GpuNodeProcRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pProcessRequestInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid pProcessRequestInfo argument");
    }

    if ((CDKResultSuccess == result) && (NULL == pProcessRequestInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid pProcessRequestInfo->hNodeSession argument");
    }

    if (CDKResultSuccess == result)
    {
        if (sizeof(CHINODEPROCESSREQUESTINFO) <= pProcessRequestInfo->size)
        {
            ChiGPUNode* pNode   = static_cast<ChiGPUNode*>(pProcessRequestInfo->hNodeSession);
            result              = pNode->ProcessRequest(pProcessRequestInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEPROCESSREQUESTINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodePostPipelineCreate
///
/// @brief  Implementation of PFNPOSTPIPELINECREATE defined in chinode.h
///
/// @param  pNodeSession node session pointer
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CDKResult GpuNodePostPipelineCreate(
    VOID* pNodeSession)
{
    if (NULL == pNodeSession)
    {
        LOG_ERROR(CamxLogGroupChi, "Null GpuNodePostPipelineCreate pNodeSession");
    }

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GpuNodeSetNodeInterface
///
/// @brief  Implementation of PFCHINODESETNODEINTERFACE defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID GpuNodeSetNodeInterface(
    ChiNodeInterface* pNodeInterface)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pNodeInterface)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pNodeInterface is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (sizeof(ChiNodeInterface) <= pNodeInterface->size)
        {
            g_ChiNodeInterface.pGetMetadata         = pNodeInterface->pGetMetadata;
            g_ChiNodeInterface.pSetMetadata         = pNodeInterface->pSetMetadata;
            g_ChiNodeInterface.pGetVendorTagBase    = pNodeInterface->pGetVendorTagBase;
            g_ChiNodeInterface.pProcessRequestDone  = pNodeInterface->pProcessRequestDone;

            // get the vendor tag base
            CHIVENDORTAGBASEINFO vendorTagBase = {0};
            vendorTagBase.size                 = sizeof(CHIVENDORTAGBASEINFO);
            vendorTagBase.pComponentName       = GpuNodeSectionName;

            result = g_ChiNodeInterface.pGetVendorTagBase(&vendorTagBase);
            if (CDKResultSuccess == result)
            {
                g_vendorTagBase = vendorTagBase.vendorTagBase;
            }
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEPROCREQUESTINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return;
}
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiNodeEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom node.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Node. This function is called
/// during the camera server initialization, which occurs during HAL process start. In addition to communicating the necessary
/// function pointers between Chi and external nodes, this function allows a node to do any initialization work that it
/// would typically do at process init. Anything done here should not be specific to a session, and any variables stored in
/// the node must be protected against multiple sessions accessing it at the same time.
///
/// @param pNodeCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the node.
///                        The node must fill in these function pointers.
///
/// @return VOID.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDK_VISIBILITY_PUBLIC VOID ChiNodeEntry(
    CHINODECALLBACKS* pNodeCallbacks)
{
    if (NULL != pNodeCallbacks)
    {
        if ((ChiNodeMajorVersion == pNodeCallbacks->majorVersion) &&
            (sizeof(CHINODECALLBACKS) <= pNodeCallbacks->size))
        {
            pNodeCallbacks->majorVersion                = ChiNodeMajorVersion;
            pNodeCallbacks->minorVersion                = ChiNodeMinorVersion;
            pNodeCallbacks->pGetCapabilities            = GpuNodeGetCaps;
            pNodeCallbacks->pQueryVendorTag             = GpuNodeQueryVendorTag;
            pNodeCallbacks->pCreate                     = GpuNodeCreate;
            pNodeCallbacks->pDestroy                    = GpuNodeDestroy;
            pNodeCallbacks->pQueryBufferInfo            = GpuNodeQueryBufferInfo;
            pNodeCallbacks->pSetBufferInfo              = GpuNodeSetBufferInfo;
            pNodeCallbacks->pProcessRequest             = GpuNodeProcRequest;
            pNodeCallbacks->pChiNodeSetNodeInterface    = GpuNodeSetNodeInterface;
            pNodeCallbacks->pPostPipelineCreate         = GpuNodePostPipelineCreate;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Chi API major version doesn't match (%d:%d) vs (%d:%d)",
                      pNodeCallbacks->majorVersion,
                      pNodeCallbacks->minorVersion,
                      ChiNodeMajorVersion,
                      ChiNodeMinorVersion);
        }
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Invalid Argument: %p", pNodeCallbacks);
    }
}
#ifdef __cplusplus
}
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiGPUNode::Initialize(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult result = CDKResultSuccess;

    /// @todo (CAMX-1854) Check for Node Capabilities using NodeCapsMask
    m_hChiSession    = pCreateInfo->hChiSession;
    m_nodeId         = pCreateInfo->nodeId;
    m_nodeCaps       = pCreateInfo->nodeCaps.nodeCapsMask;
    m_nodeFlags      = pCreateInfo->nodeFlags;
    m_processedFrame = 0;

    LOG_ERROR(CamxLogGroupChi, "#### bypass set %d", m_nodeFlags.isBypassable);


    m_pGpuNodeMutex = CamX::Mutex::Create("CHIGPUNODE");
    m_pOpenCL       = GPUOpenCL::GetInstance();

    if ((NULL == m_pOpenCL) || (NULL == m_pGpuNodeMutex))
    {
        result = CDKResultENoMemory;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::QueryBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiGPUNode::QueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    for (UINT32 i = 0; i < pQueryBufferInfo->numOutputPorts; i++)
    {
        CHINODEBUFFERREQUIREMENT* pInputOptions      = &pQueryBufferInfo->pInputOptions[i].inputBufferOption;
        CHINODEBUFFERREQUIREMENT* pOutputOptions     = &pQueryBufferInfo->pOutputPortQueryInfo[i].outputBufferOption;
        CHINODEBUFFERREQUIREMENT* pOutputRequirement = &pQueryBufferInfo->pOutputPortQueryInfo[i].pBufferRequirement[0];

        // set the nodeCaps later
        switch (m_nodeCaps)
        {
            case ChiNodeCapsGpuMemcpy:
                pInputOptions->minW     = pOutputRequirement->minW;
                pInputOptions->minH     = pOutputRequirement->minH;
                pInputOptions->maxW     = pOutputRequirement->maxW;
                pInputOptions->maxH     = pOutputRequirement->maxH;
                pInputOptions->optimalW = pOutputRequirement->optimalW;
                pInputOptions->optimalH = pOutputRequirement->optimalH;
                pInputOptions->planeAlignment[0].strideAlignment    = 128;
                pInputOptions->planeAlignment[0].scanlineAlignment  = 32;
                pInputOptions->planeAlignment[1].strideAlignment    = 128;
                pInputOptions->planeAlignment[1].scanlineAlignment  = 16;
                pOutputOptions->minW     = pOutputRequirement->minW;
                pOutputOptions->minH     = pOutputRequirement->minH;
                pOutputOptions->maxW     = pOutputRequirement->maxW;
                pOutputOptions->maxH     = pOutputRequirement->maxH;
                pOutputOptions->optimalW = pOutputRequirement->optimalW;
                pOutputOptions->optimalH = pOutputRequirement->optimalH;
                pOutputOptions->planeAlignment[0].strideAlignment   = 128;
                pOutputOptions->planeAlignment[0].scanlineAlignment = 32;
                pOutputOptions->planeAlignment[1].strideAlignment   = 128;
                pOutputOptions->planeAlignment[1].scanlineAlignment = 16;
                break;
            case ChiNodeCapsGPUGrayscale:
            case ChiNodeCapsGPURotate:
                pInputOptions->minW      = pOutputRequirement->minW;
                pInputOptions->minH      = pOutputRequirement->minH;
                pInputOptions->maxW      = pOutputRequirement->maxW;
                pInputOptions->maxH      = pOutputRequirement->maxH;
                pInputOptions->optimalW  = pOutputRequirement->optimalW;
                pInputOptions->optimalH  = pOutputRequirement->optimalH;
                pInputOptions->planeAlignment[0].strideAlignment    = 128;
                pInputOptions->planeAlignment[0].scanlineAlignment  = 32;
                pInputOptions->planeAlignment[1].strideAlignment    = 128;
                pInputOptions->planeAlignment[1].scanlineAlignment  = 16;
                pOutputOptions->minW     = ChiNodeUtils::MaxUINT32(pOutputRequirement->minW, pOutputRequirement->minH);
                pOutputOptions->minH     = ChiNodeUtils::MaxUINT32(pOutputRequirement->minW, pOutputRequirement->minH);
                pOutputOptions->maxW     = ChiNodeUtils::MaxUINT32(pOutputRequirement->maxW, pOutputRequirement->maxH);
                pOutputOptions->maxH     = ChiNodeUtils::MaxUINT32(pOutputRequirement->maxW, pOutputRequirement->maxH);
                pOutputOptions->optimalW = ChiNodeUtils::MaxUINT32(pOutputRequirement->optimalW, pOutputRequirement->optimalH);
                pOutputOptions->optimalH = ChiNodeUtils::MaxUINT32(pOutputRequirement->optimalW, pOutputRequirement->optimalH);
                pOutputOptions->planeAlignment[0].strideAlignment   = 128;
                pOutputOptions->planeAlignment[0].scanlineAlignment = 32;
                pOutputOptions->planeAlignment[1].strideAlignment   = 128;
                pOutputOptions->planeAlignment[1].scanlineAlignment = 16;
                break;
            case ChiNodeCapsGPUDownscale:
            case ChiNodeCapsGPUFlip:
                // Expected that the pipeline will properly set the resolutions, therefore keep it the same as the requirement.
            default:
                /// @todo (CAMX-1806) Node caps not set, default case is used
                pInputOptions->minW      = pOutputRequirement->minW;
                pInputOptions->minH      = pOutputRequirement->minH;
                pInputOptions->maxW      = pOutputRequirement->maxW;
                pInputOptions->maxH      = pOutputRequirement->maxH;
                pInputOptions->optimalW  = pOutputRequirement->optimalW;
                pInputOptions->optimalH  = pOutputRequirement->optimalH;
                pInputOptions->planeAlignment[0].strideAlignment    = 128;
                pInputOptions->planeAlignment[0].scanlineAlignment  = 32;
                pInputOptions->planeAlignment[1].strideAlignment    = 128;
                pInputOptions->planeAlignment[1].scanlineAlignment  = 16;
                pOutputOptions->minW     = pOutputRequirement->minW;
                pOutputOptions->minH     = pOutputRequirement->minH;
                pOutputOptions->maxW     = pOutputRequirement->maxW;
                pOutputOptions->maxH     = pOutputRequirement->maxH;
                pOutputOptions->optimalW = pOutputRequirement->optimalW;
                pOutputOptions->optimalH = pOutputRequirement->optimalH;
                pOutputOptions->planeAlignment[0].strideAlignment   = 128;
                pOutputOptions->planeAlignment[0].scanlineAlignment = 32;
                pOutputOptions->planeAlignment[1].strideAlignment   = 128;
                pOutputOptions->planeAlignment[1].scanlineAlignment = 16;
                break;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::SetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiGPUNode::SetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    m_format.width            = pSetBufferInfo->pFormat->width;
    m_format.height           = pSetBufferInfo->pFormat->height;
    m_fullOutputDimensions[0] = pSetBufferInfo->pFormat->width;
    m_fullOutputDimensions[1] = pSetBufferInfo->pFormat->height;

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::ProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiGPUNode::ProcessRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    m_pGpuNodeMutex->Lock();

    switch (m_nodeCaps)
    {
    case ChiNodeCapsGpuMemcpy:
        /// @todo (CAMX-1854) get the metadata from the camera system and only run the logic in preview/snapshot "Copy" Mode
        for (UINT32 i = 0; i < pProcessRequestInfo->inputNum; i++)
        {
            CopyImage(pProcessRequestInfo->phOutputBuffer[i], pProcessRequestInfo->phInputBuffer[i]);
        }
        break;

    case ChiNodeCapsGPURotate:
        for (UINT32 i = 0; i < pProcessRequestInfo->inputNum; i++)
        {
            UpdateOrientation(pProcessRequestInfo->frameNum);
            RotateImage(pProcessRequestInfo->phOutputBuffer[i], pProcessRequestInfo->phInputBuffer[i], m_currentRotation);
            // Update the width/height dimensions to be passed as metadata

            if (Rotate90Degrees == m_currentRotation || Rotate270Degrees == m_currentRotation)
            {
                // Output is currently rotated, swap the width and height values
                m_fullOutputDimensions[0] = pProcessRequestInfo->phInputBuffer[i]->format.height;
                m_fullOutputDimensions[1] = pProcessRequestInfo->phInputBuffer[i]->format.width;
            }
            else
            {
                m_fullOutputDimensions[0] = pProcessRequestInfo->phInputBuffer[i]->format.width;
                m_fullOutputDimensions[1] = pProcessRequestInfo->phInputBuffer[i]->format.height;
            }
        }
        break;

    case ChiNodeCapsGPUDownscale:
        if (pProcessRequestInfo->outputNum == 1)
        {
            ///
            /// @todo We should check port matchings and act accordingly
            ///

            // If 1 output buffer, there is no downscale;
            // InputFull to OutputFull
            // CopyBuffer(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[0]);
            if (pProcessRequestInfo->phOutputBuffer[0]->format.formatParams.yuvFormat[0].planeStride ==
                pProcessRequestInfo->phInputBuffer[0]->format.formatParams.yuvFormat[0].planeStride)
            {
                CopyImage(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[0]);
            }
            else if (pProcessRequestInfo->phOutputBuffer[0]->format.formatParams.yuvFormat[0].planeStride <
                     pProcessRequestInfo->phInputBuffer[0]->format.formatParams.yuvFormat[0].planeStride)
            {
                DownscaleBy4Image(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[0]);
            }
        }
        else
        {
            // If more than 1 output buffer, assume that the downscaling is successive by a factor of four for each
            // buffer with the first buffer being full output (1x scale).

            // InputFull to OutputFull
            CopyImage(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[0]);
            // CopyBuffer(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[0]);

            // InputFull to OutputDS4
            DownscaleBy4Image(pProcessRequestInfo->phOutputBuffer[1], pProcessRequestInfo->phInputBuffer[0]);

            // OutputDS4 to OutputDS16 and OutputDS16 to OutputDS64
            for (UINT32 i = 2; i < pProcessRequestInfo->outputNum; i++)
            {
                DownscaleBy4Image(pProcessRequestInfo->phOutputBuffer[i], pProcessRequestInfo->phOutputBuffer[i - 1]);
            }
        }

        // Do we even need this? (only used for gpu rotation usecase- clean up)
        // m_fullOutputDimensions[0] = pProcessRequestInfo->phOutputBuffer[0]->format.width;
        // m_fullOutputDimensions[1] = pProcessRequestInfo->phOutputBuffer[0]->format.height;
        break;
    case ChiNodeCapsGPUFlip:
        UpdateOrientation(pProcessRequestInfo->frameNum);
        UpdateFlip(pProcessRequestInfo->frameNum);

        /// @todo (CAMX-1854) get the metadata from the camera system and only run the logic in preview/snapshot "Copy" Mode
        for (UINT32 i = 0; i < pProcessRequestInfo->inputNum; i++)
        {
            if (TRUE == IsBypassableNode())
            {
                if (m_currentFlip == FlipNone)
                {
                    LOG_ERROR(CamxLogGroupChi,
                              "#### [BYPASS] ProcessRequest GPU NOFLIP  %d , rotation = %d",
                              m_currentFlip,
                              m_currentRotation);
                    pProcessRequestInfo->pBypassData[0].isBypassNeeded = TRUE;
                    pProcessRequestInfo->pBypassData[0].selectedInputPortIndex = 0;
                }
                else
                {
                    LOG_ERROR(CamxLogGroupChi,
                              "#### [BYPASS] ProcessRequest GPU Flip = %d , rotation = %d",
                              m_currentFlip,
                              m_currentRotation);
                    pProcessRequestInfo->pBypassData[0].isBypassNeeded = FALSE;
                    FlipImage(pProcessRequestInfo->phOutputBuffer[i],
                              pProcessRequestInfo->phInputBuffer[i],
                              m_currentFlip,
                              m_currentRotation);
                }
            }
            else {
                LOG_ERROR(CamxLogGroupChi,
                          "###### [INPLACE] ProcessRequest GPU Flip = %d , rotation = %d",
                          m_currentFlip,
                          m_currentRotation);
                FlipImage(pProcessRequestInfo->phOutputBuffer[i],
                          pProcessRequestInfo->phInputBuffer[i],
                          m_currentFlip,
                          m_currentRotation);
            }
        }
        break;
    }

    m_processedFrame++;
    UpdateMetaData(pProcessRequestInfo->frameNum);

    m_pGpuNodeMutex->Unlock();

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::ChiGPUNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiGPUNode::ChiGPUNode()
    : m_hChiSession(NULL)
    , m_nodeId(0)
    , m_nodeCaps(0)
    , m_processedFrame(0)
    , m_pGpuNodeMutex(NULL)
{
    memset(&m_format, 0, sizeof(CHINODEIMAGEFORMAT));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::~ChiGPUNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiGPUNode::~ChiGPUNode()
{
    m_pOpenCL = NULL;

    if (NULL != m_pGpuNodeMutex)
    {
        m_pGpuNodeMutex->Destroy();
        m_pGpuNodeMutex = NULL;
    }

    m_hChiSession = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::CopyBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiGPUNode::CopyBuffer(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput)
{
    m_pOpenCL->CopyBuffer(hOutput, hInput);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::CopyImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiGPUNode::CopyImage(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput)
{
    m_pOpenCL->CopyImage(hOutput, hInput);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::RotateImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiGPUNode::RotateImage(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput,
    RotationAngle       targetRotation)
{
    m_pOpenCL->RotateImage(hOutput, hInput, targetRotation);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::FlipImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiGPUNode::FlipImage(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput,
    FlipDirection       targetFlip,
    RotationAngle       currentOrientation)
{
    FlipDirection direction = targetFlip;
    if ((Rotate90Degrees == currentOrientation) || (Rotate270Degrees == currentOrientation))
    {
        if (FlipLeftRight == targetFlip)
        {
            direction = FlipTopBottom;
        }
        else if (FlipTopBottom == targetFlip)
        {
            direction = FlipLeftRight;
        }
    }

    m_pOpenCL->FlipImage(hOutput, hInput, direction);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::DownscaleIBy4Image
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiGPUNode::DownscaleBy4Image(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput)
{
    m_pOpenCL->DownscaleBy4Image(hOutput, hInput);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::UpdateFlip
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiGPUNode::UpdateFlip(
    UINT64 requestId)
{
    CDK_UNUSED_PARAM(requestId);
    // Currently have it always set to only flip the image from left to right
    m_currentFlip = FlipLeftRight;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::UpdateOrientation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiGPUNode::UpdateOrientation(
    UINT64 requestId)
{
    VOID* pData = NULL;
    INT32 orientation = 0;
    pData = ChiNodeUtils::GetMetaData(requestId,
                                      (ANDROID_JPEG_ORIENTATION | InputMetadataSectionMask),
                                      ChiMetadataDynamic,
                                      &g_ChiNodeInterface,
                                      m_hChiSession);

    if (NULL != pData)
    {
        orientation = *static_cast<INT32 *>(pData);
    }

    // Convert Android's Jpeg Orientation metadata
    switch (orientation)
    {
    case 0:
        m_currentRotation = Rotate0Degrees;
        break;
    case 90:
        m_currentRotation = Rotate90Degrees;
        break;
    case 180:
        m_currentRotation = Rotate180Degrees;
        break;
    case 270:
        m_currentRotation = Rotate270Degrees;
        break;
    default:
        m_currentRotation = Rotate0Degrees;
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::UpdateMetaData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiGPUNode::UpdateMetaData(
    UINT64 requestId)
{
    CHIMETADATAINFO metadataInfo     = {0};
    const UINT32    tagSize          = sizeof(g_VendorTagSectionGpuNode) / sizeof(g_VendorTagSectionGpuNode[0]);
    CHITAGDATA      tagData[tagSize] = { {0} };
    UINT32          tagList[tagSize];

    metadataInfo.size       = sizeof(CHIMETADATAINFO);
    metadataInfo.chiSession = m_hChiSession;
    metadataInfo.tagNum     = tagSize;
    metadataInfo.pTagList   = &tagList[0];
    metadataInfo.pTagData   = &tagData[0];

    UINT32 index = 0;

    UINT32  supportedFeature    = ChiNodeCapsGpuMemcpy;
    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &supportedFeature;
    tagData[index].dataSize     = g_VendorTagSectionGpuNode[index].numUnits;
    index++;

    UINT32  currentMode         = m_nodeCaps;
    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &currentMode;
    tagData[index].dataSize     = g_VendorTagSectionGpuNode[index].numUnits;
    index++;

    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &m_processedFrame;
    tagData[index].dataSize     = g_VendorTagSectionGpuNode[index].numUnits;
    index++;

    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &m_fullOutputDimensions[0];
    tagData[index].dataSize     = g_VendorTagSectionGpuNode[index].numUnits;

    g_ChiNodeInterface.pSetMetadata(&metadataInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGPUNode::GetMetaData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* ChiGPUNode::GetMetaData(
    UINT64 requestId,
    UINT32 tagId)
{
    CHIMETADATAINFO metadataInfo = {0};
    const UINT32    tagSize      = 1;
    CHITAGDATA      tagData      = {0};
    UINT32          tagList      = tagId;

    tagData.requestId         = requestId;
    tagData.offset            = 0;
    tagData.negate            = FALSE;

    metadataInfo.size         = sizeof(CHIMETADATAINFO);
    metadataInfo.chiSession   = m_hChiSession;
    metadataInfo.tagNum       = tagSize;
    metadataInfo.pTagList     = &tagList;
    metadataInfo.pTagData     = &tagData;
    metadataInfo.metadataType = ChiMetadataDynamic;

    g_ChiNodeInterface.pGetMetadata(&metadataInfo);
    return tagData.pData;
}
