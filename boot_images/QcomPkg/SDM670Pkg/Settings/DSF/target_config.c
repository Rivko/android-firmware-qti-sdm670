/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2014, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/DSF/target_config.c#7 $
$DateTime: 2017/12/01 09:43:29 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
05/11/14   arindamm     Initial creation for 8994 V1-only code.
================================================================================*/

#include "ddrss.h"
#include "target_config.h"

// Numbers used in calculating PLL fractional-divider ratios
#define TWO_TO_THE_18TH  ((double) 262144.0)  // Like it says on the box.
#define VCO_MIN_IN_KHZ 1000000

// =============================================================================
// Bit-byte remapping per the bump map. 
// =============================================================================


uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY] = {
       {//channel 0
         //0  1  2  3  4  5  6  7  8
         { 3, 2, 1, 0, 5, 6, 7, 4, DQ_DBI_BIT, DQ_UNCONNECTED }, //byte0
         { 3, 0, 2, 1, 4, 6, 7, 5, DQ_DBI_BIT, DQ_UNCONNECTED }, //byte1
       } ,
       {//channel 1
         //0  1  2  3  4  5  6  7  8
         { 3, 2, 1, 0, 5, 6, 7, 4, DQ_DBI_BIT, DQ_UNCONNECTED }, //byte0
         { 3, 0, 2, 1, 4, 6, 7, 5, DQ_DBI_BIT, DQ_UNCONNECTED }, //byte1
       } ,
       // {//channel 2
         //0  1  2  3  4  5  6  7  8
         // { 3, 1, 2, 0, 5, 6, 7, 4, DQ_DBI_BIT, DQ_UNCONNECTED }, //byte0
         // { 3, 0, 2, 1, 4, 6, 7, 5, DQ_DBI_BIT, DQ_UNCONNECTED }, //byte1
       // } ,
       // {//channel 3
         //0  1  2  3  4  5  6  7  8
         // { 3, 1, 2, 0, 5, 6, 7, 4, DQ_DBI_BIT, DQ_UNCONNECTED }, //byte0
         // { 3, 0, 2, 1, 4, 6, 7, 5, DQ_DBI_BIT, DQ_UNCONNECTED } //byte1
       // }        
      };
//                                       2          4           8
uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][DQ_PER_BYTE ] = {
       {//channel 0
       //  0  1  2  3  4  5  6  7
         { 3, 2, 1, 0, 7, 4, 5, 6}, //byte0
         { 1, 3, 2, 0, 4, 7, 5, 6}, //byte1
       },
       {//channel 1
       //  0  1  2  3  4  5  6  7
         { 3, 2, 1, 0, 7, 4, 5, 6}, //byte0
         { 1, 3, 2, 0, 4, 7, 5, 6}, //byte1
       },
       // {//channel 2
		//0  1  2  3  4  5  6  7
         // { 3, 1, 2, 0, 7, 4, 5, 6}, //byte0
         // { 1, 3, 2, 0, 4, 7, 5, 6}, //byte1
       // },                         
       // {//channel 3               
        //0  1  2  3  4  5  6  7 
         // { 3, 1, 2, 0, 7, 4, 5, 6}, //byte0
         // { 1, 3, 2, 0, 4, 7, 5, 6} //byte1
       // }
       };

uint8  bit_remapping_bimc2phy_CA[NUM_CH][NUM_CA_PHY_BIT]  = {
      //channel 0
      //0  1  2  3  4  5  6  7    8         9
      { 4, 7, 0, 2, 1, 3, CA_UNCONNECTED, CA_UNCONNECTED, CA_UNCONNECTED, CA_UNCONNECTED}, //ch0, bit

      //channel 1
      //0  1  2  3  4  5  6  7
      { 4, 7, 0, 2, 1, 3, CA_UNCONNECTED, CA_UNCONNECTED, CA_UNCONNECTED, CA_UNCONNECTED}, //ch0, bit
      //channel 2
      //0  1  2  3  4  5  6  7
      // { 4, 7, 0, 2, 1, 3, CA_UNCONNECTED, CA_UNCONNECTED, CA_UNCONNECTED, CA_UNCONNECTED}, //ch0, bit
      //channel 3
      //0  1  2  3  4  5  6  7
      // { 4, 7, 0, 2, 1, 3, CA_UNCONNECTED, CA_UNCONNECTED, CA_UNCONNECTED, CA_UNCONNECTED} //ch0, bit      
     };

