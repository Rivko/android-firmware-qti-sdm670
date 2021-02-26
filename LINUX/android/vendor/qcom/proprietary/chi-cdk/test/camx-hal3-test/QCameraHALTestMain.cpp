////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  BufferManager.h
/// @brief Buffer Manager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <signal.h>
#include <camera/VendorTagDescriptor.h>
#include "QCameraHAL3TestSnapshot.h"
#include "QCameraHAL3TestVideo.h"
#include "QCameraHAL3TestPreview.h"
#include "QCameraHAL3Base.h"
#include "g_version.h"

#define LOG_TAG "CameraTest"

using namespace std;
using namespace android;

char usage[] = " \
usage: hal3_test [-h] [-f command.txt] \n\
 -h      show usage\n\
 -f      using commands in file\n\
\n\
command in program: \n\
<order>:[Params] \n\
 Orders: \n\
  A: ADD a camera device for test \n\
     >>A:id=0,psize=1920x1080,pformat=yuv420,ssize=1920x1080,sformat=jpeg\n\
  U: Update meta setting \n\
     >>U:manualaemode=1 \n\
  D: Delete current camera device \n\
     >>D \n\
  S: trigger Snapshot and dump preview/video \n\
     >>s:2 \n\
  s: trigger Snapshot \n\
     >>S:2 \n\
  v: triger video that switch from preview \n\
     >>v:id=0,psize=1920x1080,pformat=yuv420,vsize=1920x1080,ssize=1920x1080,sformat=jpeg \n\
  p: trigger dump Preview \n\
     >>p:2 \n\
  M: set Metadata dump tag \n\
     >>M:expvalue=1,scenemode=0 \n\
  Q: Quit \n\
";

extern char *optarg;
bool IsCommandFile = false;
ifstream FileStream;

#define MAX_CAMERA 2
camera_module_t* CamModule;
QCameraHAL3Base* Camera[MAX_CAMERA];
QCameraHAL3Test* HAL3Test[MAX_CAMERA];
int              CurCameraId;

int parseCommandline(int argc, char* argv[])
{
    int c;
    while ((c = getopt(argc,argv, "hf:")) != -1) {
        printf("opt:%c\n",c);
        switch (c) {
        case 'h':
            printf("%s\n",usage);
            return 1;
        case 'f': {
            std::string str(optarg);
            FileStream.open(str.c_str());
            if (FileStream.fail()) {
                printf("Command File Open Error\n");
                return 1;
            }
            IsCommandFile = true;
        }
        default:
            break;
        }
    }
    return 0;
}

/************************************************************************
* name : CameraDeviceStatusChange
* function: static callback forwarding methods from HAL to instance
************************************************************************/
void CameraDeviceStatusChange(
    const struct camera_module_callbacks* callbacks,
    int camera_id,
    int new_status) {

}

void TorchModeStatusChange(
    const struct camera_module_callbacks* callbacks,
    const char* camera_id,
    int new_status) {

}
camera_module_callbacks_t module_callbacks = {CameraDeviceStatusChange,
    TorchModeStatusChange};
#ifdef LINUX_ENABLED
#define CAMERA_HAL_LIBERAY "/usr/lib/hw/camera.qcom.so"
#else
#define CAMERA_HAL_LIBERAY "/vendor/lib/hw/camera.qcom.so"
#endif

