#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fastcv/fastcv.h>

#include "MotionDetect.h"
#include "CVManagerCommon.h"

#ifdef _WIN32
#include <cstring>
#elif _ANDROID_
#include <pthread.h>
#include <utils/Log.h>
#else
#include <android/log.h>
#endif

#undef LOG_TAG
#define LOG_TAG "MotionDetect"

CVMotionDetect::CVMotionDetect()
{
}

CVMotionDetect::~CVMotionDetect()
{
}

void CVMotionDetect::CVMotionDetectInit()
{
    pCurImage     = NULL;
    pPreImage     = NULL;
    pDiffImage    = NULL;
    pMotionImage  = NULL;
    pMotionImageD = NULL;
    pMotionImageD2= NULL;

    mDecisionMode = 0;
    mRunningFPS   = 2;

    mDividedWidth  = 3;
    mDividedHeight = 3;

    linebuffer = (uint8_t*)fcvMemAlloc(MOTIOND_WIDTH * 48, 16);
}
void CVMotionDetect::CVMotionDetectDeInit()
{
    fcvMemFree(linebuffer);
    if(pCurImage)
    {
        fcvMemFree(pCurImage);
        pCurImage = NULL;
    }
    if(pPreImage)
    {
        fcvMemFree(pPreImage);
        pPreImage = NULL;
    }
    if(pDiffImage)
    {
        fcvMemFree(pDiffImage);
        pDiffImage = NULL;
    }

    if(pMotionImage)
    {
        fcvMemFree(pMotionImage);
        pMotionImage = NULL;
    }
    if(pMotionImageD)
    {
        fcvMemFree(pMotionImageD);
        pMotionImageD = NULL;
    }
    if(pMotionImageD2)
    {
        fcvMemFree(pMotionImageD2);
        pMotionImageD2 = NULL;
    }
}

void CVMotionDetect::ChangeMode(uint8_t mode)
{
    if(mode >= 1 && mode <=2)
        mDecisionMode = mode;
}


int32_t CVMotionDetect::CheckDividedMotion(uint8_t *ImginputY,
                            uint32_t Imgwidth,
                            uint32_t Imgheight,
                            uint32_t srcYStride,
                            uint32_t DividedWidth,
                            uint32_t DividedHeight)
{
    DPRINTF("CheckDividedMotion :E");
    int32_t result = 0;

    uint32_t offset = 0;
    int32_t widthstep = (int32_t)(Imgwidth / DividedWidth);
    int32_t heightstep = (int32_t)(Imgheight / DividedHeight);
    mDividedWidth = DividedWidth;
    mDividedHeight = DividedHeight;

    if(pCurImage == NULL && Imgwidth != 0 && Imgheight != 0){
        mImgwidth     = Imgwidth;
        mImgheight    = Imgheight;
        pCurImage     = (uint8_t*)fcvMemAlloc(Imgwidth * Imgheight, 16);
        pPreImage     = (uint8_t*)fcvMemAlloc(Imgwidth * Imgheight, 16);
        pMotionImage  = (uint8_t*)fcvMemAlloc(Imgwidth * Imgheight, 16);
        pMotionImageD = (uint8_t*)fcvMemAlloc(MOTIOND_WIDTH * MOTIOND_HEIGHT, 16);
        pMotionImageD2= (uint8_t*)fcvMemAlloc(MOTIOND_WIDTH * MOTIOND_HEIGHT, 16);
        DPRINTF("CheckDividedMotion :Alloc Memmory");
        memcpy(pPreImage, ImginputY, Imgwidth * Imgheight);
        return 0;
    } else
        memcpy(pCurImage, ImginputY, Imgwidth * Imgheight);

    fcvAbsDiffu8(pCurImage, pPreImage,Imgwidth, Imgheight, Imgwidth, pMotionImage, Imgwidth);
    fcvScaleDownNNu8(pMotionImage,Imgwidth, Imgheight, Imgwidth,pMotionImageD,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH);

    memcpy(pPreImage, ImginputY, Imgwidth * Imgheight);
    DPRINTF("CheckDividedMotion :X");
    return result;
}

