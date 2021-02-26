/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/**
 * Exposes a middle JNI layer between OC UI application and
 * SCVE STA API
 */
#include <jni.h>
#include <string.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <sys/time.h>
#include "scve/scveTextReco.hpp"
#include "scve/scveImageCorrection.hpp"

#define PACKAGE "com/qualcomm/qti/sta/engine/"
#define CLASS PACKAGE "LibraryHandler"
#define MAX_REGIONS 300
#define MAX_CANDIDATES 1

using namespace SCVE;

#define LOG_TAG "OCRNative"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define ILLEGAL_ARGUMENT_EXCEPTION  "java/lang/IllegalArgumentException"
#define RUNTIME_EXCEPTION           "java/lang/RuntimeException"

#define TEXT_RECORD_MODE_SNAPSHOT   0
#define TEXT_RECORD_MODE_PREVIEW    1

#define IC_SCALE_MODE_HIGH_RESOLUTION   0
#define IC_SCALE_MODE_LOW_RESOLUTION    1

// REGION Extraction Modes
#define IC_REGION_EXTRACTION_QUADRILATERAL      1
#define IC_REGION_EXTRACTION_CURVED_BOUNDARY    2
#define IC_REGION_EXTRACTION_CURVED_TEXTLINE    3
#define IC_REGION_EXTRACTION_AUTO               4

// Image enhancement
#define IC_IMAGE_ENHANCEMENT_WHITEBOARD     1
#define IC_IMAGE_ENHANCEMENT_PROJECTOR      2
#define IC_IMAGE_ENHANCEMENT_DOCUMENT       3
#define IC_IMAGE_ENHANCEMENT_BUSINESSCARD   4
#define IC_IMAGE_ENHANCEMENT_SCREEN         5
#define IC_IMAGE_ENHANCEMENT_AUTO           6

#define STA_RESULT_FAILED           -1
#define STA_RESULT_IN_PROGRESS       1
#define STA_RESULT_EMPTY             2
#define STA_RESULT_TIMEOUT           3

#define STA_DATA_TYPE_NONE           0x00
#define STA_DATA_TYPE_EMAIL          0x01
#define STA_DATA_TYPE_URL            0x02
#define STA_DATA_TYPE_PHONENUMBER    0x04
#define STA_DATA_TYPE_ADDRESS        0x08
#define STA_DATA_TYPE_DATETIME       0x10
#define STA_DATA_TYPE_GENERAL        0x20

#define STA_LIBRARY_VERSION 5

typedef struct {
    Context* context;
    TextReco* textRecordHandler;
    scveTextRecoResult *textRecordResults;

    ImageCorrection* icHandler;
    Image enhancedImages[MAX_CANDIDATES];
    Region boundary_pos[MAX_CANDIDATES];
} NativeContext;

static jfieldID field_STAWrapper_nativeContext_J;

///////////////////////////////////////// STA ////////////////////////////////////////////
static int toScveDataPatterns(int ocrDataPatterns) {
    int scveFlags = 0;

    if((ocrDataPatterns&STA_DATA_TYPE_EMAIL) == STA_DATA_TYPE_EMAIL) {
        scveFlags |= TEXT_RECO_CATEGORY_EMAIL;
    }
    if((ocrDataPatterns&STA_DATA_TYPE_URL) == STA_DATA_TYPE_URL) {
        scveFlags |= TEXT_RECO_CATEGORY_URL;
    }
    if((ocrDataPatterns&STA_DATA_TYPE_PHONENUMBER) == STA_DATA_TYPE_PHONENUMBER) {
        scveFlags |= TEXT_RECO_CATEGORY_PHONENUMBER;
    }
    if((ocrDataPatterns&STA_DATA_TYPE_ADDRESS) == STA_DATA_TYPE_ADDRESS) {
        scveFlags |= TEXT_RECO_CATEGORY_ADDRESS;
    }
    if((ocrDataPatterns&STA_DATA_TYPE_DATETIME) == STA_DATA_TYPE_DATETIME) {
        scveFlags |= TEXT_RECO_CATEGORY_DATETIME;
    }
    if((ocrDataPatterns&STA_DATA_TYPE_GENERAL) == STA_DATA_TYPE_GENERAL) {
        scveFlags |= TEXT_RECO_CATEGORY_GENERAL;
    }

    return scveFlags;
}
static int toSTAAppDataType(int scveDataType) {
    switch(scveDataType) {
    case TEXT_RECO_CATEGORY_EMAIL:
        return STA_DATA_TYPE_EMAIL;
    case TEXT_RECO_CATEGORY_URL:
        return STA_DATA_TYPE_URL;
    case TEXT_RECO_CATEGORY_PHONENUMBER:
        return STA_DATA_TYPE_PHONENUMBER;
    case TEXT_RECO_CATEGORY_ADDRESS:
        return STA_DATA_TYPE_ADDRESS;
    case TEXT_RECO_CATEGORY_DATETIME:
        return STA_DATA_TYPE_DATETIME;
    case TEXT_RECO_CATEGORY_GENERAL:
        return STA_DATA_TYPE_GENERAL;
    }
    return STA_DATA_TYPE_NONE;
}
static void cleanupTextRecordHandler(TextReco* textRecordHandler) {
    if (textRecordHandler) {
        textRecordHandler->DeInit();
        TextReco::deleteInstance(textRecordHandler);
        textRecordHandler = NULL;
    }
}