/************************************************************************
* name : loadCameraModule
* function: load the camera module liberay
************************************************************************/
static int loadCameraModule(const char *id, const char *path,
    camera_module_t **pCmi)
{
    int status = -EINVAL;
    void *handle = NULL;
    camera_module_t *cmi = NULL;

    handle = dlopen(path, RTLD_NOW);
    if (handle == NULL) {
        char const *err_str = dlerror();
        ALOGE("load: module=%s\n%s", path, err_str?err_str:"unknown");
        status = -EINVAL;
        cmi = NULL;
        *pCmi = cmi;
        return status;
    }

    /* Get the address of the struct hal_module_info. */
    const char *sym = HAL_MODULE_INFO_SYM_AS_STR;
    cmi = (camera_module_t *)dlsym(handle, sym);
    if (cmi == NULL) {
        ALOGE("load: couldn't find symbol %s", sym);
        status = -EINVAL;
        if (handle != NULL) {
            dlclose(handle);
            handle = NULL;
        }
        *pCmi = cmi;
        return status;
    }

    /* Check that the id matches */
    if (strcmp(id, cmi->common.id) != 0) {
        ALOGE("load: id=%s != cmi->id=%s", id, cmi->common.id);
        status = -EINVAL;
        if (handle != NULL) {
            dlclose(handle);
            handle = NULL;
        }
        *pCmi = cmi;
        return status;
    }

    cmi->common.dso = handle;
    /* success */
    status = 0;
    ALOGI("loaded HAL id=%s path=%s cmi=%p handle=%p",
        id, path, *pCmi, handle);
    *pCmi = cmi;

    return status;
}

/************************************************************************
* name : initialize
* function: initialize the camera module for camera test
************************************************************************/
int initialize ()
{

    struct camera_info info;
    vendor_tag_ops_t vendor_tag_ops_;
    //Load camera module
    int err = loadCameraModule(CAMERA_HARDWARE_MODULE_ID,CAMERA_HAL_LIBERAY,&CamModule);
    //init module
    err = CamModule->init();
    // get Vendor Tag
    if (CamModule->get_vendor_tag_ops) {
        vendor_tag_ops_ = vendor_tag_ops_t();
        CamModule->get_vendor_tag_ops(&vendor_tag_ops_);

        sp<VendorTagDescriptor> vendor_tag_desc;
        err = VendorTagDescriptor::createDescriptorFromOps(&vendor_tag_ops_,
            vendor_tag_desc);

        if (0 != err) {
            ALOGE("%s: Could not generate descriptor from vendor tag operations,"
                "received error %s (%d). Camera clients will not be able to use"
                "vendor tags", __FUNCTION__, strerror(err), err);
            return err;
        }

        // Set the global descriptor to use with camera metadata
        err = VendorTagDescriptor::setAsGlobalVendorTagDescriptor(vendor_tag_desc);

        if (0 != err) {
            ALOGE("%s: Could not set vendor tag descriptor, "
                "received error %s (%d). \n",
                __func__, strerror(-err), err);
            return err;
        }
    }

    //set callback
    err = CamModule->set_callbacks(&module_callbacks);
    //Get camera info and show to user
    int nuCameras = CamModule->get_number_of_cameras();
    for (int i = 0 ;i < nuCameras;i++){
        auto rc = CamModule->get_camera_info(i, &info);
        if (!rc) {
            printf("Camera: %d face:%d\n",i,info.facing);
        } else {
            printf("Error Get Camera:%d Info \n",i);
            return rc;
        }
    }

    return 0;
}

void print_version()
{
    char *buf = (char *)malloc(sizeof(char)*1024);
    if (buf) {
        sprintf(buf, "\
            ======================= Camera Test Version  =======================\n\
             CAMTEST_SHA1    : %s\n\
             CAMTEST_BUILD_TS: %s\n\
             CAMTESTHOSTNAME : %s\n\
             CAMBUILD_IP     : %s\n\
            =====================================================================\n",\
            CAMTEST_SHA1, CAMTEST_BUILD_TS, CAMTESTHOSTNAME, CAMBUILD_IP);
        ALOGI("%s", buf);
        printf("%s", buf);
        free(buf);
        buf = NULL;
    } else {
        ALOGE("allocate buffer %s failed", buf);
    }

}