uint8  bit_remapping_phy2bimc_CA[NUM_CH][PINS_PER_PHY] = {
      //channel 0
      //0  1  2  3  4  5  6  7    8         9
      { 2, 4, 3, 5, 0, CA_UNCONNECTED, CA_UNCONNECTED, 1, CA_UNCONNECTED, CA_UNCONNECTED}, //ch0, bit

      //channel 1
      //0  1  2  3  4  5  6  7
      { 2, 4, 3, 5, 0, CA_UNCONNECTED, CA_UNCONNECTED, 1, CA_UNCONNECTED, CA_UNCONNECTED}, //ch0, bit
      //channel 2
      //0  1  2  3  4  5  6  7
      // { 2, 4, 3, 5, 0, CA_UNCONNECTED, CA_UNCONNECTED, 1, CA_UNCONNECTED, CA_UNCONNECTED}, //ch0, bit
      //channel 3
      //0  1  2  3  4  5  6  7
      // { 2, 4, 3, 5, 0, CA_UNCONNECTED, CA_UNCONNECTED, 1, CA_UNCONNECTED, CA_UNCONNECTED} //ch0, bit      
     };    
     
       
//uint8 byte_remapping_table [NUM_CH][NUM_DQ_PCH] = {{0, 1}, {0, 1},{0, 1},{0, 1}};
uint8 byte_remapping_table [NUM_CH][NUM_DQ_PCH] = {{0, 1}, {0, 1}};

uint8 connected_bit_mapping_no_DBI_A [PINS_PER_PHY_CONNECTED_NO_DBI]     = {0, 1, 2, 3, 4, 5, 6, 8   }; // These PHY DQ pads are connected to BIMC DQ pads
uint8 connected_bit_mapping_with_DBI_A [PINS_PER_PHY_CONNECTED_WITH_DBI] = {0, 1, 2, 3, 4, 5, 6, 7, 8}; // PHY DQ[8] is DBI
uint8 connected_bit_mapping_CA [PINS_PER_PHY_CONNECTED_CA]               = {0, 1, 2, 3, 5, 8}; // These PHY CA pads are connected to BIMC CA pads

            // CA value sent back on DQ pads: CA: {0, 1, 2, 3, 4, 5} = DQ: {8, 9, 10, 11, 12, 13}
uint8 connected_bit_mapping_CA_PHY [PINS_PER_PHY_CONNECTED_CA]           = {0, 1, 2, 3, 4, 6}; //These are the odd DQ byte pads in PHY that contain data sent on the CA bus

uint8 dq_dbi_bit    = 8;
uint8 dq_spare_bit  = 9;

uint8 dll_analog_freq_range[]= {
   15 ,  /*  <300MHz  */
    7 ,  /*  <400MHz  */   
    3 ,  /*  <500MHz  */
    1 ,  /*  <700MHz  */
    0    /* < 1.89GHz  */
};

uint8 Triainig_pivot_plots_Switchboard[8][2]=
                                      {//_RD_P, _WR_P,
                                       {     0,    0}, // F_RANGE_0  200
                                       {     0,    0}, // F_RANGE_1  547
                                       {     0,    0}, // F_RANGE_2  681
                                       {     0,    0}, // F_RANGE_3  768
                                       {     0,    0}, // F_RANGE_4  1017
                                       {     0,    0}, // F_RANGE_5  1353
                                       {     0,    0}, // F_RANGE_6  1550
                                       {     0,    0}  // F_RANGE_7  1804
                                      };
