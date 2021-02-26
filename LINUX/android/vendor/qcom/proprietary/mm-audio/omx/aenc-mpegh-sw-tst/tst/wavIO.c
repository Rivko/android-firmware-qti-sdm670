/*******************************************************************************
 *  Copyright (c) 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *   CHANGE HISTORY
 *   dd/mm/yy   Code Ver     Description                        Author
 *   --------   -------      -----------                        ------
 ******************************************************************************/
/***********************************************************************************

 This software module was originally developed by

 Fraunhofer IIS

 in the course of development of the ISO/IEC 23008-3 for reference purposes and its
 performance may not have been optimized. This software module is an implementation
 of one or more tools as specified by the ISO/IEC 23008-3 standard. ISO/IEC gives
 you a royalty-free, worldwide, non-exclusive, copyright license to copy, distribute,
 and make derivative works of this software module or modifications  thereof for use
 in implementations or products claiming conformance to the ISO/IEC 23008-3 standard
 and which satisfy any specified conformance criteria. Those intending to use this
 software module in products are advised that its use may infringe existing patents.
 ISO/IEC have no liability for use of this software module or modifications thereof.
 Copyright is not released for products that do not conform to the ISO/IEC 23008-3
 standard.

 Fraunhofer IIS retains full right to modify and use the code for its own purpose,
 assign or donate the code to a third party and to inhibit third parties from using
 the code for products that do not conform to MPEG-related ITU Recommendations and/or
 ISO/IEC International Standards.

 This copyright notice must be included in all copies or derivative works.

 Copyright (c) ISO/IEC 2013.

 ***********************************************************************************/

#include <math.h>
#include <string.h>
#include "wavIO.h"

//#define CLIP_WARNING

#ifdef CLIP_WARNING
#include <stdio.h>
#endif

#include "mpegh_log_utils.h"
#define QC_KW_FIX3
#define QC_MOD_CHECKS

typedef struct _AUDIO_DATA_BUFFER
{
  int8_t  *inBufferChar;
  float *inBufferFloat;
  int8_t  *outBufferChar;
  float *outBufferFloat;

  int32_t readPosition;

} AUDIO_DATA_BUFFER, *H_AUDIO_DATA_BUFFER ;

struct _WAVIO
{
  uint32_t framesize;
  uint32_t fillLastInclompleteFrame;
  int32_t delay;

  FILE* fIn;
  uint32_t nInputChannels;
  uint32_t nInputSamplerate;
  uint32_t nInputBytesperSample;
  uint32_t nInputSamplesPerChannel;

  FILE* fOut;
  uint32_t nOutputChannels;
  uint32_t nOutputSamplerate;
  uint32_t nOutputBytesperSample;
  uint64_t nTotalSamplesWrittenPerChannel;

  AUDIO_DATA_BUFFER* audioBuffers;
};

typedef struct RiffWavHeader
  {
    /* total length of the .wav Header: 44 Byte*/
    /* riff-section*/
    int8_t riff_name[4];    /* 4 Byte, header-signature, contains "RIFF"*/
    uint32_t riff_length;   /* 4 Byte, length of the rest of the header (36 Byte) + <data_length>                                   in Byte*/
    int8_t riff_typ[4];     /* 4 Byte, contains the audio type, here "WAVE"*/

    /* format(fmt)-section*/
    int8_t fmt_name[4];     /* 4 Byte, header-signature, contains "fmt "*/
    uint32_t fmt_length;    /* 4 Byte, length of the rest of the fmt header in Byte*/
    int16_t format_tag;     /* 2 Byte, data format of the audio samples*/
    int16_t channels;       /* 2 Byte, number of used channels*/
    uint32_t fs;            /* 4 Byte, sample frequency in Herz*/
    uint32_t bytes_per_sec; /* 4 Byte, <fs> * <block_align>*/
    int16_t block_align;    /* 2 Byte, <channels> * (<bits/sample> / 8)*/
    int16_t bpsample;       /* 2 Byte, quantizer bit depth, 8, 16 or 24 Bit*/

    /* data-section*/
    int8_t data_name[4];    /* 4 Byte, header-signature, contains "data"*/
    uint32_t data_length;   /* 4 Byte, length of the data chunk in Byte*/
  } RIFFWAVHEADER;