static void cleanupNativeContext(NativeContext *nativeContext) {
    if (nativeContext->textRecordResults) {
        delete[] nativeContext->textRecordResults;
        nativeContext->textRecordResults = NULL;
    }
    cleanupTextRecordHandler(nativeContext->textRecordHandler);
        nativeContext->textRecordHandler = NULL;

    if (nativeContext->context) {
        Context::deleteInstance(nativeContext->context);
        nativeContext->context = NULL;
    }
    LOGD("STA Wrapper cleanup");
}

static void throwException(JNIEnv *_env, const char *exceptionClassName, const char *msg) {
    // Don't squelch an existing exception
    if (_env->ExceptionOccurred() == NULL) {
        jclass exceptionClass = _env->FindClass(exceptionClassName);
        if (exceptionClass != NULL) {
            _env->ThrowNew(exceptionClass, msg);
        }
    }
}
static int configureTextRecordHandler(JNIEnv *jenv, TextReco* textRecordHandler, jstring locale,
        int dataPatterns) {
    const char *nativeString = jenv->GetStringUTFChars(locale, 0);
    int scveDataPatterns = toScveDataPatterns(dataPatterns);
    Status status = textRecordHandler->Init(scveDataPatterns, nativeString, MAX_REGIONS);
    jenv->ReleaseStringUTFChars(locale, nativeString);
    return status;
}
static int startTextRecordHandler(JNIEnv *jenv, NativeContext *nativeContext, int mode,
        jstring locale, int dataPatterns) {
    if(mode == TEXT_RECORD_MODE_SNAPSHOT) {
        LOGD("initializing Snapshot mode");
        nativeContext->textRecordHandler = TextReco::newInstance(nativeContext->context,
                    IMAGE_MODE_SINGLE_FRAME, NULL);
    } else if(mode == TEXT_RECORD_MODE_PREVIEW) {
        LOGD("initializing Preview mode");
        nativeContext->textRecordHandler = TextReco::newInstance(nativeContext->context,
                    IMAGE_MODE_CONTINUOUS_FRAME, NULL);
    } else {
        throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "Unknown mode");
                return STA_RESULT_FAILED;
    }

    return configureTextRecordHandler(jenv, nativeContext->textRecordHandler, locale, dataPatterns);
}
static Status generateTextnessMap(JNIEnv *jenv, NativeContext *nativeContext, uint8_t* rgbBytes,
        int imageWidth, int imageHeight) {
    ImageDimension color_dim;
    color_dim.nHeight = imageHeight;
    color_dim.nWidth = imageWidth;
    color_dim.nStride = imageWidth * 3;
    color_dim.nStrides[0] = imageWidth * 3;
    color_dim.nStrides[1] = 0;
    color_dim.nStrides[2] = 0;

    Image image;
    image.nChannels = 3;
    image.pPtr = rgbBytes;
    image.sDim = color_dim;
    image.sFormat = SCVE::SCVE_COLORFORMAT_RGB_888;

    Rect roi;
    roi.nLocX = 0;
    roi.nLocY = 0;
    roi.nWidth = imageWidth;
    roi.nHeight = imageHeight;

    Image textnessImage;

    return nativeContext->textRecordHandler->GetTextnessMap(&image, roi, &textnessImage);
}
static Status parseGreyscale(JNIEnv *jenv, NativeContext *nativeContext, uint8_t* greyScaleBytes,
        int imageWidth, int imageHeight, jintArray values) {
    Image image;
    image.sDim.nWidth = imageWidth;
    image.sDim.nHeight = imageHeight;
    image.sDim.nStride = imageWidth;
    image.sDim.nStrides[0] = imageWidth;
    image.sDim.nStrides[1] = 0;
    image.sDim.nStrides[2] = 0;

    image.nChannels = 1;
    image.pPtr = greyScaleBytes;
    image.sFormat = (ColorFormat) SCVE_COLORFORMAT_GREY_8BIT;

    Rect roi;
    roi.nLocX = 0;
    roi.nLocY = 0;
    roi.nWidth = imageWidth;
    roi.nHeight = imageHeight;

    uint8_t sharpness = 0;
    uint16_t numRegions = 0;

    LOGD("ROI Image wd:%d, ht:%d", imageWidth, imageHeight);
    struct timeval beforeTimeValue;
    gettimeofday(&beforeTimeValue, NULL);
    LOGD("Parsing Image at %ld%03ld ms", beforeTimeValue.tv_sec, (beforeTimeValue.tv_usec / 1000));

    Status status = nativeContext->textRecordHandler->Run_Sync(&image, roi, &sharpness, &numRegions,
            nativeContext->textRecordResults);

    LOGD("Parsing status: %d", status);
    struct timeval afterTimeValue;
    gettimeofday(&afterTimeValue, NULL);
    LOGD("Parsing done at %ld%03ld ms", afterTimeValue.tv_sec, (afterTimeValue.tv_usec / 1000));
    long timeDuration = (afterTimeValue.tv_sec - beforeTimeValue.tv_sec) * 1000
            + (afterTimeValue.tv_usec - beforeTimeValue.tv_usec) / 1000;
    LOGD("Parsing time Duration: %ld", timeDuration);

    if (status == SCVE_SUCCESS && values != NULL) {
        jint *oarr = jenv->GetIntArrayElements(values, NULL);
        oarr[0] = (int32_t) numRegions;
        oarr[1] = (int32_t) sharpness;
        jenv->ReleaseIntArrayElements(values, oarr, 0);
    }
    return status;
}
static void rotateAndCrop(jbyte* inBytes, jbyte* outBytes, int width, int height, int angle,
        int roiX, int roiY, int roiWidth, int roiHeight) {
    LOGD("Rotation start: %d", angle);
    switch (angle) {
        case 0:
            for (int i = roiY; i < (roiY + roiHeight); i++) {
                for (int j = roiX; j < (roiX + roiWidth); j++) {
                    int lhsIndex = (j - roiX) + ((i - roiY) * roiWidth);
                    int rhsIndex = j + i * width;
                    outBytes[lhsIndex] = inBytes[rhsIndex];
                }
            }
            break;
        case 90:
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int xPosition = (height - i - 1);
                    int yPosition = j;
                if (xPosition >= roiX && xPosition < (roiX + roiWidth) && yPosition >= roiY
                        && yPosition < (roiY + roiHeight)) {
                        int lhsIndex = (xPosition - roiX) + ((yPosition - roiY) * roiWidth);
                        outBytes[lhsIndex] = inBytes[j + i * width];
                    }
                }
            }
            break;
        case 270:
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int xPosition = i;
                int yPosition = (width - j - 1);
                if (xPosition >= roiX && xPosition < (roiX + roiWidth) && yPosition >= roiY
                        && yPosition < (roiY + roiHeight)) {
                    int lhsIndex = (xPosition - roiX) + ((yPosition - roiY) * roiWidth);
                    outBytes[lhsIndex] = inBytes[j + i * width];
                }
            }
        }
        break;
        case 180:
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int xPosition = (width - j - 1);
                int yPosition = (height - i - 1);
                if (xPosition >= roiX && xPosition < (roiX + roiWidth) && yPosition >= roiY
                        && yPosition < (roiY + roiHeight)) {
                    int lhsIndex = (xPosition - roiX) + ((yPosition - roiY) * roiWidth);
                    outBytes[lhsIndex] = inBytes[j + (i * width)];
                }
            }
        }
        break;
    }
    LOGD("Rotation ends");
}
/*
 * Method: nClassInit
 * Signature: ()V
 */
