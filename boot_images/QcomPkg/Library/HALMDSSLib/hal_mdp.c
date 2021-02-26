/*! \file */

/*
===========================================================================

FILE:         hal_mdp.c

DESCRIPTION:  MDP Initialization

===========================================================================
===========================================================================
Copyright (c) 2012 - 2017 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"
#include "hal_mdp_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Global Data Definitions
 *------------------------------------------------------------------------ */
HAL_MDP_Hw_Private_Info    gsMdpHwInfo;
HAL_MDP_Hw_Private_Info   *pgsMdpHwInfo   = NULL;

/*
 * Limited REC601 Standard
 *
 * Limited REC601 YUV to RGB Conversion Matrix
 *    [  1.1644,          0.0000,              1.5960   ]
 *    [  1.1644,         -0.3918,             -0.8130   ]
 *    [  1.1644,          2.0172,              0.0000   ]
 *
 * Limited REC601 Standard in s(4.9)
 *
 *    [  0x0254,          0x0000,              0x0331   ]
 *    [  0x0254,          0xFF38,              0xFE60   ]
 *    [  0x0254,          0x0408,              0x0000   ]
 *
 * Matrix Value Packing
 *
 */
static uint32   MDP_CSC_YuvToRgb_Matrix_Limited_Rec601[] =
{
   0x00000254,                // Hi-WORD Co-eff R1C2 - Low-WORD Co_eff R1C1
   0x02540331,                // Hi-WORD Co-eff R2C1 - Low-WORD Co_eff R1C3
   0xFE60FF38,                // Hi-WORD Co-eff R2C3 - Low-WORD Co_eff R2C2
   0x04080254,                // Hi-WORD Co-eff R3C2 - Low-WORD Co_eff R3C1
   0x00000000                 // Hi-WORD INVALID     - Low-WORD Co_eff R3C3
};

/* Limited REC601 YUV to RGB Pre-clamp Values */
static uint16   MDP_CSC_YuvToRgb_PreClamp_Limited_Rec601[]  = { 0x10EB, 0x10F0, 0x10F0 }; // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC601 YUV to RGB Post-clamp Values  */
static uint16   MDP_CSC_YuvToRgb_PostClamp_Limited_Rec601[] = { 0x00FF, 0x00FF, 0x00FF }; // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC601 YUV to RGB Pre-bias Values  */
static uint16   MDP_CSC_YuvToRgb_PreBias_Limited_Rec601[]   = { 0xFFF0, 0xFF80, 0xFF80 };  

/* Limited REC601 YUV to RGB Post-bias Values  */
static uint16   MDP_CSC_YuvToRgb_PostBias_Limited_Rec601[]  = { 0x0000, 0x0000, 0x0000 }; 


/* CSC_10 bit  Values************************************************************************
 */
/* Limited REC601 YUV to RGB Pre-clamp Values  */
static uint32 MDP_CSC10BIT_YuvToRgb_PreClamp_Limited_Rec601[]    = {0x004003AC, 0x004003C0, 0x004003C0};  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC601 YUV to RGB Post-clamp Values */
static uint32   MDP_CSC10BIT_YuvToRgb_PostClamp_Limited_Rec601[] = {0x000003FF, 0x000003FF, 0x000003FF};   // Hi-Byte LOW clamp - Low-Byte HIGH clamp
    
/* Limited REC601 YUV to RGB Pre-bias Values  */
static uint16   MDP_CSC10BIT_YuvToRgb_PreBias_Limited_Rec601[]   = {0xFFC0, 0xFE00, 0xFE00};

/* Limited REC601 YUV to RGB Post-bias Values  */
static uint16   MDP_CSC10BIT_YuvToRgb_PostBias_Limited_Rec601[]   = { 0x0000, 0x0000, 0x0000};

/** Color Space Conversion(CSC) - Limited REC601 - YUV to RGB */
const HAL_MDP_CSCEncodingInfo  gMDP_CSC_YuvToRgb_Limited_Rec601 =
{
  MDP_CSC_YuvToRgb_Matrix_Limited_Rec601,        /* Conversion matrix */
  MDP_CSC_YuvToRgb_PreClamp_Limited_Rec601,      /* Pre-clamp         */
  MDP_CSC_YuvToRgb_PostClamp_Limited_Rec601,     /* Post-clamp        */
  MDP_CSC_YuvToRgb_PreBias_Limited_Rec601,       /* Pre-bias          */
  MDP_CSC_YuvToRgb_PostBias_Limited_Rec601,      /* Post-bias         */
  MDP_CSC10BIT_YuvToRgb_PreClamp_Limited_Rec601, /* 10 bit Pre-Clamp   */
  MDP_CSC10BIT_YuvToRgb_PostClamp_Limited_Rec601,/* 10 bit Post-Clamp  */
  MDP_CSC10BIT_YuvToRgb_PreBias_Limited_Rec601,  /* 10 bit Pre-Bias    */
  MDP_CSC10BIT_YuvToRgb_PostBias_Limited_Rec601, /* 10 bit Post-Bias   */
};



 /*
 * Limited REC601 Standard
 *
 * Limited REC601 RGB to YUV Conversion Matrix
 *    [  0.2568,          0.5041,              0.0979   ]
 *    [ -0.1482,         -0.2910,              0.4392   ]
 *    [  0.4392,         -0.3678,             -0.0714   ]
 *
 * Limited REC601 Standard in s(4.9)
 *
 *    [  0x0083,          0x0102,              0x0032   ]
 *    [  0xFFB4,          0xFF6B,              0x00E1   ]
 *    [  0x00E1,          0xFF44,              0xFFDB   ]
 *
 * Matrix Value Packing
 *
 */
uint32   MDP_CSC_RgbToYuv_Matrix_Limited_Rec601[] =
{
   0x01020083,                // Hi-WORD Co-eff R1C2 - Low-WORD Co_eff R1C1
   0xFFB40032,                // Hi-WORD Co-eff R2C1 - Low-WORD Co_eff R1C3
   0x00E1FF6B,                // Hi-WORD Co-eff R2C3 - Low-WORD Co_eff R2C2
   0xFF4400E1,                // Hi-WORD Co-eff R3C2 - Low-WORD Co_eff R3C1
   0x0000FFDB                 // Hi-WORD INVALID     - Low-WORD Co_eff R3C3
};