int32_t CVMotionDetect::CheckROIMotionByDiff(uint8_t *ImginputY,
                            uint32_t Imgwidth,
                            uint32_t Imgheight,
                            uint32_t srcYStride,
                            uint32_t mroileft,
                            uint32_t mroitop,
                            uint32_t mroiwidth,
                            uint32_t mroiheight)
{
    DPRINTF("CheckROIMotionByDiff :E");
    int32_t result      = 0;
    float  average      = 0;
    if(pCurImage == NULL && Imgwidth != 0 && Imgheight != 0){
        pCurImage = (uint8_t*)fcvMemAlloc(Imgwidth * Imgheight, 16);
        pPreImage = (uint8_t*)fcvMemAlloc(Imgwidth * Imgheight, 16);
        pDiffImage = (uint8_t*)fcvMemAlloc(Imgwidth * Imgheight , 16);
        memcpy(pPreImage, ImginputY, Imgwidth * Imgheight);
        DPRINTF(":Alloc Memmory");
        return 0;
    } else
        memcpy(pCurImage, ImginputY, Imgwidth * Imgheight);

    uint32_t offset    = mroitop*Imgwidth + mroileft;
    DPRINTF("offset %d, mroiwidth %d, mroiheight %d, stride %d",offset, mroiwidth,mroiheight, Imgwidth);
    fcvAbsDiffu8(pCurImage + offset, pPreImage + offset,\
                   mroiwidth, mroiheight, Imgwidth, pDiffImage, mroiwidth);
    fcvAverageu8(pDiffImage, mroiwidth, mroiheight, mroiwidth, &average);
    memcpy(pPreImage, pCurImage, Imgwidth * Imgheight);
    result = (int32_t)average;
    DPRINTF(":Image Diff Average %f",average);
    DPRINTF(":X");
    return result;
}

int32_t CVMotionDetect::DecisionMaking(int32_t DiffMotion)
{
    DPRINTF(":E");
    int32_t result = 0;
    switch (mDecisionMode){
        case 0:
            DecisionMakeZero(DiffMotion);
            break;
        case 1:
            DecisionMakeOne(DiffMotion);
            break;
        case 2:
            DecisionMakeTwo(DiffMotion);
            break;
        default:
            DecisionMakeOne(DiffMotion);
            break;
    }
    DPRINTF(":X");
    return result;
}

int32_t CVMotionDetect::DecisionMakeZero(int32_t Trigger)
{
    DPRINTF(":E");
    int32_t result = 0;
    if(Trigger == 0)
    {
        mCountB ++;
        mCountS = 0;
    }
    else
    {
        mCountS ++;
        mCountB = 0;
    }
    DPRINTF("%s Count B %d, S %d , runing mode %d",__FUNCTION__, mCountB, mCountS, mRunningFPS);
    if(mCountB  >= 30 && mRunningFPS != 3)
    {
        DPRINTF("%s Config Change to 3",__FUNCTION__);
        FILE *fp;
        fp = fopen("/data/cvmanager/config.txt", "w");
        if (fp != NULL)
        {
          fputc('3',fp);
          fclose(fp);
          mRunningFPS = 3;
          DPRINTF("%s Write to File 3",__FUNCTION__);
        }
        else
        {
            DPRINTF("%s Config File open Failed",__FUNCTION__);
        }
    }
    else if (mCountS >= 3 && mRunningFPS != 0)
    {
        DPRINTF("%s Config Change to 0",__FUNCTION__);
        FILE *fp;
        fp = fopen("/data/cvmanager/config.txt", "w");
        if (fp != NULL)
        {
          fputc('0',fp);
          fclose(fp);
          mRunningFPS = 0;
          DPRINTF("%s Write to File 0",__FUNCTION__);
        }
        else
        {
            DPRINTF("%s Config File open Failed",__FUNCTION__);
        }
    }
    DPRINTF(":X");
    return result;
}