static void JNICALL nClassInit(JNIEnv *jenv, jclass cls) {
    field_STAWrapper_nativeContext_J = jenv->GetFieldID(cls, "nativeContext", "J");
    if (jenv->ExceptionCheck()) {
        return;
    }
}
/*
 * Method:    nSTAInit
 * Signature: (IILjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL nSTAInit(JNIEnv* jenv, jobject obj, jint ver, jint textRecordMode,
        jstring locale, int dataPatterns) {
     LOGD("Library version %d", STA_LIBRARY_VERSION);
    if (jenv->GetLongField(obj, field_STAWrapper_nativeContext_J) != 0L) {
        throwException(jenv, RUNTIME_EXCEPTION, "Already initialized");
        return STA_RESULT_FAILED;
    }

    NativeContext *nativeContext = new NativeContext();
    memset(nativeContext, 0, sizeof(NativeContext));
    if (ver == 0) // REF
        nativeContext->context = Context::newInstance(SCVE_MODE_DEFAULT);
    else if (ver == 1)
        nativeContext->context = Context::newInstance(SCVE_MODE_CPU_OFFLOAD);
    else if (ver == 2) // VENUM
        nativeContext->context = Context::newInstance(SCVE_MODE_PERFORMANCE);
    else {
        throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "Unknown ver");
        return STA_RESULT_FAILED;
    }

    Status status = startTextRecordHandler(jenv, nativeContext, textRecordMode, locale,
            dataPatterns);
    if (status != SCVE_SUCCESS) {
        cleanupNativeContext(nativeContext);
        return status;
    }
    nativeContext->textRecordResults = new scveTextRecoResult[MAX_REGIONS];
    jenv->SetLongField(obj, field_STAWrapper_nativeContext_J, (jlong) nativeContext);

    return status;
}
/*
 * Method:    nSTAReconfigureTextHandler
 * Signature: (Ljava/lang/String;I)I
 */
 JNIEXPORT jint JNICALL nSTAReconfigureTextHandler(JNIEnv* jenv, jobject obj, jstring locale,
        int dataPatterns) {
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
            field_STAWrapper_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(jenv, RUNTIME_EXCEPTION, "Not initialized");
        return 0;
    }

    Status status = nativeContext->textRecordHandler->DeInit();
    if (status == SCVE_SUCCESS) {
        status = configureTextRecordHandler(jenv, nativeContext->textRecordHandler, locale,
                dataPatterns);
    }
    return status;
}
/*
  * Method:    nSTARestartTextHandler
  * Signature: (ILjava/lang/String;I)I
 */
 JNIEXPORT jint JNICALL nSTARestartTextHandler(JNIEnv* jenv, jobject obj, jint textRecordMode,
        jstring locale, int dataPatterns) {
     NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
             field_STAWrapper_nativeContext_J);
     if (nativeContext == NULL) {
         throwException(jenv, RUNTIME_EXCEPTION, "Not initialized");
         return 0;
     }

     cleanupTextRecordHandler(nativeContext->textRecordHandler);
     return startTextRecordHandler(jenv, nativeContext, textRecordMode, locale, dataPatterns);
 }
