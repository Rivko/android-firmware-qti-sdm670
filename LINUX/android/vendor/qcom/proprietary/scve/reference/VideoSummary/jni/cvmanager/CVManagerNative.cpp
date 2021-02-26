/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "CVManagerNative.h"
#include "CVManagerCommon.h"

#ifdef _ANDROID_
#include <android/log.h>
#endif

//=================================
//Debuging infor
//=================================
#undef LOG_TAG
#define LOG_TAG "CVManager"
//===========================================================

/***********Global *******************/
#define MIN(a,b) ((a) <= (b)? (a) : (b))
/***********Debug*********************/

#define SINGLESHOTEACH   3
#define HIGH_COMPRESSED  2
#define LOW_COMPRESSED   1
#define ULOWCOMPRESSED   0

/******Tuning Paras*********/
const uint32_t facedet_to_value[] = {0, 60, 65, 70};
const uint32_t facefr_to_value[]  = {0, 80, 85, 90};

//////////////////////////////////////////////////////
CVStatus_t CVSummaryFile(CVFileInput_t input,CVSumouput_t *output);
//////////////////////////////////////////////////////
CVoutput_t CVManagerFuse();
//////////////////////////////////////////////////////
void SaveData(int32_t data, uint8_t end);
//////////////////////////////////////////////////////
void SaveDatainit();
//////////////////////////////////////////////////////
void TestSummary();
//////////////////////////////////////////////////////
void InitModules(uint32_t MODE, CVisionInfo_t *pinfo);
//////////////////////////////////////////////////////
extern uint8_t checkmode(uint32_t input, uint32_t MODE);
//////////////////////////////////////////////////////
int32_t CVManagerCombinescore(int32_t *scores, uint32_t number, uint32_t mode);
//////////////////////////////////////////////////////
void GetCombinedscore(CVFileInput_t input, int32_t *output);
//////////////////////////////////////////////////////
void getdefaultsummary(CVFileInput_t input, CVSumouput_t *output);
//////////////////////////////////////////////////////
uint32_t getSummary(CVSuminput_t input, CVSumouput_t *moutput,uint32_t targetlength, bool exactlength);
//////////////////////////////////////////////////////
void processpeaks(uint32_t *segstart,
                  uint32_t *segstop,
                  float32_t *first_order,
                  uint32_t segcount,
                  uint32_t inputlength);
//////////////////////////////////////////////////////
uint32_t gettargetlength(uint32_t inputlength, uint8_t ratio);
//////////////////////////////////////////////////////
extern int32_t getmodulenumber(uint32_t mode);
//////////////////////////////////////////////////////
extern int32_t Getscenetype(CVSuminput_t input,CVSumouput_t *moutput);
///////////////////////////////////////////////////////
extern CVStatus_t Getfacestar(CVSuminput_t input,CVSumouput_t *moutput);
/************************************************/


static CVisionInfo_t *pInfo = NULL;
CVpara_t mPara;

CVStatus_t CVManagerInit(uint32_t mode) {
    DPRINTF(":E CVManagerInit");
    CVStatus_t status = CV_SUCCESS;
    getVersion();
    int32_t ret = 1;

    mPara = initconfig();
    readconfig(mPara);

    //Info structure initialization
    if (pInfo) {
        free(pInfo);
    }
    pInfo = (CVisionInfo_t *)malloc(sizeof(CVisionInfo_t));

    //Para initialization
    pInfo->mImginputY              = NULL;
    pInfo->mImginputUV             = NULL;
    pInfo->mwidth                  = 0;
    pInfo->mheight                 = 0;
    pInfo->msrcYStride             = 0;
    pInfo->msrcVUStride            = 0;
    pInfo->mImgoutputY             = NULL;
    pInfo->mImgoutputUV            = NULL;
    pInfo->mdstYStride             = 0;
    pInfo->mdstVUStride            = 0;

    //Thread stop flag
    pInfo->totalmodule             = 0;

    //Output init
    pInfo->mOutput.mScore          = 0;

    pInfo->initmode                = mode;

    pInfo->max_facevalue           = sizeof(facedet_to_value) / sizeof(facedet_to_value[0]);
    InitModules(mode, pInfo);

    SaveDatainit();
    DPRINTF(":X CVManagerInit");

    return status;
}

CVStatus_t CVManagerDeinit() {
    DPRINTF(":E CVManagerDeinit");
    CVStatus_t status = CV_SUCCESS;
    int32_t ret = 1;
    if (pInfo) {
        CVManagerCleanModule(pInfo);
        DPRINTF(":E CVManagerDeinit 8");
        free(pInfo);
        pInfo = NULL;
    }
    DPRINTF(":X CVManagerDeinit");
    return status;
}

