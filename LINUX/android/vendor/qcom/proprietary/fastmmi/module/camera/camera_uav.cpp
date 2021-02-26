/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <unordered_map>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

#include <camera.h>
#include <camera_parameters.h>

#include "common.h"
#include "mmi_module.h"

#define DEFAULT_EXPOSURE_VALUE  250
#define DEFAULT_GAIN_VALUE  50
#define DEFAULT_CAMERA_FPS 30
#define MAX_MSG   100

#define STR_KEY_CAM_POSITION  "type"

static pthread_mutex_t m_camera;

using namespace std;
using namespace camera;

struct CameraTestCaps {
    vector < ImageSize > pSizes, vSizes, picSizes;
    vector < string > focusModes, wbModes, isoModes;
    Range brightness, sharpness, contrast;
      vector < Range > previewFpsRanges;
      vector < VideoFPS > videoFpsValues;
      vector < string > previewFormats;
    string rawSize;
};

enum CameraTestFunction {
    CAM_FUNC_HIRES = 0,
    CAM_FUNC_OPTIC_FLOW = 1,
    CAM_FUNC_LEFT_SENSOR = 2,
    CAM_FUNC_STEREO = 3,
};


struct CameraTestConfig {
    bool DumpFrames;
    int RunTime;
    int exposureValue;
    int gainValue;
    CameraTestFunction func;
    ImageSize pSize;
    ImageSize vSize;
    ImageSize picSize;
    int fps;
};

enum CameraMsgId {
    MSG_ID_PICTURE_FRAME_DONE,
    MSG_ID_QUIT_THREAD,
    MSG_ID_MAX
};

struct CameraMsgData {
    unsigned int bufferSize;
    unsigned char *pBufferData;
};

struct CameraMsg {
    CameraMsgId id;
    CameraMsgData data;
};

struct CameraMsgQ {
    CameraMsg q[MAX_MSG];
    int head;
    int size;
};

class CameraMmiTest:ICameraListener {
  public:

    CameraMmiTest();
    CameraMmiTest(CameraTestConfig config);
    ~CameraMmiTest();

    int camFacing;

    int mmiTakePicture();
    int mmiCameraRun();
    int mmiCameraStop();
    int mmiCameraInit();
    int mmiCameraDeinit();

    /* listener methods */
    virtual void onPictureFrame(ICameraFrame * frame);

  private:
      ICameraDevice * pCamDev_;
    CameraParams pCamPara_;
    ImageSize pMmiSize_, pMmivSize_, pMmiPicSize_;
    CameraTestCaps pCamCaps_;
    CameraTestConfig pMmiConfig_;

    pthread_mutex_t m_msgMutex;
    pthread_cond_t m_msgSignal;
    CameraMsgQ m_sMsgQ;
    bool isQuitPictureThread;

    pthread_cond_t cvPicDone;
    pthread_mutex_t mutexPicDone;
    bool isPicDone;

    int setParameters();
    int setFPSindex(int fps, int &pFpsIdx, int &vFpsIdx);
    void sendMessage(CameraMsgId id, CameraMsgData * data);
    void popMessage(CameraMsg * msg);
    static int pictureThreadEntry(void *pThreadData);
    int executeHandlePicture(void);
};

CameraMmiTest *m_test;

CameraMmiTest::CameraMmiTest():
pCamDev_(NULL) {
    memset(&m_sMsgQ, 0, sizeof(CameraMsgQ));
    pthread_cond_init(&cvPicDone, NULL);
    pthread_mutex_init(&mutexPicDone, NULL);
    pthread_cond_init(&m_msgSignal, NULL);
    pthread_mutex_init(&m_msgMutex, NULL);
}

CameraMmiTest::CameraMmiTest(CameraTestConfig config) {
    pMmiConfig_ = config;
    memset(&m_sMsgQ, 0, sizeof(CameraMsgQ));
    pthread_cond_init(&cvPicDone, NULL);
    pthread_mutex_init(&mutexPicDone, NULL);
    pthread_cond_init(&m_msgSignal, NULL);
    pthread_mutex_init(&m_msgMutex, NULL);
}

typedef int (*thread_fn_type) (void *thread_data);

typedef struct picture_thread_type {
    thread_fn_type pfn;
    int priority;
    pthread_t thread;
    void *thread_data;
} picture_thread_type;

static void *picture_thread_entry(void *thread_data) {
    picture_thread_type *thread = (picture_thread_type *) thread_data;

    return (void *) thread->pfn(thread->thread_data);
}