/* Limited REC601 RGB to YUV Pre-clamp Values  */
static uint16   MDP_CSC_RgbToYuv_PreClamp_Limited_Rec601[]   = { 0x00FF, 0x00FF, 0x00FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC601 RGB to YUV Post-clamp Values */
static uint16   MDP_CSC_RgbToYuv_PostClamp_Limited_Rec601[]  = { 0x10EB, 0x10F0, 0x10F0 };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC601 RGB to YUV Pre-bias Values  */
static uint16   MDP_CSC_RgbToYuv_PreBias_Limited_Rec601[]    = { 0x0000, 0x0000, 0x0000 };  

/* Limited REC601 RGB to YUV Post-bias Values  */
static uint16   MDP_CSC_RgbToYuv_PostBias_Limited_Rec601[]   = { 0x0010, 0x0080, 0x0080 }; 

/* CSC_10 bit  Values************************************************************************
 */
/* Limited REC601 RGB to YUV Pre-clamp Values  */
static uint32 MDP_CSC10BIT_RgbToYuv_PreClamp_Limited_Rec601[]    = {0x000003FF,  0x000003FF, 0x000003FF};  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC601 RGB to YUV Post-clamp Values */
static uint32   MDP_CSC10BIT_RgbToYuv_PostClamp_Limited_Rec601[] = {0x004003AC, 0x004003C0, 0x004003C0};   // Hi-Byte LOW clamp - Low-Byte HIGH clamp
    
/* Limited REC601 RGB to YUV Pre-bias Values  */
static uint16   MDP_CSC10BIT_RgbToYuv_PreBias_Limited_Rec601[]   = {0x00000000, 0x00000000, 0x00000000};

/* Limited REC601 RGB to YUV Post-bias Values  */
static uint16   MDP_CSC10BIT_RgbToYuv_PostBias_Limited_Rec601[]   = { 0x0040, 0x0200, 0x0200};

/** Color Space Conversion(CSC) - Limited REC601 - RGB to YUV */
const HAL_MDP_CSCEncodingInfo  gMDP_CSC_RgbToYuv_Limited_Rec601 =
{
  MDP_CSC_RgbToYuv_Matrix_Limited_Rec601,            /* Conversion matrix */
  MDP_CSC_RgbToYuv_PreClamp_Limited_Rec601,          /* Pre-clamp         */
  MDP_CSC_RgbToYuv_PostClamp_Limited_Rec601,         /* Post-clamp        */
  MDP_CSC_RgbToYuv_PreBias_Limited_Rec601,           /* Pre-bias          */
  MDP_CSC_RgbToYuv_PostBias_Limited_Rec601,          /* Post-bias         */
  MDP_CSC10BIT_RgbToYuv_PreClamp_Limited_Rec601,     /* 10 bit Pre-Clamp  */
  MDP_CSC10BIT_RgbToYuv_PostClamp_Limited_Rec601,    /* 10 bit Post-Clamp */
  MDP_CSC10BIT_RgbToYuv_PreBias_Limited_Rec601,      /* 10 bit Pre-Bias   */
  MDP_CSC10BIT_RgbToYuv_PostBias_Limited_Rec601,     /* 10 bit Post-Bias  */
};



/*
 * Full REC601 Standard
 *
 * Full REC601 YUV to RGB Conversion Matrix
 *    [  1.0000,          0.0000,              1.4020   ]
 *    [  1.0000,         -0.3441,             -0.7141   ]
 *    [  1.0000,          1.7720,              0.0000   ]
 *
 * Full REC601 Standard in s(4.9)
 *
 *    [  0x0200,          0x0000,              0x02CD   ]
 *    [  0x0200,          0xFF50,              0xFE93   ]
 *    [  0x0200,          0x038B,              0x0000   ]
 *
 * Matrix Value Packing
 *
 */
static uint32 MDP_CSC_YuvToRgb_Matrix_Full_Rec601[] =
{
   0x00000200,                // Hi-WORD Co-eff R1C2 - Low-WORD Co_eff R1C1
   0x020002CD,                // Hi-WORD Co-eff R2C1 - Low-WORD Co_eff R1C3
   0xFE93FF50,                // Hi-WORD Co-eff R2C3 - Low-WORD Co_eff R2C2
   0x038B0200,                // Hi-WORD Co-eff R3C2 - Low-WORD Co_eff R3C1
   0x00000000,                // Hi-WORD INVALID     - Low-WORD Co_eff R3C3
};

/* Full REC601 YUV to RGB Pre-clamp Values */
static uint16   MDP_CSC_YuvToRgb_PreClamp_Full_Rec601[]  = { 0x00FF, 0x00FF, 0x00FF }; // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC601 YUV to RGB Post-clamp Values  */
static uint16   MDP_CSC_YuvToRgb_PostClamp_Full_Rec601[] = { 0x00FF, 0x00FF, 0x00FF }; // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC601 YUV to RGB Pre-bias Values  */
static uint16   MDP_CSC_YuvToRgb_PreBias_Full_Rec601[]   = { 0x0000, 0xFF80, 0xFF80 };  

/* Full REC601 YUV to RGB Post-bias Values  */
static uint16   MDP_CSC_YuvToRgb_PostBias_Full_Rec601[]  = { 0x0000, 0x0000, 0x0000 }; 

/* CSC_10 values **************************************************************/
/* Full REC600 YUV to RGB Pre-clamp Values  */
static uint32 MDP_CSC10BIT_YuvToRgb_PreClamp_Full_Rec601[]    = {0x003FF,  0x003FF, 0x003FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC601 YUV to RGB Post-clamp Values */
static uint32   MDP_CSC10BIT_YuvToRgb_PostClamp_Full_Rec601[] = {0x003FF,  0x003FF, 0x003FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp
    
/* Full REC601 YUV to RGB Pre-bias Values  */
static uint16   MDP_CSC10BIT_YuvToRgb_PreBias_Full_Rec601[]   = { 0x0000, 0xFE00, 0xFE00 };

/* Full REC601 YUV to RGB Post-bias Values  */
static uint16   MDP_CSC10BIT_YuvToRgb_PostBias_Full_Rec601[]  = { 0x0000, 0x0000, 0x0000 };


/** Color Space Conversion(CSC) - Full REC601 - YUV to RGB */
const HAL_MDP_CSCEncodingInfo  gMDP_CSC_YuvToRgb_Full_Rec601 =
{
  MDP_CSC_YuvToRgb_Matrix_Full_Rec601,          /* Conversion matrix */
  MDP_CSC_YuvToRgb_PreClamp_Full_Rec601,        /* Pre-clamp         */
  MDP_CSC_YuvToRgb_PostClamp_Full_Rec601,       /* Post-clamp        */
  MDP_CSC_YuvToRgb_PreBias_Full_Rec601,         /* Pre-bias          */
  MDP_CSC_YuvToRgb_PostBias_Full_Rec601,        /* Post-bias         */
  MDP_CSC10BIT_YuvToRgb_PreClamp_Full_Rec601,   /* 10 bit Pre-Clamp   */
  MDP_CSC10BIT_YuvToRgb_PostClamp_Full_Rec601,  /* 10 bit Post-Clamp  */
  MDP_CSC10BIT_YuvToRgb_PreBias_Full_Rec601,    /* 10 bit Pre-Bias    */
  MDP_CSC10BIT_YuvToRgb_PostBias_Full_Rec601,   /* 10 bit Post-Bias   */
};



/*
 * Full REC601 Standard
 *
 * Full REC601 RGB to YUV Conversion Matrix
 *    [  0.2990,          0.5870,              0.1140   ]
 *    [ -0.1687,         -0.3313,              0.5000   ]
 *    [  0.5000,         -0.4187,             -0.0813   ]
 *
 * Limited REC601 Standard in s(4.9)
 * 
 *    [  0x0099,          0x012C,              0x003A   ]
 *    [  0xFFAA,          0xFF57,              0x0100   ]
 *    [  0x0100,          0xFF2A,              0xFFD7   ]
 *
 * Matrix Value Packing
 *
 */
static uint32   MDP_CSC_RgbToYuv_Matrix_Full_Rec601[] =
{
   0x012C0099,                // Hi-WORD Co-eff R1C2 - Low-WORD Co_eff R1C1
   0xFFAA003A,                // Hi-WORD Co-eff R2C1 - Low-WORD Co_eff R1C3
   0x0100FF57,                // Hi-WORD Co-eff R2C3 - Low-WORD Co_eff R2C2
   0xFF2A0100,                // Hi-WORD Co-eff R3C2 - Low-WORD Co_eff R3C1
   0x0000FFD7                 // Hi-WORD INVALID     - Low-WORD Co_eff R3C3
};

/* Full REC601 RGB to YUV Pre-clamp Values  */
static uint16   MDP_CSC_RgbToYuv_PreClamp_Full_Rec601[]   = { 0x00FF, 0x00FF, 0x00FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC601 RGB to YUV Post-clamp Values */
static uint16   MDP_CSC_RgbToYuv_PostClamp_Full_Rec601[]  = { 0x00FF, 0x00FF, 0x00FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC601 RGB to YUV Pre-bias Values  */
static uint16   MDP_CSC_RgbToYuv_PreBias_Full_Rec601[]    = { 0x0000, 0x0000, 0x0000 };  

/* Full REC601 RGB to YUV Post-bias Values  */
static uint16   MDP_CSC_RgbToYuv_PostBias_Full_Rec601[]   = { 0x0000, 0x0080, 0x0080 }; 

/* CSC_10 values **************************************************************/
/* Full REC600 RGB to YUV Pre-clamp Values  */
static uint32 MDP_CSC10BIT_RgbToYuv_PreClamp_Full_Rec601[]    = {0x003FF,  0x003FF, 0x003FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC601 RGB to YUV Post-clamp Values */
static uint32   MDP_CSC10BIT_RgbToYuv_PostClamp_Full_Rec601[] = {0x003FF,  0x003FF, 0x003FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp
    
/* Full REC601 RGB to YUV Pre-bias Values  */
static uint16   MDP_CSC10BIT_RgbToYuv_PreBias_Full_Rec601[]   = { 0x0000, 0x0000, 0x0000 };  

/* Full REC601 RGB to YUV Post-bias Values  */
static uint16   MDP_CSC10BIT_RgbToYuv_PostBias_Full_Rec601[]       = { 0x0040, 0x0200, 0x0200 };

/** Color Space Conversion(CSC) - Full REC601 - RGB to YUV */
const HAL_MDP_CSCEncodingInfo  gMDP_CSC_RgbToYuv_Full_Rec601 =
{
  MDP_CSC_RgbToYuv_Matrix_Full_Rec601,              /* Conversion matrix */
  MDP_CSC_RgbToYuv_PreClamp_Full_Rec601,            /* Pre-clamp         */
  MDP_CSC_RgbToYuv_PostClamp_Full_Rec601,           /* Post-clamp        */
  MDP_CSC_RgbToYuv_PreBias_Full_Rec601,             /* Pre-bias          */
  MDP_CSC_RgbToYuv_PostBias_Full_Rec601,            /* Post-bias         */
  MDP_CSC10BIT_RgbToYuv_PreClamp_Full_Rec601,       /* 10 bit Pre-Clamp  */
  MDP_CSC10BIT_RgbToYuv_PostClamp_Full_Rec601,      /* 10 bit Post-Clamp */
  MDP_CSC10BIT_RgbToYuv_PreBias_Full_Rec601,        /* 10 bit Pre-Bias   */
  MDP_CSC10BIT_RgbToYuv_PostBias_Full_Rec601,       /* 10 bit Post-Bias  */
};



/*
 * Limited REC709 Standard
 *
 * Limited REC709 YUV to RGB Conversion Matrix
 *    [  1.1644,          0.0000,              1.7927   ]
 *    [  1.1644,         -0.2133,             -0.5329   ]
 *    [  1.1644,          2.1124,              0.0000   ]
 *
 * Limited REC709 Standard in s(4.9)
 *
 *    [  0x0254,          0x0000,              0x0395   ]
 *    [  0x0254,          0xFF93,              0xFEF0   ]
 *    [  0x0254,          0x0439,              0x0000   ]
 *
 * Matrix Value Packing
 *
 */
static uint32   MDP_CSC_YuvToRgb_Matrix_Limited_Rec709[] =
{
   0x00000254,                // Hi-WORD Co-eff R1C2 - Low-WORD Co_eff R1C1
   0x02540395,                // Hi-WORD Co-eff R2C1 - Low-WORD Co_eff R1C3
   0xFEF0FF93,                // Hi-WORD Co-eff R2C3 - Low-WORD Co_eff R2C2
   0x04390254,                // Hi-WORD Co-eff R3C2 - Low-WORD Co_eff R3C1
   0x00000000                 // Hi-WORD INVALID     - Low-WORD Co_eff R3C3
};

/* Limited REC709 Pre-clamp Values */
static uint16   MDP_CSC_YuvToRgb_PreClamp_Limited_Rec709[]   = { 0x10EB, 0x10F0, 0x10F0 };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC709 Post-clamp Values  */
static uint16   MDP_CSC_YuvToRgb_PostClamp_Limited_Rec709[]  = { 0x00FF, 0x00FF, 0x00FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC709 Pre-bias Values */
static uint16   MDP_CSC_YuvToRgb_PreBias_Limited_Rec709[]    = { 0xFFF0, 0xFF80, 0xFF80 }; 

/* Limited REC709 Post-bias Values  */
static uint16   MDP_CSC_YuvToRgb_PostBias_Limited_Rec709[]   = { 0x0000, 0x0000, 0x0000 }; 

/** Color Space Conversion(CSC) - Limited REC709 - YUV to RGB */
const HAL_MDP_CSCEncodingInfo  gMDP_CSC_YuvToRgb_Limited_Rec709 =
{
  MDP_CSC_YuvToRgb_Matrix_Limited_Rec709,       /* Conversion matrix */
  MDP_CSC_YuvToRgb_PreClamp_Limited_Rec709,     /* Pre-clamp         */
  MDP_CSC_YuvToRgb_PostClamp_Limited_Rec709,    /* Post-clamp        */
  MDP_CSC_YuvToRgb_PreBias_Limited_Rec709,      /* Pre-bias          */
  MDP_CSC_YuvToRgb_PostBias_Limited_Rec709,     /* Post-bias         */
  NULL,                                         /* 10 bit Pre-Clamp   */
  NULL,                                         /* 10 bit Post-Clamp  */
  NULL,                                         /* 10 bit Pre-Bias    */
  NULL,                                         /* 10 bit Post-Bias   */
};



/*
 * Limited REC709 Standard
 *
 * Limited REC709 RGB to YUV Conversion Matrix
 *    [  0.1826,          0.6142,         0.0620  ]
 *    [ -0.1007,         -0.3385,         0.4392  ]
 *    [  0.4392,         -0.3990,        -0.0402  ]
 *
 * Limited REC709 Standard in s(4.9)
 *
 *    [  0x005D,          0x013A,         0x001F  ]
 *    [  0xFFCD,          0xFF53,         0x00E0  ]
 *    [  0x00E0,          0xFF34,         0xFFEC  ]
 *
 * Matrix Value Packing
 *
 */
static uint32   MDP_CSC_RgbToYuv_Matrix_Limited_Rec709[] =
{
   0x013A005D,                // Hi-WORD Co-eff R1C2 - Low-WORD Co_eff R1C1
   0xFFCD001F,                // Hi-WORD Co-eff R2C1 - Low-WORD Co_eff R1C3
   0x00E0FF53,                // Hi-WORD Co-eff R2C3 - Low-WORD Co_eff R2C2
   0xFF3400E0,                // Hi-WORD Co-eff R3C2 - Low-WORD Co_eff R3C1
   0x0000FFEC                 // Hi-WORD INVALID     - Low-WORD Co_eff R3C3
};

/* Limited REC709 RGB to YUV Pre-clamp Values  */
static uint16   MDP_CSC_RgbToYuv_PreClamp_Limited_Rec709[]   = { 0x00FF, 0x00FF, 0x00FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC709 RGB to YUV Post-clamp Values */
static uint16   MDP_CSC_RgbToYuv_PostClamp_Limited_Rec709[]  = { 0x10EB, 0x10F0, 0x10F0 };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC709 RGB to YUV Pre-bias Values  */
static uint16   MDP_CSC_RgbToYuv_PreBias_Limited_Rec709[]    = { 0x0000, 0x0000, 0x0000 };  

/* Limited REC709 RGB to YUV Post-bias Values  */
static uint16   MDP_CSC_RgbToYuv_PostBias_Limited_Rec709[]   = { 0x0010, 0x0080, 0x0080 }; 

/* CSC_10 bit  Values************************************************************************
 */
/* Limited REC709 RGB to YUV Pre-clamp Values  */
static uint32 MDP_CSC10BIT_RgbToYuv_PreClamp_Limited_Rec709[]    = {0x000003FF,  0x000003FF, 0x000003FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Limited REC709 RGB to YUV Post-clamp Values */
static uint32   MDP_CSC10BIT_RgbToYuv_PostClamp_Limited_Rec709[] = {0x004003AC, 0x004003C0, 0x400003C0 };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp
    
/* Limited REC709 RGB to YUV Pre-bias Values  */
static uint16   MDP_CSC10BIT_RgbToYuv_PreBias_Limited_Rec709[]   = { 0x0000, 0x0000, 0x0000 };  

/* Limited REC709 RGB to YUV Post-bias Values  */
static uint16   MDP_CSC10BIT_RgbToYuv_PostBias_Limited_Rec709[]       = { 0x0040, 0x0200, 0x0200 };

/** Color Space Conversion(CSC) - Limited REC709 - RGB to YUV */
const HAL_MDP_CSCEncodingInfo  gMDP_CSC_RgbToYuv_Limited_Rec709 =
{
  MDP_CSC_RgbToYuv_Matrix_Limited_Rec709,           /* Conversion matrix */
  MDP_CSC_RgbToYuv_PreClamp_Limited_Rec709,         /* Pre-clamp         */
  MDP_CSC_RgbToYuv_PostClamp_Limited_Rec709,        /* Post-clamp        */
  MDP_CSC_RgbToYuv_PreBias_Limited_Rec709,          /* Pre-bias          */
  MDP_CSC_RgbToYuv_PostBias_Limited_Rec709,         /* Post-bias         */
  MDP_CSC10BIT_RgbToYuv_PreClamp_Limited_Rec709,    /* 10 bit Pre-Clamp  */
  MDP_CSC10BIT_RgbToYuv_PostClamp_Limited_Rec709,   /* 10 bit Post-Clamp */
  MDP_CSC10BIT_RgbToYuv_PreBias_Limited_Rec709,     /* 10 bit Pre-Bias   */
  MDP_CSC10BIT_RgbToYuv_PostBias_Limited_Rec709,    /* 10 bit Post-Bias  */
};



/*
 * Full REC709 Standard
 *
 * Full REC709 YUV to RGB Conversion Matrix
 *    [  1.0000,          0.0000,              1.5748   ]
 *    [  1.0000,         -0.1873,             -0.4681   ]
 *    [  1.0000,          1.8556,              0.0000   ]
 *
 * Full REC709 Standard in s(4.9)
 *
 *    [  0x0200,          0x0000,              0x0326   ]
 *    [  0x0200,          0xFFA1,              0xFF11   ]
 *    [  0x0200,          0x03B6,              0x0000   ]
 *
 * Matrix Value Packing
 *
 */
static uint32   MDP_CSC_YuvToRgb_Matrix_Full_Rec709[] =
{
   0x00000200,                // Hi-WORD Co-eff R1C2 - Low-WORD Co_eff R1C1
   0x02000326,                // Hi-WORD Co-eff R2C1 - Low-WORD Co_eff R1C3
   0xFF11FFA1,                // Hi-WORD Co-eff R2C3 - Low-WORD Co_eff R2C2
   0x03B60200,                // Hi-WORD Co-eff R3C2 - Low-WORD Co_eff R3C1
   0x00000000,                // Hi-WORD INVALID     - Low-WORD Co_eff R3C3
};

/* Full REC709 YUV to RGB Pre-clamp Values */
static uint16   MDP_CSC_YuvToRgb_PreClamp_Full_Rec709[]  = { 0x00FF, 0x00FF, 0x00FF }; // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC709 YUV to RGB Post-clamp Values  */
static uint16   MDP_CSC_YuvToRgb_PostClamp_Full_Rec709[] = { 0x00FF, 0x00FF, 0x00FF }; // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC709 YUV to RGB Pre-bias Values  */
static uint16   MDP_CSC_YuvToRgb_PreBias_Full_Rec709[]   = { 0x0000, 0xFF80, 0xFF80 };  

/* Full REC709 YUV to RGB Post-bias Values  */
static uint16   MDP_CSC_YuvToRgb_PostBias_Full_Rec709[]  = { 0x0000, 0x0000, 0x0000 }; 

/* CSC_10 values **************************************************************/
/* Full REC709 YUV to RGB Pre-clamp Values  */
static uint32 MDP_CSC10BIT_YuvToRgb_PreClamp_Full_Rec709[]    = {0x003FF,  0x003FF, 0x003FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC709 YUV to RGB Post-clamp Values */
static uint32   MDP_CSC10BIT_YuvToRgb_PostClamp_Full_Rec709[] = {0x003FF,  0x003FF, 0x003FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp
    
/* Full REC709 YUV to RGB Pre-bias Values  */
static uint16   MDP_CSC10BIT_YuvToRgb_PreBias_Full_Rec709[]   = { 0x0000, 0xFE00, 0xFE00 };

/* Full REC709 YUV to RGB Post-bias Values  */
static uint16   MDP_CSC10BIT_YuvToRgb_PostBias_Full_Rec709[]  = { 0x0000, 0x0000, 0x0000 };

/** Color Space Conversion(CSC) - Full REC709 - YUV to RGB */
const HAL_MDP_CSCEncodingInfo  gMDP_CSC_YuvToRgb_Full_Rec709 =
{
  MDP_CSC_YuvToRgb_Matrix_Full_Rec709,          /* Conversion matrix */
  MDP_CSC_YuvToRgb_PreClamp_Full_Rec709,        /* Pre-clamp         */
  MDP_CSC_YuvToRgb_PostClamp_Full_Rec709,       /* Post-clamp        */
  MDP_CSC_YuvToRgb_PreBias_Full_Rec709,         /* Pre-bias          */
  MDP_CSC_YuvToRgb_PostBias_Full_Rec709,        /* Post-bias         */
  MDP_CSC10BIT_YuvToRgb_PreClamp_Full_Rec709,   /* 10 bit Pre-Clamp   */
  MDP_CSC10BIT_YuvToRgb_PostClamp_Full_Rec709,  /* 10 bit Post-Clamp  */
  MDP_CSC10BIT_YuvToRgb_PreBias_Full_Rec709,    /* 10 bit Pre-Bias    */
  MDP_CSC10BIT_YuvToRgb_PostBias_Full_Rec709,   /* 10 bit Post-Bias   */
};



/*
 * Full REC709 Standard
 *
 * Full REC709 RGB to YUV Conversion Matrix
 *    [  0.2126,          0.7152,              0.0722   ]
 *    [ -0.1146,         -0.3854,              0.5000   ]
 *    [  0.5000,         -0.4542,              0.0458   ]
 *
 * Limited REC709 Standard in s(4.9)
 * 
 *    [  0x006C,          0x016E,              0x0024   ]
 *    [  0xFFC6,          0xFF3B,              0x0100   ]
 *    [  0x0100,          0xFF18,              0xFFE9   ]
 *
 * Matrix Value Packing
 *
 */
static uint32   MDP_CSC_RgbToYuv_Matrix_Full_Rec709[] =
{
   0x016E006C,                // Hi-WORD Co-eff R1C2 - Low-WORD Co_eff R1C1
   0xFFC60024,                // Hi-WORD Co-eff R2C1 - Low-WORD Co_eff R1C3
   0x0100FF3B,                // Hi-WORD Co-eff R2C3 - Low-WORD Co_eff R2C2
   0xFF180100,                // Hi-WORD Co-eff R3C2 - Low-WORD Co_eff R3C1
   0x0000FFE9                 // Hi-WORD INVALID     - Low-WORD Co_eff R3C3
};

/* Full REC709 RGB to YUV Pre-clamp Values  */
static uint16   MDP_CSC_RgbToYuv_PreClamp_Full_Rec709[]   = { 0x00FF, 0x00FF, 0x00FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC709 RGB to YUV Post-clamp Values */
static uint16   MDP_CSC_RgbToYuv_PostClamp_Full_Rec709[]  = { 0x00FF, 0x00FF, 0x00FF };  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC709 RGB to YUV Pre-bias Values  */
static uint16   MDP_CSC_RgbToYuv_PreBias_Full_Rec709[]    = { 0x0000, 0x0000, 0x0000 };  

/* Full REC709 RGB to YUV Post-bias Values  */
static uint16   MDP_CSC_RgbToYuv_PostBias_Full_Rec709[]   = { 0x0000, 0x0080, 0x0080 }; 

/* CSC_10 bit  Values************************************************************************
 */
/* Full REC709 RGB to YUV Pre-clamp Values  */
static uint32 MDP_CSC10BIT_RgbToYuv_PreClamp_Full_Rec709[]    = {0x000003FF,  0x000003FF, 0x000003FF};  // Hi-Byte LOW clamp - Low-Byte HIGH clamp

/* Full REC709 RGB to YUV Post-clamp Values */
static uint32   MDP_CSC10BIT_RgbToYuv_PostClamp_Full_Rec709[] = {0x000003FF,  0x000003FF, 0x000003FF};  // Hi-Byte LOW clamp - Low-Byte HIGH clamp
    
/* Full REC709 RGB to YUV Pre-bias Values  */
static uint16   MDP_CSC10BIT_RgbToYuv_PreBias_Full_Rec709[]    = {0x0000, 0x0000, 0x0000};

/* Full REC709 RGB to YUV Post-bias Values  */
static uint16   MDP_CSC10BIT_RgbToYuv_PostBias_Full_Rec709[]   = {0x0040, 0x0200, 0x0200};

/** Color Space Conversion(CSC) - Full REC709 - RGB to YUV */
const HAL_MDP_CSCEncodingInfo  gMDP_CSC_RgbToYuv_Full_Rec709 =
{
  MDP_CSC_RgbToYuv_Matrix_Full_Rec709,          /* Conversion matrix */
  MDP_CSC_RgbToYuv_PreClamp_Full_Rec709,        /* Pre-clamp         */
  MDP_CSC_RgbToYuv_PostClamp_Full_Rec709,       /* Post-clamp        */
  MDP_CSC_RgbToYuv_PreBias_Full_Rec709,         /* Pre-bias          */
  MDP_CSC_RgbToYuv_PostBias_Full_Rec709,        /* Post-bias         */
  MDP_CSC10BIT_RgbToYuv_PreClamp_Full_Rec709,   /* 10 bit Pre-Clamp  */
  MDP_CSC10BIT_RgbToYuv_PostClamp_Full_Rec709,  /* 10 bit Post-Clamp */
  MDP_CSC10BIT_RgbToYuv_PreBias_Full_Rec709,    /* 10 bit Pre-Bias   */
  MDP_CSC10BIT_RgbToYuv_PostBias_Full_Rec709,   /* 10 bit Post-Bias  */
};

/** Mapping table to find a virtual smart DMA pipe ID*/
const HAL_MDP_SourcePipeId gMultiRectMappingTable[HAL_MDP_SOURCE_PIPE_MAX] = 
{
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_NONE            = 0x00000000,

  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_VIG_0           = HAL_MDP_SOURCE_PIPE_VIG_BASE,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_VIG_1,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_VIG_2,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_VIG_3,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_VIG_4,

  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_RGB_0           = HAL_MDP_SOURCE_PIPE_RGB_BASE,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_RGB_1,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_RGB_2,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_RGB_3,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_RGB_4,

  HAL_MDP_SOURCE_PIPE_DMA_0_REC1, // HAL_MDP_SOURCE_PIPE_DMA_0           = HAL_MDP_SOURCE_PIPE_DMA_BASE,
  HAL_MDP_SOURCE_PIPE_DMA_1_REC1, // HAL_MDP_SOURCE_PIPE_DMA_1,
  HAL_MDP_SOURCE_PIPE_DMA_2_REC1, // HAL_MDP_SOURCE_PIPE_DMA_2,
  HAL_MDP_SOURCE_PIPE_DMA_3_REC1, // HAL_MDP_SOURCE_PIPE_DMA_3,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_DMA_4,

  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_CURSOR_0        = HAL_MDP_SOURCE_PIPE_CURSOR_BASE,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_CURSOR_1,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_CURSOR_2,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_CURSOR_3,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_CURSOR_4,

  HAL_MDP_SOURCE_PIPE_DMA_0,      // HAL_MDP_SOURCE_PIPE_DMA_0_REC1      = HAL_MDP_SOURCE_PIPE_SMART_DMA_BASE,
  HAL_MDP_SOURCE_PIPE_DMA_1,      // HAL_MDP_SOURCE_PIPE_DMA_1_REC1,
  HAL_MDP_SOURCE_PIPE_DMA_2,      // HAL_MDP_SOURCE_PIPE_DMA_2_REC1,
  HAL_MDP_SOURCE_PIPE_DMA_3,      // HAL_MDP_SOURCE_PIPE_DMA_3_REC1,
  HAL_MDP_SOURCE_PIPE_NONE,       // HAL_MDP_SOURCE_PIPE_DMA_4_REC1,
};

/* QoS priority output for write back paths; */
uint32 gMDP_Qos_WB_Paths_Priority[]    = {HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,
                                          HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,
                                          HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,
                                          HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,
                                          HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME };

/* HW block register offsets */
uintPtr  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_MAX];                   // Control path register offsets
uintPtr  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_MAX];                         // Data path register offsets
uintPtr  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_MAX];                      // Destination (DSPP) register offsets 
uintPtr  uMDPAADCRegBaseOffset[HAL_MDP_DESTINATION_PIPE_MAX];                      // AD registers offsets
uintPtr  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];            // Physical interface register offsets
uintPtr  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_MAX];                     // Layer mixer register offsets
uintPtr  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_MAX];           // Layer mixer blending state register offsets
uintPtr  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_MAX];                          // PingPong register offsets
uintPtr  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_MAX];                    // PingPong Buffer (PPB) register offsets
uintPtr  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX];                           // Source (SSPP) register offsets
uintPtr  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX];                          // Source (SSPP) Layers (Scaler) register offsets
uintPtr  uMDPSSPPPixExtRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX_COLOR_COMPONENTS];    // SSPP Pixel Ext Component Offset
uintPtr  uMDPInterfaceVideoMISRCTRLRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];  // Physical interface video misr ctrl register offset
uintPtr  uMDPInterfaceVideoMISRSignatureOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];    // Physical interface video misr signature register offset
uintPtr  uMDPInterfaceCMDMISRCTRLRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];    // Physical interface CMD misr ctrl register offset
uintPtr  uMDPInterfaceCMDMISRSignatureOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];      // Physical interface CMD misr signature register offset
uintPtr  uMDPIntfMisrStreamRegBaseOffset[HAL_MDP_INTERFACE_STREAM_MAX];            // Interface stream-based MISR configuration register offsets
uintPtr  uMDPSplitMUXRegBaseOffset[HAL_MDP_SPLITMUX_MAX];                          // PingPong Buffer (PPB) register offsets
uintPtr  uMDPDSCRegBaseOffset[HAL_MDP_DSC_MAX];                                    // DSC register offset
uintPtr  uMDPDestScalerRegBaseOffset[HAL_MDP_DESTINATION_PIPE_MAX];                // Destination scaler register offsets 
uintPtr  uMDPVbifRegBaseOffset[HAL_MDP_VBIF_MAX];                                  // VBIF register offsets