CVoutput_t CVManagerProcess(uint8_t *ImginputY,
                            uint8_t *ImginputUV,
                            uint32_t width,
                            uint32_t height,
                            uint32_t srcYStride,
                            uint32_t srcVUStride,
                            uint8_t *ImgoutputY,
                            uint8_t *ImgoutputUV,
                            uint32_t dstYStride,
                            uint32_t dstVUStride) {
    DPRINTF(":E CVManagerProcess");
    CVStatus_t status = CV_SUCCESS;

    pInfo->mwidth               = width;
    pInfo->mheight              = height;
    pInfo->msrcYStride          = srcYStride;
    pInfo->msrcVUStride         = srcVUStride;
    pInfo->mdstYStride          = dstYStride;
    pInfo->mdstVUStride         = dstVUStride;

    pInfo->mImginputY           = ImginputY;
    pInfo->mImginputUV          = ImginputUV;
    pInfo->mImgoutputY          = ImgoutputY;
    pInfo->mImgoutputUV         = ImgoutputUV;
    CVManagerModuleProcess(pInfo);

    //Get output
    pInfo->mOutput = CVManagerFuse();
    DPRINTF("CVManagerProcess output %d", pInfo->mOutput.mScore);
    DPRINTF(":X CVManagerProcess");
    return pInfo->mOutput;
}

CVStatus_t CVManagerSet(CVPara_t para, uint32_t mode)
{
    DPRINTF(":E CVManagerSet");
    CVStatus_t status = CV_SUCCESS;

    if(getmodulenumber(mode) != 1)
        return CV_ERROR;

    CVManagerModuleSet(pInfo, para, mode);

    DPRINTF(":X CVManagerSet");
    return status;
}
CVStatus_t CVManagerGet(CVPara_t *para, uint32_t mode)
{
    DPRINTF(":E CVManagerGet");
    CVStatus_t status = CV_SUCCESS;
    if(getmodulenumber(mode) != 1)
        return CV_ERROR;

    CVManagerModuleGet(pInfo, para, mode);
    DPRINTF(":X CVManagerGet");
    return status;
}


