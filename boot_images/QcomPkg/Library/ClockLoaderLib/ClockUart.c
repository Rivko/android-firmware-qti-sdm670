/*==================================================================================================

FILE: ClockUart.c

DESCRIPTION: UEFI driver for UARTDM/UARTBAM

        Copyright (c) 2017 Qualcomm Technologies Inc, All Rights Reserved
              Qualcomm Technologies Inc Proprietary and Confidential.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   configure_clocks

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <HALhwio.h>
#include "ClockUart.h"
/*==================================================================================================
                                     DEFINITIONS
==================================================================================================*/
// Clock bits masks
#define CLK_ENABLE_MASK                        0x00000001
#define CLK_OFF_MASK                           0x80000000
#define CLK_UPDATE_MASK                        0x00000001

// Clock bits locations
#define CLK_ENABLE_SHFT                        0
#define CLK_OFF_SHFT                           31
#define CLK_UPDATE_SHFT                        0

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void configure_clocks_hoya(Clock_qupv3_instance_type qupv3_wrapper, UINT32 nFrequency);


/*==================================================================================================

FUNCTION: configure_clocks

DESCRIPTION:
   This function performs all necessary clock configuration for UART enablement.
   Allow to set frequency for an instance.

PARAMETERS:
   qupv3_instance:  qupv3 wrap uart instance
   nFrequencyHz:    Frequency(Hz) will be set for uart instances.

RETURN:
   None

RESTRIC:
   This funtion should be called only one time to init uart Clocks

==================================================================================================*/
void configure_clocks(Clock_qupv3_instance_type qupv3_instance, UINT32 nFrequencyHz)
{
  configure_clocks_hoya(qupv3_instance, nFrequencyHz);
}