/*
  * Method:      nSTARestartRecording
  * Signature:   (I)I
  */
  JNIEXPORT jint JNICALL nSTARestartRecording(JNIEnv* jenv, jobject obj, jint textRecordMode) {
      NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
                    field_STAWrapper_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(jenv, RUNTIME_EXCEPTION, "Not initialized");
        return STA_RESULT_FAILED;
    }
    if (!nativeContext->textRecordHandler) {
        throwException(jenv, RUNTIME_EXCEPTION, "handle buffer cannot be null");
        return STA_RESULT_FAILED;
    }

    if(textRecordMode == TEXT_RECORD_MODE_PREVIEW) {
        LOGD("Calling Restart");
        return nativeContext->textRecordHandler->ReStart();
    }
    return SCVE_SUCCESS;
}
/*
* Method:      nSTAParseFrame
* Signature:   ([BIIIIIII[I)I
*/
 JNIEXPORT jint JNICALL nSTAParseFrame(JNIEnv* jenv, jobject obj, jbyteArray frameBytes, int width,
        int height, int angle, int roiX, int roiY, int roiWidth, int roiHeight, jintArray values) {
      NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
              field_STAWrapper_nativeContext_J);
      if (nativeContext == NULL) {
          throwException(jenv, RUNTIME_EXCEPTION, "Not initialized");
          return STA_RESULT_FAILED;
      }
      if (!nativeContext->textRecordHandler) {
          throwException(jenv, RUNTIME_EXCEPTION, "handle buffer cannot be null");
          return STA_RESULT_FAILED;
      }
      if (!nativeContext->textRecordResults) {
          throwException(jenv, RUNTIME_EXCEPTION, "Record results cannot be null");
          return STA_RESULT_FAILED;
      }
      if (!frameBytes) {
          throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "image bytes cannot be null");
          return STA_RESULT_FAILED;
      }
      if (width <= 0 || height <= 0) {
        throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION,
                "invalid image width, height cannot be 0 or less");
          return STA_RESULT_FAILED;
      }
      if (angle != 0 && angle != 90 && angle != 180 && angle != 270) {
          throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "invalid image angle");
          return STA_RESULT_FAILED;
      }

      jbyte *greyScaleBytes = jenv->GetByteArrayElements(frameBytes, NULL);
      jbyte* rotatedBytes = new jbyte[roiWidth * roiHeight];

    rotateAndCrop(greyScaleBytes, rotatedBytes, width, height, angle, roiX, roiY, roiWidth,
            roiHeight);
      if(angle == 90 || angle == 270) {// swap width and height
          int temp = width;
          width = height;
          height = temp;
      }

    Status status = parseGreyscale(jenv, nativeContext, (uint8_t*) rotatedBytes, roiWidth,
            roiHeight, values);

      delete [] rotatedBytes;
      rotatedBytes = NULL;

      jenv->ReleaseByteArrayElements( frameBytes, greyScaleBytes, 0);

      if (status == (int32_t)SCVE_TEXT_RECO_MULTI_FRAME_FUSION_EMPTY) {
          return STA_RESULT_EMPTY;
      }
      else if (status == (int32_t)SCVE_TEXT_RECO_MULTI_FRAME_FUSION_IN_PROGRESS) {
          return STA_RESULT_IN_PROGRESS;
      }
      else if (status == (int32_t)SCVE_TEXT_RECO_MULTI_FRAME_FUSION_TIMEOUT) {
          return STA_RESULT_TIMEOUT;
      }
      return status;
  }
