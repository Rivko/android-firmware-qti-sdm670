/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "CVManagerCommon.h"
#undef LOG_TAG
#define LOG_TAG "CVManagerUtil"

extern int32_t Scenetoscore[] = {
    0,  //ASDCV_AQUARIUM = 0,
    0,   //ASDCV_BEDROOM = 1,
    0,   //ASDCV_CLASSROOM = 2,
    0,   //ASDCV_CONFERENCE_ROOM = 3,
    0,   //ASDCV_DINING_ROOM = 4,
    20,  //ASDCV_HOME = 5,
    20,  //ASDCV_INDOOR = 6,
    0,   //ASDCV_KITCHEN = 7,
    0,   //ASDCV_LIVING_ROOM = 8,
    0,   //ASDCV_OFFICE = 9,
    0,  //ASDCV_RESTAURANT = 10,
    0,  //ASDCV_STORE = 11,
    0,   //ASDCV_SUPERMARKET = 12,
    0,   //ASDCV_WORK_PLACE = 13,
    0,   //ASDCV_PARKING_LOT_STRUCTURE = 14,
    20,  //ASDCV_INDUSTRIAL = 15,
    40,  //ASDCV_SPORTS = 16,
    25,  //ASDCV_AMUSEMENT_PARK = 17,
    0,   //ASDCV_ARCHITECTURE_BUILDING = 18,
    20,   //ASDCV_BRIDGE = 19,
    20,  //ASDCV_CHURCH = 20,
    20,  //ASDCV_PLAYGROUND = 21,
    20,  //ASDCV_SKYCRAPER = 22,
    15,  //ASDCV_STREET = 23,
    0,  //ASDCV_CANYON = 24,
    25,  //ASDCV_COAST = 25,
    25,  //ASDCV_DESERT = 26,
    25,  //ASDCV_FOREST = 27,
    20,   //ASDCV_GARDEN = 28,
    15,   //ASDCV_ICE = 29,
    20,  //ASDCV_LANDSCAPE = 30,
    20,  //ASDCV_MOUNTAIN = 31,
    20,  //ASDCV_OCEAN = 32,
    0,   //ASDCV_PLANTS = 33,
    20,  //ASDCV_POND = 34,
    20,  //ASDCV_RIVER = 35,
    0,  //ASDCV_SKY = 36,
    30,   //ASDCV_SWIMMING_POOL = 37,
    30,  //ASDCV_WATER = 38,
    0,  //ASDCV_PASTURE = 39
};