int main(int argc, char *argv[])
{
    int res = 0;
    for (int i = 0; i < MAX_CAMERA; i++) {
        Camera[i] = NULL;
    }
    printf("Enter Camera Testing\n");
    print_version();
    res = parseCommandline(argc, argv);
    if (res != 0){
        return -1;
    }

    res = initialize();
    if (res != 0){
        return -1;
    }

    //Wait for Test Order
    bool stop = false;
    while (!stop){
        string order;
        string param;
        ssize_t size = 0;
        size_t len = 0;
        if (IsCommandFile){
            std::getline(FileStream,order);
        } else {
            printf("CAM%d>>",CurCameraId);
            if (!std::getline(std::cin,order)) {
                printf("Failed to getline!\n");
                stop = true;
                continue;
            };
        }
        if (order.empty() || '#' == order[0]){
            continue;
        }
        int pos = order.find(':');
        string ops = order.substr(0,pos);
        param = order.substr(pos+1,order.size());
        if (ops == "sleep"){
            int second = 0;
            sscanf(param.c_str(),"%d",&second);
            sleep(second);
            continue;
        }

        printf("size: %zd Test camera:%s \n",size,order.c_str());
        switch (ops[0]) {
        case 'A':
        case 'a':{
            //add
            TestConfig* testConf = new TestConfig();
            res = testConf->parseCommandlineAdd(size ,(char*)param.c_str());
            if (res != 0 ) {
                printf("error command order res:%d\n",res);
                break;
            }
            CurCameraId = testConf->mCameraId;
            printf("add a camera :%d\n",CurCameraId);
            if (Camera[CurCameraId] == NULL) {
               Camera[CurCameraId] = new QCameraHAL3Base(CamModule,CurCameraId);
            }
            switch (testConf->mTestMode) {
            case TESTMODE_PREVIEW:{
                QCameraHAL3TestPreview* testPreview = new QCameraHAL3TestPreview(CamModule,
                    Camera[CurCameraId]->mDevice,testConf);
                testPreview->PreinitPreviewStream();
                testPreview->mDevice->openCamera();
                testPreview->run();
                HAL3Test[testConf->mCameraId] = testPreview;
                break;
            }
            case TESTMODE_SNAPSHOT:{
                //test snapshot
                QCameraHAL3TestSnapshot* testSnapshot = new QCameraHAL3TestSnapshot(CamModule,
                    Camera[CurCameraId]->mDevice,testConf);
                testSnapshot->PreinitSnapshotStreams();
                testSnapshot->mDevice->openCamera();
                testSnapshot->run();
                HAL3Test[testConf->mCameraId] = testSnapshot;
                break;
            }
            case TESTMODE_VIDEO:{
                QCameraHAL3TestVideo* testVideo = new QCameraHAL3TestVideo(CamModule,
                    Camera[CurCameraId]->mDevice,testConf);
                testVideo->PreinitVideoStreams();
                testVideo->mDevice->openCamera();
                testVideo->run();
                HAL3Test[testConf->mCameraId] = testVideo;
                break;
            }
            default: {
                printf("Wrong TEST MODE\n");
                break;
            }
            }
            break;
        }
        case 'v':{
            /*
             * switch preview to video, enter preview-only mode with
             * A:id=0,psize=1920x1080,pformat=yuv420, then change to
             * video mode with v:id=0,psize=1920x1080,pformat=yuv420,
             * vsize=1920x1080,ssize=1920x1080,sformat=jpeg
             */
            printf("video request %s\n",param.c_str());
            if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_PREVIEW) {
                QCameraHAL3TestPreview* testPreview =
                    (QCameraHAL3TestPreview*) HAL3Test[CurCameraId];
                TestConfig* testConf = testPreview->mConfig;
                res = testConf->parseCommandlineAdd(size ,(char*)param.c_str());

                QCameraHAL3TestVideo* testVideo = new QCameraHAL3TestVideo(CamModule,
                    Camera[CurCameraId]->mDevice,testConf);
                testVideo->PreinitVideoStreams();

                testPreview->stop();
                delete testPreview;
                HAL3Test[CurCameraId] = NULL;

                testVideo->run();
                HAL3Test[testConf->mCameraId] = testVideo;
                break;
            }
        }
        case 's':{
            int num = 0;
            sscanf(param.c_str(),"%d",&num);
            printf("snapshot request:%d\n",num);
            if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_SNAPSHOT) {
                QCameraHAL3TestSnapshot* testSnapshot =
                    (QCameraHAL3TestSnapshot *)HAL3Test[CurCameraId];
                StreamCapture request = {SNAPSHOT_TYPE,num};
                testSnapshot->RequestCaptures(request);
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_PREVIEW) {
                // In NonZSL switch to snapshot mode, Capture picture,
                // and swith back to preview-only mode
                QCameraHAL3TestPreview* testPreview =
                    (QCameraHAL3TestPreview*) HAL3Test[CurCameraId];
                android::CameraMetadata meta = *(testPreview->getCurrentMeta());
                testPreview->stop();
                QCameraHAL3TestSnapshot* testSnapshot = new QCameraHAL3TestSnapshot(CamModule,
                    Camera[CurCameraId]->mDevice,testPreview->mConfig);
                testSnapshot->PreinitSnapshotStreams();
                testSnapshot->setCurrentMeta(&meta);
                testSnapshot->run();
                StreamCapture request = {SNAPSHOT_TYPE,num};
                testSnapshot->RequestCaptures(request);
                delete testSnapshot;
                testSnapshot = NULL;
                testPreview->PreinitPreviewStream();
                testPreview->setCurrentMeta(&meta);
                testPreview->run();
                meta.clear();
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_VIDEO) {
                QCameraHAL3TestVideo* testVideo = (QCameraHAL3TestVideo *)HAL3Test[CurCameraId];
                StreamCapture request = {SNAPSHOT_TYPE,num};
                testVideo->RequestCaptures(request);
            }
            break;
        }
        case 'S':{
            int num = 0;
            sscanf(param.c_str(),"%d",&num);
            printf("snapshot request:%d\n",num);

            if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_SNAPSHOT) {
                QCameraHAL3TestSnapshot* testSnapshot =
                    (QCameraHAL3TestSnapshot *)HAL3Test[CurCameraId];
                StreamCapture request = {SNAPSHOT_TYPE,num};
                testSnapshot->RequestCaptures(request);
                testSnapshot->dumpPreview(num);
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_PREVIEW) {
                /*
                 * In NonZSL mode, it will be preview-only before snapshot order set,
                 * when snapshot order set by user, stop the preview-only stream and
                 * start snapshot test mode, capture pictures, then switch back to
                 * preview-only mode
                 */
                QCameraHAL3TestPreview* testPreview =
                    (QCameraHAL3TestPreview*) HAL3Test[CurCameraId];
                android::CameraMetadata meta = *(testPreview->getCurrentMeta());
                testPreview->stop();
                QCameraHAL3TestSnapshot* testSnapshot = new QCameraHAL3TestSnapshot(CamModule,
                    Camera[CurCameraId]->mDevice,testPreview->mConfig);
                testSnapshot->PreinitSnapshotStreams();
                testSnapshot->setCurrentMeta(&meta);
                testSnapshot->run();
                StreamCapture request = {SNAPSHOT_TYPE,num};
                testSnapshot->RequestCaptures(request);
                delete testSnapshot;
                testSnapshot = NULL;
                testPreview->PreinitPreviewStream();
                testPreview->setCurrentMeta(&meta);
                testPreview->run();
                meta.clear();
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_VIDEO){
                QCameraHAL3TestVideo* testVideo = (QCameraHAL3TestVideo *)HAL3Test[CurCameraId];
                StreamCapture request = {SNAPSHOT_TYPE,num};
                testVideo->RequestCaptures(request);
                testVideo->dumpPreview(num);
                testVideo->dumpVideo(num);
            }
            break;
        }
        case 'P':{
            int num = 0;
            sscanf(param.c_str(),"%d",&num);
            printf("preview request:%d\n",num);
            if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_SNAPSHOT) {
                QCameraHAL3TestSnapshot* testSnapshot =
                    (QCameraHAL3TestSnapshot *)HAL3Test[CurCameraId];
                testSnapshot->dumpPreview(num);
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_PREVIEW) {
                QCameraHAL3TestPreview* testPreview =
                    (QCameraHAL3TestPreview *)HAL3Test[CurCameraId];
                testPreview->dumpPreview(num);
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_VIDEO) {
                QCameraHAL3TestVideo* testVideo =
                    (QCameraHAL3TestVideo *)HAL3Test[CurCameraId];
                testVideo->dumpPreview(num);
                testVideo->dumpVideo(num);
            }
            break;
        }
        case 'D':{
            if (!HAL3Test[CurCameraId]){
                printf("please Add camera before Delete it\n");
                break;
            }
            if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_SNAPSHOT) {
                QCameraHAL3TestSnapshot* testSnapshot =
                    (QCameraHAL3TestSnapshot *)HAL3Test[CurCameraId];
                testSnapshot->stop();
                delete testSnapshot->mConfig;
                testSnapshot->mConfig = NULL;
                delete testSnapshot;
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_VIDEO) {
                QCameraHAL3TestVideo* testVideo =
                    (QCameraHAL3TestVideo *)HAL3Test[CurCameraId];
                testVideo->stop();
                delete testVideo->mConfig;
                testVideo->mConfig = NULL;
                delete testVideo;
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_PREVIEW) {
                QCameraHAL3TestPreview* testPreview =
                    (QCameraHAL3TestPreview *)HAL3Test[CurCameraId];
                testPreview->stop();
                delete testPreview->mConfig;
                testPreview->mConfig = NULL;
                delete testPreview;
            }
            HAL3Test[CurCameraId] = NULL;
            break;
        }
        case 'U':{
            android::CameraMetadata* meta;
            if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_SNAPSHOT) {
                QCameraHAL3TestSnapshot* testSnapshot =
                    (QCameraHAL3TestSnapshot *)HAL3Test[CurCameraId];
                meta = testSnapshot->getCurrentMeta();
                int res = HAL3Test[CurCameraId]->mConfig->parseCommandlineMetaUpdate
                    ((char*)param.c_str(),meta);
                testSnapshot->updataMetaDatas(meta);
            } else if (HAL3Test[CurCameraId]->mConfig->mTestMode == TESTMODE_PREVIEW) {
                QCameraHAL3TestPreview* testPreview =
                    (QCameraHAL3TestPreview *)HAL3Test[CurCameraId];
                meta = testPreview->getCurrentMeta();
                int res = HAL3Test[CurCameraId]->mConfig->parseCommandlineMetaUpdate
                    ((char*)param.c_str(),meta);
                testPreview->updataMetaDatas(meta);
            }
            break;
        }
        case 'M':{
            printf("meta dump:\n");
            int res = HAL3Test[CurCameraId]->mConfig->parseCommandlineMetaDump(1,
                (char*)param.c_str());
            break;
        }
        case 'Q':{
            printf("quit\n");
            for (int i = 0;i < MAX_CAMERA; i++) {
                if (!HAL3Test[i]) {
                    continue;
                }
                if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_SNAPSHOT) {
                    QCameraHAL3TestSnapshot* testSnapshot =
                        (QCameraHAL3TestSnapshot *)HAL3Test[i];
                    testSnapshot->stop();
                    delete testSnapshot->mConfig;
                    testSnapshot->mConfig = NULL;
                    delete testSnapshot;
                } else if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_VIDEO) {
                    QCameraHAL3TestVideo* testVideo =
                        (QCameraHAL3TestVideo *)HAL3Test[i];
                    testVideo->stop();
                    delete testVideo->mConfig;
                    testVideo->mConfig = NULL;
                    delete testVideo;
                } else if (HAL3Test[i]->mConfig->mTestMode == TESTMODE_PREVIEW) {
                    QCameraHAL3TestPreview* testPreview =
                        (QCameraHAL3TestPreview *)HAL3Test[i];
                    testPreview->stop();
                    delete testPreview->mConfig;
                    testPreview->mConfig = NULL;
                    delete testPreview;
                }
                HAL3Test[i] = NULL;
            }
            stop = true;
            break;
        }
        default:{
            printf("Wrong Command\n");
            break;
        }
        }
    }
    for (int i = 0;i < MAX_CAMERA;i++) {
        if (Camera[i]) {
            Camera[i]->mDevice->closeCamera();
            delete Camera[i];
            Camera[i] = NULL;
        }
    }
    printf("Exiting application\n");
    return 0;
}