/*
 * Method:      nSTAParseImage
 * Signature:   (Landroid/graphics/Bitmap;[I)I
 */
 JNIEXPORT jint JNICALL nSTAParseImage(JNIEnv* jenv, jobject obj, jobject bitmap,
        jintArray values) {
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
            field_STAWrapper_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(jenv, RUNTIME_EXCEPTION, "Not initialized");
        return 0;
    }
    if (!nativeContext->textRecordHandler) {
        throwException(jenv, RUNTIME_EXCEPTION, "handle buffer cannot be null");
        return STA_RESULT_FAILED;
    }
    if (!nativeContext->textRecordResults) {
        throwException(jenv, RUNTIME_EXCEPTION, "Record results cannot be null");
        return STA_RESULT_FAILED;
    }
    if (!bitmap) {
        throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "image buffer cannot be null");
        return STA_RESULT_FAILED;
    }

    LOGD("Converting image");
    AndroidBitmapInfo bitmapInfo;
    if (AndroidBitmap_getInfo(jenv, bitmap, &bitmapInfo) != ANDROID_BITMAP_RESULT_SUCCESS) {
        throwException(jenv, RUNTIME_EXCEPTION, "getting image information failed");
        return STA_RESULT_FAILED;
    }

    if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "Unsupported color format");
        return STA_RESULT_FAILED;
    }
    uint8_t *rgbaBytes = NULL;
    if (AndroidBitmap_lockPixels(jenv, bitmap,
            (void **) &rgbaBytes) != ANDROID_BITMAP_RESULT_SUCCESS) {
        throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "Bitmap pixels not locked");
        return STA_RESULT_FAILED;
    }
    if (rgbaBytes == NULL) {
        throwException(jenv, RUNTIME_EXCEPTION, "Not able to read pixels");
        return STA_RESULT_FAILED;
    }

    uint8_t *greyScaleBytes = new uint8_t[bitmapInfo.width * bitmapInfo.height];
    uint8_t *rgbBytes = new uint8_t[bitmapInfo.width * bitmapInfo.height * 3];
    for (int i = 0; i < (int)(bitmapInfo.width * bitmapInfo.height); i++) {
        greyScaleBytes[i] = (uint8_t) (rgbaBytes[i * 4] * 0.2989 + rgbaBytes[i * 4 + 1] * 0.5870
                + rgbaBytes[i * 4 + 2] * 0.1140);

        rgbBytes[i*3] = (uint8_t) (rgbaBytes[i * 4]);
        rgbBytes[i*3 + 1] = (uint8_t) (rgbaBytes[i * 4 + 1]);
        rgbBytes[i*3 + 2] = (uint8_t) (rgbaBytes[i * 4 + 2]);
    }

    AndroidBitmap_unlockPixels(jenv, bitmap);
    LOGD("Converting image Done");

    Status status = generateTextnessMap(jenv, nativeContext, rgbBytes, bitmapInfo.width,
            bitmapInfo.height);
    if (status == SCVE_SUCCESS) {
    Status status = parseGreyscale(jenv, nativeContext, greyScaleBytes, bitmapInfo.width,
            bitmapInfo.height, values);
    } else {
        LOGD("Textness Failed: %d", status);
    }
    delete[] greyScaleBytes;
    greyScaleBytes = NULL;

    delete[] rgbBytes;
    rgbBytes = NULL;

    return status;
}
/*
 * Method:     nSTAGetData
 * Signature:  (I[ILjava/lang/StringBuffer;)Z
 */
 JNIEXPORT jboolean JNICALL nSTAGetData(JNIEnv* jenv, jobject obj, jint index, jintArray metadata,
        jobject textHolder) {
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
            field_STAWrapper_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(jenv, RUNTIME_EXCEPTION, "Not initialized");
        return 0;
    }

    if (!nativeContext->textRecordHandler) {
        throwException(jenv, RUNTIME_EXCEPTION, "handle cannot be null");
        return JNI_FALSE;
    }
    if (nativeContext->textRecordResults == NULL) {
        throwException(jenv, RUNTIME_EXCEPTION, "Record results cannot be null");
        return JNI_FALSE;
    }
    if (!metadata || !textHolder) {
        throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "metadata or text holder cannot be null");
        return JNI_FALSE;
    }
    jclass clazz = jenv->GetObjectClass(textHolder);
    jmethodID appendMthodId = jenv->GetMethodID(clazz, "append",
            "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
    if (appendMthodId == NULL) {
        LOGD("append method not there for textHolder");
        return JNI_FALSE;
    }

    jstring recognizedText = jenv->NewStringUTF(
            (const char *) nativeContext->textRecordResults[index].text);
    jenv->CallObjectMethod(textHolder, appendMthodId, recognizedText);

    jint *metadataIntArray = jenv->GetIntArrayElements(metadata, NULL);
    metadataIntArray[0] = (int32_t) nativeContext->textRecordResults[index].line_id;

    metadataIntArray[1] = toSTAAppDataType(nativeContext->textRecordResults[index].category);

    metadataIntArray[2] = nativeContext->textRecordResults[index].vertex[0].nLocX;
    metadataIntArray[3] = nativeContext->textRecordResults[index].vertex[0].nLocY;

    metadataIntArray[4] = nativeContext->textRecordResults[index].vertex[1].nLocX;
    metadataIntArray[5] = nativeContext->textRecordResults[index].vertex[1].nLocY;

    metadataIntArray[6] = nativeContext->textRecordResults[index].vertex[2].nLocX;
    metadataIntArray[7] = nativeContext->textRecordResults[index].vertex[2].nLocY;

    metadataIntArray[8] = nativeContext->textRecordResults[index].vertex[3].nLocX;
    metadataIntArray[9] = nativeContext->textRecordResults[index].vertex[3].nLocY;

    if (nativeContext->textRecordResults[index].category == TEXT_RECO_CATEGORY_DATETIME) {
        metadataIntArray[10] = nativeContext->textRecordResults[index].datetime_count;
    }

    jenv->ReleaseIntArrayElements(metadata, metadataIntArray, 0);
    return JNI_TRUE;
}
/*
  * Method:     nSTAGetDatetime
  * Signature:  (IILjava/lang/StringBuffer;)Z
 */
 JNIEXPORT jboolean JNICALL nSTAGetDatetime(JNIEnv* jenv, jobject obj, jint resultIndex,
        jint dateIndex, jobject textHolder) {
     NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
             field_STAWrapper_nativeContext_J);
     if (nativeContext == NULL) {
         throwException(jenv, RUNTIME_EXCEPTION, "Not initialized");
         return 0;
     }

     if (!nativeContext->textRecordHandler) {
         throwException(jenv, RUNTIME_EXCEPTION, "handle cannot be null");
         return JNI_FALSE;
     }
     if (nativeContext->textRecordResults == NULL) {
         throwException(jenv, RUNTIME_EXCEPTION, "Record results cannot be null");
         return JNI_FALSE;
     }
     if (!textHolder) {
         throwException(jenv, ILLEGAL_ARGUMENT_EXCEPTION, "text holder cannot be null");
         return JNI_FALSE;
     }
     jclass clazz = jenv->GetObjectClass(textHolder);
     jmethodID appendMthodId = jenv->GetMethodID(clazz, "append",
             "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
     if (appendMthodId == NULL) {
         LOGD("append method not there for textHolder");
         return JNI_FALSE;
     }

    if (nativeContext->textRecordResults[resultIndex].category != TEXT_RECO_CATEGORY_DATETIME
            || dateIndex >= nativeContext->textRecordResults[resultIndex].datetime_count) {
         return JNI_FALSE;
     }

     jstring dateTimeText = jenv->NewStringUTF(
             (const char *) nativeContext->textRecordResults[resultIndex].datetime[dateIndex]);
     jenv->CallObjectMethod(textHolder, appendMthodId, dateTimeText);
     return JNI_TRUE;
 }
