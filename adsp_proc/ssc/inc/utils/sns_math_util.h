#pragma once
/*=============================================================================
  @file sns_math_util.h

  Mathematical Utility functions for use by Sensor Developers.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <math.h>

/*=============================================================================
  Macro Definitions
  ===========================================================================*/
#define TRI_AXIAL_SENSOR_AXES 3

#ifndef G
#define G                              9.81188f
#endif

#ifndef PI
#define PI                             3.14159f
#endif

#ifndef EARTH_SI
#define EARTH_SI                       9.80665f
#endif

#define SNS_MAX(a,b)          ({ __typeof__ (a) _a = (a);    \
                             __typeof__ (b) _b = (b);    \
                             _a > _b ? _a : _b; })

#define SNS_MIN(a,b)          ({ __typeof__ (a) _a = (a);    \
                             __typeof__ (b) _b = (b);    \
                             _a < _b ? _a : _b; })

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* 3x1 column vector */
typedef struct vector3
{
  union {
    struct {
      float x, y, z;
    };
    float data[3];
  };
} vector3;


/* 3x3 square matrix (row major) */
typedef struct matrix3
{
  union {
    struct {
      float e00, e01, e02;
      float e10, e11, e12;
      float e20, e21, e22;
    };
    float data[9];
  };
} matrix3;

/* 4-dimensional rotation quaternion */
typedef struct quaternion
{
  union {
    struct {
      float w, x, y, z;
    };
    float data[4];
  };
} quaternion;

typedef struct
{
  double w;
  double x;
  double y;
  double z;
} quaternion_d;

/*=============================================================================
  Function Definitions
  ===========================================================================*/

/**
 * creates vector3 from three numbers
 *
 * @param x
 * @param y
 * @param z
 * @return vector3
 */
static inline vector3 make_vector3(float x, float y, float z)
{
  vector3 vec;

  vec.x = x;
  vec.y = y;
  vec.z = z;

  return vec;
}

/**
 * creates vector3 from an array
 *
 * @param x
 * @param y
 * @param z
 * @return vector3
 */
static inline vector3 make_vector3_from_array(float *data)
{
  vector3 vec;

  vec.x = data[0];
  vec.y = data[1];
  vec.z = data[2];

  return vec;
}

/**
 * adds one 3D vector to another
 * @param a vector3
 * @param b vector3
 *
 * @return vector3 (a + b)
 */