#define WAVIO_RIFF_HEADER_SIZE 36

static int32_t ConvertIntToFloat(float *pfOutBuf, const int8_t *pbInBuf, uint32_t nInBytes, uint32_t nBytesPerSample, uint32_t* bytesLeft);
static void ConvertFloatToInt(int8_t *OutBuf, float *InBuf, uint32_t length, uint32_t nBytesPerSample);
static int16_t LittleToNativeEndianShort(int16_t x);
static int32_t LittleToNativeEndianLong(int32_t x);
static __inline int16_t LittleEndian16 (int16_t v);
static __inline int32_t IsLittleEndian (void);
static uint32_t BigEndian32 (int8_t a, int8_t b, int8_t c, int8_t d);

int32_t wavIO_init(WAVIO_HANDLE *hWavIO, const uint32_t framesize, const uint32_t fillLastIncompleteInputFrame, int32_t delay)
{

  H_AUDIO_DATA_BUFFER hBuffer = NULL;

  WAVIO_HANDLE hWavIOtemp = (WAVIO_HANDLE)calloc(1,sizeof(struct _WAVIO));

  if (hWavIOtemp == NULL)
  return -1;


  hWavIOtemp->framesize = framesize;
  hWavIOtemp->fillLastInclompleteFrame = fillLastIncompleteInputFrame;
  hWavIOtemp->delay = delay;

  hBuffer = (AUDIO_DATA_BUFFER*) calloc(1,sizeof(AUDIO_DATA_BUFFER));
  if (NULL == hBuffer) {
    free(hWavIOtemp);
    return -1;
  }
  hWavIOtemp->audioBuffers = hBuffer;

 *hWavIO = hWavIOtemp;

  return 0;

}

int32_t wavIO_setDelay(WAVIO_HANDLE hWavIO, int32_t delay)
{
  hWavIO->delay = delay;

  return 0;
}