//_DCC_Mon  : DCC Monitor
uint8 DCC_Training_Switchboard[8][1]=  
                                        {   //_DCC_Mon
                                           {     0 }, // F_RANGE_0  200
                                           {     1 }, // F_RANGE_1  547
                                           {     0 }, // F_RANGE_2  681
                                           {     0 }, // F_RANGE_3  768
                                           {     0 }, // F_RANGE_4  1017
                                           {     0 }, // F_RANGE_5  1353
                                           {     0 }, // F_RANGE_6  1550
                                           {     1 }  // F_RANGE_7  1804
                                        };

//_WRLVL_CF  : CDC sweep coorse and fine
uint8 WRLVL_Training_Switchboard[8][1]=  
                                        {   //_WRLVL_CF
                                           {     0 }, // F_RANGE_0  200
                                           {     0 }, // F_RANGE_1  547
                                           {     0 }, // F_RANGE_2  681
                                           {     0 }, // F_RANGE_3  768
                                           {     0 }, // F_RANGE_4  1017
                                           {     0 }, // F_RANGE_5  1353
                                           {     0 }, // F_RANGE_6  1550
                                           {     1 }  // F_RANGE_7  1804
                                        };


//_C_CF  : CDC sweep coorse and fine only center
//_C_PB  : CDC sweep coorse, fine and perbit center
//_DCC   : not used yet (reserved)
//_V_FSP1: Vref sweep FSP1  for terminated
//_V_FSB0: Vref sweep FSP0  for non-terminated

uint8 WR_Training_Switchboard[8][5]=  
                                      {//_C_CF,_C_PB,_DCC,_V_FSP1,_V_FSP0
                                       {     0,    0,   0,      0,     0 }, // F_RANGE_0  200
                                       {     0,    0,   0,      0,     0 }, // F_RANGE_1  547
                                       {     0,    0,   0,      0,     0 }, // F_RANGE_2  681
                                       {     1,    0,   0,      0,     1 }, // F_RANGE_3  768
                                       {     1,    0,   0,      0,     0 }, // F_RANGE_4  1017
                                       {     1,    0,   0,      0,     0 }, // F_RANGE_5  1353
                                       {     1,    0,   0,      0,     0 }, // F_RANGE_6  1550
                                       {     1,    1,   0,      1,     0 }  // F_RANGE_7  1804
                                      };
						 
// _C_CF   : CDC sweep coorse and fine only center
// _C_PB   : CDC sweep coorse, fine and perbit center
// _DCC    : not used yet (reserved)
// _V_FSP1 : Vref sweep HP VRef  for terminated
// _V_FSP0 : Vref sweep MP Vref  for non-terminated

uint8 RD_Training_Switchboard[8][5]=  
                                      {// _C_CF,_C_PB,_DCC, _V_HP , _V_MP
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_0   200
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_1   547
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_2   681
                                       {      1,    0,   0,      0,     1 }, // F_RANGE_3   768
                                       {      1,    0,   0,      0,     0 }, // F_RANGE_4   1017
                                       {      1,    0,   0,      0,     0 }, // F_RANGE_5   1353
                                       {      1,    0,   0,      0,     0 }, // F_RANGE_6   1550
                                       {      1,    1,   1,      1,     0 }  // F_RANGE_7   1804
                                      };
uint8  CA_in_PRFS[8][5]=  
                                      {//_C_CCF,_C_PB,_DCC,_V_FSP1,_V_FSB0
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_0    200
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_1    547
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_2    681
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_3    768
                                       {      0,    0,   0,      0,     1 }, // F_RANGE_4    1017
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_5    1353
                                       {      0,    0,   0,      0,     0 }, // F_RANGE_6    1550
                                       {      0,    1,   1,      1,     0 }  // F_RANGE_7    1804
                                      };

uint32 dll_analog_freq_range_table[]    = {300000, 400000, 500000, 700000, 1890000};
uint8  dll_analog_freq_range_table_size = sizeof(dll_analog_freq_range_table)/sizeof(uint8);
