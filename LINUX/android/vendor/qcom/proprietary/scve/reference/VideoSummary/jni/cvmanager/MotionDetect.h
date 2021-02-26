/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _CV_MotionDetect_H_
#define _CV_MotionDetect_H_

#include <stdint.h>

//#define ACTION_LAPSE_MODE 1
//#define STABLE_LAPSE_MODE 2
#define MAX_PEAKS 20
#define MOTIOND_WIDTH 160
#define MOTIOND_HEIGHT 120
//extern "C" {
class CVMotionDetect
{
public:    
    uint8_t* pCurImage;
    uint8_t* pPreImage; 
    uint8_t* pDiffImage;
    uint8_t* pMotionImage;
    uint8_t* pMotionImageD;
    uint8_t* pMotionImageD2;
    
    uint8_t mDecisionMode;
    uint8_t mRunningFPS;
    int32_t mCountB;
    int32_t mCountS;    

    int32_t mImgwidth;
    int32_t mImgheight;
        
    uint8_t mDividedWidth;
    uint8_t mDividedHeight;        

    uint32_t mTrackedLoc_xT[MAX_PEAKS];
    uint32_t mTrackedLoc_yT[MAX_PEAKS];
    uint32_t mTrackedLoc_widthT[MAX_PEAKS];
    uint32_t mTrackedLoc_heightT[MAX_PEAKS];

    uint8_t  PeaksFound;
    uint32_t PeakX[MAX_PEAKS];
    uint32_t PeakY[MAX_PEAKS];
    uint8_t *linebuffer;
public:  
    CVMotionDetect();
    ~CVMotionDetect();

    void CVMotionDetectInit();
    void CVMotionDetectDeInit();
    int32_t DecisionMakeOne(int32_t Diffmotion);
    int32_t DecisionMakeTwo(int32_t Diffmotion);
    int32_t DecisionMakeZero(int32_t Trigger);
    void ChangeMode(uint8_t mode);
    void DetermineMotionLocation(uint32_t Imgwidth,uint32_t Imgheight);    
    void FindPeaks(uint8_t *ImgSrc,uint32_t imgwidth,uint32_t imgheight);
    bool CheckPeak(int32_t x, int32_t y, int32_t range,int32_t foundpeaks);
    void regiongrow(uint32_t &minX,
                    uint32_t &maxX,
                    uint32_t &minY,
                    uint32_t &maxY,
                    uint8_t *Imgsrc,
                    uint8_t *Img,
                    uint32_t SeeDX,
                    uint32_t SeeDY,
                    uint32_t imgwidth,
                    uint32_t imgheight,
                    uint32_t threshValue);
    
    int32_t CheckDividedMotion(uint8_t *ImginputY,
                        uint32_t Imgwidth,
                        uint32_t Imgheight, 
                        uint32_t srcYStride,                            
                        uint32_t DividedWidth,
                        uint32_t DividedHeight);
    
    int32_t CheckROIMotionByDiff(uint8_t *ImginputY, 
                        uint32_t Imgwidth,
                        uint32_t Imgheight, 
                        uint32_t srcYStride,
                        uint32_t roileft,
                        uint32_t roitop,
                        uint32_t roiwidth,
                        uint32_t roiheight);
    int32_t DecisionMaking(int32_t DiffMotion);    
};

const int8_t LoG9x9[] = {0,1,1,2,2,2,1,1,0,
                         1,2,4,5,5,5,4,2,1,
                         1,4,5,3,0,3,5,4,1,
                         2,5,3,-12,-24,-12,3,5,2,
                         2,5,0,-24,-40,-24,0,5,2,
                         2,5,3,-12,-24,-12,3,5,2,
                         1,4,5,3,0,3,5,4,1,
                         1,2,4,5,5,5,4,2,1,
                         0,1,1,2,2,2,1,1,0}; //Gaussian theta = 1.4

#endif