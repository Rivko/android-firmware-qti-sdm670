/*
 * HALclkInitASM.S
 * This file contains the implementation for setting up PLL, Q6 clock, and
 * bus of SSC
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 *       All Rights Reserved.
 *  Qualcomm Technologies, Inc. Confidential and Proprietary
 *
 *                   Edit History

   $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/slpi/hw/sdm845/HALclkInitASM.S#1 $

     #when       who                  what, where, why
    --------   -------------       ----------------------------------------------
    01/07/16   dcf                 Created file.
 *
 */

#include "HALclkHWIOASM.h"

//******************************************************************************
//
//                            DATA DEFINITIONS
//
//******************************************************************************

//*******************************************************************
// SSC Subsystem PLL Data definitions.
//*******************************************************************

#define SSC_PLL_L_VAL                                          0xF
#define SSC_PLL_FRAC_VAL                                       0xA000
#define SSC_PLL_OPMODE_STANDBY                                 0x0
#define SSC_PLL_OPMODE_RUN                                     0x1
#define SSC_PLL_LOCK_DELAY_TIMEOUT_VAL                         0x7F
#define SSC_PLL_USER_CTL_VAL                                   0x00000007
#define SSC_PLL_USER_CTL_U_VAL                                 0x00004804
#define SSC_PLL_CONFIG_CTL_VAL                                 0x20485699
#define SSC_PLL_CONFIG_CTL_U_VAL                               0x00002067
#define SSC_PLL_TEST_CTL_VAL                                   0x40000000
#define SSC_PLL_TEST_CTL_U_VAL                                 0x00000000

//*******************************************************************
// QDSP6 SS PLL Data definitions.
//*******************************************************************

#define SSC_QDSP6SS_PLL_L_VAL                                  0x1F
#define SSC_QDSP6SS_PLL_CAL_L_VAL                              0x1C
#define SSC_QDSP6SS_PLL_FRAC_VAL                               0x4000
#define SSC_QDSP6SS_PLL_USER_CTL_VAL                           0x00000007
#define SSC_QDSP6SS_PLL_USER_CTL_U_VAL                         0x00004804
#define SSC_QDSP6SS_PLL_CONFIG_CTL_VAL                         0x20485699
#define SSC_QDSP6SS_PLL_CONFIG_CTL_U_VAL                       0x00002067
#define SSC_QDSP6SS_PLL_TEST_CTL_VAL                           0x40000000
#define SSC_QDSP6SS_PLL_TEST_CTL_U_VAL                         0x00000000

//*******************************************************************
// QDSP6 SS Core Clock Data definitions.
//*******************************************************************

#define SSC_QDSP6SS_STRAP_ACC_VAL                              0x0
#define SSC_QDSP6SS_RCG_DIV_VAL                                0x1
#define SSC_QDSP6SS_RCG_SRC_SEL_VAL                            0x2
#define SSC_QDSP6SS_RCG_SRC_SEL_SSC_PLL                        0x3

//*******************************************************************
// SSC Main Bus RCG config.
//*******************************************************************

#define SSC_MAIN_BUS_CFG_RCG_DIV_VAL                           0x5
#define SSC_MAIN_BUS_CFG_RCG_SRC_SEL_VAL                       0x2


//*******************************************************************
// SSC Dividers based on Main Bus RCG.
//*******************************************************************

#define SSC_QTMR_DIV_CLK_CDIV                                  0x4
#define SSC_QUPV3_2XCORE_DIV_CLK_CDIV                          0x0
#define SSC_QUPV3_CORE_DIV_CLK_CDIV                            0x1

//*******************************************************************
// Busywait settings.
//*******************************************************************

#define BUSYWAIT_ACR_0_ADDR  0x058A0040
#define BUSYWAIT_ACR_1_ADDR  0x058A0044
#define BUSYWAIT_ACR_2_ADDR  0x058A0048