/****************************************************************************
*
** FUNCTION: HAL_MDP_ReadHardwareInfo()
*/
/*!
* \brief
*     Reads the hardware capabilities for the given MDP Version
*
* \param [out]  psVersionInfo       - Version information
* \param [out]  psHwInfo            - Hardware information
* \param [out]  psMdpHwInfo         - MDP hardware information 
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_ReadHardwareInfo(HAL_HW_VersionType          *psVersionInfo,
                                     HAL_MDP_HwInfo              *psHwInfo,
                                     HAL_MDP_Hw_Private_Info     *psMdpHwInfo)
{
   // Use MDSS_MDP V4.0.0 as the base info
   HAL_MDP_ReadHardwareInfo_4_0_0(psHwInfo, psMdpHwInfo);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SourcePipe_Interrupt_Config()
*/
/*!
* \brief
*     Enables or disables the interrupt
*
* \param [in]   psInterruptConfig     - Configuration for each module.
* \param [in]   pHistIntrReg          - Histogram interupt register
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SourcePipe_Interrupt_Config(HAL_MDP_InterruptConfigType  *psInterruptConfig,
                                                uint32                       *pHistIntrReg)
{
   bool32         bEnable  = FALSE;

   switch(psInterruptConfig->eModuleId.eSourcePipeId)
   {
   case HAL_MDP_SOURCE_PIPE_VIG_0:
      {
         // VIG_0_HISTOGRAM_RESET_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_RESET_DONE)?(TRUE):(FALSE));
         *pHistIntrReg = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, VIG_0_RESET_SEQ_DONE, bEnable);

         // VIG_0_HISTOGRAM_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, VIG_0_DONE, bEnable);
      }
      break;

   case HAL_MDP_SOURCE_PIPE_VIG_1:
      {
         // VIG_1_HISTOGRAM_RESET_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_RESET_DONE)?(TRUE):(FALSE));
         *pHistIntrReg = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, VIG_1_RESET_SEQ_DONE, bEnable);

         // VIG_1_HISTOGRAM_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, VIG_1_DONE, bEnable);
      }
      break;

   case HAL_MDP_SOURCE_PIPE_VIG_2:
      {
         // VIG_2_HISTOGRAM_RESET_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_RESET_DONE)?(TRUE):(FALSE));
         *pHistIntrReg = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, VIG_2_RESET_SEQ_DONE, bEnable);

         // VIG_2_HISTOGRAM_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, VIG_2_DONE, bEnable);
      }
      break;

   case HAL_MDP_SOURCE_PIPE_VIG_3:
      {
         // VIG_3_HISTOGRAM_RESET_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_RESET_DONE)?(TRUE):(FALSE));
         *pHistIntrReg = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, VIG_3_RESET_SEQ_DONE, bEnable);

         // VIG_3_HISTOGRAM_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, VIG_3_DONE, bEnable);
      }
      break;

   default:
      break;
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SourcePipe_Interrupt_GetStatus()
*/
/*!
* \brief
*     Returns the source pipe interrupt status
*
* \param [out]  psInterruptStatus     - Interrupt received Status
* \param [in]   uMDPHistIntrStatus    - MDP histogram interrupt status
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_SourcePipe_Interrupt_GetStatus(HAL_MDP_InterruptStatusType  *psInterruptStatus,
                                                   uint32                        uMDPHistIntrStatus)
{
   uint32         uIntrStatusBitFields    = 0x00000000;

   switch(psInterruptStatus->sIntrConfig.eModuleId.eSourcePipeId)
   {
   case HAL_MDP_SOURCE_PIPE_VIG_0:

      // VIG_0_HISTOGRAM_RESET_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_VIG_0_RESET_SEQ_DONE_BMSK)
      {
          uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_RESET_DONE, TRUE);
      }
      // VIG_0_HISTOGRAM_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_VIG_0_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_DONE, TRUE);
      }
      break;

   case HAL_MDP_SOURCE_PIPE_VIG_1:

      // VIG_1_HISTOGRAM_RESET_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_VIG_1_RESET_SEQ_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_RESET_DONE, TRUE);
      }
      // VIG_1_HISTOGRAM_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_VIG_1_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_DONE, TRUE);
      }
      break;

   case HAL_MDP_SOURCE_PIPE_VIG_2:

      // VIG_2_HISTOGRAM_RESET_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_VIG_2_RESET_SEQ_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_RESET_DONE, TRUE);
      }
      // VIG_2_HISTOGRAM_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_VIG_2_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_DONE, TRUE);
      }
      break;

   case HAL_MDP_SOURCE_PIPE_VIG_3:

      // VIG_3_HISTOGRAM_RESET_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_VIG_3_RESET_SEQ_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_RESET_DONE, TRUE);
      }
      // VIG_3_HISTOGRAM_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_VIG_3_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_DONE, TRUE);
      }
      break;

   default:
      break;
   }
   psInterruptStatus->eInterruptStatus = (HAL_MDP_InterruptType)(uIntrStatusBitFields & psInterruptStatus->sIntrConfig.eInterruptSrc);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DestinationPipe_Interrupt_Config()
*/
/*!
* \brief
*     Enable/disable interrupt
*
* \param [in]   psInterruptConfig     - Configuration for each module.
* \param [in]   pHistIntrReg          - Histogram interrupt register
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DestinationPipe_Interrupt_Config(HAL_MDP_InterruptConfigType  *psInterruptConfig,
                                                     uint32                       *pHistIntrReg)
{
   bool32      bEnable = FALSE;

   switch(psInterruptConfig->eModuleId.eDestinationPipeId)
   {
   case HAL_MDP_DESTINATION_PIPE_0:
      {
         // DSPP_0_HISTOGRAM_RESET_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_RESET_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, DSPP_0_RESET_SEQ_DONE, bEnable);

         // DSPP_0_HISTOGRAM_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, DSPP_0_DONE, bEnable);
      }
      break;

   case HAL_MDP_DESTINATION_PIPE_1:
      {
         // DSPP_1_HISTOGRAM_RESET_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_RESET_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, DSPP_1_RESET_SEQ_DONE, bEnable);

         // DSPP_1_HISTOGRAM_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, DSPP_1_DONE, bEnable);
      }
      break;

   case HAL_MDP_DESTINATION_PIPE_2:
      {
         // DSPP_2_HISTOGRAM_RESET_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_RESET_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, DSPP_2_RESET_SEQ_DONE, bEnable);

         // DSPP_2_HISTOGRAM_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, DSPP_2_DONE, bEnable);
      }
      break;

   case HAL_MDP_DESTINATION_PIPE_3:
      {
         // DSPP_3_HISTOGRAM_RESET_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_RESET_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, DSPP_3_RESET_SEQ_DONE, bEnable);

         // DSPP_3_HISTOGRAM_DONE
         bEnable        = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_HIST_DONE)?(TRUE):(FALSE));
         *pHistIntrReg  = HWIO_OUT_FLD(*pHistIntrReg, MDP_HIST_INTR_EN, DSPP_3_DONE, bEnable);
      }
      break;

   default:
      break;
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DestinationPipe_Interrupt_GetStatus()
*/
/*!
* \brief
*     Returns the destination pipe interrupt status
*
* \param [out]  psInterruptStatus     - Interrupt received Status
* \param [in]   uMDPHistIntrStatus    - MDP histogram interrupt status
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_DestinationPipe_Interrupt_GetStatus(HAL_MDP_InterruptStatusType  *psInterruptStatus,
                                                        uint32                        uMDPHistIntrStatus)
{
   uint32         uIntrStatusBitFields    = 0x00000000;

   switch(psInterruptStatus->sIntrConfig.eModuleId.eDestinationPipeId)
   {
   case HAL_MDP_DESTINATION_PIPE_0:

      // DSPP_0_HISTOGRAM_RESET_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_DSPP_0_RESET_SEQ_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_RESET_DONE, 0x01);
      }
      // DSPP_0_HISTOGRAM_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_DSPP_0_DONE_BMSK )
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_DONE, 0x01);
      }
      break;

   case HAL_MDP_DESTINATION_PIPE_1:

      // DSPP_1_HISTOGRAM_RESET_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_DSPP_1_RESET_SEQ_DONE_BMSK )
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_RESET_DONE, 0x01);
      }
      // DSPP_1_HISTOGRAM_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_DSPP_1_DONE_BMSK )
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_DONE, 0x01);
      }
      break;

   case HAL_MDP_DESTINATION_PIPE_2:

      // DSPP_2_HISTOGRAM_RESET_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_DSPP_2_RESET_SEQ_DONE_BMSK )
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_RESET_DONE, 0x01);
      }
      // DSPP_2_HISTOGRAM_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_DSPP_2_DONE_BMSK )
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_DONE, 0x01);
      }
      break;

   case HAL_MDP_DESTINATION_PIPE_3:

      // DSPP_3_HISTOGRAM_RESET_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_DSPP_3_RESET_SEQ_DONE_BMSK )
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_RESET_DONE, 0x01);
      }
      // DSPP_3_HISTOGRAM_DONE
      if(uMDPHistIntrStatus & HWIO_MDP_HIST_INTR_EN_DSPP_3_DONE_BMSK )
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_HIST_DONE, 0x01);
      }
      break;

   default:
      break;
   }
   psInterruptStatus->eInterruptStatus = (HAL_MDP_InterruptType)(uIntrStatusBitFields & psInterruptStatus->sIntrConfig.eInterruptSrc);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PhysicalInterface_Interrupt_Config()
*/
/*!
* \brief
*     Enable/disable interrupt
*
* \param [in]   psInterruptConfig     - Configuration for each module.
* \param [in]   ePhyInterfaceId       - Physical interface id
* \param [out]  pIntrReg              - Pointer to interrupt register
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PhysicalInterface_Interrupt_Config(HAL_MDP_InterruptConfigType  *psInterruptConfig,
                                                       HAL_MDP_PhyInterfaceId       ePhyInterfaceId,
                                                       uint32                       *pIntrReg)
{
   bool32      bEnable = FALSE;

   switch(ePhyInterfaceId)
   {
   case HAL_MDP_PHYSICAL_INTERFACE_0:
      {
         // UNDERFLOW
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_UNDERRUN)?(TRUE):(FALSE));
         *pIntrReg   = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, INTF_0_UNDERRUN, bEnable);

         // VSYNC
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_VSYNC)?(TRUE):(FALSE));
         *pIntrReg   = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, INTF_0_VSYNC, bEnable);
      }
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_1:
      {
         // UNDERFLOW
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_UNDERRUN)?(TRUE):(FALSE));
         *pIntrReg   = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, INTF_1_UNDERRUN, bEnable);

         // VSYNC
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_VSYNC)?(TRUE):(FALSE));
         *pIntrReg   = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, INTF_1_VSYNC, bEnable);
      }
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_2:
      {
         // UNDERFLOW
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_UNDERRUN)?(TRUE):(FALSE));
         *pIntrReg   = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, INTF_2_UNDERRUN, bEnable);

         // VSYNC
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_VSYNC)?(TRUE):(FALSE));
         *pIntrReg   = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, INTF_2_VSYNC, bEnable);
      }
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_3:
      {
         // UNDERFLOW
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_UNDERRUN)?(TRUE):(FALSE));
         *pIntrReg   = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, INTF_3_UNDERRUN, bEnable);

         // VSYNC
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_VSYNC)?(TRUE):(FALSE));
         *pIntrReg   = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, INTF_3_VSYNC, bEnable);
      }
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_4:
   case HAL_MDP_PHYSICAL_INTERFACE_5:
   case HAL_MDP_PHYSICAL_INTERFACE_ID_NONE:
   case HAL_MDP_PHYSICAL_INTERFACE_MAX:
   case HAL_MDP_PHYSICAL_INTERFACE_FORCE_32BIT:
   default:
      break;
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PhysicalInterface_Interrupt_GetStatus()
*/
/*!
* \brief
*     Retrieves the interrupt status
*
* \param [out]  psInterruptStatus     - Interrupt received Status
* \param [in]   ePhyInterfaceId       - Physical interface id
* \param [in]   uMDPIntrStatus        - MDP interrupt status
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PhysicalInterface_Interrupt_GetStatus(HAL_MDP_InterruptStatusType  *psInterruptStatus,
                                                          HAL_MDP_PhyInterfaceId       ePhyInterfaceId,
                                                          uint32                       uMDPIntrStatus)
{
   uint32   uIntrStatusBitFields = 0x00000000;

   switch(ePhyInterfaceId)
   {
   case HAL_MDP_PHYSICAL_INTERFACE_0:

      // UNDERFLOW
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_INTF_0_UNDERRUN_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_UNDERRUN, 0x01);
      }
      // VSYNC
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_INTF_0_VSYNC_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_VSYNC, 0x01);
      }
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_1:

      // UNDERFLOW
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_INTF_1_UNDERRUN_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_UNDERRUN, 0x01);
      }
      // VSYNC
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_INTF_1_VSYNC_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_VSYNC, 0x01);
      }
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_2:

      // UNDERFLOW
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_INTF_2_UNDERRUN_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_UNDERRUN, 0x01);
      }
      // VSYNC
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_INTF_2_VSYNC_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_VSYNC, 0x01);
      }
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_3:

      // UNDERFLOW
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_INTF_3_UNDERRUN_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_UNDERRUN, 0x01);
      }
      // VSYNC
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_INTF_3_VSYNC_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_VSYNC, 0x01);
      }
      break;

   case HAL_MDP_PHYSICAL_INTERFACE_4:
   case HAL_MDP_PHYSICAL_INTERFACE_5:
   case HAL_MDP_PHYSICAL_INTERFACE_ID_NONE:
   case HAL_MDP_PHYSICAL_INTERFACE_MAX:
   case HAL_MDP_PHYSICAL_INTERFACE_FORCE_32BIT:
   default:
      break;
   }
   psInterruptStatus->eInterruptStatus = (HAL_MDP_InterruptType)(uIntrStatusBitFields & psInterruptStatus->sIntrConfig.eInterruptSrc);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_WritebackInterface_Interrupt_Config()
*/
/*!
* \brief
*     Enable/disable interrupt
*
* \param [in]   psInterruptConfig      - Configuration for each module.
* \param [in]   eWriteBackInterfaceId  - Write back interface id
* \param [in]   pIntrReg1              - MDP (WB) interrupt register
* \param [in]   pIntrReg2              - Rotator (WB) interrupt register
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_WritebackInterface_Interrupt_Config(HAL_MDP_InterruptConfigType  *psInterruptConfig,
                                                        HAL_MDP_WritebackInterfaceId eWriteBackInterfaceId,
                                                        uint32                       *pIntrReg1,
														uint32                       *pIntrReg2)
{
   bool32      bEnable = FALSE;

   switch(eWriteBackInterfaceId)
   {
   case HAL_MDP_WRITEBACK_INTERFACE_0:
      {
         // DATA TRANSFER DONE
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_DATA_TRANSFER_DONE)?(TRUE):(FALSE));
		 *pIntrReg2  = HWIO_OUT_FLD(*pIntrReg2, ROT_ROTTOP_INTR_EN, ROT_DONE_EN, bEnable);
      }
      break;

   case HAL_MDP_WRITEBACK_INTERFACE_1:
      {
         // DATA TRANSFER DONE
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_DATA_TRANSFER_DONE)?(TRUE):(FALSE));
		 *pIntrReg1  = HWIO_OUT_FLD(*pIntrReg1, MDP_INTR_EN, WB_1_DONE, bEnable);
      }
      break;

   case HAL_MDP_WRITEBACK_INTERFACE_2:
      {
         // DATA TRANSFER DONE
         bEnable     = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_DATA_TRANSFER_DONE)?(TRUE):(FALSE));
		 *pIntrReg1  = HWIO_OUT_FLD(*pIntrReg1, MDP_INTR_EN, WB_2_DONE, bEnable);
      }
      break;

   case HAL_MDP_WRITEBACK_INTERFACE_3:
   case HAL_MDP_WRITEBACK_INTERFACE_4:
   case HAL_MDP_WRITEBACK_INTERFACE_5:
   case HAL_MDP_WRITEBACK_INTERFACE_ID_NONE:
   case HAL_MDP_WRITEBACK_INTERFACE_MAX:
   case HAL_MDP_WRITEBACK_INTERFACE_FORCE_32BIT:
   default:
      break;
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_WritebackInterface_Interrupt_GetStatus()
*/
/*!
* \brief
*     Retrieves the interrupt status
*
* \param [out]  psInterruptStatus      - Interrupt received Status
* \param [in]   eWriteBackInterfaceId  - Write back interface id
* \param [in]   uMDPIntrStatus         - MDP interrupt status
* \param [in]   uRotatorIntrStatus     - Rotator interrupt status
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_WritebackInterface_Interrupt_GetStatus(HAL_MDP_InterruptStatusType  *psInterruptStatus,
                                                           HAL_MDP_WritebackInterfaceId eWriteBackInterfaceId,
                                                           uint32                       uMDPIntrStatus,
                                                           uint32                       uRotatorIntrStatus)
{
   uint32   uIntrStatusBitFields = 0x00000000;

   switch(eWriteBackInterfaceId)
   {
   case HAL_MDP_WRITEBACK_INTERFACE_0:

      // DATA TRANSFER DONE
      if (uRotatorIntrStatus & HWIO_MDP_INTR_EN_WB_0_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_DATA_TRANSFER_DONE, 0x01);
      }
      break;

   case HAL_MDP_WRITEBACK_INTERFACE_1:

      // DATA TRANSFER DONE
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_WB_1_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_DATA_TRANSFER_DONE, 0x01);
      }
      break;

   case HAL_MDP_WRITEBACK_INTERFACE_2:

      // DATA TRANSFER DONE
      if(uMDPIntrStatus & HWIO_MDP_INTR_EN_WB_2_DONE_BMSK)
      {
         uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_DATA_TRANSFER_DONE, 0x01);
      }
      break;

   case HAL_MDP_WRITEBACK_INTERFACE_3:
   case HAL_MDP_WRITEBACK_INTERFACE_4:
   case HAL_MDP_WRITEBACK_INTERFACE_5:
   case HAL_MDP_WRITEBACK_INTERFACE_ID_NONE:
   case HAL_MDP_WRITEBACK_INTERFACE_MAX:
   case HAL_MDP_WRITEBACK_INTERFACE_FORCE_32BIT:
   default:
      break;
   }
   psInterruptStatus->eInterruptStatus = (HAL_MDP_InterruptType)(uIntrStatusBitFields & psInterruptStatus->sIntrConfig.eInterruptSrc);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_Interrupt_Config()
*/
/*!
* \brief
*     Enable/disable interrupts for pingpong blocks
*
* \param [in]   psInterruptConfig      - Configuration for each module.
* \param [in]   ePingPongId            - pingpong block id
* \param [in]   pIntrReg               - Interrupt register
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_Interrupt_Config(HAL_MDP_InterruptConfigType  *psInterruptConfig,
                                              HAL_MDP_PingPongId           ePingPongId,
                                              uint32                       *pIntrReg)
{
   bool32             bEnable = FALSE;

   switch(ePingPongId)
   {
   case HAL_MDP_PINGPONG_0:
      {
         // PINGPONG_RD
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_RD_PTR)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_0_RD_PTR, bEnable);

         // PINGPONG_WR
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_WR_PTR)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_0_WR_PTR, bEnable);

         // PINGPONG_DONE
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_DONE)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_0_DONE, bEnable);
      }
      break;

    case HAL_MDP_PINGPONG_1:
      {
         // PINGPONG_RD
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_RD_PTR)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_1_RD_PTR, bEnable);

         // PINGPONG_WR
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_WR_PTR)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_1_WR_PTR, bEnable);

         // PINGPONG_DONE
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_DONE)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_1_DONE, bEnable);
      }
      break;

   case HAL_MDP_PINGPONG_2:
      {
         // PINGPONG_RD
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_RD_PTR)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_2_RD_PTR, bEnable);

         // PINGPONG_WR
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_WR_PTR)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_2_WR_PTR, bEnable);

         // PINGPONG_DONE
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_DONE)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_2_DONE, bEnable);
      }
      break;

   case HAL_MDP_PINGPONG_3:
      {
         // PINGPONG_RD
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_RD_PTR)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_3_RD_PTR, bEnable);

         // PINGPONG_WR
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_WR_PTR)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_3_WR_PTR, bEnable);

         // PINGPONG_DONE
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_PINGPONG_DONE)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, PING_PONG_3_DONE, bEnable);
      }
      break;

   default:
      break;
  }
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_Interrupt_GetStatus()
*/
/*!
* \brief
*     Returns the pingpong interrupt status
*
* \param [out]  psInterruptStatus     - Interrupt received Status
* \param [in]   ePingPongId           - pingpong block id
* \param [in]   uMDPIntrStatus        - MDP interrupt status
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_PingPong_Interrupt_GetStatus(HAL_MDP_InterruptStatusType   *psInterruptStatus,
                                                 HAL_MDP_PingPongId            ePingPongId,
                                                 uint32                        uMDPIntrStatus)
{
   uint32         uIntrStatusBitFields    = 0x00000000;

   switch(ePingPongId)
   {
   case HAL_MDP_PINGPONG_0:
      {
         // PINGPONG_RD
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_0_RD_PTR_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_RD_PTR, 1);
         }

         // PINGPONG_WR
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_0_WR_PTR_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_WR_PTR, 1);
         }

         // PINGPONG_DONE
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_0_DONE_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_DONE, 1);
         }
      }
      break;

   case HAL_MDP_PINGPONG_1:
      {
         // PINGPONG_RD
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_1_RD_PTR_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_RD_PTR, 1);
         }

         // PINGPONG_WR
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_1_WR_PTR_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_WR_PTR, 1);
         }

         // PINGPONG_DONE
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_1_DONE_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_DONE, 1);
         }
      }
      break;

   case HAL_MDP_PINGPONG_2:
      {
         // PINGPONG_RD
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_2_RD_PTR_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_RD_PTR, 1);
         }

         // PINGPONG_WR
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_2_WR_PTR_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_WR_PTR, 1);
         }

         // PINGPONG_DONE
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_2_DONE_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_DONE, 1);
         }
      }
      break;

   case HAL_MDP_PINGPONG_3:
      {
         // PINGPONG_RD
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_3_RD_PTR_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_RD_PTR, 1);
         }

         // PINGPONG_WR
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_3_WR_PTR_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_WR_PTR, 1);
         }

         // PINGPONG_DONE
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_PING_PONG_3_DONE_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_PINGPONG_DONE, 1);
         }
      }
      break;

   default:
      break;
   }
   psInterruptStatus->eInterruptStatus = (HAL_MDP_InterruptType)(uIntrStatusBitFields & psInterruptStatus->sIntrConfig.eInterruptSrc);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_WatchDog_Interrupt_Config()
*/
/*!
* \brief
*     Enable/disable interrupts for watch dog timer
*
* \param [in]   psInterruptConfig      - Configuration for each module.
* \param [in]   eWatchDogId            - Watch dog id
* \param [in]   pIntrReg               - Interrupt register
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_WatchDog_Interrupt_Config(HAL_MDP_InterruptConfigType  *psInterruptConfig,
                                              HAL_MDP_WatchDogId           eWatchDogId,
                                              uint32                       *pIntrReg)
{
   bool32             bEnable = FALSE;

   switch(eWatchDogId)
   {
   case HAL_MDP_WATCHDOG_0:
      {
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_WATCHDOG_DONE)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, WD_TIMER_0_DONE, bEnable);
      }
      break;

    case HAL_MDP_WATCHDOG_1:
      {
         bEnable = ((psInterruptConfig->eInterruptSrc & HAL_MDP_INTERRUPT_WATCHDOG_DONE)? (1) : (0));
         *pIntrReg = HWIO_OUT_FLD(*pIntrReg, MDP_INTR_EN, WD_TIMER_1_DONE, bEnable);
      }
      break;

   default:
      break;
  }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_WatchDog_Interrupt_GetStatus()
*/
/*!
* \brief
*     Returns the watch dog timer interrupt status
*
* \param [out]  psInterruptStatus     - Interrupt received Status
* \param [in]   eWatchDogId           - Watch dog timer id
* \param [in]   uMDPIntrStatus        - MDP interrupt status
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_WatchDog_Interrupt_GetStatus(HAL_MDP_InterruptStatusType   *psInterruptStatus,
                                                 HAL_MDP_WatchDogId            eWatchDogId,
                                                 uint32                        uMDPIntrStatus)
{
   uint32         uIntrStatusBitFields    = 0x00000000;

   switch(eWatchDogId)
   {
   case HAL_MDP_WATCHDOG_0:
      {
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_WD_TIMER_0_DONE_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_WATCHDOG_DONE, 1);
         }
      }
      break;

   case HAL_MDP_WATCHDOG_1:
      {
         if(uMDPIntrStatus & HWIO_MDP_INTR_EN_WD_TIMER_1_DONE_BMSK)
         {
            uIntrStatusBitFields = HAL_MDSS_ENABLE_REG_FIELD(uIntrStatusBitFields, HAL_MDP_INTERRUPT_WATCHDOG_DONE, 1);
         }
      }
      break;

   default:
      break;
   }
   psInterruptStatus->eInterruptStatus = (HAL_MDP_InterruptType)(uIntrStatusBitFields & psInterruptStatus->sIntrConfig.eInterruptSrc);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_Bit_Configuration()
*/
/*!
* \brief
*     Enable/disable interrupt
*
* \param [in]   psInterruptConfig      - Configuration for each module.
* \param [in]   puMDPIntrBits          - MDP interrupt bits
* \param [in]   puMDPHistIntrBits      - MDP histogram interrupt bits
* \param [in]   puMDPRotatorIntrBits   - MDP Rotator interrupt bits
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_Interrupt_Bit_Configuration(HAL_MDP_InterruptConfigType  *psInterruptConfig,
                                                uint32                       *puMDPIntrBits,
                                                uint32                       *puMDPHistIntrBits,
												uint32                       *puMDPRotatorIntrBits)
{
   HAL_MDP_InterruptConfigType  *psInterrupts      = psInterruptConfig;
   HAL_MDP_WritebackInterfaceId  eWriteBackIntfId  = HAL_MDP_WRITEBACK_INTERFACE_ID_NONE;

   switch(psInterrupts->eModuleType)
   {
   case HAL_MDP_MODULE_SOURCE_PIPE:
      {
         HAL_MDP_SourcePipe_Interrupt_Config(psInterrupts,
                                             puMDPHistIntrBits);
      }
      break;

   case HAL_MDP_MODULE_DESTINATION_PIPE:
      {
         HAL_MDP_DestinationPipe_Interrupt_Config(psInterrupts,
                                                  puMDPHistIntrBits);
      }
      break;

   case HAL_MDP_MODULE_INTERFACE:
      {
         if(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > psInterrupts->eModuleId.eInterfaceId) // Physical interface
         {
            HAL_MDP_PhysicalInterface_Interrupt_Config(psInterrupts,
                                                       (HAL_MDP_PhyInterfaceId)psInterrupts->eModuleId.eInterfaceId,
                                                       puMDPIntrBits);
         }
         else // write-back interface
         {
            if((HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_0 == psInterrupts->eModuleId.eInterfaceId) ||
               (HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_0 == psInterrupts->eModuleId.eInterfaceId))
            {
               eWriteBackIntfId = HAL_MDP_WRITEBACK_INTERFACE_0;
            }
            else if((HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_1 == psInterrupts->eModuleId.eInterfaceId) ||
                     (HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_1 == psInterrupts->eModuleId.eInterfaceId))
            {
               eWriteBackIntfId = HAL_MDP_WRITEBACK_INTERFACE_1;
            }
            else // Write-Back interface 2
            {
               eWriteBackIntfId = HAL_MDP_WRITEBACK_INTERFACE_2;
            }

            HAL_MDP_WritebackInterface_Interrupt_Config(psInterrupts,
                                                        eWriteBackIntfId,
                                                        puMDPIntrBits,
														puMDPRotatorIntrBits);
         }
      }
      break;
   case HAL_MDP_MODULE_PINGPONG:
      {
         HAL_MDP_PingPong_Interrupt_Config(psInterrupts,
                                           psInterrupts->eModuleId.ePingPongId,
                                           puMDPIntrBits);
      }
      break;

   case HAL_MDP_MODULE_WATCHDOG:
     {
         HAL_MDP_WatchDog_Interrupt_Config(psInterrupts,
                                           psInterrupts->eModuleId.eWatchDogId,
                                           puMDPIntrBits);
     }
     break;

   case HAL_MDP_MODULE_DSC:
   case HAL_MDP_MODULE_SCM:
   case HAL_MDP_MODULE_DCE:
   case HAL_MDP_MODULE_PHYSICAL_INTERFACE:
   case HAL_MDP_MODULE_WRITEBACK_INTERFACE:
   case HAL_MDP_MODULE_TYPE_NONE:
   case HAL_MDP_MODULE_LAYERMIXER:
   case HAL_MDP_MODULE_CONTROL_PATH:
   case HAL_MDP_MODULE_DATA_PATH:
   case HAL_MDP_MODULE_SMP:
   case HAL_MDP_MODULE_TRAFFIC_SHAPER:
   case HAL_MDP_MODULE_VBIF:
   case HAL_MDP_MODULE_CDM:
   case HAL_MDP_MODULE_MAX:
   case HAL_MDP_MODULE_FORCE_32BIT:
   default:
      break;
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_ParseRegInfo()
*/
/*!
* \brief
*     Parses the given interrupt register 32-bit values
*
* \param [in/out]   psInterruptStatus           - Status for each module.
* \param [in]       uMDPIntrRegValue            - Can be intr_en, intr_status or intr_clear register value
* \param [in]       uMDPHistIntrRegValue        - Can be intr_en, intr_status or intr_clear register value
* \param [in]       uMDPRotatorIntrRegValue     - Can be intr_en, intr_status or intr_clear register value
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_Interrupt_ParseRegInfo(HAL_MDP_InterruptStatusType *psInterruptStatus,
                                                         uint32                       uMDPIntrRegValue,
                                                         uint32                       uMDPHistIntrRegValue,
														 uint32                       uMDPRotatorIntrRegValue)
{
   HAL_MDSS_ErrorType            eStatus           = HAL_MDSS_STATUS_SUCCESS;
   HAL_MDP_WritebackInterfaceId  eWriteBackIntfId  = HAL_MDP_WRITEBACK_INTERFACE_ID_NONE;
   HAL_MDP_InterruptStatusType  *psIntrStatus      = psInterruptStatus;

   switch(psIntrStatus->sIntrConfig.eModuleType)
   {
   case HAL_MDP_MODULE_SOURCE_PIPE:
      {
         HAL_MDP_SourcePipe_Interrupt_GetStatus(psIntrStatus, uMDPHistIntrRegValue);
      }
      break;

   case HAL_MDP_MODULE_DESTINATION_PIPE:
      {
         HAL_MDP_DestinationPipe_Interrupt_GetStatus(psIntrStatus, uMDPHistIntrRegValue);
      }
      break;

   case HAL_MDP_MODULE_INTERFACE:
      {
         if(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > psIntrStatus->sIntrConfig.eModuleId.eInterfaceId) // Physical interface
         {
            HAL_MDP_PhysicalInterface_Interrupt_GetStatus(psIntrStatus,
                                                          (HAL_MDP_PhyInterfaceId)psIntrStatus->sIntrConfig.eModuleId.eInterfaceId,
                                                          uMDPIntrRegValue);
         }
         else // write-back interface
         {
            if((HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_0 == psIntrStatus->sIntrConfig.eModuleId.eInterfaceId) ||
               (HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_0 == psIntrStatus->sIntrConfig.eModuleId.eInterfaceId))
            {
               eWriteBackIntfId = HAL_MDP_WRITEBACK_INTERFACE_0;
            }
            else if((HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_1 == psIntrStatus->sIntrConfig.eModuleId.eInterfaceId) ||
                     (HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_1 == psIntrStatus->sIntrConfig.eModuleId.eInterfaceId))
            {
               eWriteBackIntfId = HAL_MDP_WRITEBACK_INTERFACE_1;
            }
            else // Write-Back interface 2
            {
               eWriteBackIntfId = HAL_MDP_WRITEBACK_INTERFACE_2;
            }

            HAL_MDP_WritebackInterface_Interrupt_GetStatus(psIntrStatus,
                                                           eWriteBackIntfId,
														   uMDPIntrRegValue,
														   uMDPRotatorIntrRegValue);
         }
      }
      break;

   case HAL_MDP_MODULE_PINGPONG:
      {
         HAL_MDP_PingPong_Interrupt_GetStatus(psIntrStatus, psIntrStatus->sIntrConfig.eModuleId.ePingPongId, uMDPIntrRegValue);
      }
      break;

   case HAL_MDP_MODULE_WATCHDOG:
     {
        HAL_MDP_WatchDog_Interrupt_GetStatus(psIntrStatus,
                                             psIntrStatus->sIntrConfig.eModuleId.eWatchDogId,
                                             uMDPIntrRegValue);
     }
     break;

   case HAL_MDP_MODULE_PHYSICAL_INTERFACE:
   case HAL_MDP_MODULE_WRITEBACK_INTERFACE:
   case HAL_MDP_MODULE_LAYERMIXER:
   case HAL_MDP_MODULE_CONTROL_PATH:
   case HAL_MDP_MODULE_DATA_PATH:
   case HAL_MDP_MODULE_SMP:
   case HAL_MDP_MODULE_TRAFFIC_SHAPER:
   case HAL_MDP_MODULE_VBIF:
   case HAL_MDP_MODULE_TYPE_NONE:
   case HAL_MDP_MODULE_CDM:
   case HAL_MDP_MODULE_MAX:
   case HAL_MDP_MODULE_FORCE_32BIT:
   default:
      break;
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ReadVersionInfo()
*/
/*!
* \brief
*     Returns the version information
*
* \param [in/out] psVersionInfo            - Version information
* \param [in/out] psHwInfo                 - Hardware information
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ReadVersionInfo(HAL_HW_VersionType *psVersionInfo,
                                           HAL_MDP_HwInfo     *psHwInfo)
{
   HAL_MDSS_ErrorType   eStatus               = HAL_MDSS_STATUS_SUCCESS;
   uint32               uVersionInfo   = 0x00;

   uVersionInfo = HWIO_MDSS_HW_HW_VERSION_IN;

   if(NULL != psVersionInfo)
   {
      psVersionInfo->uMajorVersion   = HWIO_GETVAL(MDSS_HW_HW_VERSION, uVersionInfo, MAJOR);
      psVersionInfo->uMinorVersion   = HWIO_GETVAL(MDSS_HW_HW_VERSION, uVersionInfo, MINOR);
      psVersionInfo->uReleaseVersion = HWIO_GETVAL(MDSS_HW_HW_VERSION, uVersionInfo, STEP);

      if (NULL != psHwInfo)
      {
         HAL_MDP_ReadHardwareInfo(psVersionInfo, psHwInfo, NULL );
      }
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Init()
*/
/*!
* \brief
*     This is the first function call to MDP HAL, it will initialize the whole MDP block and
*     set up the default values.
*
* \param [in]   uInitFlags                - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Init(HAL_MDP_InitConfigType *pInitConfig, uint32 uInitFlags)
{
   HAL_MDSS_ErrorType   eStatus        = HAL_MDSS_STATUS_SUCCESS;
   HAL_HW_VersionType   sVersionInfo   = {0};
   uint32               uIndex         = 0;
   uint32               uHWBlockIndex  = 0;

   MDSS_OSAL_MEMZERO((char *)&gsMdpHwInfo, sizeof(HAL_MDP_Hw_Private_Info));

   pgsMdpHwInfo = &gsMdpHwInfo;

   eStatus = HAL_MDP_ReadVersionInfo(&sVersionInfo, &pgsMdpHwInfo->sHwInfo);

   if(HAL_MDSS_STATUS_SUCCESS == eStatus)
   {
      /* Cache the MDP HW version info */
      pgsMdpHwInfo->sHwInfo.sMdpHwVersionInfo = sVersionInfo;

      /** Read MDP internal capabilities */
      HAL_MDP_ReadHardwareInfo(&sVersionInfo, NULL, pgsMdpHwInfo);
   }

   if (HAL_MDP_INIT_FLAGS_DISABLE_INTERRUPTS & uInitFlags)
   {
      HWIO_MDP_INTR_EN_OUT(0x0);                                     // Disable all interrupts
   }

   if (HAL_MDP_INIT_FLAGS_CLEAR_INTERRUPTS & uInitFlags)
   {
      HWIO_MDP_INTR_CLEAR_OUT(0xffffffff);                           // Acknowledge/Clear all interrupts
      HWIO_MDP_HIST_INTR_CLEAR_OUT(0xffffffff);
   }

   if(0x00 != (uInitFlags & HAL_MDP_INIT_FLAGS_VBIF_CONFIGURATION))
   {
      uint32 uVBIFId;

      // Initialize all VBif cores
      for (uVBIFId = 0; uVBIFId < HAL_MDP_GET_VBIF_COUNT(); uVBIFId++)
      {
        HAL_MDP_Vbif_Init((HAL_MDP_VbifId) (HAL_MDP_VBIF_0 + uVBIFId), HAL_MDP_VBIF_ENABLE_CLOCKGATING);
      }
   }

   if(0x00 != (uInitFlags & HAL_MDP_INIT_FLAGS_TRFCTRL_CONFIGURATION))
   {
      HAL_MDP_TrafficCtrl_Init(NULL, 0);
   }

   // Default clock control
   HAL_MDP_ClockControl_Set(0x0,
                            FALSE,
                            pgsMdpHwInfo->uHysterisisValue,
                            0x0);

   if(HAL_MDSS_STATUS_SUCCESS == eStatus) // Only initializes the HW tables
   {
      // VIG SSPP
      uHWBlockIndex  = HAL_MDP_SOURCE_PIPE_VIG_0;
      for(uIndex = 0; uIndex < HAL_MDP_GET_VIG_LAYER_COUNT(); uIndex++, uHWBlockIndex++)
      {
         HAL_MDP_SSPP_Init((HAL_MDP_SourcePipeId)uHWBlockIndex, HAL_MDP_LAYER_TYPE_VIG, HAL_MDP_VIG_LAYER_DEFAULT_FEATURE_INIT, 0x00);
      }

      // Datapath (Write-Back) block initialization
      uHWBlockIndex  = HAL_MDP_DATA_PATH_0;
      for(uIndex = 0; uIndex < HAL_MDP_GET_DATA_PATHS_COUNT(); uIndex++, uHWBlockIndex++)
      {
         HAL_MDP_DataPath_Init((HAL_MDP_DataPathId)uHWBlockIndex, HAL_MDP_DATAPATH_DEFAULT_FEATURE_INIT, 0x00);
      }

   }
   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Term()
