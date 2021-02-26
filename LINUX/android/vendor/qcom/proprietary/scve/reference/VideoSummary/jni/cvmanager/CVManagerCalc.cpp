/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "CVManagerCommon.h"

#define ASYNC 1
#undef LOG_TAG
#define LOG_TAG "CVManagerCalc"

const float AlphaTable[20][101] =
{
    /*0*/
    {0.50,0.63,0.68,0.70,0.72,0.74,0.75,0.77,0.78,0.79,0.79,0.80,0.81,0.82,0.82,0.83,0.83,0.84,0.84,0.85,0.85,0.86,0.86,0.86,0.87,0.87,0.87,0.88,0.88,0.88,0.89,0.89,0.89,0.90,0.90,0.90,0.90,0.91,0.91,0.91,0.91,0.91,0.92,0.92,0.92,0.92,0.93,0.93,0.93,0.93,0.93,0.93,0.94,0.94,0.94,0.94,0.94,0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.96,0.96,0.96,0.96,0.96,0.96,0.96,0.97,0.97,0.97,0.97,0.97,0.97,0.97,0.98,0.98,0.98,0.98,0.98,0.98,0.98,0.98,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,0.99,1.00,1.00,1.00,1.00,1.00},
    {0.30,0.40,0.46,0.50,0.53,0.55,0.57,0.59,0.60,0.62,0.63,0.64,0.65,0.66,0.67,0.68,0.69,0.70,0.71,0.72,0.72,0.73,0.74,0.75,0.75,0.76,0.76,0.77,0.78,0.78,0.79,0.79,0.80,0.80,0.81,0.81,0.82,0.82,0.82,0.83,0.83,0.84,0.84,0.84,0.85,0.85,0.86,0.86,0.86,0.87,0.87,0.87,0.88,0.88,0.88,0.89,0.89,0.89,0.90,0.90,0.90,0.91,0.91,0.91,0.91,0.92,0.92,0.92,0.93,0.93,0.93,0.93,0.94,0.94,0.94,0.94,0.95,0.95,0.95,0.95,0.96,0.96,0.96,0.96,0.97,0.97,0.97,0.97,0.97,0.98,0.98,0.98,0.98,0.99,0.99,0.99,0.99,0.99,1.00,1.00,1.00},
    {0.16,0.25,0.31,0.35,0.38,0.41,0.43,0.45,0.47,0.49,0.50,0.52,0.53,0.54,0.55,0.57,0.58,0.59,0.60,0.61,0.62,0.63,0.63,0.64,0.65,0.66,0.67,0.68,0.68,0.69,0.70,0.70,0.71,0.72,0.72,0.73,0.74,0.74,0.75,0.75,0.76,0.77,0.77,0.78,0.78,0.79,0.79,0.80,0.80,0.81,0.81,0.82,0.82,0.83,0.83,0.84,0.84,0.84,0.85,0.85,0.86,0.86,0.87,0.87,0.87,0.88,0.88,0.89,0.89,0.89,0.90,0.90,0.91,0.91,0.91,0.92,0.92,0.92,0.93,0.93,0.94,0.94,0.94,0.95,0.95,0.95,0.96,0.96,0.96,0.97,0.97,0.97,0.98,0.98,0.98,0.98,0.99,0.99,0.99,1.00,1.00},
    {0.08,0.16,0.21,0.25,0.28,0.30,0.32,0.35,0.36,0.38,0.40,0.41,0.43,0.44,0.46,0.47,0.48,0.49,0.50,0.51,0.53,0.54,0.55,0.56,0.57,0.57,0.58,0.59,0.60,0.61,0.62,0.63,0.63,0.64,0.65,0.66,0.66,0.67,0.68,0.69,0.69,0.70,0.71,0.71,0.72,0.73,0.73,0.74,0.75,0.75,0.76,0.76,0.77,0.78,0.78,0.79,0.79,0.80,0.80,0.81,0.82,0.82,0.83,0.83,0.84,0.84,0.85,0.85,0.86,0.86,0.87,0.87,0.88,0.88,0.89,0.89,0.90,0.90,0.91,0.91,0.91,0.92,0.92,0.93,0.93,0.94,0.94,0.95,0.95,0.95,0.96,0.96,0.97,0.97,0.98,0.98,0.98,0.99,0.99,1.00,1.00},
    {0.05,0.10,0.14,0.17,0.20,0.22,0.24,0.26,0.28,0.30,0.32,0.33,0.35,0.36,0.37,0.39,0.40,0.41,0.42,0.44,0.45,0.46,0.47,0.48,0.49,0.50,0.51,0.52,0.53,0.54,0.55,0.56,0.57,0.57,0.58,0.59,0.60,0.61,0.62,0.62,0.63,0.64,0.65,0.66,0.66,0.67,0.68,0.69,0.69,0.70,0.71,0.71,0.72,0.73,0.73,0.74,0.75,0.75,0.76,0.77,0.77,0.78,0.79,0.79,0.80,0.81,0.81,0.82,0.82,0.83,0.84,0.84,0.85,0.85,0.86,0.87,0.87,0.88,0.88,0.89,0.89,0.90,0.91,0.91,0.92,0.92,0.93,0.93,0.94,0.94,0.95,0.95,0.96,0.96,0.97,0.97,0.98,0.98,0.99,0.99,1.00},
    /*5*/
    {0.01,0.06,0.10,0.12,0.14,0.17,0.18,0.20,0.22,0.24,0.25,0.27,0.28,0.29,0.31,0.32,0.33,0.35,0.36,0.37,0.38,0.39,0.40,0.41,0.42,0.44,0.45,0.46,0.47,0.48,0.49,0.50,0.50,0.51,0.52,0.53,0.54,0.55,0.56,0.57,0.58,0.59,0.59,0.60,0.61,0.62,0.63,0.64,0.64,0.65,0.66,0.67,0.68,0.68,0.69,0.70,0.71,0.71,0.72,0.73,0.74,0.74,0.75,0.76,0.77,0.77,0.78,0.79,0.79,0.80,0.81,0.81,0.82,0.83,0.83,0.84,0.85,0.85,0.86,0.87,0.87,0.88,0.89,0.89,0.90,0.91,0.91,0.92,0.93,0.93,0.94,0.94,0.95,0.96,0.96,0.97,0.98,0.98,0.99,0.99,1.00},
    {0.01,0.04,0.06,0.09,0.11,0.12,0.14,0.16,0.17,0.19,0.20,0.21,0.23,0.24,0.25,0.27,0.28,0.29,0.30,0.31,0.32,0.34,0.35,0.36,0.37,0.38,0.39,0.40,0.41,0.42,0.43,0.44,0.45,0.46,0.47,0.48,0.49,0.50,0.51,0.52,0.53,0.54,0.54,0.55,0.56,0.57,0.58,0.59,0.60,0.61,0.62,0.62,0.63,0.64,0.65,0.66,0.67,0.67,0.68,0.69,0.70,0.71,0.72,0.72,0.73,0.74,0.75,0.76,0.76,0.77,0.78,0.79,0.79,0.80,0.81,0.82,0.83,0.83,0.84,0.85,0.86,0.86,0.87,0.88,0.89,0.89,0.90,0.91,0.91,0.92,0.93,0.94,0.94,0.95,0.96,0.96,0.97,0.98,0.99,0.99,1.00},
    {0.01,0.03,0.04,0.06,0.08,0.09,0.11,0.12,0.13,0.15,0.16,0.17,0.18,0.20,0.21,0.22,0.23,0.24,0.25,0.26,0.28,0.29,0.30,0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.39,0.40,0.41,0.42,0.43,0.44,0.45,0.46,0.47,0.48,0.49,0.50,0.51,0.52,0.53,0.54,0.55,0.56,0.57,0.57,0.58,0.59,0.60,0.61,0.62,0.63,0.64,0.65,0.66,0.66,0.67,0.68,0.69,0.70,0.71,0.72,0.73,0.73,0.74,0.75,0.76,0.77,0.78,0.79,0.79,0.80,0.81,0.82,0.83,0.84,0.84,0.85,0.86,0.87,0.88,0.89,0.89,0.90,0.91,0.92,0.93,0.94,0.94,0.95,0.96,0.97,0.98,0.98,0.99,1.00},
    {0.00,0.02,0.03,0.04,0.06,0.07,0.08,0.09,0.10,0.11,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.20,0.21,0.22,0.23,0.25,0.26,0.27,0.28,0.29,0.30,0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.39,0.40,0.41,0.42,0.43,0.44,0.45,0.46,0.47,0.48,0.49,0.50,0.51,0.52,0.53,0.54,0.55,0.56,0.56,0.57,0.58,0.59,0.60,0.61,0.62,0.63,0.64,0.65,0.66,0.67,0.68,0.69,0.70,0.71,0.72,0.73,0.73,0.74,0.75,0.76,0.77,0.78,0.79,0.80,0.81,0.82,0.83,0.84,0.85,0.85,0.86,0.87,0.88,0.89,0.90,0.91,0.92,0.93,0.94,0.95,0.95,0.96,0.97,0.98,0.99,1.00},
    {0.00,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.10,0.11,0.12,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.20,0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,0.30,0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.39,0.40,0.41,0.42,0.43,0.44,0.45,0.46,0.47,0.48,0.49,0.50,0.51,0.52,0.53,0.54,0.55,0.56,0.57,0.58,0.59,0.60,0.61,0.62,0.63,0.64,0.65,0.66,0.67,0.68,0.69,0.70,0.71,0.72,0.73,0.74,0.75,0.76,0.77,0.78,0.79,0.80,0.81,0.82,0.83,0.84,0.85,0.86,0.87,0.88,0.89,0.90,0.91,0.92,0.93,0.94,0.95,0.96,0.97,0.98,0.99,1.00},
    /*10*/
    {0.01,0.01,0.01,0.02,0.03,0.04,0.05,0.05,0.06,0.07,0.08,0.09,0.10,0.11,0.12,0.12,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.20,0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,0.30,0.31,0.32,0.33,0.33,0.34,0.35,0.36,0.38,0.39,0.40,0.41,0.42,0.43,0.44,0.45,0.46,0.47,0.48,0.49,0.50,0.51,0.52,0.53,0.54,0.55,0.56,0.57,0.58,0.59,0.60,0.61,0.62,0.63,0.64,0.65,0.66,0.68,0.69,0.70,0.71,0.72,0.73,0.74,0.75,0.76,0.77,0.78,0.79,0.80,0.81,0.83,0.84,0.85,0.86,0.87,0.88,0.89,0.90,0.91,0.92,0.93,0.95,0.96,0.97,0.98,0.99,1.00},
    {0.00,0.00,0.01,0.01,0.02,0.03,0.03,0.04,0.05,0.06,0.06,0.07,0.08,0.09,0.09,0.10,0.11,0.12,0.13,0.14,0.14,0.15,0.16,0.17,0.18,0.19,0.20,0.21,0.22,0.23,0.24,0.25,0.25,0.26,0.27,0.28,0.29,0.30,0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.39,0.40,0.41,0.42,0.44,0.45,0.46,0.47,0.48,0.49,0.50,0.51,0.52,0.53,0.54,0.55,0.56,0.57,0.59,0.60,0.61,0.62,0.63,0.64,0.65,0.66,0.67,0.69,0.70,0.71,0.72,0.73,0.74,0.75,0.77,0.78,0.79,0.80,0.81,0.82,0.83,0.85,0.86,0.87,0.88,0.89,0.90,0.92,0.93,0.94,0.95,0.96,0.98,0.99,1.00},
    {0.00,0.00,0.00,0.01,0.01,0.02,0.02,0.02,0.03,0.03,0.04,0.05,0.05,0.06,0.06,0.07,0.08,0.08,0.09,0.10,0.11,0.11,0.12,0.13,0.14,0.14,0.15,0.16,0.17,0.18,0.19,0.19,0.20,0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,0.30,0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.39,0.40,0.41,0.42,0.43,0.44,0.46,0.47,0.48,0.49,0.50,0.51,0.52,0.54,0.55,0.56,0.57,0.58,0.59,0.61,0.62,0.63,0.64,0.66,0.67,0.68,0.69,0.71,0.72,0.73,0.74,0.76,0.77,0.78,0.80,0.81,0.82,0.84,0.85,0.86,0.88,0.89,0.90,0.92,0.93,0.94,0.96,0.97,0.99,1.00},
    {0.00,0.00,0.00,0.00,0.00,0.01,0.01,0.01,0.01,0.02,0.02,0.02,0.03,0.03,0.04,0.04,0.04,0.05,0.05,0.06,0.06,0.07,0.08,0.08,0.09,0.09,0.10,0.11,0.11,0.12,0.13,0.14,0.14,0.15,0.16,0.17,0.18,0.18,0.19,0.20,0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,0.30,0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.40,0.41,0.42,0.43,0.44,0.46,0.47,0.48,0.49,0.51,0.52,0.53,0.55,0.56,0.57,0.59,0.60,0.61,0.63,0.64,0.66,0.67,0.68,0.70,0.71,0.73,0.74,0.76,0.77,0.79,0.80,0.82,0.84,0.85,0.87,0.88,0.90,0.92,0.93,0.95,0.97,0.98,1.00},
    {0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.01,0.01,0.01,0.01,0.01,0.02,0.02,0.02,0.02,0.03,0.03,0.03,0.04,0.04,0.04,0.05,0.05,0.06,0.06,0.07,0.07,0.08,0.08,0.09,0.09,0.10,0.11,0.11,0.12,0.13,0.13,0.14,0.15,0.16,0.16,0.17,0.18,0.19,0.20,0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,0.30,0.31,0.33,0.34,0.35,0.36,0.37,0.39,0.40,0.41,0.43,0.44,0.46,0.47,0.49,0.50,0.52,0.53,0.55,0.56,0.58,0.60,0.61,0.63,0.65,0.66,0.68,0.70,0.72,0.74,0.75,0.77,0.79,0.81,0.83,0.85,0.87,0.89,0.91,0.94,0.96,0.98,1.00},
    /*15*/
    {0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.02,0.02,0.02,0.02,0.02,0.03,0.03,0.03,0.04,0.04,0.04,0.05,0.05,0.05,0.06,0.06,0.07,0.07,0.08,0.09,0.09,0.10,0.10,0.11,0.12,0.13,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.20,0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.29,0.30,0.31,0.33,0.34,0.36,0.37,0.39,0.41,0.42,0.44,0.46,0.47,0.49,0.51,0.53,0.55,0.57,0.59,0.61,0.64,0.66,0.68,0.70,0.73,0.75,0.78,0.80,0.83,0.86,0.88,0.91,0.94,0.97,1.00},
    {0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.02,0.02,0.02,0.02,0.02,0.03,0.03,0.03,0.04,0.04,0.04,0.05,0.05,0.06,0.06,0.07,0.07,0.08,0.08,0.09,0.10,0.10,0.11,0.12,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.20,0.22,0.23,0.24,0.26,0.27,0.29,0.31,0.33,0.34,0.36,0.38,0.40,0.43,0.45,0.47,0.50,0.52,0.55,0.58,0.61,0.64,0.67,0.70,0.73,0.77,0.80,0.84,0.88,0.92,0.96,1.00},
    {0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.02,0.02,0.02,0.02,0.02,0.03,0.03,0.03,0.04,0.04,0.05,0.05,0.06,0.06,0.07,0.08,0.08,0.09,0.10,0.11,0.12,0.13,0.15,0.16,0.17,0.19,0.20,0.22,0.24,0.26,0.28,0.30,0.33,0.35,0.38,0.41,0.44,0.47,0.51,0.55,0.59,0.63,0.67,0.72,0.77,0.82,0.88,0.94,1.00},
    {0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.02,0.02,0.02,0.03,0.03,0.03,0.04,0.05,0.05,0.06,0.07,0.08,0.09,0.10,0.11,0.13,0.14,0.16,0.18,0.20,0.23,0.26,0.29,0.32,0.36,0.40,0.44,0.49,0.55,0.60,0.67,0.74,0.82,0.91,1.00},
    {0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.01,0.01,0.01,0.01,0.02,0.02,0.03,0.03,0.04,0.05,0.05,0.07,0.08,0.10,0.11,0.14,0.16,0.19,0.23,0.27,0.32,0.38,0.45,0.53,0.62,0.73,0.85,1.00}
};