//******************************************************************************
//
//                            MACRO DEFINITIONS
//
//******************************************************************************


    //**************************************************************************
    // MACRO:  CLEAR_BITS
    //
    // DESCRIPTION:
    //   This macro clears one ore more bits in a register.  r4, r5, r6, and r7
    //   may be clobbered when running this macro.
    //**************************************************************************

    .macro clear_bits val_mask
        {
            r7 = r0
            r5.h = #hi(\val_mask)
        }
        {
            r5.l = #lo(\val_mask)
        }
        {
            r4 = memw(r0)
            r5 = not(r5)
        }
        r4 = and(r4, r5)    // clear the mask bits
        {
            memw(r0) = r4  //write the reg value
        }
        barrier
    .endm


    //**************************************************************************
    // MACRO:  SET_BITS
    //
    // DESCRIPTION:
    //   This macro sets one or more bits in a register.  r4, r5 and r7 will be 
    //   clobbered when using this macro.
    //**************************************************************************

    .macro set_bits val_mask
        {
            r7 = r0
            r5.h = #hi(\val_mask)
        }
        {
            r5.l = #lo(\val_mask)
            dcinva(r7)
        }
        {
            r4 = memw(r0)
            r6 = not(r5)
        }
        r4 = and(r4, r6)    // clear the fields
        r4 = or(r4,r5)      // set the value

        {
            memw(r0) = r4  //write the reg value
        }
        barrier
    .endm


    //**************************************************************************
    // MACRO:  BUSYWAIT_INIT
    //
    // DESCRIPTION:
    //   This macro waits a specific number of microseconds.
    //**************************************************************************

    .macro __busywait_init
        r0 = ##BUSYWAIT_ACR_0_ADDR
        memw(r0) = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_RMSK
        r0 = ##BUSYWAIT_ACR_1_ADDR
        memw(r0) = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_RMSK
        r0 = ##BUSYWAIT_ACR_2_ADDR
        memw(r0) = ##HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_RMSK
        //r0 = ##HWIO_SSC_SSCAON_COUNTER_CFG_ADDR
        //memw(r0) = ##HWIO_SSC_SSCAON_COUNTER_CFG_SSC_SSCAON_QCNTR_EN_BMSK
        barrier
    .endm


//******************************************************************************
//
//                     APPLICATION   PROGRAMMING   INTERFACE
//
//******************************************************************************



//******************************************************************************
// FUNCTION: __HEXAGON_BSP_INIT
//
// DESCRIPTION:
//   This is the main API for this file.  This function will check the status of the
//   main LPASS PLL and configure it if it is not running.  Otherwise, it will just
//   add a HW vote on behalf of the LPASS processor.
//
//   The QDSP6 and AHB clocks will then be initialized if not already set up.  The 
//   purpose of this is to ensure the QDSP6 clock and AHB are running at maximum
//   frequency under SVS voltage.
//
// ASSUMPTIONS:
//   There is no stack available when this is called, so only device registers can be
//   accessed.
//******************************************************************************

   .section .start,"ax",@progbits    
   .falign
   .global __hexagon_bsp_init
   .type  __hexagon_bsp_init, @function

__hexagon_bsp_init:

  //*******************************************************************
  // Make sure the QDSP6SS Sleep clock is enabled.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR
  r1 = ##HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKEN_BMSK
  memw(r0) = r1
  barrier

  __busywait_init

  jump start_prog

__busywait:
  {
    r8 = r1
    r9 = r2
    r10 = r3
  }
  r2 = r0             // delay_count
  r0 = ##HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR
  r1 = memw(r0)       // start count
  
__buswait_loop:
  r3 = memw(r0)
  r3 = sub(r3, r1)
  {
    p0 = cmp.gtu(r3,r2)
    if (!p0.new) jump:t __buswait_loop              // If the PLL is already set up go to PLL voting.
  }
  {
    r1 = r8
    r2 = r9
    r3 = r10
  }
  jumpr r13

start_prog:

  //****************************************************************************
  //
  //                    SSC Subsystem PLL INITIAL SETUP
  //
  //****************************************************************************

  //*******************************************************************
  // Check the HW version for the PLL_CONFIG_CTL programming.
  //*******************************************************************

