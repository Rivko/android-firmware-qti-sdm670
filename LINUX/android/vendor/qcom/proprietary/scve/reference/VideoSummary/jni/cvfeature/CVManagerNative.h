/**=============================================================================

@file
CVManagerNative.h

@brief
CVManager API Definition containing common types 

Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================**/

 #ifndef _CV_MANAGER_NATIVE_H_
#define _CV_MANAGER_NATIVE_H_

#include <pthread.h>
#include <stdint.h>
#include <string.h>
//------------------------------------------------------------------------------
/// @brief
///    Predefine values for memory allocation
//------------------------------------------------------------------------------
#define MAX_SUMMARY_OUTPUT 256
#define MAX_SEG_NUMBER     256
#define MAX_FILE_INPUT     256
#define MAX_PARA_LEN       256


//------------------------------------------------------------------------------
/// @brief
///    Definition of process mode and summary mode, 
/// @details
///     select any combination of modes to run underlying modules/features
//------------------------------------------------------------------------------
#define MODE_BASIC        1 << 0
#define MODE_MOTION       1 << 1
#define MODE_FACE         1 << 2
#define MODE_ASD          1 << 3
#define MAX_MODULES       4
#define MODE_ALL          0xFFFFFFFF


//------------------------------------------------------------------------------
/// @brief
///    Definition of Parameter which can be passed to/from modules
/// @details 
///     Please refer to programming guide for detailed usage  
//------------------------------------------------------------------------------
typedef struct{
    uint8_t data[MAX_PARA_LEN];
}CVPara_t;

//------------------------------------------------------------------------------
/// @brief
///    Definition of status indicator returned by functions
//------------------------------------------------------------------------------
typedef enum {
    CV_SUCCESS = 0,
    CV_ERROR
}CVStatus_t;

//------------------------------------------------------------------------------
/// @brief
///    Definition of output data structure returned by process function call
/// @param status
///     status of function call indicating success return or not
/// @param mScore
///     Score output, range 0-100, the single value that represents the importance of the frame
/// @param modScore
///     score output for each selected mode, range 0-100, each value represents importance measure by that module
/// @param modnum
///     number of modules are/have valid outputs
//------------------------------------------------------------------------------
typedef struct {
    CVStatus_t status;
    int32_t    mScore;                    //Overall score output
    int32_t    modScore[MAX_MODULES];     //score output for each individual module, they are in order of default , motion , face
    uint32_t   modnum;                    //How many modules are used
}CVoutput_t;

//------------------------------------------------------------------------------
/// @brief
///    Definition of input data filled for single video file
/// @param MODE
///     Indicates which mode/module summary will take into final consideration
/// @param DATA_MODE
///     Represents the the data formats and how many seperate modules data are included
/// @param input_length
///     Total number of frames of data 
/// @param inputFPS
///     Frame rate of input data
/// @param desired_length
///     set to 0 
/// @param pBuffer
///     input data buffer, data is formatted in m0,n0,p0,m1,n1,p1... where m,n,p are data from separate modules. 
///     This format is similar to how we save bitmap into RGB format. 
/// @param pPara
///     additional parameter/settings to pass in
//------------------------------------------------------------------------------
typedef struct {
    uint32_t MODE;                        //Tell the summary which module score is taken into final consideration
    uint32_t DATA_MODE;    
    uint32_t input_length;                //in number of frames
    uint32_t inputFPS;                    //FPS
    uint32_t desired_length;              //is depreciated , no need to set in version 2
    int32_t *pBuffer;                     //Frame Score buffers, each module has a score, and an overall if its there
    CVPara_t*pPara;                       //para data;
}CVFileInput_t; 

//------------------------------------------------------------------------------
/// @brief
///    Definition of input data structure filled for summary operation
/// @param pFileinput
///     Pointers to each file input 
/// @param file_no
///     Number of total input files 
/// @param summarytype
///     automatically output summary video if target_lengtg is set to 0
///     Value:ULOWCOMPRESSED(timelapse):0 LOW_COMPRESSED:1 HIGH_COMPRESSED:2 SINGLESHOTEACH:3
/// @param target_length
///     Manual set summarized output video length in number of frames
///     
//------------------------------------------------------------------------------
typedef struct {
    CVFileInput_t* pFileinput[MAX_FILE_INPUT];   //FILE input pointers
    uint32_t file_no;                            //Number of input video files
    uint8_t  summarytype;                        //0 1,2,3 low to high compress ratio
    uint32_t target_length;                      //Fixed output length if set to 0, output length is decided automatically by algorithm
}CVSuminput_t;