/*
 * Method:      nSTARelease
 * Signature:   ()V
 */
 JNIEXPORT void JNICALL nSTARelease(JNIEnv* jenv, jobject obj) {
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
            field_STAWrapper_nativeContext_J);

    if (nativeContext == NULL) {
        throwException(jenv, RUNTIME_EXCEPTION, "Not initialized");
        return;
    }
    cleanupNativeContext(nativeContext);

    delete nativeContext;
    jenv->SetLongField(obj, field_STAWrapper_nativeContext_J, 0L);
}
///////////////////////////////////////// Image Correction /////////////////////////////////////////
static eRegionExtractionMode toScveRegionExtractionMode(int appRegionExtractionMode) {
    eRegionExtractionMode scveRegionExtractionMode = REGION_EXTRACTION_QUADRILATERAL;

    if (appRegionExtractionMode == IC_REGION_EXTRACTION_CURVED_BOUNDARY) {
        scveRegionExtractionMode = REGION_EXTRACTION_CURVED_BOUNDARY;
    } else if (appRegionExtractionMode == IC_REGION_EXTRACTION_CURVED_TEXTLINE) {
        scveRegionExtractionMode = REGION_EXTRACTION_CURVED_TEXTLINE;
    } else if (appRegionExtractionMode == IC_REGION_EXTRACTION_AUTO) {
        scveRegionExtractionMode = REGION_EXTRACTION_AUTO;
    }

    LOGD("IC Region Extraction Mode: %d", scveRegionExtractionMode);
    return scveRegionExtractionMode;
}
static eImageEnhancementMode toScveImageEnhancementMode(int appImageExtractionMode) {
    eImageEnhancementMode scveImageEnhancementMode = IMAGE_ENHANCEMENT_WHITEBOARD;

    if (appImageExtractionMode == IC_IMAGE_ENHANCEMENT_PROJECTOR) {
        scveImageEnhancementMode = IMAGE_ENHANCEMENT_PROJECTOR;
    } else if (appImageExtractionMode == IC_IMAGE_ENHANCEMENT_DOCUMENT) {
        scveImageEnhancementMode = IMAGE_ENHANCEMENT_DOCUMENT;
    } else if (appImageExtractionMode == IC_IMAGE_ENHANCEMENT_BUSINESSCARD) {
        scveImageEnhancementMode = IMAGE_ENHANCEMENT_BUSINESSCARD;
    } else if (appImageExtractionMode == IC_IMAGE_ENHANCEMENT_SCREEN) {
        scveImageEnhancementMode = IMAGE_ENHANCEMENT_SCREEN;
    } else if (appImageExtractionMode == IC_IMAGE_ENHANCEMENT_AUTO) {
        scveImageEnhancementMode = IMAGE_ENHANCEMENT_AUTO;
    }

    LOGD("IC Image Enhancement Mode: %d", scveImageEnhancementMode);
    return scveImageEnhancementMode;
}
void fillIcImageInfo(Image *pImage, Rect *pRect) {
    pImage->sDim.nWidth = pRect->nWidth;
    pImage->sDim.nHeight = pRect->nHeight;
    pImage->sDim.nStride = pRect->nWidth;
    pImage->nChannels = 3;
    pImage->pPtr = new uint8_t[pRect->nWidth * pRect->nHeight * 3 / 2];
    pImage->sDim.nStrides[0] = pRect->nWidth;
    pImage->sDim.nStrides[1] = pRect->nWidth;
    pImage->sDim.nStrides[2] = pRect->nWidth;
    pImage->sFormat = (ColorFormat) SCVE_COLORFORMAT_YUV_NV21;
}
void cleanUpEnhancedImages(NativeContext *nativeContext) {
      for (int i = 0; i < MAX_CANDIDATES; i++) {
        if (nativeContext->enhancedImages[i].pPtr) {
            delete[] nativeContext->enhancedImages[i].pPtr;
            nativeContext->enhancedImages[i].pPtr = NULL;
          }
      }
  }
  /*
   * Method:    nIcInit
   * Signature: ()I
 */
