/*==============================================================================

    D A L I N T E R R U P T   C O N T R O L L E R    



  DESCRIPTION
   This file is autogenerated for interrupt controller config for this platform.

          Copyright (c) 2014-2017 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
              QUALCOMM Proprietary/GTDR

  ===========================================================================*/


#include "DALReg.h"
#include "DDIInterruptController.h"
#include "InterruptControllerInterface.h"

static InterruptConfigType InterruptConfigs[] = 
{
  {0,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"slpc_l2vic_wake_irq"},
  {1,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"qdsp6_isdb_irq"},
  {2,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"q6ss_qtmr_irq_0"},
  {3,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"q6ss_qtmr_irq_1"},
  {4,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"q6ss_qtmr_irq_2"},
  {5,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"qdsp6_etintreq"},
  {7,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {8,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {9,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {12,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {13,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {14,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {15,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {16,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {17,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {18,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {19,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {20,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {21,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {22,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {23,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {24,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {25,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {26,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {29,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {30,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {30,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[0]"},
  {31,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {32,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {33,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {34,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {35,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {36,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {37,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {38,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {41,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {42,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {43,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {44,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {45,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {46,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {47,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {48,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {49,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {50,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {51,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {52,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {53,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {65,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[75]"},
  {66,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[76]"},
  {67,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[77]"},
  {68,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[78]"},
  {69,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[79]"},
  {70,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[80]"},
  {71,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[81]"},
  {72,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[82]"},
  {75,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {76,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {77,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {77,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"vsense_ssc_q6_alarm_irq"},
  {78,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {78,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"vsense_ssc_q6_alarm_irq"},
  {79,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {79,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {80,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {80,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {81,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {81,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {82,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {82,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {83,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {83,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {84,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {84,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {85,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {85,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {86,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {86,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {87,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {87,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {88,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {88,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {89,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {89,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {90,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {90,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {91,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {91,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {92,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {92,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {93,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {93,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {94,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {94,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {95,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {95,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {96,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {96,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {97,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {97,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {98,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {98,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {99,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {99,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {100,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {100,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {101,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {101,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {102,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {102,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {103,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {103,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {104,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {104,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {105,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {105,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {106,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {106,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {107,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {107,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {108,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {108,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {109,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {109,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {110,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {110,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {112,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {112,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {113,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {113,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {114,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {115,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {116,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,"na"},
  {116,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {117,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {117,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {118,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {118,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {119,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {119,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {120,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {120,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {121,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {122,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {123,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"na"},
  {125,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[1]"},
  {126,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[2]"},
  {127,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[3]"},
  {128,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[4]"},
  {132,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[8]"},
  {133,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[9]"},
  {134,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[10]"},
  {135,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[11]"},
  {136,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[12]"},
  {137,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[13]"},
  {138,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[14]"},
  {139,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[15]"},
  {140,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[16]"},
  {141,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[17]"},
  {142,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[18]"},
  {143,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[19]"},
  {144,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[20]"},
  {145,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[21]"},
  {146,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[22]"},
  {147,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[23]"},
  {148,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[24]"},
  {149,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[25]"},
  {150,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[26]"},
  {151,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[27]"},
  {152,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[28]"},
  {153,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[29]"},
  {154,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[30]"},
  {155,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[31]"},
  {156,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[32]"},
  {157,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[33]"},
  {158,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[34]"},
  {159,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[35]"},
  {160,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[36]"},
  {161,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[37]"},
  {162,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[38]"},
  {164,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[40]"},
  {165,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[41]"},
  {166,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[42]"},
  {167,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[43]"},
  {168,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[44]"},
  {169,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[45]"},
  {170,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[46]"},
  {171,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[47]"},
  {172,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[48]"},
  {173,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[49]"},
  {174,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[50]"},
  {175,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[51]"},
  {176,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[52]"},
  {177,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[53]"},
  {178,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[54]"},
  {179,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[55]"},
  {180,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[56]"},
  {181,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[57]"},
  {182,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[58]"},
  {183,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[59]"},
  {184,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[60]"},
  {185,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[61]"},
  {186,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[62]"},
  {187,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[63]"},
  {188,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[64]"},
  {189,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[65]"},
  {190,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[66]"},
  {191,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[67]"},
  {192,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[68]"},
  {193,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[69]"},
  {194,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[70]"},
  {195,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[71]"},
  {196,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[72]"},
  {197,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[73]"},
  {198,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER,"sensors_pdc_irq_out[74]"},
  {INVALID_INTERRUPT,DALINTRCTRL_ENABLE_DEFAULT_SETTINGS,""}
};


InterruptPlatformDataType  pInterruptControllerConfigData[] =
{
  {
    InterruptConfigs,
    199,
  }
};