extern CVpara_t mPara;

int32_t util_score_mapping(int32_t value,
                           int32_t lbound,
                           int32_t hbound,
                           int32_t lbound_out,
                           int32_t hbound_out,
                           uint32_t alpha,
                           uint8_t inverse)
{
    int32_t valueout = 0;
    float32_t tvalue = 0;
    if (value > hbound || value < lbound) {
        DPRINTF(" util_score_mapping error!! Value out of range %d\n", value);
        return lbound_out;
    }
    if (inverse) {
        tvalue = (float32_t)(value - hbound) / (float32_t)(lbound - hbound) * 100;
    }
    else
    {
        tvalue = (float32_t)(value - lbound) / (float32_t)(hbound - lbound) * 100;
    }

    valueout = AlphaTable[alpha][uint32_t(tvalue)] * (hbound_out - lbound_out) + lbound_out;
    return valueout;
}

uint32_t findend(CVPara_t *pPara)
{
    uint32_t offset = 0;
    for(uint32_t i = 0; i < MAX_PARA_LEN;++i)
    {
        if (pPara->data[i] == '%') {
            if (pPara->data[i + 1] == '%') {
                if (pPara->data[i + 2] == 'E') {
                    if (pPara->data[i + 3] == 'D') {
                        offset = i;
                        break;                        
                    }
                }
            }
        }
    }
    return offset;
}