//Speed up when Scene is stable and normal speed when scene is changing
int32_t CVMotionDetect::DecisionMakeTwo(int32_t DiffMotion)
{
    DPRINTF(":E");
    int32_t result = 0;
    if(DiffMotion > 5)
    {
        mCountB ++;
        mCountS = 0;
    }
    else
    {
        mCountS ++;
        mCountB = 0;
    }
    DPRINTF("%s Count B %d, S %d , runing mode %d",__FUNCTION__, mCountB, mCountS, mRunningFPS);
    if(mCountB  >= 5 && mRunningFPS != 0)
    {
        DPRINTF("%s Config Change to 0",__FUNCTION__);
        FILE *fp;
        fp = fopen("/data/cvmanager/config.txt", "w");
        if (fp != NULL)
        {
          fputc('0',fp);
          fclose(fp);
          mRunningFPS = 0;
          DPRINTF("%s Write to File 0",__FUNCTION__);
        }
        else
        {
            DPRINTF("%s Config File open Failed",__FUNCTION__);
        }
    }
    else if (mCountS >= 25 && mRunningFPS != 3)
    {
        DPRINTF("%s Config Change to 3",__FUNCTION__);
        FILE *fp;
        fp = fopen("/data/cvmanager/config.txt", "w");
        if (fp != NULL)
        {
          fputc('3',fp);
          fclose(fp);
          mRunningFPS = 3;
          DPRINTF("%s Write to File 3",__FUNCTION__);
        }
        else
        {
            DPRINTF("%s Config File open Failed",__FUNCTION__);
        }
    }
    DPRINTF(":X");
    return result;
}

int32_t CVMotionDetect::DecisionMakeOne(int32_t DiffMotion)
{
    DPRINTF(":E");
    int32_t result = 0;
    if(DiffMotion > 5)
    {
        mCountB ++;
        mCountS = 0;
    }
    else
    {
        mCountS ++;
        mCountB = 0;
    }
    DPRINTF("%s Count B %d, S %d , runing mode %d",__FUNCTION__, mCountB, mCountS, mRunningFPS);
    if(mCountB  >= 5 && mRunningFPS != 3)
    {
        DPRINTF("%s Config Change to 3",__FUNCTION__);
        FILE *fp;
        fp = fopen("/data/cvmanager/config.txt", "w");
        if (fp != NULL)
        {
          fputc('3',fp);
          fclose(fp);
          mRunningFPS = 3;
          DPRINTF("%s Write to File 3",__FUNCTION__);
        }
        else
        {
            DPRINTF("%s Config File open Failed",__FUNCTION__);
        }
    }
    else if (mCountS >= 15 && mRunningFPS != 0)
    {
        DPRINTF("%s Config Change to 0",__FUNCTION__);
        FILE *fp;
        fp = fopen("/data/cvmanager/config.txt", "w");
        if (fp != NULL)
        {
          fputc('0',fp);
          fclose(fp);
          mRunningFPS = 0;
          DPRINTF("%s Write to File 0",__FUNCTION__);
        }
        else
        {
            DPRINTF("%s Config File open Failed",__FUNCTION__);
        }
    }
    DPRINTF(":X");
    return result;
}