v1_pll_configs:
   r14 = ##SSC_QDSP6SS_PLL_L_VAL
   r17 = ##SSC_QDSP6SS_RCG_DIV_VAL
   r18 = ##SSC_MAIN_BUS_CFG_RCG_DIV_VAL
   r19 = ##SSC_PLL_L_VAL
   r20 = ##SSC_PLL_FRAC_VAL
   r21 = ##SSC_QDSP6SS_PLL_FRAC_VAL
   r23 = ##SSC_QDSP6SS_PLL_CAL_L_VAL
   jump program_pll

program_pll:

  //*******************************************************************
  // Set up the PLL CONFIG/USER/TEST control defaults
  //*******************************************************************

  /////////////////
  //// SSCPLL0 ////
  /////////////////

  r0 = ##HWIO_SSC_SCC_PLL_CONFIG_CTL_ADDR
  memw(r0) = ##SSC_PLL_CONFIG_CTL_VAL

  r0 = ##HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ADDR
  memw(r0) = #SSC_PLL_CONFIG_CTL_U_VAL

  r0 = ##HWIO_SSC_SCC_PLL_USER_CTL_ADDR
  memw(r0) = ##SSC_PLL_USER_CTL_VAL

  r0 = ##HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR
  memw(r0) = ##SSC_PLL_USER_CTL_U_VAL

  r0 = ##HWIO_SSC_SCC_PLL_TEST_CTL_ADDR
  memw(r0) = ##SSC_PLL_TEST_CTL_VAL

  r0 = ##HWIO_SSC_SCC_PLL_TEST_CTL_U_ADDR
  memw(r0) = ##SSC_PLL_TEST_CTL_U_VAL

  /////////////////
  //// SSCPLL1 ////
  /////////////////

  r0 = ##HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR
  memw(r0) = ##SSC_QDSP6SS_PLL_CONFIG_CTL_VAL

  r0 = ##HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR
  memw(r0) = ##SSC_QDSP6SS_PLL_CONFIG_CTL_U_VAL

  r0 = ##HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR
  memw(r0) = ##SSC_QDSP6SS_PLL_USER_CTL_VAL

  r0 = ##HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR
  memw(r0) = ##SSC_QDSP6SS_PLL_USER_CTL_U_VAL

  r0 = ##HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR
  memw(r0) = ##SSC_QDSP6SS_PLL_TEST_CTL_VAL

  r0 = ##HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR
  memw(r0) = ##SSC_QDSP6SS_PLL_TEST_CTL_U_VAL

  barrier
#if 0
  //*******************************************************************
  // Set up the SSC lock detect delay timeout timer.
  //*******************************************************************

  r0 = ##HWIO_SSC_SSCAON_SM_CSR1_ADDR
  r1 = memw(r0)
  set_bits HWIO_SSC_SSCAON_SM_CSR1_PLL_TMR_EN_BMSK
  {
    r2 = ##(SSC_PLL_LOCK_DELAY_TIMEOUT_VAL << HWIO_SSC_SSCAON_SM_CSR1_PLL_LOCKDLY_SHFT)
    r3 = ##(0x1 << HWIO_SSC_SSCAON_SM_CSR1_PLL_LOCKDLY_WR_SHFT)
  }
  r2 = or(r2, r3)
  r1 = or(r1, r2)
  memw(r0) = r1
#endif
  //*******************************************************************
  // Set the desired L-value.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_L_VAL_ADDR
  r1 = r19
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Duplicate the L-value to the CALIB_L register.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_CAL_L_VAL_ADDR
  r1 = r19
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Program the fractional divider value.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_FRAC_VAL_ADDR
  r1 = r20
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Turn off the PLL outputs.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_MODE_ADDR
  r1 = memw(r0)
  clear_bits HWIO_SSC_SCC_PLL_MODE_PLL_OUTCTRL_BMSK

  //*******************************************************************
  // Put the PLL into Standby mode.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_OPMODE_ADDR
  clear_bits HWIO_SSC_SCC_PLL_OPMODE_PLL_OPMODE_BMSK

  //*******************************************************************
  // Bring PLL out of reset.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_MODE_ADDR
  set_bits HWIO_SSC_SCC_PLL_MODE_PLL_RESET_N_BMSK

  //*******************************************************************
  // Put the PLL into run mode.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_OPMODE_ADDR
  r1 = memw(r0)
  r2 = ##(SSC_PLL_OPMODE_RUN << HWIO_SSC_SCC_PLL_OPMODE_PLL_OPMODE_SHFT)
  r1 = or(r1,r2)
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Poll on the lock detect value until it transitions to '1'
  //*******************************************************************

  r4 = ##HWIO_SSC_SCC_PLL_MODE_ADDR
  //r2 = ##HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_DET_FINE_BMSK
  r2 = ##HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_DET_BMSK

