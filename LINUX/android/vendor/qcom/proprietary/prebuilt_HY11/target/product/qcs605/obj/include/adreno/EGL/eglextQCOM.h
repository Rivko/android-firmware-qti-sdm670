#ifndef __eglextQCOM_h__
#define __eglextQCOM_h__

/*=============================================================================
 Copyright (c) 2009-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef EGL_QUALCOMM_shared_image
#define EGL_QUALCOMM_shared_image
#define EGL_IMAGE_SHARED_OPENGLES_QUALCOMM  0x0001 /* EGL_QUALCOMM_shared_image share_list */
#define EGL_IMAGE_SHARED_OPENGL_QUALCOMM    0x0002 /* EGL_QUALCOMM_shared_image share_list */
#define EGL_IMAGE_SHARED_OPENVG_QUALCOMM    0x0003 /* EGL_QUALCOMM_shared_image share_list */
#define EGL_IMAGE_SHARED_VIDEO_QUALCOMM     0x0004 /* EGL_QUALCOMM_shared_image share_list */
#define EGL_IMAGE_SHARED_CAMERA_QUALCOMM    0x0005 /* EGL_QUALCOMM_shared_image share_list */
#define EGL_IMAGE_SHARED_CPU_QUALCOMM       0x0006 /* EGL_QUALCOMM_shared_image share_list */
#define EGL_COMPONENT_TYPE_QUALCOMM         0x0001 /* EGL_QUALCOMM_shared_image attrib_list */
#define EGL_IMAGE_FORMAT_QUALCOMM           0x0002 /* EGL_QUALCOMM_shared_image attrib_list */
#define EGL_COLOR_QUALCOMM                  0x0001 /* EGL_QUALCOMM_shared_image attrib_list EGL_COMPONENT_TYPE_QUALCOMM */
#define EGL_DEPTH_QUALCOMM                  0x0002 /* EGL_QUALCOMM_shared_image attrib_list EGL_COMPONENT_TYPE_QUALCOMM */
#define EGL_STENCIL_QUALCOMM                0x0004 /* EGL_QUALCOMM_shared_image attrib_list EGL_COMPONENT_TYPE_QUALCOMM */
#define EGL_ALPHAMASK_QUALCOMM              0x0008 /* EGL_QUALCOMM_shared_image attrib_list EGL_COMPONENT_TYPE_QUALCOMM */
#define EGL_RGB_565_QUALCOMM                0x0001 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#define EGL_RGB_888_QUALCOMM                0x0002 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#define EGL_RGBA_8888_QUALCOMM              0x0003 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#define EGL_YUV_YUYV_QUALCOMM               0x0004 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#define EGL_YUV_UYVY_QUALCOMM               0x0005 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#define EGL_YUV_YV12_QUALCOMM               0x0006 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#define EGL_YUV_NV21_QUALCOMM               0x0007 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#define EGL_SRGB_888_QCOM                   0x0008 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#define EGL_SRGBA_8888_QCOM                 0x0009 /* EGL_QUALCOMM_shared_image attrib_list EGL_IMAGE_FORMAT_QUALCOMM */
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLImageKHR EGLAPIENTRY eglCreateSharedImageQUALCOMM ( EGLDisplay dpy, EGLContext ctx, const EGLint* share_list, const EGLint* attrib_list );
EGLAPI EGLBoolean EGLAPIENTRY eglQueryImageQUALCOMM ( EGLDisplay dpy, EGLImageKHR image, EGLint attribute, EGLint* value );
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLImageKHR (EGLAPIENTRYP PFNEGLCREATESHAREDIMAGEQUALCOMMPROC) (EGLDisplay dpy, EGLContext ctx, const EGLint* share_list, const EGLint* attrib_list );
typedef EGLBoolean (EGLAPIENTRYP PFNEGLQUERYIMAGEQUALCOMMPROC) ( EGLDisplay dpy, EGLImageKHR image, EGLint attribute, EGLint* value );
#endif /* EGL_QUALCOMM_shared_image */