void gaussian1d5(int32_t *input, uint32_t length,float32_t *output)
{
    DPRINTF("gaussiand5 E:\n");
    if (input == NULL || output == NULL) {
        return;
    }
    float32_t tbuffer[8];
    uint8_t startindx;
    uint8_t endindx;

    DPRINTF("gaussiand5 length %d\n", length);

    tbuffer[0] = input[0];
    tbuffer[1] = input[1];
    tbuffer[2] = input[2];
    tbuffer[3] = input[3];
    tbuffer[4] = input[4];
    tbuffer[5] = input[5];
    tbuffer[6] = input[6];
    tbuffer[7] = input[7];

    startindx = 0;

    for (uint32_t i = 2; i < length - 5; ++i) {
        output[i] = (0.1f *  tbuffer[startindx] +
                        0.2f *  tbuffer[(startindx + 1)%8] +
                        0.4f *  tbuffer[(startindx + 2)%8] +
                        0.2f *  tbuffer[(startindx + 3)%8] +
                        0.1f *  tbuffer[(startindx + 4)%8]);
        tbuffer[startindx] = input[i + 6];
        startindx ++;
        startindx %= 8;
    }
    for (uint32_t i = length -5; i < length -2; ++i) {
        output[i] = (0.1f *  tbuffer[startindx] +
                        0.2f *  tbuffer[(startindx + 1)%8] +
                        0.4f *  tbuffer[(startindx + 2)%8] +
                        0.2f *  tbuffer[(startindx + 3)%8] +
                        0.1f *  tbuffer[(startindx + 4)%8]);
        startindx ++;
        startindx %= 8;
    }
    DPRINTF("gaussiand5 X:\n");
}