int picture_thread_create(void **handle, thread_fn_type pfn, void *thread_data, int priority) {
    int result = 0;
    picture_thread_type *thread = (picture_thread_type *) malloc(sizeof(picture_thread_type));

    *handle = thread;

    if(thread) {
        int create_result;

        thread->pfn = pfn;
        thread->thread_data = thread_data;
        thread->priority = priority;
        create_result = pthread_create(&thread->thread, NULL, picture_thread_entry, (void *) thread);
        if(create_result != 0) {
            ALOGE("failed to create thread\n");
            result = 1;
        }
    } else {
        ALOGE("failed to allocate thread\n");
        result = 1;
    }

    return result;
}

int picture_thread_destroy(void *handle, int *thread_result) {
    int result = 0;
    picture_thread_type *thread = (picture_thread_type *) handle;

    if(thread) {
        if(pthread_join(thread->thread, (void **) thread_result) != 0) {
            ALOGE("failed to join thread\n");
            result = 1;
        }
        free(thread);
    } else {
        ALOGE("handle is null\n");
        result = 1;
    }

    return result;
}

static int dumpToFile(uint8_t * data, uint32_t size, char *name, uint64_t timestamp) {
    FILE *fp;

    fp = fopen(name, "wb");

    if(!fp) {
        ALOGE("fopen failed for %s\n", name);
        return -1;
    }

    fwrite(data, size, 1, fp);
    ALOGI("saved filename %s\n", name);
    fclose(fp);
}

void CameraMmiTest::onPictureFrame(ICameraFrame * frame) {
    static int onPicCount = 0;

    onPicCount++;
    timeval tv;

    gettimeofday(&tv, NULL);
    ALOGD("onPictureFrame start %d %d %ld.%06ld\n", onPicCount, frame->size, tv.tv_sec, tv.tv_usec);

    unsigned char *pPictureData = (unsigned char *) malloc(frame->size);

    if(!pPictureData)
        return;
    memcpy(pPictureData, frame->data, frame->size);
    CameraMsgData msgData;

    memset(&msgData, 0, sizeof(CameraMsgData));
    msgData.bufferSize = frame->size;
    msgData.pBufferData = pPictureData;

    gettimeofday(&tv, NULL);
    ALOGE("onPictureFrame after memcpy %d %p %d %ld.%06ld\n", onPicCount, msgData.pBufferData, msgData.bufferSize,
          tv.tv_sec, tv.tv_usec);
    sendMessage(MSG_ID_PICTURE_FRAME_DONE, &msgData);
}

ImageSize FHDSize(1920, 1080);
ImageSize HDSize(1280, 720);
ImageSize VGASize(640, 480);

/**
 *  FUNCTION : setParameters
 *
 *  - When camera is opened, it is initialized with default set
 *    of parameters.
 *  - This function sets required parameters based on camera and
 *    usecase
 *  - params_setXXX and params_set  only updates parameter
 *    values in a local object.
 *  - params_.commit() function will update the hardware
 *    settings with the current state of the parameter object
 *  - Some functionality will not be application for all for
 *    sensor modules. for eg. optic flow sensor does not support
 *    autofocus/focus mode.
 *  - Reference setting for different sensors and format are
 *    provided in this function.
 *
 *  */
int CameraMmiTest::setParameters() {
    int focusModeIdx = 0;
    int wbModeIdx = 2;
    int isoModeIdx = 0;
    int pFpsIdx = 3;
    int vFpsIdx = 3;
    int prevFmtIdx = 0;
    int rc = 0;

    pMmiSize_ = pMmiConfig_.pSize;
    pMmivSize_ = pMmiConfig_.vSize;
    pMmiPicSize_ = pMmiConfig_.picSize;

    if(CAM_FUNC_OPTIC_FLOW == camFacing) {
        pMmiSize_ = VGASize;
        pMmivSize_ = VGASize;
        pMmiPicSize_ = VGASize;
    }

    pCamPara_.setPictureSize(pMmiPicSize_);
    pCamPara_.setFocusMode(pCamCaps_.focusModes[focusModeIdx]);
    pCamPara_.setWhiteBalance(pCamCaps_.wbModes[wbModeIdx]);
    pCamPara_.setISO(pCamCaps_.isoModes[isoModeIdx]);

    pCamPara_.setPreviewFormat(pCamCaps_.previewFormats[prevFmtIdx]);
    pCamPara_.setPreviewSize(pMmiSize_);
    pCamPara_.setVideoSize(pMmivSize_);

    /* Find index and set FPS  */
    rc = setFPSindex(pMmiConfig_.fps, pFpsIdx, vFpsIdx);

    if(rc == -1) {
        return rc;
    }

    pCamPara_.setPreviewFpsRange(pCamCaps_.previewFpsRanges[pFpsIdx]);
    pCamPara_.setVideoFPS(pCamCaps_.videoFpsValues[vFpsIdx]);

    return pCamPara_.commit();
}