CVStatus_t CVSummaryFile(CVFileInput_t input, CVSumouput_t *poutput) {
    DPRINTF(":E CVSummary");
    CVStatus_t status = CV_SUCCESS;
    if (poutput == NULL) {
        return CV_ERROR;
    }

    if (input.input_length < input.desired_length) {
        return CV_ERROR;
    }
        int32_t *pbuffer        = (int32_t *) malloc (sizeof(int32_t) * input.input_length);
    GetCombinedscore(input, pbuffer);

    float32_t *first_order  = (float32_t *)malloc(sizeof(float32_t) * input.input_length);
    float32_t *buffer       = (float32_t *)malloc(sizeof(float32_t) * input.input_length);
    uint32_t *peaks         = (uint32_t *)malloc(input.input_length * sizeof(uint32_t));
    uint32_t peakfound      = 0;

    uint32_t *segstart      = (uint32_t *)malloc(sizeof(uint32_t) * MAX_SEG);
    uint32_t *segstop       = (uint32_t *)malloc(sizeof(uint32_t) * MAX_SEG);
    uint32_t *segmax        = (uint32_t *)malloc(sizeof(uint32_t) * MAX_SEG);
    uint32_t *segindx       = (uint32_t *)malloc(sizeof(uint32_t) * MAX_SEG);
    uint32_t *segindx_sort  = (uint32_t *)malloc(sizeof(uint32_t) * MAX_SEG);
    uint32_t segcount       = 0;
    uint8_t trend_change    = 0;

    first_order[0] = 0;
    first_order[input.input_length - 1] = 0;

    gaussian1d5(pbuffer, input.input_length, buffer);

    DPRINTF("finished gaussiand\n");
    /***Compute first derivative***/
    for (int32_t i = 1; i < input.input_length - 1; ++i) {
        first_order[i] = buffer[i + 1] - buffer[i - 1];
    }

    for (int32_t i = 5; i < input.input_length - 5; ++i) {
        if (first_order[i - 1] >= 0 &&
            first_order[i - 4] >= 0 &&
            first_order[i + 1] <= 0 &&
            first_order[i + 4] <= 0 &&
            (uint32_t)buffer[i] > mPara.data[PARA_PEAKVALTHRESH]) {
            peaks[peakfound] = i;
            peakfound++;
        }
    }

    DPRINTF("Peak found %d\n", peakfound);
    if (peakfound <= 1) {
        getdefaultsummary(input, poutput);
        return CV_SUCCESS;
    }

    /***Compute Second Derivative */
    //log1d9(input.pBuffer,input.input_length,second_order);

    /***Segment Video into Shots based on Peaks*/
    segstart[segcount] = segstop[segcount] = peaks[0];
    segindx[segcount] = 0;
    segindx_sort[segcount] = segcount;
    segmax[segcount] = (uint32_t)buffer[peaks[0]];

    for (int32_t i = 1; i < peakfound; ++i) {
        if (peaks[i] - peaks[i - 1] < mPara.data[PARA_LOCALPEAKSPACE]) {
            segstop[segcount] = peaks[i];
            if (buffer[peaks[i]] > segmax[segcount]) {
                segmax[segcount] = (uint32_t)buffer[peaks[i]];
            }
        } else {
            for (int32_t indx = segstop[segcount]; indx < peaks[i]; ++indx) {
                if (first_order[indx] >= 0) trend_change |= 1;
                else trend_change |= 2;
            }
            if (trend_change == 3) {
                DPRINTF("Segstart %d, segend %d, segmax %d",
                        segstart[segcount],
                        segstop[segcount],
                        segmax[segcount]);
                segcount++;
                segindx[segcount] = segcount;
                segindx_sort[segcount] = segcount;
                segstart[segcount] = segstop[segcount] = peaks[i];
                segmax[segcount] = (uint32_t)buffer[peaks[i]];;
            } else {
                segstop[segcount] = peaks[i];
                if (buffer[peaks[i]] > segmax[segcount]) {
                    segmax[segcount] = (uint32_t)buffer[peaks[i]];
                }
            }
            trend_change = 0;
        }
    }
    segcount++;

    //EXPAND segments in both directions to find maximum changes/shot boundary
    //EXPAND segments in both directions to find start/stop of the singal ramp
    processpeaks(segstart, segstop, first_order, segcount, input.input_length);

    DPRINTF("Total Seg count %d", segcount);
    //Sort segments in terms of segment scores
    Sort(segmax, segcount, segstart, segstop, segindx, segindx_sort);
    for (uint32_t i = 0; i < segcount; ++i) {
        DPRINTF("MergeSegment left segindx %d segstart %d, segend %d, max %d\n",
                i,
                segstart[i],
                segstop[i],
                segmax[i]);
    }
    /******************************************************/
    poutput->ouput_num = 1;
    poutput->output[0] = MergeSegment(segmax, segstart, segstop, segindx, segindx_sort, segcount, input.input_length, mPara.data[PARA_MINSHOTLEN], input.desired_length);

    /***DEBUG**********************************************/
    for (uint32_t j = 0; j < poutput->ouput_num; ++j) {
        DPRINTF("Output Numder %d , number of seg %d\n", j, poutput->output[j].output_seg);
        for (uint32_t i = 0; i < poutput->output[j].output_seg; ++i) {
            DPRINTF("Total SegStart %d, SegStop %d, Segscore %d\n",
                    poutput->output[0].start_indx[i],
                    poutput->output[0].end_indx[i],
                    poutput->output[0].seg_score[i]);
        }
    }
    /*******************************************************/

    free(pbuffer);
    free(first_order);
    free(peaks);
    free(buffer);
    free(segstart);
    free(segstop);
    free(segmax);
    free(segindx);
    free(segindx_sort);
    DPRINTF(":X CVSummary");
    return status;
}