JNIEXPORT jint JNICALL nIcInit(JNIEnv* jenv, jobject obj) {
      NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
              field_STAWrapper_nativeContext_J);
      if (nativeContext == NULL) {
          return SCVE_ERROR_INTERNAL;
      }
    nativeContext->icHandler = ImageCorrection::newInstance(nativeContext->context,
            IMAGEMODE_SINGLE_FRAME);
      return nativeContext->icHandler->Init();
  }

  /*
   * Method:      nIcRectifyImage
 * Signature:   ([BIIIII[I)I
   */
   JNIEXPORT jint JNICALL nIcRectifyImage(JNIEnv* jenv, jobject obj, jbyteArray nv21Bytes,
        jint imageWidth, jint imageHeight, jint regionExtractionMode, jint imageExtractionMode,
        jint scaleMode, jintArray outValues) {
      NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
              field_STAWrapper_nativeContext_J);
      if (nativeContext == NULL) {
          return SCVE_ERROR_INTERNAL;
      }
      if (!nativeContext->icHandler) {
          return SCVE_ERROR_INTERNAL;
      }
      if (!nv21Bytes) {
          return SCVE_ERROR_INVALID_PARAM;
      }
      if (imageWidth <= 0 || imageHeight <= 0) {
          return SCVE_ERROR_INVALID_PARAM;
      }

      uint8_t *nv21BytesArray = (uint8_t*) jenv->GetByteArrayElements(nv21Bytes, NULL);
      Image image;
      image.sDim.nWidth = imageWidth;
      image.sDim.nHeight = imageHeight;
      image.sDim.nStride = imageWidth;
      image.nChannels = 3;
      image.pPtr = (uint8_t*) nv21BytesArray;
      image.sDim.nStrides[0] = imageWidth;
      image.sDim.nStrides[1] = imageWidth;
      image.sDim.nStrides[2] = imageWidth;
      image.sFormat = (ColorFormat) SCVE_COLORFORMAT_YUV_NV21;

      Rect roi;
      roi.nLocX = 0;
      roi.nLocY = 0;
      roi.nWidth = imageWidth;
      roi.nHeight = imageHeight;

    eRegionExtractionMode jmode = toScveRegionExtractionMode(regionExtractionMode);

      Region boundaryPos[MAX_CANDIDATES];
      uint16_t candidates = 0;

      struct timeval beforeTimeValue;
      gettimeofday(&beforeTimeValue, NULL);
      LOGD("Detecting rect started at %ld%03ld ms", beforeTimeValue.tv_sec, (beforeTimeValue.tv_usec / 1000));

      Status status = nativeContext->icHandler->GetRegionCandidates(&image, roi, jmode,
              MAX_CANDIDATES, &candidates, boundaryPos);

      struct timeval afterTimeValue;
      gettimeofday(&afterTimeValue, NULL);
      LOGD("Detecting rect done at %ld%03ld ms", afterTimeValue.tv_sec, (afterTimeValue.tv_usec / 1000));
      long timeDuration = (afterTimeValue.tv_sec - beforeTimeValue.tv_sec) * 1000
            + (afterTimeValue.tv_usec - beforeTimeValue.tv_usec) / 1000;
      LOGD("Detecting rect time Duration: %ld", timeDuration);

      if (status == SCVE_SUCCESS) {
        eRectificationScale jscaleMode =
                (scaleMode == IC_SCALE_MODE_HIGH_RESOLUTION) ?
                  SCALE_HIGH_RESOLUTION : SCALE_LOW_RESOLUTION;
          Rect dims[MAX_CANDIDATES];
        status = nativeContext->icHandler->GetRectifiedImageDims(&image, candidates, boundaryPos,
                jscaleMode, dims);

          if (status == SCVE_SUCCESS) {
            cleanUpEnhancedImages(nativeContext);
            Image rectImages[MAX_CANDIDATES];
              for (int i = 0; i < candidates; i++) {
                fillIcImageInfo(&rectImages[i], &dims[i]);
                fillIcImageInfo(&nativeContext->enhancedImages[i], &dims[i]);
              }
            status = nativeContext->icHandler->GetRectifiedImages(&image, candidates, boundaryPos,
                    rectImages);
              if (status == SCVE_SUCCESS) {
                eImageEnhancementMode mod = toScveImageEnhancementMode(imageExtractionMode);
                status = nativeContext->icHandler->EnhanceImage((const Image*) &rectImages,
                        candidates, boundaryPos, mod, nativeContext->enhancedImages);
                if(status != SCVE_SUCCESS) {
                    LOGD("EnhanceImage failed: %d", status);
                }
            } else {
                LOGD("GetRectifiedImages failed: %d", status);
              }
              for (int i = 0; i < MAX_CANDIDATES; i++) {
                if (rectImages[i].pPtr) {
                    delete[] rectImages[i].pPtr;
                    rectImages[i].pPtr = NULL;
                 }
              }
        } else {
            LOGD("GetRectifiedImageDims failed: %d", status);
          }
          if (status == SCVE_SUCCESS) {
              jint *oarr = jenv->GetIntArrayElements(outValues, NULL);
              oarr[0] = (int32_t) candidates;
              jenv->ReleaseIntArrayElements(outValues, oarr, 0);
          }
    } else {
        LOGD("GetRegionCandidates failed: %d", status);
      }
      jenv->ReleaseByteArrayElements(nv21Bytes, (jbyte*) nv21BytesArray, JNI_ABORT);
      return status;
  }

  /*
   * Method:      nIcGetRectifiedImage
   * Signature:   (I[I)I
   */
  JNIEXPORT jint JNICALL nIcGetRectifiedImageInfo(JNIEnv* jenv, jobject obj, jint index,
          jintArray outValues) {
      NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
              field_STAWrapper_nativeContext_J);
      if (nativeContext == NULL) {
          return SCVE_ERROR_INTERNAL;
      }
      if (!outValues) {
          return SCVE_ERROR_INVALID_PARAM;
      }
      jint *oarr = jenv->GetIntArrayElements(outValues, NULL);
    oarr[0] = (int32_t) nativeContext->enhancedImages[index].sDim.nWidth;
    oarr[1] = (int32_t) nativeContext->enhancedImages[index].sDim.nHeight;
      jenv->ReleaseIntArrayElements(outValues, oarr, 0);

      return SCVE_SUCCESS;
  }
  /*
   * Method:      nIcGetRectifiedImage
   * Signature:   (I[B)I
   */
  JNIEXPORT jint JNICALL nIcGetRectifiedImage(JNIEnv* jenv, jobject obj, jint index,
          jbyteArray frameOutBytes) {
      NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
              field_STAWrapper_nativeContext_J);
      if (nativeContext == NULL) {
          return SCVE_ERROR_INTERNAL;
      }
      if (!frameOutBytes) {
          return SCVE_ERROR_INVALID_PARAM;
      }

      uint8_t *metadataOutArray = (uint8_t*) jenv->GetByteArrayElements(frameOutBytes, NULL);
    memcpy(metadataOutArray, nativeContext->enhancedImages[index].pPtr,
            nativeContext->enhancedImages[index].sDim.nHeight
                    * nativeContext->enhancedImages[index].sDim.nStride * 3 / 2);

      jenv->ReleaseByteArrayElements(frameOutBytes, (jbyte*) metadataOutArray, 0);
      return SCVE_SUCCESS;
  }

  /*
   * Method:    nIcWrap
   * Signature: ()I
   */
  JNIEXPORT jint JNICALL nIcWrap(JNIEnv* jenv, jobject obj) {
      NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
              field_STAWrapper_nativeContext_J);
      if (nativeContext == NULL) {
          return SCVE_ERROR_INTERNAL;
      }

      if (nativeContext->icHandler != NULL) {
          nativeContext->icHandler->DeInit();
          ImageCorrection::deleteInstance(nativeContext->icHandler);
          nativeContext->icHandler = NULL;
      }
    cleanUpEnhancedImages(nativeContext);
      return SCVE_SUCCESS;
  }

