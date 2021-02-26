////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/


#include "QCameraHAL3MainTestContext.h"
#include "QCameraHAL3Base.h"
#include "QCameraHAL3PreviewTest.h"
#include <vector>

namespace qcamera {

#define MAX_CAMERA_SUPPORTED 20
#define TEST_CMD_CONFIG_OPTION "-c"

const CAMERA_BASE_MENU_TBL_T camera_main_menu_tbl[] = {
    {MENU_START_PREVIEW,             "To Start Preview"},
    {MENU_START_VIDEO,               "To Start Video"},
    {MENU_START_CAPTURE,             "To Capture(Non-ZSL)"},
    {MENU_START_RAW_CAPTURE,         "To Raw Capture"},
    {MENU_TOGGLE_IR_MODE,            "Toggle IR Mode"},
    {MENU_TOGGLE_SVHDR_MODE,         "Toggle SVHDR Mode"},
    {MENU_TOGGLE_BINNING_CORRECTION, "Toggle Binning Correction"},
    {MENU_EXIT,                      "EXIT"},
};

const HAL3TEST_SENSOR_MENU_TBL_T sensor_tbl[] = {
    {"Rear Camera"},
    {"Front Camera"},
};

CameraHAL3Base *mCamHal3Base = NULL;
pthread_mutex_t gCamLock = PTHREAD_MUTEX_INITIALIZER;

MainTestContext::MainTestContext()
{
    mTestRunning = false;
    mCamHal3Base = NULL;
    irmode = 0;
    svhdrmode = 0;
}

int MainTestContext::hal3appGetUserEvent()
{
    int choice;
    int rc = 0, req_capture = 0;
    int preview_restart;
    uint8_t num_of_cameras;
    if (mCamHal3Base != NULL) {
        delete mCamHal3Base;
    }
    mCamHal3Base = new CameraHAL3Base(0);
    num_of_cameras = mCamHal3Base->hal3appCameraTestLoad();
    if ((num_of_cameras <= 0) || (num_of_cameras >= MAX_CAMERA_SUPPORTED)) {
        printf("\n Supported Camera Value is wrong : %d", num_of_cameras);
        printf("\n Invalid Number Of Cameras");
        goto exit;
    }
    else {
        choice = hal3appDisplaySensorMenu(num_of_cameras);
        if (choice >= num_of_cameras || choice < 0) {
            printf("\n Unsupported Parameter");
            goto exit;
        }
        else {
            mCamHal3Base->mCameraIndex = choice;
            rc = mCamHal3Base->hal3appCameraLibOpen(choice, NULL);
        }
    }
    do {
        choice = hal3appPrintMenu();
        switch(choice) {
            case MENU_START_PREVIEW:
                mCamHal3Base->hal3appCameraPreviewInit(MENU_START_PREVIEW,
                        mCamHal3Base->mCameraIndex, PREVIEW_WIDTH, PREVIEW_HEIGHT);
                mCamHal3Base->mPreviewRunning = 1; mCamHal3Base->mVideoRunning = 0;
                mCamHal3Base->mSnapShotRunning = 0;
            break;

            case MENU_START_VIDEO:
                mCamHal3Base->hal3appCameraVideoInit(MENU_START_VIDEO,
                        mCamHal3Base->mCameraIndex, VIDEO_WIDTH, VIDEO_HEIGHT);
                mCamHal3Base->mPreviewRunning = 0; mCamHal3Base->mVideoRunning = 1;
                mCamHal3Base->mSnapShotRunning = 0;
            break;

            case MENU_START_CAPTURE:
                hal3appDisplaySnapshotMenu();
                req_capture = 1; preview_restart = 0;
                if (mCamHal3Base->mPreviewRunning == 1) {
                    preview_restart = 1;
                }
                mCamHal3Base->hal3appCameraCaptureInit(0, 0, req_capture);
                mCamHal3Base->mPreviewRunning = 0; mCamHal3Base->mVideoRunning = 0;
                mCamHal3Base->mSnapShotRunning = 1;
                if (preview_restart == 1) {
                    mCamHal3Base->hal3appCameraPreviewInit(MENU_START_PREVIEW,
                            mCamHal3Base->mCameraIndex, PREVIEW_WIDTH, PREVIEW_HEIGHT);
                    mCamHal3Base->mPreviewRunning = 1; mCamHal3Base->mVideoRunning = 0;
                    mCamHal3Base->mSnapShotRunning = 0;
                }
            break;

            case MENU_START_RAW_CAPTURE:
                hal3appDisplayRawCaptureMenu();
                req_capture = 3;
                mCamHal3Base->hal3appRawCaptureInit(0, 0, req_capture);
                mCamHal3Base->mPreviewRunning = 0; mCamHal3Base->mVideoRunning = 0;
                mCamHal3Base->mSnapShotRunning = 1;
            break;

            case MENU_TOGGLE_IR_MODE:
                 if(mCamHal3Base->mPreviewRunning == 0)
                    printf(" Cant set IR/SVHDR mode in preview mode only");
                 else {
                     irmode = !irmode;
                     printf("\n Switching IR/SVHDR mode to %s",(irmode ? "On" : "Off"));
                     ALOGE("\n Switching IR/SVHDR mode to %s and %d",
                             (irmode ? "On" : "Off"),irmode);
                     mCamHal3Base->ir_mode = irmode;
                     mCamHal3Base->hal3appCameraPreviewInit(MENU_TOGGLE_IR_MODE,
                             mCamHal3Base->mCameraIndex, PREVIEW_WIDTH, PREVIEW_HEIGHT);
                 }
            break;

            case MENU_TOGGLE_SVHDR_MODE:
                 if(mCamHal3Base->mPreviewRunning == 0)
                    printf(" Cant set IR/SVHDR mode in preview mode only");
                 else {
                     svhdrmode = !svhdrmode;
                     printf("\n Switching IR/SVHDR mode to %s",(svhdrmode ? "On" : "Off"));
                     ALOGE("\n Switching IR/SVHDR mode to %s and %d",
                             (svhdrmode ? "On" : "Off"),svhdrmode);
                     mCamHal3Base->svhdr_mode = svhdrmode;
                     mCamHal3Base->hal3appCameraPreviewInit(MENU_TOGGLE_SVHDR_MODE,
                             mCamHal3Base->mCameraIndex, PREVIEW_WIDTH, PREVIEW_HEIGHT);
                 }
            break;
            case MENU_TOGGLE_BINNING_CORRECTION:
                     if (mCamHal3Base->binning_mode == 0)
                         mCamHal3Base->binning_mode =  1;
                     else
                         mCamHal3Base->binning_mode =  0;
                     LOGE(" Toggling Binning mode to :%d",mCamHal3Base->binning_mode);
                     printf(" Toggling to Binning mode :%d",mCamHal3Base->binning_mode );
                     if(mCamHal3Base->mPreviewRunning == 1)
                     mCamHal3Base->hal3appCameraPreviewInit(MENU_TOGGLE_BINNING_CORRECTION,
                                mCamHal3Base->mCameraIndex, PREVIEW_WIDTH, PREVIEW_HEIGHT);
                     if(mCamHal3Base->mVideoRunning == 1)
                                mCamHal3Base->hal3appCameraVideoInit(MENU_TOGGLE_BINNING_CORRECTION,
                        mCamHal3Base->mCameraIndex, VIDEO_WIDTH, VIDEO_HEIGHT);
            break;

            case MENU_EXIT:
                hal3appDisplayExitMenu();
                if (mCamHal3Base->mTestCaseSelected == MENU_START_PREVIEW)
                {
                    LOGD(" Preview Test End/n");
                    mCamHal3Base->hal3appPreviewEndTest(0, 0);
                }
                else if (mCamHal3Base->mTestCaseSelected == MENU_START_RAW_CAPTURE)
                {
                    LOGD(" Raw Test End/n");
                    mCamHal3Base->hal3appRawEndTest(0, 0);
                }

            break;

            default:
                printf("\n Option not in Menu\n");
        }
    }while(choice != MENU_EXIT);
    exit:
    mCamHal3Base->hal3appCameraLibClose();
    delete mCamHal3Base;
    mCamHal3Base = NULL;
    return 0;
}

int MainTestContext::hal3appDisplaySensorMenu(uint8_t num_of_cameras)
{
    int i, choice;
    printf("\n");
    printf("===========================================\n");
    printf("    Camera Sensor to be used:            \n");
    printf("===========================================\n\n");

    for ( i=0;i < num_of_cameras; i++) {
        if (i <= 1) {
            printf("\n Press %d to select %s", (i), sensor_tbl[i].menu_name);
        }
        else {
            printf("\n Press %d to select Camera%d", (i), i);
        }
    }
    printf("\n Enter your Choice:");
    fscanf(stdin, "%d", &choice);
    return choice;
}

void MainTestContext::hal3appDisplayCapabilityMenu()
{
    printf("\n");
    printf("===========================================\n");
    printf("      Sensor Capabilty are dumped at location:\n");
    printf("===========================================\n\n");
}

int MainTestContext::hal3appDisplayPreviewMenu()
{
    int choice;
    printf("\n");
    printf("===========================================\n");
    printf("Select Camera Preview Resolution:\n");
    printf("===========================================\n\n");
    printf("========Select Preview Resolutions================\n");
    printf("\nPress 1 .Aspect Ratio(4:3) Resolution 1440 X 1080");
    printf("\nPress 2 .Aspect Ratio(16:9) Resolution 1920 X 1080");
    printf("\n Enter your Choice:");
    fscanf(stdin, "%d", &choice);
    return choice;
}

int MainTestContext::hal3appDisplayVideoMenu()
{
    int choice1;
    printf("\n");
    printf("===========================================\n");
    printf("Testing Camera Recording on Different Resolution:\n");
    printf("===========================================\n\n");

    printf("========Select Video Resolutions================\n");
    printf("\nPress 1 .Aspect Ratio(4:3) Resolution 640 X 480");
    printf("\nPress 2 .Aspect Ratio(16:9) Resolution 1920 X 1080");
    printf("\nPress 3 .To select both");

    printf("\n Enter your Choice:");
    fscanf(stdin, "%d", &choice1);
    return choice1;
}

void MainTestContext::hal3appDisplayRawCaptureMenu()
{
    printf("\n");
    printf("===========================================\n");
    printf("Testing RAW Camera Capture on Different Resolution::\n");
    printf("===========================================\n\n");
}

void MainTestContext::hal3appDisplaySnapshotMenu()
{
    printf("\n");
    printf("===========================================\n");
    printf("Testing Normal Camera Capture on Resolution 5344 X 4008\n");
    printf("===========================================\n\n");
}


void MainTestContext::hal3appDisplayExitMenu()
{
    printf("\n");
    printf("===========================================\n");
    printf("      Exiting HAL3 APP test \n");
    printf("===========================================\n\n");
}

int MainTestContext::hal3appPrintMenu()
{
    int i, choice = 0;
    char ch = '0';
    printf("\n");
    printf("===========================================\n");
    printf("       HAL3 MENU \n");
    printf("===========================================\n\n");
    for ( i = 0; i < (int)(sizeof(camera_main_menu_tbl)/sizeof(camera_main_menu_tbl[0])); i++) {
        printf("\n Press %d to select %s", (i+1), camera_main_menu_tbl[i].menu_name);
    }
    printf("\n Enter your Choice:");
    do {
        std::cin >> ch;
    } while(!(ch >= '1' && ch <= '9'));
    choice = ch -'0';
    return choice;
}

enum HAL3TestModes {
  HAL3_TEST_MODE_MENU,
  HAL3_TEST_MODE_CONFIG,
  HAL3_TEST_MODE_MAX,
};

HAL3TestModes validateTestCmdParams(int argc, char *argv[])
{
  HAL3TestModes testMode = HAL3_TEST_MODE_MAX;
  if (argc == 1) {
    testMode = HAL3_TEST_MODE_MENU;
  } else if ((argc == 3) &&
              (strlen(argv[1]) == strlen(TEST_CMD_CONFIG_OPTION)) &&
              (!strncmp(argv[1], TEST_CMD_CONFIG_OPTION, strlen(argv[1])))) {
    testMode = HAL3_TEST_MODE_CONFIG;
  }
  return testMode;
}

class HAL3TestInitParamsFromConfig {
public:
    int32_t                camera_id[MAX_NUM_CAMERAS];
    uint8_t                camera_fps[MAX_NUM_CAMERAS];
    int32_t                num_cameras;
    HAL3TestSnapshotInfo   snapshot_info;
    uint32_t               recordTime;
    uint32_t               numStream[MAX_NUM_CAMERAS];
    bool                   tnr;
    bool                   vhdr;
    bool                   binning_correct;