CVStatus_t CVSummary(CVSuminput_t input, CVSumouput_t *moutput) {
    DPRINTF(":E CVSummary2");
    CVStatus_t status          = CV_SUCCESS;
    CVSumouput_t *toutput      = (CVSumouput_t *)malloc(sizeof(CVSumouput_t));
    uint32_t totalinputlength  = 0;
    uint32_t targetlength      = 0;
    uint32_t totaloutputlength = 0;
    uint32_t seglevel          = 0;
    uint32_t outputseg[MAX_FILE_INPUT];
    bool exactlength           = false;
    bool isall                 = false;

    if (input.file_no > MAX_FILE_INPUT) {
        input.file_no = MAX_FILE_INPUT;
        DPRINTF("Max file input limit hit");
    }
    moutput->ouput_num = input.file_no;

    mPara = initconfig();
    readconfig(mPara);

    for (uint32_t i = 0; i < input.file_no; ++i) {
        input.pFileinput[i]->desired_length = 0;
        CVSummaryFile(*(input.pFileinput[i]), toutput);
        moutput->output[i]   = toutput->output[0];
        totalinputlength    += input.pFileinput[i]->input_length;
    }

    if (input.target_length == 0) targetlength = gettargetlength(totalinputlength, input.summarytype);
    else {
        targetlength = input.target_length;
        exactlength  = true;
    }
    DPRINTF("Target Length %d", targetlength);
    getSummary(input,moutput,targetlength,exactlength);

    //Debug output
    for (uint32_t j = 0; j < moutput->ouput_num; ++j) {
        DPRINTF("CVSummary2 Output Number %d , number of seg %d\n", j, moutput->output[j].output_seg);
        for (uint32_t i = 0; i < moutput->output[j].output_seg; ++i){
            DPRINTF("Total SegStart %d, SegStop %d, Segscore %d\n",
                    moutput->output[j].start_indx[i],
                    moutput->output[j].end_indx[i],
                    moutput->output[j].seg_score[i]);
        }
    }
    
    free(toutput);
    DPRINTF(":X CVSummary2");
    return status;
}

CVStatus_t getVersion() {
    //DPRINTF(":E getVersion");
    CVStatus_t status = CV_SUCCESS;
    DPRINTF("CVmanager version %s", CVMANAGER_VERSION);
    DPRINTF("CVmanager version description %s", CVMANAGER_DESCRIPTION);

    //DPRINTF(":X getVersion");
    return status;
}

void FuseFace(Moduleoutput *input, uint32_t inputno, Moduleoutput *output, uint32_t mode) {
    int32_t Score = 0;
    uint32_t facecount  = 0;
    uint32_t facefr1    = 0;
    uint32_t facefr2    = 0;
    uint32_t facefr3    = 0;

    //Get max face count of last inputno frames
    for (uint32_t i = 0; i < inputno; ++i) {
        facecount = facecount >= (uint32_t)(input[i].mFaceresult & 0xFF) ? facecount : (uint32_t)(input[i].mFaceDetectedNum & 0xFF);
        facefr1   = facefr1 > (uint32_t)(input[i].mFaceresult >> 8 & 0xFF) ? facefr1 : (uint32_t)(input[i].mFaceresult >> 8 & 0xFF);
        facefr2   = facefr2 > (uint32_t)(input[i].mFaceresult >> 16 & 0xFF) ? facefr2 : (uint32_t)(input[i].mFaceresult >> 16 & 0xFF);
        facefr3   = facefr3 > (uint32_t)(input[i].mFaceresult >> 24 & 0xFF) ? facefr3 : (uint32_t)(input[i].mFaceresult >> 24 & 0xFF);
    }
    facecount = facecount >= pInfo->max_facevalue ? pInfo->max_facevalue - 1 :facecount;
    //DPRINTF("FACE fuce input 0x%X, fr1 %x, fr2 %x,fr3 %x",output ->mFaceresult,facefr1,facefr2,facefr3);
    if (facefr1 != 0xFF || facefr2 != 0xFF || facefr3 != 0xFF)
    {
        output->mScore = (facefr_to_value[facecount] & 0xFF) | (output ->mFaceresult & 0xFFFFFF00);
        //DPRINTF("%d faced detected and face recognized", facecount);
    }
    else
    {
        output->mScore = (facedet_to_value[facecount] & 0xFF) | (output ->mFaceresult & 0xFFFFFF00);
        //DPRINTF("%d faced detected none face recognized", facecount);
    }
}

//Simple Bubble Sort
int32_t getmean(int32_t input[]) {
    int32_t result  = 0;
    uint8_t swapped = 1;
    while (swapped) {
        swapped = 0;
        for (uint32_t i = 0; i < OUTPUT_HISTORY_NO - 1; ++i) {
            if (input[i] > input[i + 1]) {
                result     = input[i];
                input[i] = input[i + 1];
                input[i + 1] = result;
                swapped    = 1;
            }
        }
    }
    result = input[OUTPUT_HISTORY_NO / 2];
    return result;
}

void FuseScore(Moduleoutput *input, uint32_t inputno, Moduleoutput *output) {
    if (inputno < OUTPUT_HISTORY_NO && inputno > 0) {
        output->mScore = input[inputno - 1].mScore;
        return;
    }

    int32_t Score = 0;
    int32_t tScore[OUTPUT_HISTORY_NO];
    for (uint32_t i = 0; i < OUTPUT_HISTORY_NO; ++i) {
        tScore[i] = input[i].mScore;
    }
    output->mScore = getmean(tScore);
}