int16_t logkernel[9] = {7,9,8,5,0,-5,-8,-9,-7};

void log1d9(uint8_t *input, uint32_t length, int16_t *output) {
    if (input == NULL || output == NULL) {
        return;
    }

    memset(output,0,sizeof(int16_t)*length);
    for (uint32_t i = 4; i < length - 4; ++i) {
        output[i] = (int16_t)(logkernel[i-4] *  (int16_t)input[i-4] +
                        logkernel[i-3]       *  (int16_t)input[i-3] +
                        logkernel[i-2]       *  (int16_t)input[i-2] +
                        logkernel[i-1]       *  (int16_t)input[i-1] +
                        logkernel[i]         *  (int16_t)input[i]   +
                        logkernel[i+4]       *  (int16_t)input[i+4] +
                        logkernel[i+3]       *  (int16_t)input[i+3] +
                        logkernel[i+2]       *  (int16_t)input[i+2] +
                        logkernel[i+1]       *  (int16_t)input[i+1]);
    }
}

void Sort(uint32_t *indx, uint32_t size, uint32_t *data1, uint32_t *data2, uint32_t *dataindx, uint32_t *revindx){
    uint8_t swapped = 1;
    uint32_t tempval= 0;
    while (swapped) {
        swapped = 0;
        for (uint32_t i = 0; i < size - 1; ++i) {
            if (indx[i] < indx[i+1]) {
                tempval     = indx[i];
                indx[i]     = indx[i+1];
                indx[i+1]   = tempval;

                tempval      = data1[i];
                data1[i]     = data1[i+1];
                data1[i+1]   = tempval;

                tempval      = data2[i];
                data2[i]     = data2[i+1];
                data2[i+1]   = tempval;

                tempval      = dataindx[i];
                dataindx[i]  = dataindx[i+1];
                dataindx[i+1]= tempval;

                revindx[dataindx[i]] --;
                revindx[dataindx[i+1]] ++;
                swapped      = 1;
            }
        }
    }
}