/**
 * FUNCTION: setFPSindex
 *
 * scans through the supported fps values and returns index of
 * requested fps in the array of supported fps
 *
 * @param fps      : Required FPS  (Input)
 * @param pFpsIdx  : preview fps index (output)
 * @param vFpsIdx  : video fps index   (output)
 *
 *  */
int CameraMmiTest::setFPSindex(int fps, int &pFpsIdx, int &vFpsIdx) {
    int defaultPrevFPSIndex = -1;
    int defaultVideoFPSIndex = -1;
    int i, rc = 0;

    for(i = 0; i < pCamCaps_.previewFpsRanges.size(); i++) {
        if((pCamCaps_.previewFpsRanges[i].max) / 1000 == fps) {
            pFpsIdx = i;
            break;
        }
        if((pCamCaps_.previewFpsRanges[i].max) / 1000 == DEFAULT_CAMERA_FPS) {
            defaultPrevFPSIndex = i;
        }
    }

    if(i >= pCamCaps_.previewFpsRanges.size()) {
        if(defaultPrevFPSIndex != -1) {
            pFpsIdx = defaultPrevFPSIndex;
        } else {
            pFpsIdx = -1;
            rc = -1;
        }
    }

    for(i = 0; i < pCamCaps_.videoFpsValues.size(); i++) {
        if(fps == pCamCaps_.videoFpsValues[i]) {
            vFpsIdx = i;
            break;
        }
        if(DEFAULT_CAMERA_FPS == pCamCaps_.videoFpsValues[i]) {
            defaultVideoFPSIndex = i;
        }
    }
    if(i >= pCamCaps_.videoFpsValues.size()) {
        if(defaultVideoFPSIndex != -1) {
            vFpsIdx = defaultVideoFPSIndex;
        } else {
            vFpsIdx = -1;
            rc = -1;
        }
    }
    return rc;
}

int CameraMmiTest::mmiTakePicture() {
    int rc = FAILED;

    pthread_mutex_lock(&mutexPicDone);
    ALOGI("%s start", __FUNCTION__);
    rc = pCamDev_->takePicture();
    if(rc) {
        ALOGE("takePicture failed\n");
        pthread_mutex_unlock(&mutexPicDone);
        return rc;
    }
    pthread_mutex_unlock(&mutexPicDone);
    return SUCCESS;
}

int CameraMmiTest::mmiCameraRun() {
    int rc = FAILED;
    void *pThread = NULL;
    int thread_result = 0;

    ALOGI("%s start", __FUNCTION__);
    rc = mmiCameraInit();
    if(rc == FAILED) {
        return rc;
    }
    pCamDev_->startPreview();
    sleep(2);                   /*wait privew date return */
    if(picture_thread_create(&pThread, pictureThreadEntry, this, 0) != 0) {
        ALOGE("picture_thread_create failed\n");
    }

    rc = mmiTakePicture();
    sleep(pMmiConfig_.RunTime); /*wait onPictureFrame done */
    isQuitPictureThread = true;
    sendMessage(MSG_ID_QUIT_THREAD, NULL);
    if(picture_thread_destroy(pThread, &thread_result) != 0) {
        ALOGE("picture_thread_destroy failed\n");
    }
    return rc;
}

int CameraMmiTest::mmiCameraStop() {
    ALOGI("%s start", __FUNCTION__);
    pCamDev_->stopPreview();
    ICameraDevice::deleteInstance(&pCamDev_);
    return SUCCESS;
}