    HAL3TestInitParamsFromConfig() :
            camera_id { -1, -1, -1},
            camera_fps { 0, 0, 0},
            num_cameras(0),
            snapshot_info {
              HAL3TestStreamType::HAL3_TEST_STREAM_TYPE_MAX,
              -1,
              0,
              0,
              0
            },
            recordTime(0),
            numStream {0, 0, 0},
            tnr(0),
            vhdr(0),
            binning_correct(false) {};
};

int32_t ParseConfig(char *fileName, HAL3TestInitParamsFromConfig& initParams,
                                  std::vector<HAL3TestStreamInfo>& infos) {
  FILE *fp;
  HAL3TestStreamInfo track_info;
  memset(&track_info, 0x0, sizeof(track_info));
  bool isStreamReadCompleted = false;
  const int MAX_LINE = 128;
  char line[MAX_LINE];
  char value[50];
  char key[25];
  uint32_t id = 0;
  int32_t camera_index = -1;

  if(!(fp = fopen(fileName,"r"))) {
    ALOGE("failed to open config file: %s", fileName);
    return -1;
  }

  while(fgets(line,MAX_LINE-1,fp)) {
    if((line[0] == '\n') || (line[0] == '/') || line[0] == ' ')
      continue;
    strtok(line, "\n");
    memset(value, 0x0, sizeof(value));
    memset(key, 0x0, sizeof(key));
    if(isStreamReadCompleted) {
      memset(&track_info, 0x0, sizeof(track_info));
      isStreamReadCompleted = false;
    }
    int len = strlen(line);
    int i,j = 0;

    //This assumes new stream params always start with #
    if(!strcspn(line,"#")) {
      id++;
      continue;
     }


    if((id > 0) && (id > initParams.numStream[camera_index])) {
      break;
    }

    int pos = strcspn(line,":");
    for(i = 0; i< pos; i++){
      if(line[i] != ' ') {
        key[j] = line[i];
        j++;
      }
    }

    key[j] = '\0';
    j = 0;
    for(i = pos+1; i< len; i++) {
      if(line[i] != ' ') {
        value[j] = line[i];
        j++;
      }
    }
    value[j] = '\0';

    if (!strncmp("NumCameras", key, strlen("NumCameras"))) {
      initParams.num_cameras = atoi(value);
      printf("%s: Num cameras read %d\n", __func__, initParams.num_cameras);
      if (initParams.num_cameras  > MAX_NUM_CAMERAS) {
         ALOGE("%s: Unsupported number of Cameras %d",
             __func__, initParams.num_cameras);
         goto READ_FAILED;
      }
    } else if(!strncmp("CameraID", key, strlen("CameraID"))) {
      if (++camera_index < initParams.num_cameras) {
         initParams.camera_id[camera_index] = atoi(value);
         id = 0;
         printf("%s: Camera ID %d\n", __func__, initParams.camera_id[camera_index]);
      } else {
         printf("%s: Number requested cameras %d are more than declared  %d",
             __func__, camera_index + 1, initParams.num_cameras);
          goto READ_FAILED;
      }
    } else if(!strncmp("CameraFPS", key, strlen("CameraFPS"))) {
      initParams.camera_fps[camera_index] = atof(value);
    } else if(!strncmp("SnapshotType", key, strlen("SnapshotType"))) {
      if(!strncmp("None", value, strlen("None"))) {
        initParams.snapshot_info.type = HAL3_TEST_STREAM_TYPE_MAX;
      } else if(!strncmp("JPEG", value, strlen("JPEG"))) {
        initParams.snapshot_info.type = HAL3_TEST_STREAM_TYPE_JPEG;
      } else if(!strncmp("RAWYUV", value, strlen("RAWYUV"))) {
        initParams.snapshot_info.type = HAL3_TEST_STREAM_TYPE_YUV;
      } else if(!strncmp("RAWRDI", value, strlen("RAWRDI"))) {
        initParams.snapshot_info.type = HAL3_TEST_STREAM_TYPE_RAW;
      } else {
        ALOGE("%s: Unknown SnapshotType(%s)", __func__, value);
        goto READ_FAILED;
      }
      printf("Snapshot type %s\n", value);
    } else if(!strncmp("SnapshotCameraID", key, strlen("SnapshotCameraID"))) {
       initParams.snapshot_info.camera_id = atoi(value);
      printf("Snapshot cameraid %d\n", initParams.snapshot_info.camera_id);
    } else if(!strncmp("SnapshotWidth", key, strlen("SnapshotWidth"))) {
      initParams.snapshot_info.width = atoi(value);
    } else if(!strncmp("SnapshotHeight", key, strlen("SnapshotHeight"))) {
      initParams.snapshot_info.height = atoi(value);
    } else if(!strncmp("SnapshotCount", key, strlen("SnapshotCount"))) {
      initParams.snapshot_info.count = atoi(value);
    } else if(!strncmp("AFMode", key, strlen("AFMode"))) {
      // Not supported
      /*if(!strncmp("None", value, strlen("None"))) {
        initParams.af_mode = AfMode::kNone;
      } else if(!strncmp("Off", value, strlen("Off"))) {
        initParams.af_mode = AfMode::kOff;
      } else if(!strncmp("AUTO", value, strlen("AUTO"))) {
        initParams.af_mode = AfMode::kAuto;
      } else if(!strncmp("MACRO", value, strlen("MACRO"))) {
        initParams.af_mode = AfMode::kMacro;
      } else if(!strncmp("CVAF", value, strlen("CVAF"))) {
        initParams.af_mode = AfMode::kContinousVideo;
      } else if(!strncmp("CPAF", value, strlen("CPAF"))) {
        initParams.af_mode = AfMode::kContinuousPicture;
      } else {
        ALOGE("%s: Unknown AFMode(%s)", __func__, value);
        goto READ_FAILED;
      } */
    } else if(!strncmp("RecordingTime", key, strlen("RecordingTime"))) {
      initParams.recordTime = atoi(value);
    } else if(!strncmp("NumStream", key, strlen("NumStream"))) {
      if(atoi(value) <= 0) {
        ALOGE ("%s Number of stream can not be %d", __func__,
                atoi (value));
        goto READ_FAILED;
      }
      initParams.numStream[camera_index] = atoi(value);
    } else if(!strncmp("VHDR", key, strlen("VHDR"))) {
      initParams.vhdr = atoi(value)?true:false;
    } else if(!strncmp("TNR", key, strlen("TNR"))) {
      initParams.tnr = atoi(value)?true:false;
    } else if(!strncmp("BinningCorrect", key, strlen("BinningCorrect"))) {
      initParams.binning_correct = atoi(value)?true:false;
    } else if(!strncmp("Width", key, strlen("Width"))) {
      track_info.width = atoi(value);
    } else if(!strncmp("Height", key, strlen("Height"))) {
      track_info.height = atoi(value);
    } else if(!strncmp("FPS", key, strlen("FPS"))) {
      track_info.fps = atof(value);
    } else if(!strncmp("Bitrate", key, strlen("Bitrate"))) {
      // Not supported
      //track_info.bitrate = atoi(value);
    } else if(!strncmp("TrackType", key, strlen("TrackType"))) {
      track_info.type = HAL3_TEST_STREAM_TYPE_YUV;
      if(!strncmp("RAW", value, strlen("RAW"))) {
        track_info.type = HAL3_TEST_STREAM_TYPE_RAW;
      }else if(!strncmp("IDEALRAW", value, strlen("IDEALRAW"))){
        track_info.type = HAL3_TEST_STREAM_TYPE_IDEAL_RAW;
      }
    } else if(!strncmp("CamLowPowerMode", key, strlen("CamLowPowerMode"))) {
      // Not supported
      track_info.isvideo = atoi(value) ? false : true;
      isStreamReadCompleted = true;
    } else {
      ALOGE("Unknown Key %s found in %s", key, fileName);
      goto READ_FAILED;
    }
    if (isStreamReadCompleted) {
      track_info.camera_id = initParams.camera_id[camera_index];
      infos.push_back(track_info);
      /*printInitParameterAndTtrackInfo(*initParams,track_info, camera_index);*/
    }
  }

  if (initParams.numStream[camera_index] > infos.size()) {
    ALOGE("%s: Insufficient stream parameter for total stream count(%d/%d)",
           __func__, infos.size(), initParams.numStream);
    goto READ_FAILED;
  }

  fclose(fp);
  return 0;
READ_FAILED:
  fclose(fp);
  return -1;
}

class HAL3TestCaseFromConfig
{
private:
    CameraHAL3Base *mHAL3Base;
    QCameraHAL3ConfigTest *testcase;
    int req_sent;
    int preview_buffer_allocated;
    int video_buffer_allocated;
    int testCaseEnd;
public:
    HAL3TestCaseFromConfig(int cameraId, const std::vector<HAL3TestStreamInfo>& infos,
        const HAL3TestSnapshotInfo *snapshotInfo)
    {
        int num_of_cameras;
        int rc;
        mHAL3Base = new CameraHAL3Base(cameraId);
        num_of_cameras = mHAL3Base->hal3appCameraTestLoad();
        testcase = new QCameraHAL3ConfigTest(cameraId, mHAL3Base);
        rc = mHAL3Base->hal3appCameraLibOpen(cameraId, testcase);
        if (rc != HAL3_CAM_OK) {
          printf("\nError during Camera lib open\n");
        }
        testcase->Initialize(cameraId, infos, snapshotInfo);
    }