ssc_lock_det_loop:
  r0 = ##0x1D4C0
  r13 = add(PC, #0x8)
  jump __busywait

  r1 = memw(r4)
  r3 = and(r1, r2)
  {
    p0 = cmp.eq(r2,r3)
    if (!p0.new) jump:t ssc_lock_det_loop
  }

  //*******************************************************************
  // Enable PLL Outputs
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_PLL_USER_CTL_ADDR
  set_bits HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_MAIN_BMSK

  r0 = ##HWIO_SSC_SCC_PLL_MODE_ADDR
  set_bits HWIO_SSC_SCC_PLL_MODE_PLL_OUTCTRL_BMSK

  // ...End Peel PLL Setup...

  //****************************************************************************
  //
  //      TEMPORARILY SET THE Q6 TO RUN ON SSC WHILE SETTING UP THE Q6 PLL
  //
  //****************************************************************************
#if 0
  r0 = ##HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR
  r1 = memw(r0)
  {
    r2 = asl(r17, #HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT)
    r3 = ##(SSC_QDSP6SS_RCG_SRC_SEL_SSC_PLL << HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT)
  }
  r2 = or(r2, r3)
  r1 = or(r1, r2)
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Trigger a state update and wait for the HW to take affect
  //*******************************************************************  

  r0 = ##HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR
  set_bits HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK

  r2 = ##HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK

qdsp6_rcgr1_update:
   r1 = memw(r0)
   r3 = and(r1, r2)
   {
     p0 = cmp.eq(r3, r2)
     if (p0.new) jump:t qdsp6_rcgr1_update
   }   
#endif
  
  //*******************************************************************
  // Program the STRAP ACC register value
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_STRAP_ACC_ADDR
  r1 = ##SSC_QDSP6SS_STRAP_ACC_VAL
  memw(r0) = r1
  barrier

  //****************************************************************************
  //
  //            SET UP THE BUS DOMAIN TO RUN ON THE SSC PLL
  //
  //****************************************************************************

  //*******************************************************************
  // Program the divider and source selection to the SSC PLL.
  //*******************************************************************
  r0 = ##HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_ADDR
  r1 = memw(r0)

  {
    r2 = r18
    r3 = ##(SSC_MAIN_BUS_CFG_RCG_SRC_SEL_VAL << HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_SEL_SHFT)
  }

  r2 = or(r2, r3)
  r1 = or(r1, r2)
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Initialize the CDIV registers so that downstream clocks are not
  // overclocked.
  //******************************************************************* 
 
  r0 = ##HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR
  memw(r0) = ##SSC_QTMR_DIV_CLK_CDIV

  r0 = ##HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_ADDR
  memw(r0) = ##SSC_QUPV3_2XCORE_DIV_CLK_CDIV
  
  r0 = ##HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_ADDR
  memw(r0) = ##SSC_QUPV3_CORE_DIV_CLK_CDIV

 //*******************************************************************
  // Trigger the state update and poll on completion.
  //*******************************************************************

  r0 = ##HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ADDR
  set_bits HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_UPDATE_BMSK

  r1 = ##HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_UPDATE_BMSK

bus_rcgr_update:
   r1 = memw(r0)
   r3 = and(r1, r2)
   {
     p0 = cmp.eq(r3, r2)
     if (p0.new) jump:t bus_rcgr_update
   }   

  // ...End Main Bus clock config setup... 

  //*******************************************************************
  // Enable QTimer Clocks
  //*******************************************************************
  r0 = ##HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR
  r1 = ##HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ENABLE_BMSK
  memw(r0) = r1

  r0 = ##HWIO_SSC_SCC_QTIMER_AHB_CBCR_ADDR
  r1 = ##HWIO_SSC_SCC_QTIMER_AHB_CBCR_CLK_ENABLE_BMSK
  memw(r0) = r1

  //*******************************************************************
  // Enable SMEM Clock
  //*******************************************************************
  r0 = ##HWIO_SSC_SCC_SMEM_CBCR_ADDR
  r1 = ##HWIO_SSC_SCC_SMEM_CBCR_CLK_ENABLE_BMSK
  r2 = memw(r0)
  r1 = or(r1,r2)

  memw(r0) = r1

  //****************************************************************************
  //
  //                    QDSP6SS PLL INITIAL BRING-UP
  //
  //****************************************************************************

  //*******************************************************************
  // Set up the PLL L-value, CAL_L value, and fractional divider.
  //*******************************************************************

  {
    r0 = ##HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR
    r1 = r14
  }
  memw(r0) = r1
  barrier

  r0 = ##HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR
  r1 = r23
  memw(r0) = r1
  barrier

  r0 = ##HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_ADDR
  r1 = r21
  memw(r0) = r1
  barrier


  //*******************************************************************
  // Turn off the PLL outputs.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_PLL_MODE_ADDR
  r1 = memw(r0)
  clear_bits HWIO_SSC_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK


  //*******************************************************************
  // Put the PLL into Standby mode.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR
  clear_bits HWIO_SSC_QDSP6SS_PLL_OPMODE_PLL_OPMODE_BMSK

  //*******************************************************************
  // Bring PLL out of reset.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_PLL_MODE_ADDR
  set_bits HWIO_SSC_QDSP6SS_PLL_MODE_PLL_RESET_N_BMSK

  //*******************************************************************
  // Put the PLL into run mode.
  //*******************************************************************

  r0 = ##HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR
  r1 = memw(r0)
  r2 = ##(SSC_PLL_OPMODE_RUN << HWIO_SSC_QDSP6SS_PLL_OPMODE_PLL_OPMODE_SHFT)
  r1 = or(r1,r2)
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Poll on the lock detect value until it transitions to '1'
  //*******************************************************************

  r4 = ##HWIO_SSC_QDSP6SS_PLL_MODE_ADDR
  //r2 = ##HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_FINE_BMSK
  r2 = ##HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_DET_BMSK

q6_lock_det_loop:
  r0 = ##0x1D4C0
  r13 = add(PC, #0x8)
  jump __busywait

  r1 = memw(r4)
  r3 = and(r1, r2)
  {
    p0 = cmp.eq(r2,r3)
    if (!p0.new) jump:t q6_lock_det_loop
  }

  //*******************************************************************
  // Enable PLL Outputs
  //*******************************************************************
 
  r0 = ##HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR
  set_bits HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_MAIN_BMSK

  r0 = ##HWIO_SSC_QDSP6SS_PLL_MODE_ADDR
  set_bits HWIO_SSC_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK

  // ...End QDSP6SS PLL Setup...

  //****************************************************************************
  //
  //                 QDSP6 SS CORE CLOCK CONFIG SET TO QDSP6SS PLL
  //
  //****************************************************************************
  
  //*******************************************************************
  // Set the QDSP6 SS core clock divider and source select.
  //*******************************************************************  

  r0 = ##HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR
  r1 = memw(r0)
  {
    r2 = asl(r17, #HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT)
    r3 = ##(SSC_QDSP6SS_RCG_SRC_SEL_VAL << HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT)
  }
  r2 = or(r2, r3)
  r1 = or(r1, r2)
  memw(r0) = r1
  barrier

  //*******************************************************************
  // Trigger a state update and wait for the HW to take affect
  //*******************************************************************  

  r0 = ##HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR
  set_bits HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK

  r2 = ##HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK

qdsp6_rcgr2_update:
   r1 = memw(r0)
   r3 = and(r1, r2)
   {
     p0 = cmp.eq(r3, r2)
     if (p0.new) jump:t qdsp6_rcgr2_update
   }   

end_init:
   Jumpr r31

.size __hexagon_bsp_init, .-__hexagon_bsp_init