int32_t  CVManagerCombinescore(int32_t *scores, uint32_t number, uint32_t mode) {
    int32_t result = 0;
    uint32_t tMODE    = 0;
    uint32_t input_no = 0;
    uint32_t INDXMODE[MAX_MODULES];
    //DPRINTF("CVManagerCombinescore number of modules %d", number);
    for (uint32_t i = 0; i < MAX_MODULES; ++i) {
        tMODE       = 1 << i;
        if (checkmode(mode, tMODE)) {
            INDXMODE[input_no] = tMODE;    
            input_no++;        
        }
    }    

    //GETMAX
    for (uint32_t i = 0; i < number; ++i) {
        if ((INDXMODE[i] == MODE_MOTION || INDXMODE[i] == MODE_ASD) && INDXMODE[0] == MODE_BASIC) {
            if (scores[i] > result && scores[0] > 13) result = scores[i];
        } else if (scores[i] > result) result = scores[i];
    }
    ///////////////////
    return result;
}

CVoutput_t CVManagerFuse() {
    DPRINTF(":E CVManagerFuse");
    CVoutput_t fuseoutput;
    int32_t *Scores          = (int32_t *)malloc(sizeof(int32_t) * pInfo->totalmodule);
    fuseoutput.status         = CV_SUCCESS;
    fuseoutput.mScore         = 0;
    CVManager_Module *pModule = NULL;
    memset(&fuseoutput,0,sizeof(CVoutput_t));

    for (uint32_t i = 0; i < pInfo->totalmodule; ++i) {
        pModule = pInfo->mModuleList[i];
        if (pModule->mOutput.status == CV_SUCCESS) {
            DPRINTF("CVManagerFuse raw ouput %d from module %d", pModule->mOutput.mScore, i);
            pModule->mPreOutput[pModule->mOutputIndx] = pModule->mOutput;
            switch (pModule->mOutput.type) {
            case CVModuleOut_Score:
                FuseScore(pModule->mPreOutput, pModule->mOutputCount, &pModule->mOutput);
                Scores[i]              = pModule->mOutput.mScore;
                break;
            case CVModuleOut_FaceFR:
                FuseFace(pModule->mPreOutput, pModule->mOutputCount, &pModule->mOutput,pModule->mOutput.type);
                Scores[i]              = pModule->mOutput.mScore & 0xFF;
                DPRINTF("CVManagerFuse FACE score %d",Scores[i]);
                break;
            case CVModuleOut_ASD:
                Scores[i]              = ASDScoreTranslate(pModule->mOutput.mScore);
                break;
            default:
                FuseScore(pModule->mPreOutput, pModule->mOutputCount, &pModule->mOutput);
                Scores[i]              = pModule->mOutput.mScore;
                break;
            }          
            for(uint32_t i = 0,tMODE = 0;i < MAX_MODULES; ++i)
            {
                tMODE       = 1 << i;
                if (checkmode(pModule ->identity, tMODE)) {
                    fuseoutput.modScore[i] = pModule->mOutput.mScore;
                }
            }
            SaveData(pModule->mOutput.mScore, 0);
            pModule->mOutputIndx   = (pModule->mOutputIndx + 1) % OUTPUT_HISTORY_NO;
        }
    }       
    
    fuseoutput.mScore = CVManagerCombinescore(Scores, pInfo->totalmodule, pInfo -> initmode);
    fuseoutput.modnum = pInfo->totalmodule;

    DPRINTF("CVManagerFuse fused ouput %d", fuseoutput.mScore);
    DPRINTF(":X CVManagerFuse");
    free(Scores);
    SaveData(fuseoutput.mScore, 1);
    return fuseoutput;
}

void SaveDatainit() {
#ifdef RELEASE
    return;
#endif
    FILE *fp;
    fp = fopen("/sdcard/scveData/data.txt", "w");
    if (fp != NULL) {
        fclose(fp);
    }
}

void SaveData(int32_t data, uint8_t end) {
#ifdef RELEASE
    return;
#endif
    DPRINTF("%s Config Change to 3", __FUNCTION__);
    FILE *fp;
    fp = fopen("/sdcard/scveData/data.txt", "a");
    if (fp != NULL) {
        if (end) {
            fprintf(fp, "%d\n", data);
        } else fprintf(fp, "%d\t", data);
        fclose(fp);
    } else {
        DPRINTF("%s Config File open Failed", __FUNCTION__);
    }
}