#ifndef EGL_QCOM_create_image
#define EGL_QCOM_create_image 1
#define EGL_NEW_IMAGE_QCOM                  0x3120
#define EGL_IMAGE_FORMAT_QCOM               0x3121
#define EGL_FORMAT_RGBA_8888_QCOM           0x3122
#define EGL_FORMAT_RGB_565_QCOM             0x3123
#define EGL_FORMAT_YUYV_QCOM                0x3124
#define EGL_FORMAT_UYVY_QCOM                0x3125
#define EGL_FORMAT_YV12_QCOM                0x3126
#define EGL_FORMAT_NV21_QCOM                0x3127
#define EGL_FORMAT_NV12_TILED_QCOM          0x3128
#define EGL_FORMAT_BGRA_8888_QCOM           0x3129
#define EGL_FORMAT_BGRX_8888_QCOM           0x312A
#define EGL_FORMAT_RGBX_8888_QCOM           0x312F
#define EGL_FORMAT_R8_QCOM                  0x31C0
#define EGL_FORMAT_RG88_QCOM                0x31C1
#define EGL_FORMAT_NV12_QCOM                0x31C2
#define EGL_FORMAT_SRGBX_8888_QCOM          0x31C3
#define EGL_FORMAT_SRGBA_8888_QCOM          0x31C4
#define EGL_FORMAT_YVYU_QCOM                0x31C5
#define EGL_FORMAT_VYUY_QCOM                0x31C6
#define EGL_FORMAT_IYUV_QCOM                0x31C7
#define EGL_FORMAT_RGB_888_QCOM             0x31C8
#define EGL_FORMAT_RGBA_5551_QCOM           0x31C9
#define EGL_FORMAT_RGBA_4444_QCOM           0x31CA
#define EGL_FORMAT_R_16_FLOAT_QCOM          0x31CB
#define EGL_FORMAT_RG_1616_FLOAT_QCOM       0x31CC
#define EGL_FORMAT_RGBA_16_FLOAT_QCOM       0x31CD
#define EGL_FORMAT_RGBA_1010102_QCOM        0x31CE
#define EGL_FORMAT_FLAG_QCOM                0x31CF
#define EGL_FORMAT_FLAG_UBWC_QCOM           0x33E0
#define EGL_FORMAT_FLAG_MACROTILE_QCOM      0x33E1
#define EGL_FORMAT_ASTC_4X4_QCOM            0x33E2
#define EGL_FORMAT_ASTC_5X4_QCOM            0x33E3
#define EGL_FORMAT_ASTC_5X5_QCOM            0x33E4
#define EGL_FORMAT_ASTC_6X5_QCOM            0x33E5
#define EGL_FORMAT_ASTC_6X6_QCOM            0x33E6
#define EGL_FORMAT_ASTC_8X5_QCOM            0x33E7
#define EGL_FORMAT_ASTC_8X6_QCOM            0x33E8
#define EGL_FORMAT_ASTC_8X8_QCOM            0x33E9
#define EGL_FORMAT_ASTC_10X5_QCOM           0x33EA
#define EGL_FORMAT_ASTC_10X6_QCOM           0x33EB
#define EGL_FORMAT_ASTC_10X8_QCOM           0x33EC
#define EGL_FORMAT_ASTC_10X10_QCOM          0x33ED
#define EGL_FORMAT_ASTC_12X10_QCOM          0x33EE
#define EGL_FORMAT_ASTC_12X12_QCOM          0x33EF
#define EGL_FORMAT_ASTC_4X4_SRGB_QCOM       0x3400
#define EGL_FORMAT_ASTC_5X4_SRGB_QCOM       0x3401
#define EGL_FORMAT_ASTC_5X5_SRGB_QCOM       0x3402
#define EGL_FORMAT_ASTC_6X5_SRGB_QCOM       0x3403
#define EGL_FORMAT_ASTC_6X6_SRGB_QCOM       0x3404
#define EGL_FORMAT_ASTC_8X5_SRGB_QCOM       0x3405
#define EGL_FORMAT_ASTC_8X6_SRGB_QCOM       0x3406
#define EGL_FORMAT_ASTC_8X8_SRGB_QCOM       0x3407
#define EGL_FORMAT_ASTC_10X5_SRGB_QCOM      0x3408
#define EGL_FORMAT_ASTC_10X6_SRGB_QCOM      0x3409
#define EGL_FORMAT_ASTC_10X8_SRGB_QCOM      0x340A
#define EGL_FORMAT_ASTC_10X10_SRGB_QCOM     0x340B
#define EGL_FORMAT_ASTC_12X10_SRGB_QCOM     0x340C
#define EGL_FORMAT_ASTC_12X12_SRGB_QCOM     0x340D
#define EGL_FORMAT_TP10_QCOM                0x340E
#define EGL_FORMAT_NV12_Y_QCOM              0x340F
#define EGL_FORMAT_NV12_UV_QCOM             0x3410
#define EGL_FORMAT_NV21_VU_QCOM             0x3411
#define EGL_FORMAT_NV12_4R_QCOM             0x3412
#define EGL_FORMAT_NV12_4R_Y_QCOM           0x3413
#define EGL_FORMAT_NV12_4R_UV_QCOM          0x3414
#define EGL_FORMAT_P010_QCOM                0x3415
#define EGL_FORMAT_P010_Y_QCOM              0x3416
#define EGL_FORMAT_P010_UV_QCOM             0x3417
#define EGL_FORMAT_TP10_Y_QCOM              0x3418
#define EGL_FORMAT_TP10_UV_QCOM             0x3419
#define EGL_TARGET_TYPE_KHR                 0xFFFD
#define EGL_CLIENTBUFFER_TYPE_KHR           0xFFFC
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLBoolean EGLAPIENTRY eglQueryImageQCOM ( EGLDisplay dpy, EGLImageKHR image, EGLint attribute, EGLint* value );
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLBoolean (EGLAPIENTRYP PFNEGLQUERYIMAGEQCOMPROC) ( EGLDisplay dpy, EGLImageKHR image, EGLint attribute, EGLint* value );
#endif /* EGL_QCOM_create_image */