int32_t wavIO_openRead(WAVIO_HANDLE hWavIO, FILE *pInFileName, uint32_t *nInChannels, uint32_t *InSampleRate, uint32_t * InBytedepth, uint64_t *nTotalSamplesPerChannel, int32_t *nSamplesPerChannelFilled)
{
  uint32_t riff = BigEndian32 ('R','I','F','F');
  uint32_t junk = BigEndian32 ('J','U','N','K');
  uint32_t fmt = BigEndian32  ('f','m','t',' ');

  uint32_t i=0;

  uint32_t chunkID;

  RIFFWAVHEADER riffheader = {{ 0 }};
  int32_t fmt_read = 0;

  hWavIO->fIn = pInFileName;
  if (hWavIO->fIn == NULL) {
    return -1;
  }

  while (!fmt_read) {
    if(fread(&chunkID, sizeof(uint32_t),1,hWavIO->fIn)){};
    if (chunkID == riff) {
      memcpy(riffheader.riff_name, (int8_t*) &chunkID,4);
      if(!fread(&riffheader.riff_length, sizeof(uint32_t),1,hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      if(!fread(&riffheader.riff_typ, sizeof(uint32_t), 1, hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
    } else if (chunkID == junk) {
      /* do nothing */
    } else if (chunkID == fmt) {
      memcpy(riffheader.fmt_name, (int8_t*) &chunkID,4);
      if(!fread(&riffheader.fmt_length, sizeof(uint32_t),1,hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      if(!fread(&riffheader.format_tag, 2, 1, hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      if(!fread(&riffheader.channels,2,1,hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      if(!fread(&riffheader.fs,sizeof(uint32_t),1,hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      if(!fread(&riffheader.bytes_per_sec,sizeof(uint32_t),1,hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      if(!fread(&riffheader.block_align,2,1,hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      if(!fread(&riffheader.bpsample,2,1,hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      fmt_read = 1;
    } else {
      uint64_t pos = 0;
      i++;
      if (i > 5000) {
        break;
      }
      pos = ftell(hWavIO->fIn);
      fseek(hWavIO->fIn, pos-3, SEEK_SET);
    }
  }

  /* Search for data chunk */
  {
    /* Search data chunk */
    uint32_t i = 0;
    uint32_t dataTypeRead = 0;
    uint32_t dataType = BigEndian32 ('d','a','t','a') ;
    while(1) {
        i++;
        if( i > 5000 ) {
        /* Error */
          break;
        }

        if(!fread( &dataTypeRead, sizeof(uint32_t), 1, hWavIO->fIn )){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
        if ( dataTypeRead == dataType) {
          /* 'data' chunk found - now read dataSize */
          memcpy(riffheader.data_name, (int8_t*) &dataTypeRead, 4);
          if(!fread(&(riffheader.data_length), sizeof(uint32_t), 1 , hWavIO->fIn)){MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
          break;
        } else {
          /* 3 bytes back */
          uint64_t pos=0;
          pos = ftell(hWavIO->fIn);
          fseek(hWavIO->fIn, pos-3, SEEK_SET);
        }
      }
  }
  hWavIO->nInputBytesperSample = riffheader.bpsample/8;
  hWavIO->nInputChannels = riffheader.channels;
  hWavIO->nInputSamplerate = riffheader.fs;

  *nTotalSamplesPerChannel = riffheader.data_length / hWavIO->nInputChannels / hWavIO->nInputBytesperSample;
  *nInChannels = hWavIO->nInputChannels;
  *InSampleRate = hWavIO->nInputSamplerate;
  *InBytedepth = hWavIO->nInputBytesperSample;

  hWavIO->audioBuffers->inBufferChar = NULL;
  hWavIO->audioBuffers->inBufferFloat = NULL;

  hWavIO->audioBuffers->inBufferChar = (int8_t*)calloc(hWavIO->framesize * hWavIO->nInputChannels * hWavIO->nInputBytesperSample, sizeof(int8_t));
  hWavIO->audioBuffers->inBufferFloat = (float*)calloc(hWavIO->framesize * hWavIO->nInputChannels, sizeof(float));
  hWavIO->audioBuffers->readPosition = 0;


  hWavIO->nInputSamplesPerChannel = *nTotalSamplesPerChannel;
  if (0 == hWavIO->framesize) return -1;
  if (hWavIO->fillLastInclompleteFrame && (*nTotalSamplesPerChannel % hWavIO->framesize)) {
    *nSamplesPerChannelFilled = hWavIO->framesize - (*nTotalSamplesPerChannel % hWavIO->framesize);
  } else {
    *nSamplesPerChannelFilled = 0;
  }

  return 0;
}

int32_t wavIO_openWrite(WAVIO_HANDLE hWavIO, FILE *pOutFileName, uint32_t nOutChannels, uint32_t OutSampleRate, uint32_t bytesPerSample)
{
  RIFFWAVHEADER header = {{ 0 }};
  hWavIO->fOut = pOutFileName;
  if (hWavIO->fOut == NULL) {
      return -1;
  }

  header.riff_name[0]='R';
  header.riff_name[1]='I';
  header.riff_name[2]='F';
  header.riff_name[3]='F';


  header.riff_length=LittleToNativeEndianLong(0);

  header.riff_typ[0]='W';
  header.riff_typ[1]='A';
  header.riff_typ[2]='V';
  header.riff_typ[3]='E';


  header.fmt_name[0]='f';
  header.fmt_name[1]='m';
  header.fmt_name[2]='t';
  header.fmt_name[3]=' ';


  header.fmt_length = LittleToNativeEndianLong(16);

  if ( bytesPerSample == 4 ) {
    header.format_tag    = LittleToNativeEndianShort(3);
  } else {
    header.format_tag    = LittleToNativeEndianShort(1);
  }

  header.channels      = LittleToNativeEndianShort(nOutChannels);
  header.fs            = LittleToNativeEndianLong(OutSampleRate);
  header.bpsample      = LittleToNativeEndianShort(bytesPerSample*8);
  header.bytes_per_sec = LittleToNativeEndianLong(nOutChannels * OutSampleRate * bytesPerSample);
  header.block_align   = LittleToNativeEndianShort(nOutChannels * bytesPerSample);


  header.data_name[0]='d';
  header.data_name[1]='a';
  header.data_name[2]='t';
  header.data_name[3]='a';

  header.data_length=LittleToNativeEndianLong(0);

  /* write header to OutFile */
  fwrite(&header, sizeof(header), 1, hWavIO->fOut);

  hWavIO->nOutputChannels = nOutChannels;
  hWavIO->nOutputSamplerate = OutSampleRate;
  hWavIO->nOutputBytesperSample = bytesPerSample;

  hWavIO->audioBuffers->outBufferChar = NULL;
  hWavIO->audioBuffers->outBufferFloat = NULL;
  hWavIO->audioBuffers->outBufferChar = (int8_t*)calloc(hWavIO->framesize * hWavIO->nOutputChannels * hWavIO->nOutputBytesperSample, sizeof(int8_t));
  hWavIO->audioBuffers->outBufferFloat = (float*)calloc(hWavIO->framesize * hWavIO->nOutputChannels, sizeof(float));

  return 0;
}

int32_t wavIO_readFrame(WAVIO_HANDLE hWavIO, float **inBuffer, uint32_t *nSamplesReadPerChannel, uint32_t *isLastFrame, uint32_t * nZerosPaddedBeginning,  uint32_t * nZerosPaddedEnd)
{
  int32_t i = 0;
  uint32_t left = 0 ;
  uint32_t *h_left = &left;
  float tempfloat = 0;
  uint32_t j;
  int32_t ct_pos = -1;
  int32_t error = 0;
  int32_t samplesToRead = 0;
  int32_t zerosToRead = 0;

  int32_t delaySamples = hWavIO->delay;

  if (delaySamples > 0) {
    zerosToRead = min( (uint32_t)delaySamples, hWavIO->framesize );
    samplesToRead = hWavIO->framesize - zerosToRead;
    for (i=0;i<zerosToRead * (int32_t)hWavIO->nInputChannels;i= i+hWavIO->nInputChannels) {
      for (j=0;j<hWavIO->nInputChannels;j++) {
        hWavIO->audioBuffers->inBufferFloat[i+j] = 0.0f;
      }
    }
    if (hWavIO->nInputBytesperSample > 1) {
      if(!fread(hWavIO->audioBuffers->inBufferChar, hWavIO->nInputBytesperSample, samplesToRead * hWavIO->nInputChannels, hWavIO->fIn)) {
      MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      error = ConvertIntToFloat(hWavIO->audioBuffers->inBufferFloat, hWavIO->audioBuffers->inBufferChar, samplesToRead * hWavIO->nInputChannels * hWavIO->nInputBytesperSample * sizeof(int8_t), hWavIO->nInputBytesperSample, h_left);
      error +=0;
    } else {
      if(!fread(hWavIO->audioBuffers->inBufferFloat,hWavIO->nInputBytesperSample, samplesToRead * hWavIO->nInputChannels, hWavIO->fIn)) {
      MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
    }
    delaySamples -= zerosToRead;
    hWavIO->delay = delaySamples;
    *nSamplesReadPerChannel = samplesToRead;
    for (j=0;j<(int32_t)(hWavIO->nInputChannels);j++) {
      ct_pos = -1;
      for (i = 0;i <  (int32_t)(samplesToRead * hWavIO->nInputChannels); i = i+hWavIO->nInputChannels) {
        ct_pos++;
        tempfloat = hWavIO->audioBuffers->inBufferFloat[i+j];
        inBuffer[j][ct_pos + zerosToRead] = tempfloat;
      }
    }

  } else {
    int32_t leftSamples = hWavIO->nInputSamplesPerChannel - (hWavIO->audioBuffers->readPosition + hWavIO->framesize);
    if (leftSamples  > 0) {     /* more than one frame left */
        samplesToRead = hWavIO->framesize;
        *isLastFrame = 0;
    } else if (leftSamples == 0) { /* exactly one frame left */
        samplesToRead = hWavIO->framesize;
        *isLastFrame = 1;
    } else {                      /* less than one frame left */
        samplesToRead = hWavIO->nInputSamplesPerChannel - hWavIO->audioBuffers->readPosition;
        *isLastFrame = 1;
    }

    if (hWavIO->nInputBytesperSample > 1) {
      if(!fread(hWavIO->audioBuffers->inBufferChar, hWavIO->nInputBytesperSample, samplesToRead * hWavIO->nInputChannels, hWavIO->fIn)) {
      MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
      error = ConvertIntToFloat(hWavIO->audioBuffers->inBufferFloat, hWavIO->audioBuffers->inBufferChar, samplesToRead * hWavIO->nInputChannels * hWavIO->nInputBytesperSample * sizeof(int8_t), hWavIO->nInputBytesperSample, h_left);
      error +=0;
    } else {
      if(!fread(hWavIO->audioBuffers->inBufferFloat,hWavIO->nInputBytesperSample, samplesToRead * hWavIO->nInputChannels, hWavIO->fIn)) {
      MPEGH_LOG_ERROR("FREAD FAILED IN WAVIO.C FILE \n");}
    }
    *nSamplesReadPerChannel = samplesToRead;

    if (*isLastFrame) {

      /* Fill up frame with zeros if wanted */
      if (hWavIO->fillLastInclompleteFrame) {
          int32_t i = 0;

          /* Calculate number of samples to add  */
          int32_t nSamplesToAdd = hWavIO->framesize - samplesToRead;
          *nZerosPaddedEnd = nSamplesToAdd;

          for ( i = 0; i <  (int32_t)(hWavIO->nInputChannels * nSamplesToAdd); ++i ) {
            hWavIO->audioBuffers->inBufferFloat[i + hWavIO->nInputChannels * samplesToRead] = 0.0f;
          }
        }
    }

    for (j=0;j<(int32_t)(hWavIO->nInputChannels);j++) {
         ct_pos = -1;
         for (i = 0;i <  (int32_t)(samplesToRead * hWavIO->nInputChannels); i = i+hWavIO->nInputChannels) {
        ct_pos++;
        tempfloat = hWavIO->audioBuffers->inBufferFloat[i+j];
        inBuffer[j][ct_pos] = tempfloat;
      }
    }
  }

  hWavIO->audioBuffers->readPosition +=  samplesToRead;
  *nZerosPaddedBeginning = zerosToRead;

  return 0;
}

int32_t wavIO_writeFrame_withOffset(WAVIO_HANDLE hWavIO, float **outBuffer, uint32_t nSamplesToWritePerChannel, uint32_t *nSamplesWrittenPerChannel, uint32_t offset)
{
  uint32_t i= 0;
  int32_t ct_pos = -1;
  uint32_t j;
  float tempfloat;
  int32_t delaySamples = hWavIO->delay;

  if (delaySamples < 0) {
    uint32_t i= 0;
    int32_t samplesToWrite = nSamplesToWritePerChannel - (-1)*delaySamples;
    int32_t samplesToSkip = 0;
    ct_pos = -1;
    ct_pos = ct_pos + samplesToSkip;
    if (samplesToWrite < 0) {
      samplesToWrite = 0;
    }
    samplesToSkip = nSamplesToWritePerChannel - samplesToWrite;

    ct_pos = samplesToSkip-1;
    for (i=0; i < samplesToWrite*hWavIO->nOutputChannels; i=i+hWavIO->nOutputChannels) {
      ct_pos++;
      for (j=0;j<hWavIO->nOutputChannels;j++) {
        tempfloat = outBuffer[j][ct_pos+offset];
        hWavIO->audioBuffers->outBufferFloat[i+j] = tempfloat;
      }
    }

    ConvertFloatToInt(hWavIO->audioBuffers->outBufferChar, hWavIO->audioBuffers->outBufferFloat, samplesToWrite * hWavIO->nOutputChannels * hWavIO->nOutputBytesperSample, hWavIO->nOutputBytesperSample);
    fwrite(hWavIO->audioBuffers->outBufferChar, hWavIO->nOutputBytesperSample, samplesToWrite * hWavIO->nOutputChannels,hWavIO->fOut);

    if (samplesToWrite == 0) {
      delaySamples += nSamplesToWritePerChannel;
    } else {
      delaySamples = 0;
    }

    hWavIO->delay = delaySamples;
    *nSamplesWrittenPerChannel = samplesToWrite;
  } else {
    /* Interleave channels */
    for (i=0; i < nSamplesToWritePerChannel * hWavIO->nOutputChannels; i=i+hWavIO->nOutputChannels) {
      ct_pos++;
      for (j=0;j<hWavIO->nOutputChannels;j++) {
        tempfloat = outBuffer[j][ct_pos+offset];
        hWavIO->audioBuffers->outBufferFloat[i+j] = tempfloat;
      }
    }

    ConvertFloatToInt(hWavIO->audioBuffers->outBufferChar, hWavIO->audioBuffers->outBufferFloat, nSamplesToWritePerChannel * hWavIO->nOutputChannels * hWavIO->nOutputBytesperSample, hWavIO->nOutputBytesperSample);
    fwrite(hWavIO->audioBuffers->outBufferChar, hWavIO->nOutputBytesperSample, nSamplesToWritePerChannel * hWavIO->nOutputChannels,hWavIO->fOut);

    *nSamplesWrittenPerChannel = nSamplesToWritePerChannel;
  }

  hWavIO->nTotalSamplesWrittenPerChannel += *nSamplesWrittenPerChannel;
  return 0;
}

int32_t wavIO_writeFrame(WAVIO_HANDLE hWavIO, float **outBuffer, uint32_t nSamplesToWritePerChannel, uint32_t *nSamplesWrittenPerChannel)
{
  uint32_t i= 0;
  int32_t ct_pos = -1;
  uint32_t j;
  float tempfloat;
  int32_t delaySamples = hWavIO->delay;

  if (delaySamples < 0) {
    uint32_t i= 0;
    int32_t samplesToWrite = nSamplesToWritePerChannel - (-1)*delaySamples;
    int32_t samplesToSkip = 0;
    ct_pos = -1;
    ct_pos = ct_pos + samplesToSkip;
    if (samplesToWrite < 0) {
      samplesToWrite = 0;
    }
    samplesToSkip = nSamplesToWritePerChannel - samplesToWrite;

    ct_pos = samplesToSkip-1;
    for (i=0; i < samplesToWrite*hWavIO->nOutputChannels; i=i+hWavIO->nOutputChannels) {
      ct_pos++;
      for (j=0;j<hWavIO->nOutputChannels;j++) {
        tempfloat = outBuffer[j][ct_pos];
        hWavIO->audioBuffers->outBufferFloat[i+j] = tempfloat;
      }
    }

    ConvertFloatToInt(hWavIO->audioBuffers->outBufferChar, hWavIO->audioBuffers->outBufferFloat, samplesToWrite * hWavIO->nOutputChannels * hWavIO->nOutputBytesperSample, hWavIO->nOutputBytesperSample);
    fwrite(hWavIO->audioBuffers->outBufferChar, hWavIO->nOutputBytesperSample, samplesToWrite * hWavIO->nOutputChannels,hWavIO->fOut);

    if (samplesToWrite == 0) {
      delaySamples += nSamplesToWritePerChannel;
    } else {
      delaySamples = 0;
    }

    hWavIO->delay = delaySamples;
    *nSamplesWrittenPerChannel = samplesToWrite;
  } else {
    /* Interleave channels */
    for (i=0; i < nSamplesToWritePerChannel * hWavIO->nOutputChannels; i=i+hWavIO->nOutputChannels) {
      ct_pos++;
      for (j=0;j<hWavIO->nOutputChannels;j++) {
        tempfloat = outBuffer[j][ct_pos];
        hWavIO->audioBuffers->outBufferFloat[i+j] = tempfloat;
      }
    }

    ConvertFloatToInt(hWavIO->audioBuffers->outBufferChar, hWavIO->audioBuffers->outBufferFloat, nSamplesToWritePerChannel * hWavIO->nOutputChannels * hWavIO->nOutputBytesperSample, hWavIO->nOutputBytesperSample);
    fwrite(hWavIO->audioBuffers->outBufferChar, hWavIO->nOutputBytesperSample, nSamplesToWritePerChannel * hWavIO->nOutputChannels,hWavIO->fOut);

    *nSamplesWrittenPerChannel = nSamplesToWritePerChannel;
  }

  hWavIO->nTotalSamplesWrittenPerChannel += *nSamplesWrittenPerChannel;
  return 0;
}

int32_t wavIO_updateWavHeader( WAVIO_HANDLE hWavIO, uint64_t * nTotalSamplesWrittenPerChannel )
{
  uint64_t bytesWritten = 0;
  uint32_t tmp;

  bytesWritten = hWavIO->nTotalSamplesWrittenPerChannel * hWavIO->nOutputChannels * hWavIO->nOutputBytesperSample;

  bytesWritten+=WAVIO_RIFF_HEADER_SIZE;

  fseek(hWavIO->fOut,4,SEEK_SET);
  tmp = LittleToNativeEndianLong(bytesWritten);
  fwrite(&tmp, sizeof(int32_t), 1, hWavIO->fOut);

  bytesWritten-=WAVIO_RIFF_HEADER_SIZE;

  fseek(hWavIO->fOut,40,SEEK_SET);
  tmp = LittleToNativeEndianLong(bytesWritten);
  fwrite(&tmp, sizeof(int32_t), 1, hWavIO->fOut);

  *nTotalSamplesWrittenPerChannel = hWavIO->nTotalSamplesWrittenPerChannel;
  return 0;
}


int32_t wavIO_close(WAVIO_HANDLE hWavIO)
{
  if (hWavIO->fIn) {
    fclose(hWavIO->fIn);
    free(hWavIO->audioBuffers->inBufferChar);
    free(hWavIO->audioBuffers->inBufferFloat);
  }

  if (hWavIO->fOut) {
    fclose(hWavIO->fOut);
    free(hWavIO->audioBuffers->outBufferChar);
    free(hWavIO->audioBuffers->outBufferFloat);
  }

  free(hWavIO->audioBuffers);

  free(hWavIO);

  return 0;
}



static int32_t ConvertIntToFloat(float *pfOutBuf, const int8_t *pbInBuf, uint32_t nInBytes, uint32_t nBytesPerSample, uint32_t* bytesLeft)
{
  uint32_t i,j, nSamples, nOffset;


  if ( nBytesPerSample == 4 ) {
    memcpy(pfOutBuf, pbInBuf, nInBytes*sizeof(int8_t));
    return 0;
  }

  if (nBytesPerSample == sizeof(int16_t)) {
      const int16_t* shortBuf = (const int16_t*) pbInBuf;
      for(j = 0; j < nInBytes/nBytesPerSample; j++) {
          pfOutBuf[j] = ((float) LittleEndian16(shortBuf[j]))/32768.f;
      }
  } else if ( nBytesPerSample > 2 ) {
      union { int64_t s; int8_t c[sizeof(int64_t)]; } u;
      float fFactor =  (float)(1 << ( nBytesPerSample*8 - 1 ));
      fFactor = 1.0f / fFactor;

      u.s = 0;

      nOffset = (sizeof(int64_t) - nBytesPerSample) * 8;

      nSamples = nInBytes / nBytesPerSample;

      *bytesLeft = nInBytes % nBytesPerSample;

      for(j = 0; j < nSamples; j++) {
          int32_t n = j * nBytesPerSample;

          /* convert chars to 32 bit */
          if (IsLittleEndian()) {
              for (i = 0; i < nBytesPerSample; i++) {
                  u.c[sizeof(int64_t) - 1 - i] = pbInBuf[n + nBytesPerSample - i - 1];
              }
          } else {
              for (i = 0; i < nBytesPerSample; i++) {
                  u.c[i] = pbInBuf[n + nBytesPerSample - i - 1];
              }
          }

          u.s = u.s >> nOffset;

          pfOutBuf[j] = ((float) u.s) * fFactor;

        }
    }
  return 0;
}



static void ConvertFloatToInt(int8_t *OutBuf, float *InBuf, uint32_t length, uint32_t nBytesPerSample)
{
  uint32_t j;

  if ( nBytesPerSample == 4 ) {
    memcpy(OutBuf, InBuf, length*sizeof(int8_t));
  } else if (nBytesPerSample == sizeof(int16_t)) {
    union { int16_t s; int8_t c[sizeof(int16_t)+1]; } u;
    int32_t i;
    float fFactor   = (float)(1 << ( nBytesPerSample*8 - 1 ));
    u.s         = 0;
    for (j=0; j < length / nBytesPerSample; j++) {
         float maxVal  =  32767.f;
         float minVal  = -32768.f;
         float tmpVal  = 0.f;
         int32_t n = j * nBytesPerSample;

         tmpVal = InBuf[j] * fFactor;

         if (tmpVal > maxVal) {
             tmpVal = maxVal;
#ifdef CLIP_WARNING
             MPEGH_LOG_ERROR("wavIO warning: sample > maxVal clipped\n");
#endif
         }
         if ( tmpVal < minVal ) {
             tmpVal = minVal;
#ifdef CLIP_WARNING
             MPEGH_LOG_ERROR("wavIO warning: sample < minVal clipped\n");
#endif
         }
         u.s = (int16_t) tmpVal;

         if (IsLittleEndian()) {
             for (i=0; i< (int32_t)nBytesPerSample; i++) {
                  OutBuf[n + i] = u.c[i];
             }
         } else {
             for (i = 0; i < (int32_t)nBytesPerSample; i++) {
                  OutBuf[n + nBytesPerSample - i - 1] = u.c[i + (sizeof(int16_t) - (sizeof(int16_t)-1))];
             }
         }
    }
  } else {
      union { int64_t s; int8_t c[sizeof(int64_t)]; } u;
      int32_t i;

      /* Calculate scaling factor for 24bit */
      float fFactor   = (float)(1 << ( nBytesPerSample*8 - 1 ));
      u.s         = 0;
      for (j=0; j < length / nBytesPerSample; j++) {
           int32_t maxVal = (int32_t) fFactor - 1;
           int32_t minVal = (int32_t) -fFactor;
           int32_t n = j * nBytesPerSample;

           u.s = (int64_t) (InBuf[j] * fFactor);

           if (u.s > maxVal) {
               u.s = maxVal;
#ifdef CLIP_WARNING
               MPEGH_LOG_ERROR("wavIO warning: sample > maxVal clipped\n");
#endif
           }
           if (u.s < minVal) {
               u.s = minVal;
#ifdef CLIP_WARNING
               MPEGH_LOG_ERROR("wavIO warning: sample < minVal clipped\n");
#endif
           }
           if (IsLittleEndian()) {
               for (i=0;i< (int32_t)nBytesPerSample; i++) {
                    OutBuf[n + i] = u.c[i];
               }
           } else {
               for (i = 0; i < (int32_t)nBytesPerSample; i++) {
                    OutBuf[n + nBytesPerSample - i - 1] = u.c[i + (sizeof(int64_t) - 3)];
               }
           }
      }
   }
}

static __inline int32_t IsLittleEndian (void)
{
  int16_t s = 0x01 ;

  return *((int8_t *) &s) ? 1 : 0;
}


static __inline int16_t LittleEndian16 (int16_t v)
{ /* get signed little endian (2-compl.) and returns in native format, signed */
  if (IsLittleEndian ()) return v ;

  else return ((v << 8) & 0xFF00) | ((v >> 8) & 0x00FF) ;
}

static uint32_t BigEndian32 (int8_t a, int8_t b, int8_t c, int8_t d)
{
  if (IsLittleEndian ()) {
    return (uint32_t) d << 24 |
      (uint32_t) c << 16 |
      (uint32_t) b <<  8 |
      (uint32_t) a ;
  } else {
      return (uint32_t) a << 24 |
      (uint32_t) b << 16 |
      (uint32_t) c <<  8 |
      (uint32_t) d ;
  }
}

#if defined __BIG_ENDIAN__
static int16_t LittleToNativeEndianShort(int16_t x)
{
  int8_t *t = (int8_t*)(&x);
  int8_t tmp = t[0];
  t[0] = t[1];
  t[1] = tmp;
  return *((int16_t*)t);
}

static int32_t LittleToNativeEndianLong(int32_t x)
{
  int8_t *t = (int8_t*)(&x);
  int8_t tmp = t[0];
  t[0] = t[3];
  t[3] = tmp;
  tmp  = t[1];
  t[1] = t[2];
  t[2] = tmp;
  return *((int32_t*)t);
}

#elif defined (_M_IX86) || defined (__i386) || defined (__amd64) || defined (__x86_64__) || defined (WIN64) || defined (_WIN64 ) || (defined(__GNUC__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
static int16_t LittleToNativeEndianShort(int16_t x) { return x; }
static int32_t LittleToNativeEndianLong(int32_t x) { return x; }

#else
#error "Not sure if we are big or little endian"
#endif