void readconfig(CVpara_t &tpara)
{
    DPRINTF("readconfig: E");
    FILE*fp;
    uint32_t count = 0;
    char paratype [256];
    int32_t val;
#ifdef __ANDROID__
    if ((fp = fopen("/sdcard/scveData/cvmanager.config", "r")) != NULL) {
        while (count < PARA_MAX && !feof(fp)) {
            fscanf(fp, "%s : %d", paratype, &tpara.data[count]);
            count++;
        }
        fclose(fp);
    } else DPRINTF("readconfig: File not existed!");
#else
    if ((fp = fopen("cvmanager.config", "r")) != NULL) {
        while (count < PARA_MAX && !feof(fp)) {
            fscanf(fp, "%s : %d", paratype, &tpara.data[count]);
            count++;
        }
        fclose(fp);
    } else DPRINTF("readconfig: File not existed!");
#endif
    Scenetoscore[0] = tpara.data[PARA_ASDCV_AQUARIUM];
    Scenetoscore[1] = tpara.data[PARA_ASDCV_BEDROOM];
    Scenetoscore[2] = tpara.data[PARA_ASDCV_CLASSROOM];
    Scenetoscore[3] = tpara.data[PARA_ASDCV_CONFERENCE_ROOM];
    Scenetoscore[4] = tpara.data[PARA_ASDCV_DINING_ROOM];
    Scenetoscore[5] = tpara.data[PARA_ASDCV_HOME];
    Scenetoscore[6] = tpara.data[PARA_ASDCV_INDOOR];
    Scenetoscore[7] = tpara.data[PARA_ASDCV_KITCHEN];
    Scenetoscore[8] = tpara.data[PARA_ASDCV_LIVING_ROOM];
    Scenetoscore[9] = tpara.data[PARA_ASDCV_OFFICE];
    Scenetoscore[10] = tpara.data[PARA_ASDCV_RESTAURANT];
    Scenetoscore[11] = tpara.data[PARA_ASDCV_STORE];
    Scenetoscore[12] = tpara.data[PARA_ASDCV_SUPERMARKET];
    Scenetoscore[13] = tpara.data[PARA_ASDCV_WORK_PLACE];
    Scenetoscore[14] = tpara.data[PARA_ASDCV_PARKING_LOT_STRUCTURE];
    Scenetoscore[15] = tpara.data[PARA_ASDCV_INDUSTRIAL];
    Scenetoscore[16] = tpara.data[PARA_ASDCV_SPORTS];
    Scenetoscore[17] = tpara.data[PARA_ASDCV_AMUSEMENT_PARK];
    Scenetoscore[18] = tpara.data[PARA_ASDCV_ARCHITECTURE_BUILDING];
    Scenetoscore[19] = tpara.data[PARA_ASDCV_BRIDGE];
    Scenetoscore[20] = tpara.data[PARA_ASDCV_CHURCH];
    Scenetoscore[21] = tpara.data[PARA_ASDCV_PLAYGROUND];
    Scenetoscore[22] = tpara.data[PARA_ASDCV_SKYCRAPER];
    Scenetoscore[23] = tpara.data[PARA_ASDCV_STREET];
    Scenetoscore[24] = tpara.data[PARA_ASDCV_CANYON];
    Scenetoscore[25] = tpara.data[PARA_ASDCV_COAST];
    Scenetoscore[26] = tpara.data[PARA_ASDCV_DESERT];
    Scenetoscore[27] = tpara.data[PARA_ASDCV_FOREST];
    Scenetoscore[28] = tpara.data[PARA_ASDCV_GARDEN];
    Scenetoscore[29] = tpara.data[PARA_ASDCV_ICE];
    Scenetoscore[30] = tpara.data[PARA_ASDCV_LANDSCAPE];
    Scenetoscore[31] = tpara.data[PARA_ASDCV_MOUNTAIN];
    Scenetoscore[32] = tpara.data[PARA_ASDCV_OCEAN];
    Scenetoscore[33] = tpara.data[PARA_ASDCV_PLANTS];
    Scenetoscore[34] = tpara.data[PARA_ASDCV_POND];
    Scenetoscore[35] = tpara.data[PARA_ASDCV_RIVER];
    Scenetoscore[36] = tpara.data[PARA_ASDCV_SKY];
    Scenetoscore[37] = tpara.data[PARA_ASDCV_SWIMMING_POOL];
    Scenetoscore[38] = tpara.data[PARA_ASDCV_WATER];
    Scenetoscore[39] = tpara.data[PARA_ASDCV_PASTURE];
}
CVpara_t initconfig()
{
    CVpara_t mpara;
    mpara.data[PARA_PEAKVALTHRESH]      = PEAK_VAL_THRESH;
    mpara.data[PARA_LOCALPEAKSPACE]     = LOCAL_PEAK_RANGE;
    mpara.data[PARA_VIDEOSEGSPACE]      = VIDEO_SEG_SPACE;
    mpara.data[PARA_MINSHOTLEN]         = MIN_SHOT_LENGTH;
    mpara.data[PARA_MAXSHOTLEN]         = MAX_SHOT_LENGTH;
    mpara.data[PARA_TIMELAPSE_THRESH]   = PEAK_VAL_THRESH;
    mpara.data[PARA_LOWCOMPRE_LEN1]     = 30*15;         //  60 second or less
    mpara.data[PARA_LOWCOMPRE_LEN2]     = 30*45;         //  5 mins or less
    mpara.data[PARA_LOWCOMPRE_LEN3]     = 30*60*2;       //  30 mins or less
    mpara.data[PARA_LOWCOMPRE_LEN4]     = 30*60*10;      //  larger than 30 min
    mpara.data[PARA_HIGHCOMPRE_LEN1]    = 30*3;          //  60 second or less
    mpara.data[PARA_HIGHCOMPRE_LEN2]    = 30*6;          //  5 mins or less
    mpara.data[PARA_HIGHCOMPRE_LEN3]    = 30*30;         //  30 mins or less
    mpara.data[PARA_HIGHCOMPRE_LEN4]    = 30*60;         //  larger than 30 min
    return mpara;
}