// Used by profiler to query EGL
#define EGL_INITIALIZED                 0xFFFF
#define EGL_SWAP_INTERVAL               0xFFFE
#define EGL_WINDOW_SURFACE              0xFFFB
#define EGL_PBUFFER_SURFACE             0xFFFA
#define EGL_PIXMAP_SURFACE              0xFFF9


// Blob cache feature
/*
* EGLsizei is a signed integer type for representing the size of a memory
* buffer.
*/
typedef khronos_ssize_t EGLsizei;

/*
* EGLSetBlobFunc is a pointer to an application-provided function that a
* client API implementation may use to insert a key/value pair into the
* cache.
*/
typedef void (*EGLSetBlobFuncQCOM) (const void* key, EGLsizei keySize, const void* value, EGLsizei valueSize);

/*
* EGLGetBlobFunc is a pointer to an application-provided function that a
* client API implementation may use to retrieve a cached value from the
* cache.
*/
typedef EGLsizei (*EGLGetBlobFuncQCOM) (const void* key, EGLsizei keySize, void* value, EGLsizei valueSize);


/* GPU content protection extension token
* Application will set EGL_QCOM_PROTECTED_CONTENT attribute to create
* protected buffer and context that uses protected buffer.
*/
#ifndef EGL_QCOM_CONTENT_PROTECTION
#define EGL_QCOM_CONTENT_PROTECTION
#define EGL_QCOM_PROTECTED_CONTENT      0x32E0
#endif

/* GPU content protection extension token
* Application will set EGL_PROTECTED_CONTENT_EXT attribute to create
* protected contexts, surfaces and images.
*/
#ifndef EGL_EXT_protected_content
#define EGL_EXT_protected_content
#define EGL_PROTECTED_CONTENT_EXT       0x32C0
#endif

#if !defined(__QNXNTO__)
//EGL_QCOM_gpu_perf extension
#ifndef EGL_QCOM_gpu_perf
#define EGL_QCOM_gpu_perf 1

#define EGL_GPU_PERF_HINT_QCOM          0x32D0
#define EGL_HINT_PERSISTENT_QCOM        0x32D1

#define EGL_GPU_PERF_DEFAULT_QCOM       0x0
#define EGL_GPU_PERF_LOW_QCOM           0x1
#define EGL_GPU_PERF_HIGH_QCOM          0x3
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLBoolean EGLAPIENTRY eglGpuPerfHintQCOM ( EGLDisplay dpy, EGLContext ctx, EGLint* attrib_list );
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLBoolean (EGLAPIENTRYP PFNEGLGPUPERFHINTQCOM) ( EGLDisplay dpy, EGLContext ctx, EGLint* attrib_list );

#endif //EGL_QCOM_gpu_perf
#endif


