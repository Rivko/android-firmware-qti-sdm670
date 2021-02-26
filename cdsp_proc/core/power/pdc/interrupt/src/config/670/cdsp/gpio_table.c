/*==============================================================================
  FILE:         gpio_table.c

  OVERVIEW:     PDC hardware to local gpio mapping info.
 
  DEPENDENCIES: None
  
  Note:         Based on: Master_Interrupts_Napali v26.0

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/config/670/cdsp/gpio_table.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#include "comdef.h"
#include "pdc_internal.h"

/*============================================================================
 *                              GLOBAL VARIABLES
 *===========================================================================*/
/* Valid GPIO mux input numbers for this target */
pdc_gpio_inputs g_pdcGpioInputs[] = 
{
  /* GPIO         Configured Mux */

  /* Input 0 */
  {1,             {PDC_GPIO_INVALID}}, 
  {3,             {PDC_GPIO_INVALID}},
  {5,             {PDC_GPIO_INVALID}},
  {10,            {PDC_GPIO_INVALID}},
  {11,            {PDC_GPIO_INVALID}},

  /* Input 5 */
  {20,            {PDC_GPIO_INVALID}},
  {22,            {PDC_GPIO_INVALID}},
  {24,            {PDC_GPIO_INVALID}},
  {26,            {PDC_GPIO_INVALID}},
  {30,            {PDC_GPIO_INVALID}},
  
  /* Input 10 */
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},
  {32,            {PDC_GPIO_INVALID}},  
  {34,            {PDC_GPIO_INVALID}},  
  {36,            {PDC_GPIO_INVALID}},  
  {37,            {PDC_GPIO_INVALID}},  

  /* Input 15 */
  {38,            {PDC_GPIO_INVALID}},
  {39,            {PDC_GPIO_INVALID}},
  {40,            {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},  
  {43,            {PDC_GPIO_INVALID}},  
  
  /* Input 20 */
  {44,            {PDC_GPIO_INVALID}},  
  {46,            {PDC_GPIO_INVALID}},  
  {48,            {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},  
  {52,            {PDC_GPIO_INVALID}},

  /* Input 25 */
  {53,            {PDC_GPIO_INVALID}},
  {54,            {PDC_GPIO_INVALID}},
  {56,            {PDC_GPIO_INVALID}},  
  {57,            {PDC_GPIO_INVALID}},
  {58,            {PDC_GPIO_INVALID}},
  
  /* Input 30 */
  {59,            {PDC_GPIO_INVALID}},  
  {60,            {PDC_GPIO_INVALID}},  
  {61,            {PDC_GPIO_INVALID}},  
  {62,            {PDC_GPIO_INVALID}},  
  {63,            {PDC_GPIO_INVALID}},  

  /* Input 35 */
  {64,            {PDC_GPIO_INVALID}},
  {66,            {PDC_GPIO_INVALID}},  
  {68,            {PDC_GPIO_INVALID}},  
  {71,            {PDC_GPIO_INVALID}},
  {73,            {PDC_GPIO_INVALID}}, 

  /* Input 40 */  
  {77,            {PDC_GPIO_INVALID}},
  {78,            {PDC_GPIO_INVALID}},
  {79,            {PDC_GPIO_INVALID}},
  {80,            {PDC_GPIO_INVALID}},  
  {84,            {PDC_GPIO_INVALID}},  

  /* Input 45 */
  {85,            {PDC_GPIO_INVALID}},  
  {86,            {PDC_GPIO_INVALID}},  
  {88,            {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},  
  {91,            {PDC_GPIO_INVALID}},  
  
  /* Input 50 */
  {92,            {PDC_GPIO_INVALID}},  
  {95,            {PDC_GPIO_INVALID}},  
  {96,            {PDC_GPIO_INVALID}},  
  {97,            {PDC_GPIO_INVALID}}, 
  {101,           {PDC_GPIO_INVALID}}, 

  /* Input 55 */
  {103,           {PDC_GPIO_INVALID}}, 
  {104,           {PDC_GPIO_INVALID}}, 
  {108,           {PDC_GPIO_INVALID}}, /* core_bi_px_to_mpm[6] */
  {112,           {PDC_GPIO_INVALID}}, /* core_bi_px_to_mpm[0] */
  {113,           {PDC_GPIO_INVALID}}, /* core_bi_px_to_mpm[1] */
  
  /* Input 60 */  
  {115,           {PDC_GPIO_INVALID}}, 
  {116,           {PDC_GPIO_INVALID}}, 
  {117,           {PDC_GPIO_INVALID}}, 
  {118,           {PDC_GPIO_INVALID}}, 
  {119,           {PDC_GPIO_INVALID}}, 

  /* Input 65 */
  {120,           {PDC_GPIO_INVALID}}, 
  {121,           {PDC_GPIO_INVALID}}, 
  {122,           {PDC_GPIO_INVALID}}, 
  {123,           {PDC_GPIO_INVALID}}, 
  {124,           {PDC_GPIO_INVALID}}, 
  
  /* Input 70 */
  {125,           {PDC_GPIO_INVALID}}, 
  {126,           {PDC_GPIO_INVALID}}, /* core_bi_px_to_mpm[5] */
  {127,           {PDC_GPIO_INVALID}}, 
  {128,           {PDC_GPIO_INVALID}}, 
  {129,           {PDC_GPIO_INVALID}}, 

  /* Input 75 */
  {130,           {PDC_GPIO_INVALID}}, 
  {132,           {PDC_GPIO_INVALID}}, 
  {133,           {PDC_GPIO_INVALID}}, 
  {145,           {PDC_GPIO_INVALID}},  
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},
  
  /* Input 80 */
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},

  /* Input 85 */
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},  
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}},  
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}}, /* core_bi_px_to_mpm[2] */
  
  /* Input 90 */
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}}, /* core_bi_px_to_mpm[3] */
  {PDC_MUX_OPEN,  {PDC_GPIO_INVALID}}, /* core_bi_px_to_mpm[4] */
  {41,            {PDC_GPIO_INVALID}}, 
  {89,            {PDC_GPIO_INVALID}}, 
  {31,            {PDC_GPIO_INVALID}}, 

  /* Input 95 */
  {49,            {PDC_GPIO_INVALID}}
};

/* Size of above table */
const uint32 g_pdcGPIOInputSize = sizeof(g_pdcGpioInputs) / sizeof(g_pdcGpioInputs[0]);
