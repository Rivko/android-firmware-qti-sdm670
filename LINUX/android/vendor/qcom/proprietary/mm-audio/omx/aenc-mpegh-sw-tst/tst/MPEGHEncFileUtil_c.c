/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   MPEGHEncFileUtil_c.c            TYPE: C-source file          *]
[*                                                                           *]
[* DESCRIPTION: Implementation of MPEG-H encoder file utilities              *]
[*****************************************************************************/
#include <string.h>
#include <stdint.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <dirent.h>
#endif
#include "MPEGHEncFileUtil_c.h"

#define BITBUF_SIZE 8192 *8


/**
* Helper function to determine number of HOA components (channels) given an HOA order.
*
* @param[in] order
*    The HOA order
*
* @return
*    The number of HOA components corresponding to that order.
*/
static __inline uint32_t mhe_GetNumHOAComponentsFromOrder(uint32_t order)
{
    return ((order + 1)*(order + 1));
}

/**
* Helper function to determine the HOA order and index within that order for a given channel index and given
* HOA channel sequence.
*
* For instance, for a SID (3DMPEG) channel sequence (HOA channel order), the order and index pairs look like:
*
* (0,0), (1,+1), (1,-1), (1, 0), (2,+2), (2,-2), (2,+1), (2,-1), (2,0), (3,+3), ...
*
* Index 7 (counting from 0) would correspond to the pair (2,-1), so this function would return order=2 and orderIdx=-1,
* if hoaChannelOrder is 3DMPEG.
*
* @param[in] hoaChannelOrder
*    The channel sequence (3D or 3DMPEG)
*
* @param[in] idx
*    The channel index of PCM data.
*
* @param[out] order
*    The order corresponding to idx
*
* @param[out] orderIdx
*    The index within the order, corresponding to idx
*/
static __inline void mhe_GetHOAIndex(HOAChannelOrder hoaChannelOrder, uint32_t idx, uint32_t *order, int32_t *orderIdx)
{
    uint32_t o = 0;
    while (mhe_GetNumHOAComponentsFromOrder(o) < (idx + 1)) {
        o++;
    }
    *order = o;
    if (o > 0) {
        idx -= mhe_GetNumHOAComponentsFromOrder(o - 1);
    }

    if (hoaChannelOrder == ORDER_3D) {
        *orderIdx = idx - o;
    }
    else { /* ORDER_3DMPEG */
        int32_t oi = 2 * o - idx + 1;
        *orderIdx = (oi >> 1) * ((oi & 1) ? 1 : -1);
    }
}

typedef struct RendererInfo {
    // const char *fileName;
    // const char *friendlyName;
    int32_t hoaOrder;
}RendererInfo;

int32_t countNumberOfWaveFilesInDir(char *itemDir)
{
#ifdef WIN32
    int32_t fileCounter = 0;
    WIN32_FIND_DATA ffd;
    char extension[5] = { "*.wav" };
    char szDir[200];
    HANDLE hFind;

    // Create the search string with directory path and extension
    strcpy(szDir, itemDir);
    strcat(szDir, "\\*.wav");

    // Find the first file in the directory.
    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        printf("Could not find .wav file\n");
        FindClose(hFind);
        return -1;
    }

    // Count all the files in the directory with .wav extension
    do {
        fileCounter++;
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
    return fileCounter;
#else
    int32_t fileCounter, len;
    struct dirent *pDirent;
    DIR *pDir;

    fileCounter= 0;
    pDir = opendir(itemDir);
    if (pDir != NULL) {
        while ((pDirent = readdir(pDir)) != NULL) {
            len = strlen (pDirent->d_name);
            if (len >= 4) {
                if (strcmp (".wav", &(pDirent->d_name[len - 4])) == 0) {
                    fileCounter++;
                    //printf ("%s\n", pDirent->d_name);
                }
            }
        }
        closedir (pDir);
    } else {
        printf("\n Error in path specified...No .wav files found.. exiting");
        return -1;
    }

    if(fileCounter == 0) {
       printf("\n No .wav files found.. exiting");
       return -1;
    }

    return fileCounter;
#endif
}

int32_t MPEGHEncFileUtil_openInputWavFiles(MPEGHEncFileUtilStruct *fileData, char *itemDir, char *itemName, HOAChannelOrder hoaChannelOrder, int32_t hoaOrderFromArg)
{

  int32_t ret = WAV_SUCCESS, numComp, numWavFiles, _hoaOrder, maxHoaOrderSupportedBasedOnInputWavFileCount;

  /* No argument for hoaOrder passed from command line,
  then count the number of mono wave files in the dir
  */

  numWavFiles = countNumberOfWaveFilesInDir(itemDir);

  if (numWavFiles <= 0) {
      fprintf(stderr, "No input wave files found \n");
      return -1;
  }

  // Renderers for HOA order 3, 4, and 6 are supported
  // Input channels beyond the expected number for the specified order will be ignored.
  maxHoaOrderSupportedBasedOnInputWavFileCount = 0;
  if (numWavFiles >= (int)mhe_GetNumHOAComponentsFromOrder(3)) {
      maxHoaOrderSupportedBasedOnInputWavFileCount = 3;
  }
  if (numWavFiles >= (int)mhe_GetNumHOAComponentsFromOrder(4)) {
      maxHoaOrderSupportedBasedOnInputWavFileCount = 4;
  }
  if (numWavFiles >= (int)mhe_GetNumHOAComponentsFromOrder(6)) {
      maxHoaOrderSupportedBasedOnInputWavFileCount = 6;
  }
  if (maxHoaOrderSupportedBasedOnInputWavFileCount == 0) {
      // No Input wave files found
      ret = -1;
  }

  if (hoaOrderFromArg == -1) {
      //No hoa order input from command line.
      numComp = (int)mhe_GetNumHOAComponentsFromOrder(maxHoaOrderSupportedBasedOnInputWavFileCount);
      fileData->_numInputChannels = numComp;
      fileData->_hoaOrder = maxHoaOrderSupportedBasedOnInputWavFileCount;
  } else {
      _hoaOrder = min(hoaOrderFromArg, maxHoaOrderSupportedBasedOnInputWavFileCount);
      numComp = (int)mhe_GetNumHOAComponentsFromOrder(_hoaOrder);

      fileData->_numInputChannels = numComp;
      fileData->_hoaOrder = _hoaOrder;
  }

  for (uint32_t i = 0; i < 50; i++) {
    fileData->_inputWavFiles[i]._fp = NULL;
    /*fileData->_inputWavFiles[i]._waveBuf = NULL;*/
  }

  for (int32_t i = 0; i < numComp; i++) {
    uint32_t order;
    int32_t orderIdx;
    char numToString[12];
    mhe_GetHOAIndex(hoaChannelOrder, i, &order, &orderIdx);
    char filename_c[255];

    /* create the entire name along with path of the wave file to open */

    strlcpy(filename_c, itemDir, (strlen(itemDir) + 1));
    strlcat(filename_c, "/", (strlen("/") + 1));
    strlcat(filename_c, itemName, (strlen(itemName) + 1));
    strlcat(filename_c, "_", (strlen("_") + 1));
    sprintf(numToString, "%d", fileData->_hoaOrder); // tried using itoa() function here, but not supported in GCC
    strlcat(filename_c, numToString, (strlen(numToString) + 1));
    strlcat(filename_c, "_", (strlen("_") + 1));
    sprintf(numToString, "%d", order);
    strlcat(filename_c, numToString, (strlen(numToString) + 1));

    if (orderIdx < 0) {
      sprintf(numToString, "%d", -orderIdx);
      strlcat(filename_c, numToString, (strlen(numToString) + 1));
      strlcat(filename_c, "-", (strlen("-") + 1));
    }
    else {
      sprintf(numToString, "%d", orderIdx);
      strlcat(filename_c, numToString, (strlen(numToString) + 1));
      strlcat(filename_c, "+", (strlen("+") + 1));
    }

    strlcat(filename_c, ".wav", (strlen(".wav") + 1));

    /* open the wave file */
    ret = InWavFile_open(filename_c, &fileData->_inputWavFiles[i]);

    if (ret) {
      for (int32_t t = 0; t < i; t++) {
          if (fileData->_inputWavFiles[t]._fp != NULL)
              fclose(fileData->_inputWavFiles[t]._fp);
      }

      return ret;
    }
  }

  return ret;
}

int32_t MPEGHEncFileUtil_openMultiChannelInputWavFile(MPEGHEncFileUtilStruct *fileData, char *filePath, int32_t hoaOrderFromArg)
{
    int ret = 0;
    int _hoaOrder, _numInputChannels, maxHoaOrderSupportedBasedOnInputChannels;

    for (uint32_t i = 0; i < 50; i++) {
        fileData->_inputWavFiles[i]._fp = NULL;   // Initializing all file pointers to NULL
    }

    /* open the wave file */
    ret = InWavFile_open(filePath, &fileData->_inputWavFiles[0]);  // Usingthe 0th file pointer as one file only

    _numInputChannels = fileData->_inputWavFiles[0]._wh.numChannels;

    maxHoaOrderSupportedBasedOnInputChannels = 0;
    if (_numInputChannels >= (int)mhe_GetNumHOAComponentsFromOrder(1)) {
        maxHoaOrderSupportedBasedOnInputChannels = 1;
    }
    if (_numInputChannels >= (int)mhe_GetNumHOAComponentsFromOrder(2)) {
        maxHoaOrderSupportedBasedOnInputChannels = 2;
    }
    if (_numInputChannels >= (int)mhe_GetNumHOAComponentsFromOrder(3)) {
        maxHoaOrderSupportedBasedOnInputChannels = 3;
    }
    if (_numInputChannels >= (int)mhe_GetNumHOAComponentsFromOrder(4)) {
        maxHoaOrderSupportedBasedOnInputChannels = 4;
    }
    if (_numInputChannels >= (int)mhe_GetNumHOAComponentsFromOrder(5)) {
        maxHoaOrderSupportedBasedOnInputChannels = 5;
    }
    if (_numInputChannels >= (int)mhe_GetNumHOAComponentsFromOrder(6)) {
        maxHoaOrderSupportedBasedOnInputChannels = 6;
    }
    if (maxHoaOrderSupportedBasedOnInputChannels == 0) {
        // No matching renderer for number of HOA components in the file
        ret = -1;
    }

    // Renderers for HOA order 3, 4, and 6 are supported
    // Input channels beyond the expected number for the specified order will be ignored.
    if (hoaOrderFromArg == -1) {
        //No hoa order input from command line.
        fileData->_numInputChannels = (int)mhe_GetNumHOAComponentsFromOrder(maxHoaOrderSupportedBasedOnInputChannels);
        fileData->_hoaOrder = maxHoaOrderSupportedBasedOnInputChannels;
    } else {
        _hoaOrder = min(maxHoaOrderSupportedBasedOnInputChannels, hoaOrderFromArg);
        _numInputChannels = min(_numInputChannels, (int)mhe_GetNumHOAComponentsFromOrder(_hoaOrder));
        fileData->_numInputChannels = _numInputChannels;
        fileData->_hoaOrder = _hoaOrder;
    }

    return ret;
}

const wav_header_t* MPEGHEncFileUtil_getInputWavHeader(MPEGHEncFileUtilStruct *fileData)
{
    if (&fileData->_inputWavFiles[0] != NULL) {
        return &fileData->_inputWavFiles[0]._wh;
    }
    return NULL;
}

int32_t MPEGHEncFileUtil_readInput(MPEGHEncFileUtilStruct *fileData, FLOAT inPCM[][USAC_FRAME_SIZE], int32_t numToRead)
{
    int32_t n;
    int32_t minN = -1;
    int32_t i=0;

    if (fileData->_inputWavFiles[0]._fp != NULL && fileData->_inputWavFiles[1]._fp != NULL) {
        // if more than one file pointer is != NULL means that the input is more than one mono files
        while (fileData->_inputWavFiles[i]._fp != NULL) {
               memset(inPCM[i], 0, numToRead * sizeof(FLOAT));
               n = InWavFile_read(&inPCM[i], numToRead, &fileData->_inputWavFiles[i]);
               if (minN == -1 || minN > n) {
                   minN = n;
               }
               i++;
        }
    } else {
        // Only one Multi-channel file
        for (i = 0; i < fileData->_numInputChannels; i++) {
            memset(inPCM[i], 0, numToRead * sizeof(FLOAT));
        }
        minN = InWavFile_read(&inPCM[0], numToRead, &fileData->_inputWavFiles[0]);
    }
    return minN;
}


FILE* mpegh_file_write_open(char* outfilename)
{
    FILE* fp = NULL;
    fp = fopen(outfilename,"wb");
    return fp;
}

int32_t mpegh_file_out_write(FILE *fp, uint8_t* buf, uint32_t out_bytes)
{
    fwrite(buf, sizeof(int8_t), out_bytes, fp);
    return 0;
}

int32_t mpegh_util_file_close(FILE* fp)
{
    fclose(fp);
    return 0;
}

int32_t mpegh_file_open_n3d_wav_file(MPEGHEncFileUtilStruct *fileData, int32_t channels, int32_t sample_rate)
{
    int32_t iPeakLimiterModuleDelay = 0;
    int32_t bytesperSample = 4;
    int32_t error = -1;
    FILE *fp_amb = NULL;

    /* File open fails on Android platform */
    fp_amb = fopen("amb_dump_N3D.wav", "wb");
    if(fp_amb == NULL)
        return error;

    wavIO_init(&fileData->hWavIO_N3D, USAC_FRAME_SIZE, 0, 0);
    wavIO_setDelay(fileData->hWavIO_N3D, iPeakLimiterModuleDelay);
    error = wavIO_openWrite(fileData->hWavIO_N3D,
                            fp_amb,
                            channels,
                            sample_rate,
                            bytesperSample);

    return error;
}

int32_t mpegh_file_write_n3d_wav_file(MPEGHEncFileUtilStruct *fileData, float **pp_OutputBuffer)
{
    if (fileData->hWavIO_N3D) {
        uint32_t iwritten = 0;

        wavIO_writeFrame(fileData->hWavIO_N3D,
                            pp_OutputBuffer,
                            USAC_FRAME_SIZE, &iwritten);
    }
    return 0;
}

int32_t mpegh_file_close_n3d_wav_file(MPEGHEncFileUtilStruct *fileData)
{
    if (fileData->hWavIO_N3D) {
        uint64_t nTotalSamplesWrittenPerChannel = 0;

        wavIO_updateWavHeader(fileData->hWavIO_N3D,
                            &nTotalSamplesWrittenPerChannel);

        wavIO_close(fileData->hWavIO_N3D);
    }

    return 0;
}