void TestSummary() {
    uint8_t *testdata = (uint8_t*)malloc(1024 * 8 * sizeof(uint8_t));
    FILE *fp;
    fp = fopen("/sdcard/scveData/data.txt", "r");
    uint32_t count = 0;
    uint32_t temp1, temp2, temp3 = 0;
    if (fp != NULL) {
        while (!feof(fp)) {
            fscanf(fp, "%d %d %d %d\n", &temp1, &temp2, &temp3, &testdata[count++]);
        }
        fclose(fp);
    }
    if (count < 10) {
        return;
    }
    CVFileInput_t input;
    CVSumouput_t output;
    input.desired_length = 90;
    input.input_length   = count;
    input.inputFPS       = 30;
    //input.pBuffer        = testdata;
    CVSummaryFile(input, &output);
    fp = fopen("/sdcard/scveData/output.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "Total Video Length %d\n", count);
        for (uint32_t i = 0; i < output.output[0].output_seg; ++i) {
            fprintf(fp, "Total SegStart %d, SegStop %d, Segscore %d\n",
                    output.output[0].start_indx[i],
                    output.output[0].end_indx[i],
                    output.output[0].seg_score[i]);
        }

        fclose(fp);
    }
    free(testdata);
}

void CalculateFPS() {
#ifdef _ANDROID_

    DPRINTF(":E");
    static int calledcount = 0;
    static clock_t start, end;

    end = clock();
    if (calledcount == 0) {
        start = end;
    }
    calledcount++;
    if (((double)(end - start)) / CLOCKS_PER_SEC >= 1.0) {
        DPRINTF("Frame FPS %d", calledcount);
        calledcount = 0;
    }
#endif
}

void InitModules(uint32_t mode, CVisionInfo_t *pinfo) {
    CVManager_Module *pModule;

    if (checkmode(mode, MODE_BASIC)) {
        DPRINTF("Add Module BASIC");
    pModule =  MotionDetectCreateModule();
    CVManagerAddModule(pinfo, pModule);
    pModule ->identity = MODE_BASIC;
    }

    if (checkmode(mode, MODE_MOTION)) {
        DPRINTF("Add Module Motion");
        pModule = VideoSummaryCreateModule();
        CVManagerAddModule(pinfo, pModule);
        pModule ->identity = MODE_MOTION;
    }

    if (checkmode(mode, MODE_FACE)) {
        DPRINTF("Add Module Face");
        pModule = FaceRecognitionCreateModule();
        CVManagerAddModule(pinfo, pModule);
        pModule ->identity = MODE_FACE;
    }

    if (checkmode(mode, MODE_ASD)) {
        DPRINTF("Add Module Face");
        pModule = ASDCreateModule();
        CVManagerAddModule(pinfo, pModule);
        pModule ->identity = MODE_ASD;
    }

    CVManagerModuleInit(pInfo);
}

void GetCombinedscore(CVFileInput_t input, int32_t*output) {
    int32_t scores[MAX_MODULES];
    int32_t scoremask[MAX_MODULES];
    uint8_t found     = false;
    uint32_t input_no = 0;
    uint32_t tMODE    = 0;
    uint32_t INDXMODE[MAX_MODULES];

    for (uint32_t i = 0; i < MAX_MODULES; ++i) {
        tMODE       = 1 << i;
        if (checkmode(input.DATA_MODE, tMODE)) {
            INDXMODE[input_no] = tMODE;
            input_no ++;
        }
    }

    //DPRINTF("GetCombinedscore inputmode %d",input.MODE);
    for (uint32_t i = 0; i < input_no; ++i) {
        if (checkmode(INDXMODE[i], input.MODE)) scoremask[i] = 0xFFFFFFFF;
        else scoremask[i] = 0;
    }

    /////////////////////////////////////////
    for (uint32_t m = 0; m < input.input_length; ++m) {
        for (uint32_t i = 0; i < input_no; ++i) {
            if (INDXMODE[i] == MODE_FACE) {
                scores[i] = input.pBuffer[input_no * m + i] & 0xFF & scoremask[i];
            }
            else if (INDXMODE[i] == MODE_ASD) {
                scores[i] = ASDScoreTranslate(input.pBuffer[input_no * m + i]) & scoremask[i];
            }
            else
                scores[i] = input.pBuffer[input_no * m + i] & scoremask[i];
        }
        output[m] = CVManagerCombinescore(scores, input_no, input.DATA_MODE);
    }
}