void CVMotionDetect::regiongrow(uint32_t &minx,
    uint32_t &maxx,
    uint32_t &miny,
    uint32_t &maxy,
    uint8_t *imgsrc,
    uint8_t *img,
    uint32_t seedx,
    uint32_t seedy,
    uint32_t imgwidhth,
    uint32_t imgheight,
    uint32_t threshvalue)
{
    uint32_t index = 0;
    if(seedx > 0 & seedx < imgwidhth-1 && seedy > 0 && seedy < imgheight-1)
    {
        printf("SeedX,%d,seedY, %d,\n",seedx, seedy);
        img[seedy*imgwidhth + seedx] = 1;
        index = seedy*imgwidhth + seedx-1;
        if(imgsrc[index] >= threshvalue && img[index] != 1)
        {
            img[index] = 1;
            if(seedx - 1 < minx)
                minx = seedx -1;
            regiongrow(minx,maxx,miny,maxy,imgsrc,img,seedx-1,seedy,imgwidhth,imgheight,threshvalue);
        }
        index = (seedy-1)*imgwidhth + seedx-1;
        if(imgsrc[index] >= threshvalue && img[index] != 1)
        {
            img[index] = 1;
            if(seedx - 1 < minx)
                minx = seedx -1;
            if(seedy - 1 < miny)
                miny = seedy -1;
            regiongrow(minx,maxx,miny,maxy,imgsrc,img,seedx-1,seedy-1,imgwidhth,imgheight,threshvalue);
        }
        index = (seedy+1)*imgwidhth + seedx-1;
        if(imgsrc[index] >= threshvalue && img[index] != 1)
        {
            img[index] = 1;
            if(seedx - 1 < minx)
                minx = seedx -1;
            if(seedy + 1 > maxy)
                maxy = seedy +1;
            regiongrow(minx,maxx,miny,maxy,imgsrc,img,seedx-1,seedy+1,imgwidhth,imgheight,threshvalue);
        }
        index = seedy*imgwidhth + seedx+1;
        if(imgsrc[index] >= threshvalue && img[index] != 1)
        {
            img[index] = 1;
            if(seedx + 1 > maxx)
                maxx = seedx +1;
            regiongrow(minx,maxx,miny,maxy,imgsrc,img,seedx+1,seedy,imgwidhth,imgheight,threshvalue);
        }
        index = (seedy-1)*imgwidhth + seedx+1;
        if(imgsrc[index] >= threshvalue && img[index] != 1)
        {
            img[index] = 1;
            if(seedx + 1 > maxx)
                maxx = seedx + 1;
            if(seedy - 1 < miny)
                miny = seedy -1;
            regiongrow(minx,maxx,miny,maxy,imgsrc, img,seedx+1,seedy-1,imgwidhth,imgheight,threshvalue);
        }
        index = (seedy+1)*imgwidhth + seedx+1;
        if(imgsrc[index] >= threshvalue && img[index] != 1)
        {
            img[index] = 1;
            if(seedx + 1 > maxx)
                maxx = seedx + 1;
            if(seedy + 1 > maxy)
                maxy = seedy +1;
            regiongrow(minx,maxx,miny,maxy,imgsrc,img,seedx+1,seedy+1,imgwidhth,imgheight,threshvalue);
        }
        index = (seedy-1)*imgwidhth + seedx;
        if(imgsrc[index] >= threshvalue && img[index] != 1)
        {
            img[index] = 1;
            if(seedy - 1 < miny)
                miny = seedy -1;
            regiongrow(minx,maxx,miny,maxy,imgsrc, img,seedx,seedy-1,imgwidhth,imgheight,threshvalue);
        }
        index = (seedy+1)*imgwidhth + seedx;
        if(imgsrc[index] >= threshvalue && img[index] != 1)
        {
            img[index] = 1;
            if(seedy + 1 > maxy)
                maxy = seedy +1;
            regiongrow(minx,maxx,miny,maxy,imgsrc,img,seedx,seedy+1,imgwidhth,imgheight,threshvalue);
        }
    }
    return;
}