void CheckIndxBound(int32_t &startind,int32_t &endind, uint32_t length){
    //DPRINTF("CheckIndxBound E:\n");
    //DPRINTF("CheckIndxBound start%d, end %d\n", startind, endind);
    if (startind < 0) {
        endind   = endind - startind;
        startind = 0;
    }
    if (endind >= length) {
        startind = startind - (endind - length + 1);
        endind   = length - 1;
    }
    //DPRINTF("CheckIndxBound start%d, end %d\n", startind, endind);
}

/**
Shot: contains a continues ues scene, may have multiple segments
Segment: continuases scene, segmented by importance
*/
CVsum_t MergeSegment(uint32_t *segmax,   //Sorted
                     uint32_t *segstart,
                     uint32_t *segstop,
                     uint32_t *segindx,
                     uint32_t *sortindx,
                     uint32_t segcount,
                     uint32_t datalength,
                     int32_t minshotlength,
                     int32_t desireshotlength)  {
    DPRINTF("MergeSegment E:\n");
    CVsum_t output;
    if (datalength < desireshotlength) {
        output.output_length    = datalength;
        output.start_indx[0]    = 0;
        output.end_indx[0]      = datalength - 1;
        return output;
    }
    DPRINTF("MergeSegment Desired length %d\n", desireshotlength);

    int32_t seglength = 0;
    output.output_length    = desireshotlength;
    output.output_seg       = 0;

    int32_t *m_start   = (int32_t *)malloc(sizeof(int32_t) * MAX_SEG);
    int32_t *m_stop    = (int32_t *)malloc(sizeof(int32_t) * MAX_SEG);
    uint32_t *m_max    = (uint32_t *)malloc(sizeof(uint32_t) * MAX_SEG);
    uint32_t *m_length = (uint32_t *)malloc(sizeof(uint32_t) * MAX_SEG);
    uint32_t m_count   = 0;

    if (desireshotlength <= minshotlength && desireshotlength > 0)   {
        output.output_length    = desireshotlength;
        output.output_seg       = 1;
        output.seg_score[0] = segmax[0];
        //DPRINTF("MergeSegment segindx 0 segstart %d, segend %d\n", segstart[0], segstop[0]);
        output.start_indx[0] = (segstart[0] + segstop[0]) / 2 - desireshotlength / 2;
        output.end_indx[0]   = (segstart[0] + segstop[0]) / 2 + desireshotlength / 2;
        CheckIndxBound(output.start_indx[0], output.end_indx[0], datalength);

        seglength               = segstop[0] - segstart[0];
    } else {
        uint8_t indxmask[MAX_SEG];
        memset(indxmask, 0, sizeof(uint8_t) * MAX_SEG);

        for (uint32_t i = 0; i < segcount; ++i) {
            //DPRINTF("Current segindx %d segstart %d, segend %d\n", i, segstart[i], segstop[i]);
            //Is not part of output yet
            if (indxmask[i] == 0) {
                m_max[m_count] = segmax[i];
                m_start[m_count] = segstart[i];
                m_stop[m_count]  = segstop[i];
                seglength        = m_stop[m_count] - m_start[m_count];
                int32_t l_indx   = segindx[i] - 1;
                int32_t r_indx   = segindx[i] + 1;
                uint8_t merged   = 1;

                //Mark this segment is taken
                indxmask[i] = 1;
                while (merged) {
                    merged = 0;

                    if (l_indx >= 0 && indxmask[sortindx[l_indx]] == 0) {
                        //DPRINTF("LIndx %d segindx %d indxmask %d\n",l_indx, sortindx[l_indx],indxmask[sortindx[l_indx]]);
                        if (segstop[sortindx[l_indx]] >= m_start[m_count] - minshotlength / 2 &&
                            (m_stop[m_count] - (int32_t)(segstart[sortindx[l_indx]]) <= desireshotlength || desireshotlength == 0)&&
                            m_stop[m_count] - (int32_t)(segstart[sortindx[l_indx]]) <= mPara.data[PARA_MAXSHOTLEN]) {
                            /*DPRINTF("MergeSegment left segindx %d segstart %d, segend %d\n",
                                                sortindx[l_indx],
                                                segstart[sortindx[l_indx]],
                                                segstop[sortindx[l_indx]]);*/
                            merged = 1;
                            m_start[m_count] = segstart[sortindx[l_indx]];
                            indxmask[sortindx[l_indx]] = 1;
                            seglength = m_stop[m_count] - m_start[m_count];
                            l_indx--;
                        }
                    }

                    if (r_indx < segcount && indxmask[sortindx[r_indx]] == 0) {
                        //DPRINTF("RIndx %d segindx %d indxmask %d\n",r_indx, sortindx[r_indx],indxmask[sortindx[r_indx]]);
                        if ((segstart[sortindx[r_indx]] <= m_stop[m_count] + minshotlength / 2 )&&
                            ((int32_t)(segstop[sortindx[r_indx]]) - m_start[m_count] <= desireshotlength || desireshotlength == 0)&&
                            ((int32_t)(segstop[sortindx[r_indx]]) - m_start[m_count] <= mPara.data[PARA_MAXSHOTLEN])) {
                            /*DPRINTF("MergeSegment right segindx %d segstart %d, segend %d\n",
                                                sortindx[r_indx],
                                                segstart[sortindx[r_indx]],
                                                segstop[sortindx[r_indx]]);*/
                            m_stop[m_count] = segstop[sortindx[r_indx]];
                            indxmask[sortindx[r_indx]] = 1;
                            merged    = 1;
                            seglength = m_stop[m_count] - m_start[m_count];
                            r_indx++;
                        }
                    }

                    //Expand short Segments. its also considered merged so stay in loop
                    if (seglength < minshotlength) {
                        m_start[m_count] = (m_start[m_count] + m_stop[m_count]) / 2 - minshotlength / 2;
                        m_stop[m_count]  =  m_start[m_count] + minshotlength;
                        CheckIndxBound(m_start[m_count], m_stop[m_count], datalength);
                        seglength = minshotlength;
                        merged = 1;
                    }
                }
                m_length[m_count] = seglength;
                m_count++; //Merged into single shot
            }
        }
        for (uint32_t i = 0; i < m_count; ++i) {
            DPRINTF("Merged shot number %d  segstart %d, segend %d, seglenth %d, score %d\n",
                    i,
                    m_start[i],
                    m_stop[i],
                    m_length[i],
                    m_max[i]);
        }
        /****************************************************/
        output.output_length = 0;

        if (desireshotlength == 0) {  //Output all the shot
            uint32_t threshval = mPara.data[PARA_TIMELAPSE_THRESH];
            bool threshislow   = true;
            while(threshislow)
            {
                threshislow = false;
                for (uint32_t i = 0; i < m_count; ++i) {
                    if (m_max[i] > threshval) { // this needs to be a parameter
                        output.seg_score[output.output_seg]  = m_max[i];
                        output.start_indx[output.output_seg] = m_start[i];
                        output.end_indx[output.output_seg]   = m_stop[i];

                        seglength  = output.end_indx[output.output_seg] -
                            output.start_indx[output.output_seg];

                        output.output_length += seglength;
                        output.output_seg++;
                        if (output.output_length > datalength / 2) {
                            threshislow          = true;
                            threshval           += 5;
                            output.output_length = 0;
                            output.output_seg    = 0;
                            break;
                        }
                        if (output.output_seg >= MAX_SEG_NUMBER) {
                            break;
                        }
                    }
                }
            }

            if (output.output_length < minshotlength) {
                output.output_length = 0;
                output.output_seg    = 0;
                for (uint32_t i = 0; i < m_count; ++i) {
                    if (m_max[i] >= mPara.data[PARA_PEAKVALTHRESH]) { // this needs to be a parameter
                        output.seg_score[output.output_seg] = m_max[i];
                        output.start_indx[output.output_seg] = m_start[i];
                        output.end_indx[output.output_seg] = m_stop[i];

                        seglength  = output.end_indx[output.output_seg] -
                           output.start_indx[output.output_seg];

                        output.output_length += seglength;
                        output.output_seg++;
                        if (output.output_length > datalength / 2) {
                            break;
                        }
                        if (output.output_seg >= MAX_SEG_NUMBER) {
                            break;
                        }
                    }
                }
            }

        } else {
            int32_t remainedlength  = desireshotlength;
            for (uint32_t i = 0; i < m_count; ++i) {
                if (remainedlength >= minshotlength) {
                    output.seg_score[output.output_seg]  = m_max[i];
                    output.start_indx[output.output_seg] = m_start[i];
                    output.end_indx[output.output_seg]   = m_stop[i];

                    seglength  = output.end_indx[output.output_seg] -
                       output.start_indx[output.output_seg];

                    if (remainedlength - seglength < minshotlength) {
                        output.start_indx[output.output_seg] = (output.start_indx[output.output_seg] + output.end_indx[output.output_seg]) / 2 - remainedlength / 2;
                        output.end_indx[output.output_seg] = output.start_indx[output.output_seg] + remainedlength;

                        CheckIndxBound(output.start_indx[output.output_seg], output.end_indx[output.output_seg], datalength);
                        seglength = remainedlength;
                    }
                    remainedlength       -= seglength;
                    output.output_length += seglength;
                    output.output_seg++;
                    if (output.output_seg >= MAX_SEG_NUMBER) {
                        break;
                    }
                } else break;
            }
        }
    }

    free(m_start);
    free(m_stop);
    free(m_max);
    free(m_length);
    DPRINTF("MergeSegment X:\n");
    return output;
}

extern
int32_t getmodulenumber(uint32_t mode)
{
    int32_t moduleno = 0;
    if (mode == MODE_ALL) {
        moduleno = MAX_MODULES;
    }
    else
    {
        for (uint32_t i = 0; i < MAX_MODULES; ++i) {
            moduleno += (mode >> i) & 1;
        }
    }
    return moduleno;
}

extern 
uint8_t checkmode(uint32_t input, uint32_t MODE) {
    if ((input & MODE) != 0) {
        return 1;
    } else return 0;
}