*/
/*!
* \brief
*     Terminates the MDP HW blocks
*
* \param [in]   uFlags                - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Term(uint32    uFlags)
{
   HAL_MDSS_ErrorType      eStatus = HAL_MDSS_STATUS_SUCCESS;

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_Enable()
*/
/*!
* \brief
*     Enable interrupt
*     Caution :  The Read-Modify-Write of registers used in this function is
*                not multi-thread safe. It is the calling functions' reponsibility
*                to ensure access to this function is serialized.
*
* \param [in]   psIntrCfgList         - Configuration for each module.
* \param [in]   uNumOfModules         - Number of module
* \param [in]   uFlags                - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_Enable(HAL_MDP_InterruptConfigType  *psIntrCfgList,
                                            uint32                        uNumOfModules,
                                            uint32                        uFlags)
{
   HAL_MDSS_ErrorType            eStatus                = HAL_MDSS_STATUS_SUCCESS;
   uint32                        uMDPIntrBits           = 0;
   uint32                        uMDPHistIntrBits       = 0;
   uint32                        uMDPRotatorIntrBits    = 0;
   uint32                        uNewMDPIntrBits        = 0;   
   uint32                        uNewMDPHistIntrBits    = 0;
   uint32                        uNewMDPRotatorIntrBits = 0;
   uint32                        uModuleCnt             = 0;
   HAL_MDP_InterruptConfigType  *pIntrInfo              = psIntrCfgList;

   /** Prepare the final register values for MDP_INTR_ENABLE and MDP_HIST_INTR_EN */
   for(uModuleCnt = 0; uModuleCnt < uNumOfModules; uModuleCnt++)
   {
      HAL_MDP_Interrupt_Bit_Configuration(pIntrInfo,
                                          &uMDPIntrBits,
                                          &uMDPHistIntrBits,
										  &uMDPRotatorIntrBits);

      pIntrInfo++;
   }

   // Determine if any of the mdp interrupts has been requested to be enabled
   if ((0 != uMDPIntrBits) && 
       // Read the current status of the register and modify the one which is being requested
       (0 != (uMDPIntrBits ^ (uNewMDPIntrBits = HWIO_MDP_INTR_EN_IN))))
   {
     HWIO_MDP_INTR_EN_OUT(uNewMDPIntrBits | uMDPIntrBits);
   }
   
   // Determine if any of the mdp interrupts has been requested to be enabled
   if ((0 != uMDPHistIntrBits) && 
       // Read the current status of the register and modify the one which is being requested
       (0 != (uMDPHistIntrBits ^ (uNewMDPHistIntrBits = HWIO_MDP_HIST_INTR_EN_IN))))
   {
     HWIO_MDP_HIST_INTR_EN_OUT(uNewMDPHistIntrBits | uMDPHistIntrBits);
   }

   // Determine if any of the Rotator interrupts has been requested to be disabled
   if ((0 != uMDPRotatorIntrBits) &&
	   // Read the current status of the register and modify the one which is being requested
	   (0 != (~uMDPRotatorIntrBits ^ (uNewMDPRotatorIntrBits = HWIO_ROT_ROTTOP_INTR_EN_IN))))
   {
	   HWIO_ROT_ROTTOP_INTR_EN_OUT(uNewMDPRotatorIntrBits | uMDPRotatorIntrBits);
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_Disable()
*/
/*!
* \brief
*     disable interrupt
*     Caution :  The Read-Modify-Write of registers used in this function is
*                not multi-thread safe. It is the calling functions' reponsibility
*                to ensure access to this function is serialized.
*
* \param [in]   psIntrCfgList         - Configuration for each module.
* \param [in]   uNumOfModule          - Number of module
* \param [in]   uEnableFlags          - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_Disable(HAL_MDP_InterruptConfigType  *psIntrCfgList,
                                             uint32                        uNumOfModules,
                                             uint32                        uEnableFlags)
{
   HAL_MDSS_ErrorType            eStatus                 = HAL_MDSS_STATUS_SUCCESS;
   uint32                        uMDPIntrBits            = 0;
   uint32                        uMDPHistIntrBits        = 0;
   uint32                        uMDPRotatorIntrBits     = 0;
   uint32                        uNewMDPIntrBits         = 0;   
   uint32                        uNewMDPHistIntrBits     = 0;
   uint32                        uNewMDPRotatorIntrBits  = 0;
   uint32                        uModuleCnt              = 0;
   HAL_MDP_InterruptConfigType  *pIntrInfo               = psIntrCfgList;

   /** Prepare the final register values for MDP_INTR_ENABLE and MDP_HIST_INTR_EN */
   for(uModuleCnt = 0; uModuleCnt < uNumOfModules; uModuleCnt++)
   {
      HAL_MDP_Interrupt_Bit_Configuration(pIntrInfo,
                                          &uMDPIntrBits,
                                          &uMDPHistIntrBits,
										  &uMDPRotatorIntrBits);

      pIntrInfo++;
   }

   // Determine if any of the mdp interrupts has been requested to be disabled
   if ((0 != uMDPIntrBits) && 
       // Read the current status of the register and modify the one which is being requested
       (0 != (~uMDPIntrBits ^ (uNewMDPIntrBits = HWIO_MDP_INTR_EN_IN))))
   {
     // Disable the interrupt
     HWIO_MDP_INTR_EN_OUT(uNewMDPIntrBits & (~uMDPIntrBits));
   }
   
   // Determine if any of the mdp interrupts has been requested to be disabled
   if ((0 != uMDPHistIntrBits) && 
       // Read the current status of the register and modify the one which is being requested
       (0 != (~uMDPHistIntrBits ^ (uNewMDPHistIntrBits = HWIO_MDP_HIST_INTR_EN_IN))))
   {
    // Disable the interrupt
     HWIO_MDP_HIST_INTR_EN_OUT(uNewMDPHistIntrBits & (~uMDPHistIntrBits));
   }

   // Determine if any of the mdp interrupts has been requested to be disabled
   if ((0 != uMDPRotatorIntrBits) &&
	   // Read the current status of the register and modify the one which is being requested
	   (0 != (~uMDPRotatorIntrBits ^ (uNewMDPRotatorIntrBits = HWIO_ROT_ROTTOP_INTR_EN_IN))))
   {
	   // Disable the interrupt
	   HWIO_ROT_ROTTOP_INTR_EN_OUT(uNewMDPRotatorIntrBits & (~uMDPRotatorIntrBits));
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_Clear()
*/
/*!
* \brief
*     Clear interrupt
*
* \param [in]   psIntrCfgList         - Configuration for each module.
* \param [in]   uNumOfModules         - Number of modules
* \param [in]   uClearFlags           - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_Clear(HAL_MDP_InterruptConfigType   *psIntrCfgList,
                                            uint32                        uNumOfModules,
                                            uint32                        uClearFlags)
{
   HAL_MDSS_ErrorType            eStatus             = HAL_MDSS_STATUS_SUCCESS;
   uint32                        uMDPIntrBits        = 0x00;
   uint32                        uMDPHistIntrBits    = 0x00;
   uint32                        uMDPRotatorIntrBits = 0x00;
   uint32                        uModuleCnt          = 0x00;
   HAL_MDP_InterruptConfigType   *pIntrConfig        = psIntrCfgList;

   /** Prepare the final register values for MDP_INTR_CLEAR and MDP_HIST_INTR_CLEAR */
   for(uModuleCnt = 0; uModuleCnt < uNumOfModules; uModuleCnt++)
   {
      HAL_MDP_Interrupt_Bit_Configuration(pIntrConfig,
                                          &uMDPIntrBits,
                                          &uMDPHistIntrBits,
										  &uMDPRotatorIntrBits);
      pIntrConfig++;
   }

   // Determine if any of the mdp interrupts has been requested to be cleared
   if (0 != uMDPIntrBits)
   {
      HWIO_MDP_INTR_CLEAR_OUT(uMDPIntrBits);
   }
   
   // Determine if any of the mdp interrupts has been requested to be cleared
   if (0 != uMDPHistIntrBits)
   {
      HWIO_MDP_HIST_INTR_CLEAR_OUT(uMDPHistIntrBits);
   }

   // Determine if any of the mdp interrupts has been requested to be cleared
   if (0 != uMDPRotatorIntrBits)
   {
	   HWIO_ROT_ROTTOP_INTR_CLEAR_OUT(uMDPRotatorIntrBits);
   }
   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_GetStatus()
*/
/*!
* \brief
*     Get interrupt status
*     Interrupt status is always collect the status (set until cleared) 
*     regardless of the enable bit.
*
* \param [in/out]   psIntrStatusList            - Status for each module.
* \param [in]       uNumOfModules               - Number of modules
* \param [in]       uGetStatusFlags             - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_GetStatus(HAL_MDP_InterruptStatusType *psIntrStatusList,
                                               uint32                       uNumOfModules,
                                               uint32                       uGetStatusFlags)
{
   HAL_MDSS_ErrorType            eStatus             = HAL_MDSS_STATUS_SUCCESS;
   uint32                        uMDPIntrBits        = 0;
   uint32                        uMDPHistIntrBits    = 0;
   uint32                        uMDPRotatorIntrBits = 0;
   uint32                        uModuleCnt          = 0;
   HAL_MDP_InterruptStatusType  *psIntrStatus        = psIntrStatusList;

   if (HAL_MDSS_MODULE_MDP & psIntrStatus->uInterruptModuleId)
   {
	   uMDPIntrBits     = HWIO_MDP_INTR_STATUS_IN;
	   uMDPHistIntrBits = HWIO_MDP_HIST_INTR_STATUS_IN;
   }
   else if (HAL_MDSS_MODULE_ROT & psIntrStatus->uInterruptModuleId)
   {
	   uMDPRotatorIntrBits = HWIO_ROT_ROTTOP_INTR_STATUS_IN;
   }

   for(uModuleCnt = 0; uModuleCnt < uNumOfModules; uModuleCnt++)
   {
	   HAL_MDP_Interrupt_ParseRegInfo(psIntrStatus, uMDPIntrBits, uMDPHistIntrBits, uMDPRotatorIntrBits);
      psIntrStatus++;
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_IsEnable()
*/
/*!
* \brief
*     Returns the current state interrupt configuration
*
* \param [in/out]   psInterruptStatus           - Status for each module.
* \param [in]       uNumOfModules               - Number of modules
* \param [in]       uGetStatusFlags             - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_IsEnable(HAL_MDP_InterruptStatusType *psInterruptStatus,
                                              uint32                       uNumOfModules,
                                              uint32                       uGetStatusFlags)
{
   HAL_MDSS_ErrorType            eStatus             = HAL_MDSS_STATUS_SUCCESS;
   uint32                        uMDPIntrBits        = 0;
   uint32                        uMDPHistIntrBits    = 0;
   uint32                        uMDPRotatorIntrBits = 0;
   uint32                        uModuleCnt          = 0x00;
   HAL_MDP_InterruptStatusType  *psIntrStatus        = psInterruptStatus;

   if (HAL_MDSS_MODULE_MDP & psIntrStatus->uInterruptModuleId)
   {
	   uMDPIntrBits     = HWIO_MDP_INTR_EN_IN;
	   uMDPHistIntrBits = HWIO_MDP_HIST_INTR_EN_IN;
   }
   else if (HAL_MDSS_MODULE_ROT & psIntrStatus->uInterruptModuleId)
   {
	   uMDPRotatorIntrBits = HWIO_ROT_ROTTOP_INTR_EN_IN;
   }

   for(uModuleCnt = 0; uModuleCnt < uNumOfModules; uModuleCnt++)
   {
	   HAL_MDP_Interrupt_ParseRegInfo(psIntrStatus, uMDPIntrBits, uMDPHistIntrBits, uMDPRotatorIntrBits);
      psIntrStatus++;
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_ReadStatus_Reg()
*/
/*!
* \brief
*     Returns the interrupts status
*
* \param [in/out] puIntrStatusValue             - Interrupt received Status value
* \param [in]     uFlags                        - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_ReadStatus_Reg(uint32           *puIntrStatusValue,
                                                    uint32            uFlags)
{
   HAL_MDSS_ErrorType   eStatus                 = HAL_MDSS_STATUS_SUCCESS;
   uint32               uMDPIntrStatusBits      = HWIO_MDP_INTR_STATUS_IN;
   uint32               uMDPIntrEnabledBits     = HWIO_MDP_INTR_EN_IN;

   *puIntrStatusValue   = uMDPIntrStatusBits;

   HWIO_MDP_INTR_CLEAR_OUT((uMDPIntrStatusBits & uMDPIntrEnabledBits));    // Acknowledge only the enabled interrupts

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_IsPingPongInterrupt()
*/
/*!
* \brief
*     Checks whether a Ping Pong interrupt has fired. This function is used
*  in MDP ISR, where special handling occurs in case of Smart Panels, which
*  use Ping Pong interrupts.
*
* \param [in]  puIntrStatusValue       - Interrupt status value. If Null, 
*                                        status register is read directly
* \param [in]  uFlags                  - Reserved
*
* \retval bool32: TRUE=there is a Ping Pong interrupt
*
****************************************************************************/
bool32 HAL_MDP_IsPingPongInterrupt(uint32       *puIntrStatusValue,
                                   uint32       uFlags)
{
   uint32   uMDPIntrStatusBits      = 0x00;
   bool32   bIsPingPongInterrupt    = FALSE;

   if(NULL == puIntrStatusValue)
   {
      uMDPIntrStatusBits = HWIO_MDP_INTR_STATUS_IN;
   }
   else
   {
      uMDPIntrStatusBits = *puIntrStatusValue;
   }

   // For now, we only care about RD_PTR and PING_PONG_DONE interrupts
   // We can add WR_PTR and AUTOREFRESH to this list later if we need to
   if(0x00 != (uMDPIntrStatusBits & (HAL_MDP_INTR_STATUS_PING_PONG_RD_PTR_BMSK | HAL_MDP_INTR_STATUS_PING_PONG_DONE_BMSK)))
   {
      bIsPingPongInterrupt = TRUE;
   }

   return bIsPingPongInterrupt;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ClockControl_Set()
*/
/*!
* \brief
*     Configures MDP clock control status registers
*
* \param [in]  uClockList           - List of specific clocks to control (0 means all)
* \param [in]  bForceOn             - Force clocks on
* \param [in]  uHysterisisValue     - Hysterisis setting
* \param [in]  uFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ClockControl_Set(uint32            uClockList,
                                            bool32            bForceOn,
                                            uint32            uHysterisisValue,
                                            uint32            uFlags)
{
   HAL_MDSS_ErrorType   eStatus                 = HAL_MDSS_STATUS_SUCCESS;
   uint32               uClockCtrl;

   // Read group 0
   uClockCtrl = HWIO_MDP_CLK_CTRL0_IN;
   // SMP
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, SMP_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, SMP_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, SMP_CLK_FORCE_ON, bForceOn);
   // DMA0
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, DMA0_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, DMA0_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, DMA0_CLK_FORCE_ON, bForceOn);
   // RGB0
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, RGB0_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, RGB0_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, RGB0_CLK_FORCE_ON, bForceOn);
   // ViG0
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, VIG0_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, VIG0_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL0, VIG0_CLK_FORCE_ON, bForceOn);
   // Write group 0
   HWIO_MDP_CLK_CTRL0_OUT(uClockCtrl);

   // Read group 1
   uClockCtrl = HWIO_MDP_CLK_CTRL1_IN;
   // AHB
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, AHB_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, AHB_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, AHB_CLK_FORCE_ON, bForceOn);
   // DMA1
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, DMA1_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, DMA1_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, DMA1_CLK_FORCE_ON, bForceOn);
   // RGB1
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, RGB1_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, RGB1_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, RGB1_CLK_FORCE_ON, bForceOn);
   // ViG1
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, VIG1_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, VIG1_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL1, VIG1_CLK_FORCE_ON, bForceOn);
   // Write group 1
   HWIO_MDP_CLK_CTRL1_OUT(uClockCtrl);

   // Read group 2
   uClockCtrl = HWIO_MDP_CLK_CTRL2_IN;
   // VBIF_CLI_ARB
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ARB_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ARB_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ARB_CLK_FORCE_ON, bForceOn);
   // VBIF_CLI_WB2
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_WB2_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_WB2_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_WB2_CLK_FORCE_ON, bForceOn);
   // VBIF_CLI_ROT1
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ROT1_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ROT1_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ROT1_CLK_FORCE_ON, bForceOn);
   // VBIF_CLI_ROT0
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ROT0_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ROT0_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VBIF_CLI_ROT0_CLK_FORCE_ON, bForceOn);
   // RGB2
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, RGB2_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, RGB2_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, RGB2_CLK_FORCE_ON, bForceOn);
   // ViG2
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VIG2_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VIG2_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL2, VIG2_CLK_FORCE_ON, bForceOn);
   // Write group 2
   HWIO_MDP_CLK_CTRL2_OUT(uClockCtrl);

   // Read group 3
   uClockCtrl = HWIO_MDP_CLK_CTRL3_IN;
   // ROT0
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, ROT0_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, ROT0_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, ROT0_CLK_FORCE_ON, bForceOn);
   // PP0
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, PP0_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, PP0_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, PP0_CLK_FORCE_ON, bForceOn);
   // CURSOR0
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, CURSOR0_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, CURSOR0_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, CURSOR0_CLK_FORCE_ON, bForceOn);
   // LM_DSPP0
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, LM_DSPP0_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, LM_DSPP0_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL3, LM_DSPP0_CLK_FORCE_ON, bForceOn);
   // Write group 3
   HWIO_MDP_CLK_CTRL3_OUT(uClockCtrl);

   // Read group 4
   uClockCtrl = HWIO_MDP_CLK_CTRL4_IN;
   // AHB
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, AHB_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, AHB_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, AHB_CLK_FORCE_ON, bForceOn);
   // ROT1
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, ROT1_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, ROT1_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, ROT1_CLK_FORCE_ON, bForceOn);
   // PP1
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, PP1_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, PP1_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, PP1_CLK_FORCE_ON, bForceOn);
   // CURSOR1
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, CURSOR1_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, CURSOR1_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, CURSOR1_CLK_FORCE_ON, bForceOn);
   // LM_DSPP1
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, LM_DSPP1_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, LM_DSPP1_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL4, LM_DSPP1_CLK_FORCE_ON, bForceOn);
   // Write group 4
   HWIO_MDP_CLK_CTRL4_OUT(uClockCtrl);

   // Read group 5
   uClockCtrl = HWIO_MDP_CLK_CTRL5_IN;
   // VBIF_ARB
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, VBIF_ARB_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, VBIF_ARB_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, VBIF_ARB_CLK_FORCE_ON, bForceOn);
   // WB2
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, WB2_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, WB2_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, WB2_CLK_FORCE_ON, bForceOn);
   // PP2
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, PP2_CLK_HYSTERISIS_CTRL, uHysterisisValue);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, PP2_CLK_FORCE_OFF, 0);   
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, PP2_CLK_FORCE_ON, bForceOn);
   // CURSOR2
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, CURSOR2_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, CURSOR2_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, CURSOR2_CLK_FORCE_ON, bForceOn);
   // LM_DSPP2
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, LM_DSPP2_CLK_HYSTERISIS_CTRL, uHysterisisValue);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, LM_DSPP2_CLK_FORCE_OFF, 0);
   uClockCtrl = HWIO_OUT_FLD(uClockCtrl, MDP_CLK_CTRL5, LM_DSPP2_CLK_FORCE_ON, bForceOn);
   // Write group 4
   HWIO_MDP_CLK_CTRL5_OUT(uClockCtrl);

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_WatchDog_Setup()
*/
/*!
* \brief
*     Setup configurations for MDP watch dog registers
*
* \param [in]  HAL_MDP_WatchDogId   - Watch Dog ID (0, 1, 2...)
* \param [in]  psWatchDogConfig     - Pointer to the watch dog configuration information
* \param [in]  uFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_WatchDog_Setup(HAL_MDP_WatchDogId           eWatchDogId,
                                          HAL_MDP_WatchDogConfigType   *psWatchDogConfig,
                                          uint32                       uFlags)
{
   HAL_MDSS_ErrorType   eStatus    = HAL_MDSS_STATUS_SUCCESS;
   uintPtr              uRegOffset = 0x0;
   uint32               uRegValue  = 0x00000000;

   if(HAL_MDP_WATCHDOG_NONE == eWatchDogId)
   {
      // just return success if HW doesn't support 
   }
   else if(HAL_MDP_GET_WATCHDOG_COUNT() < (uint32)eWatchDogId)
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      // get register offset
      uRegOffset += (eWatchDogId - HAL_MDP_WATCHDOG_0) * (HWIO_MDP_WD_TIMER_1_CTL_ADDR - HWIO_MDP_WD_TIMER_0_CTL_ADDR);

      if(NULL != psWatchDogConfig->pTimerConfig)
      {
        // program counter granularity
        uRegValue = in_dword(HWIO_MDP_WD_TIMER_0_CTL2_ADDR + uRegOffset);
        uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WD_TIMER_0_CTL2, WD_TIMER_0_GRANULARITY, psWatchDogConfig->pTimerConfig->uGranularity);
        out_dword(HWIO_MDP_WD_TIMER_0_CTL2_ADDR + uRegOffset, uRegValue);

        // program main counter
        uRegValue = HWIO_OUT_FLD(0, MDP_WD_TIMER_0_LOAD_VALUE, WD_TIMER_0_LOAD_VALUE, psWatchDogConfig->pTimerConfig->uLoadValue);
        out_dword(HWIO_MDP_WD_TIMER_0_LOAD_VALUE_ADDR + uRegOffset, uRegValue);
      }

      if(NULL != psWatchDogConfig->pTimerClear)
      {
        // clear the timer counter, which causes timer to reset
        out_dword(HWIO_MDP_WD_TIMER_0_CTL_ADDR + uRegOffset, ((psWatchDogConfig->pTimerClear->bClear)? 1 : 0));
      }

      if(NULL != psWatchDogConfig->pTimerEnable)
      {
        // enable/disable the timer
        uRegValue = in_dword(HWIO_MDP_WD_TIMER_0_CTL2_ADDR + uRegOffset);
        uRegValue = HWIO_OUT_FLD(uRegOffset, MDP_WD_TIMER_0_CTL2, WD_TIMER_0_ENABLE, ((psWatchDogConfig->pTimerEnable->bEnable)? 1 : 0));
        out_dword(HWIO_MDP_WD_TIMER_0_CTL2_ADDR + uRegOffset, uRegValue);
      }
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_WatchDog_GetProperty()
*/
/*!
* \brief
*     Retrieve information of current watch dog timer counter
*
* \param [in]  HAL_MDP_WatchDogId   - Watch Dog ID (0, 1, 2...)
* \param [in]  psWatchDogInfoData   - Pointer to the watch dog info data
* \param [in]  uFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_WatchDog_GetProperty(HAL_MDP_WatchDogId               eWatchDogId,
                                                HAL_MDP_WatchDogInfoParamsType   *psWatchDogInfoData,
                                                uint32                           uFlags)
{
   HAL_MDSS_ErrorType   eStatus    = HAL_MDSS_STATUS_SUCCESS;
   uintPtr              uRegOffset = 0x0;
   uint32               uRegValue  = 0x00000000;

   if(HAL_MDP_WATCHDOG_NONE == eWatchDogId)
   {
      // just return success if HW doesn't support 
   }
   else if(HAL_MDP_GET_WATCHDOG_COUNT() < (uint32)eWatchDogId)
   {
      HAL_MDP_ASSERT();
   }
   else if(NULL == psWatchDogInfoData)
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      // get register offset
      uRegOffset += (eWatchDogId - HAL_MDP_WATCHDOG_0) * (HWIO_MDP_WD_TIMER_1_CTL_ADDR - HWIO_MDP_WD_TIMER_0_CTL_ADDR);
      // get current timer counter information
      uRegValue = in_dword(HWIO_MDP_WD_TIMER_0_STATUS_VALUE_ADDR + uRegOffset);
      psWatchDogInfoData->uCurrentCounter = uRegValue;
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_QOSSetup()
*/
/*!
* \brief
*     Set QOS remapper setting based on Configurations .
*
* \param [in]  pQOSSettings  - QOS remapper settings
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_QOSSetup(HAL_MDP_QOSRemapperType *pQOSSettings)
{
  HAL_MDSS_ErrorType   eStatus        = HAL_MDSS_STATUS_SUCCESS;
    
  /* Set QOS remapper values */
  out_dword(HWIO_MDP_QOS_REMAPPER_CLASS_0_ADDR, pQOSSettings->uQosRemapperRealTimeClients);
  out_dword(HWIO_MDP_QOS_REMAPPER_CLASS_1_ADDR, pQOSSettings->uQosRemapperNonRealTimeClients);
  
  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_GpuMemoryBankBitConfig()
*/
/*!
* \brief
*     This function will return the valid Gpu memory bank bit configuration
*     It will validate the OS provided value and if it is invalid, will set to default
*
* \param [in]   psGpuMemoryBankConfig     - Gpu highest memory bank bit config
* \param [in]   uFlags                    - Reserved.
*
* \retval uint32
*
****************************************************************************/

uint32 HAL_MDP_Get_GpuMemoryBankBitConfig(HAL_MDP_GpuMemoryBankConfigType *psGpuMemoryBankConfig,
                                          uint32                          uFlags)
{
  uint32   uGpuHighestMemoryBankBit = 0;
  
  switch(psGpuMemoryBankConfig->uGpuHighestMemoryBankBit)
  {
    case 13:
      uGpuHighestMemoryBankBit = 0x00;
      break;

    case 14:
      uGpuHighestMemoryBankBit = 0x01;
      break;

    case 15:
      uGpuHighestMemoryBankBit = 0x02;
      break;

    case 16:
      uGpuHighestMemoryBankBit = 0x03;
      break;

    default:
      uGpuHighestMemoryBankBit = pgsMdpHwInfo->uDefaultHighestBankBit;
  }

  return uGpuHighestMemoryBankBit;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SetupCommitMode()
*/
/*!
* \brief
*     Configure specific commit on split display. For ex for single commit
*     writing commit bits on  master pipe will also commit slave pipe. 
*     This applicable for video mode only.
*
* \param [in]     eCommitMode  - Requested commit mode configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SetupCommitMode(HAL_MDP_CommitModeType eCommitMode)
{
  HAL_MDSS_ErrorType   eStatus        = HAL_MDSS_STATUS_SUCCESS;
  uint32               uRegValue      = 0x00000000;

  // Check the commit mode type
  if (HAL_MDP_COMMITMODE_SINGLE == eCommitMode)
  {
    // Get the current register setting
    uRegValue = HWIO_MDP_MDP_SSPP_SPARE_0_IN;
    uRegValue = HWIO_OUT_FLD(uRegValue, MDP_MDP_SSPP_SPARE_0, MDP_SSPP_SPARE0, TRUE);

    // Update the register setting with new value
    HWIO_MDP_MDP_SSPP_SPARE_0_OUT(uRegValue);
  }

  return eStatus;
}


#ifdef __cplusplus
}
#endif