void CVMotionDetect::DetermineMotionLocation(uint32_t Imgwidth,uint32_t Imgheight )
{
    int32_t result = 0;
    /*
    int8_t LapKernel[9] = {0,-1,0,-1,4,1,0,-1,0};
    //fcvFilterGaussian5x5u8_v2(pMotionImageD,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH,pMotionImageD2,MOTIOND_WIDTH,0);
    //fcvFilterThresholdu8_v2(pMotionImageD,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH,pMotionImageD,MOTIOND_WIDTH,3);
    //fcvFilterCorr3x3s8_v2(LapKernel,pMotionImageD2,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH,pMotionImageD,MOTIOND_WIDTH);
    //fcvFilterDilate3x3u8_v2(pMotionImageD,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH,pMotionImageD2,MOTIOND_WIDTH);
    */
    fcvFilterGaussian3x3u8_v2(pMotionImageD,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH,pMotionImageD2,MOTIOND_WIDTH,1);
    fcvFilterMedian3x3u8_v2(pMotionImageD2,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH,pMotionImageD,MOTIOND_WIDTH);
    memset(pMotionImageD2,0,MOTIOND_WIDTH*MOTIOND_HEIGHT);
    memset(PeakX,0,sizeof(uint32_t)*MAX_PEAKS);
    memset(PeakY,0,sizeof(uint32_t)*MAX_PEAKS);
    memset(mTrackedLoc_widthT,0,sizeof(uint32_t) * MAX_PEAKS);
    memset(mTrackedLoc_heightT,0,sizeof(uint32_t) * MAX_PEAKS);
    FindPeaks(pMotionImageD,MOTIOND_WIDTH,MOTIOND_HEIGHT);
    fcvFilterThresholdu8_v2(pMotionImageD,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH,pMotionImageD,MOTIOND_WIDTH,3);
    fcvConnectedComponent mCC;
    for(int i = 0; i < PeaksFound; ++i)
    {
        fcvFloodfillSimpleu8(pMotionImageD,MOTIOND_WIDTH,MOTIOND_HEIGHT,MOTIOND_WIDTH,
            pMotionImageD2,MOTIOND_WIDTH,PeakX[i],PeakY[i],255,&mCC,8,linebuffer);
        if(mCC.area > 35)
        {
            PeakX[i]                 = PeakX[i] * mImgwidth / MOTIOND_WIDTH;
            PeakY[i]                 = PeakY[i] * mImgheight / MOTIOND_HEIGHT;
            mTrackedLoc_xT[i]        = mCC.rectTopLeftX * mImgwidth / MOTIOND_WIDTH;
            mTrackedLoc_yT[i]        = mCC.rectTopLeftY * mImgheight / MOTIOND_HEIGHT;
            mTrackedLoc_widthT[i]    = mCC.rectWidth * mImgwidth / MOTIOND_WIDTH;
            mTrackedLoc_heightT[i]   = mCC.rectHeight * mImgheight / MOTIOND_HEIGHT;

            result = 1;
        }
    }
    //DecisionMaking(result);
}

bool CVMotionDetect::CheckPeak(int32_t x, int32_t y, int32_t range,int32_t foundpeaks)
{
    for(int32_t i = 0;i < foundpeaks;++i)
        if(abs(x-int32_t(PeakX[i])) < range || abs(y - int32_t(PeakY[i])) < range)
            return false;
    return true;
}

void CVMotionDetect::FindPeaks(uint8_t *imgsrc,uint32_t imgwidth,uint32_t imgheight)
{
    PeaksFound = 0;
    uint32_t Area = imgwidth / 15 > imgheight / 15 ? imgwidth / 15 : imgheight / 15;

    for(uint32_t i = 0; i < imgheight;++i)
    {
        for(uint32_t j = 0;j < imgwidth; ++j)
        {
            if(imgsrc[i*imgwidth + j] > 15 && imgsrc[i*imgwidth + j] < 150)
            {
                if(CheckPeak(j,i,Area,PeaksFound))
                {
                    PeakX[PeaksFound] = j;
                    PeakY[PeaksFound] = i;
                    PeaksFound ++;
                }
            }
        }
    }
    return;
}