/*==================================================================================================

FUNCTION: configure_clocks_hoya
          This function enables the peripheral clocks for QUPV3 UART/TLMM at XBL loader.
          Support only two frequencies setting 7372800Hz, and 32000000Hz.
          A default 7372800Hz frequency is set for invalid frequency request.
          A default CLK_QUPV3_WRAP1_S7 instance is set for invalid instance request.

RESTRIC:  This funtion should be called only one time to init uart Clocks

==================================================================================================*/
static void configure_clocks_hoya(Clock_qupv3_instance_type qupv3_instance, UINT32 nFrequencyHz)
{
   UINT32 branch_enable;
   UINT32 mode, src_sel, src_div, root_config;
   UINT32 m, n, m_reg, n_reg, d_reg;
   UINT32 reg_addr;

   // Clock Frequencies
   #define CXO_FREQUENCYHZ                        19200000  // 19.2 MHz
   #define DEFAULT1_FREQUENCYHZ                    7372800  // 7.3728 MHz
   #define DEFAULT2_FREQUENCYHZ                   32000000  // 32 MHz

   // Qupve Wrap0 bits location
   #define GCC_QUPV3_WRAP0_CFG_RCGR_HW_CLK_CTRL   20
   #define GCC_QUPV3_WRAP0_CFG_RCGR_MODE_SHFT     12
   #define GCC_QUPV3_WRAP0_CFG_RCGR_SRC_SEL_SHFT   8
   #define GCC_QUPV3_WRAP0_CFG_RCGR_SRC_DIV_SHFT   0

   #define GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR    0x00152004
   #define TLMM_EAST_AHB_CLK_ENA_MASK             0x100  // bit 8
   #define TLMM_SOUTH_AHB_CLK_ENA_MASK            0x800  // bit 11
   #define TLMM_NORTH_AHB_CLK_ENA_MASK            0x1000 // bit 12
   #define TLMM_CLK_ENA_MASK                      0x40   // bit 6

   #define GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1_ADDR  0x0015200C
   // QUPV3 WRAP1 vote masks -bit 22-29 QUPV3_WRAP1_S0_CLK_ENA - QUPV3_WRAP1_S7_CLK_ENA
   #define QUPV3_WRAP1_SN_CLK_ENA_MASK            0x3FC00000
   // QUPV3 WRAP0 vote masks bit 10-17 QUPV3_WRAP0_S0_CLK_ENA - QUPV3_WRAP0_S7_CLK_ENA
   #define QUPV3_WRAP0_SN_CLK_ENA_MASK            0x0003FC00

   // Core Clocks vote mask
   #define QUPV3_WRAP_1_S_AHB_CLK_ENA_MASK        0x00200000 // bit 21
   #define QUPV3_WRAP_1_M_AHB_CLK_ENA_MASK        0x00100000 // bit 20
   #define QUPV3_WRAP1_CORE_CLK_ENA_MASK          0x00080000 // bit 19
   #define QUPV3_WRAP1_CORE_2X_CLK_ENA_MASK       0x00040000 // bit 18

   #define QUPV3_WRAP0_CORE_2X_CLK_ENA_MASK       0x00000200 // bit 9
   #define QUPV3_WRAP0_CORE_CLK_ENA_MASK          0x00000100 // bit 8
   #define QUPV3_WRAP_0_S_AHB_CLK_ENA_MASK        0x00000080 // bit 7
   #define QUPV3_WRAP_0_M_AHB_CLK_ENA_MASK        0x00000040 // bit 6

   #define GCC_QUPV3_WRAP0_S0_CBCR_ADDR           0x00117030
   //#define GCC_QUPV3_WRAP0_S0_CMD_RCGR_ADDR     0x00117034
   //#define GCC_QUPV3_WRAP0_S0_CFG_RCGR_ADDR     0x00117038
   //#define GCC_QUPV3_WRAP0_S0_M_ADDR            0x0011703C
   //#define GCC_QUPV3_WRAP0_S0_N_ADDR            0x00117040
   //#define GCC_QUPV3_WRAP0_S0_D_ADDR            0x00117044

   #define GCC_QUPV3_WRAP1_S0_CBCR_ADDR           0x00118014
   //#define GCC_QUPV3_WRAP1_S0_CMD_RCGR_ADDR     0x00118018
   //#define GCC_QUPV3_WRAP1_S0_CFG_RCGR_ADDR     0x0011801C
   //#define GCC_QUPV3_WRAP1_S0_M_ADDR            0x00118020
   //#define GCC_QUPV3_WRAP1_S0_N_ADDR            0x00118024
   //#define GCC_QUPV3_WRAP1_S0_D_ADDR            0x00118028

   #define GCC_QUPV3_WRAP_SN_OFFSET               0x130
   #define GCC_QUPV3_WRAP0_SE0_CMD_DFSR           0x00117048  // reset value 0x00008020, DFS_EN=0

   // Qupv3 wrap0 Core Clocks registers
   #define GCC_QUPV3_WRAP0_CORE_2X_CBCR           0x00117014
   #define GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR       0x00117018
   #define GCC_QUPV3_WRAP0_CORE_2X_CFG_RCGR       0x0011701C
   #define GCC_QUPV3_WRAP0_CORE_CBCR              0x11700C
   #define GCC_QUPV3_WRAP_0_M_AHB_CBCR            0x117004
   #define GCC_QUPV3_WRAP_0_S_AHB_CBCR            0x117008

   // Qupv3 wrap1 Core Clocks registers
   #define GCC_QUPV3_WRAP1_CORE_2X_CBCR           0x118004
   #define GCC_QUPV3_WRAP1_CORE_CBCR              0x118008
   #define GCC_QUPV3_WRAP_1_M_AHB_CBCR            0x11800C
   #define GCC_QUPV3_WRAP_1_S_AHB_CBCR            0x118010

   // Validate the input parameters
   // if invalid frequency request, set it to default1 frequency
   // if invalid instance request, set it to CLK_QUPV3_WRAP1_S7

   if ((nFrequencyHz != DEFAULT1_FREQUENCYHZ)||
       (nFrequencyHz != DEFAULT2_FREQUENCYHZ))
     nFrequencyHz = DEFAULT1_FREQUENCYHZ;

   if (qupv3_instance > CLK_QUPV3_WRAP1_S7)
     qupv3_instance =  CLK_QUPV3_WRAP1_S7;

   // Enable vote
   branch_enable = in_dword(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1_ADDR);

   // Enable QUPV3 WRAP0 uart
   branch_enable |= QUPV3_WRAP0_SN_CLK_ENA_MASK;

   // Enable QUPV3 WRAP1 uart
   branch_enable |= QUPV3_WRAP1_SN_CLK_ENA_MASK;

   // Enable QUP core clocks vote
   if (qupv3_instance <= CLK_QUPV3_WRAP0_S7)
   {

     branch_enable |= (QUPV3_WRAP_0_S_AHB_CLK_ENA_MASK |
                       QUPV3_WRAP_0_M_AHB_CLK_ENA_MASK |
                       QUPV3_WRAP0_CORE_CLK_ENA_MASK   |
                       QUPV3_WRAP0_CORE_2X_CLK_ENA_MASK);
   }
   else
   {
     branch_enable |= (QUPV3_WRAP_1_S_AHB_CLK_ENA_MASK |
                       QUPV3_WRAP_1_M_AHB_CLK_ENA_MASK |
                       QUPV3_WRAP1_CORE_CLK_ENA_MASK   |
                       QUPV3_WRAP1_CORE_2X_CLK_ENA_MASK);
   }
   out_dword(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_1_ADDR, branch_enable);

   // Enable vote
   branch_enable = in_dword(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR);

   // Enable tlmn ahb vote
   branch_enable |= (TLMM_EAST_AHB_CLK_ENA_MASK | TLMM_SOUTH_AHB_CLK_ENA_MASK | TLMM_NORTH_AHB_CLK_ENA_MASK);

   // Enable tlmn vote
   branch_enable |= TLMM_CLK_ENA_MASK;
   out_dword(GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR, branch_enable);

   // Config QUPV3 Uart
   mode    = 2;  // dual edge
   src_sel = 0;  // CXO (19,200,000)
   src_div = 0;  // bypass predivider
   root_config = (mode    << GCC_QUPV3_WRAP0_CFG_RCGR_MODE_SHFT)    |
                 (src_sel << GCC_QUPV3_WRAP0_CFG_RCGR_SRC_SEL_SHFT) |
                 (src_div << GCC_QUPV3_WRAP0_CFG_RCGR_SRC_DIV_SHFT);

   // select M/N:D values for BLSP_UART_APPS clock:
   //    input_clock * M / N   = output_clock
   //    19,200,000 * 12 / 125 = 1,843,200
   //    M = (output_clock * N)/input_clock

   n = (nFrequencyHz == DEFAULT2_FREQUENCYHZ)? 75: 125;
   m = (UINT64)(nFrequencyHz *n) / CXO_FREQUENCYHZ;

   m_reg = m & 0xFFFF;
   n_reg = ~(n - m) & 0xFFFF;
   d_reg = ~n & 0xFFFF;  // set duty cycle to 0.5 (duty cycle = d / n; d_val = ~(2 * d))

   if (qupv3_instance <= CLK_QUPV3_WRAP0_S7)
   {
     reg_addr = GCC_QUPV3_WRAP0_S0_CBCR_ADDR;
     reg_addr += (GCC_QUPV3_WRAP_SN_OFFSET * qupv3_instance);
   }
   else
   {
     reg_addr = GCC_QUPV3_WRAP1_S0_CBCR_ADDR;
     reg_addr += (GCC_QUPV3_WRAP_SN_OFFSET * (qupv3_instance - CLK_QUPV3_WRAP1_S0));
   }

   out_dword(reg_addr,      0);
   out_dword(reg_addr+0x04, 0);
   out_dword(reg_addr+0x08, root_config);
   out_dword(reg_addr+0x0c, m_reg);
   out_dword(reg_addr+0x10, n_reg);
   out_dword(reg_addr+0x14, d_reg);
   out_dword(reg_addr,      1);  // CLK_ENABLE
   out_dword(reg_addr+0x04, 3);  // ROOT_EN and UPDATE

   if (qupv3_instance <= CLK_QUPV3_WRAP0_S7)
   {
     /*
       gcc_qupv3_wrap0_core_2x_clk
       gcc_qupv3_wrap0_core_clk
       gcc_qupv3_wrap_0_m_ahb_clk
       gcc_qupv3_wrap_0_s_ahb_clk
     */
     out_dword(GCC_QUPV3_WRAP0_CORE_2X_CBCR,        1);  // CLK_ENABLE
     out_dword(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR,    3);  // ROOT_EN and UPDATE
     out_dword(GCC_QUPV3_WRAP0_CORE_CBCR,           1);  // CLK_ENABLE
     out_dword(GCC_QUPV3_WRAP_0_M_AHB_CBCR,         1);  // CLK_ENABLE
     out_dword(GCC_QUPV3_WRAP_0_S_AHB_CBCR,         1);  // CLK_ENABLE
   }
   else
   {
     /*
       gcc_qupv3_wrap1_core_2x_clk
       gcc_qupv3_wrap1_core_clk
       gcc_qupv3_wrap_1_m_ahb_clk
       gcc_qupv3_wrap_1_s_ahb_clk
     */
     out_dword(GCC_QUPV3_WRAP1_CORE_2X_CBCR,        1);  // CLK_ENABLE
     out_dword(GCC_QUPV3_WRAP1_CORE_CBCR,           1);  // CLK_ENABLE
     out_dword(GCC_QUPV3_WRAP_1_M_AHB_CBCR,         1);  // CLK_ENABLE
     out_dword(GCC_QUPV3_WRAP_1_S_AHB_CBCR,         1);  // CLK_ENABLE
   }
}