int CameraMmiTest::mmiCameraInit() {
    int rc = SUCCESS;

    ALOGI("%s start", __FUNCTION__);
    /* returns the number of camera-modules connected on the board */
    int n = getNumberOfCameras();

    if(n < 0) {
        ALOGE("getNumberOfCameras() failed, rc=%d\n", n);
        rc = FAILED;
        return rc;
    }

    ALOGI("num_cameras = %d\n", n);
    if(n < 1) {
        ALOGE("No cameras found.\n");
        rc = FAILED;
        return rc;

    }
    /* default id is not available */
    int camId = -1;

    /* find camera based on function */
    for(int i = 0; i < n; i++) {
        CameraInfo info;

        getCameraInfo(i, info);
        ALOGD(" i = %d , info.func = %d , camFacing = %d\n", i, info.func, camFacing);
        if(CAM_FUNC_HIRES == camFacing && CAM_FUNC_HIRES == info.func) {
            camId = i;
        } else if(CAM_FUNC_OPTIC_FLOW == camFacing && CAM_FUNC_OPTIC_FLOW == info.func) {
            camId = i;
        }
    }
    if(camId == -1) {
        ALOGE("Camera not found \n");
        rc = FAILED;
        return rc;
    }
    ALOGI("Testing camera id=%d\n", camId);
    rc = ICameraDevice::createInstance(camId, &pCamDev_);
    if(rc != 0) {
        ALOGE("could not open camera %d\n", camId);
        rc = FAILED;
        return rc;
    }
    pCamDev_->addListener(this);

    rc = pCamPara_.init(pCamDev_);
    if(rc != 0) {
        ALOGE("failed to init parameters\n");
        ICameraDevice::deleteInstance(&pCamDev_);
        rc = FAILED;
        return rc;
    }
    /* query capabilities */
    pCamCaps_.focusModes = pCamPara_.getSupportedFocusModes();
    pCamCaps_.wbModes = pCamPara_.getSupportedWhiteBalance();
    pCamCaps_.isoModes = pCamPara_.getSupportedISO();
    pCamCaps_.previewFpsRanges = pCamPara_.getSupportedPreviewFpsRanges();
    pCamCaps_.videoFpsValues = pCamPara_.getSupportedVideoFps();
    pCamCaps_.previewFormats = pCamPara_.getSupportedPreviewFormats();

    rc = setParameters();
    if(rc) {
        ALOGE("setParameters failed\n");
        ICameraDevice::deleteInstance(&pCamDev_);
        rc = FAILED;
        return rc;
    }
    return SUCCESS;
}

int CameraMmiTest::mmiCameraDeinit() {
    ALOGD("mmiCameraDeinit stop!");
    return 0;
}

CameraMmiTest::~CameraMmiTest() {
    pthread_cond_destroy(&cvPicDone);
    pthread_mutex_destroy(&mutexPicDone);
    pthread_cond_destroy(&m_msgSignal);
    pthread_mutex_destroy(&m_msgMutex);
}

/**
 *  FUNCTION: setDefaultConfig
 *
 *  set default config based on camera module
 *
 * */
static CameraTestConfig setDefaultConfig() {
    CameraTestConfig cfg;

    cfg.DumpFrames = false;
    cfg.RunTime = 3;
    cfg.exposureValue = DEFAULT_EXPOSURE_VALUE; /* Default exposure value */
    cfg.gainValue = DEFAULT_GAIN_VALUE; /* Default gain value */
    cfg.fps = DEFAULT_CAMERA_FPS;
    cfg.pSize = FHDSize;
    cfg.vSize = HDSize;
    cfg.picSize = FHDSize;
    return cfg;
}

int CameraMmiTest::pictureThreadEntry(void *pThreadData) {
    int result = 0;
    CameraMmiTest *pCameraTest = (CameraMmiTest *) pThreadData;

    result = pCameraTest->executeHandlePicture();
    return result;
}

int CameraMmiTest::executeHandlePicture(void) {
    int result = 0;
    static int onPicCount = 0;

    CameraMsg msg;

    memset(&msg, 0, sizeof(CameraMsg));
    isQuitPictureThread = false;

    while(!isQuitPictureThread) {
        popMessage(&msg);

        switch (msg.id) {
        case MSG_ID_QUIT_THREAD:
            ALOGI("CameraTest MSG_ID_QUIT_THREAD\n");
            break;
        case MSG_ID_PICTURE_FRAME_DONE:{
                if(!msg.data.pBufferData)
                    break;
                onPicCount++;
                ALOGI("CameraTest pictureThreadEntry %d, %p, %d\n", onPicCount, msg.data.pBufferData,
                      msg.data.bufferSize);
                char jpgName[128];

                if(camFacing == CAM_FUNC_HIRES) {
                    snprintf(jpgName, 128, "main_camera_snapshot_%dx%d.jpg", pMmiPicSize_.width, pMmiPicSize_.height);
                } else {
                    snprintf(jpgName, 128, "optic_camera_snapshot_%dx%d.jpg", pMmiPicSize_.width, pMmiPicSize_.height);
                }
                dumpToFile(msg.data.pBufferData, msg.data.bufferSize, jpgName, 0);
                free(msg.data.pBufferData);
                break;
            }
        default:
            ALOGE("invalid msg id %d\n", (int) msg.id);
        }
    }

    ALOGI("picture thread is exited\n");
    return result;
}