    void Start()
    {
        testcase->testStart();
    }


    void MakeSnapshot()
    {
        testcase->testMakeSnapshot();
    }

    void Stop()
    {
        testcase->testEnd();
    }

    ~HAL3TestCaseFromConfig()
    {
        int rc;
        testcase->Deinitialize();
        delete testcase;
        rc = mHAL3Base->hal3appCameraLibClose();
        rc = mHAL3Base->hal3appCameraTestUnload();
        delete mHAL3Base;
    }

};

}

int main(int argc, char *argv[])
{
    char tc_buf[3];
    int mode = 0;
    int rc = 0;

    void *binderHandle;
    binderHandle = dlopen("/usr/lib/libbinder.so", RTLD_NOW);

    qcamera::HAL3TestModes testmode;
    testmode = qcamera::validateTestCmdParams(argc, argv);
    if (testmode == qcamera::HAL3_TEST_MODE_MENU) {
        qcamera::MainTestContext main_ctx;
        printf("Please Select Execution Mode:\n");
        printf("0: Menu Based 1: Regression\n");
        printf("\n Enter your choice:");
        fgets(tc_buf, 3, stdin);
        mode = tc_buf[0] - '0';
        if (mode == 0) {
            printf("\nStarting Menu based!!\n");
        } else {
            printf("\nPlease Enter 0 or 1\n");
            printf("\nExisting the App!!\n");
            exit(1);
        }
        rc = main_ctx.hal3appGetUserEvent();
    } else if (testmode == qcamera::HAL3_TEST_MODE_CONFIG) {
        qcamera::HAL3TestInitParamsFromConfig initParams;
        std::vector<qcamera::HAL3TestStreamInfo> infos;
        qcamera::ParseConfig(argv[2], initParams, infos);
        qcamera::HAL3TestCaseFromConfig **camInstances;

        if (initParams.num_cameras) {
            camInstances =
                new qcamera::HAL3TestCaseFromConfig* [initParams.num_cameras];

            // Start cameras consecutively
            for (int i = 0; i < initParams.num_cameras; i++) {
                if (i) sleep(2); // Start cameras 2s after each other to be
                                 // consistent with recorder test
                camInstances[i] =
                    new qcamera::HAL3TestCaseFromConfig(initParams.camera_id[i],
                            infos, &initParams.snapshot_info);
                printf("Starting camera %d\n", initParams.camera_id[i]);
                camInstances[i]->Start();
            }

            sleep(2);

            for (int i = 0; i < initParams.num_cameras; i++) {
                camInstances[i]->MakeSnapshot();
            }
            printf("Waiting for %ds...\n", initParams.recordTime);
            sleep(initParams.recordTime);

            // Stop cameras
            for (int i = 0; i < initParams.num_cameras; i++) {
                if (i) sleep(2);
                printf("Stopping camera %d\n", initParams.camera_id[i]);
                camInstances[i]->Stop();
                sleep(1);
                printf("Deinitialize and close");
                delete(camInstances[i]);
                printf("Stopped\n");
            }

            delete[] camInstances;
        }
    }
    if (binderHandle)
        dlclose(binderHandle);
    printf("Exiting application\n");
    return rc;
}