void getdefaultsummary(CVFileInput_t input, CVSumouput_t *poutput) {
    poutput->ouput_num                  = 1;
    poutput->output[0].output_seg       = 1;
    if (input.desired_length == 0) poutput->output[0].output_length = MIN(mPara.data[PARA_MINSHOTLEN], input.input_length);
    else poutput->output[0].output_length = MIN(input.desired_length, input.input_length);
    poutput->output[0].start_indx[0]    = 0;
    poutput->output[0].end_indx[0]      = poutput->output[0].output_length - 1;
    poutput->output[0].seg_score[0]     = mPara.data[PARA_PEAKVALTHRESH];
}

void processpeaks(uint32_t *segstart, uint32_t *segstop, float32_t *first_order, uint32_t segcount, uint32_t inputlength) {
    //EXPAND segments in both directions to find maximum changes/shot boundary
    uint32_t sindx = 0;
    for (uint32_t i = 0; i < segcount; ++i) {
        if (i == 0) {
            for (sindx = segstart[i]; sindx > 5; sindx--) {
                if (first_order[sindx] > 1) {
                    break;
                }
            }
            segstart[i] = sindx;
        } else {
            for (sindx = segstart[i]; sindx > segstart[i] - mPara.data[PARA_VIDEOSEGSPACE]; sindx--) {
                if (first_order[sindx] > 1) {
                    break;
                }
            }
            segstart[i] = sindx;
        }

        if (i == segcount - 1) {
            for (sindx = segstop[i]; sindx < inputlength - 5; sindx++) {
                if (first_order[sindx] < -1) {
                    break;
                }
            }
            segstop[i] = sindx;
        } else {
            for (sindx = segstop[i]; sindx < segstop[i] + mPara.data[PARA_VIDEOSEGSPACE]; sindx++) {
                if (first_order[sindx] < -1) {
                    break;
                }
            }
            segstop[i] = sindx;
        }
    }
    //EXPAND segments in both directions to find start/stop of the singal ramp
    for (uint32_t i = 0; i < segcount; ++i) {
        if (i == 0) {
            for (sindx = segstart[i]; sindx > 5; sindx--) {
                if (first_order[sindx] <= 0.05) {
                    break;
                }
            }
            segstart[i] = sindx;
        } else {
            for (sindx = segstart[i]; sindx > segstart[i] - mPara.data[PARA_VIDEOSEGSPACE]; sindx--) {
                if (first_order[sindx] <= 0.05) {
                    break;
                }
            }
            segstart[i] = sindx;
        }

        if (i == segcount - 1) {
            for (sindx = segstop[i]; sindx < inputlength - 5; sindx++) {
                if (first_order[sindx] >= -0.05) {
                    break;
                }
            }
            segstop[i] = sindx;
        } else {
            for (sindx = segstop[i]; sindx < segstop[i] + mPara.data[PARA_VIDEOSEGSPACE]; sindx++) {
                if (first_order[sindx] >= -0.05) {
                    break;
                }
            }
            segstop[i] = sindx;
        }
    }
}
uint32_t gettargetlength(uint32_t inputlength, uint8_t ratio) {
    uint32_t length = 0;
    if (LOW_COMPRESSED == ratio) {
        if (inputlength < 60 * 30) {
            length = mPara.data[PARA_LOWCOMPRE_LEN1];
        } else if (inputlength < 60 * 30 * 5) {
            length = mPara.data[PARA_LOWCOMPRE_LEN2];
        } else if (inputlength < 60 * 30 * 30) {
            length = mPara.data[PARA_LOWCOMPRE_LEN3];
        } else {
            length = mPara.data[PARA_LOWCOMPRE_LEN4];
        }
    } else {
        if (inputlength < 60 * 30) {
            length = mPara.data[PARA_HIGHCOMPRE_LEN1];
        } else if (inputlength < 60 * 30 * 5) {
            length = mPara.data[PARA_HIGHCOMPRE_LEN2];
        } else if (inputlength < 60 * 30 * 30) {
            length = mPara.data[PARA_HIGHCOMPRE_LEN3];
        } else {
            length = mPara.data[PARA_HIGHCOMPRE_LEN4];
        }
    }
    DPRINTF("gettargetlength : X Returnd Length %d", length);
    return length;
}