//------------------------------------------------------------------------------
/// @brief
///    Definition of output data structure for single video file
/// @param output_length
///     Number of frames of total output from this video file
/// @param output_seg
///     Number of total summarized output segments 
/// @param start_indx
///     start indexes/start frame number for each output segments
/// @param end_indx
///     end indexes/end frame number for each output segments
/// @param seg_score
///     importance score for each output segment, range 0-100 
///     
//------------------------------------------------------------------------------
typedef struct {
    uint32_t output_length;
    uint32_t output_seg;
    int32_t start_indx[MAX_SEG_NUMBER];
    int32_t end_indx[MAX_SEG_NUMBER];
    int32_t seg_score[MAX_SEG_NUMBER];
}CVsum_t;

//------------------------------------------------------------------------------
/// @brief
///    Definition of output data structure for summary operation
/// @param ouput_num
///     Number of CVsum_t outputs, this usually equals number of input video data
/// @param data
///     Additional output infor besides summarized video segments
/// @param output 
///     output 
//------------------------------------------------------------------------------
typedef struct {
    uint32_t ouput_num;                          //For multi video file input, ouput_num is HOW MANY FILE has output, normally is equal to file_no
    CVPara_t data;                               //Category type
    CVsum_t output[MAX_SUMMARY_OUTPUT];          //For multi video file input, each individual file output. file output may has zero output_length
}CVSumouput_t;

extern "C"
{
//------------------------------------------------------------------------------
/// @brief
///    Initialize manager process operation 
/// @param mode
///     Indicates what mod/modules the cvmanager will process on
//------------------------------------------------------------------------------
    CVStatus_t CVManagerInit(uint32_t mode);

//------------------------------------------------------------------------------
/// @brief
///    Denitialize manager process operation 
//------------------------------------------------------------------------------
    CVStatus_t CVManagerDeinit();

//------------------------------------------------------------------------------
/// @brief
///    Process operation 
/// @param ImginputY
///     input image frame Y plane
/// @param ImginputUV 
///     input image frame UV plane 
/// @param width 
///     width of input image frame
/// @param height
///     height of input image frame
/// @param srcYStride 
///     Stride of input image's Y plane data
/// @param srcVUStride 
///     Stride of input image's UV plane data
/// @param ImageoutputY 
///     if there is output image, this is output images Y plane
/// @param ImageoutputUV
///     if there is output image, this is output images UV plane
/// @param dstYStride 
///     if there is output image, this is output images Y plane's stride
/// @param dstVUStride 
///     if there is output image, this is output images UV plane's stride
/// @return CVoutput_t 
///     result of process this frame
//------------------------------------------------------------------------------
    CVoutput_t CVManagerProcess(uint8_t* ImginputY,
        uint8_t* ImginputUV,
        uint32_t width,
        uint32_t height,
        uint32_t srcYStride,
        uint32_t srcVUStride,
        uint8_t* ImgoutputY,
        uint8_t* ImgoutputUV,
        uint32_t dstYStride,
        uint32_t dstVUStride);
    
//------------------------------------------------------------------------------
/// @brief
///    CVManagerSet set data operation, only between init and deinit state
/// @param para
///     data structure that contains info that needs to be passed to specific module. Multiple data can be combined/concatenated.
/// @param mode
///     indicate which module will be the receiving side
/// @return CVStatus_t 
///     SUCCESS or ERROR
//------------------------------------------------------------------------------
    CVStatus_t CVManagerSet(CVPara_t para, uint32_t mode);

//------------------------------------------------------------------------------
/// @brief
///    CVManagerGet get data operation, only between init and deinit state
/// @param para
///     data structure retrieved from get operation
/// @param mode
///     indicate which module will be the sending side
/// @return CVStatus_t 
///     SUCCESS or ERROR
//------------------------------------------------------------------------------
    CVStatus_t CVManagerGet(CVPara_t *para, uint32_t mode);
}
extern "C"
{
//------------------------------------------------------------------------------
/// @brief
///    Summarized input video data and select output video segments
/// @param input
///     input data, see data structure details above
/// @param output
///     ouptut data pointer, see data structure details above
/// @return CVStatus_t 
///     SUCCESS or ERROR
//------------------------------------------------------------------------------
    CVStatus_t CVSummary(CVSuminput_t input, CVSumouput_t *output);

//------------------------------------------------------------------------------
/// @brief
///    Get current version. version will be printed
/// @return CVStatus_t 
///     SUCCESS or ERROR
//------------------------------------------------------------------------------
    CVStatus_t getVersion();
}

#endif