static inline vector3 vector3_add(vector3 a, vector3 b)
{
  return make_vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

/**
 * subtracts one 3D vector from another
 * @param a vector3
 * @param b vector3
 *
 * @return vector3 (a - b)
 */
static inline vector3 vector3_sub(vector3 a, vector3 b)
{
  return make_vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

/**
 * calculates dot-product of two 3D vectors
 *
 * @param a vector3
 * @param b vector3
 *
 * @return float (a dot b)
 */
static inline float vector3_dot(vector3 a, vector3 b)
{
  return (a.x * b.x + a.y * b.y + a.z * b.z);
}

/**
 * calculates cross product of 2 3D vectors
 * @param a vector3
 * @param b vector3
 *
 * @return vector3 (a cross b)
 */
static inline vector3 vector3_cross(vector3 a, vector3 b)
{
  vector3 vec;
  // cross(A,B) = [Ay*Bz-Az*By, Az*Bx-Ax*Bz, Ax*By-Ay*Bx]
  vec.x = a.y * b.z - a.z * b.y;
  vec.y = a.z * b.x - a.x * b.z;
  vec.z = a.x * b.y - a.y * b.x;

  return vec;
}

/**
 * calculates vector's norm squared
 *
 * @param v 3x1 vector
 * @return float norm-squared
 */
static inline float vector3_norm_sq(vector3 v)
{
  return (v.x * v.x + v.y * v.y + v.z * v.z);
}

/**
 * calculates norm (magnitude) of a 3x1 vector
 * @param v vector3
 * @return float  vector norm
 */
static inline float vector3_norm(vector3 v)
{
  return sqrtf(vector3_norm_sq(v));
}

/**
 * returns a matrix given its rows as vectors
 *
 * @param c0 row 0
 * @param c1 row 1
 * @param c2 row 2
 *
 * @return matrix3
 */
static inline matrix3 make_matrix3_by_rows(vector3 r0, vector3 r1, vector3 r2)
{
  matrix3 mat;

  mat.e00 = r0.x;
  mat.e01 = r0.y;
  mat.e02 = r0.z;
  mat.e10 = r1.x;
  mat.e11 = r1.y;
  mat.e12 = r1.z;
  mat.e20 = r2.x;
  mat.e21 = r2.y;
  mat.e22 = r2.z;

  return mat;
}

/**
 * returns a matrix given its columns as vectors
 *
 * @param c0 column 0
 * @param c1 column 1
 * @param c2 column 2
 *
 * @return matrix3
 */
static inline matrix3 make_matrix3_by_cols(vector3 c0, vector3 c1, vector3 c2)
{
  matrix3 mat;

  mat.e00 = c0.x;
  mat.e01 = c1.x;
  mat.e02 = c2.x;
  mat.e10 = c0.y;
  mat.e11 = c1.y;
  mat.e12 = c2.y;
  mat.e20 = c0.z;
  mat.e21 = c1.z;
  mat.e22 = c2.z;

  return mat;
}

/**
 * creates a matrix3 object from raw data
 *
 * @param data pointer to a float array of size 9
 *
 * @return matrix3
 */
static inline matrix3 make_matrix3_from_array(float *data)
{
  matrix3 m;
  for(int i = 0; i < 9; i++)
  {
    m.data[i] = data[i];
  }
  return m;
}

/**
 * multiply two 3x3 matrices
 *
 * @param a first matrix
 * @param b second matrix
 *
 * @return matrix3 matrix (a x b)
 */
static inline matrix3 matrix3_multiply(matrix3 a, matrix3 b)
{
  matrix3 ab;
  for(int i = 0; i < 3; i++)
  {
    for(int j = 0; j < 3; j++)
    {
      float sum = 0.0;
      for(int k = 0; k <= 2; k++)
      {
        sum += a.data[i * 3 + k] * b.data[k * 3 + j];
      }
      ab.data[i * 3 + j] = sum;
    }
  }
  return ab;
}

/**
 * muliply a 3x3 matrix to a 3x1 vector
 *
 * @param m  3x3 Matrix
 * @param v  3x1 vector
 *
 * @return vector3 3x1 vector
 */
static inline vector3 matrix3_mul_vector3(matrix3 m, vector3 v)
{
  vector3 mv;

  for(int i = 0; i < 3; i++)
  {
    mv.data[i] = m.data[i * 3 + 0] * v.data[0] +
                 m.data[i * 3 + 1] * v.data[1] +
                 m.data[i * 3 + 2] * v.data[2];
  }
  return mv;
}

/**
 * convert a 3x3 rotation matrix to a quaternion
 * @param m Rotation matrix
 * @return quaternion
 */
static inline quaternion matrix3_to_quaternion(matrix3 m)
{
  quaternion q;
  float denom;
  float diag_sum = 1.0 + m.e00 + m.e11 + m.e22;
  if(diag_sum > 0.01)
  {
    denom = 2 * sqrtf(diag_sum);
    q.w = denom / 4;
    q.x = (m.e21 - m.e12) / denom;
    q.y = (m.e02 - m.e20) / denom;
    q.z = (m.e10 - m.e01) / denom;
  }
  else if(m.e00 > m.e11 && m.e00 > m.e22)
  {
    denom = 2 * sqrtf(1 + m.e00 - m.e11 - m.e22);
    q.w = (m.e21 - m.e12) / denom;
    q.x = denom / 4;
    q.y = (m.e10 + m.e01) / denom;
    q.z = (m.e02 + m.e20) / denom;
  }
  else if(m.e11 > m.e22)
  {
    denom = 2 * sqrtf(1 + m.e11 - m.e00 - m.e22);
    q.w = (m.e02 - m.e20) / denom;
    q.x = (m.e10 + m.e01) / denom;
    q.y = denom / 4;
    q.z = (m.e21 + m.e12) / denom;
  }
  else
  {
    denom = 2 * sqrtf(1.0 + m.e22 - m.e00 - m.e11);
    q.w = (m.e10 - m.e01) / denom;
    q.x = (m.e02 + m.e20) / denom;
    q.y = (m.e21 + m.e12) / denom;
    q.z = denom / 4;
  }
  /* any round trip conversion must (will) yield original rotation matrix,
   for example fmv_get_rot_m(rotation_matrix_to_quaternion(R)) must equal R */
  q.x = -q.x;
  q.y = -q.y;
  q.z = -q.z;

  /* make sure QW is always non-negative. That will guarantee that
   our 4 element rotation vector is always corretly interpreted by Android
   apps which have to use 3 element rotation vector thus have to assume
   non-negative QW */
  if(q.w < 0)
  {
    q.x = -q.x;
    q.y = -q.y;
    q.z = -q.z;
    q.w = -q.w;
  }
  return q;
}

/**
 * converts a quaternion to 3x3 rotation matrix
 *
 * @param q quaternion
 *
 * @return matrix3 rotation matrix
 */

static inline matrix3 quaternion_to_matrix3(quaternion q)
{
  matrix3 m;
  m.e00 = 1 - 2 * q.data[2] * q.data[2] - 2 * q.data[3] * q.data[3];
  m.e01 = 2 * (q.data[1] * q.data[2] + q.data[0] * q.data[3]);
  m.e02 = 2 * (q.data[1] * q.data[3] - q.data[0] * q.data[2]);
  m.e10 = 2 * (q.data[1] * q.data[2] - q.data[0] * q.data[3]);
  m.e11 = 1 - 2 * q.data[1] * q.data[1] - 2 * q.data[3] * q.data[3];
  m.e12 = 2 * (q.data[2] * q.data[3] + q.data[0] * q.data[1]);
  m.e20 = 2 * (q.data[1] * q.data[3] + q.data[0] * q.data[2]);
  m.e21 = 2 * (q.data[2] * q.data[3] - q.data[0] * q.data[1]);
  m.e22 = 1 - 2 * q.data[1] * q.data[1] - 2 * q.data[2] * q.data[2];
  return m;
}

/*=========================================================================
  FUNCTION:  vector_cross
  =======================================================================*/
/*!
    @brief Compute the cross product of 2 vectors

    @param[i] v1: vector (in)
    @param[i] v2: vector (in)
    @param[i] v3: vector (out)

    @return void
*/
static inline void vector_cross_d(const double *restrict v1, const double *restrict v2, double * restrict v3)
{
  // cross(A,B) = [Ay*Bz-Az*By, Az*Bx-Ax*Bz, Ax*By-Ay*Bx]
  v3[0] = v1[1] * v2[2] - v1[2] * v2[1];
  v3[1] = v1[2] * v2[0] - v1[0] * v2[2];
  v3[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

/*=========================================================================
  FUNCTION:  vector_norm
  =======================================================================*/
/*!
    @brief Compute the norm of a vector

    @param[i] v: vector

    @return void
*/
static inline double vector_norm_d(double *v)
{
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}


/*=========================================================================
  FUNCTION:  convert_rotation_vector
  =======================================================================*/
/*!
    @brief convert rotation vector (quaternion) from NED(SAE) to ENU(Android),
    or vice versa

    @param[i] q1: source vector,
    @param[i] q2: destination vector

    Swap q->x and q->y
    Reverse q->z sign
    Copy q->w as is
*/
/*=======================================================================*/
static inline void convert_rotation_vector_sae_enu_d(
  const quaternion_d *q1,
  quaternion_d *q2)
{
  q2->x = q1->y;
  q2->y = q1->x;
  q2->z = -q1->z;
  q2->w = q1->w;
}

/*=========================================================================
  FUNCTION:  rotation_matrix_to_quaternion
  =======================================================================*/
/*!
    @brief convert 3x3 six_axis_rotation matrix to quaternion

    @param[i] m: rotation matrix
    @param[i] q: quaternion

    @return void

*/
/*=======================================================================*/
static inline void rotation_matrix_to_quaternion_d(
  double m[][TRI_AXIAL_SENSOR_AXES],
  quaternion_d *q)
{
  double diag_sum;
  double denom;

/*
  Matlab script from FDD v1.7
    diag_sum = 1.0 + Matrix(1,1) + Matrix(2,2) + Matrix(3,3);
  if diag_sum > 0.01
      denom = 2.0 * sqrt(diag_sum);
      Quaternion_WXYZ = [...
              denom / 4.0,...
              (Matrix(3,2) - Matrix(2,3)) / denom,...
              (Matrix(1,3) - Matrix(3,1)) / denom,...
              (Matrix(2,1) - Matrix(1,2)) / denom];
      debug_out_case = 1;
  elseif Matrix(1,1) > Matrix(2,2) && Matrix(1,1) > Matrix(3,3)
      denom = 2.0 * sqrt(1.0 + Matrix(1,1) - Matrix(2,2) - Matrix(3,3));
      Quaternion_WXYZ = [...
              (Matrix(3,2) - Matrix(2,3)) / denom,...
              denom / 4.0,...
              (Matrix(2,1) + Matrix(1,2)) / denom,...
              (Matrix(1,3) + Matrix(3,1)) / denom];
      debug_out_case = 2;
  elseif Matrix(2,2) > Matrix(3,3)
      denom = 2.0 * sqrt(1 + Matrix(2,2) - Matrix(1,1) - Matrix(3,3));
      Quaternion_WXYZ = [...
              (Matrix(1,3) - Matrix(3,1)) / denom,...
              (Matrix(2,1) + Matrix(1,2)) / denom,...
              denom / 4.0,...
              (Matrix(3,2) + Matrix(2,3)) / denom];
      debug_out_case = 3;
  else
       denom = 2.0 * sqrt(1 + Matrix(3,3) - Matrix(1,1) - Matrix(2,2));
       Quaternion_WXYZ = [...
              (Matrix(2,1) - Matrix(1,2)) / denom,...
              (Matrix(1,3) + Matrix(3,1))  / denom,...
              (Matrix(3,2) + Matrix(2,3)) / denom,...
              denom / 4.0];
       debug_out_case = 4;
    end
*/

  diag_sum = (double)1.0 + m[0][0] + m[1][1] + m[2][2];
  if (diag_sum > 0.01)
  {
    denom = 2 *  (double) sqrt(diag_sum);
    q->w = denom / 4;
    q->x = (m[2][1] - m[1][2]) / denom;
    q->y = (m[0][2] - m[2][0]) / denom;
    q->z = (m[1][0] - m[0][1]) / denom;
  }
  else if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
  {
    denom = 2 * (double) sqrt(1 + m[0][0] - m[1][1] - m[2][2]);
    q->w = (m[2][1] - m[1][2]) / denom;
    q->x = (double) (denom / 4.0);
    q->y = (m[1][0] + m[0][1]) / denom;
    q->z = (m[0][2] + m[2][0]) / denom;
  }
  else if (m[1][1] > m[2][2])
  {
    denom = 2 * (double) sqrt(1 + m[1][1] - m[0][0] - m[2][2]);
    q->w = (m[0][2] - m[2][0]) / denom;
    q->x = (m[1][0] + m[0][1]) / denom;
    q->y = denom / 4;
    q->z = (m[2][1] + m[1][2]) / denom;
  }
  else
  {
    denom = 2 * (double) sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]);
    q->w = (m[1][0] - m[0][1]) / denom;
    q->x = (m[0][2] + m[2][0]) / denom;
    q->y = (m[2][1] + m[1][2]) / denom;
    q->z = denom / 4;
  }

  // Now any round trip conversion must (will) yield original rotation matrix,
  // for example fmv_get_rot_m(rotation_matrix_to_quaternion(R)) must equal R
  q->x = -q->x;
  q->y = -q->y;
  q->z = -q->z;

  // make sure QW is always non-negative. That will guarantee that
  // our 4 element rotation vector is always corretly interpreted by Android apps
  // which have to use 3 element rotation vector thus have to assume non-negative QW
  if (q->w < 0)
  {
    q->x = -q->x;
    q->y = -q->y;
    q->z = -q->z;
    q->w = -q->w;
  }
}