#ifdef EGL_KHR_cl_event
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLint EGLAPIENTRY eglGetSyncObjFromEglSyncQCOM ( EGLDisplay dpy, EGLSyncKHR sync, void* pSyncObj );
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLint (EGLAPIENTRYP PFNEGLGETSYNCOBJQCOMPROC) ( EGLDisplay dpy, EGLSyncKHR sync, void* pSyncObj );
#endif //EGL_KHR_cl_event



#ifndef EGL_IMG_image_plane_attribs
#define EGL_IMG_image_plane_attribs 1
#define EGL_NATIVE_BUFFER_MULTIPLANE_SEPARATE_IMG        0x3105
#define EGL_NATIVE_BUFFER_PLANE_OFFSET_IMG               0x3106
#endif //EGL_IMG_image_plane_attribs



#ifndef EGL_QCOM_lock_image
#define EGL_QCOM_lock_image 1
#ifndef EGL_KHR_cl_event2
typedef intptr_t EGLAttribKHR;      // EGL_KHR_cl_event2 also defines this but might be missing from the eglext.h being used
#endif // EGL_KHR_cl_event2
#define EGL_READ_IMAGE_BIT_QCOM                         0x0001
#define EGL_WRITE_IMAGE_BIT_QCOM                        0x0002
#define EGL_IMAGE_NUM_PLANES_QCOM                       0x32B0
#define EGL_IMAGE_PLANE_PITCH_0_QCOM                    0x32B1
#define EGL_IMAGE_PLANE_PITCH_1_QCOM                    0x32B2
#define EGL_IMAGE_PLANE_PITCH_2_QCOM                    0x32B3
#define EGL_IMAGE_PLANE_DEPTH_0_QCOM                    0x32B4
#define EGL_IMAGE_PLANE_DEPTH_1_QCOM                    0x32B5
#define EGL_IMAGE_PLANE_DEPTH_2_QCOM                    0x32B6
#define EGL_IMAGE_PLANE_WIDTH_0_QCOM                    0x32B7
#define EGL_IMAGE_PLANE_WIDTH_1_QCOM                    0x32B8
#define EGL_IMAGE_PLANE_WIDTH_2_QCOM                    0x32B9
#define EGL_IMAGE_PLANE_HEIGHT_0_QCOM                   0x32BA
#define EGL_IMAGE_PLANE_HEIGHT_1_QCOM                   0x32BB
#define EGL_IMAGE_PLANE_HEIGHT_2_QCOM                   0x32BC
#define EGL_IMAGE_PLANE_POINTER_0_QCOM                  0x32BD
#define EGL_IMAGE_PLANE_POINTER_1_QCOM                  0x32BE
#define EGL_IMAGE_PLANE_POINTER_2_QCOM                  0x32BF
#define EGL_META_PLANE_POINTER_0_QCOM                   0x3420
#define EGL_META_PLANE_POINTER_1_QCOM                   0x3421
#define EGL_META_PLANE_POINTER_2_QCOM                   0x3422
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLBoolean EGLAPIENTRY eglQueryImage64QCOM ( EGLDisplay dpy, EGLImageKHR image, EGLint attribute, EGLAttribKHR* value );
EGLAPI EGLBoolean EGLAPIENTRY eglLockImageQCOM ( EGLDisplay dpy, EGLImageKHR image, const EGLint *attrib_list );
EGLAPI EGLBoolean EGLAPIENTRY eglUnlockImageQCOM ( EGLDisplay dpy, EGLImageKHR image );
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLBoolean (EGLAPIENTRYP PFNEGLQUERYIMAGE64QCOM) ( EGLDisplay dpy, EGLImageKHR image, EGLint attribute, EGLAttribKHR* value );
typedef EGLBoolean (EGLAPIENTRYP PFNEGLLOCKIMAGEQCOM) ( EGLDisplay dpy, EGLImageKHR image, const EGLint *attrib_list );
typedef EGLBoolean (EGLAPIENTRYP PFNEGLUNLOCKIMAGEQCOM) ( EGLDisplay dpy, EGLImageKHR image );
#endif // EGL_QCOM_lock_image



/* Configless context extension token
* Applications will be able to create an EGLContext
* without specifying an EGLConfig.
*/
#ifndef EGL_KHR_no_config_context
#define EGL_KHR_no_config_context 1

#define EGL_NO_CONFIG_KHR               ((EGLConfig)0)

#endif // EGL_KHR_no_config_context



#ifdef __cplusplus
}
#endif

#endif // __eglextQCOM_h__
