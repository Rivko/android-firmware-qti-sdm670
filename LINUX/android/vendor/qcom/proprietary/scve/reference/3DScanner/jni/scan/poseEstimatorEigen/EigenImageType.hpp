/**=============================================================================

@file
   EigenImageType.h

@brief
    various Eigen Types used by 3DR
   

Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================**/

#ifndef __EIGEN_IMAGE_TYPE__
#define __EIGEN_IMAGE_TYPE__

// include Eigen
#include <unistd.h>
#define EIGEN_MALLOC_ALREADY_ALIGNED 0

#include "../3rdparty/eigen/Eigen/Core"
#include "../3rdparty/eigen/Eigen/SVD"
#include "../3rdparty/eigen/Eigen/LU"
//#include <stdint.h>

// general mat/array definition
typedef Eigen::Matrix<uint8_t,Eigen::Dynamic,Eigen::Dynamic>  Mat8U;
typedef Eigen::Matrix<uint16_t,Eigen::Dynamic,Eigen::Dynamic> Mat16U;
typedef Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic>    Mat32F;

typedef Eigen::Array<bool,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>  ArrayRowMajorBool;
typedef Eigen::Array<uint8_t,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>  ArrayRowMajor8U;
typedef Eigen::Array<uint16_t,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> ArrayRowMajor16U;
typedef Eigen::Array<float,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>    ArrayRowMajor32F;

typedef Eigen::Array<uint8_t,Eigen::Dynamic,Eigen::Dynamic>  Array8U;
typedef Eigen::Array<uint16_t,Eigen::Dynamic,Eigen::Dynamic> Array16U;
typedef Eigen::Array<float,Eigen::Dynamic,Eigen::Dynamic>    Array32F;

// fix point vector
typedef Eigen::Matrix<uint8_t,3,1>  Vec3u8;
typedef Eigen::Matrix<uint8_t,4,1>  Vec4u8;
typedef Eigen::Matrix<int16_t,2,1>  Vec2s16;
typedef Eigen::Matrix<uint32_t,2,1> Vec2u32;
typedef Eigen::Matrix<uint32_t,3,1> Vec3u32;
typedef Eigen::Matrix<int32_t,2,1>  Vec2s32;
typedef Eigen::Matrix<int32_t,3,1>  Vec3s32;
typedef Eigen::Matrix<float,2,1>    Vec2f32;
typedef Eigen::Matrix<float,3,1>    Vec3f32;
typedef Eigen::Matrix<float,4,1>    Vec4f32;
// general matx
typedef Eigen::Matrix<double,3,1 >                 Matx31Rd;    
typedef Eigen::Matrix<float,1,2 >                  Matx12Rf;
typedef Eigen::Matrix<double,3,1>  Matx31Cd;
typedef Eigen::Matrix<double,3,3>  Matx33Cd;
typedef Eigen::Matrix<double,3,4>  Matx34Cd;
typedef Eigen::Matrix<double,4,1>  Matx41Cd;
typedef Eigen::Matrix<double,4,4>  Matx44Cd;
typedef Eigen::Matrix<double,3,1 > Matx31Cd;    
typedef Eigen::Matrix<double,6,1 > Matx61Cd;
typedef Eigen::Matrix<double,7,1 > Matx71Cd;
typedef Eigen::Matrix<double,6,6 > Matx66Cd;
typedef Eigen::Matrix<double,7,7 > Matx77Cd;
typedef Eigen::Matrix<float,1,2 >  Matx12Cf;
typedef Eigen::Matrix<float,3,1 >  Matx31Cf;
typedef Eigen::Matrix<float,6,1 >  Matx61Cf;
typedef Eigen::Matrix<float,7,1 >  Matx71Cf;
typedef Eigen::Matrix<float,1,3 >  Matx13Cf;
typedef Eigen::Matrix<float,2,3>   Matx23Cf;
typedef Eigen::Matrix<float,6,6>   Matx66Cf;
typedef Eigen::Matrix<float,3,3 >  Matx33Cf;
typedef Eigen::Matrix<float,4,1>   Matx41Cf;
typedef Eigen::Matrix<float,4,4>   Matx44Cf;

// general image definition
typedef ArrayRowMajorBool ImgBool;
typedef ArrayRowMajor8U   Img8U;
typedef ArrayRowMajor16U  Img16U;
typedef ArrayRowMajor32F  Img32F;

// reference type
typedef Eigen::Ref<Img8U>         RefImg8U;
typedef Eigen::Ref<Img16U>        RefImg16U;
typedef Eigen::Ref<Img32F>        RefImg32F;

typedef Eigen::Ref<const Img8U>   RefConstImg8U;
typedef Eigen::Ref<const Img16U>  RefConstImg16U;
typedef Eigen::Ref<const Img32F>  RefConstImg32F;

typedef Eigen::Ref<const Matx31Cf> RefConstMatx31Cf;
typedef Eigen::Ref<const Matx61Cf> RefConstMatx61Cf;
typedef Eigen::Ref<const Matx13Cf> RefConstMatx13Cf;
//typedef Eigen::Ref<const Matx16Rf> RefConstMatx16Rf;

// multi channel image
typedef Img8U         Img8UC3;
typedef RefImg8U      RefImg8UC3;
typedef RefConstImg8U RefConstImg8UC3;

#endif