void CameraMmiTest::sendMessage(CameraMsgId id, CameraMsgData * data) {
    pthread_mutex_lock(&m_msgMutex);

    if(m_sMsgQ.size >= MAX_MSG) {
        ALOGE("msg m_sMsgQ is full\n");
        return;
    }

    m_sMsgQ.q[(m_sMsgQ.head + m_sMsgQ.size) % MAX_MSG].id = id;

    if(data) {
        m_sMsgQ.q[(m_sMsgQ.head + m_sMsgQ.size) % MAX_MSG].data = *data;
    }

    ++m_sMsgQ.size;
    pthread_cond_signal(&m_msgSignal);
    pthread_mutex_unlock(&m_msgMutex);
}

void CameraMmiTest::popMessage(CameraMsg * msg) {
    pthread_mutex_lock(&m_msgMutex);

    while(m_sMsgQ.size == 0) {
        pthread_cond_wait(&m_msgSignal, &m_msgMutex);
    }

    *msg = m_sMsgQ.q[m_sMsgQ.head];
    --m_sMsgQ.size;
    m_sMsgQ.head = (m_sMsgQ.head + 1) % MAX_MSG;
    pthread_mutex_unlock(&m_msgMutex);
}


int mmi_cam_parse_params(unordered_map < string, string > &params) {
    ALOGI("%s, E. param addr\n", __func__);

    const char *str_cam_positon = params[STR_KEY_CAM_POSITION].c_str();

    int facing = CAM_FUNC_HIRES;

    if(str_cam_positon != NULL) {
        if(!strcmp(str_cam_positon, "back")) {
            facing = CAM_FUNC_HIRES;
        } else if(!strcmp(str_cam_positon, "front")) {
            facing = CAM_FUNC_OPTIC_FLOW;
        }
    }

    return facing;
}

/**
* Defined case run in mmi mode,this mode support UI.
* @return, 0 -success; -1
*/
static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

/**
* Defined case run in PCBA mode, fully automatically.
*
*/
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    int rc = FAILED;

    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    pthread_mutex_lock(&m_camera);
    CameraTestConfig config = setDefaultConfig();

    m_test = new CameraMmiTest(config);
    m_test->camFacing = mmi_cam_parse_params(params);

    rc = m_test->mmiCameraRun();
    if(rc == FAILED) {
        pthread_mutex_unlock(&m_camera);
        return rc;
    }
    rc = m_test->mmiCameraStop();
    pthread_mutex_unlock(&m_camera);

    return rc;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    pthread_mutex_init(&m_camera, NULL);
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    int rc = FAILED;

    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    rc = m_test->mmiCameraDeinit();
    delete m_test;

    m_test = NULL;
    pthread_mutex_destroy(&m_camera);
    return rc;
}

static int32_t module_stop(const mmi_module_t * module) {
    int rc = FAILED;

    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

    rc = m_test->mmiCameraStop();
    return rc;
}



/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
*
*/
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) {

    ALOGI("%s start.", __FUNCTION__);
    int ret = -1;

    if(!module || !cmd) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }

    if(!strcmp(cmd, SUBCMD_MMI))
        ret = module_run_mmi(module, params);
    else if(!strcmp(cmd, SUBCMD_PCBA))
        ret = module_run_pcba(module, params);
    else {
        ALOGE("%s Invalid command: %s  received ", __FUNCTION__, cmd);
        ret = FAILED;
    }

   /** Default RUN mmi*/
    return ret;
}

/**
* Methods must be implemented by module.
*/
static struct mmi_module_methods_t module_methods = {
    .module_init = module_init,
    .module_deinit = module_deinit,
    .module_run = module_run,
    .module_stop = module_stop,
};

/**
* Every mmi module must have a data structure named MMI_MODULE_INFO_SYM
* and the fields of this data structure must be initialize in strictly sequence as definition,
* please don't change the sequence as g++ not supported in CPP file.
*/
mmi_module_t MMI_MODULE_INFO_SYM = {
    .version_major = 1,
    .version_minor = 0,
    .name = "Camera MMI test",
    .author = "Qualcomm Technologies, Inc.",
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = -1,
};