///////////////////////////////////////// JNI INTERFACE ////////////////////////////////////////////

static const JNINativeMethod methods[] = {
        { "nClassInit", "()V", (void *) nClassInit },
        { "nSTAInit", "(IILjava/lang/String;I)I", (void *) nSTAInit },
        { "nSTAReconfigureTextHandler", "(Ljava/lang/String;I)I", (void *) nSTAReconfigureTextHandler },
        { "nSTARestartTextHandler", "(ILjava/lang/String;I)I", (void *) nSTARestartTextHandler },
        { "nSTAParseImage", "(Landroid/graphics/Bitmap;[I)I", (void *) nSTAParseImage },
        { "nSTARestartRecording", "(I)I", (void *) nSTARestartRecording },
        { "nSTAParseFrame", "([BIIIIIII[I)I", (void *) nSTAParseFrame },
        { "nSTAGetData", "(I[ILjava/lang/StringBuffer;)Z", (void *) nSTAGetData },
        { "nSTAGetDatetime", "(IILjava/lang/StringBuffer;)Z", (void *) nSTAGetDatetime },
        { "nSTARelease", "()V", (void *) nSTARelease },

        { "nIcInit", "()I", (void *) nIcInit },
        { "nIcWrap", "()I", (void *) nIcWrap },
        { "nIcRectifyImage", "([BIIIII[I)I", (void *) nIcRectifyImage },
        { "nIcGetRectifiedImageInfo", "(I[I)I", (void *) nIcGetRectifiedImageInfo },
        { "nIcGetRectifiedImage", "(I[B)I", (void *) nIcGetRectifiedImage }
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return STA_RESULT_FAILED;
    }

    jclass cls = env->FindClass(CLASS);
    if (env->ExceptionCheck()) {
        return STA_RESULT_FAILED;
    }
    env->RegisterNatives(cls, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) {
        return STA_RESULT_FAILED;
    }

    return JNI_VERSION_1_4;
}