uint32_t getSummary(CVSuminput_t input, CVSumouput_t *moutput,uint32_t targetlength, bool exactlength) 
{
    DPRINTF("getSummary :E");
    CVStatus_t status          = CV_SUCCESS;
    uint32_t totalinputlength  = 0;
    uint32_t totaloutputlength = 0;
    uint32_t seglevel          = 0;    
    bool isall                 = false;
    uint32_t outputseg[MAX_FILE_INPUT];

    if (false == exactlength) {
        if (input.summarytype == 0) { //Return for timelapse case
            //Get scene type
            Getscenetype(input, moutput);
            //Get most frequent known faces
            Getfacestar(input, moutput);
            DPRINTF("getSummary :X");
            return CV_SUCCESS;
        } else if (input.summarytype == LOW_COMPRESSED || input.summarytype == HIGH_COMPRESSED) {
            seglevel          = 0;
            totaloutputlength = 0;
            for (uint32_t i = 0; i < moutput->ouput_num; ++i) {
                outputseg[i] = moutput->output[i].output_seg;
                moutput->output[i].output_length = 0;
                moutput->output[i].output_seg    = 0;
            }

            while (totaloutputlength < targetlength && !isall) {
                isall = true;
                if (seglevel < 4) { //Get first one for each of the output
                    for (uint32_t i = 0; i < moutput->ouput_num; ++i) {
                        if (outputseg[i] > seglevel && totaloutputlength < targetlength) {
                            moutput->output[i].output_length += moutput->output[i].end_indx[seglevel] -
                               moutput->output[i].start_indx[seglevel];
                            totaloutputlength                += moutput->output[i].end_indx[seglevel] -
                               moutput->output[i].start_indx[seglevel];
                            isall                            = false;
                            moutput->output[i].output_seg++;
                        }
                    }
                    seglevel++;
                } else { //sort the rest of based on current
                    int32_t maxscore  = 0;
                    uint32_t maxindx  = 0;
                    for (uint32_t i = 0; i < moutput->ouput_num; ++i) {
                        if (outputseg[i] > moutput->output[i].output_seg && totaloutputlength < targetlength) {
                            if (moutput->output[i].seg_score[moutput->output[i].output_seg] > maxscore) {
                                maxindx = i;
                                maxscore = moutput->output[i].seg_score[moutput->output[i].output_seg];
                            }
                        }
                    }
                    DPRINTF("getSummary maxid %d",maxindx);
                    if (maxscore > 0) {
                        moutput->output[maxindx].output_length += moutput->output[maxindx].end_indx[moutput->output[maxindx].output_seg] -
                           moutput->output[maxindx].start_indx[moutput->output[maxindx].output_seg];
                        totaloutputlength                      += moutput->output[maxindx].end_indx[moutput->output[maxindx].output_seg] -
                           moutput->output[maxindx].start_indx[moutput->output[maxindx].output_seg];
                        isall                                   = false;
                        moutput->output[maxindx].output_seg++;
                    }
                }
            }
        } else {
            for (uint32_t i = 0; i < moutput->ouput_num; ++i) {
                moutput->output[i].output_length = moutput->output[i].end_indx[0] -
                   moutput->output[i].start_indx[0];
                moutput->output[i].output_seg    = 1;
            }
        }
    } else {  ///USER is asking for exact length
        seglevel = 0;
        totaloutputlength = 0;
        for (uint32_t i = 0; i < moutput->ouput_num; ++i) {
            outputseg[i] = moutput->output[i].output_seg;
            moutput->output[i].output_length = 0;
            moutput->output[i].output_seg    = 0;
        }

        while (totaloutputlength < targetlength && !isall) {
            isall = true;
            for (uint32_t i = 0; i < moutput->ouput_num; ++i) {
                if (outputseg[i] > seglevel && totaloutputlength < targetlength) {
                    if (totaloutputlength +
                        moutput->output[i].end_indx[seglevel] -
                        moutput->output[i].start_indx[seglevel] > targetlength && exactlength) {
                        moutput->output[i].start_indx[seglevel] = (moutput->output[i].end_indx[seglevel] + moutput->output[i].start_indx[seglevel]) / 2 - (targetlength - totaloutputlength) / 2;
                        moutput->output[i].end_indx[seglevel] = moutput->output[i].start_indx[seglevel] + (targetlength - totaloutputlength);
                    }
                    moutput->output[i].output_length  += moutput->output[i].end_indx[seglevel] - moutput->output[i].start_indx[seglevel];
                    totaloutputlength                 += moutput->output[i].end_indx[seglevel] - moutput->output[i].start_indx[seglevel];
                    isall                              = false;
                    moutput->output[i].output_seg++;
                }
            }
            seglevel++;
        }
    }
    //Get scene type
    Getscenetype(input, moutput);
    //Get most frequent known faces
    Getfacestar(input, moutput);
    DPRINTF("getSummary :X");
    return status;
}
